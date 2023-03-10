

#include "com_port.h"
#include "com_def.h"
#include "com_define.h"
#include <assert.h>
static signed  char com_tbl_log2[257] = {
    /* 0, 1 */
    -1, 0,
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
    8
};
#define CONV_LOG2(v)                   (com_tbl_log2[v])
extern funs_handle_t avs3_funs_handle;
static  void com_mv_rounding_s32(s32 hor, int ver, s32 *rounded_hor, s32 *rounded_ver, s32 right_shift, int left_shift)
{
    int add = (right_shift > 0) ? (1 << (right_shift - 1)) : 0;
    *rounded_hor = (hor >= 0) ? (((hor + add) >> right_shift) << left_shift) : -(((-hor + add) >> right_shift) << left_shift);
    *rounded_ver = (ver >= 0) ? (((ver + add) >> right_shift) << left_shift) : -(((-ver + add) >> right_shift) << left_shift);
}
void com_mc_blk_luma(COM_PIC  *pic, pel *dst, int dst_stride, int x_pos, int y_pos, int width, int height, int widx, int max_posx, int max_posy, int max_val, int hp_flag)
{
    const s8(*coeff)[8];
    int dx, dy;
    int i_src = pic->stride_luma;

    if (hp_flag) {
        dx = x_pos & 15;
        dy = y_pos & 15;
        x_pos >>= 4;
        y_pos >>= 4;
        coeff = com_tbl_mc_l_coeff_hp;
    } else {
        dx = x_pos & 3;
        dy = y_pos & 3;
        x_pos >>= 2;
        y_pos >>= 2;
        coeff = com_tbl_mc_l_coeff;
    }

    x_pos = COM_CLIP3(-MAX_CU_SIZE - 4, max_posx, x_pos);
    y_pos = COM_CLIP3(-MAX_CU_SIZE - 4, max_posy, y_pos);

    if (hp_flag) {
        if ((dx == 0 || dx == 8) && (dy == 0 && dy == 8)) {
            dx = dx >> 2;
            dy = dy >> 2;
            pel *src = (pel*)pic->subpel.imgs[dy][dx]->addr_plane[0] + y_pos * i_src + x_pos;
            avs3_funs_handle.ipcpy[widx](src, i_src, dst, dst_stride, width, height);
        } else {
            pel *src = pic->y + y_pos * i_src + x_pos;

            if (dx == 0 && dy == 0) {
                avs3_funs_handle.ipcpy[widx](src, i_src, dst, dst_stride, width, height);
            } else if (dy == 0) {
                avs3_funs_handle.ipflt[IPFILTER_H_8][widx](src, i_src, dst, dst_stride, width, height, coeff[dx], max_val);
            } else if (dx == 0) {
                avs3_funs_handle.ipflt[IPFILTER_V_8][widx](src, i_src, dst, dst_stride, width, height, coeff[dy], max_val);
            } else {
                avs3_funs_handle.ipflt_ext[IPFILTER_EXT_8][widx](src, i_src, dst, dst_stride, width, height, coeff[dx], coeff[dy], max_val);
            }
        }
    } else {
        pel *src = (pel*)pic->subpel.imgs[dy][dx]->addr_plane[0] + y_pos * i_src + x_pos;
        avs3_funs_handle.ipcpy[widx](src, i_src, dst, dst_stride, width, height);
    }
}

pel* com_mc_blk_luma_pointer(COM_PIC *pic, int x_pos, int y_pos, int max_posx, int max_posy)
{
    int dx = x_pos & 3;
    int dy = y_pos & 3;

    x_pos >>= 2;
    y_pos >>= 2;

    x_pos = COM_CLIP3(-MAX_CU_SIZE - 4, max_posx, x_pos);
    y_pos = COM_CLIP3(-MAX_CU_SIZE - 4, max_posy, y_pos);

    return  (pel*)pic->subpel.imgs[dy][dx]->addr_plane[0] + y_pos * pic->stride_luma + x_pos;
}

static void com_mc_blk_chroma(COM_PIC *pic, int uv_flag, pel *dst, int dst_stride, int x_pos, int y_pos, int width, int height, int widx, int max_posx, int max_posy, int max_val, int hp_flag)
{
    int dx, dy;
    const s8(*coeff)[4];
    pel *src = uv_flag ? pic->v : pic->u;
    int i_src = pic->stride_chroma;

    if (hp_flag) {
        dx = x_pos & 31;
        dy = y_pos & 31;
        x_pos >>= 5;
        y_pos >>= 5;
        coeff = com_tbl_mc_c_coeff_hp;
    } else {
        dx = x_pos & 7;
        dy = y_pos & 7;
        x_pos >>= 3;
        y_pos >>= 3;
        coeff = com_tbl_mc_c_coeff;
    }

    x_pos = COM_CLIP3(-(MAX_CU_SIZE >> 1) - 2, max_posx, x_pos);
    y_pos = COM_CLIP3(-(MAX_CU_SIZE >> 1) - 2, max_posy, y_pos);

    src += y_pos * i_src + x_pos;

    if (dx == 0 && dy == 0) {
        avs3_funs_handle.ipcpy[widx](src, i_src, dst, dst_stride, width, height);
    } else if (dy == 0) {
        avs3_funs_handle.ipflt[IPFILTER_H_4][widx](src, i_src, dst, dst_stride, width, height, coeff[dx], max_val);
    } else if (dx == 0) {
        avs3_funs_handle.ipflt[IPFILTER_V_4][widx](src, i_src, dst, dst_stride, width, height, coeff[dy], max_val);
    } else {
        avs3_funs_handle.ipflt_ext[IPFILTER_EXT_4][widx](src, i_src, dst, dst_stride, width, height, coeff[dx], coeff[dy], max_val);
    }
}



void com_mc_blk_affine_luma(int x, int y, int pic_w, int pic_h, int cu_width, int cu_height, CPMV ac_mv[VER_NUM][MV_D], COM_PIC *ref_pic, pel pred[MAX_CU_DIM], int sub_w, int sub_h, int bit_depth)
{
    assert(com_tbl_log2[cu_width ] >= 4);
    assert(com_tbl_log2[cu_height] >= 4);

    int half_w = sub_w >> 1;
    int half_h = sub_h >> 1;
    s32 mv_scale_hor = (s32)ac_mv[0][MV_X] << 7;
    s32 mv_scale_ver = (s32)ac_mv[0][MV_Y] << 7;
    s32 mv_scale_tmp_hor, mv_scale_tmp_ver;

    // convert to 2^(storeBit + bit) precision
    s32 dmv_hor_x = (((s32)ac_mv[1][MV_X] - (s32)ac_mv[0][MV_X]) << 7) >> com_tbl_log2[cu_width];      // deltaMvHor
    s32 dmv_hor_y = (((s32)ac_mv[1][MV_Y] - (s32)ac_mv[0][MV_Y]) << 7) >> com_tbl_log2[cu_width];
    s32 dmv_ver_x = -dmv_hor_y;                                                                        // deltaMvVer
    s32 dmv_ver_y = dmv_hor_x;

    int widx = CONV_LOG2(sub_w) - MIN_CU_LOG2;
    int max_posx = pic_w + 4;
    int max_posy = pic_h + 4;

    // get prediction block by block
    for (int h = 0; h < cu_height; h += sub_h) {
        for (int w = 0; w < cu_width; w += sub_w) {
            int pos_x = w + half_w;
            int pos_y = h + half_h;

            if (w == 0 && h == 0) {
                pos_x = 0;
                pos_y = 0;
            } else if (w + sub_w == cu_width && h == 0) {
                pos_x = cu_width;
                pos_y = 0;
            } 

            mv_scale_tmp_hor = mv_scale_hor + dmv_hor_x * pos_x + dmv_ver_x * pos_y;
            mv_scale_tmp_ver = mv_scale_ver + dmv_hor_y * pos_x + dmv_ver_y * pos_y;

            // 1/16 precision, 18 bits for mc
            com_mv_rounding_s32(mv_scale_tmp_hor, mv_scale_tmp_ver, &mv_scale_tmp_hor, &mv_scale_tmp_ver, 7, 0);
            mv_scale_tmp_hor = COM_CLIP3(COM_INT18_MIN, COM_INT18_MAX, mv_scale_tmp_hor);
            mv_scale_tmp_ver = COM_CLIP3(COM_INT18_MIN, COM_INT18_MAX, mv_scale_tmp_ver);

            int qpel_gmv_x = ((x + w) << 4) + mv_scale_tmp_hor;
            int qpel_gmv_y = ((y + h) << 4) + mv_scale_tmp_ver;

            com_mc_blk_luma(ref_pic, pred + w, cu_width, qpel_gmv_x, qpel_gmv_y, sub_w, sub_h, widx, max_posx, max_posy, (1 << bit_depth) - 1, 1);
        }
        pred += (cu_width * sub_h);
    }
}

static void com_mc_blk_affine(int x, int y, int pic_w, int pic_h, int cu_width, int cu_height, CPMV ac_mv[VER_NUM][MV_D], COM_PIC *ref_pic, pel pred[N_C][MAX_CU_DIM], int cp_num, int sub_w, int sub_h, int lidx, int bit_depth)
{
    assert(com_tbl_log2[cu_width] >= 4);
    assert(com_tbl_log2[cu_height] >= 4);

    int qpel_gmv_x, qpel_gmv_y;
    pel *pred_y = pred[Y_C], *pred_u = pred[U_C], *pred_v = pred[V_C];
    int w, h;
    int half_w, half_h;

    int dmv_hor_x, dmv_ver_x, dmv_hor_y, dmv_ver_y;
    s32 mv_scale_hor = (s32)ac_mv[0][MV_X] << 7;
    s32 mv_scale_ver = (s32)ac_mv[0][MV_Y] << 7;
    s32 mv_scale_tmp_hor, mv_scale_tmp_ver;
    s32 mv_save[MAX_CU_SIZE >> MIN_CU_LOG2][MAX_CU_SIZE >> MIN_CU_LOG2][MV_D];

    half_w = sub_w >> 1;
    half_h = sub_h >> 1;

    dmv_hor_x = (((s32)ac_mv[1][MV_X] - (s32)ac_mv[0][MV_X]) << 7) >> com_tbl_log2[cu_width];      // deltaMvHor
    dmv_hor_y = (((s32)ac_mv[1][MV_Y] - (s32)ac_mv[0][MV_Y]) << 7) >> com_tbl_log2[cu_width];

    if (cp_num == 3) {
        dmv_ver_x = (((s32)ac_mv[2][MV_X] - (s32)ac_mv[0][MV_X]) << 7) >> com_tbl_log2[cu_height]; // deltaMvVer
        dmv_ver_y = (((s32)ac_mv[2][MV_Y] - (s32)ac_mv[0][MV_Y]) << 7) >> com_tbl_log2[cu_height];
    } else {
        dmv_ver_x = -dmv_hor_y;                                                                    // deltaMvVer
        dmv_ver_y = dmv_hor_x;
    }

    int widx = CONV_LOG2(sub_w) - MIN_CU_LOG2;
    int max_posx = pic_w + 4;
    int max_posy = pic_h + 4;

    for (h = 0; h < cu_height; h += sub_h) {
        for (w = 0; w < cu_width; w += sub_w) {
            int pos_x = w + half_w;
            int pos_y = h + half_h;

            if (w == 0 && h == 0) {
                pos_x = 0;
                pos_y = 0;
            } else if (w + sub_w == cu_width && h == 0) {
                pos_x = cu_width;
                pos_y = 0;
            } else if (w == 0 && h + sub_h == cu_height && cp_num == 3) {
                pos_x = 0;
                pos_y = cu_height;
            }

            mv_scale_tmp_hor = mv_scale_hor + dmv_hor_x * pos_x + dmv_ver_x * pos_y;
            mv_scale_tmp_ver = mv_scale_ver + dmv_hor_y * pos_x + dmv_ver_y * pos_y;

            // 1/16 precision, 18 bits for mc
            com_mv_rounding_s32(mv_scale_tmp_hor, mv_scale_tmp_ver, &mv_scale_tmp_hor, &mv_scale_tmp_ver, 7, 0);
            mv_scale_tmp_hor = COM_CLIP3(COM_INT18_MIN, COM_INT18_MAX, mv_scale_tmp_hor);
            mv_scale_tmp_ver = COM_CLIP3(COM_INT18_MIN, COM_INT18_MAX, mv_scale_tmp_ver);

            // save MVF for chroma interpolation
            int w_scu = PEL2SCU(w);
            int h_scu = PEL2SCU(h);
            mv_save[w_scu][h_scu][MV_X] = mv_scale_tmp_hor;
            mv_save[w_scu][h_scu][MV_Y] = mv_scale_tmp_ver;

            if (sub_w == 8 && sub_h == 8) {
                mv_save[w_scu + 1][h_scu][MV_X] = mv_scale_tmp_hor;
                mv_save[w_scu + 1][h_scu][MV_Y] = mv_scale_tmp_ver;
                mv_save[w_scu][h_scu + 1][MV_X] = mv_scale_tmp_hor;
                mv_save[w_scu][h_scu + 1][MV_Y] = mv_scale_tmp_ver;
                mv_save[w_scu + 1][h_scu + 1][MV_X] = mv_scale_tmp_hor;
                mv_save[w_scu + 1][h_scu + 1][MV_Y] = mv_scale_tmp_ver;
            }
            qpel_gmv_x = ((x + w) << 4) + mv_scale_tmp_hor;
            qpel_gmv_y = ((y + h) << 4) + mv_scale_tmp_ver;
            com_mc_blk_luma(ref_pic, pred_y + w, cu_width, qpel_gmv_x, qpel_gmv_y, sub_w, sub_h, widx, max_posx, max_posy, (1 << bit_depth) - 1, 1);
        }
        pred_y += (cu_width * sub_h);
    }

    // get prediction block by block (chroma)
    sub_w = 8;
    sub_h = 8;
    widx = CONV_LOG2(sub_w) - MIN_CU_LOG2;
    max_posx >>= 1;
    max_posy >>= 1;

    for (h = 0; h < cu_height; h += sub_h) {
        for (w = 0; w < cu_width; w += sub_w) {
            int w_scu = PEL2SCU(w);
            int h_scu = PEL2SCU(h);

            mv_scale_tmp_hor = (mv_save[w_scu][h_scu][MV_X] + mv_save[w_scu + 1][h_scu][MV_X] + mv_save[w_scu][h_scu + 1][MV_X] + mv_save[w_scu + 1][h_scu + 1][MV_X] + 2) >> 2;
            mv_scale_tmp_ver = (mv_save[w_scu][h_scu][MV_Y] + mv_save[w_scu + 1][h_scu][MV_Y] + mv_save[w_scu][h_scu + 1][MV_Y] + mv_save[w_scu + 1][h_scu + 1][MV_Y] + 2) >> 2;

            qpel_gmv_x = ((x + w) << 4) + mv_scale_tmp_hor;
            qpel_gmv_y = ((y + h) << 4) + mv_scale_tmp_ver;

            com_mc_blk_chroma(ref_pic, 0, pred_u + (w >> 1), cu_width >> 1, qpel_gmv_x, qpel_gmv_y, sub_w >> 1, sub_h >> 1, widx - 1, max_posx, max_posy, (1 << bit_depth) - 1, 1);
            com_mc_blk_chroma(ref_pic, 1, pred_v + (w >> 1), cu_width >> 1, qpel_gmv_x, qpel_gmv_y, sub_w >> 1, sub_h >> 1, widx - 1, max_posx, max_posy, (1 << bit_depth) - 1, 1);
        }
        pred_u += (cu_width * sub_h) >> 2;
        pred_v += (cu_width * sub_h) >> 2;
    }
}


#define FLT_8TAP_HOR(src, i, coef) ( \
                                     (src)[i-3] * (coef)[0] + \
                                     (src)[i-2] * (coef)[1] + \
                                     (src)[i-1] * (coef)[2] + \
                                     (src)[i  ] * (coef)[3] + \
                                     (src)[i+1] * (coef)[4] + \
                                     (src)[i+2] * (coef)[5] + \
                                     (src)[i+3] * (coef)[6] + \
                                     (src)[i+4] * (coef)[7])

#define FLT_8TAP_VER(src, i, i_src, coef) ( \
        (src)[i-3 * i_src] * (coef)[0] + \
        (src)[i-2 * i_src] * (coef)[1] + \
        (src)[i-1 * i_src] * (coef)[2] + \
        (src)[i          ] * (coef)[3] + \
        (src)[i+1 * i_src] * (coef)[4] + \
        (src)[i+2 * i_src] * (coef)[5] + \
        (src)[i+3 * i_src] * (coef)[6] + \
        (src)[i+4 * i_src] * (coef)[7])

#define FLT_4TAP_HOR(src, i, coef) ( \
                                     (src)[i - 1] * (coef)[0] + \
                                     (src)[i    ] * (coef)[1] + \
                                     (src)[i + 1] * (coef)[2] + \
                                     (src)[i + 2] * (coef)[3])

#define FLT_4TAP_VER(src, i, i_src, coef) ( \
        (src)[i-1 * i_src] * (coef)[0] + \
        (src)[i          ] * (coef)[1] + \
        (src)[i+1 * i_src] * (coef)[2] + \
        (src)[i+2 * i_src] * (coef)[3])


/****************************************************************************
 * interpolation for luma
 ****************************************************************************/

static void mc_if_hor_luma(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, s8 const *coeff, int max_val)
{
    int row, col;
    int sum, val;

    for (row = 0; row < height; row++) {
        for (col = 0; col < width; col++) {
            sum = FLT_8TAP_HOR(src, col, coeff);
            val = (sum + 32) >> 6;
            dst[col] = COM_CLIP3(0, max_val, val);
        }
        src += i_src;
        dst += i_dst;
    }
}

static void mc_if_ver_luma(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, s8 const *coeff, int max_val)
{
    int row, col;
    int sum, val;

    for (row = 0; row < height; row++) {
        for (col = 0; col < width; col++) {
            sum = FLT_8TAP_VER(src, col, i_src, coeff);
            val = (sum + 32) >> 6;
            dst[col] = COM_CLIP3(0, max_val, val);
        }
        src += i_src;
        dst += i_dst;
    }
}

static void mc_if_hor_ver_luma(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff_h, const s8 *coeff_v, int max_val)
{
    int row, col;
    int sum, val;
    int add1, shift1;
    int add2, shift2;

    ALIGNED_16(s16 tmp_res[(128 + 7) * 128]);
    s16 *tmp;

    if (max_val == 255) { // 8 bit_depth
        shift1 = 0;
        shift2 = 12;
    } else { // 10 bit_depth
        shift1 = 2;
        shift2 = 10;
    }

    add1 = (1 << (shift1)) >> 1;
    add2 = 1 << (shift2 - 1);

    src += -3 * i_src;
    tmp = tmp_res;

    if (shift1) {
        for (row = -3; row < height + 4; row++) {
            for (col = 0; col < width; col++) {
                sum = FLT_8TAP_HOR(src, col, coeff_h);
                tmp[col] = (sum + add1) >> shift1;
            }
            src += i_src;
            tmp += width;
        }
    } else {
        for (row = -3; row < height + 4; row++) {
            for (col = 0; col < width; col++) {
                tmp[col] = FLT_8TAP_HOR(src, col, coeff_h);
            }
            src += i_src;
            tmp += width;
        }
    }

    tmp = tmp_res + 3 * width;

    for (row = 0;
         row < height;
         row++) {
        for (col = 0; col < width; col++) {
            sum = FLT_8TAP_VER(tmp, col, width, coeff_v);
            val = (sum + add2) >> shift2;
            dst[col] = COM_CLIP3(0, max_val, val);
        }
        dst += i_dst;
        tmp += width;
    }
}

/****************************************************************************
 * interpolation for chroma
 ****************************************************************************/

static void mc_if_hor_chroma(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, s8 const *coeff, int max_val)
{
    int row, col;
    int sumu, valu;

    for (row = 0; row < height; row++) {
        for (col = 0; col < width; col++) {
            sumu = FLT_4TAP_HOR(src, col, coeff);
            valu = (sumu + 32) >> 6;
            dst[col] = COM_CLIP3(0, max_val, valu);
        }
        src += i_src;
        dst += i_dst;
    }
}

static void mc_if_ver_chroma(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, s8 const *coeff, int max_val)
{
    int row, col;
    int sumu, valu;

    for (row = 0; row < height; row++) {
        for (col = 0; col < width; col++) {
            sumu = FLT_4TAP_VER(src, col, i_src, coeff);
            valu = (sumu + 32) >> 6;
            dst[col] = COM_CLIP3(0, max_val, valu);
        }
        src += i_src;
        dst += i_dst;
    }
}

static void mc_if_hor_ver_chroma(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff_h, const s8 *coeff_v, int max_val)
{
    int row, col;
    int sumu, valu;
    int add1, shift1;
    int add2, shift2;

    ALIGNED_16(s16 tmp_res[(64 + 3) * 64]); // UV interlaced
    s16 *tmp;

    if (max_val == 255) { // 8 bit_depth
        shift1 = 0;
        shift2 = 12;
    } else { // 10 bit_depth
        shift1 = 2;
        shift2 = 10;
    }

    add1 = (1 << (shift1)) >> 1;
    add2 = 1 << (shift2 - 1);

    src += -1 * i_src;
    tmp = tmp_res;

    if (shift1) {
        for (row = -1; row < height + 2; row++) {
            for (col = 0; col < width; col++) {
                sumu = FLT_4TAP_HOR(src, col, coeff_h);
                tmp[col] = (sumu + add1) >> shift1;
            }
            src += i_src;
            tmp += width;
        }
    } else {
        for (row = -1; row < height + 2; row++) {
            for (col = 0; col < width; col++) {
                tmp[col] = FLT_4TAP_HOR(src, col, coeff_h);
            }
            src += i_src;
            tmp += width;
        }
    }

    tmp = tmp_res + 1 * width;

    for (row = 0;
         row < height;
         row++) {
        for (col = 0; col < width; col++) {
            sumu = FLT_4TAP_VER(tmp, col, width, coeff_v);
            valu = (sumu + add2) >> shift2;
            dst[col] = COM_CLIP3(0, max_val, valu);
        }
        dst += i_dst;
        tmp += width;
    }
}

/****************************************************************************
 * copy
 ****************************************************************************/

static void mc_if_cpy(const pel *src, int i_src, pel *dst, int i_dst, int width, int height)
{
    int row;
    for (row = 0; row < height; row++) {
        memcpy(dst, src, sizeof(pel)* width);
        src += i_src;
        dst += i_dst;
    }
}

/****************************************************************************
* interpolation for luma of frame
****************************************************************************/

static  pel pixel_clip(int x, int max_pixel_val)
{
    return (pel)((x < 0) ? 0 : ((x > max_pixel_val) ? max_pixel_val : x));
}

static void com_if_filter_hor_8(const pel *src, int i_src, pel *dst[3], int i_dst, s16 *dst_tmp[3], int i_dst_tmp, int width, int height, s8(*coeff)[8], int bit_depth)
{
    int i, j;
    pel *d0 = dst[0];
    pel *d1 = dst[1];
    pel *d2 = dst[2];
    s16 *dt0 = dst_tmp[0];
    s16 *dt1 = dst_tmp[1];
    s16 *dt2 = dst_tmp[2];
    int shift_tmp = bit_depth - 8;
    int add_tmp = (1 << (shift_tmp)) >> 1;
    int t1, t2, t3;
    int max_pixel = (1 << bit_depth) - 1;

    for (j = 0; j < height; j++) {
        for (i = 0; i < width; i++) {
            t1 = FLT_8TAP_HOR(src, i, coeff[0]);
            dt0[i] = (t1 + add_tmp) >> shift_tmp;
            d0[i] = pixel_clip((t1 + 32) >> 6, max_pixel);
            t2 = FLT_8TAP_HOR(src, i, coeff[1]);
            dt1[i] = (t2 + add_tmp) >> shift_tmp;
            d1[i] = pixel_clip((t2 + 32) >> 6, max_pixel);
            t3 = FLT_8TAP_HOR(src, i, coeff[2]);
            dt2[i] = (t3 + add_tmp) >> shift_tmp;
            d2[i] = pixel_clip((t3 + 32) >> 6, max_pixel);
        }
        d0 += i_dst;
        d1 += i_dst;
        d2 += i_dst;
        dt0 += i_dst_tmp;
        dt1 += i_dst_tmp;
        dt2 += i_dst_tmp;
        src += i_src;
    }
}

static void com_if_filter_ver_8(const pel *src, int i_src, pel *dst[3], int i_dst, int width, int height, s8(*coeff)[8], int bit_depth)
{
    int i, j, val;
    pel *d0 = dst[0];
    pel *d1 = dst[1];
    pel *d2 = dst[2];
    int max_pixel = (1 << bit_depth) - 1;

    for (j = 0; j < height; j++) {
        for (i = 0; i < width; i++) {
            val = (FLT_8TAP_VER(src, i, i_src, coeff[0]) + 32) >> 6;
            d0[i] = pixel_clip(val, max_pixel);

            val = (FLT_8TAP_VER(src, i, i_src, coeff[1]) + 32) >> 6;
            d1[i] = pixel_clip(val, max_pixel);

            val = (FLT_8TAP_VER(src, i, i_src, coeff[2]) + 32) >> 6;
            d2[i] = pixel_clip(val, max_pixel);
        }

        d0 += i_dst;
        d1 += i_dst;
        d2 += i_dst;
        src += i_src;
    }
}

static void uavs3e_if_ver_luma_frame_ext(const s16 *src, int i_src, pel *dst[3], int i_dst, int width, int height, s8(*coeff)[8], int bit_depth)
{
    int i, j, val;
    pel *d0 = dst[0];
    pel *d1 = dst[1];
    pel *d2 = dst[2];
    int max_pixel = (1 << bit_depth) - 1;
    int shift1 = 20 - bit_depth;
    int add1 = 1 << (shift1 - 1);

    for (j = 0; j < height; j++) {
        for (i = 0; i < width; i++) {
            val = (FLT_8TAP_VER(src, i, i_src, coeff[0]) + add1) >> shift1;
            d0[i] = pixel_clip(val, max_pixel);

            val = (FLT_8TAP_VER(src, i, i_src, coeff[1]) + add1) >> shift1;
            d1[i] = pixel_clip(val, max_pixel);

            val = (FLT_8TAP_VER(src, i, i_src, coeff[2]) + add1) >> shift1;
            d2[i] = pixel_clip(val, max_pixel);
        }

        d0 += i_dst;
        d1 += i_dst;
        d2 += i_dst;
        src += i_src;
    }
}

static void picture_padding(pel *a, int s, int width, int height, int exp_hor, int exp_upper, int exp_below)
{
    int i, j;
    pel pixel;
    pel *src, *dst;
    /* left */
    src = a;
    dst = a - exp_hor;
    for (i = 0; i < height; i++) {
        pixel = *src; /* get boundary pixel */
        for (j = 0; j < exp_hor; j++) {
            dst[j] = pixel;
        }
        dst += s;
        src += s;
    }
    /* right */
    src = a + (width - 1);
    dst = a + width;
    for (i = 0; i < height; i++) {
        pixel = *src; /* get boundary pixel */
        for (j = 0; j < exp_hor; j++) {
            dst[j] = pixel;
        }
        dst += s;
        src += s;
    }
    /* upper */
    src = a - exp_upper;
    dst = a - exp_upper - (exp_upper * s);
    for (i = 0; i < exp_upper; i++) {
        com_mcpy(dst, src, s * sizeof(pel));
        dst += s;
    }
    /* below */
    src = a + ((height - 1) * s) - exp_below;
    dst = a + ((height - 1) * s) - exp_below + s;
    for (i = 0; i < exp_below; i++) {
        com_mcpy(dst, src, s * sizeof(pel));
        dst += s;
    }
}
void com_img_padding(COM_PIC *img, int planes, int ext_size)
{
    for (int i = 0; i < planes; i++) {
        int stride = img->imgb->stride[i] / sizeof(pel);
        pel *p = (pel*)img->imgb->addr_plane[i] - ext_size * stride - ext_size;

        picture_padding(p, stride,
            img->imgb->width[i] + ext_size * 2,
            img->imgb->height[i] + ext_size * 2,
            img->imgb->pad_left[i] - ext_size,
            img->imgb->pad_up[i] - ext_size,
            img->imgb->pad_down[i] - ext_size);

        if (i == 0) {
            ext_size >>= 1;
        }
    }
}
void com_if_luma_frame(COM_PIC *img_list[4][4], s16 *tmp_buf[3], int bit_depth)
{
    int dx, dy;
    COM_PIC *img = img_list[0][0];
    int i_stride   = img->imgb->stride[0] / sizeof(pel);
    int tmp_stride = img->imgb->width[0] + 16;
    pel *dst_tmp[3];
    s8(*coefs)[8] = (s8(*)[8])com_tbl_mc_l_coeff[1];

    int ip_width = img->imgb->width[0] + 8;
    int ip_height = img->imgb->height[0] + 8;

    //horizontal positions: a,1,b
    dst_tmp[0] = (pel*)img_list[0][1]->imgb->addr_plane[0] - 8 * i_stride - 8;
    dst_tmp[1] = (pel*)img_list[0][2]->imgb->addr_plane[0] - 8 * i_stride - 8;
    dst_tmp[2] = (pel*)img_list[0][3]->imgb->addr_plane[0] - 8 * i_stride - 8;
    avs3_funs_handle.ip_flt_y_hor((pel*)img_list[0][0]->imgb->addr_plane[0] - 8 * i_stride - 8, i_stride, dst_tmp, i_stride, tmp_buf, tmp_stride, img->imgb->width[0] + 16, img->imgb->height[0] + 16, coefs, bit_depth);

    //vertical positions: c,2,j
    dst_tmp[0] = (pel*)img_list[1][0]->imgb->addr_plane[0] - 4 * i_stride - 4;
    dst_tmp[1] = (pel*)img_list[2][0]->imgb->addr_plane[0] - 4 * i_stride - 4;
    dst_tmp[2] = (pel*)img_list[3][0]->imgb->addr_plane[0] - 4 * i_stride - 4;
    avs3_funs_handle.ip_flt_y_ver((pel*)img_list[0][0]->imgb->addr_plane[0] - 4 * i_stride - 4, i_stride, dst_tmp, i_stride, ip_width, ip_height, coefs, bit_depth);

    //vertical positions: d,h,k; e,3,1; f,i,m
    for (dx = 1; dx < 4; dx++) {
        dst_tmp[0] = (pel*)img_list[1][dx]->imgb->addr_plane[0] - 4 * i_stride - 4;
        dst_tmp[1] = (pel*)img_list[2][dx]->imgb->addr_plane[0] - 4 * i_stride - 4;
        dst_tmp[2] = (pel*)img_list[3][dx]->imgb->addr_plane[0] - 4 * i_stride - 4;
        avs3_funs_handle.ip_flt_y_ver_ext(tmp_buf[dx - 1] + 4 * tmp_stride + 4, tmp_stride, dst_tmp, i_stride, ip_width, ip_height, coefs, bit_depth);
    }

    for (dx = 0; dx < 4; dx++) {
        for (dy = 0; dy < 4; dy++) {
            if (dx == 0 && dy == 0) {
                continue;
            }
            com_img_padding(img_list[dx][dy], 1, 4);
        }
    }
}

void uavs3e_funs_init_mc_c()
{
    int i;

    avs3_funs_handle.ip_flt_y_hor = com_if_filter_hor_8;
    avs3_funs_handle.ip_flt_y_ver = com_if_filter_ver_8;
    avs3_funs_handle.ip_flt_y_ver_ext = uavs3e_if_ver_luma_frame_ext;

    for (i = 0; i < CU_SIZE_NUM_RC; i++) {
        avs3_funs_handle.ipcpy[i] = mc_if_cpy;
        avs3_funs_handle.ipflt[IPFILTER_H_8][i] = mc_if_hor_luma;
        avs3_funs_handle.ipflt[IPFILTER_H_4][i] = mc_if_hor_chroma;
        avs3_funs_handle.ipflt[IPFILTER_V_8][i] = mc_if_ver_luma;
        avs3_funs_handle.ipflt[IPFILTER_V_4][i] = mc_if_ver_chroma;
        avs3_funs_handle.ipflt_ext[IPFILTER_EXT_8][i] = mc_if_hor_ver_luma;
        avs3_funs_handle.ipflt_ext[IPFILTER_EXT_4][i] = mc_if_hor_ver_chroma;
    }
}