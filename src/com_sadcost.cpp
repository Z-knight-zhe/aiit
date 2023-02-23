/**************************************************************************************
 * Copyright (c) 2018-2020 ["Peking University Shenzhen Graduate School",
 *   "Peng Cheng Laboratory", and "Guangdong Bohua UHD Innovation Corporation"]
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes the software uAVS3d developed by
 *    Peking University Shenzhen Graduate School, Peng Cheng Laboratory
 *    and Guangdong Bohua UHD Innovation Corporation.
 * 4. Neither the name of the organizations (Peking University Shenzhen Graduate School,
 *    Peng Cheng Laboratory and Guangdong Bohua UHD Innovation Corporation) nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * For more information, contact us at rgwang@pkusz.edu.cn.
 **************************************************************************************/


#include "com_port.h"
#include "com_def.h"
#include "com_define.h"
#include "iime.h"


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
void com_mc_blk_luma(COM_PIC *pic, pel *dst, int dst_stride, int x_pos, int y_pos, int width, int height, int widx, int max_posx, int max_posy, int max_val, int hp_flag);

#define DEFINE_SAD(w) \
static u32 com_get_sad_##w(pel *p_org, int i_org, pel *p_pred, int i_pred, int height) { \
    u32 uiSum = 0;                                                                       \
    while (height--) {                                                                   \
        for (int i = 0; i < w; i++) {                                                    \
            uiSum += abs(p_org[i] - p_pred[i]);                                          \
        }                                                                                \
        p_org  += i_org;                                                                 \
        p_pred += i_pred;                                                                \
    }                                                                                    \
    return uiSum;                                                                        \
}

DEFINE_SAD(4)
DEFINE_SAD(8)
DEFINE_SAD(16)
DEFINE_SAD(32)
DEFINE_SAD(64)
DEFINE_SAD(128)

#define DEFINE_SAD_X3(w) \
static void com_get_sad_x3_##w(pel *p_org, int i_org, pel *pred0, pel *pred1, pel *pred2,    \
                                                      int i_pred, u32 sad[3], int height) {  \
    u32 uiSum0 = 0, uiSum1 = 0, uiSum2 = 0;                                                  \
    while (height--) {                                                                       \
        for (int i = 0; i < w; i++) {                                                        \
            uiSum0 += abs(p_org[i] - pred0[i]);                                              \
            uiSum1 += abs(p_org[i] - pred1[i]);                                              \
            uiSum2 += abs(p_org[i] - pred2[i]);                                              \
        }                                                                                    \
        p_org += i_org;                                                                      \
        pred0 += i_pred;                                                                     \
        pred1 += i_pred;                                                                     \
        pred2 += i_pred;                                                                     \
    }                                                                                        \
    sad[0] = uiSum0;                                                                         \
    sad[1] = uiSum1;                                                                         \
    sad[2] = uiSum2;                                                                         \
}

DEFINE_SAD_X3(4)
DEFINE_SAD_X3(8)
DEFINE_SAD_X3(16)
DEFINE_SAD_X3(32)
DEFINE_SAD_X3(64)
DEFINE_SAD_X3(128)

#define DEFINE_SAD_X4(w) \
static void com_get_sad_x4_##w(pel *p_org, int i_org, pel *pred0, pel *pred1, pel *pred2, pel *pred3,    \
                                                                  int i_pred, u32 sad[4], int height) {  \
    u32 uiSum0 = 0, uiSum1 = 0, uiSum2 = 0, uiSum3 = 0;                                                  \
    while (height--) {                                                                                   \
        for (int i = 0; i < w; i++) {                                                                    \
            uiSum0 += abs(p_org[i] - pred0[i]);                                                          \
            uiSum1 += abs(p_org[i] - pred1[i]);                                                          \
            uiSum2 += abs(p_org[i] - pred2[i]);                                                          \
            uiSum3 += abs(p_org[i] - pred3[i]);                                                          \
        }                                                                                                \
        p_org += i_org;                                                                                  \
        pred0 += i_pred;                                                                                 \
        pred1 += i_pred;                                                                                 \
        pred2 += i_pred;                                                                                 \
        pred3 += i_pred;                                                                                 \
    }                                                                                                    \
    sad[0] = uiSum0;                                                                                     \
    sad[1] = uiSum1;                                                                                     \
    sad[2] = uiSum2;                                                                                     \
    sad[3] = uiSum3;                                                                                     \
}



DEFINE_SAD_X4(4)
DEFINE_SAD_X4(8)
DEFINE_SAD_X4(16)
DEFINE_SAD_X4(32)
DEFINE_SAD_X4(64)
DEFINE_SAD_X4(128)

#define DEFINE_SSD(w) \
static u64 com_get_ssd_##w(pel *p_org, int i_org, pel *p_pred, int i_pred, int height) { \
    int i;                                                                               \
    u64 uiSum = 0;                                                                       \
    for (; height != 0; height--) {                                                      \
        for (i = 0; i < w; i++) {                                                        \
            int ssd = abs(p_org[i] - p_pred[i]);                                         \
            uiSum += (ssd * ssd);                                                        \
        }                                                                                \
        p_org += i_org;                                                                  \
        p_pred += i_pred;                                                                \
    }                                                                                    \
    return uiSum;                                                                        \
}

DEFINE_SSD(4)
DEFINE_SSD(8)
DEFINE_SSD(16)
DEFINE_SSD(32)
DEFINE_SSD(64)
DEFINE_SSD(128)

#define DEFINE_VAR(w)                                                             \
static u64 com_get_var_##w(pel *p_org, int i_org) {                               \
    int i, height = w;                                                            \
    u64 uiSum = 0, uiSSD = 0;                                                     \
    for (; height != 0; height--) {                                               \
        for (i = 0; i < w; i++) {                                                 \
            uiSum += p_org[i];                                                    \
            uiSSD += (p_org[i] * p_org[i]);                                       \
        }                                                                         \
        p_org += i_org;                                                           \
    }                                                                             \
    return uiSSD - ((uiSum * uiSum) >> (CONV_LOG2(w) * 2));                       \
}

DEFINE_VAR(4)
DEFINE_VAR(8)
DEFINE_VAR(16)
DEFINE_VAR(32)
DEFINE_VAR(64)
DEFINE_VAR(128)

static u64 com_var_cost(const pel* pix, int i_pix, int width, int height)
{
    u64 sum = 0, sqr = 0;
    int x, y;

    for (y = 0; y < width; y++) {
        for (x = 0; x < height; x++) {
            sum += pix[x];
            sqr += pix[x] * pix[x];
        }
        pix += i_pix;
    }
    return sqr - (sum * sum >> 8);
}

static void ssim_4x4x2_core(const pel *pix1, int stride1, const pel *pix2, int stride2, int sums[2][4])
{
    int x, y, z;
    for (z = 0; z < 2; z++) {
        int s1 = 0, s2 = 0, ss = 0, s12 = 0;
        for (y = 0; y < 4; y++) {
            for (x = 0; x < 4; x++) {
                int a = pix1[x + y * stride1];
                int b = pix2[x + y * stride2];
                s1 += a;
                s2 += b;
                ss += a * a;
                ss += b * b;
                s12 += a * b;
            }
        }
        sums[z][0] = s1;
        sums[z][1] = s2;
        sums[z][2] = ss;
        sums[z][3] = s12;
        pix1 += 4;
        pix2 += 4;
    }
}

static float ssim_end1(int s1, int s2, int ss, int s12, float ssim_c1, float ssim_c2)
{
    float fs1 = (float)s1;
    float fs2 = (float)s2;
    float fss = (float)ss;
    float fs12 = (float)s12;
    float vars = (float)(fss * 64 - fs1 * fs1 - fs2 * fs2);
    float covar = (float)(fs12 * 64 - fs1 * fs2);
    return (float)(2 * fs1 * fs2 + ssim_c1) * (float)(2 * covar + ssim_c2) / ((float)(fs1 * fs1 + fs2 * fs2 + ssim_c1) * (float)(vars + ssim_c2));
}

static float ssim_end4(int sum0[5][4], int sum1[5][4], int width, float ssim_c1, float ssim_c2)
{
    float ssim = 0.0;
    int i;
    for (i = 0; i < width; i++)
        ssim += ssim_end1(sum0[i][0] + sum0[i + 1][0] + sum1[i][0] + sum1[i + 1][0],
            sum0[i][1] + sum0[i + 1][1] + sum1[i][1] + sum1[i + 1][1],
            sum0[i][2] + sum0[i + 1][2] + sum1[i][2] + sum1[i + 1][2],
            sum0[i][3] + sum0[i + 1][3] + sum1[i][3] + sum1[i + 1][3], ssim_c1, ssim_c2);
    return ssim;
}

static void sobel_cost(pel *pix, int i_pel, int width, int height, int *cost_ver, int *cost_hor)
{
    int ver = 0, hor = 0;
    int sum_ver = 0, sum_hor = 0;

    pel* p_u = pix - i_pel;
    pel* p_c = p_u + i_pel;
    pel* p_d = p_c + i_pel;

    for (int i = 0; i < height / 2; i++) {
        for (int j = 0; j < width / 2; j++) {
            int a = abs(p_u[j + 1] + (p_c[j + 1] << 1) + p_d[j + 1] - (p_u[j - 1] + (p_c[j - 1] << 1) + p_d[j - 1])); // ver edge 
            int b = abs(p_u[j - 1] + (p_u[j] << 1) + p_u[j + 1] - (p_d[j - 1] + (p_d[j] << 1) + p_d[j + 1])); // hor edge

            ver += COM_MIN(a, 255);
            hor += COM_MIN(b, 255);
        }
        p_u += i_pel;
        p_c += i_pel;
        p_d += i_pel;
    }
    sum_ver += ver;
    sum_hor += hor;
    *(cost_ver + 1) = ver;
    *(cost_hor + 1) = hor;
    ver = 0;
    hor = 0;

    p_u = pix - i_pel;
    p_c = p_u + i_pel;
    p_d = p_c + i_pel;

    for (int i = 0; i < height / 2; i++) {
        for (int j = width / 2; j < width; j++) {
            int a = abs(p_u[j + 1] + (p_c[j + 1] << 1) + p_d[j + 1] - (p_u[j - 1] + (p_c[j - 1] << 1) + p_d[j - 1])); // ver edge 
            int b = abs(p_u[j - 1] + (p_u[j] << 1) + p_u[j + 1] - (p_d[j - 1] + (p_d[j] << 1) + p_d[j + 1])); // hor edge

            ver += COM_MIN(a, 255);
            hor += COM_MIN(b, 255);
        }
        p_u += i_pel;
        p_c += i_pel;
        p_d += i_pel;
    }
    sum_ver += ver;
    sum_hor += hor;
    *(cost_ver + 2) = ver;
    *(cost_hor + 2) = hor;
    ver = 0;
    hor = 0;


    for (int i = height / 2; i < height; i++) {
        for (int j = 0; j < width / 2; j++) {
            int a = abs(p_u[j + 1] + (p_c[j + 1] << 1) + p_d[j + 1] - (p_u[j - 1] + (p_c[j - 1] << 1) + p_d[j - 1])); // ver edge 
            int b = abs(p_u[j - 1] + (p_u[j] << 1) + p_u[j + 1] - (p_d[j - 1] + (p_d[j] << 1) + p_d[j + 1])); // hor edge

            ver += COM_MIN(a, 255);
            hor += COM_MIN(b, 255);
        }
        p_u += i_pel;
        p_c += i_pel;
        p_d += i_pel;
    }
    sum_ver += ver;
    sum_hor += hor;
    *(cost_ver + 3) = ver;
    *(cost_hor + 3) = hor;
    ver = 0;
    hor = 0;

    p_u = pix - i_pel + height / 2 * i_pel;
    p_c = p_u + i_pel;
    p_d = p_c + i_pel;

    for (int i = height / 2; i < height; i++) {
        for (int j = width / 2; j < width; j++) {
            int a = abs(p_u[j + 1] + (p_c[j + 1] << 1) + p_d[j + 1] - (p_u[j - 1] + (p_c[j - 1] << 1) + p_d[j - 1])); // ver edge 
            int b = abs(p_u[j - 1] + (p_u[j] << 1) + p_u[j + 1] - (p_d[j - 1] + (p_d[j] << 1) + p_d[j + 1])); // hor edge

            ver += COM_MIN(a, 255);
            hor += COM_MIN(b, 255);
        }
        p_u += i_pel;
        p_c += i_pel;
        p_d += i_pel;
    }
    sum_ver += ver;
    sum_hor += hor;
    *(cost_ver + 4) = ver;
    *(cost_hor + 4) = hor;
    ver = sum_ver;
    hor = sum_hor;

    *cost_ver = ver;
    *cost_hor = hor;
}
typedef int(*parr4)[4];
float com_ssim_img_plane(pel *pix1, int stride1, pel *pix2, int stride2, int width, int height, int *cnt, int bit_depth)
{
#define MAX_PIC_WIDTH (8192*2)

    static int buf[2 * (MAX_PIC_WIDTH / 4 + 3)][4];
    int x, y, z = 0;
    float ssim = 0.0;
    int(*sum0)[4] = buf;
    int(*sum1)[4] = sum0 + (width >> 2) + 3;

    width >>= 2;
    height >>= 2;

    /* Maximum value for 10-bit is: ss*64 = (2^10-1)^2*16*4*64 = 4286582784, which will overflow in some cases.
    * s1*s1, s2*s2, and s1*s2 also obtain this value for edge cases: ((2^10-1)*16*4)^2 = 4286582784.
    * Maximum value for 9-bit is: ss*64 = (2^9-1)^2*16*4*64 = 1069551616, which will not overflow. */
    int pixel_max = bit_depth == 8 ? 255 : 1023;
    float ssim_c1 = (float)(.01 * .01 * pixel_max * pixel_max * 64);
    float ssim_c2 = (float)(.03 * .03 * pixel_max * pixel_max * 64 * 63);

#define XCHG(type,a,b) do{ type t = a; a = b; b = t; } while(0)
   
    for (y = 1; y < height; y++) {
        for (; z <= y; z++) {
            XCHG(parr4, sum0, sum1);
            for (x = 0; x < width; x += 2)
                avs3_funs_handle.ssim_4x4x2_core(&pix1[4 * (x + z * stride1)], stride1, &pix2[4 * (x + z * stride2)], stride2, &sum0[x]);
        }
        for (x = 0; x < width - 1; x += 4)
            ssim += avs3_funs_handle.ssim_end4(sum0 + x, sum1 + x, COM_MIN(4, width - x - 1), ssim_c1, ssim_c2);
    }
    *cnt = (height - 1) * (width - 1);
    return ssim;
}





void avs3_funs_init_cost_c()
{
    avs3_funs_handle.cost_sad[0] = com_get_sad_4;
    avs3_funs_handle.cost_sad[1] = com_get_sad_8;
    avs3_funs_handle.cost_sad[2] = com_get_sad_16;
    avs3_funs_handle.cost_sad[3] = com_get_sad_32;
    avs3_funs_handle.cost_sad[4] = com_get_sad_64;
    avs3_funs_handle.cost_sad[5] = com_get_sad_128;

    avs3_funs_handle.cost_sad_x3[0] = com_get_sad_x3_4;
    avs3_funs_handle.cost_sad_x3[1] = com_get_sad_x3_8;
    avs3_funs_handle.cost_sad_x3[2] = com_get_sad_x3_16;
    avs3_funs_handle.cost_sad_x3[3] = com_get_sad_x3_32;
    avs3_funs_handle.cost_sad_x3[4] = com_get_sad_x3_64;
    avs3_funs_handle.cost_sad_x3[5] = com_get_sad_x3_128;

    avs3_funs_handle.cost_sad_x4[0] = com_get_sad_x4_4;
    avs3_funs_handle.cost_sad_x4[1] = com_get_sad_x4_8;
    avs3_funs_handle.cost_sad_x4[2] = com_get_sad_x4_16;
    avs3_funs_handle.cost_sad_x4[3] = com_get_sad_x4_32;
    avs3_funs_handle.cost_sad_x4[4] = com_get_sad_x4_64;
    avs3_funs_handle.cost_sad_x4[5] = com_get_sad_x4_128;

    avs3_funs_handle.cost_ssd[0] = com_get_ssd_4;
    avs3_funs_handle.cost_ssd[1] = com_get_ssd_8;
    avs3_funs_handle.cost_ssd[2] = com_get_ssd_16;
    avs3_funs_handle.cost_ssd[3] = com_get_ssd_32;
    avs3_funs_handle.cost_ssd[4] = com_get_ssd_64;
    avs3_funs_handle.cost_ssd[5] = com_get_ssd_128;

    avs3_funs_handle.cost_satd[0][0] = com_had_4x4;
    avs3_funs_handle.cost_satd[1][0] = com_had_8x4;
    avs3_funs_handle.cost_satd[0][1] = com_had_4x8;
    avs3_funs_handle.cost_satd[1][1] = com_had_8x8;
    avs3_funs_handle.cost_satd[2][1] = com_had_16x8;
    avs3_funs_handle.cost_satd[1][2] = com_had_8x16;

    avs3_funs_handle.cost_var[0] = com_get_var_4;
    avs3_funs_handle.cost_var[1] = com_get_var_8;
    avs3_funs_handle.cost_var[2] = com_get_var_16;
    avs3_funs_handle.cost_var[3] = com_get_var_32;
    avs3_funs_handle.cost_var[4] = com_get_var_64;
    avs3_funs_handle.cost_var[5] = com_get_var_128;

    avs3_funs_handle.ssim_4x4x2_core = ssim_4x4x2_core;
    avs3_funs_handle.ssim_end4 = ssim_end4;

    avs3_funs_handle.sobel_cost = sobel_cost;

   
}