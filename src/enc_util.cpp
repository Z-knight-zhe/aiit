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

#include "com_util.h"
#include "enc_def.h"
#include "enc_util.h"

/******************************************************************************
 * picture buffer alloc/free/expand
 ******************************************************************************/
void enc_picbuf_expand(ENC_CTX *ctx, COM_PIC *pic)
{
    com_picbuf_expand(pic, pic->padsize_luma, pic->padsize_chroma);
}

/******************************************************************************
 * implementation of bitstream writer
 ******************************************************************************/
void enc_bsw_skip_slice_size(COM_BSW *bs)
{
    com_bsw_write(bs, 0, 32);
}

void enc_bsw_write_slice_size(COM_BSW *bs)
{
    u32 size;
    size = COM_BSW_GET_WRITE_BYTE(bs) - 4;
    bs->beg[0] = (u8)((size & 0xff000000) >> 24);
    bs->beg[1] = (u8)((size & 0x00ff0000) >> 16);
    bs->beg[2] = (u8)((size & 0x0000ff00) >>  8);
    bs->beg[3] = (u8)((size & 0x000000ff) >>  0);
}


void com_mcpy_SBAC_CTX_MODEL1(SBAC_CTX_MODEL* dst, SBAC_CTX_MODEL* src, int sz)
{
#pragma HLS INLINE off
    for (int i = 0; i < sz; i++) {
#pragma HLS LOOP_TRIPCOUNT max=24
        dst[i] = src[i];
    }
}

void com_mcpy_COM_SBAC_CTX(COM_SBAC_CTX* dst, COM_SBAC_CTX* src)
{
    com_mcpy_SBAC_CTX_MODEL1((dst)->skip_flag, (src)->skip_flag, NUM_SBAC_CTX_SKIP_FLAG);
    com_mcpy_SBAC_CTX_MODEL1((dst)->skip_idx_ctx, (src)->skip_idx_ctx, NUM_SBAC_CTX_SKIP_IDX);
    com_mcpy_SBAC_CTX_MODEL1((dst)->direct_flag, (src)->direct_flag, NUM_SBAC_CTX_DIRECT_FLAG);
#if !AWP
    (dst)->umve_flag = (src)->umve_flag;
#endif
    com_mcpy_SBAC_CTX_MODEL1((dst)->umve_base_idx, (src)->umve_base_idx, NUM_SBAC_CTX_UMVE_BASE_IDX);
    com_mcpy_SBAC_CTX_MODEL1((dst)->umve_step_idx, (src)->umve_step_idx, NUM_SBAC_CTX_UMVE_STEP_IDX);
    com_mcpy_SBAC_CTX_MODEL1((dst)->umve_dir_idx, (src)->umve_dir_idx, NUM_SBAC_CTX_UMVE_DIR_IDX);
    com_mcpy_SBAC_CTX_MODEL1((dst)->inter_dir, (src)->inter_dir, NUM_INTER_DIR_CTX);
    com_mcpy_SBAC_CTX_MODEL1((dst)->intra_dir, (src)->intra_dir, NUM_INTRA_DIR_CTX);
    com_mcpy_SBAC_CTX_MODEL1((dst)->pred_mode, (src)->pred_mode, NUM_PRED_MODE_CTX);
#if MODE_CONS
    com_mcpy_SBAC_CTX_MODEL1((dst)->cons_mode, (src)->cons_mode, NUM_CONS_MODE_CTX);
#endif
    com_mcpy_SBAC_CTX_MODEL1((dst)->ipf_flag, (src)->ipf_flag, NUM_IPF_CTX);
    com_mcpy_SBAC_CTX_MODEL1((dst)->refi, (src)->refi, NUM_REFI_CTX);
    com_mcpy_SBAC_CTX_MODEL1((dst)->mvr_idx, (src)->mvr_idx, NUM_MVR_IDX_CTX);
#if BD_AFFINE_AMVR
    com_mcpy_SBAC_CTX_MODEL1((dst)->affine_mvr_idx, (src)->affine_mvr_idx, NUM_AFFINE_MVR_IDX_CTX);
#endif
#if EXT_AMVR_HMVP
    com_mcpy_SBAC_CTX_MODEL1((dst)->mvp_from_hmvp_flag, (src)->mvp_from_hmvp_flag, NUM_EXTEND_AMVR_FLAG);
#endif
    for (int it = 0; it < 2; it++) {
#pragma HLS loop_tripcount max = 2
        for (int jt = 0; jt < NUM_MV_RES_CTX; jt++) {
#pragma HLS loop_tripcount max = 3
            (dst)->mvd[it][jt] = (src)->mvd[it][jt];
        }
    }
    com_mcpy_SBAC_CTX_MODEL1((dst)->ctp_zero_flag, (src)->ctp_zero_flag, NUM_CTP_ZERO_FLAG_CTX);
    com_mcpy_SBAC_CTX_MODEL1((dst)->cbf, (src)->cbf, NUM_QT_CBF_CTX);
    com_mcpy_SBAC_CTX_MODEL1((dst)->tb_split, (src)->tb_split, NUM_TB_SPLIT_CTX);
    com_mcpy_SBAC_CTX_MODEL1((dst)->run, (src)->run, NUM_SBAC_CTX_RUN);
    com_mcpy_SBAC_CTX_MODEL1((dst)->run_rdoq, (src)->run_rdoq, NUM_SBAC_CTX_RUN);
    com_mcpy_SBAC_CTX_MODEL1((dst)->last1, (src)->last1, NUM_SBAC_CTX_LAST1 * 2);
    com_mcpy_SBAC_CTX_MODEL1((dst)->last2, (src)->last2, NUM_SBAC_CTX_LAST2 * 2 - 2);
    com_mcpy_SBAC_CTX_MODEL1((dst)->level, (src)->level, NUM_SBAC_CTX_LEVEL);
    com_mcpy_SBAC_CTX_MODEL1((dst)->split_flag, (src)->split_flag, NUM_SBAC_CTX_SPLIT_FLAG);
    com_mcpy_SBAC_CTX_MODEL1((dst)->bt_split_flag, (src)->bt_split_flag, NUM_SBAC_CTX_BT_SPLIT_FLAG);
    com_mcpy_SBAC_CTX_MODEL1((dst)->split_dir, (src)->split_dir, NUM_SBAC_CTX_SPLIT_DIR);
    com_mcpy_SBAC_CTX_MODEL1((dst)->split_mode, (src)->split_mode, NUM_SBAC_CTX_SPLIT_MODE);
    com_mcpy_SBAC_CTX_MODEL1((dst)->affine_flag, (src)->affine_flag, NUM_SBAC_CTX_AFFINE_FLAG);
    com_mcpy_SBAC_CTX_MODEL1((dst)->affine_mrg_idx, (src)->affine_mrg_idx, NUM_SBAC_CTX_AFFINE_MRG);
#if SMVD
    com_mcpy_SBAC_CTX_MODEL1((dst)->smvd_flag, (src)->smvd_flag, NUM_SBAC_CTX_SMVD_FLAG);
#endif
#if DT_SYNTAX
    com_mcpy_SBAC_CTX_MODEL1((dst)->part_size, (src)->part_size, NUM_SBAC_CTX_PART_SIZE);
#endif
    com_mcpy_SBAC_CTX_MODEL1((dst)->sao_merge_flag, (src)->sao_merge_flag, NUM_SAO_MERGE_FLAG_CTX);
    com_mcpy_SBAC_CTX_MODEL1((dst)->sao_mode, (src)->sao_mode, NUM_SAO_MODE_CTX);
    com_mcpy_SBAC_CTX_MODEL1((dst)->sao_offset, (src)->sao_offset, NUM_SAO_OFFSET_CTX);
    com_mcpy_SBAC_CTX_MODEL1((dst)->alf_lcu_enable, (src)->alf_lcu_enable, NUM_ALF_LCU_CTX);
    com_mcpy_SBAC_CTX_MODEL1((dst)->delta_qp, (src)->delta_qp, NUM_SBAC_CTX_DELTA_QP);
}
void com_mcpy_COM_SBAC_CTX_INTRA_LUMA(COM_SBAC_CTX* dst, COM_SBAC_CTX* src)
{
    com_mcpy_SBAC_CTX_MODEL1((dst)->skip_flag, (src)->skip_flag, NUM_SBAC_CTX_SKIP_FLAG);
    com_mcpy_SBAC_CTX_MODEL1((dst)->direct_flag, (src)->direct_flag, NUM_SBAC_CTX_DIRECT_FLAG);
    com_mcpy_SBAC_CTX_MODEL1((dst)->intra_dir, (src)->intra_dir, NUM_INTRA_DIR_CTX);
    com_mcpy_SBAC_CTX_MODEL1((dst)->pred_mode, (src)->pred_mode, NUM_PRED_MODE_CTX);
#if DT_SYNTAX
    com_mcpy_SBAC_CTX_MODEL1((dst)->part_size, (src)->part_size, NUM_SBAC_CTX_PART_SIZE);
#endif
    com_mcpy_SBAC_CTX_MODEL1((dst)->cbf, (src)->cbf, NUM_QT_CBF_CTX);
    com_mcpy_SBAC_CTX_MODEL1((dst)->run, (src)->run, NUM_SBAC_CTX_RUN);
    com_mcpy_SBAC_CTX_MODEL1((dst)->run_rdoq, (src)->run_rdoq, NUM_SBAC_CTX_RUN);
    com_mcpy_SBAC_CTX_MODEL1((dst)->last1, (src)->last1, NUM_SBAC_CTX_LAST1 * 2);
    com_mcpy_SBAC_CTX_MODEL1((dst)->last2, (src)->last2, NUM_SBAC_CTX_LAST2 * 2 - 2);
    com_mcpy_SBAC_CTX_MODEL1((dst)->level, (src)->level, NUM_SBAC_CTX_LEVEL);
}

void com_mcpy_COM_SBAC_CTX_dir(COM_SBAC_CTX* dst, COM_SBAC_CTX* src)
{
#pragma HLS INLINE off
    com_mcpy_SBAC_CTX_MODEL1((dst)->intra_dir, (src)->intra_dir, NUM_INTRA_DIR_CTX);
    com_mcpy_SBAC_CTX_MODEL1((dst)->pred_mode, (src)->pred_mode, NUM_PRED_MODE_CTX);
}
void com_mcpy_COM_SBAC_CTX_INTRA_CHROMA(COM_SBAC_CTX* dst, COM_SBAC_CTX* src)
{
    com_mcpy_SBAC_CTX_MODEL1((dst)->intra_dir, (src)->intra_dir, NUM_INTRA_DIR_CTX);
    com_mcpy_SBAC_CTX_MODEL1((dst)->pred_mode, (src)->pred_mode, NUM_PRED_MODE_CTX);
    com_mcpy_SBAC_CTX_MODEL1((dst)->cbf, (src)->cbf, NUM_QT_CBF_CTX);
    com_mcpy_SBAC_CTX_MODEL1((dst)->run, (src)->run, NUM_SBAC_CTX_RUN);
    com_mcpy_SBAC_CTX_MODEL1((dst)->run_rdoq, (src)->run_rdoq, NUM_SBAC_CTX_RUN);
    com_mcpy_SBAC_CTX_MODEL1((dst)->last1, (src)->last1, NUM_SBAC_CTX_LAST1 * 2);
    com_mcpy_SBAC_CTX_MODEL1((dst)->last2, (src)->last2, NUM_SBAC_CTX_LAST2 * 2 - 2);
    com_mcpy_SBAC_CTX_MODEL1((dst)->level, (src)->level, NUM_SBAC_CTX_LEVEL);
}


void com_mcpy_COM_SBAC_CTX_INTER_SKIP(COM_SBAC_CTX* dst, COM_SBAC_CTX* src)
{
    com_mcpy_SBAC_CTX_MODEL1((dst)->skip_flag, (src)->skip_flag, NUM_SBAC_CTX_SKIP_FLAG);
    com_mcpy_SBAC_CTX_MODEL1((dst)->skip_idx_ctx, (src)->skip_idx_ctx, NUM_SBAC_CTX_SKIP_IDX);
#if !AWP
    (dst)->umve_flag = (src)->umve_flag;
#endif
    com_mcpy_SBAC_CTX_MODEL1((dst)->umve_base_idx, (src)->umve_base_idx, NUM_SBAC_CTX_UMVE_BASE_IDX);
    com_mcpy_SBAC_CTX_MODEL1((dst)->umve_step_idx, (src)->umve_step_idx, NUM_SBAC_CTX_UMVE_STEP_IDX);
    com_mcpy_SBAC_CTX_MODEL1((dst)->umve_dir_idx, (src)->umve_dir_idx, NUM_SBAC_CTX_UMVE_DIR_IDX);
    com_mcpy_SBAC_CTX_MODEL1((dst)->affine_flag, (src)->affine_flag, NUM_SBAC_CTX_AFFINE_FLAG);

}



void com_mcpy_COM_SBAC_CTX_INTER_rdo_skip(COM_SBAC_CTX* dst, COM_SBAC_CTX* src)
{
    com_mcpy_SBAC_CTX_MODEL1((dst)->skip_flag, (src)->skip_flag, NUM_SBAC_CTX_SKIP_FLAG);
    com_mcpy_SBAC_CTX_MODEL1((dst)->skip_idx_ctx, (src)->skip_idx_ctx, NUM_SBAC_CTX_SKIP_IDX);
    com_mcpy_SBAC_CTX_MODEL1((dst)->direct_flag, (src)->direct_flag, NUM_SBAC_CTX_DIRECT_FLAG);
#if !AWP
    (dst)->umve_flag = (src)->umve_flag;
#endif
    com_mcpy_SBAC_CTX_MODEL1((dst)->umve_base_idx, (src)->umve_base_idx, NUM_SBAC_CTX_UMVE_BASE_IDX);
    com_mcpy_SBAC_CTX_MODEL1((dst)->umve_step_idx, (src)->umve_step_idx, NUM_SBAC_CTX_UMVE_STEP_IDX);
    com_mcpy_SBAC_CTX_MODEL1((dst)->ctp_zero_flag, (src)->ctp_zero_flag, NUM_CTP_ZERO_FLAG_CTX);
    com_mcpy_SBAC_CTX_MODEL1((dst)->cbf, (src)->cbf, NUM_QT_CBF_CTX);
    com_mcpy_SBAC_CTX_MODEL1((dst)->tb_split, (src)->tb_split, NUM_TB_SPLIT_CTX);
    com_mcpy_SBAC_CTX_MODEL1((dst)->run, (src)->run, NUM_SBAC_CTX_RUN);
    com_mcpy_SBAC_CTX_MODEL1((dst)->run_rdoq, (src)->run_rdoq, NUM_SBAC_CTX_RUN);
    com_mcpy_SBAC_CTX_MODEL1((dst)->last1, (src)->last1, NUM_SBAC_CTX_LAST1 * 2);
    com_mcpy_SBAC_CTX_MODEL1((dst)->last2, (src)->last2, NUM_SBAC_CTX_LAST2 * 2 - 2);
    com_mcpy_SBAC_CTX_MODEL1((dst)->level, (src)->level, NUM_SBAC_CTX_LEVEL);
    com_mcpy_SBAC_CTX_MODEL1((dst)->affine_flag, (src)->affine_flag, NUM_SBAC_CTX_AFFINE_FLAG);
    com_mcpy_SBAC_CTX_MODEL1((dst)->affine_mrg_idx, (src)->affine_mrg_idx, NUM_SBAC_CTX_AFFINE_MRG);
    com_mcpy_SBAC_CTX_MODEL1((dst)->umve_dir_idx, (src)->umve_dir_idx, NUM_SBAC_CTX_UMVE_DIR_IDX);
}

void com_mcpy_COM_SBAC_CTX_SAO(COM_SBAC_CTX* dst, COM_SBAC_CTX* src)
{
    com_mcpy_SBAC_CTX_MODEL1((dst)->sao_merge_flag, (src)->sao_merge_flag, NUM_SAO_MERGE_FLAG_CTX);
    com_mcpy_SBAC_CTX_MODEL1((dst)->sao_mode, (src)->sao_mode, NUM_SAO_MODE_CTX);
    com_mcpy_SBAC_CTX_MODEL1((dst)->sao_offset, (src)->sao_offset, NUM_SAO_OFFSET_CTX);
}


void com_mcpy_COM_SBAC_CTX_INTER_COMP(COM_SBAC_CTX* dst, COM_SBAC_CTX* src)
{
    com_mcpy_SBAC_CTX_MODEL1((dst)->ctp_zero_flag, (src)->ctp_zero_flag, NUM_CTP_ZERO_FLAG_CTX);
    com_mcpy_SBAC_CTX_MODEL1((dst)->cbf, (src)->cbf, NUM_QT_CBF_CTX);
    com_mcpy_SBAC_CTX_MODEL1((dst)->tb_split, (src)->tb_split, NUM_TB_SPLIT_CTX);
    com_mcpy_SBAC_CTX_MODEL1((dst)->run, (src)->run, NUM_SBAC_CTX_RUN);
    com_mcpy_SBAC_CTX_MODEL1((dst)->run_rdoq, (src)->run_rdoq, NUM_SBAC_CTX_RUN);
    com_mcpy_SBAC_CTX_MODEL1((dst)->last1, (src)->last1, NUM_SBAC_CTX_LAST1 * 2);
    com_mcpy_SBAC_CTX_MODEL1((dst)->last2, (src)->last2, NUM_SBAC_CTX_LAST2 * 2 - 2);
    com_mcpy_SBAC_CTX_MODEL1((dst)->level, (src)->level, NUM_SBAC_CTX_LEVEL);
}

void com_mcpy_COM_SBAC_CTX_INTER(COM_SBAC_CTX* dst, COM_SBAC_CTX* src)
{
    com_mcpy_SBAC_CTX_MODEL1((dst)->skip_flag, (src)->skip_flag, NUM_SBAC_CTX_SKIP_FLAG);
    com_mcpy_SBAC_CTX_MODEL1((dst)->skip_idx_ctx, (src)->skip_idx_ctx, NUM_SBAC_CTX_SKIP_IDX);
    com_mcpy_SBAC_CTX_MODEL1((dst)->direct_flag, (src)->direct_flag, NUM_SBAC_CTX_DIRECT_FLAG);
#if !AWP
    (dst)->umve_flag = (src)->umve_flag;
#endif
    com_mcpy_SBAC_CTX_MODEL1((dst)->umve_dir_idx, (src)->umve_dir_idx, NUM_SBAC_CTX_UMVE_DIR_IDX);
    com_mcpy_SBAC_CTX_MODEL1((dst)->inter_dir, (src)->inter_dir, NUM_INTER_DIR_CTX);
    com_mcpy_SBAC_CTX_MODEL1((dst)->pred_mode, (src)->pred_mode, NUM_PRED_MODE_CTX);
    com_mcpy_SBAC_CTX_MODEL1((dst)->refi, (src)->refi, NUM_REFI_CTX);
    com_mcpy_SBAC_CTX_MODEL1((dst)->mvr_idx, (src)->mvr_idx, NUM_MVR_IDX_CTX);
#if BD_AFFINE_AMVR
    com_mcpy_SBAC_CTX_MODEL1((dst)->affine_mvr_idx, (src)->affine_mvr_idx, NUM_AFFINE_MVR_IDX_CTX);
#endif
    for (int it = 0; it < 2; it++) {
        for (int jt = 0; jt < NUM_MV_RES_CTX; jt++) {
            (dst)->mvd[it][jt] = (src)->mvd[it][jt];
        }
    }
    com_mcpy_SBAC_CTX_MODEL1((dst)->ctp_zero_flag, (src)->ctp_zero_flag, NUM_CTP_ZERO_FLAG_CTX);
    com_mcpy_SBAC_CTX_MODEL1((dst)->cbf, (src)->cbf, NUM_QT_CBF_CTX);
    com_mcpy_SBAC_CTX_MODEL1((dst)->tb_split, (src)->tb_split, NUM_TB_SPLIT_CTX);
    com_mcpy_SBAC_CTX_MODEL1((dst)->run, (src)->run, NUM_SBAC_CTX_RUN);
    com_mcpy_SBAC_CTX_MODEL1((dst)->run_rdoq, (src)->run_rdoq, NUM_SBAC_CTX_RUN);
    com_mcpy_SBAC_CTX_MODEL1((dst)->last1, (src)->last1, NUM_SBAC_CTX_LAST1 * 2);
    com_mcpy_SBAC_CTX_MODEL1((dst)->last2, (src)->last2, NUM_SBAC_CTX_LAST2 * 2 - 2);
    com_mcpy_SBAC_CTX_MODEL1((dst)->level, (src)->level, NUM_SBAC_CTX_LEVEL);
    com_mcpy_SBAC_CTX_MODEL1((dst)->affine_flag, (src)->affine_flag, NUM_SBAC_CTX_AFFINE_FLAG);
    com_mcpy_SBAC_CTX_MODEL1((dst)->affine_mrg_idx, (src)->affine_mrg_idx, NUM_SBAC_CTX_AFFINE_MRG);
#if SMVD
    com_mcpy_SBAC_CTX_MODEL1((dst)->smvd_flag, (src)->smvd_flag, NUM_SBAC_CTX_SMVD_FLAG);
#endif
#if EXT_AMVR_HMVP
    com_mcpy_SBAC_CTX_MODEL1((dst)->mvp_from_hmvp_flag, (src)->mvp_from_hmvp_flag, NUM_EXTEND_AMVR_FLAG);
#endif
}

void com_mcpy_COM_SBAC_CTX_COEF(COM_SBAC_CTX* dst, COM_SBAC_CTX* src)
{
    com_mcpy_SBAC_CTX_MODEL1((dst)->ctp_zero_flag, (src)->ctp_zero_flag, NUM_CTP_ZERO_FLAG_CTX);
    com_mcpy_SBAC_CTX_MODEL1((dst)->cbf, (src)->cbf, NUM_QT_CBF_CTX);
    com_mcpy_SBAC_CTX_MODEL1((dst)->tb_split, (src)->tb_split, NUM_TB_SPLIT_CTX);
    com_mcpy_SBAC_CTX_MODEL1((dst)->run, (src)->run, NUM_SBAC_CTX_RUN);
    com_mcpy_SBAC_CTX_MODEL1((dst)->run_rdoq, (src)->run_rdoq, NUM_SBAC_CTX_RUN);
    com_mcpy_SBAC_CTX_MODEL1((dst)->last1, (src)->last1, NUM_SBAC_CTX_LAST1 * 2);
    com_mcpy_SBAC_CTX_MODEL1((dst)->last2, (src)->last2, NUM_SBAC_CTX_LAST2 * 2 - 2);
    com_mcpy_SBAC_CTX_MODEL1((dst)->level, (src)->level, NUM_SBAC_CTX_LEVEL);
}

static void com_mcpy_ENC_SBAC(ENC_SBAC* dst, ENC_SBAC* src)
{
    (dst)->range = (src)->range;
    (dst)->code = (src)->code;
    (dst)->left_bits = (src)->left_bits;
    (dst)->stacked_ff = (src)->stacked_ff;
    (dst)->pending_byte = (src)->pending_byte;
    (dst)->is_pending_byte = (src)->is_pending_byte;
    com_mcpy_COM_SBAC_CTX(&(dst)->ctx, &(src)->ctx);
    (dst)->bitcounter = (src)->bitcounter;

}
void com_mcpy_ENC_SBAC_INTER_SKIP(ENC_SBAC* dst, ENC_SBAC* src)
{
    (dst)->range = (src)->range;
    (dst)->code = (src)->code;
    (dst)->left_bits = (src)->left_bits;
    (dst)->stacked_ff = (src)->stacked_ff;
    (dst)->pending_byte = (src)->pending_byte;
    (dst)->is_pending_byte = (src)->is_pending_byte;
    com_mcpy_COM_SBAC_CTX_INTER_SKIP(&(dst)->ctx, &(src)->ctx);
    (dst)->bitcounter = (src)->bitcounter;

}
void com_mcpy_ENC_SBAC_INTER_rdo_skip(ENC_SBAC* dst, ENC_SBAC* src)
{
    (dst)->range = (src)->range;
    (dst)->code = (src)->code;
    (dst)->left_bits = (src)->left_bits;
    (dst)->stacked_ff = (src)->stacked_ff;
    (dst)->pending_byte = (src)->pending_byte;
    (dst)->is_pending_byte = (src)->is_pending_byte;
    com_mcpy_COM_SBAC_CTX_INTER_rdo_skip(&(dst)->ctx, &(src)->ctx);
    (dst)->bitcounter = (src)->bitcounter;

}
void com_mcpy_ENC_SBAC_INTER_COMP(ENC_SBAC* dst, ENC_SBAC* src)
{
    (dst)->range = (src)->range;
    (dst)->code = (src)->code;
    (dst)->left_bits = (src)->left_bits;
    (dst)->stacked_ff = (src)->stacked_ff;
    (dst)->pending_byte = (src)->pending_byte;
    (dst)->is_pending_byte = (src)->is_pending_byte;
    com_mcpy_COM_SBAC_CTX_INTER_COMP(&(dst)->ctx, &(src)->ctx);
    (dst)->bitcounter = (src)->bitcounter;

}
void com_mcpy_ENC_SBAC_INTER(ENC_SBAC* dst, ENC_SBAC* src)
{
    (dst)->range = (src)->range;
    (dst)->code = (src)->code;
    (dst)->left_bits = (src)->left_bits;
    (dst)->stacked_ff = (src)->stacked_ff;
    (dst)->pending_byte = (src)->pending_byte;
    (dst)->is_pending_byte = (src)->is_pending_byte;
    com_mcpy_COM_SBAC_CTX_INTER(&(dst)->ctx, &(src)->ctx);
    (dst)->bitcounter = (src)->bitcounter;

}
void com_mcpy_ENC_SBAC_COEF(ENC_SBAC* dst, ENC_SBAC* src)
{
    (dst)->range = (src)->range;
    (dst)->code = (src)->code;
    (dst)->left_bits = (src)->left_bits;
    (dst)->stacked_ff = (src)->stacked_ff;
    (dst)->pending_byte = (src)->pending_byte;
    (dst)->is_pending_byte = (src)->is_pending_byte;
    com_mcpy_COM_SBAC_CTX_COEF(&(dst)->ctx, &(src)->ctx);
    (dst)->bitcounter = (src)->bitcounter;

}
void com_mcpy_ENC_SBAC_INTRA_LUMA(ENC_SBAC* dst, ENC_SBAC* src)
{
    (dst)->range = (src)->range;
    (dst)->code = (src)->code;
    (dst)->left_bits = (src)->left_bits;
    (dst)->stacked_ff = (src)->stacked_ff;
    (dst)->pending_byte = (src)->pending_byte;
    (dst)->is_pending_byte = (src)->is_pending_byte;
    com_mcpy_COM_SBAC_CTX_INTRA_LUMA(&(dst)->ctx, &(src)->ctx);
    (dst)->bitcounter = (src)->bitcounter;
}
void com_mcpy_ENC_SBAC_INTRA_CHROMA(ENC_SBAC* dst, ENC_SBAC* src)
{
    (dst)->range = (src)->range;
    (dst)->code = (src)->code;
    (dst)->left_bits = (src)->left_bits;
    (dst)->stacked_ff = (src)->stacked_ff;
    (dst)->pending_byte = (src)->pending_byte;
    (dst)->is_pending_byte = (src)->is_pending_byte;
    com_mcpy_COM_SBAC_CTX_INTRA_CHROMA(&(dst)->ctx, &(src)->ctx);
    (dst)->bitcounter = (src)->bitcounter;

}
void com_mcpy_ENC_SBAC_dir(ENC_SBAC* dst, ENC_SBAC* src)
{
#pragma HLS INLINE off
    (dst)->range = (src)->range;
    (dst)->code = (src)->code;
    (dst)->left_bits = (src)->left_bits;
    (dst)->stacked_ff = (src)->stacked_ff;
    (dst)->pending_byte = (src)->pending_byte;
    (dst)->is_pending_byte = (src)->is_pending_byte;
    com_mcpy_COM_SBAC_CTX_dir(&(dst)->ctx, &(src)->ctx);
    (dst)->bitcounter = (src)->bitcounter;

}