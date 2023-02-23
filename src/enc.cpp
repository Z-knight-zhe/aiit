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

#include <math.h>

#include "enc_def.h"
#include "enc_eco.h"
#include "com_util.h"
#include "aec.h"
#include "iime.h"
#include "fme.h"
#include "dbk.h"
#include "sao.h"
#include "fetch.h"
#include "rmd.h"
#include "md1.h"
#include "md2.h"
#include "rfc_cpr.hpp"
#include "rfc_dcp.hpp"
#include "lookahead.h"
#include "rate_control_picture.h"
#include "TestVector.h"
#include "avs_reg.h"
#include "pcie_utils.h"
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#pragma warning(disable:6297)
#pragma warning(disable:6011)
#if RATECONTROL
extern int op_enable_ratectrl;
#include "rate_control.h"
#include "rc_top.h"
#include "com_port.h"
//#include "enc_rcdata.h"
#include <string.h>
RC_TOP rc_top;
#endif
#if RATECONTROL
extern RateControl* pRC;

RateControl RC;
extern void RC_init_frame();
#endif
extern int op_head_info_flag;
int pic_cnt;
int isProblem = 0;
FILE* bs_fp;
int header_length;

RDOQ_MODEL rdoq_model;
RDOQ_MODEL model_cnt;
AEC_FW aec_fw;
FRAME_LEVEL_FW frame_level_fw;
SRCC_FOR_AEC srcc_for_aec;
CTU_Info core;
MD_INPUT md_input;
strFetch_Orig_Lcu0 Fetch_Orig_Lcu0;
COM_MOTION best_motion_cands[ALLOWED_HMVP_NUM];
s8 best_cnt_hmvp_cands;
extern void enc_sbac_encode_bin_trm(u32 bin, ENC_SBAC *sbac, COM_BSW *bs);
extern void write_param_sao_one_lcu(ENC_CTX *ctx, int y_pel, int x_pel, int MergeLeftAvail, int MergeUpAvail, SAOBlkParam *sao_blk_param);

const s8 com_tbl_log2[257] = {
    /* 0, 1 */
    -1, -1,
    /* 2, 3 */
    1, -1,
    /* 4 ~ 7 */
    2, -1, -1, -1,
    /* 8 ~ 15 */
    3, -1, -1, -1, -1, -1, -1, -1,
    /* 16 ~ 31 */
    4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* 31 ~ 63 */
    5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* 64 ~ 127 */
    6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* 128 ~ 255 */
    7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* 256 */
    8};

#define CONV_LOG2(v) (com_tbl_log2[v])

extern int com_bsw_flush(COM_BSW *bs);
extern int imgb_addref(COM_IMGB *imgb);
extern int imgb_getref(COM_IMGB *imgb);
extern int imgb_release(COM_IMGB *imgb);
int g_DOIPrev;

SAOBlkParam cur_saoParam[N_C];
SAOBlkParam rec_saoParam[MAX_NUM_LCU_ROW + 2][N_C];
ENC_BEF_DATA bef_data[5][5][(1 << 7 >> 1) * (1 << 7 >> 1) / ((1 << 2) * (1 << 2))];

#if USE_ENH
extern void enc_sbac_encode_bin_trm(u32 bin, ENC_SBAC *sbac, COM_BSW *bs);
extern void write_param_sao_one_lcu(ENC_CTX *ctx, int y_pel, int x_pel, int MergeLeftAvail, int MergeUpAvail, SAOBlkParam *sao_blk_param);
#endif
#define ENTROPY_BITS_TABLE_BiTS 10
#define ENTROPY_BITS_TABLE_BiTS_SHIFP (PROB_BITS - ENTROPY_BITS_TABLE_BiTS)
#define ENTROPY_BITS_TABLE_SIZE (1 << ENTROPY_BITS_TABLE_BiTS)
s32 entropy_bits[ENTROPY_BITS_TABLE_SIZE];

s64 err_scale_tbl[80][MAX_CU_DEPTH + 1];                    // [64 + 16][MAX_CU_DEPTH]
s64_affine_merge err_scale_tbl_quant[80][MAX_CU_DEPTH + 1]; // [64 + 16][MAX_CU_DEPTH]

#define ENTROPY_BITS_TABLE_BiTS 10
#define ENTROPY_BITS_TABLE_BiTS_SHIFP (PROB_BITS - ENTROPY_BITS_TABLE_BiTS)
#define ENTROPY_BITS_TABLE_SIZE (1 << ENTROPY_BITS_TABLE_BiTS)

#if FETCH_LUKE
strFetch_Orig_Lcu Fetch_Orig_Lcu;
FETCH_ORGI_LCU_IME Fetch_Orig_Lcu_IME;
strFetch_input_video Fetch_input_video;
strFetch_input_video_U8 Fetch_input_video_U8;
strFetch_input_video_U8_ptr Fetch_input_video_U8_ptr;
SKIP_MERGE_INPUT skip_input;
DBK_INPUT dbk_input;
CTU_Level_Parameters fetch_output;
strFetch_ref_window Fetch_Ref_window;
strFetch_ref_window Fetch_Ref_window_md2chroma;
FETCH_REF_WINDOW_IME Fetch_Ref_Window_IME;

#endif
#if CTU_CONTROL_LUKE
Slice_Level_Parameters ctu_ctrl_input;

// int lcu_x;
// int lcu_y;
#endif

FETCH2MD_FW fetch2md_fw;
FETCH2MD_FW *fetch2md_fw_ptr = NULL;
FETCH2DBK_FW fetch2dbk_fw;
FETCH2DBK_FW *fetch2dbk_fw_ptr = NULL;
FETCH2SAO_FW fetch2sao_fw;
FETCH2SAO_FW *fetch2sao_fw_ptr = NULL;
SAO2AEC *AEC_DATA_input = NULL;
SAO2AEC AEC_DATA_input_1;

AEC_FW fetch2dbk_firmware;
AEC_FW fetch2aec_firmware;

RMD_OUTPUT g_rmd_output;
RMD_BUF g_rmd_buf;

extern ENC_ME_CU *me_cu_info_global;
extern ENC_ME_CU me_cu_info_blk;

/* Convert ENC into ENC_CTX */
#define ENC_ID_TO_CTX_R(id, ctx)             \
    com_assert_r((id))(ctx) = (ENC_CTX *)id; \
    com_assert_r((ctx)->magic == ENC_MAGIC_CODE)

/* Convert ENC into ENC_CTX with return value if assert on */
#define ENC_ID_TO_CTX_RV(id, ctx, ret) \
    com_assert_rv((id), (ret));        \
    (ctx) = (ENC_CTX *)id;             \
    com_assert_rv((ctx)->magic == ENC_MAGIC_CODE, (ret));

#define USE_NEW_YUV 1

#if USE_NEW_YUV
// SAO rec. pic. buffer, send to RFC.
static pel sao_pic_rec_test_y[72][64];
static pel sao_pic_rec_test_u[36][32];
static pel sao_pic_rec_test_v[36][32];

static pel sao_pic_rec_test_y_last[72][64];
static pel sao_pic_rec_test_u_last[36][32];
static pel sao_pic_rec_test_v_last[36][32];
#else
// SAO rec. pic. buffer, send to RFC.
static pel sao_pic_rec_test_y[69][69];
static pel sao_pic_rec_test_u[36][36];
static pel sao_pic_rec_test_v[36][36];
#endif

static const s8 tbl_poc_gop_offset[5][15] =
    {
        {-1, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF},       /* gop_size = 2 */
        {-2, -3, -1, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF},                   /* gop_size = 4 */
        {-4, -6, -7, -5, -2, -3, -1, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF},                                           /* gop_size = 8 */
        {(s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF}, /* gop_size = 12 */
        {-8, -12, -14, -15, -13, -10, -11, -9, -4, -6, -7, -5, -2, -3, -1}                                                                                      /* gop_size = 16 */
};

static const s8 tbl_ref_depth[5][15] =
    {
        /* gop_size = 2 */
        {FRM_DEPTH_1, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF},
        /* gop_size = 4 */
        {FRM_DEPTH_1, FRM_DEPTH_2, FRM_DEPTH_2, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF},
        /* gop_size = 8 */
        {
            FRM_DEPTH_1, FRM_DEPTH_2, FRM_DEPTH_3, FRM_DEPTH_3, FRM_DEPTH_2, FRM_DEPTH_3, FRM_DEPTH_3,
            (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF},
        /* gop_size = 12 */
        {(s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF}, /* gop_size = 12 */
        /* gop_size = 16 */
        {
            FRM_DEPTH_1, FRM_DEPTH_2, FRM_DEPTH_3, FRM_DEPTH_4, FRM_DEPTH_4, FRM_DEPTH_3, FRM_DEPTH_4,
            FRM_DEPTH_4, FRM_DEPTH_2, FRM_DEPTH_3, FRM_DEPTH_4, FRM_DEPTH_4, FRM_DEPTH_3, FRM_DEPTH_4, FRM_DEPTH_4}};
static const s8 tbl_slice_ref[5][15] =
    {
        /* gop_size = 2 */
        {1, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF},
        /* gop_size = 4 */
        {1, 0, 0, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF},
        /* gop_size = 8 */
        {1, 1, 0, 0, 1, 0, 0, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF},
        /* gop_size = 12 */
        {(s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF},
        /* gop_size = 16 */
        {1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0}};

#if USE_ZXZ_HGOP
static const s8 tbl_slice_depth_P[GOP_P] =
    {FRM_DEPTH_3, FRM_DEPTH_2, FRM_DEPTH_3, FRM_DEPTH_1};
#else
static const s8 tbl_slice_depth_P[GOP_P] =
    {FRM_DEPTH_3, FRM_DEPTH_2, FRM_DEPTH_3, FRM_DEPTH_1};
#endif
static const s8 tbl_slice_depth[5][15] =
    {
        /* gop_size = 2 */
        {
            FRM_DEPTH_2, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF,
            (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF},
        /* gop_size = 4 */
        {
            FRM_DEPTH_2, FRM_DEPTH_3, FRM_DEPTH_3, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF,
            (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF},
        /* gop_size = 8 */
        {
            FRM_DEPTH_2, FRM_DEPTH_3, FRM_DEPTH_4, FRM_DEPTH_4, FRM_DEPTH_3, FRM_DEPTH_4, FRM_DEPTH_4,
            (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF},
        /* gop_size = 12 */
        {(s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF, (s8)0xFF},
        /* gop_size = 16 */
        {
            FRM_DEPTH_2, FRM_DEPTH_3, FRM_DEPTH_4, FRM_DEPTH_5, FRM_DEPTH_5, FRM_DEPTH_4, FRM_DEPTH_5,
            FRM_DEPTH_5, FRM_DEPTH_3, FRM_DEPTH_4, FRM_DEPTH_5, FRM_DEPTH_5, FRM_DEPTH_4, FRM_DEPTH_5, FRM_DEPTH_5}};
static const int com_tbl_qp_chroma_adjust[64] = {
    // this table aligned with spec
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
    20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
    30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
    40, 41, 42, 42, 43, 43, 44, 44, 45, 45,
    46, 46, 47, 47, 48, 48, 48, 49, 49, 49,
    50, 50, 50, 51};

static const int com_tbl_qp_chroma_adjust_enc[64] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
    20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
    30, 31, 32, 33, 33, 34, 35, 36, 37, 38,
    39, 40, 41, 42, 43, 44, 44, 44, 45, 46,
    47, 48, 48, 49, 50, 51, 52, 53, 54, 55,
    56, 57, 58, 59};

extern const s8 com_tbl_log2[257];

// extern void init_dct_coef();
extern void com_split_get_split_rdo_order(U8 cu_width, U8 cu_height, U3 splits[MAX_SPLIT_NUM]);
extern void com_split_get_part_structure(U3 split_mode, U14 x0, U14 y0, U8 cu_width, U8 cu_height, U12 cup, U4 cud,
                                         U3 log2_culine, COM_SPLIT_STRUCT *split_struct);

int get_transform_shift(int bit_depth, int tr_size_log2)
{

#pragma HLS INLINE off
    return 15 - bit_depth - tr_size_log2;
}

static ENC_CTX *ctx_alloc(void)
{
    ENC_CTX *ctx;
    ctx = (ENC_CTX *)com_malloc_fast(sizeof(ENC_CTX));
    com_assert_rv(ctx, NULL);
    com_mset_x64a(ctx, 0, sizeof(ENC_CTX));
    return ctx;
}

static void ctx_free(ENC_CTX *ctx)
{
    com_mfree_fast(ctx);
}

static ENC_CORE *core_alloc(void)
{
    ENC_CORE *core;
    core = (ENC_CORE *)com_malloc_fast(sizeof(ENC_CORE));
    com_assert_rv(core, NULL);
    com_mset_x64a(core, 0, sizeof(ENC_CORE));

    enc_create_cu_data(&core->cu_data_best_top, MAX_CU_DEPTH - 1, MAX_CU_DEPTH - 1);

    return core;
}

static void core_free(ENC_CORE *core)
{

    enc_delete_cu_data(&core->cu_data_best_top);

    com_mfree_fast(core);
}

static int set_init_param(ENC_PARAM *param, ENC_PARAM *param_input)
{
    com_mcpy(param, param_input, sizeof(ENC_PARAM));
    /* check input parameters */
    com_assert_rv(param->pic_width > 0 && param->pic_height > 0, COM_ERR_INVALID_ARGUMENT);
    com_assert_rv((param->pic_width & (MIN_CU_SIZE - 1)) == 0, COM_ERR_INVALID_ARGUMENT);
    com_assert_rv((param->pic_height & (MIN_CU_SIZE - 1)) == 0, COM_ERR_INVALID_ARGUMENT);
    com_assert_rv(param->qp >= (MIN_QUANT - (8 * (param->bit_depth_internal - 8))), COM_ERR_INVALID_ARGUMENT);
    com_assert_rv(param->qp <= MAX_QUANT_BASE,
                  COM_ERR_INVALID_ARGUMENT); // this assertion is align with the constraint for input QP
    com_assert_rv(param->i_period >= 0, COM_ERR_INVALID_ARGUMENT);
    if (!param->disable_hgop)
    {
        com_assert_rv(param->max_b_frames == 0 || param->max_b_frames == 1 ||
                          param->max_b_frames == 3 || param->max_b_frames == 7 ||
                          param->max_b_frames == 15,
                      COM_ERR_INVALID_ARGUMENT);
        if (param->max_b_frames != 0)
        {
            if (param->i_period % (param->max_b_frames + 1) != 0)
            {
                com_assert_rv(0, COM_ERR_INVALID_ARGUMENT);
            }
        }
    }
    /* set default encoding parameter */
    param->f_ifrm = 0;
    param->use_pic_sign = 0;
    param->gop_size = param->max_b_frames + 1;
    return COM_OK;
}

static void set_cnkh(ENC_CTX *ctx, COM_CNKH *cnkh, int ver, int ctype)
{
    cnkh->ver = ver;
    cnkh->ctype = ctype;
    cnkh->broken = 0;
}

int load_wq_matrix(char *fn, u8 *m4x4_out, u8 *m8x8_out)
{
    int i, m4x4[16], m8x8[64];
    sscanf(fn,
           "[%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d]\n",
           &m4x4[0], &m4x4[1], &m4x4[2], &m4x4[3], &m4x4[4], &m4x4[5], &m4x4[6], &m4x4[7],
           &m4x4[8], &m4x4[9], &m4x4[10], &m4x4[11], &m4x4[12], &m4x4[13], &m4x4[14], &m4x4[15],
           &m8x8[0], &m8x8[1], &m8x8[2], &m8x8[3], &m8x8[4], &m8x8[5], &m8x8[6], &m8x8[7],
           &m8x8[8], &m8x8[9], &m8x8[10], &m8x8[11], &m8x8[12], &m8x8[13], &m8x8[14], &m8x8[15],
           &m8x8[16], &m8x8[17], &m8x8[18], &m8x8[19], &m8x8[20], &m8x8[21], &m8x8[22], &m8x8[23],
           &m8x8[24], &m8x8[25], &m8x8[26], &m8x8[27], &m8x8[28], &m8x8[29], &m8x8[30], &m8x8[31],
           &m8x8[32], &m8x8[33], &m8x8[34], &m8x8[35], &m8x8[36], &m8x8[37], &m8x8[38], &m8x8[39],
           &m8x8[40], &m8x8[41], &m8x8[42], &m8x8[43], &m8x8[44], &m8x8[45], &m8x8[46], &m8x8[47],
           &m8x8[48], &m8x8[49], &m8x8[50], &m8x8[51], &m8x8[52], &m8x8[53], &m8x8[54], &m8x8[55],
           &m8x8[56], &m8x8[57], &m8x8[58], &m8x8[59], &m8x8[60], &m8x8[61], &m8x8[62], &m8x8[63]);

    for (i = 0; i < 16; i++)
    {
        m4x4_out[i] = m4x4[i];
    }
    for (i = 0; i < 64; i++)
    {
        m8x8_out[i] = m8x8[i];
    }

    return COM_OK;
}

static int find_level_id_idx(int level_id)
{
    static u8 level_id_tbl[24] = {0x00, 0x10, 0x12, 0x14, 0x20,
                                  0x22, 0x40, 0x42, 0x44, 0x46,
                                  0x48, 0x4a, 0x50, 0x52, 0x54,
                                  0x56, 0x58, 0x5a, 0x60, 0x62,
                                  0x64, 0x66, 0x68, 0x6a};

    int idx = -1;
    int i;
    for (i = 0; i < 24; i++)
    {
        if (level_id_tbl[i] == level_id)
        {
            idx = i;
        }
    }
    assert(idx > 0 && idx < 24);
    return idx;
}

#if USE_ENH
static void set_sqh(ENC_CTX *ctx, COM_SQH *sqh)
{
    int i, level_id_idx;
    static int fps_code_tbl[14] = {-1, -1, 24, 25, -1, 30, 50, -1, 60, 100, 120, 200, 240, 300};
#if BUGFIX_MAX_DPB_SIZE
    static unsigned int max_bbv_size_tbl[42] = {0, 1507328, 2015232, 2506752, 6012928,
                                                10010624, 12009472, 30015488, 12009472, 30015488,
                                                20004864, 50003968, 20004864, 50003968, 25001984,
                                                100007936, 25001984, 100007936, 25001984, 100007936,
                                                25001984, 100007936, 40009728, 160006144, 40009728,
                                                160006144, 60014592, 240009216, 60014592, 240009216,
                                                60014592, 240009216, 60014592, 240009216, 120012800,
                                                480002048, 120012800, 480002048, 240009216, 800014336,
                                                240009216, 800014336};
    static unsigned int max_bitrate_tbl[42] = {0, 1500000, 2000000, 2500000, 6000000,
                                               10000000, 12000000, 30000000, 12000000, 30000000,
                                               20000000, 50000000, 20000000, 50000000, 25000000,
                                               100000000, 25000000, 100000000, 25000000, 100000000,
                                               25000000, 100000000, 40000000, 160000000, 40000000,
                                               160000000, 60000000, 240000000, 60000000, 240000000,
                                               60000000, 240000000, 60000000, 240000000, 120000000,
                                               480000000, 120000000, 480000000, 240000000, 800000000,
                                               240000000, 800000000};
    static unsigned int max_dpb_size_in_bits_tbl[42] = {
        0, MAX_INT, MAX_INT, MAX_INT, MAX_INT,
        MAX_INT, 14622720, 14622720, 16711680, 16711680,
        14622720, 14622720, 16711680, 16711680, 14622720,
        14622720, 16711680, 16711680, 58490880, 58490880,
        66846720, 66846720, 58490880, 58490880, 66846720,
        66846720, 58490880, 58490880, 66846720, 66846720,
        226492416, 226492416, 301989888, 301989888, 226492416,
        226492416, 301989888, 301989888, 226492416, 226492416,
        301989888, 301989888};
#else
    static unsigned int max_bbv_size_tbl[24] = {0, 1507328, 2015232, 2506752, 6012928,
                                                10010624, 12009472, 30015488, 20004864, 50003968,
                                                25001984, 100007936, 25001984, 100007936, 40009728,
                                                160006144, 60014592, 240009216, 60014592, 240009216,
                                                120012800, 480002048, 240009216, 800014336};
    static unsigned int max_bitrate_tbl[24] = {0, 1500000, 2000000, 2500000, 6000000,
                                               10000000, 12000000, 30000000, 20000000, 50000000,
                                               25000000, 100000000, 25000000, 100000000, 40000000,
                                               160000000, 60000000, 240000000, 60000000, 240000000,
                                               120000000, 480000000, 240000000, 800000000};
#endif

    sqh->profile_id = PROFILE_ID;
    sqh->level_id = LEVEL_ID;
    level_id_idx = find_level_id_idx(sqh->level_id);
    sqh->progressive_sequence = 1;
    sqh->field_coded_sequence = 0;
#if LIBVC_ON
    sqh->library_stream_flag = ctx->rpm.libvc_data->is_libpic_processing ? 1 : 0;
    sqh->library_picture_enable_flag = (u8)ctx->param.library_picture_enable_flag;
    sqh->duplicate_sequence_header_flag = 1;
#endif
    sqh->sample_precision = ctx->param.bit_depth_input == 8 ? 1 : 2;
    sqh->aspect_ratio = 1;

    sqh->frame_rate_code = 15;

    for (i = 0; i < 14; i++)
    {
        if (ctx->param.fps == fps_code_tbl[i])
        {
            sqh->frame_rate_code = (u8)i;
            break;
        }
    }

    sqh->bit_rate_upper = (max_bitrate_tbl[level_id_idx] / 400) >> 18;
    sqh->bit_rate_lower = (max_bitrate_tbl[level_id_idx] / 400) - (sqh->bit_rate_upper << 18);
    sqh->low_delay = ctx->param.max_b_frames == 0 ? 1 : 0;
    sqh->temporal_id_enable_flag = 1;
    sqh->bbv_buffer_size = max_bbv_size_tbl[level_id_idx] / (1024 * 16);
#if BUGFIX_MAX_DPB_SIZE
    sqh->max_dpb_size = ctx->rpm.max_pb_size = COM_MIN(16, max_dpb_size_in_bits_tbl[level_id_idx] / (ctx->param.pic_width * ctx->param.pic_height));
#else
#if RPL_GOP32
    sqh->max_dpb_size = 8; // MX: for current CTC case(GOP32), the max dpb size needed is 8.
#else
    sqh->max_dpb_size = 7; // MX: for current CTC case(GOP16), the max dpb size needed is 7.
#endif
#endif

#if HLS_RPL
    sqh->rpls_l0_num = ctx->param.rpls_l0_cfg_num;
    sqh->rpls_l1_num = ctx->param.rpls_l1_cfg_num;
    sqh->rpl1_index_exist_flag = 1;
    sqh->rpl1_same_as_rpl0_flag = 0;

    memcpy(sqh->rpls_l0, ctx->param.rpls_l0, ctx->param.rpls_l0_cfg_num * sizeof(sqh->rpls_l0[0]));
    memcpy(sqh->rpls_l1, ctx->param.rpls_l1, ctx->param.rpls_l1_cfg_num * sizeof(sqh->rpls_l1[0]));
#endif

    sqh->horizontal_size = ctx->param.horizontal_size;
    sqh->vertical_size = ctx->param.vertical_size;
    sqh->log2_max_cu_width_height = (u8)ctx->info.log2_max_cuwh;
    sqh->min_cu_size = ctx->param.min_cu_size;
    sqh->max_part_ratio = ctx->param.max_part_ratio;
    sqh->max_split_times = ctx->param.max_split_times;
    sqh->min_qt_size = ctx->param.min_qt_size;
    sqh->max_bt_size = ctx->param.max_bt_size;
    sqh->max_eqt_size = ctx->param.max_eqt_size;
    sqh->max_dt_size = ctx->param.max_dt_size;
#if IPCM
    sqh->ipcm_enable_flag = ctx->param.ipcm_enable_flag;
#endif
    sqh->amvr_enable_flag = (u8)ctx->param.amvr_enable_flag;
    sqh->ipf_enable_flag = ctx->param.ipf_flag;
    sqh->affine_enable_flag = (u8)ctx->param.affine_enable_flag;
#if SMVD
    sqh->smvd_enable_flag = (u8)ctx->param.smvd_enable_flag;
#endif
    sqh->num_of_hmvp_cand = (u8)ctx->param.num_of_hmvp_cand;
    sqh->umve_enable_flag = (u8)ctx->param.umve_enable_flag;
#if EXT_AMVR_HMVP
    sqh->emvr_enable_flag = (u8)ctx->param.emvr_enable_flag;
#endif
#if TSCPM
    sqh->tscpm_enable_flag = (u8)ctx->param.tscpm_enable_flag;
#endif
#if SUB_TMVP
    sqh->sbtmvp_enable_flag = (u8)ctx->param.sbtmvp_enable_flag;
#endif

#if DT_PARTITION
    sqh->dt_intra_enable_flag = (u8)ctx->param.dt_intra_enable_flag;
#endif
    sqh->position_based_transform_enable_flag = (u8)ctx->param.position_based_transform_enable_flag;
    sqh->sample_adaptive_offset_enable_flag = (u8)ctx->param.sample_adaptive_offset_enable_flag;
    sqh->wq_enable = ctx->param.wq_enable;
    sqh->seq_wq_mode = ctx->param.seq_wq_mode;

    if (sqh->wq_enable)
    {
        if (sqh->seq_wq_mode)
        {
            load_wq_matrix(ctx->param.seq_wq_user, sqh->wq_4x4_matrix, sqh->wq_8x8_matrix);
        }
        else
        {
            memcpy(sqh->wq_4x4_matrix, tab_WqMDefault4x4, sizeof(tab_WqMDefault4x4));
            memcpy(sqh->wq_8x8_matrix, tab_WqMDefault8x8, sizeof(tab_WqMDefault8x8));
        }
    }

    sqh->secondary_transform_enable_flag = (u8)ctx->param.secondary_transform_enable_flag;
    sqh->chroma_format = ctx->param.chroma_format;
    sqh->encoding_precision = ctx->param.encoding_precision;

#if PHASE_2_PROFILE
    if (sqh->profile_id == 0x30 || sqh->profile_id == 0x32)
    {
        // intra
#if EIPM
        sqh->eipm_enable_flag = (u8)1;
#endif
#if PMC
        sqh->pmc_enable_flag = (u8)0;
#endif
#if ENHANCE_TSPCM
        sqh->enhance_tscpm_enable_flag = (u8)sqh->tscpm_enable_flag;
#endif
#if IIP
        sqh->iip_enable_flag = (u8)0;
#endif
#if SAWP
        sqh->sawp_enable_flag = (u8)0;
#endif
#if MIPF
        sqh->mipf_enable_flag = (u8)1;
#endif
#if IPF_CHROMA
        sqh->chroma_ipf_enable_flag = (u8)0;
#endif
        // inter
#if UMVE_ENH
        sqh->umve_enh_enable_flag = (u8)sqh->umve_enable_flag;
#endif
#if AFFINE_UMVE
#if SH_UNIFY
        sqh->affine_umve_enable_flag = (u8)sqh->affine_enable_flag;
#else
        if (sqh->affine_enable_flag && sqh->umve_enable_flag && (u8)ctx->param.affine_umve_enable_flag)
        {
            sqh->affine_umve_enable_flag = 1;
        }
        else
        {
            sqh->affine_umve_enable_flag = 0;
        }
#endif
#endif
#if ASP
        sqh->asp_enable_flag = (u8)0;
#endif
#if AWP
        sqh->awp_enable_flag = 0;
#endif
#if AWP_MVR
        sqh->awp_mvr_enable_flag = (u8)0;
#endif
#if SUB_TMVP
        sqh->sbtmvp_enable_flag = (u8)1;
#endif
#if SH_UNIFY
        sqh->emvp_enable_flag = 0;
#if ETMVP
        sqh->emvp_enable_flag = (u8)0;
#endif
#if MVAP
        sqh->emvp_enable_flag = (u8)0;
#endif
#if ETMVP
        sqh->etmvp_enable_flag = (u8)0;
#endif
#if MVAP
        sqh->mvap_enable_flag = (u8)0;
        sqh->num_of_mvap_cand = 0;
#endif
#else
#if MVAP
        sqh->mvap_enable_flag = (u8)ctx->param.mvap_enable_flag;
        sqh->num_of_mvap_cand = sqh->mvap_enable_flag ? ALLOWED_MVAP_NUM : 0;
#endif
#if ETMVP
        sqh->etmvp_enable_flag = (u8)ctx->param.etmvp_enable_flag;
        ;
#endif
#endif

#if DMVR
        sqh->dmvr_enable_flag = (u8)0;
#endif
#if BIO
        sqh->bio_enable_flag = (u8)0;
#endif
#if BGC
        sqh->bgc_enable_flag = (u8)0;
#endif
#if INTERPF
        sqh->interpf_enable_flag = (u8)ENABLE_INTERPF;
#endif
#if IPC
        sqh->ipc_enable_flag = (u8)0;
#endif
#if OBMC
        sqh->obmc_enable_flag = (u8)0;
#endif

        // transform
#if EST
        sqh->est_enable_flag = (u8)0;
#endif
#if ST_CHROMA
        sqh->st_chroma_enable_flag = (u8)0;
#endif
#if SBT
        sqh->sbt_enable_flag = (u8)0;
#endif
#if IST
        sqh->ist_enable_flag = (u8)0;
#endif
#if ISTS
#if SH_UNIFY
        // ists is always same with ist
        sqh->ists_enable_flag = sqh->ist_enable_flag;
#else
        sqh->ists_enable_flag = (u8)ctx->param.ists_enable_flag;
#endif
#endif
#if TS_INTER
#if SH_UNIFY
        // ts_inter is always same with ist
        sqh->ts_inter_enable_flag = sqh->ist_enable_flag;
#else
        sqh->ts_inter_enable_flag = (u8)ctx->param.ts_inter_enable_flag;
#endif
#endif

        // cabac
#if SRCC
        sqh->srcc_enable_flag = (u8)1;
#endif
#if CABAC_MULTI_PROB
        sqh->mcabac_enable_flag = (u8)1;
#endif

        // filter
#if DBK_SCC
#if SH_UNIFY
        sqh->loop_filter_type_enable_flag = 1;
#else
        sqh->loop_filter_type_enable_flag = ctx->param.loop_filter_type_enable_flag;
#endif
#endif
#if DBR
#if SH_UNIFY
        sqh->dbr_enable_flag = 1;
#else
        sqh->dbr_enable_flag = ctx->param.dbr_enable_flag;
#endif
        if (sqh->dbr_enable_flag)
        {
            for (i = 0; i < 6; i++)
            {
                ctx->dbr_pic_param[i].dbr_horizontal_enabled = FALSE;
                ctx->dbr_pic_param[i].dbr_vertical_enabled = FALSE;
#if EDBR
                ctx->dbr_pic_param[i].dbr_fs0_horizontal_enabled = FALSE;
                ctx->dbr_pic_param[i].dbr_fs0_vertical_enabled = FALSE;
#endif
                ctx->dbr_pic_poc[i] = 0;
            }
        }
#endif
#if ESAO
        sqh->esao_enable_flag = (u8)0;
#endif
#if CCSAO
        sqh->ccsao_enable_flag = (u8)0;
#endif
        sqh->adaptive_leveling_filter_enable_flag = (u8)0;
#if ALF_SHAPE || ALF_IMP || ALF_SHIFT
        sqh->adaptive_leveling_filter_enhance_flag = (u8)0;
#endif

        // SCC
#if USE_IBC
        sqh->ibc_flag = 0;
#endif
#if IBC_ABVR
        sqh->abvr_enable_flag = (u8)0;
#endif
#if USE_SP
        sqh->isc_enable_flag = (u8)0;
#endif
#if IBC_BVP
        sqh->num_of_hbvp_cand = (u8)0;
#endif
#if FIMC
        sqh->fimc_enable_flag = 0;
#endif
#if NN_HOOK
        sqh->nn_tools_set_hook = 0x00;
#if NN_FILTER
        sqh->nnlf_enable_flag = 0;
        sqh->num_of_nnlf = 0;
        sqh->nn_tools_set_hook = sqh->nn_tools_set_hook | (sqh->nnlf_enable_flag << 0);
#endif
#endif
    }
#endif

#if TRACE_REC // turn off SAO and ALF
    sqh->sample_adaptive_offset_enable_flag = 0;
    sqh->adaptive_leveling_filter_enable_flag = 0;
#if ESAO
    sqh->esao_enable_flag = 0;
#endif
#if CCSAO
    sqh->ccsao_enable_flag = 0;
#endif
#endif

    if (sqh->low_delay == 1)
    {
        sqh->output_reorder_delay = 0;
    }
    else
    {
        // for random access mode, GOP 16
        sqh->output_reorder_delay = com_tbl_log2[ctx->param.max_b_frames + 1]; // log2(GopSize)
        assert(com_tbl_log2[ctx->param.max_b_frames + 1] != -1);
    }

#if PATCH
    sqh->patch_stable = ctx->patch->stable;
    sqh->patch_ref_colocated = ctx->patch->ref_colocated;
    sqh->cross_patch_loop_filter = ctx->patch->cross_patch_loop_filter;
    if (ctx->patch->stable)
    {
        sqh->patch_uniform = ctx->patch->uniform;
        if (ctx->patch->uniform)
        {
            sqh->patch_width_minus1 = ctx->patch->width - 1;
            sqh->patch_height_minus1 = ctx->patch->height - 1;
        }
    }
#endif
}
#else
static void set_sqh(ENC_CTX *ctx, COM_SQH *sqh)
{
    int i, level_id_idx;
    static int fps_code_tbl[14] = {-1, -1, 24, 25, -1, 30, 50, -1, 60, 100, 120, 200, 240, 300};
    static unsigned int max_bbv_size_tbl[24] = {0, 1507328, 2015232, 2506752, 6012928,
                                                10010624, 12009472, 30015488, 20004864, 50003968,
                                                25001984, 100007936, 25001984, 100007936, 40009728,
                                                160006144, 60014592, 240009216, 60014592, 240009216,
                                                120012800, 480002048, 240009216, 800014336};
    static unsigned int max_bitrate_tbl[24] = {0, 1500000, 2000000, 2500000, 6000000,
                                               10000000, 12000000, 30000000, 20000000, 50000000,
                                               25000000, 100000000, 25000000, 100000000, 40000000,
                                               160000000, 60000000, 240000000, 60000000, 240000000,
                                               120000000, 480000000, 240000000, 800000000};
    sqh->profile_id = PROFILE_ID;
    sqh->level_id = LEVEL_ID;
    level_id_idx = find_level_id_idx(sqh->level_id);
    sqh->progressive_sequence = 1;
    sqh->field_coded_sequence = 0;

#if LIBVC_ON
    sqh->library_stream_flag = ctx->rpm.libvc_data->is_libpic_processing ? 1 : 0;
    sqh->library_picture_enable_flag = (u8)ctx->param.library_picture_enable_flag;
    sqh->duplicate_sequence_header_flag = 1;
#endif

    sqh->sample_precision = ctx->param.bit_depth_input == 8 ? 1 : 2;
    sqh->aspect_ratio = 1;

    sqh->frame_rate_code = 15;

    for (i = 0; i < 14; i++)
    {
        if (ctx->param.fps == fps_code_tbl[i])
        {
            sqh->frame_rate_code = (u8)i;
            break;
        }
    }

    sqh->bit_rate_upper = (max_bitrate_tbl[level_id_idx] / 400) >> 18;
    sqh->bit_rate_lower = (max_bitrate_tbl[level_id_idx] / 400) - (sqh->bit_rate_upper << 18);
    sqh->low_delay = ctx->param.max_b_frames == 0 ? 1 : 0;
    sqh->temporal_id_enable_flag = 1;
    sqh->bbv_buffer_size = max_bbv_size_tbl[level_id_idx] / (1024 * 16);
    sqh->max_dpb_size = 7; // MX: for current CTC case(GOP16), the avs3_max dpb size needed is 7.

#if HLS_RPL
    sqh->rpls_l0_num = ctx->param.rpls_l0_cfg_num;
    sqh->rpls_l1_num = ctx->param.rpls_l1_cfg_num;
    sqh->rpl1_index_exist_flag = 1;
    sqh->rpl1_same_as_rpl0_flag = 0;

    for (i = 0; i < ctx->param.rpls_l0_cfg_num; i++)
        sqh->rpls_l0[i] = ctx->param.rpls_l0[i];
    for (i = 0; i < ctx->param.rpls_l1_cfg_num; i++)
        sqh->rpls_l1[i] = ctx->param.rpls_l1[i];

#endif

    sqh->horizontal_size = ctx->param.horizontal_size;
    sqh->vertical_size = ctx->param.vertical_size;
    sqh->log2_max_cu_width_height = (u8)ctx->info.log2_max_cuwh;
    sqh->min_cu_size = ctx->param.min_cu_size;
    sqh->max_part_ratio = ctx->param.max_part_ratio;
    sqh->max_split_times = ctx->param.max_split_times;
    sqh->min_qt_size = ctx->param.min_qt_size;
    sqh->max_bt_size = ctx->param.max_bt_size;
    sqh->max_eqt_size = ctx->param.max_eqt_size;
    sqh->max_dt_size = ctx->param.max_dt_size;
    sqh->ipcm_enable_flag = ctx->param.ipcm_enable_flag;
    sqh->amvr_enable_flag = (u8)ctx->param.amvr_enable_flag;

    sqh->ipf_enable_flag = ctx->param.ipf_flag;
    sqh->affine_enable_flag = (u8)ctx->param.affine_enable_flag;
#if SMVD
    sqh->smvd_enable_flag = (u8)ctx->param.smvd_enable_flag;
#endif
    sqh->num_of_hmvp_cand = (u8)ctx->param.num_of_hmvp_cand;
    sqh->umve_enable_flag = (u8)ctx->param.umve_enable_flag;
#if EXT_AMVR_HMVP
    sqh->emvr_enable_flag = (u8)ctx->param.emvr_enable_flag;
#endif
#if TSCPM
    sqh->tscpm_enable_flag = (u8)ctx->param.tscpm_enable_flag;
#endif
#if DT_PARTITION
    sqh->dt_intra_enable_flag = (u8)ctx->param.dt_intra_enable_flag;
#endif
    sqh->position_based_transform_enable_flag = 0;
    sqh->sample_adaptive_offset_enable_flag = (u8)ctx->param.sample_adaptive_offset_enable_flag;
    sqh->adaptive_leveling_filter_enable_flag = (u8)ctx->param.adaptive_leveling_filter_enable_flag;
#if TRACE_REC // turn off SAO and ALF
    sqh->sample_adaptive_offset_enable_flag = 0;
    sqh->adaptive_leveling_filter_enable_flag = 0;
#endif
    sqh->wq_enable = ctx->param.wq_enable;
    sqh->seq_wq_mode = ctx->param.seq_wq_mode;

    sqh->secondary_transform_enable_flag = 0;
    sqh->chroma_format = ctx->param.chroma_format;
    sqh->encoding_precision = ctx->param.encoding_precision;

    if (sqh->low_delay == 1)
    {
        sqh->output_reorder_delay = 0;
    }
    else
    {
        // for random access mode, GOP 16
        sqh->output_reorder_delay = com_tbl_log2[ctx->param.max_b_frames + 1]; // log2(GopSize)
        assert(com_tbl_log2[ctx->param.max_b_frames + 1] != -1);
    }
 
#if PATCH
    sqh->patch_stable = ctx->patch->stable;
    sqh->patch_ref_colocated = ctx->patch->ref_colocated;
    sqh->cross_patch_loop_filter = ctx->patch->cross_patch_loop_filter;
    if (ctx->patch->stable)
    {
        sqh->patch_uniform = ctx->patch->uniform;
        if (ctx->patch->uniform)
        {
            sqh->patch_width_minus1 = ctx->patch->width - 1;
            sqh->patch_height_minus1 = ctx->patch->height - 1;
        }
    }
#endif

#if EIPM
    sqh->eipm_enable_flag = 0;
#endif
#if MIPF
    sqh->mipf_enable_flag = (u8)0;
#endif
#if UMVE_ENH
    sqh->umve_enh_enable_flag = (u8)ctx->param.umve_enh_enable_flag;
#endif
}
#endif

typedef struct _QP_ADAPT_PARAM
{
    int qp_offset_layer;
    double qp_offset_model_offset;
    double qp_offset_model_scale;
} QP_ADAPT_PARAM;

#if USE_ZXZ_HGOP
QP_ADAPT_PARAM qp_adapt_param_ra[8] = {
    {-4, 0, 0},
    {1, 0, 0},
    {1, -4.0604, 0.15},
    {2, -4.8332, 0.15},
    {3, -4.9668, 0.15},
    {4, -5.9444, 0.15},
    {5, -5.9444, 0.15},
    {6, -5.9444, 0.15}};

QP_ADAPT_PARAM qp_adapt_param_ld[8] = {
    {-1, 0.0000, 0.0000},
    {1, 0.0000, 0.0000},
    {1, -6.5000, 0.15},
    {2, -6.5000, 0.15},
    {3, -6.5000, 0.15},
    {4, -6.5000, 0.15},
    {5, -6.5000, 0.15},
    {6, -6.5000, 0.15},
};
#else

QP_ADAPT_PARAM qp_adapt_param_ra[8] = {
    {-4, 0, 0},
    {1, 0, 0},
    {1, -4.0604, 0.154575},
    {5, -4.8332, 0.17145},
    {7, -4.9668, 0.174975},
    {8, -5.9444, 0.225},
    {9, -5.9444, 0.225},
    {10, -5.9444, 0.225}};

QP_ADAPT_PARAM qp_adapt_param_ld[8] = {
    {-1, 0.0000, 0.0000},
    {1, 0.0000, 0.0000},
    {4, -6.5000, 0.2590},
    {5, -6.5000, 0.2590},
    {6, -6.5000, 0.2590},
    {7, -6.5000, 0.2590},
    {8, -6.5000, 0.2590},
    {9, -6.5000, 0.2590},
};

#endif

#if HLS_RPL // Implementation for selecting and assigning RPL0 & RPL1 candidates in the SPS to SH
static void select_assign_rpl_for_sh(ENC_CTX *ctx, COM_PIC_HEADER *pic_header)
{
    // TBD(@Chernyak) if the current picture is an IDR, simply return without doing the rest of the codes for this function
    int i;
    int gop_num = ctx->param.gop_size;
    if ((ctx->param.max_b_frames == 0) || (ctx->param.gop_size == 1))
    {
        gop_num = 4;
    }

    // Assume it the pic is in the normal GOP first. Normal GOP here means it is not the first (few) GOP in the beginning of the bitstream
    pic_header->rpl_l0_idx = pic_header->rpl_l1_idx = -1;
    pic_header->ref_pic_list_sps_flag[0] = pic_header->ref_pic_list_sps_flag[1] = 0;
    for (i = 0; i < MAX_NUM_REF_PICS; i++)
    {
        pic_header->rpl_l0.ref_pics[i] = 0;
        pic_header->rpl_l1.ref_pics[i] = 0;
        pic_header->rpl_l0.ref_pics_ddoi[i] = 0;
        pic_header->rpl_l1.ref_pics_ddoi[i] = 0;
        pic_header->rpl_l0.ref_pics_doi[i] = 0;
        pic_header->rpl_l1.ref_pics_doi[i] = 0;

#if LIBVC_ON
        pic_header->rpl_l0.library_index_flag[i] = 0;
        pic_header->rpl_l1.library_index_flag[i] = 0;
#endif
    }
    int availableRPLs = (ctx->param.rpls_l0_cfg_num < gop_num) ? ctx->param.rpls_l0_cfg_num : gop_num;
    for (i = 0; i < availableRPLs; i++)
    {
        int pocIdx = (pic_header->poc % gop_num == 0) ? gop_num : pic_header->poc % gop_num;
        if (pocIdx == ctx->param.rpls_l0[i].poc)
        {
            pic_header->rpl_l0_idx = i;
            pic_header->rpl_l1_idx = pic_header->rpl_l0_idx;
            break;
        }
    }

    // For special case when the pic is in the first (few) GOP in the beginning of the bitstream.
    if (ctx->param.i_period <= 0)
    { // For low delay configuration
        if (pic_header->poc <= (ctx->param.rpls_l0_cfg_num - gop_num))
        {
            pic_header->rpl_l0_idx = pic_header->poc + gop_num - 1;
            pic_header->rpl_l1_idx = pic_header->rpl_l0_idx;
        }
    }
    else
    { // For random access configuration
        for (i = ctx->param.gop_size; i < ctx->param.rpls_l0_cfg_num; i++)
        {
            int pocIdx = (pic_header->poc % ctx->param.i_period == 0) ? ctx->param.i_period : pic_header->poc % ctx->param.i_period;
            if (pocIdx == ctx->param.rpls_l0[i].poc)
            {
                pic_header->rpl_l0_idx = i;
                pic_header->rpl_l1_idx = i;
                break;
            }
        }
    }

    // update slice type
    if (ctx->param.i_period == 0 && pic_header->poc != 0)
    { // LD
        ctx->slice_type = ctx->param.rpls_l0[pic_header->rpl_l0_idx].slice_type;
        pic_header->slice_type = ctx->slice_type;
    }
    else if (ctx->param.i_period != 0 && pic_header->poc % ctx->param.i_period != 0)
    { // RA
        ctx->slice_type = ctx->param.rpls_l0[pic_header->rpl_l0_idx].slice_type;
        pic_header->slice_type = ctx->slice_type;
    }

    // Copy RPL0 from the candidate in SPS to this SH
    pic_header->rpl_l0.poc = pic_header->poc;
#if CUDQP_QP_MAP
    if (pic_header->rpl_l0_idx == -1)
    {
        pic_header->rpl_l0.tid = 0;
        pic_header->rpl_l0.ref_pic_num = 0;
        pic_header->rpl_l0.ref_pic_active_num = 0;
#if LIBVC_ON
        pic_header->rpl_l0.reference_to_library_enable_flag = 0;
#endif
    }
    else
    {
#endif
        pic_header->rpl_l0.tid = ctx->param.rpls_l0[pic_header->rpl_l0_idx].tid;
        pic_header->rpl_l0.ref_pic_num = ctx->param.rpls_l0[pic_header->rpl_l0_idx].ref_pic_num;
        pic_header->rpl_l0.ref_pic_active_num = ctx->param.rpls_l0[pic_header->rpl_l0_idx].ref_pic_active_num;
#if LIBVC_ON
        pic_header->rpl_l0.reference_to_library_enable_flag = ctx->param.rpls_l0[pic_header->rpl_l0_idx].reference_to_library_enable_flag;
#endif
#if CUDQP_QP_MAP
    }
#endif

    for (i = 0; i < pic_header->rpl_l0.ref_pic_num; i++)
    {
        pic_header->rpl_l0.ref_pics[i] = ctx->param.rpls_l0[pic_header->rpl_l0_idx].ref_pics[i];
        pic_header->rpl_l0.ref_pics_ddoi[i] = ctx->param.rpls_l0[pic_header->rpl_l0_idx].ref_pics_ddoi[i];
#if LIBVC_ON
        pic_header->rpl_l0.library_index_flag[i] = ctx->param.rpls_l0[pic_header->rpl_l0_idx].library_index_flag[i];
#endif
    }

    // Copy RPL1 from the candidate in SPS to this SH
    pic_header->rpl_l1.poc = pic_header->poc;
#if CUDQP_QP_MAP
    if (pic_header->rpl_l1_idx == -1)
    {
        pic_header->rpl_l1.tid = 0;
        pic_header->rpl_l1.ref_pic_num = 0;
        pic_header->rpl_l1.ref_pic_active_num = 0;
#if LIBVC_ON
        pic_header->rpl_l1.reference_to_library_enable_flag = 0;
#endif
    }
    else
    {
#endif
        pic_header->rpl_l1.tid = ctx->param.rpls_l1[pic_header->rpl_l1_idx].tid;
        pic_header->rpl_l1.ref_pic_num = ctx->param.rpls_l1[pic_header->rpl_l1_idx].ref_pic_num;
        pic_header->rpl_l1.ref_pic_active_num = ctx->param.rpls_l1[pic_header->rpl_l1_idx].ref_pic_active_num;
#if LIBVC_ON
        pic_header->rpl_l1.reference_to_library_enable_flag = ctx->param.rpls_l1[pic_header->rpl_l1_idx].reference_to_library_enable_flag;
#endif
#if CUDQP_QP_MAP
    }
#endif

    for (i = 0; i < pic_header->rpl_l1.ref_pic_num; i++)
    {
        pic_header->rpl_l1.ref_pics[i] = ctx->param.rpls_l1[pic_header->rpl_l1_idx].ref_pics[i];
        pic_header->rpl_l1.ref_pics_ddoi[i] = ctx->param.rpls_l1[pic_header->rpl_l1_idx].ref_pics_ddoi[i];
#if LIBVC_ON
        pic_header->rpl_l1.library_index_flag[i] = ctx->param.rpls_l1[pic_header->rpl_l1_idx].library_index_flag[i];
#endif
    }

    /* update by Yuqun Fan, no profile level here
    if (sh->rpl_l0_idx != -1 && PROFILE_IS_MAIN(PROFILE))
    {
    sh->ref_pic_list_sps_flag[0] = 1;
    }

    if (sh->rpl_l1_idx != -1 && PROFILE_IS_MAIN(PROFILE))
    {
    sh->ref_pic_list_sps_flag[1] = 1;
    }
    */
    if (pic_header->rpl_l0_idx != -1)
    {
        pic_header->ref_pic_list_sps_flag[0] = 1;
    }

    if (pic_header->rpl_l1_idx != -1)
    {
        pic_header->ref_pic_list_sps_flag[1] = 1;
    }
}

// Return value 0 means all ref pic listed in the given rpl are available in the DPB
// Return value 1 means there is at least one ref pic listed in the given rpl not available in the DPB
static int check_refpic_available(int currentPOC, COM_PM *pm, COM_RPL *rpl)
{
    int i;
    for (i = 0; i < rpl->ref_pic_num; i++)
    {
        int isExistInDPB = 0;
        for (int j = 0; !isExistInDPB && j < MAX_PB_SIZE; j++)
        {
#if LIBVC_ON
            if (pm->pic[j] && pm->pic[j]->is_ref && pm->pic[j]->ptr == (currentPOC - rpl->ref_pics[i]) && !rpl->library_index_flag[i])
#else
            if (pm->pic[j] && pm->pic[j]->is_ref && pm->pic[j]->ptr == (currentPOC - rpl->ref_pics[i]))
#endif
                isExistInDPB = 1;
        }
        if (!isExistInDPB)
        { // Found one ref pic missing return 1
            return 1;
        }
    }
    return 0;
}

// Return value 0 means no explicit RPL is created. The given input parameters rpl0 and rpl1 are not modified
// Return value 1 means the given input parameters rpl0 and rpl1 are modified
static int create_explicit_rpl(COM_PM *pm, COM_PIC_HEADER *sh)

{
    int currentPOC = sh->poc;
    COM_RPL *rpl0 = &sh->rpl_l0;
    COM_RPL *rpl1 = &sh->rpl_l1;
    if (!check_refpic_available(currentPOC, pm, rpl0) && !check_refpic_available(currentPOC, pm, rpl1))
    {
        return 0;
    }

    COM_PIC *pic = NULL;

    int isRPLChanged = 0;
    int ii;
    // Remove ref pic in RPL0 that is not available in the DPB
    for (ii = 0; ii < rpl0->ref_pic_num; ii++)
    {
        int isAvailable = 0;
        for (int jj = 0; !isAvailable && jj < pm->cur_num_ref_pics; jj++)
        {
            pic = pm->pic[jj];
#if LIBVC_ON
            if (pic && pic->is_ref && pic->ptr == (currentPOC - rpl0->ref_pics[ii]) && !rpl0->library_index_flag[ii])
#else
            if (pic && pic->is_ref && pic->ptr == (currentPOC - rpl0->ref_pics[ii]))
#endif
                isAvailable = 1;
            pic = NULL;
        }
        if (!isAvailable)
        {
            for (int jj = ii; jj < rpl0->ref_pic_num - 1; jj++)
            {
                rpl0->ref_pics[jj] = rpl0->ref_pics[jj + 1];
                rpl0->ref_pics_ddoi[jj] = rpl0->ref_pics_ddoi[jj + 1];
#if LIBVC_ON
                rpl0->library_index_flag[jj] = rpl0->library_index_flag[jj + 1];
#endif
            }
            ii--;
            rpl0->ref_pic_num--;
            isRPLChanged = 1;
        }
    }
    if (isRPLChanged)
    {
        sh->rpl_l0_idx = -1;
    }

    // Remove ref pic in RPL1 that is not available in the DPB
    isRPLChanged = 0;
    for (ii = 0; ii < rpl1->ref_pic_num; ii++)
    {
        int isAvailable = 0;
        for (int jj = 0; !isAvailable && jj < pm->cur_num_ref_pics; jj++)
        {
            pic = pm->pic[jj];
#if LIBVC_ON
            if (pic && pic->is_ref && pic->ptr == (currentPOC - rpl1->ref_pics[ii]) && !rpl1->library_index_flag[ii])
#else
            if (pic && pic->is_ref && pic->ptr == (currentPOC - rpl1->ref_pics[ii]))
#endif
                isAvailable = 1;

            pic = NULL;
        }
        if (!isAvailable)
        {
            for (int jj = ii; jj < rpl1->ref_pic_num - 1; jj++)
            {
                rpl1->ref_pics[jj] = rpl1->ref_pics[jj + 1];
                rpl1->ref_pics_ddoi[jj] = rpl1->ref_pics_ddoi[jj + 1];
#if LIBVC_ON
                rpl1->library_index_flag[jj] = rpl1->library_index_flag[jj + 1];
#endif
            }
            ii--;
            rpl1->ref_pic_num--;
            isRPLChanged = 1;
        }
    }
    if (isRPLChanged)
    {
        sh->rpl_l1_idx = -1;
    }

    /*if number of ref pic in RPL0 is less than its number of active ref pic, try to copy from RPL1*/
    if (rpl0->ref_pic_num < rpl0->ref_pic_active_num)
    {
        for (ii = rpl0->ref_pic_num; ii < rpl0->ref_pic_active_num; ii++)
        {
            // First we need to find ref pic in RPL1 that is not already in RPL0
            int isAlreadyIncluded = 1;
            int idx = -1;
            int status = 0;
            do
            {
                status = 0;
                idx++;
                for (int mm = 0; mm < rpl0->ref_pic_num && idx < rpl1->ref_pic_num; mm++)
                {
                    if (rpl1->ref_pics[idx] == rpl0->ref_pics[mm])
                    {
                        status = 1;
                    }
                }
                if (!status)
                {
                    isAlreadyIncluded = 0;
                }
            } while (isAlreadyIncluded && idx < rpl1->ref_pic_num);

            if (idx < rpl1->ref_pic_num)
            {
                rpl0->ref_pics[ii] = rpl1->ref_pics[idx];
                rpl0->ref_pics_ddoi[ii] = rpl1->ref_pics_ddoi[idx];
#if LIBVC_ON
                rpl0->library_index_flag[ii] = rpl1->library_index_flag[idx];
#endif
                rpl0->ref_pic_num++;
            }
        }
        if (rpl0->ref_pic_num < rpl0->ref_pic_active_num)
        {
            rpl0->ref_pic_active_num = rpl0->ref_pic_num;
        }
    }

    /*same logic as above, just apply to RPL1*/
    if (rpl1->ref_pic_num < rpl1->ref_pic_active_num)
    {
        for (ii = rpl1->ref_pic_num; ii < rpl1->ref_pic_active_num; ii++)
        {
            int isAlreadyIncluded = 1;
            int idx = -1;
            int status = 0;
            do
            {
                status = 0;
                idx++;
                for (int mm = 0; mm < rpl1->ref_pic_num && idx < rpl0->ref_pic_num; mm++)
                {
                    if (rpl0->ref_pics[idx] == rpl1->ref_pics[mm])
                    {
                        status = 1;
                    }
                }
                if (!status)
                {
                    isAlreadyIncluded = 0;
                }
            } while (isAlreadyIncluded && idx < rpl0->ref_pic_num);

            if (idx < rpl0->ref_pic_num)
            {
                rpl1->ref_pics[ii] = rpl0->ref_pics[idx];
                rpl1->ref_pics_ddoi[ii] = rpl0->ref_pics_ddoi[idx];
#if LIBVC_ON
                rpl1->library_index_flag[ii] = rpl0->library_index_flag[idx];
#endif
                rpl1->ref_pic_num++;
            }
        }
        if (rpl1->ref_pic_num < rpl1->ref_pic_active_num)
        {
            rpl1->ref_pic_active_num = rpl1->ref_pic_num;
        }
    }
    return 1;
}
#endif
static void set_sh(ENC_CTX *ctx, COM_SH_EXT *shext, s8 *sao_enable_patch)
{
    shext->fixed_slice_qp_flag = (u8)ctx->info.pic_header.fixed_picture_qp_flag;
    shext->slice_qp = (u8)ctx->info.pic_header.picture_qp;
    shext->slice_sao_enable[Y_C] = sao_enable_patch[Y_C];
    shext->slice_sao_enable[U_C] = sao_enable_patch[U_C];
    shext->slice_sao_enable[V_C] = sao_enable_patch[V_C];
}

static void set_pic_header(ENC_CTX *ctx, COM_PIC_HEADER *pic_header)
{
    double qp;

#if LIBVC_ON
    QP_ADAPT_PARAM *qp_adapt_param;
    if (ctx->rpm.libvc_data->is_libpic_processing)
    {
        qp_adapt_param = qp_adapt_param_ra;
    }
    else
    {
        qp_adapt_param = ctx->param.max_b_frames == 0 ? qp_adapt_param_ld : qp_adapt_param_ra;
    }
#else
    QP_ADAPT_PARAM *qp_adapt_param = ctx->param.max_b_frames == 0 ? qp_adapt_param_ld : qp_adapt_param_ra;
#endif
    int prev_slice_type = pic_header->slice_type;
    pic_header->bbv_delay = 0xFFFFFFFF;
    pic_header->time_code_flag = 0;
    pic_header->time_code = 0;
    pic_header->low_delay = ctx->info.sqh.low_delay;
    if (pic_header->low_delay)
    {
        pic_header->bbv_check_times = (1 << 16) - 2;
    }
    else
    {
        pic_header->bbv_check_times = 0;
    }

    pic_header->dtr = ctx->dtr;
    pic_header->slice_type = ctx->slice_type;
#if TRACE_REC // turn off Deblock
    pic_header->loop_filter_disable_flag = 1;
#else
    pic_header->loop_filter_disable_flag = (ctx->param.use_deblock) ? 0 : 1;
#endif
    pic_header->temporal_id = ctx->temporal_id;

    pic_header->pic_wq_enable = 0;
    init_pic_wq_matrix(pic_header->wq_4x4_matrix, pic_header->wq_8x8_matrix);

#if HLS_RPL
    pic_header->poc = ctx->ptr;
    select_assign_rpl_for_sh(ctx, pic_header);
    pic_header->num_ref_idx_active_override_flag = 1;
    if (pic_header->slice_type == SLICE_I)
    {
        pic_header->rpl_l0.ref_pic_active_num = 0;
        pic_header->rpl_l1.ref_pic_active_num = 0;
    }
#endif

    pic_header->progressive_frame = 1;
    pic_header->picture_structure = 1;
    pic_header->top_field_first = 0;
    pic_header->repeat_first_field = 0;
    pic_header->top_field_picture_flag = 0;

    pic_header->fixed_picture_qp_flag = !ctx->param.delta_qp_flag;
#if CUDQP_QP_MAP
    pic_header->cu_delta_qp_flag = !pic_header->fixed_picture_qp_flag && ctx->param.cu_delta_qp_flag;
    pic_header->cu_qp_group_size = ctx->param.cu_qp_group_size;
    pic_header->cu_qp_group_area_size = pic_header->cu_qp_group_size * pic_header->cu_qp_group_size;
#endif
    pic_header->random_access_decodable_flag =
        1; // the value shall be 0 for leading pictures according to text (however, not used in decoder)

    // Here the input QP is converted to be the encoding QP.
#if ENABLE_BFRAME
    COM_PIC *orgpic = ctx->pic[PIC_IDX_ORG];
#else
    COM_IMGB *refimgb = ctx->slice_type == SLICE_I ? NULL : ctx->refimgb;
    COM_PIC *orgpic = ctx->pic[PIC_IDX_ORG];
    int num_refp[2];
    num_refp[0] = (refimgb == NULL ? 0 : 1);
    num_refp[1] = 0;
#endif
#if LIBVC_ON
    pic_header->is_RLpic_flag = ctx->is_RLpic_flag;
    if (ctx->info.sqh.library_stream_flag)
    {
        pic_header->library_picture_index = ctx->ptr;
    }
    else
    {
        pic_header->library_picture_index = -1;
    }
#endif
#ifdef RC_OPEN
    if (ctx->rc.b_vbv)
        update_vbv_plan(&ctx->rc, 0);
    double icost, icost_uv[2] = {0};
    double pcost = loka_estimate_coding_cost(&ctx->pi, orgpic, &refimgb, NULL, num_refp, 8, &icost, icost_uv, NULL);

    ctx->pic[PIC_IDX_ORG]->picture_satd = pic_header->slice_type != SLICE_I ? pcost : icost;

    int qp_l0 = ctx->refimgb ? ((int)(pic_header->picture_qp + 0.5) + (prev_slice_type == SLICE_I ? 3 : 0)) : -1;
    int qp_l1 = -1;
    if (ctx->refimgb != NULL)
        imgb_release(ctx->refimgb);
    ctx->refimgb = ctx->pic[PIC_IDX_ORG]->imgb;
    imgb_addref(ctx->refimgb);
    orgpic->lay_id = ctx->slice_type == SLICE_I ? 0 : 1;
    ctx->rc.last_satd = pic_header->slice_type != SLICE_I ? pcost : icost;
    qp = rc_get_qp(&ctx->rc, orgpic, qp_l0, qp_l1);
    ctx->rc.qp = qp;
    pic_header->picture_qp = qp;
    orgpic->picture_qp = pic_header->picture_qp;
    orgpic->picture_qp_real = orgpic->picture_qp;
#elif defined LOWDELAY_RC_OPEN
    int convert[] = {0, I_SLICE, P_SLICE, B_SLICE, 0, 0};
    struct sw_picture *pic = &ctx->lowdelay_rc.pic;
    struct sw_picture *prevRefPic = &ctx->lowdelay_rc.prevRefPic;
    if (ctx->slice_type == SLICE_I)
    {
        pic->type = convert[ctx->slice_type];
        pic->poc = pic_header->poc;
        pic->x265Type = getFramePredId(convert[ctx->slice_type]);
    }
    else
    {
        prevRefPic->type = pic->type;
        prevRefPic->poc = pic->poc;
        prevRefPic->x265Type = pic->x265Type;
        pic->rpl[0][0] = prevRefPic;
        pic->poc = pic_header->poc;
        pic->x265Type = getFramePredId(convert[ctx->slice_type]);
    }
    ctx->lowdelay_rc.sliceTypePrev = convert[prev_slice_type];
    ctx->lowdelay_rc.sliceTypeCur = convert[ctx->slice_type];
    VCEncBeforePicRc(&ctx->lowdelay_rc, ctx->lowdelay_rc.outRateDenom, convert[ctx->slice_type], 0, pic);
    qp = pic_header->picture_qp = ctx->lowdelay_rc.qpHdr >> QP_FRACTIONAL_BITS;
    orgpic->picture_qp = pic_header->picture_qp;
    orgpic->picture_qp_real = orgpic->picture_qp;

#else
    qp = ctx->param.qp + ctx->info.qp_offset_bit_depth;
#if M5422
    qp += pic_header->is_RLpic_flag == 1 ? -3 : 0;
#endif
    if (ctx->param.pb_frame_qp_offset)
    {
        // to simulate IPPP structure with P frame QP = I frame QP + pb_frame_qp_offset
        assert(ctx->param.frame_qp_add == 0);
        assert(ctx->param.disable_hgop == 1);
        qp += pic_header->slice_type != SLICE_I ? ctx->param.pb_frame_qp_offset : 0;
    }
#endif

    // add one qp after certain frame (use to match target bitrate for fix qp case, e.g., Cfp bitrate matching)
    qp = COM_CLIP3(MIN_QUANT, (MAX_QUANT_BASE + ctx->info.qp_offset_bit_depth), (ctx->param.frame_qp_add != 0 && (int)(ctx->ptr) >= ctx->param.frame_qp_add) ? qp + 1.0 : qp);

    if (!ctx->param.disable_hgop)
    {
        double dqp_offset;
        int qp_offset;
        // printf("[%d+%d=%d,", (int)qp, (int)qp_adapt_param[ctx->temporal_id].qp_offset_layer, (int)(qp+ qp_adapt_param[ctx->temporal_id].qp_offset_layer));
        qp += qp_adapt_param[ctx->temporal_id].qp_offset_layer;

        dqp_offset = COM_MAX(0, qp - ctx->info.qp_offset_bit_depth) * qp_adapt_param[ctx->temporal_id].qp_offset_model_scale +
                     qp_adapt_param[ctx->temporal_id].qp_offset_model_offset + 0.5;

        double a = qp_adapt_param[ctx->temporal_id].qp_offset_model_scale;
        double b = qp_adapt_param[ctx->temporal_id].qp_offset_model_offset;
        // printf("MAX( 0 , %d - %d ) * ", (int)qp, ctx->info.qp_offset_bit_depth);
        // printf("%f + %f + 0.5", a, b);
        // printf("= %f,",dqp_offset);
        qp_offset = (int)floor(COM_CLIP3(0.0, 4.0, dqp_offset));
        // printf("%d,", (int)qp_offset);
        qp += qp_offset;
        // printf("%d]\n", (int)qp);
    }

    pic_header->picture_qp = (u8)COM_CLIP3(MIN_QUANT, (MAX_QUANT_BASE + ctx->info.qp_offset_bit_depth), qp);
    orgpic->picture_qp = pic_header->picture_qp;
    orgpic->picture_qp_real = orgpic->picture_qp;
    // ctx->qp_l0         = orgpic->picture_qp;
    if (ctx->param.qp_offset_adp)
    {
        // find a qp_offset_cb that makes com_tbl_qp_chroma_adjust[qp + qp_offset_cb] equal to com_tbl_qp_chroma_adjust_enc[qp + 1]
        int adaptive_qp_offset_encoder;
        int target_chroma_qp = com_tbl_qp_chroma_adjust_enc[COM_CLIP(pic_header->picture_qp - ctx->info.qp_offset_bit_depth + 1,
                                                                     0, 63)];
        for (adaptive_qp_offset_encoder = -5; adaptive_qp_offset_encoder < 10; adaptive_qp_offset_encoder++)
        {
            if (target_chroma_qp == com_tbl_qp_chroma_adjust[COM_CLIP(pic_header->picture_qp - ctx->info.qp_offset_bit_depth +
                                                                          adaptive_qp_offset_encoder,
                                                                      0, 63)])
            {
                break;
            }
        }
        if (pic_header->picture_qp - ctx->info.qp_offset_bit_depth + adaptive_qp_offset_encoder > 63)
        {
            adaptive_qp_offset_encoder = 63 - (pic_header->picture_qp - ctx->info.qp_offset_bit_depth);
        }
        pic_header->chroma_quant_param_delta_cb = ctx->param.qp_offset_cb + adaptive_qp_offset_encoder;
        pic_header->chroma_quant_param_delta_cr = ctx->param.qp_offset_cr + adaptive_qp_offset_encoder;
    }
    else
    {
        pic_header->chroma_quant_param_delta_cb = ctx->param.qp_offset_cb;
        pic_header->chroma_quant_param_delta_cr = ctx->param.qp_offset_cr;
    }

    pic_header->loop_filter_parameter_flag = 0;
    pic_header->alpha_c_offset = 0;
    pic_header->beta_offset = 0;
    pic_header->tool_alf_on = ctx->info.sqh.adaptive_leveling_filter_enable_flag;

    pic_header->affine_subblock_size_idx = 0; // 0->4X4, 1->8X8

    if (pic_header->chroma_quant_param_delta_cb == 0 && pic_header->chroma_quant_param_delta_cr == 0)
    {
        pic_header->chroma_quant_param_disable_flag = 1;
    }
    else
    {
        pic_header->chroma_quant_param_disable_flag = 0;
    }

    pic_header->picture_output_delay = ctx->ptr - ctx->info.pic_header.decode_order_index +
                                       ctx->info.sqh.output_reorder_delay;
}
static int get_part_num1(PART_SIZE size)
{
    switch (size)
    {
    case SIZE_2Nx2N:
        return 1;
    case SIZE_NxN:
        return 4;
    case SIZE_2NxnU:
        return 2;
    case SIZE_2NxnD:
        return 2;
    case SIZE_2NxhN:
        return 4;
    case SIZE_nLx2N:
        return 2;
    case SIZE_nRx2N:
        return 2;
    case SIZE_hNx2N:
        return 4;
    default:
        /*0 ? ((void)0) : __assert_HLS("0", "../separate/com_util.c", (unsigned int)3446, __FUNCTION__);*/
        return -1;
    }
}

void enc_init_bits_est()
{
    int i = 0;
    double p;
    // cout << "entropy_bits:";
    for (i = 0; i < ENTROPY_BITS_TABLE_SIZE; i++)
    {
        p = (MAX_PROB * (i + 0.5)) / ENTROPY_BITS_TABLE_SIZE;
        entropy_bits[i] = (s32)(-32000 * (log(p) / log(2.0) - PROB_BITS));
        // cout << entropy_bits[i] << ",";
    }
    // cout << endl;
}

int enc_ready(ENC_CTX *ctx, CTU_Info *ctu_info)
{
    ENC_CORE *core = NULL;
    int ret, i;
    int pic_width = ctx->info.pic_width = ctx->param.pic_width;
    int pic_height = ctx->info.pic_height = ctx->param.pic_height;
    s64 size;
    com_assert(ctx);
    core = core_alloc();
    com_mset_x64a(ctu_info, 0, sizeof(CTU_Info));
    com_assert_gv((ctu_info) != NULL, ret, COM_ERR_OUT_OF_MEMORY, ERR);
    com_assert_gv(core != NULL, ret, COM_ERR_OUT_OF_MEMORY, ERR);
    /* set various value */
    ctx->core = core;
#if USE_INTRA_REFRESH
    ctx->info.EncRefresh = ctx->param.EncRefresh;
    ctx->info.RefreshSize = ctx->param.RefreshSize;
    ctx->info.RefreshMode = ctx->param.RefreshMode;
#endif
#if USE_SPEED_LEVEL
    ctx->info.speed_level = ctx->param.speed_level;
#endif
#if USE_ROI_QP_CONFIG
    ctx->info.use_ROI_qp1 = ctx->param.use_ROI_qp1;
    ctx->info.ROI_qp_Start_x1 = ctx->param.ROI_qp_Start_x1;
    ctx->info.ROI_qp_End_x1 = ctx->param.ROI_qp_End_x1;
    ctx->info.ROI_qp_Start_y1 = ctx->param.ROI_qp_Start_y1;
    ctx->info.ROI_qp_End_y1 = ctx->param.ROI_qp_End_y1;
    ctx->info.ROI_QP1 = ctx->param.ROI_QP1;

    ctx->info.use_ROI_qp2 = ctx->param.use_ROI_qp2;
    ctx->info.ROI_qp_Start_x2 = ctx->param.ROI_qp_Start_x2;
    ctx->info.ROI_qp_End_x2 = ctx->param.ROI_qp_End_x2;
    ctx->info.ROI_qp_Start_y2 = ctx->param.ROI_qp_Start_y2;
    ctx->info.ROI_qp_End_y2 = ctx->param.ROI_qp_End_y2;
    ctx->info.ROI_QP2 = ctx->param.ROI_QP2;

    ctx->info.use_ROI_qp3 = ctx->param.use_ROI_qp3;
    ctx->info.ROI_qp_Start_x3 = ctx->param.ROI_qp_Start_x3;
    ctx->info.ROI_qp_End_x3 = ctx->param.ROI_qp_End_x3;
    ctx->info.ROI_qp_Start_y3 = ctx->param.ROI_qp_Start_y3;
    ctx->info.ROI_qp_End_y3 = ctx->param.ROI_qp_End_y3;
    ctx->info.ROI_QP3 = ctx->param.ROI_QP3;

    ctx->info.use_ROI_qp4 = ctx->param.use_ROI_qp4;
    ctx->info.ROI_qp_Start_x4 = ctx->param.ROI_qp_Start_x4;
    ctx->info.ROI_qp_End_x4 = ctx->param.ROI_qp_End_x4;
    ctx->info.ROI_qp_Start_y4 = ctx->param.ROI_qp_Start_y4;
    ctx->info.ROI_qp_End_y4 = ctx->param.ROI_qp_End_y4;
    ctx->info.ROI_QP4 = ctx->param.ROI_QP4;

    ctx->info.use_ROI_qp5 = ctx->param.use_ROI_qp5;
    ctx->info.ROI_qp_Start_x5 = ctx->param.ROI_qp_Start_x5;
    ctx->info.ROI_qp_End_x5 = ctx->param.ROI_qp_End_x5;
    ctx->info.ROI_qp_Start_y5 = ctx->param.ROI_qp_Start_y5;
    ctx->info.ROI_qp_End_y5 = ctx->param.ROI_qp_End_y5;
    ctx->info.ROI_QP5 = ctx->param.ROI_QP5;

    ctx->info.use_ROI_qp6 = ctx->param.use_ROI_qp6;
    ctx->info.ROI_qp_Start_x6 = ctx->param.ROI_qp_Start_x6;
    ctx->info.ROI_qp_End_x6 = ctx->param.ROI_qp_End_x6;
    ctx->info.ROI_qp_Start_y6 = ctx->param.ROI_qp_Start_y6;
    ctx->info.ROI_qp_End_y6 = ctx->param.ROI_qp_End_y6;
    ctx->info.ROI_QP6 = ctx->param.ROI_QP6;

    ctx->info.use_ROI_qp7 = ctx->param.use_ROI_qp7;
    ctx->info.ROI_qp_Start_x7 = ctx->param.ROI_qp_Start_x7;
    ctx->info.ROI_qp_End_x7 = ctx->param.ROI_qp_End_x7;
    ctx->info.ROI_qp_Start_y7 = ctx->param.ROI_qp_Start_y7;
    ctx->info.ROI_qp_End_y7 = ctx->param.ROI_qp_End_y7;
    ctx->info.ROI_QP7 = ctx->param.ROI_QP7;

    ctx->info.use_ROI_qp8 = ctx->param.use_ROI_qp8;
    ctx->info.ROI_qp_Start_x8 = ctx->param.ROI_qp_Start_x8;
    ctx->info.ROI_qp_End_x8 = ctx->param.ROI_qp_End_x8;
    ctx->info.ROI_qp_Start_y8 = ctx->param.ROI_qp_Start_y8;
    ctx->info.ROI_qp_End_y8 = ctx->param.ROI_qp_End_y8;
    ctx->info.ROI_QP8 = ctx->param.ROI_QP8;

    ctx->info.useQPMAP = ctx->param.useQPMAP;
    ctx->info.useQPMAPCon = ctx->param.useQPMAPCon;
#endif
#if CUDQP_QP_MAP
    ctx->info.useCuQPMAP = ctx->param.useCuQPMAP;
#endif
#if USE_ROI_MODE_CONFIG
    ctx->info.use_ROI_Mode1 = ctx->param.use_ROI_Mode1;
    ctx->info.ROI_Mode_Start_x1 = ctx->param.ROI_Mode_Start_x1;
    ctx->info.ROI_Mode_End_x1 = ctx->param.ROI_Mode_End_x1;
    ctx->info.ROI_Mode_Start_y1 = ctx->param.ROI_Mode_Start_y1;
    ctx->info.ROI_Mode_End_y1 = ctx->param.ROI_Mode_End_y1;
    ctx->info.ROI_Mode1 = ctx->param.ROI_Mode1;

    ctx->info.use_ROI_Mode2 = ctx->param.use_ROI_Mode2;
    ctx->info.ROI_Mode_Start_x2 = ctx->param.ROI_Mode_Start_x2;
    ctx->info.ROI_Mode_End_x2 = ctx->param.ROI_Mode_End_x2;
    ctx->info.ROI_Mode_Start_y2 = ctx->param.ROI_Mode_Start_y2;
    ctx->info.ROI_Mode_End_y2 = ctx->param.ROI_Mode_End_y2;
    ctx->info.ROI_Mode2 = ctx->param.ROI_Mode2;

    ctx->info.use_ROI_Mode3 = ctx->param.use_ROI_Mode3;
    ctx->info.ROI_Mode_Start_x3 = ctx->param.ROI_Mode_Start_x3;
    ctx->info.ROI_Mode_End_x3 = ctx->param.ROI_Mode_End_x3;
    ctx->info.ROI_Mode_Start_y3 = ctx->param.ROI_Mode_Start_y3;
    ctx->info.ROI_Mode_End_y3 = ctx->param.ROI_Mode_End_y3;
    ctx->info.ROI_Mode3 = ctx->param.ROI_Mode3;

    ctx->param.use_ROI_Mode4 = ctx->param.use_ROI_Mode4;
    ctx->param.ROI_Mode_Start_x4 = ctx->param.ROI_Mode_Start_x4;
    ctx->param.ROI_Mode_End_x4 = ctx->param.ROI_Mode_End_x4;
    ctx->param.ROI_Mode_Start_y4 = ctx->param.ROI_Mode_Start_y4;
    ctx->param.ROI_Mode_End_y4 = ctx->param.ROI_Mode_End_y4;
    ctx->param.ROI_Mode4 = ctx->param.ROI_Mode4;

    ctx->info.use_ROI_Mode5 = ctx->param.use_ROI_Mode5;
    ctx->info.ROI_Mode_Start_x5 = ctx->param.ROI_Mode_Start_x5;
    ctx->info.ROI_Mode_End_x5 = ctx->param.ROI_Mode_End_x5;
    ctx->info.ROI_Mode_Start_y5 = ctx->param.ROI_Mode_Start_y5;
    ctx->info.ROI_Mode_End_y5 = ctx->param.ROI_Mode_End_y5;
    ctx->info.ROI_Mode5 = ctx->param.ROI_Mode5;

    ctx->info.use_ROI_Mode6 = ctx->param.use_ROI_Mode6;
    ctx->info.ROI_Mode_Start_x6 = ctx->param.ROI_Mode_Start_x6;
    ctx->info.ROI_Mode_End_x6 = ctx->param.ROI_Mode_End_x6;
    ctx->info.ROI_Mode_Start_y6 = ctx->param.ROI_Mode_Start_y6;
    ctx->info.ROI_Mode_End_y6 = ctx->param.ROI_Mode_End_y6;
    ctx->info.ROI_Mode6 = ctx->param.ROI_Mode6;

    ctx->info.use_ROI_Mode7 = ctx->param.use_ROI_Mode7;
    ctx->info.ROI_Mode_Start_x7 = ctx->param.ROI_Mode_Start_x7;
    ctx->info.ROI_Mode_End_x7 = ctx->param.ROI_Mode_End_x7;
    ctx->info.ROI_Mode_Start_y7 = ctx->param.ROI_Mode_Start_y7;
    ctx->info.ROI_Mode_End_y7 = ctx->param.ROI_Mode_End_y7;
    ctx->info.ROI_Mode7 = ctx->param.ROI_Mode7;

    ctx->info.use_ROI_Mode8 = ctx->param.use_ROI_Mode8;
    ctx->info.ROI_Mode_Start_x8 = ctx->param.ROI_Mode_Start_x8;
    ctx->info.ROI_Mode_End_x8 = ctx->param.ROI_Mode_End_x8;
    ctx->info.ROI_Mode_Start_y8 = ctx->param.ROI_Mode_Start_y8;
    ctx->info.ROI_Mode_End_y8 = ctx->param.ROI_Mode_End_y8;
    ctx->info.ROI_Mode8 = ctx->param.ROI_Mode8;

    ctx->info.useModeMAP = ctx->param.useModeMAP;
    ctx->info.useModeMAPCon = ctx->param.useModeMAPCon;
#endif

#if USE_TRACE_DUMP
    ctx->info.useTraceDump = ctx->param.useTraceDump;
#endif
    enc_init_bits_est();
    ctx->info.max_cuwh = ctx->param.ctu_size;
    ctx->info.log2_max_cuwh = CONV_LOG2(ctx->info.max_cuwh);
    ctx->max_cud = (u8)ctx->info.log2_max_cuwh - MIN_CU_LOG2;
    ctx->info.pic_width_in_lcu = (pic_width + ctx->info.max_cuwh - 1) >> ctx->info.log2_max_cuwh;
    ctx->info.pic_height_in_lcu = (pic_height + ctx->info.max_cuwh - 1) >> ctx->info.log2_max_cuwh;
    ctx->info.f_lcu = ctx->info.pic_width_in_lcu * ctx->info.pic_height_in_lcu;
    ctx->info.pic_width_in_scu = (pic_width + ((1 << MIN_CU_LOG2) - 1)) >> MIN_CU_LOG2;
    ctx->info.pic_height_in_scu = (pic_height + ((1 << MIN_CU_LOG2) - 1)) >> MIN_CU_LOG2;
    ctx->info.f_scu = ctx->info.pic_width_in_scu * ctx->info.pic_height_in_scu;
    ctx->log2_culine = (u8)ctx->info.log2_max_cuwh - MIN_CU_LOG2;
    ctx->log2_cudim = ctx->log2_culine << 1;

    // if (ctx->ctu_map_cu_data == NULL) {
    size = sizeof(ENC_CU_DATA);
    // ctx->ctu_map_cu_data = (ENC_CU_DATA*)com_malloc_fast(size);
    com_assert_gv(&ctx->ctu_map_cu_data, ret, COM_ERR_OUT_OF_MEMORY, ERR);
    com_mset_x64a(&ctx->ctu_map_cu_data, 0, size);

    enc_create_cu_data(&ctx->ctu_map_cu_data, ctx->info.log2_max_cuwh - MIN_CU_LOG2, ctx->info.log2_max_cuwh - MIN_CU_LOG2);

    //}

    /* allocate maps */
    if (ctx->map.map_scu == NULL)
    {
        size = sizeof(u32) * ctx->info.f_scu;
        ctx->map.map_scu = (u32 *)com_malloc_fast(size);
        com_assert_gv(ctx->map.map_scu, ret, COM_ERR_OUT_OF_MEMORY, ERR);
        com_mset_x64a(ctx->map.map_scu, 0, size);
    }
    if (ctx->map.map_split == NULL)
    {
        size = sizeof(s8) * ctx->info.f_lcu * MAX_CU_DEPTH * NUM_BLOCK_SHAPE * MAX_CU_CNT_IN_LCU;
        ctx->map.map_split = (s8(*)[5][7][256])com_malloc_fast(size);
        com_assert_gv(ctx->map.map_split, ret, COM_ERR_OUT_OF_MEMORY, ERR);
        com_mset_x64a(ctx->map.map_split, 0, size);
    }
    if (ctx->map.map_ipm == NULL)
    {
        size = sizeof(s8) * ctx->info.f_scu;
        ctx->map.map_ipm = (s8 *)com_malloc_fast(size);
        com_assert_gv(ctx->map.map_ipm, ret, COM_ERR_OUT_OF_MEMORY, ERR);
        com_mset(ctx->map.map_ipm, -1, size);
    }
    if (ctx->map.map_patch_idx == NULL)
    {
        size = sizeof(s8) * ctx->info.f_scu;
        ctx->map.map_patch_idx = (s8 *)com_malloc_fast(size);
        com_assert_gv(ctx->map.map_patch_idx, ret, COM_ERR_OUT_OF_MEMORY, ERR);
        com_mset(ctx->map.map_patch_idx, -1, size);
    }
#if TB_SPLIT_EXT
    if (ctx->map.map_pb_tb_part == NULL)
    {
        size = sizeof(u32) * ctx->info.f_scu;
        ctx->map.map_pb_tb_part = (u32 *)com_malloc_fast(size);
        com_assert_gv(ctx->map.map_pb_tb_part, ret, COM_ERR_OUT_OF_MEMORY, ERR);
        com_mset(ctx->map.map_pb_tb_part, 0xffffffff, size);
    }
#endif
    size = sizeof(s8) * ctx->info.f_scu;
    ctx->map.map_depth = (s8 *)com_malloc_fast(size);
    com_assert_gv(ctx->map.map_depth, ret, COM_ERR_OUT_OF_MEMORY, ERR);
    com_mset(ctx->map.map_depth, -1, size);
    if (ctx->map.map_cu_mode == NULL)
    {
        size = sizeof(u32) * ctx->info.f_scu;
        ctx->map.map_cu_mode = (u32 *)com_malloc_fast(size);
        com_assert_gv(ctx->map.map_cu_mode, ret, COM_ERR_OUT_OF_MEMORY, ERR);
        com_mset_x64a(ctx->map.map_cu_mode, 0, size);
    }

    if (ctx->map.map_delta_qp == NULL)
    {
        size = sizeof(s8) * ctx->info.f_lcu;
        ctx->map.map_delta_qp = (s8 *)com_malloc_fast(size);
        com_assert_gv(ctx->map.map_delta_qp, ret, COM_ERR_OUT_OF_MEMORY, ERR);
        com_mset_x64a(ctx->map.map_delta_qp, 0, size);
    }

    ctx->pa.width = ctx->info.pic_width;
    ctx->pa.height = ctx->info.pic_height;
    ctx->pa.pad_l = PIC_PAD_SIZE_L;
    ctx->pa.pad_c = PIC_PAD_SIZE_C;
    ctx->pic_cnt = 0;
    ctx->pic_icnt = -1;
    ctx->dtr = 0;
    ctx->ptr = 0;
    ret = com_picman_init(&ctx->rpm, MAX_PB_SIZE, MAX_NUM_REF_PICS, &ctx->pa);
    com_assert_g(COM_SUCCEEDED(ret), ERR);
    ctx->pico_max_cnt = 1 + (ctx->param.max_b_frames << 1);
    ctx->frm_rnum = ctx->param.max_b_frames;
    ctx->info.bit_depth_internal = ctx->param.bit_depth_internal;
    ctx->info.bit_depth_input = ctx->param.bit_depth_input;
    ctx->info.qp_offset_bit_depth = (8 * (ctx->info.bit_depth_internal - 8));

    for (i = 0; i < ctx->pico_max_cnt; i++)
    {
        ctx->pico_buf[i] = (ENC_PICO *)com_malloc(sizeof(ENC_PICO));
        com_assert_gv(ctx->pico_buf[i], ret, COM_ERR_OUT_OF_MEMORY, ERR);
        com_mset(ctx->pico_buf[i], 0, sizeof(ENC_PICO));
    }
    // allocate memory for ppbEdgeFilter
    for (i = 0; i < LOOPFILTER_DIR_TYPE; i++)
    {
        ctx->ppbEdgeFilter[i] = (int **)calloc((ctx->info.pic_height >> LOOPFILTER_SIZE_IN_BIT), sizeof(int *));
        for (int j = 0; j < (ctx->info.pic_height >> LOOPFILTER_SIZE_IN_BIT); j++)
        {
            ctx->ppbEdgeFilter[i][j] = (int *)calloc((ctx->info.pic_width >> LOOPFILTER_SIZE_IN_BIT), sizeof(int));
        }
    }
    // CreateEdgeFilter_avs2(ctx->info.pic_width, ctx->info.pic_height, ctx->ppbEdgeFilter);

    return COM_OK;
ERR:

    // enc_delete_cu_data(&ctx->ctu_map_cu_data);
    // com_mfree_fast(ctx->ctu_map_cu_data);

    com_mfree_fast(ctx->map.map_ipm);
    com_mfree_fast(ctx->map.map_patch_idx);
    com_mfree_fast(ctx->map.map_depth);
#if TB_SPLIT_EXT
    com_mfree_fast(ctx->map.map_pb_tb_part);
#endif
    com_mfree_fast(ctx->map.map_cu_mode);
    com_mfree_fast(ctx->map.map_delta_qp);
    for (i = 0; i < ctx->pico_max_cnt; i++)
    {
        com_mfree_fast(ctx->pico_buf[i]);
    }

    if (ctx->pic_sao != NULL)
    {
        com_pic_free(&ctx->pa, ctx->pic_sao);
    }
    if (ctx->pic_dbkbuffer != NULL)
    {
        com_pic_free(&ctx->pa, ctx->pic_dbkbuffer);
    }
    if (core)
    {
        core_free(core);
    }
    return ret;
}

void enc_flush(ENC_CTX *ctx)
{
    int i;
    com_assert(ctx);
    com_mfree_fast(ctx->map.map_scu);
    com_mfree_fast(ctx->map.map_split);

    // enc_delete_cu_data(ctx->ctu_map_cu_data);
    // com_mfree_fast(ctx->ctu_map_cu_data);

    com_mfree_fast(ctx->map.map_ipm);
    com_mfree_fast(ctx->map.map_patch_idx);
#if TB_SPLIT_EXT
    com_mfree_fast(ctx->map.map_pb_tb_part);
#endif
    com_mfree_fast(ctx->map.map_depth);
    com_mfree_fast(ctx->map.map_cu_mode);
    com_mfree_fast(ctx->map.map_delta_qp);

    // com_picbuf_free(ctx->pic_alf_Org);
    // com_picbuf_free(ctx->pic_alf_Rec);
    com_picman_deinit(&ctx->rpm);
#if LIBVC_ON
    ctx->rpm.libvc_data = NULL;
#endif
    core_free(ctx->core);
    for (i = 0; i < ctx->pico_max_cnt; i++)
    {
        com_mfree_fast(ctx->pico_buf[i]);
    }
    for (i = 0; i < ENC_MAX_INBUF_CNT; i++)
    {
        if (ctx->inbuf[i])
        {
            imgb_release(ctx->inbuf[i]);
        }
    }

    // DeleteEdgeFilter_avs2(ctx->ppbEdgeFilter, ctx->info.pic_height);

    if (ctx->pic_sao != NULL)
    {
        com_pic_free(&ctx->pa, ctx->pic_sao);
    }
    if (ctx->pic_dbkbuffer != NULL)
    {
        com_pic_free(&ctx->pa, ctx->pic_dbkbuffer);
    }
}
int enc_picbuf_get_inbuf_422(ENC_CTX *ctx, COM_IMGB **imgb)
{
    int i, opt, align[COM_IMGB_MAX_PLANE], pad[COM_IMGB_MAX_PLANE];
    for (i = 0; i < ENC_MAX_INBUF_CNT; i++)
    {
        if (ctx->inbuf[i] == NULL)
        {
            opt = COM_IMGB_OPT_NONE;
            /* set align value*/
            align[0] = MIN_CU_SIZE;
            align[1] = MIN_CU_SIZE >> 1;
            align[2] = MIN_CU_SIZE >> 1;
            /* no padding */
            pad[0] = 0;
            pad[1] = 0;
            pad[2] = 0;
            *imgb = com_imgb_create(ctx->param.pic_width, ctx->param.pic_height, COM_COLORSPACE_YUV422, pad, align);
            com_assert_rv(*imgb != NULL, COM_ERR_OUT_OF_MEMORY);
            ctx->inbuf[i] = *imgb;
            imgb_addref(*imgb);
            return COM_OK;
        }
        else if (imgb_getref(ctx->inbuf[i]) == 1)
        {
            *imgb = ctx->inbuf[i];
            imgb_addref(*imgb);
            return COM_OK;
        }
    }
    return COM_ERR_UNEXPECTED;
}
int enc_picbuf_get_inbuf(ENC_CTX *ctx, COM_IMGB **imgb)
{
    int i, opt, align[COM_IMGB_MAX_PLANE], pad[COM_IMGB_MAX_PLANE];
    for (i = 0; i < ENC_MAX_INBUF_CNT; i++)
    {
        if (ctx->inbuf[i] == NULL)
        {
            opt = COM_IMGB_OPT_NONE;
            /* set align value*/
            align[0] = MIN_CU_SIZE;
            align[1] = MIN_CU_SIZE >> 1;
            align[2] = MIN_CU_SIZE >> 1;
            /* no padding */
            pad[0] = 0;
            pad[1] = 0;
            pad[2] = 0;
            *imgb = com_imgb_create(ctx->param.pic_width, ctx->param.pic_height, COM_COLORSPACE_YUV420, pad, align);
            com_assert_rv(*imgb != NULL, COM_ERR_OUT_OF_MEMORY);
            ctx->inbuf[i] = *imgb;
            imgb_addref(*imgb);
            return COM_OK;
        }
        else if (imgb_getref(ctx->inbuf[i]) == 1)
        {
            *imgb = ctx->inbuf[i];
            imgb_addref(*imgb);
            return COM_OK;
        }
    }
    return COM_ERR_UNEXPECTED;
}

int init_seq_header(ENC_CTX *ctx, COM_BITB *bitb)
{
    COM_BSW *bs;
    COM_SQH *sqh;
    bs = &ctx->bs;
    sqh = &ctx->info.sqh;
    bs->pdata[1] = (int *)&ctx->sbac_enc; // necessary
    bs->pdata[0] = (int *)&ctx->sbac_enc;
    bs->pdata[2] = (int *)&ctx->sbac_enc;
    bs->pdata[3] = (int *)&ctx->sbac_enc;
    set_sqh(ctx, sqh);
    return COM_OK;
}

static void decide_normal_gop(ENC_CTX *ctx, u32 pic_imcnt)
{
    int i_period, gop_size, pos;
    u32 pic_icnt_b;
    i_period = ctx->param.i_period;
    gop_size = ctx->param.gop_size;
    if (i_period == 0 && pic_imcnt == 0)
    {
        ctx->slice_type = SLICE_I;
        ctx->slice_depth = FRM_DEPTH_0;
        ctx->poc = pic_imcnt;
        ctx->slice_ref_flag = 1;
    }
    else if ((i_period != 0) && pic_imcnt % i_period == 0)
    {
        ctx->slice_type = SLICE_I;
        ctx->slice_depth = FRM_DEPTH_0;
#if USE_INTRA_REFRESH
        if ((pic_imcnt % i_period) == 0 && pic_imcnt != 0 && ctx->param.EncRefresh != 0)
        {
            if (gop_size == 1)
                ctx->slice_type = SLICE_P;
            else
                ctx->slice_type = SLICE_B;
        }
#endif

        ctx->poc = pic_imcnt;
        ctx->slice_ref_flag = 1;
    }
    else if (pic_imcnt % gop_size == 0)
    {
        ctx->slice_type = SLICE_B;
        ctx->slice_ref_flag = 1;
        ctx->slice_depth = FRM_DEPTH_1;
        ctx->poc = pic_imcnt;
        ctx->slice_ref_flag = 1;
    }
    else
    {
        ctx->slice_type = SLICE_B;
        if (!ctx->param.disable_hgop)
        {
            pos = (pic_imcnt % gop_size) - 1;
            ctx->slice_depth = tbl_slice_depth[gop_size >> 2][pos];
            ctx->ref_depth = tbl_ref_depth[gop_size >> 2][pos];
            ctx->poc = ((pic_imcnt / gop_size) * gop_size) +
                       tbl_poc_gop_offset[gop_size >> 2][pos];
            ctx->slice_ref_flag = tbl_slice_ref[gop_size >> 2][pos];
        }
        else
        {
            pos = (pic_imcnt % gop_size) - 1;
            ctx->slice_depth = FRM_DEPTH_2;
            ctx->ref_depth = FRM_DEPTH_1;
            ctx->poc = ((pic_imcnt / gop_size) * gop_size) - gop_size + pos + 1;
            ctx->slice_ref_flag = 0;
        }
        /* find current encoding picture's(B picture) pic_icnt */
        pic_icnt_b = ctx->poc;
        /* find pico again here */
        ctx->pico_idx = (u8)(pic_icnt_b % ctx->pico_max_cnt);
        ctx->pico = ctx->pico_buf[ctx->pico_idx];
        PIC_ORG(ctx) = &ctx->pico->pic;
    }
}

/* slice_type / slice_depth / poc / PIC_ORIG setting */
static void decide_slice_type(ENC_CTX *ctx)
{
    int pic_imcnt, pic_icnt;
    int i_period, gop_size;
    int force_cnt = 0;
    i_period = ctx->param.i_period;
    gop_size = ctx->param.gop_size;
    pic_icnt = (ctx->pic_cnt + ctx->param.max_b_frames);
    pic_imcnt = pic_icnt;
    ctx->pico_idx = pic_icnt % ctx->pico_max_cnt;
    ctx->pico = ctx->pico_buf[ctx->pico_idx];
    PIC_ORG(ctx) = &ctx->pico->pic;
    if (gop_size == 1)
    {
        pic_imcnt = (i_period > 0) ? pic_icnt % i_period : pic_icnt;
        if (pic_imcnt == 0)
        { // all intra configuration
            ctx->slice_type = SLICE_I;
#if USE_INTRA_REFRESH && 0
            if ((pic_icnt % i_period) == 0 && pic_icnt != 0 && ctx->param.EncRefresh != 0)
            {
                if (gop_size == 1)
                    ctx->slice_type = SLICE_P;
                else
                    ctx->slice_type = SLICE_B;
            }
#endif
            ctx->slice_depth = FRM_DEPTH_0;
#if LIBVC_ON
            if (ctx->rpm.libvc_data->is_libpic_processing)
            {
                ctx->poc = pic_icnt;
            }
            else
#endif

            {
                ctx->poc = (i_period > 0) ? ctx->pic_cnt % i_period : ctx->pic_cnt;
            }
            ctx->slice_ref_flag = 1;
        }
        else
        {
            ctx->slice_type = SLICE_B;
            if (!ctx->param.disable_hgop)
            {
                ctx->slice_depth = tbl_slice_depth_P[(pic_imcnt - 1) % GOP_P];
            }
            else
            {
                ctx->slice_depth = FRM_DEPTH_1;
            }
            ctx->poc = (i_period > 0) ? ctx->pic_cnt % i_period : ctx->pic_cnt;
            ctx->slice_ref_flag = 1;
        }
    }
    else
    { /* include B Picture (gop_size = 2 or 4 or 8 or 16) */
        if (pic_icnt == gop_size - 1)
        { /* special case when sequence start */
            ctx->slice_type = SLICE_I;
#if USE_INTRA_REFRESH
            if ((pic_icnt % i_period) == 0 && pic_icnt != 0 && ctx->param.EncRefresh != 0)
            {
                if (gop_size == 1)
                    ctx->slice_type = SLICE_P;
                else
                    ctx->slice_type = SLICE_B;
            }
#endif
            ctx->slice_depth = FRM_DEPTH_0;
            ctx->poc = 0;
            ctx->slice_ref_flag = 1;
            /* flush the first IDR picture */
            PIC_ORG(ctx) = &ctx->pico_buf[0]->pic;
            ctx->pico = ctx->pico_buf[0];
        }
        else if (ctx->force_slice)
        {
            for (force_cnt = ctx->force_ignored_cnt; force_cnt < gop_size; force_cnt++)
            {
                pic_icnt = (ctx->pic_cnt + ctx->param.max_b_frames + force_cnt);
                pic_imcnt = pic_icnt;
                decide_normal_gop(ctx, pic_imcnt);
                if (ctx->poc <= (int)ctx->pic_ticnt)
                {
                    break;
                }
            }
            ctx->force_ignored_cnt = force_cnt;
        }
        else
        { /* normal GOP case */
            decide_normal_gop(ctx, pic_imcnt);
        }
    }
    if (!ctx->param.disable_hgop)
    {
        ctx->temporal_id = ctx->slice_depth;
    }
    else
    {
        ctx->temporal_id = 0;
    }
    ctx->ptr = ctx->poc;
    ctx->dtr = ctx->ptr;
    ctx->info.pic_header.decode_order_index = ctx->pic_cnt;
    //printf("zhangxipeng make id: ctx->pic_cnt: %d, calc: %d, %d\n", ctx->pic_cnt, ctx->info.pic_header.decode_order_index, MAX_DIE_CNT);
}

int enc_pic_prepare(ENC_CTX *ctx, COM_BITB *bitb)
{
    ENC_PARAM *param;
    int ret;
    int size;
    com_assert_rv(PIC_ORG(ctx) != NULL, COM_ERR_UNEXPECTED);
    param = &ctx->param;
    PIC_REC(ctx) = com_picman_get_empty_pic(&ctx->rpm, &ret);
    com_assert_rv(PIC_REC(ctx) != NULL, ret);
    ctx->map.map_refi = PIC_REC(ctx)->map_refi;
    ctx->map.map_mv = PIC_REC(ctx)->map_mv;

    if (ctx->pic_sao == NULL)
    {
        ctx->pic_sao = com_pic_alloc(&ctx->rpm.pa, &ret);
        com_assert_rv(ctx->pic_sao != NULL, ret);
    }

    if (ctx->pic_dbkbuffer == NULL)
    {
        ctx->pic_dbkbuffer = com_pic_alloc(&ctx->rpm.pa, &ret);
        com_assert_rv(ctx->pic_dbkbuffer != NULL, ret);
    }
    decide_slice_type(ctx);
    ctx->lcu_cnt = ctx->info.f_lcu;
    ctx->slice_num = 0;
    if (ctx->slice_type == SLICE_I)
    {
        ctx->last_intra_ptr = ctx->ptr;
    }
    size = sizeof(s8) * ctx->info.f_scu * REFP_NUM;
    com_mset_x64a(ctx->map.map_refi, -1, size);
    size = sizeof(s16) * ctx->info.f_scu * REFP_NUM * MV_D;
    com_mset_x64a(ctx->map.map_mv, 0, size);
    /* initialize bitstream container */
    com_bsw_init(&(ctx->bs), (u8 *)bitb->addr, (u8 *)bitb->addr2, bitb->bsize, NULL);
    /* clear map */
    com_mset_x64a(ctx->map.map_scu, 0, sizeof(u32) * ctx->info.f_scu);
    com_mset_x64a(ctx->map.map_cu_mode, 0, sizeof(u32) * ctx->info.f_scu);

#if LIBVC_ON
    // this part determine whether the Ipic references library picture.
    int is_RLpic_flag = 0;
    if (ctx->info.sqh.library_picture_enable_flag && !ctx->rpm.libvc_data->is_libpic_processing && ctx->slice_type == SLICE_I)
    {
        for (int i = 0; i < ctx->rpm.libvc_data->num_RLpic; i++)
        {
            if (ctx->pic_icnt == ctx->rpm.libvc_data->list_poc_of_RLpic[i])
            {
                is_RLpic_flag = 1;
                break;
            }
        }
    }
    ctx->is_RLpic_flag = is_RLpic_flag;
#endif

    return COM_OK;
}
extern int is_first_enc;
extern int udata_size;
extern double seq_header_bit;

int enc_pic_finish(ENC_CTX *ctx, COM_BITB *bitb, ENC_STAT *stat)
{
    COM_IMGB *imgb_o, *imgb_c;
    COM_BSW *bs;
    int ret;
    int i, j;
    int ph_size;
    bs = &ctx->bs;

    // Demulate(bs);
#if USE_ENH
    Demulate_finish(bs);
#else
    Demulate_finish(bs);
#endif
    /*encode patch end*/
    ret = enc_eco_send(bs);
    com_assert_rv(ret == COM_OK, ret);
    /* de-init BSW */
    com_bsw_deinit(bs);
    /* ending */

    /* expand current encoding picture, if needs */
    // enc_picbuf_expand(ctx, PIC_REC(ctx));

    /* picture buffer management */
#if LIBVC_ON
    if (ctx->info.sqh.library_stream_flag)
    {
        ret = com_picman_put_libpic(&ctx->rpm, PIC_REC(ctx), ctx->slice_type,
                                    ctx->ptr, ctx->info.pic_header.decode_order_index, ctx->temporal_id, 1, ctx->refp, &ctx->info.pic_header);
    }
    else
#endif
    {
        ret = com_picman_put_pic(&ctx->rpm, PIC_REC(ctx), ctx->slice_type,
                                 ctx->ptr, ctx->info.pic_header.decode_order_index,
                                 ctx->info.pic_header.picture_output_delay, ctx->temporal_id, 0, ctx->refp);
    }
    com_assert_rv(ret == COM_OK, ret);
    imgb_o = PIC_ORG(ctx)->imgb;
    com_assert(imgb_o != NULL);
    imgb_c = PIC_REC(ctx)->imgb;
    com_assert(imgb_c != NULL);
    /* set stat */
    ph_size = stat->write;
    com_mset(stat, 0, sizeof(ENC_STAT));
    stat->write = ph_size + COM_BSW_GET_WRITE_BYTE(bs);

#if RATECONTROL1
    if (op_enable_ratectrl)
    {
        rc_top.header_bits = ph_size * 8;
        rc_top.data_bits = COM_BSW_GET_WRITE_BYTE(bs) * 8;
        rc_top.rc_proprocess(ctx->info.bit_depth_internal, ctx->slice_type, ctx->poc);
    }
#endif

    stat->ctype = COM_CT_PICTURE;
    stat->stype = ctx->slice_type;
    stat->fnum = ctx->pic_cnt;
    stat->qp = ctx->info.pic_header.picture_qp;
    stat->poc = ctx->param.gop_size == 1 ? ctx->pic_cnt : ctx->ptr; // for AI and IPPP cases, gop_size = 1; for RA, LDB, LDP, gop_size > 1
#if LIBVC_ON
    stat->is_RLpic_flag = ctx->is_RLpic_flag;
#endif

    for (i = 0; i < 2; i++)
    {
        stat->refpic_num[i] = ctx->rpm.num_refp[i];
        for (j = 0; j < stat->refpic_num[i]; j++)
        {
#if LIBVC_ON
            if (ctx->info.sqh.library_picture_enable_flag && !ctx->rpm.libvc_data->is_libpic_processing && ctx->refp[j][i].is_library_picture)
            {
                stat->refpic[i][j] = ctx->refp[j][i].pic->ptr;
            }
            else
#endif
            {
                stat->refpic[i][j] = ctx->refp[j][i].ptr;
            }
        }
    }
    ctx->pic_cnt++;        /* increase picture count */
    ctx->param.f_ifrm = 0; /* clear force-IDR flag */
    ctx->pico->is_used = 0;
    imgb_c->ts[0] = bitb->ts[0] = imgb_o->ts[0];
    imgb_c->ts[1] = bitb->ts[1] = imgb_o->ts[1];
    imgb_c->ts[2] = bitb->ts[2] = imgb_o->ts[2];
    imgb_c->ts[3] = bitb->ts[3] = imgb_o->ts[3];
    if (imgb_o)
    {
        imgb_release(imgb_o);
    }
    return COM_OK;
}

//***************************************************

void patch_malloc(ENC_CTX *ctx, u32 *map_scu_temp, s8 (*map_refi_temp)[REFP_NUM], s16 (*map_mv_temp)[REFP_NUM][MV_D],
                  u32 *map_cu_mode_temp, s8 *patch_sao_enable, int num_of_patch)
{
    com_mset_x64a(map_scu_temp, 0, sizeof(u32) * ctx->info.f_scu);
    com_mset_x64a(map_refi_temp, -1, sizeof(s8) * ctx->info.f_scu * REFP_NUM);
    com_mset_x64a(map_mv_temp, 0, sizeof(s16) * ctx->info.f_scu * REFP_NUM * MV_D);
    com_mset_x64a(map_cu_mode_temp, 0, sizeof(u32) * ctx->info.f_scu);
    for (int i = 0; i < num_of_patch; i++)
    {
        for (int j = 0; j < N_C; j++)
        {
            *(patch_sao_enable + i * N_C + j) = 1;
        }
    }
}
int prepare_RefInfo(ENC_CTX *ctx, COM_PIC_HEADER *pic_header, int ret, COM_SH_EXT *shext, s8 *patch_sao_enable,
                    int patch_cur_index)
{
#if HLS_RPL
    set_pic_header(ctx, pic_header);
    set_sh(ctx, shext, patch_sao_enable + (patch_cur_index + 1) * N_C);
    ctx->wq[0] = pic_header->wq_4x4_matrix;
    ctx->wq[1] = pic_header->wq_8x8_matrix;
    if (pic_header->slice_type != SLICE_I && pic_header->poc != 0)
    { // TODO: perhaps change this condition to say that if this slice is not a slice in IDR picture
        ret = create_explicit_rpl(&ctx->rpm, pic_header);
        if (ret == 1)
        {
            if (pic_header->rpl_l0_idx == -1)
                pic_header->ref_pic_list_sps_flag[0] = 0;
        }
    }
#if LIBVC_ON
    if (ctx->info.sqh.library_picture_enable_flag && !ctx->rpm.libvc_data->is_libpic_processing && ctx->is_RLpic_flag)
    {
        ctx->slice_type = SLICE_P;
        pic_header->slice_type = ctx->slice_type;

        // set rpl
        int libpic_idx;
        // libpic_idx = get_libidx(ctx->rpm.libvc_data, ctx->ptr);
        libpic_idx = get_libidx(ctx->rpm.libvc_data, ctx->pic_icnt);
        com_assert_rv(libpic_idx >= 0, libpic_idx);

        pic_header->rpl_l0_idx = pic_header->rpl_l1_idx = -1;
        pic_header->ref_pic_list_sps_flag[0] = pic_header->ref_pic_list_sps_flag[1] = 0;

        pic_header->num_ref_idx_active_override_flag = 1;
        pic_header->rpl_l0.ref_pic_active_num = 1;
        pic_header->rpl_l1.ref_pic_active_num = 1;

        pic_header->rpl_l0.reference_to_library_enable_flag = 1;
        pic_header->rpl_l1.reference_to_library_enable_flag = 1;

        for (int ii = pic_header->rpl_l0.ref_pic_num; ii > 0; ii--)
        {
            pic_header->rpl_l0.ref_pics[ii] = pic_header->rpl_l0.ref_pics[ii - 1];
            pic_header->rpl_l0.ref_pics_ddoi[ii] = pic_header->rpl_l0.ref_pics_ddoi[ii - 1];
            pic_header->rpl_l0.library_index_flag[ii] = pic_header->rpl_l0.library_index_flag[ii - 1];
        }
        pic_header->rpl_l0.ref_pics[0] = libpic_idx;
        pic_header->rpl_l0.ref_pics_ddoi[0] = libpic_idx;
        pic_header->rpl_l0.library_index_flag[0] = 1;
        pic_header->rpl_l0.ref_pic_num++;
        for (int jj = pic_header->rpl_l1.ref_pic_num; jj > 0; jj--)
        {
            pic_header->rpl_l1.ref_pics[jj] = pic_header->rpl_l1.ref_pics[jj - 1];
            pic_header->rpl_l1.ref_pics_ddoi[jj] = pic_header->rpl_l1.ref_pics_ddoi[jj - 1];
            pic_header->rpl_l1.library_index_flag[jj] = pic_header->rpl_l1.library_index_flag[jj - 1];
        }
        pic_header->rpl_l1.ref_pics[0] = libpic_idx;
        pic_header->rpl_l1.ref_pics_ddoi[0] = libpic_idx;
        pic_header->rpl_l1.library_index_flag[0] = 1;
        pic_header->rpl_l1.ref_pic_num++;
    }
#endif

    if ((pic_header->decode_order_index % DOI_CYCLE_LENGTH) < g_DOIPrev)
    {
        com_picman_dpbpic_doi_minus_cycle_length(&ctx->rpm);
    }
    g_DOIPrev = pic_header->decode_order_index % DOI_CYCLE_LENGTH;
#if LIBVC_ON
    if (!(ctx->info.sqh.library_stream_flag))
#endif
    {
        ret = com_picman_refpic_marking(&ctx->rpm, pic_header);
        com_assert_rv(ret == COM_OK, ret);
    }
    com_constrcut_ref_list_doi(pic_header);

    ret = com_picman_refp_rpl_based_init(&ctx->rpm, pic_header, ctx->refp);
    com_assert_rv(ret == COM_OK, ret);
    return COM_OK;
#endif
}
void sbac_init(ENC_CORE *core, CTU_Info *ctu_info, ENC_CTX *ctx, COM_CNKH *cnkh, COM_BSW *bs)
{
    core->x_lcu = core->y_lcu = 0;
    core->x_pel = core->y_pel = 0;
    ctu_info->x_lcu = ctu_info->y_lcu = 0;
    ctu_info->x_pel = ctu_info->y_pel = 0;
    core->lcu_num = 0;
    ctx->lcu_cnt = ctx->info.f_lcu;
    enc_sbac_init(bs);
    com_sbac_ctx_init(&(GET_SBAC_ENC(bs)->ctx));
    set_cnkh(ctx, cnkh, COM_VER_1, COM_CT_PICTURE);
    com_sbac_ctx_init(&(core->s_curr_best[ctx->info.log2_max_cuwh - 2][ctx->info.log2_max_cuwh - 2].ctx));
    COM_TRACE_SET(1);
#if TRACE_RDO_EXCLUDE_I
}
#endif
}

static int com_is_cu_dqp(COM_INFO *info)
{
    return !info->shext.fixed_slice_qp_flag && info->pic_header.cu_delta_qp_flag;
}

void ctu_ctrl_init(ENC_CTX *ctx, ENC_CORE *core, COM_PIC_HEADER *pic_header, CTU_Info *ctu_info)
{
    int lcu_qp;
    int adj_qp_cb, adj_qp_cr;

	ctu_ctrl_input.slice_enable = 1;
	//ctu_ctrl_input.slice_lcu_num = (ctx->info.pic_height + 63) / 64;
	ctu_ctrl_input.f_lcu = ((ctx->info.pic_height + 63) / 64) * ((ctx->info.pic_width + 63) / 64);

	ctu_ctrl_input.width = ctx->info.pic_width;
	ctu_ctrl_input.height = ctx->info.pic_height;
	ctu_ctrl_input.num_of_hmvp_cand = ctx->info.sqh.num_of_hmvp_cand;
	ctu_ctrl_input.slice_type = ctx->info.pic_header.slice_type;
	ctu_ctrl_input.bit_depth_internal = ctx->info.bit_depth_internal;
	ctu_ctrl_input.ipf_enable_flag = ctx->info.sqh.ipf_enable_flag;
	ctu_ctrl_input.max_part_ratio = ctx->info.sqh.max_part_ratio;
	ctu_ctrl_input.pic_width_in_scu = ctx->info.pic_width_in_scu;
	ctu_ctrl_input.pic_height_in_scu = ctx->info.pic_height_in_scu;
	ctu_ctrl_input.f_scu = ctx->info.pic_width_in_scu * ctx->info.pic_height_in_scu;
	ctu_ctrl_input.log2_max_cuwh = ctx->info.log2_max_cuwh;
#if EIPM
	ctu_ctrl_input.eipm_enable_flag = ctx->info.sqh.eipm_enable_flag;
#endif
#if MIPF
	ctu_ctrl_input.mipf_enable_flag = ctx->info.sqh.mipf_enable_flag;
#endif
#if INTERPF
	ctu_ctrl_input.interpf_enable_flag = ctx->info.sqh.interpf_enable_flag;
#endif
	ctu_ctrl_input.min_cu_size = ctx->info.sqh.min_cu_size;
	ctu_ctrl_input.max_split_times = ctx->info.sqh.max_split_times;
	ctu_ctrl_input.min_qt_size = ctx->info.sqh.min_qt_size;
	ctu_ctrl_input.max_bt_size = ctx->info.sqh.max_bt_size;
	ctu_ctrl_input.max_eqt_size = ctx->info.sqh.max_eqt_size;
	ctu_ctrl_input.affine_subblock_size_idx = ctx->info.pic_header.affine_subblock_size_idx;
	ctu_ctrl_input.ipcm_enable_flag = ctx->info.sqh.ipcm_enable_flag;
	ctu_ctrl_input.tscpm_enable_flag = ctx->info.sqh.tscpm_enable_flag;
#if CUDQP_QP_MAP
	ctu_ctrl_input.cu_dqp_enable = com_is_cu_dqp(&ctx->info);
	ctu_ctrl_input.cu_qp_group_area_size = ctx->info.pic_header.cu_qp_group_area_size;
	ctu_ctrl_input.last_lcu_qp = ctx->info.shext.slice_qp;
	ctu_ctrl_input.slice_qp = ctx->info.shext.slice_qp;
	ctu_ctrl_input.md_slice_qp= ctx->info.shext.slice_qp;
#endif
#if FRAME_LAMBDA_OPT
    int step = 4;
    int depth = 0;
    if (pic_header->poc % 4 == 0)
        depth = 0;
    else
    {
        for (int i = step >> 1; i >= 1; i >>= 1)
        {
            for (int j = 1; j < 4; j += step)
            {
                if (j == pic_header->poc % 4)
                {
                    i = 0;
                    break;
                }
            }
            step >>= 1;
            depth++;
        }
    }
#endif
	//int SHIFT_QP = 18;
#if CTU_RATE_CONTROL
	ctu_ctrl_input.fixed_slice_qp_flag = ctx->info.shext.fixed_slice_qp_flag;
	if (ctx->info.shext.fixed_slice_qp_flag)
	{
		lcu_qp = ctx->info.shext.slice_qp;
	}
#if CUDQP_QP_MAP
	else if (com_is_cu_dqp(&ctx->info))
	{
		//*map_delta_qp++ = 0;
		lcu_qp = ctu_ctrl_input.last_lcu_qp;
		assert(ctu_ctrl_input.last_lcu_qp == ctx->info.shext.slice_qp);
	}
#endif
	else
	{
		lcu_qp = ctx->info.shext.slice_qp;
		*ctx->map.map_delta_qp = (s8)(lcu_qp - ctu_ctrl_input.last_lcu_qp);
		ctu_ctrl_input.last_lcu_qp = lcu_qp;
		ctu_ctrl_input.depth = depth;
	}
#if CUDQP_QP_MAP
    ctx->cu_qp_group.pred_qp = lcu_qp; // when cudqp is on, it will be updated at gp group level
#endif

#else
    if (ctx->info.shext.fixed_slice_qp_flag)

#endif
    double qp_temp = lcu_qp - 18; // SHIFT_QP;
    core->qp_y = lcu_qp;
    ctu_info->qp_y = lcu_qp;
    adj_qp_cb = core->qp_y + pic_header->chroma_quant_param_delta_cb - ctx->info.qp_offset_bit_depth;
    adj_qp_cr = core->qp_y + pic_header->chroma_quant_param_delta_cr - ctx->info.qp_offset_bit_depth;
    adj_qp_cb = COM_CLIP(adj_qp_cb, MIN_QUANT - 16, MAX_QUANT_BASE);
    adj_qp_cr = COM_CLIP(adj_qp_cr, MIN_QUANT - 16, MAX_QUANT_BASE);
    if (adj_qp_cb >= 0)
    {
        adj_qp_cb = com_tbl_qp_chroma_adjust[COM_MIN(MAX_QUANT_BASE, adj_qp_cb)];
    }
    if (adj_qp_cr >= 0)
    {
        adj_qp_cr = com_tbl_qp_chroma_adjust[COM_MIN(MAX_QUANT_BASE, adj_qp_cr)];
    }
    core->qp_u = COM_CLIP(adj_qp_cb + ctx->info.qp_offset_bit_depth, MIN_QUANT,
                          MAX_QUANT_BASE + ctx->info.qp_offset_bit_depth);
    core->qp_v = COM_CLIP(adj_qp_cr + ctx->info.qp_offset_bit_depth, MIN_QUANT,
                          MAX_QUANT_BASE + ctx->info.qp_offset_bit_depth);

    lcu_qp -= ctx->info.qp_offset_bit_depth; // calculate lambda for 8-bit distortion

    double lambda[3];
    double dist_chroma_weight[2];

    dist_chroma_weight[0] = pow(2.0, (lcu_qp - adj_qp_cb) / 4.0);
    dist_chroma_weight[1] = pow(2.0, (lcu_qp - adj_qp_cr) / 4.0);

    lambda[0] = 1.43631 * pow(2.0, (lcu_qp - 16.0) / 4.0);
#if FRAME_LAMBDA_OPT
    if (depth > 0)
        lambda[0] *= Clip3(2.00, 4.00, (double)(qp_temp / 8.0));
#endif
    lambda[1] = lambda[0] / dist_chroma_weight[0];
    lambda[2] = lambda[0] / dist_chroma_weight[1];
    ctx->sqrt_lambda[0] = s64(sqrt(lambda[0]) * (1 << LAMBDA_SCALE));
    ctx->sqrt_lambda[1] = s64(sqrt(lambda[1]) * (1 << LAMBDA_SCALE));
    ctx->sqrt_lambda[2] = s64(sqrt(lambda[2]) * (1 << LAMBDA_SCALE));

    ctx->lambda[0] = s64((1 << LAMBDA_SCALE) * lambda[0]);
    ctx->lambda[1] = s64((1 << LAMBDA_SCALE) * lambda[1]);
    ctx->lambda[2] = s64((1 << LAMBDA_SCALE) * lambda[2]);

    ctx->dist_chroma_weight[0] = (s64)(dist_chroma_weight[0] * (1 << LAMBDA_SCALE));
    ctx->dist_chroma_weight[1] = (s64)(dist_chroma_weight[1] * (1 << LAMBDA_SCALE));

		ctu_ctrl_input.dist_chroma_weight[0][0] = ctx->dist_chroma_weight[0] & 0xffffffff;
		ctu_ctrl_input.dist_chroma_weight[0][1] = (ctx->dist_chroma_weight[0] >> 32) & 0xffffffff;
		ctu_ctrl_input.dist_chroma_weight[1][0] = ctx->dist_chroma_weight[1] & 0xffffffff;
		ctu_ctrl_input.dist_chroma_weight[1][1] = (ctx->dist_chroma_weight[1] >> 32) & 0xffffffff;
	

	//for Frame Level QP parameter Firmware assign
#if !CTU_RATE_CONTROL
	if (ctx->info.shext.fixed_slice_qp_flag) 
#endif
	{
		ctu_ctrl_input.qp_y = ctx->info.shext.slice_qp;
		ctu_ctrl_input.qp_u = core->qp_u;
		ctu_ctrl_input.qp_v = core->qp_v;

		ctu_ctrl_input.lambda[0] = ctx->lambda[0];
		ctu_ctrl_input.lambda[1] = ctx->lambda[1];
		ctu_ctrl_input.lambda[2] = ctx->lambda[2];
		ctu_ctrl_input.sqrt_lambda[0] = ctx->sqrt_lambda[0];
		ctu_ctrl_input.sqrt_lambda[1] = ctx->sqrt_lambda[1];
		ctu_ctrl_input.sqrt_lambda[2] = ctx->sqrt_lambda[2];


		/* initialize structures *****************************************/
		/* initialize pinter */
		ctu_ctrl_input.lambda_mv = (u32)ctx->sqrt_lambda[0];
		ctu_ctrl_input.ptr = ctx->ptr;
		ctu_ctrl_input.gop_size = ctx->param.gop_size;
	}
#if USE_INTRA_REFRESH
	ctu_ctrl_input.EncRefresh = ctx->info.EncRefresh;
	ctu_ctrl_input.RefreshSize = ctx->info.RefreshSize;
	ctu_ctrl_input.RefreshMode = ctx->info.RefreshMode;
	ctu_ctrl_input.pirStartRow = ctx->info.pirStartRow;
	ctu_ctrl_input.pirEndRow = ctx->info.pirEndRow;
	ctu_ctrl_input.pirStartCol = ctx->info.pirStartCol;
	ctu_ctrl_input.pirEndCol = ctx->info.pirEndCol;
#endif
#if USE_SPEED_LEVEL
	ctu_ctrl_input.speed_level = ctx->info.speed_level;
#endif
#if USE_ROI_QP_CONFIG
	ctu_ctrl_input.useQPMAP = ctx->info.useQPMAP;
#endif
#if CUDQP_QP_MAP
	ctu_ctrl_input.useCuQPMAP = ctx->info.useCuQPMAP;


	int lcu_w = (ctx->info.pic_width) >> 2;
	int lcu_h = (ctx->info.pic_height) >> 2;
	if (ctu_ctrl_input.useCuQPMAP)
	{
		//ctu_ctrl_input.CuQPMap_Frame = (unsigned int*)malloc(sizeof(unsigned int) * lcu_w * lcu_h);
		for (int i = 0; i < lcu_h; i++)
			for (int j = 0; j < lcu_w; j++)
			{
				ctu_ctrl_input.CuQPMap_Frame[j * lcu_w + i] = ctx->mapcuqp[j * lcu_w + i];
				//*(ctu_ctrl_input.CuQPMap_Frame + i * lcu_w + j) = *(ctx->mapcuqp + i * lcu_w + j);

			}
	}

#endif
#if USE_ROI_MODE_CONFIG
	ctu_ctrl_input.useModeMAP = ctx->info.useModeMAP;
#endif
#if USE_TRACE_DUMP
	ctu_ctrl_input.useTraceDump = ctx->info.useTraceDump;
#endif
}

int fetch_init(ENC_CTX *ctx, strFetch_input_video *Fetch_input_video_ptr, strFetch_input_video_U8 *Fetch_input_video_ptr_U8, strFetch_input_video_U8_ptr *Fetch_input_video_ptr_U8_ptr)
{
    COM_PIC *pic;
    pic = PIC_ORG(ctx);
    Fetch_input_video_ptr->imgY_org = pic->y;
    Fetch_input_video_ptr->imgU_org = pic->u;
    Fetch_input_video_ptr->imgV_org = pic->v;

#if ENABLE_BFRAME
    ctx->code_stru = -1;
    if ((((ctx)->info).pic_header).slice_type != SLICE_P)
    {
        for (int refidx = 0; refidx < ctx->rpm.num_refp[0]; refidx++)
        {
            if (ctx->pinter.refp[refidx][0].ptr == ctx->pinter.refp[refidx][1].ptr)
                ctx->code_stru = LDB;
            else
                ctx->code_stru = RA;
        }
    }
    else if (ctx->info.pic_header.slice_type == SLICE_P && ctx->rpm.num_refp[0] > 1)
    {
        ctx->code_stru = LDP;
    }
    Fetch_input_video_ptr_U8_ptr->code_stru = ctx->code_stru;
#endif


	//memcpy(Fetch_input_video_ptr_U8->imgY_org, (const pel*)pic->y, ctx->info.pic_width * ctx->info.pic_height * sizeof(pel));

	for (int i = 0; i < ctx->info.pic_height; i++) {
		for (int j = 0; j < ctx->info.pic_width; j++) {
			Fetch_input_video_ptr_U8->imgY_org[i * ctx->info.pic_width + j] = (pel)pic->y[i * ctx->info.pic_width + j];
		}
	}
	for (int i = 0; i < ctx->info.pic_height; i++) {
		for (int j = 0; j < ctx->info.pic_width; j++) {
			if (Fetch_input_video_ptr_U8->imgY_org[i * ctx->info.pic_width + j] != Fetch_input_video_ptr->imgY_org[i * ctx->info.pic_width + j]) {
				printf(" i = %d j= %d \n", i, j);
			}
		}
	}
#if Debug422
    for (int i = 0; i < ctx->info.pic_height; i++)
    {
        for (int j = 0; j < ctx->info.pic_width / 2; j++)
        {
            Fetch_input_video_ptr_U8->imgU_org[i * ctx->info.pic_width / 2 + j] = (pel)pic->u[i * ctx->info.pic_width / 2 + j];
            Fetch_input_video_ptr_U8->imgV_org[i * ctx->info.pic_width / 2 + j] = (pel)pic->v[i * ctx->info.pic_width / 2 + j];
        }
    }
#else
    for (int i = 0; i < ctx->info.pic_height / 2; i++)
    {
        for (int j = 0; j < ctx->info.pic_width / 2; j++)
        {
            Fetch_input_video_ptr_U8->imgU_org[i * ctx->info.pic_width / 2 + j] = (pel)pic->u[i * ctx->info.pic_width / 2 + j];
            Fetch_input_video_ptr_U8->imgV_org[i * ctx->info.pic_width / 2 + j] = (pel)pic->v[i * ctx->info.pic_width / 2 + j];
        }
    }
#endif

    Fetch_input_video_ptr_U8_ptr->imgY_org = Fetch_input_video_ptr_U8->imgY_org;
    Fetch_input_video_ptr_U8_ptr->imgU_org = Fetch_input_video_ptr_U8->imgU_org;
    Fetch_input_video_ptr_U8_ptr->imgV_org = Fetch_input_video_ptr_U8->imgV_org;

    if ((((ctx)->info).pic_header).slice_type != 1)
    {
        Fetch_input_video_ptr->RefbufY = (((((ctx)->refp)[0])[0]).pic)->y;
        Fetch_input_video_ptr->RefbufU = (((((ctx)->refp)[0])[0]).pic)->u;
        Fetch_input_video_ptr->RefbufV = (((((ctx)->refp)[0])[0]).pic)->v;

#if ENABLE_BFRAME
        int ref1_valid = 0;
        if (ctx->code_stru == RA && (((ctx)->info).pic_header).slice_type == SLICE_B && ctx->rpm.num_refp[1] >= 1)
        {
            Fetch_input_video_ptr_U8_ptr->RefbufY1 = (((((ctx)->refp)[0])[1]).pic)->y;
            Fetch_input_video_ptr_U8_ptr->RefbufU1 = (((((ctx)->refp)[0])[1]).pic)->u;
            Fetch_input_video_ptr_U8_ptr->RefbufV1 = (((((ctx)->refp)[0])[1]).pic)->v;
            Fetch_input_video_ptr_U8_ptr->y_rfcBin1 = (((((ctx)->refp)[0])[1]).pic)->y_rfcBin;
            Fetch_input_video_ptr_U8_ptr->y_rfcPrm1 = (((((ctx)->refp)[0])[1]).pic)->y_rfcPrm;
            Fetch_input_video_ptr_U8_ptr->u_rfcBin1 = (((((ctx)->refp)[0])[1]).pic)->u_rfcBin;
            Fetch_input_video_ptr_U8_ptr->u_rfcPrm1 = (((((ctx)->refp)[0])[1]).pic)->u_rfcPrm;
            Fetch_input_video_ptr_U8_ptr->v_rfcBin1 = (((((ctx)->refp)[0])[1]).pic)->v_rfcBin;
            Fetch_input_video_ptr_U8_ptr->v_rfcPrm1 = (((((ctx)->refp)[0])[1]).pic)->v_rfcPrm;
            ref1_valid = 1;
        }
        else if (ctx->rpm.num_refp[0] > 1)
        {
            Fetch_input_video_ptr_U8_ptr->RefbufY1 = (((((ctx)->refp)[1])[0]).pic)->y;
            Fetch_input_video_ptr_U8_ptr->RefbufU1 = (((((ctx)->refp)[1])[0]).pic)->u;
            Fetch_input_video_ptr_U8_ptr->RefbufV1 = (((((ctx)->refp)[1])[0]).pic)->v;
            Fetch_input_video_ptr_U8_ptr->y_rfcBin1 = (((((ctx)->refp)[1])[0]).pic)->y_rfcBin;
            Fetch_input_video_ptr_U8_ptr->y_rfcPrm1 = (((((ctx)->refp)[1])[0]).pic)->y_rfcPrm;
            Fetch_input_video_ptr_U8_ptr->u_rfcBin1 = (((((ctx)->refp)[1])[0]).pic)->u_rfcBin;
            Fetch_input_video_ptr_U8_ptr->u_rfcPrm1 = (((((ctx)->refp)[1])[0]).pic)->u_rfcPrm;
            Fetch_input_video_ptr_U8_ptr->v_rfcBin1 = (((((ctx)->refp)[1])[0]).pic)->v_rfcBin;
            Fetch_input_video_ptr_U8_ptr->v_rfcPrm1 = (((((ctx)->refp)[1])[0]).pic)->v_rfcPrm;
            ref1_valid = 1;
        }
#endif

        for (int i = 0; i < (ctx->info.pic_height + 2 * ctx->refp[0][0].pic->padsize_luma); i++)
        {
            for (int j = 0; j < (ctx->refp[0][0].pic->stride_luma); j++)
            {
                Fetch_input_video_ptr_U8->RefbufY[i * (ctx->refp[0][0].pic->stride_luma) + j] =
                    (pel)(((((ctx)->refp)[0])[0]).pic)->y[i * (ctx->refp[0][0].pic->stride_luma) + j - ((ctx->refp[0][0].pic->stride_luma + 1) * ctx->refp[0][0].pic->padsize_luma)];
#if ENABLE_BFRAME
                if (ref1_valid)
                {
                    Fetch_input_video_ptr_U8->RefbufY1[i * (ctx->refp[0][0].pic->stride_luma) + j] =
                        (pel)Fetch_input_video_ptr_U8_ptr->RefbufY1[i * (ctx->refp[0][0].pic->stride_luma) + j - ((ctx->refp[0][0].pic->stride_luma + 1) * ctx->refp[0][0].pic->padsize_luma)];
                }
#endif
            }
        }

        for (int i = 0; i < (ctx->info.pic_height / 2 + 2 * ctx->refp[0][0].pic->padsize_chroma); i++)
        {
            for (int j = 0; j < (ctx->refp[0][0].pic->stride_chroma); j++)
            {
                Fetch_input_video_ptr_U8->RefbufU[i * (ctx->refp[0][0].pic->stride_chroma) + j] =
                    (pel)(((((ctx)->refp)[0])[0]).pic)->u[i * (ctx->refp[0][0].pic->stride_chroma) + j - ((ctx->refp[0][0].pic->stride_chroma + 1) * ctx->refp[0][0].pic->padsize_chroma)];
                Fetch_input_video_ptr_U8->RefbufV[i * (ctx->refp[0][0].pic->stride_chroma) + j] =
                    (pel)(((((ctx)->refp)[0])[0]).pic)->v[i * (ctx->refp[0][0].pic->stride_chroma) + j - ((ctx->refp[0][0].pic->stride_chroma + 1) * ctx->refp[0][0].pic->padsize_chroma)];
#if ENABLE_BFRAME
                if (ref1_valid)
                {
                    Fetch_input_video_ptr_U8->RefbufU1[i * (ctx->refp[0][0].pic->stride_chroma) + j] =
                        (pel)Fetch_input_video_ptr_U8_ptr->RefbufU1[i * (ctx->refp[0][0].pic->stride_chroma) + j - ((ctx->refp[0][0].pic->stride_chroma + 1) * ctx->refp[0][0].pic->padsize_chroma)];
                    Fetch_input_video_ptr_U8->RefbufV1[i * (ctx->refp[0][0].pic->stride_chroma) + j] =
                        (pel)Fetch_input_video_ptr_U8_ptr->RefbufV1[i * (ctx->refp[0][0].pic->stride_chroma) + j - ((ctx->refp[0][0].pic->stride_chroma + 1) * ctx->refp[0][0].pic->padsize_chroma)];
                }
#endif
            }
        }

        Fetch_input_video_ptr_U8_ptr->RefbufY = Fetch_input_video_ptr_U8->RefbufY;
        Fetch_input_video_ptr_U8_ptr->RefbufU = Fetch_input_video_ptr_U8->RefbufU;
        Fetch_input_video_ptr_U8_ptr->RefbufV = Fetch_input_video_ptr_U8->RefbufV;
#if ENABLE_BFRAME
        Fetch_input_video_ptr_U8_ptr->RefbufY1 = Fetch_input_video_ptr_U8->RefbufY1;
        Fetch_input_video_ptr_U8_ptr->RefbufU1 = Fetch_input_video_ptr_U8->RefbufU1;
        Fetch_input_video_ptr_U8_ptr->RefbufV1 = Fetch_input_video_ptr_U8->RefbufV1;
#endif
        Fetch_input_video_ptr_U8_ptr->y_rfcBin = (((((ctx)->refp)[0])[0]).pic)->y_rfcBin;
        Fetch_input_video_ptr_U8_ptr->y_rfcPrm = (((((ctx)->refp)[0])[0]).pic)->y_rfcPrm;
        Fetch_input_video_ptr_U8_ptr->u_rfcBin = (((((ctx)->refp)[0])[0]).pic)->u_rfcBin;
        Fetch_input_video_ptr_U8_ptr->u_rfcPrm = (((((ctx)->refp)[0])[0]).pic)->u_rfcPrm;
        Fetch_input_video_ptr_U8_ptr->v_rfcBin = (((((ctx)->refp)[0])[0]).pic)->v_rfcBin;
        Fetch_input_video_ptr_U8_ptr->v_rfcPrm = (((((ctx)->refp)[0])[0]).pic)->v_rfcPrm;
    }
    return COM_OK;
}

#if USE_INTRA_REFRESH
void calc_intra_refresh_interval(ENC_CTX *ctx)
{
    if (ctx->param.EncRefresh)
    {

        if (ctx->param.RefreshMode == 0)
        {
            int RefreshSize = 32 * ctx->param.RefreshSize;
            int numBlocksInCol = (ctx->param.pic_height + RefreshSize - 1) / RefreshSize;
            if (ctx->info.pic_header.slice_type == SLICE_I)
            {
                ctx->info.framesSinceLastPir = 0;
                ctx->info.pirEndRow = numBlocksInCol;
            }
            else
            {
                int pocdiff = 1;
                int numPFramesInGOP = ctx->param.i_period_org;
                int increment = (numBlocksInCol + numPFramesInGOP - 1) / numPFramesInGOP;

                if (ctx->info.pirEndRow == numBlocksInCol)
                    ctx->info.pirEndRow = 0;

                ctx->info.framesSinceLastPir = ctx->info.framesSinceLastPir + pocdiff;
                if (ctx->info.framesSinceLastPir >= ctx->param.i_period)
                {
                    ctx->info.pirEndRow = 0;
                    ctx->info.framesSinceLastPir = 0;
                }
                ctx->info.pirStartRow = ctx->info.pirEndRow;
                ctx->info.pirEndRow += increment;
                /* If our intra refresh has reached the right side of the frame, we're done. */
                if (ctx->info.pirEndRow >= numBlocksInCol)
                {
                    ctx->info.pirEndRow = numBlocksInCol;
                }
            }
        }
        else
        {
            int RefreshSize = 32 * ctx->param.RefreshSize;
            int numBlocksInRow = (ctx->param.pic_width + RefreshSize - 1) / RefreshSize;
            if (ctx->info.pic_header.slice_type == SLICE_I)
            {
                ctx->info.framesSinceLastPir = 0;
                ctx->info.pirEndCol = numBlocksInRow;
            }
            else
            {
                int pocdiff = 1;
                int numPFramesInGOP = ctx->param.i_period;
                int increment = (numBlocksInRow + numPFramesInGOP - 1) / numPFramesInGOP;

                if (ctx->info.pirEndCol == numBlocksInRow)
                    ctx->info.pirEndCol = 0;

                ctx->info.framesSinceLastPir = ctx->info.framesSinceLastPir + pocdiff;
                if (ctx->info.framesSinceLastPir >= ctx->param.i_period)
                {
                    ctx->info.pirEndCol = 0;
                    ctx->info.framesSinceLastPir = 0;
                }
                ctx->info.pirStartCol = ctx->info.pirEndCol;
                ctx->info.pirEndCol += increment;
                /* If our intra refresh has reached the right side of the frame, we're done. */
                if (ctx->info.pirEndCol >= numBlocksInRow)
                {
                    ctx->info.pirEndCol = numBlocksInRow;
                }
            }
        }
    }
}
#endif

#if USE_ROI_QP_CONFIG || USE_ROI_MODE_CONFIG

void mapLcuQp(ENC_CTX *ctx, int use_ROI_qp, int ROI_qp_Start_x, int ROI_qp_Start_y, int ROI_qp_End_x, int ROI_qp_End_y, int ROI_qp, MapLcu *mapLcu)
{
    int lcu_w = (ctx->info.pic_width + 63) >> 6;
    int lcu_h = (ctx->info.pic_height + 63) >> 6;

    // int MinLCU_qp = ctu_ctrl_input[ctx->die_idx].Frame_qp - 32;
    // int MaxLCU_qp = ctu_ctrl_input[ctx->die_idx].Frame_qp + 32;

    // MinLCU_qp = Clip3(MIN_QP, MAX_QP, MinLCU_qp);
    // MaxLCU_qp = Clip3(MIN_QP, MAX_QP, MaxLCU_qp);

    for (int j = 0; j < lcu_h; j++)
    {
        for (int i = 0; i < lcu_w; i++)
        {
            if ((i >= (ROI_qp_Start_x >> 6)) && (i < (ROI_qp_End_x >> 6)) && (j >= (ROI_qp_Start_y >> 6)) && (j < (ROI_qp_End_y >> 6))) // in region
            {
                if (use_ROI_qp)
                {
                    mapLcu[j * lcu_w + i].use_ROI_qp = use_ROI_qp;

                    mapLcu[j * lcu_w + i].ROI_qp = ROI_qp;

                    // mapLcu[j*lcu_w + i].ROI_qp = Clip3(MinLCU_qp, MaxLCU_qp, mapLcu[j*lcu_w + i].ROI_qp);	//
                }
            }
        }
    }
}

void mapLcuMode(ENC_CTX *ctx, int use_ROI_mode, int ROI_Mode_Start_x, int ROI_Mode_Start_y, int ROI_Mode_End_x, int ROI_Mode_End_y, int ROI_Mode, MapLcu *mapLcu)
{
    int lcu_w = (ctx->info.pic_width + 63) >> 6;
    int lcu_h = (ctx->info.pic_height + 63) >> 6;

    // int MinLCU_qp = ctu_ctrl_input[ctx->die_idx].Frame_qp - 32;
    // int MaxLCU_qp = ctu_ctrl_input[ctx->die_idx].Frame_qp + 32;

    // MinLCU_qp = Clip3(MIN_QP, MAX_QP, MinLCU_qp);
    // MaxLCU_qp = Clip3(MIN_QP, MAX_QP, MaxLCU_qp);

    for (int j = 0; j < lcu_h; j++)
    {
        for (int i = 0; i < lcu_w; i++)
        {
            if ((i >= (ROI_Mode_Start_x >> 6)) && (i < (ROI_Mode_End_x >> 6)) && (j >= (ROI_Mode_Start_y >> 6)) && (j < (ROI_Mode_End_y >> 6))) // in region
            {
                if (use_ROI_mode)
                {
                    mapLcu[j * lcu_w + i].use_ROI_Mode = use_ROI_mode;

                    mapLcu[j * lcu_w + i].ROI_Mode = ROI_Mode;
                }
            }
        }
    }
}

#if USE_ROI_QP_CONFIG
extern char op_fname_qpmap[256];
#endif
#if USE_ROI_MODE_CONFIG
extern char op_fname_modemap[256];
#endif
void writeMapLcu(ENC_CTX *ctx)
{
    MapLcu *mapLcu;
    unsigned char *QpMapLcu;
    unsigned char *ModeMapLcu;
    int lcu_w = (ctx->info.pic_width + 63) >> 6;
    int lcu_h = (ctx->info.pic_height + 63) >> 6;
    mapLcu = (MapLcu *)malloc(sizeof(MapLcu) * lcu_w * lcu_h);

    QpMapLcu = (unsigned char *)malloc(sizeof(unsigned char) * lcu_w * lcu_h);
    ModeMapLcu = (unsigned char *)malloc(sizeof(unsigned char) * lcu_w * lcu_h);

    memset(mapLcu, 0, sizeof(MapLcu) * lcu_w * lcu_h);
    memset(QpMapLcu, 0, sizeof(unsigned char) * lcu_w * lcu_h);
    memset(ModeMapLcu, 0, sizeof(unsigned char) * lcu_w * lcu_h);
    // QP

#if USE_ROI_QP_CONFIG

    if (ctx->info.useQPMAP)
    {
        if (ctx->info.useQPMAPCon == 0)
        {
            mapLcuQp(ctx, ctx->info.use_ROI_qp1, ctx->info.ROI_qp_Start_x1, ctx->info.ROI_qp_Start_y1,
                     ctx->info.ROI_qp_End_x1, ctx->info.ROI_qp_End_y1, ctx->info.ROI_QP1, mapLcu); // 1

            mapLcuQp(ctx, ctx->info.use_ROI_qp2, ctx->info.ROI_qp_Start_x2, ctx->info.ROI_qp_Start_y2,
                     ctx->info.ROI_qp_End_x2, ctx->info.ROI_qp_End_y2, ctx->info.ROI_QP2, mapLcu); // 2

            mapLcuQp(ctx, ctx->info.use_ROI_qp3, ctx->info.ROI_qp_Start_x3, ctx->info.ROI_qp_Start_y3,
                     ctx->info.ROI_qp_End_x3, ctx->info.ROI_qp_End_y3, ctx->info.ROI_QP3, mapLcu); // 3

            mapLcuQp(ctx, ctx->info.use_ROI_qp4, ctx->info.ROI_qp_Start_x4, ctx->info.ROI_qp_Start_y4,
                     ctx->info.ROI_qp_End_x4, ctx->info.ROI_qp_End_y4, ctx->info.ROI_QP4, mapLcu); // 4

            mapLcuQp(ctx, ctx->info.use_ROI_qp5, ctx->info.ROI_qp_Start_x5, ctx->info.ROI_qp_Start_y5,
                     ctx->info.ROI_qp_End_x5, ctx->info.ROI_qp_End_y5, ctx->info.ROI_QP5, mapLcu); // 5

            mapLcuQp(ctx, ctx->info.use_ROI_qp6, ctx->info.ROI_qp_Start_x6, ctx->info.ROI_qp_Start_y6,
                     ctx->info.ROI_qp_End_x6, ctx->info.ROI_qp_End_y6, ctx->info.ROI_QP6, mapLcu); // 6

            mapLcuQp(ctx, ctx->info.use_ROI_qp7, ctx->info.ROI_qp_Start_x7, ctx->info.ROI_qp_Start_y7,
                     ctx->info.ROI_qp_End_x7, ctx->info.ROI_qp_End_y7, ctx->info.ROI_QP7, mapLcu); // 7

            mapLcuQp(ctx, ctx->info.use_ROI_qp8, ctx->info.ROI_qp_Start_x8, ctx->info.ROI_qp_Start_y8,
                     ctx->info.ROI_qp_End_x8, ctx->info.ROI_qp_End_y8, ctx->info.ROI_QP8, mapLcu); // 8
        }
        else
        {
            FILE *lcu_qpmap_in = fopen(op_fname_qpmap, "rb");
            fread(&QpMapLcuMapLcu, sizeof(unsigned char), lcu_w * lcu_h, lcu_qpmap_in);

            for (int j = 0; j < lcu_h; j++)
            {
                for (int i = 0; i < lcu_w; i++)
                {
                    mapLcu[j * lcu_w + i].use_ROI_qp = 1;
                    mapLcu[j * lcu_w + i].ROI_qp = *(QpMapLcu + i + j * lcu_w);
                }
            }

            fflush(lcu_qpmap_in);
            fclose(lcu_qpmap_in);
        }
    }

#endif
#if USE_ROI_MODE_CONFIG

    // Mode
    if (ctx->info.useModeMAP)
    {
        if (ctx->info.useModeMAPCon == 0)
        {
            mapLcuMode(ctx, ctx->info.use_ROI_Mode1, ctx->info.ROI_Mode_Start_x1, ctx->info.ROI_Mode_Start_y1,
                       ctx->info.ROI_Mode_End_x1, ctx->info.ROI_Mode_End_y1, ctx->info.ROI_Mode1, mapLcu);

            mapLcuMode(ctx, ctx->info.use_ROI_Mode2, ctx->info.ROI_Mode_Start_x2, ctx->info.ROI_Mode_Start_y2,
                       ctx->info.ROI_Mode_End_x2, ctx->info.ROI_Mode_End_y2, ctx->info.ROI_Mode2, mapLcu);

            mapLcuMode(ctx, ctx->info.use_ROI_Mode3, ctx->info.ROI_Mode_Start_x3, ctx->info.ROI_Mode_Start_y3,
                       ctx->info.ROI_Mode_End_x3, ctx->info.ROI_Mode_End_y3, ctx->info.ROI_Mode3, mapLcu);

            mapLcuMode(ctx, ctx->info.use_ROI_Mode4, ctx->info.ROI_Mode_Start_x4, ctx->info.ROI_Mode_Start_y4,
                       ctx->info.ROI_Mode_End_x4, ctx->info.ROI_Mode_End_y4, ctx->info.ROI_Mode4, mapLcu);

            mapLcuMode(ctx, ctx->info.use_ROI_Mode5, ctx->info.ROI_Mode_Start_x5, ctx->info.ROI_Mode_Start_y5,
                       ctx->info.ROI_Mode_End_x5, ctx->info.ROI_Mode_End_y5, ctx->info.ROI_Mode5, mapLcu);

            mapLcuMode(ctx, ctx->info.use_ROI_Mode6, ctx->info.ROI_Mode_Start_x6, ctx->info.ROI_Mode_Start_y6,
                       ctx->info.ROI_Mode_End_x6, ctx->info.ROI_Mode_End_y6, ctx->info.ROI_Mode6, mapLcu);

            mapLcuMode(ctx, ctx->info.use_ROI_Mode7, ctx->info.ROI_Mode_Start_x7, ctx->info.ROI_Mode_Start_y7,
                       ctx->info.ROI_Mode_End_x7, ctx->info.ROI_Mode_End_y7, ctx->info.ROI_Mode7, mapLcu);

            mapLcuMode(ctx, ctx->info.use_ROI_Mode8, ctx->info.ROI_Mode_Start_x8, ctx->info.ROI_Mode_Start_y8,
                       ctx->info.ROI_Mode_End_x8, ctx->info.ROI_Mode_End_y8, ctx->info.ROI_Mode8, mapLcu);
        }
        else
        {
            FILE *lcu_modemap_in = fopen(op_fname_modemap, "rb");
            fread(&ModeMapLcu, sizeof(unsigned char), lcu_w * lcu_h, lcu_modemap_in);

            for (int j = 0; j < lcu_h; j++)
            {
                for (int i = 0; i < lcu_w; i++)
                {
                    {
                        mapLcu[j * lcu_w + i].use_ROI_Mode = 1;
                        mapLcu[j * lcu_w + i].ROI_Mode = *(ModeMapLcu + i + j * lcu_w);
                    }
                }
            }

            fflush(lcu_modemap_in);
            fclose(lcu_modemap_in);
        }
    }

#endif

    if (ctx->info.useQPMAP || ctx->info.useModeMAP)
    {
        ctx->lcumap_in = fopen("lcuMap", "wb");
        fwrite(mapLcu, sizeof(MapLcu), lcu_w * lcu_h, ctx->lcumap_in);
        fflush(ctx->lcumap_in);
        fclose(ctx->lcumap_in);
    }

    free(mapLcu);
}
#endif
#if CUDQP_QP_MAP
extern char op_cu_qpmap[256];
void writeMapCu(ENC_CTX *ctx)
{
    int lcu_w = (ctx->info.pic_width) >> 2;
    int lcu_h = (ctx->info.pic_height) >> 2;

    ctx->mapcuqp = (unsigned int *)malloc(sizeof(unsigned int) * lcu_w * lcu_h);
    unsigned int qp = 0;
    ;

    memset(ctx->mapcuqp, 0, sizeof(unsigned char) * lcu_w * lcu_h);

    if (ctx->info.useCuQPMAP)
    {
        FILE *cu_qpmap_in = fopen(op_cu_qpmap, "rb");

        for (int j = 0; j < lcu_h; j++)
        {
            for (int i = 0; i < lcu_w; i++)
            {
                fscanf(cu_qpmap_in, "%d", &qp);
                // ctx->mapcuqp[j * lcu_w + i] = qp;
                *(ctx->mapcuqp + j * lcu_w + i) = qp;
            }
        }

        fflush(cu_qpmap_in);
        fclose(cu_qpmap_in);
    }
}
#endif
void SliceInit(ENC_CTX *ctx, ENC_CORE *core, COM_PIC_HEADER *pic_header, CTU_Info *ctu_info)
{
#if USE_INTRA_REFRESH
    calc_intra_refresh_interval(ctx);
#endif
#if USE_ROI_QP_CONFIG || USE_ROI_MODE_CONFIG
    writeMapLcu(ctx);
#endif
#if CUDQP_QP_MAP
    writeMapCu(ctx);

#endif

    ctu_ctrl_init(ctx, core, pic_header, ctu_info);

	fetch_init(ctx, &Fetch_input_video, &Fetch_input_video_U8, &Fetch_input_video_U8_ptr);
}

//****************************************************

//*************************************************************************************************************

// #if IP_Linebuffer

void com_check_split_mode_simpart(Slice_Level_Parameters *ctu_ctrl_input, int *split_allow, int cu_width_log2,
                                  int cu_height_log2, int boundary, int boundary_b, int boundary_r, int qt_depth, int bet_depth)
{
    const int min_bt_size = ctu_ctrl_input->min_cu_size;
    const int min_eqt_size = ctu_ctrl_input->min_cu_size;
    const int max_split_depth = ctu_ctrl_input->max_split_times;
    const int max_aspect_ratio = ctu_ctrl_input->max_part_ratio;
    const int min_qt_size = ctu_ctrl_input->min_qt_size;
    const int max_bt_size = ctu_ctrl_input->max_bt_size;
    const int max_eqt_size = ctu_ctrl_input->max_eqt_size;
    int max_aspect_ratio_eqt = max_aspect_ratio >> 1;
    int cu_w = 1 << cu_width_log2;
    int cu_h = 1 << cu_height_log2;
    int i;
    for (i = NO_SPLIT; i <= SPLIT_QUAD; i++)
    {
        split_allow[i] = 0;
    }
    if (boundary)
    {
        if (!boundary_r && !boundary_b)
        {
            split_allow[SPLIT_QUAD] = 1;
        }
        else if (boundary_r)
        {
            split_allow[SPLIT_BI_VER] = 1;
        }
        else if (boundary_b)
        {
            split_allow[SPLIT_BI_HOR] = 1;
        }
        assert(qt_depth + bet_depth < max_split_depth);
    }
    else
    {
        if (qt_depth + bet_depth >= max_split_depth)
        {
            split_allow[NO_SPLIT] = 1; // no further split allowed
        }
        else
        {
            if (cu_w <= cu_h * max_aspect_ratio && cu_h <= cu_w * max_aspect_ratio)
            {
                split_allow[NO_SPLIT] = 1;
            }
            if (cu_w > min_qt_size && bet_depth == 0)
            {
                split_allow[SPLIT_QUAD] = 1;
            }
            if ((cu_w <= max_bt_size && cu_h <= max_bt_size) && cu_h > min_bt_size && cu_w < cu_h * max_aspect_ratio)
            {
                split_allow[SPLIT_BI_HOR] = 1;
            }
            if ((cu_w <= max_bt_size && cu_h <= max_bt_size) && cu_w > min_bt_size && cu_h < cu_w * max_aspect_ratio)
            {
                split_allow[SPLIT_BI_VER] = 1;
            }
            if ((cu_w <= max_eqt_size && cu_h <= max_eqt_size) && cu_h > min_eqt_size * 2 && cu_w > min_eqt_size && cu_w < cu_h * max_aspect_ratio_eqt)
            {
                split_allow[SPLIT_EQT_HOR] = 1;
            }
            if ((cu_w <= max_eqt_size && cu_h <= max_eqt_size) && cu_w > min_eqt_size * 2 && cu_h > min_eqt_size && cu_h < cu_w * max_aspect_ratio_eqt)
            {
                split_allow[SPLIT_EQT_VER] = 1;
            }
        }
    }
}
void partition_me(FRAME_LEVEL_FW *frame_level_fw, ENC_ME_CU *me_cu_info, int x0, int y0, int cu_width_log2, int cu_height_log2, int pic_width,
                  int pic_height, int qt_depth, int bet_depth, int sq_parent)
{
    if (cu_width_log2 >= 2 && cu_height_log2 >= 2 && x0 < 64 && y0 < 64)
    {
        int cu_width = 1 << cu_width_log2;
        int cu_height = 1 << cu_height_log2;
        s8 split_mode = NO_SPLIT;
        int boundary = !(x0 + cu_width <= pic_width && y0 + cu_height <= pic_height);
        int boundary_r = 0, boundary_b = 0;
        int split_allow[SPLIT_QUAD + 1];
        boundary_b = boundary && (y0 + cu_height > pic_height) && !(x0 + cu_width > pic_width);
        boundary_r = boundary && (x0 + cu_width > pic_width) && !(y0 + cu_height > pic_height);
        int part_num;
        int next_sq_parent;
        int split_idx = 0;

        if (me_cu_info->info_addr_map_split[(cu_width_log2 - 2) + (cu_height_log2 - 2) * MAX_CU_DEPTH][PEL2SCU(x0)][PEL2SCU(y0)] == MAP_CU_UNAVAILABLE)
        {
            assert(me_cu_info->info_addr_map[PEL2SCU(x0)][PEL2SCU(y0)][cu_width_log2 - 2][cu_height_log2 - 2] == MAP_CU_UNAVAILABLE);
            me_cu_info->info_addr_map[PEL2SCU(x0)][PEL2SCU(y0)][cu_width_log2 - 2][cu_height_log2 - 2] = me_cu_info->info_idx;
            if (avs3_min(cu_width, cu_height) == 32)
            {
                split_idx = me_cu_info->info_idx32;
                me_cu_info->me_info32[split_idx].x0_scu = PEL2SCU(x0);
                me_cu_info->me_info32[split_idx].x0_scu = PEL2SCU(y0);
                if (cu_width == 32)
                {
                    me_cu_info->me_info32[split_idx].otherSizeLog2Minus = cu_height_log2 - 2;
                    me_cu_info->me_info32[split_idx].sideFlag = 1;
                }
                else
                {
                    me_cu_info->me_info32[split_idx].otherSizeLog2Minus = cu_width_log2 - 2;
                    me_cu_info->me_info32[split_idx].sideFlag = 0;
                }
                me_cu_info->info_idx32++;
            }
            if (avs3_min(cu_width, cu_height) == 16)
            {
                split_idx = me_cu_info->info_idx16;
                me_cu_info->me_info16[split_idx].x0_scu = PEL2SCU(x0);
                me_cu_info->me_info16[split_idx].x0_scu = PEL2SCU(y0);
                if (cu_width == 16)
                {
                    me_cu_info->me_info16[split_idx].otherSizeLog2Minus = cu_height_log2 - 2;
                    me_cu_info->me_info16[split_idx].sideFlag = 1;
                }
                else
                {
                    me_cu_info->me_info16[split_idx].otherSizeLog2Minus = cu_width_log2 - 2;
                    me_cu_info->me_info16[split_idx].sideFlag = 0;
                }
                me_cu_info->info_idx16++;
            }
            if (avs3_min(cu_width, cu_height) == 8)
            {
                split_idx = me_cu_info->info_idx8;
                me_cu_info->me_info8[split_idx].x0_scu = PEL2SCU(x0);
                me_cu_info->me_info8[split_idx].x0_scu = PEL2SCU(y0);
                if (cu_width == 8)
                {
                    me_cu_info->me_info8[split_idx].otherSizeLog2Minus = cu_height_log2 - 2;
                    me_cu_info->me_info8[split_idx].sideFlag = 1;
                }
                else
                {
                    me_cu_info->me_info8[split_idx].otherSizeLog2Minus = cu_width_log2 - 2;
                    me_cu_info->me_info8[split_idx].sideFlag = 0;
                }
                me_cu_info->info_idx8++;
            }
            if (avs3_min(cu_width, cu_height) == 4)
            {
                split_idx = me_cu_info->info_idx4;
                me_cu_info->me_info4[split_idx].x0_scu = PEL2SCU(x0);
                me_cu_info->me_info4[split_idx].x0_scu = PEL2SCU(y0);
                if (cu_width == 4)
                {
                    me_cu_info->me_info4[split_idx].otherSizeLog2Minus = cu_height_log2 - 2;
                    me_cu_info->me_info4[split_idx].sideFlag = 1;
                }
                else
                {
                    me_cu_info->me_info4[split_idx].otherSizeLog2Minus = cu_width_log2 - 2;
                    me_cu_info->me_info4[split_idx].sideFlag = 0;
                }
                me_cu_info->info_idx4++;
            }

            me_cu_info->info_addr_map_split[(cu_width_log2 - 2) + (cu_height_log2 - 2) * MAX_CU_DEPTH][PEL2SCU(x0)][PEL2SCU(y0)] = split_idx;

            if (cu_width == cu_height)
            {
                me_cu_info->me_info[me_cu_info->info_idx].sq_parent = me_cu_info->info_idx;
                next_sq_parent = me_cu_info->info_idx;
            }
            else
            {
                me_cu_info->me_info[me_cu_info->info_idx].sq_parent = sq_parent;
                next_sq_parent = sq_parent;
            }
            me_cu_info->info_idx++;
            com_check_split_mode_simpart(&ctu_ctrl_input, split_allow, cu_width_log2, cu_height_log2, boundary, boundary_b,
                                         boundary_r, qt_depth, bet_depth);
            U3 split_mode_order[MAX_SPLIT_NUM];
            int split_mode_num = 0;
            com_split_get_split_rdo_order(cu_width, cu_height, split_mode_order);
            for (split_mode_num = 1; split_mode_num < MAX_SPLIT_NUM; ++split_mode_num)
            {
                split_mode = split_mode_order[split_mode_num];
                COM_SPLIT_STRUCT split_struct;
                com_split_get_part_structure(split_mode, x0, y0, cu_width, cu_height, 0, 0, 0, &split_struct);
                if (split_allow[split_mode])
                {
                    for (part_num = 0; part_num < split_struct.part_count; ++part_num)
                    {
                        partition_me(frame_level_fw, me_cu_info, split_struct.x_pos[part_num], split_struct.y_pos[part_num], split_struct.log_cuw[part_num],
                                     split_struct.log_cuh[part_num], pic_width, pic_height, INC_QT_DEPTH(qt_depth, split_mode), INC_BET_DEPTH(bet_depth, split_mode), next_sq_parent);
                    }
                }
            }
        }
    }
}

void construct_cu_split_me(FRAME_LEVEL_FW* frame_level_fw, ENC_ME_CU* me_cu_info, int pic_width, int pic_height)
{
	int i, j, k, g;
	int buf_idx = 0;
	int p1_num = 0, p2_num = 0, p3_num = 0, p4_num = 0;
	int thd = 4;
	for (i = 0; i < MAX_CU_DEPTH; i++) {
		for (j = 0; j < MAX_CU_DEPTH; j++) {
			for (k = 0; k < SCU_IN_LCU_NUMS_LEN; k++) {
				for (g = 0; g < SCU_IN_LCU_NUMS_LEN; g++) {
					me_cu_info->info_addr_map[k][g][i][j] = MAP_CU_UNAVAILABLE;
					me_cu_info->info_addr_map_split[i + j * MAX_CU_DEPTH][k][g] = MAP_CU_UNAVAILABLE;
				}
			}
		}
	}
	me_cu_info->info_idx = 0;
	me_cu_info->info_idx32 = 0;
	me_cu_info->info_idx16 = 0;
	me_cu_info->info_idx8 = 0;
	me_cu_info->info_idx4 = 0;
	me_cu_info->pic_height_in_scu = pic_height >> 2;
	me_cu_info->pic_width_in_scu = pic_width >> 2;
	partition_me(frame_level_fw, me_cu_info, 0, 0, 6, 6, pic_width, pic_height, 0, 0, 0);
	me_cu_info->has_partitioned = 1;
}

int pinter_init_frame(ENC_CTX* ctx)
{
	ENC_PINTER* pi;
	int i, j;

	pi = &ctx->pinter;

	for (i = 0; i < MAX_NUM_REF_PICS; i++) {
		for (j = 0; j < REFP_NUM; j++)
		{
			pi->refp[i][j] = ctx->refp[i][j];
		}

	}

	return COM_OK;
}

int enc_mode_init_frame(ENC_CTX* ctx)
{
	int ret = COM_OK;
	ret = pinter_init_frame(ctx);
	com_assert_rv(ret == COM_OK, ret);
	return COM_OK;
}
void getMD2Fw(ENC_CTX* ctx, ENC_CORE* core, MD_FW* md_fw) {
	ENC_PINTER* pi = &ctx->pinter;
	md_fw->pic_width = ctx->info.pic_width;
	md_fw->pic_height = ctx->info.pic_height;
	md_fw->slice_type = ctx->info.pic_header.slice_type;
	md_fw->max_dt_size = ctx->info.sqh.max_dt_size;
	md_fw->dt_intra_enable_flag = ctx->info.sqh.dt_intra_enable_flag;
	md_fw->num_of_hmvp_cand = ctx->info.sqh.num_of_hmvp_cand;
	md_fw->ipf_enable_flag = ctx->info.sqh.ipf_enable_flag;
	md_fw->pic_width_in_scu = ctx->info.pic_width_in_scu;
	md_fw->pic_height_in_scu = ctx->info.pic_height_in_scu;
	md_fw->log2_max_cuwh = ctx->info.log2_max_cuwh;
	md_fw->max_cuwh = ctx->info.max_cuwh;
	md_fw->log2_culine = ctx->log2_culine;
	md_fw->f_scu = ctx->info.f_scu;
	if (IS_INTER_SLICE(md_fw->slice_type)) {
		for (int i = 0; i < 17; i++) {
			for (int j = 0; j < 2; j++) {
				md_fw->refp_ptr[i][j] = pi->refp[i][j].ptr;
			}
		}
#if ENABLE_BFRAME
		md_fw->num_refp[REFP_0] = ctx->rpm.num_refp[0];
		md_fw->num_refp[REFP_1] = ctx->rpm.num_refp[1];
#else
		md_fw->num_refp = ((u8)ctx->rpm.num_refp[0]);
#endif
		
		md_fw->cur_ptr = ctx->ptr;
	}
}
void getFETCH2Fw(ENC_CTX* ctx, ENC_CORE* core, FETCH2MD_FW* fetch2md_fw) {
	ENC_PINTER* pi = &ctx->pinter;
	fetch2md_fw->pic_width = ctx->info.pic_width;
	fetch2md_fw->pic_height = ctx->info.pic_height;
	fetch2md_fw->slice_type = ctx->info.pic_header.slice_type;
	fetch2md_fw->pic_width_in_scu = ctx->info.pic_width_in_scu;
	fetch2md_fw->pic_height_in_scu = ctx->info.pic_height_in_scu;
	fetch2md_fw->log2_max_cuwh = ctx->info.log2_max_cuwh;
	fetch2md_fw->max_cuwh = ctx->info.max_cuwh;
	fetch2md_fw->log2_culine = ctx->log2_culine;

	if (IS_INTER_SLICE(fetch2md_fw->slice_type)) {
		for (int i = 0; i < 17; i++) {
			for (int j = 0; j < 2; j++) {
				fetch2md_fw->refp_ptr[i][j] = pi->refp[i][j].ptr;
			}
		}
		
#if ENABLE_BFRAME
		fetch2md_fw->num_refp[0] = ((u8)ctx->rpm.num_refp[0]);
		fetch2md_fw->num_refp[1] = ((u8)ctx->rpm.num_refp[1]);
#else
		fetch2md_fw->num_refp = ((u8)ctx->rpm.num_refp[0]);
#endif
		fetch2md_fw->cur_ptr = ctx->ptr;
	}
}


void getFrameFw(ENC_CTX* ctx, ENC_CORE* core, MD_FW* md_fw, DBK_FW* dbk_fw, SAO_FW* sao_fw, AEC_FW* aec_fw) {

	md_fw->pic_width = ctx->info.pic_width;
	md_fw->pic_height = ctx->info.pic_height;
	md_fw->slice_type = ctx->info.pic_header.slice_type;
	md_fw->max_dt_size = ctx->info.sqh.max_dt_size;
	md_fw->num_of_hmvp_cand = ctx->info.sqh.num_of_hmvp_cand;
	md_fw->ipf_enable_flag = ctx->info.sqh.ipf_enable_flag;
	md_fw->pic_width_in_scu = ctx->info.pic_width_in_scu;
	md_fw->pic_height_in_scu = ctx->info.pic_height_in_scu;
	md_fw->log2_max_cuwh = ctx->info.log2_max_cuwh;
	md_fw->max_cuwh = ctx->info.max_cuwh;
	md_fw->f_scu = ctx->info.f_scu;


	dbk_fw->log2_max_cuwh = ctx->info.log2_max_cuwh;
	dbk_fw->pic_width = ctx->info.pic_width;
	dbk_fw->pic_height = ctx->info.pic_height;
	dbk_fw->pic_width_in_lcu = ctx->info.pic_width_in_lcu;
	dbk_fw->max_cuwh = ctx->info.max_cuwh;
	dbk_fw->pic_width_in_scu = ctx->info.pic_width_in_scu;
	dbk_fw->cross_patch_loop_filter = ctx->info.sqh.cross_patch_loop_filter;
	dbk_fw->chroma_quant_param_delta_cb = ctx->info.pic_header.chroma_quant_param_delta_cb;
	dbk_fw->chroma_quant_param_delta_cr = ctx->info.pic_header.chroma_quant_param_delta_cr;
	dbk_fw->pic_stride_luma = 68; ////////it is a const 
	dbk_fw->pic_stride_chroma = 36;


	sao_fw->sample_adaptive_offset_enable_flag = ctx->info.sqh.sample_adaptive_offset_enable_flag;
	sao_fw->cross_patch_loop_filter = ctx->info.sqh.cross_patch_loop_filter;
	sao_fw->pic_width = ctx->info.pic_width;
	sao_fw->pic_height = ctx->info.pic_height;
	sao_fw->log2_max_cuwh = ctx->info.log2_max_cuwh;
	sao_fw->pic_width_in_scu = ctx->info.pic_width_in_scu;
	sao_fw->bit_depth_internal = ctx->info.bit_depth_internal;
	sao_fw->qp_offset_bit_depth = ctx->info.qp_offset_bit_depth;

	aec_fw->f_scu = ctx->info.f_scu;
	aec_fw->lcu_num = core->lcu_num;
	aec_fw->skip_flag = core->skip_flag;
	aec_fw->log2_culine = ctx->log2_culine;
	aec_fw->max_cuwh = ctx->info.max_cuwh;
	aec_fw->pic_width_in_lcu = ctx->info.pic_width_in_lcu;
	aec_fw->pic_width_in_scu = ctx->info.pic_width_in_scu;
	aec_fw->slice_sao_enable[0] = ctx->info.shext.slice_sao_enable[0];
	aec_fw->slice_sao_enable[1] = ctx->info.shext.slice_sao_enable[1];
	aec_fw->slice_sao_enable[2] = ctx->info.shext.slice_sao_enable[2];
	aec_fw->pic_header_slice_type = ctx->info.pic_header.slice_type;
	aec_fw->pic_width = ctx->info.pic_width;
	aec_fw->pic_height = ctx->info.pic_height;

}

static void rmd_int_buffer(RMD_BUF* rmd_buf)
{
	const U8 v = 128;
	for (u32 i = 0; i < (sizeof(rmd_buf->up) / sizeof(rmd_buf->up[0])); i++) {
		rmd_buf->up[i] = v;
	}
	for (u32 i = 0; i < (sizeof(rmd_buf->left) / sizeof(rmd_buf->left[0])); i++) {
		rmd_buf->left[i] = v;
	}
	rmd_buf->top_left = v;
}


#if defined(LINUX)
#define print(args...) printf(args)
#else
#define print(args, ...) printf(args, __VA_ARGS__)
#endif
#define IFVCA_CLIP(n,avs3_min,avs3_max) (((n)>(avs3_max))? (avs3_max) : (((n)<(avs3_min))? (avs3_min) : (n)))

COM_IMGB* imgb_alloc(int width, int height, int cs, int bitDepth)
{
	int i;
	COM_IMGB* imgb;
	imgb = (COM_IMGB*)malloc(sizeof(COM_IMGB));
	if (imgb == NULL)
	{
		print("cannot create image buffer\n");
		return NULL;
	}
	memset(imgb, 0, sizeof(COM_IMGB));
	if (cs == COM_COLORSPACE_YUV420)
	{
		for (i = 0; i < 3; i++)
		{
			imgb->width[i] = imgb->width_aligned[i] = width;
			imgb->stride[i] = width * (bitDepth == 8 ? 1 : 2);
			imgb->height[i] = imgb->height_aligned[i] = height;
			imgb->buf_size[i] = imgb->stride[i] * height;
			imgb->addr_plane[i] = imgb->buf_addr[i] = (unsigned char*)malloc(imgb->buf_size[i]);
			if (imgb->addr_plane[i] == NULL)
			{
				print("cannot allocate picture buffer\n");
				return NULL;
			}
			if (i == 0)
			{
				width = (width + 1) >> 1;
				height = (height + 1) >> 1;
			}
		}
		imgb->np = 3;
	}
	else if (cs == COM_COLORSPACE_YUV422)
	{
		for (i = 0; i < 3; i++)
		{
			imgb->width[i] = imgb->width_aligned[i] = width;
			imgb->stride[i] = width * (bitDepth == 8 ? 1 : 2);
			imgb->height[i] = imgb->height_aligned[i] = height;
			imgb->buf_size[i] = imgb->stride[i] * height;
			imgb->addr_plane[i] = imgb->buf_addr[i] = (unsigned char*)malloc(imgb->buf_size[i]);
			if (imgb->addr_plane[i] == NULL)
			{
				print("cannot allocate picture buffer\n");
				return NULL;
			}
			if (i == 0)
			{
				width = (width + 1) >> 1;
				//height = (height + 1) >> 1;
			}
		}
		imgb->np = 3;
	}
	else
	{
		print("unsupported color space\n");
		if (imgb)free(imgb);
		return NULL;
	}
	imgb->cs = cs;
	return imgb;
}

static int imgb_read(FILE* fp, COM_IMGB* img, int bit_depth)
{
	unsigned char* p8;
	int             i, j;
	int scale = (bit_depth == 10 ? 2 : 1);
	size_t ret;
	assert(img->cs == COM_COLORSPACE_YUV420);

	for (i = 0; i < 3; i++)
	{
		p8 = (unsigned char*)img->addr_plane[i];
		for (j = 0; j < img->height[i]; j++)
		{
			ret = fread(p8, img->width[i] * scale, 1, fp);
			p8 += img->stride[i];
		}
	}
	return 0;
}

static void imgb_conv_8b_to_16b(COM_IMGB* imgb_dst, COM_IMGB* imgb_src,
	int shift)
{
	int i, j, k;
	unsigned char* s;
	short* d;
	for (i = 0; i < 3; i++)
	{
		s = (unsigned char*)imgb_src->addr_plane[i];
		d = (short*)imgb_dst->addr_plane[i];
		for (j = 0; j < imgb_src->height[i]; j++)
		{
			for (k = 0; k < imgb_src->width[i]; k++)
			{
				d[k] = (short)(s[k] << shift);
			}
			s = s + imgb_src->stride[i];
			d = (short*)(((unsigned char*)d) + imgb_dst->stride[i]);
		}
	}
}

int map_read(FILE* fp, COM_MAP* map, int f_scu, int f_lcu)
{
	int size;
	size_t ret;
	assert(map->map_scu != NULL);
	size = sizeof(u32) * f_scu;
	ret = fread(map->map_scu, size, 1, fp);

	assert(map->map_ipm != NULL);
	size = sizeof(s8) * f_scu;
	ret = fread(map->map_ipm, size, 1, fp);

	assert(map->map_patch_idx != NULL);
	size = sizeof(s8) * f_scu;
	ret = fread(map->map_patch_idx, size, 1, fp);

#if TB_SPLIT_EXT
	assert(map->map_pb_tb_part != NULL);
	size = sizeof(u32) * f_scu;
	ret = fread(map->map_pb_tb_part, size, 1, fp);
#endif

	assert(map->map_depth != NULL);
	size = sizeof(s8) * f_scu;
	ret = fread(map->map_depth, size, 1, fp);

	assert(map->map_cu_mode != NULL);
	size = sizeof(u32) * f_scu;
	ret = fread(map->map_cu_mode, size, 1, fp);



	static s8(*map_refi)[2] = NULL;
	static s16(*map_mv)[2][2] = NULL;
	
	assert(map->map_refi != NULL);
	size = sizeof(s8) * f_scu * 2;
	if(map_refi==NULL)
		map_refi = (s8(*)[2])malloc(size);
	ret = fread(map_refi, size, 1, fp);
	for (int i = 0; i < f_scu; i++)
	{
		map->map_refi[i][0] = map_refi[i][0];
		map->map_refi[i][1] = map_refi[i][1];

	}

	assert(map->map_mv != NULL);
	size = sizeof(s16) * f_scu * REFP_NUM * MV_D;
	if (map_mv == NULL)
		map_mv = (s16(*)[2][2])malloc(size);
	ret = fread(map_mv, size, 1, fp);
	for (int i = 0; i < f_scu; i++)
	{
		map->map_mv[i][0][0] = map_mv[i][0][0];
		map->map_mv[i][0][1] = map_mv[i][0][1];
		map->map_mv[i][1][0] = map_mv[i][1][0];
		map->map_mv[i][1][1] = map_mv[i][1][1];
	}

	return 0;
}

void get_AEC_FW(ENC_CTX* ctx, AEC_FW* aec_fw) {
	aec_fw->slice_sao_enable[0] = ctx->info.shext.slice_sao_enable[0];
	aec_fw->slice_sao_enable[1] = ctx->info.shext.slice_sao_enable[1];
	aec_fw->slice_sao_enable[2] = ctx->info.shext.slice_sao_enable[2];
	aec_fw->umve_enable_flag = ctx->info.sqh.umve_enable_flag;
	aec_fw->num_of_hmvp_cand = ctx->info.sqh.num_of_hmvp_cand;

	aec_fw->amvr_enable_flag = ctx->info.sqh.amvr_enable_flag;

	aec_fw->emvr_enable_flag = ctx->info.sqh.emvr_enable_flag;

	aec_fw->dt_intra_enable_flag = ctx->info.sqh.dt_intra_enable_flag;
	aec_fw->position_based_transform_enable_flag = ctx->info.sqh.position_based_transform_enable_flag;
	aec_fw->tscpm_enable_flag = ctx->info.sqh.tscpm_enable_flag;
	aec_fw->ipf_enable_flag = ctx->info.sqh.ipf_enable_flag;
	aec_fw->sample_adaptive_offset_enable_flag = ctx->info.sqh.sample_adaptive_offset_enable_flag;
	aec_fw->pic_width_in_scu = ctx->info.pic_width_in_scu;
	aec_fw->max_cuwh = ctx->info.max_cuwh;
	aec_fw->pic_width = ctx->info.pic_width;
	aec_fw->pic_height = ctx->info.pic_height;
	aec_fw->slice_type = ctx->info.pic_header.slice_type;
	aec_fw->log2_culine = ctx->log2_culine;
	aec_fw->pic_width_in_lcu = ctx->info.pic_width_in_lcu;
	aec_fw->lcu_total_cnt = ctx->info.pic_width_in_lcu * ctx->info.pic_height_in_lcu;

	aec_fw->rpm_num_refp[0] = ctx->rpm.num_refp[0];
	aec_fw->rpm_num_refp[1] = ctx->rpm.num_refp[1];
}

void get_Frame_Level_Fw(ENC_CTX* ctx, FRAME_LEVEL_FW* frame_level_fw, ENC_PINTER* pinter, COM_PM* ctx_rpm, int ctx_ptr) {
	frame_level_fw->lcu_num = 0;
	ENC_PINTER* pi = pinter;// &ctx->pinter;
	frame_level_fw->pic_width = ctx->info.pic_width;
	frame_level_fw->pic_height = ctx->info.pic_height;
	frame_level_fw->slice_type = ctx->info.pic_header.slice_type;
	frame_level_fw->max_dt_size = ctx->info.sqh.max_dt_size;
	frame_level_fw->dt_intra_enable_flag = ctx->info.sqh.dt_intra_enable_flag;
	frame_level_fw->num_of_hmvp_cand = ctx->info.sqh.num_of_hmvp_cand;
	frame_level_fw->ipf_enable_flag = ctx->info.sqh.ipf_enable_flag;
	frame_level_fw->pic_width_in_scu = ctx->info.pic_width_in_scu;
	frame_level_fw->pic_height_in_scu = ctx->info.pic_height_in_scu;
	frame_level_fw->log2_max_cuwh = ctx->info.log2_max_cuwh;
	frame_level_fw->max_cuwh = ctx->info.max_cuwh;
	frame_level_fw->log2_culine = ctx->log2_culine;
	frame_level_fw->f_scu = ctx->info.f_scu;
	if (IS_INTER_SLICE(frame_level_fw->slice_type)) {
		for (int i = 0; i < 17; i++) {
			for (int j = 0; j < 2; j++) {
				frame_level_fw->refp_ptr[i][j] = pi->refp[i][j].ptr;
			}
		}
		frame_level_fw->num_refp = ((u8)ctx_rpm->num_refp[0]);
		frame_level_fw->cur_ptr = ctx_ptr;
	}


	int lcu_qp = ctx->param.qp;// ctx->info.shext.slice_qp;
	double lambda[3];
	double dist_chroma_weight[2];
	//for (int i = 1; i <= 80; i++){
	//    lcu_qp = i;

	frame_level_fw->qp_y = lcu_qp;
	int    adj_qp_cb = frame_level_fw->qp_y + ctx->info.pic_header.chroma_quant_param_delta_cb - ctx->info.qp_offset_bit_depth;
	int adj_qp_cr = frame_level_fw->qp_y + ctx->info.pic_header.chroma_quant_param_delta_cr - ctx->info.qp_offset_bit_depth;
	adj_qp_cb = COM_CLIP(adj_qp_cb, MIN_QUANT - 16, MAX_QUANT_BASE);
	adj_qp_cr = COM_CLIP(adj_qp_cr, MIN_QUANT - 16, MAX_QUANT_BASE);
	if (adj_qp_cb >= 0) {
		adj_qp_cb = com_tbl_qp_chroma_adjust[COM_MIN(MAX_QUANT_BASE, adj_qp_cb)];
	}
	if (adj_qp_cr >= 0) {
		adj_qp_cr = com_tbl_qp_chroma_adjust[COM_MIN(MAX_QUANT_BASE, adj_qp_cr)];
	}
	frame_level_fw->qp_u = COM_CLIP(adj_qp_cb + ctx->info.qp_offset_bit_depth, MIN_QUANT,
		MAX_QUANT_BASE + ctx->info.qp_offset_bit_depth);
	frame_level_fw->qp_v = COM_CLIP(adj_qp_cr + ctx->info.qp_offset_bit_depth, MIN_QUANT,
		MAX_QUANT_BASE + ctx->info.qp_offset_bit_depth);

	dist_chroma_weight[0] = pow(2.0, (lcu_qp - adj_qp_cb) / 4.0);
	dist_chroma_weight[1] = pow(2.0, (lcu_qp - adj_qp_cr) / 4.0);


	lambda[0] = 1.43631 * pow(2.0, (lcu_qp - 16.0) / 4.0);
	lambda[1] = lambda[0] / dist_chroma_weight[0];
	lambda[2] = lambda[0] / dist_chroma_weight[1];


	//frame_level_fw->lambdas[0] = s64((1 << LAMBDA_SCALE) * lambda[0]);
	//frame_level_fw->lambdas[1] = s64((1 << LAMBDA_SCALE) * lambda[1]);
	//frame_level_fw->lambdas[2] = s64((1 << LAMBDA_SCALE) * lambda[2]);

	frame_level_fw->dist_chroma_weight[0] = (s64)(dist_chroma_weight[0] * (1 << LAMBDA_SCALE));
	frame_level_fw->dist_chroma_weight[1] = (s64)(dist_chroma_weight[1] * (1 << LAMBDA_SCALE));


	frame_level_fw->gop_size = ctx->param.gop_size;

	frame_level_fw->max_part_ratio = ctx->info.sqh.max_part_ratio;
	frame_level_fw->min_cu_size = ctx->info.sqh.min_cu_size;
	frame_level_fw->max_split_times = ctx->info.sqh.max_split_times;
	frame_level_fw->min_qt_size = ctx->info.sqh.min_qt_size;
	frame_level_fw->max_bt_size = ctx->info.sqh.max_bt_size;
	frame_level_fw->max_eqt_size = ctx->info.sqh.max_eqt_size;
	frame_level_fw->affine_subblock_size_idx = 1;
	frame_level_fw->ipcm_enable_flag = ctx->info.sqh.ipcm_enable_flag;
	frame_level_fw->secondary_transform_enable_flag = ctx->info.sqh.secondary_transform_enable_flag;


	frame_level_fw->pic_width_in_lcu = ctx->info.pic_width_in_lcu;
	frame_level_fw->pic_height_in_lcu = ctx->info.pic_height_in_lcu;
	frame_level_fw->cross_patch_loop_filter = ctx->info.sqh.cross_patch_loop_filter;
	frame_level_fw->chroma_quant_param_delta_cb = ctx->info.pic_header.chroma_quant_param_delta_cb;
	frame_level_fw->chroma_quant_param_delta_cr = ctx->info.pic_header.chroma_quant_param_delta_cr;
	frame_level_fw->pic_stride_luma = 68; ////////it is a const 
	frame_level_fw->pic_stride_chroma = 36;

	//AEC
	frame_level_fw->slice_sao_enable[0] = ctx->info.shext.slice_sao_enable[0];
	frame_level_fw->slice_sao_enable[1] = ctx->info.shext.slice_sao_enable[1];
	frame_level_fw->slice_sao_enable[2] = ctx->info.shext.slice_sao_enable[2];
	frame_level_fw->umve_enable_flag = ctx->info.sqh.umve_enable_flag;
	frame_level_fw->amvr_enable_flag = ctx->info.sqh.amvr_enable_flag;
	frame_level_fw->emvr_enable_flag = ctx->info.sqh.emvr_enable_flag;
	frame_level_fw->position_based_transform_enable_flag = ctx->info.sqh.position_based_transform_enable_flag;
	frame_level_fw->tscpm_enable_flag = ctx->info.sqh.tscpm_enable_flag;
	frame_level_fw->sample_adaptive_offset_enable_flag = ctx->info.sqh.sample_adaptive_offset_enable_flag;


	frame_level_fw->bit_depth_internal = ctx->info.bit_depth_internal;
	frame_level_fw->qp_offset_bit_depth = ctx->info.qp_offset_bit_depth;
	frame_level_fw->fixed_slice_qp_flag = ctx->info.pic_header.fixed_picture_qp_flag;

	//frame_level_fw->lcu_total_cnt = frame_level_fw->pic_width_in_lcu*frame_level_fw->pic_height_in_lcu;
	frame_level_fw->log2_scu_w = MIN_CU_LOG2;
	frame_level_fw->scu_w = MIN_CU_SIZE;

	////////////
	frame_level_fw->yuv_cs = u8(COM_COLORSPACE_YUV420);
	frame_level_fw->org_pic_y_addr = (uintptr_t)(PIC_ORG(ctx)->y) & 0xFFFFFFFF;
	frame_level_fw->org_pic_u_addr = (uintptr_t)(PIC_ORG(ctx)->u) & 0xFFFFFFFF;
	frame_level_fw->org_pic_v_addr = (uintptr_t)(PIC_ORG(ctx)->v) & 0xFFFFFFFF;
	frame_level_fw->rec_pic_y_addr = (uintptr_t)(PIC_REC(ctx)->y) & 0xFFFFFFFF;
	frame_level_fw->rec_pic_u_addr = (uintptr_t)(PIC_REC(ctx)->u) & 0xFFFFFFFF;
	frame_level_fw->rec_pic_v_addr = (uintptr_t)(PIC_REC(ctx)->v) & 0xFFFFFFFF;
	/////Control info for DBK
	frame_level_fw->glb_ctrl = 1;
	frame_level_fw->dbk_en = 1;
	frame_level_fw->cplf_en = 1;

	///SAO control
	frame_level_fw->module_enable = 1;
	frame_level_fw->sao_enable = 1;

	frame_level_fw->lambda = ctx->lambda[0];// (u32)core.lambda[0];
	frame_level_fw->patch_idx = 0;
	frame_level_fw->patch_width_in_lcu = ctx->patch->width;
	frame_level_fw->patch_height_in_lcu = ctx->patch->height;
	frame_level_fw->lcu_total_cnt = ctx->patch->width * ctx->patch->height;// frame_level_fw.pic_width_in_lcu*frame_level_fw.pic_height_in_lcu;


	frame_level_fw->frame_id++;//////////////////the initial value is "-1"
#if RATECONTROL
	frame_level_fw->EncControl= ctx->param.rc_flag;
	frame_level_fw->ECInitialQP=ctx->param.qp;
	frame_level_fw->useDQP= ctx->param.delta_qp_flag;
	frame_level_fw->TargetBitRate= ctx->param.rc_bitrate;
	frame_level_fw->framerate= ctx->param.fps;
	frame_level_fw->intra_period= ctx->param.i_period;
	frame_level_fw->TotalFrames= ctx->param.frames_to_be_encoded;
#endif
}

#if RATECONTROL

void RC_init()
{
	/*RateControl RC;*/

	pRC = &RC;
	memset(pRC, 0, sizeof(RateControl));
	frame_level_fw.prev_f_bits = 0;
	if (frame_level_fw.EncControl != 0) {
		Init_RateControl(pRC);
		
	}
}
#endif
const char fname_rec[] = REC_GOLDEN;
FILE* fp_rec;
const char fname_map[] = MAP_GOLDEN;
FILE* fp_map;
int pic_init(ENC_CTX* ctx, ENC_STAT* stat, COM_CNKH* cnkh, ENC_ME_LINE_MAP* me_line_map_ptr)
{

	ENC_CORE* core;
	COM_BSW* bs;
	COM_PIC_HEADER* pic_header;
	COM_SQH* sqh;
	COM_SH_EXT* shext;

	int   ret = 1;
	int   i;
	s8* map_delta_qp;
	int last_lcu_qp[1];
	//int last_lcu_delta_qp[1];
	PATCH_INFO* patch;
	u8 patch_idx = 0;

	int patch_cur_index[1] = { -1 };
	int num_of_patch;
	s8* patch_sao_enable = NULL;
	int seq_header_size = 0;
	COM_MODE* mod_info_curr;
	MD_RESULT md1_best_result;
	md1_best_result.cost_best = MAX_S64;

	bs = &ctx->bs;
	core = ctx->core;
	mod_info_curr = &core->mod_info_curr;
	pic_header = &ctx->info.pic_header;
	sqh = &ctx->info.sqh;
	shext = &ctx->info.shext;
	patch = ctx->patch;
	num_of_patch = patch->columns * patch->rows;
	patch_sao_enable = (s8*)com_malloc(sizeof(s8) * num_of_patch * N_C);

	rmd_int_buffer(&g_rmd_buf);

#if USE_IME_ALL_CU
	me_cu_info_global->cost_error_avg = 0;
	me_cu_info_global->mv_error[MV_X] = 0;
	me_cu_info_global->mv_error[MV_Y] = 0;
	me_cu_info_global->me_cnt = 0;
	me_cu_info_global->worse_cnt = 0;
	me_cu_info_global->sad_error_avg = 0;
	me_cu_info_global->sad_worse_cnt = 0;
#endif

	for (int i = 0; i < num_of_patch; i++) {
		for (int j = 0; j < N_C; j++) {
			*(patch_sao_enable + i * N_C + j) = 1;
		}
	}

	prepare_RefInfo(ctx, pic_header, ret, shext, patch_sao_enable, patch_cur_index[0]);

	ret = enc_mode_init_frame(ctx);
	if (ret != COM_OK) {
		if (patch_sao_enable) free(patch_sao_enable);
	}
	com_assert_rv(ret == COM_OK, ret);

	sbac_init(core, &::core, ctx, cnkh, bs);

	SliceInit(ctx, core, pic_header, &::core);

	patch->x_pel = patch->x_pat = patch->y_pel = patch->y_pat = patch->idx = 0;
	patch->left_pel = patch->x_pel;
	patch->up_pel = patch->y_pel;
	patch->right_pel = patch->x_pel + (*(patch->width_in_lcu + patch->x_pat) << ctx->info.log2_max_cuwh);
	patch->down_pel = patch->y_pel + (*(patch->height_in_lcu + patch->y_pat) << ctx->info.log2_max_cuwh);

#if USE_IME_ALL_CU || USE_ORG_QT_ONLY
	if (!me_cu_info_global->has_partitioned) {
		construct_cu_split_me(&frame_level_fw, me_cu_info_global, ctx->info.pic_width, ctx->info.pic_height);
	}
#endif

	int max_smb_num = ctx->lcu_cnt;
	get_AEC_FW(ctx, &aec_fw);
	get_Frame_Level_Fw(ctx, &frame_level_fw, &ctx->pinter, &ctx->rpm, ctx->ptr);

	int size_CTU = sizeof(ENC_CU_DATA);

	fetch2md_fw_ptr = &fetch2md_fw;
	fetch2dbk_fw_ptr = &fetch2dbk_fw;
	fetch2sao_fw_ptr = &fetch2sao_fw;
	AEC_DATA_input = &AEC_DATA_input_1;

#if RATECONTROL
	if (frame_level_fw.EncControl != 0 && frame_level_fw.frame_id == 1) {
		RC_init();
	}
	RC_init_frame();
	rate_control();
	pic_header->picture_qp = ctu_ctrl_input.qp_y;
	int lcu_qp = ctu_ctrl_input.qp_y;// ctx->info.shext.slice_qp;
	double lambda[3];
	double dist_chroma_weight[2];
	//for (int i = 1; i <= 80; i++){
	//    lcu_qp = i;

	frame_level_fw.qp_y = lcu_qp;
	int    adj_qp_cb = ctu_ctrl_input.qp_y + ctx->info.pic_header.chroma_quant_param_delta_cb - ctx->info.qp_offset_bit_depth;
	int adj_qp_cr = ctu_ctrl_input.qp_y + ctx->info.pic_header.chroma_quant_param_delta_cr - ctx->info.qp_offset_bit_depth;
	adj_qp_cb = COM_CLIP(adj_qp_cb, MIN_QUANT - 16, MAX_QUANT_BASE);
	adj_qp_cr = COM_CLIP(adj_qp_cr, MIN_QUANT - 16, MAX_QUANT_BASE);
	if (adj_qp_cb >= 0) {
		adj_qp_cb = com_tbl_qp_chroma_adjust[COM_MIN(MAX_QUANT_BASE, adj_qp_cb)];
	}
	if (adj_qp_cr >= 0) {
		adj_qp_cr = com_tbl_qp_chroma_adjust[COM_MIN(MAX_QUANT_BASE, adj_qp_cr)];
	}
	ctu_ctrl_input.qp_u = COM_CLIP(adj_qp_cb + ctx->info.qp_offset_bit_depth, MIN_QUANT,
		MAX_QUANT_BASE + ctx->info.qp_offset_bit_depth);
	ctu_ctrl_input.qp_v = COM_CLIP(adj_qp_cr + ctx->info.qp_offset_bit_depth, MIN_QUANT,
		MAX_QUANT_BASE + ctx->info.qp_offset_bit_depth);

	dist_chroma_weight[0] = pow(2.0, (lcu_qp - adj_qp_cb) / 4.0);
	dist_chroma_weight[1] = pow(2.0, (lcu_qp - adj_qp_cr) / 4.0);

	double qp_temp = lcu_qp - 18;// lcu_qp - SHIFT_QP;

	lambda[0] = 1.43631 * pow(2.0, (lcu_qp - 16.0) / 4.0);
#if FRAME_LAMBDA_OPT
	if (ctu_ctrl_input.depth > 0)
		lambda[0] *= Clip3(2.00, 4.00, (double)(qp_temp / 8.0));
#endif

	lambda[1] = lambda[2] = lambda[0];
	s64 ctx_sqrt_lambda[3], ctx_lambda[3];
	ctx_sqrt_lambda[0] = s64(sqrt(lambda[0]) * FIXED_SQRT_LAMBDA_NUM);
	ctx_sqrt_lambda[1] = s64(sqrt(lambda[1]) * FIXED_SQRT_LAMBDA_NUM);
	ctx_sqrt_lambda[2] = s64(sqrt(lambda[2]) * FIXED_SQRT_LAMBDA_NUM);

	ctx_lambda[0] = s64((1 << LAMBDA_SCALE) * lambda[0]);
	ctx_lambda[1] = s64((1 << LAMBDA_SCALE) * lambda[1]);
	ctx_lambda[2] = s64((1 << LAMBDA_SCALE) * lambda[2]);

	ctu_ctrl_input.lambda[0][0] = ctx_lambda[0] & 0xffffffff;
	ctu_ctrl_input.lambda[0][1] = (ctx_lambda[0] >> 32) & 0xffffffff;
	ctu_ctrl_input.lambda[1][0] = ctx_lambda[1] & 0xffffffff;
	ctu_ctrl_input.lambda[1][1] = (ctx_lambda[1] >> 32) & 0xffffffff;
	ctu_ctrl_input.lambda[2][0] = ctx_lambda[2] & 0xffffffff;
	ctu_ctrl_input.lambda[2][1] = (ctx_lambda[2] >> 32) & 0xffffffff;
	ctu_ctrl_input.sqrt_lambda[0][0] = ctx_sqrt_lambda[0] & 0xffffffff;
	ctu_ctrl_input.sqrt_lambda[0][1] = (ctx_sqrt_lambda[0] >> 32) & 0xffffffff;
	ctu_ctrl_input.sqrt_lambda[1][0] = ctx_sqrt_lambda[1] & 0xffffffff;
	ctu_ctrl_input.sqrt_lambda[1][1] = (ctx_sqrt_lambda[1] >> 32) & 0xffffffff;
	ctu_ctrl_input.sqrt_lambda[2][0] = ctx_sqrt_lambda[2] & 0xffffffff;
	ctu_ctrl_input.sqrt_lambda[2][1] = (ctx_sqrt_lambda[2] >> 32) & 0xffffffff;
#endif

#if LIBVC_ON
	//I-picture or RL-picture
	if (ctx->slice_type == SLICE_I || (ctx->info.sqh.library_picture_enable_flag && !ctx->rpm.libvc_data->is_libpic_processing && ctx->is_RLpic_flag))
#else
	if (ctx->slice_type == SLICE_I)
#endif
	{
		ret = enc_eco_sqh(ctx, bs, sqh);
		com_bsw_flush(bs);
		seq_header_size = COM_BSW_GET_WRITE_BYTE(bs);
		//printf("1 seq header size = %d\n", seq_header_size);
		com_bsw_init(bs, bs->cur, bs->buftmp, ((int)(bs->end - bs->cur)) + 1, NULL);
	}


	ret = enc_eco_pic_header(bs, pic_header, sqh);

	if (ctx->param.use_pic_sign) {
		enc_extension_and_user_data(ctx, bs, 0, 0, sqh, pic_header);
	}

	com_bsw_flush(bs);

	stat->write = COM_BSW_GET_WRITE_BYTE(bs) + seq_header_size;
	//printf("2 seq header size = %d\n", stat->write);
	header_length = stat->write;

	com_bsw_init(bs, bs->cur, bs->buftmp, ((int)(bs->end - bs->cur)) + 1, NULL);

	/*initial patch info*/
	patch->x_pel = patch->x_pat = patch->y_pel = patch->y_pat = patch->idx = 0;

	/*SET the patch boundary*/
	patch->left_pel = patch->x_pel;
	patch->up_pel = patch->y_pel;
	patch->right_pel = patch->x_pel + (*(patch->width_in_lcu + patch->x_pat) << ctx->info.log2_max_cuwh);
	patch->down_pel = patch->y_pel + (*(patch->height_in_lcu + patch->y_pat) << ctx->info.log2_max_cuwh);
	patch_cur_index[0] = -1;

	/* Encode patch header */
	patch->patch_sao_enable = patch_sao_enable;
	ret = enc_eco_patch_header(bs, &ctx->info.sqh, pic_header, shext, patch_idx, patch);

	com_bsw_flush(bs);
//#if USE_ENH
//	bs->code = 0;
//	bs->leftbits = 32;
//#endif
	AEC_DATA_output.bs_cur = 0;
	bs->byte_cnt_frame = 0;
	bs->slice_done = 0;
	com_bsw_init(&AEC_DATA_output.bs, AEC_DATA_output.bs_tmp, NULL, 3 * 1024 * 1024, NULL);


	//patch->patch_sao_enable = patch_sao_enable;
	set_sh(ctx, shext, patch->patch_sao_enable + patch_idx * N_C);

	com_assert_rv(ret == COM_OK, ret);

	core->x_lcu = core->y_lcu = 0;
	core->x_pel = core->y_pel = 0;
	::core.x_lcu = ::core.y_lcu = 0;
	::core.x_pel = ::core.y_pel = 0;
	core->lcu_num = 0;
	ctx->lcu_cnt = ctx->info.f_lcu;
	for (i = 0; i < ctx->info.f_scu; i++) {
		MCU_CLR_CODED_FLAG(ctx->map.map_scu[i]);
	}
	enc_sbac_init(bs);

	com_sbac_ctx_init(&(GET_SBAC_ENC(bs)->ctx));// 3th call

	last_lcu_qp[0] = ctx->info.shext.slice_qp;
	map_delta_qp = ctx->map.map_delta_qp;

	for (i = 0; i < 12; i++) {
		rdoq_model.last1[i] = PROB_INIT;
		model_cnt.last1[i] = 0;
	}
	for (i = 0; i < 22; i++) {
		rdoq_model.last2[i] = PROB_INIT;
		model_cnt.last2[i] = 0;
	}
	for (i = 0; i < 24; i++) {
		rdoq_model.run_rdoq[i] = PROB_INIT;
		model_cnt.run_rdoq[i] = 0;
		rdoq_model.level[i] = PROB_INIT;
		model_cnt.level[i] = 0;
	}
	for (i = 0; i < 3; i++) {
		rdoq_model.cbf[i] = PROB_INIT;
		model_cnt.cbf[i] = 0;
	}


#if UMVE_ENH
	ctx->info.pic_header.umve_set_flag = 0;
	if (ctx->needReset && (ctx->info.pic_header.poc > ctx->lastIPicPOC))
	{
		ctx->needReset = FALSE;
		ctx->umveAveOffset = -1;
	}
	if (ctx->dataCol && ctx->umveAveOffset > 1.1)
	{
		ctx->info.pic_header.umve_set_flag = 1;
	}
#if CRR_FIX_UMATCH
	if (ctx->info.pic_header.is_RLpic_flag)
	{
		ctx->info.pic_header.umve_set_flag = 0;
	}
#endif
#endif

#if UMVE_ENH
	ctx->dataCol = FALSE;
#endif

#if UMVE_ENH
#if CRR_FIX_UMATCH
	ctx->dataCol = (!ctx->info.sqh.umve_enh_enable_flag || pic_header->slice_type == SLICE_I || pic_header->is_RLpic_flag) ? FALSE : TRUE;
	if (pic_header->slice_type == SLICE_I || pic_header->is_RLpic_flag)
	{
		ctx->needReset = TRUE;
		ctx->lastIPicPOC = pic_header->poc;
	}
#else
	ctx->dataCol = (!ctx->info.sqh.umve_enh_enable_flag || pic_header->slice_type == SLICE_I) ? FALSE : TRUE;
	if (pic_header->slice_type == SLICE_I)
	{
		ctx->needReset = TRUE;
		ctx->lastIPicPOC = pic_header->poc;
	}
#endif
	for (u8 idx = 0; idx < UMVE_REFINE_STEP_SEC_SET && ctx->dataCol; idx++)
	{
		ctx->umveOffsetPicCount[idx] = 0;
	}
#endif
	return COM_OK;
}
int pic_end(ENC_CTX *ctx, ENC_STAT *stat, ENC_ME_LINE_MAP *me_line_map_ptr
#if USE_ROI_QP_CONFIG || USE_ROI_MODE_CONFIG
            ,
            CTX_FETCH *fetch_ctx
#endif
)
{

    COM_BSW *bs = &ctx->bs;

    if (ctx->ptr < SKIP_FRAME)
    {
        if (fp_rec == NULL)
        {
            fp_rec = fopen(fname_rec, "rb");
        }
        if (fp_rec == NULL)
        {
            print("cannot open file = %s\n", fname_rec);
            return -1;
        }
        COM_IMGB *img = imgb_alloc(ctx->info.pic_width, ctx->info.pic_height, COM_COLORSPACE_YUV420, ctx->info.bit_depth_internal);
        int ret = imgb_read(fp_rec, img, ctx->info.bit_depth_internal);
        assert(ret == 0);
        imgb_conv_8b_to_16b(PIC_REC(ctx)->imgb, img, 0);

        if (fp_map == NULL)
        {
            fp_map = fopen(fname_map, "rb");
        }
        if (fp_map == NULL)
        {
            print("cannot open file = %s\n", fname_map);
            return -1;
        }
        ret = map_read(fp_map, &ctx->map, ctx->info.f_scu, ctx->info.f_lcu);
        assert(ret == 0);
        for (int i = 0; i < ctx->info.f_scu; i++)
        {
            PIC_REC(ctx)->map_refi[i][0] = ctx->map.map_refi[i][0];
            PIC_REC(ctx)->map_refi[i][1] = ctx->map.map_refi[i][1];
        }

        for (int i = 0; i < ctx->info.f_scu; i++)
        {
            PIC_REC(ctx)->map_mv[i][0][0] = ctx->map.map_mv[i][0][0];
            PIC_REC(ctx)->map_mv[i][0][1] = ctx->map.map_mv[i][0][1];
            PIC_REC(ctx)->map_mv[i][1][0] = ctx->map.map_mv[i][1][0];
            PIC_REC(ctx)->map_mv[i][1][1] = ctx->map.map_mv[i][1][1];
        }

#if 1 // SKIP_TEST
        int lcu_cnt = ctx->info.f_lcu;
        int max_smb_num = ctx->info.f_lcu;
        int pic_width = ctx->info.pic_width;
        int pic_height = ctx->info.pic_height;
        int pix_x = 0;
        int pix_y = 0;
        int lcu_x = 0;
        int lcu_y = 0;

        int pic_width_in_lcu = ctx->info.pic_width_in_lcu;
        int pic_height_in_lcu = ctx->info.pic_height_in_lcu;
        int lcu_chroma_col_nums = pic_width_in_lcu % 2 ? (pic_width_in_lcu / 2 + 1) : (pic_width_in_lcu / 2);
        int lcu_chroma_row_nums = pic_height_in_lcu % 2 ? (pic_height_in_lcu / 2 + 1) : (pic_height_in_lcu / 2);
        while (lcu_cnt > 0)
        {
            if (lcu_x == pic_width_in_lcu)
            {
                lcu_y++;
                lcu_x = 0;
            }
            lcu_cnt--;
            pix_x = lcu_x << ctx->info.log2_max_cuwh;
            pix_y = lcu_y << ctx->info.log2_max_cuwh;

            static SAO_2RTL_PARAM_T sao_2rtl_param;

            sao_2rtl_param.lcu_luma_col_nums = pic_width_in_lcu;
            sao_2rtl_param.lcu_luma_row_nums = pic_height_in_lcu;
            sao_2rtl_param.lcu_chroma_col_nums = lcu_chroma_col_nums;
            sao_2rtl_param.lcu_chroma_row_nums = lcu_chroma_row_nums;

            sao_2rtl_param.lcu_luma_pos_x = pix_x;
            sao_2rtl_param.lcu_luma_pos_y = pix_y;
            sao_2rtl_param.lcu_chroma_pos_x = pix_x >> 1;
            sao_2rtl_param.lcu_chroma_pos_y = pix_y >> 1;

            LCU_2RFC_PARAM_T lcu_2rfc_param;

            lcu_2rfc_param.lcu_use_left = (pix_x == 0) ? 0 : 1;
            lcu_2rfc_param.lcu_use_right = ((pix_x + 64) >= pic_width) ? 0 : 1;
            lcu_2rfc_param.lcu_use_up = (pix_y == 0) ? 0 : 1;
            lcu_2rfc_param.lcu_use_down = ((pix_y + 64) >= pic_height) ? 0 : 1;

            sao_output_rearrange_skip(max_smb_num, lcu_cnt, ctx->info.pic_width_in_lcu, pix_y,
                                      pix_x, pix_y >> 1, pix_x >> 1, ctx->pic[1]->stride_luma, ctx->pic[1]->stride_chroma, &lcu_2rfc_param, &sao_2rtl_param);
            rfc_run_skip(pix_y, pix_x, ctx->pic[1]->stride_luma, ctx->pic[1]->stride_chroma,
                         &lcu_2rfc_param, sao_pic_rec_test_y, sao_pic_rec_test_u, sao_pic_rec_test_v,
                         sao_pic_rec_test_y_last, sao_pic_rec_test_u_last, sao_pic_rec_test_v_last,
                         ctx->pic[1]->y, ctx->pic[1]->u, ctx->pic[1]->v,
                         ctx->pic[1]->y_rfcBin, ctx->pic[1]->y_rfcPrm, ctx->pic[1]->u_rfcBin, ctx->pic[1]->u_rfcPrm,
                         ctx->pic[1]->v_rfcBin, ctx->pic[1]->v_rfcPrm);
            lcu_x++;
        }
#endif
    }

    //*******************************************************************************************************

    // ofs.close();
#if PATCH_DEBUG1
    for (int i = 0; i < AEC_DATA_output.bs_cur; i++)
        bs->beg[i] = AEC_DATA_output.bs.beg[i];

    bs->cur = bs->beg + AEC_DATA_output.bs_cur;
#endif
    bs->code = 0;
    bs->leftbits = 32;
    if (ctx->patch->patch_sao_enable != NULL)
    {

        // printf("free patch_sao_enable=%p \n",patch_sao_enable);
        com_mfree(ctx->patch->patch_sao_enable);
    }

#if USE_IME_LINE
    if (me_line_map_ptr != NULL)
    {
        com_mfree(me_line_map_ptr);
    }
#endif
#if RATECONTROL
	if (frame_level_fw.EncControl == 1) {
		int curr_f_bits = COM_BSW_GET_WRITE_BYTE(bs) * 8 - frame_level_fw.prev_f_bits;
		Updata_RateControl(pRC, curr_f_bits, frame_level_fw.qp_y, frame_level_fw.slice_type, frame_level_fw.frame_id,
			frame_level_fw.gop_size);
		updata_ratecontrol_parameters(COM_BSW_GET_WRITE_BYTE(bs) * 8);

		frame_level_fw.prev_f_bits = COM_BSW_GET_WRITE_BYTE(bs) * 8;
	}
#endif
#if UMVE_ENH
    if (ctx->dataCol)
    {
        BOOL umveUsed = FALSE;
        u32 umveOffsetCountTotal = 0;
        for (u8 idx = 0; idx < UMVE_REFINE_STEP_SEC_SET; idx++)
        {
            if (ctx->umveOffsetPicCount[idx] > 0)
            {
                umveUsed = TRUE;
                umveOffsetCountTotal += ctx->umveOffsetPicCount[idx];
            }
        }

        if (umveUsed)
        {
            ctx->umveAveOffset = 0.0;
            for (u8 idx = 0; idx < UMVE_REFINE_STEP_SEC_SET; idx++)
            {
                double umveOffset = 0.25 * (1 << idx);
                double p = ctx->umveOffsetPicCount[idx] * 1.0 / umveOffsetCountTotal;
                ctx->umveAveOffset += umveOffset * p;
            }
        }
        else
        {
            ctx->umveAveOffset = -1;
        }
    }
#endif

#if USE_ROI_QP_CONFIG || USE_ROI_MODE_CONFIG
    if (ctx->info.useQPMAP)
        fclose(fetch_ctx->lcumap_in);
#endif
#if CUDQP_QP_MAP
    free(ctx->mapcuqp);
    // free(ctu_ctrl_input[ctx->die_idx].CuQPMap_Frame);
#endif
    return COM_OK;
}

void init_ctx_fetch(ENC_CTX *ctx, CTX_FETCH *fetch_ctx)
{

#if USE_ROI_QP_CONFIG || USE_ROI_MODE_CONFIG
    if (ctx->info.useQPMAP || ctx->info.useModeMAP)
    {
        fetch_ctx->lcumap_in = fopen("lcuMap", "rb");
        fetch_ctx->slice_qp = ctx->info.shext.slice_qp;
    }
#endif

    fetch_ctx->lcu_cnt = ctx->lcu_cnt;

    ///////////////ctx

    fetch_ctx->sample_adaptive_offset_enable_flag = ctx->info.sqh.sample_adaptive_offset_enable_flag;
    fetch_ctx->lcu_cnt = ctx->lcu_cnt;
    fetch_ctx->stride_luma = ctx->pic[1]->stride_luma;
    fetch_ctx->stride_chroma = ctx->pic[1]->stride_chroma;
    fetch_ctx->ctx_ptr = ctx->ptr;
    fetch_ctx->ctx_info_pic_width = ctx->info.pic_width;
    fetch_ctx->ctx_info_pic_height = ctx->info.pic_height;
    fetch_ctx->ctx_info_pic_width_in_lcu = ctx->info.pic_width_in_lcu;
    fetch_ctx->ctx_info_pic_height_in_lcu = ctx->info.pic_height_in_lcu;
    fetch_ctx->ctx_info_pic_width_in_scu = ctx->info.pic_width_in_scu;
    fetch_ctx->ctx_info_pic_height_in_scu = ctx->info.pic_height_in_scu;
    fetch_ctx->ctx_info_pic_header_chroma_quant_param_delta_cb = ctx->info.pic_header.chroma_quant_param_delta_cb;
    fetch_ctx->ctx_info_pic_header_chroma_quant_param_delta_cr = ctx->info.pic_header.chroma_quant_param_delta_cr;
    fetch_ctx->ctx_info_pic_header_slice_type = ctx->info.pic_header.slice_type;

    if (fetch_ctx->ctx_info_pic_header_slice_type != 1)
    {
        fetch_ctx->ctx_pinter_refp_pic_height_chroma = ctx->pinter.refp[0][0].pic->height_chroma;
        fetch_ctx->ctx_pinter_refp_pic_height_luma = ctx->pinter.refp[0][0].pic->height_luma;
        fetch_ctx->ctx_pinter_refp_pic_stride_chroma = ctx->pinter.refp[0][0].pic->stride_chroma;
        fetch_ctx->ctx_pinter_refp_pic_stride_luma = ctx->pinter.refp[0][0].pic->stride_luma;
        fetch_ctx->ctx_pinter_refp_pic_width_chroma = ctx->pinter.refp[0][0].pic->width_chroma;
        fetch_ctx->ctx_pinter_refp_pic_width_luma = ctx->pinter.refp[0][0].pic->width_luma;
    }

    if (IS_INTER_SLICE(ctx->info.pic_header.slice_type))
    {
        for (int i = 0; i < 17; i++)
        {
            for (int j = 0; j < 2; j++)
            {
                fetch_ctx->ctx_pinter_refp_ptr[i][j] = ctx->pinter.refp[i][j].ptr;
            }
        }
        fetch_ctx->ctx_ptr = ctx->ptr;
#if ENABLE_BFRAME
        fetch_ctx->ctx_rpm_num_refp[0] = ctx->rpm.num_refp[0];
        fetch_ctx->ctx_rpm_num_refp[1] = ctx->rpm.num_refp[1];

        if (fetch_ctx->ctx_info_pic_header_slice_type != 1)
        {
            for (int j = 0; j < REFP_NUM; j++)
            {
                for (int i = 0; i < ctx->rpm.num_refp[j]; i++)
                {
                    fetch_ctx->ctx_pinter_refp_pic_stride_chroma_all[i][j] = ctx->pinter.refp[i][j].pic->stride_chroma;
                    fetch_ctx->ctx_pinter_refp_pic_stride_luma_all[i][j] = ctx->pinter.refp[i][j].pic->stride_luma;
                    fetch_ctx->ctx_pinter_refp_pic_width_chroma_all[i][j] = ctx->pinter.refp[i][j].pic->width_chroma;
                    fetch_ctx->ctx_pinter_refp_pic_width_luma_all[i][j] = ctx->pinter.refp[i][j].pic->width_luma;
                }
            }
        }
#else
        fetch_ctx->ctx_rpm_num_refp[0] = ctx->rpm.num_refp[0];
        fetch_ctx->ctx_rpm_num_refp[1] = ctx->rpm.num_refp[1];
#endif
    }
    else
    {
        fetch_ctx->ctx_rpm_num_refp[0] = 0;
        fetch_ctx->ctx_rpm_num_refp[1] = 0;
    }

    // dbk_ref
    for (int i = 0; i < MAX_NUM_REF_PICS; i++)
    {
        for (int j = 0; j < REFP_NUM; j++)
        {
            if (ctx->refp[i][j].pic == 0)
            {
                fetch_ctx->ref_ptr_dbk[i][j] = -1;
            }
            else
            {
                fetch_ctx->ref_ptr_dbk[i][j] = ctx->refp[i][j].pic->ptr;
            }
        }
    }
    /**************************************************************************************************************************/
    fetch_ctx->use_deblock = ctx->param.use_deblock;
    fetch_ctx->affine_enable_flag = ctx->info.sqh.affine_enable_flag;
    fetch_ctx->slice_sao_enable[0] = ctx->info.shext.slice_sao_enable[0];
    fetch_ctx->slice_sao_enable[1] = ctx->info.shext.slice_sao_enable[1];
    fetch_ctx->slice_sao_enable[2] = ctx->info.shext.slice_sao_enable[2];
    fetch_ctx->umve_enable_flag = ctx->info.sqh.umve_enable_flag;
    fetch_ctx->num_of_hmvp_cand = ctx->info.sqh.num_of_hmvp_cand;

    fetch_ctx->amvr_enable_flag = ctx->info.sqh.amvr_enable_flag;

    fetch_ctx->emvr_enable_flag = ctx->info.sqh.emvr_enable_flag;

    fetch_ctx->dt_intra_enable_flag = ctx->info.sqh.dt_intra_enable_flag;
    fetch_ctx->position_based_transform_enable_flag = ctx->info.sqh.position_based_transform_enable_flag;
    fetch_ctx->tscpm_enable_flag = ctx->info.sqh.tscpm_enable_flag;
    fetch_ctx->ipf_enable_flag = ctx->info.sqh.ipf_enable_flag;
    fetch_ctx->sample_adaptive_offset_enable_flag = ctx->info.sqh.sample_adaptive_offset_enable_flag;
    // fetch_ctx->pic_width_in_scu = ctx->info.pic_width_in_scu;
    fetch_ctx->max_cuwh = ctx->info.max_cuwh;
    // fetch_ctx->pic_width = ctx->info.pic_width;
    // fetch_ctx->pic_height = ctx->info.pic_height;
    // fetch_ctx->slice_type = ctx->info.pic_header.slice_type;
    fetch_ctx->log2_culine = ctx->log2_culine;
    // fetch_ctx->pic_width_in_lcu = ctx->info.pic_width_in_lcu;
    fetch_ctx->lcu_total_cnt = ctx->info.pic_width_in_lcu * ctx->info.pic_height_in_lcu;

    // fetch_ctx->rpm_num_refp[0] = ctx->rpm.num_refp[0];
    // fetch_ctx->rpm_num_refp[1] = ctx->rpm.num_refp[1];

    fetch_ctx->slice_sao_enable[0] = ctx->info.shext.slice_sao_enable[0];
    fetch_ctx->slice_sao_enable[1] = ctx->info.shext.slice_sao_enable[1];
    fetch_ctx->slice_sao_enable[2] = ctx->info.shext.slice_sao_enable[2];
    fetch_ctx->umve_enable_flag = ctx->info.sqh.umve_enable_flag;
    fetch_ctx->num_of_hmvp_cand = ctx->info.sqh.num_of_hmvp_cand;

    fetch_ctx->amvr_enable_flag = ctx->info.sqh.amvr_enable_flag;

    fetch_ctx->emvr_enable_flag = ctx->info.sqh.emvr_enable_flag;

    fetch_ctx->dt_intra_enable_flag = ctx->info.sqh.dt_intra_enable_flag;
    fetch_ctx->position_based_transform_enable_flag = ctx->info.sqh.position_based_transform_enable_flag;
    fetch_ctx->tscpm_enable_flag = ctx->info.sqh.tscpm_enable_flag;
    fetch_ctx->ipf_enable_flag = ctx->info.sqh.ipf_enable_flag;
    fetch_ctx->sample_adaptive_offset_enable_flag = ctx->info.sqh.sample_adaptive_offset_enable_flag;
    // fetch_ctx->pic_width_in_scu = ctx->info.pic_width_in_scu;
    fetch_ctx->max_cuwh = ctx->info.max_cuwh;
    // fetch_ctx->pic_width = ctx->info.pic_width;
    // fetch_ctx->pic_height = ctx->info.pic_height;
    // fetch_ctx->slice_type = ctx->info.pic_header.slice_type;
    fetch_ctx->log2_culine = ctx->log2_culine;
    // fetch_ctx->pic_width_in_lcu = ctx->info.pic_width_in_lcu;
    fetch_ctx->lcu_total_cnt = ctx->info.pic_width_in_lcu * ctx->info.pic_height_in_lcu;

    // fetch_ctx->rpm_num_refp[0] = ctx->rpm.num_refp[0];
    // fetch_ctx->rpm_num_refp[1] = ctx->rpm.num_refp[1];

    fetch_ctx->fixed_slice_qp_flag = ctx->info.pic_header.fixed_picture_qp_flag;

    fetch_ctx->cu_delta_qp_flag = ctx->info.pic_header.cu_delta_qp_flag;
    fetch_ctx->ph_ists_enable_flag = ctx->info.pic_header.ph_ists_enable_flag;
    fetch_ctx->ph_epmc_model_flag = ctx->info.pic_header.ph_epmc_model_flag;
    fetch_ctx->ibc_flag = ctx->info.pic_header.ibc_flag;
    fetch_ctx->sp_pic_flag = ctx->info.pic_header.sp_pic_flag;
    fetch_ctx->evs_ubvs_pic_flag = ctx->info.pic_header.evs_ubvs_pic_flag;
    fetch_ctx->umve_set_flag = ctx->info.pic_header.umve_set_flag;
    fetch_ctx->ph_ipc_flag = ctx->info.pic_header.ph_ipc_flag;
    fetch_ctx->cu_qp_group_area_size = ctx->info.pic_header.cu_qp_group_area_size;
    // fetch_ctx->chroma_quant_param_delta_cb=ctx->info.pic_header.chroma_quant_param_delta_cb;
    // fetch_ctx->chroma_quant_param_delta_cr=ctx->info.pic_header.chroma_quant_param_delta_cr;
    fetch_ctx->slice_qp = ctx->info.shext.slice_qp;
    fetch_ctx->min_cu_size = ctx->info.sqh.min_cu_size;
    fetch_ctx->max_part_ratio = ctx->info.sqh.max_part_ratio;
    fetch_ctx->max_split_times = ctx->info.sqh.max_split_times;
    fetch_ctx->min_qt_size = ctx->info.sqh.min_qt_size;
    fetch_ctx->max_bt_size = ctx->info.sqh.max_bt_size;
    fetch_ctx->max_eqt_size = ctx->info.sqh.max_eqt_size;
    fetch_ctx->max_dt_size = ctx->info.sqh.max_dt_size;
    fetch_ctx->abvr_enable_flag = ctx->info.sqh.abvr_enable_flag;
    fetch_ctx->umve_enh_enable_flag = ctx->info.sqh.umve_enable_flag;
    fetch_ctx->awp_enable_flag = ctx->info.sqh.awp_enable_flag;
    fetch_ctx->awp_mvr_enable_flag = ctx->info.sqh.awp_mvr_enable_flag;
    fetch_ctx->etmvp_enable_flag = ctx->info.sqh.etmvp_enable_flag;
    fetch_ctx->affine_umve_enable_flag = ctx->info.sqh.affine_umve_enable_flag;
    fetch_ctx->smvd_enable_flag = ctx->info.sqh.smvd_enable_flag;
    fetch_ctx->bgc_enable_flag = ctx->info.sqh.bgc_enable_flag;
    fetch_ctx->interpf_enable_flag = ctx->info.sqh.interpf_enable_flag;
    fetch_ctx->ipc_enable_flag = ctx->info.sqh.ipc_enable_flag;
    fetch_ctx->num_of_mvap_cand = ctx->info.sqh.num_of_mvap_cand;
    fetch_ctx->num_of_hbvp_cand = ctx->info.sqh.num_of_hbvp_cand;
    fetch_ctx->enhance_tscpm_enable_flag = ctx->info.sqh.enhance_tscpm_enable_flag;
    fetch_ctx->pmc_enable_flag = ctx->info.sqh.pmc_enable_flag;
    fetch_ctx->iip_enable_flag = ctx->info.sqh.iip_enable_flag;
    fetch_ctx->sawp_enable_flag = ctx->info.sqh.sawp_enable_flag;
    fetch_ctx->sbt_enable_flag = ctx->info.sqh.sbt_enable_flag;
    fetch_ctx->ists_enable_flag = ctx->info.sqh.ists_enable_flag;
    fetch_ctx->srcc_enable_flag = ctx->info.sqh.srcc_enable_flag;
    fetch_ctx->eipm_enable_flag = ctx->info.sqh.eipm_enable_flag;
    fetch_ctx->st_chroma_enable_flag = ctx->info.sqh.st_chroma_enable_flag;
    fetch_ctx->est_enable_flag = ctx->info.sqh.est_enable_flag;
    fetch_ctx->log2_max_cuwh = ctx->info.log2_max_cuwh;
    fetch_ctx->bit_depth_input = ctx->info.bit_depth_input;
    fetch_ctx->qp_offset_bit_depth = ctx->info.qp_offset_bit_depth;
    // fetch_ctx->ptr=ctx->ptr;
    fetch_ctx->temporal_id = ctx->temporal_id;
    for (int i = 0; i < MAX_NUM_REF_PICS; i++)
    {
        for (int j = 0; j < REFP_NUM; j++)
        {
            fetch_ctx->refp_ptr[i][j] = ctx->refp[i][j].ptr;
        }
    }

    for (int i = 0; i < UMVE_REFINE_STEP_SEC_SET; i++)
    {
        fetch_ctx->umveOffsetPicCount[i] = ctx->umveOffsetPicCount[i];
    }

    for (int i = 0; i < 256; i++)
    {
        fetch_ctx->cu_delta_qp_lcu_map[i] = ctx->cu_delta_qp_lcu_map[i];
    }

    fetch_ctx->dataCol = ctx->dataCol;
    fetch_ctx->cons_pred_mode = ctx->cons_pred_mode;
    fetch_ctx->tree_status = ctx->tree_status;
    fetch_ctx->num_delta_qp = ctx->cu_qp_group.num_delta_qp;
    fetch_ctx->pred_qp = ctx->cu_qp_group.pred_qp;
    fetch_ctx->cu_qp_group_x = ctx->cu_qp_group.cu_qp_group_x;
    fetch_ctx->cu_qp_group_y = ctx->cu_qp_group.cu_qp_group_y;
    /**************************************************************************************************************************/
}

void fetch_ctx_set(
    ENC_CTX *ctx,
    U1 *flag_valid_map_mv,
    U1 *flag_valid_map_refi,
    U1 *flag_valid_list_ptr,
    U1 *flag_valid_map_mv_1,
    U1 *flag_valid_map_refi_1,
    U1 *flag_valid_list_ptr_1,
    // COM_REFP* ctx_pinter_refp_0,
    // COM_REFP* ctx_pinter_refp_1,
    // U32* list_ptr_0,
    // U32* list_ptr_1,
    U32 refp_ptr_test[MAX_NUM_REF_PICS][REFP_NUM],
    U1 is_library_picture[MAX_NUM_REF_PICS][REFP_NUM])
{
    *flag_valid_map_mv = 0;
    *flag_valid_map_refi = 0;
    *flag_valid_list_ptr = 0;

    *flag_valid_map_mv_1 = 0;
    *flag_valid_map_refi_1 = 0;
    *flag_valid_list_ptr_1 = 0;
    // if (ctx->pinter.refp[0][1].map_mv && ctx->pinter.refp[0][1].map_refi && ctx->pinter.refp[0][1].list_ptr)
    //{
    //	flag_valid = 1;
    // }
    if (ctx->pinter.refp[0][0].map_mv)
    {
        *flag_valid_map_mv = 1;
    }
    // printf("flag_valid_map_mv = %d\n", flag_valid_map_mv);
    if (ctx->pinter.refp[0][0].map_refi)
    {
        *flag_valid_map_refi = 1;
    }
    if (ctx->pinter.refp[0][0].list_ptr)
    {
        *flag_valid_list_ptr = 1;
    }

    if (ctx->pinter.refp[0][1].map_mv)
    {
        *flag_valid_map_mv_1 = 1;
    }
    if (ctx->pinter.refp[0][1].map_refi)
    {
        *flag_valid_map_refi_1 = 1;
    }
    if (ctx->pinter.refp[0][1].list_ptr)
    {
        *flag_valid_list_ptr_1 = 1;
    }

    for (U5 i = 0; i < MAX_NUM_REF_PICS; ++i)
    {
        refp_ptr_test[i][REFP_0] = ctx->pinter.refp[i][REFP_0].ptr;
        refp_ptr_test[i][REFP_1] = ctx->pinter.refp[i][REFP_1].ptr;

        is_library_picture[i][REFP_0] = ctx->pinter.refp[i][REFP_0].is_library_picture;
        is_library_picture[i][REFP_1] = ctx->pinter.refp[i][REFP_1].is_library_picture;
    }
}

COM_SBAC_CTX aec_ctx_phase2;
ENC_SBAC aec_sbac_local;
AEC_COM_SBAC_CTX aec_ctx;
AEC_output AEC_DATA_output;
#if PATCH_DEBUG
PATCH_INFO *patch;
int patch_cur_idx;
int patch_cur_lcu_x;
int patch_cur_lcu_y;
COM_BSW *bs_patch;
ENC_CTX *ctx_patch;
ENC_STAT *stat_global;
COM_PIC_HEADER *pic_header_g;
void enc_set_patch_idx(s8 *map_patch_idx, PATCH_INFO *patch, int pic_width, int pic_height);
u32 *map_scu_temp;
s8 (*map_refi_temp)[REFP_NUM];
s16 (*map_mv_temp)[REFP_NUM][MV_D];
u32 *map_cu_mode_temp;
int Demulate_start_pos;
ENC_ME_LINE_MAP *me_line_ptr_patch;
static void enc_sbac_init1(ENC_SBAC *sbac)
{
    sbac->range = 0x1FF;
    sbac->code = 0;
    sbac->left_bits = 23;
    sbac->pending_byte = 0;
    sbac->is_pending_byte = 0;
    sbac->stacked_ff = 0;
}
static void com_sbac_ctx_init1(AEC_COM_SBAC_CTX *sbac_ctx)
{
    int i;
    sbac_ctx->umve_flag = PROB_INIT;
    sbac_ctx->umve_base_idx[0] = PROB_INIT;
    sbac_ctx->umve_step_idx[0] = PROB_INIT;
    sbac_ctx->cons_mode[0] = PROB_INIT;
    sbac_ctx->ipf_flag[0] = PROB_INIT;
    sbac_ctx->mvp_from_hmvp_flag[0] = PROB_INIT;
    sbac_ctx->tb_split[0] = PROB_INIT;
    sbac_ctx->affine_flag[0] = PROB_INIT;
    sbac_ctx->smvd_flag[0] = PROB_INIT;
    sbac_ctx->sao_mode[0] = PROB_INIT;
    sbac_ctx->sao_offset[0] = PROB_INIT;
    sbac_ctx->alf_lcu_enable[0] = PROB_INIT;

    for (i = 0; i < 2; i++)
    {
        sbac_ctx->direct_flag[i] = PROB_INIT;
        sbac_ctx->umve_dir_idx[i] = PROB_INIT;
        sbac_ctx->affine_mvr_idx[i] = PROB_INIT;
        sbac_ctx->ctp_zero_flag[i] = PROB_INIT;
        for (int j = 0; j < 3; j++)
        {
            sbac_ctx->mvd[i][j] = PROB_INIT;
        }
    }
    for (i = 0; i < 3; i++)
    {
        sbac_ctx->inter_dir[i] = PROB_INIT;
        sbac_ctx->cbf[i] = PROB_INIT;
        sbac_ctx->split_mode[i] = PROB_INIT;
        sbac_ctx->refi[i] = PROB_INIT;
        sbac_ctx->sao_merge_flag[i] = PROB_INIT;
    }
    for (i = 0; i < 4; i++)
    {
        sbac_ctx->skip_flag[i] = PROB_INIT;
        sbac_ctx->mvr_idx[i] = PROB_INIT;
        sbac_ctx->affine_mrg_idx[i] = PROB_INIT;
        sbac_ctx->delta_qp[i] = PROB_INIT;
        sbac_ctx->split_flag[i] = PROB_INIT;
    }
    for (i = 0; i < 5; i++)
    {
        sbac_ctx->split_dir[i] = PROB_INIT;
    }
    for (i = 0; i < 6; i++)
    {
        sbac_ctx->pred_mode[i] = PROB_INIT;
        sbac_ctx->part_size[i] = PROB_INIT;
    }
    for (i = 0; i < 9; i++)
    {
        sbac_ctx->bt_split_flag[i] = PROB_INIT;
    }
    for (i = 0; i < 10; i++)
    {
        sbac_ctx->intra_dir[i] = PROB_INIT;
    }
    for (i = 0; i < 11; i++)
    {
        sbac_ctx->skip_idx_ctx[i] = PROB_INIT;
    }
    for (i = 0; i < 12; i++)
    {
        sbac_ctx->last1[i] = PROB_INIT;
    }
    for (i = 0; i < 22; i++)
    {
        sbac_ctx->last2[i] = PROB_INIT;
    }
    for (i = 0; i < 24; i++)
    {
        sbac_ctx->run[i] = PROB_INIT;
        sbac_ctx->run_rdoq[i] = PROB_INIT;
        sbac_ctx->level[i] = PROB_INIT;
    }
}
#endif
ENC_ME_NEB_INFO inner_nebs[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN];

#define IME_RUN_SIM 0                       // IME sim
#define COLLECT_IME_DATA (IME_RUN_SIM && 1) // collect IME data

#define FETCH_RUN_SIM 0                         // FETCH sim
#define COLLECT_FETCH_DATA (FETCH_RUN_SIM && 1) // collect IME data

extern COM_MTIME pic_icnt;
extern COM_MTIME pic_icnt_zhang;
#if PRINT_AEC_DATA
extern FILE *SPLIT_MODE_FILE;
extern FILE *resi_y;
extern FILE *resi_u;
extern FILE *resi_v;
extern FILE *IPM;
extern FILE *PREDMODE;
extern FILE *PARTSIZE;
extern FILE *NUM_NZ;
extern FILE *NUM_REFP;
extern FILE *REF;
extern FILE *MVD;
extern FILE *MVR_IDX;
extern FILE *SBAC_STAT;
extern FILE *AFFINE_FLAG;
extern FILE *UMVE_FLAG;
extern FILE *UMVE_IDX;
extern FILE *AFFINE_UMVE_FLAG;
extern FILE *AFFINE_UMVE_IDX0;
extern FILE *SKIP_IDX;
extern FILE *SBAC_STAT_OUT;
#endif
#if Debug_TWO_PASS
FILE *md_two_pass;
#endif // Debug_TWO_PASS

#if Debug_RMD_MODE
FILE *rmd_mode;
#endif

#if USE_TRACE_DUMP
FILE *f_TraceInfo = NULL;
void trace_dump(ENC_CU_DATA *dst, int pic_cnt, U3 slice_type, U12 pix_x, U12 pix_y)
{
    U8 i, ii, i1, i2;
    U8 j;
    U8 k;
    U8 cu_width;
    U8 cu_height;
    U8 cus;
    U8 cuw_scu;
    U8 cuh_scu;
    U8 cus_scu;
    // U14 cx;
    // U14 cy;
    U22 idx_dst;
    U22 idx_src;
    // cx = x >> 2;
    // cy = y >> 2;
    U3 cu_width_log2 = 6;
    U3 cu_height_log2 = 6;
    U3 log2_cus = 6;
    cu_width = (1 << cu_width_log2);
    cu_height = (1 << cu_height_log2);
    cus = 1 << log2_cus;
    cuw_scu = 1 << (cu_width_log2 - 2);
    cuh_scu = 1 << (cu_height_log2 - 2);
    cus_scu = 1 << (log2_cus - 2);

    if (f_TraceInfo == NULL)
        f_TraceInfo = fopen("TraceInfo.txt", "w");

    fprintf(f_TraceInfo, "PicCnt %d SliceType %d LCUCnt %d PosX %d PosY %d.\n", pic_cnt, slice_type, pix_x, pix_y);

    fprintf(f_TraceInfo, "===============SplitModeInfo=====================\n");
    for (j = 0; j < cuh_scu; j++)
    {
        idx_dst = (U22)((j) << (log2_cus - 2));
        idx_src = (U22)j << (cu_width_log2 - 2);
        for (k = 0; k < 5; k++)
        {
            fprintf(f_TraceInfo, "depth=%d\n", k);
            for (i = 0; i < (NUM_BLOCK_SHAPE); i++)
            {
                fprintf(f_TraceInfo, "shape=%d | ", i);
                for (ii = 0; ii < cuw_scu; ii++)
                {
                    fprintf(f_TraceInfo, "%d ", (dst->split_mode[k][i] + idx_dst)[ii]);
                }
                fprintf(f_TraceInfo, "\n");
            }
            fprintf(f_TraceInfo, "\n");
            fprintf(f_TraceInfo, "\n");
        }
    }
    fprintf(f_TraceInfo, "==================================================\n");

    fprintf(f_TraceInfo, "=====================PredModeInfo=====================\n");
    for (j = 0; j < cuh_scu; j++)
    {
        for (ii = 0; ii < cuw_scu; ii++)
        {
            fprintf(f_TraceInfo, "%d ", (dst->pred_mode + idx_dst)[ii]);
        }
        fprintf(f_TraceInfo, "\n");
    }
    fprintf(f_TraceInfo, "==================================================\n");

    fprintf(f_TraceInfo, "==================IPMInfo(luma,chroma)=============\n");
    for (j = 0; j < cuh_scu; j++)
    {
        for (ii = 0; ii < cuw_scu; ii++)
        {
            fprintf(f_TraceInfo, "(%d,%d) ", (dst->ipm[0] + idx_dst)[ii], (dst->ipm[1] + idx_dst)[ii]);
        }
        fprintf(f_TraceInfo, "\n");
    }
    fprintf(f_TraceInfo, "==================================================\n");

    fprintf(f_TraceInfo, "==================RefIndexInfo(list0,list1)====================\n");
    for (j = 0; j < cuh_scu; j++)
    {
        for (ii = 0; ii < cuw_scu; ii++)
        {
            fprintf(f_TraceInfo, "(%d,%d) ", (dst->refi + idx_dst)[ii][0], (dst->refi + idx_dst)[ii][1]);
        }
        fprintf(f_TraceInfo, "\n");
    }
    fprintf(f_TraceInfo, "==================================================\n");

    fprintf(f_TraceInfo, "==============MVInfo(sub0,sub1,sub2,sub3)=========\n");
    for (j = 0; j < cuh_scu; j++)
    {
        for (ii = 0; ii < cuw_scu; ii++)
        {
            for (i1 = 0; i1 < 2; i1++)
            {
                for (i2 = 0; i2 < 2; i2++)
                {
                    fprintf(f_TraceInfo, "(%d,%d,%d,%d) ", (dst->mv + idx_dst)[ii][0][0], (dst->mv + idx_dst)[ii][0][1], (dst->mv + idx_dst)[ii][1][0], (dst->mv + idx_dst)[ii][1][1]);
                }
            }
            fprintf(f_TraceInfo, "\n");
        }
    }
    fprintf(f_TraceInfo, "==================================================\n");
}

#endif

#if M5422
/****************************************************************************
 * pixel_var_wxh
 ****************************************************************************/
#define PIXEL_VAR_C(name, w, h, shift)                \
    uint64_t name(pel *pix, int i_stride)             \
    {                                                 \
        uint32_t sum = 0, sqr = 0;                    \
        for (int y = 0; y < h; y++)                   \
        {                                             \
            for (int x = 0; x < w; x++)               \
            {                                         \
                sum += pix[x];                        \
                sqr += pix[x] * pix[x];               \
            }                                         \
            pix += i_stride;                          \
        }                                             \
        return sqr - ((int64_t)sum * sum >> (shift)); \
    }

PIXEL_VAR_C(pixel_var_64x64, 64, 64, 12)

/****************************************************************************
 * pixel_var2_wxh
 ****************************************************************************/
#define PIXEL_VAR2_C(name, w, h, shift)               \
    int name(pel *fenc, pel *fdec, int dec_stride)    \
    {                                                 \
        int sum = 0, sqr = 0;                         \
        for (int y = 0; y < (h); y++)                 \
        {                                             \
            for (int x = 0; x < (w); x++)             \
            {                                         \
                int diff = fenc[x] - fdec[x];         \
                sum += diff;                          \
                sqr += diff * diff;                   \
            }                                         \
            fenc += FENC_STRIDE;                      \
            fdec += dec_stride;                       \
        }                                             \
        return sqr - ((int64_t)sum * sum >> (shift)); \
    }

PIXEL_VAR2_C(pixel_var2_64x64, 64, 64, 12)

#endif

SAO_2RTL_PARAM_T sao_2rtl_param;

#define STRM_MAX_LEN 2000000
typedef struct aec_t
{
    int Ebuffer;
    int Ecodestrm_len;
    int bits_add;
    unsigned char Ecodestrm[STRM_MAX_LEN];
} aec_t;

void arienco_start_encoding(aec_t *aec)
{
    aec->bits_add = 8;
    aec->Ebuffer = 0;
    aec->Ecodestrm_len = 0;
}

static __inline void write_byte(aec_t *aec, unsigned char byte)
{
    aec->Ebuffer |= (byte << aec->bits_add);
    if (!(aec->Ebuffer & 0xFFFFFC00))
    {
        aec->Ecodestrm[(aec->Ecodestrm_len)++] = 0x02;
        aec->Ebuffer |= 0x0800;
        aec->Ebuffer <<= 6;
        aec->bits_add -= 2;
        if (aec->bits_add == 0)
        {
            aec->Ecodestrm[(aec->Ecodestrm_len)++] = (unsigned char)((aec->Ebuffer >> 8) & 0xff);
            aec->Ebuffer <<= 8;
            aec->bits_add = 8;
        }
    }
    else
    {
        aec->Ecodestrm[(aec->Ecodestrm_len)++] = (unsigned char)((aec->Ebuffer >> 8) & 0xff);
        aec->Ebuffer <<= 8;
    }
}
 
int md_slice_qp; // for debug
int enc_pic(
#if PATCH_DIVIDE_ENABLE
    int multislice_mode,
#endif
    ENC_CTX *ctx,
    COM_BITB *bitb,
    ENC_STAT *stat,
    double ctu_64x64_ime_cost[2040])
{
#ifndef VU440
	int h2c_reg, h2c_mem, c2h_mem;
	if ((c2h_mem = open(FROM_DEVICE_NAME_DEFAULT, O_RDWR)) == -1)
		LOG_ERRPR("c2h device fail\n");
	if ((h2c_mem = open(TO_DEVICE_NAME_DEFAULT, O_RDWR)) == -1)
		LOG_ERRPR(" open h2c to fail\n");
	if ((h2c_reg = open(BYPASS_DEFAULT, O_RDWR | O_SYNC)) == -1)
		LOG_ERRPR("1 open to device fail\n");
	void* map;
	void *map_base;
    void *map_fetch2;
    void *map_fetch1;
    void *map_aec;
    void *map_md;


#endif

	ENC_ME_LINE_MAP* me_line_map_ptr;
	me_line_map_ptr = (ENC_ME_LINE_MAP*)com_malloc_fast(sizeof(ENC_ME_LINE_MAP));
	com_assert(me_line_map_ptr);


	ENC_CORE* core;
	COM_PIC_HEADER* pic_header = NULL;
	COM_CNKH    cnkh;
	s8* map_delta_qp = NULL;


    pic_init(ctx, stat, &cnkh, me_line_map_ptr);
 


    core = ctx->core;

    pic_cnt = ctx->poc;

#if CABAC_MULTI_PROB

    if (ctx->info.sqh.mcabac_enable_flag)
    {
        g_compatible_back = 0;
    }
    else
    {
        g_compatible_back = 1;
    }
#endif

    CTX_FETCH fetch_ctx = {0};
    U7 real_lcu_width;
    U7 real_lcu_height;
    U2 use_deblock;

    // md_input_ptr->md_slice_qp = ctx->info.pic_header.picture_qp;//for debug

    init_ctx_fetch(ctx, &fetch_ctx);
    int lcu = 0;
    fetch_ctx_set(
        ctx,
        &(fetch_ctx.flag_valid_map_mv),
        &(fetch_ctx.flag_valid_map_refi),
        &(fetch_ctx.flag_valid_list_ptr),
        &(fetch_ctx.flag_valid_map_mv_1),
        &(fetch_ctx.flag_valid_map_refi_1),
        &(fetch_ctx.flag_valid_list_ptr_1),
        (fetch_ctx.refp_ptr_test),
        (fetch_ctx.is_library_picture));

    // RDO with adaptive lambda solution basic info
    double min_ctu_64x64_ime_cost = 999999999999999;
    double max_ctu_64x64_ime_cost = -999999999999999;

    int sum_of_ctu = ctx->lcu_cnt;
    double avg_ctu_64x64_ime_cost = 0.0;
    for (int k = 0; k < sum_of_ctu; k++)
    {
        if (ctu_64x64_ime_cost[k] < min_ctu_64x64_ime_cost)
            min_ctu_64x64_ime_cost = ctu_64x64_ime_cost[k];
        if (ctu_64x64_ime_cost[k] > max_ctu_64x64_ime_cost)
            max_ctu_64x64_ime_cost = ctu_64x64_ime_cost[k];
        avg_ctu_64x64_ime_cost += ctu_64x64_ime_cost[k];
    }
    avg_ctu_64x64_ime_cost /= sum_of_ctu;
    double mid_lambda_factor_based_ime = 1;
    double min_diff_ctu_64x64_ime_cost = min_ctu_64x64_ime_cost - avg_ctu_64x64_ime_cost;
    double max_diff_ctu_64x64_ime_cost = max_ctu_64x64_ime_cost - avg_ctu_64x64_ime_cost;
    double min_lambda_factor_based_ime = 1.0;
    if (min_diff_ctu_64x64_ime_cost != 0)
        min_lambda_factor_based_ime = -0.693147181 / min_diff_ctu_64x64_ime_cost;
    double max_lambda_factor_based_ime = 1.0;
    if (max_diff_ctu_64x64_ime_cost != 0)
    {
        max_lambda_factor_based_ime = 2 * 0.693147181 / max_diff_ctu_64x64_ime_cost;
    }
    mid_lambda_factor_based_ime = (min_lambda_factor_based_ime + max_lambda_factor_based_ime) / 2;
    // double psi_based_ime_lut[2040] = { 0 };
    s64 lambda_lut_base_on_ime[2040] = {0}; // look-up table
    s64 orig_lambda = (s64)(ctx->lambda[0]);
    for (int k = 0; k < sum_of_ctu; k++)
    {
        double psi_based_ime = (1 / (1 + mid_lambda_factor_based_ime * (ctu_64x64_ime_cost[k] - avg_ctu_64x64_ime_cost) / 2 * 0.693147181));
        if (psi_based_ime > 2.0)
            psi_based_ime = 2.0;
        if (psi_based_ime < 0.5)
            psi_based_ime = 0.5;
        // psi_based_ime_lut[k] = psi_based_ime;
        // lambda_lut_base_on_ime[k] = (s64)((double)orig_lambda * psi_based_ime_lut[k]);
        lambda_lut_base_on_ime[k] = (s64)((double)orig_lambda * psi_based_ime);
    }

	STATIC_C LCUInput buffer;
	STATIC_C Arbitor ref;
	Instruction* mem_load = NULL;
	Instruction* mem_pu = NULL;
	Instruction* mem_store = NULL;
	int mem_load_int[8 * 256];
	int mem_pu_int[8 * 256];
	int mem_store_int[8 * 256];
	int first_lcu_flag = 1;
	struct timeval time, time_begin, time_end, reg_begin, reg_end, all_begin, all_end;

	memset(&time, 0, sizeof(struct timeval));
	memset(&time_begin, 0, sizeof(struct timeval));
	memset(&time_end, 0, sizeof(struct timeval));
	memset(&reg_begin, 0, sizeof(struct timeval));
	memset(&reg_end, 0, sizeof(struct timeval));
	memset(&all_begin, 0, sizeof(struct timeval));
	memset(&all_end, 0, sizeof(struct timeval));

	asm_qtqt(0, 0,
		fetch_ctx.log2_max_cuwh, fetch_ctx.log2_max_cuwh, fetch_ctx.ctx_info_pic_width, fetch_ctx.ctx_info_pic_height,
		fetch_ctx.log2_culine, fetch_ctx.min_qt_size,
		mem_load, mem_pu, mem_store);

    map_base = mmap(0, 4 * 1024, PROT_READ | PROT_WRITE, MAP_SHARED, h2c_reg, AVS_REG_BASE);
    map_fetch2 = mmap(0, 4 * 1024, PROT_READ | PROT_WRITE, MAP_SHARED, h2c_reg, AVS_REG_FETCH2_BASE);
    map_fetch1 = mmap(0, 4 * 1024, PROT_READ | PROT_WRITE, MAP_SHARED, h2c_reg, AVS_REG_FETCH1_BASE);
    map_aec = mmap(0, 4 * 1024, PROT_READ | PROT_WRITE, MAP_SHARED, h2c_reg, AVS_REG_AEC_BASE);
    map_md = mmap(0, 4 * 1024, PROT_READ | PROT_WRITE, MAP_SHARED, h2c_reg, AVS_REG_MD_BASE);
	if (!op_head_info_flag)
	{
		if (ctx->ptr >= SKIP_FRAME) {
			gettimeofday(&time, NULL);
			all_begin = time;
		//	all_end = time;
			while (ctx->lcu_cnt > 0) {
#if ONLY_QT_QT
				int asm_pix_x, asm_pix_y;
				Register mdreg;
				Register fetch2md_mdreg;
				int mdreg_sys[4], mdreg_cond0[32], mdreg_cond1[32], mdreg_cfg0[32], mdreg_cfg1[32];
				char reg_name[100];
				asm_pix_xy(&asm_pix_x, &asm_pix_y, &ctu_ctrl_input, &fetch_ctx);
				md_run_64_reg(asm_pix_x, asm_pix_y, mdreg);
#endif	

			// adaptive lambda
			int ctu_cnt = sum_of_ctu - ctx->lcu_cnt;
			s64 new_lambda_y = lambda_lut_base_on_ime[ctu_cnt];
			if (ctx->slice_type != 1) {
				// I slice lambda unchange
				//ctu_ctrl_input.lambda[0][0] = new_lambda_y & 0xffffffff;
				//ctu_ctrl_input.lambda[0][1] = (new_lambda_y >> 32) & 0xffffffff;
				ctu_ctrl_input.lambda[0] = new_lambda_y;
			}
#ifndef VU440

				
				unsigned int result = 0;
//				printf("wait write reg\n");
				while (1) {
					result = *((unsigned int*)map_base + AVS_IRQ_OFFSET);
					if (result == 0x00010000)
						break;
                    usleep(1);
				}
//				printf("wait write reg finish\n");
				gettimeofday(&time, NULL);
				reg_begin = time;
				time_end = time;
				printf("wait %d lcu done use %d sec %d usec\n", ctx->lcu_cnt, time_end.tv_sec - time_begin.tv_sec, time_end.tv_usec - time_begin.tv_usec);	

				if (ctu_ctrl_input.slice_enable) {
					write_reg32(map_fetch2, multislice_mode, MULTISLICE_MODE);
					write_reg32(map_fetch2, fetch_ctx.ctx_ptr, FETCH_CTX_IO_CTX_PTR);
					write_reg32(map_fetch2, fetch_ctx.ctx_pinter_refp_pic_height_chroma, FETCH_CTX_IO_CTX_PINTER_REFP_PIC_HEIGHT_CHROMA);
					write_reg32(map_fetch2, fetch_ctx.ctx_pinter_refp_pic_height_luma, FETCH_CTX_IO_CTX_PINTER_REFP_PIC_HEIGHT_LUMA);
					write_reg32(map_fetch2, fetch_ctx.ctx_pinter_refp_pic_stride_chroma, FETCH_CTX_IO_CTX_PINTER_REFP_PIC_STRIDE_CHROMA);
					write_reg32(map_fetch2, fetch_ctx.ctx_pinter_refp_pic_stride_luma, FETCH_CTX_IO_CTX_PINTER_REFP_PIC_STRIDE_LUMA);
					write_reg32(map_fetch2, fetch_ctx.ctx_pinter_refp_pic_width_chroma, FETCH_CTX_IO_CTX_PINTER_REFP_PIC_WIDTH_CHROMA);
					write_reg32(map_fetch2, fetch_ctx.ctx_pinter_refp_pic_width_luma, FETCH_CTX_IO_CTX_PINTER_REFP_PIC_WIDTH_LUMA);
					write_reg32(map_fetch2, fetch_ctx.ctx_info_pic_width, FETCH_CTX_IO_CTX_INFO_PIC_WIDTH);
					write_reg32(map_fetch2, fetch_ctx.ctx_info_pic_height, FETCH_CTX_IO_CTX_INFO_PIC_HEIGHT);
					write_reg32(map_fetch2, fetch_ctx.ctx_info_pic_width_in_lcu, FETCH_CTX_IO_CTX_INFO_PIC_WIDTH_IN_LCU);
					write_reg32(map_fetch2, fetch_ctx.ctx_info_pic_height_in_lcu, FETCH_CTX_IO_CTX_INFO_PIC_HEIGHT_IN_LCU);
					write_reg32(map_fetch2, fetch_ctx.ctx_info_pic_width_in_scu, FETCH_CTX_IO_CTX_INFO_PIC_WIDTH_IN_SCU);
					write_reg32(map_fetch2, fetch_ctx.ctx_info_pic_height_in_scu, FETCH_CTX_IO_CTX_INFO_PIC_HEIGHT_IN_SCU);
					write_reg32(map_fetch2, fetch_ctx.ctx_info_pic_header_chroma_quant_param_delta_cb, FETCH_CTX_IO_CTX_INFO_PIC_HEADER_CHROMA_QUANT_PARAM_DELTA_CB);
					write_reg32(map_fetch2, fetch_ctx.ctx_info_pic_header_chroma_quant_param_delta_cr, FETCH_CTX_IO_CTX_INFO_PIC_HEADER_CHROMA_QUANT_PARAM_DELTA_CR);
					write_reg32(map_fetch2, fetch_ctx.ctx_info_pic_header_slice_type, FETCH_CTX_IO_CTX_INFO_PIC_HEADER_SLICE_TYPE);
					write_reg32(map_fetch2, fetch_ctx.sample_adaptive_offset_enable_flag, FETCH_CTX_IO_SAMPLE_ADAPTIVE_OFFSET_ENABLE_FLAG);
					write_reg32(map_fetch2, fetch_ctx.stride_luma, FETCH_CTX_IO_STRIDE_LUMA);
					write_reg32(map_fetch2, fetch_ctx.stride_chroma, FETCH_CTX_IO_STRIDE_CHROMA);
					write_reg32(map_fetch2, fetch_ctx.lcu_cnt, FETCH_CTX_IO_LCU_CNT);
					write_reg32(map_fetch2, fetch_ctx.flag_valid_map_mv, FETCH_CTX_IO_FLAG_VALID_MAP_MV);
					write_reg32(map_fetch2, fetch_ctx.flag_valid_map_refi, FETCH_CTX_IO_FLAG_VALID_MAP_REFI);
					write_reg32(map_fetch2, fetch_ctx.flag_valid_list_ptr, FETCH_CTX_IO_FLAG_VALID_LIST_PTR);
					write_reg32(map_fetch2, fetch_ctx.flag_valid_map_mv_1, FETCH_CTX_IO_FLAG_VALID_MAP_MV_1);
					write_reg32(map_fetch2, fetch_ctx.flag_valid_map_refi_1, FETCH_CTX_IO_FLAG_VALID_MAP_REFI_1);
					write_reg32(map_fetch2, fetch_ctx.flag_valid_list_ptr_1, FETCH_CTX_IO_FLAG_VALID_LIST_PTR_1);
				//	write_reg32(map_fetch2, fetch_ctx.use_deblock, FETCH_CTX_IO_USE_DEBLOCK);
					write_reg32(map_fetch2, fetch_ctx.log2_culine, FETCH_CTX_IO_LOG2_CULINE);
					write_reg32(map_fetch2, fetch_ctx.max_cuwh, FETCH_CTX_IO_MAX_CUWH);
					write_reg32(map_fetch2, fetch_ctx.affine_enable_flag, FETCH_CTX_IO_AFFINE_ENABLE_FLAG);
					write_reg32(map_fetch2, fetch_ctx.umve_enable_flag, FETCH_CTX_IO_UMVE_ENABLE_FLAG);
					write_reg32(map_fetch2, fetch_ctx.num_of_hmvp_cand, FETCH_CTX_IO_NUM_OF_HMVP_CAND);
					write_reg32(map_fetch2, fetch_ctx.amvr_enable_flag, FETCH_CTX_IO_AMVR_ENABLE_FLAG);
					write_reg32(map_fetch2, fetch_ctx.emvr_enable_flag, FETCH_CTX_IO_EMVR_ENABLE_FLAG);
					write_reg32(map_fetch2, fetch_ctx.dt_intra_enable_flag, FETCH_CTX_IO_DT_INTRA_ENABLE_FLAG);
					write_reg32(map_fetch2, fetch_ctx.position_based_transform_enable_flag, FETCH_CTX_IO_POSITION_BASED_TRANSFORM_ENABLE_FLAG);
					write_reg32(map_fetch2, fetch_ctx.tscpm_enable_flag, FETCH_CTX_IO_TSCPM_ENABLE_FLAG);
					write_reg32(map_fetch2, fetch_ctx.ipf_enable_flag, FETCH_CTX_IO_IPF_ENABLE_FLAG);
					write_reg32(map_fetch2, fetch_ctx.fixed_slice_qp_flag, FETCH_CTX_IO_FIXED_SLICE_QP_FLAG);
					write_reg32(map_fetch2, fetch_ctx.cu_delta_qp_flag, FETCH_CTX_IO_CU_DELTA_QP_FLAG);
					write_reg32(map_fetch2, fetch_ctx.ph_ists_enable_flag, FETCH_CTX_IO_PH_ISTS_ENABLE_FLAG);
					write_reg32(map_fetch2, fetch_ctx.ph_epmc_model_flag, FETCH_CTX_IO_PH_EPMC_MODEL_FLAG);
					write_reg32(map_fetch2, fetch_ctx.ibc_flag, FETCH_CTX_IO_IBC_FLAG);
					write_reg32(map_fetch2, fetch_ctx.sp_pic_flag, FETCH_CTX_IO_SP_PIC_FLAG);
					write_reg32(map_fetch2, fetch_ctx.evs_ubvs_pic_flag, FETCH_CTX_IO_EVS_UBVS_PIC_FLAG);
					write_reg32(map_fetch2, fetch_ctx.umve_set_flag, FETCH_CTX_IO_UMVE_SET_FLAG);
					write_reg32(map_fetch2, fetch_ctx.ph_ipc_flag, FETCH_CTX_IO_PH_IPC_FLAG);
					write_reg32(map_fetch2, fetch_ctx.cu_qp_group_area_size, FETCH_CTX_IO_CU_QP_GROUP_AREA_SIZE);
					write_reg32(map_fetch2, fetch_ctx.slice_qp, FETCH_CTX_IO_SLICE_QP);
			//		write_reg32(map_fetch2, 40, FETCH_CTX_IO_SLICE_QP);
					write_reg32(map_fetch2, fetch_ctx.min_cu_size, FETCH_CTX_IO_MIN_CU_SIZE);
					write_reg32(map_fetch2, fetch_ctx.max_part_ratio, FETCH_CTX_IO_MAX_PART_RATIO);
					write_reg32(map_fetch2, fetch_ctx.max_split_times, FETCH_CTX_IO_MAX_SPLIT_TIMES);
					write_reg32(map_fetch2, fetch_ctx.min_qt_size, FETCH_CTX_IO_MIN_QT_SIZE);
					write_reg32(map_fetch2, fetch_ctx.max_bt_size, FETCH_CTX_IO_MAX_BT_SIZE);
					write_reg32(map_fetch2, fetch_ctx.max_eqt_size, FETCH_CTX_IO_MAX_EQT_SIZE);
					write_reg32(map_fetch2, fetch_ctx.max_dt_size, FETCH_CTX_IO_MAX_DT_SIZE);
					write_reg32(map_fetch2, fetch_ctx.abvr_enable_flag, FETCH_CTX_IO_ABVR_ENABLE_FLAG);
					write_reg32(map_fetch2, fetch_ctx.umve_enh_enable_flag, FETCH_CTX_IO_UMVE_ENH_ENABLE_FLAG);
					write_reg32(map_fetch2, fetch_ctx.awp_enable_flag, FETCH_CTX_IO_AWP_ENABLE_FLAG);
					write_reg32(map_fetch2, fetch_ctx.awp_mvr_enable_flag, FETCH_CTX_IO_AWP_MVR_ENABLE_FLAG);
					write_reg32(map_fetch2, fetch_ctx.etmvp_enable_flag, FETCH_CTX_IO_ETMVP_ENABLE_FLAG);
					write_reg32(map_fetch2, fetch_ctx.affine_umve_enable_flag, FETCH_CTX_IO_AFFINE_UMVE_ENABLE_FLAG);
					write_reg32(map_fetch2, fetch_ctx.smvd_enable_flag, FETCH_CTX_IO_SMVD_ENABLE_FLAG);
					write_reg32(map_fetch2, fetch_ctx.bgc_enable_flag, FETCH_CTX_IO_BGC_ENABLE_FLAG);
				//	write_reg32(map_fetch2, fetch_ctx.interpf_enable_flag, FETCH_CTX_IO_INTERPF_ENABLE_FLAG);
					write_reg32(map_fetch2, fetch_ctx.ipc_enable_flag, FETCH_CTX_IO_IPC_ENABLE_FLAG);
					write_reg32(map_fetch2, fetch_ctx.num_of_mvap_cand, FETCH_CTX_IO_NUM_OF_MVAP_CAND);
					write_reg32(map_fetch2, fetch_ctx.num_of_hbvp_cand, FETCH_CTX_IO_NUM_OF_HBVP_CAND);
					write_reg32(map_fetch2, fetch_ctx.enhance_tscpm_enable_flag, FETCH_CTX_IO_ENHANCE_TSCPM_ENABLE_FLAG);
					write_reg32(map_fetch2, fetch_ctx.pmc_enable_flag, FETCH_CTX_IO_PMC_ENABLE_FLAG);
					write_reg32(map_fetch2, fetch_ctx.iip_enable_flag, FETCH_CTX_IO_IIP_ENABLE_FLAG);
					write_reg32(map_fetch2, fetch_ctx.sawp_enable_flag, FETCH_CTX_IO_SAWP_ENABLE_FLAG);
					write_reg32(map_fetch2, fetch_ctx.sbt_enable_flag, FETCH_CTX_IO_SBT_ENABLE_FLAG);
					write_reg32(map_fetch2, fetch_ctx.ists_enable_flag, FETCH_CTX_IO_ISTS_ENABLE_FLAG);
					write_reg32(map_fetch2, fetch_ctx.srcc_enable_flag, FETCH_CTX_IO_SRCC_ENABLE_FLAG);
					write_reg32(map_fetch2, fetch_ctx.eipm_enable_flag, FETCH_CTX_IO_EIPM_ENABLE_FLAG);
					write_reg32(map_fetch2, fetch_ctx.st_chroma_enable_flag, FETCH_CTX_IO_ST_CHROMA_ENABLE_FLAG);
					write_reg32(map_fetch2, fetch_ctx.est_enable_flag, FETCH_CTX_IO_EST_ENABLE_FLAG);
					write_reg32(map_fetch2, fetch_ctx.log2_max_cuwh, FETCH_CTX_IO_LOG2_MAX_CUWH);
					write_reg32(map_fetch2, fetch_ctx.bit_depth_input, FETCH_CTX_IO_BIT_DEPTH_INPUT);
					write_reg32(map_fetch2, fetch_ctx.qp_offset_bit_depth, FETCH_CTX_IO_QP_OFFSET_BIT_DEPTH);
					write_reg32(map_fetch2, fetch_ctx.temporal_id, FETCH_CTX_IO_TEMPORAL_ID);
					write_reg32(map_fetch2, fetch_ctx.dataCol, FETCH_CTX_IO_DATACOL);
					write_reg32(map_fetch2, fetch_ctx.cons_pred_mode, FETCH_CTX_IO_CONS_PRED_MODE);
					write_reg32(map_fetch2, fetch_ctx.tree_status, FETCH_CTX_IO_TREE_STATUS);
					write_reg32(map_fetch2, fetch_ctx.num_delta_qp, FETCH_CTX_IO_NUM_DELTA_QP);
					write_reg32(map_fetch2, fetch_ctx.pred_qp, FETCH_CTX_IO_PRED_QP);
			//		write_reg32(map_fetch2, 40, FETCH_CTX_IO_PRED_QP);
					write_reg32(map_fetch2, fetch_ctx.cu_qp_group_x, FETCH_CTX_IO_CU_QP_GROUP_X);
					write_reg32(map_fetch2, fetch_ctx.cu_qp_group_y, FETCH_CTX_IO_CU_QP_GROUP_Y);
					write_reg32(map_fetch2, ctx->lcu_cnt, CTX_LCU_CNT);
					write_reg32(map_fetch2, fetch_ctx.ctx_rpm_num_refp[0], CTX_RPM_NUM_REFP_0);
					write_reg32(map_fetch2, fetch_ctx.ctx_rpm_num_refp[1], CTX_RPM_NUM_REFP_1);
					write_reg32(map_fetch2, ctu_ctrl_input.f_lcu, CTU_CTRL_INPUT_F_LCU);
				}

			//	printf("ctx->lcu_cnt = %d \n", ctx->lcu_cnt);

				write_reg32(map_fetch2, ctu_ctrl_input.slice_enable, CTU_CTRL_INPUT_SLICE_ENABLE);
				if (ctu_ctrl_input.slice_enable) {
					write_reg32(map_fetch2, ctu_ctrl_input.qp_y, CTU_CTRL_INPUT_QP_Y);
					write_reg32(map_fetch2, ctu_ctrl_input.qp_u, CTU_CTRL_INPUT_QP_U);
					write_reg32(map_fetch2, ctu_ctrl_input.qp_v, CTU_CTRL_INPUT_QP_V);
			//		write_reg32(map_fetch2, 40, CTU_CTRL_INPUT_QP_Y);
			//		write_reg32(map_fetch2, 40, CTU_CTRL_INPUT_QP_U);
			//		write_reg32(map_fetch2, 40, CTU_CTRL_INPUT_QP_V);

					write_reg32(map_fetch2, ctu_ctrl_input.lambda_mv, CTU_CTRL_INPUT_LAMBDA_MV);
					write_reg32(map_fetch2, ctu_ctrl_input.mipf_enable_flag, CTU_CTRL_INPUT_MIPF_ENABLE_FLAG);
					write_reg32(map_fetch2, ctu_ctrl_input.bit_depth_internal, CTU_CTRL_INPUT_BIT_DEPTH_INTERNAL);
					write_reg32(map_fetch2, ctu_ctrl_input.depth, CTU_CTRL_INPUT_DEPTH);
					write_reg32(map_fetch2, ctu_ctrl_input.cu_dqp_enable, CTU_CTRL_INPUT_CU_DQP_ENABLE);
					write_reg32(map_fetch2, ctu_ctrl_input.useCuQPMAP, CTU_CTRL_INPUT_USECUQPMAP);
					write_reg32(map_fetch2, Fetch_input_video_U8_ptr.code_stru, CODE_STRU);

				//	write_reg32(map_fetch2, frame_level_fw.chroma_quant_param_delta_cb, FRAME_LEVEL_FW_CHROMA_QUANT_PARAM_DELTA_CB);
					write_reg32(map_fetch2, frame_level_fw.qp_offset_bit_depth, FRAME_LEVEL_FW_QP_OFFSET_BIT_DEPTH);
				//	write_reg32(map_fetch2, frame_level_fw.chroma_quant_param_delta_cr, FRAME_LEVEL_FW_CHROMA_QUANT_PARAM_DELTA_CR);
				//	write_reg32(map_fetch2, frame_level_fw.fixed_slice_qp_flag, FRAME_LEVEL_FW_FIXED_SLICE_QP_FLAG);
					write_reg32(map_fetch2, frame_level_fw.pic_width, FRAME_LEVEL_FW_PIC_WIDTH);
					write_reg32(map_fetch2, frame_level_fw.pic_height, FRAME_LEVEL_FW_PIC_HEIGHT);
				}

				memcpy(mdreg_sys, &mdreg._sys, 16);
				if (ctu_ctrl_input.slice_enable) {
					memcpy(mdreg_cond0, &mdreg._cond0, 128);
					memcpy(mdreg_cond1, &mdreg._cond1, 128);
					memcpy(mdreg_cfg0, &mdreg._cfg0, 128);
					memcpy(mdreg_cfg1, &mdreg._cfg1, 128);
				}
				int tmp;
				for (tmp = 0; tmp < 4; tmp++) {
					write_reg32(map_fetch2, mdreg_sys[tmp], MDREG_SYS_V_1 + tmp);
				}

				//lcu flag 
				if (ctu_ctrl_input.slice_enable) {
					for (tmp = 0; tmp < 32; tmp++) {
						write_reg32(map_fetch2, mdreg_cond0[tmp], MDREG_COND0_V_1 + tmp);
					}
					for (tmp = 0; tmp < 32; tmp++) {
						write_reg32(map_fetch2, mdreg_cond1[tmp], MDREG_COND1_V_1 + tmp);
					}
					for (tmp = 0; tmp < 32; tmp++) {
						write_reg32(map_fetch2, mdreg_cfg0[tmp], MDREG_CFG0_V_1 + tmp);
					}
					for (tmp = 0; tmp < 32; tmp++) {
						write_reg32(map_fetch2, mdreg_cfg1[tmp], MDREG_CFG1_V_1 + tmp);
					}
				}
				/*
				map = mmap(0, 4 * 1024, PROT_READ | PROT_WRITE, MAP_SHARED, h2c_reg, AVS_REG_FETCH1_BASE);
				unsigned long long addr_0 = 0x80000000;
				write_reg32(map, addr_0, FETCH_CTX_IO_CTX_PINTER_REFP_PTR_OFFSET);
				unsigned long long addr_1 = addr_0 + 17 * 2 * sizeof(fetch_ctx.ctx_pinter_refp_ptr);
				write_reg32(map, addr_1, FETCH_CTX_IO_REF_PTR_DBK_OFFSET);
				unsigned long long addr_2 = addr_1 + 17 * 2 * sizeof(fetch_ctx.ref_ptr_dbk);
				write_reg32(map, addr_2, FETCH_CTX_IO_REFP_PTR_TEST_OFFSET);
				unsigned long long addr_3 = addr_2 + 17 * 2 * sizeof(fetch_ctx.refp_ptr_test);
				write_reg32(map, addr_3, FETCH_CTX_IO_IS_LIBRARY_PICTURE_OFFSET);
				unsigned long long addr_4 = addr_3 + 17 * 2 * sizeof(fetch_ctx.is_library_picture);
				write_reg32(map, addr_4, FETCH_CTX_IO_SLICE_SAO_ENABLE_OFFSET);
				unsigned long long addr_5 = addr_4 + 4 * sizeof(fetch_ctx.slice_sao_enable);
				write_reg32(map, addr_5, FETCH_CTX_IO_REFP_PTR_OFFSET);
				unsigned long long addr_6 = addr_5 + 17 * 2 * sizeof(fetch_ctx.refp_ptr);
				write_reg32(map, addr_6, FETCH_CTX_IO_UMVEOFFSETPICCOUNT_OFFSET);
				unsigned long long addr_7 = addr_6 + 8 * sizeof(fetch_ctx.umveOffsetPicCount);
				write_reg32(map, addr_7, FETCH_CTX_IO_CU_DELTA_QP_LCU_MAP_OFFSET);
				unsigned long long addr_8 = addr_7 + 256 * sizeof(fetch_ctx.cu_delta_qp_lcu_map);
				write_reg32(map, addr_8, MAP_MV_OFFSET);
				unsigned long long addr_9 = addr_8 + sizeof(ctx->map.map_mv) * ctx->info.f_scu * 2 * 2;
				write_reg32(map, addr_9, MAP_REFI_OFFSET);
				unsigned long long addr_10 = addr_9 + sizeof(ctx->map.map_refi) * ctx->info.f_scu * 2;
				write_reg32(map, addr_10, MAP_SCU_OFFSET);
				unsigned long long addr_11 = addr_10 + sizeof(ctx->map.map_scu) * ctx->info.f_scu;
				write_reg32(map, addr_11, MAP_CU_MODE_OFFSET);
				unsigned long long addr_12 = addr_11 + sizeof(ctx->map.map_cu_mode) * ctx->info.f_scu;
				write_reg32(map, addr_12, CTU_CTRL_INPUT_LAMBDA_OFFSET);
				unsigned long long addr_13 = addr_12 + sizeof(ctu_ctrl_input.lambda) * 3;
				write_reg32(map, addr_13, CTU_CTRL_INPUT_DIST_CHROMA_WEIGHT_OFFSET);
				unsigned long long addr_14 = addr_13 + sizeof(ctu_ctrl_input.dist_chroma_weight) * 2 * 2;
				write_reg32(map, addr_14, CTU_CTRL_INPUT_CUQPMAP_OFFSET);
				unsigned long long addr_15 = addr_14 + sizeof(ctu_ctrl_input.CuQPMap) * 256;
				write_reg32(map, addr_15, CTU_CTRL_INPUT_CUQPMAP_FRAME_OFFSET);
				unsigned long long addr_16 = addr_15 + sizeof(ctu_ctrl_input.CuQPMap_Frame) * 1920 * 1080;
				write_reg32(map, addr_16, IMGY_ORG_OFFSET);
				unsigned long long addr_17 = addr_16 + sizeof(Fetch_input_video.imgY_org) * ctx->info.pic_width * ctx->info.pic_height;
				write_reg32(map, addr_17, IMGU_ORG_OFFSET);
				unsigned long long addr_18 = addr_17 + sizeof(Fetch_input_video.imgU_org) * ctx->info.pic_width * ctx->info.pic_height / 4;
				write_reg32(map, addr_18, IMGV_ORG_OFFSET);
				unsigned long long addr_19 = addr_18 + sizeof(Fetch_input_video.imgV_org) * ctx->info.pic_width * ctx->info.pic_height / 4;
				write_reg32(map, addr_19, CTX_PINTER_REFP_MAP_MV_0_OFFSET);
				unsigned long long addr_20 = addr_19 + sizeof(ctx->pinter.refp[0][0].map_mv) * ctx->info.f_scu * 2 * 2;
				write_reg32(map, addr_20, CTX_PINTER_REFP_MAP_MV_1_OFFSET);
				unsigned long long addr_21 = addr_20 + sizeof(ctx->pinter.refp[0][1].map_mv) * ctx->info.f_scu * 2 * 2;
				write_reg32(map, addr_21, CTX_PINTER_REFP_MAP_REFI_0_OFFSET);
				unsigned long long addr_22 = addr_21 + sizeof(ctx->pinter.refp[0][0].map_refi) * ctx->info.f_scu * 2;
				write_reg32(map, addr_22, CTX_PINTER_REFP_MAP_REFI_1_OFFSET);
				unsigned long long addr_23 = addr_22 + sizeof(ctx->pinter.refp[0][1].map_refi) * ctx->info.f_scu * 2;
				write_reg32(map, addr_23, LIST_PTR_0_OFFSET);
				unsigned long long addr_24 = addr_23 + sizeof(ctx->pinter.refp[0][0].list_ptr) * 34;
				write_reg32(map, addr_24, LIST_PTR_1_OFFSET);
				if (munmap(map, 4 * 1024) == -1)
					LOG_ERRPR("2 munmap fail\n");

				memcpy(mem_load_int, mem_load, 32 * 256);
				memcpy(mem_pu_int, mem_pu, 32 * 256);
				memcpy(mem_store_int, mem_store, 32 * 256);
				map = mmap(0, 4 * 1024, PROT_READ | PROT_WRITE, MAP_SHARED, h2c_reg, AVS_REG_MD_BASE);
				unsigned long long addr_25 = addr_24 + sizeof(ctx->pinter.refp[0][1].list_ptr) * 34;
				write_reg32(map, addr_25, MEM_LOAD_BUF_V_OFFSET);
				unsigned long long addr_26 = addr_25 + sizeof(mem_load_int) * 8 * 256;
				write_reg32(map, addr_26, MEM_PU_BUF_V_OFFSET);
				unsigned long long addr_27 = addr_26 + sizeof(mem_pu_int) * 8 * 256;
				write_reg32(map, addr_27, MEM_STORE_BUF_V_OFFSET);
				if (munmap(map, 4 * 1024) == -1)
					LOG_ERRPR("3 munmap fail\n");
				
*/

				if (ctu_ctrl_input.slice_enable) {
					/*
					unsigned long long addr_0 = 0x80000000;
					write_reg32(map, addr_0, FETCH_CTX_IO_CTX_PINTER_REFP_PTR_OFFSET);
					unsigned long long addr_1 = addr_0 + 17 * 2 * 4;
					if (addr_1 % 32) {
						addr_1 = (addr_1 / 32 + 1) * 32;
					}
					write_reg32(map, addr_1, FETCH_CTX_IO_REF_PTR_DBK_OFFSET);
					unsigned long long addr_2 = addr_1 + 17 * 2 * 4;
					if (addr_2 % 32) {
						addr_2 = (addr_2 / 32 + 1) * 32;
					}
					write_reg32(map, addr_2, FETCH_CTX_IO_REFP_PTR_TEST_OFFSET);
					unsigned long long addr_3 = addr_2 + 17 * 2 * 4;
					if (addr_3 % 32) {
						addr_3 = (addr_3 / 32 + 1) * 32;
					}
					write_reg32(map, addr_3, FETCH_CTX_IO_IS_LIBRARY_PICTURE_OFFSET);
					unsigned long long addr_4 = addr_3 + 17 * 2 * 1;
					if (addr_4 % 32) {
						addr_4 = (addr_4 / 32 + 1) * 32;
					}
					write_reg32(map, addr_4, FETCH_CTX_IO_SLICE_SAO_ENABLE_OFFSET);
					unsigned long long addr_5 = addr_4 + 4 * 1;
					if (addr_5 % 32) {
						addr_5 = (addr_5 / 32 + 1) * 32;
					}
					write_reg32(map, addr_5, FETCH_CTX_IO_REFP_PTR_OFFSET);
					unsigned long long addr_6 = addr_5 + 17 * 2 * 4;
					if (addr_6 % 32) {
						addr_6 = (addr_6 / 32 + 1) * 32;
					}
					write_reg32(map, addr_6, FETCH_CTX_IO_UMVEOFFSETPICCOUNT_OFFSET);
					unsigned long long addr_7 = addr_6 + 8 * 4;
					if (addr_7 % 32) {
						addr_7 = (addr_7 / 32 + 1) * 32;
					}
					write_reg32(map, addr_7, FETCH_CTX_IO_CU_DELTA_QP_LCU_MAP_OFFSET);
					unsigned long long addr_8 = addr_7 + 256 * 4;
					if (addr_8 % 32) {
						addr_8 = (addr_8 / 32 + 1) * 32;
					}
					write_reg32(map, addr_8, MAP_MV_OFFSET);
					unsigned long long addr_9 = addr_8 + 4 * ctx->info.f_scu * 2 * 2;
					if (addr_9 % 32) {
						addr_9 = (addr_9 / 32 + 1) * 32;
					}
					write_reg32(map, addr_9, MAP_REFI_OFFSET);
					unsigned long long addr_10 = addr_9 + 4 * ctx->info.f_scu * 2;
					if (addr_10 % 32) {
						addr_10 = (addr_10 / 32 + 1) * 32;
					}
					write_reg32(map, addr_10, MAP_SCU_OFFSET);
					unsigned long long addr_11 = addr_10 + 4 * ctx->info.f_scu;
					if (addr_11 % 32) {
						addr_11 = (addr_11 / 32 + 1) * 32;
					}
					write_reg32(map, addr_11, MAP_CU_MODE_OFFSET);
					unsigned long long addr_12 = addr_11 + 4 * ctx->info.f_scu;
					if (addr_12 % 32) {
						addr_12 = (addr_12 / 32 + 1) * 32;
					}
					write_reg32(map, addr_12, CTU_CTRL_INPUT_LAMBDA_OFFSET);
					unsigned long long addr_13 = addr_12 + 4 * 3;
					if (addr_13 % 32) {
						addr_13 = (addr_13 / 32 + 1) * 32;
					}
					write_reg32(map, addr_13, CTU_CTRL_INPUT_DIST_CHROMA_WEIGHT_OFFSET);
					unsigned long long addr_14 = addr_13 + 4 * 2 * 2;
					if (addr_14 % 32) {
						addr_14 = (addr_14 / 32 + 1) * 32;
					}
					write_reg32(map, addr_14, CTU_CTRL_INPUT_CUQPMAP_OFFSET);
					unsigned long long addr_15 = addr_14 + 4 * 256;
					if (addr_15 % 32) {
						addr_15 = (addr_15 / 32 + 1) * 32;
					}
					write_reg32(map, addr_15, CTU_CTRL_INPUT_CUQPMAP_FRAME_OFFSET);
					*/
					//unsigned long long addr_16 = addr_15 + 4 * 1920 * 1080;
					unsigned long long addr_16 = 0x90000000;
					if (addr_16 % 32) {
						addr_16 = (addr_16 / 32 + 1) * 32;
					}
					write_reg32(map_fetch1, addr_16, IMGY_ORG_OFFSET);
					unsigned long long addr_17 = addr_16 + ctx->info.pic_width * ctx->info.pic_height;
					if (addr_17 % 32) {
						addr_17 = (addr_17 / 32 + 1) * 32;
					}
					write_reg32(map_fetch1, addr_17, IMGU_ORG_OFFSET);
					unsigned long long addr_18 = addr_17 + ctx->info.pic_width * ctx->info.pic_height / 4;
					if (addr_18 % 32) {
						addr_18 = (addr_18 / 32 + 1) * 32;
					}
					write_reg32(map_fetch1, addr_18, IMGV_ORG_OFFSET);
					//unsigned long long addr_19 = addr_18 + 2 * ctx->info.pic_width * ctx->info.pic_height / 4;
					/*
					unsigned long long addr_19 = addr_15 + 4 * 1920 * 1080 ;
					if (addr_19 % 32) {
						addr_19 = (addr_19 / 32 + 1) * 32;
					}
					write_reg32(map, addr_19, CTX_PINTER_REFP_MAP_MV_0_OFFSET);
					unsigned long long addr_20 = addr_19 + 4 * ctx->info.f_scu * 2 * 2;
					if (addr_20 % 32) {
						addr_20 = (addr_20 / 32 + 1) * 32;
					}
					write_reg32(map, addr_20, CTX_PINTER_REFP_MAP_MV_1_OFFSET);
					unsigned long long addr_21 = addr_20 + 4 * ctx->info.f_scu * 2 * 2;
					if (addr_21 % 32) {
						addr_21 = (addr_21 / 32 + 1) * 32;
					}
					write_reg32(map, addr_21, CTX_PINTER_REFP_MAP_REFI_0_OFFSET);
					unsigned long long addr_22 = addr_21 + 4 * ctx->info.f_scu * 2;
					if (addr_22 % 32) {
						addr_22 = (addr_22 / 32 + 1) * 32;
					}
					write_reg32(map, addr_22, CTX_PINTER_REFP_MAP_REFI_1_OFFSET);
					unsigned long long addr_23 = addr_22 + 4 * ctx->info.f_scu * 2;
					if (addr_23 % 32) {
						addr_23 = (addr_23 / 32 + 1) * 32;
					}
					write_reg32(map, addr_23, LIST_PTR_0_OFFSET);
					unsigned long long addr_24 = addr_23 + 4 * 34;
					if (addr_24 % 32) {
						addr_24 = (addr_24 / 32 + 1) * 32;
					}
					write_reg32(map, addr_24, LIST_PTR_1_OFFSET);
					if (munmap(map, 4 * 1024) == -1)
						LOG_ERRPR("2 munmap fail\n");
					*/
					memcpy(mem_load_int, mem_load, 32 * 256);
					memcpy(mem_pu_int, mem_pu, 32 * 256);
					memcpy(mem_store_int, mem_store, 32 * 256);

					unsigned long long addr_25 = 0x80000000 + 4 * 34;
					if (addr_25 % 32) {
						addr_25 = (addr_25 / 32 + 1) * 32;
					}
					write_reg32(map_md, addr_25, MEM_LOAD_BUF_V_OFFSET);
					unsigned long long addr_26 = addr_25 + 4 * 8 * 256;
					if (addr_26 % 32) {
						addr_26 = (addr_26 / 32 + 1) * 32;
					}
					write_reg32(map_md, addr_26, MEM_PU_BUF_V_OFFSET);
					unsigned long long addr_27 = addr_26 + 4 * 8 * 256;
					if (addr_27 % 32) {
						addr_27 = (addr_27 / 32 + 1) * 32;
					}
					write_reg32(map_md, addr_27, MEM_STORE_BUF_V_OFFSET);
/*
					map = mmap(0, 4 * 1024, PROT_READ | PROT_WRITE, MAP_SHARED, h2c_reg, AVS_REG_AEC_BASE);
					unsigned int bs_address = 0xa0000000;
					write_reg32(map, bs_address, BS_DATA_OFFSET);
				//	write_reg32(map, 0x1, AEC_START);
					printf("write once\n");
				        write_reg32(map, 0x1, AEC_GLOBAL_INTR_ENABLE);
					write_reg32(map, 0x1, AEC_IP_INTR_ENABLE);	
					write_reg32(map, 0x1, AEC_START);
					
					if (munmap(map, 4 * 1024) == -1)
						LOG_ERRPR("3 munmap fail\n");
*/
				//	write_from_buffer(TO_DEVICE_NAME_DEFAULT, h2c_mem, (char*)fetch_ctx.ctx_pinter_refp_ptr, 17 * 2 * 4, addr_0);
				//	write_from_buffer(TO_DEVICE_NAME_DEFAULT, h2c_mem, (char*)fetch_ctx.ref_ptr_dbk, 17 * 2 * 4, addr_1);
				//	write_from_buffer(TO_DEVICE_NAME_DEFAULT, h2c_mem, (char*)fetch_ctx.refp_ptr_test, 17 * 2 * 4, addr_2);
				//	write_from_buffer(TO_DEVICE_NAME_DEFAULT, h2c_mem, (char*)fetch_ctx.is_library_picture, 17 * 2 * 1, addr_3);
				//	write_from_buffer(TO_DEVICE_NAME_DEFAULT, h2c_mem, (char*)fetch_ctx.slice_sao_enable, 4 * 1, addr_4);
				//	write_from_buffer(TO_DEVICE_NAME_DEFAULT, h2c_mem, (char*)fetch_ctx.refp_ptr, 17 * 2 * 4, addr_5);
				//	write_from_buffer(TO_DEVICE_NAME_DEFAULT, h2c_mem, (char*)fetch_ctx.umveOffsetPicCount, 8 * 4, addr_6);
				//	write_from_buffer(TO_DEVICE_NAME_DEFAULT, h2c_mem, (char*)fetch_ctx.cu_delta_qp_lcu_map, 256 * 4, addr_7);
				//	write_from_buffer(TO_DEVICE_NAME_DEFAULT, h2c_mem, (char*)ctx->map.map_mv, 4 * ctx->info.f_scu * 2 * 2, addr_8);
				//	write_from_buffer(TO_DEVICE_NAME_DEFAULT, h2c_mem, (char*)ctx->map.map_refi, 4 * ctx->info.f_scu * 2, addr_9);
				//	write_from_buffer(TO_DEVICE_NAME_DEFAULT, h2c_mem, (char*)ctx->map.map_scu, 4 * ctx->info.f_scu, addr_10);
				//	write_from_buffer(TO_DEVICE_NAME_DEFAULT, h2c_mem, (char*)ctx->map.map_cu_mode, 4 * ctx->info.f_scu, addr_11);
				//	write_from_buffer(TO_DEVICE_NAME_DEFAULT, h2c_mem, (char*)ctu_ctrl_input.lambda, 4 * 3, addr_12);
				//	write_from_buffer(TO_DEVICE_NAME_DEFAULT, h2c_mem, (char*)ctu_ctrl_input.dist_chroma_weight, 4 * 2 * 2, addr_13);

				//	write_from_buffer(TO_DEVICE_NAME_DEFAULT, h2c_mem, (char*)ctu_ctrl_input.CuQPMap, 4 * 256, addr_14);
				//	write_from_buffer(TO_DEVICE_NAME_DEFAULT, h2c_mem, (char*)ctu_ctrl_input.CuQPMap_Frame, 4 * 1920 * 1080, addr_15);

					/*
					char *imgY_temp = (char *)malloc(ctx->info.pic_width * ctx->info.pic_height);
					char *imgU_temp = (char *)malloc(ctx->info.pic_width * ctx->info.pic_height / 4);
					char *imgV_temp = (char *)malloc(ctx->info.pic_width * ctx->info.pic_height / 4);
					for (int img_temp = 0; img_temp < ctx->info.pic_width * ctx->info.pic_height; img_temp++) {
						imgY_temp[img_temp] = (char)(Fetch_input_video.imgY_org[img_temp] & 0xff);
					}
					for (int img_temp = 0; img_temp < ctx->info.pic_width * ctx->info.pic_height / 4; img_temp++) {
						imgU_temp[img_temp] = (char)(Fetch_input_video.imgU_org[img_temp] & 0xff);
					}
					for (int img_temp = 0; img_temp < ctx->info.pic_width * ctx->info.pic_height / 4; img_temp++) {
						imgV_temp[img_temp] = (char)(Fetch_input_video.imgV_org[img_temp] & 0xff);
					}
					write_from_buffer(TO_DEVICE_NAME_DEFAULT, h2c_mem, (char*)imgY_temp, 1 * ctx->info.pic_width * ctx->info.pic_height, addr_16);
					write_from_buffer(TO_DEVICE_NAME_DEFAULT, h2c_mem, (char*)imgU_temp, 1 * ctx->info.pic_width * ctx->info.pic_height / 4, addr_17);
					write_from_buffer(TO_DEVICE_NAME_DEFAULT, h2c_mem, (char*)imgV_temp, 1 * ctx->info.pic_width * ctx->info.pic_height / 4, addr_18);
					*/
					/*
					char* list_ptr_tmp = (char*)malloc(4 * 4 * ctx->info.f_scu);
					memset(list_ptr_tmp, 0, 4 * 4 * ctx->info.f_scu);
					write_from_buffer(TO_DEVICE_NAME_DEFAULT, h2c_mem, list_ptr_tmp, 4 * ctx->info.f_scu * 2 * 2, addr_19);
					write_from_buffer(TO_DEVICE_NAME_DEFAULT, h2c_mem, list_ptr_tmp, 4 * ctx->info.f_scu * 2 * 2, addr_20);
					write_from_buffer(TO_DEVICE_NAME_DEFAULT, h2c_mem, list_ptr_tmp, 4 * ctx->info.f_scu * 2, addr_21);
					write_from_buffer(TO_DEVICE_NAME_DEFAULT, h2c_mem, list_ptr_tmp, 4 * ctx->info.f_scu * 2, addr_22);

					write_from_buffer(TO_DEVICE_NAME_DEFAULT, h2c_mem, list_ptr_tmp, 4 * 34, addr_23);
					write_from_buffer(TO_DEVICE_NAME_DEFAULT, h2c_mem, list_ptr_tmp, 4 * 34, addr_24);
					*/
					write_from_buffer(TO_DEVICE_NAME_DEFAULT, h2c_mem, (char*)mem_load_int, 4 * 8 * 256, addr_25);
					write_from_buffer(TO_DEVICE_NAME_DEFAULT, h2c_mem, (char*)mem_pu_int, 4 * 8 * 256, addr_26);
					write_from_buffer(TO_DEVICE_NAME_DEFAULT, h2c_mem, (char*)mem_store_int, 4 * 8 * 256, addr_27);
				}
				if (ctu_ctrl_input.slice_enable == 0x1) {
					unsigned int bs_address = 0xa0000000;
					write_reg32(map_aec, bs_address, BS_DATA_OFFSET);
				    write_reg32(map_aec, 0x1, AEC_GLOBAL_INTR_ENABLE);
					write_reg32(map_aec, 0x1, AEC_IP_INTR_ENABLE);	
					write_reg32(map_aec, 0x1, AEC_START);

				}

				ctu_ctrl_input.slice_enable = 0x0;
			
				gettimeofday(&time, NULL);
				time_begin = time;
				reg_end = time;
				printf("write reg and memory use %d sec, %d usec\n", reg_end.tv_sec - reg_begin.tv_sec, reg_end.tv_usec - reg_begin.tv_usec);
			//	while(1);
				write_reg32(map_base, 1, 0);      //write start
			//	printf("%d lcu finish\n", ctx->lcu_cnt);
				ctx->lcu_cnt--;
#endif 


			}
		}
#ifndef VU440
		gettimeofday(&time, NULL);
		all_end = time;
//		printf("one frame use %d sec, %d usec\n", all_end.tv_sec - all_begin.tv_sec, all_end.tv_usec - all_begin.tv_usec);
		int aec_result;
		while (1) {

			aec_result = *((unsigned int *)map_aec + AEC_IP_INTR_STATUS);
			if (aec_result == 0x1)
				break;
            usleep(1);
		}
		unsigned int bs_length = 0;
		bs_length = *((unsigned int *)map_aec + BYTE_CNT_FRAME);
//		printf("bs_length = %d\n", bs_length);				
		char* tmp_buffer = (char*)malloc(bs_length);

		
		read_to_buffer(FROM_DEVICE_NAME_DEFAULT, c2h_mem, tmp_buffer, bs_length, 0xa0000000);
		fwrite((void*)(ctx->bs.beg - header_length), 1, header_length + 4, bs_fp);
/*****************demulator ***********************/
		struct aec_t aec_cfg;
		arienco_start_encoding(&aec_cfg);
		int z;
		for (z = 0; z < bs_length; z++) {
			write_byte(&aec_cfg, *(tmp_buffer + z));
		}
		fwrite((void *)(aec_cfg.Ecodestrm), 1, aec_cfg.Ecodestrm_len, bs_fp);
		unsigned char end_code[5] = { 0x80, 0x00, 0x00, 0x01, 0x8F };
		fwrite((void*)end_code, 1, 5, bs_fp);
/*****************demulator end***********************/
		gettimeofday(&time, NULL);
		all_begin = time;
		printf("demulator use %d sec, %d usec\n", all_begin.tv_sec - all_end.tv_sec, all_begin.tv_usec - all_end.tv_usec);

		write_reg32(map_aec, 0x1, AEC_IP_INTR_STATUS);
		
#endif
	}
    if (munmap(map, 4 * 1024) == -1)
        LOG_ERRPR("3 munmap fail\n");

    if (munmap(map_aec, 4 * 1024) == -1)
        LOG_ERRPR("AEC munmap fail\n");

    if (munmap(map_fetch1, 4 * 1024) == -1)
        LOG_ERRPR("2 munmap fail\n");

    if (munmap(map_fetch2, 4 * 1024) == -1)
        LOG_ERRPR("1 munmap fail\n");

    if (munmap(map_base, 4 * 1024) == -1)
        LOG_ERRPR("5 munmap fail\n");
	close(h2c_reg);
	close(h2c_mem);
	close(c2h_mem);
	delete[] mem_load;
	delete[] mem_pu;
	delete[] mem_store;

	pic_end(ctx, stat, me_line_map_ptr
#if USE_ROI_QP_CONFIG || USE_ROI_MODE_CONFIG
		, &fetch_ctx
#endif
	);

	/* Bit-stream re-writing (END) */
	return COM_OK;
}
#if PATCH
void enc_set_patch_idx(s8* map_patch_idx, PATCH_INFO* patch, int pic_width, int pic_height)
{
	/*get the boundary*/
	int left_scu = patch->left_pel >> MIN_CU_LOG2;
	int right_scu = patch->right_pel >> MIN_CU_LOG2;
	int down_scu = patch->down_pel >> MIN_CU_LOG2;
	int up_scu = patch->up_pel >> MIN_CU_LOG2;
	int width_scu = pic_width >> MIN_CU_LOG2;
	int height_scu = pic_height >> MIN_CU_LOG2;
	/*set idx*/
	int scu = up_scu * width_scu + left_scu;
	for (int j = up_scu; j < COM_MIN(height_scu, down_scu); j++) {
		int temp = scu;
		for (int i = left_scu; i < COM_MIN(width_scu, right_scu); i++) {
			map_patch_idx[scu] = (s8)patch->idx;
			scu++;
		}
		scu = temp;
		scu += width_scu;
	}

}
void en_copy_lcu_scu(u32* scu_temp, u32* scu_best, s8(*refi_temp)[REFP_NUM], s8(*refi_best)[REFP_NUM],
	s16(*mv_temp)[REFP_NUM][MV_D], s16(*mv_best)[REFP_NUM][MV_D], u32* cu_mode_temp, u32* cu_mode_best, PATCH_INFO* patch,
	int pic_width, int pic_height)
{
	/*get the boundary*/
	int left_scu = patch->left_pel >> MIN_CU_LOG2;
	int right_scu = patch->right_pel >> MIN_CU_LOG2;
	int down_scu = patch->down_pel >> MIN_CU_LOG2;
	int up_scu = patch->up_pel >> MIN_CU_LOG2;
	int width_scu = pic_width >> MIN_CU_LOG2;
	int height_scu = pic_height >> MIN_CU_LOG2;
	/*copy*/
	int scu = up_scu * width_scu + left_scu;
	int m;
	for (int j = up_scu; j < COM_MIN(height_scu, down_scu); j++) {
		int temp = scu;
		for (int i = left_scu; i < COM_MIN(width_scu, right_scu); i++) {
			scu_temp[scu] = scu_best[scu];
			cu_mode_temp[scu] = cu_mode_best[scu];
			for (m = 0; m < REFP_NUM; m++) {
				refi_temp[scu][m] = refi_best[scu][m];
			}
			for (m = 0; m < REFP_NUM; m++) {
				for (int n = 0; n < MV_D; n++) {
					mv_temp[scu][m][n] = mv_best[scu][m][n];
				}
			}
			scu++;
		}
		scu = temp;
		scu += width_scu;
	}
}
#endif
int enc_push_frm(ENC_CTX* ctx, COM_IMGB* imgb)
{
	COM_PIC* pic;
	ctx->pic_icnt++;
	ctx->pico_idx = ctx->pic_icnt % ctx->pico_max_cnt;
	ctx->pico = ctx->pico_buf[ctx->pico_idx];
	ctx->pico->pic_icnt = ctx->pic_icnt;
	ctx->pico->is_used = 1;
	pic = &ctx->pico->pic;
	PIC_ORG(ctx) = pic;
	/* set pushed image to current input (original) image */
	com_mset(pic, 0, sizeof(COM_PIC));
	pic->y = (pel*)imgb->addr_plane[0];
	pic->u = (pel*)imgb->addr_plane[1];
	pic->v = (pel*)imgb->addr_plane[2];
	pic->width_luma = imgb->width[0];
	pic->height_luma = imgb->height[0];
	pic->width_chroma = imgb->width[1];
	pic->height_chroma = imgb->height[1];
	pic->stride_luma = STRIDE_IMGB2PIC(imgb->stride[0]);
	pic->stride_chroma = STRIDE_IMGB2PIC(imgb->stride[1]);
	pic->imgb = imgb;
	imgb_addref(imgb);
	return COM_OK;
}

void enc_init_err_scale(int bit_depth)
{
	int qp;
	int i;
	const int quant_scale[80] = { (16302), (15024), (13777), (12634), (11626), (10624), (9742), (8958), (8192), (7512), (6889), (6305), (5793), (5303), (4878), (4467), (4091), (3756), (3444), (3161), (2894), (2654), (2435), (2235), (2048), (1878), (1722), (1579), (1449), (1329), (1218), (1117), (1024), (939), (861), (790), (724), (664), (609), (558), (512), (470), (430), (395), (362), (332), (304), (279), (256), (235), (215), (197), (181), (166), (152), (140), (128), (116), (108), (99), (91), (83), (76), (69), (64), (59), (54), (49), (45), (41), (38), (35), (32), (30), (27), (25), (23), (21), (19), (18) };

	for (qp = 0; qp < 80; qp++) {
		int q_value = quant_scale[qp];
#if USE_CTU64_ARRARY
		for (i = 0; i < MAX_CU_DEPTH + 1; i++)
#else
		for (i = 0; i < MAX_CU_DEPTH; i++)
#endif
		{
			int tr_shift = get_transform_shift(bit_depth, i + 1);
			double err_scale = pow(2.0, SCALE_BITS - tr_shift);
			err_scale = err_scale / q_value / (1 << ((bit_depth - 8)));
			err_scale_tbl[qp][i] = (s64)(err_scale * (double)(1 << ERR_SCALE_PRECISION_BITS));
			err_scale_tbl_quant[qp][i] = (s64_affine_merge)(err_scale * (double)(1 << ERR_SCALE_PRECISION_BITS));

		}
	}
	return;
}

extern ENC_PARAM          param_input;

ENC enc_create(ENC_PARAM* param_input, int* err)
{
	ENC_CTX* ctx;
	int          ret;
	int ctb_size = 64;
	vcencRateControl_s* rc = NULL;
#if ENC_DEC_TRACE
	if (fp_trace == NULL && PRINT_TOSTOUT==0)
	{
		fopen_s(&fp_trace, "enc_trace.txt", "w+");
	}
#endif
	ctx = NULL;
	/* memory allocation for ctx and core structure */
	ctx = ctx_alloc();
	PATCH_INFO* patch = NULL;
	patch = (PATCH_INFO*)com_malloc_fast(sizeof(PATCH_INFO));
	com_assert_gv(ctx != NULL, ret, COM_ERR_OUT_OF_MEMORY, ERR);
	/* set default value for encoding parameter */
	ret = set_init_param(&ctx->param, param_input);
	com_assert_g(ret == COM_OK, ERR);

	ctx->pic_sao = NULL;
	ctx->pic_alf_Org = NULL;
	//ctx->pic_alf_Rec = NULL;

	ret = com_scan_tbl_init();
	com_assert_g(ret == COM_OK, ERR);

	//init_dct_coef();

#if USE_RDOQ
	enc_init_err_scale(ctx->param.bit_depth_internal);
#endif

	ret = enc_ready(ctx, &core);
	com_assert_g(ret == COM_OK, ERR);
	/* set default value for ctx */
	ctx->magic = ENC_MAGIC_CODE;
#if PATCH
	//write patch info to ctx
	ctx->patch = patch;
	ctx->patch->stable = param_input->patch_stable;
	ctx->patch->cross_patch_loop_filter = param_input->cross_patch_loop_filter;
	ctx->patch->ref_colocated = param_input->patch_ref_colocated;
	if (ctx->patch->stable) {
		ctx->patch->uniform = param_input->patch_uniform;
		if (ctx->patch->uniform) {
			if (param_input->patch_width_in_lcu <= 0) {
				ctx->patch->width = ctx->info.pic_width_in_lcu;
				ctx->patch->columns = 1;
				param_input->patch_column_width[0] = ctx->info.pic_width_in_lcu;
				ctx->patch->width_in_lcu = param_input->patch_column_width;
			}
			else {
				ctx->patch->width = param_input->patch_width_in_lcu;
				ctx->patch->columns = ctx->info.pic_width_in_lcu / ctx->patch->width;
				for (int i = 0; i < ctx->patch->columns; i++) {
					param_input->patch_column_width[i] = ctx->patch->width;
				}
				if (ctx->info.pic_width_in_lcu % ctx->patch->width != 0) {
					if (ctx->patch->columns == 0) {
						param_input->patch_column_width[ctx->patch->columns] = ctx->info.pic_width_in_lcu - ctx->patch->width *
							ctx->patch->columns;
						ctx->patch->columns++;
					}
					else {
						param_input->patch_column_width[ctx->patch->columns - 1] = param_input->patch_column_width[ctx->patch->columns - 1] +
							ctx->info.pic_width_in_lcu - ctx->patch->width * ctx->patch->columns;
					}
				}
				ctx->patch->width_in_lcu = param_input->patch_column_width;

			}
			if (param_input->patch_height_in_lcu <= 0) {
				ctx->patch->height = ctx->info.pic_height_in_lcu;
				ctx->patch->rows = 1;
				param_input->patch_row_height[0] = ctx->info.pic_height_in_lcu;
				ctx->patch->height_in_lcu = param_input->patch_row_height;
			}
			else {
				ctx->patch->height = param_input->patch_height_in_lcu;
				ctx->patch->rows = ctx->info.pic_height_in_lcu / ctx->patch->height;
				for (int i = 0; i < ctx->patch->rows; i++) {
					param_input->patch_row_height[i] = ctx->patch->height;
				}
				if (ctx->info.pic_height_in_lcu % ctx->patch->height != 0) {
					if (ctx->patch->rows == 0) {
						param_input->patch_row_height[ctx->patch->rows] = ctx->info.pic_height_in_lcu - ctx->patch->height * ctx->patch->rows;
						ctx->patch->rows++;
					}
					else {
#if PATCH_M4839
						param_input->patch_row_height[ctx->patch->rows] = ctx->info.pic_height_in_lcu - ctx->patch->height * ctx->patch->rows;
						ctx->patch->rows++;
#else
						param_input->patch_row_height[ctx->patch->rows - 1] = param_input->patch_row_height[ctx->patch->rows - 1] +
							ctx->info.pic_height_in_lcu - ctx->patch->height * ctx->patch->rows;
#endif
					}
				}
				ctx->patch->height_in_lcu = param_input->patch_row_height;
			}
		}
	}
#endif
	enc_cfg_t cfg;

	cfg.rc_type = param_input->rc_type;
	cfg.rc_crf = param_input->rc_crf;
	cfg.rc_bitrate = param_input->rc_bitrate;
	cfg.rc_max_bitrate = param_input->rc_max_bitrate;
	cfg.rc_vbv_buf_size = param_input->rc_vbv_buf_size;
	cfg.fps_num = param_input->fps_num;
	cfg.fps_den = param_input->fps_den;
	cfg.fps = param_input->fps;
	cfg.i_period = param_input->i_period;
	cfg.pic_width = param_input->pic_width;
	cfg.pic_height = param_input->pic_height;
	cfg.rc_min_qp = param_input->rc_min_qp;
	cfg.rc_max_qp = param_input->rc_max_qp;
	cfg.max_b_frames = param_input->max_b_frames;
#if UMVE_ENH
	ctx->dataCol = FALSE;
	ctx->needReset = FALSE;
	ctx->lastIPicPOC = -1;
	ctx->umveAveOffset = -1;
#endif
#ifdef RC_OPEN
	ctx->pi.tab_mvbits_offset = inter_search_create(&ctx->pi.tab_mvbits, &ctx->info);
	ctx->info.tab_mvbits = ctx->pi.tab_mvbits;
	ctx->info.gop_size = ctx->param.gop_size;
	ctx->info.sqh.low_delay = ctx->param.max_b_frames == 0 ? 1 : 0;
	inter_search_init(&ctx->pi, &ctx->info, 0);
	rc_init(&ctx->rc, &cfg);
#endif
#ifdef LOWDELAY_RC_OPEN
	int width, height;
	rc = &ctx->lowdelay_rc;
	width = ctb_size * ((cfg.pic_width + ctb_size - 1) / ctb_size);
	height = ctb_size * ((cfg.pic_height + ctb_size - 1) / ctb_size);

	rc->ctbRows = width / ctb_size;
	rc->ctbCols = height / ctb_size;
	rc->ctbPerPic = rc->ctbRows * rc->ctbCols;
	rc->outRateDenom = 1;
	rc->outRateNum = cfg.fps;
	rc->picArea = ((cfg.pic_width + 7) & (~7)) * ((cfg.pic_height + 7) & (~7));

	rc->reciprocalOfNumBlk8 = 1.0 / (rc->ctbPerPic * (ctb_size / 8) * (ctb_size / 8));

	rc->ctbSize = ctb_size;

	{
		rcVirtualBuffer_s* vb = &rc->virtualBuffer;

		vb->bitRate = param_input->rc_bitrate * 1000;
		vb->unitsInTic = rc->outRateDenom;
		vb->timeScale = rc->outRateNum * (0 + 1);
		vb->bufferSize = param_input->rc_vbv_buf_size * 1000;
	}

	rc->hrd = ENCHW_NO;
	rc->picRc = ENCHW_NO;
	rc->ctbRc = 0;
	rc->picSkip = ENCHW_NO;
	rc->vbr = ENCHW_NO;

	rc->qpHdr = VCENC_DEFAULT_QP << QP_FRACTIONAL_BITS;
	rc->qpMin = 0 << QP_FRACTIONAL_BITS;
	rc->qpMax = 51 << QP_FRACTIONAL_BITS;

	rc->frameCoded = ENCHW_YES;
	rc->sliceTypeCur = I_SLICE;
	rc->sliceTypePrev = P_SLICE;
	rc->bitrateWindow = 150;

	/* Default initial value for intra QP delta */
	rc->intraQpDelta = INTRA_QPDELTA_DEFAULT << QP_FRACTIONAL_BITS;
	rc->fixedIntraQp = 0 << QP_FRACTIONAL_BITS;
	rc->frameQpDelta = 0 << QP_FRACTIONAL_BITS;
	rc->gopPoc = 0;
	ctx->lowdelay_rc.picArea = ((cfg.pic_width + 7) & (~7)) * ((cfg.pic_height + 7) & (~7));

	rc->i32MaxPicSize = cfg.pic_width * cfg.pic_height;
	VCEncInitRc(&ctx->lowdelay_rc, 1);
#endif


	ctx->id = (int*)ctx;
	return (ctx->id);
ERR:
	if (ctx) {
		ctx_free(ctx);
	}
	if (err) {
		*err = ret;
	}
	return NULL;
}

void enc_delete(ENC id)
{
	fclose(fp_rec);
	fclose(fp_map);
	fp_rec = NULL;
	fp_map = NULL;
	ENC_CTX* ctx = (ENC_CTX*)id;
#ifdef RC_OPEN
	inter_search_free(ctx->pi.tab_mvbits, ctx->pi.tab_mvbits_offset);
	rc_destroy(&ctx->rc);
#endif
	ENC_ID_TO_CTX_R(id, ctx);
#if ENC_DEC_TRACE
	fclose(fp_trace);
#endif
	enc_flush(ctx);
	ctx_free(ctx);
	com_scan_tbl_delete();
}


static int check_frame_delay(ENC_CTX* ctx)
{
	if (ctx->pic_icnt < ctx->frm_rnum) {
		return COM_OK_OUT_NOT_AVAILABLE;
	}
	return COM_OK;
}

static int check_more_frames(ENC_CTX* ctx)
{
	ENC_PICO* pico;
	int           i;
	if (ctx->param.force_output) {
		/* pseudo enc_push() for bumping process ****************/
		ctx->pic_icnt++;
		/**********************************************************/
		for (i = 0; i < ctx->pico_max_cnt; i++) {
			pico = ctx->pico_buf[i];
			if (pico != NULL) {
				if (pico->is_used == 1) {
					return COM_OK;
				}
			}
		}
		return COM_OK_NO_MORE_FRM;
	}
	return COM_OK;
}

int enc_pic_data_update(ENC_CTX* ctx, COM_BITB* bitb, int gop_size, int pic_cnt)
{
	bitb->err = 0;
	pic_cnt = ctx->pic_icnt - ctx->frm_rnum;
	gop_size = ctx->param.gop_size;
	ctx->force_slice = ((ctx->pic_ticnt % gop_size >= ctx->pic_ticnt - pic_cnt + 1) && ctx->param.force_output) ? 1 : 0;
	com_assert_rv(bitb->addr && bitb->bsize > 0, COM_ERR_INVALID_ARGUMENT);
	return COM_OK;
}
int enc_encode(
#if PATCH_DIVIDE_ENABLE
	int multislice_mode,
	#endif
	ENC id, COM_BITB* bitb, ENC_STAT* stat)
{
	ENC_CTX* ctx = (ENC_CTX*)id;
	int            ret;
	int            gop_size = 0, pic_cnt = 0;
	/* bumping - check whether input pictures are remaining or not in pico_buf[] */
	if (COM_OK_NO_MORE_FRM == check_more_frames(ctx))
	{
		return COM_OK_NO_MORE_FRM;
	}
	/* store input picture and return if needed */
	if (COM_OK_OUT_NOT_AVAILABLE == check_frame_delay(ctx))
	{
		return COM_OK_OUT_NOT_AVAILABLE;
	}
	/* update BSB */
	enc_pic_data_update(ctx, bitb, gop_size, pic_cnt);

	/* initialize variables for a picture encoding */
	ret = enc_pic_prepare(ctx, bitb);
	com_assert_rv(ret == COM_OK, ret);
	/* encode one picture */
	ret = enc_pic(
#if PATCH_DIVIDE_ENABLE
		multislice_mode,
#endif
		ctx, 
		bitb, 
		stat,
		NULL
	);
	com_assert_rv(ret == COM_OK, ret);
	/* finishing of encoding a picture */
	enc_pic_finish(ctx, bitb, stat);
	com_assert_rv(ret == COM_OK, ret);
#if LIBVC_ON
	/* output libpic to the buffer outside the encoder*/
	if (ctx->info.sqh.library_stream_flag)
	{
		//output to the buffer outside the decoder
		ret = com_picman_out_libpic(PIC_REC(ctx), ctx->info.pic_header.library_picture_index, &ctx->rpm);
	}
#endif

	return COM_OK;
}

bool is_pol_written = false;

int start_encode_one_frame(
#if PATCH_DIVIDE_ENABLE
	int multislice_mode,
#endif
	ENC id, COM_BITB* bitb, ENC_STAT* stat, double ctu_64x64_ime_cost[2040])
{
	ENC_CTX* ctx = (ENC_CTX*)id;

	int            ret;
	int            gop_size = 0, pic_cnt = 0;

	if (COM_OK_NO_MORE_FRM == check_more_frames(ctx))
	{
		return COM_OK_NO_MORE_FRM;
	}
	/* store input picture and return if needed */
    if (COM_OK_OUT_NOT_AVAILABLE == check_frame_delay(ctx))
	{
		return COM_OK_OUT_NOT_AVAILABLE;
	}
	/* update BSB */
	enc_pic_data_update(ctx, bitb, gop_size, pic_cnt);

	/* initialize variables for a picture encoding */
	ret = enc_pic_prepare(ctx, bitb);
	com_assert_rv(ret == COM_OK, ret);
	/* encode one picture */
	bs_fp = fopen("top_bs_file.bin", "ab");
	ret = enc_pic(
#if PATCH_DIVIDE_ENABLE
		multislice_mode,
#endif
		ctx, 
		bitb, 
		stat,
		ctu_64x64_ime_cost
	);
	fclose(bs_fp);
	com_assert_rv(ret == COM_OK, ret);
#if defined  NOP
	if (!is_pol_written) {
#endif
		char cmd[] = "pol";
		//write_file(cmd, 0x00000000, 1);// read_glb_STAT0_slice_done());
#if defined NOP
	}
#endif
	/* finishing of encoding a picture */
	enc_pic_finish(ctx, bitb, stat);
	com_assert_rv(ret == COM_OK, ret);
#if LIBVC_ON
	/* output libpic to the buffer outside the encoder*/
	if (ctx->info.sqh.library_stream_flag)
	{
		//output to the buffer outside the decoder
		ret = com_picman_out_libpic(PIC_REC(ctx), ctx->info.pic_header.library_picture_index, &ctx->rpm);
	}
#endif
#if defined NOP
	if (!is_pol_written) {
		char cmd[] = "end";
		write_file(cmd, 0, 0);
		is_pol_written = 1;
	}
#endif
	return COM_OK;
}


void enc_malloc_1d(void** dst, int size)
{
	if (*dst == NULL) {
		*dst = com_malloc_fast(size);
		com_mset(*dst, 0, size);
	}
}

void enc_malloc_2d(s8*** dst, int size_1d, int size_2d, int type_size)
{
	int i;
	if (*dst == NULL) {
		*dst = (s8**)com_malloc_fast(size_1d * sizeof(s8*));
		com_mset(*dst, 0, size_1d * sizeof(s8*));
		(*dst)[0] = (s8*)com_malloc_fast(size_1d * size_2d * type_size);
		com_mset((*dst)[0], 0, size_1d * size_2d * type_size);
		for (i = 1; i < size_1d; i++) {
			(*dst)[i] = (*dst)[i - 1] + size_2d * type_size;
		}
	}
}

int enc_create_cu_data(ENC_CU_DATA* cu_data, int cu_width_log2, int cu_height_log2)
{
	int cuw_scu, cuh_scu;
	int size_8b, size_16b, size_32b, cu_cnt, pixel_cnt;
	cuw_scu = 1 << cu_width_log2;
	cuh_scu = 1 << cu_height_log2;
	size_8b = cuw_scu * cuh_scu * sizeof(s8);
	size_16b = cuw_scu * cuh_scu * sizeof(s16);
	size_32b = cuw_scu * cuh_scu * sizeof(s32); /////////////////////////////
	cu_cnt = cuw_scu * cuh_scu;
	pixel_cnt = cu_cnt << 4;
	return COM_OK;
}

void enc_free_1d(void* dst)
{
	if (dst != NULL) {
		com_mfree_fast(dst);
	}
}

void enc_free_2d(void** dst)
{
	if (dst) {
		if (dst[0]) {
			com_mfree_fast(dst[0]);
		}
		com_mfree_fast(dst);
	}
}

int enc_delete_cu_data(ENC_CU_DATA* cu_data)
{
	return COM_OK;
}
/* ====================================================================================================================

The copyright in this software is being made available under the License included below.
This software may be subject to other third party and contributor rights, including patent rights, and no such
rights are granted under this license.

Copyright (c) 2018, HUAWEI TECHNOLOGIES CO., LTD. All rights reserved.
Copyright (c) 2018, SAMSUNG ELECTRONICS CO., LTD. All rights reserved.
Copyright (c) 2018, PEKING UNIVERSITY SHENZHEN GRADUATE SCHOOL. All rights reserved.*/
