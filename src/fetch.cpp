#include <assert.h>
#include <math.h>
#include <stdio.h>
#include "../inc/enc_def.h"
#include "../inc/fetch.h"
#include "../inc/type_def.h"
#include "../inc/rfc.hpp"
#include "../inc/rfc_dcp.hpp"
#include "../inc/aec.h"
//#include "../inc/rfc_dcp.hpp"
#pragma warning(disable:4102)
extern FRAME_LEVEL_FW frame_level_fw;
#if PATCH_DEBUG
#include "patch.h"

static void com_mcpy_SBAC_CTX_MODEL1(SBAC_CTX_MODEL* dst, SBAC_CTX_MODEL* src, int sz)
{

//#pragma HLS INLINE off
    for (int i = 0; i < sz; i++) {

#pragma HLS LOOP_TRIPCOUNT max=24
        dst[i] = src[i];
    }
}
//
static void com_mcpy_COM_SBAC_CTX(COM_SBAC_CTX* dst, COM_SBAC_CTX* src)
{
    com_mcpy_SBAC_CTX_MODEL1(dst->skip_flag, src->skip_flag, 4);
    com_mcpy_SBAC_CTX_MODEL1(dst->skip_idx_ctx, src->skip_idx_ctx, ((int)PRED_DIR_NUM) + 1 + 8 - 1);
    com_mcpy_SBAC_CTX_MODEL1(dst->direct_flag, src->direct_flag, 2);
    dst->umve_flag = src->umve_flag;
    com_mcpy_SBAC_CTX_MODEL1(dst->umve_base_idx, src->umve_base_idx, 1);
    com_mcpy_SBAC_CTX_MODEL1(dst->umve_step_idx, src->umve_step_idx, 1);
    com_mcpy_SBAC_CTX_MODEL1(dst->umve_dir_idx, src->umve_dir_idx, 2);
    com_mcpy_SBAC_CTX_MODEL1(dst->inter_dir, src->inter_dir, 3);
    com_mcpy_SBAC_CTX_MODEL1(dst->intra_dir, src->intra_dir, 10);
    com_mcpy_SBAC_CTX_MODEL1(dst->pred_mode, src->pred_mode, 6);
    com_mcpy_SBAC_CTX_MODEL1(dst->cons_mode, src->cons_mode, 1);
    com_mcpy_SBAC_CTX_MODEL1(dst->ipf_flag, src->ipf_flag, 1);
    com_mcpy_SBAC_CTX_MODEL1(dst->refi, src->refi, 3);
    com_mcpy_SBAC_CTX_MODEL1(dst->mvr_idx, src->mvr_idx, 5 - 1);
    com_mcpy_SBAC_CTX_MODEL1(dst->affine_mvr_idx, src->affine_mvr_idx, 3 - 1);
    com_mcpy_SBAC_CTX_MODEL1(dst->mvp_from_hmvp_flag, src->mvp_from_hmvp_flag, 1);
    for (int it = 0; it < 2; it++) {

#pragma HLS loop_tripcount max = 2
        for (int jt = 0; jt < 3; jt++) {

#pragma HLS loop_tripcount max = 3
            dst->mvd[it][jt] = src->mvd[it][jt];
        }
    }
    com_mcpy_SBAC_CTX_MODEL1(dst->ctp_zero_flag, src->ctp_zero_flag, 2);
    com_mcpy_SBAC_CTX_MODEL1(dst->cbf, src->cbf, 3);
    com_mcpy_SBAC_CTX_MODEL1(dst->tb_split, src->tb_split, 1);
    com_mcpy_SBAC_CTX_MODEL1(dst->run, src->run, 6 * 4);
    com_mcpy_SBAC_CTX_MODEL1(dst->run_rdoq, src->run_rdoq, 6 * 4);
    com_mcpy_SBAC_CTX_MODEL1(dst->last1, src->last1, 6 * 2);
    com_mcpy_SBAC_CTX_MODEL1(dst->last2, src->last2, 12 * 2 - 2);
    com_mcpy_SBAC_CTX_MODEL1(dst->level, src->level, 6 * 4);
    com_mcpy_SBAC_CTX_MODEL1(dst->split_flag, src->split_flag, 4);
    com_mcpy_SBAC_CTX_MODEL1(dst->bt_split_flag, src->bt_split_flag, 9);
    com_mcpy_SBAC_CTX_MODEL1(dst->split_dir, src->split_dir, 5);
    com_mcpy_SBAC_CTX_MODEL1(dst->split_mode, src->split_mode, 3);
    com_mcpy_SBAC_CTX_MODEL1(dst->affine_flag, src->affine_flag, 1);
    com_mcpy_SBAC_CTX_MODEL1(dst->affine_mrg_idx, src->affine_mrg_idx, 5 - 1);
    com_mcpy_SBAC_CTX_MODEL1(dst->smvd_flag, src->smvd_flag, 1);
    com_mcpy_SBAC_CTX_MODEL1(dst->part_size, src->part_size, 6);
    com_mcpy_SBAC_CTX_MODEL1(dst->sao_merge_flag, src->sao_merge_flag, 3);
    com_mcpy_SBAC_CTX_MODEL1(dst->sao_mode, src->sao_mode, 1);
    com_mcpy_SBAC_CTX_MODEL1(dst->sao_offset, src->sao_offset, 1);
    com_mcpy_SBAC_CTX_MODEL1(dst->alf_lcu_enable, src->alf_lcu_enable, 1);
    com_mcpy_SBAC_CTX_MODEL1(dst->delta_qp, src->delta_qp, 4);
}

static void com_mcpy_ENC_SBAC(ENC_SBAC* dst, ENC_SBAC* src)
{
    dst->range = src->range;
    dst->code = src->code;
    dst->left_bits = src->left_bits;
    dst->stacked_ff = src->stacked_ff;
    dst->pending_byte = src->pending_byte;
    dst->is_pending_byte = src->is_pending_byte;
    com_mcpy_COM_SBAC_CTX(&dst->ctx, &src->ctx);
    dst->bitcounter = src->bitcounter;
}

static int init_cu_data_ctu(ENC_CU_DATA* cu_data, int cu_width_log2, int cu_height_log2)
{
    int i;
    int j;
    int m;
    int cuw_scu;
    int cuh_scu;
    cuw_scu = 1 << (cu_width_log2 - 2);
    cuh_scu = 1 << (cu_height_log2 - 2);
    //add by Luke
    for (i = 0; i < 5; i++) {
        for (j = 0; j < ((int)NUM_BLOCK_SHAPE); j++) {
            for (m = 0; m < cuw_scu * cuh_scu; m++) {
#pragma HLS LOOP_TRIPCOUNT MAX=256

                cu_data->split_mode[i][j][m] = ((s8)0);
            }
        }
    }
    //com_mset(cu_data->mpm[0], 0, cuw_scu * cuh_scu * sizeof(u8));
        /*com_mset(cu_data->mpm[1], 0, cuw_scu * cuh_scu * sizeof(u8));
            com_mset(cu_data->ipm[0], 0, cuw_scu * cuh_scu * sizeof(s8));
            com_mset(cu_data->ipm[1], 0, cuw_scu * cuh_scu * sizeof(s8));
            com_mset(cu_data->ipf_flag, 0, cuw_scu * cuh_scu * sizeof(u8));*/
    for (m = 0; m < cuw_scu * cuh_scu; m++) {
#pragma HLS LOOP_TRIPCOUNT MAX=256
        cu_data->mpm[0][m] = ((u8)0);
        cu_data->mpm[1][m] = ((u8)0);
        cu_data->ipm[0][m] = ((s8)0);
        cu_data->ipm[1][m] = ((s8)0);
        cu_data->ipf_flag[m] = ((u8)0);
    }
    /*for (i = 0; i < 8; i++)
        {
        com_mset(cu_data->mpm_ext[i], 0, cuw_scu * cuh_scu * sizeof(u8));
        }*/
    for (i = 0; i < 8; i++) {
        for (m = 0; m < cuw_scu * cuh_scu; m++) {
#pragma HLS LOOP_TRIPCOUNT MAX=256
            cu_data->mpm_ext[i][m] = ((u8)0);
        }
    }
    return 0;
}
//extern Slice_Level_Parameters ctu_ctrl_input;

//extern CTU_Level_Parameters fetch_output;
static const int com_tbl_qp_chroma_adjust[64] = { (0), (1), (2), (3), (4), (5), (6), (7), (8), (9), (10), (11), (12), (13), (14), (15), (16), (17), (18), (19), (20), (21), (22), (23), (24), (25), (26), (27), (28), (29), (30), (31), (32), (33), (34), (35), (36), (37), (38), (39), (40), (41), (42), (42), (43), (43), (44), (44), (45), (45), (46), (46), (47), (47), (48), (48), (48), (49), (49), (49), (50), (50), (50), (51) };


extern int Demulate_start_pos;
extern COM_BSW* bs_patch;
extern ENC_CTX* ctx_patch;
extern PATCH_INFO* patch;
extern int patch_cur_idx;
extern int patch_cur_lcu_x;
extern int patch_cur_lcu_y;
extern AEC_COM_SBAC_CTX aec_ctx;
extern AEC_output AEC_DATA_output;
extern ENC_STAT* stat_global;
extern COM_PIC_HEADER* pic_header_g;
extern u32* map_scu_temp;
extern s8(*map_refi_temp)[REFP_NUM];
extern s16(*map_mv_temp)[REFP_NUM][MV_D];
extern u32* map_cu_mode_temp;
extern ENC_ME_LINE_MAP* me_line_ptr_patch;
extern ENC_ME_NEB_INFO inner_nebs[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN];
extern ENC_SBAC aec_sbac_local;

void enc_sbac_init1(ENC_SBAC* sbac)
{
    sbac->range = 0x1FF;
    sbac->code = 0;
    sbac->left_bits = 23;
    sbac->pending_byte = 0;
    sbac->is_pending_byte = 0;
    sbac->stacked_ff = 0;
}
void set_sh1(ENC_CTX* ctx, COM_SH_EXT* shext, s8* sao_enable_patch)
{
    shext->fixed_slice_qp_flag = (u8)ctx->info.pic_header.fixed_picture_qp_flag;
    shext->slice_qp = (u8)ctx->info.pic_header.picture_qp;
    shext->slice_sao_enable[Y_C] = sao_enable_patch[Y_C];
    shext->slice_sao_enable[U_C] = sao_enable_patch[U_C];
    shext->slice_sao_enable[V_C] = sao_enable_patch[V_C];
}
void com_sbac_ctx_init1(AEC_COM_SBAC_CTX* sbac_ctx)
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

    for (i = 0; i < 2; i++) {
        sbac_ctx->direct_flag[i] = PROB_INIT;
        sbac_ctx->umve_dir_idx[i] = PROB_INIT;
        sbac_ctx->affine_mvr_idx[i] = PROB_INIT;
        sbac_ctx->ctp_zero_flag[i] = PROB_INIT;
        for (int j = 0; j < 3; j++) {
            sbac_ctx->mvd[i][j] = PROB_INIT;
        }
    }
    for (i = 0; i < 3; i++) {
        sbac_ctx->inter_dir[i] = PROB_INIT;
        sbac_ctx->cbf[i] = PROB_INIT;
        sbac_ctx->split_mode[i] = PROB_INIT;
        sbac_ctx->refi[i] = PROB_INIT;
        sbac_ctx->sao_merge_flag[i] = PROB_INIT;
    }
    for (i = 0; i < 4; i++) {
        sbac_ctx->skip_flag[i] = PROB_INIT;
        sbac_ctx->mvr_idx[i] = PROB_INIT;
        sbac_ctx->affine_mrg_idx[i] = PROB_INIT;
        sbac_ctx->delta_qp[i] = PROB_INIT;
        sbac_ctx->split_flag[i] = PROB_INIT;
    }
    for (i = 0; i < 5; i++) {
        sbac_ctx->split_dir[i] = PROB_INIT;
    }
    for (i = 0; i < 6; i++) {
        sbac_ctx->pred_mode[i] = PROB_INIT;
        sbac_ctx->part_size[i] = PROB_INIT;
    }
    for (i = 0; i < 9; i++) {
        sbac_ctx->bt_split_flag[i] = PROB_INIT;
    }
    for (i = 0; i < 10; i++) {
        sbac_ctx->intra_dir[i] = PROB_INIT;
    }
    for (i = 0; i < 11; i++) {
        sbac_ctx->skip_idx_ctx[i] = PROB_INIT;
    }
    for (i = 0; i < 12; i++) {
        sbac_ctx->last1[i] = PROB_INIT;
    }
    for (i = 0; i < 22; i++) {
        sbac_ctx->last2[i] = PROB_INIT;
    }
    for (i = 0; i < 24; i++) {
        sbac_ctx->run[i] = PROB_INIT;
        sbac_ctx->run_rdoq[i] = PROB_INIT;
        sbac_ctx->level[i] = PROB_INIT;
    }
}
void init_neb_info(ENC_ME_NEB_INFO* neb);

#endif

#if 1
static void firmware_parameters_get(
    int* tmp_pix_x, 
    int* tmp_pix_y, 
    int* lcu_x,
    int* lcu_y,
    int* tmp_smb_num_width, 
    int* tmp_smb_num_height,
    CTU_Level_Parameters* fetch_output,
   // int* lcu_x, 
   // int* lcu_y,
    Slice_Level_Parameters* ctu_ctrl_input, 
    CTX_FETCH* fetch_ctx_io, 
    FETCH2SAO_FW* fetch2sao_fw_ptr
#if PATCH_DEBUG
    PATCH_INFO* patch
#endif
)
{
    static int core_x_lcu_tmp, core_y_lcu_tmp;
    static int core_x_pel, core_y_pel;

    if (ctu_ctrl_input->slice_enable) {
       // ctu_ctrl_input->slice_enable = ((unsigned char)0);
       // *lcu_x = 0;
       // *lcu_y = 0;
        core_x_lcu_tmp = 0;
        core_y_lcu_tmp = 0;
    }
    else {

        core_x_lcu_tmp++;
    }

    int img_width = fetch_ctx_io->ctx_info_pic_width;
    int img_height = fetch_ctx_io->ctx_info_pic_height;
    int lcu_width = (img_width + 63) >> 6;
#if PATCH_DEBUG == 0
    if (core_x_lcu_tmp == lcu_width) {

        core_y_lcu_tmp++;
        core_x_lcu_tmp = 0;
    }
#else
    int num_of_patch = patch->columns * patch->rows;
    patch_cur_idx = patch->idx;
    if (core->x_lcu >= *(patch->width_in_lcu + patch->x_pat) + patch_cur_lcu_x)
    {
        core->x_lcu = patch_cur_lcu_x;
        (core->y_lcu)++;
        if (core->y_lcu >= *(patch->height_in_lcu + patch->y_pat) + patch_cur_lcu_y)
        {
            // a new patch starts
            //core->cnt_hmvp_cands = 0;
            patch->patch_switch = 1;
            patch->pat = 0;

            int start_pos = AEC_DATA_output.bs_cur;
            aec_enc_sbac_encode_bin_trm1(1, &aec_sbac_local, &AEC_DATA_output.bs, &AEC_DATA_output.bs_cur);
            aec_enc_sbac_finish1(&aec_sbac_local, &AEC_DATA_output.bs, &AEC_DATA_output.bs_cur);
            en_copy_lcu_scu(map_scu_temp, ctx_patch->map.map_scu, map_refi_temp, ctx_patch->map.map_refi, map_mv_temp, ctx_patch->map.map_mv, map_cu_mode_temp, ctx_patch->map.map_cu_mode, ctx_patch->patch, ctx_patch->info.pic_width, ctx_patch->info.pic_height);

            for (int k1 = 0; k1 < ctx_patch->info.f_scu; k1++)
            {
                //save data for dbk
                //map_scu_for_dbk_patch[k1] = ctx_patch->map.map_scu[k1];
                ctx_patch->map.map_scu[k1] = 0;
                ctx_patch->map.map_cu_mode[k1] = 0;
            }
            int i, j;

            for (i = 0; i < 1 << 7 >> 1 >> 2; i++) {
//#pragma HLS pipeline
                //#pragma HLS loop_tripcount max = 8
                for (j = 0; j < 1 << 7 >> 1 >> 2; j++) {

                    //#pragma HLS loop_tripcount max = 2
                    me_line_ptr_patch->map_scu_inner[0][i][j] = ((u32)0);
                    me_line_ptr_patch->map_cu_mode_inner[0][i][j] = ((u32)0);
                    init_neb_info(&inner_nebs[0][i][j]);
                }
            }
            for (i = 0; i < 1 << 7 >> 1 >> 2; i++) {

//#pragma HLS pipeline
                //#pragma HLS loop_tripcount max = 8
                for (j = 0; j < 1 << 7 >> 1 >> 2; j++) {

                    //#pragma HLS loop_tripcount max = 2
                    me_line_ptr_patch->map_scu_inner[1][i][j] = ((u32)0);
                    me_line_ptr_patch->map_cu_mode_inner[1][i][j] = ((u32)0);
                    init_neb_info(&inner_nebs[1][i][j]);
                }
            }

            /*set patch head*/
            if (patch_cur_idx + 1 < num_of_patch)
            {
                set_sh1(ctx_patch, &ctx_patch->info.shext, patch->patch_sao_enable + (patch_cur_idx + 1) * N_C);
            }
            /* initialize entropy coder */
            enc_sbac_init1(&aec_sbac_local);
            com_sbac_ctx_init1(&aec_ctx);
            //com_sbac_ctx_init(&(core->s_curr_best[ctx_patch->info.log2_max_cuwh - 2][ctx_patch->info.log2_max_cuwh - 2].ctx_patch));

            if (ctx_patch->lcu_cnt > 1)
            {
                Demulate1(&AEC_DATA_output.bs, &ctx_patch->bs, &AEC_DATA_output.bs_cur, Demulate_start_pos);
                /*encode patch end*/
                int ret = aec_enc_eco_send(&AEC_DATA_output.bs, &ctx_patch->bs, &AEC_DATA_output.bs_cur);
                /* Encode patch header */
                int patch_idx = patch->idx + 1;
                AEC_DATA_output.bs.code = 0;
                AEC_DATA_output.bs.leftbits = 32;
                Demulate_start_pos = AEC_DATA_output.bs_cur;
                aec_com_bsw_flush1(&AEC_DATA_output.bs, &AEC_DATA_output.bs_cur);

                ret = aec_enc_eco_patch_header(&AEC_DATA_output.bs, &ctx_patch->bs, &ctx_patch->info.sqh, pic_header_g, &ctx_patch->info.shext, patch_idx, patch, &AEC_DATA_output.bs_cur);
                set_sh1(ctx_patch, &ctx_patch->info.shext, patch->patch_sao_enable + patch_idx * N_C);

                for (int i = 0; i < AEC_DATA_output.bs_cur - start_pos; i++)
                {
                    bs_patch->beg[start_pos + i + patch->len] = AEC_DATA_output.bs.beg[start_pos + i];
                }

                AEC_DATA_output.bs.uiStartPos = AEC_DATA_output.bs_cur;
                AEC_DATA_output.bs.CurAddress = AEC_DATA_output.bs_cur;

            }
            ///*update and store map_scu*/
            en_copy_lcu_scu(map_scu_temp, ctx_patch->map.map_scu, map_refi_temp, ctx_patch->map.map_refi, map_mv_temp, ctx_patch->map.map_mv, map_cu_mode_temp, ctx_patch->map.map_cu_mode, ctx_patch->patch, ctx_patch->info.pic_width, ctx_patch->info.pic_height);

            enc_sbac_init1(&aec_sbac_local);


            core->x_lcu = *(patch->width_in_lcu + patch->x_pat) + patch_cur_lcu_x;
            core->y_lcu = patch_cur_lcu_y;
            patch->x_pat++;
            if (core->x_lcu >= ctx_patch->info.pic_width_in_lcu)
            {
                core->x_lcu = 0;
                core->y_lcu = *(patch->height_in_lcu + patch->y_pat) + patch_cur_lcu_y;
                patch->y_pat++;
                patch->x_pat = 0;
            }
            patch->idx++;
            patch_cur_lcu_x = core->x_lcu;
            patch_cur_lcu_y = core->y_lcu;
            core->x_pel = core->x_lcu << ctx_patch->info.log2_max_cuwh;
            core->y_pel = core->y_lcu << ctx_patch->info.log2_max_cuwh;
            patch->x_pel = core->x_pel;
            patch->y_pel = core->y_pel;
            /*reset the patch boundary*/
            patch->left_pel = patch->x_pel;
            patch->up_pel = patch->y_pel;
            patch->right_pel = patch->x_pel + (*(patch->width_in_lcu + patch->x_pat) << ctx_patch->info.log2_max_cuwh);
            patch->down_pel = patch->y_pel + (*(patch->height_in_lcu + patch->y_pat) << ctx_patch->info.log2_max_cuwh);
        }
    }

    int core_x_lcu1, core_y_lcu1;
    core_x_lcu1 = core->x_lcu + 1;
    core_y_lcu1 = core->y_lcu;

    if (core_x_lcu1 >= *(patch->width_in_lcu + patch->x_pat) + patch_cur_lcu_x)
    {
        core_x_lcu1 = patch_cur_lcu_x;
        core_y_lcu1++;
        if (core_y_lcu1 >= *(patch->height_in_lcu + patch->y_pat) + patch_cur_lcu_y)
        {
            patch->patch_end = 1;
        }
    }
#endif
    core_x_pel = core_x_lcu_tmp << fetch_ctx_io->log2_max_cuwh;
    core_y_pel = core_y_lcu_tmp << fetch_ctx_io->log2_max_cuwh;

    //fetch_output->pix_x = core_x_lcu_tmp << fetch_ctx_io->log2_max_cuwh;
    //fetch_output->pix_y = core_y_lcu_tmp << fetch_ctx_io->log2_max_cuwh;
    //fetch_output->lcu_num = core_x_lcu_tmp + core_y_lcu_tmp * fetch_ctx_io->ctx_info_pic_width_in_lcu;
    fetch_output->aec_lcu_num = core_x_lcu_tmp + core_y_lcu_tmp * fetch_ctx_io->ctx_info_pic_width_in_lcu;
    fetch_output->dbk_lcu_num = core_x_lcu_tmp + core_y_lcu_tmp * fetch_ctx_io->ctx_info_pic_width_in_lcu;
    fetch2sao_fw_ptr->lcu_num = core_x_lcu_tmp + core_y_lcu_tmp * fetch_ctx_io->ctx_info_pic_width_in_lcu;
    
    *tmp_pix_x = core_x_lcu_tmp << fetch_ctx_io->log2_max_cuwh;
    *tmp_pix_y = core_y_lcu_tmp << fetch_ctx_io->log2_max_cuwh;
    *lcu_x = core_x_lcu_tmp;
    *lcu_y = core_y_lcu_tmp;

    static int tmp_lcu_cnt = 0;

    if (core_x_pel == 0 && core_y_pel == 0) {
        tmp_lcu_cnt = ((int)ctu_ctrl_input->f_lcu);
        //fetch_output->lcu_cnt = ((int)ctu_ctrl_input->f_lcu);
        fetch_output->aec_lcu_cnt = ((int)ctu_ctrl_input->f_lcu);
    }
    else {

        tmp_lcu_cnt--;
        //fetch_output->lcu_cnt = tmp_lcu_cnt;
        fetch_output->aec_lcu_cnt = tmp_lcu_cnt;
    }

#if CTU_RATE_CONTROL
    int lcu_qp;
    int lcu_qp_u;
    int lcu_qp_v;
    
    static const int com_tbl_qp_chroma_adjust[64] = {
        //this table aligned with spec
        0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
        10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
        20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
        30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
        40, 41, 42, 42, 43, 43, 44, 44, 45, 45,
        46, 46, 47, 47, 48, 48, 48, 49, 49, 49,
        50, 50, 50, 51
    };

#if USE_ROI_QP_CONFIG || USE_ROI_MODE_CONFIG
	MapLcu mapLcu;
	if (ctu_ctrl_input->useQPMAP || ctu_ctrl_input->useModeMAP)
	{
		fread(&mapLcu, sizeof(MapLcu), 1, fetch_ctx_io->lcumap_in);
	}

	U3 fetch_Mode_t = fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_I ? U3(0x7) :
		(ctu_ctrl_input->useModeMAP ? (mapLcu.use_ROI_Mode ? mapLcu.ROI_Mode : U3(0x7)) : U3(0x7));

#if USE_ROI_MODE_CONFIG
	if (ctu_ctrl_input->useModeMAP) {
		fetch_output->valid_mode[0] = fetch_Mode_t & 1;
		fetch_output->valid_mode[1] = (fetch_Mode_t >> 1) & 1;
		fetch_output->valid_mode[2] = (fetch_Mode_t >> 2) & 1;
	}
	else
	{
		fetch_output->valid_mode[0] = 1;
		fetch_output->valid_mode[1] = 1;
		fetch_output->valid_mode[2] = 1;
	}
#endif

#endif
#if CUDQP_QP_MAP
	fetch_output->cu_dqp_enable = ctu_ctrl_input->cu_dqp_enable;
	fetch_output->cu_qp_group_area_size = fetch_ctx_io->cu_qp_group_area_size;
	//ctu_ctrl_input->last_lcu_qp = fetch_ctx_io->slice_qp;
    fetch_output->md_slice_qp= fetch_ctx_io->slice_qp;
#endif
////////////////////////////////////////////////////////////////////////////////////////
    fetch2sao_fw_ptr->lambda[0] = ctu_ctrl_input->lambda[0];
    fetch2sao_fw_ptr->lambda[1] = ctu_ctrl_input->lambda[1];
    fetch2sao_fw_ptr->lambda[2] = ctu_ctrl_input->lambda[2];

    fetch_output->lambda_mv = ctu_ctrl_input->lambda_mv;
    fetch_output->rmd_lambda_mv = ctu_ctrl_input->lambda_mv;
    fetch_output->lambda[0] = ctu_ctrl_input->lambda[0];
    fetch_output->lambda[1] = ctu_ctrl_input->lambda[1];
    fetch_output->lambda[2] = ctu_ctrl_input->lambda[2];
///////////////////////////////////////////////////////////////////////////////////////
    if (frame_level_fw.fixed_slice_qp_flag)
    {
        lcu_qp = ctu_ctrl_input->qp_y;
        lcu_qp_u = ctu_ctrl_input->qp_u;
        lcu_qp_v = ctu_ctrl_input->qp_v;
    }
#if CUDQP_QP_MAP
	else if (ctu_ctrl_input->cu_dqp_enable)
	{
		//*map_delta_qp++ = 0;
		lcu_qp = fetch_ctx_io->slice_qp;
        lcu_qp_u = ctu_ctrl_input->qp_u;
        lcu_qp_v = ctu_ctrl_input->qp_v;
		//assert(ctu_ctrl_input.last_lcu_qp == ctx->info.shext.slice_qp);
#if CUDQP_QP_MAP

        int CuQPMap_temp[256];

        if (ctu_ctrl_input->useCuQPMAP)
        {
            for (int i = 0; i < 16; i++)
            {
#pragma HLS pipeline
                for (int j = 0; j < 16; j++)
                {
#pragma HLS pipeline
                    //ctu_ctrl_input->CuQPMap[i * 16 + j] = ctu_ctrl_input->CuQPMap_Frame[core_x_lcu_tmp*16+i+ (core_y_lcu_tmp+j) * fetch_ctx_io->ctx_info_pic_width_in_scu];
                    CuQPMap_temp[i * 16 + j] = ctu_ctrl_input->CuQPMap_Frame[core_x_lcu_tmp*16+i+ (core_y_lcu_tmp+j) * fetch_ctx_io->ctx_info_pic_width_in_scu];
                }
            }

            for (int i = 0; i < 256; i++)
            {
#pragma HLS pipeline
                fetch_output->CuQPMap[i] = CuQPMap_temp[i];
            }
        }
        else
        {
            for (int i = 0; i < 256; i++)
            {
#pragma HLS pipeline
                fetch_output->CuQPMap[i] = ctu_ctrl_input->CuQPMap[i];
            }
        }
#endif
	}
#endif
    else
    {
        //int SHIFT_QP = 18;
        //ctu_ctrl_input->qp_y = 56;// rand() % 60;//30;

#if USE_ROI_QP_CONFIG
		if (ctu_ctrl_input->useQPMAP)
			ctu_ctrl_input->qp_y = ctu_ctrl_input->useQPMAP ? (mapLcu.use_ROI_qp ? mapLcu.ROI_qp : fetch_ctx_io->slice_qp) : fetch_ctx_io->slice_qp;
#endif

        lcu_qp = ctu_ctrl_input->qp_y;
        //ctu_ctrl_input->qp_u = ctu_ctrl_input->qp_v = lcu_qp;
        lcu_qp_u = lcu_qp_v = lcu_qp;

        int    adj_qp_cb = ctu_ctrl_input->qp_y + frame_level_fw.chroma_quant_param_delta_cb - frame_level_fw.qp_offset_bit_depth;
        int adj_qp_cr = ctu_ctrl_input->qp_y + frame_level_fw.chroma_quant_param_delta_cr - frame_level_fw.qp_offset_bit_depth;
        adj_qp_cb = COM_CLIP(adj_qp_cb, MIN_QUANT - 16, MAX_QUANT_BASE);
        adj_qp_cr = COM_CLIP(adj_qp_cr, MIN_QUANT - 16, MAX_QUANT_BASE);
        if (adj_qp_cb >= 0) {
            adj_qp_cb = com_tbl_qp_chroma_adjust[COM_MIN(MAX_QUANT_BASE, adj_qp_cb)];
        }
        if (adj_qp_cr >= 0) {
            adj_qp_cr = com_tbl_qp_chroma_adjust[COM_MIN(MAX_QUANT_BASE, adj_qp_cr)];
        }
        /*ctu_ctrl_input->qp_u = COM_CLIP(adj_qp_cb + frame_level_fw.qp_offset_bit_depth, MIN_QUANT,
            MAX_QUANT_BASE + frame_level_fw.qp_offset_bit_depth);
        ctu_ctrl_input->qp_v = COM_CLIP(adj_qp_cr + frame_level_fw.qp_offset_bit_depth, MIN_QUANT,
            MAX_QUANT_BASE + frame_level_fw.qp_offset_bit_depth);*/
        lcu_qp_u = COM_CLIP(adj_qp_cb + frame_level_fw.qp_offset_bit_depth, MIN_QUANT,
            MAX_QUANT_BASE + frame_level_fw.qp_offset_bit_depth);
        lcu_qp_v = COM_CLIP(adj_qp_cr + frame_level_fw.qp_offset_bit_depth, MIN_QUANT,
            MAX_QUANT_BASE + frame_level_fw.qp_offset_bit_depth);

        //printf("qpy=%d,qpu=%d,qpv=%d\n", ctu_ctrl_input->qp_y, ctu_ctrl_input->qp_u, ctu_ctrl_input->qp_v);
        //ctu_ctrl_input->delta_qp = (s8)(lcu_qp - ctu_ctrl_input->last_lcu_qp);
        //ctu_ctrl_input->delta_qp = (s8)(lcu_qp - fetch_ctx_io->slice_qp);
        //ctu_ctrl_input->last_lcu_qp = lcu_qp;
        //fetch_output->delta_qp = (s8)(lcu_qp - fetch_ctx_io->slice_qp);
        fetch_output->aec_delta_qp = (s8)(lcu_qp - fetch_ctx_io->slice_qp);
#if 1
        s64 lamda_sqrt_temp[64] = { 306,334,364,397,433,473,515,562,
                                    613,669,729,795,867,946,1031,1125,
                                    1227,1338,1459,1591,1735,1892,2063,2250,
                                    2454,2676,2918,3183,3471,3785,4127,4501,
                                    4908,5353,5837,6366,6942,7570,8255,9002,
                                    9817,10706,11675,12732,13884,15141,16511,18005,
                                    19635,21412,23350,25464,27768,30282,33023,36011,
                                    39271,42825,46701,50928,55537,60564,66046,72023
                                    };

        s64 lamda_temp[64] = {91,109,129,154,183,218,259,309,
                            367,437,519,618,735,874,1039,1236,
                            1470,1749,2079,2473,2941,3498,4159,4947,
                            5883,6996,8319,9894,11766,13992,16639,19788,
                            23532,27985,33279,39576,47065,55970,66559,79153,
                            94130,111940,133119,158307,188260,223880,266239,316614,
                            376520,447760,532479,633228,753040,895520,1064959,1266457,
                            1506080,1791041,2129919,2532914,3012160,3582082,4259838,5065829
                            };

        s64 lamda_sqrt_bit_temp[64]= { 433,473,515,562,613,669,729,795,
                                    867,946,1031,1125,1227,1338,1459,1591,
                                    1735,1892,2063,2250,2454,2676,2918,3183,
                                    3471,3785,4127,4501,4908,5353,5837,6366,
                                    6942,7570,8255,9280,10413,11667,13053,14586,
                                    16280,18153,20222,22507,25030,27815,30890,34282,
                                    38024,42151,46701,50928,55537,60564,66046,72023,
                                    78542,85651,93403,101856,111075,121128,132092,144047
                                    };

        s64 lamda_bit_temp[64]= { 183,218,259,309,367,437,519,618,
                                735,874,1039,1236,1470,1749,2079,2473,
                                2941,3498,4159,4947,5883,6996,8319,9894,
                                11766,13992,16639,19788,23532,27985,33279,39576,
                                47065,55970,66559,84100,105896,132928,166399,207778,
                                258857,321827,399359,494709,611845,755595,931839,1147727,
                                1411950,1735071,2129919,2532914,3012160,3582082,4259838,5065829,
                                6024320,7164165,8519676,10131659,12048641,14328330,17039352,20263318
                                };

        if (ctu_ctrl_input->depth > 0)
        {
            fetch_output->lambda[0] = lamda_bit_temp[lcu_qp];
            fetch_output->lambda[1] = lamda_bit_temp[lcu_qp];
            fetch_output->lambda[2] = lamda_bit_temp[lcu_qp];

            fetch2sao_fw_ptr->lambda[0] = lamda_bit_temp[lcu_qp];
            fetch2sao_fw_ptr->lambda[1] = lamda_bit_temp[lcu_qp];
            fetch2sao_fw_ptr->lambda[2] = lamda_bit_temp[lcu_qp];
            //ctu_ctrl_input->sqrt_lambda[0] = lamda_sqrt_bit_temp[lcu_qp];
            //ctu_ctrl_input->sqrt_lambda[1] = lamda_sqrt_bit_temp[lcu_qp];
            //ctu_ctrl_input->sqrt_lambda[2] = lamda_sqrt_bit_temp[lcu_qp];

            /* initialize structures *****************************************/
            /* initialize pinter */
            fetch_output->lambda_mv = (u32)lamda_sqrt_bit_temp[lcu_qp];
            fetch_output->rmd_lambda_mv = (u32)lamda_sqrt_bit_temp[lcu_qp];
        }
        else
        {
            fetch_output->lambda[0] = lamda_temp[lcu_qp];
            fetch_output->lambda[1] = lamda_temp[lcu_qp];
            fetch_output->lambda[2] = lamda_temp[lcu_qp];

            fetch2sao_fw_ptr->lambda[0] = lamda_temp[lcu_qp];
            fetch2sao_fw_ptr->lambda[1] = lamda_temp[lcu_qp];
            fetch2sao_fw_ptr->lambda[2] = lamda_temp[lcu_qp];
            //ctu_ctrl_input->sqrt_lambda[0] = lamda_sqrt_temp[lcu_qp];
            //ctu_ctrl_input->sqrt_lambda[1] = lamda_sqrt_temp[lcu_qp];
            //ctu_ctrl_input->sqrt_lambda[2] = lamda_sqrt_temp[lcu_qp];

            /* initialize structures *****************************************/
            /* initialize pinter */
            //ctu_ctrl_input->lambda_mv = (u32)lamda_sqrt_temp[lcu_qp];
            fetch_output->lambda_mv = (u32)lamda_sqrt_temp[lcu_qp];
            fetch_output->rmd_lambda_mv = (u32)lamda_sqrt_temp[lcu_qp];
        }
#if 0
        printf("{\n");
        for (int i = 0; i < 65; ++i)
        {
            double qp_temp = i - 18;// SHIFT_QP;
            double lambda_temp = 1.43631 * pow(2.0, (i - 16.0) / 4.0);

            lambda_temp *= Clip3(2.00, 4.00, (double)(qp_temp / 8.0));

            long long ctx_sqrt_lambda_temp = (long long)(sqrt(lambda_temp) * FIXED_SQRT_LAMBDA_NUM);

            printf("%lld,", ctx_sqrt_lambda_temp);

            if (0 == ((i + 1) % 8))
            {
                printf("\n");
            }
        }
        printf("}\n");
#endif
#else
        double qp_temp = lcu_qp - 18;// SHIFT_QP;
        double lambda[3];

        lambda[0]  = 1.43631 * pow(2.0, (lcu_qp - 16.0) / 4.0);
#if FRAME_LAMBDA_OPT
        if (ctu_ctrl_input->depth > 0)
            lambda[0] *= Clip3(2.00, 4.00, (double)(qp_temp / 8.0));
#endif
        lambda[1] = lambda[2] = lambda[0];
        s64 ctx_sqrt_lambda[3], ctx_lambda[3];
        ctx_sqrt_lambda[0] = s64(sqrt(lambda[0]) * (1 << LAMBDA_SCALE));
        ctx_sqrt_lambda[1] = s64(sqrt(lambda[1]) * (1 << LAMBDA_SCALE));
        ctx_sqrt_lambda[2] = s64(sqrt(lambda[2]) * (1 << LAMBDA_SCALE));

        ctx_lambda[0] = s64((1 << LAMBDA_SCALE) * lambda[0]);
        ctx_lambda[1] = s64((1 << LAMBDA_SCALE) * lambda[1]);
        ctx_lambda[2] = s64((1 << LAMBDA_SCALE) * lambda[2]);


        ctu_ctrl_input->lambda[0] = ctx_lambda[0];
        ctu_ctrl_input->lambda[1] = ctx_lambda[1];
        ctu_ctrl_input->lambda[2] = ctx_lambda[2];
        ctu_ctrl_input->sqrt_lambda[0] = ctx_sqrt_lambda[0];
        ctu_ctrl_input->sqrt_lambda[1] = ctx_sqrt_lambda[1];
        ctu_ctrl_input->sqrt_lambda[2] = ctx_sqrt_lambda[2];


        /* initialize structures *****************************************/
        /* initialize pinter */
        ctu_ctrl_input->lambda_mv = (u32)ctx_sqrt_lambda[0];
#endif
    }

#if CUDQP_QP_MAP
	fetch_output->cu_qp_group_pred_qp = lcu_qp; //when cudqp is on, it will be updated at gp group level
#endif

#endif
    fetch_output->qp_y = ctu_ctrl_input->qp_y;
    fetch_output->qp_u = lcu_qp_u;
    fetch_output->qp_v = lcu_qp_v;

    s64 tmp1, tmp2, tmp3;
    tmp1 = ctu_ctrl_input->dist_chroma_weight[0][1];
    tmp2 = ctu_ctrl_input->dist_chroma_weight[1][1];
    fetch_output->dist_chroma_weight[0] = ctu_ctrl_input->dist_chroma_weight[0][0] | tmp1 << 32;
    fetch_output->dist_chroma_weight[1] = ctu_ctrl_input->dist_chroma_weight[1][0] | tmp2 << 32;

#if USE_INTRA_REFRESH
	fetch_output->EncRefresh = ctu_ctrl_input->EncRefresh;
	fetch_output->RefreshSize = ctu_ctrl_input->RefreshSize;
	fetch_output->RefreshMode = ctu_ctrl_input->RefreshMode;
	fetch_output->pirStartRow = ctu_ctrl_input->pirStartRow;
	fetch_output->pirEndRow = ctu_ctrl_input->pirEndRow;
	fetch_output->pirStartCol = ctu_ctrl_input->pirStartCol;
	fetch_output->pirEndCol = ctu_ctrl_input->pirEndCol;
	fetch_output->numCuInWidth = (fetch_ctx_io->ctx_info_pic_width + 32 - 1) / 32;
	fetch_output->numCuInHeight = (fetch_ctx_io->ctx_info_pic_height + 32 - 1) / 32;
#endif
#if USE_SPEED_LEVEL
	fetch_output->speed_level = ctu_ctrl_input->speed_level;
#endif
    if (core_x_pel + 64 >= img_width) 
    {
        *tmp_smb_num_width = (img_width - core_x_pel) >> 3;
    }
    else 
    {
        *tmp_smb_num_width = 8;
    }
    if (core_y_pel + 64 >= img_height) 
    {
        *tmp_smb_num_height = (img_height - core_y_pel) >> 3;
     }
    else 
    {
           *tmp_smb_num_height = 8;
    }
    fetch2sao_fw_ptr->cu_width = *tmp_smb_num_width * 8;
    fetch2sao_fw_ptr->cu_height = *tmp_smb_num_height * 8;
}
#else
static void firmware_parameters_get(
    int* tmp_pix_x, 
    int* tmp_pix_y, 
    int* lcu_x,
    int* lcu_y,
    int* tmp_smb_num_width, 
    int* tmp_smb_num_height,
    CTU_Level_Parameters* fetch_output,
   // int* lcu_x, 
   // int* lcu_y,
    Slice_Level_Parameters* ctu_ctrl_input, 
    CTX_FETCH* fetch_ctx_io, 
    FETCH2SAO_FW* fetch2sao_fw_ptr
#if PATCH_DEBUG
    PATCH_INFO* patch
#endif
)
{
    static int core_x_lcu_tmp, core_y_lcu_tmp;
    static int core_x_pel, core_y_pel;

    if (ctu_ctrl_input->slice_enable) {
        ctu_ctrl_input->slice_enable = ((unsigned char)0);
       // *lcu_x = 0;
       // *lcu_y = 0;
        core_x_lcu_tmp = 0;
        core_y_lcu_tmp = 0;
    }
    else {

        core_x_lcu_tmp++;
    }


    int img_width = fetch_ctx_io->ctx_info_pic_width;
    int img_height = fetch_ctx_io->ctx_info_pic_height;
    int lcu_width = (img_width + 63) >> 6;
#if PATCH_DEBUG == 0
    if (core_x_lcu_tmp == lcu_width) {

        core_y_lcu_tmp++;
        core_x_lcu_tmp = 0;
    }
#else
    int num_of_patch = patch->columns * patch->rows;
    patch_cur_idx = patch->idx;
    if (core->x_lcu >= *(patch->width_in_lcu + patch->x_pat) + patch_cur_lcu_x)
    {
        core->x_lcu = patch_cur_lcu_x;
        (core->y_lcu)++;
        if (core->y_lcu >= *(patch->height_in_lcu + patch->y_pat) + patch_cur_lcu_y)
        {
            // a new patch starts
            //core->cnt_hmvp_cands = 0;
            patch->patch_switch = 1;
            patch->pat = 0;

            int start_pos = AEC_DATA_output.bs_cur;
            aec_enc_sbac_encode_bin_trm1(1, &aec_sbac_local, &AEC_DATA_output.bs, &AEC_DATA_output.bs_cur);
            aec_enc_sbac_finish1(&aec_sbac_local, &AEC_DATA_output.bs, &AEC_DATA_output.bs_cur);
            en_copy_lcu_scu(map_scu_temp, ctx_patch->map.map_scu, map_refi_temp, ctx_patch->map.map_refi, map_mv_temp, ctx_patch->map.map_mv, map_cu_mode_temp, ctx_patch->map.map_cu_mode, ctx_patch->patch, ctx_patch->info.pic_width, ctx_patch->info.pic_height);

            for (int k1 = 0; k1 < ctx_patch->info.f_scu; k1++)
            {
                //save data for dbk
                //map_scu_for_dbk_patch[k1] = ctx_patch->map.map_scu[k1];
                ctx_patch->map.map_scu[k1] = 0;
                ctx_patch->map.map_cu_mode[k1] = 0;
            }
            int i, j;

            for (i = 0; i < 1 << 7 >> 1 >> 2; i++) {
//#pragma HLS pipeline
                //#pragma HLS loop_tripcount max = 8
                for (j = 0; j < 1 << 7 >> 1 >> 2; j++) {

                    //#pragma HLS loop_tripcount max = 2
                    me_line_ptr_patch->map_scu_inner[0][i][j] = ((u32)0);
                    me_line_ptr_patch->map_cu_mode_inner[0][i][j] = ((u32)0);
                    init_neb_info(&inner_nebs[0][i][j]);
                }
            }
            for (i = 0; i < 1 << 7 >> 1 >> 2; i++) {

//#pragma HLS pipeline
                //#pragma HLS loop_tripcount max = 8
                for (j = 0; j < 1 << 7 >> 1 >> 2; j++) {

                    //#pragma HLS loop_tripcount max = 2
                    me_line_ptr_patch->map_scu_inner[1][i][j] = ((u32)0);
                    me_line_ptr_patch->map_cu_mode_inner[1][i][j] = ((u32)0);
                    init_neb_info(&inner_nebs[1][i][j]);
                }
            }

            /*set patch head*/
            if (patch_cur_idx + 1 < num_of_patch)
            {
                set_sh1(ctx_patch, &ctx_patch->info.shext, patch->patch_sao_enable + (patch_cur_idx + 1) * N_C);
            }
            /* initialize entropy coder */
            enc_sbac_init1(&aec_sbac_local);
            com_sbac_ctx_init1(&aec_ctx);
            //com_sbac_ctx_init(&(core->s_curr_best[ctx_patch->info.log2_max_cuwh - 2][ctx_patch->info.log2_max_cuwh - 2].ctx_patch));

            if (ctx_patch->lcu_cnt > 1)
            {
                Demulate1(&AEC_DATA_output.bs, &ctx_patch->bs, &AEC_DATA_output.bs_cur, Demulate_start_pos);
                /*encode patch end*/
                int ret = aec_enc_eco_send(&AEC_DATA_output.bs, &ctx_patch->bs, &AEC_DATA_output.bs_cur);
                /* Encode patch header */
                int patch_idx = patch->idx + 1;
                AEC_DATA_output.bs.code = 0;
                AEC_DATA_output.bs.leftbits = 32;
                Demulate_start_pos = AEC_DATA_output.bs_cur;
                aec_com_bsw_flush1(&AEC_DATA_output.bs, &AEC_DATA_output.bs_cur);

                ret = aec_enc_eco_patch_header(&AEC_DATA_output.bs, &ctx_patch->bs, &ctx_patch->info.sqh, pic_header_g, &ctx_patch->info.shext, patch_idx, patch, &AEC_DATA_output.bs_cur);
                set_sh1(ctx_patch, &ctx_patch->info.shext, patch->patch_sao_enable + patch_idx * N_C);

                for (int i = 0; i < AEC_DATA_output.bs_cur - start_pos; i++)
                {
                    bs_patch->beg[start_pos + i + patch->len] = AEC_DATA_output.bs.beg[start_pos + i];
                }

                AEC_DATA_output.bs.uiStartPos = AEC_DATA_output.bs_cur;
                AEC_DATA_output.bs.CurAddress = AEC_DATA_output.bs_cur;

            }
            ///*update and store map_scu*/
            en_copy_lcu_scu(map_scu_temp, ctx_patch->map.map_scu, map_refi_temp, ctx_patch->map.map_refi, map_mv_temp, ctx_patch->map.map_mv, map_cu_mode_temp, ctx_patch->map.map_cu_mode, ctx_patch->patch, ctx_patch->info.pic_width, ctx_patch->info.pic_height);

            enc_sbac_init1(&aec_sbac_local);


            core->x_lcu = *(patch->width_in_lcu + patch->x_pat) + patch_cur_lcu_x;
            core->y_lcu = patch_cur_lcu_y;
            patch->x_pat++;
            if (core->x_lcu >= ctx_patch->info.pic_width_in_lcu)
            {
                core->x_lcu = 0;
                core->y_lcu = *(patch->height_in_lcu + patch->y_pat) + patch_cur_lcu_y;
                patch->y_pat++;
                patch->x_pat = 0;
            }
            patch->idx++;
            patch_cur_lcu_x = core->x_lcu;
            patch_cur_lcu_y = core->y_lcu;
            core->x_pel = core->x_lcu << ctx_patch->info.log2_max_cuwh;
            core->y_pel = core->y_lcu << ctx_patch->info.log2_max_cuwh;
            patch->x_pel = core->x_pel;
            patch->y_pel = core->y_pel;
            /*reset the patch boundary*/
            patch->left_pel = patch->x_pel;
            patch->up_pel = patch->y_pel;
            patch->right_pel = patch->x_pel + (*(patch->width_in_lcu + patch->x_pat) << ctx_patch->info.log2_max_cuwh);
            patch->down_pel = patch->y_pel + (*(patch->height_in_lcu + patch->y_pat) << ctx_patch->info.log2_max_cuwh);
        }
    }

    int core_x_lcu1, core_y_lcu1;
    core_x_lcu1 = core->x_lcu + 1;
    core_y_lcu1 = core->y_lcu;

    if (core_x_lcu1 >= *(patch->width_in_lcu + patch->x_pat) + patch_cur_lcu_x)
    {
        core_x_lcu1 = patch_cur_lcu_x;
        core_y_lcu1++;
        if (core_y_lcu1 >= *(patch->height_in_lcu + patch->y_pat) + patch_cur_lcu_y)
        {
            patch->patch_end = 1;
        }
    }
#endif
    core_x_pel = core_x_lcu_tmp << fetch_ctx_io->log2_max_cuwh;
    core_y_pel = core_y_lcu_tmp << fetch_ctx_io->log2_max_cuwh;

    //fetch_output->pix_x = core_x_lcu_tmp << fetch_ctx_io->log2_max_cuwh;
    //fetch_output->pix_y = core_y_lcu_tmp << fetch_ctx_io->log2_max_cuwh;
    fetch_output->lcu_num = core_x_lcu_tmp + core_y_lcu_tmp * fetch_ctx_io->ctx_info_pic_width_in_lcu;
    fetch2sao_fw_ptr->lcu_num = core_x_lcu_tmp + core_y_lcu_tmp * fetch_ctx_io->ctx_info_pic_width_in_lcu;
    
    *tmp_pix_x = core_x_lcu_tmp << fetch_ctx_io->log2_max_cuwh;
    *tmp_pix_y = core_y_lcu_tmp << fetch_ctx_io->log2_max_cuwh;
    *lcu_x = core_x_lcu_tmp;
    *lcu_y = core_y_lcu_tmp;

/*
#if PATCH_DEBUG == 0
    if (*lcu_x == lcu_width) {
        (*lcu_y)++;
        (*lcu_x) = 0;
    }
#endif
*/

    static int tmp_lcu_cnt = 0;
    if (core_x_pel == 0 && core_y_pel == 0) {
        tmp_lcu_cnt = ((int)ctu_ctrl_input->f_lcu);
        fetch_output->lcu_cnt = ((int)ctu_ctrl_input->f_lcu);
    }
    else {

        tmp_lcu_cnt--;
        fetch_output->lcu_cnt = tmp_lcu_cnt;
    }
/*
#if PATCH_DEBUG == 0

    (*lcu_x)++;
#endif
*/
#if CTU_RATE_CONTROL
    int lcu_qp;
    static const int com_tbl_qp_chroma_adjust[64] = {
        //this table aligned with spec
        0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
        10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
        20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
        30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
        40, 41, 42, 42, 43, 43, 44, 44, 45, 45,
        46, 46, 47, 47, 48, 48, 48, 49, 49, 49,
        50, 50, 50, 51
    };

#if USE_ROI_QP_CONFIG || USE_ROI_MODE_CONFIG
	MapLcu mapLcu;
	if (ctu_ctrl_input->useQPMAP || ctu_ctrl_input->useModeMAP)
	{
		fread(&mapLcu, sizeof(MapLcu), 1, fetch_ctx_io->lcumap_in);
	}

	U3 fetch_Mode_t = fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_I ? U3(0x7) :
		(ctu_ctrl_input->useModeMAP ? (mapLcu.use_ROI_Mode ? mapLcu.ROI_Mode : U3(0x7)) : U3(0x7));

#if USE_ROI_MODE_CONFIG
	if (ctu_ctrl_input->useModeMAP) {
		fetch_output->valid_mode[0] = fetch_Mode_t & 1;
		fetch_output->valid_mode[1] = (fetch_Mode_t >> 1) & 1;
		fetch_output->valid_mode[2] = (fetch_Mode_t >> 2) & 1;
	}
	else
	{
		fetch_output->valid_mode[0] = 1;
		fetch_output->valid_mode[1] = 1;
		fetch_output->valid_mode[2] = 1;
	}
#endif

#endif
#if CUDQP_QP_MAP
	fetch_output->cu_dqp_enable = ctu_ctrl_input->cu_dqp_enable;
	fetch_output->cu_qp_group_area_size = fetch_ctx_io->cu_qp_group_area_size;
	ctu_ctrl_input->last_lcu_qp = fetch_ctx_io->slice_qp;
    fetch_output->md_slice_qp= fetch_ctx_io->slice_qp;
#endif

    if (frame_level_fw.fixed_slice_qp_flag)
    {
        lcu_qp = ctu_ctrl_input->qp_y;
    }
#if CUDQP_QP_MAP
	else if (ctu_ctrl_input->cu_dqp_enable)
	{
		//*map_delta_qp++ = 0;
		lcu_qp = fetch_ctx_io->slice_qp;
		//assert(ctu_ctrl_input.last_lcu_qp == ctx->info.shext.slice_qp);
#if CUDQP_QP_MAP

        int CuQPMap_temp[256];

        if (ctu_ctrl_input->useCuQPMAP)
        {
            for (int i = 0; i < 16; i++)
            {
#pragma HLS pipeline
                for (int j = 0; j < 16; j++)
                {
#pragma HLS pipeline
                    //ctu_ctrl_input->CuQPMap[i * 16 + j] = ctu_ctrl_input->CuQPMap_Frame[core_x_lcu_tmp*16+i+ (core_y_lcu_tmp+j) * fetch_ctx_io->ctx_info_pic_width_in_scu];
                    CuQPMap_temp[i * 16 + j] = ctu_ctrl_input->CuQPMap_Frame[core_x_lcu_tmp*16+i+ (core_y_lcu_tmp+j) * fetch_ctx_io->ctx_info_pic_width_in_scu];
                }
            }

            for (int i = 0; i < 256; i++)
            {
#pragma HLS pipeline
                fetch_output->CuQPMap[i] = CuQPMap_temp[i];
            }
        }
        else
        {
            for (int i = 0; i < 256; i++)
            {
#pragma HLS pipeline
                fetch_output->CuQPMap[i] = ctu_ctrl_input->CuQPMap[i];
            }
        }
#endif
	}
#endif
    else
    {
        //int SHIFT_QP = 18;
        //ctu_ctrl_input->qp_y = 56;// rand() % 60;//30;

#if USE_ROI_QP_CONFIG
		if (ctu_ctrl_input->useQPMAP)
			ctu_ctrl_input->qp_y = ctu_ctrl_input->useQPMAP ? (mapLcu.use_ROI_qp ? mapLcu.ROI_qp : fetch_ctx_io->slice_qp) : fetch_ctx_io->slice_qp;
#endif

        lcu_qp = ctu_ctrl_input->qp_y;

        ctu_ctrl_input->qp_u = ctu_ctrl_input->qp_v = lcu_qp;

        int    adj_qp_cb = ctu_ctrl_input->qp_y + frame_level_fw.chroma_quant_param_delta_cb - frame_level_fw.qp_offset_bit_depth;
        int adj_qp_cr = ctu_ctrl_input->qp_y + frame_level_fw.chroma_quant_param_delta_cr - frame_level_fw.qp_offset_bit_depth;
        adj_qp_cb = COM_CLIP(adj_qp_cb, MIN_QUANT - 16, MAX_QUANT_BASE);
        adj_qp_cr = COM_CLIP(adj_qp_cr, MIN_QUANT - 16, MAX_QUANT_BASE);
        if (adj_qp_cb >= 0) {
            adj_qp_cb = com_tbl_qp_chroma_adjust[COM_MIN(MAX_QUANT_BASE, adj_qp_cb)];
        }
        if (adj_qp_cr >= 0) {
            adj_qp_cr = com_tbl_qp_chroma_adjust[COM_MIN(MAX_QUANT_BASE, adj_qp_cr)];
        }
        ctu_ctrl_input->qp_u = COM_CLIP(adj_qp_cb + frame_level_fw.qp_offset_bit_depth, MIN_QUANT,
            MAX_QUANT_BASE + frame_level_fw.qp_offset_bit_depth);
        ctu_ctrl_input->qp_v = COM_CLIP(adj_qp_cr + frame_level_fw.qp_offset_bit_depth, MIN_QUANT,
            MAX_QUANT_BASE + frame_level_fw.qp_offset_bit_depth);

        //printf("qpy=%d,qpu=%d,qpv=%d\n", ctu_ctrl_input->qp_y, ctu_ctrl_input->qp_u, ctu_ctrl_input->qp_v);
        //ctu_ctrl_input->delta_qp = (s8)(lcu_qp - ctu_ctrl_input->last_lcu_qp);
        ctu_ctrl_input->delta_qp = (s8)(lcu_qp - fetch_ctx_io->slice_qp);
        ctu_ctrl_input->last_lcu_qp = lcu_qp;
        //fetch_output->delta_qp = ctu_ctrl_input->delta_qp;
#if 1
        s64 lamda_sqrt_temp[64] = { 306,334,364,397,433,473,515,562,
                                    613,669,729,795,867,946,1031,1125,
                                    1227,1338,1459,1591,1735,1892,2063,2250,
                                    2454,2676,2918,3183,3471,3785,4127,4501,
                                    4908,5353,5837,6366,6942,7570,8255,9002,
                                    9817,10706,11675,12732,13884,15141,16511,18005,
                                    19635,21412,23350,25464,27768,30282,33023,36011,
                                    39271,42825,46701,50928,55537,60564,66046,72023
                                    };

        s64 lamda_temp[64] = {91,109,129,154,183,218,259,309,
                            367,437,519,618,735,874,1039,1236,
                            1470,1749,2079,2473,2941,3498,4159,4947,
                            5883,6996,8319,9894,11766,13992,16639,19788,
                            23532,27985,33279,39576,47065,55970,66559,79153,
                            94130,111940,133119,158307,188260,223880,266239,316614,
                            376520,447760,532479,633228,753040,895520,1064959,1266457,
                            1506080,1791041,2129919,2532914,3012160,3582082,4259838,5065829
                            };

        s64 lamda_sqrt_bit_temp[64]= { 433,473,515,562,613,669,729,795,
                                    867,946,1031,1125,1227,1338,1459,1591,
                                    1735,1892,2063,2250,2454,2676,2918,3183,
                                    3471,3785,4127,4501,4908,5353,5837,6366,
                                    6942,7570,8255,9280,10413,11667,13053,14586,
                                    16280,18153,20222,22507,25030,27815,30890,34282,
                                    38024,42151,46701,50928,55537,60564,66046,72023,
                                    78542,85651,93403,101856,111075,121128,132092,144047
                                    };

        s64 lamda_bit_temp[64]= { 183,218,259,309,367,437,519,618,
                                735,874,1039,1236,1470,1749,2079,2473,
                                2941,3498,4159,4947,5883,6996,8319,9894,
                                11766,13992,16639,19788,23532,27985,33279,39576,
                                47065,55970,66559,84100,105896,132928,166399,207778,
                                258857,321827,399359,494709,611845,755595,931839,1147727,
                                1411950,1735071,2129919,2532914,3012160,3582082,4259838,5065829,
                                6024320,7164165,8519676,10131659,12048641,14328330,17039352,20263318
                                };

        if (ctu_ctrl_input->depth > 0)
        {
            ctu_ctrl_input->lambda[0] = lamda_bit_temp[lcu_qp];
            ctu_ctrl_input->lambda[1] = lamda_bit_temp[lcu_qp];
            ctu_ctrl_input->lambda[2] = lamda_bit_temp[lcu_qp];
            ctu_ctrl_input->sqrt_lambda[0] = lamda_sqrt_bit_temp[lcu_qp];
            ctu_ctrl_input->sqrt_lambda[1] = lamda_sqrt_bit_temp[lcu_qp];
            ctu_ctrl_input->sqrt_lambda[2] = lamda_sqrt_bit_temp[lcu_qp];

            /* initialize structures *****************************************/
            /* initialize pinter */
            ctu_ctrl_input->lambda_mv = (u32)lamda_sqrt_bit_temp[lcu_qp];
        }
        else
        {
            ctu_ctrl_input->lambda[0] = lamda_temp[lcu_qp];
            ctu_ctrl_input->lambda[1] = lamda_temp[lcu_qp];
            ctu_ctrl_input->lambda[2] = lamda_temp[lcu_qp];
            ctu_ctrl_input->sqrt_lambda[0] = lamda_sqrt_temp[lcu_qp];
            ctu_ctrl_input->sqrt_lambda[1] = lamda_sqrt_temp[lcu_qp];
            ctu_ctrl_input->sqrt_lambda[2] = lamda_sqrt_temp[lcu_qp];

            /* initialize structures *****************************************/
            /* initialize pinter */
            ctu_ctrl_input->lambda_mv = (u32)lamda_sqrt_temp[lcu_qp];
        }
#if 0
        printf("{\n");
        for (int i = 0; i < 65; ++i)
        {
            double qp_temp = i - 18;// SHIFT_QP;
            double lambda_temp = 1.43631 * pow(2.0, (i - 16.0) / 4.0);

            lambda_temp *= Clip3(2.00, 4.00, (double)(qp_temp / 8.0));

            long long ctx_sqrt_lambda_temp = (long long)(sqrt(lambda_temp) * FIXED_SQRT_LAMBDA_NUM);

            printf("%lld,", ctx_sqrt_lambda_temp);

            if (0 == ((i + 1) % 8))
            {
                printf("\n");
            }
        }
        printf("}\n");
#endif
#else
        double qp_temp = lcu_qp - 18;// SHIFT_QP;
        double lambda[3];

        lambda[0]  = 1.43631 * pow(2.0, (lcu_qp - 16.0) / 4.0);
#if FRAME_LAMBDA_OPT
        if (ctu_ctrl_input->depth > 0)
            lambda[0] *= Clip3(2.00, 4.00, (double)(qp_temp / 8.0));
#endif
        lambda[1] = lambda[2] = lambda[0];
        s64 ctx_sqrt_lambda[3], ctx_lambda[3];
        ctx_sqrt_lambda[0] = s64(sqrt(lambda[0]) * (1 << LAMBDA_SCALE));
        ctx_sqrt_lambda[1] = s64(sqrt(lambda[1]) * (1 << LAMBDA_SCALE));
        ctx_sqrt_lambda[2] = s64(sqrt(lambda[2]) * (1 << LAMBDA_SCALE));

        ctx_lambda[0] = s64((1 << LAMBDA_SCALE) * lambda[0]);
        ctx_lambda[1] = s64((1 << LAMBDA_SCALE) * lambda[1]);
        ctx_lambda[2] = s64((1 << LAMBDA_SCALE) * lambda[2]);


        ctu_ctrl_input->lambda[0] = ctx_lambda[0];
        ctu_ctrl_input->lambda[1] = ctx_lambda[1];
        ctu_ctrl_input->lambda[2] = ctx_lambda[2];
        ctu_ctrl_input->sqrt_lambda[0] = ctx_sqrt_lambda[0];
        ctu_ctrl_input->sqrt_lambda[1] = ctx_sqrt_lambda[1];
        ctu_ctrl_input->sqrt_lambda[2] = ctx_sqrt_lambda[2];


        /* initialize structures *****************************************/
        /* initialize pinter */
        ctu_ctrl_input->lambda_mv = (u32)ctx_sqrt_lambda[0];
#endif
    }

#if CUDQP_QP_MAP
	fetch_output->cu_qp_group_pred_qp = lcu_qp; //when cudqp is on, it will be updated at gp group level
#endif

#endif
    fetch_output->qp_y = ctu_ctrl_input->qp_y;
    fetch_output->qp_u = ctu_ctrl_input->qp_u;
    fetch_output->qp_v = ctu_ctrl_input->qp_v;

    fetch_output->lambda_mv = ctu_ctrl_input->lambda_mv;

    s64 tmp1, tmp2, tmp3;
    fetch_output->lambda[0] = ctu_ctrl_input->lambda[0];
    fetch_output->lambda[1] = ctu_ctrl_input->lambda[1];
    fetch_output->lambda[2] = ctu_ctrl_input->lambda[2];

    tmp1 = ctu_ctrl_input->dist_chroma_weight[0][1];
    tmp2 = ctu_ctrl_input->dist_chroma_weight[1][1];
    fetch_output->dist_chroma_weight[0] = ctu_ctrl_input->dist_chroma_weight[0][0] | tmp1 << 32;
    fetch_output->dist_chroma_weight[1] = ctu_ctrl_input->dist_chroma_weight[1][0] | tmp2 << 32;

#if USE_INTRA_REFRESH
	fetch_output->EncRefresh = ctu_ctrl_input->EncRefresh;
	fetch_output->RefreshSize = ctu_ctrl_input->RefreshSize;
	fetch_output->RefreshMode = ctu_ctrl_input->RefreshMode;
	fetch_output->pirStartRow = ctu_ctrl_input->pirStartRow;
	fetch_output->pirEndRow = ctu_ctrl_input->pirEndRow;
	fetch_output->pirStartCol = ctu_ctrl_input->pirStartCol;
	fetch_output->pirEndCol = ctu_ctrl_input->pirEndCol;
	fetch_output->numCuInWidth = (fetch_ctx_io->ctx_info_pic_width + 32 - 1) / 32;
	fetch_output->numCuInHeight = (fetch_ctx_io->ctx_info_pic_height + 32 - 1) / 32;
#endif
#if USE_SPEED_LEVEL
	fetch_output->speed_level = ctu_ctrl_input->speed_level;
#endif
    if (core_x_pel + 64 >= img_width) 
    {
        *tmp_smb_num_width = (img_width - core_x_pel) >> 3;
    }
    else 
    {
        *tmp_smb_num_width = 8;
    }
    if (core_y_pel + 64 >= img_height) 
    {
        *tmp_smb_num_height = (img_height - core_y_pel) >> 3;
     }
    else 
    {
           *tmp_smb_num_height = 8;
    }
    fetch2sao_fw_ptr->cu_width = *tmp_smb_num_width * 8;
    fetch2sao_fw_ptr->cu_height = *tmp_smb_num_height * 8;
}
#endif

static void lcu_fetch_org_pixels(
    int* tmp_pix_x, 
    int* tmp_pix_y, 
    int* lcu_x,
    int* lcu_y,
    int* tmp_smb_num_width, 
    int* tmp_smb_num_height, 
    int pic_width, 
    int pic_height,
    FETCH_ORGI_LCU_IME* Fetch_Orig_Lcu_IME,
    strFetch_Orig_Lcu* Fetch_Orig_Lcu,
    Slice_Level_Parameters* ctu_ctrl_input,
    uint128_t* imgY_org_ap,
    ap_uint<64>* imgU_org_ap,
    ap_uint<64>* imgV_org_ap

#if Debug422
	,U16* imgY_org,
	U16* imgU_org,
	U16* imgV_org,
    u8* chroma_format
#endif
)
{
    int x;
    int y;
#if Debug422
    int i;
#endif
    int lcu_width;
    int lcu_height;
    int pix_x = *tmp_pix_x;
    int pix_y = *tmp_pix_y;
    U8 lcu_y_tmp = (*lcu_y);
    U8 lcu_x_tmp = (*lcu_x);
    lcu_width = (*tmp_smb_num_width) * 8;
    lcu_height = (*tmp_smb_num_height) * 8;
    //Y
    int offset_Y = pic_width;
    int offset_UV = pic_width >> 1;
    U32 idx_y_list[64];
    U32 idx_u_list[32];
    U32 idx_v_list[32];

    U8 linebuf[64];
    uint128_t d_apy[64][5];
    ap_uint<64> d_apu[32][5];
    ap_uint<64> d_apv[32][5];
    uint64_t d_ap[32][5];
    //uint64_t d_apv1[32][5];

    ap_uint<640> org_cache;

#pragma HLS array_partition variable=d_apy complete dim=2
#pragma HLS array_partition variable=d_apu complete dim=2
#pragma HLS array_partition variable=d_apv complete dim=2
#pragma HLS array_partition variable=d_ap complete dim=2
#pragma HLS array_partition variable=linebuf complete dim=0

//#pragma HLS array_reshape variable=Fetch_Orig_Lcu->y_org cyclic factor=8 dim=2
//#pragma HLS array_reshape variable=Fetch_Orig_Lcu->u_org cyclic factor=8 dim=2
//#pragma HLS array_reshape variable=Fetch_Orig_Lcu->v_org cyclic factor=8 dim=2

#pragma HLS RESOURCE variable=Fetch_Orig_Lcu->p_fenc_LCU_Y_rmd core=RAM_1P_BRAM
#pragma HLS ARRAY_PARTITION variable=Fetch_Orig_Lcu->p_fenc_LCU_Y_rmd cyclic factor=64 dim=1

#pragma HLS RESOURCE variable=Fetch_Orig_Lcu_IME->y_org core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=Fetch_Orig_Lcu_IME->u_org core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=Fetch_Orig_Lcu_IME->v_org core=RAM_1P_BRAM
#pragma HLS array_partition variable=Fetch_Orig_Lcu_IME->y_org  complete dim=2
#pragma HLS array_partition variable=Fetch_Orig_Lcu_IME->u_org  complete dim=2
#pragma HLS array_partition variable=Fetch_Orig_Lcu_IME->v_org  complete dim=2

if(ctu_ctrl_input->bit_depth_internal == 8){
    ap_uint<12> pic_width_w_d_y = (pic_width + 15) / 16; //(pic_width + 7) >> 3;
    U32 idx_y = (pic_width_w_d_y * lcu_y_tmp * 64) + lcu_x_tmp * 4;
    for (y = 0; y < 64; y++) {
#pragma HLS unroll
        idx_y_list[y] = idx_y + y * pic_width_w_d_y;
    }
    for (y = 0; y < 64; y++) {
#pragma HLS pipeline II=1
        memcpy(&d_apy[y], &imgY_org_ap[idx_y_list[y]], 4 * sizeof(uint128_t));
    }
    for (y = 0; y < 64; y++) {
#pragma HLS pipeline
        for (int i = 0; i < 4; i++) {
#pragma HLS unroll
            for (int j = 0; j < 16; j++) {
#pragma HLS unroll
                linebuf[i * 16 + j] = d_apy[y][i]((j + 1) * 8 - 1, j * 8);
            }
        }
        for (x = 0; x < 64; x++) {
#pragma HLS unroll
            //Fetch_Orig_Lcu->y_org[y][x] = pel(linebuf[x]);
            Fetch_Orig_Lcu_IME->y_org[y][x] = pel(linebuf[x]);
            Fetch_Orig_Lcu->p_fenc_LCU_Y_rmd[y * 64 + x]=pel(linebuf[x]);
        }
    }
}
#ifndef __SYNTHESIS__
else if(ctu_ctrl_input->bit_depth_internal == 10){
    ap_uint<12> pic_width_w_d_y = (pic_width + 63) / 64 * 5; //(pic_width + 7) >> 3;
    U32 idx_y = (pic_width_w_d_y * lcu_y_tmp * 64) + lcu_x_tmp * 5;
    for (y = 0; y < 64; y++) {
#pragma HLS unroll
        idx_y_list[y] = idx_y + y * pic_width_w_d_y;
    }
    for (y = 0; y < 64; y++) {
#pragma HLS pipeline II=1
        memcpy(&d_apy[y], &imgY_org_ap[idx_y_list[y]], 5 * sizeof(uint128_t));
    }
    org_cache = 0;
    for (y = 0; y < 64; y++) {
#pragma HLS pipeline
      for (int a = 0; a < 5; a++) {
#pragma HLS unroll
              org_cache((a + 1) * 128 - 1, a * 128)=d_apy[y][a];
            }
        for (x = 0; x < 64; x++) {
#pragma HLS unroll
            //Fetch_Orig_Lcu->y_org[y][x] = org_cache((x + 1) * 10 - 1, x * 10);
            Fetch_Orig_Lcu_IME->y_org[y][x] = org_cache((x + 1) * 10 - 1, x * 10);
            Fetch_Orig_Lcu->p_fenc_LCU_Y_rmd[y * 64 + x]=org_cache((x + 1) * 10 - 1, x * 10);
        }
    }
}
#endif
#if Debug422
    U16* p_src[3] = {};
    if (*chroma_format == 2) {
        p_src[1] = imgU_org + pix_y * offset_UV + (pix_x >> 1);
        p_src[2] = imgV_org + pix_y * offset_UV + (pix_x >> 1);
        for (y = 0, i = 0; y < lcu_height; y += 4, i++) {
#pragma HLS loop_tripcount max=32
            for (x = 0; x < lcu_width / 2; x++) {
#pragma HLS loop_tripcount max=32
                Fetch_Orig_Lcu->u_org[2 * i][x] = (3 * p_src[1][y * offset_UV + x] + p_src[1][(y + 2) * offset_UV + x]) / 4;
                Fetch_Orig_Lcu->u_org[2 * i + 1][x] = (p_src[1][(y + 1) * offset_UV + x] + 3 * p_src[1][(y + 3) * offset_UV + x]) / 4;
            }
        }
        for (y = 0, i = 0; y < lcu_height; y += 4, i++) {
#pragma HLS loop_tripcount max=32
            for (x = 0; x < lcu_width / 2; x++) {
#pragma HLS loop_tripcount max=32
                Fetch_Orig_Lcu->v_org[2 * i][x] = (3 * p_src[2][y * offset_UV + x] + p_src[2][(y + 2) * offset_UV + x]) / 4;
                Fetch_Orig_Lcu->v_org[2 * i + 1][x] = (p_src[2][(y + 1) * offset_UV + x] + 3 * p_src[2][(y + 3) * offset_UV + x]) / 4;
            }
        }
        for (y = 0; y < lcu_height / 2; y++) {
#pragma HLS loop_tripcount max=32
            for (x = 0; x < lcu_width / 2; x++) {
#pragma HLS loop_tripcount max=32
                Fetch_Orig_Lcu->p_fenc_LCU_UV[y * 64 + x] = Fetch_Orig_Lcu->u_org[y][x];
                Fetch_Orig_Lcu->p_fenc_LCU_UV[y * 64 + x + 32] = Fetch_Orig_Lcu->v_org[y][x];
                imgU_org[(pix_x >> 1) + x + ((pix_y >> 1) + y) * offset_UV] = Fetch_Orig_Lcu->u_org[y][x];
                imgV_org[(pix_x >> 1) + x + ((pix_y >> 1) + y) * offset_UV] = Fetch_Orig_Lcu->v_org[y][x];
            }
        }
}
#else
if(ctu_ctrl_input->bit_depth_internal == 8){
    ap_uint<12> pic_width_w_d_uv = (((pic_width >> 1) + 7) / 8); //((pic_width >> 1) + 7) >> 3;
    U32 idx_u = (pic_width_w_d_uv * lcu_y_tmp * 32) + lcu_x_tmp * 4;
    U32 idx_v = idx_u;

    for (y = 0; y < 32; y++) {
#pragma HLS unroll
        idx_u_list[y] = idx_u + y * pic_width_w_d_uv;
        idx_v_list[y] = idx_v + y * pic_width_w_d_uv;
    }

    for (y = 0; y < 32; y++) {
#pragma HLS pipeline II=1
        memcpy(&d_apu[y], &imgU_org_ap[idx_u_list[y]], 4*sizeof(ap_uint<64>));
    }

    for (y = 0; y < 32; y++){
#pragma HLS pipeline
      for (int i = 0; i < 4; i++) {
#pragma HLS unroll
            for (int j = 0; j < 8; j++) {
#pragma HLS unroll
                linebuf[i*8+j] = d_apu[y][i]((j + 1) * 8 - 1, j * 8);
        }
      }
        for (x = 0; x < 32; x++) {
#pragma HLS unroll
            //Fetch_Orig_Lcu->u_org[y][x] = pel(linebuf[x]);
            Fetch_Orig_Lcu_IME->u_org[y][x] = pel(linebuf[x]);
        }
    }
    for (y = 0; y < 32; y++) {
#pragma HLS pipeline II=1
        memcpy(&d_apv[y], &imgV_org_ap[idx_v_list[y]],4*sizeof(ap_uint<64>));
    }
    for (y = 0; y < 32; y++){
#pragma HLS pipeline
      for (int i = 0; i < 4; i++) {
#pragma HLS unroll
            for (int j = 0; j < 8; j++) {
#pragma HLS unroll
                linebuf[i*8+j] = d_apv[y][i]((j + 1) * 8 - 1, j * 8);
            }
      }
        for (x = 0; x < 32; x++) {
#pragma HLS unroll
            //Fetch_Orig_Lcu->v_org[y][x] = pel(linebuf[x]);
            Fetch_Orig_Lcu_IME->v_org[y][x] = pel(linebuf[x]);
        }
    }
}
#ifndef __SYNTHESIS__
else if(ctu_ctrl_input->bit_depth_internal == 10){
    ap_uint<12> pic_width_w_d_uv = (((pic_width >> 1) + 31) / 32) * 5; //((pic_width >> 1) + 7) >> 3;
    U32 idx_u = (pic_width_w_d_uv * lcu_y_tmp * 32) + lcu_x_tmp * 5;
    U32 idx_v = idx_u;
    for (y = 0; y < 32; y++) {
#pragma HLS unroll
        idx_u_list[y] = idx_u + y * pic_width_w_d_uv;
        idx_v_list[y] = idx_v + y * pic_width_w_d_uv;
    }
    for (y = 0; y < 32; y++) {
#pragma HLS pipeline II=1
        memcpy(&d_ap[y], &imgU_org_ap[idx_u_list[y]], 5*sizeof(uint64_t));
    }
    for (y = 0; y < 32; y++){
#pragma HLS pipeline
      for (int a = 0; a < 5; a++) {
#pragma HLS unroll
              org_cache((a + 1) * 64 - 1, a * 64)= d_ap[y][a];
            }
      for (x = 0; x < 32; x++) {
#pragma HLS unroll
            //Fetch_Orig_Lcu->u_org[y][x] = org_cache((x + 1) * 10 - 1, x * 10);
            Fetch_Orig_Lcu_IME->u_org[y][x] = org_cache((x + 1) * 10 - 1, x * 10);
        }
    }
    for (y = 0; y < 32; y++) {
#pragma HLS pipeline II=1
        memcpy(&d_ap[y], &imgV_org_ap[idx_v_list[y]],5*sizeof(uint64_t));
    }
    for (y = 0; y < 32; y++){
#pragma HLS pipeline
      for (int a = 0; a < 5; a++) {
#pragma HLS unroll
              org_cache((a + 1) * 64 - 1, a * 64)=d_ap[y][a];
            }
      for (x = 0; x < 32; x++) {
#pragma HLS unroll
            //Fetch_Orig_Lcu->v_org[y][x] = org_cache((x + 1) * 10 - 1, x * 10);
            Fetch_Orig_Lcu_IME->v_org[y][x] = org_cache((x + 1) * 10 - 1, x * 10);
        }
    }
}
#endif
#endif

}


static void init_ime_line_inner_info(int  ctx_info_pic_width_in_scu, int x_cur_ctu, int y_cur_ctu, ENC_ME_LINE_MAP* me_line_ptr, int ctu_idx,
                                     U32 map_cu_mode_inner_to_store[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN])
{
    int i = 0;
    int j = 0;
    int x_scu = x_cur_ctu >> 2;
    int y_scu = y_cur_ctu >> 2;
    int lcu_width_in_scu = 1 << 7 >> 1 >> 2;
    int pic_width_in_scu = ctx_info_pic_width_in_scu;

#if FETCH_PRAGMA_ON
#pragma HLS ARRAY_PARTITION variable=me_line_ptr->map_scu_inner complete dim=1
#pragma HLS ARRAY_PARTITION variable=me_line_ptr->map_cu_mode_inner complete dim=1
#pragma HLS ARRAY_PARTITION variable=map_cu_mode_inner_to_store complete dim=1
#endif  

LOOP3:
    for (i = 0; i < 16; i++) 
    {
#if FETCH_PRAGMA_ON
#pragma HLS pipeline
#pragma HLS loop_tripcount max = 16
#endif  
        for (j = 0; j < 16; j++) 
        {
#if FETCH_PRAGMA_ON
#pragma HLS unroll
#pragma HLS loop_tripcount max = 16
#endif  
            me_line_ptr->map_scu_inner[ctu_idx][i][j] = ((u32)0);
            me_line_ptr->map_cu_mode_inner[ctu_idx][i][j] = ((u32)0);
            map_cu_mode_inner_to_store[ctu_idx][i][j]=((u32)0);
        }
    }

}

static void init_ime_line_above_line(ENC_ME_LINE_MAP* me_line_ptr, int above_idx, ENC_ME_NEB_INFO above_nebs_line[2][MAX_WIDTH >> MIN_CU_LOG2])
{
    int i = 0;

    for (i = 0; i < me_line_ptr->pic_width_in_scu; i++) {
//#pragma HLS pipeline
#pragma HLS loop_tripcount max = 480
        init_neb_info(&above_nebs_line[above_idx][i]);
    }

}


static void fetch_line(
    int* core_x_pel, 
    int* core_y_pel, 
    CTX_FETCH* fetch_ctx_io,

    s32(*map_mv)[REFP_NUM][MV_D], 
    s32(*map_refi)[REFP_NUM], 
    u32* map_scu, 
    u32* map_cu_mode,

    ENC_ME_LINE_MAP* me_line_ptr,
    ENC_ME_LINE_MAP_IME* me_line_map_ptr_IME,
    ENC_ME_LINE_MAP_IME* me_line_map_ptr_IME_1,
    ENC_ME_NEB_INFO new_above_nebs_line[ABOVE_LINE_LEN],
    ENC_ME_NEB_INFO new_above_nebs_line_IME[ABOVE_LINE_LEN],
    ENC_ME_NEB_INFO new_above_nebs_line_IME_1[ABOVE_LINE_LEN],
    CTU_Level_Parameters* fetch_output, int lcu_cnt,
    U32 map_cu_mode_inner_to_store[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN]
)
{
    int i = 0;
    int j = 0;
    s8 lidx = (s8)0;
    int x_scu = 0;
    int y_scu = 0;
    int x_scu_buf = 0;
    int y_scu_buf = 0;
    int lcu_width_in_scu = 16;
    int pic_width_in_scu = fetch_ctx_io->ctx_info_pic_width_in_scu;
    int pic_height_in_scu = fetch_ctx_io->ctx_info_pic_height_in_scu;
    u8 scan_type = (u8)0;

    int x_cur_ctu = *core_x_pel;
    int y_cur_ctu = *core_y_pel;
    int bu_idx = 0;
    int curr_ctu_idx = 0;
    int res_height = 0;
    int x_last_ctu = x_cur_ctu - MAX_CU_SIZE2;
    int switch_flag = 0; //new line flag
#if PATCH_DEBUG
    static int last_pos = 0;
    switch_flag = x_cur_ctu - patch->left_pel - MAX_CU_SIZE2; //new line flag
#else
    switch_flag = x_last_ctu;
#endif
    int y_last_ctu = y_cur_ctu;
    if (switch_flag < 0)
    {
#if PATCH_DEBUG
        if (patch->patch_switch == 1 && patch->pat == 0)
            x_last_ctu = x_cur_ctu;
        else
#endif
        {
            if (x_cur_ctu == 0 && y_cur_ctu == 0)
                x_last_ctu = 0;
            else
            {
#if PATCH_DEBUG
                x_last_ctu = last_pos;
#else
                x_last_ctu = (fetch_ctx_io->ctx_info_pic_width_in_lcu - 1) << MAX_CU_LOG2;
#endif
                y_last_ctu = y_cur_ctu - MAX_CU_SIZE2;

            }
        }
    }
    fetch_output->x_last_ctu = x_last_ctu;
    fetch_output->y_last_ctu = y_last_ctu;

    me_line_ptr->stride_in_map = pic_width_in_scu;
    me_line_ptr->stride_in_line = lcu_width_in_scu;
    me_line_ptr->pic_width_in_scu = fetch_ctx_io->ctx_info_pic_width_in_scu;
    me_line_ptr->pic_height_in_scu = fetch_ctx_io->ctx_info_pic_height_in_scu;
    int baseAboveScuX = (x_cur_ctu >> 2) - SCUX_OFFSET;
    int baseAboveScuY = (y_cur_ctu >> 2) - 1;
    int aboveScu = 0;

    aboveScu = baseAboveScuX + (baseAboveScuY * fetch_ctx_io->ctx_info_pic_width_in_scu);
    s16 map_mv_tmp[48][2][2];
    s8 map_refi_tmp[48][2];
    u32 map_cu_mode_tmp[48];
    u32 map_scu_tmp[48];
#if FETCH_PRAGMA_ON
#pragma HLS ARRAY_PARTITION variable=map_mv_tmp complete dim=0
#pragma HLS ARRAY_PARTITION variable=map_refi_tmp complete dim=0
#pragma HLS ARRAY_PARTITION variable=map_cu_mode_tmp complete dim=0
#pragma HLS ARRAY_PARTITION variable=map_scu_tmp complete dim=0
#endif  
    for (i = 0; i < ABOVE_LINE_LEN; i++)
    {
    	//#pragma HLS unroll
#pragma HLS pipeline
        if (baseAboveScuY >= 0 && baseAboveScuX + i >= 0 && baseAboveScuX < fetch_ctx_io->ctx_info_pic_width_in_scu)
        {
            map_mv_tmp[i][0][0] = map_mv[aboveScu + i][0][0];
            map_mv_tmp[i][0][1] = map_mv[aboveScu + i][0][1];
            map_mv_tmp[i][1][0] = map_mv[aboveScu + i][1][0];
            map_mv_tmp[i][1][1] = map_mv[aboveScu + i][1][1];

            map_refi_tmp[i][0] = map_refi[aboveScu + i][0];
            map_refi_tmp[i][1] = map_refi[aboveScu + i][1];

            map_cu_mode_tmp[i] = map_cu_mode[aboveScu + i];
            map_scu_tmp[i] = map_scu[aboveScu + i];
        }
    }


#if FETCH_PRAGMA_ON
//#pragma HLS ARRAY_PARTITION variable=new_above_nebs_line->mv   complete dim=0
//#pragma HLS ARRAY_PARTITION variable=new_above_nebs_line->refi complete dim=0
//#pragma HLS ARRAY_PARTITION variable=new_above_nebs_line_IME complete dim=0
//#pragma HLS ARRAY_PARTITION variable=fetch2dbk_new_above_nebs_line complete dim=0

#pragma HLS ARRAY_PARTITION variable=me_line_ptr->new_map_scu_above_line complete dim=1
//#pragma HLS ARRAY_PARTITION variable=me_line_map_ptr_IME->new_map_scu_above_line complete dim=1
#pragma HLS ARRAY_PARTITION variable=me_line_ptr->new_map_cu_mode_above_line complete dim=1
//#pragma HLS ARRAY_PARTITION variable=me_line_map_2dbk->new_map_scu_above_line complete dim=1
#endif  
LOOP1:
    for (i = 0; i < ABOVE_LINE_LEN; i++)
    {

#if FETCH_PRAGMA_ON
//#pragma HLS unroll
#pragma HLS pipeline
#pragma HLS loop_tripcount max=48
#endif 
        if (baseAboveScuY < 0 || baseAboveScuX + i < 0 || baseAboveScuX >= fetch_ctx_io->ctx_info_pic_width_in_scu)
        {
            init_neb_info(&new_above_nebs_line[i]);
            init_neb_info(&new_above_nebs_line_IME[i]);
            init_neb_info(&new_above_nebs_line_IME_1[i]);
            me_line_ptr->new_map_scu_above_line[i] = (U32)0;
            me_line_map_ptr_IME->new_map_scu_above_line[i] = (U32)0;
            me_line_map_ptr_IME_1->new_map_scu_above_line[i] = (U32)0;
            me_line_ptr->new_map_cu_mode_above_line[i] = (U32)0;
        }
        else
        {
            new_above_nebs_line[i].mv[0][0] = map_mv_tmp[i][0][0];
            new_above_nebs_line[i].mv[0][1] = map_mv_tmp[i][0][1];
            new_above_nebs_line[i].refi[0] = map_refi_tmp[i][0];
            new_above_nebs_line[i].mv[1][0] = map_mv_tmp[i][1][0];
            new_above_nebs_line[i].mv[1][1] = map_mv_tmp[i][1][1];
            new_above_nebs_line[i].refi[1] = map_refi_tmp[i][1];

            new_above_nebs_line_IME[i].mv[0][0] = map_mv_tmp[i][0][0];
            new_above_nebs_line_IME[i].mv[0][1] = map_mv_tmp[i][0][1];
            new_above_nebs_line_IME[i].refi[0] = map_refi_tmp[i][0];
            new_above_nebs_line_IME[i].mv[1][0] = map_mv_tmp[i][1][0];
            new_above_nebs_line_IME[i].mv[1][1] = map_mv_tmp[i][1][1];
            new_above_nebs_line_IME[i].refi[1] = map_refi_tmp[i][1];

            new_above_nebs_line_IME_1[i].mv[0][0] = map_mv_tmp[i][0][0];
            new_above_nebs_line_IME_1[i].mv[0][1] = map_mv_tmp[i][0][1];
            new_above_nebs_line_IME_1[i].refi[0] = map_refi_tmp[i][0];
            new_above_nebs_line_IME_1[i].mv[1][0] = map_mv_tmp[i][1][0];
            new_above_nebs_line_IME_1[i].mv[1][1] = map_mv_tmp[i][1][1];
            new_above_nebs_line_IME_1[i].refi[1] = map_refi_tmp[i][1];

            me_line_ptr->new_map_scu_above_line[i] = map_scu_tmp[i];
            me_line_map_ptr_IME->new_map_scu_above_line[i] = map_scu_tmp[i];
            me_line_map_ptr_IME_1->new_map_scu_above_line[i] = map_scu_tmp[i];
            me_line_ptr->new_map_cu_mode_above_line[i] = map_cu_mode_tmp[i];
        }
    }


#if PATCH_DEBUG
    if (patch->patch_switch == 1 && patch->pat == 0)
    {
        scan_type = 0;
    }
    int next_curr_ctu_idx = 0;
    int next_above_line_idx = 0;

    if (scan_type == 2)
    {
        next_above_line_idx = 1 - me_line_ptr->above_line_idx;
    }
    else
    {
        next_above_line_idx = me_line_ptr->above_line_idx;
    }

    if (scan_type == 0)
    {
        next_curr_ctu_idx = 0;
        next_above_line_idx = 0;
    }
    else
    {
        next_curr_ctu_idx = 1 - me_line_ptr->curr_ctu_idx;
    }

#else
    int next_curr_ctu_idx = lcu_cnt % 2;
    int next_above_line_idx = ((int)(lcu_cnt / fetch_ctx_io->ctx_info_pic_width_in_lcu)) % 2;
#endif
    int cur_curr_ctu_idx = 1 - next_curr_ctu_idx;
    int cur_above_line_idx = 1 - next_above_line_idx;

    init_ime_line_inner_info(fetch_ctx_io->ctx_info_pic_width_in_scu, x_cur_ctu, y_cur_ctu, me_line_ptr, next_curr_ctu_idx,map_cu_mode_inner_to_store);
    
#if PATCH_DEBUG
    last_pos = x_cur_ctu;
#endif
    me_line_ptr->x_ctu = x_cur_ctu;
    me_line_ptr->y_ctu = y_cur_ctu;
    me_line_ptr->min_scu_x = x_cur_ctu >> 2;
    me_line_ptr->min_scu_y = y_cur_ctu >> 2;
    me_line_map_ptr_IME->min_scu_x = x_cur_ctu >> 2;
    //me_line_map_ptr_IME_1->min_scu_x = x_cur_ctu >> 2;
    me_line_ptr->curr_ctu_idx = next_curr_ctu_idx;
    me_line_ptr->above_line_idx = next_above_line_idx;

}

#if ENABLE_RFC_FETCH

static void ref_fetch_lcu_in_rfc_by_block(
    int pix_x,
    int pix_y,
    CTX_FETCH* fetch_ctx_io,
    pel* y_rfcBin,
    U16* y_rfcPrm,
    pel* u_rfcBin,
    U16* u_rfcPrm,
    pel* v_rfcBin,
    U16* v_rfcPrm,
    //output
    pel ctu_Y[64][64],
    pel ctu_U[32][32],
    pel ctu_V[32][32]
) {
    DCP m_dcp2;
    pel  dat[192][192] = { 0 };
    pel  dcpBitBuffer[192][192] = { 0 };
    u16 dcpPrmBuffer[48][6] = { 0 };

    int block_stride_luma = 64 * 64;
    int block_stride_chroma = 32 * 32;
    int block_num = pix_y / 64 * ((fetch_ctx_io->ctx_pinter_refp_pic_width_luma + 63) / 64) + pix_x / 64;
    int size_total = 0;
    int size_total_u = 0;
    int size_total_v = 0;

    // fetch luma CTU
    for (int idxY = 0; idxY < 64; idxY += 4) {
        for (int idxX = 0; idxX < 64; idxX += 32) {

            dcpPrmBuffer[0][0] = y_rfcPrm[((pix_y + idxY) / 4) * fetch_ctx_io->ctx_pinter_refp_pic_stride_luma / 32 + (pix_x + idxX) / 32];
            int size = (dcpPrmBuffer[0][0] & (0x000f)) + 1;
            int row = (size * 128 + 32 * 16 - 1) / (32 * 16) - 1;
            int last_row_width = (size * 128 - row * 32 * 16) / 16;


            for (int i = 0; i < row; i++) {
                for (int j = 0; j < 32; j++) {
                    dcpBitBuffer[i][j] = y_rfcBin[block_num * block_stride_luma + size_total+ i * 32 + j];
                }
            }

            for (int i = 0; i < last_row_width; i++) {
                dcpBitBuffer[row][i] = y_rfcBin[block_num * block_stride_luma + size_total + row * 32 + i];
            }

       

            m_dcp2.proc(dcpBitBuffer, dcpPrmBuffer, 0, 32, 4);
            m_dcp2.cpyDatOri(dat, 0, 32, 4);

            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 32; j++) {
                    ctu_Y[idxY + i][idxX + j] = dat[i][j];
                }
            }

            size_total += (128 * size / 16);
        }
    }

    for (int idxY = 0; idxY < 32; idxY += 4) {
        for (int idxX = 0; idxX < 32; idxX += 32) {

            dcpPrmBuffer[0][0] = u_rfcPrm[((pix_y/2 + idxY) / 4) * fetch_ctx_io->ctx_pinter_refp_pic_stride_chroma / 32 + (pix_x/2 + idxX) / 32];
            int size = (dcpPrmBuffer[0][0] & (0x000f)) + 1;
            int row = (size * 128 + 32 * 16 - 1) / (32 * 16) - 1;
            int last_row_width = (size * 128 - row * 32 * 16) / 16;


            for (int i = 0; i < row; i++) {
                for (int j = 0; j < 32; j++) {
                    dcpBitBuffer[i][j] = u_rfcBin[block_num * block_stride_chroma + size_total_u + i * 32 + j];
                }
            }

            for (int i = 0; i < last_row_width; i++) {
                dcpBitBuffer[row][i] = u_rfcBin[block_num * block_stride_chroma + size_total_u + row * 32 + i];
            }

           

            m_dcp2.proc(dcpBitBuffer, dcpPrmBuffer, 0, 32, 4);
            m_dcp2.cpyDatOri(dat, 0, 32, 4);

            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 32; j++) {
                    ctu_U[idxY + i][idxX + j] = dat[i][j];
                }
            }
            size_total_u += (128 * size / 16);
        }
    }

    for (int idxY = 0; idxY < 32; idxY += 4) {
        for (int idxX = 0; idxX < 32; idxX += 32) {

            dcpPrmBuffer[0][0] = v_rfcPrm[((pix_y / 2 + idxY) / 4) * fetch_ctx_io->ctx_pinter_refp_pic_stride_chroma / 32 + (pix_x / 2 + idxX) / 32];
            int size = (dcpPrmBuffer[0][0] & (0x000f)) + 1;
            int row = (size * 128 + 32 * 16 - 1) / (32 * 16) - 1;
            int last_row_width = (size * 128 - row * 32 * 16) / 16;


            for (int i = 0; i < row; i++) {
                for (int j = 0; j < 32; j++) {
                    dcpBitBuffer[i][j] = v_rfcBin[block_num * block_stride_chroma + size_total_v + i * 32 + j];
                }
            }

            for (int i = 0; i < last_row_width; i++) {
                dcpBitBuffer[row][i] = v_rfcBin[block_num * block_stride_chroma + size_total_v + row * 32 + i];
            }



            m_dcp2.proc(dcpBitBuffer, dcpPrmBuffer, 0, 32, 4);
            m_dcp2.cpyDatOri(dat, 0, 32, 4);

            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 32; j++) {
                    ctu_V[idxY + i][idxX + j] = dat[i][j];
                }
            }
            size_total_v += (128 * size / 16);
        }
    }

}

static void ref_fetch_lcu_in_rfc(
    //input
    int pix_x,
    int pix_y,
    CTX_FETCH* fetch_ctx_io, 
    pel* y_rfcBin,
    U16* y_rfcPrm,
    pel* u_rfcBin,
    U16* u_rfcPrm,
    pel* v_rfcBin,
    U16* v_rfcPrm,
    //output
    pel ctu_Y[64][64],
    pel ctu_U[32][32],
    pel ctu_V[32][32]
 ) {
    DCP m_dcp1;
    pel  dat[192][192] = { 0 };
    pel  dcpBitBuffer[192][192] = { 0 };
    u16 dcpPrmBuffer[48][6] = { 0 };
    pel  dcpBitBuffer_uv[192][192] = { 0 };
    u16 dcpPrmBuffer_uv[48][6] = { 0 };


/******************************fetch 4*32 block each time***********************/
    for (int idxY = 0; idxY < 64; idxY += 4) {
        for (int idxX = 0; idxX < 64; idxX += 32) {

            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 32; j++) {
                    dcpBitBuffer[i][j] = y_rfcBin[(pix_y + idxY + i) * fetch_ctx_io->ctx_pinter_refp_pic_stride_luma + pix_x + idxX + j];
                }
            }
            dcpPrmBuffer[0][0] = y_rfcPrm[((pix_y + idxY) / 4) * fetch_ctx_io->ctx_pinter_refp_pic_stride_luma / 32 + (pix_x + idxX) / 32];
            m_dcp1.proc(dcpBitBuffer, dcpPrmBuffer, 0, 32, 4);
            m_dcp1.cpyDatOri(dat, 0, 32, 4);

            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 32; j++) {
                    ctu_Y[idxY + i][idxX + j] = dat[i][j];
                }
            }
        }
    }

    //fetch chroma RFC bins and prms from frame-level buffer, then conduct DCP and derive the rec data    
   for (int idxY = 0; idxY < 32; idxY += 4) {
       for (int idxX = 0; idxX < 32; idxX += 32) {

           for (int i = 0; i < 4; i++) {
               for (int j = 0; j < 32; j++) {
                   dcpBitBuffer[i][j] = u_rfcBin[(pix_y / 2 + idxY + i) * fetch_ctx_io->ctx_pinter_refp_pic_stride_chroma + pix_x / 2 + idxX + j];
                   dcpBitBuffer_uv[i][j] = v_rfcBin[(pix_y / 2 + idxY + i) * fetch_ctx_io->ctx_pinter_refp_pic_stride_chroma + pix_x / 2 + idxX + j];

               }
           }
           dcpPrmBuffer[0][0] = u_rfcPrm[((pix_y / 2 + idxY) / 4) * fetch_ctx_io->ctx_pinter_refp_pic_stride_chroma / 32 + (pix_x / 2 + idxX) / 32];
           dcpPrmBuffer_uv[0][0] = v_rfcPrm[((pix_y / 2 + idxY) / 4) * fetch_ctx_io->ctx_pinter_refp_pic_stride_chroma / 32 + (pix_x / 2 + idxX) / 32];

           m_dcp1.proc(dcpBitBuffer, dcpPrmBuffer, 0, 32, 4);
           m_dcp1.cpyDatOri(dat, 0, 32, 4);

           for (int i = 0; i < 4; i++) {
               for (int j = 0; j < 32; j++) {
                   ctu_U[idxY + i][idxX + j] = dat[i][j];
               }
           }
           m_dcp1.proc(dcpBitBuffer_uv, dcpPrmBuffer_uv, 0, 32, 4);
           m_dcp1.cpyDatOri(dat, 0, 32, 4);
           for (int i = 0; i < 4; i++) {
               for (int j = 0; j < 32; j++) {
                   ctu_V[idxY + i][idxX + j] = dat[i][j];
               }
           }

       }
   }

/****************************fetch a CTU each time ****************************/
//fetch luma CTU
//    for (int i = 0; i < 64; i++) {
//        for (int j = 0; j < 64; j++) {
//            dcpBitBuffer[i][j] = y_rfcBin[(pix_y + i) * fetch_ctx_io->ctx_pinter_refp_pic_stride_luma + pix_x + j];
//        }
//    }
//    for (int i = 0; i < 64/4; i++) {
//        for (int j = 0; j < 64/32; j++) {
//            dcpPrmBuffer[i][j] = y_rfcPrm[(pix_y/4 + i) * fetch_ctx_io->ctx_pinter_refp_pic_stride_luma/32 + pix_x/32 + j];
//        }
//    }
//    m_dcp1.proc(dcpBitBuffer, dcpPrmBuffer, 0, 64, 64);
//    m_dcp1.cpyDatOri(dat, 0, 64, 64);
//
//    for (int i = 0; i <64; i++) {
//        for (int j = 0; j < 64; j++) {
//            ctu_Y[i][j] = dat[i][j];
//        }
//    }
//
////fetch chroma CTU
//    for (int i = 0; i < 32; i++) {
//        for (int j = 0; j < 32; j++) {
//            dcpBitBuffer[i][j] = u_rfcBin[(pix_y/2 + i) * fetch_ctx_io->ctx_pinter_refp_pic_stride_chroma + pix_x/2 + j];
//            dcpBitBuffer_uv[i][j] = v_rfcBin[(pix_y / 2 + i) * fetch_ctx_io->ctx_pinter_refp_pic_stride_chroma + pix_x / 2 + j];
//
//        }
//    }
//    for (int i = 0; i < 32 / 4; i++) {
//        for (int j = 0; j < 32 / 32; j++) {
//            dcpPrmBuffer[i][j] = u_rfcPrm[(pix_y/2/ 4 + i) * fetch_ctx_io->ctx_pinter_refp_pic_stride_chroma / 32 + pix_x/2/ 32 + j];
//            dcpPrmBuffer_uv[i][j] = v_rfcPrm[(pix_y / 2 / 4 + i) * fetch_ctx_io->ctx_pinter_refp_pic_stride_chroma / 32 + pix_x / 2 / 32 + j];
//
//        }
//    }
//
//    m_dcp1.proc(dcpBitBuffer, dcpPrmBuffer, 0, 32, 32);
//    m_dcp1.cpyDatOri(dat, 0, 32, 32);
//
//    for (int i = 0; i < 32; i++) {
//        for (int j = 0; j < 32; j++) {
//            ctu_U[i][j] = dat[i][j];
//        }
//    }
//
//    m_dcp1.proc(dcpBitBuffer_uv, dcpPrmBuffer_uv, 0, 32, 32);
//    m_dcp1.cpyDatOri(dat, 0, 32, 32);
//
//    for (int i = 0; i < 32; i++) {
//        for (int j = 0; j < 32; j++) {
//            ctu_V[i][j] = dat[i][j];
//        }
//    }
}

static void ref_fetch_lcu_in_rfc_1(
    //input
    int pix_x,
    int pix_y,
    CTX_FETCH* fetch_ctx_io, 
    pel* y_rfcBin,
    u16* y_rfcPrm,
    pel* u_rfcBin,
    u16* u_rfcPrm,
    pel* v_rfcBin,
    u16* v_rfcPrm,
    //output
    pel ctu_Y[64][64],
    pel ctu_U[32][32],
    pel ctu_V[32][32]
 ) {
    DCP m_dcp1;
    pel  dat[192][192] = { 0 };
    pel  dcpBitBuffer[192][192] = { 0 };
    u16 dcpPrmBuffer[48][6] = { 0 };
    pel  dcpBitBuffer_uv[192][192] = { 0 };
    u16 dcpPrmBuffer_uv[48][6] = { 0 };


/******************************fetch 4*32 block each time***********************/
    for (int idxY = 0; idxY < 64; idxY += 4) {
        for (int idxX = 0; idxX < 64; idxX += 32) {

            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 32; j++) {
                    dcpBitBuffer[i][j] = y_rfcBin[(pix_y + idxY + i) * fetch_ctx_io->ctx_pinter_refp_pic_stride_luma + pix_x + idxX + j];
                }
            }
            dcpPrmBuffer[0][0] = y_rfcPrm[((pix_y + idxY) / 4) * fetch_ctx_io->ctx_pinter_refp_pic_stride_luma / 32 + (pix_x + idxX) / 32];
            m_dcp1.proc(dcpBitBuffer, dcpPrmBuffer, 0, 32, 4);
            m_dcp1.cpyDatOri(dat, 0, 32, 4);

            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 32; j++) {
                    ctu_Y[idxY + i][idxX + j] = dat[i][j];
                }
            }
        }
    }

    //fetch chroma RFC bins and prms from frame-level buffer, then conduct DCP and derive the rec data    
   for (int idxY = 0; idxY < 32; idxY += 4) {
       for (int idxX = 0; idxX < 32; idxX += 32) {

           for (int i = 0; i < 4; i++) {
               for (int j = 0; j < 32; j++) {
                   dcpBitBuffer[i][j] = u_rfcBin[(pix_y / 2 + idxY + i) * fetch_ctx_io->ctx_pinter_refp_pic_stride_chroma + pix_x / 2 + idxX + j];
                   dcpBitBuffer_uv[i][j] = v_rfcBin[(pix_y / 2 + idxY + i) * fetch_ctx_io->ctx_pinter_refp_pic_stride_chroma + pix_x / 2 + idxX + j];

               }
           }
           dcpPrmBuffer[0][0] = u_rfcPrm[((pix_y / 2 + idxY) / 4) * fetch_ctx_io->ctx_pinter_refp_pic_stride_chroma / 32 + (pix_x / 2 + idxX) / 32];
           dcpPrmBuffer_uv[0][0] = v_rfcPrm[((pix_y / 2 + idxY) / 4) * fetch_ctx_io->ctx_pinter_refp_pic_stride_chroma / 32 + (pix_x / 2 + idxX) / 32];

           m_dcp1.proc(dcpBitBuffer, dcpPrmBuffer, 0, 32, 4);
           m_dcp1.cpyDatOri(dat, 0, 32, 4);

           for (int i = 0; i < 4; i++) {
               for (int j = 0; j < 32; j++) {
                   ctu_U[idxY + i][idxX + j] = dat[i][j];
               }
           }
           m_dcp1.proc(dcpBitBuffer_uv, dcpPrmBuffer_uv, 0, 32, 4);
           m_dcp1.cpyDatOri(dat, 0, 32, 4);
           for (int i = 0; i < 4; i++) {
               for (int j = 0; j < 32; j++) {
                   ctu_V[idxY + i][idxX + j] = dat[i][j];
               }
           }

       }
   }
}

static void ref_fetch_lcu_in_rfc_2(
    //input
    int pix_x,
    int pix_y,
    CTX_FETCH* fetch_ctx_io, 
    pel* y_rfcBin,
    u16* y_rfcPrm,
    pel* u_rfcBin,
    u16* u_rfcPrm,
    pel* v_rfcBin,
    u16* v_rfcPrm,
    //output
    pel ctu_Y[64][64],
    pel ctu_U[32][32],
    pel ctu_V[32][32]
 ) {
    DCP m_dcp1;
    pel  dat[192][192] = { 0 };
    pel  dcpBitBuffer[192][192] = { 0 };
    u16 dcpPrmBuffer[48][6] = { 0 };
    pel  dcpBitBuffer_uv[192][192] = { 0 };
    u16 dcpPrmBuffer_uv[48][6] = { 0 };


/******************************fetch 4*32 block each time***********************/
    for (int idxY = 0; idxY < 64; idxY += 4) {
        for (int idxX = 0; idxX < 64; idxX += 32) {

            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 32; j++) {
                    dcpBitBuffer[i][j] = y_rfcBin[(pix_y + idxY + i) * fetch_ctx_io->ctx_pinter_refp_pic_stride_luma + pix_x + idxX + j];
                }
            }
            dcpPrmBuffer[0][0] = y_rfcPrm[((pix_y + idxY) / 4) * fetch_ctx_io->ctx_pinter_refp_pic_stride_luma / 32 + (pix_x + idxX) / 32];
            m_dcp1.proc(dcpBitBuffer, dcpPrmBuffer, 0, 32, 4);
            m_dcp1.cpyDatOri(dat, 0, 32, 4);

            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 32; j++) {
                    ctu_Y[idxY + i][idxX + j] = dat[i][j];
                }
            }
        }
    }

    //fetch chroma RFC bins and prms from frame-level buffer, then conduct DCP and derive the rec data    
   for (int idxY = 0; idxY < 32; idxY += 4) {
       for (int idxX = 0; idxX < 32; idxX += 32) {

           for (int i = 0; i < 4; i++) {
               for (int j = 0; j < 32; j++) {
                   dcpBitBuffer[i][j] = u_rfcBin[(pix_y / 2 + idxY + i) * fetch_ctx_io->ctx_pinter_refp_pic_stride_chroma + pix_x / 2 + idxX + j];
                   dcpBitBuffer_uv[i][j] = v_rfcBin[(pix_y / 2 + idxY + i) * fetch_ctx_io->ctx_pinter_refp_pic_stride_chroma + pix_x / 2 + idxX + j];

               }
           }
           dcpPrmBuffer[0][0] = u_rfcPrm[((pix_y / 2 + idxY) / 4) * fetch_ctx_io->ctx_pinter_refp_pic_stride_chroma / 32 + (pix_x / 2 + idxX) / 32];
           dcpPrmBuffer_uv[0][0] = v_rfcPrm[((pix_y / 2 + idxY) / 4) * fetch_ctx_io->ctx_pinter_refp_pic_stride_chroma / 32 + (pix_x / 2 + idxX) / 32];

           m_dcp1.proc(dcpBitBuffer, dcpPrmBuffer, 0, 32, 4);
           m_dcp1.cpyDatOri(dat, 0, 32, 4);

           for (int i = 0; i < 4; i++) {
               for (int j = 0; j < 32; j++) {
                   ctu_U[idxY + i][idxX + j] = dat[i][j];
               }
           }
           m_dcp1.proc(dcpBitBuffer_uv, dcpPrmBuffer_uv, 0, 32, 4);
           m_dcp1.cpyDatOri(dat, 0, 32, 4);
           for (int i = 0; i < 4; i++) {
               for (int j = 0; j < 32; j++) {
                   ctu_V[idxY + i][idxX + j] = dat[i][j];
               }
           }

       }
   }
}

static void ref_fetch_lcu_in_rfc_3(
    //input
    int pix_x,
    int pix_y,
    CTX_FETCH* fetch_ctx_io, 
    pel* y_rfcBin,
    u16* y_rfcPrm,
    pel* u_rfcBin,
    u16* u_rfcPrm,
    pel* v_rfcBin,
    u16* v_rfcPrm,
    //output
    pel ctu_Y[64][64],
    pel ctu_U[32][32],
    pel ctu_V[32][32]
 ) {
    DCP m_dcp1;
    pel  dat[192][192] = { 0 };
    pel  dcpBitBuffer[192][192] = { 0 };
    u16 dcpPrmBuffer[48][6] = { 0 };
    pel  dcpBitBuffer_uv[192][192] = { 0 };
    u16 dcpPrmBuffer_uv[48][6] = { 0 };


/******************************fetch 4*32 block each time***********************/
    for (int idxY = 0; idxY < 64; idxY += 4) {
        for (int idxX = 0; idxX < 64; idxX += 32) {

            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 32; j++) {
                    dcpBitBuffer[i][j] = y_rfcBin[(pix_y + idxY + i) * fetch_ctx_io->ctx_pinter_refp_pic_stride_luma + pix_x + idxX + j];
                }
            }
            dcpPrmBuffer[0][0] = y_rfcPrm[((pix_y + idxY) / 4) * fetch_ctx_io->ctx_pinter_refp_pic_stride_luma / 32 + (pix_x + idxX) / 32];
            m_dcp1.proc(dcpBitBuffer, dcpPrmBuffer, 0, 32, 4);
            m_dcp1.cpyDatOri(dat, 0, 32, 4);

            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 32; j++) {
                    ctu_Y[idxY + i][idxX + j] = dat[i][j];
                }
            }
        }
    }

    //fetch chroma RFC bins and prms from frame-level buffer, then conduct DCP and derive the rec data    
   for (int idxY = 0; idxY < 32; idxY += 4) {
       for (int idxX = 0; idxX < 32; idxX += 32) {

           for (int i = 0; i < 4; i++) {
               for (int j = 0; j < 32; j++) {
                   dcpBitBuffer[i][j] = u_rfcBin[(pix_y / 2 + idxY + i) * fetch_ctx_io->ctx_pinter_refp_pic_stride_chroma + pix_x / 2 + idxX + j];
                   dcpBitBuffer_uv[i][j] = v_rfcBin[(pix_y / 2 + idxY + i) * fetch_ctx_io->ctx_pinter_refp_pic_stride_chroma + pix_x / 2 + idxX + j];

               }
           }
           dcpPrmBuffer[0][0] = u_rfcPrm[((pix_y / 2 + idxY) / 4) * fetch_ctx_io->ctx_pinter_refp_pic_stride_chroma / 32 + (pix_x / 2 + idxX) / 32];
           dcpPrmBuffer_uv[0][0] = v_rfcPrm[((pix_y / 2 + idxY) / 4) * fetch_ctx_io->ctx_pinter_refp_pic_stride_chroma / 32 + (pix_x / 2 + idxX) / 32];

           m_dcp1.proc(dcpBitBuffer, dcpPrmBuffer, 0, 32, 4);
           m_dcp1.cpyDatOri(dat, 0, 32, 4);

           for (int i = 0; i < 4; i++) {
               for (int j = 0; j < 32; j++) {
                   ctu_U[idxY + i][idxX + j] = dat[i][j];
               }
           }
           m_dcp1.proc(dcpBitBuffer_uv, dcpPrmBuffer_uv, 0, 32, 4);
           m_dcp1.cpyDatOri(dat, 0, 32, 4);
           for (int i = 0; i < 4; i++) {
               for (int j = 0; j < 32; j++) {
                   ctu_V[idxY + i][idxX + j] = dat[i][j];
               }
           }

       }
   }
}
#endif

#if FETCH_REF_BLOCK
//removed
#else

#if ADD_by_YHY

#if DEBUG_CHECK
pel refWinY0_test2[192][64];
pel refWinY1_test2[192][64];
pel refWinY2_test2[192][64];
pel refWinY3_test2[192][64];

pel refWinU0_test2[96][32];
pel refWinU1_test2[96][32];
pel refWinU2_test2[96][32];
pel refWinU3_test2[96][32];

pel refWinV0_test2[96][32];
pel refWinV1_test2[96][32];
pel refWinV2_test2[96][32];
pel refWinV3_test2[96][32];

pel refWinY0_B_test2[192][64];
pel refWinY1_B_test2[192][64];
pel refWinY2_B_test2[192][64];
pel refWinY3_B_test2[192][64];

pel refWinU0_B_test2[96][32];
pel refWinU1_B_test2[96][32];
pel refWinU2_B_test2[96][32];
pel refWinU3_B_test2[96][32];

pel refWinV0_B_test2[96][32];
pel refWinV1_B_test2[96][32];
pel refWinV2_B_test2[96][32];
pel refWinV3_B_test2[96][32];
#endif
void lcu_fetch_ref_pixels(
    int* core_x_pel, 
    int* core_y_pel, 
    CTX_FETCH* fetch_ctx_io,
    U8 ctx_rpm_num_refp_0,
    U8 ctx_rpm_num_refp_1,
    int b_use_flag,
#if DEBUG_CHECK
    uint128_t* RefbufY1,
    uint128_t* RefbufU1,
    uint128_t* RefbufV1,
#endif
    //pel refWinY0_temp[192][64],
    //pel refWinY1_temp[192][64],
    //pel refWinY2_temp[192][64],
    //pel refWinY3_temp[192][64],
    //pel swOutLv0_temp[192][256],
    pel swOutLv0_ime_temp[192][256],
    pel* refWinU_temp,
    pel* refWinV_temp,

    pel refWinY32x192_ref_temp[192][32],
    pel refWinY0_ref_temp[192][64],
    pel refWinY1_ref_temp[192][64],
    pel refWinY2_ref_temp[192][64],
    pel refWinY3_ref_temp[192][64],

    pel refWinU16x96_ref_temp[96][16],
    pel refWinU0_ref_temp[96][32],
    pel refWinU1_ref_temp[96][32],
    pel refWinU2_ref_temp[96][32],
    pel refWinU3_ref_temp[96][32],

    pel refWinV16x96_ref_temp[96][16],
    pel refWinV0_ref_temp[96][32],
    pel refWinV1_ref_temp[96][32],
    pel refWinV2_ref_temp[96][32],
    pel refWinV3_ref_temp[96][32]

#if ENABLE_BFRAME
	,int code_stru
#endif
)
{
    int pix_x = *core_x_pel;
    int pix_y = *core_y_pel;

/////////////////////////////////////////////////////////////////////////////////////////////////
    static ap_uint<10> refWinY32x192_ref_mid[192][32];
    static ap_uint<10> refWinU16x96_ref_mid[96][16];
    static ap_uint<10> refWinV16x96_ref_mid[96][16];
    static ap_uint<10> refWinY0_ref_mid[192][64];
    static ap_uint<10> refWinY1_ref_mid[192][64];
    static ap_uint<10> refWinY2_ref_mid[192][64];
    static ap_uint<10> refWinY3_ref_mid[192][64];
    static ap_uint<10> refWinU0_ref_mid[96][32];
    static ap_uint<10> refWinU1_ref_mid[96][32];
    static ap_uint<10> refWinU2_ref_mid[96][32];
    static ap_uint<10> refWinU3_ref_mid[96][32];
    static ap_uint<10> refWinV0_ref_mid[96][32];
    static ap_uint<10> refWinV1_ref_mid[96][32];
    static ap_uint<10> refWinV2_ref_mid[96][32];
    static ap_uint<10> refWinV3_ref_mid[96][32];

#pragma HLS RESOURCE variable=refWinY0_ref_mid core=RAM_S2P_BRAM
#pragma HLS RESOURCE variable=refWinY1_ref_mid core=RAM_S2P_BRAM
#pragma HLS RESOURCE variable=refWinY2_ref_mid core=RAM_S2P_BRAM
#pragma HLS RESOURCE variable=refWinY3_ref_mid core=RAM_S2P_BRAM
#pragma HLS RESOURCE variable=refWinY32x192_ref_mid core=RAM_S2P_BRAM

#pragma HLS RESOURCE variable=refWinU0_ref_mid core=RAM_S2P_BRAM
#pragma HLS RESOURCE variable=refWinU1_ref_mid core=RAM_S2P_BRAM
#pragma HLS RESOURCE variable=refWinU2_ref_mid core=RAM_S2P_BRAM
#pragma HLS RESOURCE variable=refWinU3_ref_mid core=RAM_S2P_BRAM
#pragma HLS RESOURCE variable=refWinU16x96_ref_mid core=RAM_S2P_BRAM

#pragma HLS RESOURCE variable=refWinV0_ref_mid core=RAM_S2P_BRAM
#pragma HLS RESOURCE variable=refWinV1_ref_mid core=RAM_S2P_BRAM
#pragma HLS RESOURCE variable=refWinV2_ref_mid core=RAM_S2P_BRAM
#pragma HLS RESOURCE variable=refWinV3_ref_mid core=RAM_S2P_BRAM
#pragma HLS RESOURCE variable=refWinV16x96_ref_mid core=RAM_S2P_BRAM

#pragma HLS array_reshape variable=refWinY0_ref_mid block factor=32 dim=2
#pragma HLS array_reshape variable=refWinY1_ref_mid block factor=32 dim=2
#pragma HLS array_reshape variable=refWinY2_ref_mid block factor=32 dim=2
#pragma HLS array_reshape variable=refWinY3_ref_mid block factor=32 dim=2
#pragma HLS array_reshape variable=refWinY32x192_ref_mid block factor=32 dim=2

#pragma HLS array_reshape variable=refWinU0_ref_mid block factor=16 dim=2
#pragma HLS array_reshape variable=refWinU1_ref_mid block factor=16 dim=2
#pragma HLS array_reshape variable=refWinU2_ref_mid block factor=16 dim=2
#pragma HLS array_reshape variable=refWinU3_ref_mid block factor=16 dim=2
#pragma HLS array_reshape variable=refWinU16x96_ref_mid block factor=16 dim=2

#pragma HLS array_reshape variable=refWinV0_ref_mid block factor=16 dim=2
#pragma HLS array_reshape variable=refWinV1_ref_mid block factor=16 dim=2
#pragma HLS array_reshape variable=refWinV2_ref_mid block factor=16 dim=2
#pragma HLS array_reshape variable=refWinV3_ref_mid block factor=16 dim=2
#pragma HLS array_reshape variable=refWinV16x96_ref_mid block factor=16 dim=2

// #pragma HLS array_partition variable=refWinY0_ref_mid complete dim=2
// #pragma HLS array_partition variable=refWinY1_ref_mid complete dim=2
// #pragma HLS array_partition variable=refWinY2_ref_mid complete dim=2
// #pragma HLS array_partition variable=refWinY3_ref_mid complete dim=2
// #pragma HLS array_partition variable=refWinY32x192_ref_mid complete dim=2

// #pragma HLS array_partition variable=refWinU0_ref_mid complete dim=2
// #pragma HLS array_partition variable=refWinU1_ref_mid complete dim=2
// #pragma HLS array_partition variable=refWinU2_ref_mid complete dim=2
// #pragma HLS array_partition variable=refWinU3_ref_mid complete dim=2
// #pragma HLS array_partition variable=refWinU16x96_ref_mid complete dim=2

// #pragma HLS array_partition variable=refWinV0_ref_mid complete dim=2
// #pragma HLS array_partition variable=refWinV1_ref_mid complete dim=2
// #pragma HLS array_partition variable=refWinV2_ref_mid complete dim=2
// #pragma HLS array_partition variable=refWinV3_ref_mid complete dim=2
// #pragma HLS array_partition variable=refWinV16x96_ref_mid complete dim=2

#ifndef __SYNTHESIS__
    static ap_uint<10> refWinY32x192_B_ref_mid[192][32];
    static ap_uint<10> refWinU16x96_B_ref_mid[96][16];
    static ap_uint<10> refWinV16x96_B_ref_mid[96][16];
    static ap_uint<10> refWinY0_B_ref_mid[192][64];
    static ap_uint<10> refWinY1_B_ref_mid[192][64];
    static ap_uint<10> refWinY2_B_ref_mid[192][64];
    static ap_uint<10> refWinY3_B_ref_mid[192][64];
    static ap_uint<10> refWinU0_B_ref_mid[96][32];
    static ap_uint<10> refWinU1_B_ref_mid[96][32];
    static ap_uint<10> refWinU2_B_ref_mid[96][32];
    static ap_uint<10> refWinU3_B_ref_mid[96][32];
    static ap_uint<10> refWinV0_B_ref_mid[96][32];
    static ap_uint<10> refWinV1_B_ref_mid[96][32];
    static ap_uint<10> refWinV2_B_ref_mid[96][32];
    static ap_uint<10> refWinV3_B_ref_mid[96][32];
#endif

    if(0==b_use_flag)
    {
        if(0==pix_y && 0==pix_x)
        {
            for (int j = 0; j < 192; j++)
            {
    #pragma HLS pipeline
                for (int i = 0; i < 32; i++)
                {
    #pragma HLS unroll
                    /********update luma*****************************************/
                    refWinY1_ref_mid[j][i] = refWinY1_ref_temp[j][i];
                    refWinY2_ref_mid[j][i] = refWinY2_ref_temp[j][i];
                    refWinY3_ref_mid[j][i] = refWinY3_ref_temp[j][i];

                    refWinY1_ref_mid[j][i+32] = refWinY1_ref_temp[j][i+32];
                    refWinY2_ref_mid[j][i+32] = refWinY2_ref_temp[j][i+32];
                    refWinY3_ref_mid[j][i+32] = refWinY3_ref_temp[j][i+32];
                }
            }

            for (int j = 0; j < 96; j++)
            {   
    #pragma HLS pipeline
                for (int i = 0; i < 16; i++)
                {
    #pragma HLS unroll
                    /********update chroma*****************************************/
                    refWinU1_ref_mid[j][i] = refWinU1_ref_temp[j][i];
                    refWinU2_ref_mid[j][i] = refWinU2_ref_temp[j][i];
                    refWinU3_ref_mid[j][i] = refWinU3_ref_temp[j][i];

                    refWinU1_ref_mid[j][i+16] = refWinU1_ref_temp[j][i+16];
                    refWinU2_ref_mid[j][i+16] = refWinU2_ref_temp[j][i+16];
                    refWinU3_ref_mid[j][i+16] = refWinU3_ref_temp[j][i+16];

                    refWinV1_ref_mid[j][i] = refWinV1_ref_temp[j][i];
                    refWinV2_ref_mid[j][i] = refWinV2_ref_temp[j][i];
                    refWinV3_ref_mid[j][i] = refWinV3_ref_temp[j][i];

                    refWinV1_ref_mid[j][i+16] = refWinV1_ref_temp[j][i+16];
                    refWinV2_ref_mid[j][i+16] = refWinV2_ref_temp[j][i+16];
                    refWinV3_ref_mid[j][i+16] = refWinV3_ref_temp[j][i+16];
                }
            }
        }
        else
        {

            for (int j = 0; j < 192; j++)
            {
    #pragma HLS pipeline
                for (int i = 0; i < 32; i++)
                {
    #pragma HLS unroll
                    /********update luma*****************************************/
                    refWinY3_ref_mid[j][i] = refWinY3_ref_temp[j][i];

                    refWinY3_ref_mid[j][i+32] = refWinY3_ref_temp[j][i+32];
                }
            }

            for (int j = 0; j < 96; j++)
            {   
    #pragma HLS pipeline
                for (int i = 0; i < 16; i++)
                {
    #pragma HLS unroll
                    /********update chroma*****************************************/
                    refWinU3_ref_mid[j][i] = refWinU3_ref_temp[j][i];
                    refWinV3_ref_mid[j][i] = refWinV3_ref_temp[j][i];

                    refWinU3_ref_mid[j][i+16] = refWinU3_ref_temp[j][i+16];
                    refWinV3_ref_mid[j][i+16] = refWinV3_ref_temp[j][i+16];
                }
            }          
        }
    }
#ifndef __SYNTHESIS__
    else
    {
        if(0==pix_y && 0==pix_x)
        {
            for (int j = 0; j < 192; j++)
            {
    #pragma HLS pipeline
                for (int i = 0; i < 64; i++)
                {
    #pragma HLS unroll
                    /********update luma*****************************************/
                    refWinY1_B_ref_mid[j][i] = refWinY1_ref_temp[j][i];
                    refWinY2_B_ref_mid[j][i] = refWinY2_ref_temp[j][i];
                    refWinY3_B_ref_mid[j][i] = refWinY3_ref_temp[j][i];
                }
            }

            for (int j = 0; j < 96; j++)
            {   
    #pragma HLS pipeline
                for (int i = 0; i < 32; i++)
                {
    #pragma HLS unroll
                    /********update chroma*****************************************/
                    refWinU1_B_ref_mid[j][i] = refWinU1_ref_temp[j][i];
                    refWinU2_B_ref_mid[j][i] = refWinU2_ref_temp[j][i];
                    refWinU3_B_ref_mid[j][i] = refWinU3_ref_temp[j][i];

                    refWinV1_B_ref_mid[j][i] = refWinV1_ref_temp[j][i];
                    refWinV2_B_ref_mid[j][i] = refWinV2_ref_temp[j][i];
                    refWinV3_B_ref_mid[j][i] = refWinV3_ref_temp[j][i];
                }
            }
        }
        else
        {

            for (int j = 0; j < 192; j++)
            {
    #pragma HLS pipeline
                for (int i = 0; i < 64; i++)
                {
    #pragma HLS unroll
                    /********update luma*****************************************/
                    refWinY3_B_ref_mid[j][i] = refWinY3_ref_temp[j][i];
                }
            }

            for (int j = 0; j < 96; j++)
            {   
    #pragma HLS pipeline
                for (int i = 0; i < 32; i++)
                {
    #pragma HLS unroll
                    /********update chroma*****************************************/
                    refWinU3_B_ref_mid[j][i] = refWinU3_ref_temp[j][i];
                    refWinV3_B_ref_mid[j][i] = refWinV3_ref_temp[j][i];
                }
            }          
        }
    }
#endif
//////////////////////////////////////////////////////////////////////////////

    int ref1_valid = 0;

    if (code_stru != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) && ctx_rpm_num_refp_0 > 1)
    {
        ref1_valid = 1;
    }
    else if (code_stru == RA && fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B)
    {
        ref1_valid = 1;
    }

#if DEBUG_CHECK 
    int ref_start_x;
    int ref_start_y;
    int i;
    int j;
    ref_start_x = 0;
    ref_start_y = 0;

    int pad_luma = (fetch_ctx_io->ctx_pinter_refp_pic_stride_luma - fetch_ctx_io->ctx_pinter_refp_pic_width_luma) / 2;
    int pad_chroma = (fetch_ctx_io->ctx_pinter_refp_pic_stride_chroma - fetch_ctx_io->ctx_pinter_refp_pic_width_chroma) / 2;

    int x_offset = pad_luma - REFP_SURD_OFF_W;
    int y_offset = pad_luma - REFP_SURD_OFF_H;

    if (x_offset % 16 != 0) {
        printf("Error: unsupported x_offset detected.\n");
    }

    //Y
    int offset_Y = fetch_ctx_io->ctx_info_pic_width + ((1 << 7) + 16) * 2;

    uint128_t d_ap[32];
    pel linebuf[256];
    U16 offset_Y16 = ((offset_Y + 15) >> 4) << 1;
    U32 idx_y = offset_Y16 * (pix_y + y_offset) + (pix_x >> 6) * 8 + (x_offset >> 3);
    U32 idx_y1 = idx_y + 24;

/*********************************add pragma***************************************/
#pragma HLS RESOURCE variable=refWinY0_test2 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=refWinY1_test2 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=refWinY2_test2 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=refWinY3_test2 core=RAM_1P_BRAM

#pragma HLS RESOURCE variable=refWinU0_test2 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=refWinU1_test2 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=refWinU2_test2 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=refWinU3_test2 core=RAM_1P_BRAM

#pragma HLS RESOURCE variable=refWinV0_test2 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=refWinV1_test2 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=refWinV2_test2 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=refWinV3_test2 core=RAM_1P_BRAM

#pragma HLS array_partition variable=refWinY0_test2 complete dim=2
#pragma HLS array_partition variable=refWinY1_test2 complete dim=2
#pragma HLS array_partition variable=refWinY2_test2 complete dim=2
#pragma HLS array_partition variable=refWinY3_test2 complete dim=2

#pragma HLS array_partition variable=refWinU0_test2 complete dim=2
#pragma HLS array_partition variable=refWinU1_test2 complete dim=2
#pragma HLS array_partition variable=refWinU2_test2 complete dim=2
#pragma HLS array_partition variable=refWinU3_test2 complete dim=2

#pragma HLS array_partition variable=refWinV0_test2 complete dim=2
#pragma HLS array_partition variable=refWinV1_test2 complete dim=2
#pragma HLS array_partition variable=refWinV2_test2 complete dim=2
#pragma HLS array_partition variable=refWinV3_test2 complete dim=2

////////////////////chroma////////////////////////////////////////////
    int c_srw;
    int c_srh;
    c_srw = (REFP_SURD_OFF_W) / 2;
    c_srh = (REFP_SURD_OFF_H) / 2;
    int offset_UV = (fetch_ctx_io->ctx_info_pic_width >> 1) + (((1 << 7) + 16) >> 1) * 2;

    U16 offset_UV16 = ((offset_UV +15) >> 4) << 1;
    U32 idx_u = offset_UV16 * ((pix_y>>1) + (y_offset>>1)) + (pix_x >> 6) * 4 + (x_offset>>4);
    U32 idx_v = idx_u;
    U32 idx_u1 = idx_u + 12;
    U32 idx_v1 = idx_v + 12;

#pragma HLS array_partition variable=linebuf complete dim=0
#pragma HLS array_partition variable=d_ap complete dim=0
    if (b_use_flag)
    {
        if ((256 > fetch_ctx_io->ctx_info_pic_width || pix_x == 0))
        {
#ifndef __ZP_CYCLE_OPT_IGNORE_INIT_
#define __ZP_CYCLE_OPT_IGNORE_INIT_
#endif
#ifdef __ZP_CYCLE_OPT_IGNORE_INIT_
            int tt;
            int fake_one = (fetch_ctx_io->ctx_info_pic_width != 0);
        L_ZP0: for (tt = 0; tt < fake_one; tt++) {
#pragma HLS loop_tripcount max=0
#endif // __ZP_CYCLE_OPT_IGNORE_INIT_
        loopa:
            for (j = 0; j < 192; j++) {
                for (i = 0; i < 32; i++) {
#pragma HLS pipeline
                    d_ap[i] = RefbufY1[idx_y + i];
                }
                idx_y += offset_Y16;

                for (i = 0; i < 32; i++) {
#pragma HLS unroll
                    for (int k = 0; k < 8; k++) {
#pragma HLS unroll
                        linebuf[i * 8 + k] = d_ap[i]((k + 1) * 16 - 1, k * 16);
                    }
                }
                for (i = 0; i < 64; i++) {
#pragma HLS unroll
                    refWinY0_B_test2[j][i] = linebuf[i];
                    refWinY1_B_test2[j][i] = linebuf[i + 64];
                    refWinY2_B_test2[j][i] = linebuf[i + 128];
                    refWinY3_B_test2[j][i] = linebuf[i + 192];
                }
            }
        loopb:
            for (j = 0; j < 96; j++) {
                for (i = 0; i < 16; i++) {
#pragma HLS pipeline
                    d_ap[i] = RefbufU1[idx_u + i];
                }
                idx_u += offset_UV16;

                for (i = 0; i < 16; i++) {
#pragma HLS unroll
                    for (int k = 0; k < 8; k++) {
#pragma HLS unroll
                        linebuf[i * 8 + k] = d_ap[i]((k + 1) * 16 - 1, k * 16);
                    }
                }
                for (i = 0; i < 32; i++) {
#pragma HLS unroll
                    refWinU0_B_test2[j][i] = linebuf[i];
                    refWinU1_B_test2[j][i] = linebuf[i + 32];
                    refWinU2_B_test2[j][i] = linebuf[i + 64];
                    refWinU3_B_test2[j][i] = linebuf[i + 96];
                }

                for (i = 0; i < 16; i++) {
#pragma HLS pipeline
                    d_ap[i] = RefbufV1[idx_v + i];
                }
                idx_v += offset_UV16;

                for (i = 0; i < 16; i++) {
#pragma HLS unroll
                    for (int k = 0; k < 8; k++) {
#pragma HLS unroll
                        linebuf[i * 8 + k] = d_ap[i]((k + 1) * 16 - 1, k * 16);
                    }
                }
                for (i = 0; i < 32; i++) {
#pragma HLS unroll
                    refWinV0_B_test2[j][i] = linebuf[i];
                    refWinV1_B_test2[j][i] = linebuf[i + 32];
                    refWinV2_B_test2[j][i] = linebuf[i + 64];
                    refWinV3_B_test2[j][i] = linebuf[i + 96];
                }
            }
#ifdef __ZP_CYCLE_OPT_IGNORE_INIT_
        }
#endif  //__ZP_CYCLE_OPT_IGNORE_INIT_
        }
        else if (ref1_valid)
        {

            pel RefLeftY1[192][64];
            pel RefLeftU1[96][32];
            pel RefLeftV1[96][32];
#pragma HLS array_reshape variable=RefLeftY1 cyclic factor=8 dim=2
#pragma HLS array_reshape variable=RefLeftU1 cyclic factor=8 dim=2
#pragma HLS array_reshape variable=RefLeftV1 cyclic factor=8 dim=2
            //fetch new Y
            loopc:
            for (j = 0; j < 192; j++) {
                for (i = 0; i < 8; i++) {
#pragma HLS pipeline
                    uint128_t tmp = RefbufY1[idx_y1 + i];
                    for (int k = 0; k < 8; k++) {
#pragma HLS unroll
                        RefLeftY1[j][i * 8 + k] = tmp((k + 1) * 16 - 1, k * 16);
                        refWinY0_B_test2[j][i * 8 + k] = refWinY1_B_test2[j][i * 8 + k];
                        refWinY1_B_test2[j][i * 8 + k] = refWinY2_B_test2[j][i * 8 + k];
                        refWinY2_B_test2[j][i * 8 + k] = refWinY3_B_test2[j][i * 8 + k];
                        refWinY3_B_test2[j][i * 8 + k] = RefLeftY1[j][i * 8 + k];

                    }
                }
                idx_y1 += offset_Y16;
            }
            //fetch new U
        loopd:
            for (j = 0; j < 96; j++) {
                for (i = 0; i < 4; i++) {
#pragma HLS pipeline
                    uint128_t tmp = RefbufU1[idx_u1 + i];
                    for (int k = 0; k < 8; k++) {
#pragma HLS unroll
                        RefLeftU1[j][i * 8 + k] = tmp((k + 1) * 16 - 1, k * 16);
                        refWinU0_B_test2[j][i * 8 + k] = refWinU1_B_test2[j][i * 8 + k];
                        refWinU1_B_test2[j][i * 8 + k] = refWinU2_B_test2[j][i * 8 + k];
                        refWinU2_B_test2[j][i * 8 + k] = refWinU3_B_test2[j][i * 8 + k];
                        refWinU3_B_test2[j][i * 8 + k] = RefLeftU1[j][i * 8 + k];
                    }
                }
                idx_u1 += offset_UV16;
            }

            //fetch new V
        loope:
            for (j = 0; j < 96; j++) {
                for (i = 0; i < 4; i++) {
#pragma HLS pipeline
                    //d_ap[i] = imgV_ref_ap[idx_v1 + i];
                    uint128_t tmp = RefbufV1[idx_v1 + i];
                    for (int k = 0; k < 8; k++) {
#pragma HLS unroll
                        RefLeftV1[j][i * 8 + k] = tmp((k + 1) * 16 - 1, k * 16);
                        refWinV0_B_test2[j][i * 8 + k] = refWinV1_B_test2[j][i * 8 + k];
                        refWinV1_B_test2[j][i * 8 + k] = refWinV2_B_test2[j][i * 8 + k];
                        refWinV2_B_test2[j][i * 8 + k] = refWinV3_B_test2[j][i * 8 + k];
                        refWinV3_B_test2[j][i * 8 + k] = RefLeftV1[j][i * 8 + k];
                    }
                }
                idx_v1 += offset_UV16;
            }
        }
    }
    else
    {
        /*************************fetch luma data from RFC:DCP buffer*****************************************/
        if (256 > fetch_ctx_io->ctx_info_pic_width || pix_x == 0)
        {
#ifndef __ZP_CYCLE_OPT_IGNORE_INIT_
#define __ZP_CYCLE_OPT_IGNORE_INIT_
#endif
#ifdef __ZP_CYCLE_OPT_IGNORE_INIT_	
            int tt;
            int fake_one = (fetch_ctx_io->ctx_info_pic_width != 0);
        L_ZP1: for (tt = 0; tt < fake_one; tt++) {
#pragma HLS loop_tripcount max=0
#endif // __ZP_CYCLE_OPT_IGNORE_INIT_	

            for (j = 0; j < 192; j++) {
                for (i = 0; i < 32; i++) {
#pragma HLS pipeline
                    d_ap[i] = RefbufY1[idx_y + i];
                }
                idx_y += offset_Y16;

                for (i = 0; i < 32; i++) {
#pragma HLS unroll
                    for (int k = 0; k < 8; k++) {
#pragma HLS unroll
                        linebuf[i * 8 + k] = d_ap[i]((k + 1) * 16 - 1, k * 16);
                    }
                }

                for (i = 0; i < 64; i++) {
#pragma HLS unroll
                    refWinY0_test2[j][i] = linebuf[i];
                    refWinY1_test2[j][i] = linebuf[i + 64];
                    refWinY2_test2[j][i] = linebuf[i + 128];
                    refWinY3_test2[j][i] = linebuf[i + 192];
                }
            }
            /*************************fetch chroma data from RFC:DCP buffer*****************************************/
            for (j = 0; j < 96; j++) {
                for (i = 0; i < 16; i++) {
#pragma HLS pipeline
                    d_ap[i] = RefbufU1[idx_u + i];
                }
                idx_u += offset_UV16;

                for (i = 0; i < 16; i++) {
#pragma HLS unroll
                    for (int k = 0; k < 8; k++) {
#pragma HLS unroll
                        linebuf[i * 8 + k] = d_ap[i]((k + 1) * 16 - 1, k * 16);
                    }
                }

                for (i = 0; i < 32; i++) {
#pragma HLS unroll
                    refWinU0_test2[j][i] = linebuf[i];
                    refWinU1_test2[j][i] = linebuf[i + 32];
                    refWinU2_test2[j][i] = linebuf[i + 64];
                    refWinU3_test2[j][i] = linebuf[i + 96];
                }

                for (i = 0; i < 16; i++) {
#pragma HLS pipeline
                    d_ap[i] = RefbufV1[idx_v + i];
                }
                idx_v += offset_UV16;

                for (i = 0; i < 16; i++) {
#pragma HLS unroll
                    for (int k = 0; k < 8; k++) {
#pragma HLS unroll
                        linebuf[i * 8 + k] = d_ap[i]((k + 1) * 16 - 1, k * 16);
                    }
                }

                for (i = 0; i < 32; i++) {
#pragma HLS unroll
                    refWinV0_test2[j][i] = linebuf[i];
                    refWinV1_test2[j][i] = linebuf[i + 32];
                    refWinV2_test2[j][i] = linebuf[i + 64];
                    refWinV3_test2[j][i] = linebuf[i + 96];
                }

            }
#ifdef __ZP_CYCLE_OPT_IGNORE_INIT_	
        }
#endif  //__ZP_CYCLE_OPT_IGNORE_INIT_	

        }
        else
        {
            pel RefLeftY[192][64];
            pel RefLeftU[96][32];
            pel RefLeftV[96][32];
            //#pragma HLS array_reshape variable=RefLeftY complete dim=2
#pragma HLS array_reshape variable=RefLeftY cyclic factor=8 dim=2
//#pragma HLS array_reshape variable=RefLeftU complete dim=2
#pragma HLS array_reshape variable=RefLeftU cyclic factor=8 dim=2
//#pragma HLS array_reshape variable=RefLeftV complete dim=2
#pragma HLS array_reshape variable=RefLeftV cyclic factor=8 dim=2

        //fetch new Y
            for (j = 0; j < 192; j++) {
                for (i = 0; i < 8; i++) {
#pragma HLS pipeline
                    uint128_t tmp = RefbufY1[idx_y1 + i];
                    for (int k = 0; k < 8; k++) {
#pragma HLS unroll
                        RefLeftY[j][i * 8 + k] = tmp((k + 1) * 16 - 1, k * 16);

                        refWinY0_test2[j][i * 8 + k] = refWinY1_test2[j][i * 8 + k];
                        refWinY1_test2[j][i * 8 + k] = refWinY2_test2[j][i * 8 + k];
                        refWinY2_test2[j][i * 8 + k] = refWinY3_test2[j][i * 8 + k];
                        refWinY3_test2[j][i * 8 + k] = RefLeftY[j][i * 8 + k];

                    }
                }
                idx_y1 += offset_Y16;
            }

            //fetch new U
            for (j = 0; j < 96; j++) {
                for (i = 0; i < 4; i++) {
#pragma HLS pipeline
                    uint128_t tmp = RefbufU1[idx_u1 + i];
                    for (int k = 0; k < 8; k++) {
#pragma HLS unroll
                        RefLeftU[j][i * 8 + k] = tmp((k + 1) * 16 - 1, k * 16);

                        refWinU0_test2[j][i * 8 + k] = refWinU1_test2[j][i * 8 + k];
                        refWinU1_test2[j][i * 8 + k] = refWinU2_test2[j][i * 8 + k];
                        refWinU2_test2[j][i * 8 + k] = refWinU3_test2[j][i * 8 + k];
                        refWinU3_test2[j][i * 8 + k] = RefLeftU[j][i * 8 + k];
                    }
                }
                idx_u1 += offset_UV16;
            }

            //fetch new V
            for (j = 0; j < 96; j++) {
                for (i = 0; i < 4; i++) {
#pragma HLS pipeline
                    //d_ap[i] = imgV_ref_ap[idx_v1 + i];
                    uint128_t tmp = RefbufV1[idx_v1 + i];
                    for (int k = 0; k < 8; k++) {
#pragma HLS unroll
                        RefLeftV[j][i * 8 + k] = tmp((k + 1) * 16 - 1, k * 16);

                        refWinV0_test2[j][i * 8 + k] = refWinV1_test2[j][i * 8 + k];
                        refWinV1_test2[j][i * 8 + k] = refWinV2_test2[j][i * 8 + k];
                        refWinV2_test2[j][i * 8 + k] = refWinV3_test2[j][i * 8 + k];
                        refWinV3_test2[j][i * 8 + k] = RefLeftV[j][i * 8 + k];
                    }

                }
                idx_v1 += offset_UV16;
            }
        }
    }
#endif//debug_check
#if 0
//verify origin padding
    for (int j = 0; j < 192; j++)
    {
        for (int i = 0; i < 64; i++)
        {
            refWinY0_temp[j][i] = (U10)refWinY0_test2[j][i];
            refWinY1_temp[j][i] = (U10)refWinY1_test2[j][i];
            refWinY2_temp[j][i] = (U10)refWinY2_test2[j][i];
            refWinY3_temp[j][i] = (U10)refWinY3_test2[j][i];
        }
    }

    for (int j = 0; j < 96; j++)
    {
        for (int i = 0; i < 32; i++)
        {
            refWinU0_temp[j][i] = (U10)refWinU0_test2[j][i];
            refWinU1_temp[j][i] = (U10)refWinU1_test2[j][i];
            refWinU2_temp[j][i] = (U10)refWinU2_test2[j][i];
            refWinU3_temp[j][i] = (U10)refWinU3_test2[j][i];

            refWinV0_temp[j][i] = (U10)refWinV0_test2[j][i];
            refWinV1_temp[j][i] = (U10)refWinV1_test2[j][i];
            refWinV2_temp[j][i] = (U10)refWinV2_test2[j][i];
            refWinV3_temp[j][i] = (U10)refWinV3_test2[j][i];

        }
    }
    if ((code_stru != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) &&
         ctx_rpm_num_refp_0 > 1) || (code_stru == RA &&
            fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B))
    {
        for (int j = 0; j < 192; j++)
        {
            for (int i = 0; i < 64; i++)
            {
                refWinY0_temp[j][i] = (U10)refWinY0_B_test2[j][i];
                refWinY1_temp[j][i] = (U10)refWinY1_B_test2[j][i];
                refWinY2_temp[j][i] = (U10)refWinY2_B_test2[j][i];
                refWinY3_temp[j][i] = (U10)refWinY3_B_test2[j][i];
            }
        }

        for (int j = 0; j < 96; j++)
        {
            for (int i = 0; i < 32; i++)
            {
                refWinU0_temp[j][i] = (U10)refWinU0_B_test2[j][i];
                refWinU1_temp[j][i] = (U10)refWinU1_B_test2[j][i];
                refWinU2_temp[j][i] = (U10)refWinU2_B_test2[j][i];
                refWinU3_temp[j][i] = (U10)refWinU3_B_test2[j][i];

                refWinV0_temp[j][i] = (U10)refWinV0_B_test2[j][i];
                refWinV1_temp[j][i] = (U10)refWinV1_B_test2[j][i];
                refWinV2_temp[j][i] = (U10)refWinV2_B_test2[j][i];
                refWinV3_temp[j][i] = (U10)refWinV3_B_test2[j][i];

            }
        }
    }
#endif
#if ADD_by_YHY
#if 1
    //--config ../../cfg/encode_IPPP.cfg -i D:/yanghy_file/test_data/BasketballPass_416x240_50.yuv -w 416 -h 240 -q 36 -z 50 -p 15 -f 5 -d 8
    //--config ../../cfg/encode_IPPP.cfg -i D:/yanghy_file/test_data/BQSquare_320x320_60.yuv -w 320 -h 320 -q 32 -z 60 -p 15 -f 5 -d 8
    //--config ../../cfg/encode_IPPP.cfg -i D:/yanghy_file/test_data/BasketballDrill_832x480_50.yuv -w 832 -h 480 -q 32 -z 50 -p 15 -f 5 -d 8

    int luma_width = fetch_ctx_io->ctx_pinter_refp_pic_width_luma;
    int luma_heigh = fetch_ctx_io->ctx_pinter_refp_pic_height_luma;
    int luma_mod_num_col = luma_width % 64;
    int luma_mod_num_row = luma_heigh % 64;
    int offset_luma_x = luma_mod_num_col ? 128 : 192;
    int offset_luma_y = luma_mod_num_row ? 64 : 128;

    int chroma_width = fetch_ctx_io->ctx_pinter_refp_pic_width_chroma;
    int chroma_heigh = fetch_ctx_io->ctx_pinter_refp_pic_height_chroma;
    int chroma_mod_num_col = chroma_width % 32;
    int chroma_mod_num_row = chroma_heigh % 32;

    int luma_width_padded = (fetch_ctx_io->ctx_pinter_refp_pic_width_luma + 63) / 64 * 64;
    int luma_height_padded = (fetch_ctx_io->ctx_pinter_refp_pic_height_luma + 63) / 64 * 64;

    if(0==b_use_flag)
    {
        //firsr row
        if (0 == pix_y)
        {
            if (0==pix_x)
            {
                if ((pix_x + 128) <= (luma_width_padded - 64))
                
                {
                    for (int j = 0; j < 64; j++)
                    {
        #pragma HLS pipeline 
                        for (int i = 0; i < 32; i++)
                        {
        #pragma HLS unroll
                            /*************************padding luma***********************************/
                            // swOutLv0_temp[j][i] = refWinY1_ref_mid[64][0];
                            // swOutLv0_temp[j][i+32] = refWinY1_ref_mid[64][0];
                            // swOutLv0_temp[j][i+64] = refWinY1_ref_mid[64][0];
                            // swOutLv0_temp[j][i+96] = refWinY1_ref_mid[64][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_ref_mid[64][i + 32];
                            // swOutLv0_temp[j][i+160] = refWinY2_ref_mid[64][i];
                            // swOutLv0_temp[j][i+192] = refWinY2_ref_mid[64][i + 32];
                            // swOutLv0_temp[j][i+224] = refWinY3_ref_mid[64][i];

                            swOutLv0_ime_temp[j][i] = refWinY1_ref_mid[64][0];
                            swOutLv0_ime_temp[j][i+32] = refWinY1_ref_mid[64][0];
                            swOutLv0_ime_temp[j][i+64] = refWinY1_ref_mid[64][0];
                            swOutLv0_ime_temp[j][i+96] = refWinY1_ref_mid[64][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_ref_mid[64][i + 32];
                            swOutLv0_ime_temp[j][i+160] = refWinY2_ref_mid[64][i];
                            swOutLv0_ime_temp[j][i+192] = refWinY2_ref_mid[64][i + 32];
                            swOutLv0_ime_temp[j][i+224] = refWinY3_ref_mid[64][i];
                        }
                    }
                    for (int j = 0; j < 32; j++)
                    {
            #pragma HLS pipeline 
                        for (int i = 0; i < 16; i++)
                        {
            #pragma HLS unroll
                            /*************************padding chroma***********************************/
                            refWinU_temp[j*128+i] = refWinU1_ref_mid[32][0];
                            refWinU_temp[j*128+i + 16] = refWinU1_ref_mid[32][0];
                            refWinU_temp[j*128+i+32] = refWinU1_ref_mid[32][0];
                            refWinU_temp[j*128+i + 48] = refWinU1_ref_mid[32][i];
                            refWinU_temp[j*128+i+64] = refWinU1_ref_mid[32][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU2_ref_mid[32][i];
                            refWinU_temp[j*128+i+96] = refWinU2_ref_mid[32][i + 16];
                            refWinU_temp[j*128+i + 112] = refWinU3_ref_mid[32][i];

                            refWinV_temp[j*128+i] = refWinV1_ref_mid[32][0];
                            refWinV_temp[j*128+i + 16] = refWinV1_ref_mid[32][0];
                            refWinV_temp[j*128+i+32] = refWinV1_ref_mid[32][0];
                            refWinV_temp[j*128+i + 48] = refWinV1_ref_mid[32][i];
                            refWinV_temp[j*128+i+64] = refWinV1_ref_mid[32][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV2_ref_mid[32][i];
                            refWinV_temp[j*128+i+96] = refWinV2_ref_mid[32][i + 16];
                            refWinV_temp[j*128+i + 112] = refWinV3_ref_mid[32][i];
                    
                        }
                    }

                    for (int j = 64; j < 192; j++)
                    {
        #pragma HLS pipeline 
                        for (int i = 0; i < 32; i++)
                        {
        #pragma HLS unroll
                            /*************************padding luma***********************************/
                            // swOutLv0_temp[j][i] = refWinY1_ref_mid[j][0];
                            // swOutLv0_temp[j][i+32] = refWinY1_ref_mid[j][0];
                            // swOutLv0_temp[j][i+64] = refWinY1_ref_mid[j][0];
                            // swOutLv0_temp[j][i+96] = refWinY1_ref_mid[j][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i+160] = refWinY2_ref_mid[j][i];
                            // swOutLv0_temp[j][i+192] = refWinY2_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i+224] = refWinY3_ref_mid[j][i];

                            swOutLv0_ime_temp[j][i] = refWinY1_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i+32] = refWinY1_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i+64] = refWinY1_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i+96] = refWinY1_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i+160] = refWinY2_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+192] = refWinY2_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i+224] = refWinY3_ref_mid[j][i];
                        }
                    }

                    for (int j = 32; j < 96; j++)
                    {
            #pragma HLS pipeline 
                        for (int i = 0; i < 16; i++)
                        {
            #pragma HLS unroll
                        /*************************padding chroma***********************************/
                            refWinU_temp[j*128+i] = refWinU1_ref_mid[j][0];
                            refWinU_temp[j*128+i + 16] = refWinU1_ref_mid[j][0];
                            refWinU_temp[j*128+i+32] = refWinU1_ref_mid[j][0];
                            refWinU_temp[j*128+i + 48] = refWinU1_ref_mid[j][i];
                            refWinU_temp[j*128+i+64] = refWinU1_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU2_ref_mid[j][i];
                            refWinU_temp[j*128+i+96] = refWinU2_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 112] = refWinU3_ref_mid[j][i];
                                    
                            refWinV_temp[j*128+i] = refWinV1_ref_mid[j][0];
                            refWinV_temp[j*128+i + 16] = refWinV1_ref_mid[j][0];
                            refWinV_temp[j*128+i+32] = refWinV1_ref_mid[j][0];
                            refWinV_temp[j*128+i + 48] = refWinV1_ref_mid[j][i];
                            refWinV_temp[j*128+i+64] = refWinV1_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV2_ref_mid[j][i];
                            refWinV_temp[j*128+i+96] = refWinV2_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 112] = refWinV3_ref_mid[j][i];
                        }
                    }
                }
                else if ((pix_x + 128) == luma_width_padded)
                {
                    for (int j = 0; j < 64; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 32; i++)
                        {
    #pragma HLS unroll
                            /*************************padding luma***********************************/
                            // swOutLv0_temp[j][i] = refWinY1_ref_mid[64][0];
                            // swOutLv0_temp[j][i+32] = refWinY1_ref_mid[64][0];
                            // swOutLv0_temp[j][i+64] = refWinY1_ref_mid[64][0];
                            // swOutLv0_temp[j][i+96] = refWinY1_ref_mid[64][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_ref_mid[64][i + 32];
                            // swOutLv0_temp[j][i+160] = refWinY2_ref_mid[64][i];
                            // swOutLv0_temp[j][i+192] = refWinY2_ref_mid[64][i + 32];
                            // swOutLv0_temp[j][i+224] = refWinY2_ref_mid[64][63];

                            swOutLv0_ime_temp[j][i] = refWinY1_ref_mid[64][0];
                            swOutLv0_ime_temp[j][i+32] = refWinY1_ref_mid[64][0];
                            swOutLv0_ime_temp[j][i+64] = refWinY1_ref_mid[64][0];
                            swOutLv0_ime_temp[j][i+96] = refWinY1_ref_mid[64][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_ref_mid[64][i + 32];
                            swOutLv0_ime_temp[j][i+160] = refWinY2_ref_mid[64][i];
                            swOutLv0_ime_temp[j][i+192] = refWinY2_ref_mid[64][i + 32];
                            swOutLv0_ime_temp[j][i+224] = refWinY2_ref_mid[64][63];
                        }
                    }
                    for (int j = 0; j < 32; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 16; i++)
                        {
    #pragma HLS unroll
                            /*************************padding chroma***********************************/
                            refWinU_temp[j*128+i] = refWinU1_ref_mid[32][0];
                            refWinU_temp[j*128+i + 16] = refWinU1_ref_mid[32][0];
                            refWinU_temp[j*128+i+32] = refWinU1_ref_mid[32][0];
                            refWinU_temp[j*128+i + 48] = refWinU1_ref_mid[32][i];
                            refWinU_temp[j*128+i+64] = refWinU1_ref_mid[32][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU2_ref_mid[32][i];
                            refWinU_temp[j*128+i+96] = refWinU2_ref_mid[32][i + 16];
                            refWinU_temp[j*128+i + 112] = refWinU2_ref_mid[32][31];
                                        
                            refWinV_temp[j*128+i] = refWinV1_ref_mid[32][0];
                            refWinV_temp[j*128+i + 16] = refWinV1_ref_mid[32][0];
                            refWinV_temp[j*128+i+32] = refWinV1_ref_mid[32][0];
                            refWinV_temp[j*128+i + 48] = refWinV1_ref_mid[32][i];
                            refWinV_temp[j*128+i+64] = refWinV1_ref_mid[32][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV2_ref_mid[32][i];
                            refWinV_temp[j*128+i+96] = refWinV2_ref_mid[32][i + 16];
                            refWinV_temp[j*128+i + 112] = refWinV2_ref_mid[32][31];
                        }
                    }

                    for (int j = 64; j < 192; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 32; i++)
                        {
    #pragma HLS unroll
                            /*************************padding luma***********************************/
                            // swOutLv0_temp[j][i] = refWinY1_ref_mid[j][0];
                            // swOutLv0_temp[j][i+32] = refWinY1_ref_mid[j][0];
                            // swOutLv0_temp[j][i+64] = refWinY1_ref_mid[j][0];
                            // swOutLv0_temp[j][i+96] = refWinY1_ref_mid[j][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i+160] = refWinY2_ref_mid[j][i];
                            // swOutLv0_temp[j][i+192] = refWinY2_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i+224] = refWinY2_ref_mid[j][63];

                            swOutLv0_ime_temp[j][i] = refWinY1_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i+32] = refWinY1_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i+64] = refWinY1_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i+96] = refWinY1_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i+160] = refWinY2_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+192] = refWinY2_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i+224] = refWinY2_ref_mid[j][63];
                        }
                    }

                    for (int j = 32; j < 96; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 16; i++)
                        {
    #pragma HLS unroll
                            /*************************padding chroma***********************************/
                            refWinU_temp[j*128+i] = refWinU1_ref_mid[j][0];
                            refWinU_temp[j*128+i + 16] = refWinU1_ref_mid[j][0];
                            refWinU_temp[j*128+i+32] = refWinU1_ref_mid[j][0];
                            refWinU_temp[j*128+i + 48] = refWinU1_ref_mid[j][i];
                            refWinU_temp[j*128+i+64] = refWinU1_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU2_ref_mid[j][i];
                            refWinU_temp[j*128+i+96] = refWinU2_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 112] = refWinU2_ref_mid[j][31];
                                        
                            refWinV_temp[j*128+i] = refWinV1_ref_mid[j][0];
                            refWinV_temp[j*128+i + 16] = refWinV1_ref_mid[j][0];
                            refWinV_temp[j*128+i+32] = refWinV1_ref_mid[j][0];
                            refWinV_temp[j*128+i + 48] = refWinV1_ref_mid[j][i];
                            refWinV_temp[j*128+i+64] = refWinV1_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV2_ref_mid[j][i];
                            refWinV_temp[j*128+i+96] = refWinV2_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 112] = refWinV2_ref_mid[j][31];
                        }
                    }
                }
                else if ((pix_x + 128) == luma_width_padded+64)
                {
                    for (int j = 0; j < 64; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 32; i++)
                        {
    #pragma HLS unroll
                            /*************************padding luma***********************************/
                            // swOutLv0_temp[j][i] = refWinY1_ref_mid[64][0];
                            // swOutLv0_temp[j][i+32] = refWinY1_ref_mid[64][0];
                            // swOutLv0_temp[j][i+64] = refWinY1_ref_mid[64][0];
                            // swOutLv0_temp[j][i+96] = refWinY1_ref_mid[64][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_ref_mid[64][i + 32];
                            // swOutLv0_temp[j][i+160] = refWinY1_ref_mid[64][63];
                            // swOutLv0_temp[j][i+192] = refWinY1_ref_mid[64][63];
                            // swOutLv0_temp[j][i+224] = refWinY1_ref_mid[64][63];

                            swOutLv0_ime_temp[j][i] = refWinY1_ref_mid[64][0];
                            swOutLv0_ime_temp[j][i+32] = refWinY1_ref_mid[64][0];
                            swOutLv0_ime_temp[j][i+64] = refWinY1_ref_mid[64][0];
                            swOutLv0_ime_temp[j][i+96] = refWinY1_ref_mid[64][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_ref_mid[64][i + 32];
                            swOutLv0_ime_temp[j][i+160] = refWinY1_ref_mid[64][63];
                            swOutLv0_ime_temp[j][i+192] = refWinY1_ref_mid[64][63];
                            swOutLv0_ime_temp[j][i+224] = refWinY1_ref_mid[64][63];
                        }
                    }
                    for (int j = 0; j < 32; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 16; i++)
                        {
    #pragma HLS unroll
                            /*************************padding chroma***********************************/
                            refWinU_temp[j*128+i] = refWinU1_ref_mid[32][0];
                            refWinU_temp[j*128+i + 16] = refWinU1_ref_mid[32][0];
                            refWinU_temp[j*128+i+32] = refWinU1_ref_mid[32][0];
                            refWinU_temp[j*128+i + 48] = refWinU1_ref_mid[32][i];
                            refWinU_temp[j*128+i+64] = refWinU1_ref_mid[32][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU1_ref_mid[32][31];
                            refWinU_temp[j*128+i+96] = refWinU1_ref_mid[32][31];
                            refWinU_temp[j*128+i + 112] = refWinU1_ref_mid[32][31];
                            
                            refWinV_temp[j*128+i] = refWinV1_ref_mid[32][0];
                            refWinV_temp[j*128+i + 16] = refWinV1_ref_mid[32][0];
                            refWinV_temp[j*128+i+32] = refWinV1_ref_mid[32][0];
                            refWinV_temp[j*128+i + 48] = refWinV1_ref_mid[32][i];
                            refWinV_temp[j*128+i+64] = refWinV1_ref_mid[32][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV1_ref_mid[32][31];
                            refWinV_temp[j*128+i+96] = refWinV1_ref_mid[32][31];
                            refWinV_temp[j*128+i + 112] = refWinV1_ref_mid[32][31];
                        }
                    }

                    for (int j = 64; j < 192; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 32; i++)
                        {
    #pragma HLS unroll
                            /*************************padding luma***********************************/
                            // swOutLv0_temp[j][i] = refWinY1_ref_mid[j][0];
                            // swOutLv0_temp[j][i+32] = refWinY1_ref_mid[j][0];
                            // swOutLv0_temp[j][i+64] = refWinY1_ref_mid[j][0];
                            // swOutLv0_temp[j][i+96] = refWinY1_ref_mid[j][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i+160] = refWinY1_ref_mid[j][63];
                            // swOutLv0_temp[j][i+192] = refWinY1_ref_mid[j][63];
                            // swOutLv0_temp[j][i+224] = refWinY1_ref_mid[j][63];

                            swOutLv0_ime_temp[j][i] = refWinY1_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i+32] = refWinY1_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i+64] = refWinY1_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i+96] = refWinY1_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i+160] = refWinY1_ref_mid[j][63];
                            swOutLv0_ime_temp[j][i+192] = refWinY1_ref_mid[j][63];
                            swOutLv0_ime_temp[j][i+224] = refWinY1_ref_mid[j][63];
                        }
                    }

                    for (int j = 32; j < 96; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 16; i++)
                        {
    #pragma HLS unroll
                            /*************************padding chroma***********************************/
                            refWinU_temp[j*128+i] = refWinU1_ref_mid[j][0];
                            refWinU_temp[j*128+i + 16] = refWinU1_ref_mid[j][0];
                            refWinU_temp[j*128+i+32] = refWinU1_ref_mid[j][0];
                            refWinU_temp[j*128+i + 48] = refWinU1_ref_mid[j][i];
                            refWinU_temp[j*128+i+64] = refWinU1_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU1_ref_mid[j][31];
                            refWinU_temp[j*128+i+96] = refWinU1_ref_mid[j][31];
                            refWinU_temp[j*128+i + 112] = refWinU1_ref_mid[j][31];
                                        
                            refWinV_temp[j*128+i] = refWinV1_ref_mid[j][0];
                            refWinV_temp[j*128+i + 16] = refWinV1_ref_mid[j][0];
                            refWinV_temp[j*128+i+32] = refWinV1_ref_mid[j][0];
                            refWinV_temp[j*128+i + 48] = refWinV1_ref_mid[j][i];
                            refWinV_temp[j*128+i+64] = refWinV1_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV1_ref_mid[j][31];
                            refWinV_temp[j*128+i+96] = refWinV1_ref_mid[j][31];
                            refWinV_temp[j*128+i + 112] = refWinV1_ref_mid[j][31];
                        }
                    }
                }
            }
            else if (64 == pix_x)
            {
                if ((pix_x + 128) <= (luma_width_padded - 64))
                {
                    for (int j = 0; j < 64; j++)
                    {
        #pragma HLS pipeline 
                        for (int i = 0; i < 32; i++)
                        {
        #pragma HLS unroll
                            /*************************padding luma*************************************/
                            // swOutLv0_temp[j][i] = refWinY0_ref_mid[64][0];
                            // swOutLv0_temp[j][i+32] = refWinY0_ref_mid[64][i];
                            // swOutLv0_temp[j][i+64] = refWinY0_ref_mid[64][i + 32];
                            // swOutLv0_temp[j][i+96] = refWinY1_ref_mid[64][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_ref_mid[64][i + 32];
                            // swOutLv0_temp[j][i+160] = refWinY2_ref_mid[64][i];
                            // swOutLv0_temp[j][i+192] = refWinY2_ref_mid[64][i + 32];
                            // swOutLv0_temp[j][i+224] = refWinY3_ref_mid[64][i];

                            swOutLv0_ime_temp[j][i] = refWinY0_ref_mid[64][0];
                            swOutLv0_ime_temp[j][i+32] = refWinY0_ref_mid[64][i];
                            swOutLv0_ime_temp[j][i+64] = refWinY0_ref_mid[64][i + 32];
                            swOutLv0_ime_temp[j][i+96] = refWinY1_ref_mid[64][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_ref_mid[64][i + 32];
                            swOutLv0_ime_temp[j][i+160] = refWinY2_ref_mid[64][i];
                            swOutLv0_ime_temp[j][i+192] = refWinY2_ref_mid[64][i + 32];
                            swOutLv0_ime_temp[j][i+224] = refWinY3_ref_mid[64][i];
                        }
                    }

                    for (int j = 0; j < 32; j++)
                    {
            #pragma HLS pipeline 
                        for (int i = 0; i < 16; i++)
                        {
            #pragma HLS unroll
                            /*************************padding chroma*************************************/
                            refWinU_temp[j*128+i] = refWinU0_ref_mid[32][0];
                            refWinU_temp[j*128+i + 16] = refWinU0_ref_mid[32][i];
                            refWinU_temp[j*128+i+32] = refWinU0_ref_mid[32][i + 16];
                            refWinU_temp[j*128+i + 48] = refWinU1_ref_mid[32][i];
                            refWinU_temp[j*128+i+64] = refWinU1_ref_mid[32][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU2_ref_mid[32][i];
                            refWinU_temp[j*128+i+96] = refWinU2_ref_mid[32][i + 16];
                            refWinU_temp[j*128+i + 112] = refWinU3_ref_mid[32][i];

                            refWinV_temp[j*128+i] = refWinV0_ref_mid[32][0];
                            refWinV_temp[j*128+i + 16] = refWinV0_ref_mid[32][i];
                            refWinV_temp[j*128+i+32] = refWinV0_ref_mid[32][i + 16];
                            refWinV_temp[j*128+i + 48] = refWinV1_ref_mid[32][i];
                            refWinV_temp[j*128+i+64] = refWinV1_ref_mid[32][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV2_ref_mid[32][i];
                            refWinV_temp[j*128+i+96] = refWinV2_ref_mid[32][i + 16];
                            refWinV_temp[j*128+i + 112] = refWinV3_ref_mid[32][i];
                        }
                    }

                    for (int j = 64; j < 192; j++)
                    {
        #pragma HLS pipeline 
                        for (int i = 0; i < 32; i++)
                        {
        #pragma HLS unroll 
                            /*************************padding luma*************************************/
                            // swOutLv0_temp[j][i] = refWinY0_ref_mid[j][0];
                            // swOutLv0_temp[j][i+32] = refWinY0_ref_mid[j][i];
                            // swOutLv0_temp[j][i+64] = refWinY0_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i+96] = refWinY1_ref_mid[j][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i+160] = refWinY2_ref_mid[j][i];
                            // swOutLv0_temp[j][i+192] = refWinY2_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i+224] = refWinY3_ref_mid[j][i];

                            swOutLv0_ime_temp[j][i] = refWinY0_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i+32] = refWinY0_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+64] = refWinY0_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i+96] = refWinY1_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i+160] = refWinY2_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+192] = refWinY2_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i+224] = refWinY3_ref_mid[j][i];
                        }
                    }

                    for (int j = 32; j < 96; j++)
                    {
            #pragma HLS pipeline 
                        for (int i = 0; i < 16; i++)
                        {
            #pragma HLS unroll 
                            /*************************padding chroma*************************************/
                            refWinU_temp[j*128+i] = refWinU0_ref_mid[j][0];
                            refWinU_temp[j*128+i + 16] = refWinU0_ref_mid[j][i];
                            refWinU_temp[j*128+i + 32] = refWinU0_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 48] = refWinU1_ref_mid[j][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU2_ref_mid[j][i];
                            refWinU_temp[j*128+i + 96] = refWinU2_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 112] = refWinU3_ref_mid[j][i];
                                        
                            refWinV_temp[j*128+i] = refWinV0_ref_mid[j][0];
                            refWinV_temp[j*128+i + 16] = refWinV0_ref_mid[j][i];
                            refWinV_temp[j*128+i + 32] = refWinV0_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 48] = refWinV1_ref_mid[j][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV2_ref_mid[j][i];
                            refWinV_temp[j*128+i + 96] = refWinV2_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 112] = refWinV3_ref_mid[j][i];
                        }
                    } 
                }
                else if ((pix_x + 128) == luma_width_padded)
                {
                    for (int j = 0; j < 64; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 32; i++)
                        {
    #pragma HLS unroll
                            /*************************padding luma*************************************/
                            // swOutLv0_temp[j][i] = refWinY0_ref_mid[64][0];
                            // swOutLv0_temp[j][i+32] = refWinY0_ref_mid[64][i];
                            // swOutLv0_temp[j][i+64] = refWinY0_ref_mid[64][i + 32];
                            // swOutLv0_temp[j][i+96] = refWinY1_ref_mid[64][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_ref_mid[64][i + 32];
                            // swOutLv0_temp[j][i+160] = refWinY2_ref_mid[64][i];
                            // swOutLv0_temp[j][i+192] = refWinY2_ref_mid[64][i+32];
                            // swOutLv0_temp[j][i+224] = refWinY2_ref_mid[64][63];

                            swOutLv0_ime_temp[j][i] = refWinY0_ref_mid[64][0];
                            swOutLv0_ime_temp[j][i+32] = refWinY0_ref_mid[64][i];
                            swOutLv0_ime_temp[j][i+64] = refWinY0_ref_mid[64][i + 32];
                            swOutLv0_ime_temp[j][i+96] = refWinY1_ref_mid[64][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_ref_mid[64][i + 32];
                            swOutLv0_ime_temp[j][i+160] = refWinY2_ref_mid[64][i];
                            swOutLv0_ime_temp[j][i+192] = refWinY2_ref_mid[64][i+32];
                            swOutLv0_ime_temp[j][i+224] = refWinY2_ref_mid[64][63];
                        }
                    }

                    for (int j = 0; j < 32; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 16; i++)
                        {
    #pragma HLS unroll
                            /*************************padding chroma*************************************/
                            refWinU_temp[j*128+i] = refWinU0_ref_mid[32][0];
                            refWinU_temp[j*128+i + 16] = refWinU0_ref_mid[32][i];
                            refWinU_temp[j*128+i + 32] = refWinU0_ref_mid[32][i + 16];
                            refWinU_temp[j*128+i + 48] = refWinU1_ref_mid[32][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_ref_mid[32][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU2_ref_mid[32][i];
                            refWinU_temp[j*128+i + 96] = refWinU2_ref_mid[32][i+16];
                            refWinU_temp[j*128+i + 112] = refWinU2_ref_mid[32][31];
                                        
                            refWinV_temp[j*128+i] = refWinV0_ref_mid[32][0];
                            refWinV_temp[j*128+i + 16] = refWinV0_ref_mid[32][i];
                            refWinV_temp[j*128+i + 32] = refWinV0_ref_mid[32][i + 16];
                            refWinV_temp[j*128+i + 48] = refWinV1_ref_mid[32][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_ref_mid[32][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV2_ref_mid[32][i];
                            refWinV_temp[j*128+i + 96] = refWinV2_ref_mid[32][i+16];
                            refWinV_temp[j*128+i + 112] = refWinV2_ref_mid[32][31];
                        }
                    }

                    for (int j = 64; j < 192; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 32; i++)
                        {
    #pragma HLS unroll 
                            /*************************padding luma*************************************/
                            // swOutLv0_temp[j][i] = refWinY0_ref_mid[j][0];
                            // swOutLv0_temp[j][i+32] = refWinY0_ref_mid[j][i];
                            // swOutLv0_temp[j][i+64] = refWinY0_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i+96] = refWinY1_ref_mid[j][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i+160] = refWinY2_ref_mid[j][i];
                            // swOutLv0_temp[j][i+192] = refWinY2_ref_mid[j][i+32];
                            // swOutLv0_temp[j][i+224] = refWinY2_ref_mid[j][63];

                            swOutLv0_ime_temp[j][i] = refWinY0_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i+32] = refWinY0_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+64] = refWinY0_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i+96] = refWinY1_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i+160] = refWinY2_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+192] = refWinY2_ref_mid[j][i+32];
                            swOutLv0_ime_temp[j][i+224] = refWinY2_ref_mid[j][63];
                        }
                    }

                    for (int j = 32; j < 96; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 16; i++)
                        {
    #pragma HLS unroll 
                            /*************************padding chroma*************************************/
                            refWinU_temp[j*128+i] = refWinU0_ref_mid[j][0];
                            refWinU_temp[j*128+i + 16] = refWinU0_ref_mid[j][i];
                            refWinU_temp[j*128+i + 32] = refWinU0_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 48] = refWinU1_ref_mid[j][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU2_ref_mid[j][i];
                            refWinU_temp[j*128+i + 96] = refWinU2_ref_mid[j][i+16];
                            refWinU_temp[j*128+i + 112] = refWinU2_ref_mid[j][31];
                                        
                            refWinV_temp[j*128+i] = refWinV0_ref_mid[j][0];
                            refWinV_temp[j*128+i + 16] = refWinV0_ref_mid[j][i];
                            refWinV_temp[j*128+i + 32] = refWinV0_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 48] = refWinV1_ref_mid[j][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV2_ref_mid[j][i];
                            refWinV_temp[j*128+i + 96] = refWinV2_ref_mid[j][i+16];
                            refWinV_temp[j*128+i + 112] = refWinV2_ref_mid[j][31];
                        }
                    }
                }
                else if ((pix_x + 128) == luma_width_padded+64)
                {
                    for (int j = 0; j < 64; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 32; i++)
                        {
    #pragma HLS unroll
                            /*************************padding luma*************************************/
                            // swOutLv0_temp[j][i] = refWinY0_ref_mid[64][0];
                            // swOutLv0_temp[j][i+32] = refWinY0_ref_mid[64][i];
                            // swOutLv0_temp[j][i+64] = refWinY0_ref_mid[64][i + 32];
                            // swOutLv0_temp[j][i+96] = refWinY1_ref_mid[64][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_ref_mid[64][i+32];
                            // swOutLv0_temp[j][i+160] = refWinY1_ref_mid[64][63];
                            // swOutLv0_temp[j][i+192] = refWinY1_ref_mid[64][63];
                            // swOutLv0_temp[j][i+224] = refWinY1_ref_mid[64][63];

                            swOutLv0_ime_temp[j][i] = refWinY0_ref_mid[64][0];
                            swOutLv0_ime_temp[j][i+32] = refWinY0_ref_mid[64][i];
                            swOutLv0_ime_temp[j][i+64] = refWinY0_ref_mid[64][i + 32];
                            swOutLv0_ime_temp[j][i+96] = refWinY1_ref_mid[64][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_ref_mid[64][i+32];
                            swOutLv0_ime_temp[j][i+160] = refWinY1_ref_mid[64][63];
                            swOutLv0_ime_temp[j][i+192] = refWinY1_ref_mid[64][63];
                            swOutLv0_ime_temp[j][i+224] = refWinY1_ref_mid[64][63];
                        }
                    }

                    for (int j = 0; j < 32; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 16; i++)
                        {
    #pragma HLS unroll
                            /*************************padding chroma*************************************/
                            refWinU_temp[j*128+i] = refWinU0_ref_mid[32][0];
                            refWinU_temp[j*128+i + 16] = refWinU0_ref_mid[32][i];
                            refWinU_temp[j*128+i + 32] = refWinU0_ref_mid[32][i + 16];
                            refWinU_temp[j*128+i + 48] = refWinU1_ref_mid[32][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_ref_mid[32][i+16];
                            refWinU_temp[j*128+i + 80] = refWinU1_ref_mid[32][31];
                            refWinU_temp[j*128+i + 96] = refWinU1_ref_mid[32][31];
                            refWinU_temp[j*128+i + 112] = refWinU1_ref_mid[32][31];
                                        
                            refWinV_temp[j*128+i] = refWinV0_ref_mid[32][0];
                            refWinV_temp[j*128+i + 16] = refWinV0_ref_mid[32][i];
                            refWinV_temp[j*128+i + 32] = refWinV0_ref_mid[32][i + 16];
                            refWinV_temp[j*128+i + 48] = refWinV1_ref_mid[32][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_ref_mid[32][i+16];
                            refWinV_temp[j*128+i + 80] = refWinV1_ref_mid[32][31];
                            refWinV_temp[j*128+i + 96] = refWinV1_ref_mid[32][31];
                            refWinV_temp[j*128+i + 112] = refWinV1_ref_mid[32][31];
                        }
                    }

                    for (int j = 64; j < 192; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 32; i++)
                        {
    #pragma HLS unroll 
                            /*************************padding luma*************************************/
                            // swOutLv0_temp[j][i] = refWinY0_ref_mid[j][0];
                            // swOutLv0_temp[j][i+32] = refWinY0_ref_mid[j][i];
                            // swOutLv0_temp[j][i+64] = refWinY0_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i+96] = refWinY1_ref_mid[j][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_ref_mid[j][i+32];
                            // swOutLv0_temp[j][i+160] = refWinY1_ref_mid[j][63];
                            // swOutLv0_temp[j][i+192] = refWinY1_ref_mid[j][63];
                            // swOutLv0_temp[j][i+224] = refWinY1_ref_mid[j][63];

                            swOutLv0_ime_temp[j][i] = refWinY0_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i+32] = refWinY0_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+64] = refWinY0_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i+96] = refWinY1_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_ref_mid[j][i+32];
                            swOutLv0_ime_temp[j][i+160] = refWinY1_ref_mid[j][63];
                            swOutLv0_ime_temp[j][i+192] = refWinY1_ref_mid[j][63];
                            swOutLv0_ime_temp[j][i+224] = refWinY1_ref_mid[j][63];
                        }
                    }

                    for (int j = 32; j < 96; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 16; i++)
                        {
    #pragma HLS unroll 
                            /*************************padding chroma*************************************/
                            refWinU_temp[j*128+i] = refWinU0_ref_mid[j][0];
                            refWinU_temp[j*128+i + 16] = refWinU0_ref_mid[j][i];
                            refWinU_temp[j*128+i+32] = refWinU0_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 48] = refWinU1_ref_mid[j][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_ref_mid[j][i+16];
                            refWinU_temp[j*128+i + 80] = refWinU1_ref_mid[j][31];
                            refWinU_temp[j*128+i + 96] = refWinU1_ref_mid[j][31];
                            refWinU_temp[j*128+i + 112] = refWinU1_ref_mid[j][31];
                                        
                            refWinV_temp[j*128+i] = refWinV0_ref_mid[j][0];
                            refWinV_temp[j*128+i + 16] = refWinV0_ref_mid[j][i];
                            refWinV_temp[j*128+i + 32] = refWinV0_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 48] = refWinV1_ref_mid[j][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_ref_mid[j][i+16];
                            refWinV_temp[j*128+i + 80] = refWinV1_ref_mid[j][31];
                            refWinV_temp[j*128+i + 96] = refWinV1_ref_mid[j][31];
                            refWinV_temp[j*128+i + 112] = refWinV1_ref_mid[j][31];
                        }
                    }
                }
            }
            else
            {
                if ((pix_x + 128) <= (luma_width_padded - 64))
                {
                    for (int j = 0; j < 64; j++)
                    {
        #pragma HLS pipeline
                        for (int i = 0; i < 32; i++)
                        {
        #pragma HLS unroll
                            /******************************padding luma***************************/
                            // swOutLv0_temp[j][i] = refWinY32x192_ref_mid[64][i];
                            // swOutLv0_temp[j][i+32] = refWinY0_ref_mid[64][i];
                            // swOutLv0_temp[j][i+64] = refWinY0_ref_mid[64][i + 32];
                            // swOutLv0_temp[j][i+96] = refWinY1_ref_mid[64][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_ref_mid[64][i + 32];
                            // swOutLv0_temp[j][i+160] = refWinY2_ref_mid[64][i];
                            // swOutLv0_temp[j][i+192] = refWinY2_ref_mid[64][i + 32];
                            // swOutLv0_temp[j][i+224] = refWinY3_ref_mid[64][i];

                            swOutLv0_ime_temp[j][i] = refWinY32x192_ref_mid[64][i];
                            swOutLv0_ime_temp[j][i+32] = refWinY0_ref_mid[64][i];
                            swOutLv0_ime_temp[j][i+64] = refWinY0_ref_mid[64][i + 32];
                            swOutLv0_ime_temp[j][i+96] = refWinY1_ref_mid[64][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_ref_mid[64][i + 32];
                            swOutLv0_ime_temp[j][i+160] = refWinY2_ref_mid[64][i];
                            swOutLv0_ime_temp[j][i+192] = refWinY2_ref_mid[64][i + 32];
                            swOutLv0_ime_temp[j][i+224] = refWinY3_ref_mid[64][i];
                        }
                    }

                    for (int j = 0; j < 32; j++)
                    {
            #pragma HLS pipeline
                        for (int i = 0; i < 16; i++)
                        {
            #pragma HLS unroll
                            /******************************padding chroma***************************/
                            refWinU_temp[j*128+i] = refWinU16x96_ref_mid[32][i];
                            refWinU_temp[j*128+i + 16] = refWinU0_ref_mid[32][i];
                            refWinU_temp[j*128+i + 32] = refWinU0_ref_mid[32][i + 16];
                            refWinU_temp[j*128+i + 48] = refWinU1_ref_mid[32][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_ref_mid[32][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU2_ref_mid[32][i];
                            refWinU_temp[j*128+i + 96] = refWinU2_ref_mid[32][i + 16];
                            refWinU_temp[j*128+i + 112] = refWinU3_ref_mid[32][i];

                            refWinV_temp[j*128+i] = refWinV16x96_ref_mid[32][i];
                            refWinV_temp[j*128+i + 16] = refWinV0_ref_mid[32][i];
                            refWinV_temp[j*128+i + 32] = refWinV0_ref_mid[32][i + 16];
                            refWinV_temp[j*128+i + 48] = refWinV1_ref_mid[32][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_ref_mid[32][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV2_ref_mid[32][i];
                            refWinV_temp[j*128+i + 96] = refWinV2_ref_mid[32][i + 16];
                            refWinV_temp[j*128+i + 112] = refWinV3_ref_mid[32][i];
                        }
                    }

                    for (int j = 64; j < 192; j++)
                    {
        #pragma HLS pipeline
                        for (int i = 0; i < 32; i++)
                        {
        #pragma HLS unroll
                            /******************************padding luma***************************/
                            // swOutLv0_temp[j][i] = refWinY32x192_ref_mid[j][i];
                            // swOutLv0_temp[j][i + 32] = refWinY0_ref_mid[j][i];
                            // swOutLv0_temp[j][i+64] = refWinY0_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 96] = refWinY1_ref_mid[j][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 160] = refWinY2_ref_mid[j][i];
                            // swOutLv0_temp[j][i+192] = refWinY2_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 224] = refWinY3_ref_mid[j][i];

                            swOutLv0_ime_temp[j][i] = refWinY32x192_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i + 32] = refWinY0_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+64] = refWinY0_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 96] = refWinY1_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 160] = refWinY2_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+192] = refWinY2_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 224] = refWinY3_ref_mid[j][i];
                        }
                    }

                    for (int j = 32; j < 96; j++)
                    {
            #pragma HLS pipeline
                        for (int i = 0; i < 16; i++)
                        {
            #pragma HLS unroll
                            /******************************padding chroma***************************/
                            refWinU_temp[j*128+i] = refWinU16x96_ref_mid[j][i];
                            refWinU_temp[j*128+i + 16] = refWinU0_ref_mid[j][i];
                            refWinU_temp[j*128+i + 32] = refWinU0_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 48] = refWinU1_ref_mid[j][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU2_ref_mid[j][i];
                            refWinU_temp[j*128+i + 96] = refWinU2_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 112] = refWinU3_ref_mid[j][i];
                                        
                            refWinV_temp[j*128+i] = refWinV16x96_ref_mid[j][i];
                            refWinV_temp[j*128+i + 16] = refWinV0_ref_mid[j][i];
                            refWinV_temp[j*128+i + 32] = refWinV0_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 48] = refWinV1_ref_mid[j][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV2_ref_mid[j][i];
                            refWinV_temp[j*128+i + 96] = refWinV2_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 112] = refWinV3_ref_mid[j][i];
                        }
                    }
                }
                else if ((pix_x + 128) == luma_width_padded)
                {
                    for (int j = 0; j < 64; j++)
                    {
    #pragma HLS pipeline
                        for (int i = 0; i < 32; i++)
                        {
    #pragma HLS unroll
                            /******************************padding luma***************************/
                            // swOutLv0_temp[j][i] = refWinY32x192_ref_mid[64][i];
                            // swOutLv0_temp[j][i + 32] = refWinY0_ref_mid[64][i];
                            // swOutLv0_temp[j][i+64] = refWinY0_ref_mid[64][i + 32];
                            // swOutLv0_temp[j][i + 96] = refWinY1_ref_mid[64][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_ref_mid[64][i + 32];
                            // swOutLv0_temp[j][i + 160] = refWinY2_ref_mid[64][i];
                            // swOutLv0_temp[j][i+192] = refWinY2_ref_mid[64][i + 32];
                            // swOutLv0_temp[j][i + 224] = refWinY2_ref_mid[64][63];

                            swOutLv0_ime_temp[j][i] = refWinY32x192_ref_mid[64][i];
                            swOutLv0_ime_temp[j][i + 32] = refWinY0_ref_mid[64][i];
                            swOutLv0_ime_temp[j][i+64] = refWinY0_ref_mid[64][i + 32];
                            swOutLv0_ime_temp[j][i + 96] = refWinY1_ref_mid[64][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_ref_mid[64][i + 32];
                            swOutLv0_ime_temp[j][i + 160] = refWinY2_ref_mid[64][i];
                            swOutLv0_ime_temp[j][i+192] = refWinY2_ref_mid[64][i + 32];
                            swOutLv0_ime_temp[j][i + 224] = refWinY2_ref_mid[64][63];
                        }
                    }

                    for (int j = 0; j < 32; j++)
                    {
    #pragma HLS pipeline
                        for (int i = 0; i < 16; i++)
                        {
    #pragma HLS unroll
                            /******************************padding chroma***************************/
                            refWinU_temp[j*128+i] = refWinU16x96_ref_mid[32][i];
                            refWinU_temp[j*128+i + 16] = refWinU0_ref_mid[32][i];
                            refWinU_temp[j*128+i + 32] = refWinU0_ref_mid[32][i + 16];
                            refWinU_temp[j*128+i + 48] = refWinU1_ref_mid[32][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_ref_mid[32][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU2_ref_mid[32][i];
                            refWinU_temp[j*128+i + 96] = refWinU2_ref_mid[32][i + 16];
                            refWinU_temp[j*128+i + 112] = refWinU2_ref_mid[32][31];
                                        
                            refWinV_temp[j*128+i] = refWinV16x96_ref_mid[32][i];
                            refWinV_temp[j*128+i + 16] = refWinV0_ref_mid[32][i];
                            refWinV_temp[j*128+i + 32] = refWinV0_ref_mid[32][i + 16];
                            refWinV_temp[j*128+i + 48] = refWinV1_ref_mid[32][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_ref_mid[32][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV2_ref_mid[32][i];
                            refWinV_temp[j*128+i + 96] = refWinV2_ref_mid[32][i + 16];
                            refWinV_temp[j*128+i + 112] = refWinV2_ref_mid[32][31];
                        }
                    }

                    for (int j = 64; j < 192; j++)
                    {
    #pragma HLS pipeline
                        for (int i = 0; i < 32; i++)
                        {
    #pragma HLS unroll
                            /******************************padding luma***************************/
                            // swOutLv0_temp[j][i] = refWinY32x192_ref_mid[j][i];
                            // swOutLv0_temp[j][i + 32] = refWinY0_ref_mid[j][i];
                            // swOutLv0_temp[j][i+64] = refWinY0_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 96] = refWinY1_ref_mid[j][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 160] = refWinY2_ref_mid[j][i];
                            // swOutLv0_temp[j][i+192] = refWinY2_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 224] = refWinY2_ref_mid[j][63];

                            swOutLv0_ime_temp[j][i] = refWinY32x192_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i + 32] = refWinY0_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+64] = refWinY0_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 96] = refWinY1_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 160] = refWinY2_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+192] = refWinY2_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 224] = refWinY2_ref_mid[j][63];
                        }
                    }

                    for (int j = 32; j < 96; j++)
                    {
    #pragma HLS pipeline
                        for (int i = 0; i < 16; i++)
                        {
    #pragma HLS unroll
                            /******************************padding chroma***************************/
                            refWinU_temp[j*128+i] = refWinU16x96_ref_mid[j][i];
                            refWinU_temp[j*128+i + 16] = refWinU0_ref_mid[j][i];
                            refWinU_temp[j*128+i + 32] = refWinU0_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 48] = refWinU1_ref_mid[j][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU2_ref_mid[j][i];
                            refWinU_temp[j*128+i + 96] = refWinU2_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 112] = refWinU2_ref_mid[j][31];
                                        
                            refWinV_temp[j*128+i] = refWinV16x96_ref_mid[j][i];
                            refWinV_temp[j*128+i + 16] = refWinV0_ref_mid[j][i];
                            refWinV_temp[j*128+i + 32] = refWinV0_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 48] = refWinV1_ref_mid[j][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV2_ref_mid[j][i];
                            refWinV_temp[j*128+i + 96] = refWinV2_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 112] = refWinV2_ref_mid[j][31];
                        }
                    }
                }
                else if ((pix_x + 128) == luma_width_padded+64)
                {
                    for (int j = 0; j < 64; j++)
                    {
    #pragma HLS pipeline
                        for (int i = 0; i < 32; i++)
                        {
    #pragma HLS unroll
                            /******************************padding luma***************************/
                            // swOutLv0_temp[j][i] = refWinY32x192_ref_mid[64][i];
                            // swOutLv0_temp[j][i+32] = refWinY0_ref_mid[64][i];
                            // swOutLv0_temp[j][i+64] = refWinY0_ref_mid[64][i + 32];
                            // swOutLv0_temp[j][i+96] = refWinY1_ref_mid[64][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_ref_mid[64][i + 32];
                            // swOutLv0_temp[j][i+160] = refWinY1_ref_mid[64][63];
                            // swOutLv0_temp[j][i+192] = refWinY1_ref_mid[64][63];
                            // swOutLv0_temp[j][i+224] = refWinY1_ref_mid[64][63];

                            swOutLv0_ime_temp[j][i] = refWinY32x192_ref_mid[64][i];
                            swOutLv0_ime_temp[j][i+32] = refWinY0_ref_mid[64][i];
                            swOutLv0_ime_temp[j][i+64] = refWinY0_ref_mid[64][i + 32];
                            swOutLv0_ime_temp[j][i+96] = refWinY1_ref_mid[64][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_ref_mid[64][i + 32];
                            swOutLv0_ime_temp[j][i+160] = refWinY1_ref_mid[64][63];
                            swOutLv0_ime_temp[j][i+192] = refWinY1_ref_mid[64][63];
                            swOutLv0_ime_temp[j][i+224] = refWinY1_ref_mid[64][63];
                        }
                    }

                    for (int j = 0; j < 32; j++)
                    {
    #pragma HLS pipeline
                        for (int i = 0; i < 16; i++)
                        {
    #pragma HLS unroll
                            /******************************padding chroma***************************/
                            refWinU_temp[j*128+i] = refWinU16x96_ref_mid[32][i];
                            refWinU_temp[j*128+i + 16] = refWinU0_ref_mid[32][i];
                            refWinU_temp[j*128+i+32] = refWinU0_ref_mid[32][i + 16];
                            refWinU_temp[j*128+i + 48] = refWinU1_ref_mid[32][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_ref_mid[32][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU1_ref_mid[32][31];
                            refWinU_temp[j*128+i + 96] = refWinU1_ref_mid[32][31];
                            refWinU_temp[j*128+i + 112] = refWinU1_ref_mid[32][31];
                                        
                            refWinV_temp[j*128+i] = refWinV16x96_ref_mid[32][i];
                            refWinV_temp[j*128+i + 16] = refWinV0_ref_mid[32][i];
                            refWinV_temp[j*128+i + 32] = refWinV0_ref_mid[32][i + 16];
                            refWinV_temp[j*128+i + 48] = refWinV1_ref_mid[32][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_ref_mid[32][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV1_ref_mid[32][31];
                            refWinV_temp[j*128+i + 96] = refWinV1_ref_mid[32][31];
                            refWinV_temp[j*128+i + 112] = refWinV1_ref_mid[32][31];
                        }
                    }

                    for (int j = 64; j < 192; j++)
                    {
    #pragma HLS pipeline
                        for (int i = 0; i < 32; i++)
                        {
    #pragma HLS unroll
                            /******************************padding luma***************************/
                            // swOutLv0_temp[j][i] = refWinY32x192_ref_mid[j][i];
                            // swOutLv0_temp[j][i+32] = refWinY0_ref_mid[j][i];
                            // swOutLv0_temp[j][i+64] = refWinY0_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i+96] = refWinY1_ref_mid[j][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i+160] = refWinY1_ref_mid[j][63];
                            // swOutLv0_temp[j][i+192] = refWinY1_ref_mid[j][63];
                            // swOutLv0_temp[j][i+224] = refWinY1_ref_mid[j][63];

                            swOutLv0_ime_temp[j][i] = refWinY32x192_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+32] = refWinY0_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+64] = refWinY0_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i+96] = refWinY1_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i+160] = refWinY1_ref_mid[j][63];
                            swOutLv0_ime_temp[j][i+192] = refWinY1_ref_mid[j][63];
                            swOutLv0_ime_temp[j][i+224] = refWinY1_ref_mid[j][63];
                        }
                    }

                    for (int j = 32; j < 96; j++)
                    {
    #pragma HLS pipeline
                        for (int i = 0; i < 16; i++)
                        {
    #pragma HLS unroll
                            /******************************padding chroma***************************/
                            refWinU_temp[j*128+i] = refWinU16x96_ref_mid[j][i];
                            refWinU_temp[j*128+i + 16] = refWinU0_ref_mid[j][i];
                            refWinU_temp[j*128+i+32] = refWinU0_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 48] = refWinU1_ref_mid[j][i];
                            refWinU_temp[j*128+i+64] = refWinU1_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU1_ref_mid[j][31];
                            refWinU_temp[j*128+i+96] = refWinU1_ref_mid[j][31];
                            refWinU_temp[j*128+i + 112] = refWinU1_ref_mid[j][31];
                                        
                            refWinV_temp[j*128+i] = refWinV16x96_ref_mid[j][i];
                            refWinV_temp[j*128+i + 16] = refWinV0_ref_mid[j][i];
                            refWinV_temp[j*128+i+32] = refWinV0_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 48] = refWinV1_ref_mid[j][i];
                            refWinV_temp[j*128+i+64] = refWinV1_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV1_ref_mid[j][31];
                            refWinV_temp[j*128+i+96] = refWinV1_ref_mid[j][31];
                            refWinV_temp[j*128+i + 112] = refWinV1_ref_mid[j][31];
                        }
                    }
                }
            }
        }
        //last row
        else if((pix_y + 64)== luma_height_padded)
        {
            if (0 == pix_x)
            {
                if ((pix_x + 128) <= (luma_width_padded - 64))
                {
                    for (int j = 0; j < 128; j++)
                    {
            #pragma HLS pipeline 
                        for (int i = 0; i < 32; i++)
                        {
            #pragma HLS unroll
                            /**********************padding luma*******************************/
                            // swOutLv0_temp[j][i] = refWinY1_ref_mid[j][0];
                            // swOutLv0_temp[j][i + 32] = refWinY1_ref_mid[j][0];
                            // swOutLv0_temp[j][i+64] = refWinY1_ref_mid[j][0];
                            // swOutLv0_temp[j][i + 96] = refWinY1_ref_mid[j][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 160] = refWinY2_ref_mid[j][i];
                            // swOutLv0_temp[j][i+192] = refWinY2_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 224] = refWinY3_ref_mid[j][i];

                            swOutLv0_ime_temp[j][i] = refWinY1_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i + 32] = refWinY1_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i+64] = refWinY1_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i + 96] = refWinY1_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 160] = refWinY2_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+192] = refWinY2_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 224] = refWinY3_ref_mid[j][i];
                        }
                    }

                    for (int j = 0; j < 64; j++)
                    {
            #pragma HLS pipeline 
                        for (int i = 0; i < 16; i++)
                        {
            #pragma HLS unroll
                            /**********************padding chroma*******************************/
                            refWinU_temp[j*128+i] = refWinU1_ref_mid[j][0];
                            refWinU_temp[j*128+i + 16] = refWinU1_ref_mid[j][0];
                            refWinU_temp[j*128+i + 32] = refWinU1_ref_mid[j][0];
                            refWinU_temp[j*128+i + 48] = refWinU1_ref_mid[j][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU2_ref_mid[j][i];
                            refWinU_temp[j*128+i + 96] = refWinU2_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 112] = refWinU3_ref_mid[j][i];
                                        
                            refWinV_temp[j*128+i] = refWinV1_ref_mid[j][0];
                            refWinV_temp[j*128+i + 16] = refWinV1_ref_mid[j][0];
                            refWinV_temp[j*128+i + 32] = refWinV1_ref_mid[j][0];
                            refWinV_temp[j*128+i + 48] = refWinV1_ref_mid[j][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV2_ref_mid[j][i];
                            refWinV_temp[j*128+i + 96] = refWinV2_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 112] = refWinV3_ref_mid[j][i];
                        }
                    }

                    for (int j = 128; j < 192; j++)
                    {
            #pragma HLS pipeline 
                        for (int i = 0; i < 32; i++)
                        {
            #pragma HLS unroll
                            /**********************padding luma*******************************/
                            // swOutLv0_temp[j][i] = refWinY1_ref_mid[127][0];
                            // swOutLv0_temp[j][i + 32] = refWinY1_ref_mid[127][0];
                            // swOutLv0_temp[j][i+64] = refWinY1_ref_mid[127][0];
                            // swOutLv0_temp[j][i + 96] = refWinY1_ref_mid[127][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_ref_mid[127][i + 32];
                            // swOutLv0_temp[j][i + 160] = refWinY2_ref_mid[127][i];
                            // swOutLv0_temp[j][i+192] = refWinY2_ref_mid[127][i + 32];
                            // swOutLv0_temp[j][i + 224] = refWinY3_ref_mid[127][i];

                            swOutLv0_ime_temp[j][i] = refWinY1_ref_mid[127][0];
                            swOutLv0_ime_temp[j][i + 32] = refWinY1_ref_mid[127][0];
                            swOutLv0_ime_temp[j][i+64] = refWinY1_ref_mid[127][0];
                            swOutLv0_ime_temp[j][i + 96] = refWinY1_ref_mid[127][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_ref_mid[127][i + 32];
                            swOutLv0_ime_temp[j][i + 160] = refWinY2_ref_mid[127][i];
                            swOutLv0_ime_temp[j][i+192] = refWinY2_ref_mid[127][i + 32];
                            swOutLv0_ime_temp[j][i + 224] = refWinY3_ref_mid[127][i];
                        }
                    }

                    for (int j = 64; j < 96; j++)
                    {
            #pragma HLS pipeline 
                        for (int i = 0; i < 16; i++)
                        {
            #pragma HLS unroll
                            /**********************padding chroma*******************************/
                            refWinU_temp[j*128+i] = refWinU1_ref_mid[63][0];
                            refWinU_temp[j*128+i + 16] = refWinU1_ref_mid[63][0];
                            refWinU_temp[j*128+i + 32] = refWinU1_ref_mid[63][0];
                            refWinU_temp[j*128+i + 48] = refWinU1_ref_mid[63][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_ref_mid[63][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU2_ref_mid[63][i];
                            refWinU_temp[j*128+i + 96] = refWinU2_ref_mid[63][i + 16];
                            refWinU_temp[j*128+i + 112] = refWinU3_ref_mid[63][i];
                                        
                            refWinV_temp[j*128+i] = refWinV1_ref_mid[63][0];
                            refWinV_temp[j*128+i + 16] = refWinV1_ref_mid[63][0];
                            refWinV_temp[j*128+i + 32] = refWinV1_ref_mid[63][0];
                            refWinV_temp[j*128+i + 48] = refWinV1_ref_mid[63][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_ref_mid[63][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV2_ref_mid[63][i];
                            refWinV_temp[j*128+i + 96] = refWinV2_ref_mid[63][i + 16];
                            refWinV_temp[j*128+i + 112] = refWinV3_ref_mid[63][i];
                        }
                    }
                }
                else if ((pix_x + 128) == luma_width_padded)
                {
                    for (int j = 0; j < 128; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 32; i++)
                        {
    #pragma HLS unroll
                            /**********************padding luma*******************************/
                            // swOutLv0_temp[j][i] = refWinY1_ref_mid[j][0];
                            // swOutLv0_temp[j][i + 32] = refWinY1_ref_mid[j][0];
                            // swOutLv0_temp[j][i+64] = refWinY1_ref_mid[j][0];
                            // swOutLv0_temp[j][i + 96] = refWinY1_ref_mid[j][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 160] = refWinY2_ref_mid[j][i];
                            // swOutLv0_temp[j][i+192] = refWinY2_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 224] = refWinY2_ref_mid[j][63];

                            swOutLv0_ime_temp[j][i] = refWinY1_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i + 32] = refWinY1_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i+64] = refWinY1_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i + 96] = refWinY1_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 160] = refWinY2_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+192] = refWinY2_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 224] = refWinY2_ref_mid[j][63];
                        }
                    }

                    for (int j = 0; j < 64; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 16; i++)
                        {
    #pragma HLS unroll
                            /**********************padding chroma*******************************/
                            refWinU_temp[j*128+i] = refWinU1_ref_mid[j][0];
                            refWinU_temp[j*128+i + 16] = refWinU1_ref_mid[j][0];
                            refWinU_temp[j*128+i + 32] = refWinU1_ref_mid[j][0];
                            refWinU_temp[j*128+i + 48] = refWinU1_ref_mid[j][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU2_ref_mid[j][i];
                            refWinU_temp[j*128+i + 96] = refWinU2_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 112] = refWinU2_ref_mid[j][31];
                                        
                            refWinV_temp[j*128+i] = refWinV1_ref_mid[j][0];
                            refWinV_temp[j*128+i + 16] = refWinV1_ref_mid[j][0];
                            refWinV_temp[j*128+i + 32] = refWinV1_ref_mid[j][0];
                            refWinV_temp[j*128+i + 48] = refWinV1_ref_mid[j][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV2_ref_mid[j][i];
                            refWinV_temp[j*128+i + 96] = refWinV2_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 112] = refWinV2_ref_mid[j][31];
                        }
                    }

                    for (int j = 128; j < 192; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 32; i++)
                        {
    #pragma HLS unroll
                            /**********************padding luma*******************************/
                            // swOutLv0_temp[j][i] = refWinY1_ref_mid[127][0];
                            // swOutLv0_temp[j][i + 32] = refWinY1_ref_mid[127][0];
                            // swOutLv0_temp[j][i+64] = refWinY1_ref_mid[127][0];
                            // swOutLv0_temp[j][i + 96] = refWinY1_ref_mid[127][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_ref_mid[127][i + 32];
                            // swOutLv0_temp[j][i + 160] = refWinY2_ref_mid[127][i];
                            // swOutLv0_temp[j][i+192] = refWinY2_ref_mid[127][i + 32];
                            // swOutLv0_temp[j][i + 224] = refWinY2_ref_mid[127][63];

                            swOutLv0_ime_temp[j][i] = refWinY1_ref_mid[127][0];
                            swOutLv0_ime_temp[j][i + 32] = refWinY1_ref_mid[127][0];
                            swOutLv0_ime_temp[j][i+64] = refWinY1_ref_mid[127][0];
                            swOutLv0_ime_temp[j][i + 96] = refWinY1_ref_mid[127][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_ref_mid[127][i + 32];
                            swOutLv0_ime_temp[j][i + 160] = refWinY2_ref_mid[127][i];
                            swOutLv0_ime_temp[j][i+192] = refWinY2_ref_mid[127][i + 32];
                            swOutLv0_ime_temp[j][i + 224] = refWinY2_ref_mid[127][63];
                        }
                    }

                    for (int j = 64; j < 96; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 16; i++)
                        {
    #pragma HLS unroll
                            /**********************padding chroma*******************************/
                            refWinU_temp[j*128+i] = refWinU1_ref_mid[63][0];
                            refWinU_temp[j*128+i + 16] = refWinU1_ref_mid[63][0];
                            refWinU_temp[j*128+i + 32] = refWinU1_ref_mid[63][0];
                            refWinU_temp[j*128+i + 48] = refWinU1_ref_mid[63][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_ref_mid[63][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU2_ref_mid[63][i];
                            refWinU_temp[j*128+i + 96] = refWinU2_ref_mid[63][i + 16];
                            refWinU_temp[j*128+i + 112] = refWinU2_ref_mid[63][31];
                                        
                            refWinV_temp[j*128+i] = refWinV1_ref_mid[63][0];
                            refWinV_temp[j*128+i + 16] = refWinV1_ref_mid[63][0];
                            refWinV_temp[j*128+i + 32] = refWinV1_ref_mid[63][0];
                            refWinV_temp[j*128+i + 48] = refWinV1_ref_mid[63][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_ref_mid[63][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV2_ref_mid[63][i];
                            refWinV_temp[j*128+i + 96] = refWinV2_ref_mid[63][i + 16];
                            refWinV_temp[j*128+i + 112] = refWinV2_ref_mid[63][31];
                        }
                    }
                }
                else if ((pix_x + 128) == (luma_width_padded + 64))
                {
                    for (int j = 0; j < 128; j++)
                    {
        #pragma HLS pipeline 
                        for (int i = 0; i < 32; i++)
                        {
        #pragma HLS unroll
                            /**********************padding luma*******************************/
                            // swOutLv0_temp[j][i] = refWinY1_ref_mid[j][0];
                            // swOutLv0_temp[j][i + 32] = refWinY1_ref_mid[j][0];
                            // swOutLv0_temp[j][i+64] = refWinY1_ref_mid[j][0];
                            // swOutLv0_temp[j][i + 96] = refWinY1_ref_mid[j][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 160] = refWinY1_ref_mid[j][63];
                            // swOutLv0_temp[j][i+192] = refWinY1_ref_mid[j][63];
                            // swOutLv0_temp[j][i + 224] = refWinY1_ref_mid[j][63];

                            swOutLv0_ime_temp[j][i] = refWinY1_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i + 32] = refWinY1_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i+64] = refWinY1_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i + 96] = refWinY1_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 160] = refWinY1_ref_mid[j][63];
                            swOutLv0_ime_temp[j][i+192] = refWinY1_ref_mid[j][63];
                            swOutLv0_ime_temp[j][i + 224] = refWinY1_ref_mid[j][63];
                        }
                    }

                    for (int j = 0; j < 64; j++)
                    {
        #pragma HLS pipeline 
                        for (int i = 0; i < 16; i++)
                        {
        #pragma HLS unroll
                            /**********************padding chroma*******************************/
                            refWinU_temp[j*128+i] = refWinU1_ref_mid[j][0];
                            refWinU_temp[j*128+i + 16] = refWinU1_ref_mid[j][0];
                            refWinU_temp[j*128+i + 32] = refWinU1_ref_mid[j][0];
                            refWinU_temp[j*128+i + 48] = refWinU1_ref_mid[j][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU1_ref_mid[j][31];
                            refWinU_temp[j*128+i + 96] = refWinU1_ref_mid[j][31];
                            refWinU_temp[j*128+i + 112] = refWinU1_ref_mid[j][31];
                                        
                            refWinV_temp[j*128+i] = refWinV1_ref_mid[j][0];
                            refWinV_temp[j*128+i + 16] = refWinV1_ref_mid[j][0];
                            refWinV_temp[j*128+i + 32] = refWinV1_ref_mid[j][0];
                            refWinV_temp[j*128+i + 48] = refWinV1_ref_mid[j][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV1_ref_mid[j][31];
                            refWinV_temp[j*128+i + 96] = refWinV1_ref_mid[j][31];
                            refWinV_temp[j*128+i + 112] = refWinV1_ref_mid[j][31];
                        }
                    }

                    for (int j = 128; j < 192; j++)
                    {
        #pragma HLS pipeline 
                        for (int i = 0; i < 32; i++)
                        {
        #pragma HLS unroll
                            /**********************padding luma*******************************/
                            // swOutLv0_temp[j][i] = refWinY1_ref_mid[127][0];
                            // swOutLv0_temp[j][i + 32] = refWinY1_ref_mid[127][0];
                            // swOutLv0_temp[j][i+64] = refWinY1_ref_mid[127][0];
                            // swOutLv0_temp[j][i + 96] = refWinY1_ref_mid[127][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_ref_mid[127][i + 32];
                            // swOutLv0_temp[j][i + 160] = refWinY1_ref_mid[127][63];
                            // swOutLv0_temp[j][i+192] = refWinY1_ref_mid[127][63];
                            // swOutLv0_temp[j][i + 224] = refWinY1_ref_mid[127][63];

                            swOutLv0_ime_temp[j][i] = refWinY1_ref_mid[127][0];
                            swOutLv0_ime_temp[j][i + 32] = refWinY1_ref_mid[127][0];
                            swOutLv0_ime_temp[j][i+64] = refWinY1_ref_mid[127][0];
                            swOutLv0_ime_temp[j][i + 96] = refWinY1_ref_mid[127][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_ref_mid[127][i + 32];
                            swOutLv0_ime_temp[j][i + 160] = refWinY1_ref_mid[127][63];
                            swOutLv0_ime_temp[j][i+192] = refWinY1_ref_mid[127][63];
                            swOutLv0_ime_temp[j][i + 224] = refWinY1_ref_mid[127][63];
                        }
                    }

                    for (int j = 64; j < 96; j++)
                    {
        #pragma HLS pipeline 
                        for (int i = 0; i < 16; i++)
                        {
        #pragma HLS unroll
                            /**********************padding chroma*******************************/
                            refWinU_temp[j*128+i] = refWinU1_ref_mid[63][0];
                            refWinU_temp[j*128+i + 16] = refWinU1_ref_mid[63][0];
                            refWinU_temp[j*128+i + 32] = refWinU1_ref_mid[63][0];
                            refWinU_temp[j*128+i + 48] = refWinU1_ref_mid[63][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_ref_mid[63][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU1_ref_mid[63][31];
                            refWinU_temp[j*128+i + 96] = refWinU1_ref_mid[63][31];
                            refWinU_temp[j*128+i + 112] = refWinU1_ref_mid[63][31];
                                        
                            refWinV_temp[j*128+i] = refWinV1_ref_mid[63][0];
                            refWinV_temp[j*128+i + 16] = refWinV1_ref_mid[63][0];
                            refWinV_temp[j*128+i + 32] = refWinV1_ref_mid[63][0];
                            refWinV_temp[j*128+i + 48] = refWinV1_ref_mid[63][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_ref_mid[63][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV1_ref_mid[63][31];
                            refWinV_temp[j*128+i + 96] = refWinV1_ref_mid[63][31];
                            refWinV_temp[j*128+i + 112] = refWinV1_ref_mid[63][31];
                        }
                    }
                }
            }
            else if (64 == pix_x)
            {
                if ((pix_x + 128) <= (luma_width_padded - 64))
                {
                    for (int j = 0; j < 128; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 32; i++)
                        {
    #pragma HLS unroll
                            /*********************padding luma********************/
                            // swOutLv0_temp[j][i] = refWinY0_ref_mid[j][0];
                            // swOutLv0_temp[j][i + 32] = refWinY0_ref_mid[j][i];
                            // swOutLv0_temp[j][i+64] = refWinY0_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 96] = refWinY1_ref_mid[j][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 160] = refWinY2_ref_mid[j][i];
                            // swOutLv0_temp[j][i+192] = refWinY2_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 224] = refWinY3_ref_mid[j][i];

                            swOutLv0_ime_temp[j][i] = refWinY0_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i + 32] = refWinY0_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+64] = refWinY0_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 96] = refWinY1_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 160] = refWinY2_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+192] = refWinY2_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 224] = refWinY3_ref_mid[j][i];
                        }
                    }

                    for (int j = 0; j < 64; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 16; i++)
                        {
    #pragma HLS unroll
                            /*********************padding chroma********************/
                            refWinU_temp[j*128+i] = refWinU0_ref_mid[j][0];
                            refWinU_temp[j*128+i + 16] = refWinU0_ref_mid[j][i];
                            refWinU_temp[j*128+i + 32] = refWinU0_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 48] = refWinU1_ref_mid[j][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU2_ref_mid[j][i];
                            refWinU_temp[j*128+i + 96] = refWinU2_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 112] = refWinU3_ref_mid[j][i];
                                        
                            refWinV_temp[j*128+i] = refWinV0_ref_mid[j][0];
                            refWinV_temp[j*128+i + 16] = refWinV0_ref_mid[j][i];
                            refWinV_temp[j*128+i + 32] = refWinV0_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 48] = refWinV1_ref_mid[j][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV2_ref_mid[j][i];
                            refWinV_temp[j*128+i + 96] = refWinV2_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 112] = refWinV3_ref_mid[j][i];
                        }
                    }

                    for (int j = 128; j < 192; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 32; i++)
                        {
    #pragma HLS unroll
                            /*********************padding luma********************/
                            // swOutLv0_temp[j][i] = refWinY0_ref_mid[127][0];
                            // swOutLv0_temp[j][i + 32] = refWinY0_ref_mid[127][i];
                            // swOutLv0_temp[j][i+64] = refWinY0_ref_mid[127][i + 32];
                            // swOutLv0_temp[j][i + 96] = refWinY1_ref_mid[127][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_ref_mid[127][i + 32];
                            // swOutLv0_temp[j][i + 160] = refWinY2_ref_mid[127][i];
                            // swOutLv0_temp[j][i+192] = refWinY2_ref_mid[127][i + 32];
                            // swOutLv0_temp[j][i + 224] = refWinY3_ref_mid[127][i];

                            swOutLv0_ime_temp[j][i] = refWinY0_ref_mid[127][0];
                            swOutLv0_ime_temp[j][i + 32] = refWinY0_ref_mid[127][i];
                            swOutLv0_ime_temp[j][i+64] = refWinY0_ref_mid[127][i + 32];
                            swOutLv0_ime_temp[j][i + 96] = refWinY1_ref_mid[127][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_ref_mid[127][i + 32];
                            swOutLv0_ime_temp[j][i + 160] = refWinY2_ref_mid[127][i];
                            swOutLv0_ime_temp[j][i+192] = refWinY2_ref_mid[127][i + 32];
                            swOutLv0_ime_temp[j][i + 224] = refWinY3_ref_mid[127][i];
                        }
                    }

                    for (int j = 64; j < 96; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 16; i++)
                        {
    #pragma HLS unroll
                            /*********************padding chroma********************/
                            refWinU_temp[j*128+i] = refWinU0_ref_mid[63][0];
                            refWinU_temp[j*128+i + 16] = refWinU0_ref_mid[63][i];
                            refWinU_temp[j*128+i + 32] = refWinU0_ref_mid[63][i + 16];
                            refWinU_temp[j*128+i + 48] = refWinU1_ref_mid[63][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_ref_mid[63][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU2_ref_mid[63][i];
                            refWinU_temp[j*128+i + 96] = refWinU2_ref_mid[63][i + 16];
                            refWinU_temp[j*128+i + 112] = refWinU3_ref_mid[63][i];
                                        
                            refWinV_temp[j*128+i] = refWinV0_ref_mid[63][0];
                            refWinV_temp[j*128+i + 16] = refWinV0_ref_mid[63][i];
                            refWinV_temp[j*128+i + 32] = refWinV0_ref_mid[63][i + 16];
                            refWinV_temp[j*128+i + 48] = refWinV1_ref_mid[63][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_ref_mid[63][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV2_ref_mid[63][i];
                            refWinV_temp[j*128+i + 96] = refWinV2_ref_mid[63][i + 16];
                            refWinV_temp[j*128+i + 112] = refWinV3_ref_mid[63][i];
                        }
                    }
                }
                else if ((pix_x + 128) == luma_width_padded)
                {
                    for (int j = 0; j < 128; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 32; i++)
                        {
    #pragma HLS unroll
                            /*********************padding luma********************/
                            // swOutLv0_temp[j][i] = refWinY0_ref_mid[j][0];
                            // swOutLv0_temp[j][i + 32] = refWinY0_ref_mid[j][i];
                            // swOutLv0_temp[j][i+64] = refWinY0_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 96] = refWinY1_ref_mid[j][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 160] = refWinY2_ref_mid[j][i];
                            // swOutLv0_temp[j][i+192] = refWinY2_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 224] = refWinY2_ref_mid[j][63];

                            swOutLv0_ime_temp[j][i] = refWinY0_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i + 32] = refWinY0_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+64] = refWinY0_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 96] = refWinY1_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 160] = refWinY2_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+192] = refWinY2_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 224] = refWinY2_ref_mid[j][63];
                        }
                    }

                    for (int j = 0; j < 64; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 16; i++)
                        {
    #pragma HLS unroll
                            /*********************padding chroma********************/
                            refWinU_temp[j*128+i] = refWinU0_ref_mid[j][0];
                            refWinU_temp[j*128+i + 16] = refWinU0_ref_mid[j][i];
                            refWinU_temp[j*128+i + 32] = refWinU0_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 48] = refWinU1_ref_mid[j][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU2_ref_mid[j][i];
                            refWinU_temp[j*128+i + 96] = refWinU2_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 112] = refWinU2_ref_mid[j][31];
                                        
                            refWinV_temp[j*128+i] = refWinV0_ref_mid[j][0];
                            refWinV_temp[j*128+i + 16] = refWinV0_ref_mid[j][i];
                            refWinV_temp[j*128+i + 32] = refWinV0_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 48] = refWinV1_ref_mid[j][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV2_ref_mid[j][i];
                            refWinV_temp[j*128+i + 96] = refWinV2_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 112] = refWinV2_ref_mid[j][31];
                        }
                    }

                    for (int j = 128; j < 192; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 32; i++)
                        {
    #pragma HLS unroll
                            /*********************padding luma********************/
                            // swOutLv0_temp[j][i] = refWinY0_ref_mid[127][0];
                            // swOutLv0_temp[j][i + 32] = refWinY0_ref_mid[127][i];
                            // swOutLv0_temp[j][i+64] = refWinY0_ref_mid[127][i + 32];
                            // swOutLv0_temp[j][i + 96] = refWinY1_ref_mid[127][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_ref_mid[127][i + 32];
                            // swOutLv0_temp[j][i + 160] = refWinY2_ref_mid[127][i];
                            // swOutLv0_temp[j][i+192] = refWinY2_ref_mid[127][i + 32];
                            // swOutLv0_temp[j][i + 224] = refWinY2_ref_mid[127][63];

                            swOutLv0_ime_temp[j][i] = refWinY0_ref_mid[127][0];
                            swOutLv0_ime_temp[j][i + 32] = refWinY0_ref_mid[127][i];
                            swOutLv0_ime_temp[j][i+64] = refWinY0_ref_mid[127][i + 32];
                            swOutLv0_ime_temp[j][i + 96] = refWinY1_ref_mid[127][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_ref_mid[127][i + 32];
                            swOutLv0_ime_temp[j][i + 160] = refWinY2_ref_mid[127][i];
                            swOutLv0_ime_temp[j][i+192] = refWinY2_ref_mid[127][i + 32];
                            swOutLv0_ime_temp[j][i + 224] = refWinY2_ref_mid[127][63];
                        }
                    }

                    for (int j = 64; j < 96; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 16; i++)
                        {
    #pragma HLS unroll
                            /*********************padding chroma********************/
                            refWinU_temp[j*128+i] = refWinU0_ref_mid[63][0];
                            refWinU_temp[j*128+i + 16] = refWinU0_ref_mid[63][i];
                            refWinU_temp[j*128+i + 32] = refWinU0_ref_mid[63][i + 16];
                            refWinU_temp[j*128+i + 48] = refWinU1_ref_mid[63][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_ref_mid[63][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU2_ref_mid[63][i];
                            refWinU_temp[j*128+i + 96] = refWinU2_ref_mid[63][i + 16];
                            refWinU_temp[j*128+i + 112] = refWinU2_ref_mid[63][31];
                                        
                            refWinV_temp[j*128+i] = refWinV0_ref_mid[63][0];
                            refWinV_temp[j*128+i + 16] = refWinV0_ref_mid[63][i];
                            refWinV_temp[j*128+i + 32] = refWinV0_ref_mid[63][i + 16];
                            refWinV_temp[j*128+i + 48] = refWinV1_ref_mid[63][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_ref_mid[63][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV2_ref_mid[63][i];
                            refWinV_temp[j*128+i + 96] = refWinV2_ref_mid[63][i + 16];
                            refWinV_temp[j*128+i + 112] = refWinV2_ref_mid[63][31];
                        }
                    }
                }
                else if ((pix_x + 128) == (luma_width_padded+64))
                {
                    for (int j = 0; j < 128; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 32; i++)
                        {
    #pragma HLS unroll
                            /*********************padding luma********************/
                            // swOutLv0_temp[j][i] = refWinY0_ref_mid[j][0];
                            // swOutLv0_temp[j][i + 32] = refWinY0_ref_mid[j][i];
                            // swOutLv0_temp[j][i+64] = refWinY0_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 96] = refWinY1_ref_mid[j][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 160] = refWinY1_ref_mid[j][63];
                            // swOutLv0_temp[j][i+192] = refWinY1_ref_mid[j][63];
                            // swOutLv0_temp[j][i + 224] = refWinY1_ref_mid[j][63];

                            swOutLv0_ime_temp[j][i] = refWinY0_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i + 32] = refWinY0_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+64] = refWinY0_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 96] = refWinY1_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 160] = refWinY1_ref_mid[j][63];
                            swOutLv0_ime_temp[j][i+192] = refWinY1_ref_mid[j][63];
                            swOutLv0_ime_temp[j][i + 224] = refWinY1_ref_mid[j][63];
                        }
                    }

                    for (int j = 0; j < 64; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 16; i++)
                        {
    #pragma HLS unroll
                            /*********************padding chroma********************/
                            refWinU_temp[j*128+i] = refWinU0_ref_mid[j][0];
                            refWinU_temp[j*128+i + 16] = refWinU0_ref_mid[j][i];
                            refWinU_temp[j*128+i + 32] = refWinU0_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 48] = refWinU1_ref_mid[j][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU1_ref_mid[j][31];
                            refWinU_temp[j*128+i + 96] = refWinU1_ref_mid[j][31];
                            refWinU_temp[j*128+i + 112] = refWinU1_ref_mid[j][31];
                                        
                            refWinV_temp[j*128+i] = refWinV0_ref_mid[j][0];
                            refWinV_temp[j*128+i + 16] = refWinV0_ref_mid[j][i];
                            refWinV_temp[j*128+i + 32] = refWinV0_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 48] = refWinV1_ref_mid[j][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV1_ref_mid[j][31];
                            refWinV_temp[j*128+i + 96] = refWinV1_ref_mid[j][31];
                            refWinV_temp[j*128+i + 112] = refWinV1_ref_mid[j][31];
                        }
                    }

                    for (int j = 128; j < 192; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 32; i++)
                        {
    #pragma HLS unroll
                            /*********************padding luma********************/
                            // swOutLv0_temp[j][i] = refWinY0_ref_mid[127][0];
                            // swOutLv0_temp[j][i + 32] = refWinY0_ref_mid[127][i];
                            // swOutLv0_temp[j][i+64] = refWinY0_ref_mid[127][i + 32];
                            // swOutLv0_temp[j][i + 96] = refWinY1_ref_mid[127][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_ref_mid[127][i + 32];
                            // swOutLv0_temp[j][i + 160] = refWinY1_ref_mid[127][63];
                            // swOutLv0_temp[j][i+192] = refWinY1_ref_mid[127][63];
                            // swOutLv0_temp[j][i + 224] = refWinY1_ref_mid[127][63];

                            swOutLv0_ime_temp[j][i] = refWinY0_ref_mid[127][0];
                            swOutLv0_ime_temp[j][i + 32] = refWinY0_ref_mid[127][i];
                            swOutLv0_ime_temp[j][i+64] = refWinY0_ref_mid[127][i + 32];
                            swOutLv0_ime_temp[j][i + 96] = refWinY1_ref_mid[127][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_ref_mid[127][i + 32];
                            swOutLv0_ime_temp[j][i + 160] = refWinY1_ref_mid[127][63];
                            swOutLv0_ime_temp[j][i+192] = refWinY1_ref_mid[127][63];
                            swOutLv0_ime_temp[j][i + 224] = refWinY1_ref_mid[127][63];
                        }
                    }

                    for (int j = 64; j < 96; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 16; i++)
                        {
    #pragma HLS unroll
                            /*********************padding chroma********************/
                            refWinU_temp[j*128+i] = refWinU0_ref_mid[63][0];
                            refWinU_temp[j*128+i + 16] = refWinU0_ref_mid[63][i];
                            refWinU_temp[j*128+i + 32] = refWinU0_ref_mid[63][i + 16];
                            refWinU_temp[j*128+i + 48] = refWinU1_ref_mid[63][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_ref_mid[63][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU1_ref_mid[63][31];
                            refWinU_temp[j*128+i + 96] = refWinU1_ref_mid[63][31];
                            refWinU_temp[j*128+i + 112] = refWinU1_ref_mid[63][31];
                                        
                            refWinV_temp[j*128+i] = refWinV0_ref_mid[63][0];
                            refWinV_temp[j*128+i + 16] = refWinV0_ref_mid[63][i];
                            refWinV_temp[j*128+i + 32] = refWinV0_ref_mid[63][i + 16];
                            refWinV_temp[j*128+i + 48] = refWinV1_ref_mid[63][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_ref_mid[63][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV1_ref_mid[63][31];
                            refWinV_temp[j*128+i + 96] = refWinV1_ref_mid[63][31];
                            refWinV_temp[j*128+i + 112] = refWinV1_ref_mid[63][31];
                        }
                    }
                }
            }
            else
            {
                if ((pix_x + 128) <= (luma_width_padded - 64))
                {
                    for (int j = 0; j < 128; j++)
                    {
        #pragma HLS pipeline 
                        for (int i = 0; i < 32; i++)
                        {
        #pragma HLS unroll
                            /*****************************padding luma******************************************/
                            // swOutLv0_temp[j][i] = refWinY32x192_ref_mid[j][i];
                            // swOutLv0_temp[j][i + 32] = refWinY0_ref_mid[j][i];
                            // swOutLv0_temp[j][i+64] = refWinY0_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 96] = refWinY1_ref_mid[j][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 160] = refWinY2_ref_mid[j][i];
                            // swOutLv0_temp[j][i+192] = refWinY2_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 224] = refWinY3_ref_mid[j][i];

                            swOutLv0_ime_temp[j][i] = refWinY32x192_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i + 32] = refWinY0_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+64] = refWinY0_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 96] = refWinY1_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 160] = refWinY2_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+192] = refWinY2_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 224] = refWinY3_ref_mid[j][i];
                        }
                    }

                    for (int j = 0; j < 64; j++)
                    {
        #pragma HLS pipeline 
                        for (int i = 0; i < 16; i++)
                        {
        #pragma HLS unroll
                            /*****************************padding chroma******************************************/
                                refWinU_temp[j*128+i] = refWinU16x96_ref_mid[j][i];
                                refWinU_temp[j*128+i + 16] = refWinU0_ref_mid[j][i];
                                refWinU_temp[j*128+i + 32] = refWinU0_ref_mid[j][i + 16];
                                refWinU_temp[j*128+i + 48] = refWinU1_ref_mid[j][i];
                                refWinU_temp[j*128+i + 64] = refWinU1_ref_mid[j][i + 16];
                                refWinU_temp[j*128+i + 80] = refWinU2_ref_mid[j][i];
                                refWinU_temp[j*128+i + 96] = refWinU2_ref_mid[j][i + 16];
                                refWinU_temp[j*128+i + 112] = refWinU3_ref_mid[j][i];
                                            
                                refWinV_temp[j*128+i] = refWinV16x96_ref_mid[j][i];
                                refWinV_temp[j*128+i + 16] = refWinV0_ref_mid[j][i];
                                refWinV_temp[j*128+i + 32] = refWinV0_ref_mid[j][i + 16];
                                refWinV_temp[j*128+i + 48] = refWinV1_ref_mid[j][i];
                                refWinV_temp[j*128+i + 64] = refWinV1_ref_mid[j][i + 16];
                                refWinV_temp[j*128+i + 80] = refWinV2_ref_mid[j][i];
                                refWinV_temp[j*128+i + 96] = refWinV2_ref_mid[j][i + 16];
                                refWinV_temp[j*128+i + 112] = refWinV3_ref_mid[j][i];
                        }
                    }

                    for (int j = 128; j < 192; j++)
                    {
        #pragma HLS pipeline 
                        for (int i = 0; i < 32; i++)
                        {
        #pragma HLS unroll
                            /*****************************padding luma*********************************/
                            // swOutLv0_temp[j][i] = refWinY32x192_ref_mid[127][i];
                            // swOutLv0_temp[j][i + 32] = refWinY0_ref_mid[127][i];
                            // swOutLv0_temp[j][i+64] = refWinY0_ref_mid[127][i + 32];
                            // swOutLv0_temp[j][i + 96] = refWinY1_ref_mid[127][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_ref_mid[127][i + 32];
                            // swOutLv0_temp[j][i + 160] = refWinY2_ref_mid[127][i];
                            // swOutLv0_temp[j][i+192] = refWinY2_ref_mid[127][i + 32];
                            // swOutLv0_temp[j][i + 224] = refWinY3_ref_mid[127][i];

                            swOutLv0_ime_temp[j][i] = refWinY32x192_ref_mid[127][i];
                            swOutLv0_ime_temp[j][i + 32] = refWinY0_ref_mid[127][i];
                            swOutLv0_ime_temp[j][i+64] = refWinY0_ref_mid[127][i + 32];
                            swOutLv0_ime_temp[j][i + 96] = refWinY1_ref_mid[127][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_ref_mid[127][i + 32];
                            swOutLv0_ime_temp[j][i + 160] = refWinY2_ref_mid[127][i];
                            swOutLv0_ime_temp[j][i+192] = refWinY2_ref_mid[127][i + 32];
                            swOutLv0_ime_temp[j][i + 224] = refWinY3_ref_mid[127][i];
                        }
                    }

                    for (int j = 64; j < 96; j++)
                    {
        #pragma HLS pipeline 
                        for (int i = 0; i < 16; i++)
                        {
        #pragma HLS unroll
                            /*****************************padding chroma******************************************/
                            refWinU_temp[j*128+i] = refWinU16x96_ref_mid[63][i];
                            refWinU_temp[j*128+i + 16] = refWinU0_ref_mid[63][i];
                            refWinU_temp[j*128+i + 32] = refWinU0_ref_mid[63][i + 16];
                            refWinU_temp[j*128+i + 48] = refWinU1_ref_mid[63][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_ref_mid[63][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU2_ref_mid[63][i];
                            refWinU_temp[j*128+i + 96] = refWinU2_ref_mid[63][i + 16];
                            refWinU_temp[j*128+i + 112] = refWinU3_ref_mid[63][i];
                                        
                            refWinV_temp[j*128+i] = refWinV16x96_ref_mid[63][i];
                            refWinV_temp[j*128+i + 16] = refWinV0_ref_mid[63][i];
                            refWinV_temp[j*128+i + 32] = refWinV0_ref_mid[63][i + 16];
                            refWinV_temp[j*128+i + 48] = refWinV1_ref_mid[63][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_ref_mid[63][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV2_ref_mid[63][i];
                            refWinV_temp[j*128+i + 96] = refWinV2_ref_mid[63][i + 16];
                            refWinV_temp[j*128+i + 112] = refWinV3_ref_mid[63][i];
                        }
                    }
                }
                else if ((pix_x + 128) == luma_width_padded)
                {
                    for (int j = 0; j < 128; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 32; i++)
                        {
    #pragma HLS unroll
                            /*****************************padding luma******************************************/
                            // swOutLv0_temp[j][i] = refWinY32x192_ref_mid[j][i];
                            // swOutLv0_temp[j][i + 32] = refWinY0_ref_mid[j][i];
                            // swOutLv0_temp[j][i+64] = refWinY0_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 96] = refWinY1_ref_mid[j][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 160] = refWinY2_ref_mid[j][i];
                            // swOutLv0_temp[j][i+192] = refWinY2_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 224] = refWinY2_ref_mid[j][63];

                            swOutLv0_ime_temp[j][i] = refWinY32x192_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i + 32] = refWinY0_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+64] = refWinY0_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 96] = refWinY1_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 160] = refWinY2_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+192] = refWinY2_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 224] = refWinY2_ref_mid[j][63];
                        }
                    }

                    for (int j = 0; j < 64; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 16; i++)
                        {
    #pragma HLS unroll
                            /*****************************padding chroma******************************************/
                            refWinU_temp[j*128+i] = refWinU16x96_ref_mid[j][i];
                            refWinU_temp[j*128+i + 16] = refWinU0_ref_mid[j][i];
                            refWinU_temp[j*128+i + 32] = refWinU0_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 48] = refWinU1_ref_mid[j][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU2_ref_mid[j][i];
                            refWinU_temp[j*128+i + 96] = refWinU2_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 112] = refWinU2_ref_mid[j][31];
                                        
                            refWinV_temp[j*128+i] = refWinV16x96_ref_mid[j][i];
                            refWinV_temp[j*128+i + 16] = refWinV0_ref_mid[j][i];
                            refWinV_temp[j*128+i + 32] = refWinV0_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 48] = refWinV1_ref_mid[j][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV2_ref_mid[j][i];
                            refWinV_temp[j*128+i + 96] = refWinV2_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 112] = refWinV2_ref_mid[j][31];
                        }
                    }

                    for (int j = 128; j < 192; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 32; i++)
                        {
    #pragma HLS unroll
                            /*****************************padding luma*********************************/
                            // swOutLv0_temp[j][i] = refWinY32x192_ref_mid[127][i];
                            // swOutLv0_temp[j][i + 32] = refWinY0_ref_mid[127][i];
                            // swOutLv0_temp[j][i+64] = refWinY0_ref_mid[127][i + 32];
                            // swOutLv0_temp[j][i + 96] = refWinY1_ref_mid[127][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_ref_mid[127][i + 32];
                            // swOutLv0_temp[j][i + 160] = refWinY2_ref_mid[127][i];
                            // swOutLv0_temp[j][i+192] = refWinY2_ref_mid[127][i + 32];
                            // swOutLv0_temp[j][i + 224] = refWinY2_ref_mid[127][63];

                            swOutLv0_ime_temp[j][i] = refWinY32x192_ref_mid[127][i];
                            swOutLv0_ime_temp[j][i + 32] = refWinY0_ref_mid[127][i];
                            swOutLv0_ime_temp[j][i+64] = refWinY0_ref_mid[127][i + 32];
                            swOutLv0_ime_temp[j][i + 96] = refWinY1_ref_mid[127][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_ref_mid[127][i + 32];
                            swOutLv0_ime_temp[j][i + 160] = refWinY2_ref_mid[127][i];
                            swOutLv0_ime_temp[j][i+192] = refWinY2_ref_mid[127][i + 32];
                            swOutLv0_ime_temp[j][i + 224] = refWinY2_ref_mid[127][63];
                        }
                    }

                    for (int j = 64; j < 96; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 16; i++)
                        {
    #pragma HLS unroll
                            /*****************************padding chroma******************************************/
                            refWinU_temp[j*128+i] = refWinU16x96_ref_mid[63][i];
                            refWinU_temp[j*128+i + 16] = refWinU0_ref_mid[63][i];
                            refWinU_temp[j*128+i + 32] = refWinU0_ref_mid[63][i + 16];
                            refWinU_temp[j*128+i + 48] = refWinU1_ref_mid[63][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_ref_mid[63][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU2_ref_mid[63][i];
                            refWinU_temp[j*128+i + 96] = refWinU2_ref_mid[63][i + 16];
                            refWinU_temp[j*128+i + 112] = refWinU2_ref_mid[63][31];
                                        
                            refWinV_temp[j*128+i] = refWinV16x96_ref_mid[63][i];
                            refWinV_temp[j*128+i + 16] = refWinV0_ref_mid[63][i];
                            refWinV_temp[j*128+i + 32] = refWinV0_ref_mid[63][i + 16];
                            refWinV_temp[j*128+i + 48] = refWinV1_ref_mid[63][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_ref_mid[63][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV2_ref_mid[63][i];
                            refWinV_temp[j*128+i + 96] = refWinV2_ref_mid[63][i + 16];
                            refWinV_temp[j*128+i + 112] = refWinV2_ref_mid[63][31];
                        }
                    }
                }
                else if ((pix_x + 128) == (luma_width_padded+64))
                {
                    for (int j = 0; j < 128; j++)
                    {
        #pragma HLS pipeline 
                        for (int i = 0; i < 32; i++)
                        {
        #pragma HLS unroll
                            /*****************************padding luma******************************************/
                            // swOutLv0_temp[j][i] = refWinY32x192_ref_mid[j][i];
                            // swOutLv0_temp[j][i + 32] = refWinY0_ref_mid[j][i];
                            // swOutLv0_temp[j][i+64] = refWinY0_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 96] = refWinY1_ref_mid[j][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 160] = refWinY1_ref_mid[j][63];
                            // swOutLv0_temp[j][i+192] = refWinY1_ref_mid[j][63];
                            // swOutLv0_temp[j][i + 224] = refWinY1_ref_mid[j][63];

                            swOutLv0_ime_temp[j][i] = refWinY32x192_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i + 32] = refWinY0_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+64] = refWinY0_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 96] = refWinY1_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 160] = refWinY1_ref_mid[j][63];
                            swOutLv0_ime_temp[j][i+192] = refWinY1_ref_mid[j][63];
                            swOutLv0_ime_temp[j][i + 224] = refWinY1_ref_mid[j][63];
                        }
                    }

                    for (int j = 0; j < 64; j++)
                    {
        #pragma HLS pipeline 
                        for (int i = 0; i < 16; i++)
                        {
        #pragma HLS unroll
                            /*****************************padding chroma******************************************/
                            refWinU_temp[j*128+i] = refWinU16x96_ref_mid[j][i];
                            refWinU_temp[j*128+i + 16] = refWinU0_ref_mid[j][i];
                            refWinU_temp[j*128+i + 32] = refWinU0_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 48] = refWinU1_ref_mid[j][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU1_ref_mid[j][31];
                            refWinU_temp[j*128+i + 96] = refWinU1_ref_mid[j][31];
                            refWinU_temp[j*128+i + 112] = refWinU1_ref_mid[j][31];
                                        
                            refWinV_temp[j*128+i] = refWinV16x96_ref_mid[j][i];
                            refWinV_temp[j*128+i + 16] = refWinV0_ref_mid[j][i];
                            refWinV_temp[j*128+i + 32] = refWinV0_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 48] = refWinV1_ref_mid[j][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV1_ref_mid[j][31];
                            refWinV_temp[j*128+i + 96] = refWinV1_ref_mid[j][31];
                            refWinV_temp[j*128+i + 112] = refWinV1_ref_mid[j][31];
                        }
                    }

                    for (int j = 128; j < 192; j++)
                    {
        #pragma HLS pipeline 
                        for (int i = 0; i < 32; i++)
                        {
        #pragma HLS unroll
                            /*****************************padding luma*********************************/
                            // swOutLv0_temp[j][i] = refWinY32x192_ref_mid[127][i];
                            // swOutLv0_temp[j][i + 32] = refWinY0_ref_mid[127][i];
                            // swOutLv0_temp[j][i+64] = refWinY0_ref_mid[127][i + 32];
                            // swOutLv0_temp[j][i + 96] = refWinY1_ref_mid[127][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_ref_mid[127][i + 32];
                            // swOutLv0_temp[j][i + 160] = refWinY1_ref_mid[127][63];
                            // swOutLv0_temp[j][i+192] = refWinY1_ref_mid[127][63];
                            // swOutLv0_temp[j][i + 224] = refWinY1_ref_mid[127][63];

                            swOutLv0_ime_temp[j][i] = refWinY32x192_ref_mid[127][i];
                            swOutLv0_ime_temp[j][i + 32] = refWinY0_ref_mid[127][i];
                            swOutLv0_ime_temp[j][i+64] = refWinY0_ref_mid[127][i + 32];
                            swOutLv0_ime_temp[j][i + 96] = refWinY1_ref_mid[127][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_ref_mid[127][i + 32];
                            swOutLv0_ime_temp[j][i + 160] = refWinY1_ref_mid[127][63];
                            swOutLv0_ime_temp[j][i+192] = refWinY1_ref_mid[127][63];
                            swOutLv0_ime_temp[j][i + 224] = refWinY1_ref_mid[127][63];
                        }
                    }

                    for (int j = 64; j < 96; j++)
                    {
        #pragma HLS pipeline 
                        for (int i = 0; i < 16; i++)
                        {
        #pragma HLS unroll
                            /*****************************padding chroma******************************************/
                            refWinU_temp[j*128+i] = refWinU16x96_ref_mid[63][i];
                            refWinU_temp[j*128+i + 16] = refWinU0_ref_mid[63][i];
                            refWinU_temp[j*128+i + 32] = refWinU0_ref_mid[63][i + 16];
                            refWinU_temp[j*128+i + 48] = refWinU1_ref_mid[63][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_ref_mid[63][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU1_ref_mid[63][31];
                            refWinU_temp[j*128+i + 96] = refWinU1_ref_mid[63][31];
                            refWinU_temp[j*128+i + 112] = refWinU1_ref_mid[63][31];
                                        
                            refWinV_temp[j*128+i] = refWinV16x96_ref_mid[63][i];
                            refWinV_temp[j*128+i + 16] = refWinV0_ref_mid[63][i];
                            refWinV_temp[j*128+i + 32] = refWinV0_ref_mid[63][i + 16];
                            refWinV_temp[j*128+i + 48] = refWinV1_ref_mid[63][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_ref_mid[63][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV1_ref_mid[63][31];
                            refWinV_temp[j*128+i + 96] = refWinV1_ref_mid[63][31];
                            refWinV_temp[j*128+i + 112] = refWinV1_ref_mid[63][31];
                        }
                    }
                }
            }
        }
        //middle row
        else
        {
            if (0 == pix_x)
            {
                if ((pix_x + 128) <= (luma_width_padded - 64))
                {
                    for (int j = 0; j < 192; j++)
                    {
        #pragma HLS pipeline
                        for (int i = 0; i < 32; i++)
                        {
        #pragma HLS unroll
                            /**************************padding luma*********************************/
                            // swOutLv0_temp[j][i] = refWinY1_ref_mid[j][0];
                            // swOutLv0_temp[j][i + 32] = refWinY1_ref_mid[j][0];
                            // swOutLv0_temp[j][i+64] = refWinY1_ref_mid[j][0];
                            // swOutLv0_temp[j][i + 96] = refWinY1_ref_mid[j][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 160] = refWinY2_ref_mid[j][i];
                            // swOutLv0_temp[j][i+192] = refWinY2_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 224] = refWinY3_ref_mid[j][i];

                            swOutLv0_ime_temp[j][i] = refWinY1_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i + 32] = refWinY1_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i+64] = refWinY1_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i + 96] = refWinY1_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 160] = refWinY2_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+192] = refWinY2_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 224] = refWinY3_ref_mid[j][i];
                        }
                    }
                    for (int j = 0; j < 96; j++)
                    {
        #pragma HLS pipeline
                        for (int i = 0; i < 16; i++)
                        {
        #pragma HLS unroll
                            /**************************padding chroma*********************************/
                            refWinU_temp[j*128+i] = refWinU1_ref_mid[j][0];
                            refWinU_temp[j*128+i + 16] = refWinU1_ref_mid[j][0];
                            refWinU_temp[j*128+i + 32] = refWinU1_ref_mid[j][0];
                            refWinU_temp[j*128+i + 48] = refWinU1_ref_mid[j][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU2_ref_mid[j][i];
                            refWinU_temp[j*128+i + 96] = refWinU2_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 112] = refWinU3_ref_mid[j][i];
                                        
                            refWinV_temp[j*128+i] = refWinV1_ref_mid[j][0];
                            refWinV_temp[j*128+i + 16] = refWinV1_ref_mid[j][0];
                            refWinV_temp[j*128+i + 32] = refWinV1_ref_mid[j][0];
                            refWinV_temp[j*128+i + 48] = refWinV1_ref_mid[j][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV2_ref_mid[j][i];
                            refWinV_temp[j*128+i + 96] = refWinV2_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 112] = refWinV3_ref_mid[j][i];
                        }
                    }
                }
                else if ((pix_x + 128) == luma_width_padded)
                {
                    for (int j = 0; j < 192; j++)
                    {
    #pragma HLS pipeline
                        for (int i = 0; i < 32; i++)
                        {
    #pragma HLS unroll
                            /**************************padding luma*********************************/
                            // swOutLv0_temp[j][i] = refWinY1_ref_mid[j][0];
                            // swOutLv0_temp[j][i + 32] = refWinY1_ref_mid[j][0];
                            // swOutLv0_temp[j][i+64] = refWinY1_ref_mid[j][0];
                            // swOutLv0_temp[j][i + 96] = refWinY1_ref_mid[j][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 160] = refWinY2_ref_mid[j][i];
                            // swOutLv0_temp[j][i+192] = refWinY2_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 224] = refWinY2_ref_mid[j][63];

                            swOutLv0_ime_temp[j][i] = refWinY1_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i + 32] = refWinY1_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i+64] = refWinY1_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i + 96] = refWinY1_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 160] = refWinY2_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+192] = refWinY2_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 224] = refWinY2_ref_mid[j][63];
                        }
                    }
                    for (int j = 0; j < 96; j++)
                    {
    #pragma HLS pipeline
                        for (int i = 0; i < 16; i++)
                        {
    #pragma HLS unroll
                            /**************************padding chroma*********************************/
                            refWinU_temp[j*128+i] = refWinU1_ref_mid[j][0];
                            refWinU_temp[j*128+i + 16] = refWinU1_ref_mid[j][0];
                            refWinU_temp[j*128+i + 32] = refWinU1_ref_mid[j][0];
                            refWinU_temp[j*128+i + 48] = refWinU1_ref_mid[j][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU2_ref_mid[j][i];
                            refWinU_temp[j*128+i + 96] = refWinU2_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 112] = refWinU2_ref_mid[j][31];
                                        
                            refWinV_temp[j*128+i] = refWinV1_ref_mid[j][0];
                            refWinV_temp[j*128+i + 16] = refWinV1_ref_mid[j][0];
                            refWinV_temp[j*128+i + 32] = refWinV1_ref_mid[j][0];
                            refWinV_temp[j*128+i + 48] = refWinV1_ref_mid[j][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV2_ref_mid[j][i];
                            refWinV_temp[j*128+i + 96] = refWinV2_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 112] = refWinV2_ref_mid[j][31];
                        }
                    }
                }
                else if ((pix_x + 128) == (luma_width_padded + 64))
                {
                    for (int j = 0; j < 192; j++)
                    {
    #pragma HLS pipeline
                        for (int i = 0; i < 32; i++)
                        {
    #pragma HLS unroll
                            /**************************padding luma*********************************/
                            // swOutLv0_temp[j][i] = refWinY1_ref_mid[j][0];
                            // swOutLv0_temp[j][i + 32] = refWinY1_ref_mid[j][0];
                            // swOutLv0_temp[j][i+64] = refWinY1_ref_mid[j][0];
                            // swOutLv0_temp[j][i + 96] = refWinY1_ref_mid[j][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 160] = refWinY1_ref_mid[j][63];
                            // swOutLv0_temp[j][i+192] = refWinY1_ref_mid[j][63];
                            // swOutLv0_temp[j][i + 224] = refWinY1_ref_mid[j][63];

                            swOutLv0_ime_temp[j][i] = refWinY1_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i + 32] = refWinY1_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i+64] = refWinY1_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i + 96] = refWinY1_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 160] = refWinY1_ref_mid[j][63];
                            swOutLv0_ime_temp[j][i+192] = refWinY1_ref_mid[j][63];
                            swOutLv0_ime_temp[j][i + 224] = refWinY1_ref_mid[j][63];
                        }
                    }
                    for (int j = 0; j < 96; j++)
                    {
    #pragma HLS pipeline
                        for (int i = 0; i < 16; i++)
                        {
    #pragma HLS unroll
                            /**************************padding chroma*********************************/
                            refWinU_temp[j*128+i] = refWinU1_ref_mid[j][0];
                            refWinU_temp[j*128+i + 16] = refWinU1_ref_mid[j][0];
                            refWinU_temp[j*128+i + 32] = refWinU1_ref_mid[j][0];
                            refWinU_temp[j*128+i + 48] = refWinU1_ref_mid[j][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU1_ref_mid[j][31];
                            refWinU_temp[j*128+i + 96] = refWinU1_ref_mid[j][31];
                            refWinU_temp[j*128+i + 112] = refWinU1_ref_mid[j][31];
                                        
                            refWinV_temp[j*128+i] = refWinV1_ref_mid[j][0];
                            refWinV_temp[j*128+i + 16] = refWinV1_ref_mid[j][0];
                            refWinV_temp[j*128+i + 32] = refWinV1_ref_mid[j][0];
                            refWinV_temp[j*128+i + 48] = refWinV1_ref_mid[j][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV1_ref_mid[j][31];
                            refWinV_temp[j*128+i + 96] = refWinV1_ref_mid[j][31];
                            refWinV_temp[j*128+i + 112] = refWinV1_ref_mid[j][31];
                        }
                    }
                }
            }
            else if (64 == pix_x)
            {
                if ((pix_x + 128) <= (luma_width_padded - 64))
                {
                    for (int j = 0; j < 192; j++)
                    {
        #pragma HLS pipeline
                        for (int i = 0; i < 32; i++)
                        {
        #pragma HLS unroll
                            /*******************************padding luma**************************************************/
                            // swOutLv0_temp[j][i] = refWinY0_ref_mid[j][0];
                            // swOutLv0_temp[j][i + 32] = refWinY0_ref_mid[j][i];
                            // swOutLv0_temp[j][i+64] = refWinY0_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 96] = refWinY1_ref_mid[j][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 160] = refWinY2_ref_mid[j][i];
                            // swOutLv0_temp[j][i+192] = refWinY2_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 224] = refWinY3_ref_mid[j][i];

                            swOutLv0_ime_temp[j][i] = refWinY0_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i + 32] = refWinY0_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+64] = refWinY0_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 96] = refWinY1_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 160] = refWinY2_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+192] = refWinY2_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 224] = refWinY3_ref_mid[j][i];
                        }
                    }

                    for (int j = 0; j < 96; j++)
                    {
        #pragma HLS pipeline
                        for (int i = 0; i < 16; i++)
                        {
        #pragma HLS unroll
                            /*******************************padding chroma**************************************************/
                            refWinU_temp[j*128+i] = refWinU0_ref_mid[j][0];
                            refWinU_temp[j*128+i + 16] = refWinU0_ref_mid[j][i];
                            refWinU_temp[j*128+i + 32] = refWinU0_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 48] = refWinU1_ref_mid[j][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU2_ref_mid[j][i];
                            refWinU_temp[j*128+i + 96] = refWinU2_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 112] = refWinU3_ref_mid[j][i];
                                        
                            refWinV_temp[j*128+i] = refWinV0_ref_mid[j][0];
                            refWinV_temp[j*128+i + 16] = refWinV0_ref_mid[j][i];
                            refWinV_temp[j*128+i + 32] = refWinV0_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 48] = refWinV1_ref_mid[j][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV2_ref_mid[j][i];
                            refWinV_temp[j*128+i + 96] = refWinV2_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 112] = refWinV3_ref_mid[j][i];
                        }
                    }
                }
                else if ((pix_x + 128) == luma_width_padded)
                {
                    for (int j = 0; j < 192; j++)
                    {
    #pragma HLS pipeline
                        for (int i = 0; i < 32; i++)
                        {
    #pragma HLS unroll
                            /*******************************padding luma**************************************************/
                            // swOutLv0_temp[j][i] = refWinY0_ref_mid[j][0];
                            // swOutLv0_temp[j][i + 32] = refWinY0_ref_mid[j][i];
                            // swOutLv0_temp[j][i+64] = refWinY0_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 96] = refWinY1_ref_mid[j][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 160] = refWinY2_ref_mid[j][i];
                            // swOutLv0_temp[j][i+192] = refWinY2_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 224] = refWinY2_ref_mid[j][63];

                            swOutLv0_ime_temp[j][i] = refWinY0_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i + 32] = refWinY0_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+64] = refWinY0_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 96] = refWinY1_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 160] = refWinY2_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+192] = refWinY2_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 224] = refWinY2_ref_mid[j][63];
                        }
                    }

                    for (int j = 0; j < 96; j++)
                    {
    #pragma HLS pipeline
                        for (int i = 0; i < 16; i++)
                        {
    #pragma HLS unroll
                            /*******************************padding chroma**************************************************/
                            refWinU_temp[j*128+i] = refWinU0_ref_mid[j][0];
                            refWinU_temp[j*128+i + 16] = refWinU0_ref_mid[j][i];
                            refWinU_temp[j*128+i + 32] = refWinU0_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 48] = refWinU1_ref_mid[j][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU2_ref_mid[j][i];
                            refWinU_temp[j*128+i + 96] = refWinU2_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 112] = refWinU2_ref_mid[j][31];
                                        
                            refWinV_temp[j*128+i] = refWinV0_ref_mid[j][0];
                            refWinV_temp[j*128+i + 16] = refWinV0_ref_mid[j][i];
                            refWinV_temp[j*128+i + 32] = refWinV0_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 48] = refWinV1_ref_mid[j][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV2_ref_mid[j][i];
                            refWinV_temp[j*128+i + 96] = refWinV2_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 112] = refWinV2_ref_mid[j][31];
                        }
                    }
                }
                else if ((pix_x + 128) == (luma_width_padded+64))
                {
                    for (int j = 0; j < 192; j++)
                    {
    #pragma HLS pipeline
                        for (int i = 0; i < 32; i++)
                        {
    #pragma HLS unroll
                            /*******************************padding luma**************************************************/
                            // swOutLv0_temp[j][i] = refWinY0_ref_mid[j][0];
                            // swOutLv0_temp[j][i + 32] = refWinY0_ref_mid[j][i];
                            // swOutLv0_temp[j][i+64] = refWinY0_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 96] = refWinY1_ref_mid[j][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 160] = refWinY1_ref_mid[j][63];
                            // swOutLv0_temp[j][i+192] = refWinY1_ref_mid[j][63];
                            // swOutLv0_temp[j][i + 224] = refWinY1_ref_mid[j][63];

                            swOutLv0_ime_temp[j][i] = refWinY0_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i + 32] = refWinY0_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+64] = refWinY0_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 96] = refWinY1_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 160] = refWinY1_ref_mid[j][63];
                            swOutLv0_ime_temp[j][i+192] = refWinY1_ref_mid[j][63];
                            swOutLv0_ime_temp[j][i + 224] = refWinY1_ref_mid[j][63];
                        }
                    }

                    for (int j = 0; j < 96; j++)
                    {
    #pragma HLS pipeline
                        for (int i = 0; i < 16; i++)
                        {
    #pragma HLS unroll
                            /*******************************padding chroma**************************************************/
                            refWinU_temp[j*128+i] = refWinU0_ref_mid[j][0];
                            refWinU_temp[j*128+i + 16] = refWinU0_ref_mid[j][i];
                            refWinU_temp[j*128+i + 32] = refWinU0_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 48] = refWinU1_ref_mid[j][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU1_ref_mid[j][31];
                            refWinU_temp[j*128+i + 96] = refWinU1_ref_mid[j][31];
                            refWinU_temp[j*128+i + 112] = refWinU1_ref_mid[j][31];
                                        
                            refWinV_temp[j*128+i] = refWinV0_ref_mid[j][0];
                            refWinV_temp[j*128+i + 16] = refWinV0_ref_mid[j][i];
                            refWinV_temp[j*128+i + 32] = refWinV0_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 48] = refWinV1_ref_mid[j][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV1_ref_mid[j][31];
                            refWinV_temp[j*128+i + 96] = refWinV1_ref_mid[j][31];
                            refWinV_temp[j*128+i + 112] = refWinV1_ref_mid[j][31];
                        }
                    }
                }
            }
            else
            {
                if ((pix_x + 128) <= (luma_width_padded - 64))
                {
                    for (int j = 0; j < 192; j++)
                    {
        #pragma HLS pipeline
                        for (int i = 0; i < 32; i++)
                        {
        #pragma HLS unroll
                            /*****************padding luma***************************************************/
                            // swOutLv0_temp[j][i] = refWinY32x192_ref_mid[j][i];
                            // swOutLv0_temp[j][i + 32] = refWinY0_ref_mid[j][i];
                            // swOutLv0_temp[j][i+64] = refWinY0_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 96] = refWinY1_ref_mid[j][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 160] = refWinY2_ref_mid[j][i];
                            // swOutLv0_temp[j][i+192] = refWinY2_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 224] = refWinY3_ref_mid[j][i];

                            swOutLv0_ime_temp[j][i] = refWinY32x192_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i + 32] = refWinY0_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+64] = refWinY0_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 96] = refWinY1_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 160] = refWinY2_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+192] = refWinY2_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 224] = refWinY3_ref_mid[j][i];
                        }
                    }

                    for (int j = 0; j < 96; j++)
                    {
        #pragma HLS pipeline
                        for (int i = 0; i < 16; i++)
                        {
        #pragma HLS unroll
                            /*****************padding chroma***************************************************/
                            refWinU_temp[j * 128 + i] = refWinU16x96_ref_mid[j][i];
                            refWinU_temp[j * 128 + i + 16] = refWinU0_ref_mid[j][i];
                            refWinU_temp[j * 128 + i + 32] = refWinU0_ref_mid[j][i + 16];
                            refWinU_temp[j * 128 + i + 48] = refWinU1_ref_mid[j][i];
                            refWinU_temp[j * 128 + i + 64] = refWinU1_ref_mid[j][i + 16];
                            refWinU_temp[j * 128 + i + 80] = refWinU2_ref_mid[j][i];
                            refWinU_temp[j * 128 + i + 96] = refWinU2_ref_mid[j][i + 16];
                            refWinU_temp[j * 128 + i + 112] = refWinU3_ref_mid[j][i];
                                        
                            refWinV_temp[j*128+i] = refWinV16x96_ref_mid[j][i];
                            refWinV_temp[j*128+i + 16] = refWinV0_ref_mid[j][i];
                            refWinV_temp[j*128+i + 32] = refWinV0_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 48] = refWinV1_ref_mid[j][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV2_ref_mid[j][i];
                            refWinV_temp[j*128+i + 96] = refWinV2_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 112] = refWinV3_ref_mid[j][i];
                        }
                    }
                }
                else if ((pix_x + 128) == luma_width_padded)
                {
                    for (int j = 0; j < 192; j++)
                    {
    #pragma HLS pipeline
                        for (int i = 0; i < 32; i++)
                        {
    #pragma HLS unroll
                            /*****************padding luma***************************************************/
                            // swOutLv0_temp[j][i] = refWinY32x192_ref_mid[j][i];
                            // swOutLv0_temp[j][i + 32] = refWinY0_ref_mid[j][i];
                            // swOutLv0_temp[j][i+64] = refWinY0_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 96] = refWinY1_ref_mid[j][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 160] = refWinY2_ref_mid[j][i];
                            // swOutLv0_temp[j][i+192] = refWinY2_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 224] = refWinY2_ref_mid[j][63];

                            swOutLv0_ime_temp[j][i] = refWinY32x192_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i + 32] = refWinY0_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+64] = refWinY0_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 96] = refWinY1_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 160] = refWinY2_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+192] = refWinY2_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 224] = refWinY2_ref_mid[j][63];
                        }
                    }

                    for (int j = 0; j < 96; j++)
                    {
    #pragma HLS pipeline
                        for (int i = 0; i < 16; i++)
                        {
    #pragma HLS unroll
                            /*****************padding chroma***************************************************/
                            refWinU_temp[j*128+i] = refWinU16x96_ref_mid[j][i];
                            refWinU_temp[j*128+i + 16] = refWinU0_ref_mid[j][i];
                            refWinU_temp[j*128+i + 32] = refWinU0_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 48] = refWinU1_ref_mid[j][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU2_ref_mid[j][i];
                            refWinU_temp[j*128+i + 96] = refWinU2_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 112] = refWinU2_ref_mid[j][31];
                                        
                            refWinV_temp[j*128+i] = refWinV16x96_ref_mid[j][i];
                            refWinV_temp[j*128+i + 16] = refWinV0_ref_mid[j][i];
                            refWinV_temp[j*128+i + 32] = refWinV0_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 48] = refWinV1_ref_mid[j][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV2_ref_mid[j][i];
                            refWinV_temp[j*128+i + 96] = refWinV2_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 112] = refWinV2_ref_mid[j][31];
                        }
                    }
                }
                else if ((pix_x + 128) == (luma_width_padded+64))
                {
                    for (int j = 0; j < 192; j++)
                    {
    #pragma HLS pipeline
                        for (int i = 0; i < 32; i++)
                        {
    #pragma HLS unroll
                            /*****************padding luma***************************************************/
                            // swOutLv0_temp[j][i] = refWinY32x192_ref_mid[j][i];
                            // swOutLv0_temp[j][i + 32] = refWinY0_ref_mid[j][i];
                            // swOutLv0_temp[j][i+64] = refWinY0_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 96] = refWinY1_ref_mid[j][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 160] = refWinY1_ref_mid[j][63];
                            // swOutLv0_temp[j][i+192] = refWinY1_ref_mid[j][63];
                            // swOutLv0_temp[j][i + 224] = refWinY1_ref_mid[j][63];

                            swOutLv0_ime_temp[j][i] = refWinY32x192_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i + 32] = refWinY0_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+64] = refWinY0_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 96] = refWinY1_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 160] = refWinY1_ref_mid[j][63];
                            swOutLv0_ime_temp[j][i+192] = refWinY1_ref_mid[j][63];
                            swOutLv0_ime_temp[j][i + 224] = refWinY1_ref_mid[j][63];
                        }
                    }

                    for (int j = 0; j < 96; j++)
                    {
    #pragma HLS pipeline
                        for (int i = 0; i < 16; i++)
                        {
    #pragma HLS unroll
                            /*****************padding chroma***************************************************/
                            refWinU_temp[j*128+i] = refWinU16x96_ref_mid[j][i];
                            refWinU_temp[j*128+i + 16] = refWinU0_ref_mid[j][i];
                            refWinU_temp[j*128+i + 32] = refWinU0_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 48] = refWinU1_ref_mid[j][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU1_ref_mid[j][31];
                            refWinU_temp[j*128+i + 96] = refWinU1_ref_mid[j][31];
                            refWinU_temp[j*128+i + 112] = refWinU1_ref_mid[j][31];
                                        
                            refWinV_temp[j*128+i] = refWinV16x96_ref_mid[j][i];
                            refWinV_temp[j*128+i + 16] = refWinV0_ref_mid[j][i];
                            refWinV_temp[j*128+i + 32] = refWinV0_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 48] = refWinV1_ref_mid[j][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV1_ref_mid[j][31];
                            refWinV_temp[j*128+i + 96] = refWinV1_ref_mid[j][31];
                            refWinV_temp[j*128+i + 112] = refWinV1_ref_mid[j][31];
                        }
                    }
                }
            }
        }
    }
#ifndef __SYNTHESIS__
    else
    {
        //first row
        if (0 == pix_y)
        {
            if (0==pix_x)
            {
                if ((pix_x + 128) <= (luma_width_padded - 64))
                
                {
                    for (int j = 0; j < 64; j++)
                    {
        #pragma HLS pipeline 
                        for (int i = 0; i < 32; i++)
                        {
        #pragma HLS unroll
                            /*************************padding luma***********************************/
                            // swOutLv0_temp[j][i] = refWinY1_B_ref_mid[64][0];
                            // swOutLv0_temp[j][i+32] = refWinY1_B_ref_mid[64][0];
                            // swOutLv0_temp[j][i+64] = refWinY1_B_ref_mid[64][0];
                            // swOutLv0_temp[j][i+96] = refWinY1_B_ref_mid[64][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_B_ref_mid[64][i + 32];
                            // swOutLv0_temp[j][i+160] = refWinY2_B_ref_mid[64][i];
                            // swOutLv0_temp[j][i+192] = refWinY2_B_ref_mid[64][i + 32];
                            // swOutLv0_temp[j][i+224] = refWinY3_B_ref_mid[64][i];

                            swOutLv0_ime_temp[j][i] = refWinY1_B_ref_mid[64][0];
                            swOutLv0_ime_temp[j][i+32] = refWinY1_B_ref_mid[64][0];
                            swOutLv0_ime_temp[j][i+64] = refWinY1_B_ref_mid[64][0];
                            swOutLv0_ime_temp[j][i+96] = refWinY1_B_ref_mid[64][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_B_ref_mid[64][i + 32];
                            swOutLv0_ime_temp[j][i+160] = refWinY2_B_ref_mid[64][i];
                            swOutLv0_ime_temp[j][i+192] = refWinY2_B_ref_mid[64][i + 32];
                            swOutLv0_ime_temp[j][i+224] = refWinY3_B_ref_mid[64][i];
                        }
                    }
                    for (int j = 0; j < 32; j++)
                    {
            #pragma HLS pipeline 
                        for (int i = 0; i < 16; i++)
                        {
            #pragma HLS unroll
                            /*************************padding chroma***********************************/
                            refWinU_temp[j*128+i] = refWinU1_B_ref_mid[32][0];
                            refWinU_temp[j*128+i + 16] = refWinU1_B_ref_mid[32][0];
                            refWinU_temp[j*128+i+32] = refWinU1_B_ref_mid[32][0];
                            refWinU_temp[j*128+i + 48] = refWinU1_B_ref_mid[32][i];
                            refWinU_temp[j*128+i+64] = refWinU1_B_ref_mid[32][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU2_B_ref_mid[32][i];
                            refWinU_temp[j*128+i+96] = refWinU2_B_ref_mid[32][i + 16];
                            refWinU_temp[j*128+i + 112] = refWinU3_B_ref_mid[32][i];

                            refWinV_temp[j*128+i] = refWinV1_B_ref_mid[32][0];
                            refWinV_temp[j*128+i + 16] = refWinV1_B_ref_mid[32][0];
                            refWinV_temp[j*128+i+32] = refWinV1_B_ref_mid[32][0];
                            refWinV_temp[j*128+i + 48] = refWinV1_B_ref_mid[32][i];
                            refWinV_temp[j*128+i+64] = refWinV1_B_ref_mid[32][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV2_B_ref_mid[32][i];
                            refWinV_temp[j*128+i+96] = refWinV2_B_ref_mid[32][i + 16];
                            refWinV_temp[j*128+i + 112] = refWinV3_B_ref_mid[32][i];
                    
                        }
                    }

                    for (int j = 64; j < 192; j++)
                    {
        #pragma HLS pipeline 
                        for (int i = 0; i < 32; i++)
                        {
        #pragma HLS unroll
                            /*************************padding luma***********************************/
                            // swOutLv0_temp[j][i] = refWinY1_B_ref_mid[j][0];
                            // swOutLv0_temp[j][i+32] = refWinY1_B_ref_mid[j][0];
                            // swOutLv0_temp[j][i+64] = refWinY1_B_ref_mid[j][0];
                            // swOutLv0_temp[j][i+96] = refWinY1_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_B_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i+160] = refWinY2_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i+192] = refWinY2_B_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i+224] = refWinY3_B_ref_mid[j][i];

                            swOutLv0_ime_temp[j][i] = refWinY1_B_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i+32] = refWinY1_B_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i+64] = refWinY1_B_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i+96] = refWinY1_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_B_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i+160] = refWinY2_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+192] = refWinY2_B_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i+224] = refWinY3_B_ref_mid[j][i];
                        }
                    }

                    for (int j = 32; j < 96; j++)
                    {
            #pragma HLS pipeline 
                        for (int i = 0; i < 16; i++)
                        {
            #pragma HLS unroll
                        /*************************padding chroma***********************************/
                            refWinU_temp[j*128+i] = refWinU1_B_ref_mid[j][0];
                            refWinU_temp[j*128+i + 16] = refWinU1_B_ref_mid[j][0];
                            refWinU_temp[j*128+i+32] = refWinU1_B_ref_mid[j][0];
                            refWinU_temp[j*128+i + 48] = refWinU1_B_ref_mid[j][i];
                            refWinU_temp[j*128+i+64] = refWinU1_B_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU2_B_ref_mid[j][i];
                            refWinU_temp[j*128+i+96] = refWinU2_B_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 112] = refWinU3_B_ref_mid[j][i];
                                    
                            refWinV_temp[j*128+i] = refWinV1_B_ref_mid[j][0];
                            refWinV_temp[j*128+i + 16] = refWinV1_B_ref_mid[j][0];
                            refWinV_temp[j*128+i+32] = refWinV1_B_ref_mid[j][0];
                            refWinV_temp[j*128+i + 48] = refWinV1_B_ref_mid[j][i];
                            refWinV_temp[j*128+i+64] = refWinV1_B_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV2_B_ref_mid[j][i];
                            refWinV_temp[j*128+i+96] = refWinV2_B_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 112] = refWinV3_B_ref_mid[j][i];
                        }
                    }
                }
                else if ((pix_x + 128) == luma_width_padded)
                {
                    for (int j = 0; j < 64; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 32; i++)
                        {
    #pragma HLS unroll
                            /*************************padding luma***********************************/
                            // swOutLv0_temp[j][i] = refWinY1_B_ref_mid[64][0];
                            // swOutLv0_temp[j][i+32] = refWinY1_B_ref_mid[64][0];
                            // swOutLv0_temp[j][i+64] = refWinY1_B_ref_mid[64][0];
                            // swOutLv0_temp[j][i+96] = refWinY1_B_ref_mid[64][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_B_ref_mid[64][i + 32];
                            // swOutLv0_temp[j][i+160] = refWinY2_B_ref_mid[64][i];
                            // swOutLv0_temp[j][i+192] = refWinY2_B_ref_mid[64][i + 32];
                            // swOutLv0_temp[j][i+224] = refWinY2_B_ref_mid[64][63];

                            swOutLv0_ime_temp[j][i] = refWinY1_B_ref_mid[64][0];
                            swOutLv0_ime_temp[j][i+32] = refWinY1_B_ref_mid[64][0];
                            swOutLv0_ime_temp[j][i+64] = refWinY1_B_ref_mid[64][0];
                            swOutLv0_ime_temp[j][i+96] = refWinY1_B_ref_mid[64][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_B_ref_mid[64][i + 32];
                            swOutLv0_ime_temp[j][i+160] = refWinY2_B_ref_mid[64][i];
                            swOutLv0_ime_temp[j][i+192] = refWinY2_B_ref_mid[64][i + 32];
                            swOutLv0_ime_temp[j][i+224] = refWinY2_B_ref_mid[64][63];
                        }
                    }
                    for (int j = 0; j < 32; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 16; i++)
                        {
    #pragma HLS unroll
                            /*************************padding chroma***********************************/
                            refWinU_temp[j*128+i] = refWinU1_B_ref_mid[32][0];
                            refWinU_temp[j*128+i + 16] = refWinU1_B_ref_mid[32][0];
                            refWinU_temp[j*128+i+32] = refWinU1_B_ref_mid[32][0];
                            refWinU_temp[j*128+i + 48] = refWinU1_B_ref_mid[32][i];
                            refWinU_temp[j*128+i+64] = refWinU1_B_ref_mid[32][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU2_B_ref_mid[32][i];
                            refWinU_temp[j*128+i+96] = refWinU2_B_ref_mid[32][i + 16];
                            refWinU_temp[j*128+i + 112] = refWinU2_B_ref_mid[32][31];
                                        
                            refWinV_temp[j*128+i] = refWinV1_B_ref_mid[32][0];
                            refWinV_temp[j*128+i + 16] = refWinV1_B_ref_mid[32][0];
                            refWinV_temp[j*128+i+32] = refWinV1_B_ref_mid[32][0];
                            refWinV_temp[j*128+i + 48] = refWinV1_B_ref_mid[32][i];
                            refWinV_temp[j*128+i+64] = refWinV1_B_ref_mid[32][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV2_B_ref_mid[32][i];
                            refWinV_temp[j*128+i+96] = refWinV2_B_ref_mid[32][i + 16];
                            refWinV_temp[j*128+i + 112] = refWinV2_B_ref_mid[32][31];
                        }
                    }

                    for (int j = 64; j < 192; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 32; i++)
                        {
    #pragma HLS unroll
                            /*************************padding luma***********************************/
                            // swOutLv0_temp[j][i] = refWinY1_B_ref_mid[j][0];
                            // swOutLv0_temp[j][i+32] = refWinY1_B_ref_mid[j][0];
                            // swOutLv0_temp[j][i+64] = refWinY1_B_ref_mid[j][0];
                            // swOutLv0_temp[j][i+96] = refWinY1_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_B_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i+160] = refWinY2_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i+192] = refWinY2_B_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i+224] = refWinY2_B_ref_mid[j][63];

                            swOutLv0_ime_temp[j][i] = refWinY1_B_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i+32] = refWinY1_B_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i+64] = refWinY1_B_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i+96] = refWinY1_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_B_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i+160] = refWinY2_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+192] = refWinY2_B_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i+224] = refWinY2_B_ref_mid[j][63];
                        }
                    }

                    for (int j = 32; j < 96; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 16; i++)
                        {
    #pragma HLS unroll
                            /*************************padding chroma***********************************/
                            refWinU_temp[j*128+i] = refWinU1_B_ref_mid[j][0];
                            refWinU_temp[j*128+i + 16] = refWinU1_B_ref_mid[j][0];
                            refWinU_temp[j*128+i+32] = refWinU1_B_ref_mid[j][0];
                            refWinU_temp[j*128+i + 48] = refWinU1_B_ref_mid[j][i];
                            refWinU_temp[j*128+i+64] = refWinU1_B_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU2_B_ref_mid[j][i];
                            refWinU_temp[j*128+i+96] = refWinU2_B_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 112] = refWinU2_B_ref_mid[j][31];
                                        
                            refWinV_temp[j*128+i] = refWinV1_B_ref_mid[j][0];
                            refWinV_temp[j*128+i + 16] = refWinV1_B_ref_mid[j][0];
                            refWinV_temp[j*128+i+32] = refWinV1_B_ref_mid[j][0];
                            refWinV_temp[j*128+i + 48] = refWinV1_B_ref_mid[j][i];
                            refWinV_temp[j*128+i+64] = refWinV1_B_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV2_B_ref_mid[j][i];
                            refWinV_temp[j*128+i+96] = refWinV2_B_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 112] = refWinV2_B_ref_mid[j][31];
                        }
                    }
                }
                else if ((pix_x + 128) == luma_width_padded+64)
                {
                    for (int j = 0; j < 64; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 32; i++)
                        {
    #pragma HLS unroll
                            /*************************padding luma***********************************/
                            // swOutLv0_temp[j][i] = refWinY1_B_ref_mid[64][0];
                            // swOutLv0_temp[j][i+32] = refWinY1_B_ref_mid[64][0];
                            // swOutLv0_temp[j][i+64] = refWinY1_B_ref_mid[64][0];
                            // swOutLv0_temp[j][i+96] = refWinY1_B_ref_mid[64][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_B_ref_mid[64][i + 32];
                            // swOutLv0_temp[j][i+160] = refWinY1_B_ref_mid[64][63];
                            // swOutLv0_temp[j][i+192] = refWinY1_B_ref_mid[64][63];
                            // swOutLv0_temp[j][i+224] = refWinY1_B_ref_mid[64][63];

                            swOutLv0_ime_temp[j][i] = refWinY1_B_ref_mid[64][0];
                            swOutLv0_ime_temp[j][i+32] = refWinY1_B_ref_mid[64][0];
                            swOutLv0_ime_temp[j][i+64] = refWinY1_B_ref_mid[64][0];
                            swOutLv0_ime_temp[j][i+96] = refWinY1_B_ref_mid[64][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_B_ref_mid[64][i + 32];
                            swOutLv0_ime_temp[j][i+160] = refWinY1_B_ref_mid[64][63];
                            swOutLv0_ime_temp[j][i+192] = refWinY1_B_ref_mid[64][63];
                            swOutLv0_ime_temp[j][i+224] = refWinY1_B_ref_mid[64][63];
                        }
                    }
                    for (int j = 0; j < 32; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 16; i++)
                        {
    #pragma HLS unroll
                            /*************************padding chroma***********************************/
                            refWinU_temp[j*128+i] = refWinU1_B_ref_mid[32][0];
                            refWinU_temp[j*128+i + 16] = refWinU1_B_ref_mid[32][0];
                            refWinU_temp[j*128+i+32] = refWinU1_B_ref_mid[32][0];
                            refWinU_temp[j*128+i + 48] = refWinU1_B_ref_mid[32][i];
                            refWinU_temp[j*128+i+64] = refWinU1_B_ref_mid[32][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU1_B_ref_mid[32][31];
                            refWinU_temp[j*128+i+96] = refWinU1_B_ref_mid[32][31];
                            refWinU_temp[j*128+i + 112] = refWinU1_B_ref_mid[32][31];
                            
                            refWinV_temp[j*128+i] = refWinV1_B_ref_mid[32][0];
                            refWinV_temp[j*128+i + 16] = refWinV1_B_ref_mid[32][0];
                            refWinV_temp[j*128+i+32] = refWinV1_B_ref_mid[32][0];
                            refWinV_temp[j*128+i + 48] = refWinV1_B_ref_mid[32][i];
                            refWinV_temp[j*128+i+64] = refWinV1_B_ref_mid[32][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV1_B_ref_mid[32][31];
                            refWinV_temp[j*128+i+96] = refWinV1_B_ref_mid[32][31];
                            refWinV_temp[j*128+i + 112] = refWinV1_B_ref_mid[32][31];
                        }
                    }

                    for (int j = 64; j < 192; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 32; i++)
                        {
    #pragma HLS unroll
                            /*************************padding luma***********************************/
                            // swOutLv0_temp[j][i] = refWinY1_B_ref_mid[j][0];
                            // swOutLv0_temp[j][i+32] = refWinY1_B_ref_mid[j][0];
                            // swOutLv0_temp[j][i+64] = refWinY1_B_ref_mid[j][0];
                            // swOutLv0_temp[j][i+96] = refWinY1_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_B_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i+160] = refWinY1_B_ref_mid[j][63];
                            // swOutLv0_temp[j][i+192] = refWinY1_B_ref_mid[j][63];
                            // swOutLv0_temp[j][i+224] = refWinY1_B_ref_mid[j][63];

                            swOutLv0_ime_temp[j][i] = refWinY1_B_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i+32] = refWinY1_B_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i+64] = refWinY1_B_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i+96] = refWinY1_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_B_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i+160] = refWinY1_B_ref_mid[j][63];
                            swOutLv0_ime_temp[j][i+192] = refWinY1_B_ref_mid[j][63];
                            swOutLv0_ime_temp[j][i+224] = refWinY1_B_ref_mid[j][63];
                        }
                    }

                    for (int j = 32; j < 96; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 16; i++)
                        {
    #pragma HLS unroll
                            /*************************padding chroma***********************************/
                            refWinU_temp[j*128+i] = refWinU1_B_ref_mid[j][0];
                            refWinU_temp[j*128+i + 16] = refWinU1_B_ref_mid[j][0];
                            refWinU_temp[j*128+i+32] = refWinU1_B_ref_mid[j][0];
                            refWinU_temp[j*128+i + 48] = refWinU1_B_ref_mid[j][i];
                            refWinU_temp[j*128+i+64] = refWinU1_B_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU1_B_ref_mid[j][31];
                            refWinU_temp[j*128+i+96] = refWinU1_B_ref_mid[j][31];
                            refWinU_temp[j*128+i + 112] = refWinU1_B_ref_mid[j][31];
                                        
                            refWinV_temp[j*128+i] = refWinV1_B_ref_mid[j][0];
                            refWinV_temp[j*128+i + 16] = refWinV1_B_ref_mid[j][0];
                            refWinV_temp[j*128+i+32] = refWinV1_B_ref_mid[j][0];
                            refWinV_temp[j*128+i + 48] = refWinV1_B_ref_mid[j][i];
                            refWinV_temp[j*128+i+64] = refWinV1_B_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV1_B_ref_mid[j][31];
                            refWinV_temp[j*128+i+96] = refWinV1_B_ref_mid[j][31];
                            refWinV_temp[j*128+i + 112] = refWinV1_B_ref_mid[j][31];
                        }
                    }
                }
            }
            else if (64 == pix_x)
            {
                if ((pix_x + 128) <= (luma_width_padded - 64))
                {
                    for (int j = 0; j < 64; j++)
                    {
        #pragma HLS pipeline 
                        for (int i = 0; i < 32; i++)
                        {
        #pragma HLS unroll
                            /*************************padding luma*************************************/
                            // swOutLv0_temp[j][i] = refWinY0_B_ref_mid[64][0];
                            // swOutLv0_temp[j][i+32] = refWinY0_B_ref_mid[64][i];
                            // swOutLv0_temp[j][i+64] = refWinY0_B_ref_mid[64][i + 32];
                            // swOutLv0_temp[j][i+96] = refWinY1_B_ref_mid[64][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_B_ref_mid[64][i + 32];
                            // swOutLv0_temp[j][i+160] = refWinY2_B_ref_mid[64][i];
                            // swOutLv0_temp[j][i+192] = refWinY2_B_ref_mid[64][i + 32];
                            // swOutLv0_temp[j][i+224] = refWinY3_B_ref_mid[64][i];

                            swOutLv0_ime_temp[j][i] = refWinY0_B_ref_mid[64][0];
                            swOutLv0_ime_temp[j][i+32] = refWinY0_B_ref_mid[64][i];
                            swOutLv0_ime_temp[j][i+64] = refWinY0_B_ref_mid[64][i + 32];
                            swOutLv0_ime_temp[j][i+96] = refWinY1_B_ref_mid[64][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_B_ref_mid[64][i + 32];
                            swOutLv0_ime_temp[j][i+160] = refWinY2_B_ref_mid[64][i];
                            swOutLv0_ime_temp[j][i+192] = refWinY2_B_ref_mid[64][i + 32];
                            swOutLv0_ime_temp[j][i+224] = refWinY3_B_ref_mid[64][i];
                        }
                    }

                    for (int j = 0; j < 32; j++)
                    {
            #pragma HLS pipeline 
                        for (int i = 0; i < 16; i++)
                        {
            #pragma HLS unroll
                            /*************************padding chroma*************************************/
                            refWinU_temp[j*128+i] = refWinU0_B_ref_mid[32][0];
                            refWinU_temp[j*128+i + 16] = refWinU0_B_ref_mid[32][i];
                            refWinU_temp[j*128+i+32] = refWinU0_B_ref_mid[32][i + 16];
                            refWinU_temp[j*128+i + 48] = refWinU1_B_ref_mid[32][i];
                            refWinU_temp[j*128+i+64] = refWinU1_B_ref_mid[32][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU2_B_ref_mid[32][i];
                            refWinU_temp[j*128+i+96] = refWinU2_B_ref_mid[32][i + 16];
                            refWinU_temp[j*128+i + 112] = refWinU3_B_ref_mid[32][i];

                            refWinV_temp[j*128+i] = refWinV0_B_ref_mid[32][0];
                            refWinV_temp[j*128+i + 16] = refWinV0_B_ref_mid[32][i];
                            refWinV_temp[j*128+i+32] = refWinV0_B_ref_mid[32][i + 16];
                            refWinV_temp[j*128+i + 48] = refWinV1_B_ref_mid[32][i];
                            refWinV_temp[j*128+i+64] = refWinV1_B_ref_mid[32][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV2_B_ref_mid[32][i];
                            refWinV_temp[j*128+i+96] = refWinV2_B_ref_mid[32][i + 16];
                            refWinV_temp[j*128+i + 112] = refWinV3_B_ref_mid[32][i];
                        }
                    }

                    for (int j = 64; j < 192; j++)
                    {
        #pragma HLS pipeline 
                        for (int i = 0; i < 32; i++)
                        {
        #pragma HLS unroll 
                            /*************************padding luma*************************************/
                            // swOutLv0_temp[j][i] = refWinY0_B_ref_mid[j][0];
                            // swOutLv0_temp[j][i+32] = refWinY0_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i+64] = refWinY0_B_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i+96] = refWinY1_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_B_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i+160] = refWinY2_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i+192] = refWinY2_B_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i+224] = refWinY3_B_ref_mid[j][i];

                            swOutLv0_ime_temp[j][i] = refWinY0_B_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i+32] = refWinY0_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+64] = refWinY0_B_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i+96] = refWinY1_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_B_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i+160] = refWinY2_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+192] = refWinY2_B_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i+224] = refWinY3_B_ref_mid[j][i];
                        }
                    }

                    for (int j = 32; j < 96; j++)
                    {
            #pragma HLS pipeline 
                        for (int i = 0; i < 16; i++)
                        {
            #pragma HLS unroll 
                            /*************************padding chroma*************************************/
                            refWinU_temp[j*128+i] = refWinU0_B_ref_mid[j][0];
                            refWinU_temp[j*128+i + 16] = refWinU0_B_ref_mid[j][i];
                            refWinU_temp[j*128+i + 32] = refWinU0_B_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 48] = refWinU1_B_ref_mid[j][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_B_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU2_B_ref_mid[j][i];
                            refWinU_temp[j*128+i + 96] = refWinU2_B_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 112] = refWinU3_B_ref_mid[j][i];
                                        
                            refWinV_temp[j*128+i] = refWinV0_B_ref_mid[j][0];
                            refWinV_temp[j*128+i + 16] = refWinV0_B_ref_mid[j][i];
                            refWinV_temp[j*128+i + 32] = refWinV0_B_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 48] = refWinV1_B_ref_mid[j][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_B_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV2_B_ref_mid[j][i];
                            refWinV_temp[j*128+i + 96] = refWinV2_B_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 112] = refWinV3_B_ref_mid[j][i];
                        }
                    } 
                }
                else if ((pix_x + 128) == luma_width_padded)
                {
                    for (int j = 0; j < 64; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 32; i++)
                        {
    #pragma HLS unroll
                            /*************************padding luma*************************************/
                            // swOutLv0_temp[j][i] = refWinY0_B_ref_mid[64][0];
                            // swOutLv0_temp[j][i+32] = refWinY0_B_ref_mid[64][i];
                            // swOutLv0_temp[j][i+64] = refWinY0_B_ref_mid[64][i + 32];
                            // swOutLv0_temp[j][i+96] = refWinY1_B_ref_mid[64][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_B_ref_mid[64][i + 32];
                            // swOutLv0_temp[j][i+160] = refWinY2_B_ref_mid[64][i];
                            // swOutLv0_temp[j][i+192] = refWinY2_B_ref_mid[64][i+32];
                            // swOutLv0_temp[j][i+224] = refWinY2_B_ref_mid[64][63];

                            swOutLv0_ime_temp[j][i] = refWinY0_B_ref_mid[64][0];
                            swOutLv0_ime_temp[j][i+32] = refWinY0_B_ref_mid[64][i];
                            swOutLv0_ime_temp[j][i+64] = refWinY0_B_ref_mid[64][i + 32];
                            swOutLv0_ime_temp[j][i+96] = refWinY1_B_ref_mid[64][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_B_ref_mid[64][i + 32];
                            swOutLv0_ime_temp[j][i+160] = refWinY2_B_ref_mid[64][i];
                            swOutLv0_ime_temp[j][i+192] = refWinY2_B_ref_mid[64][i+32];
                            swOutLv0_ime_temp[j][i+224] = refWinY2_B_ref_mid[64][63];
                        }
                    }

                    for (int j = 0; j < 32; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 16; i++)
                        {
    #pragma HLS unroll
                            /*************************padding chroma*************************************/
                            refWinU_temp[j*128+i] = refWinU0_B_ref_mid[32][0];
                            refWinU_temp[j*128+i + 16] = refWinU0_B_ref_mid[32][i];
                            refWinU_temp[j*128+i + 32] = refWinU0_B_ref_mid[32][i + 16];
                            refWinU_temp[j*128+i + 48] = refWinU1_B_ref_mid[32][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_B_ref_mid[32][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU2_B_ref_mid[32][i];
                            refWinU_temp[j*128+i + 96] = refWinU2_B_ref_mid[32][i+16];
                            refWinU_temp[j*128+i + 112] = refWinU2_B_ref_mid[32][31];
                                        
                            refWinV_temp[j*128+i] = refWinV0_B_ref_mid[32][0];
                            refWinV_temp[j*128+i + 16] = refWinV0_B_ref_mid[32][i];
                            refWinV_temp[j*128+i + 32] = refWinV0_B_ref_mid[32][i + 16];
                            refWinV_temp[j*128+i + 48] = refWinV1_B_ref_mid[32][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_B_ref_mid[32][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV2_B_ref_mid[32][i];
                            refWinV_temp[j*128+i + 96] = refWinV2_B_ref_mid[32][i+16];
                            refWinV_temp[j*128+i + 112] = refWinV2_B_ref_mid[32][31];
                        }
                    }

                    for (int j = 64; j < 192; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 32; i++)
                        {
    #pragma HLS unroll 
                            /*************************padding luma*************************************/
                            // swOutLv0_temp[j][i] = refWinY0_B_ref_mid[j][0];
                            // swOutLv0_temp[j][i+32] = refWinY0_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i+64] = refWinY0_B_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i+96] = refWinY1_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_B_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i+160] = refWinY2_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i+192] = refWinY2_B_ref_mid[j][i+32];
                            // swOutLv0_temp[j][i+224] = refWinY2_B_ref_mid[j][63];

                            swOutLv0_ime_temp[j][i] = refWinY0_B_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i+32] = refWinY0_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+64] = refWinY0_B_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i+96] = refWinY1_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_B_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i+160] = refWinY2_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+192] = refWinY2_B_ref_mid[j][i+32];
                            swOutLv0_ime_temp[j][i+224] = refWinY2_B_ref_mid[j][63];
                        }
                    }

                    for (int j = 32; j < 96; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 16; i++)
                        {
    #pragma HLS unroll 
                            /*************************padding chroma*************************************/
                            refWinU_temp[j*128+i] = refWinU0_B_ref_mid[j][0];
                            refWinU_temp[j*128+i + 16] = refWinU0_B_ref_mid[j][i];
                            refWinU_temp[j*128+i + 32] = refWinU0_B_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 48] = refWinU1_B_ref_mid[j][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_B_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU2_B_ref_mid[j][i];
                            refWinU_temp[j*128+i + 96] = refWinU2_B_ref_mid[j][i+16];
                            refWinU_temp[j*128+i + 112] = refWinU2_B_ref_mid[j][31];
                                        
                            refWinV_temp[j*128+i] = refWinV0_B_ref_mid[j][0];
                            refWinV_temp[j*128+i + 16] = refWinV0_B_ref_mid[j][i];
                            refWinV_temp[j*128+i + 32] = refWinV0_B_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 48] = refWinV1_B_ref_mid[j][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_B_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV2_B_ref_mid[j][i];
                            refWinV_temp[j*128+i + 96] = refWinV2_B_ref_mid[j][i+16];
                            refWinV_temp[j*128+i + 112] = refWinV2_B_ref_mid[j][31];
                        }
                    }
                }
                else if ((pix_x + 128) == luma_width_padded+64)
                {
                    for (int j = 0; j < 64; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 32; i++)
                        {
    #pragma HLS unroll
                            /*************************padding luma*************************************/
                            // swOutLv0_temp[j][i] = refWinY0_B_ref_mid[64][0];
                            // swOutLv0_temp[j][i+32] = refWinY0_B_ref_mid[64][i];
                            // swOutLv0_temp[j][i+64] = refWinY0_B_ref_mid[64][i + 32];
                            // swOutLv0_temp[j][i+96] = refWinY1_B_ref_mid[64][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_B_ref_mid[64][i+32];
                            // swOutLv0_temp[j][i+160] = refWinY1_B_ref_mid[64][63];
                            // swOutLv0_temp[j][i+192] = refWinY1_B_ref_mid[64][63];
                            // swOutLv0_temp[j][i+224] = refWinY1_B_ref_mid[64][63];

                            swOutLv0_ime_temp[j][i] = refWinY0_B_ref_mid[64][0];
                            swOutLv0_ime_temp[j][i+32] = refWinY0_B_ref_mid[64][i];
                            swOutLv0_ime_temp[j][i+64] = refWinY0_B_ref_mid[64][i + 32];
                            swOutLv0_ime_temp[j][i+96] = refWinY1_B_ref_mid[64][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_B_ref_mid[64][i+32];
                            swOutLv0_ime_temp[j][i+160] = refWinY1_B_ref_mid[64][63];
                            swOutLv0_ime_temp[j][i+192] = refWinY1_B_ref_mid[64][63];
                            swOutLv0_ime_temp[j][i+224] = refWinY1_B_ref_mid[64][63];
                        }
                    }

                    for (int j = 0; j < 32; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 16; i++)
                        {
    #pragma HLS unroll
                            /*************************padding chroma*************************************/
                            refWinU_temp[j*128+i] = refWinU0_B_ref_mid[32][0];
                            refWinU_temp[j*128+i + 16] = refWinU0_B_ref_mid[32][i];
                            refWinU_temp[j*128+i + 32] = refWinU0_B_ref_mid[32][i + 16];
                            refWinU_temp[j*128+i + 48] = refWinU1_B_ref_mid[32][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_B_ref_mid[32][i+16];
                            refWinU_temp[j*128+i + 80] = refWinU1_B_ref_mid[32][31];
                            refWinU_temp[j*128+i + 96] = refWinU1_B_ref_mid[32][31];
                            refWinU_temp[j*128+i + 112] = refWinU1_B_ref_mid[32][31];
                                        
                            refWinV_temp[j*128+i] = refWinV0_B_ref_mid[32][0];
                            refWinV_temp[j*128+i + 16] = refWinV0_B_ref_mid[32][i];
                            refWinV_temp[j*128+i + 32] = refWinV0_B_ref_mid[32][i + 16];
                            refWinV_temp[j*128+i + 48] = refWinV1_B_ref_mid[32][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_B_ref_mid[32][i+16];
                            refWinV_temp[j*128+i + 80] = refWinV1_B_ref_mid[32][31];
                            refWinV_temp[j*128+i + 96] = refWinV1_B_ref_mid[32][31];
                            refWinV_temp[j*128+i + 112] = refWinV1_B_ref_mid[32][31];
                        }
                    }

                    for (int j = 64; j < 192; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 32; i++)
                        {
    #pragma HLS unroll 
                            /*************************padding luma*************************************/
                            // swOutLv0_temp[j][i] = refWinY0_B_ref_mid[j][0];
                            // swOutLv0_temp[j][i+32] = refWinY0_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i+64] = refWinY0_B_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i+96] = refWinY1_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_B_ref_mid[j][i+32];
                            // swOutLv0_temp[j][i+160] = refWinY1_B_ref_mid[j][63];
                            // swOutLv0_temp[j][i+192] = refWinY1_B_ref_mid[j][63];
                            // swOutLv0_temp[j][i+224] = refWinY1_B_ref_mid[j][63];

                            swOutLv0_ime_temp[j][i] = refWinY0_B_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i+32] = refWinY0_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+64] = refWinY0_B_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i+96] = refWinY1_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_B_ref_mid[j][i+32];
                            swOutLv0_ime_temp[j][i+160] = refWinY1_B_ref_mid[j][63];
                            swOutLv0_ime_temp[j][i+192] = refWinY1_B_ref_mid[j][63];
                            swOutLv0_ime_temp[j][i+224] = refWinY1_B_ref_mid[j][63];
                        }
                    }

                    for (int j = 32; j < 96; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 16; i++)
                        {
    #pragma HLS unroll 
                            /*************************padding chroma*************************************/
                            refWinU_temp[j*128+i] = refWinU0_B_ref_mid[j][0];
                            refWinU_temp[j*128+i + 16] = refWinU0_B_ref_mid[j][i];
                            refWinU_temp[j*128+i+32] = refWinU0_B_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 48] = refWinU1_B_ref_mid[j][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_B_ref_mid[j][i+16];
                            refWinU_temp[j*128+i + 80] = refWinU1_B_ref_mid[j][31];
                            refWinU_temp[j*128+i + 96] = refWinU1_B_ref_mid[j][31];
                            refWinU_temp[j*128+i + 112] = refWinU1_B_ref_mid[j][31];
                                        
                            refWinV_temp[j*128+i] = refWinV0_B_ref_mid[j][0];
                            refWinV_temp[j*128+i + 16] = refWinV0_B_ref_mid[j][i];
                            refWinV_temp[j*128+i + 32] = refWinV0_B_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 48] = refWinV1_B_ref_mid[j][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_B_ref_mid[j][i+16];
                            refWinV_temp[j*128+i + 80] = refWinV1_B_ref_mid[j][31];
                            refWinV_temp[j*128+i + 96] = refWinV1_B_ref_mid[j][31];
                            refWinV_temp[j*128+i + 112] = refWinV1_B_ref_mid[j][31];
                        }
                    }
                }
            }
            else
            {
                if ((pix_x + 128) <= (luma_width_padded - 64))
                {
                    for (int j = 0; j < 64; j++)
                    {
        #pragma HLS pipeline
                        for (int i = 0; i < 32; i++)
                        {
        #pragma HLS unroll
                            /******************************padding luma***************************/
                            // swOutLv0_temp[j][i] = refWinY32x192_B_ref_mid[64][i];
                            // swOutLv0_temp[j][i+32] = refWinY0_B_ref_mid[64][i];
                            // swOutLv0_temp[j][i+64] = refWinY0_B_ref_mid[64][i + 32];
                            // swOutLv0_temp[j][i+96] = refWinY1_B_ref_mid[64][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_B_ref_mid[64][i + 32];
                            // swOutLv0_temp[j][i+160] = refWinY2_B_ref_mid[64][i];
                            // swOutLv0_temp[j][i+192] = refWinY2_B_ref_mid[64][i + 32];
                            // swOutLv0_temp[j][i+224] = refWinY3_B_ref_mid[64][i];

                            swOutLv0_ime_temp[j][i] = refWinY32x192_B_ref_mid[64][i];
                            swOutLv0_ime_temp[j][i+32] = refWinY0_B_ref_mid[64][i];
                            swOutLv0_ime_temp[j][i+64] = refWinY0_B_ref_mid[64][i + 32];
                            swOutLv0_ime_temp[j][i+96] = refWinY1_B_ref_mid[64][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_B_ref_mid[64][i + 32];
                            swOutLv0_ime_temp[j][i+160] = refWinY2_B_ref_mid[64][i];
                            swOutLv0_ime_temp[j][i+192] = refWinY2_B_ref_mid[64][i + 32];
                            swOutLv0_ime_temp[j][i+224] = refWinY3_B_ref_mid[64][i];
                        }
                    }

                    for (int j = 0; j < 32; j++)
                    {
            #pragma HLS pipeline
                        for (int i = 0; i < 16; i++)
                        {
            #pragma HLS unroll
                            /******************************padding chroma***************************/
                            refWinU_temp[j*128+i] = refWinU16x96_B_ref_mid[32][i];
                            refWinU_temp[j*128+i + 16] = refWinU0_B_ref_mid[32][i];
                            refWinU_temp[j*128+i + 32] = refWinU0_B_ref_mid[32][i + 16];
                            refWinU_temp[j*128+i + 48] = refWinU1_B_ref_mid[32][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_B_ref_mid[32][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU2_B_ref_mid[32][i];
                            refWinU_temp[j*128+i + 96] = refWinU2_B_ref_mid[32][i + 16];
                            refWinU_temp[j*128+i + 112] = refWinU3_B_ref_mid[32][i];

                            refWinV_temp[j*128+i] = refWinV16x96_B_ref_mid[32][i];
                            refWinV_temp[j*128+i + 16] = refWinV0_B_ref_mid[32][i];
                            refWinV_temp[j*128+i + 32] = refWinV0_B_ref_mid[32][i + 16];
                            refWinV_temp[j*128+i + 48] = refWinV1_B_ref_mid[32][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_B_ref_mid[32][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV2_B_ref_mid[32][i];
                            refWinV_temp[j*128+i + 96] = refWinV2_B_ref_mid[32][i + 16];
                            refWinV_temp[j*128+i + 112] = refWinV3_B_ref_mid[32][i];
                        }
                    }

                    for (int j = 64; j < 192; j++)
                    {
        #pragma HLS pipeline
                        for (int i = 0; i < 32; i++)
                        {
        #pragma HLS unroll
                            /******************************padding luma***************************/
                            // swOutLv0_temp[j][i] = refWinY32x192_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i + 32] = refWinY0_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i+64] = refWinY0_B_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 96] = refWinY1_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_B_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 160] = refWinY2_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i+192] = refWinY2_B_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 224] = refWinY3_B_ref_mid[j][i];

                            swOutLv0_ime_temp[j][i] = refWinY32x192_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i + 32] = refWinY0_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+64] = refWinY0_B_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 96] = refWinY1_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_B_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 160] = refWinY2_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+192] = refWinY2_B_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 224] = refWinY3_B_ref_mid[j][i];
                        }
                    }

                    for (int j = 32; j < 96; j++)
                    {
            #pragma HLS pipeline
                        for (int i = 0; i < 16; i++)
                        {
            #pragma HLS unroll
                            /******************************padding chroma***************************/
                            refWinU_temp[j*128+i] = refWinU16x96_B_ref_mid[j][i];
                            refWinU_temp[j*128+i + 16] = refWinU0_B_ref_mid[j][i];
                            refWinU_temp[j*128+i + 32] = refWinU0_B_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 48] = refWinU1_B_ref_mid[j][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_B_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU2_B_ref_mid[j][i];
                            refWinU_temp[j*128+i + 96] = refWinU2_B_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 112] = refWinU3_B_ref_mid[j][i];
                                        
                            refWinV_temp[j*128+i] = refWinV16x96_B_ref_mid[j][i];
                            refWinV_temp[j*128+i + 16] = refWinV0_B_ref_mid[j][i];
                            refWinV_temp[j*128+i + 32] = refWinV0_B_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 48] = refWinV1_B_ref_mid[j][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_B_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV2_B_ref_mid[j][i];
                            refWinV_temp[j*128+i + 96] = refWinV2_B_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 112] = refWinV3_B_ref_mid[j][i];
                        }
                    }
                }
                else if ((pix_x + 128) == luma_width_padded)
                {
                    for (int j = 0; j < 64; j++)
                    {
    #pragma HLS pipeline
                        for (int i = 0; i < 32; i++)
                        {
    #pragma HLS unroll
                            /******************************padding luma***************************/
                            // swOutLv0_temp[j][i] = refWinY32x192_B_ref_mid[64][i];
                            // swOutLv0_temp[j][i + 32] = refWinY0_B_ref_mid[64][i];
                            // swOutLv0_temp[j][i+64] = refWinY0_B_ref_mid[64][i + 32];
                            // swOutLv0_temp[j][i + 96] = refWinY1_B_ref_mid[64][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_B_ref_mid[64][i + 32];
                            // swOutLv0_temp[j][i + 160] = refWinY2_B_ref_mid[64][i];
                            // swOutLv0_temp[j][i+192] = refWinY2_B_ref_mid[64][i + 32];
                            // swOutLv0_temp[j][i + 224] = refWinY2_B_ref_mid[64][63];

                            swOutLv0_ime_temp[j][i] = refWinY32x192_B_ref_mid[64][i];
                            swOutLv0_ime_temp[j][i + 32] = refWinY0_B_ref_mid[64][i];
                            swOutLv0_ime_temp[j][i+64] = refWinY0_B_ref_mid[64][i + 32];
                            swOutLv0_ime_temp[j][i + 96] = refWinY1_B_ref_mid[64][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_B_ref_mid[64][i + 32];
                            swOutLv0_ime_temp[j][i + 160] = refWinY2_B_ref_mid[64][i];
                            swOutLv0_ime_temp[j][i+192] = refWinY2_B_ref_mid[64][i + 32];
                            swOutLv0_ime_temp[j][i + 224] = refWinY2_B_ref_mid[64][63];
                        }
                    }

                    for (int j = 0; j < 32; j++)
                    {
    #pragma HLS pipeline
                        for (int i = 0; i < 16; i++)
                        {
    #pragma HLS unroll
                            /******************************padding chroma***************************/
                            refWinU_temp[j*128+i] = refWinU16x96_B_ref_mid[32][i];
                            refWinU_temp[j*128+i + 16] = refWinU0_B_ref_mid[32][i];
                            refWinU_temp[j*128+i + 32] = refWinU0_B_ref_mid[32][i + 16];
                            refWinU_temp[j*128+i + 48] = refWinU1_B_ref_mid[32][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_B_ref_mid[32][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU2_B_ref_mid[32][i];
                            refWinU_temp[j*128+i + 96] = refWinU2_B_ref_mid[32][i + 16];
                            refWinU_temp[j*128+i + 112] = refWinU2_B_ref_mid[32][31];
                                        
                            refWinV_temp[j*128+i] = refWinV16x96_B_ref_mid[32][i];
                            refWinV_temp[j*128+i + 16] = refWinV0_B_ref_mid[32][i];
                            refWinV_temp[j*128+i + 32] = refWinV0_B_ref_mid[32][i + 16];
                            refWinV_temp[j*128+i + 48] = refWinV1_B_ref_mid[32][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_B_ref_mid[32][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV2_B_ref_mid[32][i];
                            refWinV_temp[j*128+i + 96] = refWinV2_B_ref_mid[32][i + 16];
                            refWinV_temp[j*128+i + 112] = refWinV2_B_ref_mid[32][31];
                        }
                    }

                    for (int j = 64; j < 192; j++)
                    {
    #pragma HLS pipeline
                        for (int i = 0; i < 32; i++)
                        {
    #pragma HLS unroll
                            /******************************padding luma***************************/
                            // swOutLv0_temp[j][i] = refWinY32x192_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i + 32] = refWinY0_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i+64] = refWinY0_B_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 96] = refWinY1_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_B_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 160] = refWinY2_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i+192] = refWinY2_B_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 224] = refWinY2_B_ref_mid[j][63];

                            swOutLv0_ime_temp[j][i] = refWinY32x192_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i + 32] = refWinY0_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+64] = refWinY0_B_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 96] = refWinY1_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_B_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 160] = refWinY2_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+192] = refWinY2_B_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 224] = refWinY2_B_ref_mid[j][63];
                        }
                    }

                    for (int j = 32; j < 96; j++)
                    {
    #pragma HLS pipeline
                        for (int i = 0; i < 16; i++)
                        {
    #pragma HLS unroll
                            /******************************padding chroma***************************/
                            refWinU_temp[j*128+i] = refWinU16x96_B_ref_mid[j][i];
                            refWinU_temp[j*128+i + 16] = refWinU0_B_ref_mid[j][i];
                            refWinU_temp[j*128+i + 32] = refWinU0_B_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 48] = refWinU1_B_ref_mid[j][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_B_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU2_B_ref_mid[j][i];
                            refWinU_temp[j*128+i + 96] = refWinU2_B_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 112] = refWinU2_B_ref_mid[j][31];
                                        
                            refWinV_temp[j*128+i] = refWinV16x96_B_ref_mid[j][i];
                            refWinV_temp[j*128+i + 16] = refWinV0_B_ref_mid[j][i];
                            refWinV_temp[j*128+i + 32] = refWinV0_B_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 48] = refWinV1_B_ref_mid[j][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_B_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV2_B_ref_mid[j][i];
                            refWinV_temp[j*128+i + 96] = refWinV2_B_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 112] = refWinV2_B_ref_mid[j][31];
                        }
                    }
                }
                else if ((pix_x + 128) == luma_width_padded+64)
                {
                    for (int j = 0; j < 64; j++)
                    {
    #pragma HLS pipeline
                        for (int i = 0; i < 32; i++)
                        {
    #pragma HLS unroll
                            /******************************padding luma***************************/
                            // swOutLv0_temp[j][i] = refWinY32x192_B_ref_mid[64][i];
                            // swOutLv0_temp[j][i+32] = refWinY0_B_ref_mid[64][i];
                            // swOutLv0_temp[j][i+64] = refWinY0_B_ref_mid[64][i + 32];
                            // swOutLv0_temp[j][i+96] = refWinY1_B_ref_mid[64][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_B_ref_mid[64][i + 32];
                            // swOutLv0_temp[j][i+160] = refWinY1_B_ref_mid[64][63];
                            // swOutLv0_temp[j][i+192] = refWinY1_B_ref_mid[64][63];
                            // swOutLv0_temp[j][i+224] = refWinY1_B_ref_mid[64][63];

                            swOutLv0_ime_temp[j][i] = refWinY32x192_B_ref_mid[64][i];
                            swOutLv0_ime_temp[j][i+32] = refWinY0_B_ref_mid[64][i];
                            swOutLv0_ime_temp[j][i+64] = refWinY0_B_ref_mid[64][i + 32];
                            swOutLv0_ime_temp[j][i+96] = refWinY1_B_ref_mid[64][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_B_ref_mid[64][i + 32];
                            swOutLv0_ime_temp[j][i+160] = refWinY1_B_ref_mid[64][63];
                            swOutLv0_ime_temp[j][i+192] = refWinY1_B_ref_mid[64][63];
                            swOutLv0_ime_temp[j][i+224] = refWinY1_B_ref_mid[64][63];
                        }
                    }

                    for (int j = 0; j < 32; j++)
                    {
    #pragma HLS pipeline
                        for (int i = 0; i < 16; i++)
                        {
    #pragma HLS unroll
                            /******************************padding chroma***************************/
                            refWinU_temp[j*128+i] = refWinU16x96_B_ref_mid[32][i];
                            refWinU_temp[j*128+i + 16] = refWinU0_B_ref_mid[32][i];
                            refWinU_temp[j*128+i+32] = refWinU0_B_ref_mid[32][i + 16];
                            refWinU_temp[j*128+i + 48] = refWinU1_B_ref_mid[32][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_B_ref_mid[32][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU1_B_ref_mid[32][31];
                            refWinU_temp[j*128+i + 96] = refWinU1_B_ref_mid[32][31];
                            refWinU_temp[j*128+i + 112] = refWinU1_B_ref_mid[32][31];
                                        
                            refWinV_temp[j*128+i] = refWinV16x96_B_ref_mid[32][i];
                            refWinV_temp[j*128+i + 16] = refWinV0_B_ref_mid[32][i];
                            refWinV_temp[j*128+i + 32] = refWinV0_B_ref_mid[32][i + 16];
                            refWinV_temp[j*128+i + 48] = refWinV1_B_ref_mid[32][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_B_ref_mid[32][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV1_B_ref_mid[32][31];
                            refWinV_temp[j*128+i + 96] = refWinV1_B_ref_mid[32][31];
                            refWinV_temp[j*128+i + 112] = refWinV1_B_ref_mid[32][31];
                        }
                    }

                    for (int j = 64; j < 192; j++)
                    {
    #pragma HLS pipeline
                        for (int i = 0; i < 32; i++)
                        {
    #pragma HLS unroll
                            /******************************padding luma***************************/
                            // swOutLv0_temp[j][i] = refWinY32x192_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i+32] = refWinY0_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i+64] = refWinY0_B_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i+96] = refWinY1_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_B_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i+160] = refWinY1_B_ref_mid[j][63];
                            // swOutLv0_temp[j][i+192] = refWinY1_B_ref_mid[j][63];
                            // swOutLv0_temp[j][i+224] = refWinY1_B_ref_mid[j][63];

                            swOutLv0_ime_temp[j][i] = refWinY32x192_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+32] = refWinY0_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+64] = refWinY0_B_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i+96] = refWinY1_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_B_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i+160] = refWinY1_B_ref_mid[j][63];
                            swOutLv0_ime_temp[j][i+192] = refWinY1_B_ref_mid[j][63];
                            swOutLv0_ime_temp[j][i+224] = refWinY1_B_ref_mid[j][63];
                        }
                    }

                    for (int j = 32; j < 96; j++)
                    {
    #pragma HLS pipeline
                        for (int i = 0; i < 16; i++)
                        {
    #pragma HLS unroll
                            /******************************padding chroma***************************/
                            refWinU_temp[j*128+i] = refWinU16x96_B_ref_mid[j][i];
                            refWinU_temp[j*128+i + 16] = refWinU0_B_ref_mid[j][i];
                            refWinU_temp[j*128+i+32] = refWinU0_B_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 48] = refWinU1_B_ref_mid[j][i];
                            refWinU_temp[j*128+i+64] = refWinU1_B_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU1_B_ref_mid[j][31];
                            refWinU_temp[j*128+i+96] = refWinU1_B_ref_mid[j][31];
                            refWinU_temp[j*128+i + 112] = refWinU1_B_ref_mid[j][31];
                                        
                            refWinV_temp[j*128+i] = refWinV16x96_B_ref_mid[j][i];
                            refWinV_temp[j*128+i + 16] = refWinV0_B_ref_mid[j][i];
                            refWinV_temp[j*128+i+32] = refWinV0_B_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 48] = refWinV1_B_ref_mid[j][i];
                            refWinV_temp[j*128+i+64] = refWinV1_B_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV1_B_ref_mid[j][31];
                            refWinV_temp[j*128+i+96] = refWinV1_B_ref_mid[j][31];
                            refWinV_temp[j*128+i + 112] = refWinV1_B_ref_mid[j][31];
                        }
                    }
                }
            }
        }
        //last row
        else if((pix_y + 64)== luma_height_padded)
        {
            if (0 == pix_x)
            {
                if ((pix_x + 128) <= (luma_width_padded - 64))
                {
                    for (int j = 0; j < 128; j++)
                    {
            #pragma HLS pipeline 
                        for (int i = 0; i < 32; i++)
                        {
            #pragma HLS unroll
                            /**********************padding luma*******************************/
                            // swOutLv0_temp[j][i] = refWinY1_B_ref_mid[j][0];
                            // swOutLv0_temp[j][i + 32] = refWinY1_B_ref_mid[j][0];
                            // swOutLv0_temp[j][i+64] = refWinY1_B_ref_mid[j][0];
                            // swOutLv0_temp[j][i + 96] = refWinY1_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_B_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 160] = refWinY2_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i+192] = refWinY2_B_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 224] = refWinY3_B_ref_mid[j][i];

                            swOutLv0_ime_temp[j][i] = refWinY1_B_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i + 32] = refWinY1_B_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i+64] = refWinY1_B_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i + 96] = refWinY1_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_B_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 160] = refWinY2_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+192] = refWinY2_B_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 224] = refWinY3_B_ref_mid[j][i];
                        }
                    }

                    for (int j = 0; j < 64; j++)
                    {
            #pragma HLS pipeline 
                        for (int i = 0; i < 16; i++)
                        {
            #pragma HLS unroll
                            /**********************padding chroma*******************************/
                            refWinU_temp[j*128+i] = refWinU1_B_ref_mid[j][0];
                            refWinU_temp[j*128+i + 16] = refWinU1_B_ref_mid[j][0];
                            refWinU_temp[j*128+i + 32] = refWinU1_B_ref_mid[j][0];
                            refWinU_temp[j*128+i + 48] = refWinU1_B_ref_mid[j][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_B_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU2_B_ref_mid[j][i];
                            refWinU_temp[j*128+i + 96] = refWinU2_B_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 112] = refWinU3_B_ref_mid[j][i];
                                        
                            refWinV_temp[j*128+i] = refWinV1_B_ref_mid[j][0];
                            refWinV_temp[j*128+i + 16] = refWinV1_B_ref_mid[j][0];
                            refWinV_temp[j*128+i + 32] = refWinV1_B_ref_mid[j][0];
                            refWinV_temp[j*128+i + 48] = refWinV1_B_ref_mid[j][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_B_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV2_B_ref_mid[j][i];
                            refWinV_temp[j*128+i + 96] = refWinV2_B_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 112] = refWinV3_B_ref_mid[j][i];
                        }
                    }

                    for (int j = 128; j < 192; j++)
                    {
            #pragma HLS pipeline 
                        for (int i = 0; i < 32; i++)
                        {
            #pragma HLS unroll
                            /**********************padding luma*******************************/
                            // swOutLv0_temp[j][i] = refWinY1_B_ref_mid[127][0];
                            // swOutLv0_temp[j][i + 32] = refWinY1_B_ref_mid[127][0];
                            // swOutLv0_temp[j][i+64] = refWinY1_B_ref_mid[127][0];
                            // swOutLv0_temp[j][i + 96] = refWinY1_B_ref_mid[127][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_B_ref_mid[127][i + 32];
                            // swOutLv0_temp[j][i + 160] = refWinY2_B_ref_mid[127][i];
                            // swOutLv0_temp[j][i+192] = refWinY2_B_ref_mid[127][i + 32];
                            // swOutLv0_temp[j][i + 224] = refWinY3_B_ref_mid[127][i];

                            swOutLv0_ime_temp[j][i] = refWinY1_B_ref_mid[127][0];
                            swOutLv0_ime_temp[j][i + 32] = refWinY1_B_ref_mid[127][0];
                            swOutLv0_ime_temp[j][i+64] = refWinY1_B_ref_mid[127][0];
                            swOutLv0_ime_temp[j][i + 96] = refWinY1_B_ref_mid[127][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_B_ref_mid[127][i + 32];
                            swOutLv0_ime_temp[j][i + 160] = refWinY2_B_ref_mid[127][i];
                            swOutLv0_ime_temp[j][i+192] = refWinY2_B_ref_mid[127][i + 32];
                            swOutLv0_ime_temp[j][i + 224] = refWinY3_B_ref_mid[127][i];
                        }
                    }

                    for (int j = 64; j < 96; j++)
                    {
            #pragma HLS pipeline 
                        for (int i = 0; i < 16; i++)
                        {
            #pragma HLS unroll
                            /**********************padding chroma*******************************/
                            refWinU_temp[j*128+i] = refWinU1_B_ref_mid[63][0];
                            refWinU_temp[j*128+i + 16] = refWinU1_B_ref_mid[63][0];
                            refWinU_temp[j*128+i + 32] = refWinU1_B_ref_mid[63][0];
                            refWinU_temp[j*128+i + 48] = refWinU1_B_ref_mid[63][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_B_ref_mid[63][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU2_B_ref_mid[63][i];
                            refWinU_temp[j*128+i + 96] = refWinU2_B_ref_mid[63][i + 16];
                            refWinU_temp[j*128+i + 112] = refWinU3_B_ref_mid[63][i];
                                        
                            refWinV_temp[j*128+i] = refWinV1_B_ref_mid[63][0];
                            refWinV_temp[j*128+i + 16] = refWinV1_B_ref_mid[63][0];
                            refWinV_temp[j*128+i + 32] = refWinV1_B_ref_mid[63][0];
                            refWinV_temp[j*128+i + 48] = refWinV1_B_ref_mid[63][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_B_ref_mid[63][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV2_B_ref_mid[63][i];
                            refWinV_temp[j*128+i + 96] = refWinV2_B_ref_mid[63][i + 16];
                            refWinV_temp[j*128+i + 112] = refWinV3_B_ref_mid[63][i];
                        }
                    }
                }
                else if ((pix_x + 128) == luma_width_padded)
                {
                    for (int j = 0; j < 128; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 32; i++)
                        {
    #pragma HLS unroll
                            /**********************padding luma*******************************/
                            // swOutLv0_temp[j][i] = refWinY1_B_ref_mid[j][0];
                            // swOutLv0_temp[j][i + 32] = refWinY1_B_ref_mid[j][0];
                            // swOutLv0_temp[j][i+64] = refWinY1_B_ref_mid[j][0];
                            // swOutLv0_temp[j][i + 96] = refWinY1_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_B_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 160] = refWinY2_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i+192] = refWinY2_B_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 224] = refWinY2_B_ref_mid[j][63];

                            swOutLv0_ime_temp[j][i] = refWinY1_B_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i + 32] = refWinY1_B_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i+64] = refWinY1_B_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i + 96] = refWinY1_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_B_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 160] = refWinY2_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+192] = refWinY2_B_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 224] = refWinY2_B_ref_mid[j][63];
                        }
                    }

                    for (int j = 0; j < 64; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 16; i++)
                        {
    #pragma HLS unroll
                            /**********************padding chroma*******************************/
                            refWinU_temp[j*128+i] = refWinU1_B_ref_mid[j][0];
                            refWinU_temp[j*128+i + 16] = refWinU1_B_ref_mid[j][0];
                            refWinU_temp[j*128+i + 32] = refWinU1_B_ref_mid[j][0];
                            refWinU_temp[j*128+i + 48] = refWinU1_B_ref_mid[j][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_B_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU2_B_ref_mid[j][i];
                            refWinU_temp[j*128+i + 96] = refWinU2_B_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 112] = refWinU2_B_ref_mid[j][31];
                                        
                            refWinV_temp[j*128+i] = refWinV1_B_ref_mid[j][0];
                            refWinV_temp[j*128+i + 16] = refWinV1_B_ref_mid[j][0];
                            refWinV_temp[j*128+i + 32] = refWinV1_B_ref_mid[j][0];
                            refWinV_temp[j*128+i + 48] = refWinV1_B_ref_mid[j][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_B_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV2_B_ref_mid[j][i];
                            refWinV_temp[j*128+i + 96] = refWinV2_B_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 112] = refWinV2_B_ref_mid[j][31];
                        }
                    }

                    for (int j = 128; j < 192; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 32; i++)
                        {
    #pragma HLS unroll
                            /**********************padding luma*******************************/
                            // swOutLv0_temp[j][i] = refWinY1_B_ref_mid[127][0];
                            // swOutLv0_temp[j][i + 32] = refWinY1_B_ref_mid[127][0];
                            // swOutLv0_temp[j][i+64] = refWinY1_B_ref_mid[127][0];
                            // swOutLv0_temp[j][i + 96] = refWinY1_B_ref_mid[127][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_B_ref_mid[127][i + 32];
                            // swOutLv0_temp[j][i + 160] = refWinY2_B_ref_mid[127][i];
                            // swOutLv0_temp[j][i+192] = refWinY2_B_ref_mid[127][i + 32];
                            // swOutLv0_temp[j][i + 224] = refWinY2_B_ref_mid[127][63];

                            swOutLv0_ime_temp[j][i] = refWinY1_B_ref_mid[127][0];
                            swOutLv0_ime_temp[j][i + 32] = refWinY1_B_ref_mid[127][0];
                            swOutLv0_ime_temp[j][i+64] = refWinY1_B_ref_mid[127][0];
                            swOutLv0_ime_temp[j][i + 96] = refWinY1_B_ref_mid[127][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_B_ref_mid[127][i + 32];
                            swOutLv0_ime_temp[j][i + 160] = refWinY2_B_ref_mid[127][i];
                            swOutLv0_ime_temp[j][i+192] = refWinY2_B_ref_mid[127][i + 32];
                            swOutLv0_ime_temp[j][i + 224] = refWinY2_B_ref_mid[127][63];
                        }
                    }

                    for (int j = 64; j < 96; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 16; i++)
                        {
    #pragma HLS unroll
                            /**********************padding chroma*******************************/
                            refWinU_temp[j*128+i] = refWinU1_B_ref_mid[63][0];
                            refWinU_temp[j*128+i + 16] = refWinU1_B_ref_mid[63][0];
                            refWinU_temp[j*128+i + 32] = refWinU1_B_ref_mid[63][0];
                            refWinU_temp[j*128+i + 48] = refWinU1_B_ref_mid[63][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_B_ref_mid[63][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU2_B_ref_mid[63][i];
                            refWinU_temp[j*128+i + 96] = refWinU2_B_ref_mid[63][i + 16];
                            refWinU_temp[j*128+i + 112] = refWinU2_B_ref_mid[63][31];
                                        
                            refWinV_temp[j*128+i] = refWinV1_B_ref_mid[63][0];
                            refWinV_temp[j*128+i + 16] = refWinV1_B_ref_mid[63][0];
                            refWinV_temp[j*128+i + 32] = refWinV1_B_ref_mid[63][0];
                            refWinV_temp[j*128+i + 48] = refWinV1_B_ref_mid[63][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_B_ref_mid[63][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV2_B_ref_mid[63][i];
                            refWinV_temp[j*128+i + 96] = refWinV2_B_ref_mid[63][i + 16];
                            refWinV_temp[j*128+i + 112] = refWinV2_B_ref_mid[63][31];
                        }
                    }
                }
                else if ((pix_x + 128) == (luma_width_padded + 64))
                {
                    for (int j = 0; j < 128; j++)
                    {
        #pragma HLS pipeline 
                        for (int i = 0; i < 32; i++)
                        {
        #pragma HLS unroll
                            /**********************padding luma*******************************/
                            // swOutLv0_temp[j][i] = refWinY1_B_ref_mid[j][0];
                            // swOutLv0_temp[j][i + 32] = refWinY1_B_ref_mid[j][0];
                            // swOutLv0_temp[j][i+64] = refWinY1_B_ref_mid[j][0];
                            // swOutLv0_temp[j][i + 96] = refWinY1_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_B_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 160] = refWinY1_B_ref_mid[j][63];
                            // swOutLv0_temp[j][i+192] = refWinY1_B_ref_mid[j][63];
                            // swOutLv0_temp[j][i + 224] = refWinY1_B_ref_mid[j][63];

                            swOutLv0_ime_temp[j][i] = refWinY1_B_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i + 32] = refWinY1_B_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i+64] = refWinY1_B_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i + 96] = refWinY1_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_B_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 160] = refWinY1_B_ref_mid[j][63];
                            swOutLv0_ime_temp[j][i+192] = refWinY1_B_ref_mid[j][63];
                            swOutLv0_ime_temp[j][i + 224] = refWinY1_B_ref_mid[j][63];
                        }
                    }

                    for (int j = 0; j < 64; j++)
                    {
        #pragma HLS pipeline 
                        for (int i = 0; i < 16; i++)
                        {
        #pragma HLS unroll
                            /**********************padding chroma*******************************/
                            refWinU_temp[j*128+i] = refWinU1_B_ref_mid[j][0];
                            refWinU_temp[j*128+i + 16] = refWinU1_B_ref_mid[j][0];
                            refWinU_temp[j*128+i + 32] = refWinU1_B_ref_mid[j][0];
                            refWinU_temp[j*128+i + 48] = refWinU1_B_ref_mid[j][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_B_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU1_B_ref_mid[j][31];
                            refWinU_temp[j*128+i + 96] = refWinU1_B_ref_mid[j][31];
                            refWinU_temp[j*128+i + 112] = refWinU1_B_ref_mid[j][31];
                                        
                            refWinV_temp[j*128+i] = refWinV1_B_ref_mid[j][0];
                            refWinV_temp[j*128+i + 16] = refWinV1_B_ref_mid[j][0];
                            refWinV_temp[j*128+i + 32] = refWinV1_B_ref_mid[j][0];
                            refWinV_temp[j*128+i + 48] = refWinV1_B_ref_mid[j][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_B_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV1_B_ref_mid[j][31];
                            refWinV_temp[j*128+i + 96] = refWinV1_B_ref_mid[j][31];
                            refWinV_temp[j*128+i + 112] = refWinV1_B_ref_mid[j][31];
                        }
                    }

                    for (int j = 128; j < 192; j++)
                    {
        #pragma HLS pipeline 
                        for (int i = 0; i < 32; i++)
                        {
        #pragma HLS unroll
                            /**********************padding luma*******************************/
                            // swOutLv0_temp[j][i] = refWinY1_B_ref_mid[127][0];
                            // swOutLv0_temp[j][i + 32] = refWinY1_B_ref_mid[127][0];
                            // swOutLv0_temp[j][i+64] = refWinY1_B_ref_mid[127][0];
                            // swOutLv0_temp[j][i + 96] = refWinY1_B_ref_mid[127][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_B_ref_mid[127][i + 32];
                            // swOutLv0_temp[j][i + 160] = refWinY1_B_ref_mid[127][63];
                            // swOutLv0_temp[j][i+192] = refWinY1_B_ref_mid[127][63];
                            // swOutLv0_temp[j][i + 224] = refWinY1_B_ref_mid[127][63];

                            swOutLv0_ime_temp[j][i] = refWinY1_B_ref_mid[127][0];
                            swOutLv0_ime_temp[j][i + 32] = refWinY1_B_ref_mid[127][0];
                            swOutLv0_ime_temp[j][i+64] = refWinY1_B_ref_mid[127][0];
                            swOutLv0_ime_temp[j][i + 96] = refWinY1_B_ref_mid[127][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_B_ref_mid[127][i + 32];
                            swOutLv0_ime_temp[j][i + 160] = refWinY1_B_ref_mid[127][63];
                            swOutLv0_ime_temp[j][i+192] = refWinY1_B_ref_mid[127][63];
                            swOutLv0_ime_temp[j][i + 224] = refWinY1_B_ref_mid[127][63];
                        }
                    }

                    for (int j = 64; j < 96; j++)
                    {
        #pragma HLS pipeline 
                        for (int i = 0; i < 16; i++)
                        {
        #pragma HLS unroll
                            /**********************padding chroma*******************************/
                            refWinU_temp[j*128+i] = refWinU1_B_ref_mid[63][0];
                            refWinU_temp[j*128+i + 16] = refWinU1_B_ref_mid[63][0];
                            refWinU_temp[j*128+i + 32] = refWinU1_B_ref_mid[63][0];
                            refWinU_temp[j*128+i + 48] = refWinU1_B_ref_mid[63][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_B_ref_mid[63][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU1_B_ref_mid[63][31];
                            refWinU_temp[j*128+i + 96] = refWinU1_B_ref_mid[63][31];
                            refWinU_temp[j*128+i + 112] = refWinU1_B_ref_mid[63][31];
                                        
                            refWinV_temp[j*128+i] = refWinV1_B_ref_mid[63][0];
                            refWinV_temp[j*128+i + 16] = refWinV1_B_ref_mid[63][0];
                            refWinV_temp[j*128+i + 32] = refWinV1_B_ref_mid[63][0];
                            refWinV_temp[j*128+i + 48] = refWinV1_B_ref_mid[63][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_B_ref_mid[63][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV1_B_ref_mid[63][31];
                            refWinV_temp[j*128+i + 96] = refWinV1_B_ref_mid[63][31];
                            refWinV_temp[j*128+i + 112] = refWinV1_B_ref_mid[63][31];
                        }
                    }
                }
            }
            else if (64 == pix_x)
            {
                if ((pix_x + 128) <= (luma_width_padded - 64))
                {
                    for (int j = 0; j < 128; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 32; i++)
                        {
    #pragma HLS unroll
                            /*********************padding luma********************/
                            // swOutLv0_temp[j][i] = refWinY0_B_ref_mid[j][0];
                            // swOutLv0_temp[j][i + 32] = refWinY0_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i+64] = refWinY0_B_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 96] = refWinY1_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_B_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 160] = refWinY2_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i+192] = refWinY2_B_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 224] = refWinY3_B_ref_mid[j][i];

                            swOutLv0_ime_temp[j][i] = refWinY0_B_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i + 32] = refWinY0_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+64] = refWinY0_B_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 96] = refWinY1_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_B_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 160] = refWinY2_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+192] = refWinY2_B_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 224] = refWinY3_B_ref_mid[j][i];
                        }
                    }

                    for (int j = 0; j < 64; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 16; i++)
                        {
    #pragma HLS unroll
                            /*********************padding chroma********************/
                            refWinU_temp[j*128+i] = refWinU0_B_ref_mid[j][0];
                            refWinU_temp[j*128+i + 16] = refWinU0_B_ref_mid[j][i];
                            refWinU_temp[j*128+i + 32] = refWinU0_B_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 48] = refWinU1_B_ref_mid[j][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_B_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU2_B_ref_mid[j][i];
                            refWinU_temp[j*128+i + 96] = refWinU2_B_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 112] = refWinU3_B_ref_mid[j][i];
                                        
                            refWinV_temp[j*128+i] = refWinV0_B_ref_mid[j][0];
                            refWinV_temp[j*128+i + 16] = refWinV0_B_ref_mid[j][i];
                            refWinV_temp[j*128+i + 32] = refWinV0_B_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 48] = refWinV1_B_ref_mid[j][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_B_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV2_B_ref_mid[j][i];
                            refWinV_temp[j*128+i + 96] = refWinV2_B_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 112] = refWinV3_B_ref_mid[j][i];
                        }
                    }

                    for (int j = 128; j < 192; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 32; i++)
                        {
    #pragma HLS unroll
                            /*********************padding luma********************/
                            // swOutLv0_temp[j][i] = refWinY0_B_ref_mid[127][0];
                            // swOutLv0_temp[j][i + 32] = refWinY0_B_ref_mid[127][i];
                            // swOutLv0_temp[j][i+64] = refWinY0_B_ref_mid[127][i + 32];
                            // swOutLv0_temp[j][i + 96] = refWinY1_B_ref_mid[127][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_B_ref_mid[127][i + 32];
                            // swOutLv0_temp[j][i + 160] = refWinY2_B_ref_mid[127][i];
                            // swOutLv0_temp[j][i+192] = refWinY2_B_ref_mid[127][i + 32];
                            // swOutLv0_temp[j][i + 224] = refWinY3_B_ref_mid[127][i];

                            swOutLv0_ime_temp[j][i] = refWinY0_B_ref_mid[127][0];
                            swOutLv0_ime_temp[j][i + 32] = refWinY0_B_ref_mid[127][i];
                            swOutLv0_ime_temp[j][i+64] = refWinY0_B_ref_mid[127][i + 32];
                            swOutLv0_ime_temp[j][i + 96] = refWinY1_B_ref_mid[127][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_B_ref_mid[127][i + 32];
                            swOutLv0_ime_temp[j][i + 160] = refWinY2_B_ref_mid[127][i];
                            swOutLv0_ime_temp[j][i+192] = refWinY2_B_ref_mid[127][i + 32];
                            swOutLv0_ime_temp[j][i + 224] = refWinY3_B_ref_mid[127][i];
                        }
                    }

                    for (int j = 64; j < 96; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 16; i++)
                        {
    #pragma HLS unroll
                            /*********************padding chroma********************/
                            refWinU_temp[j*128+i] = refWinU0_B_ref_mid[63][0];
                            refWinU_temp[j*128+i + 16] = refWinU0_B_ref_mid[63][i];
                            refWinU_temp[j*128+i + 32] = refWinU0_B_ref_mid[63][i + 16];
                            refWinU_temp[j*128+i + 48] = refWinU1_B_ref_mid[63][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_B_ref_mid[63][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU2_B_ref_mid[63][i];
                            refWinU_temp[j*128+i + 96] = refWinU2_B_ref_mid[63][i + 16];
                            refWinU_temp[j*128+i + 112] = refWinU3_B_ref_mid[63][i];
                                        
                            refWinV_temp[j*128+i] = refWinV0_B_ref_mid[63][0];
                            refWinV_temp[j*128+i + 16] = refWinV0_B_ref_mid[63][i];
                            refWinV_temp[j*128+i + 32] = refWinV0_B_ref_mid[63][i + 16];
                            refWinV_temp[j*128+i + 48] = refWinV1_B_ref_mid[63][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_B_ref_mid[63][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV2_B_ref_mid[63][i];
                            refWinV_temp[j*128+i + 96] = refWinV2_B_ref_mid[63][i + 16];
                            refWinV_temp[j*128+i + 112] = refWinV3_B_ref_mid[63][i];
                        }
                    }
                }
                else if ((pix_x + 128) == luma_width_padded)
                {
                    for (int j = 0; j < 128; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 32; i++)
                        {
    #pragma HLS unroll
                            /*********************padding luma********************/
                            // swOutLv0_temp[j][i] = refWinY0_B_ref_mid[j][0];
                            // swOutLv0_temp[j][i + 32] = refWinY0_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i+64] = refWinY0_B_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 96] = refWinY1_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_B_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 160] = refWinY2_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i+192] = refWinY2_B_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 224] = refWinY2_B_ref_mid[j][63];

                            swOutLv0_ime_temp[j][i] = refWinY0_B_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i + 32] = refWinY0_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+64] = refWinY0_B_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 96] = refWinY1_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_B_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 160] = refWinY2_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+192] = refWinY2_B_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 224] = refWinY2_B_ref_mid[j][63];
                        }
                    }

                    for (int j = 0; j < 64; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 16; i++)
                        {
    #pragma HLS unroll
                            /*********************padding chroma********************/
                            refWinU_temp[j*128+i] = refWinU0_B_ref_mid[j][0];
                            refWinU_temp[j*128+i + 16] = refWinU0_B_ref_mid[j][i];
                            refWinU_temp[j*128+i + 32] = refWinU0_B_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 48] = refWinU1_B_ref_mid[j][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_B_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU2_B_ref_mid[j][i];
                            refWinU_temp[j*128+i + 96] = refWinU2_B_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 112] = refWinU2_B_ref_mid[j][31];
                                        
                            refWinV_temp[j*128+i] = refWinV0_B_ref_mid[j][0];
                            refWinV_temp[j*128+i + 16] = refWinV0_B_ref_mid[j][i];
                            refWinV_temp[j*128+i + 32] = refWinV0_B_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 48] = refWinV1_B_ref_mid[j][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_B_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV2_B_ref_mid[j][i];
                            refWinV_temp[j*128+i + 96] = refWinV2_B_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 112] = refWinV2_B_ref_mid[j][31];
                        }
                    }

                    for (int j = 128; j < 192; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 32; i++)
                        {
    #pragma HLS unroll
                            /*********************padding luma********************/
                            // swOutLv0_temp[j][i] = refWinY0_B_ref_mid[127][0];
                            // swOutLv0_temp[j][i + 32] = refWinY0_B_ref_mid[127][i];
                            // swOutLv0_temp[j][i+64] = refWinY0_B_ref_mid[127][i + 32];
                            // swOutLv0_temp[j][i + 96] = refWinY1_B_ref_mid[127][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_B_ref_mid[127][i + 32];
                            // swOutLv0_temp[j][i + 160] = refWinY2_B_ref_mid[127][i];
                            // swOutLv0_temp[j][i+192] = refWinY2_B_ref_mid[127][i + 32];
                            // swOutLv0_temp[j][i + 224] = refWinY2_B_ref_mid[127][63];

                            swOutLv0_ime_temp[j][i] = refWinY0_B_ref_mid[127][0];
                            swOutLv0_ime_temp[j][i + 32] = refWinY0_B_ref_mid[127][i];
                            swOutLv0_ime_temp[j][i+64] = refWinY0_B_ref_mid[127][i + 32];
                            swOutLv0_ime_temp[j][i + 96] = refWinY1_B_ref_mid[127][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_B_ref_mid[127][i + 32];
                            swOutLv0_ime_temp[j][i + 160] = refWinY2_B_ref_mid[127][i];
                            swOutLv0_ime_temp[j][i+192] = refWinY2_B_ref_mid[127][i + 32];
                            swOutLv0_ime_temp[j][i + 224] = refWinY2_B_ref_mid[127][63];
                        }
                    }

                    for (int j = 64; j < 96; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 16; i++)
                        {
    #pragma HLS unroll
                            /*********************padding chroma********************/
                            refWinU_temp[j*128+i] = refWinU0_B_ref_mid[63][0];
                            refWinU_temp[j*128+i + 16] = refWinU0_B_ref_mid[63][i];
                            refWinU_temp[j*128+i + 32] = refWinU0_B_ref_mid[63][i + 16];
                            refWinU_temp[j*128+i + 48] = refWinU1_B_ref_mid[63][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_B_ref_mid[63][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU2_B_ref_mid[63][i];
                            refWinU_temp[j*128+i + 96] = refWinU2_B_ref_mid[63][i + 16];
                            refWinU_temp[j*128+i + 112] = refWinU2_B_ref_mid[63][31];
                                        
                            refWinV_temp[j*128+i] = refWinV0_B_ref_mid[63][0];
                            refWinV_temp[j*128+i + 16] = refWinV0_B_ref_mid[63][i];
                            refWinV_temp[j*128+i + 32] = refWinV0_B_ref_mid[63][i + 16];
                            refWinV_temp[j*128+i + 48] = refWinV1_B_ref_mid[63][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_B_ref_mid[63][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV2_B_ref_mid[63][i];
                            refWinV_temp[j*128+i + 96] = refWinV2_B_ref_mid[63][i + 16];
                            refWinV_temp[j*128+i + 112] = refWinV2_B_ref_mid[63][31];
                        }
                    }
                }
                else if ((pix_x + 128) == (luma_width_padded+64))
                {
                    for (int j = 0; j < 128; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 32; i++)
                        {
    #pragma HLS unroll
                            /*********************padding luma********************/
                            // swOutLv0_temp[j][i] = refWinY0_B_ref_mid[j][0];
                            // swOutLv0_temp[j][i + 32] = refWinY0_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i+64] = refWinY0_B_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 96] = refWinY1_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_B_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 160] = refWinY1_B_ref_mid[j][63];
                            // swOutLv0_temp[j][i+192] = refWinY1_B_ref_mid[j][63];
                            // swOutLv0_temp[j][i + 224] = refWinY1_B_ref_mid[j][63];

                            swOutLv0_ime_temp[j][i] = refWinY0_B_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i + 32] = refWinY0_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+64] = refWinY0_B_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 96] = refWinY1_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_B_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 160] = refWinY1_B_ref_mid[j][63];
                            swOutLv0_ime_temp[j][i+192] = refWinY1_B_ref_mid[j][63];
                            swOutLv0_ime_temp[j][i + 224] = refWinY1_B_ref_mid[j][63];
                        }
                    }

                    for (int j = 0; j < 64; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 16; i++)
                        {
    #pragma HLS unroll
                            /*********************padding chroma********************/
                            refWinU_temp[j*128+i] = refWinU0_B_ref_mid[j][0];
                            refWinU_temp[j*128+i + 16] = refWinU0_B_ref_mid[j][i];
                            refWinU_temp[j*128+i + 32] = refWinU0_B_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 48] = refWinU1_B_ref_mid[j][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_B_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU1_B_ref_mid[j][31];
                            refWinU_temp[j*128+i + 96] = refWinU1_B_ref_mid[j][31];
                            refWinU_temp[j*128+i + 112] = refWinU1_B_ref_mid[j][31];
                                        
                            refWinV_temp[j*128+i] = refWinV0_B_ref_mid[j][0];
                            refWinV_temp[j*128+i + 16] = refWinV0_B_ref_mid[j][i];
                            refWinV_temp[j*128+i + 32] = refWinV0_B_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 48] = refWinV1_B_ref_mid[j][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_B_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV1_B_ref_mid[j][31];
                            refWinV_temp[j*128+i + 96] = refWinV1_B_ref_mid[j][31];
                            refWinV_temp[j*128+i + 112] = refWinV1_B_ref_mid[j][31];
                        }
                    }

                    for (int j = 128; j < 192; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 32; i++)
                        {
    #pragma HLS unroll
                            /*********************padding luma********************/
                            // swOutLv0_temp[j][i] = refWinY0_B_ref_mid[127][0];
                            // swOutLv0_temp[j][i + 32] = refWinY0_B_ref_mid[127][i];
                            // swOutLv0_temp[j][i+64] = refWinY0_B_ref_mid[127][i + 32];
                            // swOutLv0_temp[j][i + 96] = refWinY1_B_ref_mid[127][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_B_ref_mid[127][i + 32];
                            // swOutLv0_temp[j][i + 160] = refWinY1_B_ref_mid[127][63];
                            // swOutLv0_temp[j][i+192] = refWinY1_B_ref_mid[127][63];
                            // swOutLv0_temp[j][i + 224] = refWinY1_B_ref_mid[127][63];

                            swOutLv0_ime_temp[j][i] = refWinY0_B_ref_mid[127][0];
                            swOutLv0_ime_temp[j][i + 32] = refWinY0_B_ref_mid[127][i];
                            swOutLv0_ime_temp[j][i+64] = refWinY0_B_ref_mid[127][i + 32];
                            swOutLv0_ime_temp[j][i + 96] = refWinY1_B_ref_mid[127][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_B_ref_mid[127][i + 32];
                            swOutLv0_ime_temp[j][i + 160] = refWinY1_B_ref_mid[127][63];
                            swOutLv0_ime_temp[j][i+192] = refWinY1_B_ref_mid[127][63];
                            swOutLv0_ime_temp[j][i + 224] = refWinY1_B_ref_mid[127][63];
                        }
                    }

                    for (int j = 64; j < 96; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 16; i++)
                        {
    #pragma HLS unroll
                            /*********************padding chroma********************/
                            refWinU_temp[j*128+i] = refWinU0_B_ref_mid[63][0];
                            refWinU_temp[j*128+i + 16] = refWinU0_B_ref_mid[63][i];
                            refWinU_temp[j*128+i + 32] = refWinU0_B_ref_mid[63][i + 16];
                            refWinU_temp[j*128+i + 48] = refWinU1_B_ref_mid[63][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_B_ref_mid[63][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU1_B_ref_mid[63][31];
                            refWinU_temp[j*128+i + 96] = refWinU1_B_ref_mid[63][31];
                            refWinU_temp[j*128+i + 112] = refWinU1_B_ref_mid[63][31];
                                        
                            refWinV_temp[j*128+i] = refWinV0_B_ref_mid[63][0];
                            refWinV_temp[j*128+i + 16] = refWinV0_B_ref_mid[63][i];
                            refWinV_temp[j*128+i + 32] = refWinV0_B_ref_mid[63][i + 16];
                            refWinV_temp[j*128+i + 48] = refWinV1_B_ref_mid[63][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_B_ref_mid[63][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV1_B_ref_mid[63][31];
                            refWinV_temp[j*128+i + 96] = refWinV1_B_ref_mid[63][31];
                            refWinV_temp[j*128+i + 112] = refWinV1_B_ref_mid[63][31];
                        }
                    }
                }
            }
            else
            {
                if ((pix_x + 128) <= (luma_width_padded - 64))
                {
                    for (int j = 0; j < 128; j++)
                    {
        #pragma HLS pipeline 
                        for (int i = 0; i < 32; i++)
                        {
        #pragma HLS unroll
                            /*****************************padding luma******************************************/
                            // swOutLv0_temp[j][i] = refWinY32x192_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i + 32] = refWinY0_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i+64] = refWinY0_B_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 96] = refWinY1_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_B_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 160] = refWinY2_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i+192] = refWinY2_B_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 224] = refWinY3_B_ref_mid[j][i];

                            swOutLv0_ime_temp[j][i] = refWinY32x192_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i + 32] = refWinY0_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+64] = refWinY0_B_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 96] = refWinY1_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_B_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 160] = refWinY2_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+192] = refWinY2_B_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 224] = refWinY3_B_ref_mid[j][i];
                        }
                    }

                    for (int j = 0; j < 64; j++)
                    {
        #pragma HLS pipeline 
                        for (int i = 0; i < 16; i++)
                        {
        #pragma HLS unroll
                            /*****************************padding chroma******************************************/
                                refWinU_temp[j*128+i] = refWinU16x96_B_ref_mid[j][i];
                                refWinU_temp[j*128+i + 16] = refWinU0_B_ref_mid[j][i];
                                refWinU_temp[j*128+i + 32] = refWinU0_B_ref_mid[j][i + 16];
                                refWinU_temp[j*128+i + 48] = refWinU1_B_ref_mid[j][i];
                                refWinU_temp[j*128+i + 64] = refWinU1_B_ref_mid[j][i + 16];
                                refWinU_temp[j*128+i + 80] = refWinU2_B_ref_mid[j][i];
                                refWinU_temp[j*128+i + 96] = refWinU2_B_ref_mid[j][i + 16];
                                refWinU_temp[j*128+i + 112] = refWinU3_B_ref_mid[j][i];
                                            
                                refWinV_temp[j*128+i] = refWinV16x96_B_ref_mid[j][i];
                                refWinV_temp[j*128+i + 16] = refWinV0_B_ref_mid[j][i];
                                refWinV_temp[j*128+i + 32] = refWinV0_B_ref_mid[j][i + 16];
                                refWinV_temp[j*128+i + 48] = refWinV1_B_ref_mid[j][i];
                                refWinV_temp[j*128+i + 64] = refWinV1_B_ref_mid[j][i + 16];
                                refWinV_temp[j*128+i + 80] = refWinV2_B_ref_mid[j][i];
                                refWinV_temp[j*128+i + 96] = refWinV2_B_ref_mid[j][i + 16];
                                refWinV_temp[j*128+i + 112] = refWinV3_B_ref_mid[j][i];
                        }
                    }

                    for (int j = 128; j < 192; j++)
                    {
        #pragma HLS pipeline 
                        for (int i = 0; i < 32; i++)
                        {
        #pragma HLS unroll
                            /*****************************padding luma*********************************/
                            // swOutLv0_temp[j][i] = refWinY32x192_B_ref_mid[127][i];
                            // swOutLv0_temp[j][i + 32] = refWinY0_B_ref_mid[127][i];
                            // swOutLv0_temp[j][i+64] = refWinY0_B_ref_mid[127][i + 32];
                            // swOutLv0_temp[j][i + 96] = refWinY1_B_ref_mid[127][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_B_ref_mid[127][i + 32];
                            // swOutLv0_temp[j][i + 160] = refWinY2_B_ref_mid[127][i];
                            // swOutLv0_temp[j][i+192] = refWinY2_B_ref_mid[127][i + 32];
                            // swOutLv0_temp[j][i + 224] = refWinY3_B_ref_mid[127][i];

                            swOutLv0_ime_temp[j][i] = refWinY32x192_B_ref_mid[127][i];
                            swOutLv0_ime_temp[j][i + 32] = refWinY0_B_ref_mid[127][i];
                            swOutLv0_ime_temp[j][i+64] = refWinY0_B_ref_mid[127][i + 32];
                            swOutLv0_ime_temp[j][i + 96] = refWinY1_B_ref_mid[127][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_B_ref_mid[127][i + 32];
                            swOutLv0_ime_temp[j][i + 160] = refWinY2_B_ref_mid[127][i];
                            swOutLv0_ime_temp[j][i+192] = refWinY2_B_ref_mid[127][i + 32];
                            swOutLv0_ime_temp[j][i + 224] = refWinY3_B_ref_mid[127][i];
                        }
                    }

                    for (int j = 64; j < 96; j++)
                    {
        #pragma HLS pipeline 
                        for (int i = 0; i < 16; i++)
                        {
        #pragma HLS unroll
                            /*****************************padding chroma******************************************/
                            refWinU_temp[j*128+i] = refWinU16x96_B_ref_mid[63][i];
                            refWinU_temp[j*128+i + 16] = refWinU0_B_ref_mid[63][i];
                            refWinU_temp[j*128+i + 32] = refWinU0_B_ref_mid[63][i + 16];
                            refWinU_temp[j*128+i + 48] = refWinU1_B_ref_mid[63][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_B_ref_mid[63][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU2_B_ref_mid[63][i];
                            refWinU_temp[j*128+i + 96] = refWinU2_B_ref_mid[63][i + 16];
                            refWinU_temp[j*128+i + 112] = refWinU3_B_ref_mid[63][i];
                                        
                            refWinV_temp[j*128+i] = refWinV16x96_B_ref_mid[63][i];
                            refWinV_temp[j*128+i + 16] = refWinV0_B_ref_mid[63][i];
                            refWinV_temp[j*128+i + 32] = refWinV0_B_ref_mid[63][i + 16];
                            refWinV_temp[j*128+i + 48] = refWinV1_B_ref_mid[63][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_B_ref_mid[63][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV2_B_ref_mid[63][i];
                            refWinV_temp[j*128+i + 96] = refWinV2_B_ref_mid[63][i + 16];
                            refWinV_temp[j*128+i + 112] = refWinV3_B_ref_mid[63][i];
                        }
                    }
                }
                else if ((pix_x + 128) == luma_width_padded)
                {
                    for (int j = 0; j < 128; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 32; i++)
                        {
    #pragma HLS unroll
                            /*****************************padding luma******************************************/
                            // swOutLv0_temp[j][i] = refWinY32x192_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i + 32] = refWinY0_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i+64] = refWinY0_B_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 96] = refWinY1_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_B_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 160] = refWinY2_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i+192] = refWinY2_B_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 224] = refWinY2_B_ref_mid[j][63];

                            swOutLv0_ime_temp[j][i] = refWinY32x192_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i + 32] = refWinY0_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+64] = refWinY0_B_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 96] = refWinY1_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_B_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 160] = refWinY2_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+192] = refWinY2_B_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 224] = refWinY2_B_ref_mid[j][63];
                        }
                    }

                    for (int j = 0; j < 64; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 16; i++)
                        {
    #pragma HLS unroll
                            /*****************************padding chroma******************************************/
                            refWinU_temp[j*128+i] = refWinU16x96_B_ref_mid[j][i];
                            refWinU_temp[j*128+i + 16] = refWinU0_B_ref_mid[j][i];
                            refWinU_temp[j*128+i + 32] = refWinU0_B_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 48] = refWinU1_B_ref_mid[j][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_B_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU2_B_ref_mid[j][i];
                            refWinU_temp[j*128+i + 96] = refWinU2_B_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 112] = refWinU2_B_ref_mid[j][31];
                                        
                            refWinV_temp[j*128+i] = refWinV16x96_B_ref_mid[j][i];
                            refWinV_temp[j*128+i + 16] = refWinV0_B_ref_mid[j][i];
                            refWinV_temp[j*128+i + 32] = refWinV0_B_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 48] = refWinV1_B_ref_mid[j][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_B_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV2_B_ref_mid[j][i];
                            refWinV_temp[j*128+i + 96] = refWinV2_B_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 112] = refWinV2_B_ref_mid[j][31];
                        }
                    }

                    for (int j = 128; j < 192; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 32; i++)
                        {
    #pragma HLS unroll
                            /*****************************padding luma*********************************/
                            // swOutLv0_temp[j][i] = refWinY32x192_B_ref_mid[127][i];
                            // swOutLv0_temp[j][i + 32] = refWinY0_B_ref_mid[127][i];
                            // swOutLv0_temp[j][i+64] = refWinY0_B_ref_mid[127][i + 32];
                            // swOutLv0_temp[j][i + 96] = refWinY1_B_ref_mid[127][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_B_ref_mid[127][i + 32];
                            // swOutLv0_temp[j][i + 160] = refWinY2_B_ref_mid[127][i];
                            // swOutLv0_temp[j][i+192] = refWinY2_B_ref_mid[127][i + 32];
                            // swOutLv0_temp[j][i + 224] = refWinY2_B_ref_mid[127][63];

                            swOutLv0_ime_temp[j][i] = refWinY32x192_B_ref_mid[127][i];
                            swOutLv0_ime_temp[j][i + 32] = refWinY0_B_ref_mid[127][i];
                            swOutLv0_ime_temp[j][i+64] = refWinY0_B_ref_mid[127][i + 32];
                            swOutLv0_ime_temp[j][i + 96] = refWinY1_B_ref_mid[127][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_B_ref_mid[127][i + 32];
                            swOutLv0_ime_temp[j][i + 160] = refWinY2_B_ref_mid[127][i];
                            swOutLv0_ime_temp[j][i+192] = refWinY2_B_ref_mid[127][i + 32];
                            swOutLv0_ime_temp[j][i + 224] = refWinY2_B_ref_mid[127][63];
                        }
                    }

                    for (int j = 64; j < 96; j++)
                    {
    #pragma HLS pipeline 
                        for (int i = 0; i < 16; i++)
                        {
    #pragma HLS unroll
                            /*****************************padding chroma******************************************/
                            refWinU_temp[j*128+i] = refWinU16x96_B_ref_mid[63][i];
                            refWinU_temp[j*128+i + 16] = refWinU0_B_ref_mid[63][i];
                            refWinU_temp[j*128+i + 32] = refWinU0_B_ref_mid[63][i + 16];
                            refWinU_temp[j*128+i + 48] = refWinU1_B_ref_mid[63][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_B_ref_mid[63][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU2_B_ref_mid[63][i];
                            refWinU_temp[j*128+i + 96] = refWinU2_B_ref_mid[63][i + 16];
                            refWinU_temp[j*128+i + 112] = refWinU2_B_ref_mid[63][31];
                                        
                            refWinV_temp[j*128+i] = refWinV16x96_B_ref_mid[63][i];
                            refWinV_temp[j*128+i + 16] = refWinV0_B_ref_mid[63][i];
                            refWinV_temp[j*128+i + 32] = refWinV0_B_ref_mid[63][i + 16];
                            refWinV_temp[j*128+i + 48] = refWinV1_B_ref_mid[63][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_B_ref_mid[63][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV2_B_ref_mid[63][i];
                            refWinV_temp[j*128+i + 96] = refWinV2_B_ref_mid[63][i + 16];
                            refWinV_temp[j*128+i + 112] = refWinV2_B_ref_mid[63][31];
                        }
                    }
                }
                else if ((pix_x + 128) == (luma_width_padded+64))
                {
                    for (int j = 0; j < 128; j++)
                    {
        #pragma HLS pipeline 
                        for (int i = 0; i < 32; i++)
                        {
        #pragma HLS unroll
                            /*****************************padding luma******************************************/
                            // swOutLv0_temp[j][i] = refWinY32x192_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i + 32] = refWinY0_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i+64] = refWinY0_B_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 96] = refWinY1_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_B_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 160] = refWinY1_B_ref_mid[j][63];
                            // swOutLv0_temp[j][i+192] = refWinY1_B_ref_mid[j][63];
                            // swOutLv0_temp[j][i + 224] = refWinY1_B_ref_mid[j][63];

                            swOutLv0_ime_temp[j][i] = refWinY32x192_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i + 32] = refWinY0_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+64] = refWinY0_B_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 96] = refWinY1_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_B_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 160] = refWinY1_B_ref_mid[j][63];
                            swOutLv0_ime_temp[j][i+192] = refWinY1_B_ref_mid[j][63];
                            swOutLv0_ime_temp[j][i + 224] = refWinY1_B_ref_mid[j][63];
                        }
                    }

                    for (int j = 0; j < 64; j++)
                    {
        #pragma HLS pipeline 
                        for (int i = 0; i < 16; i++)
                        {
        #pragma HLS unroll
                            /*****************************padding chroma******************************************/
                            refWinU_temp[j*128+i] = refWinU16x96_B_ref_mid[j][i];
                            refWinU_temp[j*128+i + 16] = refWinU0_B_ref_mid[j][i];
                            refWinU_temp[j*128+i + 32] = refWinU0_B_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 48] = refWinU1_B_ref_mid[j][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_B_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU1_B_ref_mid[j][31];
                            refWinU_temp[j*128+i + 96] = refWinU1_B_ref_mid[j][31];
                            refWinU_temp[j*128+i + 112] = refWinU1_B_ref_mid[j][31];
                                        
                            refWinV_temp[j*128+i] = refWinV16x96_B_ref_mid[j][i];
                            refWinV_temp[j*128+i + 16] = refWinV0_B_ref_mid[j][i];
                            refWinV_temp[j*128+i + 32] = refWinV0_B_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 48] = refWinV1_B_ref_mid[j][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_B_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV1_B_ref_mid[j][31];
                            refWinV_temp[j*128+i + 96] = refWinV1_B_ref_mid[j][31];
                            refWinV_temp[j*128+i + 112] = refWinV1_B_ref_mid[j][31];
                        }
                    }

                    for (int j = 128; j < 192; j++)
                    {
        #pragma HLS pipeline 
                        for (int i = 0; i < 32; i++)
                        {
        #pragma HLS unroll
                            /*****************************padding luma*********************************/
                            // swOutLv0_temp[j][i] = refWinY32x192_B_ref_mid[127][i];
                            // swOutLv0_temp[j][i + 32] = refWinY0_B_ref_mid[127][i];
                            // swOutLv0_temp[j][i+64] = refWinY0_B_ref_mid[127][i + 32];
                            // swOutLv0_temp[j][i + 96] = refWinY1_B_ref_mid[127][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_B_ref_mid[127][i + 32];
                            // swOutLv0_temp[j][i + 160] = refWinY1_B_ref_mid[127][63];
                            // swOutLv0_temp[j][i+192] = refWinY1_B_ref_mid[127][63];
                            // swOutLv0_temp[j][i + 224] = refWinY1_B_ref_mid[127][63];

                            swOutLv0_ime_temp[j][i] = refWinY32x192_B_ref_mid[127][i];
                            swOutLv0_ime_temp[j][i + 32] = refWinY0_B_ref_mid[127][i];
                            swOutLv0_ime_temp[j][i+64] = refWinY0_B_ref_mid[127][i + 32];
                            swOutLv0_ime_temp[j][i + 96] = refWinY1_B_ref_mid[127][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_B_ref_mid[127][i + 32];
                            swOutLv0_ime_temp[j][i + 160] = refWinY1_B_ref_mid[127][63];
                            swOutLv0_ime_temp[j][i+192] = refWinY1_B_ref_mid[127][63];
                            swOutLv0_ime_temp[j][i + 224] = refWinY1_B_ref_mid[127][63];
                        }
                    }

                    for (int j = 64; j < 96; j++)
                    {
        #pragma HLS pipeline 
                        for (int i = 0; i < 16; i++)
                        {
        #pragma HLS unroll
                            /*****************************padding chroma******************************************/
                            refWinU_temp[j*128+i] = refWinU16x96_B_ref_mid[63][i];
                            refWinU_temp[j*128+i + 16] = refWinU0_B_ref_mid[63][i];
                            refWinU_temp[j*128+i + 32] = refWinU0_B_ref_mid[63][i + 16];
                            refWinU_temp[j*128+i + 48] = refWinU1_B_ref_mid[63][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_B_ref_mid[63][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU1_B_ref_mid[63][31];
                            refWinU_temp[j*128+i + 96] = refWinU1_B_ref_mid[63][31];
                            refWinU_temp[j*128+i + 112] = refWinU1_B_ref_mid[63][31];
                                        
                            refWinV_temp[j*128+i] = refWinV16x96_B_ref_mid[63][i];
                            refWinV_temp[j*128+i + 16] = refWinV0_B_ref_mid[63][i];
                            refWinV_temp[j*128+i + 32] = refWinV0_B_ref_mid[63][i + 16];
                            refWinV_temp[j*128+i + 48] = refWinV1_B_ref_mid[63][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_B_ref_mid[63][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV1_B_ref_mid[63][31];
                            refWinV_temp[j*128+i + 96] = refWinV1_B_ref_mid[63][31];
                            refWinV_temp[j*128+i + 112] = refWinV1_B_ref_mid[63][31];
                        }
                    }
                }
            }
        }
        //mid row
        else
        {
            if (0 == pix_x)
            {
                if ((pix_x + 128) <= (luma_width_padded - 64))
                {
                    for (int j = 0; j < 192; j++)
                    {
        #pragma HLS pipeline
                        for (int i = 0; i < 32; i++)
                        {
        #pragma HLS unroll
                            /**************************padding luma*********************************/
                            // swOutLv0_temp[j][i] = refWinY1_B_ref_mid[j][0];
                            // swOutLv0_temp[j][i + 32] = refWinY1_B_ref_mid[j][0];
                            // swOutLv0_temp[j][i+64] = refWinY1_B_ref_mid[j][0];
                            // swOutLv0_temp[j][i + 96] = refWinY1_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_B_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 160] = refWinY2_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i+192] = refWinY2_B_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 224] = refWinY3_B_ref_mid[j][i];

                            swOutLv0_ime_temp[j][i] = refWinY1_B_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i + 32] = refWinY1_B_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i+64] = refWinY1_B_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i + 96] = refWinY1_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_B_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 160] = refWinY2_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+192] = refWinY2_B_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 224] = refWinY3_B_ref_mid[j][i];
                        }
                    }
                    for (int j = 0; j < 96; j++)
                    {
        #pragma HLS pipeline
                        for (int i = 0; i < 16; i++)
                        {
        #pragma HLS unroll
                            /**************************padding chroma*********************************/
                            refWinU_temp[j*128+i] = refWinU1_B_ref_mid[j][0];
                            refWinU_temp[j*128+i + 16] = refWinU1_B_ref_mid[j][0];
                            refWinU_temp[j*128+i + 32] = refWinU1_B_ref_mid[j][0];
                            refWinU_temp[j*128+i + 48] = refWinU1_B_ref_mid[j][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_B_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU2_B_ref_mid[j][i];
                            refWinU_temp[j*128+i + 96] = refWinU2_B_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 112] = refWinU3_B_ref_mid[j][i];
                                        
                            refWinV_temp[j*128+i] = refWinV1_B_ref_mid[j][0];
                            refWinV_temp[j*128+i + 16] = refWinV1_B_ref_mid[j][0];
                            refWinV_temp[j*128+i + 32] = refWinV1_B_ref_mid[j][0];
                            refWinV_temp[j*128+i + 48] = refWinV1_B_ref_mid[j][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_B_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV2_B_ref_mid[j][i];
                            refWinV_temp[j*128+i + 96] = refWinV2_B_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 112] = refWinV3_B_ref_mid[j][i];
                        }
                    }
                }
                else if ((pix_x + 128) == luma_width_padded)
                {
                    for (int j = 0; j < 192; j++)
                    {
    #pragma HLS pipeline
                        for (int i = 0; i < 32; i++)
                        {
    #pragma HLS unroll
                            /**************************padding luma*********************************/
                            // swOutLv0_temp[j][i] = refWinY1_B_ref_mid[j][0];
                            // swOutLv0_temp[j][i + 32] = refWinY1_B_ref_mid[j][0];
                            // swOutLv0_temp[j][i+64] = refWinY1_B_ref_mid[j][0];
                            // swOutLv0_temp[j][i + 96] = refWinY1_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_B_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 160] = refWinY2_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i+192] = refWinY2_B_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 224] = refWinY2_B_ref_mid[j][63];

                            swOutLv0_ime_temp[j][i] = refWinY1_B_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i + 32] = refWinY1_B_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i+64] = refWinY1_B_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i + 96] = refWinY1_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_B_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 160] = refWinY2_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+192] = refWinY2_B_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 224] = refWinY2_B_ref_mid[j][63];
                        }
                    }
                    for (int j = 0; j < 96; j++)
                    {
    #pragma HLS pipeline
                        for (int i = 0; i < 16; i++)
                        {
    #pragma HLS unroll
                            /**************************padding chroma*********************************/
                            refWinU_temp[j*128+i] = refWinU1_B_ref_mid[j][0];
                            refWinU_temp[j*128+i + 16] = refWinU1_B_ref_mid[j][0];
                            refWinU_temp[j*128+i + 32] = refWinU1_B_ref_mid[j][0];
                            refWinU_temp[j*128+i + 48] = refWinU1_B_ref_mid[j][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_B_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU2_B_ref_mid[j][i];
                            refWinU_temp[j*128+i + 96] = refWinU2_B_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 112] = refWinU2_B_ref_mid[j][31];
                                        
                            refWinV_temp[j*128+i] = refWinV1_B_ref_mid[j][0];
                            refWinV_temp[j*128+i + 16] = refWinV1_B_ref_mid[j][0];
                            refWinV_temp[j*128+i + 32] = refWinV1_B_ref_mid[j][0];
                            refWinV_temp[j*128+i + 48] = refWinV1_B_ref_mid[j][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_B_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV2_B_ref_mid[j][i];
                            refWinV_temp[j*128+i + 96] = refWinV2_B_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 112] = refWinV2_B_ref_mid[j][31];
                        }
                    }
                }
                else if ((pix_x + 128) == (luma_width_padded + 64))
                {
                    for (int j = 0; j < 192; j++)
                    {
    #pragma HLS pipeline
                        for (int i = 0; i < 32; i++)
                        {
    #pragma HLS unroll
                            /**************************padding luma*********************************/
                            // swOutLv0_temp[j][i] = refWinY1_B_ref_mid[j][0];
                            // swOutLv0_temp[j][i + 32] = refWinY1_B_ref_mid[j][0];
                            // swOutLv0_temp[j][i+64] = refWinY1_B_ref_mid[j][0];
                            // swOutLv0_temp[j][i + 96] = refWinY1_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_B_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 160] = refWinY1_B_ref_mid[j][63];
                            // swOutLv0_temp[j][i+192] = refWinY1_B_ref_mid[j][63];
                            // swOutLv0_temp[j][i + 224] = refWinY1_B_ref_mid[j][63];

                            swOutLv0_ime_temp[j][i] = refWinY1_B_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i + 32] = refWinY1_B_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i+64] = refWinY1_B_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i + 96] = refWinY1_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_B_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 160] = refWinY1_B_ref_mid[j][63];
                            swOutLv0_ime_temp[j][i+192] = refWinY1_B_ref_mid[j][63];
                            swOutLv0_ime_temp[j][i + 224] = refWinY1_B_ref_mid[j][63];
                        }
                    }
                    for (int j = 0; j < 96; j++)
                    {
    #pragma HLS pipeline
                        for (int i = 0; i < 16; i++)
                        {
    #pragma HLS unroll
                            /**************************padding chroma*********************************/
                            refWinU_temp[j*128+i] = refWinU1_B_ref_mid[j][0];
                            refWinU_temp[j*128+i + 16] = refWinU1_B_ref_mid[j][0];
                            refWinU_temp[j*128+i + 32] = refWinU1_B_ref_mid[j][0];
                            refWinU_temp[j*128+i + 48] = refWinU1_B_ref_mid[j][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_B_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU1_B_ref_mid[j][31];
                            refWinU_temp[j*128+i + 96] = refWinU1_B_ref_mid[j][31];
                            refWinU_temp[j*128+i + 112] = refWinU1_B_ref_mid[j][31];
                                        
                            refWinV_temp[j*128+i] = refWinV1_B_ref_mid[j][0];
                            refWinV_temp[j*128+i + 16] = refWinV1_B_ref_mid[j][0];
                            refWinV_temp[j*128+i + 32] = refWinV1_B_ref_mid[j][0];
                            refWinV_temp[j*128+i + 48] = refWinV1_B_ref_mid[j][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_B_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV1_B_ref_mid[j][31];
                            refWinV_temp[j*128+i + 96] = refWinV1_B_ref_mid[j][31];
                            refWinV_temp[j*128+i + 112] = refWinV1_B_ref_mid[j][31];
                        }
                    }
                }
            }
            else if (64 == pix_x)
            {
                if ((pix_x + 128) <= (luma_width_padded - 64))
                {
                    for (int j = 0; j < 192; j++)
                    {
        #pragma HLS pipeline
                        for (int i = 0; i < 32; i++)
                        {
        #pragma HLS unroll
                            /*******************************padding luma**************************************************/
                            // swOutLv0_temp[j][i] = refWinY0_B_ref_mid[j][0];
                            // swOutLv0_temp[j][i + 32] = refWinY0_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i+64] = refWinY0_B_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 96] = refWinY1_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_B_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 160] = refWinY2_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i+192] = refWinY2_B_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 224] = refWinY3_B_ref_mid[j][i];

                            swOutLv0_ime_temp[j][i] = refWinY0_B_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i + 32] = refWinY0_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+64] = refWinY0_B_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 96] = refWinY1_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_B_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 160] = refWinY2_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+192] = refWinY2_B_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 224] = refWinY3_B_ref_mid[j][i];
                        }
                    }

                    for (int j = 0; j < 96; j++)
                    {
        #pragma HLS pipeline
                        for (int i = 0; i < 16; i++)
                        {
        #pragma HLS unroll
                            /*******************************padding chroma**************************************************/
                            refWinU_temp[j*128+i] = refWinU0_B_ref_mid[j][0];
                            refWinU_temp[j*128+i + 16] = refWinU0_B_ref_mid[j][i];
                            refWinU_temp[j*128+i + 32] = refWinU0_B_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 48] = refWinU1_B_ref_mid[j][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_B_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU2_B_ref_mid[j][i];
                            refWinU_temp[j*128+i + 96] = refWinU2_B_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 112] = refWinU3_B_ref_mid[j][i];
                                        
                            refWinV_temp[j*128+i] = refWinV0_B_ref_mid[j][0];
                            refWinV_temp[j*128+i + 16] = refWinV0_B_ref_mid[j][i];
                            refWinV_temp[j*128+i + 32] = refWinV0_B_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 48] = refWinV1_B_ref_mid[j][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_B_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV2_B_ref_mid[j][i];
                            refWinV_temp[j*128+i + 96] = refWinV2_B_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 112] = refWinV3_B_ref_mid[j][i];
                        }
                    }
                }
                else if ((pix_x + 128) == luma_width_padded)
                {
                    for (int j = 0; j < 192; j++)
                    {
    #pragma HLS pipeline
                        for (int i = 0; i < 32; i++)
                        {
    #pragma HLS unroll
                            /*******************************padding luma**************************************************/
                            // swOutLv0_temp[j][i] = refWinY0_B_ref_mid[j][0];
                            // swOutLv0_temp[j][i + 32] = refWinY0_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i+64] = refWinY0_B_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 96] = refWinY1_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_B_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 160] = refWinY2_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i+192] = refWinY2_B_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 224] = refWinY2_B_ref_mid[j][63];

                            swOutLv0_ime_temp[j][i] = refWinY0_B_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i + 32] = refWinY0_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+64] = refWinY0_B_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 96] = refWinY1_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_B_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 160] = refWinY2_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+192] = refWinY2_B_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 224] = refWinY2_B_ref_mid[j][63];
                        }
                    }

                    for (int j = 0; j < 96; j++)
                    {
    #pragma HLS pipeline
                        for (int i = 0; i < 16; i++)
                        {
    #pragma HLS unroll
                            /*******************************padding chroma**************************************************/
                            refWinU_temp[j*128+i] = refWinU0_B_ref_mid[j][0];
                            refWinU_temp[j*128+i + 16] = refWinU0_B_ref_mid[j][i];
                            refWinU_temp[j*128+i + 32] = refWinU0_B_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 48] = refWinU1_B_ref_mid[j][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_B_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU2_B_ref_mid[j][i];
                            refWinU_temp[j*128+i + 96] = refWinU2_B_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 112] = refWinU2_B_ref_mid[j][31];
                                        
                            refWinV_temp[j*128+i] = refWinV0_B_ref_mid[j][0];
                            refWinV_temp[j*128+i + 16] = refWinV0_B_ref_mid[j][i];
                            refWinV_temp[j*128+i + 32] = refWinV0_B_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 48] = refWinV1_B_ref_mid[j][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_B_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV2_B_ref_mid[j][i];
                            refWinV_temp[j*128+i + 96] = refWinV2_B_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 112] = refWinV2_B_ref_mid[j][31];
                        }
                    }
                }
                else if ((pix_x + 128) == (luma_width_padded+64))
                {
                    for (int j = 0; j < 192; j++)
                    {
    #pragma HLS pipeline
                        for (int i = 0; i < 32; i++)
                        {
    #pragma HLS unroll
                            /*******************************padding luma**************************************************/
                            // swOutLv0_temp[j][i] = refWinY0_B_ref_mid[j][0];
                            // swOutLv0_temp[j][i + 32] = refWinY0_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i+64] = refWinY0_B_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 96] = refWinY1_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_B_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 160] = refWinY1_B_ref_mid[j][63];
                            // swOutLv0_temp[j][i+192] = refWinY1_B_ref_mid[j][63];
                            // swOutLv0_temp[j][i + 224] = refWinY1_B_ref_mid[j][63];

                            swOutLv0_ime_temp[j][i] = refWinY0_B_ref_mid[j][0];
                            swOutLv0_ime_temp[j][i + 32] = refWinY0_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+64] = refWinY0_B_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 96] = refWinY1_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_B_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 160] = refWinY1_B_ref_mid[j][63];
                            swOutLv0_ime_temp[j][i+192] = refWinY1_B_ref_mid[j][63];
                            swOutLv0_ime_temp[j][i + 224] = refWinY1_B_ref_mid[j][63];
                        }
                    }

                    for (int j = 0; j < 96; j++)
                    {
    #pragma HLS pipeline
                        for (int i = 0; i < 16; i++)
                        {
    #pragma HLS unroll
                            /*******************************padding chroma**************************************************/
                            refWinU_temp[j*128+i] = refWinU0_B_ref_mid[j][0];
                            refWinU_temp[j*128+i + 16] = refWinU0_B_ref_mid[j][i];
                            refWinU_temp[j*128+i + 32] = refWinU0_B_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 48] = refWinU1_B_ref_mid[j][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_B_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU1_B_ref_mid[j][31];
                            refWinU_temp[j*128+i + 96] = refWinU1_B_ref_mid[j][31];
                            refWinU_temp[j*128+i + 112] = refWinU1_B_ref_mid[j][31];
                                        
                            refWinV_temp[j*128+i] = refWinV0_B_ref_mid[j][0];
                            refWinV_temp[j*128+i + 16] = refWinV0_B_ref_mid[j][i];
                            refWinV_temp[j*128+i + 32] = refWinV0_B_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 48] = refWinV1_B_ref_mid[j][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_B_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV1_B_ref_mid[j][31];
                            refWinV_temp[j*128+i + 96] = refWinV1_B_ref_mid[j][31];
                            refWinV_temp[j*128+i + 112] = refWinV1_B_ref_mid[j][31];
                        }
                    }
                }
            }
            else
            {
                if ((pix_x + 128) <= (luma_width_padded - 64))
                {
                    for (int j = 0; j < 192; j++)
                    {
        #pragma HLS pipeline
                        for (int i = 0; i < 32; i++)
                        {
        #pragma HLS unroll
                            /*****************padding luma***************************************************/
                            // swOutLv0_temp[j][i] = refWinY32x192_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i + 32] = refWinY0_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i+64] = refWinY0_B_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 96] = refWinY1_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_B_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 160] = refWinY2_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i+192] = refWinY2_B_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 224] = refWinY3_B_ref_mid[j][i];

                            swOutLv0_ime_temp[j][i] = refWinY32x192_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i + 32] = refWinY0_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+64] = refWinY0_B_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 96] = refWinY1_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_B_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 160] = refWinY2_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+192] = refWinY2_B_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 224] = refWinY3_B_ref_mid[j][i];
                        }
                    }

                    for (int j = 0; j < 96; j++)
                    {
        #pragma HLS pipeline
                        for (int i = 0; i < 16; i++)
                        {
        #pragma HLS unroll
                            /*****************padding chroma***************************************************/
                            refWinU_temp[j * 128 + i] = refWinU16x96_B_ref_mid[j][i];
                            refWinU_temp[j * 128 + i + 16] = refWinU0_B_ref_mid[j][i];
                            refWinU_temp[j * 128 + i + 32] = refWinU0_B_ref_mid[j][i + 16];
                            refWinU_temp[j * 128 + i + 48] = refWinU1_B_ref_mid[j][i];
                            refWinU_temp[j * 128 + i + 64] = refWinU1_B_ref_mid[j][i + 16];
                            refWinU_temp[j * 128 + i + 80] = refWinU2_B_ref_mid[j][i];
                            refWinU_temp[j * 128 + i + 96] = refWinU2_B_ref_mid[j][i + 16];
                            refWinU_temp[j * 128 + i + 112] = refWinU3_B_ref_mid[j][i];
                                        
                            refWinV_temp[j*128+i] = refWinV16x96_B_ref_mid[j][i];
                            refWinV_temp[j*128+i + 16] = refWinV0_B_ref_mid[j][i];
                            refWinV_temp[j*128+i + 32] = refWinV0_B_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 48] = refWinV1_B_ref_mid[j][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_B_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV2_B_ref_mid[j][i];
                            refWinV_temp[j*128+i + 96] = refWinV2_B_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 112] = refWinV3_B_ref_mid[j][i];
                        }
                    }
                }
                else if ((pix_x + 128) == luma_width_padded)
                {
                    for (int j = 0; j < 192; j++)
                    {
    #pragma HLS pipeline
                        for (int i = 0; i < 32; i++)
                        {
    #pragma HLS unroll
                            /*****************padding luma***************************************************/
                            // swOutLv0_temp[j][i] = refWinY32x192_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i + 32] = refWinY0_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i+64] = refWinY0_B_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 96] = refWinY1_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_B_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 160] = refWinY2_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i+192] = refWinY2_B_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 224] = refWinY2_B_ref_mid[j][63];

                            swOutLv0_ime_temp[j][i] = refWinY32x192_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i + 32] = refWinY0_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+64] = refWinY0_B_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 96] = refWinY1_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_B_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 160] = refWinY2_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+192] = refWinY2_B_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 224] = refWinY2_B_ref_mid[j][63];
                        }
                    }

                    for (int j = 0; j < 96; j++)
                    {
    #pragma HLS pipeline
                        for (int i = 0; i < 16; i++)
                        {
    #pragma HLS unroll
                            /*****************padding chroma***************************************************/
                            refWinU_temp[j*128+i] = refWinU16x96_B_ref_mid[j][i];
                            refWinU_temp[j*128+i + 16] = refWinU0_B_ref_mid[j][i];
                            refWinU_temp[j*128+i + 32] = refWinU0_B_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 48] = refWinU1_B_ref_mid[j][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_B_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU2_B_ref_mid[j][i];
                            refWinU_temp[j*128+i + 96] = refWinU2_B_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 112] = refWinU2_B_ref_mid[j][31];
                                        
                            refWinV_temp[j*128+i] = refWinV16x96_B_ref_mid[j][i];
                            refWinV_temp[j*128+i + 16] = refWinV0_B_ref_mid[j][i];
                            refWinV_temp[j*128+i + 32] = refWinV0_B_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 48] = refWinV1_B_ref_mid[j][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_B_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV2_B_ref_mid[j][i];
                            refWinV_temp[j*128+i + 96] = refWinV2_B_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 112] = refWinV2_B_ref_mid[j][31];
                        }
                    }
                }
                else if ((pix_x + 128) == (luma_width_padded+64))
                {
                    for (int j = 0; j < 192; j++)
                    {
    #pragma HLS pipeline
                        for (int i = 0; i < 32; i++)
                        {
    #pragma HLS unroll
                            /*****************padding luma***************************************************/
                            // swOutLv0_temp[j][i] = refWinY32x192_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i + 32] = refWinY0_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i+64] = refWinY0_B_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 96] = refWinY1_B_ref_mid[j][i];
                            // swOutLv0_temp[j][i+128] = refWinY1_B_ref_mid[j][i + 32];
                            // swOutLv0_temp[j][i + 160] = refWinY1_B_ref_mid[j][63];
                            // swOutLv0_temp[j][i+192] = refWinY1_B_ref_mid[j][63];
                            // swOutLv0_temp[j][i + 224] = refWinY1_B_ref_mid[j][63];

                            swOutLv0_ime_temp[j][i] = refWinY32x192_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i + 32] = refWinY0_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+64] = refWinY0_B_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 96] = refWinY1_B_ref_mid[j][i];
                            swOutLv0_ime_temp[j][i+128] = refWinY1_B_ref_mid[j][i + 32];
                            swOutLv0_ime_temp[j][i + 160] = refWinY1_B_ref_mid[j][63];
                            swOutLv0_ime_temp[j][i+192] = refWinY1_B_ref_mid[j][63];
                            swOutLv0_ime_temp[j][i + 224] = refWinY1_B_ref_mid[j][63];
                        }
                    }

                    for (int j = 0; j < 96; j++)
                    {
    #pragma HLS pipeline
                        for (int i = 0; i < 16; i++)
                        {
    #pragma HLS unroll
                            /*****************padding chroma***************************************************/
                            refWinU_temp[j*128+i] = refWinU16x96_B_ref_mid[j][i];
                            refWinU_temp[j*128+i + 16] = refWinU0_B_ref_mid[j][i];
                            refWinU_temp[j*128+i + 32] = refWinU0_B_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 48] = refWinU1_B_ref_mid[j][i];
                            refWinU_temp[j*128+i + 64] = refWinU1_B_ref_mid[j][i + 16];
                            refWinU_temp[j*128+i + 80] = refWinU1_B_ref_mid[j][31];
                            refWinU_temp[j*128+i + 96] = refWinU1_B_ref_mid[j][31];
                            refWinU_temp[j*128+i + 112] = refWinU1_B_ref_mid[j][31];
                                        
                            refWinV_temp[j*128+i] = refWinV16x96_B_ref_mid[j][i];
                            refWinV_temp[j*128+i + 16] = refWinV0_B_ref_mid[j][i];
                            refWinV_temp[j*128+i + 32] = refWinV0_B_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 48] = refWinV1_B_ref_mid[j][i];
                            refWinV_temp[j*128+i + 64] = refWinV1_B_ref_mid[j][i + 16];
                            refWinV_temp[j*128+i + 80] = refWinV1_B_ref_mid[j][31];
                            refWinV_temp[j*128+i + 96] = refWinV1_B_ref_mid[j][31];
                            refWinV_temp[j*128+i + 112] = refWinV1_B_ref_mid[j][31];
                        }
                    }
                }
            }
        }
    }
#endif

    if(0==b_use_flag)
    {
        for (int j = 0; j < 192; j++)
        {
    #pragma HLS pipeline
            for (int i = 0; i < 32; i++)
            {
    #pragma HLS unroll
                /*****************************update luma***********************/
                refWinY32x192_ref_mid[j][i] = refWinY0_ref_mid[j][i + 32];
            }

            for (int i = 0; i < 32; i++)
            {
    #pragma HLS unroll
                /********update luma*****************************************/
                refWinY0_ref_mid[j][i] = refWinY1_ref_mid[j][i];
                refWinY1_ref_mid[j][i] = refWinY2_ref_mid[j][i];
                refWinY2_ref_mid[j][i] = refWinY3_ref_mid[j][i];

                refWinY0_ref_mid[j][i+32] = refWinY1_ref_mid[j][i+32];
                refWinY1_ref_mid[j][i+32] = refWinY2_ref_mid[j][i+32];
                refWinY2_ref_mid[j][i+32] = refWinY3_ref_mid[j][i+32];
            }
        }

        for (int j = 0; j < 96; j++)
        {
    #pragma HLS pipeline
            for (int i = 0; i < 16; i++)
            {
    #pragma HLS unroll
                /*****************************update chroma***********************/
                refWinU16x96_ref_mid[j][i] = refWinU0_ref_mid[j][i + 16];
                refWinV16x96_ref_mid[j][i] = refWinV0_ref_mid[j][i + 16];
            }

            
            for (int i = 0; i < 16; i++)
            {
    #pragma HLS unroll
                /********update chroma*****************************************/
                refWinU0_ref_mid[j][i] = refWinU1_ref_mid[j][i];
                refWinU1_ref_mid[j][i] = refWinU2_ref_mid[j][i];
                refWinU2_ref_mid[j][i] = refWinU3_ref_mid[j][i];

                refWinU0_ref_mid[j][i+16] = refWinU1_ref_mid[j][i+16];
                refWinU1_ref_mid[j][i+16] = refWinU2_ref_mid[j][i+16];
                refWinU2_ref_mid[j][i+16] = refWinU3_ref_mid[j][i+16];

                refWinV0_ref_mid[j][i] = refWinV1_ref_mid[j][i];
                refWinV1_ref_mid[j][i] = refWinV2_ref_mid[j][i];
                refWinV2_ref_mid[j][i] = refWinV3_ref_mid[j][i];

                refWinV0_ref_mid[j][i+16] = refWinV1_ref_mid[j][i+16];
                refWinV1_ref_mid[j][i+16] = refWinV2_ref_mid[j][i+16];
                refWinV2_ref_mid[j][i+16] = refWinV3_ref_mid[j][i+16];
            }
        }
    }
#ifndef __SYNTHESIS__
    else
    {
        for (int j = 0; j < 192; j++)
        {
    #pragma HLS pipeline
            for (int i = 0; i < 32; i++)
            {
    #pragma HLS unroll
                /*****************************update luma***********************/
                refWinY32x192_B_ref_mid[j][i] = refWinY0_B_ref_mid[j][i + 32];
            }

            for (int i = 0; i < 32; i++)
            {
    #pragma HLS unroll
                /********update luma*****************************************/
                refWinY0_B_ref_mid[j][i] = refWinY1_B_ref_mid[j][i];
                refWinY1_B_ref_mid[j][i] = refWinY2_B_ref_mid[j][i];
                refWinY2_B_ref_mid[j][i] = refWinY3_B_ref_mid[j][i];

                refWinY0_B_ref_mid[j][i+32] = refWinY1_B_ref_mid[j][i+32];
                refWinY1_B_ref_mid[j][i+32] = refWinY2_B_ref_mid[j][i+32];
                refWinY2_B_ref_mid[j][i+32] = refWinY3_B_ref_mid[j][i+32];
            }
        }

        for (int j = 0; j < 96; j++)
        {
    #pragma HLS pipeline
            for (int i = 0; i < 16; i++)
            {
    #pragma HLS unroll
                /*****************************update chroma***********************/
                refWinU16x96_B_ref_mid[j][i] = refWinU0_B_ref_mid[j][i + 16];
                refWinV16x96_B_ref_mid[j][i] = refWinV0_B_ref_mid[j][i + 16];
            }

            
            for (int i = 0; i < 16; i++)
            {
    #pragma HLS unroll
                /********update chroma*****************************************/
                refWinU0_B_ref_mid[j][i] = refWinU1_B_ref_mid[j][i];
                refWinU1_B_ref_mid[j][i] = refWinU2_B_ref_mid[j][i];
                refWinU2_B_ref_mid[j][i] = refWinU3_B_ref_mid[j][i];

                refWinU0_B_ref_mid[j][i+16] = refWinU1_B_ref_mid[j][i+16];
                refWinU1_B_ref_mid[j][i+16] = refWinU2_B_ref_mid[j][i+16];
                refWinU2_B_ref_mid[j][i+16] = refWinU3_B_ref_mid[j][i+16];

                refWinV0_B_ref_mid[j][i] = refWinV1_B_ref_mid[j][i];
                refWinV1_B_ref_mid[j][i] = refWinV2_B_ref_mid[j][i];
                refWinV2_B_ref_mid[j][i] = refWinV3_B_ref_mid[j][i];

                refWinV0_B_ref_mid[j][i+16] = refWinV1_B_ref_mid[j][i+16];
                refWinV1_B_ref_mid[j][i+16] = refWinV2_B_ref_mid[j][i+16];
                refWinV2_B_ref_mid[j][i+16] = refWinV3_B_ref_mid[j][i+16];
            }
        }
    }
#endif
#endif
#if DEBUG_CHECK//checkout test
    if (b_use_flag)
    {
        for (int j = 0; j < 192; j++)
        {
            for (int i = 0; i < 64; i++)
            {
                if (refWinY0_B_test2[j][i] != swOutLv0_ime_temp[j][i])
                {
                    printf("refWinY0_b[%d][%d] = %d\trefWinY0_B_test2[%d][%d] = %d\n", j, i, swOutLv0_ime_temp[j][i], j, i, refWinY0_B_test2[j][i]);
                }
                if (refWinY1_B_test2[j][i] != swOutLv0_ime_temp[j][i+64])
                {
                    printf("refWinY1_b[%d][%d] = %d\trefWinY1_B_test2[%d][%d] = %d\n", j, i, swOutLv0_ime_temp[j][i+64], j, i, refWinY1_B_test2[j][i]);
                }
                if (refWinY2_B_test2[j][i] != swOutLv0_ime_temp[j][i+128])
                {
                    printf("refWinY2_b[%d][%d] = %d\trefWinY2_B_test2[%d][%d] = %d\n", j, i, swOutLv0_ime_temp[j][i+128], j, i, refWinY2_B_test2[j][i]);
                }
                if (refWinY3_B_test2[j][i] != swOutLv0_ime_temp[j][i+192])
                {
                    printf("refWinY3_b[%d][%d] = %d\trefWinY3_B_test2[%d][%d] = %d\n", j, i, swOutLv0_ime_temp[j][i+192], j, i, refWinY3_B_test2[j][i]);
                }
            }
        }

        for (int j = 0; j < 96; j++)
        {
            for (int i = 0; i < 32; i++)
            {
                if (refWinU0_B_test2[j][i] != refWinU_temp[j * 128 + i])
                {
                    printf("refWinU0_b[%d][%d] = %d\trefWinU0_B_test2[%d][%d] = %d\n", j, i, refWinU_temp[j * 128 + i], j, i, refWinU0_B_test2[j][i]);
                }
                if (refWinV0_B_test2[j][i] != refWinV_temp[j * 128 + i])
                {
                    printf("refWinV0_b[%d][%d] = %d\trefWinV0_B_test2[%d][%d] = %d\n", j, i, refWinV_temp[j*128+i], j, i, refWinV0_B_test2[j][i]);
                }

                if (refWinU1_B_test2[j][i] != refWinU_temp[j * 128 + i+32])
                {
                    printf("refWinU1_b[%d][%d] = %d\trefWinU1_B_test2[%d][%d] = %d\n", j, i, refWinU_temp[j * 128 + i+32], j, i, refWinU1_B_test2[j][i]);
                }
                if (refWinV1_B_test2[j][i] != refWinV_temp[j * 128 + i+32])
                {
                    printf("refWinV1_b[%d][%d] = %d\trefWinV1_B_test2[%d][%d] = %d\n", j, i, refWinV_temp[j * 128 + i+32], j, i, refWinV1_B_test2[j][i]);
                }

                if (refWinU2_B_test2[j][i] != refWinU_temp[j * 128 + i+64])
                {
                    printf("refWinU2_b[%d][%d] = %d\trefWinU2_B_test2[%d][%d] = %d\n", j, i, refWinU_temp[j * 128 + i+64], j, i, refWinU2_B_test2[j][i]);
                }
                if (refWinV2_B_test2[j][i] != refWinV_temp[j * 128 + i+64])
                {
                    printf("refWinV2_b[%d][%d] = %d\trefWinV2_B_test2[%d][%d] = %d\n", j, i, refWinV_temp[j * 128 + i+64], j, i, refWinV2_B_test2[j][i]);
                }

                if (refWinU3_B_test2[j][i] != refWinU_temp[j * 128 + i+96])
                {
                    printf("refWinU3_b[%d][%d] = %d\trefWinU3_B_test2[%d][%d] = %d\n", j, i, refWinU_temp[j * 128 + i+96], j, i, refWinU3_B_test2[j][i]);
                }
                if (refWinV3_B_test2[j][i] != refWinV_temp[j * 128 + i+96])
                {
                    printf("refWinV3_b[%d][%d] = %d\trefWinV3_B_test2[%d][%d] = %d\n", j, i, refWinV_temp[j * 128 + i+96], j, i, refWinV3_B_test2[j][i]);
                }
            }
        }
    }
    else
    {
        // checkout test ok !
        for (int j = 0; j < 192; j++)
        {
            for (int i = 0; i < 64; i++)
            {
                if (refWinY0_test2[j][i] != swOutLv0_ime_temp[j][i])
                {
                    printf("refWinY0[%d][%d] = %d\trefWinY0_test2[%d][%d] = %d\n", j, i, swOutLv0_ime_temp[j][i], j, i, refWinY0_test2[j][i]);
                }
                if (refWinY1_test2[j][i] != swOutLv0_ime_temp[j][i+64])
                {
                    printf("refWinY1[%d][%d] = %d\trefWinY1_test2[%d][%d] = %d\n", j, i, swOutLv0_ime_temp[j][i+64], j, i, refWinY1_test2[j][i]);
                }
                if (refWinY2_test2[j][i] != swOutLv0_ime_temp[j][i+128])
                {
                    printf("refWinY2[%d][%d] = %d\trefWinY2_test2[%d][%d] = %d\n", j, i, swOutLv0_ime_temp[j][i+128], j, i, refWinY2_test2[j][i]);
                }
                if (refWinY3_test2[j][i] != swOutLv0_ime_temp[j][i+192])
                {
                    printf("refWinY3[%d][%d] = %d\trefWinY3_test2[%d][%d] = %d\n", j, i, swOutLv0_ime_temp[j][i+192], j, i, refWinY3_test2[j][i]);
                }
            }
        }

        for (int j = 0; j < 96; j++)
        {
            for (int i = 0; i < 32; i++)
            {
                if (refWinU0_test2[j][i] != refWinU_temp[j * 128 + i])
                {
                    printf("refWinU0[%d][%d] = %d\trefWinU0_test2[%d][%d] = %d\n", j, i, refWinU_temp[j * 128 + i], j, i, refWinU0_test2[j][i]);
                }
                if (refWinV0_test2[j][i] != refWinV_temp[j * 128 + i])
                {
                    printf("refWinV0[%d][%d] = %d\trefWinV0_test2[%d][%d] = %d\n", j, i, refWinV_temp[j * 128 + i], j, i, refWinV0_test2[j][i]);
                }

                if (refWinU1_test2[j][i] != refWinU_temp[j * 128 + i+32])
                {
                    printf("refWinU1[%d][%d] = %d\trefWinU1_test2[%d][%d] = %d\n", j, i, refWinU_temp[j * 128 + i+32], j, i, refWinU1_test2[j][i]);
                }
                if (refWinV1_test2[j][i] != refWinV_temp[j * 128 + i+32])
                {
                    printf("refWinV1[%d][%d] = %d\trefWinV1_test2[%d][%d] = %d\n", j, i, refWinV_temp[j * 128 + i+32], j, i, refWinV1_test2[j][i]);
                }

                if (refWinU2_test2[j][i] != refWinU_temp[j * 128 + i+64])
                {
                    printf("refWinU2[%d][%d] = %d\trefWinU2_test2[%d][%d] = %d\n", j, i, refWinU_temp[j * 128 + i+64], j, i, refWinU2_test2[j][i]);
                }
                if (refWinV2_test2[j][i] != refWinV_temp[j * 128 + i+64])
                {
                    printf("refWinV2[%d][%d] = %d\trefWinV2_test2[%d][%d] = %d\n", j, i, refWinV_temp[j * 128 + i+64], j, i, refWinV2_test2[j][i]);
                }

                if (refWinU3_test2[j][i] != refWinU_temp[j * 128 + i+96])
                {
                    printf("refWinU3[%d][%d] = %d\trefWinU3_test2[%d][%d] = %d\n", j, i, refWinU_temp[j * 128 + i+96], j, i, refWinU3_test2[j][i]);
                }
                if (refWinV3_test2[j][i] != refWinV_temp[j * 128 + i+96])
                {
                    printf("refWinV3[%d][%d] = %d\trefWinV3_test2[%d][%d] = %d\n", j, i, refWinV_temp[j * 128 + i+96], j, i, refWinV3_test2[j][i]);
                }
            }
        }
    }
#endif//test check
#endif//ADD_by_YHY
}
#endif//ADD_by_YHY
#endif//FETCH_REF_BLOCK

#if 0
static void set_control_points(
    int* core_x_pel, 
    int* core_y_pel, 
    CTX_FETCH* fetch_ctx_io, 
    strFetch_ref_window* Fetch_Refwin_ptr
)
{
    int pix_x = *core_x_pel;
    int pix_y = *core_y_pel;
    int lenth_hor = 0;
    int lenth_ver = 0;
    int pad_luma = (fetch_ctx_io->ctx_pinter_refp_pic_stride_luma - fetch_ctx_io->ctx_pinter_refp_pic_width_luma) / 2;
    int pad_chroma = (fetch_ctx_io->ctx_pinter_refp_pic_stride_chroma - fetch_ctx_io->ctx_pinter_refp_pic_width_chroma) / 2;
    Fetch_Refwin_ptr->CtrPos[0][0][0] = (((-pad_luma > pix_x - (REFP_SURD_OFF_W) ? -pad_luma : pix_x - (REFP_SURD_OFF_W))));
    Fetch_Refwin_ptr->CtrPos[1][0][0] = (((fetch_ctx_io->ctx_pinter_refp_pic_width_luma + pad_luma < pix_x + REFP_SURD_OFF_W + 64 ?
        fetch_ctx_io->ctx_pinter_refp_pic_width_luma + pad_luma : pix_x + REFP_SURD_OFF_W + 64)));

    Fetch_Refwin_ptr->CtrPos[0][1][0] = (((-pad_luma > pix_y - (REFP_SURD_OFF_H) ? -pad_luma : pix_y - (REFP_SURD_OFF_H))));
    Fetch_Refwin_ptr->CtrPos[1][1][0] = (((fetch_ctx_io->ctx_pinter_refp_pic_width_luma + pad_luma < pix_y + REFP_SURD_OFF_H + 64 ?
        fetch_ctx_io->ctx_pinter_refp_pic_width_luma + pad_luma : pix_y + REFP_SURD_OFF_H + 64)));


    lenth_hor = ((int)Fetch_Refwin_ptr->CtrPos[1][0][0]) - ((int)Fetch_Refwin_ptr->CtrPos[0][0][0]);
    lenth_ver = ((int)Fetch_Refwin_ptr->CtrPos[1][1][0]) - ((int)Fetch_Refwin_ptr->CtrPos[0][1][0]);

    U13 tempa = 0;
    U13 tempb = 0;
    U13 tempc = 0;
    U13 tempd = 0;
    tempa = ((((int)Fetch_Refwin_ptr->CtrPos[0][0][0]) - (pix_x - (REFP_SURD_OFF_W))));
    Fetch_Refwin_ptr->CtrPos[0][0][1] = tempa;
    tempb = ((((int)Fetch_Refwin_ptr->CtrPos[0][0][1]) + lenth_hor));
    Fetch_Refwin_ptr->CtrPos[1][0][1] = tempb;
    tempc = ((((int)Fetch_Refwin_ptr->CtrPos[0][1][0]) - (pix_y - (REFP_SURD_OFF_H))));
    Fetch_Refwin_ptr->CtrPos[0][1][1] = tempc;
    tempd = ((((int)Fetch_Refwin_ptr->CtrPos[0][1][1]) + lenth_ver));
    Fetch_Refwin_ptr->CtrPos[1][1][1] = tempd;

#if ENABLE_BFRAME
    for(int i=0;i< RANGE_NUM;i++)
    {
        for (int j = 0; j < DIR_DIM; j++)
        {
            for (int k = 0; k < MAP_DIMS; k++)
            {
                Fetch_Refwin_ptr->CtrPos_ref1[i][j][k] = Fetch_Refwin_ptr->CtrPos[i][j][k];
            }
        }
    }
#endif
}

#else
static void set_control_points_IME(
#if PATCH_DIVIDE_ENABLE
    int multislice_mode,
#endif
    int* core_x_pel,
    int* core_y_pel,
    CTX_FETCH* fetch_ctx_io,
    U8 ctx_rpm_num_refp_0,
    U8 ctx_rpm_num_refp_1,
    FETCH_REF_WINDOW_IME* Fetch_Ref_Window_IME
)
{
    int REFP_SURD_OFF_W_PATCH = REFP_SURD_OFF_W;
    int REFP_SURD_OFF_H_PATCH = REFP_SURD_OFF_H;

    if (multislice_mode != 0)
    {
        REFP_SURD_OFF_W_PATCH = (SRW / 4 - 1);
        REFP_SURD_OFF_H_PATCH = (SRH / 4 - 1);
    }
    //printf("multislice_mode = %d\n", multislice_mode);

    int pix_x = *core_x_pel;
    int pix_y = *core_y_pel;
    int lenth_hor = 0;
    int lenth_ver = 0;
    int pad_luma = (fetch_ctx_io->ctx_pinter_refp_pic_stride_luma - fetch_ctx_io->ctx_pinter_refp_pic_width_luma) / 2;
    int pad_chroma = (fetch_ctx_io->ctx_pinter_refp_pic_stride_chroma - fetch_ctx_io->ctx_pinter_refp_pic_width_chroma) / 2;
    Fetch_Ref_Window_IME->CtrPos[0][0][0] = (((-pad_luma > pix_x - (REFP_SURD_OFF_W_PATCH) ? -pad_luma : pix_x - (REFP_SURD_OFF_W_PATCH))));
    Fetch_Ref_Window_IME->CtrPos[1][0][0] = (((fetch_ctx_io->ctx_pinter_refp_pic_width_luma + pad_luma < pix_x + REFP_SURD_OFF_W_PATCH + 64 ?
        fetch_ctx_io->ctx_pinter_refp_pic_width_luma + pad_luma : pix_x + REFP_SURD_OFF_W_PATCH + 64)));

    Fetch_Ref_Window_IME->CtrPos[0][1][0] = (((-pad_luma > pix_y - (REFP_SURD_OFF_H_PATCH) ? -pad_luma : pix_y - (REFP_SURD_OFF_H_PATCH))));
    Fetch_Ref_Window_IME->CtrPos[1][1][0] = (((fetch_ctx_io->ctx_pinter_refp_pic_width_luma + pad_luma < pix_y + REFP_SURD_OFF_H_PATCH + 64 ?
        fetch_ctx_io->ctx_pinter_refp_pic_width_luma + pad_luma : pix_y + REFP_SURD_OFF_H_PATCH + 64)));


    lenth_hor = ((int)Fetch_Ref_Window_IME->CtrPos[1][0][0]) - ((int)Fetch_Ref_Window_IME->CtrPos[0][0][0]);
    lenth_ver = ((int)Fetch_Ref_Window_IME->CtrPos[1][1][0]) - ((int)Fetch_Ref_Window_IME->CtrPos[0][1][0]);

    U13 tempa = 0;
    U13 tempb = 0;
    U13 tempc = 0;
    U13 tempd = 0;
    tempa = ((((int)Fetch_Ref_Window_IME->CtrPos[0][0][0]) - (pix_x - (REFP_SURD_OFF_W_PATCH))));
    Fetch_Ref_Window_IME->CtrPos[0][0][1] = tempa;
    tempb = ((((int)Fetch_Ref_Window_IME->CtrPos[0][0][1]) + lenth_hor));
    Fetch_Ref_Window_IME->CtrPos[1][0][1] = tempb;
    tempc = ((((int)Fetch_Ref_Window_IME->CtrPos[0][1][0]) - (pix_y - (REFP_SURD_OFF_H_PATCH))));
    Fetch_Ref_Window_IME->CtrPos[0][1][1] = tempc;
    tempd = ((((int)Fetch_Ref_Window_IME->CtrPos[0][1][1]) + lenth_ver));
    Fetch_Ref_Window_IME->CtrPos[1][1][1] = tempd;

#if ENABLE_BFRAME
    if (fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_P && ctx_rpm_num_refp_0 > 1)
    {
        int pad_luma_ref1 = (fetch_ctx_io->ctx_pinter_refp_pic_stride_luma_all[1][0] - fetch_ctx_io->ctx_pinter_refp_pic_width_luma_all[1][0]) / 2;
        int pad_chroma_ref1 = (fetch_ctx_io->ctx_pinter_refp_pic_stride_chroma_all[1][0] - fetch_ctx_io->ctx_pinter_refp_pic_width_chroma_all[1][0]) / 2;
        Fetch_Ref_Window_IME->CtrPos_ref1[0][0][0] = (((-pad_luma_ref1 > pix_x - (REFP_SURD_OFF_W_PATCH) ? -pad_luma_ref1 : pix_x - (REFP_SURD_OFF_W_PATCH))));
        Fetch_Ref_Window_IME->CtrPos_ref1[1][0][0] = (((fetch_ctx_io->ctx_pinter_refp_pic_width_luma_all[1][0] + pad_luma_ref1 < pix_x + REFP_SURD_OFF_W_PATCH + 64 ?
            fetch_ctx_io->ctx_pinter_refp_pic_width_luma_all[1][0] + pad_luma_ref1 : pix_x + REFP_SURD_OFF_W_PATCH + 64)));
        Fetch_Ref_Window_IME->CtrPos_ref1[0][1][0] = (((-pad_luma_ref1 > pix_y - (REFP_SURD_OFF_H_PATCH) ? -pad_luma_ref1 : pix_y - (REFP_SURD_OFF_H_PATCH))));
        Fetch_Ref_Window_IME->CtrPos_ref1[1][1][0] = (((fetch_ctx_io->ctx_pinter_refp_pic_width_luma_all[1][0] + pad_luma_ref1 < pix_y + REFP_SURD_OFF_H_PATCH + 64 ?
            fetch_ctx_io->ctx_pinter_refp_pic_width_luma_all[1][0] + pad_luma_ref1 : pix_y + REFP_SURD_OFF_H_PATCH + 64)));
        lenth_hor = ((int)Fetch_Ref_Window_IME->CtrPos_ref1[1][0][0]) - ((int)Fetch_Ref_Window_IME->CtrPos_ref1[0][0][0]);
        lenth_ver = ((int)Fetch_Ref_Window_IME->CtrPos_ref1[1][1][0]) - ((int)Fetch_Ref_Window_IME->CtrPos_ref1[0][1][0]);

        tempa = ((((int)Fetch_Ref_Window_IME->CtrPos_ref1[0][0][0]) - (pix_x - (REFP_SURD_OFF_W_PATCH))));
        Fetch_Ref_Window_IME->CtrPos_ref1[0][0][1] = tempa;
        tempb = ((((int)Fetch_Ref_Window_IME->CtrPos_ref1[0][0][1]) + lenth_hor));
        Fetch_Ref_Window_IME->CtrPos[1][0][1] = tempb;
        tempc = ((((int)Fetch_Ref_Window_IME->CtrPos_ref1[0][1][0]) - (pix_y - (REFP_SURD_OFF_H_PATCH))));
        Fetch_Ref_Window_IME->CtrPos_ref1[0][1][1] = tempc;
        tempd = ((((int)Fetch_Ref_Window_IME->CtrPos_ref1[0][1][1]) + lenth_ver));
        Fetch_Ref_Window_IME->CtrPos_ref1[1][1][1] = tempd;
    }
    else if (fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B)
    {
        int pad_luma_ref1 = (fetch_ctx_io->ctx_pinter_refp_pic_stride_luma_all[0][1] - fetch_ctx_io->ctx_pinter_refp_pic_width_luma_all[0][1]) / 2;
        int pad_chroma_ref1 = (fetch_ctx_io->ctx_pinter_refp_pic_stride_chroma_all[0][1] - fetch_ctx_io->ctx_pinter_refp_pic_width_chroma_all[0][1]) / 2;
        Fetch_Ref_Window_IME->CtrPos_ref1[0][0][0] = (((-pad_luma_ref1 > pix_x - (REFP_SURD_OFF_W_PATCH) ? -pad_luma_ref1 : pix_x - (REFP_SURD_OFF_W_PATCH))));
        Fetch_Ref_Window_IME->CtrPos_ref1[1][0][0] = (((fetch_ctx_io->ctx_pinter_refp_pic_width_luma_all[0][1] + pad_luma_ref1 < pix_x + REFP_SURD_OFF_W_PATCH + 64 ?
            fetch_ctx_io->ctx_pinter_refp_pic_width_luma_all[0][1] + pad_luma_ref1 : pix_x + REFP_SURD_OFF_W_PATCH + 64)));
        Fetch_Ref_Window_IME->CtrPos_ref1[0][1][0] = (((-pad_luma_ref1 > pix_y - (REFP_SURD_OFF_H_PATCH) ? -pad_luma_ref1 : pix_y - (REFP_SURD_OFF_H_PATCH))));
        Fetch_Ref_Window_IME->CtrPos_ref1[1][1][0] = (((fetch_ctx_io->ctx_pinter_refp_pic_width_luma_all[0][1] + pad_luma_ref1 < pix_y + REFP_SURD_OFF_H_PATCH + 64 ?
            fetch_ctx_io->ctx_pinter_refp_pic_width_luma_all[0][1] + pad_luma_ref1 : pix_y + REFP_SURD_OFF_H_PATCH + 64)));
        lenth_hor = ((int)Fetch_Ref_Window_IME->CtrPos_ref1[1][0][0]) - ((int)Fetch_Ref_Window_IME->CtrPos_ref1[0][0][0]);
        lenth_ver = ((int)Fetch_Ref_Window_IME->CtrPos_ref1[1][1][0]) - ((int)Fetch_Ref_Window_IME->CtrPos_ref1[0][1][0]);

        tempa = ((((int)Fetch_Ref_Window_IME->CtrPos_ref1[0][0][0]) - (pix_x - (REFP_SURD_OFF_W_PATCH))));
        Fetch_Ref_Window_IME->CtrPos_ref1[0][0][1] = tempa;
        tempb = ((((int)Fetch_Ref_Window_IME->CtrPos_ref1[0][0][1]) + lenth_hor));
        Fetch_Ref_Window_IME->CtrPos[1][0][1] = tempb;
        tempc = ((((int)Fetch_Ref_Window_IME->CtrPos_ref1[0][1][0]) - (pix_y - (REFP_SURD_OFF_H_PATCH))));
        Fetch_Ref_Window_IME->CtrPos_ref1[0][1][1] = tempc;
        tempd = ((((int)Fetch_Ref_Window_IME->CtrPos_ref1[0][1][1]) + lenth_ver));
        Fetch_Ref_Window_IME->CtrPos_ref1[1][1][1] = tempd;

    }
#endif
}

static void set_control_points(
#if PATCH_DIVIDE_ENABLE
    int multislice_mode,
#endif
    int* core_x_pel,
    int* core_y_pel,
    CTX_FETCH* fetch_ctx_io,
    U8 ctx_rpm_num_refp_0,
    U8 ctx_rpm_num_refp_1,
    strFetch_ref_window* Fetch_Ref_window,
    strFetch_ref_window* Fetch_Ref_window_md2chroma,
    FETCH_REF_WINDOW_IME* Fetch_Ref_Window_IME
)
{

    int REFP_SURD_OFF_W_PATCH = REFP_SURD_OFF_W;
    int REFP_SURD_OFF_H_PATCH = REFP_SURD_OFF_H;

    if (multislice_mode != 0)
    {
        REFP_SURD_OFF_W_PATCH = (SRW / 4 - 1);
        REFP_SURD_OFF_H_PATCH = (SRH / 4 - 1);
    }
    //printf("multislice_mode = %d\n", multislice_mode);

    int pix_x = *core_x_pel;
    int pix_y = *core_y_pel;
    int lenth_hor = 0;
    int lenth_ver = 0;
    int pad_luma = (fetch_ctx_io->ctx_pinter_refp_pic_stride_luma - fetch_ctx_io->ctx_pinter_refp_pic_width_luma) / 2;
    int pad_chroma = (fetch_ctx_io->ctx_pinter_refp_pic_stride_chroma - fetch_ctx_io->ctx_pinter_refp_pic_width_chroma) / 2;
    Fetch_Ref_window->CtrPos[0][0][0] = (((-pad_luma > pix_x - (REFP_SURD_OFF_W_PATCH) ? -pad_luma : pix_x - (REFP_SURD_OFF_W_PATCH))));
    Fetch_Ref_window->CtrPos[1][0][0] = (((fetch_ctx_io->ctx_pinter_refp_pic_width_luma + pad_luma < pix_x + REFP_SURD_OFF_W_PATCH + 64 ?
        fetch_ctx_io->ctx_pinter_refp_pic_width_luma + pad_luma : pix_x + REFP_SURD_OFF_W_PATCH + 64)));

    Fetch_Ref_window_md2chroma->CtrPos[0][0][0] = (((-pad_luma > pix_x - (REFP_SURD_OFF_W_PATCH) ? -pad_luma : pix_x - (REFP_SURD_OFF_W_PATCH))));
    Fetch_Ref_window_md2chroma->CtrPos[1][0][0] = (((fetch_ctx_io->ctx_pinter_refp_pic_width_luma + pad_luma < pix_x + REFP_SURD_OFF_W_PATCH + 64 ?
        fetch_ctx_io->ctx_pinter_refp_pic_width_luma + pad_luma : pix_x + REFP_SURD_OFF_W_PATCH + 64)));

    Fetch_Ref_Window_IME->CtrPos[0][0][0] = (((-pad_luma > pix_x - (REFP_SURD_OFF_W_PATCH) ? -pad_luma : pix_x - (REFP_SURD_OFF_W_PATCH))));
    Fetch_Ref_Window_IME->CtrPos[1][0][0] = (((fetch_ctx_io->ctx_pinter_refp_pic_width_luma + pad_luma < pix_x + REFP_SURD_OFF_W_PATCH + 64 ?
        fetch_ctx_io->ctx_pinter_refp_pic_width_luma + pad_luma : pix_x + REFP_SURD_OFF_W_PATCH + 64)));

    Fetch_Ref_window->CtrPos[0][1][0] = (((-pad_luma > pix_y - (REFP_SURD_OFF_H_PATCH) ? -pad_luma : pix_y - (REFP_SURD_OFF_H_PATCH))));
    Fetch_Ref_window->CtrPos[1][1][0] = (((fetch_ctx_io->ctx_pinter_refp_pic_width_luma + pad_luma < pix_y + REFP_SURD_OFF_H_PATCH + 64 ?
        fetch_ctx_io->ctx_pinter_refp_pic_width_luma + pad_luma : pix_y + REFP_SURD_OFF_H_PATCH + 64)));

    Fetch_Ref_window_md2chroma->CtrPos[0][1][0] = (((-pad_luma > pix_y - (REFP_SURD_OFF_H_PATCH) ? -pad_luma : pix_y - (REFP_SURD_OFF_H_PATCH))));
    Fetch_Ref_window_md2chroma->CtrPos[1][1][0] = (((fetch_ctx_io->ctx_pinter_refp_pic_width_luma + pad_luma < pix_y + REFP_SURD_OFF_H_PATCH + 64 ?
        fetch_ctx_io->ctx_pinter_refp_pic_width_luma + pad_luma : pix_y + REFP_SURD_OFF_H_PATCH + 64)));

    Fetch_Ref_Window_IME->CtrPos[0][1][0] = (((-pad_luma > pix_y - (REFP_SURD_OFF_H_PATCH) ? -pad_luma : pix_y - (REFP_SURD_OFF_H_PATCH))));
    Fetch_Ref_Window_IME->CtrPos[1][1][0] = (((fetch_ctx_io->ctx_pinter_refp_pic_width_luma + pad_luma < pix_y + REFP_SURD_OFF_H_PATCH + 64 ?
        fetch_ctx_io->ctx_pinter_refp_pic_width_luma + pad_luma : pix_y + REFP_SURD_OFF_H_PATCH + 64)));

    lenth_hor = ((int)Fetch_Ref_window->CtrPos[1][0][0]) - ((int)Fetch_Ref_window->CtrPos[0][0][0]);
    lenth_ver = ((int)Fetch_Ref_window->CtrPos[1][1][0]) - ((int)Fetch_Ref_window->CtrPos[0][1][0]);

    U13 tempa = 0;
    U13 tempb = 0;
    U13 tempc = 0;
    U13 tempd = 0;
    tempa = ((((int)Fetch_Ref_window->CtrPos[0][0][0]) - (pix_x - (REFP_SURD_OFF_W_PATCH))));
    Fetch_Ref_window->CtrPos[0][0][1] = tempa;
    Fetch_Ref_window_md2chroma->CtrPos[0][0][1] = tempa;
    Fetch_Ref_Window_IME->CtrPos[0][0][1] = tempa;

    tempb = ((((int)Fetch_Ref_window->CtrPos[0][0][1]) + lenth_hor));
    Fetch_Ref_window->CtrPos[1][0][1] = tempb;
    Fetch_Ref_window_md2chroma->CtrPos[1][0][1] = tempb;
    Fetch_Ref_Window_IME->CtrPos[1][0][1] = tempb;

    tempc = ((((int)Fetch_Ref_window->CtrPos[0][1][0]) - (pix_y - (REFP_SURD_OFF_H_PATCH))));
    Fetch_Ref_window->CtrPos[0][1][1] = tempc;
    Fetch_Ref_window_md2chroma->CtrPos[0][1][1] = tempc;
    Fetch_Ref_Window_IME->CtrPos[0][1][1] = tempc;

    tempd = ((((int)Fetch_Ref_window->CtrPos[0][1][1]) + lenth_ver));
    Fetch_Ref_window->CtrPos[1][1][1] = tempd;
    Fetch_Ref_window_md2chroma->CtrPos[1][1][1] = tempd;
    Fetch_Ref_Window_IME->CtrPos[1][1][1] = tempd;

#if ENABLE_BFRAME
    if (fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_P && ctx_rpm_num_refp_0 > 1)
    {
         int pad_luma_ref1 = (fetch_ctx_io->ctx_pinter_refp_pic_stride_luma - fetch_ctx_io->ctx_pinter_refp_pic_width_luma) / 2;
        int pad_chroma_ref1 = (fetch_ctx_io->ctx_pinter_refp_pic_stride_chroma - fetch_ctx_io->ctx_pinter_refp_pic_width_chroma) / 2;

        Fetch_Ref_window->CtrPos_ref1[0][0][0] = (((-pad_luma_ref1 > pix_x - (REFP_SURD_OFF_W_PATCH) ? -pad_luma_ref1 : pix_x - (REFP_SURD_OFF_W_PATCH))));
        Fetch_Ref_window->CtrPos_ref1[1][0][0] = (((fetch_ctx_io->ctx_pinter_refp_pic_width_luma + pad_luma_ref1 < pix_x + REFP_SURD_OFF_W_PATCH + 64 ?
            fetch_ctx_io->ctx_pinter_refp_pic_width_luma + pad_luma_ref1 : pix_x + REFP_SURD_OFF_W_PATCH + 64)));

        Fetch_Ref_window_md2chroma->CtrPos_ref1[0][0][0] = (((-pad_luma_ref1 > pix_x - (REFP_SURD_OFF_W_PATCH) ? -pad_luma_ref1 : pix_x - (REFP_SURD_OFF_W_PATCH))));
        Fetch_Ref_window_md2chroma->CtrPos_ref1[1][0][0] = (((fetch_ctx_io->ctx_pinter_refp_pic_width_luma + pad_luma_ref1 < pix_x + REFP_SURD_OFF_W_PATCH + 64 ?
            fetch_ctx_io->ctx_pinter_refp_pic_width_luma + pad_luma_ref1 : pix_x + REFP_SURD_OFF_W_PATCH + 64)));

        Fetch_Ref_Window_IME->CtrPos_ref1[0][0][0] = (((-pad_luma_ref1 > pix_x - (REFP_SURD_OFF_W_PATCH) ? -pad_luma_ref1 : pix_x - (REFP_SURD_OFF_W_PATCH))));
        Fetch_Ref_Window_IME->CtrPos_ref1[1][0][0] = (((fetch_ctx_io->ctx_pinter_refp_pic_width_luma + pad_luma_ref1 < pix_x + REFP_SURD_OFF_W_PATCH + 64 ?
            fetch_ctx_io->ctx_pinter_refp_pic_width_luma + pad_luma_ref1 : pix_x + REFP_SURD_OFF_W_PATCH + 64)));

        Fetch_Ref_window->CtrPos_ref1[0][1][0] = (((-pad_luma_ref1 > pix_y - (REFP_SURD_OFF_H_PATCH) ? -pad_luma_ref1 : pix_y - (REFP_SURD_OFF_H_PATCH))));
        Fetch_Ref_window->CtrPos_ref1[1][1][0] = (((fetch_ctx_io->ctx_pinter_refp_pic_width_luma + pad_luma_ref1 < pix_y + REFP_SURD_OFF_H_PATCH + 64 ?
            fetch_ctx_io->ctx_pinter_refp_pic_width_luma + pad_luma_ref1 : pix_y + REFP_SURD_OFF_H_PATCH + 64)));
        
        Fetch_Ref_window_md2chroma->CtrPos_ref1[0][1][0] = (((-pad_luma_ref1 > pix_y - (REFP_SURD_OFF_H_PATCH) ? -pad_luma_ref1 : pix_y - (REFP_SURD_OFF_H_PATCH))));
        Fetch_Ref_window_md2chroma->CtrPos_ref1[1][1][0] = (((fetch_ctx_io->ctx_pinter_refp_pic_width_luma + pad_luma_ref1 < pix_y + REFP_SURD_OFF_H_PATCH + 64 ?
            fetch_ctx_io->ctx_pinter_refp_pic_width_luma + pad_luma_ref1 : pix_y + REFP_SURD_OFF_H_PATCH + 64)));

        Fetch_Ref_Window_IME->CtrPos_ref1[0][1][0] = (((-pad_luma_ref1 > pix_y - (REFP_SURD_OFF_H_PATCH) ? -pad_luma_ref1 : pix_y - (REFP_SURD_OFF_H_PATCH))));
        Fetch_Ref_Window_IME->CtrPos_ref1[1][1][0] = (((fetch_ctx_io->ctx_pinter_refp_pic_width_luma + pad_luma_ref1 < pix_y + REFP_SURD_OFF_H_PATCH + 64 ?
            fetch_ctx_io->ctx_pinter_refp_pic_width_luma + pad_luma_ref1 : pix_y + REFP_SURD_OFF_H_PATCH + 64)));

        lenth_hor = ((int)Fetch_Ref_window->CtrPos_ref1[1][0][0]) - ((int)Fetch_Ref_window->CtrPos_ref1[0][0][0]);
        lenth_ver = ((int)Fetch_Ref_window->CtrPos_ref1[1][1][0]) - ((int)Fetch_Ref_window->CtrPos_ref1[0][1][0]);

        tempa = ((((int)Fetch_Ref_window->CtrPos_ref1[0][0][0]) - (pix_x - (REFP_SURD_OFF_W_PATCH))));
        Fetch_Ref_window->CtrPos_ref1[0][0][1] = tempa;
        Fetch_Ref_window_md2chroma->CtrPos_ref1[0][0][1] = tempa;
        Fetch_Ref_Window_IME->CtrPos_ref1[0][0][1] = tempa;

        tempb = ((((int)Fetch_Ref_window->CtrPos_ref1[0][0][1]) + lenth_hor));
        Fetch_Ref_window->CtrPos[1][0][1] = tempb;
        Fetch_Ref_window_md2chroma->CtrPos[1][0][1] = tempb;
        Fetch_Ref_Window_IME->CtrPos[1][0][1] = tempb;

        tempc = ((((int)Fetch_Ref_window->CtrPos_ref1[0][1][0]) - (pix_y - (REFP_SURD_OFF_H_PATCH))));
        Fetch_Ref_window->CtrPos_ref1[0][1][1] = tempc;
        Fetch_Ref_window_md2chroma->CtrPos_ref1[0][1][1] = tempc;
        Fetch_Ref_Window_IME->CtrPos_ref1[0][1][1] = tempc;

        tempd = ((((int)Fetch_Ref_window->CtrPos_ref1[0][1][1]) + lenth_ver));
        Fetch_Ref_window->CtrPos_ref1[1][1][1] = tempd;
        Fetch_Ref_window_md2chroma->CtrPos_ref1[1][1][1] = tempd;
        Fetch_Ref_Window_IME->CtrPos_ref1[1][1][1] = tempd;
    }
    else if (fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B)
    {
        int pad_luma_ref1 = (fetch_ctx_io->ctx_pinter_refp_pic_stride_luma - fetch_ctx_io->ctx_pinter_refp_pic_width_luma) / 2;
        int pad_chroma_ref1 = (fetch_ctx_io->ctx_pinter_refp_pic_stride_chroma - fetch_ctx_io->ctx_pinter_refp_pic_width_chroma) / 2;
        Fetch_Ref_window->CtrPos_ref1[0][0][0] = (((-pad_luma_ref1 > pix_x - (REFP_SURD_OFF_W_PATCH) ? -pad_luma_ref1 : pix_x - (REFP_SURD_OFF_W_PATCH))));
        Fetch_Ref_window->CtrPos_ref1[1][0][0] = (((fetch_ctx_io->ctx_pinter_refp_pic_width_luma + pad_luma_ref1 < pix_x + REFP_SURD_OFF_W_PATCH + 64 ?
            fetch_ctx_io->ctx_pinter_refp_pic_width_luma + pad_luma_ref1 : pix_x + REFP_SURD_OFF_W_PATCH + 64)));

        Fetch_Ref_window_md2chroma->CtrPos_ref1[0][0][0] = (((-pad_luma_ref1 > pix_x - (REFP_SURD_OFF_W_PATCH) ? -pad_luma_ref1 : pix_x - (REFP_SURD_OFF_W_PATCH))));
        Fetch_Ref_window_md2chroma->CtrPos_ref1[1][0][0] = (((fetch_ctx_io->ctx_pinter_refp_pic_width_luma + pad_luma_ref1 < pix_x + REFP_SURD_OFF_W_PATCH + 64 ?
            fetch_ctx_io->ctx_pinter_refp_pic_width_luma + pad_luma_ref1 : pix_x + REFP_SURD_OFF_W_PATCH + 64)));

        Fetch_Ref_Window_IME->CtrPos_ref1[0][0][0] = (((-pad_luma_ref1 > pix_x - (REFP_SURD_OFF_W_PATCH) ? -pad_luma_ref1 : pix_x - (REFP_SURD_OFF_W_PATCH))));
        Fetch_Ref_Window_IME->CtrPos_ref1[1][0][0] = (((fetch_ctx_io->ctx_pinter_refp_pic_width_luma + pad_luma_ref1 < pix_x + REFP_SURD_OFF_W_PATCH + 64 ?
            fetch_ctx_io->ctx_pinter_refp_pic_width_luma + pad_luma_ref1 : pix_x + REFP_SURD_OFF_W_PATCH + 64)));

        Fetch_Ref_window->CtrPos_ref1[0][1][0] = (((-pad_luma_ref1 > pix_y - (REFP_SURD_OFF_H_PATCH) ? -pad_luma_ref1 : pix_y - (REFP_SURD_OFF_H_PATCH))));
        Fetch_Ref_window->CtrPos_ref1[1][1][0] = (((fetch_ctx_io->ctx_pinter_refp_pic_width_luma + pad_luma_ref1 < pix_y + REFP_SURD_OFF_H_PATCH + 64 ?
            fetch_ctx_io->ctx_pinter_refp_pic_width_luma + pad_luma_ref1 : pix_y + REFP_SURD_OFF_H_PATCH + 64)));

        Fetch_Ref_window_md2chroma->CtrPos_ref1[0][1][0] = (((-pad_luma_ref1 > pix_y - (REFP_SURD_OFF_H_PATCH) ? -pad_luma_ref1 : pix_y - (REFP_SURD_OFF_H_PATCH))));
        Fetch_Ref_window_md2chroma->CtrPos_ref1[1][1][0] = (((fetch_ctx_io->ctx_pinter_refp_pic_width_luma + pad_luma_ref1 < pix_y + REFP_SURD_OFF_H_PATCH + 64 ?
            fetch_ctx_io->ctx_pinter_refp_pic_width_luma + pad_luma_ref1 : pix_y + REFP_SURD_OFF_H_PATCH + 64)));

        Fetch_Ref_Window_IME->CtrPos_ref1[0][1][0] = (((-pad_luma_ref1 > pix_y - (REFP_SURD_OFF_H_PATCH) ? -pad_luma_ref1 : pix_y - (REFP_SURD_OFF_H_PATCH))));
        Fetch_Ref_Window_IME->CtrPos_ref1[1][1][0] = (((fetch_ctx_io->ctx_pinter_refp_pic_width_luma + pad_luma_ref1 < pix_y + REFP_SURD_OFF_H_PATCH + 64 ?
            fetch_ctx_io->ctx_pinter_refp_pic_width_luma + pad_luma_ref1 : pix_y + REFP_SURD_OFF_H_PATCH + 64)));

        lenth_hor = ((int)Fetch_Ref_window->CtrPos_ref1[1][0][0]) - ((int)Fetch_Ref_window->CtrPos_ref1[0][0][0]);
        lenth_ver = ((int)Fetch_Ref_window->CtrPos_ref1[1][1][0]) - ((int)Fetch_Ref_window->CtrPos_ref1[0][1][0]);

        tempa = ((((int)Fetch_Ref_window->CtrPos_ref1[0][0][0]) - (pix_x - (REFP_SURD_OFF_W_PATCH))));
        Fetch_Ref_window->CtrPos_ref1[0][0][1] = tempa;
        Fetch_Ref_window_md2chroma->CtrPos_ref1[0][0][1] = tempa;
        Fetch_Ref_Window_IME->CtrPos_ref1[0][0][1] = tempa;

        tempb = ((((int)Fetch_Ref_window->CtrPos_ref1[0][0][1]) + lenth_hor));
        Fetch_Ref_window->CtrPos[1][0][1] = tempb;
        Fetch_Ref_window_md2chroma->CtrPos[1][0][1] = tempb;
        Fetch_Ref_Window_IME->CtrPos[1][0][1] = tempb;

        tempc = ((((int)Fetch_Ref_window->CtrPos_ref1[0][1][0]) - (pix_y - (REFP_SURD_OFF_H_PATCH))));
        Fetch_Ref_window->CtrPos_ref1[0][1][1] = tempc;
        Fetch_Ref_window_md2chroma->CtrPos_ref1[0][1][1] = tempc;
        Fetch_Ref_Window_IME->CtrPos_ref1[0][1][1] = tempc;

        tempd = ((((int)Fetch_Ref_window->CtrPos_ref1[0][1][1]) + lenth_ver));
        Fetch_Ref_window->CtrPos_ref1[1][1][1] = tempd;
        Fetch_Ref_window_md2chroma->CtrPos_ref1[1][1][1] = tempd;
        Fetch_Ref_Window_IME->CtrPos_ref1[1][1][1] = tempd;

    }
#endif
}

#endif

static void avg_pool_sw(U16* plane_h, U16* plane_l, int height_h, int width_h, int height_l, int width_l, int stride_h,
    int stride_l, int start_x, int start_y)
{
    int i = 0;
    int j = 0;
    int temp_x = 0;
    int temp_y = 0;
    for (i = 0; i < height_h; i++) {

#pragma HLS loop_tripcount max = 32
        temp_y = i * 2 + start_y;
        for (j = 0; j < width_h; j++) {

#pragma HLS loop_tripcount max = 32
            temp_x = j * 2 + start_x;
            plane_h[j + i * stride_h] = ((U16)((((int)plane_l[temp_x + temp_y * stride_l]) +
                ((int)plane_l[temp_x + 1 + temp_y * stride_l]) + ((int)plane_l[temp_x + (temp_y + 1) * stride_l]) +
                ((int)plane_l[temp_x + 1 + (temp_y + 1) * stride_l])) >> 2));
        }
    }
}


static void fetch_co_left_line(
    CTX_FETCH* fetch_ctx_io,
    ENC_ME_LINE_MAP* me_line_ptr, 
    ENC_ME_LINE_MAP_IME* me_line_map_ptr_IME,
    ENC_ME_LINE_MAP_IME* me_line_map_ptr_IME_1,
    s32(*ctx_pinter_refp_mv)[REFP_NUM][MV_D],
    s32(*ctx_pinter_refp_refi)[REFP_NUM], 
    u32* ctx_pinter_refp_list_ptr,
    int* tmp_fetchout_pix_x, 
    int* tmp_fetchout_pix_y
)
{
    int i = 0;
    int pic_width_in_scu = fetch_ctx_io->ctx_info_pic_width >> 2;
    int pic_height_in_scu = fetch_ctx_io->ctx_info_pic_height >> 2;
    int x_scu = ((*tmp_fetchout_pix_x) >> 2) - 1;
    int y_scu = (*tmp_fetchout_pix_y) >> 2;
    int scup = x_scu + y_scu * pic_width_in_scu;

    S2 co_left_refi_tmp[16] = {};

    for (i = 0; i < 16; i++) {
        #pragma HLS pipeline
        if (x_scu >= 0 && y_scu < pic_height_in_scu) {
            me_line_ptr->co_left_mv[i][0] = ctx_pinter_refp_mv[scup][0][0];
            me_line_ptr->co_left_mv[i][1] = ctx_pinter_refp_mv[scup][0][1];
            me_line_ptr->co_left_refi[i] = ctx_pinter_refp_refi[scup][0];
            me_line_map_ptr_IME->co_left_mv[i][0] = ctx_pinter_refp_mv[scup][0][0];
            me_line_map_ptr_IME->co_left_mv[i][1] = ctx_pinter_refp_mv[scup][0][1];
            me_line_map_ptr_IME->co_left_refi[i] = ctx_pinter_refp_refi[scup][0];

            me_line_map_ptr_IME_1->co_left_mv[i][0] = ctx_pinter_refp_mv[scup][0][0];
            me_line_map_ptr_IME_1->co_left_mv[i][1] = ctx_pinter_refp_mv[scup][0][1];
            me_line_map_ptr_IME_1->co_left_refi[i] = ctx_pinter_refp_refi[scup][0];
            co_left_refi_tmp[i] = ctx_pinter_refp_refi[scup][0];
        }
        else {
            me_line_ptr->co_left_mv[i][0] = ((S14)0);
            me_line_ptr->co_left_mv[i][1] = ((S14)0);
            me_line_ptr->co_left_refi[i] = ((S2)(-1));
            me_line_map_ptr_IME->co_left_mv[i][0] = ((S14)0);
            me_line_map_ptr_IME->co_left_mv[i][1] = ((S14)0);
            me_line_map_ptr_IME->co_left_refi[i] = ((S2)(-1));

            me_line_map_ptr_IME_1->co_left_mv[i][0] = ((S14)0);
            me_line_map_ptr_IME_1->co_left_mv[i][1] = ((S14)0);
            me_line_map_ptr_IME_1->co_left_refi[i] = ((S2)(-1));
            co_left_refi_tmp[i] = ((S2)(-1));
        }
        if (co_left_refi_tmp[i] > 0)
        {
            me_line_ptr->co_left_ptr_ref[i] = ctx_pinter_refp_list_ptr[co_left_refi_tmp[i]];
            me_line_map_ptr_IME->co_left_ptr_ref[i] = ctx_pinter_refp_list_ptr[co_left_refi_tmp[i]];
            me_line_map_ptr_IME_1->co_left_ptr_ref[i] = ctx_pinter_refp_list_ptr[co_left_refi_tmp[i]];
        }
        else
        { 
            me_line_ptr->co_left_ptr_ref[i] = 0;
            me_line_map_ptr_IME->co_left_ptr_ref[i] = 0;
            me_line_map_ptr_IME_1->co_left_ptr_ref[i] = 0;
		}
        scup += pic_width_in_scu;
        y_scu++;
    }

}

static void fetch_cpy2md(CTU_Level_Parameters fetch_output, MD_INPUT* input) {
    input->pix_x = fetch_output.pix_x;
    input->pix_y = fetch_output.pix_y;
    for (int i = 0; i < 3; i++)
        input->lambda[i] = fetch_output.lambda[i];
    input->lambda_mv = fetch_output.lambda_mv;
    input->qp_y = fetch_output.qp_y;
    input->qp_u = fetch_output.qp_u;
    input->qp_v = fetch_output.qp_v;
    input->dist_chroma_weight[0] = fetch_output.dist_chroma_weight[0];
    input->dist_chroma_weight[1] = fetch_output.dist_chroma_weight[1];
    input->x_last_ctu = fetch_output.x_last_ctu;
    input->y_last_ctu = fetch_output.y_last_ctu;
}

static void fetch_skip_merge(
    CTX_FETCH* fetch_ctx_io, 
    Slice_Level_Parameters* ctu_ctrl_input,
    SKIP_MERGE_INPUT* input, 
    //s32(*ctx_pinter_refp_mv)[REFP_NUM][MV_D],
    //s32(*ctx_pinter_refp_refi)[REFP_NUM], 
    int* tmp_fetchout_pix_x, 
    int* tmp_fetchout_pix_y
#if ENABLE_BFRAME
#if FETCH_COM_REF_DEBUG
    //, U1 flag_valid
    , U1 flag_valid_map_mv
    , U1 flag_valid_map_refi
    , U1 flag_valid_list_ptr
    , U1 flag_valid_map_mv_1
    , U1 flag_valid_map_refi_1
    , U1 flag_valid_list_ptr_1
    , s32(*ctx_pinter_refp_map_mv_0)[REFP_NUM][MV_D]
    , s32(*ctx_pinter_refp_map_mv_1)[REFP_NUM][MV_D]
    , s32(*ctx_pinter_refp_map_refi_0)[REFP_NUM]
    , s32(*ctx_pinter_refp_map_refi_1)[REFP_NUM]
    , u32* list_ptr_0
    , u32* list_ptr_1
    //, int ptr_0
    //, int ptr_1
    , U32 refp_ptr_test[MAX_NUM_REF_PICS][REFP_NUM]
    //, int code_stru
#else   
	, COM_REFP(*ctx_pinter_refp)[REFP_NUM]
#endif  // #if FETCH_COM_REF_DEBUG
#endif
)
{
    int i;
    int j;
    int pic_width_in_scu = fetch_ctx_io->ctx_info_pic_width >> 2;
    int pic_height_in_scu = fetch_ctx_io->ctx_info_pic_height >> 2;
    int ctu_x_pos = *tmp_fetchout_pix_x;
    int ctu_y_pos = *tmp_fetchout_pix_y;
    int ctu_x_scu = ctu_x_pos >> 2;
    int ctu_y_scu = ctu_y_pos >> 2;

// #pragma HLS array_partition variable=input->refp_map_mv complete dim=3
// #pragma HLS array_partition variable=input->refp_map_mv complete dim=2
// #pragma HLS array_partition variable=input->refp_map_refi complete dim=2
// #pragma HLS array_partition variable=input->refp_map_mv1 complete dim=1
// #pragma HLS array_partition variable=input->refp_map_mv1 complete dim=4
// #pragma HLS array_partition variable=input->refp_map_refi1 complete dim=1
// #pragma HLS array_partition variable=input->refp_map_refi1 complete dim=4
// #pragma HLS array_partition variable=input->refp_list_ptr1 complete dim=0

    for (i = 0; i < 1 << (5 - 1); i++) {
        #pragma HLS pipeline
        for (j = 0; j < 1 << (5 - 1); j++) {
#pragma HLS unroll
            int x_scu = ctu_x_scu + j;
            int y_scu = ctu_y_scu + i;
            if (x_scu < pic_width_in_scu && y_scu < pic_height_in_scu) {
                int scup = x_scu + y_scu * pic_width_in_scu;
                input->refp_map_mv[i][j][0] = ctx_pinter_refp_map_mv_0[scup][0][0];
                input->refp_map_mv[i][j][1] = ctx_pinter_refp_map_mv_0[scup][0][1];
                input->refp_map_refi[i][j] = ctx_pinter_refp_map_refi_0[scup][0];
            }
            else {
                input->refp_map_mv[i][j][0] = ((s16)0);
                input->refp_map_mv[i][j][1] = ((s16)0);
                input->refp_map_refi[i][j] = ((S2)(-1));
            }
        }
    }

#if !FETCH_COM_REF_DEBUG
#if ENABLE_BFRAME
	for (i = 0; i < 1 << (5 - 1); i++) {
#pragma HLS pipeline
		for (j = 0; j < 1 << (5 - 1); j++) {
#pragma HLS unroll
			int x_scu = ctu_x_scu + j;
			int y_scu = ctu_y_scu + i;
			if (x_scu < pic_width_in_scu && y_scu < pic_height_in_scu) {
				int scup = x_scu + y_scu * pic_width_in_scu;
				if (ctx_pinter_refp[0][REFP_0].map_mv)
				{
					input->refp_map_mv1[REFP_0][i][j][0] = ctx_pinter_refp[0][REFP_0].map_mv[scup][0][0];
					input->refp_map_mv1[REFP_0][i][j][1] = ctx_pinter_refp[0][REFP_0].map_mv[scup][0][1];
				}
				else
				{
					input->refp_map_mv1[REFP_0][i][j][0] = 0;
					input->refp_map_mv1[REFP_0][i][j][1] = 0;
				}
				if (ctx_pinter_refp[0][REFP_0].map_refi)
				{
					input->refp_map_refi1[REFP_0][i][j][REFP_0] = ctx_pinter_refp[0][REFP_0].map_refi[scup][0];
					input->refp_map_refi1[REFP_0][i][j][REFP_1] = ctx_pinter_refp[0][REFP_0].map_refi[scup][1];
				}
				else
				{
					input->refp_map_refi1[REFP_0][i][j][REFP_0] = REFI_INVALID;
					input->refp_map_refi1[REFP_0][i][j][REFP_1] = REFI_INVALID;
				}
				if (ctx_pinter_refp[0][REFP_1].map_mv)
				{
					input->refp_map_mv1[REFP_1][i][j][0] = ctx_pinter_refp[0][REFP_1].map_mv[scup][0][0];
					input->refp_map_mv1[REFP_1][i][j][1] = ctx_pinter_refp[0][REFP_1].map_mv[scup][0][1];
				}
				else
				{
					input->refp_map_mv1[REFP_1][i][j][0] = 0;
					input->refp_map_mv1[REFP_1][i][j][1] = 0;
				}
				if (ctx_pinter_refp[0][REFP_1].map_refi)
				{
					input->refp_map_refi1[REFP_1][i][j][REFP_0] = ctx_pinter_refp[0][REFP_1].map_refi[scup][0];
					input->refp_map_refi1[REFP_1][i][j][REFP_1] = ctx_pinter_refp[0][REFP_1].map_refi[scup][1];
				}
				else
				{
					input->refp_map_refi1[REFP_1][i][j][REFP_0] = REFI_INVALID;
					input->refp_map_refi1[REFP_1][i][j][REFP_1] = REFI_INVALID;
				}
			}
			else {
				input->refp_map_mv1[REFP_0][i][j][0] = ((s16)0);
				input->refp_map_mv1[REFP_0][i][j][1] = ((s16)0);
				input->refp_map_mv1[REFP_1][i][j][0] = ((s16)0);
				input->refp_map_mv1[REFP_1][i][j][1] = ((s16)0);
				input->refp_map_refi1[REFP_0][i][j][REFP_0] = ((S2)(-1));
				input->refp_map_refi1[REFP_0][i][j][REFP_1] = ((S2)(-1));
				input->refp_map_refi1[REFP_1][i][j][REFP_0] = ((S2)(-1));
				input->refp_map_refi1[REFP_1][i][j][REFP_1] = ((S2)(-1));
			}
		}
	}
	for (i = 0; i < MAX_NUM_REF_PICS; ++i) {
#pragma HLS unroll
		input->refp_ptr1[i][REFP_0] = ctx_pinter_refp[i][REFP_0].ptr;
		input->refp_ptr1[i][REFP_1] = ctx_pinter_refp[i][REFP_1].ptr;
		if (ctx_pinter_refp[0][REFP_0].list_ptr)
		{
			input->refp_list_ptr1[REFP_0][i] = ctx_pinter_refp[0][REFP_0].list_ptr[i];
		}
		else
		{
			input->refp_list_ptr1[REFP_0][i] = 0;
		}
		if (ctx_pinter_refp[0][REFP_1].list_ptr)
		{
			input->refp_list_ptr1[REFP_1][i] = ctx_pinter_refp[0][REFP_1].list_ptr[i];
		}
		else
		{
			input->refp_list_ptr1[REFP_1][i] = 0;
		}

	}
#endif
#else
#if ENABLE_BFRAME

//#if FETCH_COM_REF_DEBUG
    for (i = 0; i < 1 << (5 - 1); i++) 
    {
#pragma HLS pipeline
        for (j = 0; j < 1 << (5 - 1); j++) 
        {
#pragma HLS unroll
            int x_scu = ctu_x_scu + j;
            int y_scu = ctu_y_scu + i;
            if (x_scu < pic_width_in_scu && y_scu < pic_height_in_scu) 
            {
                int scup = x_scu + y_scu * pic_width_in_scu;

                if (flag_valid_map_mv)
                {
                    input->refp_map_mv1[REFP_0][i][j][0] = ctx_pinter_refp_map_mv_0[scup][0][0];
                    input->refp_map_mv1[REFP_0][i][j][1] = ctx_pinter_refp_map_mv_0[scup][0][1];

                }
                else
                {
                    input->refp_map_mv1[REFP_0][i][j][0] = 0;
                    input->refp_map_mv1[REFP_0][i][j][1] = 0;
                }

                if (flag_valid_map_refi)
                {
                    input->refp_map_refi1[REFP_0][i][j][REFP_0] = ctx_pinter_refp_map_refi_0[scup][0];
                    input->refp_map_refi1[REFP_0][i][j][REFP_1] = ctx_pinter_refp_map_refi_0[scup][1];
                }
                else
                {
                    input->refp_map_refi1[REFP_0][i][j][REFP_0] = REFI_INVALID;
                    input->refp_map_refi1[REFP_0][i][j][REFP_1] = REFI_INVALID;
                }

                if (flag_valid_map_mv_1)
                {
                    input->refp_map_mv1[REFP_1][i][j][0] = ctx_pinter_refp_map_mv_1[scup][0][0];
                    input->refp_map_mv1[REFP_1][i][j][1] = ctx_pinter_refp_map_mv_1[scup][0][1];
                }
                else
                {
                    input->refp_map_mv1[REFP_1][i][j][0] = 0;
                    input->refp_map_mv1[REFP_1][i][j][1] = 0;
                }

                if (flag_valid_map_refi_1)
                {

                    input->refp_map_refi1[REFP_1][i][j][REFP_0] = ctx_pinter_refp_map_refi_1[scup][0];
                    input->refp_map_refi1[REFP_1][i][j][REFP_1] = ctx_pinter_refp_map_refi_1[scup][1];
                }
                else
                {
                    input->refp_map_refi1[REFP_1][i][j][REFP_0] = REFI_INVALID;
                    input->refp_map_refi1[REFP_1][i][j][REFP_1] = REFI_INVALID;
                }
            }
            else 
            {
                input->refp_map_mv1[REFP_0][i][j][0] = ((s16)0);
                input->refp_map_mv1[REFP_0][i][j][1] = ((s16)0);
                input->refp_map_mv1[REFP_1][i][j][0] = ((s16)0);
                input->refp_map_mv1[REFP_1][i][j][1] = ((s16)0);
                input->refp_map_refi1[REFP_0][i][j][REFP_0] = ((S2)(-1));
                input->refp_map_refi1[REFP_0][i][j][REFP_1] = ((S2)(-1));
                input->refp_map_refi1[REFP_1][i][j][REFP_0] = ((S2)(-1));
                input->refp_map_refi1[REFP_1][i][j][REFP_1] = ((S2)(-1));
            }

            //========================================================================================


            //if (code_stru == -1 && fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_P)
            //{
            //    if (x_scu < pic_width_in_scu && y_scu < pic_height_in_scu) {
            //        int scup = x_scu + y_scu * pic_width_in_scu;
            //        input->refp_map_mv1[0][i][j][0] = ctx_pinter_refp_mv[scup][0][0];
            //        input->refp_map_mv1[0][i][j][1] = ctx_pinter_refp_mv[scup][0][1];
            //        input->refp_map_refi1[0][i][j][0] = ctx_pinter_refp_refi[scup][0];
            //    }
            //    else {
            //        input->refp_map_mv1[0][i][j][0] = ((s16)0);
            //        input->refp_map_mv1[0][i][j][1] = ((s16)0);
            //        input->refp_map_refi1[0][i][j][0] = ((S2)(-1));
            //    }

            //}



            //========================================================================================

        }
    }
    for (i = 0; i < MAX_NUM_REF_PICS; ++i) 
    {
#pragma HLS unroll
        //input->refp_ptr1[i][REFP_0] = ctx_pinter_refp[i][REFP_0].ptr;
        //input->refp_ptr1[i][REFP_1] = ctx_pinter_refp[i][REFP_1].ptr;
        input->refp_ptr1[i][REFP_0] = refp_ptr_test[i][0];
        input->refp_ptr1[i][REFP_1] = refp_ptr_test[i][1];

        if (flag_valid_list_ptr)
        {
            input->refp_list_ptr1[REFP_0][i] = list_ptr_0[i];
        }
        else
        {
            input->refp_list_ptr1[REFP_0][i] = 0;
        }

        if (flag_valid_list_ptr_1)
        {
            input->refp_list_ptr1[REFP_1][i] = list_ptr_1[i];
        }
        else
        {
            input->refp_list_ptr1[REFP_1][i] = 0;
        }

    }


//#else
//
//    for (i = 0; i < 1 << (5 - 1); i++) {
//        //		//#pragma HLS pipeline
//        for (j = 0; j < 1 << (5 - 1); j++) {
//#pragma HLS unroll
//            int x_scu = ctu_x_scu + j;
//            int y_scu = ctu_y_scu + i;
//            if (x_scu < pic_width_in_scu && y_scu < pic_height_in_scu) {
//                int scup = x_scu + y_scu * pic_width_in_scu;
//                //if (ctx_pinter_refp[0][REFP_0].map_mv)
//                if (flag_valid_map_mv)
//                {
//                    input->refp_map_mv1[REFP_0][i][j][0] = ctx_pinter_refp[0][REFP_0].map_mv[scup][0][0];
//                    input->refp_map_mv1[REFP_0][i][j][1] = ctx_pinter_refp[0][REFP_0].map_mv[scup][0][1];
//
//                    //input->refp_map_mv1[REFP_0][i][j][0] == ctx_pinter_refp_map_mv_0[scup][0][0];
//                    //input->refp_map_mv1[REFP_0][i][j][1] == ctx_pinter_refp_map_mv_0[scup][0][1];
//                    
//                }
//                else
//                {
//                    input->refp_map_mv1[REFP_0][i][j][0] = 0;
//                    input->refp_map_mv1[REFP_0][i][j][1] = 0;
//                }
//                //if (ctx_pinter_refp[0][REFP_0].map_refi)
//                if (flag_valid_map_refi)
//                {
//                    input->refp_map_refi1[REFP_0][i][j][REFP_0] = ctx_pinter_refp[0][REFP_0].map_refi[scup][0];
//                    input->refp_map_refi1[REFP_0][i][j][REFP_1] = ctx_pinter_refp[0][REFP_0].map_refi[scup][1];
//
//
//                    //input->refp_map_refi1[REFP_0][i][j][REFP_0] = ctx_pinter_refp_map_refi_0[scup][0];
//                    //input->refp_map_refi1[REFP_0][i][j][REFP_1] = ctx_pinter_refp_map_refi_0[scup][1];
//                }
//                else
//                {
//                    input->refp_map_refi1[REFP_0][i][j][REFP_0] = REFI_INVALID;
//                    input->refp_map_refi1[REFP_0][i][j][REFP_1] = REFI_INVALID;
//                }
//                //if (ctx_pinter_refp[0][REFP_1].map_mv)
//                if (flag_valid_map_mv_1)
//                {
//                    input->refp_map_mv1[REFP_1][i][j][0] = ctx_pinter_refp[0][REFP_1].map_mv[scup][0][0];
//                    input->refp_map_mv1[REFP_1][i][j][1] = ctx_pinter_refp[0][REFP_1].map_mv[scup][0][1];
//
//                    //input->refp_map_mv1[REFP_1][i][j][0] = ctx_pinter_refp_map_mv_1[scup][0][0];
//                    //input->refp_map_mv1[REFP_1][i][j][1] = ctx_pinter_refp_map_mv_1[scup][0][1];
//                }
//                else
//                {
//                    input->refp_map_mv1[REFP_1][i][j][0] = 0;
//                    input->refp_map_mv1[REFP_1][i][j][1] = 0;
//                }
//                //if (ctx_pinter_refp[0][REFP_1].map_refi)
//                if (flag_valid_map_refi_1)
//                {
//                    input->refp_map_refi1[REFP_1][i][j][REFP_0] = ctx_pinter_refp[0][REFP_1].map_refi[scup][0];
//                    input->refp_map_refi1[REFP_1][i][j][REFP_1] = ctx_pinter_refp[0][REFP_1].map_refi[scup][1];
//
//                    //input->refp_map_refi1[REFP_1][i][j][REFP_0] = ctx_pinter_refp_map_refi_1[scup][0];
//                    //input->refp_map_refi1[REFP_1][i][j][REFP_1] = ctx_pinter_refp_map_refi_1[scup][1];
//                }
//                else
//                {
//                    input->refp_map_refi1[REFP_1][i][j][REFP_0] = REFI_INVALID;
//                    input->refp_map_refi1[REFP_1][i][j][REFP_1] = REFI_INVALID;
//                }
//            }
//            else {
//                input->refp_map_mv1[REFP_0][i][j][0] = ((s16)0);
//                input->refp_map_mv1[REFP_0][i][j][1] = ((s16)0);
//                input->refp_map_mv1[REFP_1][i][j][0] = ((s16)0);
//                input->refp_map_mv1[REFP_1][i][j][1] = ((s16)0);
//                input->refp_map_refi1[REFP_0][i][j][REFP_0] = ((S2)(-1));
//                input->refp_map_refi1[REFP_0][i][j][REFP_1] = ((S2)(-1));
//                input->refp_map_refi1[REFP_1][i][j][REFP_0] = ((S2)(-1));
//                input->refp_map_refi1[REFP_1][i][j][REFP_1] = ((S2)(-1));
//            }
//        }
//    }
//    for (i = 0; i < MAX_NUM_REF_PICS; ++i) {
//#pragma HLS unroll
//        input->refp_ptr1[i][REFP_0] = ctx_pinter_refp[i][REFP_0].ptr;
//        input->refp_ptr1[i][REFP_1] = ctx_pinter_refp[i][REFP_1].ptr;
//        //if (ctx_pinter_refp[0][REFP_0].list_ptr)
//        if (flag_valid_list_ptr)
//        {
//            input->refp_list_ptr1[REFP_0][i] = ctx_pinter_refp[0][REFP_0].list_ptr[i];
//
//            //input->refp_list_ptr1[REFP_0][i] = list_ptr_0[i];
//        }
//        else
//        {
//            input->refp_list_ptr1[REFP_0][i] = 0;
//        }
//        //if (ctx_pinter_refp[0][REFP_1].list_ptr)
//        if (flag_valid_list_ptr_1)
//        {
//            input->refp_list_ptr1[REFP_1][i] = ctx_pinter_refp[0][REFP_1].list_ptr[i];
//
//            //input->refp_list_ptr1[REFP_1][i] = list_ptr_1[i];
//        }
//        else
//        {
//            input->refp_list_ptr1[REFP_1][i] = 0;
//        }
//
//    }
//
//#endif

#endif
#endif  // #if FETCH_COM_REF_DEBUG
}

#if 0
static void fetch2ime_ref(
    int* core_x_pel,
    int* core_y_pel,
    CTX_FETCH* fetch_ctx_io,
    U16* RefbufY,
    U16* RefbufU,
    U16* RefbufV,
    strFetch_ref_window* Fetch_Refwin_ptr
)
{

    int ref_start_x;
    int ref_start_y;
    int pix_x = *core_x_pel;
    int pix_y = *core_y_pel;
    int search_window_width = SWW;
    int search_window_height = SWH;
    int i;
    int j;
    ref_start_x = 0;
    ref_start_y = 0;

    int pad_luma = (fetch_ctx_io->ctx_pinter_refp_pic_stride_luma - fetch_ctx_io->ctx_pinter_refp_pic_width_luma) / 2;
    int pad_chroma = (fetch_ctx_io->ctx_pinter_refp_pic_stride_chroma - fetch_ctx_io->ctx_pinter_refp_pic_width_chroma) / 2;

    int x_offset = pad_luma - (REFP_SURD_OFF_W);
    int y_offset = pad_luma - (REFP_SURD_OFF_H);

    //Y
    int offset_Y = fetch_ctx_io->ctx_info_pic_width + ((1 << 7) + 16) * 2;


    for (j = 0; j < search_window_height; j++) {
//        //#pragma HLS pipeline
        for (i = 0; i < search_window_width; i++) {
            ref_start_x = pix_x - (REFP_SURD_OFF_W)+i;
            ref_start_y = pix_y - (REFP_SURD_OFF_H)+j;
            if (ref_start_x >= -pad_luma && ref_start_x < fetch_ctx_io->ctx_pinter_refp_pic_width_luma + pad_luma && ref_start_y >= -pad_luma
                && ref_start_y < fetch_ctx_io->ctx_pinter_refp_pic_height_luma + pad_luma) {

                Fetch_Refwin_ptr->RefWindowLCU[i + j * search_window_width] = RefbufY[(pix_y + j + y_offset) * offset_Y + (pix_x + i + x_offset)];
            }
            else
            {
                Fetch_Refwin_ptr->RefWindowLCU[i + j * search_window_width] = 0;
            }
        }
    }


    ////////////////////chroma////////////////////////////////////////////
    int c_srw;
    int c_srh;
    c_srw = (REFP_SURD_OFF_W) / 2;
    c_srh = (REFP_SURD_OFF_H) / 2;
    int offset_UV = (fetch_ctx_io->ctx_info_pic_width >> 1) + (((1 << 7) + 16) >> 1) * 2;


    for (j = 0; j < search_window_height >> 1; j++) {
//        //#pragma HLS pipeline
        for (i = 0; i < search_window_width >> 1; i++) {
            ref_start_x = (pix_x >> 1) - c_srw + i;
            ref_start_y = (pix_y >> 1) - c_srh + j;
            if (ref_start_x >= -pad_chroma && ref_start_x < fetch_ctx_io->ctx_pinter_refp_pic_width_chroma + pad_chroma && ref_start_y >= -pad_chroma
                && ref_start_y < fetch_ctx_io->ctx_pinter_refp_pic_height_chroma + pad_chroma) {
                Fetch_Refwin_ptr->RefWindowLCU_UP[i + j * (search_window_width >> 1)] = RefbufU[((pix_y >> 1) + j + y_offset / 2) * offset_UV + ((pix_x >> 1) + i + x_offset / 2)];
                Fetch_Refwin_ptr->RefWindowLCU_VP[i + j * (search_window_width >> 1)] = RefbufV[((pix_y >> 1) + j + y_offset / 2) * offset_UV + ((pix_x >> 1) + i + x_offset / 2)];
            }
        }
    }

}
#endif


void fetch2md(
    FETCH2MD_FW* fetch2md_fw_ptr, 
    FETCH2DBK_FW* fetch2dbk_fw_ptr, 
    FETCH2SAO_FW* fetch2sao_fw_ptr,
    Slice_Level_Parameters* ctu_ctrl_input, 
    CTX_FETCH* fetch_ctx_io,
    U8 ctx_rpm_num_refp_0,
    U8 ctx_rpm_num_refp_1
) 
{

    //ctu_ctrl_input->amvr_enable_flag = 1;
    //ctu_ctrl_input->emvr_enable_flag = 1;

    fetch2dbk_fw_ptr->pic_width_in_lcu = fetch_ctx_io->ctx_info_pic_width_in_lcu;
    fetch2dbk_fw_ptr->pic_height_in_lcu = fetch_ctx_io->ctx_info_pic_height_in_lcu;
    fetch2dbk_fw_ptr->chroma_quant_param_delta_cb = fetch_ctx_io->ctx_info_pic_header_chroma_quant_param_delta_cb;
    fetch2dbk_fw_ptr->chroma_quant_param_delta_cr = fetch_ctx_io->ctx_info_pic_header_chroma_quant_param_delta_cr;
    fetch2dbk_fw_ptr->pic_width = frame_level_fw.pic_width;
    fetch2dbk_fw_ptr->pic_height = frame_level_fw.pic_height;
    fetch2dbk_fw_ptr->qp_offset_bit_depth = frame_level_fw.qp_offset_bit_depth;
    fetch2dbk_fw_ptr->bit_depth = ctu_ctrl_input->bit_depth_internal;

    for (int i = 0; i < MAX_NUM_REF_PICS; ++i)
    {
#pragma HLS pipeline
        fetch2dbk_fw_ptr->ref_ptr_dbk[i][REFP_0]=fetch_ctx_io->ref_ptr_dbk[i][0];
        fetch2dbk_fw_ptr->ref_ptr_dbk[i][REFP_1]=fetch_ctx_io->ref_ptr_dbk[i][1];
    }

    s64 tmp1, tmp2, tmp3;
    //fetch2sao_fw_ptr->lambda[0] = ctu_ctrl_input->lambda[0];
    //fetch2sao_fw_ptr->lambda[1] = ctu_ctrl_input->lambda[1];
    //fetch2sao_fw_ptr->lambda[2] = ctu_ctrl_input->lambda[2];

    fetch2sao_fw_ptr->pic_width = fetch_ctx_io->ctx_info_pic_width;
    fetch2sao_fw_ptr->pic_height = fetch_ctx_io->ctx_info_pic_height;
    fetch2sao_fw_ptr->pic_width_in_scu = fetch_ctx_io->ctx_info_pic_width_in_scu;
    fetch2sao_fw_ptr->pic_width_in_lcu = fetch_ctx_io->ctx_info_pic_width_in_lcu;
    fetch2sao_fw_ptr->pic_height_in_lcu = fetch_ctx_io->ctx_info_pic_height_in_lcu;

    fetch2sao_fw_ptr->max_cuwh = 64;
    fetch2sao_fw_ptr->log2_max_cuwh = fetch_ctx_io->log2_max_cuwh;
    fetch2sao_fw_ptr->log2_culine = 4;
    fetch2sao_fw_ptr->slice_type = fetch_ctx_io->ctx_info_pic_header_slice_type;
    fetch2sao_fw_ptr->num_of_hmvp_cand = fetch_ctx_io->num_of_hmvp_cand;
    //fetch2sao_fw_ptr->amvr_enable_flag = ctu_ctrl_input->amvr_enable_flag;
    //fetch2sao_fw_ptr->emvr_enable_flag = ctu_ctrl_input->emvr_enable_flag;
    fetch2sao_fw_ptr->amvr_enable_flag = 1;
    fetch2sao_fw_ptr->emvr_enable_flag = 1;
    fetch2sao_fw_ptr->dt_intra_enable_flag = 0;
    fetch2sao_fw_ptr->position_based_transform_enable_flag = ((u8)0);
    fetch2sao_fw_ptr->tscpm_enable_flag = fetch_ctx_io->tscpm_enable_flag;
    fetch2sao_fw_ptr->ipf_enable_flag = fetch_ctx_io->ipf_enable_flag;
    //fetch2sao_fw_ptr->num_refp = ((fetch_ctx_io->ctx_info_pic_header_slice_type == 1) ? 0 : 1);
    fetch2sao_fw_ptr->sample_adaptive_offset_enable_flag = fetch_ctx_io->sample_adaptive_offset_enable_flag;
    fetch2sao_fw_ptr->stride_chroma = fetch_ctx_io->stride_chroma;
    fetch2sao_fw_ptr->stride_luma = fetch_ctx_io->stride_luma;
    fetch2sao_fw_ptr->bit_depth = ctu_ctrl_input->bit_depth_internal;
    fetch2sao_fw_ptr->qp_offset_bit_depth=frame_level_fw.qp_offset_bit_depth;
    //fetch2sao_fw_ptr->qp_offset_bit_depth = ctu_ctrl_input->q
    fetch2md_fw_ptr->pic_width = fetch_ctx_io->ctx_info_pic_width;
    fetch2md_fw_ptr->pic_height = fetch_ctx_io->ctx_info_pic_height;
    fetch2md_fw_ptr->slice_type = fetch_ctx_io->ctx_info_pic_header_slice_type;
    fetch2md_fw_ptr->max_part_ratio = fetch_ctx_io->max_part_ratio;
    fetch2md_fw_ptr->pic_width_in_scu = fetch_ctx_io->ctx_info_pic_width_in_scu;
    fetch2md_fw_ptr->pic_height_in_scu = fetch_ctx_io->ctx_info_pic_height_in_scu;
    fetch2md_fw_ptr->log2_max_cuwh = fetch_ctx_io->log2_max_cuwh;
    fetch2md_fw_ptr->min_cu_size = fetch_ctx_io->min_cu_size;
    fetch2md_fw_ptr->max_split_times = fetch_ctx_io->max_split_times;
    fetch2md_fw_ptr->min_qt_size = fetch_ctx_io->min_qt_size;
    fetch2md_fw_ptr->max_bt_size = fetch_ctx_io->max_bt_size;
    fetch2md_fw_ptr->max_eqt_size = fetch_ctx_io->max_eqt_size;
    fetch2md_fw_ptr->bit_depth_internal = ctu_ctrl_input->bit_depth_internal;
    fetch2md_fw_ptr->max_cuwh = 64;
    fetch2md_fw_ptr->log2_culine = 4;
    fetch2md_fw_ptr->affine_enable_flag=fetch_ctx_io->affine_enable_flag;
#if CUDQP_QP_MAP
    fetch2md_fw_ptr->chroma_quant_param_delta_cb = fetch_ctx_io->ctx_info_pic_header_chroma_quant_param_delta_cb;
    fetch2md_fw_ptr->chroma_quant_param_delta_cr = fetch_ctx_io->ctx_info_pic_header_chroma_quant_param_delta_cr;
#endif

#if UMVE_ENH
    fetch2md_fw_ptr->umve_set_flag=fetch_ctx_io->umve_set_flag;
#endif

#if EIPM
    fetch2md_fw_ptr->eipm_enable_flag = fetch_ctx_io->eipm_enable_flag;
#endif
#if MIPF
    //fetch2sao_fw_ptr->mipf_enable_flag = ctu_ctrl_input->mipf_enable_flag;
#endif
#if MIPF
    fetch2md_fw_ptr->mipf_enable_flag = ctu_ctrl_input->mipf_enable_flag;
#endif
#if INTERPF
    fetch2md_fw_ptr->interpf_enable_flag = fetch_ctx_io->interpf_enable_flag;
#endif
    if (IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type)) {
        for (int i = 0; i < 17; i++) {
#pragma HLS unroll
            for (int j = 0; j < 2; j++) {
#pragma HLS unroll
                fetch2md_fw_ptr->refp_ptr[i][j] = fetch_ctx_io->ctx_pinter_refp_ptr[i][j];
                fetch2md_fw_ptr->refp_ptr_1[i][j] = fetch_ctx_io->ctx_pinter_refp_ptr[i][j];
            }
        }
        fetch2md_fw_ptr->cur_ptr = fetch_ctx_io->ctx_ptr;
#if ENABLE_BFRAME
        fetch2md_fw_ptr->num_refp[0] = ctx_rpm_num_refp_0;
        fetch2sao_fw_ptr->num_refp[0] = ctx_rpm_num_refp_0;
        fetch2md_fw_ptr->num_refp[1] = ctx_rpm_num_refp_1;
        fetch2sao_fw_ptr->num_refp[1] = ctx_rpm_num_refp_1;
#else
        fetch2md_fw_ptr->num_refp = fetch_ctx_io->ctx_rpm_num_refp;
        fetch2sao_fw_ptr->num_refp = fetch_ctx_io->ctx_rpm_num_refp;
#endif
        
        fetch2sao_fw_ptr->cur_ptr = fetch_ctx_io->ctx_ptr;
        
    }
    else {
        for (int i = 0; i < 17; i++) {
#pragma HLS unroll
            for (int j = 0; j < 2; j++) {
#pragma HLS unroll
                fetch2md_fw_ptr->refp_ptr[i][j] = -1;
                fetch2md_fw_ptr->refp_ptr_1[i][j] = -1;
            }
        }
        fetch2md_fw_ptr->cur_ptr = 0;
        
        fetch2sao_fw_ptr->cur_ptr = 0;
        
#if ENABLE_BFRAME
        // fetch2sao_fw_ptr->num_refp[0] = 0;
        // fetch2md_fw_ptr->num_refp[0] = 0;
        // fetch2sao_fw_ptr->num_refp[1] = 0;
        // fetch2md_fw_ptr->num_refp[1] = 0;
        fetch2md_fw_ptr->num_refp[0] = ctx_rpm_num_refp_0;
        fetch2sao_fw_ptr->num_refp[0] = ctx_rpm_num_refp_0;
        fetch2md_fw_ptr->num_refp[1] = ctx_rpm_num_refp_1;
        fetch2sao_fw_ptr->num_refp[1] = ctx_rpm_num_refp_1;
#else
        fetch2sao_fw_ptr->num_refp = 0;
        fetch2md_fw_ptr->num_refp = 0;
#endif
    }
}

pel imgY_org_align16[3840*2160];
pel imgU_org_align16[1920*1080];
pel imgV_org_align16[1920*1080];
uint128_t imgY_org_buf[3840*2160/8];
ap_uint<64> imgU_org_buf[1920*1080/8];
ap_uint<64> imgV_org_buf[1920*1080/8];

#if DEBUG_CHECK
pel imgY_ref_align16[(3840+288)*(2160+288)];
pel imgU_ref_align16[(1920+144)*(1080+144)];
pel imgV_ref_align16[(1920+144)*(1080+144)];
uint128_t imgY_ref_buf[(3840+288)*(2160+288)/NUM_ELEMENTS];
uint128_t imgU_ref_buf[(1920+144)*(1080+144)/NUM_ELEMENTS];
uint128_t imgV_ref_buf[(1920+144)*(1080+144)/NUM_ELEMENTS];
pel imgY_b_align16[(3840 + 288) * (2160 + 288)];
pel imgU_b_align16[(1920 + 144) * (1080 + 144)];
pel imgV_b_align16[(1920 + 144) * (1080 + 144)];
uint128_t imgY_b_buf[(3840 + 288) * (2160 + 288) / NUM_ELEMENTS];
uint128_t imgU_b_buf[(1920 + 144) * (1080 + 144) / NUM_ELEMENTS];
uint128_t imgV_b_buf[(1920 + 144) * (1080 + 144) / NUM_ELEMENTS];
#endif

U32 rtl_use_flag_y[64];
U32 rtl_use_flag_u[64];
U32 rtl_use_flag_v[64];
pel refWinY32x192_ref[192][32];
pel refWinU16x96_ref[96][16];
pel refWinV16x96_ref[96][16];

U32 rtl_use_flag_y_B[64];
U32 rtl_use_flag_u_B[64];
U32 rtl_use_flag_v_B[64];
pel refWinY32x192_B_ref[192][32];
pel refWinU16x96_B_ref[96][16];
pel refWinV16x96_B_ref[96][16];

#ifndef __SYNTHESIS__
pel refWinY0_ref[192][64];//0
pel refWinY1_ref[192][64];//1
pel refWinY2_ref[192][64];//2
pel refWinY3_ref[192][64];//3
pel refWinU0_ref[96][32];//0
pel refWinU1_ref[96][32];//1
pel refWinU2_ref[96][32];//2
pel refWinU3_ref[96][32];//3
pel refWinV0_ref[96][32];//0
pel refWinV1_ref[96][32];//1
pel refWinV2_ref[96][32];//2
pel refWinV3_ref[96][32];//3
pel refWinY0_B_ref[192][64];//0
pel refWinY1_B_ref[192][64];//1
pel refWinY2_B_ref[192][64];//2
pel refWinY3_B_ref[192][64];//3
pel refWinU0_B_ref[96][32];//0
pel refWinU1_B_ref[96][32];//1
pel refWinU2_B_ref[96][32];//2
pel refWinU3_B_ref[96][32];//3
pel refWinV0_B_ref[96][32];//0
pel refWinV1_B_ref[96][32];//1
pel refWinV2_B_ref[96][32];//2
pel refWinV3_B_ref[96][32];//3

void get_ctu_data(strFetch_input_video_U8_ptr* Fetch_input_video_U8_ptr, CTX_FETCH* fetch_ctx_io, U8 ctx_rpm_num_refp_0,U8 ctx_rpm_num_refp_1,
    strFetch_ref_window* Fetch_Refwin_ptr, int code_stru_temp,
    int tmp_pix_y, int tmp_pix_x, int luma_width, int luma_heigh, int chroma_width, int chroma_heigh)
{

    int luma_mod_num_col1 = luma_width % 64;
    int luma_mod_num_row1 = luma_heigh % 64;
    int chroma_mod_num_col1 = chroma_width % 32;
    int chroma_mod_num_row1 = chroma_heigh % 32;
    int offset_luma_x1 = luma_mod_num_col1 ? 128 : 192;
    int offset_luma_y1 = luma_mod_num_row1 ? 64 : 128;

    int luma_width_padded = (fetch_ctx_io->ctx_pinter_refp_pic_width_luma + 63) / 64 * 64;
    int luma_height_padded = (fetch_ctx_io->ctx_pinter_refp_pic_height_luma + 63) / 64 * 64;
    int chroma_width_padded = (fetch_ctx_io->ctx_pinter_refp_pic_width_chroma + 31) / 32 * 32;
    int chroma_height_padded = (fetch_ctx_io->ctx_pinter_refp_pic_height_chroma + 31) / 32 * 32;

    //first row
    if (0 == tmp_pix_y)
    {
        if (0 == tmp_pix_x)
        {
            if ((tmp_pix_x + 128) <= (luma_width_padded - 64))
            {
                for (int i = 0; i < 2; ++i)
                {
                    ref_fetch_lcu_in_rfc_1(tmp_pix_x, tmp_pix_y + i * 64, fetch_ctx_io, Fetch_input_video_U8_ptr->y_rfcBin,
                        Fetch_input_video_U8_ptr->y_rfcPrm, Fetch_input_video_U8_ptr->u_rfcBin, Fetch_input_video_U8_ptr->u_rfcPrm,
                        Fetch_input_video_U8_ptr->v_rfcBin, Fetch_input_video_U8_ptr->v_rfcPrm,
                        refWinY1_ref + (i + 1) * 64, refWinU1_ref + (i + 1) * 32, refWinV1_ref + (i + 1) * 32);

                    ref_fetch_lcu_in_rfc_2(tmp_pix_x + 64, tmp_pix_y + i * 64, fetch_ctx_io, Fetch_input_video_U8_ptr->y_rfcBin,
                        Fetch_input_video_U8_ptr->y_rfcPrm, Fetch_input_video_U8_ptr->u_rfcBin, Fetch_input_video_U8_ptr->u_rfcPrm,
                        Fetch_input_video_U8_ptr->v_rfcBin, Fetch_input_video_U8_ptr->v_rfcPrm,
                        refWinY2_ref + (i + 1) * 64, refWinU2_ref + (i + 1) * 32, refWinV2_ref + (i + 1) * 32);

                    ref_fetch_lcu_in_rfc_3(tmp_pix_x + 128, tmp_pix_y + i * 64, fetch_ctx_io, Fetch_input_video_U8_ptr->y_rfcBin,
                        Fetch_input_video_U8_ptr->y_rfcPrm, Fetch_input_video_U8_ptr->u_rfcBin, Fetch_input_video_U8_ptr->u_rfcPrm,
                        Fetch_input_video_U8_ptr->v_rfcBin, Fetch_input_video_U8_ptr->v_rfcPrm,
                        refWinY3_ref + (i + 1) * 64, refWinU3_ref + (i + 1) * 32, refWinV3_ref + (i + 1) * 32);

                    if ((code_stru_temp != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) &&
                         ctx_rpm_num_refp_0 > 1) || (code_stru_temp == RA &&
                            fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B))
                    {
                        ref_fetch_lcu_in_rfc_1(tmp_pix_x, tmp_pix_y + i * 64, fetch_ctx_io, Fetch_input_video_U8_ptr->y_rfcBin1,
                            Fetch_input_video_U8_ptr->y_rfcPrm1, Fetch_input_video_U8_ptr->u_rfcBin1, Fetch_input_video_U8_ptr->u_rfcPrm1,
                            Fetch_input_video_U8_ptr->v_rfcBin1, Fetch_input_video_U8_ptr->v_rfcPrm1,
                            refWinY1_B_ref + (i + 1) * 64, refWinU1_B_ref + (i + 1) * 32, refWinV1_B_ref + (i + 1) * 32);

                        ref_fetch_lcu_in_rfc_2(tmp_pix_x + 64, tmp_pix_y + i * 64, fetch_ctx_io, Fetch_input_video_U8_ptr->y_rfcBin1,
                            Fetch_input_video_U8_ptr->y_rfcPrm1, Fetch_input_video_U8_ptr->u_rfcBin1, Fetch_input_video_U8_ptr->u_rfcPrm1,
                            Fetch_input_video_U8_ptr->v_rfcBin1, Fetch_input_video_U8_ptr->v_rfcPrm1,
                            refWinY2_B_ref + (i + 1) * 64, refWinU2_B_ref + (i + 1) * 32, refWinV2_B_ref + (i + 1) * 32);

                        ref_fetch_lcu_in_rfc_3(tmp_pix_x + 128, tmp_pix_y + i * 64, fetch_ctx_io, Fetch_input_video_U8_ptr->y_rfcBin1,
                            Fetch_input_video_U8_ptr->y_rfcPrm1, Fetch_input_video_U8_ptr->u_rfcBin1, Fetch_input_video_U8_ptr->u_rfcPrm1,
                            Fetch_input_video_U8_ptr->v_rfcBin1, Fetch_input_video_U8_ptr->v_rfcPrm1,
                            refWinY3_B_ref + (i + 1) * 64, refWinU3_B_ref + (i + 1) * 32, refWinV3_B_ref + (i + 1) * 32);
                    }
                }

                if ((tmp_pix_x + 128) == (luma_width_padded - 64))
                {
                    int offset_luma_mod_col2 = luma_mod_num_col1 ? luma_mod_num_col1 : 64;
                    int offset_chroma_mod_col2 = chroma_mod_num_col1 ? chroma_mod_num_col1 : 32;

                    for (int j = 64; j < 192; ++j)
                    {
                        for (int i = offset_luma_mod_col2; i < 64; ++i)
                        {
                            refWinY3_ref[j][i] = refWinY3_ref[j][luma_mod_num_col1 - 1];
                        }
                    }

                    for (int j = 32; j < 96; ++j)
                    {
                        for (int i = offset_chroma_mod_col2; i < 32; ++i)
                        {
                            refWinU3_ref[j][i] = refWinU3_ref[j][chroma_mod_num_col1 - 1];
                            refWinV3_ref[j][i] = refWinV3_ref[j][chroma_mod_num_col1 - 1];
                        }
                    }

                    if ((code_stru_temp != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) &&
                         ctx_rpm_num_refp_0 > 1) || (code_stru_temp == RA &&
                            fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B))
                    {
                        for (int j = 64; j < 192; ++j)
                        {
                            for (int i = offset_luma_mod_col2; i < 64; ++i)
                            {
                                refWinY3_B_ref[j][i] = refWinY3_B_ref[j][luma_mod_num_col1 - 1];
                            }
                        }

                        for (int j = 32; j < 96; ++j)
                        {
                            for (int i = offset_chroma_mod_col2; i < 32; ++i)
                            {
                                refWinU3_B_ref[j][i] = refWinU3_B_ref[j][chroma_mod_num_col1 - 1];
                                refWinV3_B_ref[j][i] = refWinV3_B_ref[j][chroma_mod_num_col1 - 1];
                            }
                        }
                    }
                }
                if ((tmp_pix_y + 128) == luma_height_padded)
                {
                    int offset_luma_mod_row2 = luma_mod_num_row1 ? luma_mod_num_row1 : 64;
                    int offset_chroma_mod_row2 = chroma_mod_num_row1 ? chroma_mod_num_row1 : 32;

                    for (int j = 128 + offset_luma_mod_row2; j < 192; ++j)
                    {
                        for (int i = 0; i < 32; ++i)
                        {
                            refWinY1_ref[j][i] = refWinY1_ref[128 + offset_luma_mod_row2 - 1][i];
                            refWinY1_ref[j][i + 32] = refWinY1_ref[128 + offset_luma_mod_row2 - 1][i + 32];

                            refWinY2_ref[j][i] = refWinY2_ref[128 + offset_luma_mod_row2 - 1][i];
                            refWinY2_ref[j][i + 32] = refWinY2_ref[128 + offset_luma_mod_row2 - 1][i + 32];

                            refWinY3_ref[j][i] = refWinY3_ref[128 + offset_luma_mod_row2 - 1][i];
                            refWinY3_ref[j][i + 32] = refWinY3_ref[128 + offset_luma_mod_row2 - 1][i + 32];
                        }
                    }

                    for (int j = 64 + offset_chroma_mod_row2; j < 96; ++j)
                    {
                        for (int i = 0; i < 16; ++i)
                        {
                            refWinU1_ref[j][i] = refWinU1_ref[64 + offset_chroma_mod_row2 - 1][i];
                            refWinU1_ref[j][i + 16] = refWinU1_ref[64 + offset_chroma_mod_row2 - 1][i + 16];
                            refWinU2_ref[j][i] = refWinU2_ref[64 + offset_chroma_mod_row2 - 1][i];
                            refWinU2_ref[j][i + 16] = refWinU2_ref[64 + offset_chroma_mod_row2 - 1][i + 16];
                            refWinU3_ref[j][i] = refWinU3_ref[64 + offset_chroma_mod_row2 - 1][i];
                            refWinU3_ref[j][i + 16] = refWinU3_ref[64 + offset_chroma_mod_row2 - 1][i + 16];

                            refWinV1_ref[j][i] = refWinV1_ref[64 + offset_chroma_mod_row2 - 1][i];
                            refWinV1_ref[j][i + 16] = refWinV1_ref[64 + offset_chroma_mod_row2 - 1][i + 16];
                            refWinV2_ref[j][i] = refWinV2_ref[64 + offset_chroma_mod_row2 - 1][i];
                            refWinV2_ref[j][i + 16] = refWinV2_ref[64 + offset_chroma_mod_row2 - 1][i + 16];
                            refWinV3_ref[j][i] = refWinV3_ref[64 + offset_chroma_mod_row2 - 1][i];
                            refWinV3_ref[j][i + 16] = refWinV3_ref[64 + offset_chroma_mod_row2 - 1][i + 16];
                        }
                    }

                    if ((code_stru_temp != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) &&
                         ctx_rpm_num_refp_0 > 1) || (code_stru_temp == RA &&
                            fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B))
                    {
                        for (int j = 128 + offset_luma_mod_row2; j < 192; ++j)
                        {
                            for (int i = 0; i < 32; ++i)
                            {

                                refWinY1_B_ref[j][i] = refWinY1_B_ref[128 + offset_luma_mod_row2 - 1][i];
                                refWinY1_B_ref[j][i + 32] = refWinY1_B_ref[128 + offset_luma_mod_row2 - 1][i + 32];

                                refWinY2_B_ref[j][i] = refWinY2_B_ref[128 + offset_luma_mod_row2 - 1][i];
                                refWinY2_B_ref[j][i + 32] = refWinY2_B_ref[128 + offset_luma_mod_row2 - 1][i + 32];

                                refWinY3_B_ref[j][i] = refWinY3_B_ref[128 + offset_luma_mod_row2 - 1][i];
                                refWinY3_B_ref[j][i + 32] = refWinY3_B_ref[128 + offset_luma_mod_row2 - 1][i + 32];
                            }
                        }

                        for (int j = 64 + offset_chroma_mod_row2; j < 96; ++j)
                        {
                            for (int i = 0; i < 16; ++i)
                            {
                                refWinU1_B_ref[j][i] = refWinU1_B_ref[64 + offset_chroma_mod_row2 - 1][i];
                                refWinU1_B_ref[j][i + 16] = refWinU1_B_ref[64 + offset_chroma_mod_row2 - 1][i + 16];
                                refWinU2_B_ref[j][i] = refWinU2_B_ref[64 + offset_chroma_mod_row2 - 1][i];
                                refWinU2_B_ref[j][i + 16] = refWinU2_B_ref[64 + offset_chroma_mod_row2 - 1][i + 16];
                                refWinU3_B_ref[j][i] = refWinU3_B_ref[64 + offset_chroma_mod_row2 - 1][i];
                                refWinU3_B_ref[j][i + 16] = refWinU3_B_ref[64 + offset_chroma_mod_row2 - 1][i + 16];

                                refWinV1_B_ref[j][i] = refWinV1_B_ref[64 + offset_chroma_mod_row2 - 1][i];
                                refWinV1_B_ref[j][i + 16] = refWinV1_B_ref[64 + offset_chroma_mod_row2 - 1][i + 16];
                                refWinV2_B_ref[j][i] = refWinV2_B_ref[64 + offset_chroma_mod_row2 - 1][i];
                                refWinV2_B_ref[j][i + 16] = refWinV2_B_ref[64 + offset_chroma_mod_row2 - 1][i + 16];
                                refWinV3_B_ref[j][i] = refWinV3_B_ref[64 + offset_chroma_mod_row2 - 1][i];
                                refWinV3_B_ref[j][i + 16] = refWinV3_B_ref[64 + offset_chroma_mod_row2 - 1][i + 16];
                            }
                        }
                    }
                }
            }
            else if ((tmp_pix_x + 128) == luma_width_padded)
            {
                for (int i = 0; i < 2; ++i)
                {
                    ref_fetch_lcu_in_rfc_1(tmp_pix_x, tmp_pix_y + i * 64, fetch_ctx_io, Fetch_input_video_U8_ptr->y_rfcBin,
                        Fetch_input_video_U8_ptr->y_rfcPrm, Fetch_input_video_U8_ptr->u_rfcBin, Fetch_input_video_U8_ptr->u_rfcPrm,
                        Fetch_input_video_U8_ptr->v_rfcBin, Fetch_input_video_U8_ptr->v_rfcPrm,
                        refWinY1_ref + (i + 1) * 64, refWinU1_ref + (i + 1) * 32, refWinV1_ref + (i + 1) * 32);

                    ref_fetch_lcu_in_rfc_2(tmp_pix_x + 64, tmp_pix_y + i * 64, fetch_ctx_io, Fetch_input_video_U8_ptr->y_rfcBin,
                        Fetch_input_video_U8_ptr->y_rfcPrm, Fetch_input_video_U8_ptr->u_rfcBin, Fetch_input_video_U8_ptr->u_rfcPrm,
                        Fetch_input_video_U8_ptr->v_rfcBin, Fetch_input_video_U8_ptr->v_rfcPrm,
                        refWinY2_ref + (i + 1) * 64, refWinU2_ref + (i + 1) * 32, refWinV2_ref + (i + 1) * 32);

                    if ((code_stru_temp != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) &&
                         ctx_rpm_num_refp_0 > 1) || (code_stru_temp == RA &&
                            fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B))
                    {
                        ref_fetch_lcu_in_rfc_1(tmp_pix_x, tmp_pix_y + i * 64, fetch_ctx_io, Fetch_input_video_U8_ptr->y_rfcBin1,
                            Fetch_input_video_U8_ptr->y_rfcPrm1, Fetch_input_video_U8_ptr->u_rfcBin1, Fetch_input_video_U8_ptr->u_rfcPrm1,
                            Fetch_input_video_U8_ptr->v_rfcBin1, Fetch_input_video_U8_ptr->v_rfcPrm1,
                            refWinY1_B_ref + (i + 1) * 64, refWinU1_B_ref + (i + 1) * 32, refWinV1_B_ref + (i + 1) * 32);

                        ref_fetch_lcu_in_rfc_2(tmp_pix_x + 64, tmp_pix_y + i * 64, fetch_ctx_io, Fetch_input_video_U8_ptr->y_rfcBin1,
                            Fetch_input_video_U8_ptr->y_rfcPrm1, Fetch_input_video_U8_ptr->u_rfcBin1, Fetch_input_video_U8_ptr->u_rfcPrm1,
                            Fetch_input_video_U8_ptr->v_rfcBin1, Fetch_input_video_U8_ptr->v_rfcPrm1,
                            refWinY2_B_ref + (i + 1) * 64, refWinU2_B_ref + (i + 1) * 32, refWinV2_B_ref + (i + 1) * 32);
                    }
                }

                if ((tmp_pix_x + 128) == luma_width_padded)
                {
                    int offset_luma_mod_col2 = luma_mod_num_col1 ? luma_mod_num_col1 : 64;
                    int offset_chroma_mod_col2 = chroma_mod_num_col1 ? chroma_mod_num_col1 : 32;

                    for (int j = 64; j < 192; ++j)
                    {
                        for (int i = offset_luma_mod_col2; i < 64; ++i)
                        {
                            refWinY2_ref[j][i] = refWinY2_ref[j][luma_mod_num_col1 - 1];
                        }
                    }

                    for (int j = 32; j < 96; ++j)
                    {
                        for (int i = offset_chroma_mod_col2; i < 32; ++i)
                        {
                            refWinU2_ref[j][i] = refWinU2_ref[j][chroma_mod_num_col1 - 1];
                            refWinV2_ref[j][i] = refWinV2_ref[j][chroma_mod_num_col1 - 1];
                        }
                    }

                    if ((code_stru_temp != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) &&
                         ctx_rpm_num_refp_0 > 1) || (code_stru_temp == RA &&
                            fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B))
                    {
                        for (int j = 64; j < 192; ++j)
                        {
                            for (int i = offset_luma_mod_col2; i < 64; ++i)
                            {
                                refWinY2_B_ref[j][i] = refWinY2_B_ref[j][luma_mod_num_col1 - 1];
                            }
                        }

                        for (int j = 32; j < 96; ++j)
                        {
                            for (int i = offset_chroma_mod_col2; i < 32; ++i)
                            {
                                refWinU2_B_ref[j][i] = refWinU2_B_ref[j][chroma_mod_num_col1 - 1];
                                refWinV2_B_ref[j][i] = refWinV2_B_ref[j][chroma_mod_num_col1 - 1];
                            }
                        }
                    }
                }

                if ((tmp_pix_y + 128) == luma_height_padded)
                {
                    int offset_luma_mod_row2 = luma_mod_num_row1 ? luma_mod_num_row1 : 64;
                    int offset_chroma_mod_row2 = chroma_mod_num_row1 ? chroma_mod_num_row1 : 32;

                    for (int j = 128 + offset_luma_mod_row2; j < 192; ++j)
                    {
                        for (int i = 0; i < 32; ++i)
                        {
                            refWinY1_ref[j][i] = refWinY1_ref[128 + offset_luma_mod_row2 - 1][i];
                            refWinY1_ref[j][i + 32] = refWinY1_ref[128 + offset_luma_mod_row2 - 1][i + 32];

                            refWinY2_ref[j][i] = refWinY2_ref[128 + offset_luma_mod_row2 - 1][i];
                            refWinY2_ref[j][i + 32] = refWinY2_ref[128 + offset_luma_mod_row2 - 1][i + 32];
                        }
                    }

                    for (int j = 64 + offset_chroma_mod_row2; j < 96; ++j)
                    {
                        for (int i = 0; i < 16; ++i)
                        {
                            refWinU1_ref[j][i] = refWinU1_ref[64 + offset_chroma_mod_row2 - 1][i];
                            refWinU1_ref[j][i + 16] = refWinU1_ref[64 + offset_chroma_mod_row2 - 1][i + 16];
                            refWinU2_ref[j][i] = refWinU2_ref[64 + offset_chroma_mod_row2 - 1][i];
                            refWinU2_ref[j][i + 16] = refWinU2_ref[64 + offset_chroma_mod_row2 - 1][i + 16];

                            refWinV1_ref[j][i] = refWinV1_ref[64 + offset_chroma_mod_row2 - 1][i];
                            refWinV1_ref[j][i + 16] = refWinV1_ref[64 + offset_chroma_mod_row2 - 1][i + 16];
                            refWinV2_ref[j][i] = refWinV2_ref[64 + offset_chroma_mod_row2 - 1][i];
                            refWinV2_ref[j][i + 16] = refWinV2_ref[64 + offset_chroma_mod_row2 - 1][i + 16];
                        }
                    }

                    if ((code_stru_temp != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) &&
                         ctx_rpm_num_refp_0 > 1) || (code_stru_temp == RA &&
                            fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B))
                    {
                        for (int j = 128 + offset_luma_mod_row2; j < 192; ++j)
                        {
                            for (int i = 0; i < 32; ++i)
                            {
                                refWinY1_B_ref[j][i] = refWinY1_B_ref[128 + offset_luma_mod_row2 - 1][i];
                                refWinY1_B_ref[j][i + 32] = refWinY1_B_ref[128 + offset_luma_mod_row2 - 1][i + 32];

                                refWinY2_B_ref[j][i] = refWinY2_B_ref[128 + offset_luma_mod_row2 - 1][i];
                                refWinY2_B_ref[j][i + 32] = refWinY2_B_ref[128 + offset_luma_mod_row2 - 1][i + 32];
                            }
                        }

                        for (int j = 64 + offset_chroma_mod_row2; j < 96; ++j)
                        {
                            for (int i = 0; i < 16; ++i)
                            {
                                refWinU1_B_ref[j][i] = refWinU1_B_ref[64 + offset_chroma_mod_row2 - 1][i];
                                refWinU1_B_ref[j][i + 16] = refWinU1_B_ref[64 + offset_chroma_mod_row2 - 1][i + 16];
                                refWinU2_B_ref[j][i] = refWinU2_B_ref[64 + offset_chroma_mod_row2 - 1][i];
                                refWinU2_B_ref[j][i + 16] = refWinU2_B_ref[64 + offset_chroma_mod_row2 - 1][i + 16];

                                refWinV1_B_ref[j][i] = refWinV1_B_ref[64 + offset_chroma_mod_row2 - 1][i];
                                refWinV1_B_ref[j][i + 16] = refWinV1_B_ref[64 + offset_chroma_mod_row2 - 1][i + 16];
                                refWinV2_B_ref[j][i] = refWinV2_B_ref[64 + offset_chroma_mod_row2 - 1][i];
                                refWinV2_B_ref[j][i + 16] = refWinV2_B_ref[64 + offset_chroma_mod_row2 - 1][i + 16];
                            }
                        }
                    }
                }

                int col_ctu_nums = (luma_width_padded / 64 - 2) * 64;
                if ((tmp_pix_y + 128) == luma_height_padded && col_ctu_nums >= 0)
                {
                    for (int i = 0; i < 2; ++i)
                    {
                        ref_fetch_lcu_in_rfc_3(tmp_pix_x - col_ctu_nums, tmp_pix_y + i * 64, fetch_ctx_io, Fetch_input_video_U8_ptr->y_rfcBin,
                            Fetch_input_video_U8_ptr->y_rfcPrm, Fetch_input_video_U8_ptr->u_rfcBin, Fetch_input_video_U8_ptr->u_rfcPrm,
                            Fetch_input_video_U8_ptr->v_rfcBin, Fetch_input_video_U8_ptr->v_rfcPrm,
                            refWinY3_ref + i * 64, refWinU3_ref + i * 32, refWinV3_ref + i * 32);

                        if ((code_stru_temp != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) &&
                             ctx_rpm_num_refp_0 > 1) || (code_stru_temp == RA &&
                                fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B))
                        {
                            ref_fetch_lcu_in_rfc_3(tmp_pix_x - col_ctu_nums, tmp_pix_y + i * 64, fetch_ctx_io, Fetch_input_video_U8_ptr->y_rfcBin1,
                                Fetch_input_video_U8_ptr->y_rfcPrm1, Fetch_input_video_U8_ptr->u_rfcBin1, Fetch_input_video_U8_ptr->u_rfcPrm1,
                                Fetch_input_video_U8_ptr->v_rfcBin1, Fetch_input_video_U8_ptr->v_rfcPrm1,
                                refWinY3_B_ref + i * 64, refWinU3_B_ref + i * 32, refWinV3_B_ref + i * 32);
                        }
                    }

                    if ((tmp_pix_x - col_ctu_nums + 64) == luma_width_padded)
                    {
                        int offset_luma_mod_col2 = luma_mod_num_col1 ? luma_mod_num_col1 : 64;
                        int offset_chroma_mod_col2 = chroma_mod_num_col1 ? chroma_mod_num_col1 : 32;

                        for (int j = 0; j < 128; ++j)
                        {
                            for (int i = offset_luma_mod_col2; i < 64; ++i)
                            {
                                refWinY3_ref[j][i] = refWinY3_ref[j][luma_mod_num_col1 - 1];
                            }
                        }

                        for (int j = 0; j < 64; ++j)
                        {
                            for (int i = offset_chroma_mod_col2; i < 32; ++i)
                            {
                                refWinU3_ref[j][i] = refWinU3_ref[j][chroma_mod_num_col1 - 1];
                                refWinV3_ref[j][i] = refWinV3_ref[j][chroma_mod_num_col1 - 1];
                            }
                        }

                        if ((code_stru_temp != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) &&
                             ctx_rpm_num_refp_0 > 1) || (code_stru_temp == RA &&
                                fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B))
                        {
                            for (int j = 0; j < 128; ++j)
                            {
                                for (int i = offset_luma_mod_col2; i < 64; ++i)
                                {
                                    refWinY3_B_ref[j][i] = refWinY3_B_ref[j][luma_mod_num_col1 - 1];
                                }
                            }

                            for (int j = 0; j < 64; ++j)
                            {
                                for (int i = offset_chroma_mod_col2; i < 32; ++i)
                                {
                                    refWinU3_B_ref[j][i] = refWinU3_B_ref[j][chroma_mod_num_col1 - 1];
                                    refWinV3_B_ref[j][i] = refWinV3_B_ref[j][chroma_mod_num_col1 - 1];
                                }
                            }
                        }
                    }

                    int offset_luma_mod_row2 = luma_mod_num_row1 ? luma_mod_num_row1 : 64;
                    int offset_chroma_mod_row2 = chroma_mod_num_row1 ? chroma_mod_num_row1 : 32;

                    for (int j = 64 + offset_luma_mod_row2; j < 128; ++j)
                    {
                        for (int i = 0; i < 32; ++i)
                        {
                            refWinY3_ref[j][i] = refWinY3_ref[64 + offset_luma_mod_row2 - 1][i];
                            refWinY3_ref[j][i + 32] = refWinY3_ref[64 + offset_luma_mod_row2 - 1][i + 32];
                        }
                    }

                    for (int j = 32 + offset_chroma_mod_row2; j < 64; ++j)
                    {
                        for (int i = 0; i < 16; ++i)
                        {
                            refWinU3_ref[j][i] = refWinU3_ref[32 + offset_chroma_mod_row2 - 1][i];
                            refWinU3_ref[j][i + 16] = refWinU3_ref[32 + offset_chroma_mod_row2 - 1][i + 16];

                            refWinV3_ref[j][i] = refWinV3_ref[32 + offset_chroma_mod_row2 - 1][i];
                            refWinV3_ref[j][i + 16] = refWinV3_ref[32 + offset_chroma_mod_row2 - 1][i + 16];
                        }
                    }

                    if ((code_stru_temp != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) &&
                         ctx_rpm_num_refp_0 > 1) || (code_stru_temp == RA &&
                            fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B))
                    {
                        for (int j = 64 + offset_luma_mod_row2; j < 128; ++j)
                        {
                            for (int i = 0; i < 32; ++i)
                            {
                                refWinY3_B_ref[j][i] = refWinY3_B_ref[64 + offset_luma_mod_row2 - 1][i];
                                refWinY3_B_ref[j][i + 32] = refWinY3_B_ref[64 + offset_luma_mod_row2 - 1][i + 32];
                            }
                        }

                        for (int j = 32 + offset_chroma_mod_row2; j < 64; ++j)
                        {
                            for (int i = 0; i < 16; ++i)
                            {
                                refWinU3_B_ref[j][i] = refWinU3_B_ref[32 + offset_chroma_mod_row2 - 1][i];
                                refWinU3_B_ref[j][i + 16] = refWinU3_B_ref[32 + offset_chroma_mod_row2 - 1][i + 16];

                                refWinV3_B_ref[j][i] = refWinV3_B_ref[32 + offset_chroma_mod_row2 - 1][i];
                                refWinV3_B_ref[j][i + 16] = refWinV3_B_ref[32 + offset_chroma_mod_row2 - 1][i + 16];
                            }
                        }
                    }
                }
                else if ((tmp_pix_y + 192) <= luma_height_padded && col_ctu_nums >= 0)
                {
                    for (int i = 0; i < 3; ++i)
                    {
                        ref_fetch_lcu_in_rfc_3(tmp_pix_x - col_ctu_nums, tmp_pix_y + i * 64, fetch_ctx_io, Fetch_input_video_U8_ptr->y_rfcBin,
                            Fetch_input_video_U8_ptr->y_rfcPrm, Fetch_input_video_U8_ptr->u_rfcBin, Fetch_input_video_U8_ptr->u_rfcPrm,
                            Fetch_input_video_U8_ptr->v_rfcBin, Fetch_input_video_U8_ptr->v_rfcPrm,
                            refWinY3_ref + i * 64, refWinU3_ref + i * 32, refWinV3_ref + i * 32);

                        if ((code_stru_temp != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) &&
                             ctx_rpm_num_refp_0 > 1) || (code_stru_temp == RA &&
                                fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B))
                        {
                            ref_fetch_lcu_in_rfc_3(tmp_pix_x - col_ctu_nums, tmp_pix_y + i * 64, fetch_ctx_io, Fetch_input_video_U8_ptr->y_rfcBin1,
                                Fetch_input_video_U8_ptr->y_rfcPrm1, Fetch_input_video_U8_ptr->u_rfcBin1, Fetch_input_video_U8_ptr->u_rfcPrm1,
                                Fetch_input_video_U8_ptr->v_rfcBin1, Fetch_input_video_U8_ptr->v_rfcPrm1,
                                refWinY3_B_ref + i * 64, refWinU3_B_ref + i * 32, refWinV3_B_ref + i * 32);
                        }
                    }

                    if ((tmp_pix_x - col_ctu_nums + 64) == luma_width_padded)
                    {
                        int offset_luma_mod_col2 = luma_mod_num_col1 ? luma_mod_num_col1 : 64;
                        int offset_chroma_mod_col2 = chroma_mod_num_col1 ? chroma_mod_num_col1 : 32;

                        for (int j = 0; j < 192; ++j)
                        {
                            for (int i = offset_luma_mod_col2; i < 64; ++i)
                            {
                                refWinY3_ref[j][i] = refWinY3_ref[j][luma_mod_num_col1 - 1];
                            }
                        }

                        for (int j = 0; j < 96; ++j)
                        {
                            for (int i = offset_chroma_mod_col2; i < 32; ++i)
                            {
                                refWinU3_ref[j][i] = refWinU3_ref[j][chroma_mod_num_col1 - 1];
                                refWinV3_ref[j][i] = refWinV3_ref[j][chroma_mod_num_col1 - 1];
                            }
                        }

                        if ((code_stru_temp != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) &&
                             ctx_rpm_num_refp_0 > 1) || (code_stru_temp == RA &&
                                fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B))
                        {
                            for (int j = 0; j < 192; ++j)
                            {
                                for (int i = offset_luma_mod_col2; i < 64; ++i)
                                {
                                    refWinY3_B_ref[j][i] = refWinY3_B_ref[j][luma_mod_num_col1 - 1];
                                }
                            }

                            for (int j = 0; j < 96; ++j)
                            {
                                for (int i = offset_chroma_mod_col2; i < 32; ++i)
                                {
                                    refWinU3_B_ref[j][i] = refWinU3_B_ref[j][chroma_mod_num_col1 - 1];
                                    refWinV3_B_ref[j][i] = refWinV3_B_ref[j][chroma_mod_num_col1 - 1];
                                }
                            }
                        }
                    }

                    if ((tmp_pix_y + 192) == luma_height_padded)
                    {
                        int offset_luma_mod_row2 = luma_mod_num_row1 ? luma_mod_num_row1 : 64;
                        int offset_chroma_mod_row2 = chroma_mod_num_row1 ? chroma_mod_num_row1 : 32;

                        for (int j = 128 + offset_luma_mod_row2; j < 192; ++j)
                        {
                            for (int i = 0; i < 32; ++i)
                            {
                                refWinY3_ref[j][i] = refWinY3_ref[64 + offset_luma_mod_row2 - 1][i];
                                refWinY3_ref[j][i + 32] = refWinY3_ref[64 + offset_luma_mod_row2 - 1][i + 32];
                            }
                        }

                        for (int j = 64 + offset_chroma_mod_row2; j < 96; ++j)
                        {
                            for (int i = 0; i < 16; ++i)
                            {
                                refWinU3_ref[j][i] = refWinU3_ref[32 + offset_chroma_mod_row2 - 1][i];
                                refWinU3_ref[j][i + 16] = refWinU3_ref[32 + offset_chroma_mod_row2 - 1][i + 16];

                                refWinV3_ref[j][i] = refWinV3_ref[32 + offset_chroma_mod_row2 - 1][i];
                                refWinV3_ref[j][i + 16] = refWinV3_ref[32 + offset_chroma_mod_row2 - 1][i + 16];
                            }
                        }

                        if ((code_stru_temp != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) &&
                             ctx_rpm_num_refp_0 > 1) || (code_stru_temp == RA &&
                                fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B))
                        {
                            for (int j = 128 + offset_luma_mod_row2; j < 192; ++j)
                            {
                                for (int i = 0; i < 32; ++i)
                                {
                                    refWinY3_B_ref[j][i] = refWinY3_B_ref[64 + offset_luma_mod_row2 - 1][i];
                                    refWinY3_B_ref[j][i + 32] = refWinY3_B_ref[64 + offset_luma_mod_row2 - 1][i + 32];
                                }
                            }

                            for (int j = 64 + offset_chroma_mod_row2; j < 96; ++j)
                            {
                                for (int i = 0; i < 16; ++i)
                                {
                                    refWinU3_B_ref[j][i] = refWinU3_B_ref[32 + offset_chroma_mod_row2 - 1][i];
                                    refWinU3_B_ref[j][i + 16] = refWinU3_B_ref[32 + offset_chroma_mod_row2 - 1][i + 16];

                                    refWinV3_B_ref[j][i] = refWinV3_B_ref[32 + offset_chroma_mod_row2 - 1][i];
                                    refWinV3_B_ref[j][i + 16] = refWinV3_B_ref[32 + offset_chroma_mod_row2 - 1][i + 16];
                                }
                            }
                        }
                    }
                }
            }
            else if ((tmp_pix_x + 128) == (luma_width_padded + 64))
            {
                for (int i = 0; i < 2; ++i)
                {
                    ref_fetch_lcu_in_rfc_1(tmp_pix_x, tmp_pix_y + i * 64, fetch_ctx_io, Fetch_input_video_U8_ptr->y_rfcBin,
                        Fetch_input_video_U8_ptr->y_rfcPrm, Fetch_input_video_U8_ptr->u_rfcBin, Fetch_input_video_U8_ptr->u_rfcPrm,
                        Fetch_input_video_U8_ptr->v_rfcBin, Fetch_input_video_U8_ptr->v_rfcPrm,
                        refWinY1_ref + (i + 1) * 64, refWinU1_ref + (i + 1) * 32, refWinV1_ref + (i + 1) * 32);

                    if ((code_stru_temp != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) &&
                         ctx_rpm_num_refp_0 > 1) || (code_stru_temp == RA &&
                            fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B))
                    {
                        ref_fetch_lcu_in_rfc_1(tmp_pix_x, tmp_pix_y + i * 64, fetch_ctx_io, Fetch_input_video_U8_ptr->y_rfcBin1,
                            Fetch_input_video_U8_ptr->y_rfcPrm1, Fetch_input_video_U8_ptr->u_rfcBin1, Fetch_input_video_U8_ptr->u_rfcPrm1,
                            Fetch_input_video_U8_ptr->v_rfcBin1, Fetch_input_video_U8_ptr->v_rfcPrm1,
                            refWinY1_B_ref + (i + 1) * 64, refWinU1_B_ref + (i + 1) * 32, refWinV1_B_ref + (i + 1) * 32);
                    }
                }

                if ((tmp_pix_x + 128) == (luma_width_padded + 64))
                {
                    int offset_luma_mod_col2 = luma_mod_num_col1 ? luma_mod_num_col1 : 64;
                    int offset_chroma_mod_col2 = chroma_mod_num_col1 ? chroma_mod_num_col1 : 32;

                    for (int j = 64; j < 192; ++j)
                    {
                        for (int i = offset_luma_mod_col2; i < 64; ++i)
                        {
                            refWinY1_ref[j][i] = refWinY1_ref[j][luma_mod_num_col1 - 1];
                        }
                    }

                    for (int j = 32; j < 96; ++j)
                    {
                        for (int i = offset_chroma_mod_col2; i < 32; ++i)
                        {
                            refWinU1_ref[j][i] = refWinU1_ref[j][chroma_mod_num_col1 - 1];
                            refWinV1_ref[j][i] = refWinV1_ref[j][chroma_mod_num_col1 - 1];
                        }
                    }

                    if ((code_stru_temp != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) &&
                         ctx_rpm_num_refp_0 > 1) || (code_stru_temp == RA &&
                            fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B))
                    {
                        for (int j = 64; j < 192; ++j)
                        {
                            for (int i = offset_luma_mod_col2; i < 64; ++i)
                            {
                                refWinY1_B_ref[j][i] = refWinY1_B_ref[j][luma_mod_num_col1 - 1];
                            }
                        }

                        for (int j = 32; j < 96; ++j)
                        {
                            for (int i = offset_chroma_mod_col2; i < 32; ++i)
                            {
                                refWinU1_B_ref[j][i] = refWinU1_B_ref[j][chroma_mod_num_col1 - 1];
                                refWinV1_B_ref[j][i] = refWinV1_B_ref[j][chroma_mod_num_col1 - 1];
                            }
                        }
                    }
                }
                if ((tmp_pix_y + 128) == luma_height_padded)
                {
                    int offset_luma_mod_row2 = luma_mod_num_row1 ? luma_mod_num_row1 : 64;
                    int offset_chroma_mod_row2 = chroma_mod_num_row1 ? chroma_mod_num_row1 : 32;

                    for (int j = 128 + offset_luma_mod_row2; j < 192; ++j)
                    {
                        for (int i = 0; i < 32; ++i)
                        {
                            refWinY1_ref[j][i] = refWinY1_ref[128 + offset_luma_mod_row2 - 1][i];
                            refWinY1_ref[j][i + 32] = refWinY1_ref[128 + offset_luma_mod_row2 - 1][i + 32];
                        }
                    }

                    for (int j = 64 + offset_chroma_mod_row2; j < 96; ++j)
                    {
                        for (int i = 0; i < 16; ++i)
                        {
                            refWinU1_ref[j][i] = refWinU1_ref[64 + offset_chroma_mod_row2 - 1][i];
                            refWinU1_ref[j][i + 16] = refWinU1_ref[64 + offset_chroma_mod_row2 - 1][i + 16];

                            refWinV1_ref[j][i] = refWinV1_ref[64 + offset_chroma_mod_row2 - 1][i];
                            refWinV1_ref[j][i + 16] = refWinV1_ref[64 + offset_chroma_mod_row2 - 1][i + 16];
                        }
                    }

                    if ((code_stru_temp != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) &&
                         ctx_rpm_num_refp_0 > 1) || (code_stru_temp == RA &&
                            fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B))
                    {
                        for (int j = 128 + offset_luma_mod_row2; j < 192; ++j)
                        {
                            for (int i = 0; i < 32; ++i)
                            {
                                refWinY1_B_ref[j][i] = refWinY1_B_ref[128 + offset_luma_mod_row2 - 1][i];
                                refWinY1_B_ref[j][i + 32] = refWinY1_B_ref[128 + offset_luma_mod_row2 - 1][i + 32];
                            }
                        }

                        for (int j = 64 + offset_chroma_mod_row2; j < 96; ++j)
                        {
                            for (int i = 0; i < 16; ++i)
                            {
                                refWinU1_B_ref[j][i] = refWinU1_B_ref[64 + offset_chroma_mod_row2 - 1][i];
                                refWinU1_B_ref[j][i + 16] = refWinU1_B_ref[64 + offset_chroma_mod_row2 - 1][i + 16];

                                refWinV1_B_ref[j][i] = refWinV1_B_ref[64 + offset_chroma_mod_row2 - 1][i];
                                refWinV1_B_ref[j][i + 16] = refWinV1_B_ref[64 + offset_chroma_mod_row2 - 1][i + 16];
                            }
                        }
                    }
                }

                int col_ctu_nums = (luma_width_padded / 64 - 2) * 64;
                if ((tmp_pix_y + 128) == luma_height_padded && col_ctu_nums >= 0)
                {
                    for (int i = 0; i < 2; ++i)
                    {
                        ref_fetch_lcu_in_rfc_3(tmp_pix_x - col_ctu_nums, tmp_pix_y + i * 64, fetch_ctx_io, Fetch_input_video_U8_ptr->y_rfcBin,
                            Fetch_input_video_U8_ptr->y_rfcPrm, Fetch_input_video_U8_ptr->u_rfcBin, Fetch_input_video_U8_ptr->u_rfcPrm,
                            Fetch_input_video_U8_ptr->v_rfcBin, Fetch_input_video_U8_ptr->v_rfcPrm,
                            refWinY3_ref + i * 64, refWinU3_ref + i * 32, refWinV3_ref + i * 32);

                        if ((code_stru_temp != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) &&
                             ctx_rpm_num_refp_0 > 1) || (code_stru_temp == RA &&
                                fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B))
                        {
                            ref_fetch_lcu_in_rfc_3(tmp_pix_x - col_ctu_nums, tmp_pix_y + i * 64, fetch_ctx_io, Fetch_input_video_U8_ptr->y_rfcBin1,
                                Fetch_input_video_U8_ptr->y_rfcPrm1, Fetch_input_video_U8_ptr->u_rfcBin1, Fetch_input_video_U8_ptr->u_rfcPrm1,
                                Fetch_input_video_U8_ptr->v_rfcBin1, Fetch_input_video_U8_ptr->v_rfcPrm1,
                                refWinY3_B_ref + i * 64, refWinU3_B_ref + i * 32, refWinV3_B_ref + i * 32);
                        }
                    }

                    if ((tmp_pix_x - col_ctu_nums + 64) == luma_width_padded)
                    {
                        int offset_luma_mod_col2 = luma_mod_num_col1 ? luma_mod_num_col1 : 64;
                        int offset_chroma_mod_col2 = chroma_mod_num_col1 ? chroma_mod_num_col1 : 32;

                        for (int j = 0; j < 128; ++j)
                        {
                            for (int i = offset_luma_mod_col2; i < 64; ++i)
                            {
                                refWinY3_ref[j][i] = refWinY3_ref[j][luma_mod_num_col1 - 1];
                            }
                        }

                        for (int j = 0; j < 64; ++j)
                        {
                            for (int i = offset_chroma_mod_col2; i < 32; ++i)
                            {
                                refWinU3_ref[j][i] = refWinU3_ref[j][chroma_mod_num_col1 - 1];
                                refWinV3_ref[j][i] = refWinV3_ref[j][chroma_mod_num_col1 - 1];
                            }
                        }

                        if ((code_stru_temp != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) &&
                             ctx_rpm_num_refp_0 > 1) || (code_stru_temp == RA &&
                                fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B))
                        {
                            for (int j = 0; j < 128; ++j)
                            {
                                for (int i = offset_luma_mod_col2; i < 64; ++i)
                                {
                                    refWinY3_B_ref[j][i] = refWinY3_B_ref[j][luma_mod_num_col1 - 1];
                                }
                            }

                            for (int j = 0; j < 64; ++j)
                            {
                                for (int i = offset_chroma_mod_col2; i < 32; ++i)
                                {
                                    refWinU3_B_ref[j][i] = refWinU3_B_ref[j][chroma_mod_num_col1 - 1];
                                    refWinV3_B_ref[j][i] = refWinV3_B_ref[j][chroma_mod_num_col1 - 1];
                                }
                            }
                        }
                    }

                    int offset_luma_mod_row2 = luma_mod_num_row1 ? luma_mod_num_row1 : 64;
                    int offset_chroma_mod_row2 = chroma_mod_num_row1 ? chroma_mod_num_row1 : 32;

                    for (int j = 64 + offset_luma_mod_row2; j < 128; ++j)
                    {
                        for (int i = 0; i < 32; ++i)
                        {
                            refWinY3_ref[j][i] = refWinY3_ref[64 + offset_luma_mod_row2 - 1][i];
                            refWinY3_ref[j][i + 32] = refWinY3_ref[64 + offset_luma_mod_row2 - 1][i + 32];
                        }
                    }

                    for (int j = 32 + offset_chroma_mod_row2; j < 64; ++j)
                    {
                        for (int i = 0; i < 16; ++i)
                        {
                            refWinU3_ref[j][i] = refWinU3_ref[32 + offset_chroma_mod_row2 - 1][i];
                            refWinU3_ref[j][i + 16] = refWinU3_ref[32 + offset_chroma_mod_row2 - 1][i + 16];

                            refWinV3_ref[j][i] = refWinV3_ref[32 + offset_chroma_mod_row2 - 1][i];
                            refWinV3_ref[j][i + 16] = refWinV3_ref[32 + offset_chroma_mod_row2 - 1][i + 16];
                        }
                    }

                    if ((code_stru_temp != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) &&
                         ctx_rpm_num_refp_0 > 1) || (code_stru_temp == RA &&
                            fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B))
                    {
                        for (int j = 64 + offset_luma_mod_row2; j < 128; ++j)
                        {
                            for (int i = 0; i < 32; ++i)
                            {
                                refWinY3_B_ref[j][i] = refWinY3_B_ref[64 + offset_luma_mod_row2 - 1][i];
                                refWinY3_B_ref[j][i + 32] = refWinY3_B_ref[64 + offset_luma_mod_row2 - 1][i + 32];
                            }
                        }

                        for (int j = 32 + offset_chroma_mod_row2; j < 64; ++j)
                        {
                            for (int i = 0; i < 16; ++i)
                            {
                                refWinU3_B_ref[j][i] = refWinU3_B_ref[32 + offset_chroma_mod_row2 - 1][i];
                                refWinU3_B_ref[j][i + 16] = refWinU3_B_ref[32 + offset_chroma_mod_row2 - 1][i + 16];

                                refWinV3_B_ref[j][i] = refWinV3_B_ref[32 + offset_chroma_mod_row2 - 1][i];
                                refWinV3_B_ref[j][i + 16] = refWinV3_B_ref[32 + offset_chroma_mod_row2 - 1][i + 16];
                            }
                        }
                    }
                }
                else if ((tmp_pix_y + 192) <= luma_height_padded && col_ctu_nums >= 0)
                {
                    for (int i = 0; i < 3; ++i)
                    {
                        ref_fetch_lcu_in_rfc_3(tmp_pix_x - col_ctu_nums, tmp_pix_y + i * 64, fetch_ctx_io, Fetch_input_video_U8_ptr->y_rfcBin,
                            Fetch_input_video_U8_ptr->y_rfcPrm, Fetch_input_video_U8_ptr->u_rfcBin, Fetch_input_video_U8_ptr->u_rfcPrm,
                            Fetch_input_video_U8_ptr->v_rfcBin, Fetch_input_video_U8_ptr->v_rfcPrm,
                            refWinY3_ref + i * 64, refWinU3_ref + i * 32, refWinV3_ref + i * 32);

                        if ((code_stru_temp != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) &&
                             ctx_rpm_num_refp_0 > 1) || (code_stru_temp == RA &&
                                fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B))
                        {
                            ref_fetch_lcu_in_rfc_3(tmp_pix_x - col_ctu_nums, tmp_pix_y + i * 64, fetch_ctx_io, Fetch_input_video_U8_ptr->y_rfcBin1,
                                Fetch_input_video_U8_ptr->y_rfcPrm1, Fetch_input_video_U8_ptr->u_rfcBin1, Fetch_input_video_U8_ptr->u_rfcPrm1,
                                Fetch_input_video_U8_ptr->v_rfcBin1, Fetch_input_video_U8_ptr->v_rfcPrm1,
                                refWinY3_B_ref + i * 64, refWinU3_B_ref + i * 32, refWinV3_B_ref + i * 32);
                        }
                    }

                    if ((tmp_pix_x - col_ctu_nums + 64) == luma_width_padded)
                    {
                        int offset_luma_mod_col2 = luma_mod_num_col1 ? luma_mod_num_col1 : 64;
                        int offset_chroma_mod_col2 = chroma_mod_num_col1 ? chroma_mod_num_col1 : 32;

                        for (int j = 0; j < 192; ++j)
                        {
                            for (int i = offset_luma_mod_col2; i < 64; ++i)
                            {
                                refWinY3_ref[j][i] = refWinY3_ref[j][luma_mod_num_col1 - 1];
                            }
                        }

                        for (int j = 0; j < 96; ++j)
                        {
                            for (int i = offset_chroma_mod_col2; i < 32; ++i)
                            {
                                refWinU3_ref[j][i] = refWinU3_ref[j][chroma_mod_num_col1 - 1];
                                refWinV3_ref[j][i] = refWinV3_ref[j][chroma_mod_num_col1 - 1];
                            }
                        }

                        if ((code_stru_temp != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) &&
                             ctx_rpm_num_refp_0 > 1) || (code_stru_temp == RA &&
                                fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B))
                        {
                            for (int j = 0; j < 192; ++j)
                            {
                                for (int i = offset_luma_mod_col2; i < 64; ++i)
                                {
                                    refWinY3_B_ref[j][i] = refWinY3_B_ref[j][luma_mod_num_col1 - 1];
                                }
                            }

                            for (int j = 0; j < 96; ++j)
                            {
                                for (int i = offset_chroma_mod_col2; i < 32; ++i)
                                {
                                    refWinU3_B_ref[j][i] = refWinU3_B_ref[j][chroma_mod_num_col1 - 1];
                                    refWinV3_B_ref[j][i] = refWinV3_B_ref[j][chroma_mod_num_col1 - 1];
                                }
                            }
                        }
                    }
                    if ((tmp_pix_y + 192) == luma_height_padded)
                    {
                        int offset_luma_mod_row2 = luma_mod_num_row1 ? luma_mod_num_row1 : 64;
                        int offset_chroma_mod_row2 = chroma_mod_num_row1 ? chroma_mod_num_row1 : 32;

                        for (int j = 128 + offset_luma_mod_row2; j < 192; ++j)
                        {
                            for (int i = 0; i < 32; ++i)
                            {
                                refWinY3_ref[j][i] = refWinY3_ref[64 + offset_luma_mod_row2 - 1][i];
                                refWinY3_ref[j][i + 32] = refWinY3_ref[64 + offset_luma_mod_row2 - 1][i + 32];
                            }
                        }

                        for (int j = 64 + offset_chroma_mod_row2; j < 96; ++j)
                        {
                            for (int i = 0; i < 16; ++i)
                            {
                                refWinU3_ref[j][i] = refWinU3_ref[32 + offset_chroma_mod_row2 - 1][i];
                                refWinU3_ref[j][i + 16] = refWinU3_ref[32 + offset_chroma_mod_row2 - 1][i + 16];

                                refWinV3_ref[j][i] = refWinV3_ref[32 + offset_chroma_mod_row2 - 1][i];
                                refWinV3_ref[j][i + 16] = refWinV3_ref[32 + offset_chroma_mod_row2 - 1][i + 16];
                            }
                        }

                        if ((code_stru_temp != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) &&
                             ctx_rpm_num_refp_0 > 1) || (code_stru_temp == RA &&
                                fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B))
                        {
                            for (int j = 128 + offset_luma_mod_row2; j < 192; ++j)
                            {
                                for (int i = 0; i < 32; ++i)
                                {
                                    refWinY3_B_ref[j][i] = refWinY3_B_ref[64 + offset_luma_mod_row2 - 1][i];
                                    refWinY3_B_ref[j][i + 32] = refWinY3_B_ref[64 + offset_luma_mod_row2 - 1][i + 32];
                                }
                            }

                            for (int j = 64 + offset_chroma_mod_row2; j < 96; ++j)
                            {
                                for (int i = 0; i < 16; ++i)
                                {
                                    refWinU3_B_ref[j][i] = refWinU3_B_ref[32 + offset_chroma_mod_row2 - 1][i];
                                    refWinU3_B_ref[j][i + 16] = refWinU3_B_ref[32 + offset_chroma_mod_row2 - 1][i + 16];

                                    refWinV3_B_ref[j][i] = refWinV3_B_ref[32 + offset_chroma_mod_row2 - 1][i];
                                    refWinV3_B_ref[j][i + 16] = refWinV3_B_ref[32 + offset_chroma_mod_row2 - 1][i + 16];
                                }
                            }
                        }
                    }
                }
            }
        }
        else if (tmp_pix_x < luma_width_padded && tmp_pix_x >= 64)
        {
            if ((tmp_pix_x + 128) <= (luma_width_padded - 64))
            {
                for (int i = 0; i < 2; ++i)
                {
                    ref_fetch_lcu_in_rfc_3(tmp_pix_x + 128, tmp_pix_y + i * 64, fetch_ctx_io, Fetch_input_video_U8_ptr->y_rfcBin,
                        Fetch_input_video_U8_ptr->y_rfcPrm, Fetch_input_video_U8_ptr->u_rfcBin, Fetch_input_video_U8_ptr->u_rfcPrm,
                        Fetch_input_video_U8_ptr->v_rfcBin, Fetch_input_video_U8_ptr->v_rfcPrm,
                        refWinY3_ref + (i + 1) * 64, refWinU3_ref + (i + 1) * 32, refWinV3_ref + (i + 1) * 32);

                    if ((code_stru_temp != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) &&
                         ctx_rpm_num_refp_0 > 1) || (code_stru_temp == RA &&
                            fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B))
                    {
                        ref_fetch_lcu_in_rfc_3(tmp_pix_x + 128, tmp_pix_y + i * 64, fetch_ctx_io, Fetch_input_video_U8_ptr->y_rfcBin1,
                            Fetch_input_video_U8_ptr->y_rfcPrm1, Fetch_input_video_U8_ptr->u_rfcBin1, Fetch_input_video_U8_ptr->u_rfcPrm1,
                            Fetch_input_video_U8_ptr->v_rfcBin1, Fetch_input_video_U8_ptr->v_rfcPrm1,
                            refWinY3_B_ref + (i + 1) * 64, refWinU3_B_ref + (i + 1) * 32, refWinV3_B_ref + (i + 1) * 32);
                    }
                }

                if ((tmp_pix_x + 128) == (luma_width_padded - 64))
                {
                    int offset_luma_mod_col2 = luma_mod_num_col1 ? luma_mod_num_col1 : 64;
                    int offset_chroma_mod_col2 = chroma_mod_num_col1 ? chroma_mod_num_col1 : 32;

                    for (int j = 64; j < 192; ++j)
                    {
                        for (int i = offset_luma_mod_col2; i < 64; ++i)
                        {
                            refWinY3_ref[j][i] = refWinY3_ref[j][luma_mod_num_col1 - 1];
                        }
                    }

                    for (int j = 32; j < 96; ++j)
                    {
                        for (int i = offset_chroma_mod_col2; i < 32; ++i)
                        {
                            refWinU3_ref[j][i] = refWinU3_ref[j][chroma_mod_num_col1 - 1];
                            refWinV3_ref[j][i] = refWinV3_ref[j][chroma_mod_num_col1 - 1];
                        }
                    }

                    if ((code_stru_temp != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) &&
                         ctx_rpm_num_refp_0 > 1) || (code_stru_temp == RA &&
                            fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B))
                    {
                        for (int j = 64; j < 192; ++j)
                        {
                            for (int i = offset_luma_mod_col2; i < 64; ++i)
                            {
                                refWinY3_B_ref[j][i] = refWinY3_B_ref[j][luma_mod_num_col1 - 1];
                            }
                        }

                        for (int j = 32; j < 96; ++j)
                        {
                            for (int i = offset_chroma_mod_col2; i < 32; ++i)
                            {
                                refWinU3_B_ref[j][i] = refWinU3_B_ref[j][chroma_mod_num_col1 - 1];
                                refWinV3_B_ref[j][i] = refWinV3_B_ref[j][chroma_mod_num_col1 - 1];
                            }
                        }
                    }
                }
                if ((tmp_pix_y + 128) == luma_height_padded)
                {
                    int offset_luma_mod_row2 = luma_mod_num_row1 ? luma_mod_num_row1 : 64;
                    int offset_chroma_mod_row2 = chroma_mod_num_row1 ? chroma_mod_num_row1 : 32;

                    for (int j = 128 + offset_luma_mod_row2; j < 192; ++j)
                    {
                        for (int i = 0; i < 32; ++i)
                        {
                            refWinY3_ref[j][i] = refWinY3_ref[128 + offset_luma_mod_row2 - 1][i];
                            refWinY3_ref[j][i + 32] = refWinY3_ref[128 + offset_luma_mod_row2 - 1][i + 32];
                        }
                    }

                    for (int j = 64 + offset_chroma_mod_row2; j < 96; ++j)
                    {
                        for (int i = 0; i < 16; ++i)
                        {
                            refWinU3_ref[j][i] = refWinU3_ref[64 + offset_chroma_mod_row2 - 1][i];
                            refWinU3_ref[j][i + 16] = refWinU3_ref[64 + offset_chroma_mod_row2 - 1][i + 16];

                            refWinV3_ref[j][i] = refWinV3_ref[64 + offset_chroma_mod_row2 - 1][i];
                            refWinV3_ref[j][i + 16] = refWinV3_ref[64 + offset_chroma_mod_row2 - 1][i + 16];
                        }
                    }

                    if ((code_stru_temp != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) &&
                         ctx_rpm_num_refp_0 > 1) || (code_stru_temp == RA &&
                            fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B))
                    {
                        for (int j = 128 + offset_luma_mod_row2; j < 192; ++j)
                        {
                            for (int i = 0; i < 32; ++i)
                            {
                                refWinY3_B_ref[j][i] = refWinY3_B_ref[128 + offset_luma_mod_row2 - 1][i];
                                refWinY3_B_ref[j][i + 32] = refWinY3_B_ref[128 + offset_luma_mod_row2 - 1][i + 32];
                            }
                        }

                        for (int j = 64 + offset_chroma_mod_row2; j < 96; ++j)
                        {
                            for (int i = 0; i < 16; ++i)
                            {
                                refWinU3_B_ref[j][i] = refWinU3_B_ref[64 + offset_chroma_mod_row2 - 1][i];
                                refWinU3_B_ref[j][i + 16] = refWinU3_B_ref[64 + offset_chroma_mod_row2 - 1][i + 16];

                                refWinV3_B_ref[j][i] = refWinV3_B_ref[64 + offset_chroma_mod_row2 - 1][i];
                                refWinV3_B_ref[j][i + 16] = refWinV3_B_ref[64 + offset_chroma_mod_row2 - 1][i + 16];
                            }
                        }
                    }
                }
            }
            else if ((tmp_pix_x + 128) == luma_width_padded)
            {
                int col_ctu_nums = (luma_width_padded / 64 - 2) * 64;
                if ((tmp_pix_y + 128) == luma_height_padded && col_ctu_nums >= 0)
                {
                    for (int i = 0; i < 2; ++i)
                    {
                        ref_fetch_lcu_in_rfc_3(tmp_pix_x - col_ctu_nums, tmp_pix_y + i * 64, fetch_ctx_io, Fetch_input_video_U8_ptr->y_rfcBin,
                            Fetch_input_video_U8_ptr->y_rfcPrm, Fetch_input_video_U8_ptr->u_rfcBin, Fetch_input_video_U8_ptr->u_rfcPrm,
                            Fetch_input_video_U8_ptr->v_rfcBin, Fetch_input_video_U8_ptr->v_rfcPrm,
                            refWinY3_ref + i * 64, refWinU3_ref + i * 32, refWinV3_ref + i * 32);

                        if ((code_stru_temp != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) &&
                             ctx_rpm_num_refp_0 > 1) || (code_stru_temp == RA &&
                                fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B))
                        {
                            ref_fetch_lcu_in_rfc_3(tmp_pix_x - col_ctu_nums, tmp_pix_y + i * 64, fetch_ctx_io, Fetch_input_video_U8_ptr->y_rfcBin1,
                                Fetch_input_video_U8_ptr->y_rfcPrm1, Fetch_input_video_U8_ptr->u_rfcBin1, Fetch_input_video_U8_ptr->u_rfcPrm1,
                                Fetch_input_video_U8_ptr->v_rfcBin1, Fetch_input_video_U8_ptr->v_rfcPrm1,
                                refWinY3_B_ref + i * 64, refWinU3_B_ref + i * 32, refWinV3_B_ref + i * 32);
                        }
                    }

                    if ((tmp_pix_x - col_ctu_nums + 64) == luma_width_padded)
                    {
                        int offset_luma_mod_col2 = luma_mod_num_col1 ? luma_mod_num_col1 : 64;
                        int offset_chroma_mod_col2 = chroma_mod_num_col1 ? chroma_mod_num_col1 : 32;

                        for (int j = 0; j < 128; ++j)
                        {
                            for (int i = offset_luma_mod_col2; i < 64; ++i)
                            {
                                refWinY3_ref[j][i] = refWinY3_ref[j][luma_mod_num_col1 - 1];
                            }
                        }

                        for (int j = 0; j < 64; ++j)
                        {
                            for (int i = offset_chroma_mod_col2; i < 32; ++i)
                            {
                                refWinU3_ref[j][i] = refWinU3_ref[j][chroma_mod_num_col1 - 1];
                                refWinV3_ref[j][i] = refWinV3_ref[j][chroma_mod_num_col1 - 1];
                            }
                        }

                        if ((code_stru_temp != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) &&
                             ctx_rpm_num_refp_0 > 1) || (code_stru_temp == RA &&
                                fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B))
                        {
                            for (int j = 0; j < 128; ++j)
                            {
                                for (int i = offset_luma_mod_col2; i < 64; ++i)
                                {
                                    refWinY3_B_ref[j][i] = refWinY3_B_ref[j][luma_mod_num_col1 - 1];
                                }
                            }

                            for (int j = 0; j < 64; ++j)
                            {
                                for (int i = offset_chroma_mod_col2; i < 32; ++i)
                                {
                                    refWinU3_B_ref[j][i] = refWinU3_B_ref[j][chroma_mod_num_col1 - 1];
                                    refWinV3_B_ref[j][i] = refWinV3_B_ref[j][chroma_mod_num_col1 - 1];
                                }
                            }
                        }
                    }

                    int offset_luma_mod_row2 = luma_mod_num_row1 ? luma_mod_num_row1 : 64;
                    int offset_chroma_mod_row2 = chroma_mod_num_row1 ? chroma_mod_num_row1 : 32;

                    for (int j = 64 + offset_luma_mod_row2; j < 128; ++j)
                    {
                        for (int i = 0; i < 32; ++i)
                        {
                            refWinY3_ref[j][i] = refWinY3_ref[64 + offset_luma_mod_row2 - 1][i];
                            refWinY3_ref[j][i + 32] = refWinY3_ref[64 + offset_luma_mod_row2 - 1][i + 32];
                        }
                    }

                    for (int j = 32 + offset_chroma_mod_row2; j < 64; ++j)
                    {
                        for (int i = 0; i < 16; ++i)
                        {
                            refWinU3_ref[j][i] = refWinU3_ref[32 + offset_chroma_mod_row2 - 1][i];
                            refWinU3_ref[j][i + 16] = refWinU3_ref[32 + offset_chroma_mod_row2 - 1][i + 16];

                            refWinV3_ref[j][i] = refWinV3_ref[32 + offset_chroma_mod_row2 - 1][i];
                            refWinV3_ref[j][i + 16] = refWinV3_ref[32 + offset_chroma_mod_row2 - 1][i + 16];
                        }
                    }

                    if ((code_stru_temp != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) &&
                         ctx_rpm_num_refp_0 > 1) || (code_stru_temp == RA &&
                            fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B))
                    {
                        for (int j = 64 + offset_luma_mod_row2; j < 128; ++j)
                        {
                            for (int i = 0; i < 32; ++i)
                            {
                                refWinY3_B_ref[j][i] = refWinY3_B_ref[64 + offset_luma_mod_row2 - 1][i];
                                refWinY3_B_ref[j][i + 32] = refWinY3_B_ref[64 + offset_luma_mod_row2 - 1][i + 32];
                            }
                        }

                        for (int j = 32 + offset_chroma_mod_row2; j < 64; ++j)
                        {
                            for (int i = 0; i < 16; ++i)
                            {
                                refWinU3_B_ref[j][i] = refWinU3_B_ref[32 + offset_chroma_mod_row2 - 1][i];
                                refWinU3_B_ref[j][i + 16] = refWinU3_B_ref[32 + offset_chroma_mod_row2 - 1][i + 16];

                                refWinV3_B_ref[j][i] = refWinV3_B_ref[32 + offset_chroma_mod_row2 - 1][i];
                                refWinV3_B_ref[j][i + 16] = refWinV3_B_ref[32 + offset_chroma_mod_row2 - 1][i + 16];
                            }
                        }
                    }
                }
                else if ((tmp_pix_y + 192) <= luma_height_padded && col_ctu_nums >= 0)
                {
                    for (int i = 0; i < 3; ++i)
                    {
                        ref_fetch_lcu_in_rfc_3(tmp_pix_x - col_ctu_nums, tmp_pix_y + i * 64, fetch_ctx_io, Fetch_input_video_U8_ptr->y_rfcBin,
                            Fetch_input_video_U8_ptr->y_rfcPrm, Fetch_input_video_U8_ptr->u_rfcBin, Fetch_input_video_U8_ptr->u_rfcPrm,
                            Fetch_input_video_U8_ptr->v_rfcBin, Fetch_input_video_U8_ptr->v_rfcPrm,
                            refWinY3_ref + i * 64, refWinU3_ref + i * 32, refWinV3_ref + i * 32);

                        if ((code_stru_temp != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) &&
                             ctx_rpm_num_refp_0 > 1) || (code_stru_temp == RA &&
                                fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B))
                        {
                            ref_fetch_lcu_in_rfc_3(tmp_pix_x - col_ctu_nums, tmp_pix_y + i * 64, fetch_ctx_io, Fetch_input_video_U8_ptr->y_rfcBin1,
                                Fetch_input_video_U8_ptr->y_rfcPrm1, Fetch_input_video_U8_ptr->u_rfcBin1, Fetch_input_video_U8_ptr->u_rfcPrm1,
                                Fetch_input_video_U8_ptr->v_rfcBin1, Fetch_input_video_U8_ptr->v_rfcPrm1,
                                refWinY3_B_ref + i * 64, refWinU3_B_ref + i * 32, refWinV3_B_ref + i * 32);
                        }
                    }

                    if ((tmp_pix_x - col_ctu_nums + 64) == luma_width_padded)
                    {
                        int offset_luma_mod_col2 = luma_mod_num_col1 ? luma_mod_num_col1 : 64;
                        int offset_chroma_mod_col2 = chroma_mod_num_col1 ? chroma_mod_num_col1 : 32;

                        for (int j = 0; j < 192; ++j)
                        {
                            for (int i = offset_luma_mod_col2; i < 64; ++i)
                            {
                                refWinY3_ref[j][i] = refWinY3_ref[j][luma_mod_num_col1 - 1];
                            }
                        }

                        for (int j = 0; j < 96; ++j)
                        {
                            for (int i = offset_chroma_mod_col2; i < 32; ++i)
                            {
                                refWinU3_ref[j][i] = refWinU3_ref[j][chroma_mod_num_col1 - 1];
                                refWinV3_ref[j][i] = refWinV3_ref[j][chroma_mod_num_col1 - 1];
                            }
                        }

                        if ((code_stru_temp != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) &&
                             ctx_rpm_num_refp_0 > 1) || (code_stru_temp == RA &&
                                fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B))
                        {
                            for (int j = 0; j < 192; ++j)
                            {
                                for (int i = offset_luma_mod_col2; i < 64; ++i)
                                {
                                    refWinY3_B_ref[j][i] = refWinY3_B_ref[j][luma_mod_num_col1 - 1];
                                }
                            }

                            for (int j = 0; j < 96; ++j)
                            {
                                for (int i = offset_chroma_mod_col2; i < 32; ++i)
                                {
                                    refWinU3_B_ref[j][i] = refWinU3_B_ref[j][chroma_mod_num_col1 - 1];
                                    refWinV3_B_ref[j][i] = refWinV3_B_ref[j][chroma_mod_num_col1 - 1];
                                }
                            }
                        }
                    }

                    if ((tmp_pix_y + 192) == luma_height_padded)
                    {
                        int offset_luma_mod_row2 = luma_mod_num_row1 ? luma_mod_num_row1 : 64;
                        int offset_chroma_mod_row2 = chroma_mod_num_row1 ? chroma_mod_num_row1 : 32;

                        for (int j = 128 + offset_luma_mod_row2; j < 192; ++j)
                        {
                            for (int i = 0; i < 32; ++i)
                            {
                                refWinY3_ref[j][i] = refWinY3_ref[128 + offset_luma_mod_row2 - 1][i];
                                refWinY3_ref[j][i + 32] = refWinY3_ref[128 + offset_luma_mod_row2 - 1][i + 32];
                            }
                        }

                        for (int j = 64 + offset_chroma_mod_row2; j < 96; ++j)
                        {
                            for (int i = 0; i < 16; ++i)
                            {
                                refWinU3_ref[j][i] = refWinU3_ref[64 + offset_chroma_mod_row2 - 1][i];
                                refWinU3_ref[j][i + 16] = refWinU3_ref[64 + offset_chroma_mod_row2 - 1][i + 16];

                                refWinV3_ref[j][i] = refWinV3_ref[64 + offset_chroma_mod_row2 - 1][i];
                                refWinV3_ref[j][i + 16] = refWinV3_ref[64 + offset_chroma_mod_row2 - 1][i + 16];
                            }
                        }

                        if ((code_stru_temp != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) &&
                             ctx_rpm_num_refp_0 > 1) || (code_stru_temp == RA &&
                                fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B))
                        {
                            for (int j = 128 + offset_luma_mod_row2; j < 192; ++j)
                            {
                                for (int i = 0; i < 32; ++i)
                                {
                                    refWinY3_B_ref[j][i] = refWinY3_B_ref[128 + offset_luma_mod_row2 - 1][i];
                                    refWinY3_B_ref[j][i + 32] = refWinY3_B_ref[128 + offset_luma_mod_row2 - 1][i + 32];
                                }
                            }

                            for (int j = 64 + offset_chroma_mod_row2; j < 96; ++j)
                            {
                                for (int i = 0; i < 16; ++i)
                                {
                                    refWinU3_B_ref[j][i] = refWinU3_B_ref[64 + offset_chroma_mod_row2 - 1][i];
                                    refWinU3_B_ref[j][i + 16] = refWinU3_B_ref[64 + offset_chroma_mod_row2 - 1][i + 16];

                                    refWinV3_B_ref[j][i] = refWinV3_B_ref[64 + offset_chroma_mod_row2 - 1][i];
                                    refWinV3_B_ref[j][i + 16] = refWinV3_B_ref[64 + offset_chroma_mod_row2 - 1][i + 16];
                                }
                            }
                        }
                    }
                }
            }
            else if ((tmp_pix_x + 128) == (luma_width_padded + 64))
            {
                int col_ctu_nums = (luma_width_padded / 64 - 2) * 64;
                if ((tmp_pix_y + 128) == luma_height_padded && col_ctu_nums >= 0)
                {
                    for (int i = 0; i < 2; ++i)
                    {
                        ref_fetch_lcu_in_rfc_3(tmp_pix_x - col_ctu_nums, tmp_pix_y + i * 64, fetch_ctx_io, Fetch_input_video_U8_ptr->y_rfcBin,
                            Fetch_input_video_U8_ptr->y_rfcPrm, Fetch_input_video_U8_ptr->u_rfcBin, Fetch_input_video_U8_ptr->u_rfcPrm,
                            Fetch_input_video_U8_ptr->v_rfcBin, Fetch_input_video_U8_ptr->v_rfcPrm,
                            refWinY3_ref + i * 64, refWinU3_ref + i * 32, refWinV3_ref + i * 32);

                        if ((code_stru_temp != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) &&
                             ctx_rpm_num_refp_0 > 1) || (code_stru_temp == RA &&
                                fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B))
                        {
                            ref_fetch_lcu_in_rfc_3(tmp_pix_x - col_ctu_nums, tmp_pix_y + i * 64, fetch_ctx_io, Fetch_input_video_U8_ptr->y_rfcBin1,
                                Fetch_input_video_U8_ptr->y_rfcPrm1, Fetch_input_video_U8_ptr->u_rfcBin1, Fetch_input_video_U8_ptr->u_rfcPrm1,
                                Fetch_input_video_U8_ptr->v_rfcBin1, Fetch_input_video_U8_ptr->v_rfcPrm1,
                                refWinY3_B_ref + i * 64, refWinU3_B_ref + i * 32, refWinV3_B_ref + i * 32);
                        }
                    }

                    if ((tmp_pix_x - col_ctu_nums + 64) == luma_width_padded)
                    {
                        int offset_luma_mod_col2 = luma_mod_num_col1 ? luma_mod_num_col1 : 64;
                        int offset_chroma_mod_col2 = chroma_mod_num_col1 ? chroma_mod_num_col1 : 32;

                        for (int j = 0; j < 128; ++j)
                        {
                            for (int i = offset_luma_mod_col2; i < 64; ++i)
                            {
                                refWinY3_ref[j][i] = refWinY3_ref[j][luma_mod_num_col1 - 1];
                            }
                        }

                        for (int j = 0; j < 64; ++j)
                        {
                            for (int i = offset_chroma_mod_col2; i < 32; ++i)
                            {
                                refWinU3_ref[j][i] = refWinU3_ref[j][chroma_mod_num_col1 - 1];
                                refWinV3_ref[j][i] = refWinV3_ref[j][chroma_mod_num_col1 - 1];
                            }
                        }

                        if ((code_stru_temp != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) &&
                             ctx_rpm_num_refp_0 > 1) || (code_stru_temp == RA &&
                                fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B))
                        {
                            for (int j = 0; j < 128; ++j)
                            {
                                for (int i = offset_luma_mod_col2; i < 64; ++i)
                                {
                                    refWinY3_B_ref[j][i] = refWinY3_B_ref[j][luma_mod_num_col1 - 1];
                                }
                            }

                            for (int j = 0; j < 64; ++j)
                            {
                                for (int i = offset_chroma_mod_col2; i < 32; ++i)
                                {
                                    refWinU3_B_ref[j][i] = refWinU3_B_ref[j][chroma_mod_num_col1 - 1];
                                    refWinV3_B_ref[j][i] = refWinV3_B_ref[j][chroma_mod_num_col1 - 1];
                                }
                            }
                        }
                    }

                    int offset_luma_mod_row2 = luma_mod_num_row1 ? luma_mod_num_row1 : 64;
                    int offset_chroma_mod_row2 = chroma_mod_num_row1 ? chroma_mod_num_row1 : 32;

                    for (int j = 64 + offset_luma_mod_row2; j < 128; ++j)
                    {
                        for (int i = 0; i < 32; ++i)
                        {
                            refWinY3_ref[j][i] = refWinY3_ref[64 + offset_luma_mod_row2 - 1][i];
                            refWinY3_ref[j][i + 32] = refWinY3_ref[64 + offset_luma_mod_row2 - 1][i + 32];
                        }
                    }

                    for (int j = 32 + offset_chroma_mod_row2; j < 64; ++j)
                    {
                        for (int i = 0; i < 16; ++i)
                        {
                            refWinU3_ref[j][i] = refWinU3_ref[32 + offset_chroma_mod_row2 - 1][i];
                            refWinU3_ref[j][i + 16] = refWinU3_ref[32 + offset_chroma_mod_row2 - 1][i + 16];

                            refWinV3_ref[j][i] = refWinV3_ref[32 + offset_chroma_mod_row2 - 1][i];
                            refWinV3_ref[j][i + 16] = refWinV3_ref[32 + offset_chroma_mod_row2 - 1][i + 16];
                        }
                    }

                    if ((code_stru_temp != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) &&
                         ctx_rpm_num_refp_0 > 1) || (code_stru_temp == RA &&
                            fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B))
                    {
                        for (int j = 64 + offset_luma_mod_row2; j < 128; ++j)
                        {
                            for (int i = 0; i < 32; ++i)
                            {
                                refWinY3_B_ref[j][i] = refWinY3_B_ref[64 + offset_luma_mod_row2 - 1][i];
                                refWinY3_B_ref[j][i + 32] = refWinY3_B_ref[64 + offset_luma_mod_row2 - 1][i + 32];
                            }
                        }

                        for (int j = 32 + offset_chroma_mod_row2; j < 64; ++j)
                        {
                            for (int i = 0; i < 16; ++i)
                            {
                                refWinU3_B_ref[j][i] = refWinU3_B_ref[32 + offset_chroma_mod_row2 - 1][i];
                                refWinU3_B_ref[j][i + 16] = refWinU3_B_ref[32 + offset_chroma_mod_row2 - 1][i + 16];

                                refWinV3_B_ref[j][i] = refWinV3_B_ref[32 + offset_chroma_mod_row2 - 1][i];
                                refWinV3_B_ref[j][i + 16] = refWinV3_B_ref[32 + offset_chroma_mod_row2 - 1][i + 16];
                            }
                        }
                    }
                }
                else if ((tmp_pix_y + 192) <= luma_height_padded && col_ctu_nums >= 0)
                {
                    for (int i = 0; i < 3; ++i)
                    {
                        ref_fetch_lcu_in_rfc_3(tmp_pix_x - col_ctu_nums, tmp_pix_y + i * 64, fetch_ctx_io, Fetch_input_video_U8_ptr->y_rfcBin,
                            Fetch_input_video_U8_ptr->y_rfcPrm, Fetch_input_video_U8_ptr->u_rfcBin, Fetch_input_video_U8_ptr->u_rfcPrm,
                            Fetch_input_video_U8_ptr->v_rfcBin, Fetch_input_video_U8_ptr->v_rfcPrm,
                            refWinY3_ref + i * 64, refWinU3_ref + i * 32, refWinV3_ref + i * 32);

                        if ((code_stru_temp != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) &&
                             ctx_rpm_num_refp_0 > 1) || (code_stru_temp == RA &&
                                fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B))
                        {
                            ref_fetch_lcu_in_rfc_3(tmp_pix_x - col_ctu_nums, tmp_pix_y + i * 64, fetch_ctx_io, Fetch_input_video_U8_ptr->y_rfcBin1,
                                Fetch_input_video_U8_ptr->y_rfcPrm1, Fetch_input_video_U8_ptr->u_rfcBin1, Fetch_input_video_U8_ptr->u_rfcPrm1,
                                Fetch_input_video_U8_ptr->v_rfcBin1, Fetch_input_video_U8_ptr->v_rfcPrm1,
                                refWinY3_B_ref + i * 64, refWinU3_B_ref + i * 32, refWinV3_B_ref + i * 32);
                        }
                    }

                    if ((tmp_pix_x - col_ctu_nums + 64) == luma_width_padded)
                    {
                        int offset_luma_mod_col2 = luma_mod_num_col1 ? luma_mod_num_col1 : 64;
                        int offset_chroma_mod_col2 = chroma_mod_num_col1 ? chroma_mod_num_col1 : 32;

                        for (int j = 0; j < 192; ++j)
                        {
                            for (int i = offset_luma_mod_col2; i < 64; ++i)
                            {
                                refWinY3_ref[j][i] = refWinY3_ref[j][luma_mod_num_col1 - 1];
                            }
                        }

                        for (int j = 0; j < 96; ++j)
                        {
                            for (int i = offset_chroma_mod_col2; i < 32; ++i)
                            {
                                refWinU3_ref[j][i] = refWinU3_ref[j][chroma_mod_num_col1 - 1];
                                refWinV3_ref[j][i] = refWinV3_ref[j][chroma_mod_num_col1 - 1];
                            }
                        }

                        if ((code_stru_temp != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) &&
                             ctx_rpm_num_refp_0 > 1) || (code_stru_temp == RA &&
                                fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B))
                        {
                            for (int j = 0; j < 192; ++j)
                            {
                                for (int i = offset_luma_mod_col2; i < 64; ++i)
                                {
                                    refWinY3_B_ref[j][i] = refWinY3_B_ref[j][luma_mod_num_col1 - 1];
                                }
                            }

                            for (int j = 0; j < 96; ++j)
                            {
                                for (int i = offset_chroma_mod_col2; i < 32; ++i)
                                {
                                    refWinU3_B_ref[j][i] = refWinU3_B_ref[j][chroma_mod_num_col1 - 1];
                                    refWinV3_B_ref[j][i] = refWinV3_B_ref[j][chroma_mod_num_col1 - 1];
                                }
                            }
                        }
                    }

                    if ((tmp_pix_y + 192) == luma_height_padded)
                    {
                        int offset_luma_mod_row2 = luma_mod_num_row1 ? luma_mod_num_row1 : 64;
                        int offset_chroma_mod_row2 = chroma_mod_num_row1 ? chroma_mod_num_row1 : 32;

                        for (int j = 128 + offset_luma_mod_row2; j < 192; ++j)
                        {
                            for (int i = 0; i < 32; ++i)
                            {
                                refWinY3_ref[j][i] = refWinY3_ref[128 + offset_luma_mod_row2 - 1][i];
                                refWinY3_ref[j][i + 32] = refWinY3_ref[128 + offset_luma_mod_row2 - 1][i + 32];
                            }
                        }

                        for (int j = 64 + offset_chroma_mod_row2; j < 96; ++j)
                        {
                            for (int i = 0; i < 16; ++i)
                            {
                                refWinU3_ref[j][i] = refWinU3_ref[64 + offset_chroma_mod_row2 - 1][i];
                                refWinU3_ref[j][i + 16] = refWinU3_ref[64 + offset_chroma_mod_row2 - 1][i + 16];

                                refWinV3_ref[j][i] = refWinV3_ref[64 + offset_chroma_mod_row2 - 1][i];
                                refWinV3_ref[j][i + 16] = refWinV3_ref[64 + offset_chroma_mod_row2 - 1][i + 16];
                            }
                        }

                        if ((code_stru_temp != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) &&
                             ctx_rpm_num_refp_0 > 1) || (code_stru_temp == RA &&
                                fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B))
                        {
                            for (int j = 128 + offset_luma_mod_row2; j < 192; ++j)
                            {
                                for (int i = 0; i < 32; ++i)
                                {
                                    refWinY3_B_ref[j][i] = refWinY3_B_ref[128 + offset_luma_mod_row2 - 1][i];
                                    refWinY3_B_ref[j][i + 32] = refWinY3_B_ref[128 + offset_luma_mod_row2 - 1][i + 32];
                                }
                            }

                            for (int j = 64 + offset_chroma_mod_row2; j < 96; ++j)
                            {
                                for (int i = 0; i < 16; ++i)
                                {
                                    refWinU3_B_ref[j][i] = refWinU3_B_ref[64 + offset_chroma_mod_row2 - 1][i];
                                    refWinU3_B_ref[j][i + 16] = refWinU3_B_ref[64 + offset_chroma_mod_row2 - 1][i + 16];

                                    refWinV3_B_ref[j][i] = refWinV3_B_ref[64 + offset_chroma_mod_row2 - 1][i];
                                    refWinV3_B_ref[j][i + 16] = refWinV3_B_ref[64 + offset_chroma_mod_row2 - 1][i + 16];
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    //middle row
    else if (tmp_pix_y <= (luma_height_padded - 128))
    {
        if ((tmp_pix_x + 128) <= (luma_width_padded - 64))
        {
            for (int i = -1; i < 2; ++i)
            {
                ref_fetch_lcu_in_rfc_3(tmp_pix_x + 128, tmp_pix_y + i * 64, fetch_ctx_io, Fetch_input_video_U8_ptr->y_rfcBin,
                    Fetch_input_video_U8_ptr->y_rfcPrm, Fetch_input_video_U8_ptr->u_rfcBin, Fetch_input_video_U8_ptr->u_rfcPrm,
                    Fetch_input_video_U8_ptr->v_rfcBin, Fetch_input_video_U8_ptr->v_rfcPrm,
                    refWinY3_ref + (i + 1) * 64, refWinU3_ref + (i + 1) * 32, refWinV3_ref + (i + 1) * 32);

                if ((code_stru_temp != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) &&
                    ctx_rpm_num_refp_0 > 1) || (code_stru_temp == RA &&
                        fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B))
                {
                    ref_fetch_lcu_in_rfc_3(tmp_pix_x + 128, tmp_pix_y + i * 64, fetch_ctx_io, Fetch_input_video_U8_ptr->y_rfcBin1,
                        Fetch_input_video_U8_ptr->y_rfcPrm1, Fetch_input_video_U8_ptr->u_rfcBin1, Fetch_input_video_U8_ptr->u_rfcPrm1,
                        Fetch_input_video_U8_ptr->v_rfcBin1, Fetch_input_video_U8_ptr->v_rfcPrm1,
                        refWinY3_B_ref + (i + 1) * 64, refWinU3_B_ref + (i + 1) * 32, refWinV3_B_ref + (i + 1) * 32);
                }
            }

            if ((tmp_pix_x + 128) == (luma_width_padded - 64))
            {
                int offset_luma_mod_col2 = luma_mod_num_col1 ? luma_mod_num_col1 : 64;
                int offset_chroma_mod_col2 = chroma_mod_num_col1 ? chroma_mod_num_col1 : 32;

                for (int j = 0; j < 192; ++j)
                {
                    for (int i = offset_luma_mod_col2; i < 64; ++i)
                    {
                        refWinY3_ref[j][i] = refWinY3_ref[j][luma_mod_num_col1 - 1];
                    }
                }

                for (int j = 0; j < 96; ++j)
                {
                    for (int i = offset_chroma_mod_col2; i < 32; ++i)
                    {
                        refWinU3_ref[j][i] = refWinU3_ref[j][chroma_mod_num_col1 - 1];
                        refWinV3_ref[j][i] = refWinV3_ref[j][chroma_mod_num_col1 - 1];
                    }
                }

                if ((code_stru_temp != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) &&
                    ctx_rpm_num_refp_0 > 1) || (code_stru_temp == RA &&
                        fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B))
                {
                    for (int j = 0; j < 192; ++j)
                    {
                        for (int i = offset_luma_mod_col2; i < 64; ++i)
                        {
                            refWinY3_B_ref[j][i] = refWinY3_B_ref[j][luma_mod_num_col1 - 1];
                        }
                    }

                    for (int j = 0; j < 96; ++j)
                    {
                        for (int i = offset_chroma_mod_col2; i < 32; ++i)
                        {
                            refWinU3_B_ref[j][i] = refWinU3_B_ref[j][chroma_mod_num_col1 - 1];
                            refWinV3_B_ref[j][i] = refWinV3_B_ref[j][chroma_mod_num_col1 - 1];
                        }
                    }
                }
            }
            if ((tmp_pix_y + 128) == luma_height_padded)
            {
                int offset_luma_mod_row2 = luma_mod_num_row1 ? luma_mod_num_row1 : 64;
                int offset_chroma_mod_row2 = chroma_mod_num_row1 ? chroma_mod_num_row1 : 32;

                for (int j = 128 + offset_luma_mod_row2; j < 192; ++j)
                {
                    for (int i = 0; i < 32; ++i)
                    {
                        refWinY3_ref[j][i] = refWinY3_ref[128 + offset_luma_mod_row2 - 1][i];
                        refWinY3_ref[j][i + 32] = refWinY3_ref[128 + offset_luma_mod_row2 - 1][i + 32];
                    }
                }

                for (int j = 64 + offset_chroma_mod_row2; j < 96; ++j)
                {
                    for (int i = 0; i < 16; ++i)
                    {
                        refWinU3_ref[j][i] = refWinU3_ref[64 + offset_chroma_mod_row2 - 1][i];
                        refWinU3_ref[j][i + 16] = refWinU3_ref[64 + offset_chroma_mod_row2 - 1][i + 16];

                        refWinV3_ref[j][i] = refWinV3_ref[64 + offset_chroma_mod_row2 - 1][i];
                        refWinV3_ref[j][i + 16] = refWinV3_ref[64 + offset_chroma_mod_row2 - 1][i + 16];
                    }
                }

                if ((code_stru_temp != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) &&
                    ctx_rpm_num_refp_0 > 1) || (code_stru_temp == RA &&
                        fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B))
                {
                    for (int j = 128 + offset_luma_mod_row2; j < 192; ++j)
                    {
                        for (int i = 0; i < 32; ++i)
                        {
                            refWinY3_B_ref[j][i] = refWinY3_B_ref[128 + offset_luma_mod_row2 - 1][i];
                            refWinY3_B_ref[j][i + 32] = refWinY3_B_ref[128 + offset_luma_mod_row2 - 1][i + 32];
                        }
                    }

                    for (int j = 64 + offset_chroma_mod_row2; j < 96; ++j)
                    {
                        for (int i = 0; i < 16; ++i)
                        {
                            refWinU3_B_ref[j][i] = refWinU3_B_ref[64 + offset_chroma_mod_row2 - 1][i];
                            refWinU3_B_ref[j][i + 16] = refWinU3_B_ref[64 + offset_chroma_mod_row2 - 1][i + 16];

                            refWinV3_B_ref[j][i] = refWinV3_B_ref[64 + offset_chroma_mod_row2 - 1][i];
                            refWinV3_B_ref[j][i + 16] = refWinV3_B_ref[64 + offset_chroma_mod_row2 - 1][i + 16];
                        }
                    }
                }
            }
        }
        else if ((tmp_pix_x + 128) == luma_width_padded)
        {
            int col_ctu_nums = (luma_width_padded / 64 - 2) * 64;
            if ((tmp_pix_y + 128) == luma_height_padded && col_ctu_nums >= 0)
            {
                for (int i = 0; i < 2; ++i)
                {
                    ref_fetch_lcu_in_rfc_3(tmp_pix_x - col_ctu_nums, tmp_pix_y + i * 64, fetch_ctx_io, Fetch_input_video_U8_ptr->y_rfcBin,
                        Fetch_input_video_U8_ptr->y_rfcPrm, Fetch_input_video_U8_ptr->u_rfcBin, Fetch_input_video_U8_ptr->u_rfcPrm,
                        Fetch_input_video_U8_ptr->v_rfcBin, Fetch_input_video_U8_ptr->v_rfcPrm,
                        refWinY3_ref + i * 64, refWinU3_ref + i * 32, refWinV3_ref + i * 32);

                    if ((code_stru_temp != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) &&
                        ctx_rpm_num_refp_0 > 1) || (code_stru_temp == RA &&
                            fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B))
                    {
                        ref_fetch_lcu_in_rfc_3(tmp_pix_x - col_ctu_nums, tmp_pix_y + i * 64, fetch_ctx_io, Fetch_input_video_U8_ptr->y_rfcBin1,
                            Fetch_input_video_U8_ptr->y_rfcPrm1, Fetch_input_video_U8_ptr->u_rfcBin1, Fetch_input_video_U8_ptr->u_rfcPrm1,
                            Fetch_input_video_U8_ptr->v_rfcBin1, Fetch_input_video_U8_ptr->v_rfcPrm1,
                            refWinY3_B_ref + i * 64, refWinU3_B_ref + i * 32, refWinV3_B_ref + i * 32);
                    }
                }

                if ((tmp_pix_x - col_ctu_nums + 64) == luma_width_padded)
                {
                    int offset_luma_mod_col2 = luma_mod_num_col1 ? luma_mod_num_col1 : 64;
                    int offset_chroma_mod_col2 = chroma_mod_num_col1 ? chroma_mod_num_col1 : 32;

                    for (int j = 0; j < 128; ++j)
                    {
                        for (int i = offset_luma_mod_col2; i < 64; ++i)
                        {
                            refWinY3_ref[j][i] = refWinY3_ref[j][luma_mod_num_col1 - 1];
                        }
                    }

                    for (int j = 0; j < 64; ++j)
                    {
                        for (int i = offset_chroma_mod_col2; i < 32; ++i)
                        {
                            refWinU3_ref[j][i] = refWinU3_ref[j][chroma_mod_num_col1 - 1];
                            refWinV3_ref[j][i] = refWinV3_ref[j][chroma_mod_num_col1 - 1];
                        }
                    }

                    if ((code_stru_temp != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) &&
                        ctx_rpm_num_refp_0 > 1) || (code_stru_temp == RA &&
                            fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B))
                    {
                        for (int j = 0; j < 128; ++j)
                        {
                            for (int i = offset_luma_mod_col2; i < 64; ++i)
                            {
                                refWinY3_B_ref[j][i] = refWinY3_B_ref[j][luma_mod_num_col1 - 1];
                            }
                        }

                        for (int j = 0; j < 64; ++j)
                        {
                            for (int i = offset_chroma_mod_col2; i < 32; ++i)
                            {
                                refWinU3_B_ref[j][i] = refWinU3_B_ref[j][chroma_mod_num_col1 - 1];
                                refWinV3_B_ref[j][i] = refWinV3_B_ref[j][chroma_mod_num_col1 - 1];
                            }
                        }
                    }
                }

                int offset_luma_mod_row2 = luma_mod_num_row1 ? luma_mod_num_row1 : 64;
                int offset_chroma_mod_row2 = chroma_mod_num_row1 ? chroma_mod_num_row1 : 32;

                for (int j = 64 + offset_luma_mod_row2; j < 128; ++j)
                {
                    for (int i = 0; i < 32; ++i)
                    {
                        refWinY3_ref[j][i] = refWinY3_ref[64 + offset_luma_mod_row2 - 1][i];
                        refWinY3_ref[j][i + 32] = refWinY3_ref[64 + offset_luma_mod_row2 - 1][i + 32];
                    }
                }

                for (int j = 32 + offset_chroma_mod_row2; j < 64; ++j)
                {
                    for (int i = 0; i < 16; ++i)
                    {
                        refWinU3_ref[j][i] = refWinU3_ref[32 + offset_chroma_mod_row2 - 1][i];
                        refWinU3_ref[j][i + 16] = refWinU3_ref[32 + offset_chroma_mod_row2 - 1][i + 16];

                        refWinV3_ref[j][i] = refWinV3_ref[32 + offset_chroma_mod_row2 - 1][i];
                        refWinV3_ref[j][i + 16] = refWinV3_ref[32 + offset_chroma_mod_row2 - 1][i + 16];
                    }
                }

                if ((code_stru_temp != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) &&
                    ctx_rpm_num_refp_0 > 1) || (code_stru_temp == RA &&
                        fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B))
                {
                    for (int j = 64 + offset_luma_mod_row2; j < 128; ++j)
                    {
                        for (int i = 0; i < 32; ++i)
                        {
                            refWinY3_B_ref[j][i] = refWinY3_B_ref[64 + offset_luma_mod_row2 - 1][i];
                            refWinY3_B_ref[j][i + 32] = refWinY3_B_ref[64 + offset_luma_mod_row2 - 1][i + 32];
                        }
                    }

                    for (int j = 32 + offset_chroma_mod_row2; j < 64; ++j)
                    {
                        for (int i = 0; i < 16; ++i)
                        {
                            refWinU3_B_ref[j][i] = refWinU3_B_ref[32 + offset_chroma_mod_row2 - 1][i];
                            refWinU3_B_ref[j][i + 16] = refWinU3_B_ref[32 + offset_chroma_mod_row2 - 1][i + 16];

                            refWinV3_B_ref[j][i] = refWinV3_B_ref[32 + offset_chroma_mod_row2 - 1][i];
                            refWinV3_B_ref[j][i + 16] = refWinV3_B_ref[32 + offset_chroma_mod_row2 - 1][i + 16];
                        }
                    }
                }
            }
            else if ((tmp_pix_y + 192) <= luma_height_padded && col_ctu_nums >= 0)
            {
                for (int i = 0; i < 3; ++i)
                {
                    ref_fetch_lcu_in_rfc_3(tmp_pix_x - col_ctu_nums, tmp_pix_y + i * 64, fetch_ctx_io, Fetch_input_video_U8_ptr->y_rfcBin,
                        Fetch_input_video_U8_ptr->y_rfcPrm, Fetch_input_video_U8_ptr->u_rfcBin, Fetch_input_video_U8_ptr->u_rfcPrm,
                        Fetch_input_video_U8_ptr->v_rfcBin, Fetch_input_video_U8_ptr->v_rfcPrm,
                        refWinY3_ref + i * 64, refWinU3_ref + i * 32, refWinV3_ref + i * 32);

                    if ((code_stru_temp != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) &&
                        ctx_rpm_num_refp_0 > 1) || (code_stru_temp == RA &&
                            fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B))
                    {
                        ref_fetch_lcu_in_rfc_3(tmp_pix_x - col_ctu_nums, tmp_pix_y + i * 64, fetch_ctx_io, Fetch_input_video_U8_ptr->y_rfcBin1,
                            Fetch_input_video_U8_ptr->y_rfcPrm1, Fetch_input_video_U8_ptr->u_rfcBin1, Fetch_input_video_U8_ptr->u_rfcPrm1,
                            Fetch_input_video_U8_ptr->v_rfcBin1, Fetch_input_video_U8_ptr->v_rfcPrm1,
                            refWinY3_B_ref + i * 64, refWinU3_B_ref + i * 32, refWinV3_B_ref + i * 32);
                    }
                }

                if ((tmp_pix_x - col_ctu_nums + 64) == luma_width_padded)
                {
                    int offset_luma_mod_col2 = luma_mod_num_col1 ? luma_mod_num_col1 : 64;
                    int offset_chroma_mod_col2 = chroma_mod_num_col1 ? chroma_mod_num_col1 : 32;

                    for (int j = 0; j < 192; ++j)
                    {
                        for (int i = offset_luma_mod_col2; i < 64; ++i)
                        {
                            refWinY3_ref[j][i] = refWinY3_ref[j][luma_mod_num_col1 - 1];
                        }
                    }

                    for (int j = 0; j < 96; ++j)
                    {
                        for (int i = offset_chroma_mod_col2; i < 32; ++i)
                        {
                            refWinU3_ref[j][i] = refWinU3_ref[j][chroma_mod_num_col1 - 1];
                            refWinV3_ref[j][i] = refWinV3_ref[j][chroma_mod_num_col1 - 1];
                        }
                    }

                    if ((code_stru_temp != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) &&
                        ctx_rpm_num_refp_0 > 1) || (code_stru_temp == RA &&
                            fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B))
                    {
                        for (int j = 0; j < 192; ++j)
                        {
                            for (int i = offset_luma_mod_col2; i < 64; ++i)
                            {
                                refWinY3_B_ref[j][i] = refWinY3_B_ref[j][luma_mod_num_col1 - 1];
                            }
                        }

                        for (int j = 0; j < 96; ++j)
                        {
                            for (int i = offset_chroma_mod_col2; i < 32; ++i)
                            {
                                refWinU3_B_ref[j][i] = refWinU3_B_ref[j][chroma_mod_num_col1 - 1];
                                refWinV3_B_ref[j][i] = refWinV3_B_ref[j][chroma_mod_num_col1 - 1];
                            }
                        }
                    }
                }

                if ((tmp_pix_y + 192) == luma_height_padded)
                {
                    int offset_luma_mod_row2 = luma_mod_num_row1 ? luma_mod_num_row1 : 64;
                    int offset_chroma_mod_row2 = chroma_mod_num_row1 ? chroma_mod_num_row1 : 32;

                    for (int j = 128 + offset_luma_mod_row2; j < 192; ++j)
                    {
                        for (int i = 0; i < 32; ++i)
                        {
                            refWinY3_ref[j][i] = refWinY3_ref[128 + offset_luma_mod_row2 - 1][i];
                            refWinY3_ref[j][i + 32] = refWinY3_ref[128 + offset_luma_mod_row2 - 1][i + 32];
                        }
                    }

                    for (int j = 64 + offset_chroma_mod_row2; j < 96; ++j)
                    {
                        for (int i = 0; i < 16; ++i)
                        {
                            refWinU3_ref[j][i] = refWinU3_ref[64 + offset_chroma_mod_row2 - 1][i];
                            refWinU3_ref[j][i + 16] = refWinU3_ref[64 + offset_chroma_mod_row2 - 1][i + 16];

                            refWinV3_ref[j][i] = refWinV3_ref[64 + offset_chroma_mod_row2 - 1][i];
                            refWinV3_ref[j][i + 16] = refWinV3_ref[64 + offset_chroma_mod_row2 - 1][i + 16];
                        }
                    }

                    if ((code_stru_temp != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) &&
                        ctx_rpm_num_refp_0 > 1) || (code_stru_temp == RA &&
                            fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B))
                    {
                        for (int j = 128 + offset_luma_mod_row2; j < 192; ++j)
                        {
                            for (int i = 0; i < 32; ++i)
                            {
                                refWinY3_B_ref[j][i] = refWinY3_B_ref[128 + offset_luma_mod_row2 - 1][i];
                                refWinY3_B_ref[j][i + 32] = refWinY3_B_ref[128 + offset_luma_mod_row2 - 1][i + 32];
                            }
                        }

                        for (int j = 64 + offset_chroma_mod_row2; j < 96; ++j)
                        {
                            for (int i = 0; i < 16; ++i)
                            {
                                refWinU3_B_ref[j][i] = refWinU3_B_ref[64 + offset_chroma_mod_row2 - 1][i];
                                refWinU3_B_ref[j][i + 16] = refWinU3_B_ref[64 + offset_chroma_mod_row2 - 1][i + 16];

                                refWinV3_B_ref[j][i] = refWinV3_B_ref[64 + offset_chroma_mod_row2 - 1][i];
                                refWinV3_B_ref[j][i + 16] = refWinV3_B_ref[64 + offset_chroma_mod_row2 - 1][i + 16];
                            }
                        }
                    }
                }
            }
        }
        else if ((tmp_pix_x + 128) == (luma_width_padded + 64))
        {
            int col_ctu_nums = (luma_width_padded / 64 - 2) * 64;
            if ((tmp_pix_y + 128) == luma_height_padded && col_ctu_nums >= 0)
            {
                for (int i = 0; i < 2; ++i)
                {
                    ref_fetch_lcu_in_rfc_3(tmp_pix_x - col_ctu_nums, tmp_pix_y + i * 64, fetch_ctx_io, Fetch_input_video_U8_ptr->y_rfcBin,
                        Fetch_input_video_U8_ptr->y_rfcPrm, Fetch_input_video_U8_ptr->u_rfcBin, Fetch_input_video_U8_ptr->u_rfcPrm,
                        Fetch_input_video_U8_ptr->v_rfcBin, Fetch_input_video_U8_ptr->v_rfcPrm,
                        refWinY3_ref + i * 64, refWinU3_ref + i * 32, refWinV3_ref + i * 32);

                    if ((code_stru_temp != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) &&
                        ctx_rpm_num_refp_0 > 1) || (code_stru_temp == RA &&
                            fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B))
                    {
                        ref_fetch_lcu_in_rfc_3(tmp_pix_x - col_ctu_nums, tmp_pix_y + i * 64, fetch_ctx_io, Fetch_input_video_U8_ptr->y_rfcBin1,
                            Fetch_input_video_U8_ptr->y_rfcPrm1, Fetch_input_video_U8_ptr->u_rfcBin1, Fetch_input_video_U8_ptr->u_rfcPrm1,
                            Fetch_input_video_U8_ptr->v_rfcBin1, Fetch_input_video_U8_ptr->v_rfcPrm1,
                            refWinY3_B_ref + i * 64, refWinU3_B_ref + i * 32, refWinV3_B_ref + i * 32);
                    }
                }

                if ((tmp_pix_x - col_ctu_nums + 64) == luma_width_padded)
                {
                    int offset_luma_mod_col2 = luma_mod_num_col1 ? luma_mod_num_col1 : 64;
                    int offset_chroma_mod_col2 = chroma_mod_num_col1 ? chroma_mod_num_col1 : 32;

                    for (int j = 0; j < 128; ++j)
                    {
                        for (int i = offset_luma_mod_col2; i < 64; ++i)
                        {
                            refWinY3_ref[j][i] = refWinY3_ref[j][luma_mod_num_col1 - 1];
                        }
                    }

                    for (int j = 0; j < 64; ++j)
                    {
                        for (int i = offset_chroma_mod_col2; i < 32; ++i)
                        {
                            refWinU3_ref[j][i] = refWinU3_ref[j][chroma_mod_num_col1 - 1];
                            refWinV3_ref[j][i] = refWinV3_ref[j][chroma_mod_num_col1 - 1];
                        }
                    }

                    if ((code_stru_temp != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) &&
                        ctx_rpm_num_refp_0 > 1) || (code_stru_temp == RA &&
                            fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B))
                    {
                        for (int j = 0; j < 128; ++j)
                        {
                            for (int i = offset_luma_mod_col2; i < 64; ++i)
                            {
                                refWinY3_B_ref[j][i] = refWinY3_B_ref[j][luma_mod_num_col1 - 1];
                            }
                        }

                        for (int j = 0; j < 64; ++j)
                        {
                            for (int i = offset_chroma_mod_col2; i < 32; ++i)
                            {
                                refWinU3_B_ref[j][i] = refWinU3_B_ref[j][chroma_mod_num_col1 - 1];
                                refWinV3_B_ref[j][i] = refWinV3_B_ref[j][chroma_mod_num_col1 - 1];
                            }
                        }
                    }
                }

                int offset_luma_mod_row2 = luma_mod_num_row1 ? luma_mod_num_row1 : 64;
                int offset_chroma_mod_row2 = chroma_mod_num_row1 ? chroma_mod_num_row1 : 32;

                for (int j = 64 + offset_luma_mod_row2; j < 128; ++j)
                {
                    for (int i = 0; i < 32; ++i)
                    {
                        refWinY3_ref[j][i] = refWinY3_ref[64 + offset_luma_mod_row2 - 1][i];
                        refWinY3_ref[j][i + 32] = refWinY3_ref[64 + offset_luma_mod_row2 - 1][i + 32];
                    }
                }

                for (int j = 32 + offset_chroma_mod_row2; j < 64; ++j)
                {
                    for (int i = 0; i < 16; ++i)
                    {
                        refWinU3_ref[j][i] = refWinU3_ref[32 + offset_chroma_mod_row2 - 1][i];
                        refWinU3_ref[j][i + 16] = refWinU3_ref[32 + offset_chroma_mod_row2 - 1][i + 16];

                        refWinV3_ref[j][i] = refWinV3_ref[32 + offset_chroma_mod_row2 - 1][i];
                        refWinV3_ref[j][i + 16] = refWinV3_ref[32 + offset_chroma_mod_row2 - 1][i + 16];
                    }
                }

                if ((code_stru_temp != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) &&
                    ctx_rpm_num_refp_0 > 1) || (code_stru_temp == RA &&
                        fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B))
                {
                    for (int j = 64 + offset_luma_mod_row2; j < 128; ++j)
                    {
                        for (int i = 0; i < 32; ++i)
                        {
                            refWinY3_B_ref[j][i] = refWinY3_B_ref[64 + offset_luma_mod_row2 - 1][i];
                            refWinY3_B_ref[j][i + 32] = refWinY3_B_ref[64 + offset_luma_mod_row2 - 1][i + 32];
                        }
                    }

                    for (int j = 32 + offset_chroma_mod_row2; j < 64; ++j)
                    {
                        for (int i = 0; i < 16; ++i)
                        {
                            refWinU3_B_ref[j][i] = refWinU3_B_ref[32 + offset_chroma_mod_row2 - 1][i];
                            refWinU3_B_ref[j][i + 16] = refWinU3_B_ref[32 + offset_chroma_mod_row2 - 1][i + 16];

                            refWinV3_B_ref[j][i] = refWinV3_B_ref[32 + offset_chroma_mod_row2 - 1][i];
                            refWinV3_B_ref[j][i + 16] = refWinV3_B_ref[32 + offset_chroma_mod_row2 - 1][i + 16];
                        }
                    }
                }
            }
            else if ((tmp_pix_y + 192) <= luma_height_padded && col_ctu_nums >= 0)
            {
                for (int i = 0; i < 3; ++i)
                {
                    ref_fetch_lcu_in_rfc_3(tmp_pix_x - col_ctu_nums, tmp_pix_y + i * 64, fetch_ctx_io, Fetch_input_video_U8_ptr->y_rfcBin,
                        Fetch_input_video_U8_ptr->y_rfcPrm, Fetch_input_video_U8_ptr->u_rfcBin, Fetch_input_video_U8_ptr->u_rfcPrm,
                        Fetch_input_video_U8_ptr->v_rfcBin, Fetch_input_video_U8_ptr->v_rfcPrm,
                        refWinY3_ref + i * 64, refWinU3_ref + i * 32, refWinV3_ref + i * 32);

                    if ((code_stru_temp != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) &&
                        ctx_rpm_num_refp_0 > 1) || (code_stru_temp == RA &&
                            fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B))
                    {
                        ref_fetch_lcu_in_rfc_3(tmp_pix_x - col_ctu_nums, tmp_pix_y + i * 64, fetch_ctx_io, Fetch_input_video_U8_ptr->y_rfcBin1,
                            Fetch_input_video_U8_ptr->y_rfcPrm1, Fetch_input_video_U8_ptr->u_rfcBin1, Fetch_input_video_U8_ptr->u_rfcPrm1,
                            Fetch_input_video_U8_ptr->v_rfcBin1, Fetch_input_video_U8_ptr->v_rfcPrm1,
                            refWinY3_B_ref + i * 64, refWinU3_B_ref + i * 32, refWinV3_B_ref + i * 32);
                    }
                }

                if ((tmp_pix_x - col_ctu_nums + 64) == luma_width_padded)
                {
                    int offset_luma_mod_col2 = luma_mod_num_col1 ? luma_mod_num_col1 : 64;
                    int offset_chroma_mod_col2 = chroma_mod_num_col1 ? chroma_mod_num_col1 : 32;

                    for (int j = 0; j < 192; ++j)
                    {
                        for (int i = offset_luma_mod_col2; i < 64; ++i)
                        {
                            refWinY3_ref[j][i] = refWinY3_ref[j][luma_mod_num_col1 - 1];
                        }
                    }

                    for (int j = 0; j < 96; ++j)
                    {
                        for (int i = offset_chroma_mod_col2; i < 32; ++i)
                        {
                            refWinU3_ref[j][i] = refWinU3_ref[j][chroma_mod_num_col1 - 1];
                            refWinV3_ref[j][i] = refWinV3_ref[j][chroma_mod_num_col1 - 1];
                        }
                    }

                    if ((code_stru_temp != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) &&
                        ctx_rpm_num_refp_0 > 1) || (code_stru_temp == RA &&
                            fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B))
                    {
                        for (int j = 0; j < 192; ++j)
                        {
                            for (int i = offset_luma_mod_col2; i < 64; ++i)
                            {
                                refWinY3_B_ref[j][i] = refWinY3_B_ref[j][luma_mod_num_col1 - 1];
                            }
                        }

                        for (int j = 0; j < 96; ++j)
                        {
                            for (int i = offset_chroma_mod_col2; i < 32; ++i)
                            {
                                refWinU3_B_ref[j][i] = refWinU3_B_ref[j][chroma_mod_num_col1 - 1];
                                refWinV3_B_ref[j][i] = refWinV3_B_ref[j][chroma_mod_num_col1 - 1];
                            }
                        }
                    }
                }

                if ((tmp_pix_y + 192) == luma_height_padded)
                {
                    int offset_luma_mod_row2 = luma_mod_num_row1 ? luma_mod_num_row1 : 64;
                    int offset_chroma_mod_row2 = chroma_mod_num_row1 ? chroma_mod_num_row1 : 32;

                    for (int j = 128 + offset_luma_mod_row2; j < 192; ++j)
                    {
                        for (int i = 0; i < 32; ++i)
                        {
                            refWinY3_ref[j][i] = refWinY3_ref[128 + offset_luma_mod_row2 - 1][i];
                            refWinY3_ref[j][i + 32] = refWinY3_ref[128 + offset_luma_mod_row2 - 1][i + 32];
                        }
                    }

                    for (int j = 64 + offset_chroma_mod_row2; j < 96; ++j)
                    {
                        for (int i = 0; i < 16; ++i)
                        {
                            refWinU3_ref[j][i] = refWinU3_ref[64 + offset_chroma_mod_row2 - 1][i];
                            refWinU3_ref[j][i + 16] = refWinU3_ref[64 + offset_chroma_mod_row2 - 1][i + 16];

                            refWinV3_ref[j][i] = refWinV3_ref[64 + offset_chroma_mod_row2 - 1][i];
                            refWinV3_ref[j][i + 16] = refWinV3_ref[64 + offset_chroma_mod_row2 - 1][i + 16];
                        }
                    }

                    if ((code_stru_temp != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) &&
                        ctx_rpm_num_refp_0 > 1) || (code_stru_temp == RA &&
                            fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B))
                    {
                        for (int j = 128 + offset_luma_mod_row2; j < 192; ++j)
                        {
                            for (int i = 0; i < 32; ++i)
                            {
                                refWinY3_B_ref[j][i] = refWinY3_B_ref[128 + offset_luma_mod_row2 - 1][i];
                                refWinY3_B_ref[j][i + 32] = refWinY3_B_ref[128 + offset_luma_mod_row2 - 1][i + 32];
                            }
                        }

                        for (int j = 64 + offset_chroma_mod_row2; j < 96; ++j)
                        {
                            for (int i = 0; i < 16; ++i)
                            {
                                refWinU3_B_ref[j][i] = refWinU3_B_ref[64 + offset_chroma_mod_row2 - 1][i];
                                refWinU3_B_ref[j][i + 16] = refWinU3_B_ref[64 + offset_chroma_mod_row2 - 1][i + 16];

                                refWinV3_B_ref[j][i] = refWinV3_B_ref[64 + offset_chroma_mod_row2 - 1][i];
                                refWinV3_B_ref[j][i + 16] = refWinV3_B_ref[64 + offset_chroma_mod_row2 - 1][i + 16];
                            }
                        }
                    }
                }
            }
        }
    }
    //last row
    else if (tmp_pix_y == (luma_height_padded - 64))
    {
        if ((tmp_pix_x + 128) <= (luma_width_padded - 64))
        {
            for (int i = -1; i < 1; ++i)
            {
                ref_fetch_lcu_in_rfc_3(tmp_pix_x + 128, tmp_pix_y + i * 64, fetch_ctx_io, Fetch_input_video_U8_ptr->y_rfcBin,
                    Fetch_input_video_U8_ptr->y_rfcPrm, Fetch_input_video_U8_ptr->u_rfcBin, Fetch_input_video_U8_ptr->u_rfcPrm,
                    Fetch_input_video_U8_ptr->v_rfcBin, Fetch_input_video_U8_ptr->v_rfcPrm,
                    refWinY3_ref + (i + 1) * 64, refWinU3_ref + (i + 1) * 32, refWinV3_ref + (i + 1) * 32);

                if ((code_stru_temp != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) &&
                    ctx_rpm_num_refp_0 > 1) || (code_stru_temp == RA &&
                        fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B))
                {
                    ref_fetch_lcu_in_rfc_3(tmp_pix_x + 128, tmp_pix_y + i * 64, fetch_ctx_io, Fetch_input_video_U8_ptr->y_rfcBin1,
                        Fetch_input_video_U8_ptr->y_rfcPrm1, Fetch_input_video_U8_ptr->u_rfcBin1, Fetch_input_video_U8_ptr->u_rfcPrm1,
                        Fetch_input_video_U8_ptr->v_rfcBin1, Fetch_input_video_U8_ptr->v_rfcPrm1,
                        refWinY3_B_ref + (i + 1) * 64, refWinU3_B_ref + (i + 1) * 32, refWinV3_B_ref + (i + 1) * 32);
                }
            }

            if ((tmp_pix_x + 128) == (luma_width_padded - 64))
            {
                int offset_luma_mod_col2 = luma_mod_num_col1 ? luma_mod_num_col1 : 64;
                int offset_chroma_mod_col2 = chroma_mod_num_col1 ? chroma_mod_num_col1 : 32;

                for (int j = 0; j < 128; ++j)
                {
                    for (int i = offset_luma_mod_col2; i < 64; ++i)
                    {
                        refWinY3_ref[j][i] = refWinY3_ref[j][luma_mod_num_col1 - 1];
                    }
                }

                for (int j = 0; j < 64; ++j)
                {
                    for (int i = offset_chroma_mod_col2; i < 32; ++i)
                    {
                        refWinU3_ref[j][i] = refWinU3_ref[j][chroma_mod_num_col1 - 1];
                        refWinV3_ref[j][i] = refWinV3_ref[j][chroma_mod_num_col1 - 1];
                    }
                }

                if ((code_stru_temp != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) &&
                    ctx_rpm_num_refp_0 > 1) || (code_stru_temp == RA &&
                        fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B))
                {
                    for (int j = 0; j < 128; ++j)
                    {
                        for (int i = offset_luma_mod_col2; i < 64; ++i)
                        {
                            refWinY3_B_ref[j][i] = refWinY3_B_ref[j][luma_mod_num_col1 - 1];
                        }
                    }

                    for (int j = 0; j < 64; ++j)
                    {
                        for (int i = offset_chroma_mod_col2; i < 32; ++i)
                        {
                            refWinU3_B_ref[j][i] = refWinU3_B_ref[j][chroma_mod_num_col1 - 1];
                            refWinV3_B_ref[j][i] = refWinV3_B_ref[j][chroma_mod_num_col1 - 1];
                        }
                    }
                }
            }

            int offset_luma_mod_row2 = luma_mod_num_row1 ? luma_mod_num_row1 : 64;
            int offset_chroma_mod_row2 = chroma_mod_num_row1 ? chroma_mod_num_row1 : 32;

            for (int j = 64 + offset_luma_mod_row2; j < 128; ++j)
            {
                for (int i = 0; i < 32; ++i)
                {
                    refWinY3_ref[j][i] = refWinY3_ref[64 + offset_luma_mod_row2 - 1][i];
                    refWinY3_ref[j][i + 32] = refWinY3_ref[64 + offset_luma_mod_row2 - 1][i + 32];
                }
            }

            for (int j = 32 + offset_chroma_mod_row2; j < 64; ++j)
            {
                for (int i = 0; i < 16; ++i)
                {
                    refWinU3_ref[j][i] = refWinU3_ref[32 + offset_chroma_mod_row2 - 1][i];
                    refWinU3_ref[j][i + 16] = refWinU3_ref[32 + offset_chroma_mod_row2 - 1][i + 16];

                    refWinV3_ref[j][i] = refWinV3_ref[32 + offset_chroma_mod_row2 - 1][i];
                    refWinV3_ref[j][i + 16] = refWinV3_ref[32 + offset_chroma_mod_row2 - 1][i + 16];
                }
            }

            if ((code_stru_temp != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) &&
                ctx_rpm_num_refp_0 > 1) || (code_stru_temp == RA &&
                    fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B))
            {
                for (int j = 64 + offset_luma_mod_row2; j < 128; ++j)
                {
                    for (int i = 0; i < 32; ++i)
                    {
                        refWinY3_B_ref[j][i] = refWinY3_B_ref[64 + offset_luma_mod_row2 - 1][i];
                        refWinY3_B_ref[j][i + 32] = refWinY3_B_ref[64 + offset_luma_mod_row2 - 1][i + 32];
                    }
                }

                for (int j = 32 + offset_chroma_mod_row2; j < 64; ++j)
                {
                    for (int i = 0; i < 16; ++i)
                    {
                        refWinU3_B_ref[j][i] = refWinU3_B_ref[32 + offset_chroma_mod_row2 - 1][i];
                        refWinU3_B_ref[j][i + 16] = refWinU3_B_ref[32 + offset_chroma_mod_row2 - 1][i + 16];

                        refWinV3_B_ref[j][i] = refWinV3_B_ref[32 + offset_chroma_mod_row2 - 1][i];
                        refWinV3_B_ref[j][i + 16] = refWinV3_B_ref[32 + offset_chroma_mod_row2 - 1][i + 16];
                    }
                }
            }
        }
    }
}
#endif

//send commond to RTL for getting CTU's num
#ifdef __SYNTHESIS__
void write_inf_2_rtl_luma(u32* inf_luma,int rtl_use_idx,u8 ctx_luma,u8 cty_luma,u8 block_luma,u8 addr_luma)
{
    inf_luma[rtl_use_idx] = (0xff & ctx_luma) | (0xff00 & (cty_luma << 8)) | (0xff0000 & (block_luma << 16)) | (0xff000000 & (addr_luma << 24));
}

void write_inf_2_rtl_chroma(u32* inf_chroma,int rtl_use_idx,u8 ctx_chroma,u8 cty_chroma,u8 block_chroma,u8 addr_chroma)
{
    inf_chroma[rtl_use_idx] = (0xff & ctx_chroma) | (0xff00 & (cty_chroma << 8)) | (0xff0000 & (block_chroma << 16)) | (0xff000000 & (addr_chroma << 24));
}

void get_ctu_from_rtl(CTX_FETCH* fetch_ctx_io,int code_stru_temp,Slice_Level_Parameters* ctu_ctrl_input,
                      U32 rtl_use_flag_y_tmep[64],U32 rtl_use_flag_u_tmep[64],U32 rtl_use_flag_v_tmep[64]
                      /*U32 rtl_use_flag_y_B_tmep[64],U32 rtl_use_flag_u_B_tmep[64],U32 rtl_use_flag_v_B_tmep[64]*/)
{

    int tmp_pix_y, tmp_pix_x;

    static int core_x_lcu_tmp_run, core_y_lcu_tmp_run;
    if (ctu_ctrl_input->slice_enable) 
    {
        core_x_lcu_tmp_run = 0;
        core_y_lcu_tmp_run = 0;
    }
    else 
    {
        core_x_lcu_tmp_run++;
    }
    
    int luma_width_padded = (fetch_ctx_io->ctx_pinter_refp_pic_width_luma + 63) / 64 * 64;
    int luma_height_padded = (fetch_ctx_io->ctx_pinter_refp_pic_height_luma + 63) / 64 * 64;

    int img_width = fetch_ctx_io->ctx_info_pic_width;
    int img_height = fetch_ctx_io->ctx_info_pic_height;
    int lcu_width = (img_width + 63) >> 6;

    if (core_x_lcu_tmp_run == lcu_width) 
    {
        core_y_lcu_tmp_run++;
        core_x_lcu_tmp_run = 0;
    }
    
    tmp_pix_y = core_y_lcu_tmp_run << fetch_ctx_io->log2_max_cuwh;
    tmp_pix_x = core_x_lcu_tmp_run << fetch_ctx_io->log2_max_cuwh; 

    int rtl_use_idx_y = 0;
    int rtl_use_idx_u = 0;
    int rtl_use_idx_v = 0;
    // int rtl_use_idx_y_B = 0;
    // int rtl_use_idx_u_B = 0;
    // int rtl_use_idx_v_B = 0;
    // int b_frame_flag = 0;

    int first_col_flag = 0;
    int for_st = 0;
    int for_ed = 0;
    int ctu_x = 0;
    int ctu_y = 0;

    //first row
    if (0 == tmp_pix_y)
    {
        if (0 == tmp_pix_x)
        {
            if ((tmp_pix_x + 128) <= (luma_width_padded - 64))
            {
                first_col_flag = 1;
                for_st = 0;
                for_ed = 2;
                ctu_x = tmp_pix_x / 64;
                ctu_y = tmp_pix_y / 64;
                //1 1 1
                //2 2 2
                //3 3 3

                /*if ((code_stru_temp != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) &&
                        ctx_rpm_num_refp_0 > 1) || (code_stru_temp == RA &&
                            fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B))
                {
                    b_frame_flag=1;
                    //1 1 1
                    //2 2 2
                    //3 3 3
                }*/
            }
            else if ((tmp_pix_x + 128) == luma_width_padded)
            {

                first_col_flag = 2;
                for_st = 0;
                for_ed = 2;
                ctu_x = tmp_pix_x / 64;
                ctu_y = tmp_pix_y / 64;
                //1 1 1
                //2 2 2
                /*if ((code_stru_temp != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) &&
                        ctx_rpm_num_refp_0 > 1) || (code_stru_temp == RA &&
                            fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B))
                {
                    b_frame_flag=1;
                    //1 1 1
                    //2 2 2
                }*/
            }
            else if ((tmp_pix_x + 128) == (luma_width_padded + 64))
            {

                first_col_flag = 3;
                for_st = 0;
                for_ed = 2;
                ctu_x = tmp_pix_x / 64;
                ctu_y = tmp_pix_y / 64;
                //1 1 1
                /*if ((code_stru_temp != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) &&
                        ctx_rpm_num_refp_0 > 1) || (code_stru_temp == RA &&
                            fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B))
                {
                    b_frame_flag=1;
                    //1 1 1
                }*/
            }
        }
        else if (tmp_pix_x < luma_width_padded && tmp_pix_x >= 64)
        {
            if ((tmp_pix_x + 128) <= (luma_width_padded - 64))
            {

                for_st = 0;
                for_ed = 2;
                ctu_x = (tmp_pix_x+128) / 64;
                ctu_y = tmp_pix_y / 64;
                //3 3 3
                /*if ((code_stru_temp != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) &&
                        ctx_rpm_num_refp_0 > 1) || (code_stru_temp == RA &&
                            fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B))
                {
                    b_frame_flag=1;
                    //3 3 3
                }*/
            }
            else if ((tmp_pix_x + 128) == luma_width_padded || (tmp_pix_x + 128) == (luma_width_padded + 64))
            {
                int col_ctu_nums = (luma_width_padded / 64 - 2) * 64;
                if ((tmp_pix_y + 128) == luma_height_padded && col_ctu_nums >= 0)
                {

                    for_st = 0;
                    for_ed = 2;
                    ctu_x = (tmp_pix_x - col_ctu_nums) / 64;
                    ctu_y = tmp_pix_y / 64;
                    //3 3 3
                    /*if ((code_stru_temp != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) &&
                        ctx_rpm_num_refp_0 > 1) || (code_stru_temp == RA &&
                            fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B))
                    {
                        b_frame_flag=1;
                        //3 3 3
                    }*/
                }
                else if ((tmp_pix_y + 192) <= luma_height_padded && col_ctu_nums >= 0)
                {
                    for_st = 0;
                    for_ed = 3;
                    ctu_x = (tmp_pix_x - col_ctu_nums) / 64;
                    ctu_y = tmp_pix_y / 64;
                    //3 3 3
                    /*if ((code_stru_temp != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) &&
                        ctx_rpm_num_refp_0 > 1) || (code_stru_temp == RA &&
                            fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B))
                    {
                        b_frame_flag=1;
                        //3 3 3
                    }*/
                }
            }
        }
    }
    //middle row
    else if (tmp_pix_y <= (luma_height_padded - 128))
    {
        if ((tmp_pix_x + 128) <= (luma_width_padded - 64))
        {

            for_st = -1;
            for_ed = 2;
            ctu_x = (tmp_pix_x+128) / 64;
            ctu_y = tmp_pix_y / 64;
            //3 3 3
            /*if ((code_stru_temp != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) &&
                        ctx_rpm_num_refp_0 > 1) || (code_stru_temp == RA &&
                            fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B))
            {
                b_frame_flag=1;
                //3 3 3
            }*/
        }
        else if ((tmp_pix_x + 128) == luma_width_padded || (tmp_pix_x + 128) == (luma_width_padded + 64))
        {
            int col_ctu_nums = (luma_width_padded / 64 - 2) * 64;
            if ((tmp_pix_y + 128) == luma_height_padded && col_ctu_nums >= 0)
            {
                for_st = 0;
                for_ed = 2;
                ctu_x = (tmp_pix_x - col_ctu_nums) / 64;
                ctu_y = tmp_pix_y / 64;
                //3 3 3
                /*if ((code_stru_temp != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) &&
                        ctx_rpm_num_refp_0 > 1) || (code_stru_temp == RA &&
                            fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B))
                {
                    b_frame_flag=1;
                    //3 3 3
                }*/
            }
            else if ((tmp_pix_y + 192) <= luma_height_padded && col_ctu_nums >= 0)
            {
                for_st = 0;
                for_ed = 3;
                ctu_x = (tmp_pix_x - col_ctu_nums) / 64;
                ctu_y = tmp_pix_y / 64;
                //3 3 3
                /*if ((code_stru_temp != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) &&
                        ctx_rpm_num_refp_0 > 1) || (code_stru_temp == RA &&
                            fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B))
                {
                    b_frame_flag=1;
                    //3 3 3
                }*/
            }
        }
    }
    //last row
    else if (tmp_pix_y == (luma_height_padded - 64))
    {
        if ((tmp_pix_x + 128) <= (luma_width_padded - 64))
        {

            
            for_st = -1;
            for_ed = 1;
            ctu_x = (tmp_pix_x+128) / 64;
            ctu_y = tmp_pix_y / 64;
            //3 3 3
            /*if ((code_stru_temp != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) &&
                        ctx_rpm_num_refp_0 > 1) || (code_stru_temp == RA &&
                            fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B))
            {
                b_frame_flag=1;
                //3 3 3
            }*/
        }
    }

    if (1==first_col_flag)
    {
        for (int i = for_st; i < for_ed; ++i)
        {
#pragma HLS loop_tripcount max = 16
            write_inf_2_rtl_luma(rtl_use_flag_y_tmep, rtl_use_idx_y, ctu_x, ctu_y + i, 1, (i + 1) * 64);
            rtl_use_idx_y++;
            write_inf_2_rtl_chroma(rtl_use_flag_u_tmep, rtl_use_idx_u, ctu_x, ctu_y + i, 1, (i + 1) * 32);
            rtl_use_idx_u++;
            write_inf_2_rtl_chroma(rtl_use_flag_v_tmep, rtl_use_idx_v, ctu_x, ctu_y + i, 1, (i + 1) * 32);
            rtl_use_idx_v++;

            write_inf_2_rtl_luma(rtl_use_flag_y_tmep, rtl_use_idx_y, ctu_x + 1, ctu_y + i, 2, (i + 1) * 64);
            rtl_use_idx_y++;
            write_inf_2_rtl_chroma(rtl_use_flag_u_tmep, rtl_use_idx_u, ctu_x + 1, ctu_y + i, 2, (i + 1) * 32);
            rtl_use_idx_u++;
            write_inf_2_rtl_chroma(rtl_use_flag_v_tmep, rtl_use_idx_v, ctu_x + 1, ctu_y + i, 2, (i + 1) * 32);
            rtl_use_idx_v++;

            write_inf_2_rtl_luma(rtl_use_flag_y_tmep, rtl_use_idx_y, ctu_x + 2, ctu_y + i, 3, (i + 1) * 64);
            rtl_use_idx_y++;
            write_inf_2_rtl_chroma(rtl_use_flag_u_tmep, rtl_use_idx_u, ctu_x + 2, ctu_y + i, 3, (i + 1) * 32);
            rtl_use_idx_u++;
            write_inf_2_rtl_chroma(rtl_use_flag_v_tmep, rtl_use_idx_v, ctu_x + 2, ctu_y + i, 3, (i + 1) * 32);
            rtl_use_idx_v++;

            /*if (1==b_frame_flag)
            {
                write_inf_2_rtl_luma(rtl_use_flag_y_B_tmep, rtl_use_idx_y_B, ctu_x, ctu_y + i, 1, (i + 1) * 64);
                rtl_use_idx_y_B++;
                write_inf_2_rtl_chroma(rtl_use_flag_u_B_tmep, rtl_use_idx_u_B, ctu_x, ctu_y + i, 1, (i + 1) * 32);
                rtl_use_idx_u++;
                write_inf_2_rtl_chroma(rtl_use_flag_v_B_tmep, rtl_use_idx_v_B, ctu_x, ctu_y + i, 1, (i + 1) * 32);
                rtl_use_idx_v++;

                write_inf_2_rtl_luma(rtl_use_flag_y_B_tmep, rtl_use_idx_y_B, ctu_x + 1, ctu_y + i, 2, (i + 1) * 64);
                rtl_use_idx_y++;
                write_inf_2_rtl_chroma(rtl_use_flag_u_B_tmep, rtl_use_idx_u_B, ctu_x + 1, ctu_y + i, 2, (i + 1) * 32);
                rtl_use_idx_u++;
                write_inf_2_rtl_chroma(rtl_use_flag_v_B_tmep, rtl_use_idx_v_B, ctu_x + 1, ctu_y + i, 2, (i + 1) * 32);
                rtl_use_idx_v++;

                write_inf_2_rtl_luma(rtl_use_flag_y_B_tmep, rtl_use_idx_y_B, ctu_x + 2, ctu_y + i, 3, (i + 1) * 64);
                rtl_use_idx_y++;
                write_inf_2_rtl_chroma(rtl_use_flag_u_B_tmep, rtl_use_idx_u_B, ctu_x + 2, ctu_y + i, 3, (i + 1) * 32);
                rtl_use_idx_u++;
                write_inf_2_rtl_chroma(rtl_use_flag_v_B_tmep, rtl_use_idx_v_B, ctu_x + 2, ctu_y + i, 3, (i + 1) * 32);
                rtl_use_idx_v++;
            }*/
        }
    }
    else if (2==first_col_flag)
    {
        for (int i = for_st; i < for_ed; ++i)
        {
#pragma HLS loop_tripcount max = 16
            write_inf_2_rtl_luma(rtl_use_flag_y_tmep, rtl_use_idx_y, ctu_x, ctu_y + i, 1, (i + 1) * 64);
            rtl_use_idx_y++;
            write_inf_2_rtl_chroma(rtl_use_flag_u_tmep, rtl_use_idx_u, ctu_x, ctu_y + i, 1, (i + 1) * 32);
            rtl_use_idx_u++;
            write_inf_2_rtl_chroma(rtl_use_flag_v_tmep, rtl_use_idx_v, ctu_x, ctu_y + i, 1, (i + 1) * 32);
            rtl_use_idx_v++;

            write_inf_2_rtl_luma(rtl_use_flag_y_tmep, rtl_use_idx_y, ctu_x + 1, ctu_y + i, 2, (i + 1) * 64);
            rtl_use_idx_y++;
            write_inf_2_rtl_chroma(rtl_use_flag_u_tmep, rtl_use_idx_u, ctu_x + 1, ctu_y + i, 2, (i + 1) * 32);
            rtl_use_idx_u++;
            write_inf_2_rtl_chroma(rtl_use_flag_v_tmep, rtl_use_idx_v, ctu_x + 1, ctu_y + i, 2, (i + 1) * 32);
            rtl_use_idx_v++;

            /*if (1==b_frame_flag)
            {
                write_inf_2_rtl_luma(rtl_use_flag_y_B_tmep, rtl_use_idx_y_B, ctu_x, ctu_y + i, 1, (i + 1) * 64);
                rtl_use_idx_y_B++;
                write_inf_2_rtl_chroma(rtl_use_flag_u_B_tmep, rtl_use_idx_u_B, ctu_x, ctu_y + i, 1, (i + 1) * 32);
                rtl_use_idx_u++;
                write_inf_2_rtl_chroma(rtl_use_flag_v_B_tmep, rtl_use_idx_v_B, ctu_x, ctu_y + i, 1, (i + 1) * 32);
                rtl_use_idx_v++;

                write_inf_2_rtl_luma(rtl_use_flag_y_B_tmep, rtl_use_idx_y_B, ctu_x + 1, ctu_y + i, 2, (i + 1) * 64);
                rtl_use_idx_y++;
                write_inf_2_rtl_chroma(rtl_use_flag_u_B_tmep, rtl_use_idx_u_B, ctu_x + 1, ctu_y + i, 2, (i + 1) * 32);
                rtl_use_idx_u++;
                write_inf_2_rtl_chroma(rtl_use_flag_v_B_tmep, rtl_use_idx_v_B, ctu_x + 1, ctu_y + i, 2, (i + 1) * 32);
                rtl_use_idx_v++;
            }*/
        }
    }
    else if (3==first_col_flag)
    {
        for (int i = for_st; i < for_ed; ++i)
        {
#pragma HLS loop_tripcount max = 16
            write_inf_2_rtl_luma(rtl_use_flag_y_tmep, rtl_use_idx_y, ctu_x, ctu_y + i, 1, (i + 1) * 64);
            rtl_use_idx_y++;
            write_inf_2_rtl_chroma(rtl_use_flag_u_tmep, rtl_use_idx_u, ctu_x, ctu_y + i, 1, (i + 1) * 32);
            rtl_use_idx_u++;
            write_inf_2_rtl_chroma(rtl_use_flag_v_tmep, rtl_use_idx_v, ctu_x, ctu_y + i, 1, (i + 1) * 32);
            rtl_use_idx_v++;

            /*if (1==b_frame_flag)
            {
                write_inf_2_rtl_luma(rtl_use_flag_y_B_tmep, rtl_use_idx_y_B, ctu_x, ctu_y + i, 1, (i + 1) * 64);
                rtl_use_idx_y_B++;
                write_inf_2_rtl_chroma(rtl_use_flag_u_B_tmep, rtl_use_idx_u_B, ctu_x, ctu_y + i, 1, (i + 1) * 32);
                rtl_use_idx_u++;
                write_inf_2_rtl_chroma(rtl_use_flag_v_B_tmep, rtl_use_idx_v_B, ctu_x, ctu_y + i, 1, (i + 1) * 32);
                rtl_use_idx_v++;
            }*/
        }
    }
    else
    {
        for (int i = for_st; i < for_ed; ++i)
        {
#pragma HLS loop_tripcount max = 16
            write_inf_2_rtl_luma(rtl_use_flag_y_tmep, rtl_use_idx_y, ctu_x, ctu_y + i, 3, (i + 1) * 64);
            rtl_use_idx_y++;
            write_inf_2_rtl_chroma(rtl_use_flag_u_tmep, rtl_use_idx_u, ctu_x, ctu_y + i, 3, (i + 1) * 32);
            rtl_use_idx_u++;
            write_inf_2_rtl_chroma(rtl_use_flag_v_tmep, rtl_use_idx_v, ctu_x, ctu_y + i, 3, (i + 1) * 32);
            rtl_use_idx_v++;

            /*if (1 == b_frame_flag)
            {
                write_inf_2_rtl_luma(rtl_use_flag_y_B_tmep, rtl_use_idx_y_B, ctu_x, ctu_y + i, 3, (i + 1) * 64);
                rtl_use_idx_y_B++;
                write_inf_2_rtl_chroma(rtl_use_flag_u_B_tmep, rtl_use_idx_u_B, ctu_x, ctu_y + i, 3, (i + 1) * 32);
                rtl_use_idx_u_B++;
                write_inf_2_rtl_chroma(rtl_use_flag_v_B_tmep, rtl_use_idx_v_B, ctu_x, ctu_y + i, 3, (i + 1) * 32);
            }*/
        }
    }
}

void rfc_dcp_dummy(
    pel  refWinY0_ref[192][64],//0
    pel  refWinY1_ref[192][64],//1
    pel  refWinY2_ref[192][64],//2
    pel  refWinY3_ref[192][64],//3
    pel  refWinU0_ref[96][32],//0
    pel  refWinU1_ref[96][32],//1
    pel  refWinU2_ref[96][32],//2
    pel  refWinU3_ref[96][32],//3
    pel  refWinV0_ref[96][32],//0
    pel  refWinV1_ref[96][32],//1
    pel  refWinV2_ref[96][32],//2
    pel  refWinV3_ref[96][32],//3
    /*
    pel  refWinY0_B_ref[192][64],//0
    pel  refWinY1_B_ref[192][64],//1
    pel  refWinY2_B_ref[192][64],//2
    pel  refWinY3_B_ref[192][64],//3
    pel  refWinU0_B_ref[96][32],//0
    pel  refWinU1_B_ref[96][32],//1
    pel  refWinU2_B_ref[96][32],//2
    pel  refWinU3_B_ref[96][32],//3
    pel  refWinV0_B_ref[96][32],//0
    pel  refWinV1_B_ref[96][32],//1
    pel  refWinV2_B_ref[96][32],//2
    pel  refWinV3_B_ref[96][32],//3
    */
    U32 rtl_use_flag_y[64],
    U32 rtl_use_flag_u[64],
    U32 rtl_use_flag_v[64]
    /*
    U32 rtl_use_flag_y_B[64],
    U32 rtl_use_flag_u_B[64],
    U32 rtl_use_flag_v_B[64]*/
    )
{
#pragma HLS inline off
    int dummy_flag_y = 0;
    int dummy_flag_u = 0;
    int dummy_flag_v = 0;

    // int dummy_flag_y_B = 0;
    // int dummy_flag_u_B = 0;
    // int dummy_flag_v_B = 0;

#define __ZP_CYCLE_OPT_IGNORE_INIT_
#ifdef __ZP_CYCLE_OPT_IGNORE_INIT_	
    int tt;
    int fake_one = 1;
L_ZP1: for (tt = 0; tt < fake_one; tt++) {
#pragma HLS loop_tripcount max=0
#endif // __ZP_CYCLE_OPT_IGNORE_INIT_
    for (int i = 0; i < 8; ++i)
    {
#pragma HLS pipeline
#pragma HLS LOOP_TRIPCOUNT max=0
        dummy_flag_y = dummy_flag_y + rtl_use_flag_y[i];
        dummy_flag_u = dummy_flag_y + rtl_use_flag_u[i];
        dummy_flag_v = dummy_flag_y + rtl_use_flag_v[i];

        // dummy_flag_y_B = dummy_flag_y_B + rtl_use_flag_y_B[i];
        // dummy_flag_u_B = dummy_flag_y_B + rtl_use_flag_u_B[i];
        // dummy_flag_v_B = dummy_flag_y_B + rtl_use_flag_v_B[i];
    }
    for (int i = 0; i < 192; ++i)
    {
#pragma HLS pipeline
        for (int j = 0; j < 64; ++j)
        {
#pragma HLS unroll
#pragma HLS LOOP_TRIPCOUNT max=0
            refWinY0_ref[i][j] = i * j+dummy_flag_y;
            refWinY1_ref[i][j] = i * j * 2;
            refWinY2_ref[i][j] = i * j * 4;
            refWinY3_ref[i][j] = i * j * 8;

            // refWinY0_B_ref[i][j] = i * j * 16+dummy_flag_y_B;
            // refWinY1_B_ref[i][j] = i * j * 32;
            // refWinY2_B_ref[i][j] = i * j * 64;
            // refWinY3_B_ref[i][j] = i * j * 128;

        }
    }
    for (int i = 0; i < 96; ++i)
    {
#pragma HLS pipeline
        for (int j = 0; j < 32; ++j)
        {
#pragma HLS unroll
#pragma HLS LOOP_TRIPCOUNT max=0
            refWinU0_ref[i][j] = i * j * 1+dummy_flag_u;
            refWinU1_ref[i][j] = i * j * 2;
            refWinU2_ref[i][j] = i * j * 4;
            refWinU3_ref[i][j] = i * j * 8;

            refWinV0_ref[i][j] = i * j * 16+dummy_flag_v;
            refWinV1_ref[i][j] = i * j * 32;
            refWinV2_ref[i][j] = i * j * 64;
            refWinV3_ref[i][j] = i * j * 128;

            // refWinU0_B_ref[i][j] = i * j * 256+dummy_flag_u_B;
            // refWinU1_B_ref[i][j] = i * j * 512;
            // refWinU2_B_ref[i][j] = i * j * 1024;
            // refWinU3_B_ref[i][j] = i * j * 2048;

            // refWinV0_B_ref[i][j] = i * j * 4096+dummy_flag_v_B;
            // refWinV1_B_ref[i][j] = i * j * 8192;
            // refWinV2_B_ref[i][j] = i * j * 16384;
            // refWinV3_B_ref[i][j] = i * j * 32768;

        }
    }
#ifdef __ZP_CYCLE_OPT_IGNORE_INIT_	
}
#endif  //__ZP_CYCLE_OPT_IGNORE_INIT_	
return;
}
#endif

static void firmware_to_dbk_aec(AEC_FW* fetch2dbk_firmware,AEC_FW* fetch2aec_firmware,CTX_FETCH* fetch_ctx_io,U8 ctx_rpm_num_refp_0,
    U8 ctx_rpm_num_refp_1)
{
    fetch2dbk_firmware->umve_enable_flag = fetch_ctx_io->umve_enable_flag;
	fetch2dbk_firmware->num_of_hmvp_cand = fetch_ctx_io->num_of_hmvp_cand;
	fetch2dbk_firmware->amvr_enable_flag = fetch_ctx_io->amvr_enable_flag;
	fetch2dbk_firmware->emvr_enable_flag = fetch_ctx_io->emvr_enable_flag;
	fetch2dbk_firmware->dt_intra_enable_flag =fetch_ctx_io->dt_intra_enable_flag;
	fetch2dbk_firmware->position_based_transform_enable_flag = fetch_ctx_io->position_based_transform_enable_flag;
	fetch2dbk_firmware->tscpm_enable_flag = fetch_ctx_io->tscpm_enable_flag;
	fetch2dbk_firmware->ipf_enable_flag = fetch_ctx_io->ipf_enable_flag;
	fetch2dbk_firmware->sample_adaptive_offset_enable_flag = fetch_ctx_io->sample_adaptive_offset_enable_flag;
	fetch2dbk_firmware->pic_width_in_scu = fetch_ctx_io->ctx_info_pic_width_in_scu;
	fetch2dbk_firmware->max_cuwh = fetch_ctx_io->max_cuwh;
	fetch2dbk_firmware->pic_width = fetch_ctx_io->ctx_info_pic_width;
	fetch2dbk_firmware->pic_height = fetch_ctx_io->ctx_info_pic_height;
	fetch2dbk_firmware->slice_type = fetch_ctx_io->ctx_info_pic_header_slice_type;
	fetch2dbk_firmware->log2_culine = fetch_ctx_io->log2_culine;
	fetch2dbk_firmware->pic_width_in_lcu = fetch_ctx_io->ctx_info_pic_width_in_lcu;
	fetch2dbk_firmware->lcu_total_cnt = fetch_ctx_io->ctx_info_pic_width_in_lcu * fetch_ctx_io->ctx_info_pic_height_in_lcu;
	fetch2dbk_firmware->rpm_num_refp[0] = ctx_rpm_num_refp_0;
	fetch2dbk_firmware->rpm_num_refp[1] = ctx_rpm_num_refp_1;

	fetch2aec_firmware->umve_enable_flag = fetch_ctx_io->umve_enable_flag;
	fetch2aec_firmware->num_of_hmvp_cand = fetch_ctx_io->num_of_hmvp_cand;
	fetch2aec_firmware->amvr_enable_flag = fetch_ctx_io->amvr_enable_flag;
	fetch2aec_firmware->emvr_enable_flag = fetch_ctx_io->emvr_enable_flag;
	fetch2aec_firmware->dt_intra_enable_flag = fetch_ctx_io->dt_intra_enable_flag;
	fetch2aec_firmware->position_based_transform_enable_flag = fetch_ctx_io->position_based_transform_enable_flag;
	fetch2aec_firmware->tscpm_enable_flag = fetch_ctx_io->tscpm_enable_flag;
	fetch2aec_firmware->ipf_enable_flag = fetch_ctx_io->ipf_enable_flag;
	fetch2aec_firmware->sample_adaptive_offset_enable_flag = fetch_ctx_io->sample_adaptive_offset_enable_flag;
	fetch2aec_firmware->pic_width_in_scu = fetch_ctx_io->ctx_info_pic_width_in_scu;
	fetch2aec_firmware->max_cuwh = fetch_ctx_io->max_cuwh;
	fetch2aec_firmware->pic_width = fetch_ctx_io->ctx_info_pic_width;
	fetch2aec_firmware->pic_height = fetch_ctx_io->ctx_info_pic_height;
	fetch2aec_firmware->slice_type = fetch_ctx_io->ctx_info_pic_header_slice_type;
	fetch2aec_firmware->log2_culine = fetch_ctx_io->log2_culine;
	fetch2aec_firmware->pic_width_in_lcu = fetch_ctx_io->ctx_info_pic_width_in_lcu;
	fetch2aec_firmware->lcu_total_cnt = fetch_ctx_io->ctx_info_pic_width_in_lcu * fetch_ctx_io->ctx_info_pic_height_in_lcu;
	fetch2aec_firmware->rpm_num_refp[0] = ctx_rpm_num_refp_0;
	fetch2aec_firmware->rpm_num_refp[1] = ctx_rpm_num_refp_1;

    fetch2aec_firmware->affine_enable_flag = fetch_ctx_io->affine_enable_flag;
	fetch2aec_firmware->fixed_slice_qp_flag = fetch_ctx_io->fixed_slice_qp_flag;
	fetch2aec_firmware->cu_delta_qp_flag=fetch_ctx_io->cu_delta_qp_flag;
	fetch2aec_firmware->ph_ists_enable_flag=fetch_ctx_io->ph_ists_enable_flag;
	fetch2aec_firmware->ph_epmc_model_flag=fetch_ctx_io->ph_epmc_model_flag;
	fetch2aec_firmware->ibc_flag=fetch_ctx_io->ibc_flag;
	fetch2aec_firmware->sp_pic_flag=fetch_ctx_io->sp_pic_flag;
	fetch2aec_firmware->evs_ubvs_pic_flag=fetch_ctx_io->evs_ubvs_pic_flag;
	fetch2aec_firmware->umve_set_flag=fetch_ctx_io->umve_set_flag;
	fetch2aec_firmware->ph_ipc_flag=fetch_ctx_io->ph_ipc_flag;
	fetch2aec_firmware->cu_qp_group_area_size=fetch_ctx_io->cu_qp_group_area_size;
	fetch2aec_firmware->chroma_quant_param_delta_cb=fetch_ctx_io->ctx_info_pic_header_chroma_quant_param_delta_cb;
	fetch2aec_firmware->chroma_quant_param_delta_cr=fetch_ctx_io->ctx_info_pic_header_chroma_quant_param_delta_cr;
	fetch2aec_firmware->slice_qp=fetch_ctx_io->slice_qp;
	fetch2aec_firmware->min_cu_size=fetch_ctx_io->min_cu_size;
	fetch2aec_firmware->max_part_ratio=fetch_ctx_io->max_part_ratio;
	fetch2aec_firmware->max_split_times=fetch_ctx_io->max_split_times;
	fetch2aec_firmware->min_qt_size=fetch_ctx_io->min_qt_size;
	fetch2aec_firmware->max_bt_size=fetch_ctx_io->max_bt_size;
	fetch2aec_firmware->max_eqt_size=fetch_ctx_io->max_eqt_size;
	fetch2aec_firmware->max_dt_size=fetch_ctx_io->max_dt_size;
	fetch2aec_firmware->abvr_enable_flag=fetch_ctx_io->abvr_enable_flag;
	fetch2aec_firmware->umve_enh_enable_flag=fetch_ctx_io->umve_enh_enable_flag;
	fetch2aec_firmware->awp_enable_flag=fetch_ctx_io->awp_enable_flag;
	fetch2aec_firmware->awp_mvr_enable_flag=fetch_ctx_io->awp_mvr_enable_flag;
	fetch2aec_firmware->etmvp_enable_flag=fetch_ctx_io->etmvp_enable_flag;
	fetch2aec_firmware->affine_umve_enable_flag=fetch_ctx_io->affine_umve_enable_flag;
	fetch2aec_firmware->smvd_enable_flag=fetch_ctx_io->smvd_enable_flag;
	fetch2aec_firmware->bgc_enable_flag=fetch_ctx_io->bgc_enable_flag;
	fetch2aec_firmware->interpf_enable_flag=fetch_ctx_io->interpf_enable_flag;
	fetch2aec_firmware->ipc_enable_flag=fetch_ctx_io->ipc_enable_flag;
	fetch2aec_firmware->num_of_mvap_cand=fetch_ctx_io->num_of_mvap_cand;
	fetch2aec_firmware->num_of_hbvp_cand=fetch_ctx_io->num_of_hbvp_cand;
	fetch2aec_firmware->enhance_tscpm_enable_flag=fetch_ctx_io->enhance_tscpm_enable_flag;
	fetch2aec_firmware->pmc_enable_flag=fetch_ctx_io->pmc_enable_flag;
	fetch2aec_firmware->iip_enable_flag=fetch_ctx_io->iip_enable_flag;
	fetch2aec_firmware->sawp_enable_flag=fetch_ctx_io->sawp_enable_flag;
	fetch2aec_firmware->sbt_enable_flag=fetch_ctx_io->sbt_enable_flag;
	fetch2aec_firmware->ists_enable_flag=fetch_ctx_io->ists_enable_flag;
	fetch2aec_firmware->srcc_enable_flag=fetch_ctx_io->srcc_enable_flag;
	fetch2aec_firmware->eipm_enable_flag=fetch_ctx_io->eipm_enable_flag;
	fetch2aec_firmware->st_chroma_enable_flag=fetch_ctx_io->st_chroma_enable_flag;
	fetch2aec_firmware->est_enable_flag=fetch_ctx_io->est_enable_flag;
	fetch2aec_firmware->log2_max_cuwh=fetch_ctx_io->log2_max_cuwh;
	fetch2aec_firmware->bit_depth_input=fetch_ctx_io->bit_depth_input;
    fetch2aec_firmware->qp_offset_bit_depth=fetch_ctx_io->qp_offset_bit_depth;
	fetch2aec_firmware->ptr=fetch_ctx_io->ctx_ptr;
	fetch2aec_firmware->temporal_id=fetch_ctx_io->temporal_id;
	fetch2aec_firmware->dataCol=fetch_ctx_io->dataCol;
	fetch2aec_firmware->cons_pred_mode=fetch_ctx_io->cons_pred_mode;
    fetch2aec_firmware->tree_status=fetch_ctx_io->tree_status;
	fetch2aec_firmware->num_delta_qp=fetch_ctx_io->num_delta_qp;
	fetch2aec_firmware->pred_qp=fetch_ctx_io->pred_qp;
	fetch2aec_firmware->cu_qp_group_x=fetch_ctx_io->cu_qp_group_x;
	fetch2aec_firmware->cu_qp_group_y=fetch_ctx_io->cu_qp_group_y;

    for (int i = 0; i < 3; i++)
    {
#pragma HLS unroll
        fetch2dbk_firmware->slice_sao_enable[i] = fetch_ctx_io->slice_sao_enable[i];
        fetch2aec_firmware->slice_sao_enable[i] = fetch_ctx_io->slice_sao_enable[i];
    }

    for (int i = 0; i < MAX_NUM_REF_PICS; i++)
    {
#pragma HLS unroll
        fetch2aec_firmware->refp_ptr[i][0] = fetch_ctx_io->refp_ptr[i][0];
        fetch2aec_firmware->refp_ptr[i][1] = fetch_ctx_io->refp_ptr[i][1];
    }

    for (int i = 0; i < UMVE_REFINE_STEP_SEC_SET; i++)
    {
#pragma HLS unroll
        fetch2aec_firmware->umveOffsetPicCount[i] = fetch_ctx_io->umveOffsetPicCount[i];
    }

    for (int i = 0; i < 256; i++)
    {
#pragma HLS unroll factor=16
        fetch2aec_firmware->cu_delta_qp_lcu_map[i] = fetch_ctx_io->cu_delta_qp_lcu_map[i];
    }
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
)
{
#ifdef __SYNTHESIS__
    pel refWinY0_ref[192][64];//0
    pel refWinY1_ref[192][64];//1
    pel refWinY2_ref[192][64];//2
    pel refWinY3_ref[192][64];//3
    pel refWinU0_ref[96][32];//0
    pel refWinU1_ref[96][32];//1
    pel refWinU2_ref[96][32];//2
    pel refWinU3_ref[96][32];//3
    pel refWinV0_ref[96][32];//0
    pel refWinV1_ref[96][32];//1
    pel refWinV2_ref[96][32];//2
    pel refWinV3_ref[96][32];//3
    pel refWinY0_B_ref[192][64];//0
    pel refWinY1_B_ref[192][64];//1
    pel refWinY2_B_ref[192][64];//2
    pel refWinY3_B_ref[192][64];//3
    pel refWinU0_B_ref[96][32];//0
    pel refWinU1_B_ref[96][32];//1
    pel refWinU2_B_ref[96][32];//2
    pel refWinU3_B_ref[96][32];//3
    pel refWinV0_B_ref[96][32];//0
    pel refWinV1_B_ref[96][32];//1
    pel refWinV2_B_ref[96][32];//2
    pel refWinV3_B_ref[96][32];//3
#endif
/*//fetch_core to complete
#pragma HLS RESOURCE variable=refWinY0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=refWinY1 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=refWinY2 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=refWinY3 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=refWinU0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=refWinU1 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=refWinU2 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=refWinU3 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=refWinV0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=refWinV1 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=refWinV2 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=refWinV3 core=RAM_1P_BRAM

#pragma HLS RESOURCE variable=refWinY0_b core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=refWinY1_b core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=refWinY2_b core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=refWinY3_b core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=refWinU0_b core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=refWinU1_b core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=refWinU2_b core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=refWinU3_b core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=refWinV0_b core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=refWinV1_b core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=refWinV2_b core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=refWinV3_b core=RAM_1P_BRAM
*/
/*//origin node code
#pragma HLS INTERFACE m_axi port=imgY_org_buf num_read_outstanding=16 max_read_burst_length=8 depth=518400 offset=slave bundle=fetch_maxi
#pragma HLS INTERFACE m_axi port=imgU_org_buf num_read_outstanding=16 max_read_burst_length=8 depth=129600 offset=slave bundle=fetch_maxi
#pragma HLS INTERFACE m_axi port=imgV_org_buf num_read_outstanding=16 max_read_burst_length=8 depth=129600 offset=slave bundle=fetch_maxi
#pragma HLS INTERFACE m_axi port=imgY_ref_buf num_read_outstanding=16 max_read_burst_length=8 depth=631584 offset=slave bundle=fetch_maxi
#pragma HLS INTERFACE m_axi port=imgU_ref_buf num_read_outstanding=16 max_read_burst_length=8 depth=157896 offset=slave bundle=fetch_maxi
#pragma HLS INTERFACE m_axi port=imgV_ref_buf num_read_outstanding=16 max_read_burst_length=8 depth=157896 offset=slave bundle=fetch_maxi
#pragma HLS INTERFACE m_axi port=imgY_b_buf num_read_outstanding=16 max_read_burst_length=8 depth=631584 offset=slave bundle=fetch_maxi
#pragma HLS INTERFACE m_axi port=imgU_b_buf num_read_outstanding=16 max_read_burst_length=8 depth=157896 offset=slave bundle=fetch_maxi
#pragma HLS INTERFACE m_axi port=imgV_b_buf num_read_outstanding=16 max_read_burst_length=8 depth=157896 offset=slave bundle=fetch_maxi*/
    U32 org_width = fetch_ctx_io->ctx_info_pic_width; 
    U32 org_height = fetch_ctx_io->ctx_info_pic_height;
    if(ctu_ctrl_input->bit_depth_internal == 8)
    {
        image_align16(imgY_org_align16, Fetch_input_video_U8_ptr->imgY_org,
                org_width, org_height);
        image_align8(imgU_org_align16, Fetch_input_video_U8_ptr->imgU_org,
                org_width>>1, org_height>>1);
        image_align8(imgV_org_align16, Fetch_input_video_U8_ptr->imgV_org,
                org_width>>1, org_height>>1);
        align16_to_ap_data_8bit (imgY_org_buf, imgY_org_align16, org_width, 
                org_height);
        align8_to_ap_data_8bit (imgU_org_buf, imgU_org_align16, org_width>>1, 
                org_height>>1);
        align8_to_ap_data_8bit (imgV_org_buf, imgV_org_align16, org_width>>1, 
                org_height>>1);
    }
    else if(ctu_ctrl_input->bit_depth_internal == 10)
    {
        image_align64(imgY_org_align16, Fetch_input_video_U8_ptr->imgY_org,
                org_width, org_height);
        image_align32(imgU_org_align16, Fetch_input_video_U8_ptr->imgU_org,
                org_width>>1, org_height>>1);
        image_align32(imgV_org_align16, Fetch_input_video_U8_ptr->imgV_org,
                org_width>>1, org_height>>1);
        align64_to_ap_data_10bit (imgY_org_buf, imgY_org_align16, org_width, 
                org_height);
        align32_to_ap_data_10bit (imgU_org_buf, imgU_org_align16, org_width>>1, 
                org_height>>1);
        align32_to_ap_data_10bit (imgV_org_buf, imgV_org_align16, org_width>>1, 
                org_height>>1);
    }
#if DEBUG_CHECK
	if (((int)(fetch_ctx_io->ctx_info_pic_header_slice_type)) != SLICE_I)
	{
		image_align16(imgY_ref_align16, Fetch_input_video_U8_ptr->RefbufY,
			org_width + 288, org_height + 288);
		image_align16(imgU_ref_align16, Fetch_input_video_U8_ptr->RefbufU,
			(org_width >> 1) + 144, (org_height >> 1) + 144);
		image_align16(imgV_ref_align16, Fetch_input_video_U8_ptr->RefbufV,
			(org_width >> 1) + 144, (org_height >> 1) + 144);
	}

    if (((int)(fetch_ctx_io->ctx_info_pic_header_slice_type)) != SLICE_I)
    {
        align16_to_ap_data(imgY_ref_buf, imgY_ref_align16, org_width + 288,
            org_height + 288);
        align16_to_ap_data(imgU_ref_buf, imgU_ref_align16, (org_width >> 1) + 144,
            (org_height >> 1) + 144);
        align16_to_ap_data(imgV_ref_buf, imgV_ref_align16, (org_width >> 1) + 144,
            (org_height >> 1) + 144);
    }
#if ENABLE_BFRAME
    if (((int)(fetch_ctx_io->ctx_info_pic_header_slice_type)) != SLICE_I)
    {
        image_align16(imgY_b_align16, Fetch_input_video_U8_ptr->RefbufY1,
            org_width + 288, org_height + 288);
        image_align16(imgU_b_align16, Fetch_input_video_U8_ptr->RefbufU1,
            (org_width >> 1) + 144, (org_height >> 1) + 144);
        image_align16(imgV_b_align16, Fetch_input_video_U8_ptr->RefbufV1,
            (org_width >> 1) + 144, (org_height >> 1) + 144);

        align16_to_ap_data(imgY_b_buf, imgY_b_align16, org_width + 288,
            org_height + 288);
        align16_to_ap_data(imgU_b_buf, imgU_b_align16, (org_width >> 1) + 144,
            (org_height >> 1) + 144);
        align16_to_ap_data(imgV_b_buf, imgV_b_align16, (org_width >> 1) + 144,
            (org_height >> 1) + 144);
    }
#endif
#endif

    //get CTU num
    if (((int)(fetch_ctx_io->ctx_info_pic_header_slice_type)) != SLICE_I) 
    {
        int code_stru_temp = Fetch_input_video_U8_ptr->code_stru;
#ifndef __SYNTHESIS__
/******************************************get CTU num***************************************************************************************/
        int luma_width1 = fetch_ctx_io->ctx_pinter_refp_pic_width_luma;
        int luma_heigh1 = fetch_ctx_io->ctx_pinter_refp_pic_height_luma;
        int chroma_width1 = fetch_ctx_io->ctx_pinter_refp_pic_width_chroma;
        int chroma_heigh1 = fetch_ctx_io->ctx_pinter_refp_pic_height_chroma;

        int luma_mod_num_col1 = luma_width1 % 64;
        int luma_mod_num_row1 = luma_heigh1 % 64;
        int chroma_mod_num_col1 = chroma_width1 % 32;
        int chroma_mod_num_row1 = chroma_heigh1 % 32;
        int offset_luma_x1 = luma_mod_num_col1 ? 128 : 192;
        int offset_luma_y1 = luma_mod_num_row1 ? 64 : 128;

        int luma_width_padded = (fetch_ctx_io->ctx_pinter_refp_pic_width_luma + 63) / 64 * 64;
        int luma_height_padded = (fetch_ctx_io->ctx_pinter_refp_pic_height_luma + 63) / 64 * 64;
        int chroma_width_padded = (fetch_ctx_io->ctx_pinter_refp_pic_width_chroma + 31) / 32 * 32;
        int chroma_height_padded = (fetch_ctx_io->ctx_pinter_refp_pic_height_chroma + 31) / 32 * 32;

        int tmp_pix_y, tmp_pix_x;

        static int core_x_lcu_tmp_run, core_y_lcu_tmp_run;
        if (ctu_ctrl_input->slice_enable) 
        {
            core_x_lcu_tmp_run = 0;
            core_y_lcu_tmp_run = 0;
        }
        else 
        {
            core_x_lcu_tmp_run++;
        }
        
        int img_width = fetch_ctx_io->ctx_info_pic_width;
        int img_height = fetch_ctx_io->ctx_info_pic_height;
        int lcu_width = (img_width + 63) >> 6;

        if (core_x_lcu_tmp_run == lcu_width) 
        {
            core_y_lcu_tmp_run++;
            core_x_lcu_tmp_run = 0;
        }
        
        tmp_pix_y = core_y_lcu_tmp_run << fetch_ctx_io->log2_max_cuwh;
        tmp_pix_x = core_x_lcu_tmp_run << fetch_ctx_io->log2_max_cuwh; 
        get_ctu_data(Fetch_input_video_U8_ptr, fetch_ctx_io,ctx_rpm_num_refp_0,ctx_rpm_num_refp_1,
                     Fetch_Ref_window, code_stru_temp, 
                     tmp_pix_y, tmp_pix_x,luma_width1, luma_heigh1, chroma_width1, chroma_heigh1);
#endif
#ifdef __SYNTHESIS__
        get_ctu_from_rtl(fetch_ctx_io,code_stru_temp,ctu_ctrl_input,
                         rtl_use_flag_y,rtl_use_flag_u,rtl_use_flag_v
                         /*rtl_use_flag_y_B,rtl_use_flag_u_B,rtl_use_flag_v_B*/);

    rfc_dcp_dummy(
        refWinY0_ref,
        refWinY1_ref,
        refWinY2_ref,
        refWinY3_ref,

        refWinU0_ref,
        refWinU1_ref,
        refWinU2_ref,
        refWinU3_ref,

        refWinV0_ref,
        refWinV1_ref,
        refWinV2_ref,
        refWinV3_ref,

        // refWinY0_B_ref,
        // refWinY1_B_ref,
        // refWinY2_B_ref,
        // refWinY3_B_ref,

        // refWinU0_B_ref,
        // refWinU1_B_ref,
        // refWinU2_B_ref,
        // refWinU3_B_ref,

        // refWinV0_B_ref,
        // refWinV1_B_ref,
        // refWinV2_B_ref,
        // refWinV3_B_ref,
        rtl_use_flag_y,
        rtl_use_flag_u,
        rtl_use_flag_v
        // rtl_use_flag_y_B,
        // rtl_use_flag_u_B,
        // rtl_use_flag_v_B
    );
#endif//__SYNTHESIS__
    /************************************************************************************************************************************************/
    }

    fetch_core (
#if PATCH_DIVIDE_ENABLE
        multislice_mode,
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////////////
        map_cu_mode_inner_to_store,
        fetch2dbk_firmware,
        fetch2aec_firmware,
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
        fetch_ctx_io,
        ctx_rpm_num_refp_0,
        ctx_rpm_num_refp_1,
        //ctx_pinter_refp_0->map_mv,
        //ctx_pinter_refp_0->map_refi,
        //ctx_pinter_refp_0->list_ptr,
        ctx_map->map_mv,
        ctx_map->map_refi,
        ctx_map->map_scu,
        ctx_map->map_cu_mode,
        ctx_lcu_cnt,
        fetch_output, 
        fetch2md_fw_ptr, 
        fetch2dbk_fw_ptr, 
        fetch2sao_fw_ptr, 
        ctu_ctrl_input,
        Fetch_Orig_Lcu_IME,
        Fetch_Orig_Lcu,
        imgY_org_buf,
        imgU_org_buf,
        imgV_org_buf,
#if DEBUG_CHECK
        imgY_ref_buf,
        imgU_ref_buf,
        imgV_ref_buf,
#if ENABLE_BFRAME
        imgY_b_buf,
        imgU_b_buf,
        imgV_b_buf,
#endif
#endif

        refWinY0_ref,//0
        refWinY1_ref,//1
        refWinY2_ref,//2
        refWinY3_ref,//3
        refWinU0_ref,//0
        refWinU1_ref,//1
        refWinU2_ref,//2
        refWinU3_ref,//3
        refWinV0_ref,//0
        refWinV1_ref,//1
        refWinV2_ref,//2
        refWinV3_ref,//3

        refWinY0_B_ref,//0
        refWinY1_B_ref,//1
        refWinY2_B_ref,//2
        refWinY3_B_ref,//3
        refWinU0_B_ref,//0
        refWinU1_B_ref,//1
        refWinU2_B_ref,//2
        refWinU3_B_ref,//3
        refWinV0_B_ref,//0
        refWinV1_B_ref,//1
        refWinV2_B_ref,//2
        refWinV3_B_ref,//3

        Fetch_input_video_U8_ptr->code_stru,
        Fetch_Ref_Window_IME,
        Fetch_Ref_window,
        Fetch_Ref_window_md2chroma, 
	    skip_input_ptr,
	    new_above_nebs_line,
	    new_above_nebs_line_IME,
        new_above_nebs_line_IME_1,
	    me_line_map_ptr,
	    me_line_map_ptr_IME,
        me_line_map_ptr_IME_1,
        use_deblock,
	    real_lcu_width,
	    real_lcu_height
#if ENABLE_BFRAME
#if FETCH_COM_REF_DEBUG
        //, flag_valid_map_mv_1
        //, flag_valid_map_refi_1
        //, flag_valid_list_ptr_1
        //, flag_valid_map_mv_1
        //, flag_valid_map_refi_1
        //, flag_valid_list_ptr_1
        , ctx_pinter_refp_0->map_mv
        , ctx_pinter_refp_1->map_mv
        , ctx_pinter_refp_0->map_refi
        , ctx_pinter_refp_1->map_refi
        , list_ptr_0
        , list_ptr_1
        , mdreg
	    , fetch2md_mdreg
        //, refp_ptr_test
        //, is_library_picture
#else
        , ctx_pinter_refp_all
#endif // #if FETCH_COM_REF_DEBUG
#endif
#if Debug422
        ,chroma_format
#endif
    );
}

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
)
{
/**********************************pragma***************************************/
#pragma HLS RESOURCE variable=refWinY0_ref core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=refWinY1_ref core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=refWinY2_ref core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=refWinY3_ref core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=refWinY32x192_ref core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=refWinU0_ref core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=refWinU1_ref core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=refWinU2_ref core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=refWinU3_ref core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=refWinU16x96_ref core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=refWinV0_ref core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=refWinV1_ref core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=refWinV2_ref core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=refWinV3_ref core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=refWinV16x96_ref core=RAM_1P_BRAM

#pragma HLS array_reshape variable=refWinY0_ref complete dim=2
#pragma HLS array_reshape variable=refWinY1_ref complete dim=2
#pragma HLS array_reshape variable=refWinY2_ref complete dim=2
#pragma HLS array_reshape variable=refWinY3_ref complete dim=2
#pragma HLS array_reshape variable=refWinY32x192_ref complete dim=2
#pragma HLS array_reshape variable=refWinU0_ref complete dim=2
#pragma HLS array_reshape variable=refWinU1_ref complete dim=2
#pragma HLS array_reshape variable=refWinU2_ref complete dim=2
#pragma HLS array_reshape variable=refWinU3_ref complete dim=2
#pragma HLS array_reshape variable=refWinU16x96_ref complete dim=2
#pragma HLS array_reshape variable=refWinV0_ref complete dim=2
#pragma HLS array_reshape variable=refWinV1_ref complete dim=2
#pragma HLS array_reshape variable=refWinV2_ref complete dim=2
#pragma HLS array_reshape variable=refWinV3_ref complete dim=2
#pragma HLS array_reshape variable=refWinV16x96_ref complete dim=2

#pragma HLS RESOURCE variable=refWinY0_B_ref core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=refWinY1_B_ref core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=refWinY2_B_ref core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=refWinY3_B_ref core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=refWinY32x192_B_ref core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=refWinU0_B_ref core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=refWinU1_B_ref core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=refWinU2_B_ref core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=refWinU3_B_ref core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=refWinU16x96_B_ref core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=refWinV0_B_ref core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=refWinV1_B_ref core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=refWinV2_B_ref core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=refWinV3_B_ref core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=refWinV16x96_B_ref core=RAM_1P_BRAM

#pragma HLS array_reshape variable=refWinY0_B_ref complete dim=2
#pragma HLS array_reshape variable=refWinY1_B_ref complete dim=2
#pragma HLS array_reshape variable=refWinY2_B_ref complete dim=2
#pragma HLS array_reshape variable=refWinY3_B_ref complete dim=2
#pragma HLS array_reshape variable=refWinY32x192_B_ref complete dim=2
#pragma HLS array_reshape variable=refWinU0_B_ref complete dim=2
#pragma HLS array_reshape variable=refWinU1_B_ref complete dim=2
#pragma HLS array_reshape variable=refWinU2_B_ref complete dim=2
#pragma HLS array_reshape variable=refWinU3_B_ref complete dim=2
#pragma HLS array_reshape variable=refWinU16x96_B_ref complete dim=2
#pragma HLS array_reshape variable=refWinV0_B_ref complete dim=2
#pragma HLS array_reshape variable=refWinV1_B_ref complete dim=2
#pragma HLS array_reshape variable=refWinV2_B_ref complete dim=2
#pragma HLS array_reshape variable=refWinV3_B_ref complete dim=2
#pragma HLS array_reshape variable=refWinV16x96_B_ref complete dim=2

//#pragma HLS RESOURCE variable=rtl_use_flag core=RAM_1P_BRAM
//#pragma HLS array_partition variable=rtl_use_flag complete dim=0
//#pragma HLS INTERFACE s_axilite port=return bundle=BUS_ALL   
#pragma HLS INTERFACE m_axi port=imgY_org_ap num_read_outstanding=16 max_read_burst_length=8 depth=518400 offset=slave bundle=fetch_maxi128 latency=40
#pragma HLS INTERFACE m_axi port=imgU_org_ap num_read_outstanding=16 max_read_burst_length=8 depth=129600 offset=slave bundle=fetch_maxi64 latency=40
#pragma HLS INTERFACE m_axi port=imgV_org_ap num_read_outstanding=16 max_read_burst_length=8 depth=129600 offset=slave bundle=fetch_maxi64 latency=40

#if DEBUG_CHECK
#pragma HLS INTERFACE m_axi port=imgY_ref_ap num_read_outstanding=16 max_read_burst_length=8 depth=631584 offset=slave bundle=fetch_maxi
#pragma HLS INTERFACE m_axi port=imgU_ref_ap num_read_outstanding=16 max_read_burst_length=8 depth=157896 offset=slave bundle=fetch_maxi
#pragma HLS INTERFACE m_axi port=imgV_ref_ap num_read_outstanding=16 max_read_burst_length=8 depth=157896 offset=slave bundle=fetch_maxi
#endif

#pragma HLS INTERFACE m_axi port=map_scu        offset=slave bundle=m_axi_map depth=6240
#pragma HLS INTERFACE m_axi port=map_mv         offset=slave bundle=m_axi_map depth=24960
#pragma HLS INTERFACE m_axi port=map_refi       offset=slave bundle=m_axi_map depth=12480
#pragma HLS INTERFACE m_axi port=map_cu_mode    offset=slave bundle=m_axi_map depth=6240
//#pragma HLS INTERFACE m_axi port=map_mv_ref         offset=slave bundle=m_axi_map1 depth=24960 num_read_outstanding=16 max_read_burst_length=16 
//#pragma HLS INTERFACE m_axi port=map_refi_ref       offset=slave bundle=m_axi_map2 depth=12480 num_read_outstanding=16 max_read_burst_length=16 
//#pragma HLS INTERFACE m_axi port=list_ptr    offset=slave bundle=m_axi_map depth=32
#pragma HLS INTERFACE m_axi port=ctx_pinter_refp_map_mv_0  depth=24960 offset=slave bundle=m_axi_map3 num_read_outstanding=16 max_read_burst_length=16 
#pragma HLS INTERFACE m_axi port=ctx_pinter_refp_map_mv_1  depth=24960 offset=slave bundle=m_axi_map4 num_read_outstanding=16 max_read_burst_length=16 
#pragma HLS INTERFACE m_axi port=ctx_pinter_refp_map_refi_0  depth=12480 offset=slave bundle=m_axi_map5 num_read_outstanding=16 max_read_burst_length=16 
#pragma HLS INTERFACE m_axi port=ctx_pinter_refp_map_refi_1  depth=12480 offset=slave bundle=m_axi_map6 num_read_outstanding=16 max_read_burst_length=16 
#pragma HLS INTERFACE m_axi port=list_ptr_0  depth=32 offset=slave bundle=m_axi_map1 num_read_outstanding=16 max_read_burst_length=16 
#pragma HLS INTERFACE m_axi port=list_ptr_1  depth=32 offset=slave bundle=m_axi_map2 num_read_outstanding=16 max_read_burst_length=16 

// fetch_output
#pragma HLS INTERFACE axis port=fetch_output->pix_x 
#pragma HLS INTERFACE axis port=fetch_output->pix_y
#pragma HLS INTERFACE axis port=fetch_output->rmd_pix_x 
#pragma HLS INTERFACE axis port=fetch_output->rmd_pix_y
#pragma HLS INTERFACE axis port=fetch_output->dbk_pix_x 
#pragma HLS INTERFACE axis port=fetch_output->dbk_pix_y
#pragma HLS INTERFACE axis port=fetch_output->sao_pix_x 
#pragma HLS INTERFACE axis port=fetch_output->sao_pix_y

//#pragma HLS INTERFACE axis port=fetch_output->lcu_cnt   // check??, to aec
//#pragma HLS INTERFACE axis port=fetch_output->lcu_num   // check??, to aec
#pragma HLS INTERFACE axis port=fetch_output->aec_lcu_cnt   // check??, to aec
#pragma HLS INTERFACE axis port=fetch_output->aec_lcu_num   // check??, to aec
#pragma HLS INTERFACE axis port=fetch_output->dbk_lcu_num   // check??, to aec

#pragma HLS INTERFACE axis port=fetch_output->qp_y 
#pragma HLS INTERFACE axis port=fetch_output->qp_u 
#pragma HLS INTERFACE axis port=fetch_output->qp_v 
#pragma HLS INTERFACE axis port=fetch_output->lambda_mv 
#pragma HLS INTERFACE axis port=fetch_output->rmd_lambda_mv 

#pragma HLS RESOURCE variable=fetch_output->lambda core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fetch_output->dist_chroma_weight core=RAM_1P_BRAM
#pragma HLS INTERFACE axis port=fetch_output->x_last_ctu 
#pragma HLS INTERFACE axis port=fetch_output->y_last_ctu 
//#pragma HLS INTERFACE axis port=fetch_output->delta_qp
#pragma HLS INTERFACE axis port=fetch_output->aec_delta_qp

#pragma HLS INTERFACE axis port=fetch_output->cu_dqp_enable 
#pragma HLS INTERFACE axis port=fetch_output->md_slice_qp 
#pragma HLS INTERFACE axis port=fetch_output->cu_qp_group_pred_qp 
#pragma HLS INTERFACE axis port=fetch_output->cu_qp_group_area_size
#pragma HLS RESOURCE variable=fetch_output->CuQPMap core=RAM_1P_BRAM

// fetch2md_fw_ptr
#pragma HLS INTERFACE axis port=fetch2md_fw_ptr->pic_width  
#pragma HLS INTERFACE axis port=fetch2md_fw_ptr->pic_height  
#pragma HLS INTERFACE axis port=fetch2md_fw_ptr->slice_type  
#pragma HLS INTERFACE axis port=fetch2md_fw_ptr->affine_enable_flag     //check
#pragma HLS INTERFACE axis port=fetch2md_fw_ptr->pic_width_in_scu  
#pragma HLS INTERFACE axis port=fetch2md_fw_ptr->pic_height_in_scu
#pragma HLS INTERFACE axis port=fetch2md_fw_ptr->log2_max_cuwh  
#pragma HLS INTERFACE axis port=fetch2md_fw_ptr->log2_culine
#pragma HLS INTERFACE axis port=fetch2md_fw_ptr->max_cuwh
#pragma HLS INTERFACE axis port=fetch2md_fw_ptr->max_part_ratio  
#pragma HLS INTERFACE axis port=fetch2md_fw_ptr->min_cu_size  
#pragma HLS INTERFACE axis port=fetch2md_fw_ptr->max_split_times  
#pragma HLS INTERFACE axis port=fetch2md_fw_ptr->min_qt_size  
#pragma HLS INTERFACE axis port=fetch2md_fw_ptr->max_bt_size  
#pragma HLS INTERFACE axis port=fetch2md_fw_ptr->max_eqt_size  
#pragma HLS RESOURCE variable=fetch2md_fw_ptr->refp_ptr core=RAM_1P_BRAM    // check, to ime
#pragma HLS RESOURCE variable=fetch2md_fw_ptr->refp_ptr_1 core=RAM_1P_BRAM
//#pragma HLS INTERFACE axis port=fetch2md_fw_ptr->num_refp
#pragma HLS RESOURCE variable=fetch2md_fw_ptr->num_refp core=RAM_1P_BRAM  
#pragma HLS INTERFACE axis port=fetch2md_fw_ptr->cur_ptr
#pragma HLS INTERFACE axis port=fetch2md_fw_ptr->bit_depth_internal
#pragma HLS INTERFACE axis port=fetch2md_fw_ptr->eipm_enable_flag
#pragma HLS INTERFACE axis port=fetch2md_fw_ptr->mipf_enable_flag
#pragma HLS INTERFACE axis port=fetch2md_fw_ptr->interpf_enable_flag
#pragma HLS INTERFACE axis port=fetch2md_fw_ptr->chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis port=fetch2md_fw_ptr->chroma_quant_param_delta_cr
#if UMVE_ENH
#pragma HLS INTERFACE axis port=fetch2md_fw_ptr->umve_set_flag
#endif

// fetch2dbk_fw_ptr
#pragma HLS INTERFACE axis port=fetch2dbk_fw_ptr->pic_width_in_lcu
#pragma HLS INTERFACE axis port=fetch2dbk_fw_ptr->pic_height_in_lcu
#pragma HLS INTERFACE axis port=fetch2dbk_fw_ptr->chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis port=fetch2dbk_fw_ptr->chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis port=fetch2dbk_fw_ptr->pic_width
#pragma HLS INTERFACE axis port=fetch2dbk_fw_ptr->pic_height
#pragma HLS INTERFACE axis port=fetch2dbk_fw_ptr->bit_depth
#pragma HLS INTERFACE axis port=fetch2dbk_fw_ptr->pic_width_in_lcu
#pragma HLS INTERFACE axis port=fetch2dbk_fw_ptr->qp_offset_bit_depth
#pragma HLS RESOURCE  variable=fetch2dbk_fw_ptr->ref_ptr_dbk core=RAM_1P_BRAM

// fetch2sao_fw_ptr
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->pic_width
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->pic_height
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->pic_width_in_scu
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->pic_width_in_lcu
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->pic_height_in_lcu
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->sample_adaptive_offset_enable_flag
#pragma HLS RESOURCE  variable=fetch2sao_fw_ptr->lambda core=RAM_1P_BRAM
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->stride_luma
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->stride_chroma
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->lcu_cnt
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->lcu_num   // check ??
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->max_cuwh
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->log2_max_cuwh
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->num_refp
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->log2_culine
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->slice_type
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->num_of_hmvp_cand
//#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->affine_enable_flag
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->amvr_enable_flag
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->emvr_enable_flag
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->dt_intra_enable_flag
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->position_based_transform_enable_flag
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->tscpm_enable_flag
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->ipf_enable_flag
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->cur_ptr
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->pix_x
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->pix_y
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->cu_width  // check ??
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->cu_height // check ??
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->bit_depth // check ??
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->qp_offset_bit_depth
//#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->mipf_enable_flag

// ctu_ctrl_input
//#pragma HLS INTERFACE s_axilite port=ctu_ctrl_input->width bundle=fetch_para
//#pragma HLS INTERFACE s_axilite port=ctu_ctrl_input->height bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=ctu_ctrl_input->f_lcu bundle=fetch_para
//#pragma HLS INTERFACE s_axilite port=ctu_ctrl_input->slice_type bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=ctu_ctrl_input->qp_y bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=ctu_ctrl_input->qp_u bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=ctu_ctrl_input->qp_v bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=ctu_ctrl_input->lambda_mv bundle=fetch_para
#pragma HLS INTERFACE m_axi port=ctu_ctrl_input->lambda offset=slave bundle=m_axi_map
//#pragma HLS INTERFACE m_axi port=ctu_ctrl_input->sqrt_lambda offset=slave bundle=m_axi_map
#pragma HLS INTERFACE m_axi port=ctu_ctrl_input->dist_chroma_weight offset=slave bundle=m_axi_map
//#pragma HLS INTERFACE s_axilite port=ctu_ctrl_input->ptr bundle=fetch_para
//#pragma HLS INTERFACE s_axilite port=ctu_ctrl_input->gop_size bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=ctu_ctrl_input->slice_enable bundle=fetch_para
//#pragma HLS INTERFACE s_axilite port=ctu_ctrl_input->num_of_hmvp_cand bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=ctu_ctrl_input->bit_depth_internal bundle=fetch_para
//#pragma HLS INTERFACE s_axilite port=ctu_ctrl_input->ipf_enable_flag bundle=fetch_para
//#pragma HLS INTERFACE s_axilite port=ctu_ctrl_input->max_part_ratio bundle=fetch_para
//#pragma HLS INTERFACE s_axilite port=ctu_ctrl_input->pic_width_in_scu bundle=fetch_para
//#pragma HLS INTERFACE s_axilite port=ctu_ctrl_input->pic_height_in_scu bundle=fetch_para
//#pragma HLS INTERFACE s_axilite port=ctu_ctrl_input->f_scu bundle=fetch_para
//#pragma HLS INTERFACE s_axilite port=ctu_ctrl_input->log2_max_cuwh bundle=fetch_para
//#pragma HLS INTERFACE s_axilite port=ctu_ctrl_input->min_cu_size bundle=fetch_para
//#pragma HLS INTERFACE s_axilite port=ctu_ctrl_input->max_split_times bundle=fetch_para
//#pragma HLS INTERFACE s_axilite port=ctu_ctrl_input->min_qt_size bundle=fetch_para
//#pragma HLS INTERFACE s_axilite port=ctu_ctrl_input->max_bt_size bundle=fetch_para
//#pragma HLS INTERFACE s_axilite port=ctu_ctrl_input->max_eqt_size bundle=fetch_para
//#pragma HLS INTERFACE s_axilite port=ctu_ctrl_input->affine_subblock_size_idx bundle=fetch_para
//#pragma HLS INTERFACE s_axilite port=ctu_ctrl_input->tscpm_enable_flag bundle=fetch_para
//#pragma HLS INTERFACE s_axilite port=ctu_ctrl_input->amvr_enable_flag bundle=fetch_para//?
//#pragma HLS INTERFACE s_axilite port=ctu_ctrl_input->emvr_enable_flag bundle=fetch_para
//#pragma HLS INTERFACE s_axilite port=ctu_ctrl_input->affine_enable_flag bundle=fetch_para
//#pragma HLS INTERFACE s_axilite port=ctu_ctrl_input->ipcm_enable_flag bundle=fetch_para
//#pragma HLS INTERFACE s_axilite port=ctu_ctrl_input->eipm_enable_flag bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=ctu_ctrl_input->mipf_enable_flag bundle=fetch_para
//#pragma HLS INTERFACE s_axilite port=ctu_ctrl_input->interpf_enable_flag bundle=fetch_para
//#pragma HLS INTERFACE s_axilite port=ctu_ctrl_input->fixed_slice_qp_flag bundle=fetch_para
//#pragma HLS INTERFACE axis port=ctu_ctrl_input->amvr_enable_flag
//#pragma HLS INTERFACE axis port=ctu_ctrl_input->emvr_enable_flag
//#pragma HLS INTERFACE axis port=ctu_ctrl_input->last_lcu_qp
//#pragma HLS INTERFACE axis port=ctu_ctrl_input->delta_qp
#pragma HLS INTERFACE s_axilite port=ctu_ctrl_input->depth bundle=fetch_para
//#pragma HLS INTERFACE s_axilite port=ctu_ctrl_input->slice_qp bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=ctu_ctrl_input->cu_dqp_enable bundle=fetch_para
//#pragma HLS INTERFACE s_axilite port=ctu_ctrl_input->cu_qp_group_area_size bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=ctu_ctrl_input->useCuQPMAP bundle=fetch_para
//#pragma HLS INTERFACE s_axilite port=ctu_ctrl_input->CuQPMap bundle=fetch_para
//#pragma HLS INTERFACE s_axilite port=ctu_ctrl_input->CuQPMap_Frame bundle=fetch_para
#pragma HLS INTERFACE m_axi port=ctu_ctrl_input->CuQPMap offset=slave bundle=m_axi_map
#pragma HLS INTERFACE m_axi port=ctu_ctrl_input->CuQPMap_Frame offset=slave bundle=m_axi_map

// skip_input_ptr
#pragma HLS RESOURCE variable=skip_input_ptr->refp_map_mv core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=skip_input_ptr->refp_map_refi core=RAM_1P_BRAM
#pragma HLS INTERFACE axis port=skip_input_ptr->slice_type
#pragma HLS INTERFACE axis port=skip_input_ptr->ptr
#pragma HLS INTERFACE axis port=skip_input_ptr->umve_enable_flag
#pragma HLS INTERFACE axis port=skip_input_ptr->pix_x
#pragma HLS RESOURCE variable=skip_input_ptr->refp_map_mv1 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=skip_input_ptr->refp_map_refi1 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=skip_input_ptr->refp_list_ptr1 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=skip_input_ptr->refp_ptr1 core=RAM_1P_BRAM

// new_above_nebs_line
#pragma HLS RESOURCE variable=new_above_nebs_line core=RAM_1P_BRAM

// new_above_nebs_line_IME
#pragma HLS RESOURCE variable=new_above_nebs_line_IME core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=new_above_nebs_line_IME_1 core=RAM_1P_BRAM

// me_line_map_ptr
#pragma HLS RESOURCE variable=me_line_map_ptr->new_map_scu_above_line core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_line_map_ptr->new_map_cu_mode_above_line core=RAM_1P_BRAM
#pragma HLS INTERFACE axis port=me_line_map_ptr->min_scu_x  
#pragma HLS INTERFACE axis port=me_line_map_ptr->min_scu_y  
#pragma HLS INTERFACE axis port=me_line_map_ptr->stride_in_map  //check
#pragma HLS INTERFACE axis port=me_line_map_ptr->stride_in_line  //check
#pragma HLS INTERFACE axis port=me_line_map_ptr->pic_width_in_scu  
#pragma HLS INTERFACE axis port=me_line_map_ptr->pic_height_in_scu  //check
#pragma HLS INTERFACE axis port=me_line_map_ptr->above_line_idx  
#pragma HLS INTERFACE axis port=me_line_map_ptr->curr_ctu_idx  
#pragma HLS INTERFACE axis port=me_line_map_ptr->x_ctu  //check
#pragma HLS INTERFACE axis port=me_line_map_ptr->y_ctu  //check
#pragma HLS RESOURCE variable=me_line_map_ptr->map_scu_inner core=RAM_1P_BRAM     //check
#pragma HLS RESOURCE variable=me_line_map_ptr->map_cu_mode_inner core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_line_map_ptr->co_left_mv core=RAM_1P_BRAM      //check ??
#pragma HLS RESOURCE variable=me_line_map_ptr->co_left_refi core=RAM_1P_BRAM    //check ??
#pragma HLS RESOURCE variable=me_line_map_ptr->co_left_ptr_ref core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=map_cu_mode_inner_to_store core=RAM_1P_BRAM

// me_line_map_ptr_IME
#pragma HLS RESOURCE variable=me_line_map_ptr_IME->new_map_scu_above_line core=RAM_1P_BRAM
#pragma HLS INTERFACE axis port=me_line_map_ptr_IME->min_scu_x
#pragma HLS RESOURCE variable=me_line_map_ptr_IME->co_left_mv core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_line_map_ptr_IME->co_left_refi core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_line_map_ptr_IME->co_left_ptr_ref core=RAM_1P_BRAM

#pragma HLS RESOURCE variable=me_line_map_ptr_IME_1->new_map_scu_above_line core=RAM_1P_BRAM
//#pragma HLS INTERFACE axis port=me_line_map_ptr_IME_1->min_scu_x
#pragma HLS RESOURCE variable=me_line_map_ptr_IME_1->co_left_mv core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_line_map_ptr_IME_1->co_left_refi core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_line_map_ptr_IME_1->co_left_ptr_ref core=RAM_1P_BRAM

//discrete variable
#pragma HLS INTERFACE s_axilite port=multislice_mode bundle=fetch_para
//#pragma HLS INTERFACE m_axi port=ctx_lcu_cnt offset=slave bundle=m_axi_map depth=6240
#pragma HLS INTERFACE s_axilite port=ctx_lcu_cnt  bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=code_stru bundle=fetch_para
#pragma HLS INTERFACE axis port=use_deblock
#pragma HLS INTERFACE axis port=real_lcu_width
#pragma HLS INTERFACE axis port=real_lcu_height
#pragma HLS INTERFACE s_axilite port=frame_level_fw.chroma_quant_param_delta_cb bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=frame_level_fw.qp_offset_bit_depth bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=frame_level_fw.chroma_quant_param_delta_cr bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=frame_level_fw.fixed_slice_qp_flag bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=frame_level_fw.pic_width bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=frame_level_fw.pic_height bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=ctx_rpm_num_refp_0 bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=ctx_rpm_num_refp_1 bundle=fetch_para

// Fetch_Ref_window 
#pragma HLS INTERFACE axis port=Fetch_Ref_window->code_stru
#pragma HLS RESOURCE variable=Fetch_Ref_window->CtrPos_ref1 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=Fetch_Ref_window->CtrPos core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=Fetch_Ref_window->lidx core=RAM_1P_BRAM
//#pragma HLS ARRAY_PARTITION variable=Fetch_Ref_window->lidx complete dim=0
#pragma HLS RESOURCE variable=Fetch_Ref_window->cur_refi core=RAM_1P_BRAM
//#pragma HLS ARRAY_PARTITION variable=Fetch_Ref_window->cur_refi complete dim=0
#pragma HLS RESOURCE variable=Fetch_Ref_window->refp_pic_ptr core=RAM_1P_BRAM
//#pragma HLS ARRAY_PARTITION variable=Fetch_Ref_window->refp_pic_ptr complete dim=0
#pragma HLS RESOURCE variable=Fetch_Ref_window->refp_is_library_picture core=RAM_1P_BRAM
//#pragma HLS ARRAY_PARTITION variable=Fetch_Ref_window->refp_is_library_picture complete dim=0

//#pragma HLS RESOURCE variable=Fetch_Ref_window->swOutLv0 core=RAM_1P_BRAM
//#pragma HLS RESOURCE variable=Fetch_Ref_window->swOutLv0_ref1 core=RAM_1P_BRAM
//#pragma HLS ARRAY_RESHAPE variable=Fetch_Ref_window->swOutLv0 complete dim=2
//#pragma HLS ARRAY_RESHAPE variable=Fetch_Ref_window->swOutLv0_ref1 complete dim=2
//#pragma HLS ARRAY_RESHAPE variable=Fetch_Ref_window->RefWindowLCU_UP cyclic factor=128 dim=1
//#pragma HLS ARRAY_RESHAPE variable=Fetch_Ref_window->RefWindowLCU_VP cyclic factor=128 dim=1
//#pragma HLS ARRAY_RESHAPE variable=Fetch_Ref_window->RefWindowLCU_UP_ref1 cyclic factor=128 dim=1
//#pragma HLS ARRAY_RESHAPE variable=Fetch_Ref_window->RefWindowLCU_VP_ref1 cyclic factor=128 dim=1

//Fetch_Ref_Window_IME
#pragma HLS RESOURCE variable=Fetch_Ref_Window_IME->CtrPos core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=Fetch_Ref_Window_IME->lidx core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=Fetch_Ref_Window_IME->cur_refi core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=Fetch_Ref_Window_IME->CtrPos_ref1 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=Fetch_Ref_Window_IME->swOutLv0 core=RAM_1P_BRAM
#pragma HLS ARRAY_PARTITION variable=Fetch_Ref_Window_IME->swOutLv0 complete dim=2
//#pragma HLS RESOURCE variable=Fetch_Ref_Window_IME->swOutLv0_ref1 core=RAM_1P_BRAM
//#pragma HLS ARRAY_PARTITION variable=Fetch_Ref_Window_IME->swOutLv0_ref1 complete dim=2

//Fetch_Ref_window_md2chroma
#pragma HLS INTERFACE axis port=Fetch_Ref_window_md2chroma->code_stru
#pragma HLS RESOURCE variable=Fetch_Ref_window_md2chroma->CtrPos core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=Fetch_Ref_window_md2chroma->CtrPos_ref1 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=Fetch_Ref_window_md2chroma->lidx core=RAM_1P_BRAM
#pragma HLS ARRAY_PARTITION variable=Fetch_Ref_window_md2chroma->lidx complete dim=0
#pragma HLS RESOURCE variable=Fetch_Ref_window_md2chroma->cur_refi core=RAM_1P_BRAM
#pragma HLS ARRAY_PARTITION variable=Fetch_Ref_window_md2chroma->cur_refi complete dim=0
#pragma HLS RESOURCE variable=Fetch_Ref_window_md2chroma->refp_pic_ptr core=RAM_1P_BRAM
#pragma HLS ARRAY_PARTITION variable=Fetch_Ref_window_md2chroma->refp_pic_ptr complete dim=0
#pragma HLS RESOURCE variable=Fetch_Ref_window_md2chroma->refp_is_library_picture core=RAM_1P_BRAM
#pragma HLS ARRAY_PARTITION variable=Fetch_Ref_window_md2chroma->refp_is_library_picture complete dim=0
#pragma HLS ARRAY_RESHAPE variable=Fetch_Ref_window_md2chroma->RefWindowLCU_UP cyclic factor=128 dim=1
#pragma HLS ARRAY_RESHAPE variable=Fetch_Ref_window_md2chroma->RefWindowLCU_VP cyclic factor=128 dim=1
//#pragma HLS ARRAY_RESHAPE variable=Fetch_Ref_window_md2chroma->RefWindowLCU_UP_ref1 cyclic factor=128 dim=1
//#pragma HLS ARRAY_RESHAPE variable=Fetch_Ref_window_md2chroma->RefWindowLCU_VP_ref1 cyclic factor=128 dim=1

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// fetch_ctx_io
//#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->ctx_pinter_refp_ptr bundle=fetch_para
#pragma HLS INTERFACE m_axi port=fetch_ctx_io->ctx_pinter_refp_ptr offset=slave bundle=m_axi_map
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->ctx_ptr bundle=fetch_para
//#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->ctx_rpm_num_refp bundle=fetch_para
//#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->ctx_pinter_refp_pic_width_chroma_all bundle=fetch_para
//#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->ctx_pinter_refp_pic_width_luma_all bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->ctx_pinter_refp_pic_height_chroma  bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->ctx_pinter_refp_pic_height_luma  bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->ctx_pinter_refp_pic_stride_chroma  bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->ctx_pinter_refp_pic_stride_luma  bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->ctx_pinter_refp_pic_width_chroma  bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->ctx_pinter_refp_pic_width_luma  bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->ctx_info_pic_width  bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->ctx_info_pic_height  bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->ctx_info_pic_width_in_lcu  bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->ctx_info_pic_height_in_lcu  bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->ctx_info_pic_width_in_scu  bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->ctx_info_pic_height_in_scu  bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->ctx_info_pic_header_chroma_quant_param_delta_cb  bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->ctx_info_pic_header_chroma_quant_param_delta_cr  bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->ctx_info_pic_header_slice_type  bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->sample_adaptive_offset_enable_flag bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->stride_chroma bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->stride_luma bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->lcu_cnt bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->flag_valid_map_mv     bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->flag_valid_map_refi   bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->flag_valid_list_ptr   bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->flag_valid_map_mv_1   bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->flag_valid_map_refi_1 bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->flag_valid_list_ptr_1 bundle=fetch_para
//#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->refp_ptr_test         bundle=fetch_para
#pragma HLS INTERFACE m_axi port=fetch_ctx_io->refp_ptr_test offset=slave bundle=m_axi_map

//#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->is_library_picture    bundle=fetch_para
#pragma HLS INTERFACE m_axi port=fetch_ctx_io->is_library_picture offset=slave bundle=m_axi_map
//#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->ctx_pinter_refp_pic_stride_luma_all bundle=fetch_para
//#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->ref_ptr_dbk bundle=fetch_para
#pragma HLS INTERFACE m_axi port=fetch_ctx_io->ref_ptr_dbk offset=slave bundle=m_axi_map

#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->affine_enable_flag bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->umve_enable_flag bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->num_of_hmvp_cand bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->amvr_enable_flag bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->emvr_enable_flag bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->dt_intra_enable_flag bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->position_based_transform_enable_flag bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->tscpm_enable_flag bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->ipf_enable_flag bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->max_cuwh bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->log2_culine bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->fixed_slice_qp_flag bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->cu_delta_qp_flag bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->ph_ists_enable_flag bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->ph_epmc_model_flag bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->ibc_flag bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->sp_pic_flag bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->evs_ubvs_pic_flag bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->umve_set_flag bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->ph_ipc_flag bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->cu_qp_group_area_size bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->slice_qp bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->min_cu_size bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->max_part_ratio bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->max_split_times bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->min_qt_size bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->max_bt_size bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->max_eqt_size bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->max_dt_size bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->abvr_enable_flag bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->umve_enh_enable_flag bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->awp_enable_flag bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->awp_mvr_enable_flag bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->etmvp_enable_flag bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->affine_umve_enable_flag bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->smvd_enable_flag bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->bgc_enable_flag bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->interpf_enable_flag bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->ipc_enable_flag bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->num_of_mvap_cand bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->num_of_hbvp_cand bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->enhance_tscpm_enable_flag bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->pmc_enable_flag bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->iip_enable_flag bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->sawp_enable_flag bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->sbt_enable_flag bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->ists_enable_flag bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->srcc_enable_flag bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->eipm_enable_flag bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->st_chroma_enable_flag bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->est_enable_flag bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->log2_max_cuwh bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->bit_depth_input bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->qp_offset_bit_depth bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->temporal_id bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->dataCol bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->cons_pred_mode bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->tree_status bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->num_delta_qp bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->pred_qp bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->cu_qp_group_x bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->cu_qp_group_y bundle=fetch_para
//#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->slice_sao_enable bundle=fetch_para
//#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->refp_ptr bundle=fetch_para
//#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->umveOffsetPicCount bundle=fetch_para
//#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->cu_delta_qp_lcu_map bundle=fetch_para

#pragma HLS INTERFACE m_axi port=fetch_ctx_io->slice_sao_enable offset=slave bundle=m_axi_map
#pragma HLS INTERFACE m_axi port=fetch_ctx_io->refp_ptr offset=slave bundle=m_axi_map
#pragma HLS INTERFACE m_axi port=fetch_ctx_io->umveOffsetPicCount offset=slave bundle=m_axi_map
#pragma HLS INTERFACE m_axi port=fetch_ctx_io->cu_delta_qp_lcu_map offset=slave bundle=m_axi_map

#pragma HLS INTERFACE s_axilite port=fetch_ctx_io->use_deblock bundle=fetch_para

//fetch2dbk_firmware
#pragma HLS INTERFACE axis port=fetch2dbk_firmware->umve_enable_flag
#pragma HLS INTERFACE axis port=fetch2dbk_firmware->num_of_hmvp_cand
#pragma HLS INTERFACE axis port=fetch2dbk_firmware->amvr_enable_flag
#pragma HLS INTERFACE axis port=fetch2dbk_firmware->emvr_enable_flag
#pragma HLS INTERFACE axis port=fetch2dbk_firmware->dt_intra_enable_flag
#pragma HLS INTERFACE axis port=fetch2dbk_firmware->position_based_transform_enable_flag
#pragma HLS INTERFACE axis port=fetch2dbk_firmware->tscpm_enable_flag
#pragma HLS INTERFACE axis port=fetch2dbk_firmware->ipf_enable_flag
#pragma HLS INTERFACE axis port=fetch2dbk_firmware->sample_adaptive_offset_enable_flag
#pragma HLS INTERFACE axis port=fetch2dbk_firmware->pic_width_in_scu
#pragma HLS INTERFACE axis port=fetch2dbk_firmware->max_cuwh
#pragma HLS INTERFACE axis port=fetch2dbk_firmware->pic_width
#pragma HLS INTERFACE axis port=fetch2dbk_firmware->pic_height
#pragma HLS INTERFACE axis port=fetch2dbk_firmware->slice_type
#pragma HLS INTERFACE axis port=fetch2dbk_firmware->log2_culine
#pragma HLS INTERFACE axis port=fetch2dbk_firmware->pic_width_in_lcu
#pragma HLS INTERFACE axis port=fetch2dbk_firmware->lcu_total_cnt
#pragma HLS RESOURCE variable=fetch2dbk_firmware->rpm_num_refp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fetch2dbk_firmware->slice_sao_enable core=RAM_1P_BRAM

//fetch2aec_firmware
#pragma HLS INTERFACE axis port=fetch2aec_firmware->affine_enable_flag
#pragma HLS INTERFACE axis port=fetch2aec_firmware->umve_enable_flag
#pragma HLS INTERFACE axis port=fetch2aec_firmware->num_of_hmvp_cand
#pragma HLS INTERFACE axis port=fetch2aec_firmware->amvr_enable_flag
#pragma HLS INTERFACE axis port=fetch2aec_firmware->emvr_enable_flag
#pragma HLS INTERFACE axis port=fetch2aec_firmware->dt_intra_enable_flag
#pragma HLS INTERFACE axis port=fetch2aec_firmware->position_based_transform_enable_flag
#pragma HLS INTERFACE axis port=fetch2aec_firmware->tscpm_enable_flag
#pragma HLS INTERFACE axis port=fetch2aec_firmware->ipf_enable_flag
#pragma HLS INTERFACE axis port=fetch2aec_firmware->sample_adaptive_offset_enable_flag
#pragma HLS INTERFACE axis port=fetch2aec_firmware->pic_width_in_scu
#pragma HLS INTERFACE axis port=fetch2aec_firmware->max_cuwh
#pragma HLS INTERFACE axis port=fetch2aec_firmware->pic_width
#pragma HLS INTERFACE axis port=fetch2aec_firmware->pic_height
#pragma HLS INTERFACE axis port=fetch2aec_firmware->slice_type
#pragma HLS INTERFACE axis port=fetch2aec_firmware->log2_culine
#pragma HLS INTERFACE axis port=fetch2aec_firmware->pic_width_in_lcu
#pragma HLS INTERFACE axis port=fetch2aec_firmware->lcu_total_cnt
#pragma HLS RESOURCE variable=fetch2aec_firmware->rpm_num_refp core=RAM_1P_BRAM
#pragma HLS INTERFACE axis port=fetch2aec_firmware->fixed_slice_qp_flag
#pragma HLS INTERFACE axis port=fetch2aec_firmware->cu_delta_qp_flag
#pragma HLS INTERFACE axis port=fetch2aec_firmware->ph_ists_enable_flag
#pragma HLS INTERFACE axis port=fetch2aec_firmware->ph_epmc_model_flag
#pragma HLS INTERFACE axis port=fetch2aec_firmware->ibc_flag
#pragma HLS INTERFACE axis port=fetch2aec_firmware->sp_pic_flag
#pragma HLS INTERFACE axis port=fetch2aec_firmware->evs_ubvs_pic_flag
#pragma HLS INTERFACE axis port=fetch2aec_firmware->umve_set_flag
#pragma HLS INTERFACE axis port=fetch2aec_firmware->ph_ipc_flag
#pragma HLS INTERFACE axis port=fetch2aec_firmware->cu_qp_group_area_size
#pragma HLS INTERFACE axis port=fetch2aec_firmware->chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis port=fetch2aec_firmware->chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis port=fetch2aec_firmware->slice_qp
#pragma HLS INTERFACE axis port=fetch2aec_firmware->min_cu_size
#pragma HLS INTERFACE axis port=fetch2aec_firmware->max_part_ratio
#pragma HLS INTERFACE axis port=fetch2aec_firmware->max_split_times
#pragma HLS INTERFACE axis port=fetch2aec_firmware->min_qt_size
#pragma HLS INTERFACE axis port=fetch2aec_firmware->max_bt_size
#pragma HLS INTERFACE axis port=fetch2aec_firmware->max_eqt_size
#pragma HLS INTERFACE axis port=fetch2aec_firmware->max_dt_size
#pragma HLS INTERFACE axis port=fetch2aec_firmware->abvr_enable_flag
#pragma HLS INTERFACE axis port=fetch2aec_firmware->umve_enh_enable_flag
#pragma HLS INTERFACE axis port=fetch2aec_firmware->awp_enable_flag
#pragma HLS INTERFACE axis port=fetch2aec_firmware->awp_mvr_enable_flag
#pragma HLS INTERFACE axis port=fetch2aec_firmware->etmvp_enable_flag
#pragma HLS INTERFACE axis port=fetch2aec_firmware->affine_umve_enable_flag
#pragma HLS INTERFACE axis port=fetch2aec_firmware->smvd_enable_flag
#pragma HLS INTERFACE axis port=fetch2aec_firmware->bgc_enable_flag
#pragma HLS INTERFACE axis port=fetch2aec_firmware->interpf_enable_flag
#pragma HLS INTERFACE axis port=fetch2aec_firmware->ipc_enable_flag
#pragma HLS INTERFACE axis port=fetch2aec_firmware->num_of_mvap_cand
#pragma HLS INTERFACE axis port=fetch2aec_firmware->num_of_hbvp_cand
#pragma HLS INTERFACE axis port=fetch2aec_firmware->enhance_tscpm_enable_flag
#pragma HLS INTERFACE axis port=fetch2aec_firmware->pmc_enable_flag
#pragma HLS INTERFACE axis port=fetch2aec_firmware->iip_enable_flag
#pragma HLS INTERFACE axis port=fetch2aec_firmware->sawp_enable_flag
#pragma HLS INTERFACE axis port=fetch2aec_firmware->sbt_enable_flag
#pragma HLS INTERFACE axis port=fetch2aec_firmware->ists_enable_flag
#pragma HLS INTERFACE axis port=fetch2aec_firmware->srcc_enable_flag
#pragma HLS INTERFACE axis port=fetch2aec_firmware->eipm_enable_flag
#pragma HLS INTERFACE axis port=fetch2aec_firmware->st_chroma_enable_flag
#pragma HLS INTERFACE axis port=fetch2aec_firmware->est_enable_flag
#pragma HLS INTERFACE axis port=fetch2aec_firmware->log2_max_cuwh
#pragma HLS INTERFACE axis port=fetch2aec_firmware->bit_depth_input
#pragma HLS INTERFACE axis port=fetch2aec_firmware->qp_offset_bit_depth
#pragma HLS INTERFACE axis port=fetch2aec_firmware->ptr
#pragma HLS INTERFACE axis port=fetch2aec_firmware->temporal_id
#pragma HLS INTERFACE axis port=fetch2aec_firmware->dataCol
#pragma HLS INTERFACE axis port=fetch2aec_firmware->cons_pred_mode
#pragma HLS INTERFACE axis port=fetch2aec_firmware->tree_status
#pragma HLS INTERFACE axis port=fetch2aec_firmware->num_delta_qp
#pragma HLS INTERFACE axis port=fetch2aec_firmware->pred_qp
#pragma HLS INTERFACE axis port=fetch2aec_firmware->cu_qp_group_x
#pragma HLS INTERFACE axis port=fetch2aec_firmware->cu_qp_group_y

#pragma HLS RESOURCE variable=fetch2aec_firmware->slice_sao_enable core=RAM_1P_BRAM
#pragma HLS array_partition variable=fetch2aec_firmware->slice_sao_enable complete dim=0

#pragma HLS RESOURCE variable=fetch2aec_firmware->refp_ptr core=RAM_1P_BRAM
#pragma HLS array_partition variable=fetch2aec_firmware->refp_ptr complete dim=0

#pragma HLS RESOURCE variable=fetch2aec_firmware->umveOffsetPicCount core=RAM_1P_BRAM
#pragma HLS array_partition variable=fetch2aec_firmware->umveOffsetPicCount complete dim=0

#pragma HLS RESOURCE variable=fetch2aec_firmware->cu_delta_qp_lcu_map core=RAM_1P_BRAM
#pragma HLS array_partition variable=fetch2aec_firmware->cu_delta_qp_lcu_map cyclic factor=16 dim=1

//---------------mdreg;fetch2md_mdreg-----------------//
#pragma HLS INTERFACE s_axilite port=mdreg->_sys bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=mdreg->_cond0 bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=mdreg->_cond1 bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=mdreg->_cfg0 bundle=fetch_para
#pragma HLS INTERFACE s_axilite port=mdreg->_cfg1 bundle=fetch_para

#pragma HLS INTERFACE axis port=fetch2md_mdreg->_sys
#pragma HLS INTERFACE axis port=fetch2md_mdreg->_cond0
#pragma HLS INTERFACE axis port=fetch2md_mdreg->_cond1
#pragma HLS INTERFACE axis port=fetch2md_mdreg->_cfg0
#pragma HLS INTERFACE axis port=fetch2md_mdreg->_cfg1

    fetch2md_mdreg->_sys = mdreg->_sys;
    fetch2md_mdreg->_cond0 = mdreg->_cond0;
    fetch2md_mdreg->_cond1 = mdreg->_cond1;
    fetch2md_mdreg->_cfg0 = mdreg->_cfg0;
    fetch2md_mdreg->_cfg1 = mdreg->_cfg1;

    firmware_to_dbk_aec(fetch2dbk_firmware,fetch2aec_firmware,fetch_ctx_io,
                        ctx_rpm_num_refp_0,ctx_rpm_num_refp_1);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int tmp_pix_x, tmp_pix_y, tmp_smb_num_width, tmp_smb_num_height;
    int lcu_x, lcu_y;

    static U12 lcu_cnt_tmp;

    firmware_parameters_get(
        &tmp_pix_x, 
        &tmp_pix_y, 
        &lcu_x,
        &lcu_y,
        &tmp_smb_num_width, 
        &tmp_smb_num_height, 
        fetch_output, 
        ctu_ctrl_input,
        fetch_ctx_io, 
        fetch2sao_fw_ptr
#if PATCH_DEBUG
        patch
#endif
    );

    use_deblock=(U2)(fetch_ctx_io->use_deblock);
	real_lcu_width = tmp_smb_num_width * 8;
	real_lcu_height = tmp_smb_num_height * 8;

    if (tmp_pix_x == 0 && tmp_pix_y == 0) 
    {
        lcu_cnt_tmp = (fetch_ctx_io->lcu_cnt) - 1;
    }
    else 
    {
        lcu_cnt_tmp--;
    }
    *ctx_lcu_cnt = lcu_cnt_tmp;
    fetch2sao_fw_ptr->lcu_cnt = lcu_cnt_tmp;
    fetch2sao_fw_ptr->pix_x = tmp_pix_x;
    fetch2sao_fw_ptr->pix_y = tmp_pix_y;

    lcu_fetch_org_pixels(
        &tmp_pix_x, 
        &tmp_pix_y, 
        &lcu_x,
        &lcu_y,
        &tmp_smb_num_width, 
        &tmp_smb_num_height,
        fetch_ctx_io->ctx_info_pic_width, 
        fetch_ctx_io->ctx_info_pic_height,
        Fetch_Orig_Lcu_IME,
        Fetch_Orig_Lcu, 
        ctu_ctrl_input,
        imgY_org_ap,
        imgU_org_ap,
        imgV_org_ap
#if Debug422
        ,(U16*)Fetch_input_video_U8_ptr->imgY_org,
        (U16*)Fetch_input_video_U8_ptr->imgU_org,
        (U16*)Fetch_input_video_U8_ptr->imgV_org,
        chroma_format
#endif
    );

    fetch2md(
        fetch2md_fw_ptr, 
        fetch2dbk_fw_ptr, 
        fetch2sao_fw_ptr, 
        ctu_ctrl_input, 
        fetch_ctx_io,
        ctx_rpm_num_refp_0,
        ctx_rpm_num_refp_1
    );


    fetch_line(
        &tmp_pix_x, 
        &tmp_pix_y, 
        fetch_ctx_io,
        map_mv,
        map_refi,
        map_scu,
        map_cu_mode,
        me_line_map_ptr, 
        me_line_map_ptr_IME,
        me_line_map_ptr_IME_1, 
        new_above_nebs_line, 
        new_above_nebs_line_IME,
        new_above_nebs_line_IME_1, 
        fetch_output,
        fetch_ctx_io->ctx_info_pic_height_in_lcu * fetch_ctx_io->ctx_info_pic_width_in_lcu - lcu_cnt_tmp - 1,
        map_cu_mode_inner_to_store
    );

    Fetch_Ref_window->code_stru = code_stru;
    Fetch_Ref_window_md2chroma->code_stru = code_stru;

    if (((int)(fetch_ctx_io->ctx_info_pic_header_slice_type)) != SLICE_I) 
    {
        
        int b_use_flag = 0;

        Fetch_Ref_window->lidx[REFP_0] = 0;
        Fetch_Ref_window->cur_refi[REFP_0] = 0;

        Fetch_Ref_window_md2chroma->lidx[REFP_0] = 0;
        Fetch_Ref_window_md2chroma->cur_refi[REFP_0] = 0;

        Fetch_Ref_Window_IME->lidx[REFP_0] = 0;
        Fetch_Ref_Window_IME->cur_refi[REFP_0] = 0;

        if (code_stru != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) && ctx_rpm_num_refp_0 > 1)
        {
            Fetch_Ref_window->lidx[REFP_1] = 0;
            Fetch_Ref_window->cur_refi[REFP_1] = 1;

            Fetch_Ref_window_md2chroma->lidx[REFP_1] = 0;
            Fetch_Ref_window_md2chroma->cur_refi[REFP_1] = 1;

            Fetch_Ref_Window_IME->lidx[REFP_1] = 0;
            Fetch_Ref_Window_IME->cur_refi[REFP_1] = 1;
        }
        else if (code_stru == RA && fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B)
        {
            Fetch_Ref_window->lidx[REFP_1] = 1;
            Fetch_Ref_window->cur_refi[REFP_1] = 0;

            Fetch_Ref_window_md2chroma->lidx[REFP_1] = 1;
            Fetch_Ref_window_md2chroma->cur_refi[REFP_1] = 0;

            Fetch_Ref_Window_IME->lidx[REFP_1] = 1;
            Fetch_Ref_Window_IME->cur_refi[REFP_1] = 0;
        }
        else
        {
            Fetch_Ref_window->lidx[REFP_1] = 0;
            Fetch_Ref_window->cur_refi[REFP_1] = -1;

            Fetch_Ref_window_md2chroma->lidx[REFP_1] = 0;
            Fetch_Ref_window_md2chroma->cur_refi[REFP_1] = -1;

            Fetch_Ref_Window_IME->lidx[REFP_1] = 0;
            Fetch_Ref_Window_IME->cur_refi[REFP_1] = -1;
        }

        lcu_fetch_ref_pixels(
            &tmp_pix_x, 
            &tmp_pix_y, 
            fetch_ctx_io,
            ctx_rpm_num_refp_0,
            ctx_rpm_num_refp_1,
            b_use_flag, 
#if DEBUG_CHECK 
            imgY_ref_ap,
            imgU_ref_ap,
            imgV_ref_ap,
#endif 
            Fetch_Ref_Window_IME->swOutLv0,
            Fetch_Ref_window_md2chroma->RefWindowLCU_UP,
            Fetch_Ref_window_md2chroma->RefWindowLCU_VP,

            refWinY32x192_ref,
            refWinY0_ref,
            refWinY1_ref,
            refWinY2_ref,
            refWinY3_ref,

            refWinU16x96_ref,
            refWinU0_ref,
            refWinU1_ref,
            refWinU2_ref,
            refWinU3_ref,

            refWinV16x96_ref,
            refWinV0_ref,
            refWinV1_ref,
            refWinV2_ref,
            refWinV3_ref
#if ENABLE_BFRAME
            , code_stru
#endif
        );
#ifndef __SYNTHESIS__
        if ((code_stru != RA && IS_INTER_SLICE(fetch_ctx_io->ctx_info_pic_header_slice_type) &&
             ctx_rpm_num_refp_0 > 1) || (code_stru == RA &&
                fetch_ctx_io->ctx_info_pic_header_slice_type == SLICE_B))
        {
            b_use_flag = 1;
            lcu_fetch_ref_pixels(
                &tmp_pix_x,
                &tmp_pix_y,
                fetch_ctx_io,
                ctx_rpm_num_refp_0,
                ctx_rpm_num_refp_1,
                b_use_flag,
#if DEBUG_CHECK 
                imgY_b_ap,
                imgU_b_ap,
                imgV_b_ap,
#endif 
#if ENABLE_BFRAME
                Fetch_Ref_Window_IME->swOutLv0_ref1,
                Fetch_Ref_window_md2chroma->RefWindowLCU_UP_ref1,
                Fetch_Ref_window_md2chroma->RefWindowLCU_VP_ref1,

                refWinY32x192_B_ref,
                refWinY0_B_ref,
                refWinY1_B_ref,
                refWinY2_B_ref,
                refWinY3_B_ref,

                refWinU16x96_B_ref,
                refWinU0_B_ref,
                refWinU1_B_ref,
                refWinU2_B_ref,
                refWinU3_B_ref,

                refWinV16x96_B_ref,
                refWinV0_B_ref,
                refWinV1_B_ref,
                refWinV2_B_ref,
                refWinV3_B_ref
#endif			
#if ENABLE_BFRAME
                , code_stru
#endif
            );
        }
#endif
        set_control_points(
#if PATCH_DIVIDE_ENABLE
            multislice_mode,
#endif
            &tmp_pix_x, 
            &tmp_pix_y, 
            fetch_ctx_io,
            ctx_rpm_num_refp_0,
            ctx_rpm_num_refp_1,
            Fetch_Ref_window,
            Fetch_Ref_window_md2chroma,
            Fetch_Ref_Window_IME
        );

        /*set_control_points_IME(
#if PATCH_DIVIDE_ENABLE
            multislice_mode,
#endif
            &tmp_pix_x, 
            &tmp_pix_y, 
            fetch_ctx_io, 
            ctx_rpm_num_refp_0,
            ctx_rpm_num_refp_1,
            Fetch_Ref_Window_IME
        );*/

        fetch_co_left_line(
            fetch_ctx_io, 
            me_line_map_ptr, 
            me_line_map_ptr_IME,
            me_line_map_ptr_IME_1, 
            ctx_pinter_refp_map_mv_0,
            ctx_pinter_refp_map_refi_0,
            list_ptr_0,
            &tmp_pix_x, 
            &tmp_pix_y
        );

        fetch_skip_merge(
            fetch_ctx_io, 
            ctu_ctrl_input,
            skip_input_ptr, 
            //map_mv_ref,
            //map_refi_ref,
            &tmp_pix_x, 
            &tmp_pix_y
#if ENABLE_BFRAME
#if FETCH_COM_REF_DEBUG
            //,flag_valid
            , fetch_ctx_io->flag_valid_map_mv
            , fetch_ctx_io->flag_valid_map_refi
            , fetch_ctx_io->flag_valid_list_ptr
            , fetch_ctx_io->flag_valid_map_mv_1
            , fetch_ctx_io->flag_valid_map_refi_1
            , fetch_ctx_io->flag_valid_list_ptr_1
            , ctx_pinter_refp_map_mv_0
            , ctx_pinter_refp_map_mv_1
            , ctx_pinter_refp_map_refi_0
            , ctx_pinter_refp_map_refi_1
            , list_ptr_0
            , list_ptr_1
            //, ptr_0
            //, ptr_1
            , fetch_ctx_io->refp_ptr_test
            //, code_stru
#else
			, ctx_pinter_refp_all
#endif  // #if FETCH_COM_REF_DEBUG
#endif
        );

#if ENABLE_BFRAME
		for (int i = 0; i < MAX_NUM_REF_PICS; ++i)
		{
//#pragma HLS pipeline
#pragma HLS unroll
#if FETCH_COM_REF_DEBUG
			Fetch_Ref_window->refp_pic_ptr[i][REFP_0] = fetch_ctx_io->refp_ptr_test[i][0];
			Fetch_Ref_window->refp_pic_ptr[i][REFP_1] = fetch_ctx_io->refp_ptr_test[i][1];
			Fetch_Ref_window->refp_is_library_picture[i][REFP_0] = fetch_ctx_io->is_library_picture[i][0];
			Fetch_Ref_window->refp_is_library_picture[i][REFP_1] = fetch_ctx_io->is_library_picture[i][1];

            Fetch_Ref_window_md2chroma->refp_pic_ptr[i][REFP_0] = fetch_ctx_io->refp_ptr_test[i][0];
			Fetch_Ref_window_md2chroma->refp_pic_ptr[i][REFP_1] = fetch_ctx_io->refp_ptr_test[i][1];
			Fetch_Ref_window_md2chroma->refp_is_library_picture[i][REFP_0] = fetch_ctx_io->is_library_picture[i][0];
			Fetch_Ref_window_md2chroma->refp_is_library_picture[i][REFP_1] = fetch_ctx_io->is_library_picture[i][1];
#else
            Fetch_Ref_window->refp_pic_ptr[i][REFP_0] = ctx_pinter_refp_all[i][REFP_0].ptr;
            Fetch_Ref_window->refp_pic_ptr[i][REFP_1] = ctx_pinter_refp_all[i][REFP_1].ptr;
            Fetch_Ref_window->refp_is_library_picture[i][REFP_0] = ctx_pinter_refp_all[i][REFP_0].is_library_picture;
            Fetch_Ref_window->refp_is_library_picture[i][REFP_1] = ctx_pinter_refp_all[i][REFP_1].is_library_picture;

            Fetch_Ref_window_md2chroma->refp_pic_ptr[i][REFP_0] = ctx_pinter_refp_all[i][REFP_0].ptr;
            Fetch_Ref_window_md2chroma->refp_pic_ptr[i][REFP_1] = ctx_pinter_refp_all[i][REFP_1].ptr;
            Fetch_Ref_window_md2chroma->refp_is_library_picture[i][REFP_0] = ctx_pinter_refp_all[i][REFP_0].is_library_picture;
            Fetch_Ref_window_md2chroma->refp_is_library_picture[i][REFP_1] = ctx_pinter_refp_all[i][REFP_1].is_library_picture;
#endif  // #if FETCH_COM_REF_DEBUG
		}
#endif
    }

    fetch_output->pix_x = (U12)tmp_pix_x;
    fetch_output->pix_y = (U12)tmp_pix_y;
    fetch_output->rmd_pix_x = (U12)tmp_pix_x;
    fetch_output->rmd_pix_y = (U12)tmp_pix_y;
    fetch_output->dbk_pix_x = (U12)tmp_pix_x;
    fetch_output->dbk_pix_y = (U12)tmp_pix_y;
    fetch_output->sao_pix_x = (U12)tmp_pix_x;
    fetch_output->sao_pix_y = (U12)tmp_pix_y;

    skip_input_ptr->umve_enable_flag = 1;
    skip_input_ptr->pix_x = tmp_pix_x;
    skip_input_ptr->slice_type = fetch_ctx_io->ctx_info_pic_header_slice_type;
    skip_input_ptr->ptr = fetch_ctx_io->ctx_ptr;

}
#else
//remove
#endif
// align image to 16-bytes boundary
void image_align16 (pel* dst, pel* src, U32 src_width, U32 src_height) {

    U32 width_padded = (src_width + 15) / 16 * 16;

    pel* linebuf = (pel *)calloc(width_padded, sizeof(pel));

    if (linebuf == NULL) {
        printf("unable to allocate memory for linebuf, quit.\n");
        exit(-1);
    }

    for (int i=0; i<src_height; i++) {
        memcpy(linebuf, src, src_width*sizeof(pel));
        memcpy(dst, linebuf, width_padded*sizeof(pel));
        src += src_width;
        dst += width_padded;
    }
    free(linebuf);
}

void image_align8 (pel* dst, pel* src, U32 src_width, U32 src_height) {

    U32 width_padded = (src_width + 7) / 8 * 8;

    pel* linebuf = (pel *)calloc(width_padded, sizeof(pel));

    if (linebuf == NULL) {
        printf("unable to allocate memory for linebuf, quit.\n");
        exit(-1);
    }

    for (int i=0; i<src_height; i++) {
        memcpy(linebuf, src, src_width*sizeof(pel));
        memcpy(dst, linebuf, width_padded*sizeof(pel));
        src += src_width;
        dst += width_padded;
    }
    free(linebuf);
}

void image_align32(pel* dst, pel* src, U32 src_width, U32 src_height) {

    U32 width_padded = (src_width + 31) / 32 * 32;

    pel* linebuf = (pel*)calloc(width_padded, sizeof(pel));

    if (linebuf == NULL) {
        printf("unable to allocate memory for linebuf, quit.\n");
        exit(-1);
    }

    for (int i = 0; i < src_height; i++) {
        memcpy(linebuf, src, src_width * sizeof(pel));
        memcpy(dst, linebuf, width_padded * sizeof(pel));
        src += src_width;
        dst += width_padded;
    }
    free(linebuf);
}
void image_align64(pel* dst, pel* src, U32 src_width, U32 src_height) {

    U32 width_padded = (src_width + 63) / 64 * 64;

    pel* linebuf = (pel*)calloc(width_padded, sizeof(pel));

    if (linebuf == NULL) {
        printf("unable to allocate memory for linebuf, quit.\n");
        exit(-1);
    }

    for (int i = 0; i < src_height; i++) {
        memcpy(linebuf, src, src_width * sizeof(pel));
        memcpy(dst, linebuf, width_padded * sizeof(pel));
        src += src_width;
        dst += width_padded;
    }
    free(linebuf);
}
void align16_to_ap_data_8bit (uint128_t* dst, pel* src, U32 src_width, U32 src_height) {

	U32 width_padded = (src_width + 15) / 16 * 16; // src_width aligned to ac_uint<128> boundary already

    pel* linebuf = (pel *)calloc(width_padded, sizeof(pel));
    uint128_t* tmp_dst = dst;

    if (linebuf == NULL) {
        printf("unable to allocate memory for linebuf, quit.\n");
        exit(-1);
    }

    for (int i=0; i<src_height; i++) {
        memcpy(linebuf, src, width_padded*sizeof(pel));
        for (int j=0; j<width_padded/16;j++) {
            for (int k=0;k<16;k++) {
                (*tmp_dst)((k+1)*8-1, k*8) = U8(linebuf[j*16 + k]);
            }
            tmp_dst++;
        }
        src += width_padded;
    }
    free(linebuf);
}
void align8_to_ap_data_8bit (ap_uint<64>* dst, pel* src, U32 src_width, U32 src_height) {

	U32 width_padded = (src_width + 7) / 8 * 8; // src_width aligned to ac_uint<128> boundary already

    pel* linebuf = (pel *)calloc(width_padded, sizeof(pel));
    ap_uint<64>* tmp_dst = dst;

    if (linebuf == NULL) {
        printf("unable to allocate memory for linebuf, quit.\n");
        exit(-1);
    }

    for (int i=0; i<src_height; i++) {
        memcpy(linebuf, src, width_padded*sizeof(pel));
        for (int j=0; j<width_padded/8;j++) {
            for (int k=0;k<8;k++) {
                (*tmp_dst)((k+1)*8-1, k*8) = U8(linebuf[j*8 + k]);
            }
            tmp_dst++;
        }
        src += width_padded;
    }
    free(linebuf);
}
void align16_to_ap_data(ap_uint<128>* dst, pel* src, U32 src_width, U32 src_height) {

	U32 width_padded = (src_width + 15) / 16 * 16; // src_width aligned to ac_uint<128> boundary already

    pel* linebuf = (pel *)calloc(width_padded, sizeof(pel));
    uint128_t* tmp_dst = dst;

    if (linebuf == NULL) {
        printf("unable to allocate memory for linebuf, quit.\n");
        exit(-1);
    }

    for (int i=0; i<src_height; i++) {
        memcpy(linebuf, src, width_padded*sizeof(pel));
        for (int j=0; j<width_padded/NUM_ELEMENTS;j++) {
            for (int k=0;k<NUM_ELEMENTS;k++) {
                (*tmp_dst)((k+1)*16-1, k*16) = linebuf[j*NUM_ELEMENTS + k];
            }
            tmp_dst++;
        }
        src += width_padded;
    }
    free(linebuf);
}
void align32_to_ap_data_10bit(ap_uint<64>* dst, pel* src, U32 src_width, U32 src_height) {

    U32 width_padded = (src_width + 31) / 32 * 32; // src_width aligned to ac_uint<128> boundary already
    ap_uint<320> tmp_combine;
    pel* linebuf = (pel*)calloc(width_padded, sizeof(pel));
    ap_uint<64>* tmp_dst = dst;

    if (linebuf == NULL) {
        printf("unable to allocate memory for linebuf, quit.\n");
        exit(-1);
    }

    for (int i = 0; i < src_height; i++) {
        memcpy(linebuf, src, width_padded * sizeof(pel));
        for (int j = 0; j < width_padded / 32; j++) {
            for (int a = 0; a < 32; a++) {
              tmp_combine((a + 1) * 10 - 1, a * 10)=U10(linebuf[j * 32 + a]);
            }
            for (int k = 0; k < 5; k++) {
                (*tmp_dst) = tmp_combine((k + 1) * 64 - 1, k * 64);
                tmp_dst++;
            }
            
        }
        src += width_padded;
    }
    free(linebuf);
}

void align64_to_ap_data_10bit(uint128_t* dst, pel* src, U32 src_width, U32 src_height) {

    U32 width_padded = (src_width + 63) / 64 * 64; // src_width aligned to ac_uint<128> boundary already
    ap_uint<640> tmp_combine;
    pel* linebuf = (pel*)calloc(width_padded, sizeof(pel));
    uint128_t* tmp_dst = dst;
    int cnt = 0;
    if (linebuf == NULL) {
        printf("unable to allocate memory for linebuf, quit.\n");
        exit(-1);
    }

    for (int i = 0; i < src_height; i++) {
        memcpy(linebuf, src, width_padded * sizeof(pel));
        for (int j = 0; j < width_padded / 64; j++) {
            for (int a = 0; a < 64; a++) {
              tmp_combine((a + 1) * 10 - 1, a * 10)=U10(linebuf[j * 64 + a]);
            }
            for (int k = 0; k < 5; k++) {
                (*tmp_dst) = tmp_combine((k + 1) * 128 - 1, k * 128);
                tmp_dst++;
                cnt++;
            }
            
        }
        src += width_padded;
    }
    free(linebuf);
}

void lcu_fetch_org_y_bus_read (U32 start, U16 stride, uint128_t* img, hls::stream<uint128_t> &sfifo)
{
    U32 pos;
    pos = start;
    for (int y=0;y<64;y++) {
        for (int i=0;i<8;i++) {
#pragma HLS pipeline
            sfifo.write(img[pos+i]);
        }
        pos += stride;
    }
}

void lcu_fetch_org_y_write_back (hls::stream<uint128_t> &sfifo, pel pix[64][64])
{

    pel linebuf[64];
    uint128_t d;
#pragma HLS array_partition variable=linebuf complete dim=0

    for (int y=0;y<64;y++) {
        for (int i=0;i<8;i++) {
#pragma HLS unroll
            sfifo.read(d);
            for (int k=0;k<8;k++) {
#pragma HLS unroll
                linebuf[i*8+k] = d((k+1)*16-1,k*16);
            }
        }
        for (int x=0;x<64;x++) {
#pragma HLS unroll
            pix[y][x] = linebuf[x];
        }
    }
}

void lcu_fetch_org_y (U32 start, U16 stride, uint128_t* img, pel pix[64][64])
{
#pragma HLS DATAFLOW
    hls::stream<uint128_t> sfifo;
#pragma HLS STREAM variable=sfifo depth=512
    lcu_fetch_org_y_bus_read (start, stride, img, sfifo);
    lcu_fetch_org_y_write_back(sfifo, pix);
}
