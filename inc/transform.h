#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_
#include "com_def.h"
#include "enc_def.h"
//#include "md1.h"

extern s8 com_tbl_tm16[NUM_TRANS_TYPE][16][16];

int com_get_forward_trans_shift(int log2_size, int type, int bit_depth);

int com_get_inverse_trans_shift(int log2_size, int type, int bit_depth);

//void memcpy_rewrintra(s16* des, s16* src, int n);
//void memcpy_rewrintra_u8(u8* des, u8* src, int n);
void itx_dct2_pb64(s16* coeff, s16* block, int shift, int line, int max_tr_val, int min_tr_val, int size);

void com_itrans_v(s16* coef_dq, s16* coef_temp, int cu_width_log2, int cu_height_log2, int bit_depth);

void com_itrans_h(s16* resi, s16* coef_temp, int cu_width_log2, int cu_height_log2, int bit_depth);


template <const int W, const int H>
void com_recon_t(s16 resi[H][W], pel pred[H][W], int is_coef, int cu_width, int cu_height,
    int s_rec, pel rec[H][W], int bit_depth)
{
    int i;
    int j;
    s16 t0;

    if (is_coef == 0) {
        for (i = 0; i < cu_height; i++) {
            for (j = 0; j < cu_width; j++) {
                rec[i][j] = ((pel)((0 > (((1 << bit_depth) - 1 < ((int)pred[i][j]) ? (1 << bit_depth) - 1 : ((
                    int)pred[i][j]))) ? 0 : (((1 << bit_depth) - 1 < ((int)pred[i][j]) ? (1 << bit_depth) - 1 : ((
                        int)pred[i][j]))))));
            }
        }
    }
    else
    {
        for (i = 0; i < cu_height; i++) {
            for (j = 0; j < cu_width; j++) {
                t0 = ((s16)(((int)resi[i][j]) + ((int)pred[i][j])));
                rec[i][j] = ((pel)((0 > (((1 << bit_depth) - 1 < ((int)t0) ? (1 << bit_depth) - 1 : ((int)t0))) ? 0 : (((
                    1 << bit_depth) - 1 < ((int)t0) ? (1 << bit_depth) - 1 : ((int)t0))))));
            }
        }
    }
}

template < const int W, const int H>
s64 enc_ssd_16b_t(s16 src1[H][W], u8 src2[H][W], int bit_depth)
{
    const int shift = (bit_depth - 8) << 1;
    s64 ssd = 0;
    int i, j;
    int diff;

    for (i = 0; i < H; i++) {
        for (j = 0; j < W; j++) {
            diff = ((int)src1[i][j]) - ((int)src2[i][j]);
            ssd += ((diff * diff >> shift));
        }
    }
    return ssd;
}


//-------------------------------------------------------------------------
// template
//-------------------------------------------------------------------------
template <const int W, const int H,
    void(*f_dct2)(s16 coef_in[H][W], s16 coef_out[H][W], int shift)>
    void transform_h_t(int bit_depth, int width_log2, s16 coef_in[H][W], s16 coef_out[H][W])
{
    int shift1;
    shift1 = com_get_forward_trans_shift(width_log2, 0, bit_depth);
    f_dct2(coef_in, coef_out, shift1);
}

template <const int W, const int H,
    void(*f_dct2)(s16 coef_in[H][W], s16 coef_out[H][W], int shift)>
    void transform_v_t(int bit_depth, int height_log2, s16 coef_in[H][W], s16 coef_out[H][W])
{
    int shift2;
    shift2 = com_get_forward_trans_shift(height_log2, 1, bit_depth);
    f_dct2(coef_in, coef_out, shift2);
}

template <const int W, const int H,
    void(*f_dct2)(s16 coef_in[H][W], s16 coef_out[H][W], int shift)>
    void inv_transform_v_t(int bit_depth, int num_nz, s16 coef_in[H][W], s16 coef_out[H][W])
{
    int shift1;
    if (num_nz) {
        shift1 = com_get_inverse_trans_shift(3, 0, bit_depth);
        f_dct2(coef_in, coef_out, shift1);
    }
    else {
        for (int i = 0; i < H; i++)
            for (int j = 0; j < W; j++)
                coef_out[i][j] = 0;
    }
}

template <const int W, const int H,
    void(*f_dct2)(s16 coef_in[H][W], s16 coef_out[H][W], int shift)>
    void inv_transform_h_t(int bit_depth, int num_nz, pel pred_cache[H][W], u8 org[H][W], s16 coef_in[H][W], s32* dist, pel rec[H][W])
{
    s16 coef_out[H][W];
    int shift2;
    if (num_nz) {
        shift2 = com_get_inverse_trans_shift(3, 1, bit_depth);
        f_dct2(coef_in, coef_out, shift2);
    }
    else {
        for (int i = 0; i < H; i++)
            for (int j = 0; j < W; j++)
                coef_out[i][j] = 0;
    }
    com_recon_t<W, H>(coef_out, pred_cache, num_nz, W, H, W, rec, bit_depth);
    *dist = (s32)enc_ssd_16b_t<W, H>(rec, org, bit_depth);
}

//-------------------------------------------------------------------------
// DCT 4x4
//-------------------------------------------------------------------------
void tx_dct2_pb4x4_row(s16 src[4][4], s16 dst[4][4], int shift);

void tx_dct2_pb4x4_col(s16 src[4][4], s16 dst[4][4], int shift);

void itx_dct2_pb4x4_col(s16 src[4][4], s16 dst[4][4], int shift);

void itx_dct2_pb4x4_row(s16 src[4][4], s16 dst[4][4], int shift);

//-------------------------------------------------------------------------
// DCT 8x4
//-------------------------------------------------------------------------
void tx_dct2_pb8x4_row(s16 src[4][8], s16 dst[4][8], int shift);

void tx_dct2_pb8x4_col(s16 src[4][8], s16 dst[4][8], int shift);

void itx_dct2_pb8x4_col(s16 src[4][8], s16 dst[4][8], int shift);

void itx_dct2_pb8x4_row(s16 src[4][8], s16 dst[4][8], int shift);

//-------------------------------------------------------------------------
// DCT 4x8
//-------------------------------------------------------------------------
void tx_dct2_pb4x8_row(s16 src[8][4], s16 dst[8][4], int shift);

void tx_dct2_pb4x8_col(s16 src[8][4], s16 dst[8][4], int shift);

void itx_dct2_pb4x8_col(s16 src[8][4], s16 dst[8][4], int shift);

void itx_dct2_pb4x8_row(s16 src[8][4], s16 dst[8][4], int shift);

//-------------------------------------------------------------------------
// DCT 8x8
//-------------------------------------------------------------------------
void tx_dct2_pb8x8_row(s16 src[8][8], s16 dst[8][8], int shift);

void tx_dct2_pb8x8_col(s16 src[8][8], s16 dst[8][8], int shift);

void itx_dct2_pb8x8_col(s16 src[8][8], s16 dst[8][8], int shift);

void itx_dct2_pb8x8_row(s16 src[8][8], s16 dst[8][8], int shift);

//-------------------------------------------------------------------------
// DCT 16x4
//-------------------------------------------------------------------------
void tx_dct2_pb16x4_row(s16 src[4][16], s16 dst[4][16], int shift);

void tx_dct2_pb16x4_col(s16 src[4][16], s16 dst[4][16], int shift);

void itx_dct2_pb16x4_col(s16 src[4][16], s16 dst[4][16], int shift);

void itx_dct2_pb16x4_row(s16 src[4][16], s16 dst[4][16], int shift);

//-------------------------------------------------------------------------
// DCT 4x16
//-------------------------------------------------------------------------
void tx_dct2_pb4x16_row(s16 src[16][4], s16 dst[16][4], int shift);

void tx_dct2_pb4x16_col(s16 src[16][4], s16 dst[16][4], int shift);

void itx_dct2_pb4x16_col(s16 src[16][4], s16 dst[16][4], int shift);

void itx_dct2_pb4x16_row(s16 src[16][4], s16 dst[16][4], int shift);

//-------------------------------------------------------------------------
// DCT 16x8
//-------------------------------------------------------------------------
void tx_dct2_pb16x8_row(s16 src[8][16], s16 dst[8][16], int shift);

void tx_dct2_pb16x8_col(s16 src[8][16], s16 dst[8][16], int shift);

void itx_dct2_pb16x8_col(s16 src[8][16], s16 dst[8][16], int shift);

void itx_dct2_pb16x8_row(s16 src[8][16], s16 dst[8][16], int shift);

//-------------------------------------------------------------------------
// DCT 8x16
//-------------------------------------------------------------------------
void tx_dct2_pb8x16_row(s16 src[16][8], s16 dst[16][8], int shift);

void tx_dct2_pb8x16_col(s16 src[16][8], s16 dst[16][8], int shift);

void itx_dct2_pb8x16_col(s16 src[16][8], s16 dst[16][8], int shift);

void itx_dct2_pb8x16_row(s16 src[16][8], s16 dst[16][8], int shift);

//-------------------------------------------------------------------------
// DCT 16x16
//-------------------------------------------------------------------------
void tx_dct2_pb16x16_row(s16 src[16][16], s16 dst[16][16], int shift);

void tx_dct2_pb16x16_col(s16 src[16][16], s16 dst[16][16], int shift);

void itx_dct2_pb16x16_col(s16 src[16][16], s16 dst[16][16], int shift);

void itx_dct2_pb16x16_row(s16 src[16][16], s16 dst[16][16], int shift);

//-------------------------------------------------------------------------
// DCT 32x4
//-------------------------------------------------------------------------
void tx_dct2_pb32x4_row(s16 src[4][32], s16 dst[4][32], int shift);

void tx_dct2_pb32x4_col(s16 src[4][32], s16 dst[4][32], int shift);

void itx_dct2_pb32x4_col(s16 src[4][32], s16 dst[4][32], int shift);

void itx_dct2_pb32x4_row(s16 src[4][32], s16 dst[4][32], int shift);

//-------------------------------------------------------------------------
// DCT 4x32
//-------------------------------------------------------------------------
void tx_dct2_pb4x32_row(s16 src[32][4], s16 dst[32][4], int shift);

void tx_dct2_pb4x32_col(s16 src[32][4], s16 dst[32][4], int shift);

void itx_dct2_pb4x32_col(s16 src[32][4], s16 dst[32][4], int shift);

void itx_dct2_pb4x32_row(s16 src[32][4], s16 dst[32][4], int shift);

//-------------------------------------------------------------------------
// DCT 32x8
//-------------------------------------------------------------------------
void tx_dct2_pb32x8_row(s16 src[8][32], s16 dst[8][32], int shift);

void tx_dct2_pb32x8_col(s16 src[8][32], s16 dst[8][32], int shift);

void itx_dct2_pb32x8_col(s16 src[8][32], s16 dst[8][32], int shift);

void itx_dct2_pb32x8_row(s16 src[8][32], s16 dst[8][32], int shift);

//-------------------------------------------------------------------------
// DCT 8x32
//-------------------------------------------------------------------------
void tx_dct2_pb8x32_row(s16 src[32][8], s16 dst[32][8], int shift);

void tx_dct2_pb8x32_col(s16 src[32][8], s16 dst[32][8], int shift);

void itx_dct2_pb8x32_col(s16 src[32][8], s16 dst[32][8], int shift);

void itx_dct2_pb8x32_row(s16 src[32][8], s16 dst[32][8], int shift);

//-------------------------------------------------------------------------
// DCT 32x16
//-------------------------------------------------------------------------
void tx_dct2_pb32x16_row(s16 src[16][32], s16 dst[16][32], int shift);

void tx_dct2_pb32x16_col(s16 src[16][32], s16 dst[16][32], int shift);

void itx_dct2_pb32x16_col(s16 src[16][32], s16 dst[16][32], int shift);

void itx_dct2_pb32x16_row(s16 src[16][32], s16 dst[16][32], int shift);

//-------------------------------------------------------------------------
// DCT 16x32
//-------------------------------------------------------------------------
void tx_dct2_pb16x32_row(s16 src[32][16], s16 dst[32][16], int shift);

void tx_dct2_pb16x32_col(s16 src[32][16], s16 dst[32][16], int shift);

void itx_dct2_pb16x32_col(s16 src[32][16], s16 dst[32][16], int shift);

void itx_dct2_pb16x32_row(s16 src[32][16], s16 dst[32][16], int shift);

//-------------------------------------------------------------------------
// DCT 32x32
//-------------------------------------------------------------------------
void tx_dct2_pb32x32_row(s16 src[32][32], s16 dst[32][32], int shift);

void tx_dct2_pb32x32_col(s16 src[32][32], s16 dst[32][32], int shift);

void itx_dct2_pb32x32_col(s16 src[32][32], s16 dst[32][32], int shift);

void itx_dct2_pb32x32_row(s16 src[32][32], s16 dst[32][32], int shift);

//-------------------------------------------------------------------------
// DCT 64x8
//-------------------------------------------------------------------------
void tx_dct2_pb64x8_row(s16 src[8][64], s16 dst[8][64], int shift);

void tx_dct2_pb64x8_col(s16 src[8][64], s16 dst[8][64], int shift);

void itx_dct2_pb64x8_col(s16 src[8][64], s16 dst[8][64], int shift);

void itx_dct2_pb64x8_row(s16 src[8][64], s16 dst[8][64], int shift);

//-------------------------------------------------------------------------
// DCT 8x64
//-------------------------------------------------------------------------
void tx_dct2_pb8x64_row(s16 src[64][8], s16 dst[64][8], int shift);

void tx_dct2_pb8x64_col(s16 src[64][8], s16 dst[64][8], int shift);

void itx_dct2_pb8x64_col(s16 src[64][8], s16 dst[64][8], int shift);

void itx_dct2_pb8x64_row(s16 src[64][8], s16 dst[64][8], int shift);

//-------------------------------------------------------------------------
// DCT 64x16
//-------------------------------------------------------------------------
void tx_dct2_pb64x16_row(s16 src[16][64], s16 dst[16][64], int shift);

void tx_dct2_pb64x16_col(s16 src[16][64], s16 dst[16][64], int shift);

void itx_dct2_pb64x16_col(s16 src[16][64], s16 dst[16][64], int shift);

void itx_dct2_pb64x16_row(s16 src[16][64], s16 dst[16][64], int shift);

//-------------------------------------------------------------------------
// DCT 16x64
//-------------------------------------------------------------------------
void tx_dct2_pb16x64_row(s16 src[64][16], s16 dst[64][16], int shift);

void tx_dct2_pb16x64_col(s16 src[64][16], s16 dst[64][16], int shift);

void itx_dct2_pb16x64_col(s16 src[64][16], s16 dst[64][16], int shift);

void itx_dct2_pb16x64_row(s16 src[64][16], s16 dst[64][16], int shift);

//-------------------------------------------------------------------------
// DCT 64x32
//-------------------------------------------------------------------------
void tx_dct2_pb64x32_row(s16 src[32][64], s16 dst[32][64], int shift);

void tx_dct2_pb64x32_col(s16 src[32][64], s16 dst[32][64], int shift);

void itx_dct2_pb64x32_col(s16 src[32][64], s16 dst[32][64], int shift);

void itx_dct2_pb64x32_row(s16 src[32][64], s16 dst[32][64], int shift);

//-------------------------------------------------------------------------
// DCT 32x64
//-------------------------------------------------------------------------
void tx_dct2_pb32x64_row(s16 src[64][32], s16 dst[64][32], int shift);

void tx_dct2_pb32x64_col(s16 src[64][32], s16 dst[64][32], int shift);

void itx_dct2_pb32x64_col(s16 src[64][32], s16 dst[64][32], int shift);

void itx_dct2_pb32x64_row(s16 src[64][32], s16 dst[64][32], int shift);

//-------------------------------------------------------------------------
// DCT 64x64
//-------------------------------------------------------------------------
void tx_dct2_pb64x64_row(s16 src[64][64], s16 dst[64][64], int shift);

void tx_dct2_pb64x64_col(s16 src[64][64], s16 dst[64][64], int shift);

void itx_dct2_pb64x64_col(s16 src[64][64], s16 dst[64][64], int shift);

void itx_dct2_pb64x64_row(s16 src[64][64], s16 dst[64][64], int shift);


template <typename dst_type, typename src_type>
void memcpy_rewrintra(dst_type* des, const src_type* src, int n)
{

#pragma HLS INLINE off
    int z;
    for (z = 0; z < n; z++) {

#pragma HLS LOOP_TRIPCOUNT max=64
        des[z] = src[z];
    }
}
#endif // _TRANSFORM_H_
