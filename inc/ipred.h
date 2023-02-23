#ifndef _IPRED_H_
#define _IPRED_H_
#include "com_def.h"
#include "enc_def.h"
void diff_16b(int w, int h, s16 *src1, s16 *src2, int s_src1, int s_src2, int s_diff, s16 *diff);
void enc_diff_16b(int log2w, int log2h, s16 *src1, s16 *src2, int s_src1, int s_src2, int s_diff, s16 *diff);
void enc_diff_8b(int log2w, int log2h, u8 *src1, pel *src2, int s_src1, int s_src2, int s_diff, s16 *diff);
void enc_diff(int log2w, int log2h, u8 *src1, pel *src2, int s_src1, int s_src2, int s_diff, s16 *diff);
void diff_8b(int w, int h, u8 *src1, pel *src2, int s_src1, int s_src2, int s_diff, s16 *diff);
void diff1(int w, int h, u8 *src1, pel *src2, int s_src1, int s_src2, int s_diff, s16 *diff);

u16 com_get_avail_intra_ip(int x_scu, int y_scu);


template<const int W, const int H>
void ipred_luma1(u8 orgY[H][W], pel pred_cache[H][W], s16 coef[H][W], s8 intra_mode,
    u8 src_le[2 * H + 3], u8 src_up[2 * W + 3], u16 avail_tb);

void ipred_luma(u8 orgY_16x16[64][64], pel pred_cache_16x16[64][64], s16 coef_tmp_16x16[64][64], s8 intra_mode, u8 src_le_temp[128 + 3], u8 src_up_temp[128 + 3], u16 avail_tb, int cu_width_log2, int cu_height_log2);
void ipred_8x4(u8 orgY_8x8[64][64], pel pred_cache_8x8[64][64], s16 coef_tmp_8x8[64][64], s8 intra_mode, u8 src_le_temp[128 + 3], u8 src_up_temp[128 + 3], u16 avail_tb);
void ipred_4x8(u8 orgY_8x8[64][64], pel pred_cache_8x8[64][64], s16 coef_tmp_8x8[64][64], s8 intra_mode, u8 src_le_temp[128 + 3], u8 src_up_temp[128 + 3], u16 avail_tb);
void ipred_8x8(u8 orgY_8x8[64][64], pel pred_cache_8x8[64][64], s16 coef_tmp_8x8[64][64], s8 intra_mode, u8 src_le_temp[128 + 3], u8 src_up_temp[128 + 3], u16 avail_tb);

// inner interface between ipred_c
template<const int W, const int H>
void ipred_vert_1(u8* src_up, u8 orgY[H][W], pel pred_cache[H][W], s16 coef_tmp[H][W]);
template<const int W, const int H>
void ipred_hor_1(u8* src_le, u8 orgY[H][W], pel pred_cache[H][W], s16 coef_tmp[H][W]);
template<const int W, const int H>
void ipred_dc_1(u8* src_le, u8* src_up, int bit_depth, u16 avail_cu, u8 orgY[][W], pel pred_cache[][W], s16 coef_tmp[][W]);
template<const int W, const int H>
void ipred_plane_1(const u8* src_le, const u8* src_up, const u8 orgY[H][W],
    pel pred_cache[H][W], s16 coef[H][W]);
template<const int W, const int H>
void ipred_bi_1(u8* src_le, u8* src_up, u8 orgY[H][W], pel pred_cache[H][W], s16 coef_tmp[H][W]);
template<const int W, const int H>
void ipred_ang_1(u8* src_le, u8* src_up, s8 intra_mode, u8 orgY[][W], pel pred_cache[][W], s16 coef[][W]);
#endif // _IPRED_H_