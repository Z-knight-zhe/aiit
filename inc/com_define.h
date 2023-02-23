

#ifndef _COM_DEFINE_H_
#define _COM_DEFINE_H_

#include "com_def.h"
typedef s16 pel;


#define FETCH_OPT                           0       // fetch optima
#if FETCH_OPT
#if 1
#include "ap_int.h"
#else
#include "/home/ubuntu/software/Vivado/2019.2/include/gmp.h"
#include <ap_int.h>
#endif  

typedef  ap_uint<128>						U128;
typedef  ap_uint<256>						U256;
typedef  ap_int<128>						S128;
typedef  ap_int<256>						S256;

typedef  ap_uint<64>						U64_v;
typedef  ap_uint<1024>						U1024;

#endif  // #if FETCH_OPT
#define MV_X                               0
#define MV_Y                               1
#define MV_D                               2


/************************* REF LIST DEFINE ************************************************/
#define REFP_0                             0
#define REFP_1                             1
#define REFP_NUM                           2

#define MAX_NUM_ACTIVE_REF_FRAME           4
#define VER_NUM                             4
#if defined(_WIN32) && !defined(__GNUC__)
#define DECLARE_ALIGNED(var, n) __declspec(align(n)) var
#else
#define DECLARE_ALIGNED(var, n) var __attribute__((aligned (n)))
#endif
#define ALIGNED_32(var)    DECLARE_ALIGNED(var, 32)
#define ALIGNED_16(var)    DECLARE_ALIGNED(var, 16)
#define ALIGNED_8(var)     DECLARE_ALIGNED(var,  8)
#define ALIGNED_4(var)     DECLARE_ALIGNED(var,  4)
#define COM_ALIGN(val, align)   ((((val) + (align) - 1) / (align)) * (align))


enum uavs3e_ipfilter_conf {
    IPFILTER_H_8,
    IPFILTER_H_4,
    IPFILTER_V_8,
    IPFILTER_V_4,
    NUM_IPFILTER
};
enum uavs3e_ipfilter_ext_conf {
    IPFILTER_EXT_8,
    IPFILTER_EXT_4,
    NUM_IPFILTER_Ext
};

#define CU_SIZE_NUM_RC                        (7 - 2 + 1) // 4 --> 128
typedef struct funs_handle_t {
    void(*intra_pred_dc)(pel *src, pel *dst, int i_dst, int width, int height, u16 cu_avail, int bit_depth);
    void(*intra_pred_plane)(pel *src, pel *dst, int i_dst, int width, int height, int bit_depth);
    void(*intra_pred_bi)(pel *src, pel *dst, int i_dst, int width, int height, int bit_depth);
    void(*intra_pred_plane_ipf)(pel *src, s16 *dst, int width, int height);
    void(*intra_pred_bi_ipf)(pel *src, s16 *dst, int width, int height);
    void(*intra_pred_ver)(pel *src, pel *dst, int i_dst, int width, int height);
    void(*intra_pred_hor)(pel *src, pel *dst, int i_dst, int width, int height);
    void(*intra_pred_ang[IPD_CNT])(pel *src, pel *dst, int i_dst, int uiDirMode, int iWidth, int iHeight);
    void(*intra_pred_ipf_core)(pel *src, pel *dst, int i_dst, int ipm, int w, int h, int bit_depth);
    void(*intra_pred_ipf_core_s16)(pel *src, pel *dst, int i_dst, s16 *pred, int ipm, int w, int h, int bit_depth);

    void(*deblock_luma[2])(pel *src, int stride, int alpha, int beta, int flag);
    void(*deblock_chroma[2])(pel *srcu, pel *srcv, int stride, int alphau, int betau, int alphav, int betav, int flag);

    //void(*sao)(pel *src, int i_src, pel *dst, int i_dst, com_sao_param_t *sao_params, int height, int width, int avail_left, int avail_right, int avail_up, int avail_down, int bit_depth);
    //void(*sao_stat)(com_pic_t *pic_org, com_pic_t *pic_rec, com_sao_stat_t *saostatsData, int bit_depth, int compIdx, int pix_x, int pix_y, int lcu_pix_width, int lcu_pix_height, int lcu_available_left, int lcu_available_right, int lcu_available_up, int lcu_available_down);

    //void(*alf)(pel *dst, int i_dst, pel *src, int i_src, int lcu_width, int lcu_height, int *coef, int sample_bit_depth);
    //void(*alf_fix)(pel *dst, int i_dst, pel *src, int i_src, int lcu_width, int lcu_height, int *coef, int sample_bit_depth);
    //void(*alf_calc)(pel *p_org, int i_org, pel *p_alf, int i_alf, int xPos, int yPos, int width, int height, double eCorr[9][9], double yCorr[9], int isAboveAvail, int isBelowAvail);

    void(*ipcpy[CU_SIZE_NUM_RC])(const pel *src, int i_src, pel *dst, int i_dst, int width, int height);
    void(*ipflt[NUM_IPFILTER][CU_SIZE_NUM_RC])(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val);
    void(*ipflt_ext[NUM_IPFILTER_Ext][CU_SIZE_NUM_RC])(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coef_x, const s8 *coef_y, int max_val);

    void(*ip_flt_y_hor)(const pel *src, int i_src, pel *dst[3], int i_dst, s16 *dst_tmp[3], int i_dst_tmp, int width, int height, s8(*coeff)[8], int bit_depth);
    void(*ip_flt_y_ver)(const pel *src, int i_src, pel *dst[3], int i_dst, int width, int height, s8(*coeff)[8], int bit_depth);
    void(*ip_flt_y_ver_ext)(const s16 *src, int i_src, pel *dst[3], int i_dst, int width, int height, s8(*coeff)[8], int bit_depth);

    void(*recon[CU_SIZE_NUM_RC])(s16 *resi, pel *pred, int i_pred, int width, int height, pel *rec, int i_rec, int cbf, int bit_depth);
    /*
    void(*dquant[2])(s16 *coef, s16 *coef_out, u8 *wq_matrix[2], int log2_w, int log2_h, int scale, int shift, int bit_depth);

    void(*itrans_dct2[MAX_TR_LOG2][MAX_TR_LOG2])(s16 *coef, s16 *resi, int bit_depth);
    void(*itrans_dct8_dst7[2][MAX_TR_LOG2])(s16 *coeff, s16 *block, int shift, int line, int max_tr_val, int min_tr_val);
    void(*trans_dct2[MAX_TR_LOG2][MAX_TR_LOG2])(s16 *coef, s16 *resi, int bit_depth);
    void(*trans_dct8_dst7[2][MAX_TR_LOG2])(s16 *src, s16 *dst, int shift, int line);
    */
    u32(*cost_sad[CU_SIZE_NUM_RC])(pel *p_org, int i_org, pel *p_pred, int i_pred, int height);
    void(*cost_sad_x3[CU_SIZE_NUM_RC])(pel *p_org, int i_org, pel *pred0, pel *pred1, pel *pred2, int i_pred, u32 sad[3], int height);
    void(*cost_sad_x4[CU_SIZE_NUM_RC])(pel *p_org, int i_org, pel *pred0, pel *pred1, pel *pred2, pel *pred3, int i_pred, u32 sad[4], int height);

    u64(*cost_ssd[CU_SIZE_NUM_RC])(pel *p_org, int i_org, pel *p_pred, int i_pred, int height);

    u32(*cost_satd[3][3])(pel *p_org, int i_org, pel *p_pred, int i_pred);

    u64(*cost_var[CU_SIZE_NUM_RC])(pel* pix, int i_pix);

    void(*ssim_4x4x2_core)(const pel *pix1, int stride1, const pel *pix2, int stride2, int sums[2][4]);
    float(*ssim_end4)(int sum0[5][4], int sum1[5][4], int width, float ssim_c1, float ssim_c2);

    void(*pel_diff[CU_SIZE_NUM_RC])(pel *org, int i_org, pel *pred, int i_pred, s16 *resi, int i_resi, int height);
    void(*pel_avrg[CU_SIZE_NUM_RC])(pel *dst, int i_dst, pel *src1, pel *src2, int height);

    void(*affine_sobel_flt_hor)(pel *pred, int i_pred, s16 *deriv, int i_deriv, int width, int height);
    void(*affine_sobel_flt_ver)(pel *pred, int i_pred, s16 *deriv, int i_deriv, int width, int height);
    void(*affine_coef_computer)(s16 *resi, int i_resi, s16(*deriv)[MAX_CU_DIM], int i_deriv, s64(*coef)[5], int width, int height);

    int(*quant_rdoq)(s16 *coef, int num, int q_value, int q_bits, s32 err_scale, int precision_bits, u32* abs_coef, s16* abs_level, s64 *uncoded_err);
    int(*quant_check)(s16 *coef, int num, int threshold);

    void(*sobel_cost)(pel *pix, int i_pel, int width, int height, int *cost_ver, int *cost_hor);

} funs_handle_t;
const s8 com_tbl_mc_l_coeff_hp[16][8] = {
        { 0, 0, 0, 64, 0, 0, 0, 0 },
        { 0, 1, -3, 63, 4, -2, 1, 0 },
        { -1, 2, -5, 62, 8, -3, 1, 0 },
        { -1, 3, -8, 60, 13, -4, 1, 0 },
        { -1, 4, -10, 58, 17, -5, 1, 0 },
        { -1, 4, -11, 52, 26, -8, 3, -1 },
        { -1, 3, -9, 47, 31, -10, 4, -1 },
        { -1, 4, -11, 45, 34, -10, 4, -1 },
        { -1, 4, -11, 40, 40, -11, 4, -1 },
        { -1, 4, -10, 34, 45, -11, 4, -1 },
        { -1, 4, -10, 31, 47, -9, 3, -1 },
        { -1, 3, -8, 26, 52, -11, 4, -1 },
        { 0, 1, -5, 17, 58, -10, 4, -1 },
        { 0, 1, -4, 13, 60, -8, 3, -1 },
        { 0, 1, -3, 8, 62, -5, 2, -1 },
        { 0, 1, -2, 4, 63, -3, 1, 0 }
};

const s8 com_tbl_mc_l_coeff[4][8] = {
        { 0, 0, 0, 64, 0, 0, 0, 0 },
        { -1, 4, -10, 57, 19, -7, 3, -1 },
        { -1, 4, -11, 40, 40, -11, 4, -1 },
        { -1, 3, -7, 19, 57, -10, 4, -1 }
};
const s8 com_tbl_mc_c_coeff_hp[32][4] = {
        { 0, 64, 0, 0 },
        { -1, 63, 2, 0 },
        { -2, 62, 4, 0 },
        { -2, 60, 7, -1 },
        { -2, 58, 10, -2 },
        { -3, 57, 12, -2 },
        { -4, 56, 14, -2 },
        { -4, 55, 15, -2 },
        { -4, 54, 16, -2 },
        { -5, 53, 18, -2 },
        { -6, 52, 20, -2 },
        { -6, 49, 24, -3 },
        { -6, 46, 28, -4 },
        { -5, 44, 29, -4 },
        { -4, 42, 30, -4 },
        { -4, 39, 33, -4 },
        { -4, 36, 36, -4 },
        { -4, 33, 39, -4 },
        { -4, 30, 42, -4 },
        { -4, 29, 44, -5 },
        { -4, 28, 46, -6 },
        { -3, 24, 49, -6 },
        { -2, 20, 52, -6 },
        { -2, 18, 53, -5 },
        { -2, 16, 54, -4 },
        { -2, 15, 55, -4 },
        { -2, 14, 56, -4 },
        { -2, 12, 57, -3 },
        { -2, 10, 58, -2 },
        { -1, 7, 60, -2 },
        { 0, 4, 62, -2 },
        { 0, 2, 63, -1 }
};
const s8 com_tbl_mc_c_coeff[8][4] = {
        { 0, 64, 0, 0 },
        { -4, 62, 6, 0 },
        { -6, 56, 15, -1 },
        { -5, 47, 25, -3 },
        { -4, 36, 36, -4 },
        { -3, 25, 47, -5 },
        { -1, 15, 56, -6 },
        { 0, 6, 62, -4 }
};
const u8 com_tbl_qp_chroma_ajudst[64] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
    20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
    30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
    40, 41, 42, 42, 43, 43, 44, 44, 45, 45,
    46, 46, 47, 47, 48, 48, 48, 49, 49, 49,
    50, 50, 50, 51
};

//static u8 com_tbl_qp_chroma_adjust_enc[64] = {
//    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
//    10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
//    20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
//    30, 31, 32, 33, 33, 34, 35, 36, 37, 38,
//    39, 40, 41, 42, 43, 44, 44, 44, 45, 46,
//    47, 48, 48, 49, 50, 51, 52, 53, 54, 55,
//    56, 57, 58, 59
//};


static u32 com_had_16x8(pel *org, int s_org, pel *cur, int s_cur)
{
    int k;
    int i;
    int j;
    int jj;
    int satd = 0;
    int diff[128];
    int m1[8][16];
    int m2[8][16];
    for (k = 0; k < 128; k += 16) {
        diff[k + 0] = (int)(org[0] - cur[0]);
        diff[k + 1] = (int)(org[1] - cur[1]);
        diff[k + 2] = (int)(org[2] - cur[2]);
        diff[k + 3] = (int)(org[3] - cur[3]);
        diff[k + 4] = (int)(org[4] - cur[4]);
        diff[k + 5] = (int)(org[5] - cur[5]);
        diff[k + 6] = (int)(org[6] - cur[6]);
        diff[k + 7] = (int)(org[7] - cur[7]);
        diff[k + 8] = (int)(org[8] - cur[8]);
        diff[k + 9] = (int)(org[9] - cur[9]);
        diff[k + 10] = (int)(org[10] - cur[10]);
        diff[k + 11] = (int)(org[11] - cur[11]);
        diff[k + 12] = (int)(org[12] - cur[12]);
        diff[k + 13] = (int)(org[13] - cur[13]);
        diff[k + 14] = (int)(org[14] - cur[14]);
        diff[k + 15] = (int)(org[15] - cur[15]);
        cur += s_cur;
        org += s_org;
    }
    for (j = 0; j < 8; j++) {
        jj = j << 4;
        m2[j][0] = diff[jj] + diff[jj + 8];
        m2[j][1] = diff[jj + 1] + diff[jj + 9];
        m2[j][2] = diff[jj + 2] + diff[jj + 10];
        m2[j][3] = diff[jj + 3] + diff[jj + 11];
        m2[j][4] = diff[jj + 4] + diff[jj + 12];
        m2[j][5] = diff[jj + 5] + diff[jj + 13];
        m2[j][6] = diff[jj + 6] + diff[jj + 14];
        m2[j][7] = diff[jj + 7] + diff[jj + 15];
        m2[j][8] = diff[jj] - diff[jj + 8];
        m2[j][9] = diff[jj + 1] - diff[jj + 9];
        m2[j][10] = diff[jj + 2] - diff[jj + 10];
        m2[j][11] = diff[jj + 3] - diff[jj + 11];
        m2[j][12] = diff[jj + 4] - diff[jj + 12];
        m2[j][13] = diff[jj + 5] - diff[jj + 13];
        m2[j][14] = diff[jj + 6] - diff[jj + 14];
        m2[j][15] = diff[jj + 7] - diff[jj + 15];
        m1[j][0] = m2[j][0] + m2[j][4];
        m1[j][1] = m2[j][1] + m2[j][5];
        m1[j][2] = m2[j][2] + m2[j][6];
        m1[j][3] = m2[j][3] + m2[j][7];
        m1[j][4] = m2[j][0] - m2[j][4];
        m1[j][5] = m2[j][1] - m2[j][5];
        m1[j][6] = m2[j][2] - m2[j][6];
        m1[j][7] = m2[j][3] - m2[j][7];
        m1[j][8] = m2[j][8] + m2[j][12];
        m1[j][9] = m2[j][9] + m2[j][13];
        m1[j][10] = m2[j][10] + m2[j][14];
        m1[j][11] = m2[j][11] + m2[j][15];
        m1[j][12] = m2[j][8] - m2[j][12];
        m1[j][13] = m2[j][9] - m2[j][13];
        m1[j][14] = m2[j][10] - m2[j][14];
        m1[j][15] = m2[j][11] - m2[j][15];
        m2[j][0] = m1[j][0] + m1[j][2];
        m2[j][1] = m1[j][1] + m1[j][3];
        m2[j][2] = m1[j][0] - m1[j][2];
        m2[j][3] = m1[j][1] - m1[j][3];
        m2[j][4] = m1[j][4] + m1[j][6];
        m2[j][5] = m1[j][5] + m1[j][7];
        m2[j][6] = m1[j][4] - m1[j][6];
        m2[j][7] = m1[j][5] - m1[j][7];
        m2[j][8] = m1[j][8] + m1[j][10];
        m2[j][9] = m1[j][9] + m1[j][11];
        m2[j][10] = m1[j][8] - m1[j][10];
        m2[j][11] = m1[j][9] - m1[j][11];
        m2[j][12] = m1[j][12] + m1[j][14];
        m2[j][13] = m1[j][13] + m1[j][15];
        m2[j][14] = m1[j][12] - m1[j][14];
        m2[j][15] = m1[j][13] - m1[j][15];
        m1[j][0] = m2[j][0] + m2[j][1];
        m1[j][1] = m2[j][0] - m2[j][1];
        m1[j][2] = m2[j][2] + m2[j][3];
        m1[j][3] = m2[j][2] - m2[j][3];
        m1[j][4] = m2[j][4] + m2[j][5];
        m1[j][5] = m2[j][4] - m2[j][5];
        m1[j][6] = m2[j][6] + m2[j][7];
        m1[j][7] = m2[j][6] - m2[j][7];
        m1[j][8] = m2[j][8] + m2[j][9];
        m1[j][9] = m2[j][8] - m2[j][9];
        m1[j][10] = m2[j][10] + m2[j][11];
        m1[j][11] = m2[j][10] - m2[j][11];
        m1[j][12] = m2[j][12] + m2[j][13];
        m1[j][13] = m2[j][12] - m2[j][13];
        m1[j][14] = m2[j][14] + m2[j][15];
        m1[j][15] = m2[j][14] - m2[j][15];
    }
    for (i = 0; i < 16; i++) {
        m2[0][i] = m1[0][i] + m1[4][i];
        m2[1][i] = m1[1][i] + m1[5][i];
        m2[2][i] = m1[2][i] + m1[6][i];
        m2[3][i] = m1[3][i] + m1[7][i];
        m2[4][i] = m1[0][i] - m1[4][i];
        m2[5][i] = m1[1][i] - m1[5][i];
        m2[6][i] = m1[2][i] - m1[6][i];
        m2[7][i] = m1[3][i] - m1[7][i];
        m1[0][i] = m2[0][i] + m2[2][i];
        m1[1][i] = m2[1][i] + m2[3][i];
        m1[2][i] = m2[0][i] - m2[2][i];
        m1[3][i] = m2[1][i] - m2[3][i];
        m1[4][i] = m2[4][i] + m2[6][i];
        m1[5][i] = m2[5][i] + m2[7][i];
        m1[6][i] = m2[4][i] - m2[6][i];
        m1[7][i] = m2[5][i] - m2[7][i];
        m2[0][i] = m1[0][i] + m1[1][i];
        m2[1][i] = m1[0][i] - m1[1][i];
        m2[2][i] = m1[2][i] + m1[3][i];
        m2[3][i] = m1[2][i] - m1[3][i];
        m2[4][i] = m1[4][i] + m1[5][i];
        m2[5][i] = m1[4][i] - m1[5][i];
        m2[6][i] = m1[6][i] + m1[7][i];
        m2[7][i] = m1[6][i] - m1[7][i];
    }
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 16; j++) {
            satd += abs(m2[i][j]);
        }
    }

    //satd = ((int)(((double)satd) / sqrt(16.0 * 8.0) * 2.0));
    satd = (int)(((s64)satd * 20000 + 56569) / 113137);//fix_point num = 10000
    return satd;
}

static u32 com_had_8x16(pel *org, int s_org, pel *cur, int s_cur)
{
    int k;
    int i;
    int j;
    int jj;
    int satd = 0;
    int diff[128];
    int m1[16][8];
    int m2[16][8];
    for (k = 0; k < 128; k += 8) {
        diff[k + 0] = int(org[0] - cur[0]);
        diff[k + 1] = int(org[1] - cur[1]);
        diff[k + 2] = int(org[2] - cur[2]);
        diff[k + 3] = int(org[3] - cur[3]);
        diff[k + 4] = int(org[4] - cur[4]);
        diff[k + 5] = int(org[5] - cur[5]);
        diff[k + 6] = int(org[6] - cur[6]);
        diff[k + 7] = int(org[7] - cur[7]);
        cur += s_cur;
        org += s_org;
    }
    for (j = 0; j < 16; j++) {
        jj = j << 3;
        m2[j][0] = diff[jj] + diff[jj + 4];
        m2[j][1] = diff[jj + 1] + diff[jj + 5];
        m2[j][2] = diff[jj + 2] + diff[jj + 6];
        m2[j][3] = diff[jj + 3] + diff[jj + 7];
        m2[j][4] = diff[jj] - diff[jj + 4];
        m2[j][5] = diff[jj + 1] - diff[jj + 5];
        m2[j][6] = diff[jj + 2] - diff[jj + 6];
        m2[j][7] = diff[jj + 3] - diff[jj + 7];
        m1[j][0] = m2[j][0] + m2[j][2];
        m1[j][1] = m2[j][1] + m2[j][3];
        m1[j][2] = m2[j][0] - m2[j][2];
        m1[j][3] = m2[j][1] - m2[j][3];
        m1[j][4] = m2[j][4] + m2[j][6];
        m1[j][5] = m2[j][5] + m2[j][7];
        m1[j][6] = m2[j][4] - m2[j][6];
        m1[j][7] = m2[j][5] - m2[j][7];
        m2[j][0] = m1[j][0] + m1[j][1];
        m2[j][1] = m1[j][0] - m1[j][1];
        m2[j][2] = m1[j][2] + m1[j][3];
        m2[j][3] = m1[j][2] - m1[j][3];
        m2[j][4] = m1[j][4] + m1[j][5];
        m2[j][5] = m1[j][4] - m1[j][5];
        m2[j][6] = m1[j][6] + m1[j][7];
        m2[j][7] = m1[j][6] - m1[j][7];
    }
    for (i = 0; i < 8; i++) {
        m1[0][i] = m2[0][i] + m2[8][i];
        m1[1][i] = m2[1][i] + m2[9][i];
        m1[2][i] = m2[2][i] + m2[10][i];
        m1[3][i] = m2[3][i] + m2[11][i];
        m1[4][i] = m2[4][i] + m2[12][i];
        m1[5][i] = m2[5][i] + m2[13][i];
        m1[6][i] = m2[6][i] + m2[14][i];
        m1[7][i] = m2[7][i] + m2[15][i];
        m1[8][i] = m2[0][i] - m2[8][i];
        m1[9][i] = m2[1][i] - m2[9][i];
        m1[10][i] = m2[2][i] - m2[10][i];
        m1[11][i] = m2[3][i] - m2[11][i];
        m1[12][i] = m2[4][i] - m2[12][i];
        m1[13][i] = m2[5][i] - m2[13][i];
        m1[14][i] = m2[6][i] - m2[14][i];
        m1[15][i] = m2[7][i] - m2[15][i];
        m2[0][i] = m1[0][i] + m1[4][i];
        m2[1][i] = m1[1][i] + m1[5][i];
        m2[2][i] = m1[2][i] + m1[6][i];
        m2[3][i] = m1[3][i] + m1[7][i];
        m2[4][i] = m1[0][i] - m1[4][i];
        m2[5][i] = m1[1][i] - m1[5][i];
        m2[6][i] = m1[2][i] - m1[6][i];
        m2[7][i] = m1[3][i] - m1[7][i];
        m2[8][i] = m1[8][i] + m1[12][i];
        m2[9][i] = m1[9][i] + m1[13][i];
        m2[10][i] = m1[10][i] + m1[14][i];
        m2[11][i] = m1[11][i] + m1[15][i];
        m2[12][i] = m1[8][i] - m1[12][i];
        m2[13][i] = m1[9][i] - m1[13][i];
        m2[14][i] = m1[10][i] - m1[14][i];
        m2[15][i] = m1[11][i] - m1[15][i];
        m1[0][i] = m2[0][i] + m2[2][i];
        m1[1][i] = m2[1][i] + m2[3][i];
        m1[2][i] = m2[0][i] - m2[2][i];
        m1[3][i] = m2[1][i] - m2[3][i];
        m1[4][i] = m2[4][i] + m2[6][i];
        m1[5][i] = m2[5][i] + m2[7][i];
        m1[6][i] = m2[4][i] - m2[6][i];
        m1[7][i] = m2[5][i] - m2[7][i];
        m1[8][i] = m2[8][i] + m2[10][i];
        m1[9][i] = m2[9][i] + m2[11][i];
        m1[10][i] = m2[8][i] - m2[10][i];
        m1[11][i] = m2[9][i] - m2[11][i];
        m1[12][i] = m2[12][i] + m2[14][i];
        m1[13][i] = m2[13][i] + m2[15][i];
        m1[14][i] = m2[12][i] - m2[14][i];
        m1[15][i] = m2[13][i] - m2[15][i];
        m2[0][i] = m1[0][i] + m1[1][i];
        m2[1][i] = m1[0][i] - m1[1][i];
        m2[2][i] = m1[2][i] + m1[3][i];
        m2[3][i] = m1[2][i] - m1[3][i];
        m2[4][i] = m1[4][i] + m1[5][i];
        m2[5][i] = m1[4][i] - m1[5][i];
        m2[6][i] = m1[6][i] + m1[7][i];
        m2[7][i] = m1[6][i] - m1[7][i];
        m2[8][i] = m1[8][i] + m1[9][i];
        m2[9][i] = m1[8][i] - m1[9][i];
        m2[10][i] = m1[10][i] + m1[11][i];
        m2[11][i] = m1[10][i] - m1[11][i];
        m2[12][i] = m1[12][i] + m1[13][i];
        m2[13][i] = m1[12][i] - m1[13][i];
        m2[14][i] = m1[14][i] + m1[15][i];
        m2[15][i] = m1[14][i] - m1[15][i];
    }
    for (i = 0; i < 16; i++) {
        for (j = 0; j < 8; j++) {
            satd += abs(m2[i][j]);
        }
    }

    //satd = ((int)(((double)satd) / sqrt(16.0 * 8.0) * 2.0));
    satd = (int)(((s64)satd * 20000 + 56569) / 113137);//fix_point num = 10000
    return satd;
}

static u32 com_had_8x4(pel *org, int s_org, pel *cur, int s_cur)
{
    int k;
    int i;
    int j;
    int jj;
    int satd = 0;
    int diff[32];
    int m1[4][8];
    int m2[4][8];
    for (k = 0; k < 32; k += 8) {
        diff[k + 0] = (org[0] - cur[0]);
        diff[k + 1] = (org[1] - cur[1]);
        diff[k + 2] = (org[2] - cur[2]);
        diff[k + 3] = (org[3] - cur[3]);
        diff[k + 4] = (org[4] - cur[4]);
        diff[k + 5] = (org[5] - cur[5]);
        diff[k + 6] = (org[6] - cur[6]);
        diff[k + 7] = (org[7] - cur[7]);
        cur += s_cur;
        org += s_org;
    }
    for (j = 0; j < 4; j++) {
        jj = j << 3;
        m2[j][0] = diff[jj] + diff[jj + 4];
        m2[j][1] = diff[jj + 1] + diff[jj + 5];
        m2[j][2] = diff[jj + 2] + diff[jj + 6];
        m2[j][3] = diff[jj + 3] + diff[jj + 7];
        m2[j][4] = diff[jj] - diff[jj + 4];
        m2[j][5] = diff[jj + 1] - diff[jj + 5];
        m2[j][6] = diff[jj + 2] - diff[jj + 6];
        m2[j][7] = diff[jj + 3] - diff[jj + 7];
        m1[j][0] = m2[j][0] + m2[j][2];
        m1[j][1] = m2[j][1] + m2[j][3];
        m1[j][2] = m2[j][0] - m2[j][2];
        m1[j][3] = m2[j][1] - m2[j][3];
        m1[j][4] = m2[j][4] + m2[j][6];
        m1[j][5] = m2[j][5] + m2[j][7];
        m1[j][6] = m2[j][4] - m2[j][6];
        m1[j][7] = m2[j][5] - m2[j][7];
        m2[j][0] = m1[j][0] + m1[j][1];
        m2[j][1] = m1[j][0] - m1[j][1];
        m2[j][2] = m1[j][2] + m1[j][3];
        m2[j][3] = m1[j][2] - m1[j][3];
        m2[j][4] = m1[j][4] + m1[j][5];
        m2[j][5] = m1[j][4] - m1[j][5];
        m2[j][6] = m1[j][6] + m1[j][7];
        m2[j][7] = m1[j][6] - m1[j][7];
    }
    for (i = 0; i < 8; i++) {
        m1[0][i] = m2[0][i] + m2[2][i];
        m1[1][i] = m2[1][i] + m2[3][i];
        m1[2][i] = m2[0][i] - m2[2][i];
        m1[3][i] = m2[1][i] - m2[3][i];
        m2[0][i] = m1[0][i] + m1[1][i];
        m2[1][i] = m1[0][i] - m1[1][i];
        m2[2][i] = m1[2][i] + m1[3][i];
        m2[3][i] = m1[2][i] - m1[3][i];
    }
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 8; j++) {
            satd += abs(m2[i][j]);
        }
    }

    //satd = ((int)(((double)satd) / sqrt(4.0 * 8.0) * 2.0));
    satd = (int)(((s64)satd * 40000 + 56569) / 113137);//fix_point num = 20000

    return satd;
}

static u32 com_had_4x8(pel *org, int s_org, pel *cur, int s_cur)
{
    int k;
    int i;
    int j;
    int jj;
    int satd = 0;
    int diff[32];
    int m1[8][4];
    int m2[8][4];
    for (k = 0; k < 32; k += 4) {
        diff[k + 0] = (org[0] - cur[0]);
        diff[k + 1] = (org[1] - cur[1]);
        diff[k + 2] = (org[2] - cur[2]);
        diff[k + 3] = (org[3] - cur[3]);
        cur += s_cur;
        org += s_org;
    }
    for (j = 0; j < 8; j++) {
        jj = j << 2;
        m2[j][0] = diff[jj] + diff[jj + 2];
        m2[j][1] = diff[jj + 1] + diff[jj + 3];
        m2[j][2] = diff[jj] - diff[jj + 2];
        m2[j][3] = diff[jj + 1] - diff[jj + 3];
        m1[j][0] = m2[j][0] + m2[j][1];
        m1[j][1] = m2[j][0] - m2[j][1];
        m1[j][2] = m2[j][2] + m2[j][3];
        m1[j][3] = m2[j][2] - m2[j][3];
    }
    for (i = 0; i < 4; i++) {
        m2[0][i] = m1[0][i] + m1[4][i];
        m2[1][i] = m1[1][i] + m1[5][i];
        m2[2][i] = m1[2][i] + m1[6][i];
        m2[3][i] = m1[3][i] + m1[7][i];
        m2[4][i] = m1[0][i] - m1[4][i];
        m2[5][i] = m1[1][i] - m1[5][i];
        m2[6][i] = m1[2][i] - m1[6][i];
        m2[7][i] = m1[3][i] - m1[7][i];
        m1[0][i] = m2[0][i] + m2[2][i];
        m1[1][i] = m2[1][i] + m2[3][i];
        m1[2][i] = m2[0][i] - m2[2][i];
        m1[3][i] = m2[1][i] - m2[3][i];
        m1[4][i] = m2[4][i] + m2[6][i];
        m1[5][i] = m2[5][i] + m2[7][i];
        m1[6][i] = m2[4][i] - m2[6][i];
        m1[7][i] = m2[5][i] - m2[7][i];
        m2[0][i] = m1[0][i] + m1[1][i];
        m2[1][i] = m1[0][i] - m1[1][i];
        m2[2][i] = m1[2][i] + m1[3][i];
        m2[3][i] = m1[2][i] - m1[3][i];
        m2[4][i] = m1[4][i] + m1[5][i];
        m2[5][i] = m1[4][i] - m1[5][i];
        m2[6][i] = m1[6][i] + m1[7][i];
        m2[7][i] = m1[6][i] - m1[7][i];
    }
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 4; j++) {
            satd += abs(m2[i][j]);
        }
    }

    //satd = ((int)(((double)satd) / sqrt(4.0 * 8.0) * 2.0));
    satd = (int)(((s64)satd * 40000 + 56569) / 113137);//fix_point num = 20000
    return satd;
}

static u32 com_had_8x8(pel *org, int s_org, pel *cur, int s_cur)
{
    int k;
    int i;
    int j;
    int jj;
    int satd = 0;
    int diff[64];
    int m1[8][8];
    int m2[8][8];
    int m3[8][8];
    for (k = 0; k < 64; k += 8) {
        diff[k + 0] = (int)(org[0] - cur[0]);
        diff[k + 1] = (int)(org[1] - cur[1]);
        diff[k + 2] = (int)(org[2] - cur[2]);
        diff[k + 3] = (int)(org[3] - cur[3]);
        diff[k + 4] = (int)(org[4] - cur[4]);
        diff[k + 5] = (int)(org[5] - cur[5]);
        diff[k + 6] = (int)(org[6] - cur[6]);
        diff[k + 7] = (int)(org[7] - cur[7]);
        cur += s_cur;
        org += s_org;
    }
    /* horizontal */
    for (j = 0; j < 8; j++) {
        jj = j << 3;
        m2[j][0] = diff[jj] + diff[jj + 4];
        m2[j][1] = diff[jj + 1] + diff[jj + 5];
        m2[j][2] = diff[jj + 2] + diff[jj + 6];
        m2[j][3] = diff[jj + 3] + diff[jj + 7];
        m2[j][4] = diff[jj] - diff[jj + 4];
        m2[j][5] = diff[jj + 1] - diff[jj + 5];
        m2[j][6] = diff[jj + 2] - diff[jj + 6];
        m2[j][7] = diff[jj + 3] - diff[jj + 7];
        m1[j][0] = m2[j][0] + m2[j][2];
        m1[j][1] = m2[j][1] + m2[j][3];
        m1[j][2] = m2[j][0] - m2[j][2];
        m1[j][3] = m2[j][1] - m2[j][3];
        m1[j][4] = m2[j][4] + m2[j][6];
        m1[j][5] = m2[j][5] + m2[j][7];
        m1[j][6] = m2[j][4] - m2[j][6];
        m1[j][7] = m2[j][5] - m2[j][7];
        m2[j][0] = m1[j][0] + m1[j][1];
        m2[j][1] = m1[j][0] - m1[j][1];
        m2[j][2] = m1[j][2] + m1[j][3];
        m2[j][3] = m1[j][2] - m1[j][3];
        m2[j][4] = m1[j][4] + m1[j][5];
        m2[j][5] = m1[j][4] - m1[j][5];
        m2[j][6] = m1[j][6] + m1[j][7];
        m2[j][7] = m1[j][6] - m1[j][7];
    }
    /* vertical */
    for (i = 0; i < 8; i++) {
        m3[0][i] = m2[0][i] + m2[4][i];
        m3[1][i] = m2[1][i] + m2[5][i];
        m3[2][i] = m2[2][i] + m2[6][i];
        m3[3][i] = m2[3][i] + m2[7][i];
        m3[4][i] = m2[0][i] - m2[4][i];
        m3[5][i] = m2[1][i] - m2[5][i];
        m3[6][i] = m2[2][i] - m2[6][i];
        m3[7][i] = m2[3][i] - m2[7][i];
        m1[0][i] = m3[0][i] + m3[2][i];
        m1[1][i] = m3[1][i] + m3[3][i];
        m1[2][i] = m3[0][i] - m3[2][i];
        m1[3][i] = m3[1][i] - m3[3][i];
        m1[4][i] = m3[4][i] + m3[6][i];
        m1[5][i] = m3[5][i] + m3[7][i];
        m1[6][i] = m3[4][i] - m3[6][i];
        m1[7][i] = m3[5][i] - m3[7][i];
        m2[0][i] = m1[0][i] + m1[1][i];
        m2[1][i] = m1[0][i] - m1[1][i];
        m2[2][i] = m1[2][i] + m1[3][i];
        m2[3][i] = m1[2][i] - m1[3][i];
        m2[4][i] = m1[4][i] + m1[5][i];
        m2[5][i] = m1[4][i] - m1[5][i];
        m2[6][i] = m1[6][i] + m1[7][i];
        m2[7][i] = m1[6][i] - m1[7][i];
    }
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 8; j++) {
            satd += abs(m2[i][j]);
        }
    }
    satd = (satd + 2) >> 2;
    return satd;
}

static u32 com_had_4x4(pel *org, int s_org, pel *cur, int s_cur)
{

#pragma HLS INLINE off
    int k;
    int satd = 0;
    int diff[16];
    int m[16];
    int d[16];
    for (k = 0; k < 16; k += 4) {

#pragma HLS LOOP_TRIPCOUNT max=4
        diff[k + 0] = (int)(org[0] - cur[0]);
        diff[k + 1] = (int)(org[1] - cur[1]);
        diff[k + 2] = (int)(org[2] - cur[2]);
        diff[k + 3] = (int)(org[3] - cur[3]);
        cur += s_cur;
        org += s_org;
    }
    m[0] = diff[0] + diff[12];
    m[1] = diff[1] + diff[13];
    m[2] = diff[2] + diff[14];
    m[3] = diff[3] + diff[15];
    m[4] = diff[4] + diff[8];
    m[5] = diff[5] + diff[9];
    m[6] = diff[6] + diff[10];
    m[7] = diff[7] + diff[11];
    m[8] = diff[4] - diff[8];
    m[9] = diff[5] - diff[9];
    m[10] = diff[6] - diff[10];
    m[11] = diff[7] - diff[11];
    m[12] = diff[0] - diff[12];
    m[13] = diff[1] - diff[13];
    m[14] = diff[2] - diff[14];
    m[15] = diff[3] - diff[15];
    d[0] = m[0] + m[4];
    d[1] = m[1] + m[5];
    d[2] = m[2] + m[6];
    d[3] = m[3] + m[7];
    d[4] = m[8] + m[12];
    d[5] = m[9] + m[13];
    d[6] = m[10] + m[14];
    d[7] = m[11] + m[15];
    d[8] = m[0] - m[4];
    d[9] = m[1] - m[5];
    d[10] = m[2] - m[6];
    d[11] = m[3] - m[7];
    d[12] = m[12] - m[8];
    d[13] = m[13] - m[9];
    d[14] = m[14] - m[10];
    d[15] = m[15] - m[11];
    m[0] = d[0] + d[3];
    m[1] = d[1] + d[2];
    m[2] = d[1] - d[2];
    m[3] = d[0] - d[3];
    m[4] = d[4] + d[7];
    m[5] = d[5] + d[6];
    m[6] = d[5] - d[6];
    m[7] = d[4] - d[7];
    m[8] = d[8] + d[11];
    m[9] = d[9] + d[10];
    m[10] = d[9] - d[10];
    m[11] = d[8] - d[11];
    m[12] = d[12] + d[15];
    m[13] = d[13] + d[14];
    m[14] = d[13] - d[14];
    m[15] = d[12] - d[15];
    d[0] = m[0] + m[1];
    d[1] = m[0] - m[1];
    d[2] = m[2] + m[3];
    d[3] = m[3] - m[2];
    d[4] = m[4] + m[5];
    d[5] = m[4] - m[5];
    d[6] = m[6] + m[7];
    d[7] = m[7] - m[6];
    d[8] = m[8] + m[9];
    d[9] = m[8] - m[9];
    d[10] = m[10] + m[11];
    d[11] = m[11] - m[10];
    d[12] = m[12] + m[13];
    d[13] = m[12] - m[13];
    d[14] = m[14] + m[15];
    d[15] = m[15] - m[14];
    for (k = 0; k < 16; k++) {

#pragma HLS LOOP_TRIPCOUNT max=16
        satd += abs(d[k]);
    }
    satd = (satd + 1) >> 1;
    return satd;
}


#define COM_IPRED_CHK_CONV(mode) ((mode) == IPD_VER || (mode) == IPD_HOR || (mode) == IPD_DC || (mode) == IPD_BI)

#define COM_IPRED_CONV_L2C(mode)\
    ((mode) == IPD_VER) ? IPD_VER_C : \
    ((mode) == IPD_HOR ? IPD_HOR_C : ((mode) == IPD_DC ? IPD_DC_C : IPD_BI_C))

const char tab_auc_dir_dxdy[2][IPD_CNT][2] = {
        {
            // dx/dy
            { 0, 0 }, { 0, 0 }, { 0, 0 }, { 11, 2 }, { 2, 0 },
            { 11, 3 }, { 1, 0 }, { 93, 7 }, { 1, 1 }, { 93, 8 },
            { 1, 2 }, { 1, 3 }, { 0, 0 }, { 1, 3 }, { 1, 2 },
            { 93, 8 }, { 1, 1 }, { 93, 7 }, { 1, 0 }, { 11, 3 },
            { 2, 0 }, { 11, 2 }, { 4, 0 }, { 8, 0 }, { 0, 0 },
            { 8, 0 }, { 4, 0 }, { 11, 2 }, { 2, 0 }, { 11, 3 },
            { 1, 0 }, { 93, 7 }, { 1, 1 },
        },
        {
            // dy/dx
            { 0, 0 }, { 0, 0 }, { 0, 0 }, { 93, 8 }, { 1, 1 },
            { 93, 7 }, { 1, 0 }, { 11, 3 }, { 2, 0 }, { 11, 2 },
            { 4, 0 }, { 8, 0 }, { 0, 0 }, { 8, 0 }, { 4, 0 },
            { 11, 2 }, { 2, 0 }, { 11, 3 }, { 1, 0 }, { 93, 7 },
            { 1, 1 }, { 93, 8 }, { 1, 2 }, { 1, 3 }, { 0, 0 },
            { 1, 3 }, { 1, 2 }, { 93, 8 }, { 1, 1 }, { 93, 7 },
            { 1, 0 }, { 11, 3 }, { 2, 0 }
        }
};
#endif /* _COM_DEF_H_ */