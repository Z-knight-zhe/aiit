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

#ifndef _ENC_ECO_H_
#define _ENC_ECO_H_

//#ifdef __cplusplus
//extern "C"
//{
//#endif
#include "enc_def.h"

#define GET_SBAC_ENC(bs)   ((ENC_SBAC *)(bs)->pdata[1])
#define MODIFY_ZXZ 1
int enc_eco_cnkh(COM_BSW* bs, COM_CNKH* cnkh);
int enc_eco_sqh(ENC_CTX* ctx, COM_BSW* bs, COM_SQH* sqh);
int enc_eco_pic_header(COM_BSW* bs, COM_PIC_HEADER* sh, COM_SQH* sqh);
int enc_eco_patch_header(COM_BSW* bs, COM_SQH* sqh, COM_PIC_HEADER* ph, COM_SH_EXT* sh, u8 patch_idx, PATCH_INFO* patch);
#if PATCH
int enc_eco_send(COM_BSW* bs);
#endif
int enc_eco_udata(ENC_CTX* ctx, COM_BSW* bs);
//int encode_pred_mode(COM_BSW * bs, ENC_SBAC *sbac, u8 pred_mode, COM_MODE *mod_info_curr_ptr, COM_INFO* info, ENC_ME_LINE_MAP* me_line_ptr);
//int encode_ipf_flag(COM_BSW *bs, ENC_SBAC *sbac, u8 ipf_flag);
//void enc_eco_unit_xu(ENC_CTX * ctx, ENC_CORE *core, int cup, u8 pred_mode, int cu_width, int cu_height, COM_BSW *bs, ENC_CU_DATA *cu_data, int pb_part, u8 affine_flag, int slice_type, int refi0, int refi1);

int encode_mvd(COM_BSW* bs, ENC_SBAC* sabc, s16 mvd[MV_D]);
void enc_sbac_init(COM_BSW* bs);
int ace_get_log2(int v);

void enc_sbac_finish(COM_BSW* bs);
//void enc_sbac_encode_bin(u32 bin, ENC_SBAC* sbac, SBAC_CTX_MODEL* ctx_model, COM_BSW* bs);
void aec_enc_sbac_encode_bin(u32 bin, ENC_SBAC* sbac, SBAC_CTX_MODEL* ctx_model, COM_BSW* bs);

#if PATCH_DEBUG
void sbac_put_byte1(u8 writing_byte, ENC_SBAC* sbac, COM_BSW* bs, int* bs_cur);
void Demulate1(COM_BSW* bs, COM_BSW* ctx_bs, int* bs_cur, int bs_start_pos);

#endif

//void enc_sbac_encode_bin_trm(u32 bin, ENC_SBAC *sbac, COM_BSW *bs);

//int encode_coef(COM_BSW * bs, ENC_SBAC *sbac, s16 coef[N_C][MAX_CU_DIM], int cu_width_log2, int cu_height_log2, u8 pred_mode, u8 tree_status, COM_MODE *mod_info_curr_ptr, int bit_depth_input, u8 position_based_transform_enable_flag);

//#if CHROMA_NOT_SPLIT
//int enc_eco_unit_chroma(ENC_CTX * ctx, ENC_CORE *core, ENC_CORE_ARRARY *core_inner, int x, int y, int cup, int cu_width, int cu_height, int f_lcu);
//#endif

//#if MODE_CONS
//int  enc_eco_cons_pred_mode_child(COM_BSW * bs, ENC_CORE_ARRARY *core_inner, u8 cons_pred_mode_child);
//void aec_enc_eco_cons_pred_mode_child(COM_BSW * bs, u8 cons_pred_mode_child);
//#endif
int enc_extension_and_user_data(ENC_CTX* ctx, COM_BSW* bs, int i, u8 isExtension, COM_SQH* sqh, COM_PIC_HEADER* pic_header);

void enc_eco_lcu_delta_qp(COM_BSW* bs, int val, int last_dqp);
//void enc_eco_slice_end_flag(COM_BSW * bs, int flag);

int encode_refidx(COM_BSW* bs, ENC_SBAC* sbac, int num_refp, int refi);


void encode_skip_flag(COM_BSW* bs, ENC_SBAC* sbac, int flag, COM_MODE* mod_info_curr_ptr, int f_scu, COM_INFO* info, ENC_ME_LINE_MAP* me_line_ptr);
void encode_skip_flag_cu(COM_BSW* bs, ENC_SBAC* sbac, int flag, u32 map_scu_a, u32 map_scu_b, int x_scu, int y_scu, int cu_width, int cu_height);
void encode_umve_flag(COM_BSW* bs, ENC_SBAC* sbac, int flag);
void encode_affine_flag_cu(COM_BSW* bs, ENC_SBAC* sbac, int flag, COM_MODE* mod_info_curr_ptr);
void encode_umve_idx(COM_BSW* bs, int umve_idx);
//int encode_skip_flag_skip(ENC_SBAC * sbac, int flag, u32 map_scu_a, u32 map_scu_b, int x_scu, int y_scu, int cu_width, int cu_height);
//int encode_umve_flag_skip(ENC_SBAC *sbac, int flag);
//int encode_umve_idx_skip(ENC_SBAC *sbac, int umve_idx);
//void encode_skip_idx_cu(COM_BSW *bs, ENC_SBAC *sbac, int skip_idx, int num_hmvp_cands, u8 slice_type);
//int encode_skip_idx_skip(ENC_SBAC *sbac, int skip_idx, int num_hmvp_cands);
//int encode_affine_flag_skip(ENC_SBAC *sbac, int flag, int cu_width, int cu_height);

//void enc_eco_split_flag(u32 *map_scu, u32 *map_cu_mode, int cu_width, int cu_height, int x, int y, COM_BSW * bs, ENC_SBAC *sbac, int flag, int pic_width);
void encode_skip_idx(COM_BSW* bs, int skip_idx, int num_hmvp_cands,
#if MVAP
    int num_mvap_cands,
#endif
    ENC_CTX_HLS* ctx);
void encode_direct_flag(COM_BSW* bs, ENC_SBAC* sbac, int direct_flag, COM_MODE* mod_info_curr_ptr);
//! \todo Change list of arguments
void enc_eco_xcoef(COM_BSW* bs, ENC_SBAC* sbac, s16* coef, int log2_w, int log2_h, int num_sig, int ch_type);
//! \todo Change list of arguments
int encode_intra_dir(COM_BSW* bs, u8 ipm,
#if EIPM
    u8 eipm_flag,
#endif
    u8 mpm[2]
);
int encode_intra_dir_c(u8 ipm, u8 ipm_l
#if TSCPM
    , u8 tscpm_enable_flag
#endif
#if ENHANCE_TSPCM
    , u8 enhance_tscpm_enable_flag
#endif
#if PMC || EPMC
    , u8 pmc_enable_flag
#endif
#if SAWP
    , u8 sawp_flag
#endif // SAWP
);
int encode_mvr_idx(COM_BSW* bs, ENC_SBAC* sbac, u8 mvr_idx, BOOL is_affine_mode);

#if EXT_AMVR_HMVP
void encode_extend_amvr_flag(COM_BSW* bs, ENC_SBAC* sbac, u8 mvp_from_hmvp_flag);
#endif

void encode_affine_flag(COM_BSW* bs, ENC_SBAC* sbac, int flag, ENC_CTX_HLS* ctx, int cu_width, int cu_height);
void encode_affine_mrg_idx(COM_BSW* bs, ENC_SBAC* sbac, s16 affine_mrg_idx, ENC_CTX_HLS* ctx);

#if SMVD
void encode_smvd_flag(COM_BSW* bs, ENC_SBAC* sbac, int flag);
#endif

#if DT_SYNTAX

void encode_part_size(ENC_SBAC* sbac, COM_BSW* bs, int part_size, int cu_w, int cu_h, int pred_mode, u8 max_dt_size, u8 dt_intra_enable_flag);

#endif

#if USE_ENH
int enc_eco_DB_param(COM_BSW* bs, COM_PIC_HEADER* sh
#if DBK_SCC || DBR
    , COM_SQH* sqh
#endif
);
#else
int enc_eco_DB_param(COM_BSW* bs, COM_PIC_HEADER* sh);
#endif




//void RDO_sao_mergeflag_AEC(int value1, int value2, int* pbin_count); //ENC_CTX* ctx);
//void RDO_sao_mode_AEC(int value1, int* pbin_count);//ENC_CTX* ctx);   int* pbin_count);
//int RDO_sao_offset(SAOBlkParam* saoBlkParam, int* pbin_count, int EO_OFFSET_MAP[8]);//ENC_CTX* ctx);
//void RDO_sao_type_AEC(int value1, int value2, int* pbin_count); //ENC_CTX* ctx);
//void RDO_sao_type(SAOBlkParam* saoBlkParam, int* pbin_count); //ENC_CTX* ctx);
//void RDO_sao_offset_AEC(int value1, int value2, int* pbin_count, int EO_OFFSET_MAP[8]); //ENC_CTX* ctx);

void Demulate(COM_BSW* bs);
void Demulate_finish(COM_BSW* bs);
//#ifdef __cplusplus
//}
//#endif
#endif /* _ENC_ECO_H_ */

#define GET_SBAC_ENC(bs)   ((ENC_SBAC *)(bs)->pdata[1])

//int enc_eco_run_length_cc_est_2(s16 *coef, int log2_w, int log2_h, int num_sig, int ch_type);
int enc_eco_run_length_cc_est_2_1(s16* coef, int log2_w, int log2_h, int num_sig, int ch_type);
void enc_sbac_encode_binW_model(u32 bin, ENC_SBAC* sbac, SBAC_CTX_MODEL* model1, SBAC_CTX_MODEL* model2, COM_BSW* bs);//huwq
int enc_eco_level_model(u32 sym, ENC_SBAC* sbac, SBAC_CTX_MODEL* model, COM_BSW* bs);//huwq
int enc_eco_run_model(u32 sym, ENC_SBAC* sbac, SBAC_CTX_MODEL* model, COM_BSW* bs);
