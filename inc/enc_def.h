/* ====================================================================================================================

  The copyright in this software is being made available under the License included below.
  This software may be subject to other third party and contributor rights, including patent rights, and no such
  rights are granted under this license.

  Copyright (c) 2018, HUAWEI TECHNOLOGIES CO., LTD. All rights reserved.
  Copyright (c) 2018, SAMSUNG ELECTRONICS CO., LTD. All rights reserved.
  Copyright (c) 2018, PEKING UNIVERSITY SHENZHEN GRADUATE SCHOOL. All rights reserved.
  Copyright (c) 2018, PENGCHENG LABORATORY. All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted only for
  the purpose of developing standards within Audio and Video Coding Standard Workgroup of China (AVS) and for testing and
  promoting such standards. The following conditions are required to be met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and
      the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
      the following disclaimer in the documentation and/or other materials provided with the distribution.
    * The name of HUAWEI TECHNOLOGIES CO., LTD. or SAMSUNG ELECTRONICS CO., LTD. may not be used to endorse or promote products derived from
      this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

* ====================================================================================================================
*/

#ifndef _ENC_DEF_H_
#define _ENC_DEF_H_

#include<string>
#include<iostream>
//using namespace std;
#include "com_def.h"
#include "enc_bsw.h"
//#include "enc_sad.h"

//#define PRINT_SPLIT_MODE_IPM		1

#define RATECONTROL                         0  // M3580 M3627 M3689
#define rc_algorithm_0                      0  // = 1, use adaptive bit allocation; = 0, fixed bit allocation
#define rc_algorithm_1                      0  // = 1, use blur_complex
#define rc_algorithm_2                      1  // = 1, alpha and beta update
#define rc_algorithm_3                      0  // x264
#define rc_algorithm_4                      0  // two-pass, encode N frames find best alpha and beta
#define rc_algorithm_5                      1  // no use GOPbitsLeft
#define rc_algorithm_6                      1  // scc rate control
#define rc_algorithm_7                      0  // ??2????????|¨¬?¡§¡ã??¡è¡§¡ä¡§a?
#define rc_algorithm_8                      0  // 2?¡§a?¡§¡è¡§¡èD¡§a????lamadao¡§aqp|¨¬???¡§o?¡§???t
#define rc_algorithm_9                      1  // turn off intra satd
#define rc_I_slice_cost                     0  //??¡§¡§?1??¨¤??¨º?AI????¡§o?¨¤2??¡§|?a??
#define LCU_RATECONTROL						0  //CTU-level rate allocation for All-Intra cfg
#define rc_algorithm_10						0  // = 1 pre-analysis (SATD calculation) for I frame
#define HEVC_RC                             1  //open with RATECONTROL
#if  HEVC_RC
#define U0132_TARGET_BITS_SATURATION        0 ///< Rate control with target bits saturation method
#if  U0132_TARGET_BITS_SATURATION
#define V0078_ADAPTIVE_LOWER_BOUND          1 ///< Target bits saturation with adaptive lower bound
#endif
#endif
extern int  op_qp;
extern char op_fname_inp[256];
extern char op_sc_tv_path[512];
extern char op_sc_vcd[256];
extern std::string g_tv_path;
#include "ratectrl.h"
#include "rate_control_picture.h"
#define COM_IPRED_CHK_CONV(mode)\
	((mode) == IPD_VER || (mode) == IPD_HOR || (mode) == IPD_DC || (mode) == IPD_BI)

#define COM_IPRED_CONV_L2C_CHK(mode, chk) \
if (COM_IPRED_CHK_CONV(mode)) \
	{\
	(mode) = ((mode) == IPD_VER) ? IPD_VER_C : ((mode) == IPD_HOR ? IPD_HOR_C : \
	((mode) == IPD_DC ? IPD_DC_C : IPD_BI_C)); \
	(chk) = 1; \
	}\
	else \
	(chk) = 0;

/* support RDOQ */
#define SCALE_BITS               15    /* Inherited from TMuC, pressumably for fractional bit estimates in RDOQ */
#if USE_RDOQ
#define ERR_SCALE_PRECISION_BITS 20
#endif
/* encoder magic code */
#define ENC_MAGIC_CODE         0x45565945 /* EVYE */

#define GOP_P                    4

enum PIC_BUFS
{
    PIC_IDX_ORG = 0,
    PIC_IDX_REC = 1,
    PIC_BUF_NUM = 2,
};

/* motion vector accuracy level for inter-mode decision */
#define ME_LEV_IPEL              1
#define ME_LEV_HPEL              2
#define ME_LEV_QPEL              3

/* maximum inbuf count */
#define ENC_MAX_INBUF_CNT      32

/* maximum cost value */
#define MAX_COST                (1.7e+308)


/* virtual frame depth B picture */
#define FRM_DEPTH_0                   0
#define FRM_DEPTH_1                   1
#define FRM_DEPTH_2                   2
#define FRM_DEPTH_3                   3
#define FRM_DEPTH_4                   4
#define FRM_DEPTH_5                   5
#define FRM_DEPTH_MAX                 6
/* I-slice, P-slice, B-slice + depth + 1 (avs3_max for GOP 8 size)*/
#define LIST_NUM                      1

/* instance identifier for encoder */
typedef void  * ENC;

/*****************************************************************************
 * original picture buffer structure
 *****************************************************************************/
typedef struct _ENC_PICO
{
    /* original picture store */
    COM_PIC                pic;
    /* input picture count */
    u32                     pic_icnt;
    /* be used for encoding input */
    u8                      is_used;

    /* address of sub-picture */
    COM_PIC              * spic;
} ENC_PICO;


/*****************************************************************************
 * 


 prediction structure
 *****************************************************************************/
typedef struct _ENC_PINTRA
{
    pel               * addr_org[N_C];
    int                 stride_org[N_C];
    pel               * addr_rec_pic[N_C];
    int                 stride_rec[N_C];
    int                 slice_type;
} ENC_PINTRA;


/*****************************************************************************
 * code structure
 *****************************************************************************/
#if ENABLE_BFRAME
#define LDB                            0
#define RA                             1
#define LDP                            LDB
#endif

/*****************************************************************************
 * inter prediction structure
 *****************************************************************************/

#define MV_RANGE_MIN           0
#define MV_RANGE_MAX           1
#define MV_RANGE_DIM           2

typedef struct _ENC_PINTER ENC_PINTER;
struct _ENC_PINTER
{
    COM_REFP 		refp[MAX_NUM_REF_PICS][REFP_NUM];
};

/* encoder parameter */
typedef struct _ENC_PARAM
{
    /* picture size of input sequence (width) */
    int            horizontal_size;
    /* picture size of input sequence (height) */
    int            vertical_size;
    
    /* picture size of pictures in DPB (width) */
    int            pic_width;  // be a multiple of 8 (MINI_SIZE)
    /* picture size of pictures in DPB (height) */
    int            pic_height; // be a multiple of 8 (MINI_SIZE)
    /* qp value for I- and P- slice */
    int            qp;
    /* frame per second */
    int            fps;
    int         fps_num;
    int         fps_den;
    /* I-frame period */
    int            i_period;
#if USE_INTRA_REFRESH
	int            i_period_org;
#endif
    /* force I-frame */
    int            f_ifrm;
    /* picture bit depth*/
    int            bit_depth_input;
    int            bit_depth_internal;
    /* use picture signature embedding */
    int            use_pic_sign;
    int            max_b_frames;
    /* start bumping process if force_output is on */
    int            force_output;
    int            disable_hgop;
    int            gop_size;
    int            use_dqp;
    int            frame_qp_add;           /* 10 bits*/
    int            pb_frame_qp_offset;
    int            ipcm_enable_flag;
    int            amvr_enable_flag;
    int            affine_enable_flag;
    int            smvd_enable_flag;
    int            use_deblock;
    int            num_of_hmvp_cand;
    int            ipf_flag;
#if MIPF
	int            mipf_flag;
#endif
#if TSCPM
    int            tscpm_enable_flag;
#endif
#if SUB_TMVP
	int            sbtmvp_enable_flag;
#endif
#if CUDQP_QP_MAP
	u8             cu_delta_qp_flag;
	u8             cu_qp_group_size;
#endif
    int            umve_enable_flag;
#if UMVE_ENH
	int            umve_enh_enable_flag;
#endif
#if EXT_AMVR_HMVP
    int            emvr_enable_flag;
#endif
#if DT_PARTITION
    int            dt_intra_enable_flag;
#endif
    int            wq_enable;
    int            seq_wq_mode;
    char           seq_wq_user[2048];
    int            pic_wq_data_idx;
    char           pic_wq_user[2048];
    int            wq_param;
    int            wq_model;
    char           wq_param_detailed[256];
    char           wq_param_undetailed[256];

    int            sample_adaptive_offset_enable_flag;
    int            adaptive_leveling_filter_enable_flag;
    int            secondary_transform_enable_flag;
    u8             position_based_transform_enable_flag;
    u8             library_picture_enable_flag;
    u8             delta_qp_flag;
    u8             chroma_format;
    u8             encoding_precision;
#if HLS_RPL
    COM_RPL        rpls_l0[MAX_NUM_RPLS];
    COM_RPL        rpls_l1[MAX_NUM_RPLS];
    int            rpls_l0_cfg_num;
    int            rpls_l1_cfg_num;
#endif
#if PATCH
    int            patch_stable;
    int            cross_patch_loop_filter;
    int            patch_uniform;
    int            patch_ref_colocated;
    int            patch_width_in_lcu;
    int            patch_height_in_lcu;
    int            patch_columns;
    int            patch_rows;
    int            patch_column_width[64];
    int            patch_row_height[128];
#endif
#if LIBVC_ON
    int            qp_offset_libpic;
#endif
    int            sub_sample_ratio;
    int            frames_to_be_encoded;
    u8             ctu_size;
    u8             min_cu_size;
    u8             max_part_ratio;
    u8             max_split_times;
    u8             min_qt_size;
    u8             max_bt_size;
    u8             max_eqt_size;
    u8             max_dt_size;
    int            qp_offset_cb;
    int            qp_offset_cr;
    int            qp_offset_adp;
    int            bit_depth;
    
    //#========== Rate Control =========================
    int            rc_type;
    int            rc_bitrate;
    int            rc_max_bitrate;
    int            rc_vbv_buf_size;
    int            rc_min_qp;
    int            rc_max_qp;
    int            rc_crf;
		int			   rc_flag;
#if USE_SP
	int            sp_enable_flag;
	int            evs_ubvs_enable_flag;
#endif
#if USE_INTRA_REFRESH
	U1 EncRefresh;
	U1 RefreshMode;
	U8 RefreshSize;
#endif 
#if USE_SPEED_LEVEL
	U8 speed_level;
#endif
#if USE_ROI_QP_CONFIG
	int use_ROI_qp1;
	int ROI_qp_Start_x1;
	int ROI_qp_Start_y1;
	int ROI_qp_End_x1;
	int ROI_qp_End_y1;
	int ROI_QP1;

	int use_ROI_qp2;
	int ROI_qp_Start_x2;
	int ROI_qp_Start_y2;
	int ROI_qp_End_x2;
	int ROI_qp_End_y2;
	int ROI_QP2;

	int use_ROI_qp3;
	int ROI_qp_Start_x3;
	int ROI_qp_Start_y3;
	int ROI_qp_End_x3;
	int ROI_qp_End_y3;
	int ROI_QP3;

	int use_ROI_qp4;
	int ROI_qp_Start_x4;
	int ROI_qp_Start_y4;
	int ROI_qp_End_x4;
	int ROI_qp_End_y4;
	int ROI_QP4;

	int use_ROI_qp5;
	int ROI_qp_Start_x5;
	int ROI_qp_Start_y5;
	int ROI_qp_End_x5;
	int ROI_qp_End_y5;
	int ROI_QP5;

	int use_ROI_qp6;
	int ROI_qp_Start_x6;
	int ROI_qp_Start_y6;
	int ROI_qp_End_x6;
	int ROI_qp_End_y6;
	int ROI_QP6;

	int use_ROI_qp7;
	int ROI_qp_Start_x7;
	int ROI_qp_Start_y7;
	int ROI_qp_End_x7;
	int ROI_qp_End_y7;
	int ROI_QP7;

	int use_ROI_qp8;
	int ROI_qp_Start_x8;
	int ROI_qp_Start_y8;
	int ROI_qp_End_x8;
	int ROI_qp_End_y8;
	int ROI_QP8;
#endif
#if CUDQP_QP_MAP
	int useCuQPMAP;
#endif
#if USE_ROI_QP_CONFIG || USE_ROI_MODE_CONFIG
	int useQPMAP;
	int useModeMAP;
	int useQPMAPCon;
	int useModeMAPCon;
#endif

#if USE_ROI_MODE_CONFIG
	U1  use_ROI_Mode1;
	U12 ROI_Mode_Start_x1;
	U12 ROI_Mode_Start_y1;
	U12 ROI_Mode_End_x1;
	U12 ROI_Mode_End_y1;
	U3  ROI_Mode1;//MSB~LSB  intra inter skip  default value: 3'b111

	U1  use_ROI_Mode2;
	U12 ROI_Mode_Start_x2;
	U12 ROI_Mode_Start_y2;
	U12 ROI_Mode_End_x2;
	U12 ROI_Mode_End_y2;
	U3  ROI_Mode2;//MSB~LSB  intra inter skip  default value: 3'b111

	U1  use_ROI_Mode3;
	U12 ROI_Mode_Start_x3;
	U12 ROI_Mode_Start_y3;
	U12 ROI_Mode_End_x3;
	U12 ROI_Mode_End_y3;
	U3  ROI_Mode3;//MSB~LSB  intra inter skip  default value: 3'b111

	U1  use_ROI_Mode4;
	U12 ROI_Mode_Start_x4;
	U12 ROI_Mode_Start_y4;
	U12 ROI_Mode_End_x4;
	U12 ROI_Mode_End_y4;
	U3  ROI_Mode4;//MSB~LSB  intra inter skip  default value: 3'b111

	U1  use_ROI_Mode5;
	U12 ROI_Mode_Start_x5;
	U12 ROI_Mode_Start_y5;
	U12 ROI_Mode_End_x5;
	U12 ROI_Mode_End_y5;
	U3  ROI_Mode5;//MSB~LSB  intra inter skip  default value: 3'b111

	U1  use_ROI_Mode6;
	U12 ROI_Mode_Start_x6;
	U12 ROI_Mode_Start_y6;
	U12 ROI_Mode_End_x6;
	U12 ROI_Mode_End_y6;
	U3  ROI_Mode6;//MSB~LSB  intra inter skip  default value: 3'b111

	U1  use_ROI_Mode7;
	U12 ROI_Mode_Start_x7;
	U12 ROI_Mode_Start_y7;
	U12 ROI_Mode_End_x7;
	U12 ROI_Mode_End_y7;
	U3  ROI_Mode7;//MSB~LSB  intra inter skip  default value: 3'b111

	U1  use_ROI_Mode8;
	U12 ROI_Mode_Start_x8;
	U12 ROI_Mode_Start_y8;
	U12 ROI_Mode_End_x8;
	U12 ROI_Mode_End_y8;
	U3  ROI_Mode8;//MSB~LSB  intra inter skip  default value: 3'b111
#endif

#if USE_TRACE_DUMP
	int useTraceDump;
#endif
} ENC_PARAM;

typedef struct _ENC_SBAC
{
    u32            range;
    u32            code;
    int            left_bits;
    u32            stacked_ff;
    u32            pending_byte;
    u32            is_pending_byte;
    COM_SBAC_CTX  ctx;
    u32            bitcounter;
    u8             is_bitcount;
} ENC_SBAC;

typedef struct _ENC_SBAC_SKIP
{
	u32            range;
	u32            code;
	int            left_bits;
	u32            stacked_ff;
	u32            pending_byte;
	u32            is_pending_byte;
	COM_SBAC_CTX_SKIP  ctx;
	u32            bitcounter;
	u8             is_bitcount;
} ENC_SBAC_SKIP;
typedef struct _COM_SP_INFO
{
	u8  is_matched;
	u16 length;
	s16 offset_x;
	s16 offset_y;
	int match_dict[4];
	pel pixel[4][N_C];
	u8  n_recent_flag;
	s8  n_recent_idx;
} COM_SP_INFO;

typedef struct _COM_SP_EVS_INFO
{
	u8  is_matched;
	u16 length;
	pel pixel[N_C];
	u8  match_type;
	u8  srb_index;
	u8  pv_type;
	u8  pvflag;
	u16 srb_bits;
	u16 srb_dist;
	u8  esc_flag;
	int pos;
}COM_SP_EVS_INFO;
typedef struct _COM_SP_PIX
{
	pel Y;
	pel U;
	pel V;
}COM_SP_PIX;

typedef struct _ENC_CU_DATA_COPY
{
	COM_SP_INFO sp_strInfo[CU_SIZE];
	COM_SP_EVS_INFO evs_str_copy_info[CU_SIZE];
	COM_SP_PIX unpred_pix_info[CU_SIZE];
} ENC_CU_DATA_COPY;

typedef struct _ENC_CU_DATA
{
//#define CU_SIZE (1 << (MAX_CU_DEPTH - 1))* (1 << (MAX_CU_DEPTH - 1))
    U8  split_mode[MAX_CU_DEPTH][NUM_BLOCK_SHAPE][MAX_CU_CNT_IN_LCU];
#if TB_SPLIT_EXT
    //int* pb_part;
   // int* tb_part;
	int pb_part[CU_SIZE];
	int tb_part[CU_SIZE];
#endif
    /*u8  *pred_mode;
    u8  **mpm;
    u8  **mpm_ext;
    s8  **ipm;
	s8  **refi;*/
	u8  pred_mode[CU_SIZE];
	u8  mpm[2][CU_SIZE];
	U8  mpm_ext[8][CU_SIZE];
	S8  ipm[2][CU_SIZE];
	S3  refi[CU_SIZE][REFP_NUM];
   
	U8  mvr_idx[CU_SIZE];
	U8  umve_flag[CU_SIZE];
	U8  umve_idx[CU_SIZE];
	U8  skip_idx[CU_SIZE];

    /*u8  *mvr_idx;
    u8  *umve_flag;
    u8  *umve_idx;
    u8  *skip_idx;*/
#if EXT_AMVR_HMVP
   // u8  *mvp_from_hmvp_flag;
	U8  mvp_from_hmvp_flag[CU_SIZE];
#endif

    S14 mv[MAX_CU_CNT_IN_LCU][REFP_NUM][MV_D];
    s16 mvd[MAX_CU_CNT_IN_LCU][REFP_NUM][MV_D];
	int num_nz_coef[N_C][CU_SIZE];
	u32 map_scu[CU_SIZE];
	U8  affine_flag[CU_SIZE];
    //int *num_nz_coef[N_C];
    //u32 *map_scu;
   // u8  *affine_flag;
#if SMVD
   // u8  *smvd_flag;
	U8  smvd_flag[CU_SIZE];
#endif
    //u32 *map_cu_mode;
	u32 map_cu_mode[CU_SIZE];
#if TB_SPLIT_EXT
   // u32 *map_pb_tb_part;
	u32 map_pb_tb_part[CU_SIZE];
#endif
	U4  depth[CU_SIZE];
	U8  ipf_flag[CU_SIZE];
	// s16 coef[N_C][CU_SIZE << 4];
	s16 coef_y[CU_SIZE >> 2][CU_SIZE >> 2];
	s16 coef_u[(CU_SIZE << 4) >> 2];
	s16 coef_v[(CU_SIZE << 4) >> 2];

	//pel reco[N_C][CU_SIZE << 4];
	pel reco_y[CU_SIZE >> 2][CU_SIZE >> 2];
	pel reco_u[(CU_SIZE << 4) >> 2];
	pel reco_v[(CU_SIZE << 4) >> 2];
#if SAWP
	u8 sawp_flag[CU_SIZE];
	//u8* sawp_idx0;
	//u8* sawp_idx1;
	u8 sawp_idx0[CU_SIZE];
	u8 sawp_idx1[CU_SIZE];
#endif

#if AWP_MVR
	u8 awp_mvr_flag0[CU_SIZE];
	//u8* awp_mvr_idx0;
	u8 awp_mvr_idx0[CU_SIZE];
	u8 awp_mvr_flag1[CU_SIZE];
	//u8* awp_mvr_idx1;
	u8 awp_mvr_idx1[CU_SIZE];
#endif

#if IBC_ABVR
	//u8* bvr_idx;
	u8 bvr_idx[CU_SIZE];
#endif
#if INTERPF
	u8 inter_filter_flag[CU_SIZE];
#endif
#if IPC
	u8 ipc_flag[CU_SIZE];
#endif
#if BGC
	u8 bgc_flag[CU_SIZE];
	//u8* bgc_idx;
	u8 bgc_idx[CU_SIZE];
#endif

#if USE_IBC
	u8 ibc_flag[CU_SIZE];
#endif
#if IBC_BVP
	//u8* cbvp_idx;
	//s8* cnt_hbvp_cands;
	u8 cbvp_idx[CU_SIZE];
	s8 cnt_hbvp_cands[CU_SIZE];
#endif
#if USE_SP
	u8 map_usp[CU_SIZE];
	u8 sp_flag[CU_SIZE];
	u16 sub_string_no[CU_SIZE];
	COM_SP_INFO sp_strInfo[CU_SIZE];
	u8 sp_copy_direction[CU_SIZE];
	u8 is_sp_pix_completed[CU_SIZE];
	u8 cs2_flag[CU_SIZE];
	u8 evs_copy_direction[CU_SIZE];
	int evs_sub_string_no[CU_SIZE];
	COM_SP_EVS_INFO evs_str_copy_info[CU_SIZE];
	int unpred_pix_num[CU_SIZE];
	COM_SP_PIX unpred_pix_info[CU_SIZE];
	u8 equal_val_str_present_flag[CU_SIZE];
	u8 unpredictable_pix_present_flag[CU_SIZE];
	u8 pvbuf_size[CU_SIZE];
	u8 pvbuf_size_prev[CU_SIZE];
	s16 p_SRB[CU_SIZE];
	u8 pvbuf_reused_flag[CU_SIZE];
	s16 p_SRB_prev[CU_SIZE];
	u8 all_comp_flag[CU_SIZE];
	u8 all_comp_pre_flag[CU_SIZE];
	u8 m_dpb_idx[CU_SIZE];
	u8 m_dpb_idx_prev[CU_SIZE];
	u8 m_dpb_reYonly[CU_SIZE];
	u8 m_dpb_reYonly_prev[CU_SIZE];
	u8 cuS_flag[CU_SIZE];
	u8 cuS_pre_flag[CU_SIZE];
	s16 pv_x[CU_SIZE];
	s16 pv_x_prev[CU_SIZE];
	s16 pv_y[CU_SIZE];
	s16 pv_y_prev[CU_SIZE];
	s16 srb_u[CU_SIZE];
	s16 srb_v[CU_SIZE];
#endif

#if AFFINE_UMVE
	u8 affine_umve_flag[CU_SIZE];
	s8 affine_umve_idx[VER_NUM][CU_SIZE];
#endif
#if ETMVP
	u8 etmvp_flag[CU_SIZE];
#endif
#if UNIFIED_HMVP_1
	u8 mvap_flag[CU_SIZE];
	u8 sub_tmvp_flag[CU_SIZE];
#endif
#if AWP
	u8 awp_flag[CU_SIZE];
	//u8* awp_idx0;
	//u8* awp_idx1;
	u8 awp_idx0[CU_SIZE];
	u8 awp_idx1[CU_SIZE];
#endif
#if ST_CHROMA
	u8 st_chroma_tu_flag[CU_SIZE];
#endif
#if IIP
	u8 iip_flag[CU_SIZE];
#endif
#if IST
	u8 ist_tu_flag[CU_SIZE];
#endif
#if EST
	u8 est_tu_flag[CU_SIZE];
#endif
#if SAWP
	//u8** sawp_mpm;
	u8 sawp_mpm[2][CU_SIZE];
#endif // SAWP

	//for sao
	pel y_org_sao[64][64];
	pel u_org_sao[32][32];
	pel v_org_sao[32][32];
#if CUDQP_QP_MAP
	U6 	cu_luma_qp[CU_SIZE];
#endif

	void print(int w, int h) const
	{
#if PRINT_CU_DATA
		::printf("split_mode\n");
		for (int i = 0; i < MAX_CU_DEPTH; ++i)
		{
			for (int j = 0; j < NUM_BLOCK_SHAPE; ++j)
			{
				::printf("%d %d: ", i, j);
				for (int k = 0; k < 256; ++k)
				{
					::printf("%d ", split_mode[i][j][k]);
				}
				::printf("\n");
			}
		}
		::printf("ipm\n");
		for (int k = 0; k < 2; ++k)
		{
			for (int i = 0; i < (h >> 2); ++i)
			{
				for (int j = 0; j < (w >> 2); ++j)
				{
					::printf("%d ", ipm[k][i * (w >> 2) + j]);
				}
			}
			::printf("\n");
		}
#endif
	}

	void copy_split_mode(const _ENC_CU_DATA& _Right)
	{

#pragma HLS INLINE OFF
		for (int k = 0; k < MAX_CU_CNT_IN_LCU; ++k)
		{
#pragma HLS pipeline
#pragma HLS unroll factor=4
			for (int j = 0; j < NUM_BLOCK_SHAPE; ++j)
			{
				for (int i = 0; i < MAX_CU_DEPTH; ++i)
				{
					split_mode[i][j][k] = _Right.split_mode[i][j][k];
				}
			}
		}
	}

	void copy_cu_info(const _ENC_CU_DATA& _Right)
	{
#pragma HLS INLINE OFF
		for (int i = 0; i < CU_SIZE; ++i)
		{
#pragma HLS pipeline
#if TB_SPLIT_EXT
			pb_part[i] = _Right.pb_part[i];
			tb_part[i] = _Right.tb_part[i];
#endif
			pred_mode[i] = _Right.pred_mode[i];
			for (int j = 0; j < 2; ++j)
			{
				mpm[j][i] = _Right.mpm[j][i];
				ipm[j][i] = _Right.ipm[j][i];
				refi[i][j] = _Right.refi[i][j];
				for (int k = 0; k < 2; ++k)
				{
					mv[i][j][k] = _Right.mv[i][j][k];
					mvd[i][j][k] = _Right.mvd[i][j][k];
				}
			}
			//U8  mpm_ext[8][CU_SIZE];
			mvr_idx[i] = _Right.mvr_idx[i];
			umve_flag[i] = _Right.umve_flag[i];
			umve_idx[i] = _Right.umve_idx[i];
			skip_idx[i] = _Right.skip_idx[i];
#if EXT_AMVR_HMVP
			mvp_from_hmvp_flag[i] = _Right.mvp_from_hmvp_flag[i];
#endif
			for (int j = 0; j < 3; ++j)
			{
				num_nz_coef[j][i] = _Right.num_nz_coef[j][i];
			}
			affine_flag[i] = _Right.affine_flag[i];
			ipf_flag[i] = _Right.ipf_flag[i];
			map_scu[i] = _Right.map_scu[i];
			map_cu_mode[i] = _Right.map_cu_mode[i];
			//U8  smvd_flag[CU_SIZE];
			//u32 map_pb_tb_part[CU_SIZE];
			//U4  depth[CU_SIZE];
		}

	}

	void copy_coef_reco(const _ENC_CU_DATA& _Right)
			{
#pragma HLS INLINE OFF
		for (int i = 0; i < (CU_SIZE >> 4); ++i)
		{
			for (int j = 0; j < (CU_SIZE >> 4); ++j)
			{
#pragma HLS pipeline
				for (int m = 0; m < 4; ++m)
				{
					for (int n = 0; n < 4; ++n)
					{
						coef_y[4*i+m][4*j+n] = _Right.coef_y[4*i+m][4*j+n];
						reco_y[4*i+m][4*j+n] = _Right.reco_y[4*i+m][4*j+n];
					}
				}
			}
		}
	}
	void copy(const _ENC_CU_DATA& _Right)
	{
		copy_split_mode(_Right);
		copy_cu_info(_Right);
		copy_coef_reco(_Right);
	}

} ENC_CU_DATA;
typedef struct _ENC_CU_DATA1
{
#if TB_SPLIT_EXT
	int pb_part[CU_SIZE];
	int tb_part[CU_SIZE];
#endif
	u8  pred_mode[CU_SIZE];
	u8  mpm[2][CU_SIZE];
	//U8  mpm_ext[8][CU_SIZE];
	S8  ipm[2][CU_SIZE];
	S3  refi[CU_SIZE][REFP_NUM];
	U8  mvr_idx[CU_SIZE];
	U8  umve_flag[CU_SIZE];
	U8  umve_idx[CU_SIZE];
	U8  skip_idx[CU_SIZE];
#if EXT_AMVR_HMVP
	U8  mvp_from_hmvp_flag[CU_SIZE];
#endif
	//S14 mv[MAX_CU_CNT_IN_LCU][REFP_NUM][MV_D];
	s16 mvd[MAX_CU_CNT_IN_LCU][REFP_NUM][MV_D];
	int num_nz_coef[N_C][CU_SIZE];
	//u32 map_scu[CU_SIZE];
	U8  affine_flag[CU_SIZE];
#if SMVD
	//U8  smvd_flag[CU_SIZE];
#endif
	//u32 map_cu_mode[CU_SIZE];
#if TB_SPLIT_EXT
	//u32 map_pb_tb_part[CU_SIZE];
#endif
	//U4  depth[CU_SIZE];
	U8  ipf_flag[CU_SIZE];
	s16 coef[N_C][CU_SIZE << 4];
	//S16 reco_y[CU_SIZE << 4];
	//S16 reco_u[(CU_SIZE << 4) >> 2];
	//S16 reco_v[(CU_SIZE << 4) >> 2];
} ENC_CU_DATA1;

typedef struct _ENC_CU_DATA_ARRAY
{
	U8  split_mode[MAX_CU_DEPTH][NUM_BLOCK_SHAPE][MAX_CU_CNT_IN_LCU];
	S14 mv[MAX_CU_CNT_IN_LCU][REFP_NUM][MV_D];
	S16 mvd[MAX_CU_CNT_IN_LCU][REFP_NUM][MV_D];

#define CU_SIZE (1 << (MAX_CU_DEPTH - 1))* (1 << (MAX_CU_DEPTH - 1))

	int pb_part[CU_SIZE];
	int tb_part[CU_SIZE];
	U3  pred_mode[CU_SIZE];
	U8  mvr_idx[CU_SIZE];
	U8  umve_flag[CU_SIZE];
	U8  umve_idx[CU_SIZE];
	U8  skip_idx[CU_SIZE];
	u32 map_scu[CU_SIZE];
	U8  affine_flag[CU_SIZE];
	U8  smvd_flag[CU_SIZE];
	U32 map_cu_mode[CU_SIZE];
	U32 map_pb_tb_part[CU_SIZE];
	U4  depth[CU_SIZE];
	U8 ipf_flag[CU_SIZE];
	U8 mvp_from_hmvp_flag[CU_SIZE];

#if INTERPF
	U8 inter_filter_flag[CU_SIZE];
#endif
	U8  mpm[2][CU_SIZE];
	U8  mpm_ext[8][CU_SIZE];
	S8  ipm[2][CU_SIZE];
	S3  refi[CU_SIZE][REFP_NUM];

	int num_nz_coef[N_C][CU_SIZE];
	//s16 coef[N_C][CU_SIZE << 4];
	S16 coef_y[CU_SIZE >> 2][CU_SIZE >> 2];
	S16 coef_u[(CU_SIZE << 4)>>2];
	S16 coef_v[(CU_SIZE << 4)>>2];

	//pel reco[N_C][CU_SIZE << 4];
	pel reco_y[CU_SIZE >> 2][CU_SIZE >> 2];
	pel reco_u[(CU_SIZE << 4) >> 2];
	pel reco_v[(CU_SIZE << 4) >> 2];

	U12 x_chroma;
	U12 y_chroma;
	U12 x_last;
	U12 y_last;
	BOOL last_child;
#if CUDQP_QP_MAP
	U6 	cu_luma_qp[CU_SIZE];
#endif
} ENC_CU_DATA_ARRAY;


typedef struct _ENC_CU_DATA_ARRAY_INNER
{
	S14 mv[MAX_CU_CNT_IN_LCU][REFP_NUM][MV_D];
	S16 mvd[MAX_CU_CNT_IN_LCU][REFP_NUM][MV_D];

#define CU_SIZE (1 << (MAX_CU_DEPTH - 1))* (1 << (MAX_CU_DEPTH - 1))

	int pb_part[CU_SIZE];
	int tb_part[CU_SIZE];
	U3  pred_mode[CU_SIZE];
	U8  mvr_idx[CU_SIZE];
	U8  umve_flag[CU_SIZE];
	U8  umve_idx[CU_SIZE];
	U8  skip_idx[CU_SIZE];
	u32 map_scu[CU_SIZE];
	U8  affine_flag[CU_SIZE];
	U8  smvd_flag[CU_SIZE];
	U32 map_cu_mode[CU_SIZE];
	U32 map_pb_tb_part[CU_SIZE];
	U4  depth[CU_SIZE];
	U8 ipf_flag[CU_SIZE];
	U8 mvp_from_hmvp_flag[CU_SIZE];


	U8  mpm[2][CU_SIZE];
	U8  mpm_ext[8][CU_SIZE];
	S8  ipm[2][CU_SIZE];
	S3  refi[CU_SIZE][REFP_NUM];

	int num_nz_coef[N_C][CU_SIZE];
	//s16 coef[N_C][CU_SIZE << 4];
	S16 coef_y[CU_SIZE >> 2][CU_SIZE >> 2];
	S16 coef_u[(CU_SIZE << 4) >> 2];
	S16 coef_v[(CU_SIZE << 4) >> 2];

	//pel reco[N_C][CU_SIZE << 4];
	pel reco_y[CU_SIZE >> 2][CU_SIZE >> 2];
	pel reco_u[(CU_SIZE << 4) >> 2];
	pel reco_v[(CU_SIZE << 4) >> 2];

	U12 x_chroma;
	U12 y_chroma;
	U12 x_last;
	U12 y_last;
	BOOL last_child;
#if CUDQP_QP_MAP
	U6 	cu_luma_qp[CU_SIZE];
#endif
} ENC_CU_DATA_ARRAY_INNER;

template<size_t CUR_SIZE>
struct ENC_CU_DATA_ARRAY_SIZE
{
#define SIZE_SCU	(CUR_SIZE >> 2) * (CUR_SIZE >> 2)
	U8		split_mode[MAX_CU_DEPTH][NUM_BLOCK_SHAPE][256];
	S14		mv[SIZE_SCU][REFP_NUM][MV_D];
	S16		mvd[SIZE_SCU][REFP_NUM][MV_D];

	int		pb_part[SIZE_SCU];
	int		tb_part[SIZE_SCU];
	U3		pred_mode[SIZE_SCU];
	U8		mvr_idx[SIZE_SCU];
	U8		umve_flag[SIZE_SCU];
	U8		umve_idx[SIZE_SCU];
	U8		skip_idx[SIZE_SCU];
	u32		map_scu[SIZE_SCU];
	U8		affine_flag[SIZE_SCU];
	U8		smvd_flag[SIZE_SCU];
	U32		map_cu_mode[SIZE_SCU];
	U32		map_pb_tb_part[SIZE_SCU];
	U4		depth[SIZE_SCU];
	U8		ipf_flag[SIZE_SCU];
	U8		mvp_from_hmvp_flag[SIZE_SCU];

#if INTERPF
	U8		inter_filter_flag[SIZE_SCU];
#endif

	U8		mpm[2][SIZE_SCU];
	U8		mpm_ext[8][SIZE_SCU];
	S8		ipm[2][SIZE_SCU];
	S3		refi[SIZE_SCU][REFP_NUM];

	int		num_nz_coef[N_C][SIZE_SCU];

	S16		coef_y[CUR_SIZE][CUR_SIZE];

	pel		reco_y[CUR_SIZE][CUR_SIZE];

	U12		x_chroma;
	U12		y_chroma;
	U12		x_last;
	U12		y_last;
	BOOL	last_child;

#if CUDQP_QP_MAP
	int cu_qp_group_pred_qp;
	U8		cu_luma_qp[SIZE_SCU];
#endif

	void print(int w, int h) const
	{
#if PRINT_CU_DATA_SIZE
		printf("split_mode\n");
		for (int i = 0; i < MAX_CU_DEPTH; ++i)
		{
			for (int j = 0; j < NUM_BLOCK_SHAPE; ++j)
			{
				printf("%d %d: ", i, j);
				for (int k = 0; k < 256; ++k)
				{
					printf("%d ", split_mode[i][j][k]);
				}
				printf("\n");
			}
		}
		printf("ipm\n");
		for (int k = 0; k < 2; ++k)
		{
			for (int i = 0; i < (h >> 2); ++i)
			{
				for (int j = 0; j < (w >> 2); ++j)
				{
					printf("%d ", ipm[k][i * (w >> 2) + j]);
				}
			}
			printf("\n");
		}
#endif
	}

	void get_cu_data_from_curblk(ENC_CU_DATA_ARRAY_SIZE<CUR_SIZE / 2>* src, int x_cur_cu, int y_cur_cu, int cu_width_log2, int cu_height_log2, int log2_prev_cuw)
	{
#if SYN_TOP_GET_CU_DATA_FROM_CURBLK
#pragma HLS ARRAY_PARTITION variable=ipm complete dim=1
#pragma HLS ARRAY_PARTITION variable=mpm complete dim=1
#pragma HLS ARRAY_PARTITION variable=mv complete dim=2
#pragma HLS ARRAY_PARTITION variable=mv complete dim=3
#pragma HLS ARRAY_PARTITION variable=mvd complete dim=2
#pragma HLS ARRAY_PARTITION variable=mvd complete dim=3
#pragma HLS ARRAY_PARTITION variable=refi complete dim=2
#pragma HLS ARRAY_PARTITION variable=mpm_ext complete dim=1
#pragma HLS ARRAY_PARTITION variable=num_nz_coef complete dim=1
#pragma HLS ARRAY_PARTITION variable=coef_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=coef_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=reco_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=reco_y cyclic factor=4 dim=2

#pragma HLS ARRAY_PARTITION variable=src->ipm complete dim=1
#pragma HLS ARRAY_PARTITION variable=src->mpm complete dim=1
#pragma HLS ARRAY_PARTITION variable=src->mv complete dim=2
#pragma HLS ARRAY_PARTITION variable=src->mv complete dim=3
#pragma HLS ARRAY_PARTITION variable=src->mvd complete dim=2
#pragma HLS ARRAY_PARTITION variable=src->mvd complete dim=3
#pragma HLS ARRAY_PARTITION variable=src->refi complete dim=2
#pragma HLS ARRAY_PARTITION variable=src->mpm_ext complete dim=1
#pragma HLS ARRAY_PARTITION variable=src->num_nz_coef complete dim=1
#pragma HLS ARRAY_PARTITION variable=src->coef_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=src->coef_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=src->reco_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=src->reco_y cyclic factor=4 dim=2
#endif //SYN_TOP_GET_CU_DATA_FROM_CURBLK
//#pragma HLS ARRAY_PARTITION variable=coef_y cyclic factor=4 dim=1
//#pragma HLS ARRAY_PARTITION variable=coef_y cyclic factor=4 dim=2
//#pragma HLS ARRAY_PARTITION variable=reco_y cyclic factor=4 dim=1
//#pragma HLS ARRAY_PARTITION variable=reco_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=src->mpm_ext complete dim=1
#pragma HLS ARRAY_PARTITION variable=mpm_ext complete dim=1
#pragma HLS ARRAY_PARTITION variable=mvd complete dim=2
#pragma HLS ARRAY_PARTITION variable=mvd complete dim=3
#pragma HLS ARRAY_PARTITION variable=src->mvd complete dim=2
#pragma HLS ARRAY_PARTITION variable=src->mvd complete dim=3
		int x_cur_cu_scu = x_cur_cu >> 2;
		int y_cur_cu_scu = y_cur_cu >> 2;
		int cuh_scu = 1 << (cu_height_log2 - 2);
		int cuw_scu = 1 << (cu_width_log2 - 2);
		
		for (int i = 0; i < cuh_scu; i++)
		{
#pragma HLS LOOP_TRIPCOUNT max=CUR_SIZE/8			
#pragma HLS pipeline			
			int idx_dst = ((y_cur_cu_scu + i) << (log2_prev_cuw - 2)) + x_cur_cu_scu;
			int idx_src = i << (cu_width_log2 - 2);

			for (int j = 0; j < cuw_scu; j++)
			{
#pragma HLS LOOP_TRIPCOUNT max=CUR_SIZE/8
#pragma HLS PIPELINE
				(pred_mode + idx_dst)[j] = (src->pred_mode + idx_src)[j];
#if CUDQP_QP_MAP
				(cu_luma_qp + idx_dst)[j] = (src->cu_luma_qp + idx_src)[j];
#endif
				(mpm[0] + idx_dst)[j] = (src->mpm[0] + idx_src)[j];
				(mpm[1] + idx_dst)[j] = (src->mpm[1] + idx_src)[j];
				(ipm[0] + idx_dst)[j] = (src->ipm[0] + idx_src)[j];

				(affine_flag + idx_dst)[j] = (src->affine_flag + idx_src)[j];
				(smvd_flag + idx_dst)[j] = (src->smvd_flag + idx_src)[j];
				(depth + idx_dst)[j] = (src->depth + idx_src)[j];
				(map_scu + idx_dst)[j] = (src->map_scu + idx_src)[j];
				(map_cu_mode + idx_dst)[j] = (src->map_cu_mode + idx_src)[j];
				(map_pb_tb_part + idx_dst)[j] = (src->map_pb_tb_part + idx_src)[j];
				
				(umve_flag + idx_dst)[j] = (src->umve_flag + idx_src)[j];
				(umve_idx + idx_dst)[j] = (src->umve_idx + idx_src)[j];
				(skip_idx + idx_dst)[j] = (src->skip_idx + idx_src)[j];
				(mvr_idx + idx_dst)[j] = (src->mvr_idx + idx_src)[j];
				(mvp_from_hmvp_flag + idx_dst)[j] = (src->mvp_from_hmvp_flag + idx_src)[j];
				(ipf_flag + idx_dst)[j] = (src->ipf_flag + idx_src)[j];

				(mv + idx_dst)[j][0][0] = (src->mv + idx_src)[j][0][0];
				(mv + idx_dst)[j][0][1] = (src->mv + idx_src)[j][0][1];
				(mv + idx_dst)[j][1][0] = (src->mv + idx_src)[j][1][0];
				(mv + idx_dst)[j][1][1] = (src->mv + idx_src)[j][1][1];
				(mvd + idx_dst)[j][0][0] = (src->mvd + idx_src)[j][0][0];
				(mvd + idx_dst)[j][0][1] = (src->mvd + idx_src)[j][0][1];
				(mvd + idx_dst)[j][1][0] = (src->mvd + idx_src)[j][1][0];
				(mvd + idx_dst)[j][1][1] = (src->mvd + idx_src)[j][1][1];
				(refi + idx_dst)[j][0] = (src->refi + idx_src)[j][0];
				(refi + idx_dst)[j][1] = (src->refi + idx_src)[j][1];

				(num_nz_coef[0] + idx_dst)[j] = (src->num_nz_coef[0] + idx_src)[j];
				(pb_part + idx_dst)[j] = (src->pb_part + idx_src)[j];
				(tb_part + idx_dst)[j] = (src->tb_part + idx_src)[j];
				for (int m = 0; m < 8; m++)
				{
					(mpm_ext[m] + idx_dst)[j] = (src->mpm_ext[m] + idx_src)[j];
				}
				for (int m = 0; m < 4; m++) {
					for (int n = 0; n < 4; n++) {
						coef_y[y_cur_cu + i * 4 + m][x_cur_cu + j * 4 + n] = src->coef_y[i * 4 + m][j * 4 + n];
						reco_y[y_cur_cu + i * 4 + m][x_cur_cu + j * 4 + n] = src->reco_y[i * 4 + m][j * 4 + n];
					}
				}
			}
		}
	}
	void get_cu_data_from_curblk(ENC_CU_DATA_ARRAY_SIZE<CUR_SIZE>* src, int x_cur_cu, int y_cur_cu, int cu_width_log2, int cu_height_log2, int log2_prev_cuw)
	{
#if SYN_TOP_GET_CU_DATA_FROM_CURBLK
#pragma HLS ARRAY_PARTITION variable=ipm complete dim=1
#pragma HLS ARRAY_PARTITION variable=mpm complete dim=1
#pragma HLS ARRAY_PARTITION variable=mv complete dim=2
#pragma HLS ARRAY_PARTITION variable=mv complete dim=3
#pragma HLS ARRAY_PARTITION variable=mvd complete dim=2
#pragma HLS ARRAY_PARTITION variable=mvd complete dim=3
#pragma HLS ARRAY_PARTITION variable=refi complete dim=2
#pragma HLS ARRAY_PARTITION variable=mpm_ext complete dim=1
#pragma HLS ARRAY_PARTITION variable=num_nz_coef complete dim=1
#pragma HLS ARRAY_PARTITION variable=coef_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=coef_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=reco_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=reco_y cyclic factor=4 dim=2

#pragma HLS ARRAY_PARTITION variable=src->ipm complete dim=1
#pragma HLS ARRAY_PARTITION variable=src->mpm complete dim=1
#pragma HLS ARRAY_PARTITION variable=src->mv complete dim=2
#pragma HLS ARRAY_PARTITION variable=src->mv complete dim=3
#pragma HLS ARRAY_PARTITION variable=src->mvd complete dim=2
#pragma HLS ARRAY_PARTITION variable=src->mvd complete dim=3
#pragma HLS ARRAY_PARTITION variable=src->refi complete dim=2
#pragma HLS ARRAY_PARTITION variable=src->mpm_ext complete dim=1
#pragma HLS ARRAY_PARTITION variable=src->num_nz_coef complete dim=1
#pragma HLS ARRAY_PARTITION variable=src->coef_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=src->coef_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=src->reco_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=src->reco_y cyclic factor=4 dim=2
#endif //SYN_TOP_GET_CU_DATA_FROM_CURBLK
#pragma HLS ARRAY_PARTITION variable=mpm_ext complete dim=0
#pragma HLS ARRAY_PARTITION variable=src->mpm_ext complete dim=0

#pragma HLS ARRAY_PARTITION variable=coef_y complete dim=1
#pragma HLS ARRAY_PARTITION variable=coef_y complete dim=2
#pragma HLS ARRAY_PARTITION variable=reco_y complete dim=1
#pragma HLS ARRAY_PARTITION variable=reco_y complete dim=2

#pragma HLS ARRAY_PARTITION variable=src->coef_y complete dim=1
#pragma HLS ARRAY_PARTITION variable=src->coef_y complete dim=2
#pragma HLS ARRAY_PARTITION variable=src->reco_y complete dim=1
#pragma HLS ARRAY_PARTITION variable=src->reco_y complete dim=2

#pragma HLS ARRAY_PARTITION variable=src->ipm complete dim=2
#pragma HLS ARRAY_PARTITION variable=src->mpm complete dim=2
		int x_cur_cu_scu = x_cur_cu >> 2;
		int y_cur_cu_scu = y_cur_cu >> 2;
		int cuh_scu = 1 << (cu_height_log2 - 2);
		int cuw_scu = 1 << (cu_width_log2 - 2);


		for (int i = 0; i < cuh_scu; i++)
		{
#pragma HLS LOOP_TRIPCOUNT max=CUR_SIZE/4
#pragma HLS pipeline
			int idx_dst = ((y_cur_cu_scu + i) << (log2_prev_cuw - 2)) + x_cur_cu_scu;
			int idx_src = i << (cu_width_log2 - 2);

			for (int j = 0; j < cuw_scu; j++)
			{
#pragma HLS LOOP_TRIPCOUNT max=CUR_SIZE/4
#pragma HLS PIPELINE
				(pred_mode + idx_dst)[j] = (src->pred_mode + idx_src)[j];
#if CUDQP_QP_MAP
				(cu_luma_qp + idx_dst)[j] = (src->cu_luma_qp + idx_src)[j];
#endif
				(mpm[0] + idx_dst)[j] = (src->mpm[0] + idx_src)[j];
				(mpm[1] + idx_dst)[j] = (src->mpm[1] + idx_src)[j];
				(ipm[0] + idx_dst)[j] = (src->ipm[0] + idx_src)[j];

				(affine_flag + idx_dst)[j] = (src->affine_flag + idx_src)[j];
				(smvd_flag + idx_dst)[j] = (src->smvd_flag + idx_src)[j];
				(depth + idx_dst)[j] = (src->depth + idx_src)[j];
				(map_scu + idx_dst)[j] = (src->map_scu + idx_src)[j];
				(map_cu_mode + idx_dst)[j] = (src->map_cu_mode + idx_src)[j];
				(map_pb_tb_part + idx_dst)[j] = (src->map_pb_tb_part + idx_src)[j];

				(umve_flag + idx_dst)[j] = (src->umve_flag + idx_src)[j];
				(umve_idx + idx_dst)[j] = (src->umve_idx + idx_src)[j];
				(skip_idx + idx_dst)[j] = (src->skip_idx + idx_src)[j];
				(mvr_idx + idx_dst)[j] = (src->mvr_idx + idx_src)[j];
				(mvp_from_hmvp_flag + idx_dst)[j] = (src->mvp_from_hmvp_flag + idx_src)[j];
				(ipf_flag + idx_dst)[j] = (src->ipf_flag + idx_src)[j];

				(mv + idx_dst)[j][0][0] = (src->mv + idx_src)[j][0][0];
				(mv + idx_dst)[j][0][1] = (src->mv + idx_src)[j][0][1];
				(mv + idx_dst)[j][1][0] = (src->mv + idx_src)[j][1][0];
				(mv + idx_dst)[j][1][1] = (src->mv + idx_src)[j][1][1];
				(mvd + idx_dst)[j][0][0] = (src->mvd + idx_src)[j][0][0];
				(mvd + idx_dst)[j][0][1] = (src->mvd + idx_src)[j][0][1];
				(mvd + idx_dst)[j][1][0] = (src->mvd + idx_src)[j][1][0];
				(mvd + idx_dst)[j][1][1] = (src->mvd + idx_src)[j][1][1];
				(refi + idx_dst)[j][0] = (src->refi + idx_src)[j][0];
				(refi + idx_dst)[j][1] = (src->refi + idx_src)[j][1];

				(num_nz_coef[0] + idx_dst)[j] = (src->num_nz_coef[0] + idx_src)[j];
				(pb_part + idx_dst)[j] = (src->pb_part + idx_src)[j];
				(tb_part + idx_dst)[j] = (src->tb_part + idx_src)[j];
				for (int m = 0; m < 8; m++)
				{
					(mpm_ext[m] + idx_dst)[j] = (src->mpm_ext[m] + idx_src)[j];
				}
				 for (int m = 0; m < 4; m++) {
					for (int n = 0; n < 4; n++) {
						coef_y[y_cur_cu + i * 4 + m][x_cur_cu + j * 4 + n] = src->coef_y[i * 4 + m][j * 4 + n];
						reco_y[y_cur_cu + i * 4 + m][x_cur_cu + j * 4 + n] = src->reco_y[i * 4 + m][j * 4 + n];
					}
				}
			}

		}
	}
};

template<size_t CUR_SIZE>
struct ENC_CU_DATA_ARRAY_CHROMA_SIZE
{
#define SIZE_SCU	(CUR_SIZE >> 2) * (CUR_SIZE >> 2)

	S8		ipm[2][SIZE_SCU];
	u32		map_scu[SIZE_SCU];
	int		num_nz_coef[N_C][SIZE_SCU];

	S16		coef_u[CUR_SIZE >> 1][CUR_SIZE >> 1];
	S16		coef_v[CUR_SIZE >> 1][CUR_SIZE >> 1];

	pel		reco_u[CUR_SIZE >> 1][CUR_SIZE >> 1];
	pel		reco_v[CUR_SIZE >> 1][CUR_SIZE >> 1];

	U12		x_chroma;
	U12		y_chroma;
	U12		x_last;
	U12		y_last;
	BOOL	last_child;

	void get_cu_data_from_curblk_c(ENC_CU_DATA_ARRAY_CHROMA_SIZE<CUR_SIZE / 2>* src, int x_cur_cu, int y_cur_cu, int cu_width_log2, int cu_height_log2, int log2_prev_cuw)
	{
		int x_cur_cu_scu = x_cur_cu >> 2;
		int y_cur_cu_scu = y_cur_cu >> 2;
		int cuh_scu = 1 << (cu_height_log2 - 2);
		int cuw_scu = 1 << (cu_width_log2 - 2);

#pragma HLS ARRAY_PARTITION variable=coef_u cyclic factor=CUR_SIZE>>1 dim=2
#pragma HLS ARRAY_PARTITION variable=coef_v cyclic factor=CUR_SIZE>>1 dim=2
#pragma HLS ARRAY_PARTITION variable=reco_u cyclic factor=CUR_SIZE>>1 dim=2
#pragma HLS ARRAY_PARTITION variable=reco_v cyclic factor=CUR_SIZE>>1 dim=2

		for (int i = 0; i < 16; i++)
		{
#pragma HLS LOOP_TRIPCOUNT max=16
#pragma HLS PIPELINE
			int idx_dst = ((y_cur_cu_scu + i) << (log2_prev_cuw - 2)) + x_cur_cu_scu;
			int idx_src = i << (cu_width_log2 - 2);
			for (int j = 0; j < 16; j++)
			{
#pragma HLS LOOP_TRIPCOUNT max=16
				if(j < cuw_scu && i < cuh_scu)
				{
					(ipm[1] + idx_dst)[j] = (src->ipm[1] + idx_src)[j];
					(map_scu + idx_dst)[j] = (src->map_scu + idx_src)[j];

					for (int k = 1; k < 3; k++)
					{
						(num_nz_coef[k] + idx_dst)[j] = (src->num_nz_coef[k] + idx_src)[j];
					}
				}
				else
				{
					break;
				}
			}
		}

		for (int i = 0; i < CUR_SIZE >> 2; i++)
		{
#pragma HLS LOOP_TRIPCOUNT max=32
#pragma HLS PIPELINE
			for (int j = 0; j < CUR_SIZE >> 2; j++)
			{
#pragma HLS LOOP_TRIPCOUNT max=32
				if (i < (1 << (cu_height_log2 - 1)) && j < (1 << (cu_width_log2 - 1)))
				{
					coef_u[(y_cur_cu >> 1) + i][(x_cur_cu >> 1) + j] = src->coef_u[i][j];
					coef_v[(y_cur_cu >> 1) + i][(x_cur_cu >> 1) + j] = src->coef_v[i][j];
					reco_u[(y_cur_cu >> 1) + i][(x_cur_cu >> 1) + j] = src->reco_u[i][j];
					reco_v[(y_cur_cu >> 1) + i][(x_cur_cu >> 1) + j] = src->reco_v[i][j];
				}
				else
				{
					break;
				}
			}
		}
	}
	void get_cu_data_from_curblk_c(ENC_CU_DATA_ARRAY_CHROMA_SIZE<CUR_SIZE>* src, int x_cur_cu, int y_cur_cu, int cu_width_log2, int cu_height_log2, int log2_prev_cuw)
	{
		int x_cur_cu_scu = x_cur_cu >> 2;
		int y_cur_cu_scu = y_cur_cu >> 2;
		int cuh_scu = 1 << (cu_height_log2 - 2);
		int cuw_scu = 1 << (cu_width_log2 - 2);

		for (int i = 0; i < 16; i++)
		{
#pragma HLS LOOP_TRIPCOUNT max=16
#pragma HLS PIPELINE
			int idx_dst = ((y_cur_cu_scu + i) << (log2_prev_cuw - 2)) + x_cur_cu_scu;
			int idx_src = i << (cu_width_log2 - 2);

			for (int j = 0; j < 16; j++)
			{
#pragma HLS LOOP_TRIPCOUNT max=16
				if(j < cuw_scu && i < cuh_scu)
				{
					(ipm[1] + idx_dst)[j] = (src->ipm[1] + idx_src)[j];
					(map_scu + idx_dst)[j] = (src->map_scu + idx_src)[j];

					for (int k = 1; k < 3; k++)
					{
						(num_nz_coef[k] + idx_dst)[j] = (src->num_nz_coef[k] + idx_src)[j];
					}
				}
				else
				{
					break;
				}
			}
		}

		for (int i = 0; i < CUR_SIZE>>2; i++)
		{
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT max=32
			for (int j = 0; j < CUR_SIZE>>2; j++)
			{
#pragma HLS LOOP_TRIPCOUNT max=32
				if(i < (1 << (cu_height_log2 - 1)) && j < (1 << (cu_width_log2 - 1)))
				{
					coef_u[(y_cur_cu >> 1) + i][(x_cur_cu >> 1) + j] = src->coef_u[i][j];
					coef_v[(y_cur_cu >> 1) + i][(x_cur_cu >> 1) + j] = src->coef_v[i][j];
					reco_u[(y_cur_cu >> 1) + i][(x_cur_cu >> 1) + j] = src->reco_u[i][j];
					reco_v[(y_cur_cu >> 1) + i][(x_cur_cu >> 1) + j] = src->reco_v[i][j];
				}
				else
				{
					break;
				}
			}
		}
	}
};

typedef struct _ENC_BEF_DATA
{
    U2    visit;
    U2    nosplit;
    U2    split;
    U2    split_visit;
	S64 split_cost[MAX_SPLIT_NUM];
	U8    mvr_idx_history;

	U8    affine_flag_history;

#if EXT_AMVR_HMVP
	U8    mvr_hmvp_idx_history;
#endif
} ENC_BEF_DATA;

typedef struct _EncALFVar
{
    AlfCorrData **m_alfCorr[N_C];
    AlfCorrData **m_alfNonSkippedCorr[N_C];
    AlfCorrData  *m_alfCorrMerged[N_C];

    double  *m_y_merged[NO_VAR_BINS];
    double **m_E_merged[NO_VAR_BINS];
    double   m_y_temp[ALF_MAX_NUM_COEF];
    double   m_pixAcc_merged[NO_VAR_BINS];

    double **m_E_temp;
    ALFParam  **m_alfPictureParam;

    int *m_coeffNoFilter[NO_VAR_BINS];

    int **m_filterCoeffSym;
    int m_varIndTab[NO_VAR_BINS];

    int *m_numSlicesDataInOneLCU;

    pel **m_varImg;
    BOOL **m_AlfLCUEnabled;
    unsigned int m_uiBitIncrement;

#if ALF_LOW_LANTENCY_ENC
    AlfCorrData ** **m_alfPrevCorr;           //[TemporalLayer] [YUV] [NumCUInFrame] ->AlfCorrData
    ALFParam ** *m_temporal_alfPictureParam;  //[TemporalLayer] [YUV] -> ALFParam
#endif
} EncALFVar;

/*****************************************************************************
 * CORE information used for encoding process.
 *
 * The variables in this structure are very often used in encoding process.
 *****************************************************************************/
typedef struct _ENC_CORE
{

    COM_MODE       mod_info_curr;

	ENC_CU_DATA cu_data_best_top;

    U11            lcu_num;

    int             qp_y;
    int             qp_u;
    int             qp_v;
    /* X address of current LCU */
    U6            x_lcu;
    /* Y address of current LCU */
	U6            y_lcu;
    /* X address of current CU in SCU unit */
    int            x_scu;
    /* Y address of current CU in SCU unit */
    int            y_scu;
    /* left pel position of current LCU */
    U12            x_pel;
    /* top pel position of current LCU */
	U12            y_pel;
    
    /* CU position in current LCU in SCU unit */
    int            cup;
    /* CU depth */
    int            cud;

    u8             skip_flag;
#if UMVE_ENH
	pel            predBuf[UMVE_MAX_REFINE_NUM_SEC_SET * UMVE_BASE_NUM + MAX_SKIP_NUM][MAX_CU_DIM];
	double         cu_candCost_list[UMVE_MAX_REFINE_NUM_SEC_SET * UMVE_BASE_NUM + MAX_SKIP_NUM];
	int            cu_cand_list[UMVE_MAX_REFINE_NUM_SEC_SET * UMVE_BASE_NUM + MAX_SKIP_NUM];
#endif
    ENC_SBAC     s_curr_best[MAX_CU_DEPTH][MAX_CU_DEPTH];
    ENC_SBAC     s_next_best[MAX_CU_DEPTH][MAX_CU_DEPTH];

    ENC_SBAC     s_curr_before_split[MAX_CU_DEPTH][MAX_CU_DEPTH];
    //ENC_BEF_DATA bef_data[MAX_CU_DEPTH][MAX_CU_DEPTH][MAX_CU_CNT_IN_LCU];

    COM_MOTION motion_cands[ALLOWED_HMVP_NUM];
    s8 cnt_hmvp_cands;

    //u8    skip_mvps_check;
	s16 ctmp[3][4096];

#if USE_SP
	typedef struct _ENC_PARENT_INFO
	{
		double          p_RDCost;
		double          c_sumRDCost;
		int             p_x;
		int             p_y;
		int             p_width_log2;
		int             p_height_log2;
	} ENC_PARENT_INFO;
#endif
#if USE_SP
	ENC_PARENT_INFO  cu_parent_info;
	COM_MOTION    parent_offset[SP_MAX_SPS_CANDS];
	u16           p_offset_num;
	COM_MOTION    brother_offset[SP_MAX_SPS_CANDS];
	u16           b_offset_num;
	s8            n_offset_num;
	COM_MOTION    n_recent_offset[SP_RECENT_CANDS];
	u8            sp_skip_non_scc;
	u8            n_pv_num;
	pel           n_recent_pv[3][MAX_SRB_PRED_SIZE];
	u8            n_pv_num_last;
	pel           n_recent_pv_last[3][MAX_SRB_PRED_SIZE];
	u8            n_recent_all_comp_flag[MAX_SRB_PRED_SIZE];
	u8            n_recent_allcomflag_last[MAX_SRB_PRED_SIZE];
	u8            n_recent_dpb_reYonly[MAX_SRB_PRED_SIZE];
	u8            n_recent_dpb_reYonly_last[MAX_SRB_PRED_SIZE];
	u8            n_recent_dpb_idx[MAX_SRB_PRED_SIZE];
	u8            n_recent_dpb_idx_last[MAX_SRB_PRED_SIZE];
	u8            n_recent_cuS_flag[MAX_SRB_PRED_SIZE];
	u8            n_recent_cuSflag_last[MAX_SRB_PRED_SIZE];
	s16           n_recent_pv_x[MAX_SRB_PRED_SIZE];
	s16           n_recent_pv_x_last[MAX_SRB_PRED_SIZE];
	s16           n_recent_pv_y[MAX_SRB_PRED_SIZE];
	s16           n_recent_pv_y_last[MAX_SRB_PRED_SIZE];
#endif
#if PMC || EPMC
	int            qp_v_pmc;
#endif
} ENC_CORE;

#if PCNL_FETCH

typedef struct _CTU_Info {
    //int            lcu_num;
    u8             qp_y;
    u8             qp_u;
    u8             qp_v;
    /* X address of current LCU */
    int            x_lcu;
    /* Y address of current LCU */
    int            y_lcu;
    /* X address of current CU in SCU unit */
    int            x_scu;
    /* Y address of current CU in SCU unit */
    int            y_scu;
    /* left pel position of current LCU */
    int            x_pel;
    /* top pel position of current LCU */
    int            y_pel;
    u8             skip_flag;

	u32             lambda_mv;
	s64             lambda[3];
	s64             sqrt_lambda[3];
	s64          dist_chroma_weight[2];

#if CTU_CONTROL_LUKE
	/* total count of remained LCU for encoding one picture. if a picture is
	encoded properly, this value should reach to zero */
	int  lcu_cnt;
	int    x_last_ctu;
	int    y_last_ctu;
#endif
	int  smb_num_width;
	int  smb_num_height;
	int ptr;

}CTU_Info;

typedef struct SAO_parameter
{
	ENC_SBAC     s_sao_init, s_sao_cur_blk, s_sao_next_blk;
	ENC_SBAC     s_sao_cur_type, s_sao_next_type;
	ENC_SBAC     s_sao_cur_mergetype, s_sao_next_mergetype;
} SAO_parameter;

typedef struct _MD_ENC_CORE_ARRARY
{

	int up_luma_pred_mode;
} MD_ENC_CORE_ARRARY;


struct _SKIP_RDO{
	S14 pmv_cands[MAX_SKIP_NUM + UMVE_MAX_REFINE_NUM * UMVE_BASE_NUM][REFP_NUM][MV_D];
	S2 refi_cands[MAX_SKIP_NUM + UMVE_MAX_REFINE_NUM * UMVE_BASE_NUM][REFP_NUM];
	S7 mode_list[MAX_INTER_SKIP_RDO];
	U4 num_rdo;
	//SKIP_ELEMENT mode[MAX_INTER_SKIP_RDO];
};

#if UMVE_ENH
typedef struct _UMVE_ENH_STRU {
	pel            predBuf[UMVE_MAX_REFINE_NUM_SEC_SET * UMVE_BASE_NUM + MAX_SKIP_NUM][MAX_CU_DIM];
	double         cu_candCost_list[UMVE_MAX_REFINE_NUM_SEC_SET * UMVE_BASE_NUM + MAX_SKIP_NUM];
	int            cu_cand_list[UMVE_MAX_REFINE_NUM_SEC_SET * UMVE_BASE_NUM + MAX_SKIP_NUM];
}UMVE_ENH_STRU;
#endif

typedef struct _SKIP_RDO SKIP_RDO;
struct _MERGE_RDO{
	S2 mrg_list_refi[AFF_MAX_NUM_MRG][REFP_NUM];
	CPMV mrg_list_cp_mv[AFF_MAX_NUM_MRG][REFP_NUM][VER_NUM][MV_D];
	U2 mrg_list_cp_num[AFF_MAX_NUM_MRG];
	U3 num_cands;
	U1 valid[AFF_MAX_NUM_MRG];
};
typedef struct _MERGE_RDO MERGE_RDO;

typedef struct _ENC_CORE_AFFINE_MERGE_INTERFACE{
    COM_MODE       mod_info_best;
    COM_MODE       mod_info_curr;
    int             qp_y;
    ENC_SBAC     s_curr_best;
    ENC_SBAC     s_next_best;
#if TR_SAVE_LOAD
    //u8           best_tb_part_hist;
#endif
    s64         cost_best;
} ENC_CORE_AFFINE_MERGE_INTERFACE;
typedef struct _ENC_CORE_SKIP_INTERFACE{
    COM_MODE       mod_info_best;
    COM_MODE       mod_info_curr;
    int             qp_y;
    int            cup;
    ENC_SBAC     s_curr_best;
    ENC_SBAC     s_next_best;
    ENC_BEF_DATA bef_data;
#if TR_SAVE_LOAD
    //u8           best_tb_part_hist;
#endif
#if TR_EARLY_TERMINATE
    //s64          dist_pred_luma;
#endif
    COM_MOTION motion_cands[ALLOWED_HMVP_NUM];
    s8             cnt_hmvp_cands;
    s64            cost_best;
} ENC_CORE_SKIP_INTERFACE;

typedef struct _ENC_CORE_AFF_RDO
{
	/* mode decision structure */
	COM_MODE       mod_info_best;
	COM_MODE       mod_info_curr;
	int             qp_y;
	/* CU position in current LCU in SCU unit */
	int            cup;
	/* CU depth */

	/* platform specific data, if needed */
	//void          *pf;
	/* bitstream structure for RDO */
	COM_BSW       bs_temp;
	/* SBAC structure for full RDO */
	ENC_SBAC     s_curr_best;
	ENC_SBAC     s_next_best;

	ENC_BEF_DATA bef_data;

	s64          cost_best;

} ENC_CORE_AFF_RDO;
#if CUDQP
typedef struct _COM_CU_QP_GROUP
{
	int num_delta_qp;
	int cu_qp_group_x;
	int cu_qp_group_y;
	int pred_qp;
	//encoder only
	int target_qp_for_qp_group;
	int qp_group_width;
	int qp_group_height;
} COM_CU_QP_GROUP;
#endif
/******************************************************************************
 * CONTEXT used for encoding process.
 *
 * All have to be stored are in this structure.
 *****************************************************************************/
typedef struct _ENC_CTX ENC_CTX;
struct _ENC_CTX
{
    COM_INFO              info;
    /* address of current input picture, ref_picture  buffer structure */
    ENC_PICO            *pico_buf[ENC_MAX_INBUF_CNT];
    /* address of current input picture buffer structure */
    ENC_PICO            *pico;
    /* index of current input picture buffer in pico_buf[] */
    u32                    pico_idx;
    int                    pico_max_cnt;
    /* magic code */
    u32                    magic;
    /* ENC identifier */
    int                  *id;
    /* address of core structure */
    ENC_CORE             *core;
    /* address indicating original picture and reconstructed picture */
    COM_PIC              *pic[PIC_BUF_NUM];
    /* reference picture (0: foward, 1: backward) */
    COM_REFP              refp[MAX_NUM_REF_PICS][REFP_NUM];
    /* encoding parameter */
    ENC_PARAM             param;
    /* bitstream structure */
    COM_BSW                bs;
    /* bitstream structure for RDO */
    COM_BSW                bs_temp;

    /* reference picture manager */
    COM_PM                 rpm;

    /* current encoding picture count(This is not PicNum or FrameNum.
    Just count of encoded picture correctly) */
    u32                    pic_cnt;
    /* current picture input count (only update when CTX0) */
    int                    pic_icnt;
    /* total input picture count (only used for bumping process) */
    u32                    pic_ticnt;
    /* remaining pictures is encoded to p or b slice (only used for bumping process) */
    u8                     force_slice;
    /* ignored pictures for force slice count (unavailable pictures cnt in gop,\
    only used for bumping process) */
    int                     force_ignored_cnt;
    /* initial frame return number(delayed input count) due to B picture or Forecast */
    int                    frm_rnum;
    /* current encoding slice number in one picture */
    int                    slice_num;
    /* first mb number of current encoding slice in one picture */
    int                    sl_first_mb;
    /* current slice type */
    u8                     slice_type;
    /* slice depth for current picture */
    u8                     slice_depth;
    /* whether current picture is referred or not */
    u8                     ref_depth;
    /* flag whether current picture is refecened picture or not */
    u8                     slice_ref_flag;
    /* current picture POC number */
    int                    poc;
    /* maximum CU depth */
    u8                     max_cud;
    ENC_SBAC               sbac_enc;
    /* address of inbufs */
    COM_IMGB              *inbuf[ENC_MAX_INBUF_CNT];
    /* last coded intra picture's presentation temporal reference */
    int                    last_intra_ptr;

    /* total count of remained LCU for encoding one picture. if a picture is
    encoded properly, this value should reach to zero */
    U11                    lcu_cnt;

    /* log2 of SCU count in a LCU row */
    u8                     log2_culine;
    /* log2 of SCU count in a LCU (== log2_culine * 2) */
    u8                     log2_cudim;
    /* intra prediction analysis */
    //ENC_PINTRA             pintra;
    /* inter prediction analysis */
    ENC_PINTER             pinter;
    inter_search_t         pi;
    /* picture buffer allocator */
    PICBUF_ALLOCATOR       pa;
    /* current picture's decoding temporal reference */
    int                    dtr;
    /* current picture's presentation temporal reference */
    int                    ptr;
    /*current picutre's layer id for hierachical structure */
    u8                     temporal_id;

    /* cu data for current LCU */
	//ENC_CU_DATA           *ctu_map_cu_data;
	ENC_CU_DATA           ctu_map_cu_data;
	ENC_CU_DATA           ctu_map_cu_data_luma;

    COM_MAP                map;
#if CHROMA_NOT_SPLIT
    u8                     tree_status;
#endif
#if MODE_CONS
    //u8                     cons_pred_mode;
#endif

    int **ppbEdgeFilter[LOOPFILTER_DIR_TYPE];

    COM_PIC               *pic_sao;
	COM_PIC               *pic_dbkbuffer;
    SAOStatData         ***saostatData; //[SMB][comp][types]
    SAOBlkParam          **saoBlkParams; //[SMB][comp]
    SAOBlkParam          **rec_saoBlkParams;//[SMB][comp]
#if DBR
	DBR_PARAM              dbr_pic_param[6];
	u8                     dbr_pic_poc[6];
#endif
    int                    pic_alf_on[N_C];
    EncALFVar             *Enc_ALF;
    COM_PIC               *pic_alf_Org;
    COM_PIC               *pic_alf_Rec;

	
    s64                    lambda[3];
    s64                    sqrt_lambda[3];
    s64                    dist_chroma_weight[2];
#if PATCH
    PATCH_INFO             *patch;
#endif
#if LIBVC_ON
    int                    is_RLpic_flag;
#endif
#if UMVE_ENH
	BOOL   dataCol;
	BOOL   needReset;
	s32    lastIPicPOC;
	u32    umveOffsetPicCount[UMVE_REFINE_STEP_SEC_SET];
	double umveAveOffset;
#endif
    u8 *wq[2];
#if FIXED_SPLIT
    int                    ctu_idx_in_sequence;
    int                    ctu_idx_in_picture;
    int                    ctu_idx_in_seq_I_slice_ctu;
    int                    ctu_idx_in_seq_B_slice_ctu;
    s8             split_combination[6];
#endif
    pel small_sq_dbk_y[MAX_NUM_LCU_ROW + 1][2][2];
    pel small_sq_dbk_u[MAX_NUM_LCU_ROW + 1][2][2];
    pel small_sq_dbk_v[MAX_NUM_LCU_ROW + 1][2][2];
    pel rctngl_ver_dbk_y[68][2];
    pel rctngl_ver_dbk_u[36][2];
    pel rctngl_ver_dbk_v[36][2];
    pel rctngl_hor_dbk_y[MAX_NUM_LCU_ROW][2][68];
    pel rctngl_hor_dbk_u[MAX_NUM_LCU_ROW][2][36];
    pel rctngl_hor_dbk_v[MAX_NUM_LCU_ROW][2][36];

    int bin_count;
    enc_rc_t rc;
    COM_IMGB              *refimgb;
	int cons_pred_mode;

#if CUDQP
	int    cu_delta_qp_lcu_map[256];
	COM_CU_QP_GROUP cu_qp_group;
#endif
    vcencRateControl_s lowdelay_rc;

#if ENABLE_BFRAME
	int code_stru;
#endif

#if USE_ROI_QP_CONFIG  || USE_ROI_MODE_CONFIG
	FILE *lcumap_in;
#endif
#if CUDQP_QP_MAP
	unsigned int* mapcuqp;
#endif
};

struct ENC_CTX_HLS
{
    COM_INFO_HLS           info;

    int                    refp_ptr[MAX_NUM_REF_PICS][REFP_NUM];

	int                    num_refp[REFP_NUM];
	/* current slice type */
	u8                     slice_type;
	/* log2 of SCU count in a LCU row */
	u8                     log2_culine;
	/* current picture's presentation temporal reference */
	int                    ptr;
	/*current picutre's layer id for hierachical structure */
	u8                     temporal_id;
	/* cu data for current LCU */
	//ENC_CU_DATA           *ctu_map_cu_data;
	ENC_CU_DATA            ctu_map_cu_data;
	//ENC_CU_DATA            ctu_map_cu_data_luma;
#if CHROMA_NOT_SPLIT
	u8                     tree_status;
#endif
#if UMVE_ENH
	BOOL                   dataCol;
	u32                    umveOffsetPicCount[UMVE_REFINE_STEP_SEC_SET];
#endif
	int                    cons_pred_mode;
#if CUDQP
	int                    cu_delta_qp_lcu_map[256];
	COM_CU_QP_GROUP        cu_qp_group;
#endif
	int fixed_slice_qp_flag;
	int delta_qp;
	int lcu_cnt;
};

typedef struct _DBK_INPUT{
	pel best_rec_y[64][64];
	pel best_rec_u[32][32];
	pel best_rec_v[32][32];
} DBK_INPUT;

typedef struct _ENC_CTX_SKIP_INTERFACE{
    COM_INFO              info;
    ENC_CORE_SKIP_INTERFACE* core;
    COM_PIC              *pic[PIC_BUF_NUM];
    COM_REFP              refp[MAX_NUM_REF_PICS][REFP_NUM];
    ENC_PINTER             pinter;
    int                    ptr;
    COM_MAP                map;
    u8                     tree_status;
#if MODE_CONS
    u8                     cons_pred_mode;
#endif

    int **ppbEdgeFilter[LOOPFILTER_DIR_TYPE];
    s64                    lambda[3];
    s64                    sqrt_lambda[3];
    s64                    dist_chroma_weight[2];
    u8 *wq[2];
    COM_PM                 rpm;
} ENC_CTX_SKIP_INTERFACE;

typedef struct _ENC_CTX_AFF_RDO ENC_CTX_AFF_RDO;
struct _ENC_CTX_AFF_RDO
{
	COM_INFO              info;

	ENC_CORE_AFF_RDO             *core;

	COM_REFP              refp[MAX_NUM_REF_PICS][REFP_NUM];

	/* reference picture manager */
	COM_PM                 rpm;

	/* inter prediction analysis */
	ENC_PINTER             pinter;

	int                    ptr;

	COM_MAP                map;
#if CHROMA_NOT_SPLIT
	u8                     tree_status;
#endif
#if MODE_CONS
	u8                     cons_pred_mode;
#endif

	int **ppbEdgeFilter[LOOPFILTER_DIR_TYPE];

	s64                    lambda[3];

	s64                    dist_chroma_weight[2];

};


/*****************************************************************************
 * API for encoder only
 *****************************************************************************/
struct _RDOQ_MODEL {
    u32 cbf[3];
    u32 run_rdoq[24];
    u32 level[24];
    u32 last1[12];
    u32 last2[22];
};
struct _RDOQ_ARRAY {
	s32 rdoq_est_run[24][2];
	int rdoq_est_cbf[3][2];
	s32 rdoq_est_level[(6 * 4)][2];
	s32 rdoq_est_last[2][6][12][2];
};
void enc_init_err_scale(int bit_depth);
typedef struct _RDOQ_MODEL RDOQ_MODEL;
typedef struct _RDOQ_ARRAY RDOQ_ARRAY;
ENC enc_create(ENC_PARAM * param, int * err);
void enc_delete(ENC id);
int start_encode_one_frame(
#if PATCH_DIVIDE_ENABLE
	int multislice_mode,
#endif
	ENC id, COM_BITB * bitb, ENC_STAT * stat, double ctu_64x64_ime_cost[2040]);
int enc_encode(ENC id, COM_BITB* bitb, ENC_STAT* stat);
int enc_pic_prepare(ENC_CTX * ctx, COM_BITB * bitb);
int enc_pic_finish(ENC_CTX * ctx, COM_BITB * bitb, ENC_STAT * stat);

int enc_pic(
#if PATCH_DIVIDE_ENABLE
	int multislice_mode,
	#endif
	ENC_CTX * ctx, COM_BITB * bitb, ENC_STAT * stat,
	double ctu_64x64_ime_cost[2040]);

int enc_deblock_avs2(ENC_CTX * ctx, COM_PIC * pic/*, int width_luma, int height_luma, int ppb_height, int ppb_width, int f_scu, int map_split, int pic_uv, int pic_y*/);
int enc_push_frm(ENC_CTX * ctx, COM_IMGB * img);
int enc_ready(ENC_CTX * ctx, CTU_Info* core);
void enc_flush(ENC_CTX * ctx);
int enc_picbuf_get_inbuf_422(ENC_CTX* ctx, COM_IMGB** img);
int enc_picbuf_get_inbuf(ENC_CTX * ctx, COM_IMGB ** img);
#if REPEAT_SEQ_HEADER
int init_seq_header(ENC_CTX * ctx, COM_BITB * bitb);
#else
int enc_seq_header(ENC_CTX * ctx, COM_BITB * bitb, ENC_STAT * stat);
#endif
#if PATCH
void en_copy_lcu_scu(u32 * scu_temp, u32 * scu_best, s8(*refi_temp)[REFP_NUM], s8(*refi_best)[REFP_NUM], s16(*mv_temp)[REFP_NUM][MV_D], s16(*mv_best)[REFP_NUM][MV_D], u32 *cu_mode_temp, u32 *cu_mode_best, PATCH_INFO * patch, int pic_width, int pic_height);
#endif
#include "enc_util.h"
#include "enc_eco.h"
//#include "enc_mode.h"
//#include "enc_tq.h"
//#include "enc_pintra.h"
//#include "enc_pinter.h"
//#include "rmd.h"

////////////////////rmd.h////////////////////
#ifndef _RMD_H_
#define _RMD_H_


//#include "type_def.h"
//#include "com_def.h"
#define RMD_CU_SIZE_NUM 1293

struct RMD_OUTPUT
{
#if USE_SPEED_LEVEL
	u8 modes_8x8[64][IPD_RDO_CNT + EXTRA_RMD_MODE];
	u8 modes_16x8[32][IPD_RDO_CNT + EXTRA_RMD_MODE];
	u8 modes_32x8[16][IPD_RDO_CNT + EXTRA_RMD_MODE];
	u8 modes_64x8[8][IPD_RDO_CNT + EXTRA_RMD_MODE];
	u8 modes_8x16[32][IPD_RDO_CNT + EXTRA_RMD_MODE];
	u8 modes_16x16[16][IPD_RDO_CNT + EXTRA_RMD_MODE];
	u8 modes_32x16[8][IPD_RDO_CNT + EXTRA_RMD_MODE];
	u8 modes_64x16[4][IPD_RDO_CNT + EXTRA_RMD_MODE];
	u8 modes_8x32[16][IPD_RDO_CNT+ EXTRA_RMD_MODE];
	u8 modes_16x32[8][IPD_RDO_CNT+ EXTRA_RMD_MODE];
	u8 modes_32x32[4][IPD_RDO_CNT+ EXTRA_RMD_MODE];
	u8 modes_64x32[2][IPD_RDO_CNT+ EXTRA_RMD_MODE];
	u8 modes_8x64[8][IPD_RDO_CNT + EXTRA_RMD_MODE];
	u8 modes_16x64[4][IPD_RDO_CNT+ EXTRA_RMD_MODE];
	u8 modes_32x64[2][IPD_RDO_CNT+ EXTRA_RMD_MODE];
	u8 modes_64x64[1][IPD_RDO_CNT+ EXTRA_RMD_MODE];
#else
	u8 modes_8x8[64][IPD_RDO_CNT];
	u8 modes_16x8[32][IPD_RDO_CNT];
	u8 modes_32x8[16][IPD_RDO_CNT];
	u8 modes_64x8[8][IPD_RDO_CNT];
	u8 modes_8x16[32][IPD_RDO_CNT];
	u8 modes_16x16[16][IPD_RDO_CNT];
	u8 modes_32x16[8][IPD_RDO_CNT];
	u8 modes_64x16[4][IPD_RDO_CNT];
	u8 modes_8x32[16][IPD_RDO_CNT];
	u8 modes_16x32[8][IPD_RDO_CNT];
	u8 modes_32x32[4][IPD_RDO_CNT];
	u8 modes_64x32[2][IPD_RDO_CNT];
	u8 modes_8x64[8][IPD_RDO_CNT];
	u8 modes_16x64[4][IPD_RDO_CNT];
	u8 modes_32x64[2][IPD_RDO_CNT];
	u8 modes_64x64[1][IPD_RDO_CNT];
#endif

	friend void copy(int w, int h, RMD_OUTPUT& _Left, const RMD_OUTPUT& _Right)
	{
#define COPY_RMD(W, H)																\
		if (w == W && h == H)														\
		{																			\
			for (int i = 0; i < (64 / W) * (64 / H); ++i)							\
			{																		\
				for (int j = 0; j < IPD_RDO_CNT; ++j)								\
				{																	\
					_Left.modes_##W##x##H [i][j] = _Right.modes_##W##x##H [i][j];	\
				}																	\
			}																		\
		}																			\
		else

		COPY_RMD(8, 8)
		COPY_RMD(16, 8)
		COPY_RMD(32, 8)
		COPY_RMD(64, 8)
		COPY_RMD(8, 16)
		COPY_RMD(16, 16)
		COPY_RMD(32, 16)
		COPY_RMD(64, 16)
		COPY_RMD(8, 32)
		COPY_RMD(16, 32)
		COPY_RMD(32, 32)
		COPY_RMD(64, 32)
		COPY_RMD(8, 64)
		COPY_RMD(16, 64)
		COPY_RMD(32, 64)
		COPY_RMD(64, 64)
		{
			assert(false);
		}

#undef COPY_RMD
	}

	friend void copy(RMD_OUTPUT& _Left, const RMD_OUTPUT& _Right)
	{
#define COPY_RMD(W, H)																\
		{																			\
			for (int i = 0; i < (64 / W) * (64 / H); ++i)							\
			{																		\
				for (int j = 0; j < IPD_RDO_CNT; ++j)								\
				{																	\
					_Left.modes_##W##x##H [i][j] = _Right.modes_##W##x##H [i][j];	\
				}																	\
			}																		\
		}

		COPY_RMD(8, 8)
		COPY_RMD(16, 8)
		COPY_RMD(32, 8)
		COPY_RMD(64, 8)
		COPY_RMD(8, 16)
		COPY_RMD(16, 16)
		COPY_RMD(32, 16)
		COPY_RMD(64, 16)
		COPY_RMD(8, 32)
		COPY_RMD(16, 32)
		COPY_RMD(32, 32)
		COPY_RMD(64, 32)
		COPY_RMD(8, 64)
		COPY_RMD(16, 64)
		COPY_RMD(32, 64)
		COPY_RMD(64, 64)

#undef COPY_RMD
	}
};


typedef struct {
    pel up[max_buffer_width];
    pel left[MAX_CU_SIZE2];
    pel top_left;
}RMD_BUF;

#if USE_SPEED_LEVEL
void rmd_run(U7 real_lcu_width, U7 real_lcu_height, U12 *pix_x, U12 *pix_y, u32 lambda, pel Orig_Lcu_ptr_p_fenc_LCU_Y_rmd[MAX_CU_SIZE2 * MAX_CU_SIZE2], RMD_OUTPUT* rmd_output, U8 speed_level);
#else
//void rmd_run(U7 real_lcu_width, U7 real_lcu_height, U12 *pix_x, U12 *pix_y, u32 lambda, pel Orig_Lcu_ptr_p_fenc_LCU_Y_rmd[MAX_CU_SIZE2 * MAX_CU_SIZE2], RMD_OUTPUT* rmd_output);
void rmd_run(U7 real_lcu_width, U7 real_lcu_height, U12 * fetch_output_rmd_pix_x, U12 * fetch_output_rmd_pix_y, u32 fetch_output_rmd_lambda_mv, 
             pel Fetch_Orig_Lcu_p_fenc_LCU_Y_rmd[MAX_CU_SIZE2 * MAX_CU_SIZE2], RMD_OUTPUT * rmd_output);
#endif



#endif /* _RMD_H_ */
///////////////////////////rmd.h//////////////////////////////////////////



#include "enc_tbl.h"
//#include "iime.h"
//#include "kernel_enc.h"
//#include "enc_rate_est.h"
#define CU_SIZE_NUM64 1

#define CU_SIZE_NUM_32 12
#define CU_SIZE_NUM_16 80
#define CU_SIZE_NUM_8 400
#define CU_SIZE_NUM_4 1056

#define CU_SIZE_NUM32 CU_SIZE_NUM_32
#define CU_SIZE_NUM16 CU_SIZE_NUM_16
#define CU_SIZE_NUM8 CU_SIZE_NUM_8
#define CU_SIZE_NUM4 CU_SIZE_NUM_4

typedef struct _ENC_ME_INFO_ALL
{
	s16 mv[MV_D];
	u8 refi;
	u32 cost;
	u8 visited;
	s16 sq_parent;
	u16 x0_scu; //10
	u16 y0_scu; //10
	u8 otherSizeLog2Minus; //3
	u8 sideFlag;//1
}ENC_ME_INFO_ALL;

#define CU_SIZE_NUM 1600
typedef struct _ENC_ME_CU
{
	s16 info_addr_map[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN][MAX_CU_DEPTH][MAX_CU_DEPTH];
	s16 info_addr_map_split[MAX_CU_DEPTH*MAX_CU_DEPTH][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN];
	double cost_error_avg;
	double sad_error_avg;
	double mv_error[MV_D];
	int worse_cnt;
	int sad_worse_cnt;
	u32 me_cnt;
	u8 has_partitioned;
	int info_idx;
	int refp_num;
	int pic_width_in_scu;
	int pic_height_in_scu;
	int info_idx32;
	int info_idx16;
	int info_idx8;
	int info_idx4;
	ENC_ME_INFO_ALL me_info[CU_SIZE_NUM];
	ENC_ME_INFO_ALL me_info32[CU_SIZE_NUM_32];
	ENC_ME_INFO_ALL me_info16[CU_SIZE_NUM_16];
	ENC_ME_INFO_ALL me_info8[CU_SIZE_NUM_8];
	ENC_ME_INFO_ALL me_info4[CU_SIZE_NUM_4];
	s16 swSubLv0TopLeft[MV_D];
}ENC_ME_CU;

#define MAC_8TAP(filter, buf, offset)	((filter)[1] * (buf)[(offset) + 1] +\
													 (filter)[2] * (buf)[(offset) + 2] +\
													 (filter)[3] * (buf)[(offset) + 3] +\
													 (filter)[4] * (buf)[(offset) + 4] +\
													 (filter)[5] * (buf)[(offset) + 5] +\
													 (filter)[6] * (buf)[(offset) + 6]+ \
                                                     (filter)[0] * ((buf)[(offset)] + (buf)[(offset) + 7]))

#if ENABLE_BFRAME
typedef struct _MV_DIR
{
	U1 lidx;
	S8 cur_refi;
} MV_DIR;
#define STAT_DIR 0
typedef struct _ME_MV_DIR_BW
{
	MV_DIR block64x64MvDir[1][1];
	MV_DIR block32x32MvDir[2][2];
	MV_DIR block64x32MvDir[1][2];
	MV_DIR block32x64MvDir[2][1];
	MV_DIR block16x16MvDir[4][4];
	MV_DIR block32x16MvDir[2][4];
	MV_DIR block16x32MvDir[4][2];
	MV_DIR block64x16MvDir[1][4];
	MV_DIR block16x64MvDir[4][1];
	MV_DIR block8x8MvDir  [8][8];
	MV_DIR block16x8MvDir [4][8];
	MV_DIR block8x16MvDir [8][4];
	MV_DIR block32x8MvDir [2][8];
	MV_DIR block8x32MvDir [8][2];
	MV_DIR block64x8MvDir [1][8];
	MV_DIR block8x64MvDir [8][1];
	MV_DIR block32x4MvDir [2][16];
	MV_DIR block4x32MvDir [16][2];
	MV_DIR block16x4MvDir [4][16];
	MV_DIR block4x16MvDir [16][4];
	MV_DIR block8x4MvDir  [8][16];
	MV_DIR block4x8MvDir  [16][8];

	friend void copy(int w, int h, _ME_MV_DIR_BW& _Left, const _ME_MV_DIR_BW& _Right)
	{
#define COPY_ME(W, H)																			\
		if (w == W && h == H)																	\
		{																						\
			for (int i = 0; i < (64 / W) ; ++i)													\
			{																					\
				for (int j = 0; j < (64 / H); ++j)												\
				{																				\
					_Left.block##W##x##H##MvDir [i][j] = _Right.block##W##x##H##MvDir [i][j];	\
				}																				\
			}																					\
		}																						\
		else

		COPY_ME(64, 64)
		COPY_ME(32, 32)
		COPY_ME(64, 32)
		COPY_ME(32, 64)
		COPY_ME(16, 16)
		COPY_ME(32, 16)
		COPY_ME(16, 32)
		COPY_ME(64, 16)
		COPY_ME(16, 64)
		COPY_ME(8, 8)
		COPY_ME(16, 8)
		COPY_ME(8, 16)
		COPY_ME(32, 8)
		COPY_ME(8, 32)
		COPY_ME(64, 8)
		COPY_ME(8, 64)
		COPY_ME(32, 4)
		COPY_ME(4, 32)
		COPY_ME(16, 4)
		COPY_ME(4, 16)
		COPY_ME(8, 4)
		COPY_ME(4, 8)
		{
			assert(false);
		}

#undef COPY_ME
	}

	friend void copy(_ME_MV_DIR_BW& _Left, const _ME_MV_DIR_BW& _Right)
	{
#define COPY_ME(W, H)																			\
		{																						\
			for (int i = 0; i < (64 / W) ; ++i)													\
			{																					\
				for (int j = 0; j < (64 / H); ++j)												\
				{																				\
					_Left.block##W##x##H##MvDir [i][j] = _Right.block##W##x##H##MvDir [i][j];	\
				}																				\
			}																					\
		}

		COPY_ME(64, 64)
		COPY_ME(32, 32)
		COPY_ME(64, 32)
		COPY_ME(32, 64)
		COPY_ME(16, 16)
		COPY_ME(32, 16)
		COPY_ME(16, 32)
		COPY_ME(64, 16)
		COPY_ME(16, 64)
		COPY_ME(8, 8)
		COPY_ME(16, 8)
		COPY_ME(8, 16)
		COPY_ME(32, 8)
		COPY_ME(8, 32)
		COPY_ME(64, 8)
		COPY_ME(8, 64)
		COPY_ME(32, 4)
		COPY_ME(4, 32)
		COPY_ME(16, 4)
		COPY_ME(4, 16)
		COPY_ME(8, 4)
		COPY_ME(4, 8)

#undef COPY_ME
}

#if STAT_DIR
	float ref_ratio[REFP_NUM];
	float total;
	template<unsigned H, unsigned V>
	void statDir(MV_DIR dirs[H][V])
	{
		for (int i = 0; i < H; i++)
		{
			for (int j = 0; j < V; j++)
			{
				if (dirs[i][j].lidx < 0 || dirs[i][j].cur_refi < 0)
					continue;
				total += 1;
				if (dirs[i][j].lidx == 0 && dirs[i][j].cur_refi == 0)
					ref_ratio[0] += 1;
				else
				{
					ref_ratio[1] += 1;
				}
			}
		}
	}
	void statDir()
	{
		ref_ratio[0] = ref_ratio[1] = total = 0;
		statDir<  1,1 >  (block64x64MvDir);
		statDir < 2,2 >  (block32x32MvDir);
		statDir < 1,2 >  (block64x32MvDir);
		statDir < 2,1 >  (block32x64MvDir);
		statDir < 4,4 >  (block16x16MvDir);
		statDir < 2,4 >  (block32x16MvDir);
		statDir < 4,2 >  (block16x32MvDir);
		statDir < 1,4 >  (block64x16MvDir);
		statDir < 4,1 >  (block16x64MvDir);
		statDir < 8,8 >  (block8x8MvDir);
		statDir < 4,8 >  (block16x8MvDir);
		statDir < 8,4 >  (block8x16MvDir);
		statDir < 2,8 >  (block32x8MvDir);
		statDir < 8,2 >  (block8x32MvDir);
		statDir < 1,8 >  (block64x8MvDir);
		statDir < 8,1 >  (block8x64MvDir);
		statDir < 2,16 > (block32x4MvDir);
		statDir < 16,2 > (block4x32MvDir);
		statDir < 4,16 > (block16x4MvDir);
		statDir < 16,4 > (block4x16MvDir);
		statDir < 8,16 > (block8x4MvDir);
		statDir < 16,8 > (block4x8MvDir	);
		ref_ratio[0] /= total; 
		ref_ratio[1] /= total;
		printf("LIST_0: %.2f%%,LIST_1: %.2f%%\n", ref_ratio[0]*100, ref_ratio[1]*100);
	}
#endif
}ME_MV_DIR;

typedef struct _ME_MV_DIR_BW_16
{
	MV_DIR block16x16MvDir[4][4];
	MV_DIR block16x8MvDir[4][8];
	MV_DIR block8x16MvDir[8][4];
}ME_MV_DIR_16;
#endif


typedef struct _ENC_FME_INFO_ALL
{
	S14 sadTree64x64Fmv[1][1][2];
	S14 sadTree32x32Fmv[2][2][2];
	S14 sadTree64x32Fmv[1][2][2];
	S14 sadTree32x64Fmv[2][1][2];
	S14 sadTree16x16Fmv[4][4][2];
	S14 sadTree32x16Fmv[2][4][2];
	S14 sadTree16x32Fmv[4][2][2];
	S14 sadTree64x16Fmv[1][4][2];
	S14 sadTree16x64Fmv[4][1][2];
	S14 sadTree8x8Fmv[8][8][2];
	S14 sadTree16x8Fmv[4][8][2];
	S14 sadTree8x16Fmv[8][4][2];
	S14 sadTree32x8Fmv[2][8][2];
	S14 sadTree8x32Fmv[8][2][2];
	S14 sadTree64x8Fmv[1][8][2];
	S14 sadTree8x64Fmv[8][1][2];
	S14 sadTree4x8Fmv[16][8][2];
	S14 sadTree8x4Fmv[8][16][2];
	S14 sadTree16x4Fmv[4][16][2];
	S14 sadTree4x16Fmv[16][4][2];
	S14 sadTree32x4Fmv[2][16][2];
	S14 sadTree4x32Fmv[16][2][2];

	friend void copy(int w, int h, _ENC_FME_INFO_ALL& _Left, const _ENC_FME_INFO_ALL& _Right)
	{
#define COPY_FME(W, H)																					\
		if (w == W && h == H)																			\
		{																								\
			for (int i = 0; i < (64 / W) ; ++i)															\
			{																							\
				for (int j = 0; j < (64 / H); ++j)														\
				{																						\
					for(int k = 0; k < 2; ++k)															\
					{																					\
						_Left.sadTree##W##x##H##Fmv[i][j][k] = _Right.sadTree##W##x##H##Fmv[i][j][k];	\
					}																					\
				}																						\
			}																							\
		}																								\
		else

		COPY_FME(64, 64)
		COPY_FME(32, 32)
		COPY_FME(64, 32)
		COPY_FME(32, 64)
		COPY_FME(16, 16)
		COPY_FME(32, 16)
		COPY_FME(16, 32)
		COPY_FME(64, 16)
		COPY_FME(16, 64)
		COPY_FME(8, 8)
		COPY_FME(16, 8)
		COPY_FME(8, 16)
		COPY_FME(32, 8)
		COPY_FME(8, 32)
		COPY_FME(64, 8)
		COPY_FME(8, 64)
		COPY_FME(4, 8)
		COPY_FME(8, 4)
		COPY_FME(16, 4)
		COPY_FME(4, 16)
		COPY_FME(32, 4)
		COPY_FME(4, 32)
		{
			assert(false);
		}

#undef COPY_FME
	}

	friend void copy(_ENC_FME_INFO_ALL& _Left, const _ENC_FME_INFO_ALL& _Right)
	{
#define COPY_FME(W, H)																					\
		{																								\
			for (int i = 0; i < (64 / W) ; ++i)															\
			{																							\
				for (int j = 0; j < (64 / H); ++j)														\
				{																						\
					for(int k = 0; k < 2; ++k)															\
					{																					\
						_Left.sadTree##W##x##H##Fmv[i][j][k] = _Right.sadTree##W##x##H##Fmv[i][j][k];	\
					}																					\
				}																						\
			}																							\
		}

		COPY_FME(64, 64)
		COPY_FME(32, 32)
		COPY_FME(64, 32)
		COPY_FME(32, 64)
		COPY_FME(16, 16)
		COPY_FME(32, 16)
		COPY_FME(16, 32)
		COPY_FME(64, 16)
		COPY_FME(16, 64)
		COPY_FME(8, 8)
		COPY_FME(16, 8)
		COPY_FME(8, 16)
		COPY_FME(32, 8)
		COPY_FME(8, 32)
		COPY_FME(64, 8)
		COPY_FME(8, 64)
		COPY_FME(4, 8)
		COPY_FME(8, 4)
		COPY_FME(16, 4)
		COPY_FME(4, 16)
		COPY_FME(32, 4)
		COPY_FME(4, 32)

#undef COPY_FME
	}
}ENC_FME_INFO_ALL;

typedef struct _ENC_FME_INFO_ALL_16
{
	S14 sadTree16x16Fmv[4][4][2];
	S14 sadTree16x8Fmv[4][8][2];
	S14 sadTree8x16Fmv[8][4][2];
}ENC_FME_INFO_ALL_16;


#define CHROMA_INDE     1
#define AEC_UNRECUR		1

#define ENC_ECO_XU 1

#define OPEN_ALL_KERNEL 0
#define KERNEL_MAXSIZE 1

#if     OPEN_ALL_KERNEL
#define OPEN_ALL_KERNEL_PART 1

#define OPEN_ENC_ECO_UNIT 1
#define OPEN_ANALYZE_INTRA_CU 1
#define OPEN_SAODBK 1
#define OPEN_SAODBK_AVS2 1
#define OPEN_ENC_ECO_UNIT_CHROMA 1
#define OPEN_PINTER_ME_EPZS 1
#define OPEN_ANALYZE_AFFINE_MERGE 1
#define OPEN_ANALYZE_DIRECT_SKIP 1
#define OPEN_AFF_ME 1
#define OPEN_AFF_RDO 1

#else
#define OPEN_ENC_ECO_UNIT 0
#define OPEN_ANALYZE_INTRA_CU 0
#define OPEN_SAODBK 0
#define OPEN_SAODBK_AVS2 0
#define OPEN_ENC_ECO_UNIT_CHROMA 0
#define OPEN_PINTER_ME_EPZS 0
#define OPEN_ANALYZE_AFFINE_MERGE 0
#define OPEN_ANALYZE_DIRECT_SKIP 0
#define OPEN_AFF_ME 0
#define OPEN_AFF_RDO 0

#endif

#if 0 //unsed now
int enc_eco_unit_count;
int analyze_direct_skip_count;
int analyze_affine_merge_count;
int enc_SAO_rdo_count;
int enc_deblock_sao_avs2_count;
int enc_eco_unit_chroma_count;
int pinter_me_epzs_count;

int analyze_intra_cu_cnt;


int aff_me_count;
int aff_rdo_count;
#endif 
#define ALL_AMVR_IME 1
#define USE_MMEA 1
#define ALL_AMVR_MVP 1
#define USE_AFFINE_FETCH 1

#define ME_PADDING_AREA 1

#if ALL_AMVR_IME
#define DIR_NUMS 2
#define DIR_HOR 0
#define DIR_VER 1
#define DIR_NUMS 2
#define MMEA_LEVEL 3
#define MMEA_LEVEL0_SR 8
#define MMEA_LEVEL1_SR 28
//#define MMEA_LEVEL2_SR 128
#define LEVEL0 0
#define LEVEL1 1
#define LEVEL2 2
#define BOUND_NUMS 4
#define LEFT 3
#define RIGHT 2
#define UP 1
#define DOWN 0
#define MMEA_ORG_SR 72
#define MMEA_DSW 0
#define MMEA_AVG_POOL 0
#define MAX_NUM_INT_MVR 3
#endif

#define ORG_CHANGE 1   
#if ALL_AMVR_IME
typedef struct _ENC_SW
{
	s16 ver_range;
	s16 hor_range;
	s8 ref_idx;
	int lidx;
	s16 search_lenth[MMEA_LEVEL][DIR_NUMS];
	s16 center[MMEA_LEVEL][MV_D];
	s16 range[MMEA_LEVEL][MV_RANGE_DIM][MV_D];
	s16 ctr_point[MMEA_LEVEL][MV_RANGE_DIM][MV_D];
	s16 align_size[MMEA_LEVEL][BOUND_NUMS];
	int pool_type;
	BOOL consist_lv0;
	int buffer_stride[MMEA_LEVEL];
	BOOL fetch_from_pic_y;
	//pel* pic_y_lv0_ptr;
	//pel* pic_y_lv1_ptr;
	//pel* pic_y_lv2_ptr;
	s16  max_mv_offset[MV_D];

}ENC_SW;
typedef struct _ENC_MMEA_INFO
{
	//ENC_SW* sw;
	//int x;
	//int y;
	//int w;
	//int h;
	int stride_org[MMEA_LEVEL];
	int if_search[MMEA_LEVEL];
	int mvr_nums;
	s16 best_mv[MAX_NUM_INT_MVR][MV_D];
	u32 best_cost[MAX_NUM_INT_MVR];
	u8 num_refp;
	u32 lambda_mv;
	int bit_depth;
	BOOL isEearlyTer;
	int org_offset_lv[MMEA_LEVEL];
	//pel *org_lv0_ptr;
	//pel *org_lv1_ptr;
	//pel *org_lv2_ptr;
}ENC_MMEA_INFO;

#endif


typedef struct _ENC_AFFME_IN
{

	/* global info */
	U13 pic_height_luma;
	U13 pic_width_luma;
	U12 pic_height_chroma;
	U12 pic_width_chroma;
	int stride_pic_y;
	int stride_pic_uv;
	int u2v_x_offset;
	U13 stride_org_y;
	U12 stride_org_uv;

	U2 lidx;
	U4 bit_depth;
	U5 num_refp;
	U24 org_offset[N_C];

	/* Frame dependencies removal. */
	/* Consider array output problem. */
	u32 lambda_mv;
	//s32 *mot_bits;
	u8  curr_mvr;
	//COM_REFP refp[MAX_NUM_REF_PICS][REFP_NUM];
	//int (*best_mv_uni)[REFP_NUM][MAX_NUM_ACTIVE_REF_FRAME][MV_D];
	//COM_INFO *info;
	//u32* map_scu;
	int refp_ptr[MAX_NUM_REF_PICS][REFP_NUM];
	U9 ptr;

	/* buffers */
	pel  p_error[MAX_CU_DIM];
	int  i_gradient[2][MAX_CU_DIM];
	pel  pred_buf[MAX_CU_DIM];
	s32 mvf_range[MV_RANGE_DIM][MV_D];
	CPMV ac_mv_scale[VER_NUM][MV_D];
	s32 cp_mv_four[VER_NUM][MV_D];
	CPMV temp_affine_mvp[VER_NUM][MV_D];
	CPMV temp_trans_mv[VER_NUM][MV_D];
	s16 mv_cliped[REFP_NUM][MV_D];
	u32 cost_trans[REFP_NUM][MAX_NUM_ACTIVE_REF_FRAME];
	CPMV  affine_mvp_scale[REFP_NUM][MAX_NUM_ACTIVE_REF_FRAME][VER_NUM][MV_D];
	CPMV  affine_mv_scale[REFP_NUM][MAX_NUM_ACTIVE_REF_FRAME][VER_NUM][MV_D];
}ENC_AFFME_IN;

typedef struct _ENC_AFFME_OUT
{
	s8 * refi_temp;
}ENC_AFFME_OUT;

//typedef struct _MD_INPUT{
//	U13 pix_x;
//	U13 pix_y;
//	u32             lambda_mv;
//	s64          lambda[3];
//	s64             lambda_y;
//	s64             lambda_u;
//	s64             lambda_v;
//	U6             qp_y;
//	U6             qp_u;
//	U6             qp_v;
//	S64				dist_chroma_weight[2];
//	U16				x_last_ctu;
//	U16				y_last_ctu;
//	S14 CtrPos2MD[RANGE_NUM][DIR_DIM][MAP_DIMS];
//#if ENABLE_BFRAME
//	S14 CtrPos2MD_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS];
//#endif // ENABLE_BFRAME
////**************for md chroma****************************
//	U8  split_mode[MAX_CU_DEPTH][NUM_BLOCK_SHAPE][MAX_CU_CNT_IN_LCU];
//	U1 do_chroma;
//	U3  pred_mode[CU_SIZE];
//	S8  ipm[2][CU_SIZE];
//	U8  affine_flag[CU_SIZE];
//	S14 mv[MAX_CU_CNT_IN_LCU][REFP_NUM][MV_D];
//	S3  refi[CU_SIZE][REFP_NUM];
//	int num_nz_coef[N_C][CU_SIZE];
//	u32 map_scu[CU_SIZE];
//	u32 map_cu_mode[CU_SIZE];
//#if USE_TSCPM
//	pel reco_y[CU_SIZE << 4];
//#endif
////*******************************************************
//}MD_INPUT;

typedef struct _MD1_RESULT{
    s64 cost_best;
	U3 best_split_mode;
	U2 best_cons_pred_mode;
}MD_RESULT, MD1_RESULT;

typedef struct _FETCH2MD_FW{
	/////Frame-level paramters
	U13 pic_width;
	U13 pic_height;
	U2  slice_type;
	U1 affine_enable_flag;
	U1 umve_enable_flag;
	U11  pic_width_in_scu;
	U11  pic_height_in_scu;
	//U3               chroma_quant_param_delta_cb;
	//U3               chroma_quant_param_delta_cr;
	U3  log2_max_cuwh;
	U3  log2_culine;
	U7 max_cuwh;

#if CTX_CONTROL_LUKE
	U4              bit_depth_internal;
	U4              max_part_ratio;
	U3               min_cu_size;
	U3               max_split_times;
	U4               min_qt_size;
	U7               max_bt_size;
	U7               max_eqt_size;
#endif
#if EIPM
	int              eipm_enable_flag;
#endif
#if MIPF
	int              mipf_enable_flag;
#endif
#if INTERPF
	int				 interpf_enable_flag;
#endif
	S16 refp_ptr[17][2];
	S16 refp_ptr_1[17][2];
#if ENABLE_BFRAME
	U8 num_refp[REFP_NUM];
#else
	U5 num_refp;
#endif
	
	U9 cur_ptr; // ref pic
#if CUDQP_QP_MAP
	s8 chroma_quant_param_delta_cb;
	s8 chroma_quant_param_delta_cr;
#endif

#if UMVE_ENH
	u8               umve_set_flag;
#endif

}FETCH2MD_FW;

typedef struct _FETCH2DBK_FW {
	U6 pic_width_in_lcu;
	U6 pic_height_in_lcu;
	U3 chroma_quant_param_delta_cb;
	U3 chroma_quant_param_delta_cr;
	U13 pic_width;
	U12 pic_height;
	U4 bit_depth;
	int qp_offset_bit_depth;
	int ref_ptr_dbk[MAX_NUM_REF_PICS][REFP_NUM];
}FETCH2DBK_FW;

typedef struct _FETCH2SAO_FW {
	U13 pic_width;
	U13 pic_height;
	U11  pic_width_in_scu;
	U7 pic_width_in_lcu;
	U7 pic_height_in_lcu;
	u8               sample_adaptive_offset_enable_flag;
	s64             lambda[3];
	u8                     tree_status;
	int              stride_luma;
	/* Stride of chroma picture */
	int              stride_chroma;
	int                    lcu_cnt;
	int				lcu_num;
	//////////////////////////////
	U7 max_cuwh;
	U3 log2_max_cuwh;
#if ENABLE_BFRAME
	U8 num_refp[REFP_NUM];
#else
	U5 num_refp;
#endif
	
	U3 log2_culine;
	U2 slice_type;
	U4 num_of_hmvp_cand;
	U1 affine_enable_flag;
	U1 amvr_enable_flag;
	U1 emvr_enable_flag;
	U1 dt_intra_enable_flag; //
	U1 position_based_transform_enable_flag;
	U1 tscpm_enable_flag;
	U1 ipf_enable_flag;
	U9 cur_ptr; // ref pic
#if MIPF
	int              mipf_enable_flag;
#endif
	int pix_x;
	int pix_y;
	int cu_width;
	int cu_height;
	U4 bit_depth;
	U32 qp_offset_bit_depth;
}FETCH2SAO_FW;
struct SAO2AEC {
	int rpm_num_refp[2];
	u8 slice_sao_enable[3];
	int x0;
	int y0;
	int cup;
	int cu_width;
	int cu_height;
	int cud;
	int parent_split;
	int qt_depth;
	int bet_depth;
	u8 cons_pred_mode;
	u8 tree_status;
	u8 log2_culine;
	int pic_header_slice_type;
	int info_max_cuwh;
	int info_pic_width;
	int info_pic_height;
	int sample_adaptive_offset_enable_flag;
	int lcu_cnt;
	int lcu_num;
	int sqh_umve_enable_flag;
	u8 sqh_num_of_hmvp_cand;
	u8 sqh_affine_enable_flag;
	u8 sqh_amvr_enable_flag;
	int sqh_emvr_enable_flag;
	u8 sqh_dt_intra_enable_flag;
	u8 sqh_position_based_transform_enable_flag;
	u8 sqh_tscpm_enable_flag;
	int sqh_ipf_enable_flag;
	u8 sqh_sample_adaptive_offset_enable_flag;
	int info_pic_width_in_scu;
	int info_pic_width_in_lcu;
	int x_lcu;
	int y_lcu;
	int x_pel;
	int y_pel;
	int ptr;
	int MergeLeftAvail;
	int MergeUpAvail;
};

struct SRCC_FOR_AEC
{
	int line_idx_in_lcu_y;
	int line_idx_in_lcu_u;
	int line_idx_in_lcu_v;
	s16 coef_for_srcc[3][256][16];
	s16 blkpos_for_srcc[3][256][16];
};
typedef struct _MD_FW
{
	U13 pic_width;
	U13 pic_height;
	U2  slice_type;
	U2  slice_qp;
	U7 max_dt_size;
	U1 affine_enable_flag;
	U1 umve_enable_flag;
	U1 dt_intra_enable_flag;//
	U4 num_of_hmvp_cand;
	U1  ipf_enable_flag;
	U11  pic_width_in_scu;
	U11  pic_height_in_scu;
	U3  log2_max_cuwh;
    U3  log2_culine;
	U7 max_cuwh;
	U21 f_scu;
	U6             gop_size;
#if CTX_CONTROL_LUKE
	U4              bit_depth_internal;
	U4              max_part_ratio;
	U3               min_cu_size;
	U3               max_split_times;
	U4               min_qt_size;
	U7               max_bt_size;
	U7               max_eqt_size;
	U1				 affine_subblock_size_idx;
	U1				 ipcm_enable_flag;
	U1			 position_based_transform_enable_flag;
	U1              secondary_transform_enable_flag;
	U1              tscpm_enable_flag;
	U1				 amvr_enable_flag;
	U1				 emvr_enable_flag;
#endif
#if EIPM
	int eipm_enable_flag;
#endif
#if MIPF
	int              mipf_enable_flag;
#endif
#if INTERPF
	int				 interpf_enable_flag;
#endif
#if UMVE_ENH
	u8               umve_set_flag;
#endif
	int refp_ptr[17][2];
#if ENABLE_BFRAME
	U5 num_refp[REFP_NUM];
#else
	U5 num_refp;
#endif
    
    U9 cur_ptr; // ref pic
#if CUDQP_QP_MAP
	s8 chroma_quant_param_delta_cb;
	s8 chroma_quant_param_delta_cr;
#endif

	friend void copy(_MD_FW& _Left, const _MD_FW& _Right)
	{
		_Left.pic_width = _Right.pic_width;
		_Left.pic_height = _Right.pic_height;
		_Left.slice_type = _Right.slice_type;
		_Left.slice_qp = _Right.slice_qp;
		_Left.max_dt_size = _Right.max_dt_size;
		_Left.affine_enable_flag = _Right.affine_enable_flag;
		_Left.umve_enable_flag = _Right.umve_enable_flag;
		_Left.dt_intra_enable_flag = _Right.dt_intra_enable_flag;
		_Left.num_of_hmvp_cand = _Right.num_of_hmvp_cand;
		_Left.ipf_enable_flag = _Right.ipf_enable_flag;
		_Left.pic_width_in_scu = _Right.pic_width_in_scu;
		_Left.pic_height_in_scu = _Right.pic_height_in_scu;
		_Left.log2_max_cuwh = _Right.log2_max_cuwh;
		_Left.log2_culine = _Right.log2_culine;
		_Left.max_cuwh = _Right.max_cuwh;
		_Left.f_scu = _Right.f_scu;
		_Left.gop_size = _Right.gop_size;
#if CTX_CONTROL_LUKE
		_Left.bit_depth_internal = _Right.bit_depth_internal;
		_Left.max_part_ratio = _Right.max_part_ratio;
		_Left.min_cu_size = _Right.min_cu_size;
		_Left.max_split_times = _Right.max_split_times;
		_Left.min_qt_size = _Right.min_qt_size;
		_Left.max_bt_size = _Right.max_bt_size;
		_Left.max_eqt_size = _Right.max_eqt_size;
		_Left.affine_subblock_size_idx = _Right.affine_subblock_size_idx;
		_Left.ipcm_enable_flag = _Right.ipcm_enable_flag;
		_Left.position_based_transform_enable_flag = _Right.position_based_transform_enable_flag;
		_Left.secondary_transform_enable_flag = _Right.secondary_transform_enable_flag;
		_Left.tscpm_enable_flag = _Right.tscpm_enable_flag;
		_Left.amvr_enable_flag = _Right.amvr_enable_flag;
		_Left.emvr_enable_flag = _Right.emvr_enable_flag;
#endif
#if EIPM
		_Left.eipm_enable_flag = _Right.eipm_enable_flag;
#endif
#if MIPF
		_Left.mipf_enable_flag = _Right.mipf_enable_flag;
#endif
#if INTERPF
		_Left.interpf_enable_flag = _Right.interpf_enable_flag;
#endif
#if UMVE_ENH
		_Left.umve_set_flag = _Right.umve_set_flag;
#endif
		for (int i = 0; i < 17; ++i)
		{
			for (int j = 0; j < 2; ++j)
			{
				_Left.refp_ptr[i][j] = _Right.refp_ptr[i][j];
			}
		}
#if ENABLE_BFRAME
		for (int j = 0; j < REFP_NUM; ++j)
		{
			_Left.num_refp[j] = _Right.num_refp[j];
	}
#else
		_Left.num_refp = _Right.num_refp;
#endif

		_Left.cur_ptr = _Right.cur_ptr; // ref pic
#if CUDQP_QP_MAP
		_Left.chroma_quant_param_delta_cb = _Right.chroma_quant_param_delta_cb;
		_Left.chroma_quant_param_delta_cr = _Right.chroma_quant_param_delta_cr;
#endif
	}
}MD_FW;

typedef struct _DBK_FW{
	int log2_max_cuwh; //ctx->info. log2_max_cuwh;
	int pic_width;// ctx->info.pic_width;
	int pic_height;// ctx->info.pic_height;
	int pic_width_in_lcu; //ctx->info.pic_width_in_lcu;
	int max_cuwh; //ctx->info.max_cuwh;
	int pic_width_in_scu; //ctx->info.pic_width_in_scu;
	u8 cross_patch_loop_filter; //ctx->info.sqh.cross_patch_loop_filter;
	s8 chroma_quant_param_delta_cb; //ctx->info.pic_header.chroma_quant_param_delta_cb;
	s8 chroma_quant_param_delta_cr; //ctx->info.pic_header.chroma_quant_param_delta_cr;
	int  pic_stride_luma ;///ctx
	int  pic_stride_chroma ;
}DBK_FW;
typedef struct _SAO_FW{
	u8 sample_adaptive_offset_enable_flag;
	u8 cross_patch_loop_filter;
	int  pic_width; /////
	int  pic_height;
	int  log2_max_cuwh;//ctx->info
	int  pic_width_in_scu;//ctx->info
	int  bit_depth_internal;//ctx->info
	int  qp_offset_bit_depth;// =ctx->info.qp_offset_bit_depth = (8 * (ctx->info.bit_depth_internal - 8));
}SAO_FW;
typedef struct _AEC_FW{
	int   f_scu;
	int lcu_num;// core_lcu_num; ////
	u8 skip_flag;// core_skip_flag;
	int log2_culine; //ctx_log2_culine;
	int max_cuwh;// ctx_info_max_cuwh;
	int pic_width_in_lcu;// ctx_info_pic_width_in_lcu;
	int pic_width_in_scu;// ctx_info_pic_width_in_scu;
	u8 slice_sao_enable[3]; //ctx_info_shext_slice_sao_enable[3];
	u8 pic_header_slice_type; //ctx_info_pic_header_slice_type;
	u8 affine_enable_flag; //ctx_info_sqh_addine_enable_flag;
	int pic_width; //ctx_info_pic_width;
	int pic_height; //ctx_info_pic_height;

	int umve_enable_flag;//    ctx_info_sqh_umve_enable_flag = 1
	u8 num_of_hmvp_cand;////ctx->info.sqh.num_of_hmvp_cand
	u8 amvr_enable_flag;// ctx_info_sqh_amvr_enable_flag = 1
	int emvr_enable_flag;//    ctx_info_sqh_emvr_enable_flag = 1
	u8 dt_intra_enable_flag;// ctx_info_sqh_dt_intra_enable_flag = 0
	u8 position_based_transform_enable_flag;//ctx_info_sqh_position_based_transform_enable_flag = 0
	u8 tscpm_enable_flag;// ctx_info_sqh_tscpm_enable_flag = 0
	int ipf_enable_flag;
	u8 sample_adaptive_offset_enable_flag;

	u8 slice_type; //ctx_info_pic_header_slice_type;
	int lcu_total_cnt;

	U8  rpm_num_refp[REFP_NUM];
	int fixed_slice_qp_flag;
/***********************************************************************************/
    u8 cu_delta_qp_flag;
	u8 ph_ists_enable_flag;
	u8 ph_epmc_model_flag;
	u8 ibc_flag;
	u8 sp_pic_flag;
	u8 evs_ubvs_pic_flag;
	u8 umve_set_flag;
	u8 ph_ipc_flag;
	int cu_qp_group_area_size;
	s8 chroma_quant_param_delta_cb;
	s8 chroma_quant_param_delta_cr;
	u8 slice_qp;
	u8 min_cu_size;
	u8 max_part_ratio;
	u8 max_split_times;
	u8 min_qt_size;
	u8 max_bt_size;
	u8 max_eqt_size;
	u8 max_dt_size;
	u8 abvr_enable_flag;
	int umve_enh_enable_flag;
	int awp_enable_flag;
	int awp_mvr_enable_flag;
	int etmvp_enable_flag;
	u8 affine_umve_enable_flag;
	u8 smvd_enable_flag;
	u8 bgc_enable_flag;
	u8 interpf_enable_flag;
	u8 ipc_enable_flag;
	u8 num_of_mvap_cand;
	u8 num_of_hbvp_cand;
	u8 enhance_tscpm_enable_flag;
	u8 pmc_enable_flag;
	u8 iip_enable_flag;
	u8 sawp_enable_flag;
	u8 sbt_enable_flag;
	u8 ists_enable_flag;
	u8 srcc_enable_flag;
	u8 eipm_enable_flag;
	u8 st_chroma_enable_flag;
	u8 est_enable_flag;
	int log2_max_cuwh;
	int bit_depth_input;
    int qp_offset_bit_depth;
	int ptr;
	u8 temporal_id;
	int refp_ptr[MAX_NUM_REF_PICS][REFP_NUM];
	//int num_refp[REFP_NUM];
	BOOL dataCol;
	u32 umveOffsetPicCount[UMVE_REFINE_STEP_SEC_SET];
	int cons_pred_mode;
	u8 tree_status;
	int cu_delta_qp_lcu_map[256];
	int num_delta_qp;
	int pred_qp;
	int cu_qp_group_x;
	int cu_qp_group_y;
/***********************************************************************************/
}AEC_FW;
typedef struct _FRAME_LEVEL_FW{
	u8 num_of_hmvp_cand; //ctx_info_sqh_num_of_hmvp_cand = 8
	u8 amvr_enable_flag;// ctx_info_sqh_amvr_enable_flag = 1
	int emvr_enable_flag;//	ctx_info_sqh_emvr_enable_flag = 1
	u8 dt_intra_enable_flag;// ctx_info_sqh_dt_intra_enable_flag = 0
	u8 position_based_transform_enable_flag;//ctx_info_sqh_position_based_transform_enable_flag = 0
	u8 tscpm_enable_flag;// ctx_info_sqh_tscpm_enable_flag = 0
	int umve_enable_flag;//	ctx_info_sqh_umve_enable_flag = 1
	int affine_enable_flag;// unsigned char  affine_enable_flag = 1

	int pic_width;
	int pic_height;
	int pic_width_in_scu;
	int pic_height_in_scu;
	int pic_width_in_lcu; //ctx->info.pic_width_in_lcu;
	int pic_height_in_lcu;
	int log2_max_cuwh;
	int log2_culine;
	int max_cuwh;
	int f_scu;
	int log2_scu_w;
	int scu_w;
	int pic_stride_luma;///ctx
	int pic_stride_chroma;

	unsigned char max_dt_size;
	int gop_size;
	u8  max_part_ratio;
	u8  min_cu_size;
	u8  max_split_times;
	u8  min_qt_size;
	u8  max_bt_size;
	u8  max_eqt_size;
	int affine_subblock_size_idx;
	u8 cross_patch_loop_filter; //ctx->info.sqh.cross_patch_loop_filter;
	int bit_depth_internal;
	u8 yuv_cs;

	unsigned char  slice_type;
	short patch_idx;
	int patch_width_in_lcu;
	int patch_height_in_lcu;

	int ipf_enable_flag;
	int ipcm_enable_flag;
	int secondary_transform_enable_flag;
	u8 slice_sao_enable[3]; //ctx_info_shext_slice_sao_enable[3];
	u8 sample_adaptive_offset_enable_flag;

	int refp_ptr[17][2];
	u8 num_refp;
	int cur_ptr; // ref pic
	int lcu_total_cnt;
	int frame_id;
	int lcu_num;

	u32 lambdas[3];
	u32 lambda;///////s64??
	u32 qp_y;
	u32 qp_u;
	u32 qp_v;
	u32 dist_chroma_weight[2];
	s8 chroma_quant_param_delta_cb; //ctx->info.pic_header.chroma_quant_param_delta_cb;
	s8 chroma_quant_param_delta_cr; //ctx->info.pic_header.chroma_quant_param_delta_cr;
	int  qp_offset_bit_depth;// =ctx->info.qp_offset_bit_depth = (8 * (ctx->info.bit_depth_internal - 8));
	u8 fixed_slice_qp_flag;

	unsigned int org_pic_y_addr;
	unsigned int org_pic_u_addr;
	unsigned int org_pic_v_addr;
	unsigned int rec_pic_y_addr;
	unsigned int rec_pic_u_addr;
	unsigned int rec_pic_v_addr;

	u8 glb_ctrl;
	u8 dbk_en;
	u8 cplf_en;
	u8 module_enable;
	u8 sao_enable;
	#if RATECONTROL
	int EncControl;
	int ECInitialQP;
	int useDQP;
	int intra_period;
	int TargetBitRate;
	int framerate;
	int prev_f_bits;
	int TotalFrames;
#endif
}FRAME_LEVEL_FW;
#if USE_SP
/*****************************************************************************
 * SP mode structure
 *****************************************************************************/
typedef enum _SP_HASH_STATUS_IDX
{
	HS_CURR_BEST,
	HS_NEXT_BEST,
	HS_TEMP_BEST,
	HS_CTX_NUM,
} SP_HASH_STATUS_IDX;
typedef enum _SP_SRB_RUN_MODE
{
	SRB_RUN_LEFT = 0,
	SRB_RUN_ABOVE = 1,
	NUM_SRB_RUN = 2
}SP_SRB_RUN_MODE;
typedef enum _SP_CHANNEL_TYPE
{
	CHANNEL_TYPE_LUMA = 0,
	CHANNEL_TYPE_CHROMA = 1,
	MAX_NUM_CHANNEL_TYPE = 2
}SP_CHANNEL_TYPE;
typedef enum _SP_MATCH_TYPE
{
	MATCH_POS_WIDTH = 0,
	MATCH_POS_ONE = 1,
	MATCH_NONE = 2,
	NUMBER_OF_MATCH_TYPES = 3
}SP_MATCH_TYPE;

typedef struct _CS2_MODE_INFO
{
	u8              string_copy_direction;
	int             sub_string_no;
	COM_SP_EVS_INFO p_evs_copy_info[SP_STRING_INFO_NO];
	int             unpredict_pix_num;
	COM_SP_PIX      unpredict_pix_info[SP_STRING_INFO_NO];
	//SRB
	u8              m_cs2_mode_flag;
	u8              m_evs_present_flag;
	u8              m_unpredictable_pixel_present_flag;
	pel             m_srb[3][MAX_SRB_SIZE];
	u8              m_pvbuf_size;
	u8              m_pvbuf_reused_flag[MAX_SRB_PRED_SIZE];
	pel             m_srb_prev[3][MAX_SRB_PRED_SIZE];
	u8              m_pvbuf_size_prev;
	u8              m_bit_depth;
	u8              m_all_comp_flag[MAX_SRB_SIZE];
	u8              m_all_comp_pre_flag[MAX_SRB_PRED_SIZE];
	u8              m_cuS_flag[MAX_SRB_SIZE];
	u8              m_cuS_pre_flag[MAX_SRB_PRED_SIZE];
	s16             m_pv_x[MAX_SRB_SIZE];
	s16             m_pv_prev_x[MAX_SRB_PRED_SIZE];
	s16             m_pv_y[MAX_SRB_SIZE];
	s16             m_pv_prev_y[MAX_SRB_PRED_SIZE];
	u8              m_dpb_idx[MAX_SRB_SIZE];
	u8              m_dpb_idx_prev[MAX_SRB_PRED_SIZE];
	u8              m_dpb_reYonly[MAX_SRB_SIZE];
	u8              m_dpb_reYonly_prev[MAX_SRB_PRED_SIZE];
} CS2_MODE_INFO;

//////////////////////////add new struct for sao///////////////////////
typedef struct
{
	pel y_org_sao[64][64];
	pel u_org_sao[32][32];
	pel v_org_sao[32][32];
}MD2SAO_;

#endif
#define AEC_STAGE1_DATA 0
#define AEC_STAGE2_DATA 0
#define PRINT_AEC_DATA  0
#define CTU_RATE_CONTROL  1
#endif /* _ENC_DEF_H_ */
#endif
