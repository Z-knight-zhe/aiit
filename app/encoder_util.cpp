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

#include "com_typedef.h"
#include "app_util.h"
#include "app_args.h"
#include "enc_def.h"
#include "ratectrl.h"
#include "rate_control_picture.h"
//#include "aec.h"
#include <math.h>

#include <time.h>

#include "fetch.h"
#include "rmd.h"
#include "md1.h"
#include "md2.h"
#include "dbk.h"
#include "sao.h"
#include "aec.h"
#include "encoder_util.h"
using namespace std;
#pragma warning(disable:6297)
#pragma warning(disable:26812)
#if LINUX
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#ifdef MCC_SIM
#ifdef USE_KERNEL_TOP

#include "__merlinkernel_top.h"

#else

#ifdef USE_KERNEL_SAODBK
#include "__merlink_SAODBK.h"
#endif

#ifdef USE_KERNEL_mpm
#include "__merlink_mpm.h"
#endif

#ifdef USE_KERNEL_AEC_SAO
#include "__merlink_AEC_SAO.h"
#endif

#ifdef USE_KERNEL_UNIT_CHROMA
#include "__merlink_unit_chroma.h"
#endif

#ifdef USE_KERNEL_IME_MVP
#include "__merlink_ime_mvp.h"
#endif

#ifdef USE_KERNEL_IME_AFFGRA
#include "__merlink_ime_affgra.h"
#endif

#ifdef USE_KERNEL_IME_AFFMVP
#include "__merlink_ime_affmvp.h"
#endif

#ifdef USE_KERNEL_IME_TRAN
#include "__merlink_ime_tran.h"
#endif

#ifdef USE_KERNEL_IME_MC
#include "__merlink_ime_mc.h"
#endif

#ifdef USE_KERNEL_IME_AFFMC
#include "__merlink_ime_affmc.h"
#endif

#ifdef USE_KERNEL_IME_AFFMC_L
#include "__merlink_ime_affmc_l.h"
#endif

#ifdef USE_KERNEL_SUB_ME
#include "__merlink_sub_me.h"
#endif

#ifdef USE_KERNEL_com_get_affine_memory_access
#include "__merlink_com_get_affine_memory_access.h"
#endif

#ifdef USE_KERNEL_CHECK_BEST
#include "__merlink_check_best.h"
#endif

#ifdef USE_KERNEL_com_get_affine_merge_candidate
#include "__merlink_com_get_affine_merge_candidate.h"
#endif

#ifdef USE_KERNEL_INTER_CAND
#include "__merlink_inter_cand.h"
#endif

#ifdef USE_KERNEL_EST_INTER
#include "__merlink_est_inter.h"
#endif

#ifdef USE_KERNEL_enc_comp
#include "__merlink_enc_comp.h"
#endif

#ifdef USE_KERNEL_TQ
#include "__merlink_TQ.h"
#endif

#ifdef USE_KERNEL_enc_eco_xu
#include "__merlink_enc_eco.h"
#endif

#ifdef USE_KERNEL_encode_ipf_flag
#include "__merlink_encode_ipf_flag.h"
#endif

#ifdef USE_KERNEL_AFFINE_MERGE
#include "__merlink_affine_merge.h"
#endif

#ifdef USE_KERNEL_SKIP
#include "__merlink_skip.h"
#endif

#ifdef USE_KERNEL_enc_eco_unit
#include "__merlink_enc_eco_unit.h"
#endif

#ifdef USE_KERNEL_AFFINE_PRED_ME
#include "__merlink_aff_me.h"
#endif

#ifdef USE_KERNEL_AFFINE_PRED_RDO
#include "__merlink_aff_rdo.h"
#endif

#endif // USE_KERNEL_TOP
#endif // MCC_SIM

void handler(int sig)
{
    void *array[10];
    size_t size;
    // get void*'s for all entries on the stack
    size = backtrace(array, 10);
    // print out all the frames to stderr
    fprintf(stderr, "Error: signal %d:\n", sig);
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    exit(1);
}
#endif

#define VERBOSE_FRAME              VERBOSE_1
#define MAX_BUMP_FRM_CNT           (8 <<1)

#define MAX_BS_BUF                 (32*1024*1024)
#define PRECISE_BS_SIZE            1

extern int imgb_addref(COM_IMGB * imgb);
extern int imgb_release(COM_IMGB * imgb);
#if Debug422
extern strFetch_input_video_U8_ptr Fetch_input_video_U8_ptr;
#endif


#define CALC_SSIM           0    //calculate Multi-scale SSIM (ms_ssim)


static char op_fname_cfg[256] = "\0"; /* config file path name */
char op_fname_inp[256] = "\0"; /* input original video */
static char op_fname_out[256] = "\0"; /* output bitstream */
static char op_fname_rec[256] = "\0"; /* reconstructed video */
#if USE_ROI_QP_CONFIG
char op_fname_qpmap[256] = "\0";
#endif
#if USE_ROI_MODE_CONFIG
char op_fname_modemap[256] = "\0";
#endif
#if CUDQP_QP_MAP
char op_cu_qpmap[256] = "\0";
#endif
static int  op_max_frm_num = 0;
#if Debug422
static int  op_chroma_format = 1;
#endif
static int  op_use_pic_signature = 0;
static int  op_w = 0;
static int  op_h = 0;
int  op_qp = 0;
static int  op_fps = 0;
static int  op_i_period = 0;
static int  op_max_b_frames= 0;
static int  op_disable_hgop = 0;
static int  op_bit_depth_input = 8;
static int  op_skip_frames = 0;
static int  op_bit_depth_internal = 0;
static int  op_frame_qp_add = 0;
static int  op_pb_frame_qp_offset = 0;
static int  op_qp_offset_cb = 1;
static int  op_qp_offset_cr = 1;
static int  op_delta_qp_flag = 0;
static int  op_qp_offset_adp = 1;
#if CUDQP_QP_MAP
static int  op_cu_delta_qp_flag = 0;
static int  op_cu_qp_group_size = 16;
#endif
static int  op_tool_ipcm          = 1;
static int  op_tool_amvr          = 1;
static int  op_tool_affine        = 1;
static int  op_tool_smvd          = 1;
static int  op_tool_deblock       = 1;
static int  op_tool_hmvp          = 8;
static int  op_tool_tscpm         = 1;
static int  op_tool_umve          = 1;
static int  op_tool_emvr          = 1;
static int  op_tool_ipf           = 0;

#if DT_PARTITION
static int  op_tool_dt_intra      = 1;
#endif
static int  op_tool_pbt           = 1;

static int  op_tool_sao           = 1;

static int  op_tool_alf           = 1;

static int  op_tool_secTrans      = 0;
#if PATCH
static int op_patch_stable        = 1;
static int op_cross_patch_loop_filter = 1;
static int op_patch_ref_colocated = 0;
static int op_patch_uniform       = 1;
static int op_patch_width_in_lcu  = 0;
static int op_patch_height_in_lcu = 0;
static int op_multislice_mode = 0;
int op_head_info_flag = 0;
int op_patch_mode_on = 0;
#endif

#if USE_INTRA_REFRESH
static int op_intra_refresh_control = 0;
static int op_intra_refresh_size = 1;
static int op_intra_refresh_mode = 0;
#endif

#if USE_SPEED_LEVEL
static int op_speed_level = 1;
#endif

#if USE_ROI_QP_CONFIG
static int op_roi_qp_enable_1 = 0;
static int op_roi_qp_enable_2 = 0;
static int op_roi_qp_enable_3 = 0;
static int op_roi_qp_enable_4 = 0;
static int op_roi_qp_enable_5 = 0;
static int op_roi_qp_enable_6 = 0;
static int op_roi_qp_enable_7 = 0;
static int op_roi_qp_enable_8 = 0;

static int op_roi_qp_start_x_1 = 0;
static int op_roi_qp_start_x_2 = 0;
static int op_roi_qp_start_x_3 = 0;
static int op_roi_qp_start_x_4 = 0;
static int op_roi_qp_start_x_5 = 0;
static int op_roi_qp_start_x_6 = 0;
static int op_roi_qp_start_x_7 = 0;
static int op_roi_qp_start_x_8 = 0;

static int op_roi_qp_end_x_1 = 0;
static int op_roi_qp_end_x_2 = 0;
static int op_roi_qp_end_x_3 = 0;
static int op_roi_qp_end_x_4 = 0;
static int op_roi_qp_end_x_5 = 0;
static int op_roi_qp_end_x_6 = 0;
static int op_roi_qp_end_x_7 = 0;
static int op_roi_qp_end_x_8 = 0;

static int op_roi_qp_start_y_1 = 0;
static int op_roi_qp_start_y_2 = 0;
static int op_roi_qp_start_y_3 = 0;
static int op_roi_qp_start_y_4 = 0;
static int op_roi_qp_start_y_5 = 0;
static int op_roi_qp_start_y_6 = 0;
static int op_roi_qp_start_y_7 = 0;
static int op_roi_qp_start_y_8 = 0;

static int op_roi_qp_end_y_1 = 0;
static int op_roi_qp_end_y_2 = 0;
static int op_roi_qp_end_y_3 = 0;
static int op_roi_qp_end_y_4 = 0;
static int op_roi_qp_end_y_5 = 0;
static int op_roi_qp_end_y_6 = 0;
static int op_roi_qp_end_y_7 = 0;
static int op_roi_qp_end_y_8 = 0;

static int op_roi_qp_1 = 0;
static int op_roi_qp_2 = 0;
static int op_roi_qp_3 = 0;
static int op_roi_qp_4 = 0;
static int op_roi_qp_5 = 0;
static int op_roi_qp_6 = 0;
static int op_roi_qp_7 = 0;
static int op_roi_qp_8 = 0;

static int op_useQPMAP = 0;
static int op_useQPMAPCon = 0;
#endif

#if CUDQP_QP_MAP
static int op_useCuQPMAP = 0;

#endif

#if USE_ROI_MODE_CONFIG
static int op_roi_mode_enable_1 = 0;
static int op_roi_mode_enable_2 = 0;
static int op_roi_mode_enable_3 = 0;
static int op_roi_mode_enable_4 = 0;
static int op_roi_mode_enable_5 = 0;
static int op_roi_mode_enable_6 = 0;
static int op_roi_mode_enable_7 = 0;
static int op_roi_mode_enable_8 = 0;

static int op_roi_mode_start_x_1 = 0;
static int op_roi_mode_start_x_2 = 0;
static int op_roi_mode_start_x_3 = 0;
static int op_roi_mode_start_x_4 = 0;
static int op_roi_mode_start_x_5 = 0;
static int op_roi_mode_start_x_6 = 0;
static int op_roi_mode_start_x_7 = 0;
static int op_roi_mode_start_x_8 = 0;

static int op_roi_mode_end_x_1 = 0;
static int op_roi_mode_end_x_2 = 0;
static int op_roi_mode_end_x_3 = 0;
static int op_roi_mode_end_x_4 = 0;
static int op_roi_mode_end_x_5 = 0;
static int op_roi_mode_end_x_6 = 0;
static int op_roi_mode_end_x_7 = 0;
static int op_roi_mode_end_x_8 = 0;

static int op_roi_mode_start_y_1 = 0;
static int op_roi_mode_start_y_2 = 0;
static int op_roi_mode_start_y_3 = 0;
static int op_roi_mode_start_y_4 = 0;
static int op_roi_mode_start_y_5 = 0;
static int op_roi_mode_start_y_6 = 0;
static int op_roi_mode_start_y_7 = 0;
static int op_roi_mode_start_y_8 = 0;

static int op_roi_mode_end_y_1 = 0;
static int op_roi_mode_end_y_2 = 0;
static int op_roi_mode_end_y_3 = 0;
static int op_roi_mode_end_y_4 = 0;
static int op_roi_mode_end_y_5 = 0;
static int op_roi_mode_end_y_6 = 0;
static int op_roi_mode_end_y_7 = 0;
static int op_roi_mode_end_y_8 = 0;

static int op_roi_mode_1 = 0;
static int op_roi_mode_2 = 0;
static int op_roi_mode_3 = 0;
static int op_roi_mode_4 = 0;
static int op_roi_mode_5 = 0;
static int op_roi_mode_6 = 0;
static int op_roi_mode_7 = 0;
static int op_roi_mode_8 = 0;

static int op_useModeMAP = 0;
static int op_useModeMAPCon = 0;
#endif

#if USE_TRACE_DUMP
static int op_useTraceDump = 0;
#endif

#if HLS_RPL
static char  op_rpl0[MAX_NUM_RPLS][256];
static char  op_rpl1[MAX_NUM_RPLS][256];
#endif

#if LIBVC_ON
static int  op_tool_libpic = 0;
static int  op_qp_offset_libpic = -6;
static char op_fname_libout[256] = "libpic.bin"; /* output libpic bitstream */
static char op_fname_librec[256] = "\0"; /* reconstructed libpic */
static char op_fname_libdependency[256] = "\0"; /* the file storing library picture information */
static int  op_skip_frames_when_extract_libpic = 0;
static int  op_max_frm_num_when_extract_libpic = 0;
#endif
static int  op_subSampleRatio     = 1;

static int  op_wq_enable = 0;
static int  op_seq_wq_mode = 0;
static char op_seq_wq_user[2048] = "\0";
static int  op_pic_wq_data_idx = 0;
static char op_pic_wq_user[2048] = "\0";
static int  op_wq_param = 0;
static int  op_wq_model = 0;
static char op_wq_param_detailed  [256] = "[64,49,53,58,58,64]";
static char op_wq_param_undetailed[256] = "[67,71,71,80,80,106]";
static int  op_ctu_size        = 128;
static int  op_min_cu_size     = 4;
static int  op_max_part_ratio  = 8;
static int  op_max_split_times = 6;
static int  op_min_qt_size     = 8;
static int  op_max_bt_size     = 128;
static int  op_fps_num = 25;
static int  op_fps_den = 1;
static int  op_rc_min_qp = 0;
static int  op_rc_max_qp = 50;
static int  op_bitrate = 400;
static int  op_rc_method = 0;
static int  op_vbv_max_bitrate = 0;
static int  op_vbv_buffer_size = 0;

static int  op_max_eqt_size    = 64;
static int  op_max_dt_size     = 64;
#if UMVE_ENH
static int  op_tool_umve_enh = 1;
#endif
// SC_ENV parameters
#if defined LINUX
char op_sc_tv_path[512] = "../test_vector";
#else 
char op_sc_tv_path[512] = "..\\..\\test_vector";
#endif

char op_sc_vcd[256] = "./bin/trace";

std::string g_tv_path = "";

glb_reg_tv g_glb_info;
md_info_tv g_md_info;

#if RATECONTROL
int op_enable_ratectrl = 0;
int op_target_bitrate = 0;
int op_rc_initialqp = 0;
int op_block_ratectrl = 0;
int op_param_update = 0;
int op_intra_group = 0;
#endif

typedef enum _OP_FLAGS
{
    OP_FLAG_FNAME_CFG,
    OP_FLAG_FNAME_INP,
    OP_FLAG_FNAME_OUT,
    OP_FLAG_FNAME_REC,
    OP_FLAG_WIDTH_INP,
    OP_FLAG_HEIGHT_INP,
    OP_FLAG_QP,
    OP_FLAG_FPS,
    OP_FLAG_IPERIOD,
    OP_FLAG_MAX_FRM_NUM,
#if Debug422
    OP_FLAG_CHROMA_FORMAT,
#endif
    OP_FLAG_USE_PIC_SIGN,
    OP_FLAG_VERBOSE,
    OP_FLAG_MAX_B_FRAMES,
    OP_FLAG_DISABLE_HGOP,
    OP_FLAG_OUT_BIT_DEPTH,
    OP_FLAG_IN_BIT_DEPTH,
    OP_FLAG_SKIP_FRAMES,
    OP_FLAG_ADD_QP_FRAME,
    OP_FLAG_PB_FRAME_QP_OFFSET,
    OP_FLAG_QP_OFFSET_CB,
    OP_FLAG_QP_OFFSET_CR,
    OP_FLAG_QP_OFFSET_ADP,
    OP_FLAG_DELTA_QP,
#if CUDQP_QP_MAP
	OP_FLAG_CU_DELTA_QP,
	OP_CU_QP_GROUP_SIZE,
#endif
    OP_TOOL_IPCM,
    OP_TOOL_AMVR,
    OP_TOOL_IPF,
    OP_TOOL_AFFINE,
    OP_TOOL_SMVD,
    OP_TOOL_DT_INTRA,
    OP_TOOL_PBT,
    OP_TOOL_DEBLOCK,
    OP_TOOL_SAO,
    OP_TOOL_ALF,
    OP_WQ_ENABLE,
    OP_SEQ_WQ_MODE,
    OP_SEQ_WQ_FILE,
    OP_PIC_WQ_DATA_IDX,
    OP_PIC_WQ_FILE,
    OP_WQ_PARAM,
    OP_WQ_PARAM_MODEL,
    OP_WQ_PARAM_DETAILED,
    OP_WQ_PARAM_UNDETAILED,

    OP_TOOL_HMVP,
    OP_TOOL_TSCPM,
    OP_TOOL_UMVE,
    OP_TOOL_EMVR,
#if PATCH
    OP_PATCH_STABLE,
    OP_CROSS_PATCH_LOOP_FILTER,
    OP_PATCH_REF_COLOCATED,
    OP_PATCH_UNIFORM,
    OP_PATCH_WIDTH,
    OP_PATCH_HEIGHT,
    OP_PATCH_COLUMNS,
    OP_PATCH_ROWS,
    OP_COLUMN_WIDTH,
    OP_ROW_HEIGHT,
    OP_MULTISLICE_MODE,
#endif
#if USE_INTRA_REFRESH
	OP_INTRA_REFRESH_CONTROL,
	OP_INTRA_REFRESH_SIZE,
	OP_INTRA_REFRESH_MODE,
#endif
#if USE_SPEED_LEVEL
	OP_SPEED_LEVEL,
#endif
#if USE_ROI_QP_CONFIG
	OP_ROI_QP_ENABLE_1,
	OP_ROI_QP_ENABLE_2,
	OP_ROI_QP_ENABLE_3,
	OP_ROI_QP_ENABLE_4,
	OP_ROI_QP_ENABLE_5,
	OP_ROI_QP_ENABLE_6,
	OP_ROI_QP_ENABLE_7,
	OP_ROI_QP_ENABLE_8,

	OP_ROI_QP_START_X_1,
	OP_ROI_QP_START_X_2,
	OP_ROI_QP_START_X_3,
	OP_ROI_QP_START_X_4,
	OP_ROI_QP_START_X_5,
	OP_ROI_QP_START_X_6,
	OP_ROI_QP_START_X_7,
	OP_ROI_QP_START_X_8,

	OP_ROI_QP_END_X_1,
	OP_ROI_QP_END_X_2,
	OP_ROI_QP_END_X_3,
	OP_ROI_QP_END_X_4,
	OP_ROI_QP_END_X_5,
	OP_ROI_QP_END_X_6,
	OP_ROI_QP_END_X_7,
	OP_ROI_QP_END_X_8,

	OP_ROI_QP_START_Y_1,
	OP_ROI_QP_START_Y_2,
	OP_ROI_QP_START_Y_3,
	OP_ROI_QP_START_Y_4,
	OP_ROI_QP_START_Y_5,
	OP_ROI_QP_START_Y_6,
	OP_ROI_QP_START_Y_7,
	OP_ROI_QP_START_Y_8,

	OP_ROI_QP_END_Y_1,
	OP_ROI_QP_END_Y_2,
	OP_ROI_QP_END_Y_3,
	OP_ROI_QP_END_Y_4,
	OP_ROI_QP_END_Y_5,
	OP_ROI_QP_END_Y_6,
	OP_ROI_QP_END_Y_7,
	OP_ROI_QP_END_Y_8,

	OP_ROI_QP_1,
	OP_ROI_QP_2,
	OP_ROI_QP_3,
	OP_ROI_QP_4,
	OP_ROI_QP_5,
	OP_ROI_QP_6,
	OP_ROI_QP_7,
	OP_ROI_QP_8,

	OP_USE_QP_MAP,
	OP_USE_QP_MAPCon,
	OP_QPMAP_FLAG_FNAME,
#endif
#if CUDQP_QP_MAP
        OP_USE_CU_QP_MAP,
        OP_CUQPMAP_FLAG_FNAME,
#endif
#if USE_ROI_MODE_CONFIG
	OP_ROI_MODE_ENABLE_1,
	OP_ROI_MODE_ENABLE_2,
	OP_ROI_MODE_ENABLE_3,
	OP_ROI_MODE_ENABLE_4,
	OP_ROI_MODE_ENABLE_5,
	OP_ROI_MODE_ENABLE_6,
	OP_ROI_MODE_ENABLE_7,
	OP_ROI_MODE_ENABLE_8,

	OP_ROI_MODE_START_X_1,
	OP_ROI_MODE_START_X_2,
	OP_ROI_MODE_START_X_3,
	OP_ROI_MODE_START_X_4,
	OP_ROI_MODE_START_X_5,
	OP_ROI_MODE_START_X_6,
	OP_ROI_MODE_START_X_7,
	OP_ROI_MODE_START_X_8,

	OP_ROI_MODE_END_X_1,
	OP_ROI_MODE_END_X_2,
	OP_ROI_MODE_END_X_3,
	OP_ROI_MODE_END_X_4,
	OP_ROI_MODE_END_X_5,
	OP_ROI_MODE_END_X_6,
	OP_ROI_MODE_END_X_7,
	OP_ROI_MODE_END_X_8,

	OP_ROI_MODE_START_Y_1,
	OP_ROI_MODE_START_Y_2,
	OP_ROI_MODE_START_Y_3,
	OP_ROI_MODE_START_Y_4,
	OP_ROI_MODE_START_Y_5,
	OP_ROI_MODE_START_Y_6,
	OP_ROI_MODE_START_Y_7,
	OP_ROI_MODE_START_Y_8,

	OP_ROI_MODE_END_Y_1,
	OP_ROI_MODE_END_Y_2,
	OP_ROI_MODE_END_Y_3,
	OP_ROI_MODE_END_Y_4,
	OP_ROI_MODE_END_Y_5,
	OP_ROI_MODE_END_Y_6,
	OP_ROI_MODE_END_Y_7,
	OP_ROI_MODE_END_Y_8,

	OP_ROI_MODE_1,
	OP_ROI_MODE_2,
	OP_ROI_MODE_3,
	OP_ROI_MODE_4,
	OP_ROI_MODE_5,
	OP_ROI_MODE_6,
	OP_ROI_MODE_7,
	OP_ROI_MODE_8,

	OP_USE_MODE_MAP,
	OP_USE_MODE_MAPCon,
	OP_MODE_FLAG_FNAME,
#endif

#if USE_TRACE_DUMP
	OP_TRACE_DUMP,
#endif

#if HLS_RPL
    OP_FLAG_RPL0_0,
    OP_FLAG_RPL0_1,
    OP_FLAG_RPL0_2,
    OP_FLAG_RPL0_3,
    OP_FLAG_RPL0_4,
    OP_FLAG_RPL0_5,
    OP_FLAG_RPL0_6,
    OP_FLAG_RPL0_7,
    OP_FLAG_RPL0_8,
    OP_FLAG_RPL0_9,
    OP_FLAG_RPL0_10,
    OP_FLAG_RPL0_11,
    OP_FLAG_RPL0_12,
    OP_FLAG_RPL0_13,
    OP_FLAG_RPL0_14,
    OP_FLAG_RPL0_15,
    OP_FLAG_RPL0_16,
    OP_FLAG_RPL0_17,
    OP_FLAG_RPL0_18,
    OP_FLAG_RPL0_19,
    OP_FLAG_RPL0_20,
    //...
    OP_FLAG_RPL0_31,

    OP_FLAG_RPL1_0,
    OP_FLAG_RPL1_1,
    OP_FLAG_RPL1_2,
    OP_FLAG_RPL1_3,
    OP_FLAG_RPL1_4,
    OP_FLAG_RPL1_5,
    OP_FLAG_RPL1_6,
    OP_FLAG_RPL1_7,
    OP_FLAG_RPL1_8,
    OP_FLAG_RPL1_9,
    OP_FLAG_RPL1_10,
    OP_FLAG_RPL1_11,
    OP_FLAG_RPL1_12,
    OP_FLAG_RPL1_13,
    OP_FLAG_RPL1_14,
    OP_FLAG_RPL1_15,
    OP_FLAG_RPL1_16,
    OP_FLAG_RPL1_17,
    OP_FLAG_RPL1_18,
    OP_FLAG_RPL1_19,
    OP_FLAG_RPL1_20,
    //...
    OP_FLAG_RPL1_31,
#endif
#if LIBVC_ON
    OP_LIBPIC_ENABLE,
    OP_FLAG_QP_OFFSET_LIBPIC,
    OP_FLAG_FNAME_LIBOUT,
    OP_FLAG_FNAME_LIBREC,
    OP_FLAG_FNAME_LIBDATA,
    OP_FLAG_SKIP_FRAMES_WHEN_EXTRACT_LIBPIC,
    OP_FLAG_MAX_FRM_NUM_WHEN_EXTRACT_LIBPIC,
#endif
    OP_TEMP_SUB_RATIO,
    OP_CTU_SZE,
    OP_MIN_CU_SIZE,
    OP_MAX_PART_RATIO,
    OP_MAX_SPLIT_TIMES,
    OP_MIN_QT_SIZE,
    OP_MAX_BT_SIZE,
    OP_MAX_EQT_SIZE,
    OP_MAX_DT_SIZE,
    OP_FPS_NUM,
    OP_FPS_DEN,
    OP_RC_MIN_QP,
    OP_RC_MAX_QP,
    OP_RC_METHOD,
    OP_BITRATE,
    OP_VBV_MAX_RATE,
    OP_VBV_BUF_SIZE,
#if RATECONTROL
	OP_ENABLE_RATECONTROL,
	OP_TARGET_BITRATE,
	OP_RC_INITIALQP,
	OP_BLOCK_RATECONTROL,
	OP_PARAM_UPDATE,
	OP_INTRA_GROUP
#endif
    OP_FLAG_MAX
} OP_FLAGS;

static int op_flag[OP_FLAG_MAX] = {0};

static COM_ARGS_OPTION options[] =
{
    {
        COM_ARGS_NO_KEY, "config", ARGS_TYPE_STRING,
        &op_flag[OP_FLAG_FNAME_CFG], op_fname_cfg,
        "file name of configuration"
    },
    {
        'i', "input", ARGS_TYPE_STRING|ARGS_TYPE_MANDATORY,
        &op_flag[OP_FLAG_FNAME_INP], op_fname_inp,
        "file name of input video"
    },
    {
        'o', "output", ARGS_TYPE_STRING,
        &op_flag[OP_FLAG_FNAME_OUT], op_fname_out,
        "file name of output bitstream"
    },
    {
        'r', "recon", ARGS_TYPE_STRING,
        &op_flag[OP_FLAG_FNAME_REC], op_fname_rec,
        "file name of reconstructed video"
    },
    {
        'w',  "width", ARGS_TYPE_INTEGER|ARGS_TYPE_MANDATORY,
        &op_flag[OP_FLAG_WIDTH_INP], &op_w,
        "pixel width of input video"
    },
    {
        'h',  "height", ARGS_TYPE_INTEGER|ARGS_TYPE_MANDATORY,
        &op_flag[OP_FLAG_HEIGHT_INP], &op_h,
        "pixel height of input video"
    },
    {
        'q',  "op_qp", ARGS_TYPE_INTEGER,
        &op_flag[OP_FLAG_QP], &op_qp,
        "QP value (-16~63 for 8bit input, 0~79 for 10bit input)"
    },
    {
        'z',  "frame_rate", ARGS_TYPE_INTEGER|ARGS_TYPE_MANDATORY,
        &op_flag[OP_FLAG_FPS], &op_fps,
        "frame rate (Hz)"
    },
    {
        'p',  "i_period", ARGS_TYPE_INTEGER,
        &op_flag[OP_FLAG_IPERIOD], &op_i_period,
        "I-picture period"
    },
    {
        'g',  "max_b_frames", ARGS_TYPE_INTEGER,
        &op_flag[OP_FLAG_MAX_B_FRAMES], &op_max_b_frames,
        "Number of maximum B frames (1,3,7,15)\n"
    },
    {
        'f',  "frames", ARGS_TYPE_INTEGER,
        &op_flag[OP_FLAG_MAX_FRM_NUM], &op_max_frm_num,
        "maximum number of frames to be encoded"
    },
#if Debug422
    {
        'c',  "chroma_format", ARGS_TYPE_INTEGER,
        &op_flag[OP_FLAG_CHROMA_FORMAT], &op_chroma_format,
        "chroma format\n"
        "\t 0: 0=4:0:0\n"
        "\t 1: 1=4:2:0\n"
        "\t 2: 2=4:2:2\n"
    },
#endif
    {
        's',  "signature", ARGS_TYPE_INTEGER,
        &op_flag[OP_FLAG_USE_PIC_SIGN], &op_use_pic_signature,
        "embed picture signature (HASH) for conformance checking in decoding"
        "\t 0: disable\n"
        "\t 1: enable\n"
    },
    {
        'v',  "verbose", ARGS_TYPE_INTEGER,
        &op_flag[OP_FLAG_VERBOSE], &op_verbose,
        "verbose level\n"
        "\t 0: no message\n"
        "\t 1: frame-level messages (default)\n"
        "\t 2: all messages\n"
    },
    {
        'd',  "input_bit_depth", ARGS_TYPE_INTEGER,
        &op_flag[OP_FLAG_IN_BIT_DEPTH], &op_bit_depth_input,
        "input bitdepth (8(default), 10) "
    },
    {
        COM_ARGS_NO_KEY,  "internal_bit_depth", ARGS_TYPE_INTEGER,
        &op_flag[OP_FLAG_OUT_BIT_DEPTH], &op_bit_depth_internal,
        "output bitdepth (8, 10)(default: same as input bitdpeth) "
    },
    {
        COM_ARGS_NO_KEY,  "disable_hgop", ARGS_TYPE_INTEGER,
        &op_flag[OP_FLAG_DISABLE_HGOP], &op_disable_hgop,
        "disable hierarchical GOP. if not set, hierarchical GOP is used"
    },
    {
        COM_ARGS_NO_KEY,  "skip_frames", ARGS_TYPE_INTEGER,
        &op_flag[OP_FLAG_SKIP_FRAMES], &op_skip_frames,
        "number of skipped frames before encoding. default 0"
    },
    {
        COM_ARGS_NO_KEY,  "qp_add_frm", ARGS_TYPE_INTEGER,
        &op_flag[OP_FLAG_ADD_QP_FRAME], &op_frame_qp_add,
        "one more qp are added after this number of frames, disable:0 (default)"
    },
    {
        COM_ARGS_NO_KEY, "pb_frame_qp_offset", ARGS_TYPE_INTEGER,
        &op_flag[OP_FLAG_PB_FRAME_QP_OFFSET], &op_pb_frame_qp_offset,
        "add fixed delta qp to p/b frames, for IPPP structure, default 0"
    },
    {
        COM_ARGS_NO_KEY,  "qp_offset_cb", ARGS_TYPE_INTEGER,
        &op_flag[OP_FLAG_QP_OFFSET_CB], &op_qp_offset_cb,
        "qp offset for cb, disable:0 (default)"
    },

    {
        COM_ARGS_NO_KEY,  "qp_offset_cr", ARGS_TYPE_INTEGER,
        &op_flag[OP_FLAG_QP_OFFSET_CR], &op_qp_offset_cr,
        "qp offset for cr, disable:0 (default)"
    },
    {
        COM_ARGS_NO_KEY,  "qp_offset_adp", ARGS_TYPE_INTEGER,
        &op_flag[OP_FLAG_QP_OFFSET_ADP], &op_qp_offset_adp,
        "adaptive frame-level qp offset for cb and cr, disable:0, enable: 1(default)"
    },
    {
        COM_ARGS_NO_KEY,  "lcu_delta_qp", ARGS_TYPE_INTEGER,
        &op_flag[OP_FLAG_DELTA_QP], &op_delta_qp_flag,
        "Random qp for lcu, on/off flag"
    },
#if CUDQP_QP_MAP
		{
			COM_ARGS_NO_KEY, "cu_delta_qp", ARGS_TYPE_INTEGER,
			&op_flag[OP_FLAG_CU_DELTA_QP], &op_cu_delta_qp_flag,
			"contrast-based cu delta qp, on/off flag"
		},
	{
		COM_ARGS_NO_KEY, "cu_qp_group_size", ARGS_TYPE_INTEGER,
		&op_flag[OP_CU_QP_GROUP_SIZE], &op_cu_qp_group_size,
		"cu qp group size (i.e., 8, 16, 32, 64), default 16"
	},
#endif
    {
        COM_ARGS_NO_KEY,  "ipcm", ARGS_TYPE_INTEGER,
        &op_flag[OP_TOOL_IPCM], &op_tool_ipcm,
        "ipcm on/off flag"
    },
    {
        COM_ARGS_NO_KEY,  "amvr", ARGS_TYPE_INTEGER,
        &op_flag[OP_TOOL_AMVR], &op_tool_amvr,
        "amvr on/off flag"
    },
    {
        COM_ARGS_NO_KEY,  "ipf", ARGS_TYPE_INTEGER,
        &op_flag[OP_TOOL_IPF], &op_tool_ipf,
        "Intra prediction filter on/off flag"
    },

    {
        COM_ARGS_NO_KEY, "wq_enable", ARGS_TYPE_INTEGER,
        &op_flag[OP_WQ_ENABLE], &op_wq_enable,
        "Weight Quant on/off, disable: 0(default), enable: 1"
    },
    {
        COM_ARGS_NO_KEY, "seq_wq_mode", ARGS_TYPE_INTEGER,
        &op_flag[OP_SEQ_WQ_MODE], &op_seq_wq_mode,
        "Seq Weight Quant (0: default, 1: User Define)"
    },
    {
        COM_ARGS_NO_KEY, "seq_wq_user", ARGS_TYPE_STRING,
        &op_flag[OP_SEQ_WQ_FILE], &op_seq_wq_user,
        "User Defined Seq WQ Matrix"
    },
    {
        COM_ARGS_NO_KEY, "pic_wq_data_idx", ARGS_TYPE_INTEGER,
        &op_flag[OP_PIC_WQ_DATA_IDX], &op_pic_wq_data_idx,
        "Pic level WQ data index, 0(default):refer to seq_header,  1:derived by WQ parameter,  2:load from pic_header"
    },
    {
        COM_ARGS_NO_KEY, "pic_wq_user", ARGS_TYPE_STRING,
        &op_flag[OP_PIC_WQ_FILE], &op_pic_wq_user,
        "User Defined Pic WQ Matrix"
    },
    {
        COM_ARGS_NO_KEY, "wq_param", ARGS_TYPE_INTEGER,
        &op_flag[OP_WQ_PARAM], &op_wq_param,
        "WQ Param (0=Default, 1=UnDetailed, 2=Detailed)"
    },
    {
        COM_ARGS_NO_KEY, "wq_model", ARGS_TYPE_INTEGER,
        &op_flag[OP_WQ_PARAM_MODEL], &op_wq_model,
        "WQ Model (0~2, default:0)"
    },
    {
        COM_ARGS_NO_KEY, "wq_param_detailed", ARGS_TYPE_STRING,
        &op_flag[OP_WQ_PARAM_DETAILED], &op_wq_param_detailed,
        "default:[64,49,53,58,58,64]"
    },
    {
        COM_ARGS_NO_KEY, "wq_param_undetailed", ARGS_TYPE_STRING,
        &op_flag[OP_WQ_PARAM_UNDETAILED], &op_wq_param_undetailed,
        "default:[67,71,71,80,80,106]"
    },
    {
        COM_ARGS_NO_KEY,  "hmvp", ARGS_TYPE_INTEGER,
        &op_flag[OP_TOOL_HMVP], &op_tool_hmvp,
        "number of hmvp skip candidates (default: 8, disable: 0)"
    },
    {
        COM_ARGS_NO_KEY,  "tscpm", ARGS_TYPE_INTEGER,
        &op_flag[OP_TOOL_TSCPM], &op_tool_tscpm,
        "tscpm on/off flag"
    },
    {
        COM_ARGS_NO_KEY,  "umve", ARGS_TYPE_INTEGER,
        &op_flag[OP_TOOL_UMVE], &op_tool_umve,
        "umve on/off flag"
    },
    {
        COM_ARGS_NO_KEY,  "emvr", ARGS_TYPE_INTEGER,
        &op_flag[OP_TOOL_EMVR], &op_tool_emvr,
        "emvr on/off flag"
    },
    {
        COM_ARGS_NO_KEY,  "affine", ARGS_TYPE_INTEGER,
        &op_flag[OP_TOOL_AFFINE], &op_tool_affine,
        "affine on/off flag"
    },
    {
        COM_ARGS_NO_KEY,  "smvd", ARGS_TYPE_INTEGER,
        &op_flag[OP_TOOL_SMVD], &op_tool_smvd,
        "smvd on/off flag (on: 1, off: 0, default: 1)"
    },
    {
        COM_ARGS_NO_KEY,  "dt_intra", ARGS_TYPE_INTEGER,
        &op_flag[OP_TOOL_DT_INTRA], &op_tool_dt_intra,
        "dt_intra on/off flag (on: 1, off: 0, default: 1)"
    },
    {
        COM_ARGS_NO_KEY,  "pbt", ARGS_TYPE_INTEGER,
        &op_flag[OP_TOOL_PBT], &op_tool_pbt,
        "pbt on/off flag (on: 1, off: 0, default: 1)"
    },
    {
        COM_ARGS_NO_KEY, "deblock", ARGS_TYPE_INTEGER,
        &op_flag[OP_TOOL_DEBLOCK], &op_tool_deblock,
        "deblock on/off flag (on: 1, off: 0, default: 1)"
    },
    {
        COM_ARGS_NO_KEY, "sao", ARGS_TYPE_INTEGER,
        &op_flag[OP_TOOL_SAO], &op_tool_sao,
        "sao on/off flag (on: 1, off: 0, default: 1)"
    },
    {
        COM_ARGS_NO_KEY, "alf", ARGS_TYPE_INTEGER,
        &op_flag[OP_TOOL_ALF], &op_tool_alf,
        "alf on/off flag (on: 1, off: 0, default: 1)"
    },
#if PATCH
    {
        COM_ARGS_NO_KEY, "patch_stable", ARGS_TYPE_INTEGER,
        &op_flag[OP_PATCH_STABLE], &op_patch_stable,
        "stable_patch_flag(0:all the pic as the same patch size;1:each pic as the different patch size)"
    },
    {
        COM_ARGS_NO_KEY, "cross_patch_loopfilter", ARGS_TYPE_INTEGER,
        &op_flag[OP_CROSS_PATCH_LOOP_FILTER], &op_cross_patch_loop_filter,
        "loop_filter_across_patch_flag(1:cross;0:non cross)"
    },
    {
        COM_ARGS_NO_KEY, "patch_ref_colocated", ARGS_TYPE_INTEGER,
        &op_flag[OP_PATCH_REF_COLOCATED], &op_patch_ref_colocated,
        "if the MV out of the patch boundary,0:non use colocated;1:use colocated"
    },
    {
        COM_ARGS_NO_KEY, "patch_uniform", ARGS_TYPE_INTEGER,
        &op_flag[OP_PATCH_UNIFORM], &op_patch_uniform,
        "0:all the patch are in the same size;1:each patch in the different size"
    },
    {
        COM_ARGS_NO_KEY, "patch_width_in_lcu", ARGS_TYPE_INTEGER,
        &op_flag[OP_PATCH_WIDTH], &op_patch_width_in_lcu,
        "when uniform is 1,the nember of LCU in horizon in a patch"
    },
    {
        COM_ARGS_NO_KEY, "patch_height_in_lcu", ARGS_TYPE_INTEGER,
        &op_flag[OP_PATCH_HEIGHT], &op_patch_height_in_lcu,
        "when uniform is 1,the nember of LCU in vertical in a patch"
    },
    {
        COM_ARGS_NO_KEY, "multislice_mode", ARGS_TYPE_INTEGER,
        &op_flag[OP_MULTISLICE_MODE], &op_multislice_mode,
        "0: multislice off; 1: HOR_BI; 2:VER_BI; 3:QUARD; 4~9(1+slice_num):CUSTOM_HOR; 10~15(7+slice_num):CUSTOM_VER;in custom mode, 3<= slice_num <=8;)"
    },
#if USE_INTRA_REFRESH
	{
		COM_ARGS_NO_KEY, "IntraRefreshControl", ARGS_TYPE_INTEGER,
		&op_flag[OP_INTRA_REFRESH_CONTROL], &op_intra_refresh_control,
		"0: off; 1: on"
	},
	{
		COM_ARGS_NO_KEY, "IntraRefreshSize", ARGS_TYPE_INTEGER,
		&op_flag[OP_INTRA_REFRESH_SIZE], &op_intra_refresh_size,
		"1: 32"
	},
	{
		COM_ARGS_NO_KEY, "IntraRefreshMode", ARGS_TYPE_INTEGER,
		&op_flag[OP_INTRA_REFRESH_MODE], &op_intra_refresh_mode,
		"0: row; 1: col"
	},
#endif
#if USE_SPEED_LEVEL
	{
		COM_ARGS_NO_KEY, "SpeedLevel", ARGS_TYPE_INTEGER,
		&op_flag[OP_SPEED_LEVEL], &op_speed_level,
		"1: 4K60; 2: 4K30; 3: 1080P60"
	},
#endif

#if USE_ROI_QP_CONFIG
	//************************ROI Area 1 ********************************
	{
		COM_ARGS_NO_KEY, "ROI_QP_Coding1", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_QP_ENABLE_1], &op_roi_qp_enable_1,
		"0: close; 1: open"
	},
	{
		COM_ARGS_NO_KEY, "ROI_QP_Start_x1", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_QP_START_X_1], &op_roi_qp_start_x_1,
		"0~pic_width-64"
	},
	{
		COM_ARGS_NO_KEY, "ROI_QP_End_x1", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_QP_END_X_1], &op_roi_qp_end_x_1,
		"64~pic_width-1"
	},
	{
		COM_ARGS_NO_KEY, "ROI_QP_Start_y1", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_QP_START_Y_1], &op_roi_qp_start_y_1,
		"0~pic_width-64"
	},
	{
		COM_ARGS_NO_KEY, "ROI_QP_End_y1", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_QP_END_Y_1], &op_roi_qp_end_y_1,
		"64~pic_width-1"
	},
	{
		COM_ARGS_NO_KEY, "ROI_QP1", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_QP_1], &op_roi_qp_1,
		"0~63"
	},

	//************************ROI Area 2 ********************************
	{
		COM_ARGS_NO_KEY, "ROI_QP_Coding2", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_QP_ENABLE_2], &op_roi_qp_enable_2,
		"0: close; 1: open"
	},
	{
		COM_ARGS_NO_KEY, "ROI_QP_Start_x2", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_QP_START_X_2], &op_roi_qp_start_x_2,
		"0~pic_width-64"
	},
	{
		COM_ARGS_NO_KEY, "ROI_QP_End_x2", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_QP_END_X_2], &op_roi_qp_end_x_2,
		"64~pic_width-1"
	},
	{
		COM_ARGS_NO_KEY, "ROI_QP_Start_y2", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_QP_START_Y_2], &op_roi_qp_start_y_2,
		"0~pic_width-64"
	},
	{
		COM_ARGS_NO_KEY, "ROI_QP_End_y2", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_QP_END_Y_2], &op_roi_qp_end_y_2,
		"64~pic_width-1"
	},
	{
		COM_ARGS_NO_KEY, "ROI_QP2", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_QP_2], &op_roi_qp_2,
		"0~63"
	},

	//************************ROI Area 3 ********************************
	{
		COM_ARGS_NO_KEY, "ROI_QP_Coding3", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_QP_ENABLE_3], &op_roi_qp_enable_3,
		"0: close; 1: open"
	},
	{
		COM_ARGS_NO_KEY, "ROI_QP_Start_x3", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_QP_START_X_3], &op_roi_qp_start_x_3,
		"0~pic_width-64"
	},
	{
		COM_ARGS_NO_KEY, "ROI_QP_End_x3", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_QP_END_X_3], &op_roi_qp_end_x_3,
		"64~pic_width-1"
	},
	{
		COM_ARGS_NO_KEY, "ROI_QP_Start_y3", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_QP_START_Y_3], &op_roi_qp_start_y_3,
		"0~pic_width-64"
	},
	{
		COM_ARGS_NO_KEY, "ROI_QP_End_y3", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_QP_END_Y_3], &op_roi_qp_end_y_3,
		"64~pic_width-1"
	},
	{
		COM_ARGS_NO_KEY, "ROI_QP3", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_QP_3], &op_roi_qp_3,
		"0~63"
	},

	//************************ROI Area 4 ********************************
	{
		COM_ARGS_NO_KEY, "ROI_QP_Coding4", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_QP_ENABLE_4], &op_roi_qp_enable_4,
		"0: close; 1: open"
	},
	{
		COM_ARGS_NO_KEY, "ROI_QP_Start_x4", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_QP_START_X_4], &op_roi_qp_start_x_4,
		"0~pic_width-64"
	},
	{
		COM_ARGS_NO_KEY, "ROI_QP_End_x4", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_QP_END_X_4], &op_roi_qp_end_x_4,
		"64~pic_width-1"
	},
	{
		COM_ARGS_NO_KEY, "ROI_QP_Start_y4", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_QP_START_Y_4], &op_roi_qp_start_y_4,
		"0~pic_width-64"
	},
	{
		COM_ARGS_NO_KEY, "ROI_QP_End_y4", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_QP_END_Y_4], &op_roi_qp_end_y_4,
		"64~pic_width-1"
	},
	{
		COM_ARGS_NO_KEY, "ROI_QP4", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_QP_4], &op_roi_qp_4,
		"0~63"
	},

	//************************ROI Area 5 ********************************
	{
		COM_ARGS_NO_KEY, "ROI_QP_Coding5", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_QP_ENABLE_5], &op_roi_qp_enable_5,
		"0: close; 1: open"
	},
	{
		COM_ARGS_NO_KEY, "ROI_QP_Start_x5", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_QP_START_X_5], &op_roi_qp_start_x_5,
		"0~pic_width-64"
	},
	{
		COM_ARGS_NO_KEY, "ROI_QP_End_x5", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_QP_END_X_5], &op_roi_qp_end_x_5,
		"64~pic_width-1"
	},
	{
		COM_ARGS_NO_KEY, "ROI_QP_Start_y5", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_QP_START_Y_5], &op_roi_qp_start_y_5,
		"0~pic_width-64"
	},
	{
		COM_ARGS_NO_KEY, "ROI_QP_End_y5", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_QP_END_Y_5], &op_roi_qp_end_y_5,
		"64~pic_width-1"
	},
	{
		COM_ARGS_NO_KEY, "ROI_QP5", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_QP_5], &op_roi_qp_5,
		"0~63"
	},

	//************************ROI Area 6 ********************************
	{
		COM_ARGS_NO_KEY, "ROI_QP_Coding6", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_QP_ENABLE_6], &op_roi_qp_enable_6,
		"0: close; 1: open"
	},
	{
		COM_ARGS_NO_KEY, "ROI_QP_Start_x6", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_QP_START_X_6], &op_roi_qp_start_x_6,
		"0~pic_width-64"
	},
	{
		COM_ARGS_NO_KEY, "ROI_QP_End_x6", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_QP_END_X_6], &op_roi_qp_end_x_6,
		"64~pic_width-1"
	},
	{
		COM_ARGS_NO_KEY, "ROI_QP_Start_y6", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_QP_START_Y_6], &op_roi_qp_start_y_6,
		"0~pic_width-64"
	},
	{
		COM_ARGS_NO_KEY, "ROI_QP_End_y6", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_QP_END_Y_6], &op_roi_qp_end_y_6,
		"64~pic_width-1"
	},
	{
		COM_ARGS_NO_KEY, "ROI_QP6", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_QP_6], &op_roi_qp_6,
		"0~63"
	},

	//************************ROI Area 7 ********************************
	{
		COM_ARGS_NO_KEY, "ROI_QP_Coding7", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_QP_ENABLE_7], &op_roi_qp_enable_7,
		"0: close; 1: open"
	},
	{
		COM_ARGS_NO_KEY, "ROI_QP_Start_x7", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_QP_START_X_7], &op_roi_qp_start_x_7,
		"0~pic_width-64"
	},
	{
		COM_ARGS_NO_KEY, "ROI_QP_End_x7", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_QP_END_X_7], &op_roi_qp_end_x_7,
		"64~pic_width-1"
	},
	{
		COM_ARGS_NO_KEY, "ROI_QP_Start_y7", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_QP_START_Y_7], &op_roi_qp_start_y_7,
		"0~pic_width-64"
	},
	{
		COM_ARGS_NO_KEY, "ROI_QP_End_y7", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_QP_END_Y_7], &op_roi_qp_end_y_7,
		"64~pic_width-1"
	},
	{
		COM_ARGS_NO_KEY, "ROI_QP7", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_QP_7], &op_roi_qp_7,
		"0~63"
	},

	//************************ROI Area 8 ********************************
	{
		COM_ARGS_NO_KEY, "ROI_QP_Coding8", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_QP_ENABLE_8], &op_roi_qp_enable_8,
		"0: close; 1: open"
	},
	{
		COM_ARGS_NO_KEY, "ROI_QP_Start_x8", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_QP_START_X_8], &op_roi_qp_start_x_8,
		"0~pic_width-64"
	},
	{
		COM_ARGS_NO_KEY, "ROI_QP_End_x8", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_QP_END_X_8], &op_roi_qp_end_x_8,
		"64~pic_width-1"
	},
	{
		COM_ARGS_NO_KEY, "ROI_QP_Start_y8", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_QP_START_Y_8], &op_roi_qp_start_y_8,
		"0~pic_width-64"
	},
	{
		COM_ARGS_NO_KEY, "ROI_QP_End_y8", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_QP_END_Y_8], &op_roi_qp_end_y_8,
		"64~pic_width-1"
	},
	{
		COM_ARGS_NO_KEY, "ROI_QP8", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_QP_8], &op_roi_qp_8,
		"0~63"
	},

	{
		COM_ARGS_NO_KEY, "useQPMAP", ARGS_TYPE_INTEGER,
		&op_flag[OP_USE_QP_MAP], &op_useQPMAP,
		"0,1"
	},

	{
		COM_ARGS_NO_KEY, "useQPMapCon", ARGS_TYPE_INTEGER,
		&op_flag[OP_USE_QP_MAPCon], &op_useQPMAPCon,
		"0,1"
	},

	{
		'm', "QPMapFile", ARGS_TYPE_STRING,
		&op_flag[OP_QPMAP_FLAG_FNAME], op_fname_qpmap,
		"file name of QPMapFile"
	},
#endif

#if USE_ROI_MODE_CONFIG
			//************************ROI Area 1 ********************************
	{
		COM_ARGS_NO_KEY, "ROI_MODE_Coding1", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_MODE_ENABLE_1], &op_roi_mode_enable_1,
		"0: close; 1: open"
	},
	{
		COM_ARGS_NO_KEY, "ROI_MODE_Start_x1", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_MODE_START_X_1], &op_roi_mode_start_x_1,
		"0~pic_width-64"
	},
	{
		COM_ARGS_NO_KEY, "ROI_MODE_End_x1", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_MODE_END_X_1], &op_roi_mode_end_x_1,
		"64~pic_width-1"
	},
	{
		COM_ARGS_NO_KEY, "ROI_MODE_Start_y1", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_MODE_START_Y_1], &op_roi_mode_start_y_1,
		"0~pic_width-64"
	},
	{
		COM_ARGS_NO_KEY, "ROI_MODE_End_y1", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_MODE_END_Y_1], &op_roi_mode_end_y_1,
		"64~pic_width-1"
	},
	{
		COM_ARGS_NO_KEY, "ROI_MODE1", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_MODE_1], &op_roi_mode_1,
		"0~2"
	},

			//************************ROI Area 2 ********************************
	{
		COM_ARGS_NO_KEY, "ROI_MODE_Coding2", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_MODE_ENABLE_2], &op_roi_mode_enable_2,
		"0: close; 1: open"
	},
	{
		COM_ARGS_NO_KEY, "ROI_MODE_Start_x2", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_MODE_START_X_2], &op_roi_mode_start_x_2,
		"0~pic_width-64"
	},
	{
		COM_ARGS_NO_KEY, "ROI_MODE_End_x2", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_MODE_END_X_2], &op_roi_mode_end_x_2,
		"64~pic_width-1"
	},
	{
		COM_ARGS_NO_KEY, "ROI_MODE_Start_y2", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_MODE_START_Y_2], &op_roi_mode_start_y_2,
		"0~pic_width-64"
	},
	{
		COM_ARGS_NO_KEY, "ROI_MODE_End_y2", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_MODE_END_Y_2], &op_roi_mode_end_y_2,
		"64~pic_width-1"
	},
	{
		COM_ARGS_NO_KEY, "ROI_MODE2", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_MODE_2], &op_roi_mode_2,
		"0~2"
	},

			//************************ROI Area 3 ********************************
	{
		COM_ARGS_NO_KEY, "ROI_MODE_Coding3", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_MODE_ENABLE_3], &op_roi_mode_enable_3,
		"0: close; 1: open"
	},
	{
		COM_ARGS_NO_KEY, "ROI_MODE_Start_x3", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_MODE_START_X_3], &op_roi_mode_start_x_3,
		"0~pic_width-64"
	},
	{
		COM_ARGS_NO_KEY, "ROI_MODE_End_x3", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_MODE_END_X_3], &op_roi_mode_end_x_3,
		"64~pic_width-1"
	},
	{
		COM_ARGS_NO_KEY, "ROI_MODE_Start_y3", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_MODE_START_Y_3], &op_roi_mode_start_y_3,
		"0~pic_width-64"
	},
	{
		COM_ARGS_NO_KEY, "ROI_MODE_End_y3", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_MODE_END_Y_3], &op_roi_mode_end_y_3,
		"64~pic_width-1"
	},
	{
		COM_ARGS_NO_KEY, "ROI_MODE3", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_MODE_3], &op_roi_mode_3,
		"0~2"
	},

			//************************ROI Area 4 ********************************
	{
		COM_ARGS_NO_KEY, "ROI_MODE_Coding4", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_MODE_ENABLE_4], &op_roi_mode_enable_4,
		"0: close; 1: open"
	},
	{
		COM_ARGS_NO_KEY, "ROI_MODE_Start_x4", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_MODE_START_X_4], &op_roi_mode_start_x_4,
		"0~pic_width-64"
	},
	{
		COM_ARGS_NO_KEY, "ROI_MODE_End_x4", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_MODE_END_X_4], &op_roi_mode_end_x_4,
		"64~pic_width-1"
	},
	{
		COM_ARGS_NO_KEY, "ROI_MODE_Start_y4", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_MODE_START_Y_4], &op_roi_mode_start_y_4,
		"0~pic_width-64"
	},
	{
		COM_ARGS_NO_KEY, "ROI_MODE_End_y4", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_MODE_END_Y_4], &op_roi_mode_end_y_4,
		"64~pic_width-1"
	},
	{
		COM_ARGS_NO_KEY, "ROI_MODE4", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_MODE_4], &op_roi_mode_4,
		"0~2"
	},

			//************************ROI Area 5 ********************************
	{
		COM_ARGS_NO_KEY, "ROI_MODE_Coding5", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_MODE_ENABLE_5], &op_roi_mode_enable_5,
		"0: close; 1: open"
	},
	{
		COM_ARGS_NO_KEY, "ROI_MODE_Start_x5", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_MODE_START_X_5], &op_roi_mode_start_x_5,
		"0~pic_width-64"
	},
	{
		COM_ARGS_NO_KEY, "ROI_MODE_End_x5", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_MODE_END_X_5], &op_roi_mode_end_x_5,
		"64~pic_width-1"
	},
	{
		COM_ARGS_NO_KEY, "ROI_MODE_Start_y5", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_MODE_START_Y_5], &op_roi_mode_start_y_5,
		"0~pic_width-64"
	},
	{
		COM_ARGS_NO_KEY, "ROI_MODE_End_y5", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_MODE_END_Y_5], &op_roi_mode_end_y_5,
		"64~pic_width-1"
	},
	{
		COM_ARGS_NO_KEY, "ROI_MODE5", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_MODE_5], &op_roi_mode_5,
		"0~2"
	},

			//************************ROI Area 6 ********************************
	{
		COM_ARGS_NO_KEY, "ROI_MODE_Coding6", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_MODE_ENABLE_6], &op_roi_mode_enable_6,
		"0: close; 1: open"
	},
	{
		COM_ARGS_NO_KEY, "ROI_MODE_Start_x6", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_MODE_START_X_6], &op_roi_mode_start_x_6,
		"0~pic_width-64"
	},
	{
		COM_ARGS_NO_KEY, "ROI_MODE_End_x6", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_MODE_END_X_6], &op_roi_mode_end_x_6,
		"64~pic_width-1"
	},
	{
		COM_ARGS_NO_KEY, "ROI_MODE_Start_y6", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_MODE_START_Y_6], &op_roi_mode_start_y_6,
		"0~pic_width-64"
	},
	{
		COM_ARGS_NO_KEY, "ROI_MODE_End_y6", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_MODE_END_Y_6], &op_roi_mode_end_y_6,
		"64~pic_width-1"
	},
	{
		COM_ARGS_NO_KEY, "ROI_MODE6", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_MODE_6], &op_roi_mode_6,
		"0~2"
	},

			//************************ROI Area 7 ********************************
	{
		COM_ARGS_NO_KEY, "ROI_MODE_Coding7", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_MODE_ENABLE_7], &op_roi_mode_enable_7,
		"0: close; 1: open"
	},
	{
		COM_ARGS_NO_KEY, "ROI_MODE_Start_x7", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_MODE_START_X_7], &op_roi_mode_start_x_7,
		"0~pic_width-64"
	},
	{
		COM_ARGS_NO_KEY, "ROI_MODE_End_x7", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_MODE_END_X_7], &op_roi_mode_end_x_7,
		"64~pic_width-1"
	},
	{
		COM_ARGS_NO_KEY, "ROI_MODE_Start_y7", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_MODE_START_Y_7], &op_roi_mode_start_y_7,
		"0~pic_width-64"
	},
	{
		COM_ARGS_NO_KEY, "ROI_MODE_End_y7", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_MODE_END_Y_7], &op_roi_mode_end_y_7,
		"64~pic_width-1"
	},
	{
		COM_ARGS_NO_KEY, "ROI_MODE7", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_MODE_7], &op_roi_mode_7,
		"0~2"
	},

			//************************ROI Area 8 ********************************
	{
		COM_ARGS_NO_KEY, "ROI_MODE_Coding8", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_MODE_ENABLE_8], &op_roi_mode_enable_8,
		"0: close; 1: open"
	},
	{
		COM_ARGS_NO_KEY, "ROI_MODE_Start_x8", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_MODE_START_X_8], &op_roi_mode_start_x_8,
		"0~pic_width-64"
	},
	{
		COM_ARGS_NO_KEY, "ROI_MODE_End_x8", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_MODE_END_X_8], &op_roi_mode_end_x_8,
		"64~pic_width-1"
	},
	{
		COM_ARGS_NO_KEY, "ROI_MODE_Start_y8", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_MODE_START_Y_8], &op_roi_mode_start_y_8,
		"0~pic_width-64"
	},
	{
		COM_ARGS_NO_KEY, "ROI_MODE_End_y8", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_MODE_END_Y_8], &op_roi_mode_end_y_8,
		"64~pic_width-1"
	},
	{
		COM_ARGS_NO_KEY, "ROI_MODE8", ARGS_TYPE_INTEGER,
		&op_flag[OP_ROI_MODE_8], &op_roi_mode_8,
		"0~2"
	},

	{
		COM_ARGS_NO_KEY, "useModeMAP", ARGS_TYPE_INTEGER,
		&op_flag[OP_USE_MODE_MAP], &op_useModeMAP,
		"0,1"
	},

	{
		COM_ARGS_NO_KEY, "useModeMapCon", ARGS_TYPE_INTEGER,
		&op_flag[OP_USE_MODE_MAPCon], &op_useModeMAPCon,
		"0,1"
	},

	{
		'n', "ModeMapFile", ARGS_TYPE_STRING,
		&op_flag[OP_MODE_FLAG_FNAME], op_fname_modemap,
		"file name of ModeMapFile"
	},
#endif
#if CUDQP_QP_MAP
    {
        COM_ARGS_NO_KEY, "useCuQPMAP", ARGS_TYPE_INTEGER,
        &op_flag[OP_USE_CU_QP_MAP], &op_useCuQPMAP,
        "0,1"
    },

    {
        'm', "CuQPMapFile", ARGS_TYPE_STRING,
        &op_flag[OP_CUQPMAP_FLAG_FNAME], op_cu_qpmap,
        "file name of CuQPMapFile"
    },
#endif
#if USE_TRACE_DUMP
	{
		COM_ARGS_NO_KEY, "useTraceDump", ARGS_TYPE_INTEGER,
		&op_flag[OP_TRACE_DUMP], &op_useTraceDump,
		"0,1"
	},
#endif

#endif
#if HLS_RPL
    {
        COM_ARGS_NO_KEY,  "RPL0_0", ARGS_TYPE_STRING,
        &op_flag[OP_FLAG_RPL0_0], &op_rpl0[0],
        "RPL0_0"
    },
    {
        COM_ARGS_NO_KEY,  "RPL0_1", ARGS_TYPE_STRING,
        &op_flag[OP_FLAG_RPL0_1], &op_rpl0[1],
        "RPL0_1"
    },
    {
        COM_ARGS_NO_KEY,  "RPL0_2", ARGS_TYPE_STRING,
        &op_flag[OP_FLAG_RPL0_2], &op_rpl0[2],
        "RPL0_2"
    },
    {
        COM_ARGS_NO_KEY,  "RPL0_3", ARGS_TYPE_STRING,
        &op_flag[OP_FLAG_RPL0_3], &op_rpl0[3],
        "RPL0_3"
    },
    {
        COM_ARGS_NO_KEY,  "RPL0_4", ARGS_TYPE_STRING,
        &op_flag[OP_FLAG_RPL0_4], &op_rpl0[4],
        "RPL0_4"
    },
    {
        COM_ARGS_NO_KEY,  "RPL0_5", ARGS_TYPE_STRING,
        &op_flag[OP_FLAG_RPL0_5], &op_rpl0[5],
        "RPL0_5"
    },
    {
        COM_ARGS_NO_KEY,  "RPL0_6", ARGS_TYPE_STRING,
        &op_flag[OP_FLAG_RPL0_6], &op_rpl0[6],
        "RPL0_6"
    },
    {
        COM_ARGS_NO_KEY,  "RPL0_7", ARGS_TYPE_STRING,
        &op_flag[OP_FLAG_RPL0_7], &op_rpl0[7],
        "RPL0_7"
    },
    {
        COM_ARGS_NO_KEY,  "RPL0_8", ARGS_TYPE_STRING,
        &op_flag[OP_FLAG_RPL0_8], &op_rpl0[8],
        "RPL0_8"
    },
    {
        COM_ARGS_NO_KEY,  "RPL0_9", ARGS_TYPE_STRING,
        &op_flag[OP_FLAG_RPL0_9], &op_rpl0[9],
        "RPL0_9"
    },
    {
        COM_ARGS_NO_KEY,  "RPL0_10", ARGS_TYPE_STRING,
        &op_flag[OP_FLAG_RPL0_10], &op_rpl0[10],
        "RPL0_10"
    },
    {
        COM_ARGS_NO_KEY,  "RPL0_11", ARGS_TYPE_STRING,
        &op_flag[OP_FLAG_RPL0_11], &op_rpl0[11],
        "RPL0_11"
    },
    {
        COM_ARGS_NO_KEY,  "RPL0_12", ARGS_TYPE_STRING,
        &op_flag[OP_FLAG_RPL0_12], &op_rpl0[12],
        "RPL0_12"
    },
    {
        COM_ARGS_NO_KEY,  "RPL0_13", ARGS_TYPE_STRING,
        &op_flag[OP_FLAG_RPL0_13], &op_rpl0[13],
        "RPL0_13"
    },
    {
        COM_ARGS_NO_KEY,  "RPL0_14", ARGS_TYPE_STRING,
        &op_flag[OP_FLAG_RPL0_14], &op_rpl0[14],
        "RPL0_14"
    },
    {
        COM_ARGS_NO_KEY,  "RPL0_15", ARGS_TYPE_STRING,
        &op_flag[OP_FLAG_RPL0_15], &op_rpl0[15],
        "RPL0_15"
    },
    {
        COM_ARGS_NO_KEY,  "RPL0_16", ARGS_TYPE_STRING,
        &op_flag[OP_FLAG_RPL0_16], &op_rpl0[16],
        "RPL0_16"
    },
    {
        COM_ARGS_NO_KEY,  "RPL0_17", ARGS_TYPE_STRING,
        &op_flag[OP_FLAG_RPL0_17], &op_rpl0[17],
        "RPL0_17"
    },
    {
        COM_ARGS_NO_KEY,  "RPL0_18", ARGS_TYPE_STRING,
        &op_flag[OP_FLAG_RPL0_18], &op_rpl0[18],
        "RPL0_18"
    },
    {
        COM_ARGS_NO_KEY,  "RPL0_19", ARGS_TYPE_STRING,
        &op_flag[OP_FLAG_RPL0_19], &op_rpl0[19],
        "RPL0_19"
    },
    {
        COM_ARGS_NO_KEY,  "RPL0_20", ARGS_TYPE_STRING,
        &op_flag[OP_FLAG_RPL0_20], &op_rpl0[20],
        "RPL0_20"
    },

    {
        COM_ARGS_NO_KEY,  "RPL1_0", ARGS_TYPE_STRING,
        &op_flag[OP_FLAG_RPL1_0], &op_rpl1[0],
        "RPL1_0"
    },

    {
        COM_ARGS_NO_KEY,  "RPL1_1", ARGS_TYPE_STRING,
        &op_flag[OP_FLAG_RPL1_1], &op_rpl1[1],
        "RPL1_1"
    },
    {
        COM_ARGS_NO_KEY,  "RPL1_2", ARGS_TYPE_STRING,
        &op_flag[OP_FLAG_RPL1_2], &op_rpl1[2],
        "RPL1_2"
    },
    {
        COM_ARGS_NO_KEY,  "RPL1_3", ARGS_TYPE_STRING,
        &op_flag[OP_FLAG_RPL1_3], &op_rpl1[3],
        "RPL1_3"
    },
    {
        COM_ARGS_NO_KEY,  "RPL1_4", ARGS_TYPE_STRING,
        &op_flag[OP_FLAG_RPL1_4], &op_rpl1[4],
        "RPL1_4"
    },
    {
        COM_ARGS_NO_KEY,  "RPL1_5", ARGS_TYPE_STRING,
        &op_flag[OP_FLAG_RPL1_5], &op_rpl1[5],
        "RPL1_5"
    },
    {
        COM_ARGS_NO_KEY,  "RPL1_6", ARGS_TYPE_STRING,
        &op_flag[OP_FLAG_RPL1_6], &op_rpl1[6],
        "RPL1_6"
    },
    {
        COM_ARGS_NO_KEY,  "RPL1_7", ARGS_TYPE_STRING,
        &op_flag[OP_FLAG_RPL1_7], &op_rpl1[7],
        "RPL1_7"
    },
    {
        COM_ARGS_NO_KEY,  "RPL1_8", ARGS_TYPE_STRING,
        &op_flag[OP_FLAG_RPL1_8], &op_rpl1[8],
        "RPL1_8"
    },
    {
        COM_ARGS_NO_KEY,  "RPL1_9", ARGS_TYPE_STRING,
        &op_flag[OP_FLAG_RPL1_9], &op_rpl1[9],
        "RPL1_9"
    },
    {
        COM_ARGS_NO_KEY,  "RPL1_10", ARGS_TYPE_STRING,
        &op_flag[OP_FLAG_RPL1_10], &op_rpl1[10],
        "RPL1_10"
    },
    {
        COM_ARGS_NO_KEY,  "RPL1_11", ARGS_TYPE_STRING,
        &op_flag[OP_FLAG_RPL1_11], &op_rpl1[11],
        "RPL1_11"
    },
    {
        COM_ARGS_NO_KEY,  "RPL1_12", ARGS_TYPE_STRING,
        &op_flag[OP_FLAG_RPL1_12], &op_rpl1[12],
        "RPL1_12"
    },
    {
        COM_ARGS_NO_KEY,  "RPL1_13", ARGS_TYPE_STRING,
        &op_flag[OP_FLAG_RPL1_13], &op_rpl1[13],
        "RPL1_13"
    },
    {
        COM_ARGS_NO_KEY,  "RPL1_14", ARGS_TYPE_STRING,
        &op_flag[OP_FLAG_RPL1_14], &op_rpl1[14],
        "RPL1_14"
    },
    {
        COM_ARGS_NO_KEY,  "RPL1_15", ARGS_TYPE_STRING,
        &op_flag[OP_FLAG_RPL1_15], &op_rpl1[15],
        "RPL1_15"
    },
    {
        COM_ARGS_NO_KEY,  "RPL1_16", ARGS_TYPE_STRING,
        &op_flag[OP_FLAG_RPL1_16], &op_rpl1[16],
        "RPL1_16"
    },
    {
        COM_ARGS_NO_KEY,  "RPL1_17", ARGS_TYPE_STRING,
        &op_flag[OP_FLAG_RPL1_17], &op_rpl1[17],
        "RPL1_17"
    },
    {
        COM_ARGS_NO_KEY,  "RPL1_18", ARGS_TYPE_STRING,
        &op_flag[OP_FLAG_RPL1_18], &op_rpl1[18],
        "RPL1_18"
    },
    {
        COM_ARGS_NO_KEY,  "RPL1_19", ARGS_TYPE_STRING,
        &op_flag[OP_FLAG_RPL1_19], &op_rpl1[19],
        "RPL1_19"
    },
    {
        COM_ARGS_NO_KEY,  "RPL1_20", ARGS_TYPE_STRING,
        &op_flag[OP_FLAG_RPL1_20], &op_rpl1[20],
        "RPL1_20"
    },
#endif
#if LIBVC_ON
        {
            COM_ARGS_NO_KEY,  "libpic", ARGS_TYPE_INTEGER,
            &op_flag[OP_LIBPIC_ENABLE], &op_tool_libpic,
            "libpic on/off flag"
        },
            {
        COM_ARGS_NO_KEY,  "qp_offset_libpic", ARGS_TYPE_INTEGER,
        &op_flag[OP_FLAG_QP_OFFSET_LIBPIC], &op_qp_offset_libpic,
        "qp offset for libpic, default:-6"
            },
    {
        COM_ARGS_NO_KEY,  "libout", ARGS_TYPE_STRING,
        &op_flag[OP_FLAG_FNAME_LIBOUT], op_fname_libout,
        "file name of output libpic bitstream"
    },

    {
        COM_ARGS_NO_KEY,  "librec", ARGS_TYPE_STRING,
        &op_flag[OP_FLAG_FNAME_LIBREC], op_fname_librec,
        "file name of reconstructed libpic"
    },
    {
        COM_ARGS_NO_KEY,  "libdependency", ARGS_TYPE_STRING,
        &op_flag[OP_FLAG_FNAME_LIBDATA], op_fname_libdependency,
        "file name of library picture dependency information"
    },
    {
        COM_ARGS_NO_KEY,  "skip_frames_when_extract_libpic", ARGS_TYPE_INTEGER,
        &op_flag[OP_FLAG_SKIP_FRAMES_WHEN_EXTRACT_LIBPIC], &op_skip_frames_when_extract_libpic,
        "number of skipped frames before extract libpic. default: the same as the number of skipped frames before encoding"
    },
    {
        COM_ARGS_NO_KEY,  "frames_when_extract_libpic", ARGS_TYPE_INTEGER,
        &op_flag[OP_FLAG_MAX_FRM_NUM_WHEN_EXTRACT_LIBPIC], &op_max_frm_num_when_extract_libpic,
        "maximum number of frames when extract libpic. default: the same as the nmaximum number of frames to be encoded"
    },
#endif


    {
        COM_ARGS_NO_KEY, "TemporalSubsampleRatio", ARGS_TYPE_INTEGER,
        &op_flag[OP_TEMP_SUB_RATIO], &op_subSampleRatio,
        "Subsampling Ratio (default: 8 for AI, 1 for RA and LD)"
    },
    {
        COM_ARGS_NO_KEY, "ctu_size", ARGS_TYPE_INTEGER,
        &op_flag[OP_CTU_SZE], &op_ctu_size,
        "ctu_size (default: 128; allowed values: 32, 64, 128)"
    },
    {
        COM_ARGS_NO_KEY, "min_cu_size", ARGS_TYPE_INTEGER,
        &op_flag[OP_MIN_CU_SIZE], &op_min_cu_size,
        "min_cu_size (default: 4; allowed values: 4)"
    },
    {
        COM_ARGS_NO_KEY, "max_part_ratio", ARGS_TYPE_INTEGER,
        &op_flag[OP_MAX_PART_RATIO], &op_max_part_ratio,
        "max_part_ratio (default:8; allowed values: 8)"
    },
    {
        COM_ARGS_NO_KEY, "max_split_times", ARGS_TYPE_INTEGER,
        &op_flag[OP_MAX_SPLIT_TIMES], &op_max_split_times,
        "max_split_times (default: 6, allowed values: 6)"
    },
    {
        COM_ARGS_NO_KEY, "min_qt_size", ARGS_TYPE_INTEGER,
        &op_flag[OP_MIN_QT_SIZE], &op_min_qt_size,
        "min_qt_size (default: 8, allowed values: 4, 8, 16, 32, 64, 128)"
    },
    {
        COM_ARGS_NO_KEY, "max_bt_size", ARGS_TYPE_INTEGER,
        &op_flag[OP_MAX_BT_SIZE], &op_max_bt_size,
        "max_bt_size (default: 128, allowed values: 64, 128)"
    },
    {
        COM_ARGS_NO_KEY, "max_eqt_size", ARGS_TYPE_INTEGER,
        &op_flag[OP_MAX_EQT_SIZE], &op_max_eqt_size,
        "max_eqt_size (default: 64, allowed values: 8, 16, 32, 64)"
    },
    {
        COM_ARGS_NO_KEY, "max_dt_size", ARGS_TYPE_INTEGER,
        &op_flag[OP_MAX_DT_SIZE], &op_max_dt_size,
        "max_dt_size (default: 64, allowed values: 16, 32, 64)"
    },
    {
        COM_ARGS_NO_KEY, "fps_num", ARGS_TYPE_INTEGER,
        &op_flag[OP_FPS_NUM], &op_fps_num,
        "fps"
    },
    {
        COM_ARGS_NO_KEY, "fps_den", ARGS_TYPE_INTEGER,
        &op_flag[OP_FPS_DEN], &op_fps_den,
        "fps"
    },
    {
        COM_ARGS_NO_KEY, "rc_min_qp", ARGS_TYPE_INTEGER,
        &op_flag[OP_RC_MIN_QP], &op_rc_min_qp,
        "fps"
    },
    {
        COM_ARGS_NO_KEY, "rc_max_qp", ARGS_TYPE_INTEGER,
        &op_flag[OP_RC_MAX_QP], &op_rc_max_qp,
        "fps"
    },
    {
        COM_ARGS_NO_KEY, "rc_method", ARGS_TYPE_INTEGER,
        &op_flag[OP_RC_METHOD], &op_rc_method,
        "rc_type (CRF,ABR,CBR)"
    },
    {
        COM_ARGS_NO_KEY, "bitrate", ARGS_TYPE_INTEGER,
        &op_flag[OP_BITRATE], &op_bitrate,
        "abr  cbr  bitrate"
    },
    {
        COM_ARGS_NO_KEY, "max_bitrate", ARGS_TYPE_INTEGER,
        &op_flag[OP_VBV_MAX_RATE], &op_vbv_max_bitrate,
        "cbr(vbv_max_bitrate == bitrate)"
    },
    {
        COM_ARGS_NO_KEY, "vbv-bufsize", ARGS_TYPE_INTEGER,
        &op_flag[OP_VBV_BUF_SIZE], &op_vbv_buffer_size,
        "cbr(vbv_max_bitrate == bitrate)"
	},
#if RATECONTROL 
	{
		COM_ARGS_NO_KEY, "enable_ratectrl", ARGS_TYPE_INTEGER,
		&op_flag[OP_ENABLE_RATECONTROL], &op_enable_ratectrl,
		"(0: Disable, 1: Enable)"
	},
	{
		COM_ARGS_NO_KEY, "target_bitrate", ARGS_TYPE_INTEGER,
		&op_flag[OP_TARGET_BITRATE], &op_target_bitrate,
		"kbps"
	},
	{
		COM_ARGS_NO_KEY, "rc_initialqp", ARGS_TYPE_INTEGER,
		&op_flag[OP_RC_INITIALQP], &op_rc_initialqp,
		"initial QP"
	},
	{
		COM_ARGS_NO_KEY, "block_ratectrl", ARGS_TYPE_INTEGER,
		&op_flag[OP_BLOCK_RATECONTROL], &op_block_ratectrl,
		"(0:picture-level ratecontrol, 1:block-level ratecontrol)"
	},
	{
		COM_ARGS_NO_KEY, "param_update", ARGS_TYPE_INTEGER,
		&op_flag[OP_PARAM_UPDATE], &op_param_update,
		"(0: Disable, 1: Enable)"
	},
	{
		COM_ARGS_NO_KEY, "intra_group", ARGS_TYPE_INTEGER,
		&op_flag[OP_INTRA_GROUP], &op_intra_group,
		"(intra gop  group size)"
	},
#endif
    {0, "", COM_ARGS_VAL_TYPE_NONE, NULL, NULL, ""} /* termination */
};

#define NUM_ARG_OPTION   ((int)(sizeof(options)/sizeof(options[0]))-1)
static void print_usage(void)
{
    int i;
    char str[1024];
    //v0print("< Usage >\n");
    for(i=0; i<NUM_ARG_OPTION; i++)
    {
        if(com_args_get_help(options, i, str) < 0) return;
        //v0print("%s\n", str);
    }
}

static int get_conf(ENC_PARAM * param)
{
    memset(param, 0, sizeof(ENC_PARAM));
    param->horizontal_size = op_w;
    param->vertical_size   = op_h;
    param->pic_width  = (param->horizontal_size + MINI_SIZE - 1) / MINI_SIZE * MINI_SIZE;
    param->pic_height = (param->vertical_size   + MINI_SIZE - 1) / MINI_SIZE * MINI_SIZE;
    param->qp = op_qp;
    param->fps_num = op_fps_num;
    param->fps_den = op_fps_den;
    param->fps = op_fps;
    param->i_period = op_i_period;
    param->max_b_frames = op_max_b_frames;
    param->frame_qp_add = op_frame_qp_add;
    param->pb_frame_qp_offset = op_pb_frame_qp_offset;
#if USE_INTRA_REFRESH
	param->EncRefresh = op_intra_refresh_control;
	param->RefreshSize = op_intra_refresh_size;
	param->RefreshMode = op_intra_refresh_mode;
    param->i_period_org = param->i_period;
    if (param->EncRefresh)
        param->i_period = 0;
#endif
#if USE_SPEED_LEVEL
	param->speed_level = op_speed_level;
#endif

#if USE_ROI_QP_CONFIG
	param->use_ROI_qp1 = op_roi_qp_enable_1;
	param->ROI_qp_Start_x1 = op_roi_qp_start_x_1;
	param->ROI_qp_End_x1 = op_roi_qp_end_x_1;
	param->ROI_qp_Start_y1 = op_roi_qp_start_y_1;
	param->ROI_qp_End_y1 = op_roi_qp_end_y_1;
	param->ROI_QP1 = op_roi_qp_1;

	param->use_ROI_qp2 = op_roi_qp_enable_2;
	param->ROI_qp_Start_x2 = op_roi_qp_start_x_2;
	param->ROI_qp_End_x2 = op_roi_qp_end_x_2;
	param->ROI_qp_Start_y2 = op_roi_qp_start_y_2;
	param->ROI_qp_End_y2 = op_roi_qp_end_y_2;
	param->ROI_QP2 = op_roi_qp_2;

	param->use_ROI_qp3 = op_roi_qp_enable_3;
	param->ROI_qp_Start_x3 = op_roi_qp_start_x_3;
	param->ROI_qp_End_x3 = op_roi_qp_end_x_3;
	param->ROI_qp_Start_y3 = op_roi_qp_start_y_3;
	param->ROI_qp_End_y3 = op_roi_qp_end_y_3;
	param->ROI_QP3 = op_roi_qp_3;

	param->use_ROI_qp4 = op_roi_qp_enable_4;
	param->ROI_qp_Start_x4 = op_roi_qp_start_x_4;
	param->ROI_qp_End_x4 = op_roi_qp_end_x_4;
	param->ROI_qp_Start_y4 = op_roi_qp_start_y_4;
	param->ROI_qp_End_y4 = op_roi_qp_end_y_4;
	param->ROI_QP4 = op_roi_qp_4;

	param->use_ROI_qp5 = op_roi_qp_enable_5;
	param->ROI_qp_Start_x5 = op_roi_qp_start_x_5;
	param->ROI_qp_End_x5 = op_roi_qp_end_x_5;
	param->ROI_qp_Start_y5 = op_roi_qp_start_y_5;
	param->ROI_qp_End_y5 = op_roi_qp_end_y_5;
	param->ROI_QP5 = op_roi_qp_5;

	param->use_ROI_qp6 = op_roi_qp_enable_6;
	param->ROI_qp_Start_x6 = op_roi_qp_start_x_6;
	param->ROI_qp_End_x6 = op_roi_qp_end_x_6;
	param->ROI_qp_Start_y6 = op_roi_qp_start_y_6;
	param->ROI_qp_End_y6 = op_roi_qp_end_y_6;
	param->ROI_QP6 = op_roi_qp_6;

	param->use_ROI_qp7 = op_roi_qp_enable_7;
	param->ROI_qp_Start_x7 = op_roi_qp_start_x_7;
	param->ROI_qp_End_x7 = op_roi_qp_end_x_7;
	param->ROI_qp_Start_y7 = op_roi_qp_start_y_7;
	param->ROI_qp_End_y7 = op_roi_qp_end_y_7;
	param->ROI_QP7 = op_roi_qp_7;

	param->use_ROI_qp8 = op_roi_qp_enable_8;
	param->ROI_qp_Start_x8 = op_roi_qp_start_x_8;
	param->ROI_qp_End_x8 = op_roi_qp_end_x_8;
	param->ROI_qp_Start_y8 = op_roi_qp_start_y_8;
	param->ROI_qp_End_y8 = op_roi_qp_end_y_8;
	param->ROI_QP8 = op_roi_qp_8;

	param->useQPMAP = op_useQPMAP;
	param->useQPMAPCon = op_useQPMAPCon;
#endif
#if CUDQP_QP_MAP
    param->useCuQPMAP = op_useCuQPMAP;
#endif
#if USE_ROI_MODE_CONFIG
	param->use_ROI_Mode1 = op_roi_mode_enable_1;
	param->ROI_Mode_Start_x1 = op_roi_mode_start_x_1;
	param->ROI_Mode_End_x1 = op_roi_mode_end_x_1;
	param->ROI_Mode_Start_y1 = op_roi_mode_start_y_1;
	param->ROI_Mode_End_y1 = op_roi_mode_end_y_1;
	param->ROI_Mode1 = op_roi_mode_1;

	param->use_ROI_Mode2 = op_roi_mode_enable_2;
	param->ROI_Mode_Start_x2 = op_roi_mode_start_x_2;
	param->ROI_Mode_End_x2 = op_roi_mode_end_x_2;
	param->ROI_Mode_Start_y2 = op_roi_mode_start_y_2;
	param->ROI_Mode_End_y2 = op_roi_mode_end_y_2;
	param->ROI_Mode2 = op_roi_mode_2;

	param->use_ROI_Mode3 = op_roi_mode_enable_3;
	param->ROI_Mode_Start_x3 = op_roi_mode_start_x_3;
	param->ROI_Mode_End_x3 = op_roi_mode_end_x_3;
	param->ROI_Mode_Start_y3 = op_roi_mode_start_y_3;
	param->ROI_Mode_End_y3 = op_roi_mode_end_y_3;
	param->ROI_Mode3 = op_roi_mode_3;

	param->use_ROI_Mode4 = op_roi_mode_enable_4;
	param->ROI_Mode_Start_x4 = op_roi_mode_start_x_4;
	param->ROI_Mode_End_x4 = op_roi_mode_end_x_4;
	param->ROI_Mode_Start_y4 = op_roi_mode_start_y_4;
	param->ROI_Mode_End_y4 = op_roi_mode_end_y_4;
	param->ROI_Mode4 = op_roi_mode_4;

	param->use_ROI_Mode5 = op_roi_mode_enable_5;
	param->ROI_Mode_Start_x5 = op_roi_mode_start_x_5;
	param->ROI_Mode_End_x5 = op_roi_mode_end_x_5;
	param->ROI_Mode_Start_y5 = op_roi_mode_start_y_5;
	param->ROI_Mode_End_y5 = op_roi_mode_end_y_5;
	param->ROI_Mode5 = op_roi_mode_5;

	param->use_ROI_Mode6 = op_roi_mode_enable_6;
	param->ROI_Mode_Start_x6 = op_roi_mode_start_x_6;
	param->ROI_Mode_End_x6 = op_roi_mode_end_x_6;
	param->ROI_Mode_Start_y6 = op_roi_mode_start_y_6;
	param->ROI_Mode_End_y6 = op_roi_mode_end_y_6;
	param->ROI_Mode6 = op_roi_mode_6;

	param->use_ROI_Mode7 = op_roi_mode_enable_7;
	param->ROI_Mode_Start_x7 = op_roi_mode_start_x_7;
	param->ROI_Mode_End_x7 = op_roi_mode_end_x_7;
	param->ROI_Mode_Start_y7 = op_roi_mode_start_y_7;
	param->ROI_Mode_End_y7 = op_roi_mode_end_y_7;
	param->ROI_Mode7 = op_roi_mode_7;

	param->use_ROI_Mode8 = op_roi_mode_enable_8;
	param->ROI_Mode_Start_x8 = op_roi_mode_start_x_8;
	param->ROI_Mode_End_x8 = op_roi_mode_end_x_8;
	param->ROI_Mode_Start_y8 = op_roi_mode_start_y_8;
	param->ROI_Mode_End_y8 = op_roi_mode_end_y_8;
	param->ROI_Mode8 = op_roi_mode_8;

	param->useModeMAP = op_useModeMAP;
	param->useModeMAPCon = op_useModeMAPCon;
#endif

#if USE_TRACE_DUMP
	param->useTraceDump = op_useTraceDump;
#endif
	int i;
    if( param->horizontal_size % MINI_SIZE != 0 )
    {
        printf( "Note: picture width in encoding process is padded to %d (input value %d)\n", param->pic_width, param->horizontal_size );
    }
    if( param->vertical_size % MINI_SIZE != 0 )
    {
        printf( "Note: picture height in encoding process is padded to %d (input value %d)\n", param->pic_height, param->vertical_size );
    }
    if(op_disable_hgop)
    {
        param->disable_hgop = 1;
    }
    param->bit_depth_input = op_bit_depth_input;
    if (op_bit_depth_internal == 0)
    {
        op_bit_depth_internal = op_bit_depth_input;
    }
    param->bit_depth_internal = op_bit_depth_internal;

    if (param->bit_depth_input > param->bit_depth_internal)
    {
        printf("Warning: Data precision may lose because input bit depth is higher than internal one !\n");
    }

    assert(param->bit_depth_input == 8 || param->bit_depth_input == 10);
    assert(param->bit_depth_internal == 8 || param->bit_depth_internal == 10);

	param->ipcm_enable_flag = 0;

    param->amvr_enable_flag = op_tool_amvr;
    param->affine_enable_flag = op_tool_affine;
    param->smvd_enable_flag = op_tool_smvd;
    param->use_deblock = op_tool_deblock;
    param->num_of_hmvp_cand = op_tool_hmvp;
    param->ipf_flag = op_tool_ipf;
#if SUB_TMVP
    param->sbtmvp_enable_flag = 1;
    //#else
       // param->sbtmvp_enable_flag = op_tool_sbtmvp;

#endif

#if TSCPM
    param->tscpm_enable_flag = op_tool_tscpm;
#endif
    param->umve_enable_flag = op_tool_umve;
#if UMVE_ENH
    param->umve_enh_enable_flag = param->umve_enable_flag && op_tool_umve_enh;
#endif
#if EXT_AMVR_HMVP
    if (param->amvr_enable_flag && param->num_of_hmvp_cand)
    {
        param->emvr_enable_flag = op_tool_emvr;
    }
    else
    {
        param->emvr_enable_flag = 0;
    }
#endif
#if DT_PARTITION
    param->dt_intra_enable_flag = op_tool_dt_intra;
#endif
    param->position_based_transform_enable_flag = op_tool_pbt;

    param->wq_enable = op_wq_enable;
    param->seq_wq_mode = op_seq_wq_mode;
    strcpy(param->seq_wq_user, op_seq_wq_user);

    param->pic_wq_data_idx = op_pic_wq_data_idx;
    strcpy(param->pic_wq_user, op_pic_wq_user);

    param->wq_param = op_wq_param;
    param->wq_model = op_wq_model;
    strcpy(param->wq_param_detailed, op_wq_param_detailed);
    strcpy(param->wq_param_undetailed, op_wq_param_undetailed);

#if PATCH
    param->patch_stable = op_patch_stable;
    param->cross_patch_loop_filter = op_cross_patch_loop_filter;
    param->patch_ref_colocated = op_patch_ref_colocated;

    if (param->patch_stable)
    {
        param->patch_uniform = op_patch_uniform;
        if (param->patch_uniform)
        {
#if PATCH_TEST
            param->patch_width_in_lcu = (param->pic_width + op_ctu_size - 1) / op_ctu_size;
            param->patch_width_in_lcu = (param->patch_width_in_lcu + 1) / 2;
            param->patch_height_in_lcu = (param->pic_height + op_ctu_size - 1) / op_ctu_size;
            param->patch_height_in_lcu = (param->patch_width_in_lcu + 1) / 2;;
#else
            param->patch_width_in_lcu = op_patch_width_in_lcu;
            param->patch_height_in_lcu = op_patch_height_in_lcu;
#endif
        }
        else
        {
            printf("please set patch_uniform = 1\n");
            exit(-1);
        }
    }
#endif
    param->sample_adaptive_offset_enable_flag = op_tool_sao;
    param->adaptive_leveling_filter_enable_flag = op_tool_alf;
    param->secondary_transform_enable_flag = op_tool_secTrans;

#if LIBVC_ON
    param->library_picture_enable_flag = op_tool_libpic;
#else
    param->library_picture_enable_flag = 0;
#endif
#if Debug422
    param->chroma_format = op_chroma_format; //must be 1 in base profile 
#else
    param->chroma_format = 1; //must be 1 in base profile 
#endif
    param->encoding_precision = (param->bit_depth_internal == 8) ? 1 : 2;
#if HLS_RPL
    for (i = 0; i < MAX_NUM_RPLS && op_rpl0[i][0] != 0; ++i)
    {
        char* slice_type = strtok(op_rpl0[i], "|");
        if (strcmp(slice_type, "B") == 0)
        {
            param->rpls_l0[i].slice_type = SLICE_B;
        }
        else if (strcmp(slice_type, "P") == 0)
        {
            param->rpls_l0[i].slice_type = SLICE_P;
        }
        else if (strcmp(slice_type, "I") == 0)
        {
            param->rpls_l0[i].slice_type = SLICE_I;
        }
        else
        {
            printf("\n wrong slice type\n");
            assert(0);
        }
        param->rpls_l0[i].poc = atoi(strtok(NULL, "|"));
        param->rpls_l0[i].tid = atoi(strtok(NULL, "|"));
        param->rpls_l0[i].ref_pic_active_num = atoi(strtok(NULL, "|"));
#if LIBVC_ON
        param->rpls_l0[i].reference_to_library_enable_flag = 0;
#endif
        int j = 0;
        int k = 0;
        int ddoi = 0;
        do
        {
            char* val = strtok(NULL, "|");
            if (!val)
                break;

#if LIBVC_ON
            if (atoi(val) == 9999)
            {
                ddoi = 1;
                continue;
            }
            if (ddoi == 1)
            {
                param->rpls_l0[i].ref_pics_ddoi[k++] = atoi(val);
            }
            else
            {
                param->rpls_l0[i].ref_pics[j] = atoi(val);
                param->rpls_l0[i].library_index_flag[j] = 0;
                j++;
            }
#else
            if (atoi(val) == 9999)
            {
                ddoi = 1;
                continue;
            }
            if (ddoi == 1)
            {
                param->rpls_l0[i].ref_pics_ddoi[k++] = atoi(val);
            }
            else
            {
                param->rpls_l0[i].ref_pics[j++] = atoi(val);
            }
#endif

        }
        while (1);

        param->rpls_l0[i].ref_pic_num = j;
        ++param->rpls_l0_cfg_num;
    }

    for (i = 0; i < MAX_NUM_RPLS && op_rpl1[i][0] != 0; ++i)
    {
        char* slice_type = strtok(op_rpl1[i], "|");
        if (strcmp(slice_type, "B") == 0)
        {
            param->rpls_l1[i].slice_type = SLICE_B;
        }
        else if (strcmp(slice_type, "P") == 0)
        {
            param->rpls_l1[i].slice_type = SLICE_P;
        }
        else if (strcmp(slice_type, "I") == 0)
        {
            param->rpls_l1[i].slice_type = SLICE_I;
        }
        else
        {
            printf("\n wrong slice type\n");
            assert(0);
        }
        param->rpls_l1[i].poc = atoi(strtok(NULL, "|"));
        param->rpls_l1[i].tid = atoi(strtok(NULL, "|"));
        param->rpls_l1[i].ref_pic_active_num = atoi(strtok(NULL, "|"));
#if LIBVC_ON
        param->rpls_l1[i].reference_to_library_enable_flag = 0;
#endif

        int j = 0;
        int k = 0;
        int ddoi = 0;
        do
        {
            char* val = strtok(NULL, "|");
            if (!val)
                break;

#if LIBVC_ON
            if (atoi(val) == 9999)
            {
                ddoi = 1;
                continue;
            }
            if (ddoi == 1)
            {
                param->rpls_l1[i].ref_pics_ddoi[k++] = atoi(val);
            }
            else
            {
                param->rpls_l1[i].ref_pics[j] = atoi(val);
                param->rpls_l1[i].library_index_flag[j] = 0;
                j++;
            }
#else
            if (atoi(val) == 9999)
            {
                ddoi = 1;
                continue;
            }
            if (ddoi == 1)
            {
                param->rpls_l1[i].ref_pics_ddoi[k++] = atoi(val);
            }
            else
            {
                param->rpls_l1[i].ref_pics[j++] = atoi(val);
            }
#endif


        }
        while (1);

        param->rpls_l1[i].ref_pic_num = j;
        ++param->rpls_l1_cfg_num;
    }
#endif

#if LIBVC_ON
    param->qp_offset_libpic = op_qp_offset_libpic;
#endif
    param->sub_sample_ratio = op_subSampleRatio;
    param->frames_to_be_encoded = (op_max_frm_num + op_subSampleRatio - 1) / op_subSampleRatio;
    param->use_pic_sign = op_use_pic_signature;
    param->ctu_size = op_ctu_size;
    param->min_cu_size = op_min_cu_size;
    param->max_part_ratio = op_max_part_ratio;
    param->max_split_times = op_max_split_times;
    param->min_qt_size = op_min_qt_size;
    param->max_bt_size = op_max_bt_size;
    param->max_eqt_size = op_max_eqt_size;
    param->max_dt_size = op_max_dt_size;
    param->rc_max_qp = op_rc_max_qp;
    param->rc_bitrate = op_bitrate;
    param->rc_max_bitrate = op_vbv_max_bitrate;
    param->rc_vbv_buf_size = op_vbv_buffer_size;
    //verify parameters allowed in Profile
    assert(param->ctu_size == 32 || param->ctu_size == 64 || param->ctu_size == 128);
    assert(param->min_cu_size == 4);
    assert(param->max_part_ratio == 8);
    assert(param->max_split_times == 6);
    assert(param->min_qt_size == 4 || param->min_qt_size == 8 || param->min_qt_size == 16 || param->min_qt_size == 32 || param->min_qt_size == 64 || param->min_qt_size == 128);
    assert(param->max_bt_size == 64 || param->max_bt_size == 128);
    assert(param->max_eqt_size == 8 || param->max_eqt_size == 16 || param->max_eqt_size == 32 || param->max_eqt_size == 64);
    assert(param->max_dt_size == 16 || param->max_dt_size == 32 || param->max_dt_size == 64);

    param->qp_offset_cb = op_qp_offset_cb;
    param->qp_offset_cr = op_qp_offset_cr;

    param->delta_qp_flag = op_delta_qp_flag;
    param->qp_offset_adp = op_qp_offset_adp;
	#if RATECONTROL
    param->rc_flag = op_enable_ratectrl;
#endif
#if CUDQP_QP_MAP
	param->cu_delta_qp_flag = op_delta_qp_flag ? op_cu_delta_qp_flag : 0;
	param->cu_qp_group_size = op_cu_qp_group_size;
	if (param->cu_qp_group_size > param->ctu_size)
	{
		printf("cu_qp_group_size shall be not larger than ctu_size\n");
		exit(-1);
	}
#endif
    return 0;
}

static int set_extra_config(ENC id)
{
    ENC_CTX *ctx = (ENC_CTX *)id;
    ctx->param.use_pic_sign = op_use_pic_signature;
    return 0;
}
static void print_stat_init(void)
{
    if(op_verbose < VERBOSE_FRAME) return;
	printf("-------------------------------------------------------------------------------\n");
	printf("  Input YUV file           : %s \n", op_fname_inp);
    if(op_flag[OP_FLAG_FNAME_OUT])
    {
        printf("  Output bitstream         : %s \n", op_fname_out);
    }
    if(op_flag[OP_FLAG_FNAME_REC])
    {
        printf("  Output YUV file          : %s \n", op_fname_rec);
    }

#ifdef MD_DBG_TB
    printf("MD_DBG_TB enable ...\n");
#endif// MD_DBG_TB
#ifdef CALL_DBG
    printf("GPROF CALL_DBG enable ...\n");
#endif// CALL_DBG

	printf("--------------------------------------------------------------------------------------\n");
	printf(" POC       QP   PSNR-Y    PSNR-U    PSNR-V    Bits      EncT(ms)   MS-SSIM   Ref. List\n");
	printf("--------------------------------------------------------------------------------------\n");
    //print_flush(stdout);
}

static void print_config(ENC_PARAM param)
{
	printf("HPM version : %s -------------------------------------------------------------------------------\n", HPM_VERSION);
	printf("< Configurations >\n");
    if (op_flag[OP_FLAG_FNAME_CFG])
    {
        printf("\tconfig file name     : %s\n", op_fname_cfg);
    }
    printf("\twidth                    : %d\n", param.pic_width);
    printf("\theight                   : %d\n", param.pic_height);
    printf("\tFPS                      : %d\n", param.fps);
    printf("\tintra picture period     : %d\n", param.i_period);
    printf("\tinput bit depth          : %d\n", param.bit_depth_input);
    printf("\tinternal bit depth       : %d\n", param.bit_depth_internal);
    printf("\tQP                       : %d\n", param.qp);
    printf("\tframes                   : %d\n", op_max_frm_num);
    printf("\thierarchical GOP         : %s\n", !param.disable_hgop ? "enabled" : "disabled");
    printf("\tPatch flag               : %d\n", param.patch_width_in_lcu | param.patch_height_in_lcu);
    if (param.patch_width_in_lcu | param.patch_height_in_lcu)
    {
        if (param.patch_width_in_lcu)
        {
            printf("\tPatch width in lcu       : %d\n", param.patch_width_in_lcu);
        }
        else
        {
            printf("\tPatch width in lcu       : %s\n", "same as picture");
        }

        if (param.patch_height_in_lcu)
        {
            printf("\tPatch height in lcu      : %d\n", param.patch_height_in_lcu);
        }
        else
        {
            printf("\tPatch height in lcu      : %s\n", "same as picture");
        }
    }

    printf("\tsub_sample_ratio         : %d\n", param.sub_sample_ratio);
    printf("\tdelta_qp flag            : %d\n", param.delta_qp_flag);
#if CUDQP_QP_MAP
	print("\tcu delta qp flag         : %d\n", param.cu_delta_qp_flag);
	print("\tcu qp group size         : %d\n", param.cu_qp_group_size);
#endif
    printf("\tqp_offset_cb             : %d\n", param.qp_offset_cb);
    printf("\tqp_offset_cr             : %d\n", param.qp_offset_cr);
    printf("\tqp_offset_adp            : %d\n", param.qp_offset_adp);
    //printf("\tqp_offset_libpic         : %d\n", param.qp_offset_libpic);
    printf("\tseq_ref2_lib_enable      : %d\n", param.library_picture_enable_flag);

    //printf split configure
    printf("\nCU split CFG: ");
    printf("\n\tctu_size:        %d", param.ctu_size);
    printf("\n\tmin_cu_size:     %d", param.min_cu_size);
    printf("\n\tmax_part_ratio:  %d", param.max_part_ratio);
    printf("\n\tmax_split_times: %d", param.max_split_times);
    printf("\n\tmin_qt_size:     %d", param.min_qt_size);
    printf("\n\tmax_bt_size:     %d", param.max_bt_size);
    printf("\n\tmax_eqt_size:    %d", param.max_eqt_size);
    printf("\n\tmax_dt_size:     %d", param.max_dt_size);
    printf("\n");

    // printf tool configurations
    printf("\nTool CFG:\n");
    //loop filter
    printf("DEBLOCK: %d, ", param.use_deblock);
    printf("CROSS_PATCH_LP: %d, ", param.cross_patch_loop_filter);
    printf("SAO: %d, ", param.sample_adaptive_offset_enable_flag);
    printf("ALF: %d, ", param.adaptive_leveling_filter_enable_flag);
    printf("\n");

    //inter
    printf("AMVR: %d, ", param.amvr_enable_flag);
    printf("HMVP_NUM: %d, ", param.num_of_hmvp_cand);
    printf("AFFINE: %d, ", param.affine_enable_flag);

#if SMVD
    printf("SMVD: %d, ", param.smvd_enable_flag);
#endif
    printf("UMVE: %d, ", param.umve_enable_flag);
#if EXT_AMVR_HMVP
    printf("EMVR: %d, ", param.emvr_enable_flag);
#endif
    printf("\n");

    //intra
    printf("IPCM: %d, ", param.ipcm_enable_flag);
#if TSCPM
    printf("TSCPM: %d, ", param.tscpm_enable_flag);
#endif
    printf("IPF: %d, ", param.ipf_flag);
    printf("\n");

    //transform & quant
    printf("PBT: %d, ", param.position_based_transform_enable_flag);
    printf("SecondaryTr: %d, ", param.secondary_transform_enable_flag);
    printf("WeightedQuant: %d, ", param.wq_enable);
    printf("\n");

    //DT
#if DT_PARTITION
    printf("INTRA_DT: %d, ", param.dt_intra_enable_flag);
    printf("MaxDTSize: %d, ", param.max_dt_size);
#endif
    printf("\n");

#if USE_ENH
    printf("Phase 2 Tool CFG\n");
#if SUB_TMVP
    printf("SbTMVP: %d, ", param.sbtmvp_enable_flag);
#endif
#if SRCC
    printf("SRCC: %d, ", SRCC);
#endif
#if UMVE_ENH
    printf("UMVE_ENH: %d, ", param.umve_enh_enable_flag);
#endif
#if CABAC_MULTI_PROB
    printf("MCABAC: %d, ", CABAC_MULTI_PROB);
#endif
#if EIPM
    printf("EIPM: %d, ", EIPM);
#endif
#if MIPF
    printf("MIPF: %d, ", MIPF);
#endif
    printf("\n");
#endif
    print_flush(stdout);
}

static void calc_psnr(COM_IMGB * org, COM_IMGB * rec, double psnr[3], int bit_depth)
{
    double sum[3], mse[3];
    short *o, *r;
    int i, j, k;
#if PSNR_1020
    int peak_val = (bit_depth == 8) ? 255 : 1020;
#else
    int peak_val = (bit_depth == 8) ? 255 : 1023;
#endif
#if Debug422
    for (i = 0; i < org->np; i++)
    {
        if (i == 0)
        {
            o = (short*)org->addr_plane[i];
            r = (short*)rec->addr_plane[i];
            sum[i] = 0;
            for (j = 0; j < org->height[i]; j++)
            {
                for (k = 0; k < org->width[i]; k++)
                {
                    sum[i] += (o[k] - r[k]) * (o[k] - r[k]);
                }
                o = (short*)((unsigned char*)o + org->stride[i]);
                r = (short*)((unsigned char*)r + rec->stride[i]);
            }
            mse[i] = sum[i] / (org->width[i] * org->height[i]);
            psnr[i] = (mse[i] == 0.0) ? 100. : fabs(10 * log10(((peak_val * peak_val) / mse[i])));
        }
        else
        {
            o = i == 1 ? Fetch_input_video_U8_ptr.imgU_org : Fetch_input_video_U8_ptr.imgV_org;
            r = (short*)rec->addr_plane[i];
            sum[i] = 0;
            for (j = 0; j < org->height[i] / 2; j++)
            {
                for (k = 0; k < org->width[i]; k++)
                {
                    sum[i] += (o[k] - r[k]) * (o[k] - r[k]);
                }
                o = (short*)((unsigned char*)o + org->stride[i]);
                r = (short*)((unsigned char*)r + rec->stride[i]);
            }
            mse[i] = sum[i] / (org->width[i] * org->height[i] / 2);
            psnr[i] = (mse[i] == 0.0) ? 100. : fabs(10 * log10(((peak_val * peak_val) / mse[i])));
        }
    }
#else
    for (i = 0; i < org->np; i++)
    {
        o = (short*)org->addr_plane[i];
        r = (short*)rec->addr_plane[i];
        sum[i] = 0;
        for (j = 0; j < org->height[i]; j++)
        {
            for (k = 0; k < org->width[i]; k++)
            {
                sum[i] += (o[k] - r[k]) * (o[k] - r[k]);
            }
            o = (short*)((unsigned char*)o + org->stride[i]);
            r = (short*)((unsigned char*)r + rec->stride[i]);
        }
        mse[i] = sum[i] / (org->width[i] * org->height[i]);
        psnr[i] = (mse[i] == 0.0) ? 100. : fabs(10 * log10(((peak_val * peak_val) / mse[i])));
    }
#endif

}
static int imgb_list_alloc_422(IMGB_LIST* list, int width, int height, int horizontal_size, int vertical_size, int bit_depth)
{
    int i;
    memset(list, 0, sizeof(IMGB_LIST) * MAX_BUMP_FRM_CNT);
    for (i = 0; i < MAX_BUMP_FRM_CNT; i++)
    {
        list[i].imgb = imgb_alloc(width, height, COM_COLORSPACE_YUV422, bit_depth);
        if (list[i].imgb == NULL) goto ERR;
        list[i].imgb->horizontal_size = horizontal_size;
        list[i].imgb->vertical_size = vertical_size;
        if (horizontal_size % 2 == 1 || vertical_size % 2 == 1)
        {
            printf("\n[Error] width and height of a YUV4:2:0 input video shall be at least a multiple of 2");
            goto ERR;
        }
    }
    return 0;
ERR:
    for (i = 0; i < MAX_BUMP_FRM_CNT; i++)
    {
        if (list[i].imgb)
        {
            imgb_free(list[i].imgb);
            list[i].imgb = NULL;
        }
    }
    return -1;
}

static int imgb_list_alloc(IMGB_LIST *list, int width, int height, int horizontal_size, int vertical_size, int bit_depth)
{
    int i;
    memset(list, 0, sizeof(IMGB_LIST)*MAX_BUMP_FRM_CNT);
    for(i=0; i<MAX_BUMP_FRM_CNT; i++)
    {
        list[i].imgb =  imgb_alloc(width, height, COM_COLORSPACE_YUV420, bit_depth);
        if(list[i].imgb == NULL) goto ERR;
        list[i].imgb->horizontal_size = horizontal_size;
        list[i].imgb->vertical_size   = vertical_size;
        if( horizontal_size % 2 == 1 || vertical_size % 2 == 1 )
        {
            printf( "\n[Error] width and height of a YUV4:2:0 input video shall be at least a multiple of 2" );
            goto ERR;
        }
    }
    return 0;
ERR:
    for(i=0; i<MAX_BUMP_FRM_CNT; i++)
    {
        if(list[i].imgb)
        {
            imgb_free(list[i].imgb);
            list[i].imgb = NULL;
        }
    }
    return -1;
}

static void imgb_list_free(IMGB_LIST *list)
{
    int i;
    for(i=0; i<MAX_BUMP_FRM_CNT; i++)
    {
        if(list[i].imgb)
        {
            imgb_free(list[i].imgb);
            list[i].imgb = NULL;
        }
    }
}

static IMGB_LIST *store_rec_img(IMGB_LIST *list, COM_IMGB *imgb, COM_MTIME ts, int bit_depth)
{
    int i;

    for(i=0; i<MAX_BUMP_FRM_CNT; i++)
    {
        if(list[i].used == 0)
        {
            assert(list[i].imgb->cs == imgb->cs);
            imgb_cpy_conv_rec(list[i].imgb, imgb, bit_depth);
            list[i].used = 1;
            list[i].ts = ts;
            return &list[i];
        }
    }
    return NULL;
}

static IMGB_LIST *imgb_list_get_empty(IMGB_LIST *list)
{
    int i;
    /* store original imgb for PSNR */
    for(i=0; i<MAX_BUMP_FRM_CNT; i++)
    {
        if(list[i].used == 0)
        {
            return &list[i];
        }
    }
    return NULL;
}

static void imgb_list_make_used(IMGB_LIST *list, COM_MTIME ts)
{
    list->used = 1;
    list->ts = list->imgb->ts[0] = ts;
}

static int cal_psnr(IMGB_LIST * imgblist_inp, COM_IMGB * imgb_rec, COM_MTIME ts, double psnr[3]
                   )
{
    int            i;
    /* calculate PSNR */
    psnr[0] = psnr[1] = psnr[2] = 0;
    for(i = 0; i < MAX_BUMP_FRM_CNT; i++)
    {
        if(imgblist_inp[i].ts == ts && imgblist_inp[i].used == 1)
        {
            calc_psnr(imgblist_inp[i].imgb, imgb_rec, psnr, op_bit_depth_internal);
            imgblist_inp[i].used = 0;
            return 0;
        }
    }
    return -1;
}

static int write_rec(IMGB_LIST *list, COM_MTIME *ts, int bit_depth_write, int write_rec_flag, char *filename)
{
    int i;
    for(i=0; i<MAX_BUMP_FRM_CNT; i++)
    {
        if(list[i].ts == (*ts) && list[i].used == 1)
        {
            if (write_rec_flag)
            {
                if (imgb_write(filename, list[i].imgb, bit_depth_write))
                {
                    //v0print("cannot write reconstruction image\n");
                    return -1;
                }
            }

            list[i].used = 0;
            (*ts)++;
            break;
        }
    }

    return 0;
}

void print_psnr(ENC_STAT * stat, double * psnr, int bitrate, COM_CLK clk_end)
{

#if LIBVC_ON
    char* stype;
#else
    char  stype;
#endif

    int i, j;

#if LIBVC_ON
    if (op_tool_libpic && stat->is_RLpic_flag)
    {
        stype = "RL";
    }
    else
    {
        switch (stat->stype)
        {
        case COM_ST_I:
            stype = (char  *)"I";
            break;
        case COM_ST_P:
            stype = (char  *)"P";
            break;
        case COM_ST_B:
            stype = (char  *)"B";
            break;
        case COM_ST_UNKNOWN:
        default:
            stype = (char  *)"U";
            break;
        }
    }
#else
    switch (stat->stype)
    {
    case COM_ST_I:
        stype = 'I';
        break;
    case COM_ST_P:
        stype = 'P';
        break;
    case COM_ST_B:
        stype = 'B';
        break;
    case COM_ST_UNKNOWN:
    default:
        stype = 'U';
        break;
    }
#endif



#if LIBVC_ON
    printf("%-7d(%2s) %-5d%-10.4f%-10.4f%-10.4f%-10d%-10d", \
        stat->poc, stype, stat->qp, psnr[0], psnr[1], psnr[2], \
        bitrate, com_clk_msec(clk_end));
#else
    v1print("%-7d(%c) %-5d%-10.4f%-10.4f%-10.4f%-10d%-10d%", \
        stat->poc, stype, stat->qp, psnr[0], psnr[1], psnr[2], \
        bitrate, com_clk_msec(clk_end), ms_ssim);
#endif

    for (i = 0; i < 2; i++)
    {
#if LIBVC_ON
        if (op_tool_libpic && stat->is_RLpic_flag)
        {
            v1print("[Lib%d ", i);
        }
        else
#endif

        {
            v1print("[L%d ", i);
        }
        for (j = 0; j < stat->refpic_num[i]; j++) printf("%d ", stat->refpic[i][j]);
        printf("] ");
    }
    printf("\n");
    print_flush(stdout);
}

int setup_bumping(ENC id)
{
    ENC_CTX* ctx = (ENC_CTX *)id;
    printf("Entering bumping process... \n");
    ctx->param.force_output = 1;
    ctx->pic_ticnt = ctx->pic_icnt;

    return 0;
}




#if LIBVC_ON
void print_libenc_data(LibVCData libvc_data)
{
    v1print("  \n\nTotal bits of libpic dependency file(bits) : %-.0f\n", libvc_data.bits_dependencyFile);
    v1print("  Total libpic frames : %d ,\tTotal libpic bits(bits) : %-.0f\n", libvc_data.num_lib_pic, libvc_data.bits_libpic);
}

void set_livcdata_enc(ENC id_lib, LibVCData* libvc_data)
{
    ENC_CTX* tmp_ctx = (ENC_CTX*)id_lib;
    tmp_ctx->rpm.libvc_data = libvc_data;
}

double cnt_libpic_dependency_info(char* fname, int FileOpenFlag, LibVCData libvc_data)
{
    // library_id
    int size = libvc_data.num_lib_pic;
    unsigned char ucIdLib, ucIdHighSystem, ucIdLowSystm;

    //write file
    if (size > 0 && FileOpenFlag)
    {
        /* libic information file - remove contents and close */
        FILE* fp;
        fp = fopen(op_fname_libdependency, "wb");
        if (fp == NULL)
        {
            v0print("cannot open libpic information file (%s)\n", op_fname_libdependency);
            return -1;
        }

        fclose(fp);
        for (short i = 0; i < size; i++)
        {
            ucIdLib = (unsigned char)i & 0x00FF;
            ucIdHighSystem = (unsigned char)(i >> 8);
            ucIdLowSystm = (unsigned char)(i & 0x00FF);

            write_data(fname, &ucIdLib, 1, NOT_END_OF_VIDEO_SEQUENCE);
            write_data(fname, &ucIdHighSystem, 1, NOT_END_OF_VIDEO_SEQUENCE);
            write_data(fname, &ucIdLowSystm, 1, NOT_END_OF_VIDEO_SEQUENCE);

        }
    }

    //count the file bits
    double bits_dependencyfile = (double)(size * (double)(sizeof(ucIdLib)) * 3 * 8);

    return bits_dependencyfile;
}

int run_extract_feature(LibVCData* libvc_data, int candpic_idx, COM_IMGB* imgb_tmp)
{
    //// === extract feature from library picture candidates === //

    // set histogram information.
    PIC_HIST_FEATURE* feature_of_candidate_pic = &libvc_data->list_hist_feature_of_candidate_pic[candpic_idx];
    feature_of_candidate_pic->num_component = 3;
    feature_of_candidate_pic->num_of_hist_interval = 1 << op_bit_depth_input;
    feature_of_candidate_pic->length_of_interval = 1;

    // create histogram and entropy.

    for (int i = 0; i < feature_of_candidate_pic->num_component; i++)
    {
        if (feature_of_candidate_pic->list_hist_feature[i] == NULL)
        {
            feature_of_candidate_pic->list_hist_feature[i] = (int*)malloc(feature_of_candidate_pic->num_of_hist_interval * sizeof(int));
            if (feature_of_candidate_pic->list_hist_feature[i] == NULL)
            {
                v0print("cannot create feature buffer\n");
                return -1;
            }
            memset(feature_of_candidate_pic->list_hist_feature[i], 0, feature_of_candidate_pic->num_of_hist_interval * sizeof(int));
        }
    }

    for (int i = 0; i < feature_of_candidate_pic->num_component; i++)
    {
        unsigned short* pixel;

        int height = imgb_tmp->height[i];
        int width = imgb_tmp->width[i];
        int idx_interval = -1;

        int length_of_interval = feature_of_candidate_pic->length_of_interval;
        int num_of_hist_interval = feature_of_candidate_pic->num_of_hist_interval;

        // reset histogram to 0.
        memset(feature_of_candidate_pic->list_hist_feature[i], 0, feature_of_candidate_pic->num_of_hist_interval * sizeof(int));


        pixel = (unsigned short*)imgb_tmp->addr_plane[i];

        for (int ih = 0; ih < height; ih++)
        {
            for (int iw = 0; iw < width; iw++)
            {
                idx_interval = pixel[iw] / length_of_interval;

                if (idx_interval < 0 || idx_interval > num_of_hist_interval)
                {
                    return -1;
                }
                feature_of_candidate_pic->list_hist_feature[i][idx_interval]++;
            }
            pixel += width;
        }
    }

    return 0;
}

int run_decide_libpic_candidate_set_and_extract_feature(ENC_PARAM param_in, LibVCData* libvc_data)
{
    FILE* fp_inp_tmp = NULL;
    STATES          state_tmp = STATE_ENCODING;
    int             pic_skip_tmp, pic_icnt_tmp;
    int             candpic_cnt;
    COM_IMGB* imgb_tmp = NULL;
    int             pic_width;
    int             pic_height;
    int             ret_tmp;

    fp_inp_tmp = fopen(op_fname_inp, "rb");
    if (fp_inp_tmp == NULL)
    {
        v0print("cannot open original file (%s)\n", op_fname_inp);
        print_usage();
        return -1;
    }

    pic_width = param_in.pic_width;
    pic_height = param_in.pic_height;
    imgb_tmp = imgb_alloc(pic_width, pic_height, COM_COLORSPACE_YUV420, 10);
    imgb_tmp->horizontal_size = param_in.horizontal_size;
    imgb_tmp->vertical_size = param_in.vertical_size;

    if (imgb_tmp == NULL)
    {
        v0print("cannot allocate image \n");
        return -1;
    }

    pic_skip_tmp = 0;
    pic_icnt_tmp = 0;
    candpic_cnt = 0;
    if ((op_flag[OP_FLAG_SKIP_FRAMES] && op_skip_frames > 0) || (op_flag[OP_FLAG_SKIP_FRAMES_WHEN_EXTRACT_LIBPIC] && op_skip_frames_when_extract_libpic > 0))
    {
        state_tmp = STATE_SKIPPING;
    }

    while (1)
    {
        if (state_tmp == STATE_SKIPPING)
        {
            if (pic_skip_tmp < op_skip_frames_when_extract_libpic)
            {
                if (imgb_read_conv(fp_inp_tmp, imgb_tmp, param_in.bit_depth_input, param_in.bit_depth_input))
                {
                    v2print("reached end of original file (or reading error) when skip frames\n");
                    return -1;
                }
            }
            else
            {
                state_tmp = STATE_ENCODING;
            }
            pic_skip_tmp++;
            continue;
        }
        if (state_tmp == STATE_ENCODING)
        {
            /* read original image */
            if (pic_icnt_tmp == op_max_frm_num_when_extract_libpic || imgb_read_conv(fp_inp_tmp, imgb_tmp, param_in.bit_depth_input, param_in.bit_depth_input))
            {
                v2print("reached end of original file (or reading error)\n");
                break;
            }
            else if (pic_icnt_tmp != candpic_cnt * param_in.i_period)
            {
                pic_icnt_tmp++;
                continue;
            }

            //store library picture candidate set
            libvc_data->num_candidate_pic++;
            libvc_data->list_poc_of_candidate_pic[candpic_cnt] = candpic_cnt * param_in.i_period;
            libvc_data->list_candidate_pic[candpic_cnt] = imgb_tmp;

            //extract feature from library picture candidates
            ret_tmp = run_extract_feature(libvc_data, candpic_cnt, imgb_tmp);
            if (ret_tmp)
            {
                v0print("error when extract feature\n");
                return -1;
            }

            pic_icnt_tmp++;
            candpic_cnt++;

            imgb_tmp = NULL;
            imgb_tmp = imgb_alloc(pic_width, pic_height, COM_COLORSPACE_YUV420, 10);
            imgb_tmp->horizontal_size = param_in.horizontal_size;
            imgb_tmp->vertical_size = param_in.vertical_size;

            if (imgb_tmp == NULL)
            {
                v0print("cannot allocate image \n");
                return -1;
            }

        }
    }

    if (imgb_tmp)
    {
        imgb_free(imgb_tmp);
        imgb_tmp = NULL;
    }
    if (fp_inp_tmp) fclose(fp_inp_tmp);

    return 0;
}

double get_mse_of_hist_feature(PIC_HIST_FEATURE* x, PIC_HIST_FEATURE* y)
{
    int num_component = x->num_component;
    int num_of_hist_interval = x->num_of_hist_interval;


    double sum = 0;
    for (int i = 0; i < num_component; i++)
    {
        for (int j = 0; j < num_of_hist_interval; j++)
        {
            sum += (x->list_hist_feature[i][j] - y->list_hist_feature[i][j]) * (x->list_hist_feature[i][j] - y->list_hist_feature[i][j]);
        }
    }

    return sqrt(sum);
}

double get_sad_of_hist_feature(PIC_HIST_FEATURE* x, PIC_HIST_FEATURE* y)
{
    int num_component = x->num_component;
    int num_of_hist_interval = x->num_of_hist_interval;

    double sum = 0;
    for (int i = 0; i < num_component; i++)
    {
        for (int j = 0; j < num_of_hist_interval; j++)
        {
            sum += abs(x->list_hist_feature[i][j] - y->list_hist_feature[i][j]);
        }
    }

    return sqrt(sum);
}

int run_classify(LibVCData* libvc_data, int num_cluster, int* list_cluster_center_idx, int cur_candpic_idx)
{
    PIC_HIST_FEATURE* cur_feature = NULL;
    PIC_HIST_FEATURE* center_feature = NULL;
    double mse;
    double min_mse;
    int cluster_idx;
    int center_idx;

    cur_feature = &libvc_data->list_hist_feature_of_candidate_pic[cur_candpic_idx];

    cluster_idx = 0;
    center_idx = list_cluster_center_idx[cluster_idx];
    center_feature = &libvc_data->list_hist_feature_of_candidate_pic[center_idx];
    min_mse = get_mse_of_hist_feature(cur_feature, center_feature);

    for (int i = 1; i < num_cluster; i++)
    {
        center_idx = list_cluster_center_idx[i];
        center_feature = &libvc_data->list_hist_feature_of_candidate_pic[center_idx];
        mse = get_mse_of_hist_feature(cur_feature, center_feature);
        if (mse < min_mse)
        {
            min_mse = mse;
            cluster_idx = i;
        }
    }

    cur_feature = NULL;
    center_feature = NULL;

    return cluster_idx;
}

double get_total_mse(LibVCData* libvc_data, int* list_cluster_center_idx, int* list_candpic_cluster_idx)
{
    double total_mse = 0;
    int cluster_idx, center_idx;
    PIC_HIST_FEATURE* cur_feature = NULL;
    PIC_HIST_FEATURE* center_feature = NULL;

    for (int i = 0; i < libvc_data->num_candidate_pic; i++)
    {
        cur_feature = &libvc_data->list_hist_feature_of_candidate_pic[i];
        cluster_idx = list_candpic_cluster_idx[i];
        center_idx = list_cluster_center_idx[cluster_idx];
        center_feature = &libvc_data->list_hist_feature_of_candidate_pic[center_idx];

        total_mse += get_mse_of_hist_feature(cur_feature, center_feature);
    }
    cur_feature = NULL;
    center_feature = NULL;

    return total_mse;
}

int update_center(LibVCData* libvc_data, int cluster_member_num, int* cluster_member_idx)
{
    double cluster_mse = MAX_COST;
    double min_cluster_mse = MAX_COST;
    int center_idx = 0;
    int tmp_center_idx, tmp_cur_idx;
    PIC_HIST_FEATURE* cur_feature = NULL;
    PIC_HIST_FEATURE* center_feature = NULL;

    for (int i = 0; i < cluster_member_num; i++)
    {
        tmp_center_idx = cluster_member_idx[i];
        center_feature = &libvc_data->list_hist_feature_of_candidate_pic[tmp_center_idx];

        cluster_mse = 0;
        for (int j = 0; j < cluster_member_num; j++)
        {
            tmp_cur_idx = cluster_member_idx[j];
            cur_feature = &libvc_data->list_hist_feature_of_candidate_pic[tmp_cur_idx];
            cluster_mse += get_mse_of_hist_feature(cur_feature, center_feature);
        }
        if (i == 0)
        {
            min_cluster_mse = cluster_mse;
            center_idx = tmp_center_idx;
        }
        else
        {
            if (cluster_mse < min_cluster_mse)
            {
                min_cluster_mse = cluster_mse;
                center_idx = tmp_center_idx;
            }
        }
    }

    cur_feature = NULL;
    center_feature = NULL;

    return center_idx;

}

int get_a_new_center(LibVCData* libvc_data, int num_cluster, int* list_cluster_center_idx, int* list_candpic_cluster_idx)
{
    double max_mse = -1;
    double mse = 0;
    int new_center_idx = -1;
    int cluster_idx, center_idx;
    PIC_HIST_FEATURE* cur_feature = NULL;
    PIC_HIST_FEATURE* center_feature = NULL;

    for (int i = 0; i < libvc_data->num_candidate_pic; i++)
    {
        cur_feature = &libvc_data->list_hist_feature_of_candidate_pic[i];
        cluster_idx = list_candpic_cluster_idx[i];
        center_idx = list_cluster_center_idx[cluster_idx];
        center_feature = &libvc_data->list_hist_feature_of_candidate_pic[center_idx];

        mse = get_mse_of_hist_feature(cur_feature, center_feature);
        if (mse > max_mse)
        {
            max_mse = mse;
            new_center_idx = i;
        }

    }

    cur_feature = NULL;
    center_feature = NULL;

    return new_center_idx;
}

double get_cluster_cost(LibVCData* libvc_data, int num_cluster, int* list_cluster_center_idx, int* list_cluster_member_num, int list_cluster_member_idx[MAX_CANDIDATE_PIC][MAX_CANDIDATE_PIC])
{
    double normalize;
    double center_cost, inter_cost, delta_inter_cost;
    double cluster_cost;
    double list_center_cost[MAX_CANDIDATE_PIC];
    double list_inter_cost[MAX_CANDIDATE_PIC];
    double* joint_distribution = NULL;
    int center_idx, member_idx;
    COM_IMGB* center_pic = NULL;
    COM_IMGB* member_pic = NULL;
    unsigned short* center_pixel_org = NULL;
    unsigned short* center_pixel_y = NULL;
    unsigned short* member_pixel_y = NULL;
    PIC_HIST_FEATURE* center_feature = NULL;

    int width = libvc_data->list_candidate_pic[0]->width[0];
    int height = libvc_data->list_candidate_pic[0]->height[0];
    int num_of_hist_interval = libvc_data->list_hist_feature_of_candidate_pic[0].num_of_hist_interval;


    for (int i = 0; i < MAX_CANDIDATE_PIC; i++)
    {
        list_center_cost[i] = 0;
        list_inter_cost[i] = 0;
    }

    for (int i = 0; i < num_cluster; i++)
    {
        center_cost = 0;
        center_idx = list_cluster_center_idx[i];
        center_feature = &libvc_data->list_hist_feature_of_candidate_pic[center_idx];
        for (int n = 0; n < num_of_hist_interval; n++)
        {
            normalize = (double)center_feature->list_hist_feature[0][n] / (double)(width * height);
            center_cost += normalize ? normalize * (log(normalize) / log(2.0)) : 0;
        }
        center_cost = -center_cost;
        list_center_cost[i] = center_cost;
        center_feature = NULL;
    }


    for (int i = 0; i < num_cluster; i++)
    {
        inter_cost = 0;
        center_idx = list_cluster_center_idx[i];
        center_pic = libvc_data->list_candidate_pic[center_idx];
        center_pixel_org = (unsigned short*)center_pic->addr_plane[0];

        joint_distribution = (double*)malloc(sizeof(double) * num_of_hist_interval * num_of_hist_interval);
        for (int j = 0; j < list_cluster_member_num[i]; j++)
        {
            delta_inter_cost = 0;
            center_pixel_y = center_pixel_org;

            // initialize
            for (int k = 0; k < num_of_hist_interval; k++)
            {
                for (int t = 0; t < num_of_hist_interval; t++)
                {
                    joint_distribution[k * num_of_hist_interval + t] = 0;
                }
            }

            member_idx = list_cluster_member_idx[i][j];
            member_pic = libvc_data->list_candidate_pic[member_idx];
            member_pixel_y = (unsigned short*)member_pic->addr_plane[0];

            for (int k = 0; k < height; k++)
            {
                for (int t = 0; t < width; t++)
                {
                    joint_distribution[center_pixel_y[t] * num_of_hist_interval + member_pixel_y[t]]++;
                }

                center_pixel_y += width;
                member_pixel_y += width;

            }
            for (int k = 0; k < num_of_hist_interval; k++)
            {
                for (int t = 0; t < num_of_hist_interval; t++)
                {
                    normalize = joint_distribution[k * num_of_hist_interval + t] / (width * height);
                    delta_inter_cost += normalize ? normalize * (log(normalize) / log(2.0)) : 0;
                }
            }
            inter_cost += -delta_inter_cost - list_center_cost[i];
        }
        list_inter_cost[i] = inter_cost;
        free(joint_distribution);
        joint_distribution = NULL;
        center_pixel_org = NULL;
        center_pixel_y = NULL;
        member_pixel_y = NULL;
        center_pic = NULL;
        member_pic = NULL;

    }

    cluster_cost = 0;
    for (int i = 0; i < num_cluster; i++)
    {

        if (list_cluster_member_num[i] == 1)
        {
            cluster_cost += list_center_cost[i];
        }
        else
        {
            cluster_cost += list_center_cost[i];
            cluster_cost += list_inter_cost[i];
        }
    }
    return cluster_cost;
}

double k_means(LibVCData* libvc_data, int num_cluster, int* list_init_cluster_center_idx, int* list_candpic_cluster_center_idx, int* list_final_cluster_member_num)
{
    double total_mse_old, total_mse_new;
    int cluster_idx = 0;
    static int list_cluster_center_idx[MAX_CANDIDATE_PIC];
    static int list_cluster_member_num[MAX_CANDIDATE_PIC];
    static int list_cluster_member_idx[MAX_CANDIDATE_PIC][MAX_CANDIDATE_PIC];
    static int list_candpic_cluster_idx[MAX_CANDIDATE_PIC];
    double cluster_cost;

    for (int i = 0; i < MAX_CANDIDATE_PIC; i++)
    {
        list_candpic_cluster_idx[i] = -1;
        list_cluster_center_idx[i] = -1;
        list_cluster_member_num[i] = 0;
        for (int j = 0; j < MAX_CANDIDATE_PIC; j++)
        {
            list_cluster_member_idx[i][j] = -1;
        }
    }

    // set original cluster centers
    for (int i = 0; i < num_cluster; i++)
    {
        list_cluster_center_idx[i] = list_init_cluster_center_idx[i];
    }
    // classify each candpic according to the original cluster centers
    for (int candpic_idx = 0; candpic_idx < libvc_data->num_candidate_pic; candpic_idx++)
    {
        cluster_idx = run_classify(libvc_data, num_cluster, list_cluster_center_idx, candpic_idx);
        list_candpic_cluster_idx[candpic_idx] = cluster_idx;
        list_cluster_member_idx[cluster_idx][list_cluster_member_num[cluster_idx]] = candpic_idx;
        list_cluster_member_num[cluster_idx]++;
    }
    total_mse_old = -1;
    total_mse_new = get_total_mse(libvc_data, list_cluster_center_idx, list_candpic_cluster_idx);


    while (fabs(total_mse_new - total_mse_old) >= 1)
    {
        total_mse_old = total_mse_new;
        // update cluster means
        for (int i = 0; i < num_cluster; i++)
        {
            list_cluster_center_idx[i] = update_center(libvc_data, list_cluster_member_num[i], list_cluster_member_idx[i]);
            // clear
            for (int j = 0; j < list_cluster_member_num[i]; j++)
            {
                list_cluster_member_idx[i][j] = -1;
            }
            list_cluster_member_num[i] = 0;
        }
        total_mse_new = get_total_mse(libvc_data, list_cluster_center_idx, list_candpic_cluster_idx);

        // classify each candpic according to the original cluster means
        for (int candpic_idx = 0; candpic_idx < libvc_data->num_candidate_pic; candpic_idx++)
        {
            cluster_idx = run_classify(libvc_data, num_cluster, list_cluster_center_idx, candpic_idx);
            list_candpic_cluster_idx[candpic_idx] = cluster_idx;
            list_cluster_member_idx[cluster_idx][list_cluster_member_num[cluster_idx]] = candpic_idx;
            list_cluster_member_num[cluster_idx]++;
        }
        //total_mse_new = get_total_mse(libvc_data, list_cluster_center_idx, list_candpic_cluster_idx);
    }


    for (int i = 0; i < libvc_data->num_candidate_pic; i++)
    {
        cluster_idx = list_candpic_cluster_idx[i];
        list_candpic_cluster_center_idx[i] = list_cluster_center_idx[cluster_idx];
    }
    for (int i = 0; i < num_cluster; i++)
    {
        list_final_cluster_member_num[i] = list_cluster_member_num[i];
    }

    // get the initial point of the next new cluster
    for (int i = 0; i < num_cluster; i++)
    {
        list_init_cluster_center_idx[i] = list_cluster_center_idx[i];
    }
    list_init_cluster_center_idx[num_cluster] = get_a_new_center(libvc_data, num_cluster, list_cluster_center_idx, list_candpic_cluster_idx);

    // get the final cluster cost
    cluster_cost = get_cluster_cost(libvc_data, num_cluster, list_cluster_center_idx, list_cluster_member_num, list_cluster_member_idx);


    return cluster_cost;
}

void Encoder::run_generate_libpic(LibVCData* libvc_data, int* list_libpic_idx)
{
    // === process kmeans to generate library picture === //

    double tmp_cluster_cost, best_cluster_cost;
    int tmp_num_cluster, best_num_cluster;
    int tmp_list_candpic_cluster_center_idx[MAX_CANDIDATE_PIC];
    int list_final_candpic_cluster_center_idx[MAX_CANDIDATE_PIC];
    int list_init_cluster_center_idx[MAX_NUM_LIBPIC + 1];
    int list_final_cluster_center_idx[MAX_NUM_LIBPIC];
    int tmp_list_final_cluster_member_num[MAX_NUM_LIBPIC];
    int list_final_cluster_member_num[MAX_NUM_LIBPIC];

    int num_libpic;
    int list_poc_of_libpic[MAX_NUM_LIBPIC];
    int max_num_cluster;

    // initialize all list.
    for (int i = 0; i < MAX_CANDIDATE_PIC; i++)
    {
        tmp_list_candpic_cluster_center_idx[i] = -1;
        list_final_candpic_cluster_center_idx[i] = -1;
    }
    for (int i = 0; i < MAX_NUM_LIBPIC; i++)
    {
        list_poc_of_libpic[i] = -1;
        list_init_cluster_center_idx[i] = -1;
        list_final_cluster_center_idx[i] = -1;
        tmp_list_final_cluster_member_num[i] = 0;
        list_final_cluster_member_num[i] = 0;
    }
    list_init_cluster_center_idx[MAX_NUM_LIBPIC] = -1;

    // cluster candidate pic by using k_means && choose the best cluster result
    // initialize k_means with num_cluster = 1;
    tmp_num_cluster = 1;
    //list_init_cluster_center_idx[0] = rand() % libvc_data->num_candidate_pic;
    list_init_cluster_center_idx[0] = 0;
    tmp_cluster_cost = k_means(libvc_data, tmp_num_cluster, list_init_cluster_center_idx, tmp_list_candpic_cluster_center_idx, tmp_list_final_cluster_member_num);
    best_num_cluster = tmp_num_cluster;
    best_cluster_cost = tmp_cluster_cost;
    for (int i = 0; i < libvc_data->num_candidate_pic; i++)
    {
        list_final_candpic_cluster_center_idx[i] = tmp_list_candpic_cluster_center_idx[i];
    }
    for (int i = 0; i < best_num_cluster; i++)
    {
        list_final_cluster_center_idx[i] = list_init_cluster_center_idx[i];
        list_final_cluster_member_num[i] = tmp_list_final_cluster_member_num[i];
    }

    max_num_cluster = avs3_min(libvc_data->num_candidate_pic, MAX_NUM_LIBPIC);
    for (tmp_num_cluster = 2; tmp_num_cluster <= max_num_cluster; tmp_num_cluster++)
    {
        int is_break = 0;
        for (int i = 0; i < tmp_num_cluster - 1; i++)
        {
            // the last one center is the new added on.
            if (list_init_cluster_center_idx[tmp_num_cluster - 1] == list_init_cluster_center_idx[i])
            {
                is_break = 1;
                break;
            }
        }
        if (is_break)
        {
            break;
        }

        tmp_cluster_cost = k_means(libvc_data, tmp_num_cluster, list_init_cluster_center_idx, tmp_list_candpic_cluster_center_idx, tmp_list_final_cluster_member_num);
        if (tmp_cluster_cost < best_cluster_cost)
        {
            best_cluster_cost = tmp_cluster_cost;
            best_num_cluster = tmp_num_cluster;
            for (int t = 0; t < libvc_data->num_candidate_pic; t++)
            {
                list_final_candpic_cluster_center_idx[t] = tmp_list_candpic_cluster_center_idx[t];
            }
            for (int k = 0; k < best_num_cluster; k++)
            {
                list_final_cluster_center_idx[k] = list_init_cluster_center_idx[k];
                list_final_cluster_member_num[k] = tmp_list_final_cluster_member_num[k];
            }
        }
    }

    // avoid the cluster with only one library picture.
    num_libpic = 0;
    for (int i = 0; i < best_num_cluster; i++)
    {
        if (list_final_cluster_member_num[i] > 1)
        {
            int center_idx = list_final_cluster_center_idx[i];
            list_libpic_idx[num_libpic] = center_idx;
            list_poc_of_libpic[num_libpic] = libvc_data->list_poc_of_candidate_pic[center_idx];
            num_libpic++;
        }
    }
    num_libpic = avs3_min(MAX_NUM_LIBPIC, num_libpic);
    //reorder the library frame according to POC value
    for (int i = 0; i < num_libpic; i++)
    {
        for (int j = i + 1; j < num_libpic; j++)
        {
            if (list_poc_of_libpic[i] > list_poc_of_libpic[j])
            {
                int tmp = list_poc_of_libpic[i];
                list_poc_of_libpic[i] = list_poc_of_libpic[j];
                list_poc_of_libpic[j] = tmp;
                tmp = list_libpic_idx[i];
                list_libpic_idx[i] = list_libpic_idx[j];
                list_libpic_idx[j] = tmp;
            }
        }
    }

    // store the libpic infomation
    int num_libpic_candidate = 0;
    if(param_input.i_period)
        num_libpic_candidate = op_max_frm_num_when_extract_libpic / param_input.i_period;
 #if M5422
    libvc_data->num_lib_pic = 1;
    libvc_data->i_period = op_i_period;
 #else
    num_libpic = libvc_data->num_candidate_pic / 2;
    libvc_data->num_lib_pic = num_libpic;
#endif
#if M5422      
     libvc_data->list_poc_of_libpic[0] = 0;       
#else
    for (int i = 0; i < libvc_data->num_lib_pic; i++)
    {
        //libvc_data->list_poc_of_libpic[i] = list_poc_of_libpic[i];
        libvc_data->list_poc_of_libpic[i] = i * param_input.i_period * 2;
    }
#endif	
}

void Encoder::run_decide_pic_referencing_libpic(LibVCData* libvc_data, int* list_libpic_idx)
{
    // decide sequence pictures that reference library picture

    PIC_HIST_FEATURE* cur_feature = NULL;
    PIC_HIST_FEATURE* lib_feature = NULL;
    int list_candpic_libidx[MAX_CANDIDATE_PIC];

    double min_diff = MAX_DIFFERENCE_OF_RLPIC_AND_LIBPIC;
    int num_RLpic;


    for (int i = 0; i < MAX_CANDIDATE_PIC; i++)
    {
        list_candpic_libidx[i] = -1;
    }

    /*for (int candpic_idx = 0; candpic_idx < libvc_data->num_candidate_pic; candpic_idx++)
    {
        cur_feature = &libvc_data->list_hist_feature_of_candidate_pic[candpic_idx];

        for (int libpid = 0; libpid < libvc_data->num_lib_pic; libpid++)
        {
            lib_idx = list_libpic_idx[libpid];
            lib_feature = &libvc_data->list_hist_feature_of_candidate_pic[lib_idx];
            diff = get_sad_of_hist_feature(cur_feature, lib_feature);
            if (diff < min_diff)
            {
                min_diff = diff;
                list_candpic_libidx[candpic_idx] = libpid;
            }
        }
        min_diff = MAX_DIFFERENCE_OF_RLPIC_AND_LIBPIC;
    }
    cur_feature = NULL;
    lib_feature = NULL;*/

    // store the RLpic infomation
    num_RLpic = 0;
    int seperate_I = op_max_frm_num_when_extract_libpic - 1  - libvc_data->list_poc_of_candidate_pic[libvc_data->num_candidate_pic - 1];
    for (int candpic_idx = 0; candpic_idx < libvc_data->num_candidate_pic; candpic_idx++)
    {        
        libvc_data->list_poc_of_RLpic[num_RLpic] = libvc_data->list_poc_of_candidate_pic[candpic_idx];
#if M5422
        libvc_data->list_libidx_for_RLpic[num_RLpic] = 0;
#else
        libvc_data->list_libidx_for_RLpic[num_RLpic] = num_RLpic / 2;
#endif
        num_RLpic++;    
    }

    if (seperate_I < param_input.i_period && seperate_I >= 0) {
        libvc_data->list_libidx_for_RLpic[libvc_data->num_candidate_pic - 1] = libvc_data->list_libidx_for_RLpic[libvc_data->num_candidate_pic - 2];
    }

	//only one I frame in squence, libpic set off, means no RLpic
	if (num_RLpic == 1) {
		num_RLpic = 0;
	}
    libvc_data->num_RLpic = num_RLpic;
	


    //clear buffer
    for (int i = 0; i < libvc_data->num_candidate_pic; i++)
    {
        if (libvc_data->list_candidate_pic[i])
        {
            imgb_free(libvc_data->list_candidate_pic[i]);
            libvc_data->list_candidate_pic[i] = NULL;
        }
        for (int j = 0; j < MAX_NUM_COMPONENT; j++)
        {
            if (libvc_data->list_hist_feature_of_candidate_pic[i].list_hist_feature[j])
            {
                free(libvc_data->list_hist_feature_of_candidate_pic[i].list_hist_feature[j]);
                libvc_data->list_hist_feature_of_candidate_pic[i].list_hist_feature[j] = NULL;
            }
        }
    }

}

int Encoder::compute_LibVCData(ENC_PARAM param_in, LibVCData* libvc_data)
{
    int ret = 0;
    int list_libpic_idx[MAX_NUM_LIBPIC];
    for (int i = 0; i < MAX_NUM_LIBPIC; i++)
    {
        list_libpic_idx[i] = -1;
    }

    /*
    for (int i = 0; i * param_in.i_period < param_in.frames_to_be_encoded; i++)
    {
        libvc_data->list_poc_of_RLpic[i] = i * param_in.i_period;
        libvc_data->num_RLpic++;
    }

    libvc_data->num_lib_pic = 3;
    for (int i = 0; i < libvc_data->num_lib_pic; i++)
    {
        libvc_data->list_poc_of_libpic[i] = libvc_data->list_poc_of_RLpic[i];
    }
    for (int i = 0; i < libvc_data->num_RLpic / 3; i++)
    {
        libvc_data->list_libidx_for_RLpic[i] = 0;
    }
    for (int i = libvc_data->num_RLpic / 3; i < libvc_data->num_RLpic * 2 / 3; i++)
    {
        libvc_data->list_libidx_for_RLpic[i] = 1;
    }
    for (int i = libvc_data->num_RLpic * 2 / 3; i < libvc_data->num_RLpic; i++)
    {
        libvc_data->list_libidx_for_RLpic[i] = 2;
    }
    */

    // step 1
    ret = run_decide_libpic_candidate_set_and_extract_feature(param_in, libvc_data);
    if (ret)
    {
        // error when decide libpic candidate set and extract feature, clear buffer
        for (int i = 0; i < MAX_CANDIDATE_PIC; i++)
        {
            if (libvc_data->list_candidate_pic[i])
            {
                imgb_free(libvc_data->list_candidate_pic[i]);
                libvc_data->list_candidate_pic[i] = NULL;
            }
            for (int j = 0; j < MAX_NUM_COMPONENT; j++)
            {
                if (libvc_data->list_hist_feature_of_candidate_pic[i].list_hist_feature[j])
                {
                    free(libvc_data->list_hist_feature_of_candidate_pic[i].list_hist_feature[j]);
                    libvc_data->list_hist_feature_of_candidate_pic[i].list_hist_feature[j] = NULL;
                }
            }
        }

        return -1;
    }

    // step 2
    run_generate_libpic(libvc_data, list_libpic_idx);

    // step 3
    run_decide_pic_referencing_libpic(libvc_data, list_libpic_idx);

    // stat libdependency info
    libvc_data->bits_dependencyFile = cnt_libpic_dependency_info(op_fname_libdependency, op_flag[OP_FLAG_FNAME_LIBDATA], *libvc_data);


    return 0;
}


#endif


int Configure(int argc, const char * argv[], COM_ARGS_OPTION * ops, int ret)
{

	/* parse options */
	ret = com_args_parse_all(argc, argv, options);
#if LIBVC_ON
    if (op_flag[OP_FLAG_FNAME_OUT])
    {
        /* libic bitstream file - remove contents and close */
        FILE* fp;
        fp = fopen(op_fname_libout, "wb");
        if (fp == NULL)
        {
            v0print("cannot open libpic bitstream file (%s)\n", op_fname_libout);
            return -1;
        }
        fclose(fp);
    }
    if (op_flag[OP_FLAG_FNAME_LIBREC])
    {
        /* reconstruction libic file - remove contents and close */
        FILE* fp;
        fp = fopen(op_fname_librec, "wb");
        if (fp == NULL)
        {
            v0print("cannot open reconstruction libpic file (%s)\n", op_fname_librec);
            return -1;
        }
        fclose(fp);
    }
#endif
	if (ret != 0)
	{
		//if (ret > 0) v0print("-%c argument should be set\n", ret);
		//if (ret < 0) v0print("Configuration error, please refer to the usage.\n");
		print_usage();
		return -1;
	}
	if (op_flag[OP_FLAG_FNAME_OUT])
	{
		/* bitstream file - remove contents and close */
		FILE * fp;
		fp = fopen(op_fname_out, "wb");
		if (fp == NULL)
		{
			printf("cannot open bitstream file (%s)\n", op_fname_out);
			return -1;
		}
		fclose(fp);
	}
	if (op_flag[OP_FLAG_FNAME_REC])
	{
		/* reconstruction file - remove contents and close */
		FILE * fp;
		fp = fopen(op_fname_rec, "wb");
		if (fp == NULL)
		{
			printf("cannot open reconstruction file (%s)\n", op_fname_rec);
			return -1;
		}
		fclose(fp);
	}

	return COM_OK;
}
int Encoder::AllocateBitstream()
{
#if LIBVC_ON
    bs_buf_lib = (unsigned char*)malloc(MAX_BS_BUF);
    if (bs_buf_lib == NULL)
    {
        v0print("cannot allocate bitstream buffer, size=%d", MAX_BS_BUF);
        return -1;
    }
    bs_buf_lib2 = (unsigned char*)malloc(MAX_BS_BUF);
    if (bs_buf_lib2 == NULL)
    {
        v0print("ERROR: cannot allocate bit buffer, size2=%d\n", MAX_BS_BUF);
        return -1;
    }
#endif

	/* allocate bitstream buffer */
	bs_buf = (unsigned char*)malloc(MAX_BS_BUF);
	if (bs_buf == NULL)
	{
		//v0print("cannot allocate bitstream buffer, size=%d", MAX_BS_BUF);
		return -1;
	}
	bs_buf2 = (unsigned char*)malloc(MAX_BS_BUF);
	if (bs_buf2 == NULL)
	{
		//v0print("cannot allocate bitstream buffer, size=%d", MAX_BS_BUF);
		return -1;
	}

	return COM_OK;
}


int Encoder::firmware_init()
{
    /* open original file */
#if LIBVC_ON
    fp_inp_lib = fopen(op_fname_inp, "rb");
    if (fp_inp_lib == NULL)
    {
        v0print("cannot open original file (%s)\n", op_fname_inp);
        print_usage();
        return -1;
    }
#endif
	/* open original file */
	fp_inp = fopen(op_fname_inp, "rb");
	if (fp_inp == NULL)
	{
		//v0print("cannot open original file (%s)\n", op_fname_inp);
		print_usage();
		return -1;
	}

#if AEC_STAGE1_DATA
  if(fp_stage1_InStream == NULL){
    char fp_stage1_InStream_name[300];
    int j;
    j = sprintf(fp_stage1_InStream_name,"%s",op_fname_out); 
    sprintf(fp_stage1_InStream_name+j,"_%s","stage1_InStream.dat"); 
		fp_stage1_InStream = fopen(fp_stage1_InStream_name, "w");
  }

  if(fp_stage1_OutStream == NULL){
    char fp_stage1_OutStream_name[300];
    int j;
    j = sprintf(fp_stage1_OutStream_name,"%s",op_fname_out); 
    sprintf(fp_stage1_OutStream_name+j,"_%s","stage1_OutStream.dat"); 
		fp_stage1_OutStream = fopen(fp_stage1_OutStream_name, "w");
  }
#endif

#if AEC_STAGE2_DATA
  if (fp_stage2_InStream == NULL) {
      char fp_stage2_InStream_name[300];
      int j;
      j = sprintf(fp_stage2_InStream_name, "%s", op_fname_out);
      sprintf(fp_stage2_InStream_name + j, "_%s", "stage2_InStream.dat");
      fp_stage2_InStream = fopen(fp_stage2_InStream_name, "w");
  }

  if (fp_stage2_OutStream == NULL) {
      char fp_stage2_OutStream_name[300];
      int j;
      j = sprintf(fp_stage2_OutStream_name, "%s", op_fname_out);
      sprintf(fp_stage2_OutStream_name + j, "_%s", "stage2_OutStream.dat");
      fp_stage2_OutStream = fopen(fp_stage2_OutStream_name, "w");
  }
#endif
#if AEC_STAGE3_DEBUG
#if AEC_STAGE3_CTX_DEBUG
	if (fp_ctx == NULL) {
    char fp_ctx_name[300];
    int j;
    j = sprintf(fp_ctx_name,"%s",op_fname_out); 
    sprintf(fp_ctx_name+j,"_%s","ctx.dat"); 
		fp_ctx = fopen(fp_ctx_name, "w");
	}
#endif
#if AEC_STAGE3_BS_DEBUG
  if (fp_bs == NULL){
    char fp_bs_name[300];
    int j;
    j = sprintf(fp_bs_name,"%s",op_fname_out); 
    sprintf(fp_bs_name+j,"_%s","bs.dat"); 
    fp_bs = fopen(fp_bs_name,"w");
    //fp_bs = fopen("test_bs.dat","w");
  }
#endif

#if AEC_STAGE3_BINA_DEBUG
  if (fp_bina == NULL){
    char fp_bina_name[300];
    int j;
    j = sprintf(fp_bina_name,"%s",op_fname_out); 
    sprintf(fp_bina_name+j,"_%s","bina.dat"); 
    fp_bina = fopen(fp_bina_name,"w");
    //fp_bs = fopen("test_bs.dat","w");
  }
#endif


#endif

#if AEC_STAGE3_PHASE2_DEBUG
#if AEC_STAGE3_CTX_PHASE2_DEBUG
  if (fp_ctx == NULL) {
      char fp_ctx_name[300];
      int j;
      j = sprintf(fp_ctx_name, "%s", op_fname_out);
      sprintf(fp_ctx_name + j, "_%s", "ctx.dat");
      fp_ctx = fopen(fp_ctx_name, "w");
  }
#endif
#if AEC_STAGE3_BS_PHASE2_DEBUG
  if (fp_bs == NULL) {
      char fp_bs_name[300];
      int j;
      j = sprintf(fp_bs_name, "%s", op_fname_out);
      sprintf(fp_bs_name + j, "_%s", "bs.dat");
      fp_bs = fopen(fp_bs_name, "w");
      //fp_bs = fopen("test_bs.dat","w");
  }
#endif
#if AEC_STAGE3_RLPS_PHASE2_DEBUG
  if (fp_rLPS == NULL) {
      char fp_rLPS_name[300];
      int j;
      j = sprintf(fp_rLPS_name, "%s", op_fname_out);
      sprintf(fp_rLPS_name + j, "_%s", "rLPS.dat");
      fp_rLPS = fopen(fp_rLPS_name, "w");
      //fp_bs = fopen("test_bs.dat","w");
  }
#endif
#if AEC_STAGE3_BINA_PHASE2_DEBUG
  if (fp_bina == NULL) {
      char fp_bina_name[300];
      int j;
      j = sprintf(fp_bina_name, "%s", op_fname_out);
      sprintf(fp_bina_name + j, "_%s", "bina.dat");
      fp_bina = fopen(fp_bina_name, "w");
      //fp_bs = fopen("test_bs.dat","w");
  }
#endif
#endif
	return COM_OK;
}

int Encoder::init_img()
{
	/* read configurations and set values for create descriptor */
	if (get_conf(&param_input))
	{
		print_usage();
		return -1;
	}
#if LIBVC_ON
    if (!op_flag[OP_FLAG_SKIP_FRAMES_WHEN_EXTRACT_LIBPIC])
    {
        op_skip_frames_when_extract_libpic = op_skip_frames;
    }
    if (!op_flag[OP_FLAG_MAX_FRM_NUM_WHEN_EXTRACT_LIBPIC])
    {
        op_max_frm_num_when_extract_libpic = param_input.frames_to_be_encoded;
    }

    init_libvcdata(&libvc_data);

    if (op_tool_libpic)
    {
     
        libvc_data.library_picture_enable_flag = param_input.library_picture_enable_flag;

        ret = compute_LibVCData(param_input, &libvc_data);
        if (ret)
        {
            v0print("Error when compute LibVCData!");
            return -1;
        }
    }
#endif
    if (!op_tool_libpic || libvc_data.num_lib_pic == 0) {
        id = enc_create(&param_input, NULL);
        if (id == NULL)
        {
            //v0print("cannot create encoder\n");
            return -1;
        }
#if LIBVC_ON
        set_livcdata_enc(id, &libvc_data);
#endif
        if (set_extra_config(id))
        {
            //v0print("cannot set extra configurations\n");
            return -1;
        }
    }
	/* create encoder */


	return COM_OK;
}

int Encoder::malloc_picture()
{
	/* create image lists */
#if Debug422
    if (imgb_list_alloc_422(ilist_org, param_input.pic_width, param_input.pic_height, param_input.horizontal_size, param_input.vertical_size, 10))
#else
    if (imgb_list_alloc(ilist_org, param_input.pic_width, param_input.pic_height, param_input.horizontal_size, param_input.vertical_size, 10))
#endif

	{
		//v0print("cannot allocate image list for original image\n");
		return -1;
	}
	if (imgb_list_alloc(ilist_rec, param_input.pic_width, param_input.pic_height, param_input.horizontal_size, param_input.vertical_size, param_input.bit_depth_internal))
	{
		//v0print("cannot allocate image list for reconstructed image\n");
		return -1;
	}

	return COM_OK;
}

void Encoder::parameter_init()
{
	bitrate = 0;
	bitb.addr = bs_buf;
	bitb.addr2 = bs_buf2;
	bitb.bsize = MAX_BS_BUF;
#if WRITE_MD5_IN_USER_DATA //discount user data bits in per-frame bits
	udata_size = (op_use_pic_signature) ? 23 : 0;
#endif
	bitb.err = 0; // update BSB

	clk_tot = 0;
	pic_icnt = 0;
	pic_ocnt = 0;
	pic_skip = 0;
	g_DOIPrev = 0;
    COM_MTIME pic_icnt_zhang = 0;
}

void Encoder::information_init()
{
	print_config(param_input);
	print_stat_init();
	parameter_init();
}

void Encoder::free_picture()
{
	enc_delete(id);
	imgb_list_free(ilist_org);
	imgb_list_free(ilist_rec);
	if (fp_inp) fclose(fp_inp);
	if (bs_buf) free(bs_buf); /* release bitstream buffer */
}


int Encoder::img_buffer_init()
{
	ilist_t = imgb_list_get_empty(ilist_org);
	if (ilist_t == NULL)
	{
		//v0print("cannot get empty orignal buffer\n");
		return -1;
	}

	return COM_OK;
}
COM_MTIME          pic_icnt_zhang;
int Encoder::state_encoding()
{
	skip_frames(fp_inp, ilist_t->imgb, param_input.sub_sample_ratio - 1, param_input.bit_depth_input);
	imgb_list_make_used(ilist_t, pic_icnt);
    /* get encoding buffer */
#if Debug422
    if (COM_OK != enc_picbuf_get_inbuf_422((ENC_CTX*)id, &imgb_enc))
#else
    if (COM_OK != enc_picbuf_get_inbuf((ENC_CTX*)id, &imgb_enc))
#endif
	{
		//v0print("Cannot get original image buffer\n");
		return -1;
	}
	/* copy original image to encoding buffer */
	imgb_cpy_internal(imgb_enc, ilist_t->imgb);
	/* push image to encoder */
	ret = enc_push_frm((ENC_CTX *)id, imgb_enc);
	if (COM_FAILED(ret))
	{
		//v0print("enc_push() failed\n");
		return -1;
	}
	/* release encoding buffer */
	imgb_release(imgb_enc);
	pic_icnt++;
    pic_icnt_zhang = pic_icnt;
	return COM_OK;
}

int Encoder::finish_encode_one_frame()
{
	if (op_flag[OP_FLAG_FNAME_OUT] && stat1.write > 0)
	{
		int end_of_seq = ((param_input.frames_to_be_encoded) == num_encoded_frames) ? END_OF_VIDEO_SEQUENCE : NOT_END_OF_VIDEO_SEQUENCE;
        memcpy(bs_buf_tmp + bs_tmp_size, bs_buf, stat1.write);
        bs_tmp_size += stat1.write;
        //if (write_data(op_fname_out, bs_buf, stat1.write, end_of_seq))
		//{
		//	//v0print("cannot write bitstream\n");
		//	return -1;
		//}
	}
	/* get reconstructed image */
	size = sizeof(COM_IMGB**);
	imgb_rec = PIC_REC((ENC_CTX *)id)->imgb;
	imgb_addref(imgb_rec);
	if (COM_FAILED(ret))
	{
		v0print("failed to get reconstruction image\n");
		return -1;
	}

	/* calculate PSNR */
	if (cal_psnr(ilist_org, imgb_rec, imgb_rec->ts[0], psnr))
	{
		v0print("cannot calculate PSNR\n");
		return -1;
	}

	/* store reconstructed image to list only for writing out */
	ilist_t = store_rec_img(ilist_rec, imgb_rec, imgb_rec->ts[0], param_input.bit_depth_internal);
	if (ilist_t == NULL)
	{
		v0print("cannot put reconstructed image to list\n");
		return -1;
	}

	if (write_rec(ilist_rec, &pic_ocnt, param_input.bit_depth_internal, op_flag[OP_FLAG_FNAME_REC], op_fname_rec))
	{
		v0print("cannot write reconstruction image\n");
		return -1;
	}
    COM_PIC* pic = ((ENC_CTX *)id)->pic[PIC_IDX_ORG];
	if (is_first_enc)
	{
		print_psnr(&stat1, psnr, (stat1.write - udata_size + (int)seq_header_bit) << 3, clk_end);
		is_first_enc = 0;
        pic->picture_bits = (stat1.write - udata_size + (int)seq_header_bit) << 3;
	}
	else
	{
		print_psnr(&stat1, psnr, (stat1.write - udata_size) << 3, clk_end);
        pic->picture_bits = (stat1.write - udata_size) << 3;
	}
	bitrate += (stat1.write - udata_size);
   
    

    char * ext_Info = NULL;

#ifdef    RC_OPEN
    rc_update(&((ENC_CTX*)id)->rc, pic, ext_Info, 0);
#endif


#ifdef LOWDELAY_RC_OPEN
    VCEncAfterPicRc(&((ENC_CTX*)id)->lowdelay_rc, 0, pic->picture_bits / 8, pic->picture_qp, 1);
#endif
	for (i = 0; i < 3; i++) psnr_avg[i] += psnr[i];
	/* release recon buffer */
	if (imgb_rec) imgb_release(imgb_rec);

	return COM_OK;
}
int Encoder::frame_init()
{
    #if !ONLY_VIVADO_HLS
		print_flush(stdout);
#endif
 #if M5422
     float ration = 0.7;
	if (libvc_data.sum_intra_var * ration < libvc_data.sum_inter_var && pic_icnt < param_input.frames_to_be_encoded)
     {
	   for(int num = libvc_data.num_libpic_outside;num<= libvc_data.num_RLpic;num++)
	   {
           libvc_data.list_poc_of_libpic[num] = pic_icnt;
           libvc_data.list_libidx_for_RLpic[num] = libvc_data.num_libpic_outside;
	   }
       
	    libvc_data.num_lib_pic++;
    }
#endif
		if (op_tool_libpic) {
			if (pic_icnt_lib != libvc_data.list_poc_of_libpic[libpic_icnt] && pic_icnt_lib - 2 != pic_icnt && libvc_data.num_lib_pic != 0 && pic_icnt_lib <= encoder_change_condition) {
				ilist_t_lib = imgb_list_get_empty(ilist_org_lib);
				imgb_read_conv(fp_inp_lib, ilist_t_lib->imgb, param_input.bit_depth_input, param_input.bit_depth_internal);
			}
		}

#if M5422
 if (libvc_data.sum_intra_var * ration < libvc_data.sum_inter_var && pic_icnt_lib == libvc_data.list_poc_of_libpic[libpic_icnt] || pic_icnt_lib == 0) {

     libvc_data.sum_intra_var = 0;
     libvc_data.sum_inter_var = 0;
#else
        if (pic_icnt_lib == libvc_data.list_poc_of_libpic[libpic_icnt]) {
#endif
			if (op_tool_libpic) {
				if (libvc_data.num_lib_pic > 0)
				{
					ori_flag = param_input.library_picture_enable_flag;
					ori_iperiod = param_input.i_period;
					ori_max_b_frames = param_input.max_b_frames;
					ori_qp = param_input.qp;

					param_input.i_period = 1;
					param_input.max_b_frames = 0;
					param_input.qp = ori_qp + param_input.qp_offset_libpic;

					libvc_data.is_libpic_processing = 1;
					libvc_data.library_picture_enable_flag = param_input.library_picture_enable_flag = 0;
					param_input.frames_to_be_encoded = param_input.frames_to_be_encoded;// +encoder_change_condition;
				}


				/* id_lib = enc_create(&param_input, NULL);
				 if (id_lib == NULL)
				 {
					 v0print("cannot create encoder\n");
					 return -1;
				 }
				 set_livcdata_enc(id_lib, &libvc_data);*/

				if (pic_icnt_lib == 0) {
					id_lib = enc_create(&param_input, NULL);
					if (id_lib == NULL)
					{
						v0print("cannot create encoder\n");
						return -1;
					}
					set_livcdata_enc(id_lib, &libvc_data);

					if (set_extra_config(id_lib))
					{
						v0print("cannot set extra configurations\n");
						return -1;
					}
					/* create image lists */
#if Debug422
                    if (imgb_list_alloc_422(ilist_org_lib, param_input.pic_width, param_input.pic_height, param_input.horizontal_size, param_input.vertical_size, 10))
#else
                    if (imgb_list_alloc(ilist_org_lib, param_input.pic_width, param_input.pic_height, param_input.horizontal_size, param_input.vertical_size, 10))
#endif

					{
						v0print("cannot allocate image list for original image\n");
						return -1;
					}
					if (imgb_list_alloc(ilist_rec_lib, param_input.pic_width, param_input.pic_height, param_input.horizontal_size, param_input.vertical_size, param_input.bit_depth_internal))
					{
						v0print("cannot allocate image list for reconstructed image\n");
						return -1;
					}
					ret_lib = init_seq_header((ENC_CTX*)id_lib, &bitb_lib);
				}


				if (state_lib == STATE_ENCODING)
				{
					ilist_t_lib = imgb_list_get_empty(ilist_org_lib);
					/*****************code here are defined in img_buffer_init()************* */
					/*****************code here are defined in img_buffer_init()************* */

					if (ilist_t_lib == NULL)
					{
						v0print("cannot get empty orignal buffer\n");
						return -1;
					}

					if (pic_icnt_lib == op_max_frm_num_when_extract_libpic || imgb_read_conv(fp_inp_lib, ilist_t_lib->imgb, param_input.bit_depth_input, param_input.bit_depth_internal) || libpic_icnt == libvc_data.num_lib_pic)
					{
						v2print("reached end of original file (or reading error)\n");
						state_lib = STATE_BUMPING;
						setup_bumping(id_lib);
						return SKIP_LIBPIC;
					}
					if (pic_icnt_lib != libvc_data.list_poc_of_libpic[libpic_icnt])
					{
						pic_icnt_lib++;
						return SKIP_LIBPIC;
					}

					imgb_list_make_used(ilist_t_lib, libpic_icnt);
					libpic_icnt++;

					/* get encodng buffer */
					//com_assert_rv((&imgb_enc) != NULL, COM_ERR_INVALID_ARGUMENT);
#if Debug422
                    if (COM_OK != enc_picbuf_get_inbuf_422((ENC_CTX*)id_lib, &imgb_enc_lib))
#else
                    if (COM_OK != enc_picbuf_get_inbuf((ENC_CTX*)id_lib, &imgb_enc_lib))
#endif
					{
						v0print("Cannot get original image buffer\n");
						return -1;
					}
					/* copy original image to encoding buffer */
					imgb_cpy_internal(imgb_enc_lib, ilist_t_lib->imgb);

					ret_lib = enc_push_frm((ENC_CTX*)id_lib, imgb_enc_lib);
					if (COM_FAILED(ret_lib))
					{
						v0print("enc_push() failed\n");
						return -1;
					}
					/* release encoding buffer */

					imgb_enc_lib->release(imgb_enc_lib);
					pic_icnt_lib++;

					clk_beg_lib = com_clk_get();
				}
			}
        }
        else {
            
            /********************the common process in lib and main************************/
            if (state == STATE_ENCODING)
            {
                img_buffer_init();

				/* read original image */
				if (pic_icnt == param_input.frames_to_be_encoded || imgb_read_conv(fp_inp, ilist_t->imgb, param_input.bit_depth_input, param_input.bit_depth_internal))
				{
					printf("reached end of original file (or reading error)\n");
                    state_lib = STATE_BUMPING;
                    setup_bumping(id_lib);
					return COM_OK;
				}

                state_encoding();
            }
            /* encoding */
            clk_beg = com_clk_get();
        }
        return COM_OK;
}
int Encoder::frame_end(int * end)
{

    if (pic_icnt_lib - 1 == libvc_data.list_poc_of_libpic[libpic_icnt - 1]) {
		if (op_tool_libpic) {
			num_encoded_frames_lib += ret == COM_OK;
			if (COM_FAILED(ret))
			{
				v0print("enc_encode() failed\n");
				return -1;
			}
			clk_end_lib = com_clk_from(clk_beg_lib);
			clk_tot_lib += clk_end_lib;
			/* store bitstream */
			if (ret == COM_OK_OUT_NOT_AVAILABLE)
			{
				v1print("--> RETURN OK BUT PICTURE IS NOT AVAILABLE YET\n");
				//return COM_OK;
			}
			else if (ret == COM_OK)
			{
				if (op_flag[OP_FLAG_FNAME_OUT] && stat_lib.write > 0)
				{
					int end_of_seq = libpic_icnt == num_encoded_frames_lib ? END_OF_VIDEO_SEQUENCE : NOT_END_OF_VIDEO_SEQUENCE;

                    memcpy(bs_buf_lib_tmp + bs_lib_size, bs_buf_lib, stat_lib.write);
                    bs_lib_size += stat_lib.write;
                    if (write_data(op_fname_libout, bs_buf_lib, stat_lib.write, end_of_seq))
					{
						v0print("cannot write bitstream\n");
						return -1;
					}
				}
				/* get reconstructed image */
				size_lib = sizeof(COM_IMGB**);
				imgb_rec_lib = PIC_REC((ENC_CTX*)id_lib)->imgb;
				imgb_rec_lib->addref(imgb_rec_lib);
				if (COM_FAILED(ret))
				{
					v0print("failed to get reconstruction image\n");
					return -1;
				}

				/* calculate PSNR */
#if CALC_SSIM
				if (cal_psnr(ilist_org_lib, imgb_rec_lib, imgb_rec_lib->ts[0], psnr_lib, &ms_ssim_lib))
#else

				if (cal_psnr(ilist_org_lib, imgb_rec_lib, imgb_rec_lib->ts[0], psnr_lib))
#endif
				{
					v0print("cannot calculate PSNR\n");
					return -1;
				}

				/* store reconstructed image to list only for writing out */
				ilist_t_lib = store_rec_img(ilist_rec_lib, imgb_rec_lib, imgb_rec_lib->ts[0], param_input.bit_depth_internal);
				if (ilist_t_lib == NULL)
				{
					v0print("cannot put reconstructed image to list\n");
					return -1;
				}

				if (write_rec(ilist_rec_lib, &pic_ocnt, param_input.bit_depth_internal, op_flag[OP_FLAG_FNAME_LIBREC], op_fname_librec))
				{
					v0print("cannot write reconstruction image\n");
					return -1;
				}

				bitrate_lib += (stat_lib.write - udata_size_lib);
				for (int i = 0; i < 3; i++) psnr_avg_lib[i] += psnr_lib[i];
#if CALC_SSIM
				ms_ssim_avg_lib += ms_ssim_lib;
#endif
				/* release recon buffer */
				if (imgb_rec_lib) imgb_rec_lib->release(imgb_rec_lib);
			}
			else if (ret == COM_OK_NO_MORE_FRM)
			{
				//return COM_OK;
			}
			else
			{
				v2print("invaild return value (%d)\n", ret);
				return -1;
			}
			if ((op_flag[OP_FLAG_MAX_FRM_NUM] || op_flag[OP_FLAG_MAX_FRM_NUM_WHEN_EXTRACT_LIBPIC]) && pic_icnt_lib >= op_max_frm_num_when_extract_libpic
				&& state_lib == STATE_ENCODING)
			{
				state_lib = STATE_BUMPING;
				setup_bumping(id_lib);
			}


			while (libpic_icnt - pic_ocnt > 0)
			{
				write_rec(ilist_rec_lib, &pic_ocnt, param_input.bit_depth_internal, op_flag[OP_FLAG_FNAME_LIBREC], op_fname_librec);
			}
			if (libpic_icnt != pic_ocnt)
			{
				v2print("number of input(=%d) and output(=%d) is not matched\n",
					(int)libpic_icnt, (int)pic_ocnt);
			}

			psnr_avg_lib[0] /= pic_ocnt;
			psnr_avg_lib[1] /= pic_ocnt;
			psnr_avg_lib[2] /= pic_ocnt;
#if CALC_SSIM
			ms_ssim_avg_lib /= pic_ocnt_lib;
#endif


			if (pic_icnt_lib == encoder_change_condition) {
				bits_libpic = bitrate_lib * 8;
				libvc_data.bits_libpic += bits_libpic;

				bitrate_lib *= (param_input.fps * 8);
				bitrate_lib /= pic_ocnt;
				bitrate_lib /= 1000;
			}
			/* store remained reconstructed pictures in output list */
			if (pic_icnt == encoder_change_condition) {
				enc_delete(id_lib);
				imgb_list_free(ilist_org_lib);
				imgb_list_free(ilist_rec_lib);
				if (fp_inp_lib)  fclose(fp_inp_lib);
				if (bs_buf_lib)  free(bs_buf_lib); /* release bitstream buffer */
				if (bs_buf_lib2) free(bs_buf_lib2); /* release bitstream buffer */


				if (err)
				{
					v0print("Error when encode lib pic!");
					return -1;
				}

				// for slice parallel coding
				if (op_flag[OP_FLAG_SKIP_FRAMES] || op_flag[OP_FLAG_SKIP_FRAMES_WHEN_EXTRACT_LIBPIC])
				{
					for (int k = 0; k < libvc_data.num_RLpic; k++)
					{

						int RLpoc_in_seq = libvc_data.list_poc_of_RLpic[k] + op_skip_frames_when_extract_libpic - op_skip_frames;
						if (RLpoc_in_seq<0 || RLpoc_in_seq>op_max_frm_num - 1)
						{
							libvc_data.list_poc_of_RLpic[k] = -1;
						}
						else
						{
							libvc_data.list_poc_of_RLpic[k] = RLpoc_in_seq;
						}
					}
				}
				// restore the following param for sequence pic encoding

				param_input.i_period = ori_iperiod;
				param_input.max_b_frames = ori_max_b_frames;
				param_input.qp = ori_qp;

				libvc_data.is_libpic_processing = 0;
				libvc_data.library_picture_enable_flag = param_input.library_picture_enable_flag = ori_flag;
				libvc_data.is_libpic_prepared = 1;

				id = enc_create(&param_input, NULL);

				if (id == NULL)
				{
					//v0print("cannot create encoder\n");
					return -1;
				}
				if (pic_icnt == encoder_change_condition) {
					set_livcdata_enc(id, &libvc_data);
				}

				if (op_tool_libpic)
				{
					print_libenc_data(libvc_data);
				}

				if (set_extra_config(id))
				{
					//v0print("cannot set extra configurations\n");
					return -1;
				}

				pic_ocnt = pic_icnt;

				init_seq_header((ENC_CTX*)id, &bitb);
				/* encode Sequence Header if needed **************************************/
			}
			param_input.i_period = ori_iperiod;
			param_input.max_b_frames = ori_max_b_frames;
			param_input.qp = ori_qp;

			libvc_data.is_libpic_processing = 0;
			libvc_data.library_picture_enable_flag = param_input.library_picture_enable_flag = ori_flag;
			libvc_data.is_libpic_prepared = 1;

			//if (pic_icnt_lib == 1) {
			id = enc_create(&param_input, NULL);
			if (id == NULL)
			{
				//v0print("cannot create encoder\n");
				return -1;
			}

			set_livcdata_enc(id, &libvc_data);
			ENC_CTX* tmp_ctx = (ENC_CTX*)id;
			tmp_ctx->pic_cnt = pic_icnt;
			tmp_ctx->pic_icnt = pic_icnt - 1;

			if (op_tool_libpic)
			{
				print_libenc_data(libvc_data);
			}
			if (set_extra_config(id))
			{
				//v0print("cannot set extra configurations\n");
				return -1;
			}
			init_seq_header((ENC_CTX*)id, &bitb);

			pic_ocnt = pic_icnt;
			pic_icnt_lib++;
		}
    }
    else 
    {
        *end = 0;
        if (COM_FAILED(ret))
        {
            v0print("enc_encode() failed\n");
            *end = 1;
            return -1;
        }
        num_encoded_frames += ret == COM_OK;
	    clk_end = com_clk_from(clk_beg);
	    clk_tot += clk_end;

	    /* store bitstream */
        if (ret == COM_OK_OUT_NOT_AVAILABLE)
        {
            v1print("--> RETURN OK BUT PICTURE IS NOT AVAILABLE YET\n");
            return COM_OK_OUT_NOT_AVAILABLE;
        }
        else if (ret == COM_OK)
        {
            finish_encode_one_frame();
        }
        else if (ret == COM_OK_NO_MORE_FRM)
        {
            *end = 1;
        }
        else
        {
            v2print("invaild return value (%d)\n", ret);
            return -1;
        }
        if (op_flag[OP_FLAG_MAX_FRM_NUM] && pic_icnt >= param_input.frames_to_be_encoded
            && state == STATE_ENCODING)
        {
            state = STATE_BUMPING;
            setup_bumping(id);
        }
		if (op_tool_libpic && libvc_data.num_lib_pic != 0) {
			pic_icnt_lib = (pic_icnt_lib - 1 == pic_icnt) ? pic_icnt_lib : pic_icnt;
		}
    }
    return COM_OK;
  
}
#if PRINT_AEC_DATA
FILE* SPLIT_MODE_FILE;
FILE* resi_y;
FILE* resi_u;
FILE* resi_v;
FILE* IPM;
FILE* PREDMODE;
FILE* PARTSIZE;
FILE* NUM_NZ;
FILE* NUM_REFP;
FILE* REF;
FILE* MVD;
FILE* MVR_IDX;
FILE* SBAC_STAT;
FILE* AFFINE_FLAG;
FILE* UMVE_FLAG;
FILE* UMVE_IDX;
FILE* AFFINE_UMVE_FLAG;
FILE* AFFINE_UMVE_IDX0;
FILE* SKIP_IDX;
FILE* SBAC_STAT_OUT;
#endif
int Encoder::encode_one_frame(
#if PATCH_DIVIDE_ENABLE
    int multislice_mode
#endif
)
{
	err = 0;
    if (libvc_data.num_lib_pic == 0)
        encoder_change_condition = 0;
    else
        encoder_change_condition = (op_tool_libpic == 0 ) ? 0 : (libvc_data.list_poc_of_libpic[libvc_data.num_lib_pic - 1] + 1);
#if M5422
    encoder_change_condition = param_input.frames_to_be_encoded + 2 ;
#endif
#if PRINT_AEC_DATA
    AFFINE_FLAG = fopen("E:\\MyGit\\AVS3IP_master_h4e\\H5\\run\\DATA\\AFFINE_FLAG.txt", "w");
    SPLIT_MODE_FILE = fopen("E:\\MyGit\\AVS3IP_master_h4e\\H5\\run\\DATA\\SPLIT_MODE.txt", "w");
    resi_y = fopen("E:\\MyGit\\AVS3IP_master_h4e\\H5\\run\\DATA\\resi_y.txt", "w");
    resi_u = fopen("E:\\MyGit\\AVS3IP_master_h4e\\H5\\run\\DATA\\resi_u.txt", "w");
    resi_v = fopen("E:\\MyGit\\AVS3IP_master_h4e\\H5\\run\\DATA\\resi_v.txt", "w");
    IPM = fopen("E:\\MyGit\\AVS3IP_master_h4e\\H5\\run\\DATA\\ipm.txt", "w");
    PREDMODE = fopen("E:\\MyGit\\AVS3IP_master_h4e\\H5\\run\\DATA\\PREDMODE.txt", "w");
    PARTSIZE = fopen("E:\\MyGit\\AVS3IP_master_h4e\\H5\\run\\DATA\\PARTSIZE.txt", "w");
    NUM_NZ = fopen("E:\\MyGit\\AVS3IP_master_h4e\\H5\\run\\DATA\\NUM_NZ.txt", "w");
    NUM_REFP = fopen("E:\\MyGit\\AVS3IP_master_h4e\\H5\\run\\DATA\\NUM_REFP.txt", "w");
    REF = fopen("E:\\MyGit\\AVS3IP_master_h4e\\H5\\run\\DATA\\REF.txt", "w");
    MVD = fopen("E:\\MyGit\\AVS3IP_master_h4e\\H5\\run\\DATA\\MVD.txt", "w");
    MVR_IDX = fopen("E:\\MyGit\\AVS3IP_master_h4e\\H5\\run\\DATA\\MVR_IDX.txt", "w");
    SBAC_STAT = fopen("E:\\MyGit\\AVS3IP_master_h4e\\H5\\run\\DATA\\SBAC_STAT.txt", "w");
    UMVE_FLAG = fopen("E:\\MyGit\\AVS3IP_master_h4e\\H5\\run\\DATA\\UMVE_FLAG.txt", "w");
    UMVE_IDX = fopen("E:\\MyGit\\AVS3IP_master_h4e\\H5\\run\\DATA\\UMVE_IDX.txt", "w");
    AFFINE_UMVE_FLAG = fopen("E:\\MyGit\\AVS3IP_master_h4e\\H5\\run\\DATA\\AFFINE_UMVE_FLAG.txt", "w");
    AFFINE_UMVE_IDX0 = fopen("E:\\MyGit\\AVS3IP_master_h4e\\H5\\run\\DATA\\AFFINE_UMVE_IDX0.txt", "w");
    SKIP_IDX = fopen("E:\\MyGit\\AVS3IP_master_h4e\\H5\\run\\DATA\\SKIP_IDX.txt", "w");
    SBAC_STAT_OUT = fopen("E:\\MyGit\\AVS3IP_master_h4e\\H5\\run\\DATA\\SBAC_STAT_OUT.txt", "w");
#endif
    /* encode lib pictures *******************************************************/
    while (1)
    {
        int end = 0;
        int ret_init = 0;

        // store history frame ime cost for naxt frame lamdba adaptive
        static double ctu_64x64_ime_cost[2040];

        ret_init = frame_init();

        if (ret_init == SKIP_LIBPIC) {
            continue;
        }

  #if M5422
     if (pic_icnt % op_i_period == 0 && pic_icnt != libvc_data.list_poc_of_libpic[libpic_icnt - 1])
     {
		libvc_data.end_of_intra_period = 1;
    
     }
	else
    {
		libvc_data.end_of_intra_period = 0;
    }
  #endif

        if (pic_icnt == libvc_data.list_poc_of_libpic[libpic_icnt - 1]) {
            stat_c = stat_lib;
            id_c = id_lib;
            bitb_c = bitb_lib;
        }
        else {
            stat_c = stat1;
            id_c = id;
            bitb_c = bitb;
        }

        ret = start_encode_one_frame(
#if PATCH_DIVIDE_ENABLE
            multislice_mode,
#endif
            id_c, &bitb_c, &stat_c, ctu_64x64_ime_cost);

        if (pic_icnt == libvc_data.list_poc_of_libpic[libpic_icnt - 1]) {
            stat_lib = stat_c;
            id_lib = id_c;
            bitb_lib = bitb_c;
        }
        else {
            stat1 = stat_c;
            id = id_c;
            bitb = bitb_c;
        }
        frame_end(&end);
        if (end == 1) {
            return COM_OK;
        }
    }
#if PRINT_AEC_DATA
    fclose(AFFINE_FLAG);
    fclose(SPLIT_MODE_FILE);
    fclose(resi_y);
    fclose(resi_u);
    fclose(resi_v);
    fclose(IPM);
    fclose(PREDMODE);
    fclose(PARTSIZE);
    fclose(NUM_NZ);
    fclose(NUM_REFP);
    fclose(REF);
    fclose(MVD);
    fclose(MVR_IDX);
    fclose(UMVE_FLAG);
    fclose(UMVE_IDX);
    fclose(AFFINE_UMVE_FLAG);
    fclose(AFFINE_UMVE_IDX0);
    fclose(SKIP_IDX);
    fclose(SBAC_STAT);
    fclose(SBAC_STAT_OUT);
#endif
	return COM_OK;
}

void Encoder::rec_pictures_store()
{
	while (pic_icnt - pic_ocnt > 0)
	{
		write_rec(ilist_rec, &pic_ocnt, param_input.bit_depth_internal, op_flag[OP_FLAG_FNAME_REC], op_fname_rec);
	}

	if (pic_icnt != pic_ocnt)
	{
		v2print("number of input(=%d) and output(=%d) is not matched\n",
			(int)pic_icnt, (int)pic_ocnt);
	}
}

void Encoder::report()
{
    pic_ocnt = pic_ocnt;
	printf("===============================================================================\n");
	psnr_avg[0] /= avs3_max(1, pic_ocnt);
	psnr_avg[1] /= avs3_max(1, pic_ocnt);
	psnr_avg[2] /= avs3_max(1, pic_ocnt);

	printf("  PSNR Y(dB)       : %-5.4f\n", psnr_avg[0]);
	printf("  PSNR U(dB)       : %-5.4f\n", psnr_avg[1]);
	printf("  PSNR V(dB)       : %-5.4f\n", psnr_avg[2]);

#if LIBVC_ON
    if (op_tool_libpic)
    {
#if M5422
        bits_libpic = bitrate_lib * 8;
        libvc_data.bits_libpic += bits_libpic;
#endif
        bitrate = bitrate * 8 + libvc_data.bits_dependencyFile + libvc_data.bits_libpic;

    }
    else
    {
        bitrate = bitrate * 8;
    }
    v1print("  Total bits(bits) : %-.0f\n", bitrate);
    bitrate *= param_input.fps;
#else
    v1print("  Total bits(bits) : %-.0f\n", bitrate * 8);
    bitrate *= (param_input.fps * 8);
#endif

	bitrate /= avs3_max(1, pic_ocnt);
	bitrate /= 1000;
	printf("  bitrate(kbps)    : %-5.4f\n", bitrate);
	printf("===============================================================================\n");
	printf("Encoded frame count               = %d\n", (int)pic_ocnt);
	printf("Total encoding time               = %.3f msec,",
		(float)com_clk_msec(clk_tot));
	printf(" %.3f sec\n", (float)(com_clk_msec(clk_tot) / 1000.0));
	printf("Average encoding time for a frame = %.3f msec\n",
		(float)com_clk_msec(clk_tot) / avs3_max(1, pic_ocnt));
	printf("Average encoding speed            = %.3f frames/sec\n",
		((float)pic_ocnt * 1000) / ((float)com_clk_msec(avs3_max(1, clk_tot))));
	printf("===============================================================================\n");
	print_flush(stdout);
#if LIBVC_ON
    delete_libvcdata(&libvc_data);
#endif

}

ENC_ME_CU* me_cu_info_global;

ENC_ME_CU me_cu_info_blk;

int merlin_init()
{
#ifdef MCC_SIM
#ifdef USE_KERNEL_TOP	
     __merlin_init("kernel_top.xclbin");
#else


#ifdef USE_KERNEL_SAODBK
	__merlin_init("k_SAODBK.xclbin");
#endif

#ifdef USE_KERNEL_AEC_SAO
	__merlin_init("k_AEC_SAO.xclbin");
#endif

#ifdef USE_KERNEL_UNIT_CHROMA
	__merlin_init("k_unit_chroma.xclbin");
#endif

#ifdef USE_KERNEL_IME_AFFMVP
	__merlin_init("k_ime_affmvp.xclbin");
#endif

#ifdef USE_KERNEL_IME_MVP
	 __merlin_init("k_ime_mvp.xclbin");
#endif

#ifdef USE_KERNEL_IME_AFFGRA
	 __merlin_init("k_ime_affgra.xclbin");
#endif

#ifdef USE_KERNEL_IME_TRAN
	__merlin_init("k_ime_tran.xclbin");
#endif

#ifdef USE_KERNEL_IME_MC
	__merlin_init("k_ime_mc.xclbin");
#endif

#ifdef USE_KERNEL_IME_AFFMC  
	__merlin_init("k_ime_affmc.xclbin");
#endif

#ifdef USE_KERNEL_IME_AFFMC_L
	__merlin_init("k_ime_affmc_l.xclbin");
#endif

#ifdef USE_KERNEL_SUB_ME
	__merlin_init("k_sub_me.xclbin");
#endif

#ifdef USE_KERNEL_com_get_affine_memory_access
   __merlin_init("k_com_get_affine_memory_access.xclbin");
#endif

#ifdef USE_KERNEL_CHECK_BEST
    __merlin_init("k_check_best.xclbin");
#endif

#ifdef USE_KERNEL_com_get_affine_merge_candidate
    __merlin_init("k_com_get_affine_merge_candidate.xclbin");
#endif

#ifdef USE_KERNEL_INTER_CAND
    __merlin_init("k_inter_cand.xclbin");
#endif

#ifdef USE_KERNEL_EST_INTER
    __merlin_init("k_est_inter.xclbin");
#endif

#ifdef USE_KERNEL_enc_comp
	__merlin_init("k_enc_comp.xclbin");
#endif

#ifdef USE_KERNEL_TQ
	__merlin_init("k_TQ.xclbin");
#endif

#ifdef USE_KERNEL_enc_eco_xu
	__merlin_init("k_enc_eco.xclbin");
#endif

#ifdef USE_KERNEL_encode_ipf_flag
	__merlin_init("k_encode_ipf_flag.xclbin");
#endif

#ifdef USE_KERNEL_AFFINE_MERGE
    __merlin_init("k_affine_merge.xclbin");
#endif

#ifdef USE_KERNEL_SKIP
    __merlin_init("k_skip.xclbin");
#endif

#ifdef USE_KERNEL_enc_eco_unit
	__merlin_init("k_enc_eco_unit.xclbin");
#endif

#ifdef USE_KERNEL_AFFINE_PRED_ME
	__merlin_init("k_aff_me.xclbin");
#endif

#ifdef USE_KERNEL_AFFINE_PRED_RDO
	__merlin_init("k_aff_rdo.xclbin");
#endif

#endif	// USE_KERNEL_TOP

#ifdef USE_KERNEL_SAODBK
    __merlin_init_enc_SAO_rdo_m();
#endif

#ifdef USE_KERNEL_AEC_SAO
    __merlin_init_writeParaSAO_one_LCU_m();
#endif

#ifdef USE_KERNEL_UNIT_CHROMA
    __merlin_init_enc_eco_unit_chroma_m();
#endif
    
#ifdef USE_KERNEL_mpm
	__merlin_init_com_get_mpm_m();
#endif

#ifdef USE_KERNEL_IME_AFFMVP
	  __merlin_init_com_get_affine_mvp_re_m();
#endif

#ifdef USE_KERNEL_IME_MVP
	 __merlin_init_com_derive_mvp_re_m();
#endif

#ifdef USE_KERNEL_IME_AFFGRA
	 __merlin_init_pinter_affine_me_gradient_m();
#endif

#ifdef USE_KERNEL_IME_TRAN
	__merlin_init_me_ipel_m();
#endif 

#ifdef USE_KERNEL_IME_MC
	__merlin_init_com_mc_m();
#endif

#ifdef USE_KERNEL_IME_AFFMC  
	__merlin_init_com_affine_mc_m();
#endif

#ifdef USE_KERNEL_IME_AFFMC_L
	__merlin_init_com_affine_mc_l_m();
#endif

#ifdef USE_KERNEL_SUB_ME
	__merlin_init_me_spel_pattern_m();
#endif

#ifdef USE_KERNEL_com_get_affine_memory_access
    __merlin_init_com_get_affine_memory_access_m();
#endif

#ifdef USE_KERNEL_CHECK_BEST
    __merlin_init_check_best_mode_m();
#endif

#ifdef USE_KERNEL_com_get_affine_merge_candidate
    __merlin_init_com_get_affine_merge_cand_m();
#endif

#ifdef USE_KERNEL_INTER_CAND
    __merlin_init_derive_inter_cands_m();
#endif

#ifdef USE_KERNEL_EST_INTER
    __merlin_init_enc_bit_est_inter_m();
#endif

#ifdef USE_KERNEL_enc_comp
	__merlin_init_enc_bit_est_inter_comp_m();
#endif

#ifdef USE_KERNEL_TQ
	__merlin_init_enc_tq_nnz_m();
#endif

#ifdef USE_KERNEL_enc_eco_xu
	__merlin_init_enc_eco_unit_xu_m();
#endif

#ifdef USE_KERNEL_encode_ipf_flag
	__merlin_init_encode_ipf_flag_m();
#endif

#ifdef USE_KERNEL_AFFINE_MERGE
    __merlin_init_analyze_affine_merge_m();
#endif

#ifdef USE_KERNEL_SKIP
    __merlin_init_analyze_direct_skip_m();
#endif

#ifdef USE_KERNEL_enc_eco_unit
	__merlin_init_enc_eco_unit_m();
#endif

#ifdef USE_KERNEL_AFFINE_PRED_ME
	__merlin_init_affine_me_all_refi_m();
#endif

#ifdef USE_KERNEL_AFFINE_PRED_RDO
	__merlin_init_pinter_residue_rdo_best_m();
#endif

#ifdef USE_KERNEL_FME_TOP
    __merlin_init_pinter_me_epzs_m();
#endif

#ifdef USE_KERNEL_SAODBK_AVS2
    __merlin_init_enc_deblock_sao_avs2_m();
#endif

#ifdef USE_KERNEL_INTRA_TOP
    __merlin_init_analyze_intra_cu_m();
#endif

#endif	// MCC_SIM
  srand((unsigned int)(time(NULL)));

  return COM_OK;

  }

int merlin_release()
{
#ifdef MCC_SIM

#ifdef USE_KERNEL_mpm
	__merlin_release_com_get_mpm_m();
#endif
#ifdef USE_KERNEL_AEC_SAO
 	__merlin_release_writeParaSAO_one_LCU_m();
#endif

#ifdef USE_KERNEL_UNIT_CHROMA
    __merlin_release_enc_eco_unit_chroma_m();
#endif

#ifdef USE_KERNEL_IME_AFFMVP
    __merlin_release_com_get_affine_mvp_re_m();
#endif

#ifdef USE_KERNEL_IME_MVP
   __merlin_release_com_derive_mvp_re_m();
#endif

#ifdef USE_KERNEL_IME_AFFGRA
   __merlin_release_pinter_affine_me_gradient_m();
#endif

#ifdef USE_KERNEL_IME_TRAN
   __merlin_release_me_ipel_m();
#endif

#ifdef USE_KERNEL_IME_MC
   __merlin_release_com_mc_m();
#endif

#ifdef USE_KERNEL_IME_AFFMC  
   __merlin_release_com_affine_mc_m();
#endif

#ifdef USE_KERNEL_IME_AFFMC_L
   __merlin_release_com_affine_mc_l_m();
#endif 

#ifdef USE_KERNEL_SUB_ME
   __merlin_release_me_spel_pattern_m();
#endif

#ifdef USE_KERNEL_com_get_affine_memory_access
    __merlin_release_com_get_affine_memory_access_m();
#endif
#ifdef USE_KERNEL_CHECK_BEST
    __merlin_release_check_best_mode_m();
#endif
#ifdef USE_KERNEL_com_get_affine_merge_candidate
    __merlin_release_com_get_affine_merge_cand_m();
#endif
#ifdef USE_KERNEL_INTER_CAND
    __merlin_release_derive_inter_cands_m();
#endif
#ifdef USE_KERNEL_EST_INTER
    __merlin_release_enc_bit_est_inter_m();
#endif
#ifdef USE_KERNEL_enc_comp
	__merlin_release_enc_bit_est_inter_comp_m();
#endif

#ifdef USE_KERNEL_TQ
	__merlin_release_enc_tq_nnz_m();
#endif

#ifdef USE_KERNEL_enc_eco_xu
	__merlin_release_enc_eco_unit_xu_m();
#endif

#ifdef USE_KERNEL_encode_ipf_flag
	__merlin_release_encode_ipf_flag_m();
#endif

#ifdef USE_KERNEL_AFFINE_MERGE
    __merlin_release_analyze_affine_merge_m();
#endif

#ifdef USE_KERNEL_SKIP
    __merlin_release_analyze_direct_skip_m();
#endif

#ifdef USE_KERNEL_enc_eco_unit
	__merlin_release_enc_eco_unit_m();
#endif

#ifdef USE_KERNEL_SAODBK
	__merlin_release_enc_SAO_rdo_m();
#endif


#ifdef USE_KERNEL_AFFINE_PRED_ME
	__merlin_release_affine_me_all_refi_m();
#endif

#ifdef USE_KERNEL_AFFINE_PRED_RDO
	__merlin_release_pinter_residue_rdo_best_m();
#endif

#ifdef USE_KERNEL_FME_TOP
    __merlin_release_pinter_me_epzs_m();
#endif

#ifdef USE_KERNEL_SAODBK_AVS2
    __merlin_release_enc_deblock_sao_avs2_m();
#endif

#ifdef USE_KERNEL_INTRA_TOP
    __merlin_release_analyze_intra_cu_m();
#endif

    __merlin_release();
#endif	// MCC_SIM
return COM_OK;
}


void avs3_funs_init_cost_c();
void uavs3e_funs_init_intra_pred_c();
void uavs3e_funs_init_mc_c();

void Encoder::encoder_info_config()
{
    pic_width = op_w;
    pic_height = op_h;
    split_mode = op_multislice_mode;
    fname_inp = op_fname_inp;
    fname_out = op_fname_out;
    fname_rec = op_fname_rec;
    bit_depth_out = op_bit_depth_input;
    frame_num = op_max_frm_num;
    fname_rec = op_fname_rec;

    assert(split_mode < 16 && split_mode >= 0);
    int split_mode_in = split_mode;

    if (split_mode_in > 3 && split_mode_in < 10)
    {
        patch_num = split_mode - 1;
        split_mode_in = CUSTOM_HOR;
    }
    else if (split_mode_in >= 10)
    {
        patch_num = split_mode - 7;
        split_mode_in = CUSTOM_VER;
    }

    if (split_mode_in != NONE_SPLIT)
    {
        //to do
        op_tool_libpic = 0;
        op_tool_sao = 0;
        op_cross_patch_loop_filter = 0;
    }

    if (split_mode_in == NONE_SPLIT)
    {
        patch_num = 1;
        head_info_flag = 0;
    }
    else if (split_mode_in == QUARD)
    {
        patch_num = 4;
        if (patch_idx == HEAD_INFO)
        {
            head_info_flag = 1;
        }
        else
        {
            head_info_flag = 0;
        }
    }
    else if (split_mode_in == CUSTOM_HOR || 
        split_mode_in == CUSTOM_VER)
    {
        if (patch_idx == HEAD_INFO)
        {
            head_info_flag = 1;
        }
        else
        {
            head_info_flag = 0;
        }
    }
    else
    {
        patch_num = 2;
        if (patch_idx == HEAD_INFO)
        {
            head_info_flag = 1;
        }
        else
        {
            head_info_flag = 0;
        }
    }

    op_head_info_flag = head_info_flag;
    op_patch_mode_on = (op_multislice_mode == NONE_SPLIT) ? 0 : 1;

    if (!head_info_flag)
    {
        get_patch_size(patch_idx);
        op_w = patch_width;
        op_h = patch_height;
    }
    else
    {
        get_patch_size(0);
    }

    if (patch_idx < HEAD_INFO && split_mode_in != NONE_SPLIT)
    {
        string split_file = op_fname_inp + std::to_string(patch_idx) + ".patch";
        strcpy(op_fname_inp, split_file.c_str());
        string split_out_file = op_fname_rec + std::to_string(patch_idx) + ".patch";
        strcpy(op_fname_rec, split_out_file.c_str());
    }

}

int Encoder::encoder_init(int argc, const char **argv)
{
    merlin_init();
    srand((unsigned int)(time(NULL)));

	Configure(argc, argv, options, ret);

    encoder_info_config();

	firmware_init();

	AllocateBitstream();
#if USE_IME_ALL_CU || USE_ORG_QT_ONLY
    me_cu_info_global = &me_cu_info_blk;
    me_cu_info_global->has_partitioned = 0;
    clk_tot_lib = 0;
    pic_icnt_lib = 0;
    pic_ocnt_lib = 0;
    pic_skip_lib = 0;
    bits_libpic = 0;
    bitrate_lib = 0;
    bitb_lib.addr = bs_buf_lib;
    bitb_lib.addr2 = bs_buf_lib2;
    bitb_lib.bsize = MAX_BS_BUF;

#if WRITE_MD5_IN_USER_DATA //discount user data bits in per-frame bits
    udata_size_lib = (op_use_pic_signature) ? 23 : 0;
#endif
#if !PRECISE_BS_SIZE
    udata_size_lib += 4; /* 4-byte prefix (length field of chunk) */
#endif
    /* encode Sequence Header if needed **************************************/
    bitb_lib.err = 0; // update BSB
#endif

    //g_tv_path = string(op_sc_tv_path);
    //cout << "TV_PATH = " << g_tv_path << endl;

    //memset(bytes_global, 0, sizeof(bytes_global));
    //memset(bytes_fetch, 0, sizeof(bytes_fetch));
    //memset(bytes_dbk, 0, sizeof(bytes_dbk));
    //memset(bytes_sao, 0, sizeof(bytes_sao));
    //memset(bytes_aec, 0, sizeof(bytes_aec));
    //frame_level_fw.frame_id = -1;/////////////////////

	init_img();

	malloc_picture();

	information_init();
    if (!op_tool_libpic || libvc_data.num_lib_pic == 0) {
        init_seq_header((ENC_CTX*)id, &bitb);
    }
    avs3_funs_init_cost_c();
    uavs3e_funs_init_intra_pred_c();
    uavs3e_funs_init_mc_c();


   return COM_OK;
}
int Encoder::encoder_end()
{
#if AEC_STAGE1_DATA
    fclose(fp_stage1_InStream);
    fclose(fp_stage1_OutStream);
#endif

#if AEC_STAGE2_DATA
    fclose(fp_stage2_InStream);
    fclose(fp_stage2_OutStream);
#endif
    rec_pictures_store();
    //rc_destroy(ctx->rc);
#ifndef VU440
	FILE *fp = fopen("top_bs_file.bin", "ab");
	char end[4] = {(char)0x00,(char)0x00, (char)0x01, (char)0xb1 };
	fwrite(end, 1, 4, fp);
	fclose(fp);
#endif
	report();

//#ifndef NOP
//    char cmd[] = "end";
//    write_file(cmd, 0, 0);
//#endif

    merlin_release();
    return COM_OK;
}

void Encoder::checkSize(const int mode)
{
    //Check whether the current number of divisions meets the decoder requirements 

    int pic_width_in_lcu = 0;
    int pic_height_in_lcu = 0;
    int split_num = 0; // this is the number of patches that decoder supports
    bool sizeWrong = false;

    pic_width_in_lcu = (pic_width + 64 - 1) >> 6;
    pic_height_in_lcu = (pic_height + 64 - 1) >> 6;

    if (mode == HOR_BI)
    {
        split_num = pic_height_in_lcu / op_patch_height_in_lcu;
        sizeWrong = split_num == 2 ? false : true;
    }
    else if (mode == VER_BI)
    {
        split_num = pic_width_in_lcu / op_patch_width_in_lcu;
        sizeWrong = split_num == 2 ? false : true;
    }
    else if (mode == QUARD)
    {
        split_num = (pic_width_in_lcu / op_patch_width_in_lcu) * pic_height_in_lcu / op_patch_height_in_lcu;
        sizeWrong = split_num == 4 ? false : true;
    }
    else if (mode == CUSTOM_HOR)
    {
        split_num = pic_height_in_lcu / op_patch_height_in_lcu;
        sizeWrong = split_num == patch_num ? false : true;
    }
    else if (mode == CUSTOM_VER)
    {
        split_num = pic_width_in_lcu / op_patch_width_in_lcu;
        sizeWrong = split_num == patch_num ? false : true;
    }

    if (sizeWrong)
    {
        printf("[Error] This split mode is not supported, please change the multislice_mode flag!\n");
        exit(1);
    }

}


void Encoder::get_patch_size(int patch_idx)
{
    assert(split_mode < 16 && split_mode >= 0);
    int split_mode_in = 0;

    if (split_mode > 3 && split_mode < 10)
    {
        split_mode_in = CUSTOM_HOR;
    }
    else if (split_mode >= 10)
    {
        split_mode_in = CUSTOM_VER;
    }
    else
    {
        split_mode_in = split_mode;
    }

    if (split_mode_in == HOR_BI)
    {
        assert(patch_idx < 2);

        patch_width = pic_width;
        int lcu_num = ceil((float)pic_height / 64);
        op_patch_width_in_lcu = ceil((float)pic_width / 64);
        op_patch_height_in_lcu = lcu_num / 2;
        if (patch_idx & 1)
        {
            patch_height = pic_height - (lcu_num / 2) * 64;
        }
        else
        {
            patch_height = (lcu_num / 2) * 64;
        }
        checkSize(HOR_BI);
    }
    else if (split_mode_in == VER_BI)
    {
        assert(patch_idx < 2);

        patch_height = pic_height;
        int lcu_num = ceil((float)pic_width / 64);
        op_patch_width_in_lcu = lcu_num / 2;
        op_patch_height_in_lcu = ceil((float)pic_height / 64);
        if (patch_idx & 1)
        {
            patch_width = pic_width - (lcu_num / 2) * 64;
        }
        else
        {
            patch_width = (lcu_num / 2) * 64;
        }
        checkSize(VER_BI);

    }
    else if (split_mode_in == QUARD)
    {
        int lcu_num_w = ceil((float)pic_width / 64);
        int lcu_num_h = ceil((float)pic_height / 64);
        op_patch_width_in_lcu = lcu_num_w / 2;
        op_patch_height_in_lcu = lcu_num_h / 2;
        if (patch_idx < 2)
        {
            patch_height = (lcu_num_h / 2) * 64;
            if (patch_idx & 1)
            {
                patch_width = pic_width - (lcu_num_w / 2) * 64;
            }
            else
            {
                patch_width = (lcu_num_w / 2) * 64;
            }
        }
        else
        {
            patch_height = pic_height - (lcu_num_h / 2) * 64;
            if (patch_idx & 1)
            {
                patch_width = pic_width - (lcu_num_w / 2) * 64;
            }
            else
            {
                patch_width = (lcu_num_w / 2) * 64;
            }
        }
        checkSize(QUARD);

    }
    else if (split_mode_in == CUSTOM_HOR)
    {
        assert(patch_idx < PATCH_CNT);
        assert(patch_num >= 3 && patch_num <= PATCH_CNT);

        patch_width = pic_width;
        int lcu_num = ceil((float)pic_height / 64);
        op_patch_width_in_lcu = ceil((float)pic_width / 64);
        op_patch_height_in_lcu = lcu_num / patch_num;
        if (patch_idx < patch_num - 1)
        {
            patch_height = (lcu_num / patch_num) * 64;
        }
        else
        {
            patch_height = pic_height - ((lcu_num / patch_num) * 64) * (patch_num - 1);
        }
        checkSize(CUSTOM_HOR);

    }
    else if (split_mode_in == CUSTOM_VER)
    {
        assert(patch_idx < PATCH_CNT);
        assert(patch_num >= 3 && patch_num <= PATCH_CNT);

        patch_height = pic_height;
        int lcu_num = ceil((float)pic_width / 64);
        op_patch_width_in_lcu = lcu_num / patch_num;
        op_patch_height_in_lcu = ceil((float)pic_height / 64);
        if (patch_idx < patch_num - 1)
        {
            patch_width = (lcu_num / patch_num) * 64;
        }
        else
        {
            patch_width = pic_width - (lcu_num / patch_num) * 64 * (patch_num - 1);
        }
        checkSize(CUSTOM_VER);

    }
    else
    {
        patch_width = pic_width;
        patch_height = pic_height;
    }
}

void Encoder::run(
#if PATCH_DIVIDE_ENABLE
    int multislice_mode,
#endif
    int argc, const char** argv)
{
    encoder_init(argc, argv);

    encode_one_frame(
#if PATCH_DIVIDE_ENABLE
        multislice_mode
#endif
    );

    encoder_end();
}

int Multislice::parse_multislice_mode(const int split_mode)
{
    assert(split_mode < 16 && split_mode >= 0);
    int split_mode_in = 0;

    if (split_mode > 3 && split_mode < 10)
    {
        split_mode_in = CUSTOM_HOR;
    }
    else if (split_mode >= 10)
    {
        split_mode_in = CUSTOM_VER;
    }
    else
    {
        split_mode_in = split_mode;
    }

    return split_mode_in;
}

void Multislice::get_patch_size(
    const int split_mode,
    const int patch_idx,
    const int pic_width,
    const int pic_height,
    const int patch_num,
    int& patch_width,
    int& patch_height
    )
{

    int split_mode_in = parse_multislice_mode(split_mode);

    if (split_mode == HOR_BI)
    {
        assert(patch_idx < 2);

        patch_width = pic_width;
        int lcu_num = ceil((float)pic_height / 64);

        if (patch_idx & 1)
        {
            patch_height = pic_height - (lcu_num / 2) * 64;
        }
        else
        {
            patch_height = (lcu_num / 2) * 64;
        }
    }
    else if (split_mode_in == VER_BI)
    {
        assert(patch_idx < 2);
        patch_height = pic_height;
        int lcu_num = ceil((float)pic_width / 64);
        if (patch_idx & 1)
        {
            patch_width = pic_width - (lcu_num / 2) * 64;
        }
        else
        {
            patch_width = (lcu_num / 2) * 64;
        }
    }
    else if (split_mode_in == QUARD)
    {
        int lcu_num_w = ceil((float)pic_width / 64);
        int lcu_num_h = ceil((float)pic_height / 64);
        if (patch_idx < 2)
        {
            patch_height = (lcu_num_h / 2) * 64;
            if (patch_idx & 1)
            {
                patch_width = pic_width - (lcu_num_w / 2) * 64;
            }
            else
            {
                patch_width = (lcu_num_w / 2) * 64;
            }
        }
        else
        {
            patch_height = pic_height - (lcu_num_h / 2) * 64;
            if (patch_idx & 1)
            {
                patch_width = pic_width - (lcu_num_w / 2) * 64;
            }
            else
            {
                patch_width = (lcu_num_w / 2) * 64;
            }
        }
    }
    else if (split_mode_in == CUSTOM_HOR)
    {
        assert(patch_idx < PATCH_CNT);
        assert(patch_num >= 3 && patch_num <= PATCH_CNT);

        patch_width = pic_width;
        int lcu_num = ceil((float)pic_height / 64);
        op_patch_width_in_lcu = ceil((float)pic_width / 64);
        op_patch_height_in_lcu = lcu_num / patch_num;
        if (patch_idx < patch_num - 1)
        {
            patch_height = (lcu_num / patch_num) * 64;
        }
        else
        {
            patch_height = pic_height - ((lcu_num / patch_num) * 64) * (patch_num - 1);
        }
    }
    else if (split_mode_in == CUSTOM_VER)
    {
        assert(patch_idx < PATCH_CNT);
        assert(patch_num >= 3 && patch_num <= PATCH_CNT);

        patch_height = pic_height;
        int lcu_num = ceil((float)pic_width / 64);
        op_patch_width_in_lcu = lcu_num / patch_num;
        op_patch_height_in_lcu = ceil((float)pic_height / 64);
        if (patch_idx < patch_num - 1)
        {
            patch_width = (lcu_num / patch_num) * 64;
        }
        else
        {
            patch_width = pic_width - (lcu_num / patch_num) * 64 * (patch_num - 1);
        }
    }
    else
    {
        patch_width = pic_width;
        patch_height = pic_height;
    }
}

static bool is_file_exist(string path) {
#if defined(__linux__)
    if (access(path.c_str(), 0) == 0)return true;
    else return false;
#else
    if (_access(path.c_str(), 0) == 0)return true;
    else return false;
#endif
}

void remove_file(string file)
{
    FILE* fp = fopen(file.c_str(), "w");
    fclose(fp);
    is_file_exist(file);
    remove(file.c_str());
}

void Multislice::run(int argc, const char** argv)
{
    int patch_num = 0;
    int multislice_mode = 1;
    int patch_width = 0;
    int patch_height = 0;
    int pic_width = 0;
    int pic_height = 0;
    int bit_depth = 0;
    int frame_num = 0;
    char fname_inp[256];
    char fname_rec[256];

    encoder[HEAD_INFO].set_patch_idx(HEAD_INFO);
    encoder[HEAD_INFO].run(
#if PATCH_DIVIDE_ENABLE
        multislice_mode,
#endif
        argc, argv);

    bs_head_fp = encoder[HEAD_INFO].get_bs_addr();
    bs_head_size = encoder[HEAD_INFO].get_bs_size();

    multislice_mode = encoder[HEAD_INFO].get_split_mode();
    patch_num = encoder[HEAD_INFO].get_patch_num();
    pic_width = encoder[HEAD_INFO].get_pic_width();
    pic_height = encoder[HEAD_INFO].get_pic_height();
    strcpy(fname_inp, encoder[HEAD_INFO].get_fname_inp());
    strcpy(fname_rec, encoder[HEAD_INFO].get_fname_rec());
    bit_depth = encoder[HEAD_INFO].get_bit_depth();
    frame_num = encoder[HEAD_INFO].get_frame_num();

    if (multislice_mode != NONE_SPLIT)
    {
        for (int i = 0; i < patch_num; i++)
        {
            get_patch_size(
                multislice_mode,
                i,
                pic_width,
                pic_height,
                patch_num,
                patch_width,
                patch_height
            );

            yuv_split(
                fname_inp,
                pic_width,
                pic_height,
                bit_depth,
                patch_width,
                patch_height,
                i,
                multislice_mode,
                frame_num,
                i == (patch_num - 1)
                );

            encoder[i].set_patch_idx(i);
            encoder[i].run(
#if PATCH_DIVIDE_ENABLE
                multislice_mode,
#endif
                argc, argv);

            bs_fp[i] = encoder[i].get_bs_addr();
            bs_tmp_size[i] = encoder[i].get_bs_size();
            bs_lib_size[i] = encoder[i].get_bs_lib_size();

            remove_file(fname_inp + std::to_string(i) + ".patch");
        }
        yuv_merge(
            fname_rec,
            pic_width,
            pic_height,
            bit_depth,
            multislice_mode,
            frame_num,
            patch_num
        );

        for (int i = 0; i < patch_num; i++)
        {
            remove_file(fname_rec + std::to_string(i) + ".patch");
        }

    }

    bs_process(
        multislice_mode,
        frame_num,
        patch_num
        );
}

static int xFindNextStartCode(unsigned char* fp, int* ruiPacketSize, unsigned char* pucBuffer, int inp_bs_size)
{
    unsigned int uiDummy = 0;
    char bEndOfStream = 0;
    size_t ret = 0;
    int bs_cnt = 0;

    uiDummy = (unsigned int)fp[0] | ((unsigned int)fp[1] << 8);
    fp += 2;
    bs_cnt += 2;
    if (bs_cnt == inp_bs_size)
    {
        return -1;
    }
    //ret = fread(&uiDummy, 1, 2, fp);
    //if (ret != 2)
    //{
    //    return -1;
    //}

    //if (feof(fp))
    //{
    //    return -1;
    //}
    assert(0 == uiDummy);

    uiDummy = (unsigned int)fp[0];
    fp += 1;
    bs_cnt += 1;
    if (bs_cnt == inp_bs_size)
    {
        return -1;
    }
    //ret = fread(&uiDummy, 1, 1, fp);
    //if (ret != 1)
    //{
    //    return -1;
    //}
    //if (feof(fp))
    //{
    //    return -1;
    //}
    assert(1 == uiDummy);

    int iNextStartCodeBytes = 0;
    unsigned int iBytesRead = 0;
    unsigned int uiZeros = 0;
    unsigned char pucBuffer_Temp[16];
    int iBytesRead_Temp = 0;

    pucBuffer[iBytesRead++] = 0x00;
    pucBuffer[iBytesRead++] = 0x00;
    pucBuffer[iBytesRead++] = 0x01;
    while (1)
    {
        unsigned char ucByte = 0;
        ucByte = fp[0];
        fp += 1;
        //ret = fread(&ucByte, 1, 1, fp);
        bs_cnt += 1;
        if (bs_cnt == inp_bs_size)
        {
            iNextStartCodeBytes = 0;
            bEndOfStream = 1;
            break;
        }
        //if (feof(fp))
        //{
        //    iNextStartCodeBytes = 0;
        //    bEndOfStream = 1;
        //    break;
        //}
        pucBuffer[iBytesRead++] = ucByte;
        if (1 < ucByte)
        {
            uiZeros = 0;
        }
        else if (0 == ucByte)
        {
            uiZeros++;
        }
        else if (uiZeros > 1)
        {
            iBytesRead_Temp = 0;
            pucBuffer_Temp[iBytesRead_Temp] = ucByte;

            iBytesRead_Temp++;
            ucByte = fp[0];
            fp += 1;
            bs_cnt += 1;
            //ret = fread(&ucByte, 1, 1, fp);
            //if (ret != 1)
            //{
            //    return -1;
            //}
            pucBuffer_Temp[iBytesRead_Temp] = ucByte;
            pucBuffer[iBytesRead++] = ucByte;
            iBytesRead_Temp++;

            if (pucBuffer_Temp[0] == 0x01 && (pucBuffer_Temp[1] == 0xb3 || pucBuffer_Temp[1] == 0xb6 || pucBuffer_Temp[1] == 0xb0 || pucBuffer_Temp[1] == 0x00 || pucBuffer_Temp[1] == 0xb1))
            {
                iNextStartCodeBytes = 2 + 1 + 1;
                uiZeros = 0;
                break;
            }
            else
            {
                uiZeros = 0;
                iNextStartCodeBytes = 0;// 2 + 1 + 1;
            }
        }
        else
        {
            uiZeros = 0;
        }
    }
    *ruiPacketSize = iBytesRead - iNextStartCodeBytes;

    if (bEndOfStream)
    {
        return 0;
    }
    fp -= -1 * iNextStartCodeBytes;
    //if (fseek(fp, -1 * iNextStartCodeBytes, SEEK_CUR) != 0)
    //{
    //    printf("file seek failed!\n");
    //};
    return 0;
}

unsigned int initParsingConvertPayloadToRBSP(const unsigned int uiBytesRead, unsigned char* pBuffer, unsigned char* pBuffer2)
{
    unsigned int uiZeroCount = 0;
    unsigned int uiBytesReadOffset = 0;
    unsigned int uiBitsReadOffset = 0;
    const unsigned char* pucRead = pBuffer;
    unsigned char* pucWrite = pBuffer2;
    unsigned int uiWriteOffset = uiBytesReadOffset;
    unsigned char ucCurByte = pucRead[uiBytesReadOffset];

    for (uiBytesReadOffset = 0; uiBytesReadOffset < uiBytesRead; uiBytesReadOffset++)
    {
        ucCurByte = pucRead[uiBytesReadOffset];
        if (2 <= uiZeroCount && 0x02 == pucRead[uiBytesReadOffset])
        {
            pucWrite[uiWriteOffset] = ((pucRead[uiBytesReadOffset] >> 2) << (uiBitsReadOffset + 2));
            uiBitsReadOffset += 2;
            uiZeroCount = 0;
            if (uiBitsReadOffset >= 8)
            {
                uiBitsReadOffset = 0;
                continue;
            }
            if (uiBytesReadOffset >= uiBytesRead)
            {
                break;
            }
        }
        else if (2 <= uiZeroCount && 0x01 == pucRead[uiBytesReadOffset])
        {
            uiBitsReadOffset = 0;
            pucWrite[uiWriteOffset] = pucRead[uiBytesReadOffset];
        }
        else
        {
            pucWrite[uiWriteOffset] = (pucRead[uiBytesReadOffset] << uiBitsReadOffset);
        }

        if (uiBytesReadOffset + 1 < uiBytesRead)
        {
            pucWrite[uiWriteOffset] |= (pucRead[uiBytesReadOffset + 1] >> (8 - uiBitsReadOffset));
        }
        uiWriteOffset++;

        if (0x00 == ucCurByte)
        {
            uiZeroCount++;
        }
        else
        {
            uiZeroCount = 0;
        }
    }

    // th just clear the remaining bits in the buffer
    for (unsigned int ui = uiWriteOffset; ui < uiBytesRead; ui++)
    {
        pucWrite[ui] = 0;
    }
    memcpy(pBuffer, pBuffer2, uiWriteOffset);
    pBuffer[uiWriteOffset] = 0x00;
    pBuffer[uiWriteOffset + 1] = 0x00;
    pBuffer[uiWriteOffset + 2] = 0x01;
    return uiBytesRead;
}

static int read_a_bs(unsigned char* fp, int* pos, unsigned char* bs_buf, int inp_bs_size)
{
    int read_size, bs_size;
    unsigned char* fp_tmp = NULL;
    int remain_size = 0;
    bs_size = 0;
    read_size = 0;
    unsigned char* bs_buf2 = NULL;
    bs_buf2 = (unsigned char*)malloc(32 * 1024 * 1024);

    fp[inp_bs_size] = 0x00;
    fp[inp_bs_size + 1] = 0x00;
    fp[inp_bs_size + 2] = 0x01;
    fp[inp_bs_size + 3] = 0xB1;

    fp_tmp = fp + *pos;

    //add 4 for sequence end flag
    remain_size = inp_bs_size - *pos + 4;

    assert(remain_size >= 0);

    int ret = 0;
    ret = xFindNextStartCode(fp_tmp, &bs_size, bs_buf, remain_size);
    if (ret == -1)
    {
        return -1;
    }
    read_size = initParsingConvertPayloadToRBSP(bs_size, bs_buf, bs_buf2);

    free(bs_buf2);
    return read_size;
}


int Multislice::detect_bs_type()
{
    if (bs_tmp[3] == 0xB0)
        return SEQ_HEAD;
    else if (bs_tmp[3] == 0xB1)
        return SEQ_END;
    else if (bs_tmp[3] == 0xB3 || bs_tmp[3] == 0xB6)
        return PIC_HEAD;
    else if (bs_tmp[3] >= 0x00 && bs_tmp[3] <= 0x8E)
        return CTU_INFO;
    else
        return ERR_INFO;
}

void Multislice::bs_process_one_frame(int split_mode_in, const int patch_num)
{
    int bs_size = 0;
    int bs_type = 0;
    int bs_type_tmp = 0;
    int patch_idx_pos[PATCH_CNT - 1] = { 0 };
    int patch_cnt = 0;

    int split_mode = parse_multislice_mode(split_mode_in);

    if (split_mode == QUARD)
    {
        patch_cnt = 4;
    }
    else if (split_mode == CUSTOM_HOR ||
        split_mode == CUSTOM_VER)
    {
        patch_cnt = patch_num;
    }
    else
    {
        patch_cnt = 2;
    }

    while (1)
    {
        //get bs tpye
        bs_size = read_a_bs(bs_fp[0], &bs_read_pos_tmp, bs_tmp, bs_tmp_size[0]);
        bs_type = detect_bs_type();
        bs_read_pos_tmp += bs_size;

        if (bs_type == SEQ_HEAD)
        {
            bs_size = read_a_bs(bs_head_fp, &bs_read_pos_head, bs_tmp, bs_head_size);
            bs_type_tmp = detect_bs_type();
            assert(bs_type_tmp == SEQ_HEAD);

            //to-do
            if (bs_type_tmp == SEQ_HEAD)
            {
                memcpy(bs_final + bs_read_pos, bs_tmp, bs_size * sizeof(unsigned char));
                bs_read_pos += bs_size;
            }
        }
        else if (bs_type == PIC_HEAD)
        {
            //add pic head
            while (1)
            {
                bs_size = read_a_bs(bs_fp[0], &bs_read_pos_patch[0], bs_tmp, bs_tmp_size[0]);
                bs_type_tmp = detect_bs_type();
                bs_read_pos_patch[0] += bs_size;

                if (bs_type_tmp == PIC_HEAD)
                {
                    memcpy(bs_final + bs_read_pos, bs_tmp, bs_size * sizeof(unsigned char));
                    bs_read_pos += bs_size;
                    break;
                }
            }

            //add patch ctu info
            for (int i = 0; i < patch_cnt; i++)
            {
                while (1)
                {
                    bs_size = read_a_bs(bs_fp[i], &bs_read_pos_patch[i], bs_tmp, bs_tmp_size[i]);
                    bs_type_tmp = detect_bs_type();
                    bs_read_pos_patch[i] += bs_size;

                    if (bs_type_tmp == CTU_INFO)
                    {
                        memcpy(bs_final + bs_read_pos, bs_tmp, bs_size * sizeof(unsigned char));
                        bs_read_pos += bs_size;
                        break;
                    }
                }

                if (i < patch_cnt - 1)
                {
                    patch_idx_pos[i] = bs_read_pos + 3;
                }
            }

            for (int i = 0; i < patch_cnt - 1; i++)
            {
                if (i == 0)
                {
                    bs_final[patch_idx_pos[i]] = 0x01;  //add patch idx 
                }
                else if (i == 1)
                {
                    bs_final[patch_idx_pos[i]] = 0x02;  //add patch idx 
                }
                else if (i == 2)
                {
                    bs_final[patch_idx_pos[i]] = 0x03;  //add patch idx 
                }
                else if (i == 3)
                {
                    bs_final[patch_idx_pos[i]] = 0x04;  //add patch idx 
                }
                else if (i == 4)
                {
                    bs_final[patch_idx_pos[i]] = 0x05;  //add patch idx 
                }
                else if (i == 5)
                {
                    bs_final[patch_idx_pos[i]] = 0x06;  //add patch idx 
                }
                else if (i == 6)
                {
                    bs_final[patch_idx_pos[i]] = 0x07;  //add patch idx 
                }
                else if (i == 7)
                {
                    bs_final[patch_idx_pos[i]] = 0x08;  //add patch idx 
                }
            }
            break;
        }
        else if (bs_type == CTU_INFO)
        {
            continue;
        }
        else if (bs_type == SEQ_END)
        {
            while (1)
            {
                bs_size = read_a_bs(bs_head_fp, &bs_read_pos_head, bs_tmp, bs_head_size);
                bs_type_tmp = detect_bs_type();
                bs_read_pos_head += bs_size;

                if (bs_type_tmp == SEQ_END)
                {
                    memcpy(bs_final + bs_read_pos, bs_tmp, bs_size * sizeof(unsigned char));
                    bs_read_pos += bs_size;
                    break;
                }
            }
        }
        else
        {
            assert(bs_type != ERR_INFO);
        }
    }
}


void Multislice::bs_process(
    int split_mode, 
    int frame_num,
    const int patch_num)
{
    unsigned char* bs = bs_final;
    int size = 0; 
    FILE* fp;
    fp = fopen(op_fname_out, "ab");

    if (split_mode == NONE_SPLIT)
    {
        size = encoder[PATCH_CNT].get_bs_size();
        memcpy(bs, encoder[PATCH_CNT].get_bs_addr(), size);
    }
    else
    {
        for (int i = 0; i < frame_num; i++)
        {
            bs_process_one_frame(split_mode, patch_num);
        }
        size = bs_read_pos;
    }

    fwrite(bs, 1, size, fp);
    unsigned char video_sequence_end_code;
    video_sequence_end_code = 0x00;
    fwrite(&video_sequence_end_code, 1, 1, fp);
    video_sequence_end_code = 0x00;
    fwrite(&video_sequence_end_code, 1, 1, fp);
    video_sequence_end_code = 0x01;
    fwrite(&video_sequence_end_code, 1, 1, fp);
    video_sequence_end_code = 0xB1;
    fwrite(&video_sequence_end_code, 1, 1, fp);
    fclose(fp);
}



void Multislice::yuv_split(
    const char* org_file,
    const int pic_width,
    const int pic_height,
    const int bit_depth,
    const int patch_width,
    const int patch_height,
    const int patch_idx,
    const int split_mode,
    const int frame_num,
    const int is_last_patch
)
{
    FILE* org_fp = fopen(org_file, "rb");
    string patch_yuv_file = org_file + std::to_string(patch_idx) + ".patch";
    remove_file(patch_yuv_file);
    FILE* fp_out = fopen(patch_yuv_file.c_str(), "ab");
    int size_yuv_org[3];
    int size_yuv_split[3];
    int scale = (bit_depth == 10) ? 2 : 1;
    int num_comp = 3;
    int x_pos = 0;
    int y_pos = 0;

    size_yuv_org[0] = pic_width * pic_height;
    size_yuv_org[1] = size_yuv_org[2] = (pic_width >> 1) * (pic_height >> 1);
    size_yuv_split[0] = patch_width * patch_height;
    size_yuv_split[1] = size_yuv_split[2] = (patch_width >> 1) * (patch_height >> 1);

    int split_mode_in = parse_multislice_mode(split_mode);

    if (patch_idx == 0)
    {
        x_pos = 0;
        y_pos = 0;
    }
    else
    {
        if (split_mode_in == HOR_BI)
        {
            x_pos = 0;
            y_pos = pic_height - patch_height;
        }
        else if (split_mode_in == VER_BI)
        {
            x_pos = pic_width - patch_width;
            y_pos = 0;
        }
        else if (split_mode_in == QUARD)
        {
            if (patch_idx == 1)
            {
                x_pos = pic_width - patch_width;
                y_pos = 0;
            }
            else if (patch_idx == 2)
            {
                x_pos = 0;
                y_pos = pic_height - patch_height;
            }
            else
            {
                x_pos = pic_width - patch_width;
                y_pos = pic_height - patch_height;
            }
        }
        else if (split_mode_in == CUSTOM_HOR)
        {
            x_pos = 0;
            y_pos = is_last_patch ? pic_height - patch_height : patch_height * patch_idx;
        }
        else if (split_mode_in == CUSTOM_VER)
        {
            x_pos = is_last_patch ? pic_width - patch_width : patch_width * patch_idx;
            y_pos = 0;
        }
    }

    for (int i = 0; i < frame_num; i++)
    {
        for (int comp = 0; comp < num_comp; comp++)
        {
            int size_byte_org = ((pic_width * pic_height) >> (comp > 0 ? 2 : 0)) * scale;
            unsigned char* buf_org = (unsigned char*)malloc(size_byte_org);
            int size_byte_split = ((patch_width * patch_height) >> (comp > 0 ? 2 : 0)) * scale;
            unsigned char* buf_split = (unsigned char*)malloc(size_byte_split);
            int width_split = patch_width >> (comp > 0 ? 1 : 0);
            int height_split = patch_height >> (comp > 0 ? 1 : 0);
            int width_org = pic_width >> (comp > 0 ? 1 : 0);
            int height_org = pic_height >> (comp > 0 ? 1 : 0);

            if (fread(buf_org, 1, size_byte_org, org_fp) != (unsigned)size_byte_org)
            {
                assert(0);
            }

            for (int y = 0; y < height_split; y++)
            {
                int ind_src = (y + (y_pos >> (comp > 0 ? 1 : 0))) * width_org;
                int ind_dst = y * width_split;
                for (int x = 0; x < width_split; x++)
                {
                    int i_dst = ind_dst + x;
                    int i_src = ind_src + x + (x_pos >> (comp > 0 ? 1 : 0));
                    if (bit_depth == 10)
                    {
                        buf_split[i_dst] = (buf_org[i_src * 2] | (buf_org[i_src * 2 + 1] << 8));
                    }
                    else
                    {
                        buf_split[i_dst] = buf_org[i_src];
                    }
                }
            }

            fwrite(buf_split, 1, size_byte_split, fp_out);

            free(buf_org);
            free(buf_split);
        }

    }
    
    fclose(fp_out);
    fclose(org_fp);
}

void Multislice::yuv_merge(
    const char* org_rec_file,
    const int pic_width,
    const int pic_height,
    const int bit_depth,
    const int split_mode,
    const int frame_num,
    const int patch_num
)
{
    int split_mode_in = parse_multislice_mode(split_mode);

    string patch_rec_file[PATCH_CNT];
    remove_file(org_rec_file);
    FILE* org_rec_fp = fopen(org_rec_file, "ab");
    FILE* patch_rec_fp[PATCH_CNT];
    long long patch_fp_cnt[PATCH_CNT] = { 0 };

    for (int i = 0; i < patch_num; i++)
    {
        patch_rec_file[i] = org_rec_file + std::to_string(i) + ".patch";
        patch_rec_fp[i] = fopen(patch_rec_file[i].c_str(), "rb");
    }

    int size_yuv_org[3];
    size_yuv_org[0] = pic_width * pic_height;
    size_yuv_org[1] = size_yuv_org[2] = (pic_width >> 1) * (pic_height >> 1);
    int scale = (bit_depth == 10) ? 2 : 1;
    int num_comp = 3;

    for (int i = 0; i < frame_num; i++)
    {
        for (int comp = 0; comp < num_comp; comp++)
        {
            int size_byte_org = ((pic_width * pic_height) >> (comp > 0 ? 2 : 0)) * scale;
            unsigned char* buf_rec_out = (unsigned char*)malloc(size_byte_org);

            for (int j = 0; j < patch_num; j++)
            {
                int patch_width = 0;
                int patch_height = 0;
                get_patch_size(
                    split_mode,
                    j,
                    pic_width,
                    pic_height,
                    patch_num,
                    patch_width,
                    patch_height
                );
                int size_yuv_split[3];
                size_yuv_split[0] = patch_width * patch_height;
                size_yuv_split[1] = size_yuv_split[2] = (patch_width >> 1) * (patch_height >> 1);
                int x_pos = 0;
                int y_pos = 0;
                if (j == 0)
                {
                    x_pos = 0;
                    y_pos = 0;
                }
                else
                {
                    if (split_mode_in == HOR_BI)
                    {
                        x_pos = 0;
                        y_pos = pic_height - patch_height;
                    }
                    else if (split_mode_in == VER_BI)
                    {
                        x_pos = pic_width - patch_width;
                        y_pos = 0;
                    }
                    else if (split_mode_in == QUARD)
                    {
                        if (j == 1)
                        {
                            x_pos = pic_width - patch_width;
                            y_pos = 0;
                        }
                        else if (j == 2)
                        {
                            x_pos = 0;
                            y_pos = pic_height - patch_height;
                        }
                        else
                        {
                            x_pos = pic_width - patch_width;
                            y_pos = pic_height - patch_height;
                        }
                    }
                    else if (split_mode_in == CUSTOM_HOR)
                    {
                        x_pos = 0;
                        int is_last_patch = (j == patch_num - 1);
                        y_pos = is_last_patch ? pic_height - patch_height : patch_height * j;
                    }
                    else if (split_mode_in == CUSTOM_VER)
                    {
                        int is_last_patch = (j == patch_num - 1);
                        x_pos = is_last_patch ? pic_width - patch_width : patch_width * j;
                        y_pos = 0;
                    }
                }


                int size_byte_split = ((patch_width * patch_height) >> (comp > 0 ? 2 : 0)) * scale;
                unsigned char* buf_split_rec = (unsigned char*)malloc(size_byte_split);
                int width_split = patch_width >> (comp > 0 ? 1 : 0);
                int height_split = patch_height >> (comp > 0 ? 1 : 0);
                int width_org = pic_width >> (comp > 0 ? 1 : 0);
                int height_org = pic_height >> (comp > 0 ? 1 : 0);

                fseek(patch_rec_fp[j], patch_fp_cnt[j], SEEK_SET);

                if (fread(buf_split_rec, 1, size_byte_split, patch_rec_fp[j]) != (unsigned)size_byte_split)
                {
                    assert(0);
                }
                patch_fp_cnt[j] += size_byte_split;

                for (int y = 0; y < height_split; y++)
                {
                    int ind_dst = (y + (y_pos >> (comp > 0 ? 1 : 0))) * width_org;
                    int ind_src = y * width_split;
                    for (int x = 0; x < width_split; x++)
                    {
                        int i_src = ind_src + x;
                        int i_dst = ind_dst + x + (x_pos >> (comp > 0 ? 1 : 0));
                        if (bit_depth == 10)
                        {
                            buf_rec_out[i_dst] = (buf_split_rec[i_src * 2] | (buf_split_rec[i_src * 2 + 1] << 8));
                        }
                        else
                        {
                            buf_rec_out[i_dst] = buf_split_rec[i_src];
                        }
                    }
                }

                free(buf_split_rec);
            }
            
            fwrite(buf_rec_out, 1, size_byte_org, org_rec_fp);
            free(buf_rec_out);
        }

    }

    fclose(org_rec_fp);
    for (int i = 0; i < patch_num; i++)
    {
        fclose(patch_rec_fp[i]);
    }
}

Encoder::Encoder()
{
    bs_buf_lib_tmp = (unsigned char*)malloc(MAX_BS_BUF);
    bs_buf_tmp = (unsigned char*)malloc(MAX_BS_BUF);
}

Multislice::Multislice()
{
    bs_final = (unsigned char*)malloc(MAX_BS_BUF);
    bs_tmp = (unsigned char*)malloc(MAX_BS_BUF);
}

Encoder::~Encoder()
{
    free(bs_buf_lib_tmp);
    free(bs_buf_tmp);
}

Multislice::~Multislice()
{
    free(bs_final);
    free(bs_tmp);
}
