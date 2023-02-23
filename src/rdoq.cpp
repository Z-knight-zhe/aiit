#include <assert.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "com_port.h"
#include "com_def.h"
#include "enc_def.h"
#include "rdoq.h"
#pragma warning(disable:4477)
#pragma warning(disable:6297)
extern s64 err_scale_tbl[80][5 + 1];

static s64 get_ic_rate_cost_rl(u32 abs_level, u32 run, s32 ctx_run, u32 ctx_level, s64 lambda, int last_pos,
    s32 rdoq_est_run_local[24][2], s32 rdoq_est_level_local[24][2])
{

#pragma HLS INLINE off
    s32 rate;
    int golomb;
    if (abs_level - ((unsigned int)9) <= ((unsigned int)0)) {
        golomb = 1;
    }
    else if (abs_level - ((unsigned int)9) <= ((unsigned int)2)) {
        golomb = 3;
    }
    else if (abs_level - ((unsigned int)9) <= ((unsigned int)6)) {
        golomb = 5;
    }
    else if (abs_level - ((unsigned int)9) <= ((unsigned int)14)) {
        golomb = 7;
    }
    else if (abs_level - ((unsigned int)9) <= ((unsigned int)30)) {
        golomb = 9;
    }
    else if (abs_level - ((unsigned int)9) <= ((unsigned int)62)) {
        golomb = 11;
    }
    else if (abs_level - ((unsigned int)9) <= ((unsigned int)126)) {
        golomb = 13;
    }
    else if (abs_level - ((unsigned int)9) <= ((unsigned int)254)) {
        golomb = 15;
    }
    else if (abs_level - ((unsigned int)9) <= ((unsigned int)510)) {
        golomb = 17;
    }
    else if (abs_level - ((unsigned int)9) <= ((unsigned int)1022)) {
        golomb = 19;
    }
    else if (abs_level - ((unsigned int)9) <= ((unsigned int)2046)) {
        golomb = 21;
    }
    else if (abs_level - ((unsigned int)9) <= ((unsigned int)4094)) {
        golomb = 23;
    }
    else {
        golomb = 25;
    }
    if (abs_level == ((unsigned int)0)) {
        rate = 0;
        if (run == ((unsigned int)0)) {
            rate += rdoq_est_run_local[ctx_run][0];
        }
        else {
            rate += rdoq_est_run_local[ctx_run + 1][0];
        }
    }
    else {
        // sign of coeff
        rate = 32768;
        if (!last_pos) {
            if (run == ((unsigned int)0)) {
                rate += rdoq_est_run_local[ctx_run][1];
            }
            else {
                rate += rdoq_est_run_local[ctx_run + 1][1];
            }
        }
        if (abs_level == ((unsigned int)1)) {
            rate += rdoq_est_level_local[ctx_level][1];
        }
        else if (abs_level >= ((unsigned int)9)) {
            rate += rdoq_est_level_local[ctx_level][0];
            rate += rdoq_est_level_local[ctx_level + ((unsigned int)1)][0] * 7;
            //get bits of exp-golomb
            rate += 32768 * golomb;
        }
        else {
            rate += rdoq_est_level_local[ctx_level][0];
            rate += rdoq_est_level_local[ctx_level + ((unsigned int)1)][0] * ((s32)(abs_level - ((unsigned int)2)));
            rate += rdoq_est_level_local[ctx_level + ((unsigned int)1)][1];
        }
    }
    return (s64)(((long long)rate) * lambda);
}

u32 get_coded_level_rl(s64 *rd64_uncoded_cost, s64 *rd64_coded_cost, s64 level_double, u32 max_abs_level,
    u32 run, u16 ctx_run, u16 ctx_level, s32 q_bits, s64 err_scale, s64 lambda, int last_pos, s32 rdoq_est_run_local[24][2],
    s32 rdoq_est_level_local[24][2])
{

#pragma HLS INLINE off
    u32 best_abs_level = (u32)0;
    s64 err = level_double * err_scale >> 20;
    u32 min_abs_level;
    *rd64_uncoded_cost = err * err;
    *rd64_coded_cost = *rd64_uncoded_cost + get_ic_rate_cost_rl((u32)0, run, (s32)ctx_run, (u32)ctx_level, lambda,
        last_pos, rdoq_est_run_local, rdoq_est_level_local);
    min_abs_level = (max_abs_level > ((unsigned int)1) ? max_abs_level - ((unsigned int)1) : ((unsigned int)1));
    s64 cost_temp;
    s64 i64Delta1 = level_double - ((long long)(max_abs_level << q_bits));
    s64 i64Delta2 = level_double - ((long long)(min_abs_level << q_bits));
    s64 err1 = i64Delta1 * err_scale >> 20;
    s64 err2 = i64Delta2 * err_scale >> 20;
    s64 cost1 = err1 * err1 + get_ic_rate_cost_rl(max_abs_level, run, (s32)ctx_run, (u32)ctx_level, lambda, last_pos,
        rdoq_est_run_local, rdoq_est_level_local);
    s64 cost2 = err2 * err2 + get_ic_rate_cost_rl(min_abs_level, run, (s32)ctx_run, (u32)ctx_level, lambda, last_pos,
        rdoq_est_run_local, rdoq_est_level_local);
    if (cost1 > cost2) {
        best_abs_level = min_abs_level;
        cost_temp = cost2;
    }
    else {
        best_abs_level = max_abs_level;
        cost_temp = cost1;
    }
    if (cost_temp < *rd64_coded_cost) {
        *rd64_coded_cost = cost_temp;
    }
    else {
        best_abs_level = ((u32)0);
    }
    return best_abs_level;
}
const int quant_scale[80] = { (16302), (15024), (13777), (12634), (11626), (10624), (9742), (8958), (8192), (7512), (6889), (6305), (5793), (5303), (4878), (4467), (4091), (3756), (3444), (3161), (2894), (2654), (2435), (2235), (2048), (1878), (1722), (1579), (1449), (1329), (1218), (1117), (1024), (939), (861), (790), (724), (664), (609), (558), (512), (470), (430), (395), (362), (332), (304), (279), (256), (235), (215), (197), (181), (166), (152), (140), (128), (116), (108), (99), (91), (83), (76), (69), (64), (59), (54), (49), (45), (41), (38), (35), (32), (30), (27), (25), (23), (21), (19), (18) };
int rdoq_est_ctp_zero_flag[2];
int rdoq_est_cbf[NUM_QT_CBF_CTX][2];
int rdoq_est_ctp_zero_flag_flatten[2];
int rdoq_est_cbf_flatten[NUM_QT_CBF_CTX * 2];
s32 rdoq_est_run[NUM_SBAC_CTX_RUN][2];
s32 rdoq_est_level[NUM_SBAC_CTX_LEVEL][2];
s32 rdoq_est_last[2][NUM_SBAC_CTX_LAST1][NUM_SBAC_CTX_LAST2][2];
s32 rdoq_est_last_flatten[2 * NUM_SBAC_CTX_LAST1 * NUM_SBAC_CTX_LAST2 * 2];
extern RDOQ_MODEL rdoq_model;
extern RDOQ_MODEL model_cnt;
static void update_rdoq_model(u32 cnt, u32* model)
{
    if (cnt == 0)
    {
        *model = 2046;
    }
    else if (cnt == 1)
    {
        *model = 3703;
    }
    else if (cnt == 2)
    {
        *model = 6142;
    }
    else if (cnt == 3)
    {
        *model = 8003;
    }
    else if (cnt == 4)
    {
        *model = 8095;
    }
    else
    {
        if (cnt & 1)
        {
            *model = 8187;
        }
        else
        {
            *model = 8104;
        }
    }
}
extern s32 entropy_bits[1 << 10];

static s32 biari_no_bits(int symbol, SBAC_CTX_MODEL *cm)
{
    s32 est_bits;
    u8 cmps;
    u16 prob_lps;
    cmps = ((u8)(*cm & ((unsigned int)1)));
    symbol = ((int)((u8)(symbol != 0)));
    prob_lps = ((u16)((*cm & ((unsigned int)((1 << 11) - 1))) >> 1));
    prob_lps = ((u16)((symbol != ((int)cmps) ? ((int)prob_lps) : (1 << 11) - 1 - ((int)prob_lps))));
    /* (s32)(-32000*(log(p)/log(2.0)-MCABAC_PROB_BITS)); */
    est_bits = entropy_bits[((int)prob_lps) >> (11 - 10)];
    return est_bits;
}

static s32 biari_no_bitsW(int symbol, SBAC_CTX_MODEL *cm1, SBAC_CTX_MODEL *cm2)
{
    s32 est_bits;
    u16 prob_lps;
    u16 prob_lps1 = (u16)((*cm1 & ((unsigned int)((1 << 11) - 1))) >> 1);
    u16 prob_lps2 = (u16)((*cm2 & ((unsigned int)((1 << 11) - 1))) >> 1);
    u16 cmps;
    u16 cmps1 = (u16)(*cm1 & ((unsigned int)1));
    u16 cmps2 = (u16)(*cm2 & ((unsigned int)1));
    if (((int)cmps1) == ((int)cmps2)) {
        cmps = cmps1;
        prob_lps = ((u16)((((int)prob_lps1) + ((int)prob_lps2)) >> 1));
    }
    else {
        if (((int)prob_lps1) < ((int)prob_lps2)) {
            cmps = cmps1;
            prob_lps = ((u16)((256 << 2) - 1 - ((((int)prob_lps2) - ((int)prob_lps1)) >> 1)));
        }
        else {
            cmps = cmps2;
            prob_lps = ((u16)((256 << 2) - 1 - ((((int)prob_lps1) - ((int)prob_lps2)) >> 1)));
        }
    }
    symbol = ((int)((u8)(symbol != 0)));
    prob_lps = ((u16)((symbol != ((int)cmps) ? ((int)prob_lps) : (1 << 11) - 1 - ((int)prob_lps))));
    /* (s32)(-32000*(log(p)/log(2.0)-MCABAC_PROB_BITS)); */
    est_bits = entropy_bits[((int)prob_lps) >> (11 - 10)];
    return est_bits;
}
 int cbf_0;
 int cbf_1;
void enc_rdoq_bit_est()
{
    int bin;
    int ctx;

    if (cbf_0 == 0)
        rdoq_model.cbf[0] = 2046;
    else if (cbf_0 == 1)
        rdoq_model.cbf[0] = 3778;
    else if (cbf_0 == 2)
        rdoq_model.cbf[0] = 3508;
    else if (cbf_0 == 3)
        rdoq_model.cbf[0] = 3282;
    else if (cbf_0 == 4)
        rdoq_model.cbf[0] = 3090;
    else if (cbf_0 == 5)
        rdoq_model.cbf[0] = 2928;
    else if (cbf_0 == 6)
        rdoq_model.cbf[0] = 2792;
    else if (cbf_0 == 7)
        rdoq_model.cbf[0] = 2678;
    else if (cbf_0 == 8)
        rdoq_model.cbf[0] = 2582;
    else if (cbf_0 == 9)
        rdoq_model.cbf[0] = 2500;
    else if (cbf_0 == 10)
        rdoq_model.cbf[0] = 2430;
    else if (cbf_0 == 11)
        rdoq_model.cbf[0] = 2374;
    else if (cbf_0 == 12)
        rdoq_model.cbf[0] = 2324;
    else if (cbf_0 == 13)
        rdoq_model.cbf[0] = 2282;
    else if (cbf_0 == 14)
        rdoq_model.cbf[0] = 2248;
    else if (cbf_0 == 15)
        rdoq_model.cbf[0] = 2218;
    else if (cbf_0 == 16)
        rdoq_model.cbf[0] = 2194;
    else if (cbf_0 == 17)
        rdoq_model.cbf[0] = 2172;
    else if (cbf_0 == 18)
        rdoq_model.cbf[0] = 2156;
    else if (cbf_0 == 19)
        rdoq_model.cbf[0] = 2142;
    else if (cbf_0 == 20)
        rdoq_model.cbf[0] = 2130;
    else if (cbf_0 == 21)
        rdoq_model.cbf[0] = 2118;
    else if (cbf_0 == 22)
        rdoq_model.cbf[0] = 2108;
    else if (cbf_0 == 23)
        rdoq_model.cbf[0] = 2102;
    else if (cbf_0 == 24)
        rdoq_model.cbf[0] = 2096;
    else if (cbf_0 == 25)
        rdoq_model.cbf[0] = 2090;
    else if (cbf_0 == 26)
        rdoq_model.cbf[0] = 2086;
    else if (cbf_0 == 27)
        rdoq_model.cbf[0] = 2082;
    else if (cbf_0 == 28)
        rdoq_model.cbf[0] = 2078;
    else if (cbf_0 == 29)
        rdoq_model.cbf[0] = 2076;
    else if (cbf_0 == 30)
        rdoq_model.cbf[0] = 2074;
    else if (cbf_0 == 31)
        rdoq_model.cbf[0] = 2072;
    else if (cbf_0 == 32)
        rdoq_model.cbf[0] = 2070;
    else if (cbf_0 == 33)
        rdoq_model.cbf[0] = 2068;
    else if (cbf_0 == 34)
        rdoq_model.cbf[0] = 2066;
    else if (cbf_0 == 35)
        rdoq_model.cbf[0] = 2064;
    else {
        rdoq_model.cbf[0] = 2062;
    }
    if (cbf_1 == 0)
        rdoq_model.cbf[1] = 2046;
    else if (cbf_1 == 1)
        rdoq_model.cbf[1] = 3778;
    else if (cbf_1 == 2)
        rdoq_model.cbf[1] = 3508;
    else if (cbf_1 == 3)
        rdoq_model.cbf[1] = 3282;
    else if (cbf_1 == 4)
        rdoq_model.cbf[1] = 3090;
    else if (cbf_1 == 5)
        rdoq_model.cbf[1] = 2928;
    else if (cbf_1 == 6)
        rdoq_model.cbf[1] = 2792;
    else if (cbf_1 == 7)
        rdoq_model.cbf[1] = 2678;
    else if (cbf_1 == 8)
        rdoq_model.cbf[1] = 2582;
    else if (cbf_1 == 9)
        rdoq_model.cbf[1] = 2500;
    else if (cbf_1 == 10)
        rdoq_model.cbf[1] = 2430;
    else if (cbf_1 == 11)
        rdoq_model.cbf[1] = 2374;
    else if (cbf_1 == 12)
        rdoq_model.cbf[1] = 2324;
    else if (cbf_1 == 13)
        rdoq_model.cbf[1] = 2282;
    else if (cbf_1 == 14)
        rdoq_model.cbf[1] = 2248;
    else if (cbf_1 == 15)
        rdoq_model.cbf[1] = 2218;
    else if (cbf_1 == 16)
        rdoq_model.cbf[1] = 2194;
    else if (cbf_1 == 17)
        rdoq_model.cbf[1] = 2172;
    else if (cbf_1 == 18)
        rdoq_model.cbf[1] = 2156;
    else if (cbf_1 == 19)
        rdoq_model.cbf[1] = 2142;
    else if (cbf_1 == 20)
        rdoq_model.cbf[1] = 2130;
    else if (cbf_1 == 21)
        rdoq_model.cbf[1] = 2118;
    else if (cbf_1 == 22)
        rdoq_model.cbf[1] = 2108;
    else if (cbf_1 == 23)
        rdoq_model.cbf[1] = 2102;
    else if (cbf_1 == 24)
        rdoq_model.cbf[1] = 2096;
    else if (cbf_1 == 25)
        rdoq_model.cbf[1] = 2090;
    else if (cbf_1 == 26)
        rdoq_model.cbf[1] = 2086;
    else if (cbf_1 == 27)
        rdoq_model.cbf[1] = 2082;
    else if (cbf_1 == 28)
        rdoq_model.cbf[1] = 2078;
    else if (cbf_1 == 29)
        rdoq_model.cbf[1] = 2076;
    else if (cbf_1 == 30)
        rdoq_model.cbf[1] = 2074;
    else if (cbf_1 == 31)
        rdoq_model.cbf[1] = 2072;
    else if (cbf_1 == 32)
        rdoq_model.cbf[1] = 2070;
    else if (cbf_1 == 33)
        rdoq_model.cbf[1] = 2068;
    else if (cbf_1 == 34)
        rdoq_model.cbf[1] = 2066;
    else if (cbf_1 == 35)
        rdoq_model.cbf[1] = 2064;
    else {
        rdoq_model.cbf[1] = 2062;
    }
    if (cbf_1 == 0)
        rdoq_model.cbf[2] = 2046;
    else if (cbf_1 == 1)
        rdoq_model.cbf[2] = 3778;
    else if (cbf_1 == 2)
        rdoq_model.cbf[2] = 3508;
    else if (cbf_1 == 3)
        rdoq_model.cbf[2] = 3282;
    else if (cbf_1 == 4)
        rdoq_model.cbf[2] = 3090;
    else if (cbf_1 == 5)
        rdoq_model.cbf[2] = 2928;
    else if (cbf_1 == 6)
        rdoq_model.cbf[2] = 2792;
    else if (cbf_1 == 7)
        rdoq_model.cbf[2] = 2678;
    else if (cbf_1 == 8)
        rdoq_model.cbf[2] = 2582;
    else if (cbf_1 == 9)
        rdoq_model.cbf[2] = 2500;
    else if (cbf_1 == 10)
        rdoq_model.cbf[2] = 2430;
    else if (cbf_1 == 11)
        rdoq_model.cbf[2] = 2374;
    else if (cbf_1 == 12)
        rdoq_model.cbf[2] = 2324;
    else if (cbf_1 == 13)
        rdoq_model.cbf[2] = 2282;
    else if (cbf_1 == 14)
        rdoq_model.cbf[2] = 2248;
    else if (cbf_1 == 15)
        rdoq_model.cbf[2] = 2218;
    else if (cbf_1 == 16)
        rdoq_model.cbf[2] = 2194;
    else if (cbf_1 == 17)
        rdoq_model.cbf[2] = 2172;
    else if (cbf_1 == 18)
        rdoq_model.cbf[2] = 2156;
    else if (cbf_1 == 19)
        rdoq_model.cbf[2] = 2142;
    else if (cbf_1 == 20)
        rdoq_model.cbf[2] = 2130;
    else if (cbf_1 == 21)
        rdoq_model.cbf[2] = 2118;
    else if (cbf_1 == 22)
        rdoq_model.cbf[2] = 2108;
    else if (cbf_1 == 23)
        rdoq_model.cbf[2] = 2102;
    else if (cbf_1 == 24)
        rdoq_model.cbf[2] = 2096;
    else if (cbf_1 == 25)
        rdoq_model.cbf[2] = 2090;
    else if (cbf_1 == 26)
        rdoq_model.cbf[2] = 2086;
    else if (cbf_1 == 27)
        rdoq_model.cbf[2] = 2082;
    else if (cbf_1 == 28)
        rdoq_model.cbf[2] = 2078;
    else if (cbf_1 == 29)
        rdoq_model.cbf[2] = 2076;
    else if (cbf_1 == 30)
        rdoq_model.cbf[2] = 2074;
    else if (cbf_1 == 31)
        rdoq_model.cbf[2] = 2072;
    else if (cbf_1 == 32)
        rdoq_model.cbf[2] = 2070;
    else if (cbf_1 == 33)
        rdoq_model.cbf[2] = 2068;
    else if (cbf_1 == 34)
        rdoq_model.cbf[2] = 2066;
    else if (cbf_1 == 35)
        rdoq_model.cbf[2] = 2064;
    else {
        rdoq_model.cbf[2] = 2062;
    }
    for (ctx = 0; ctx < 6 * 4; ctx++) {
        update_rdoq_model(model_cnt.level[ctx], &rdoq_model.level[ctx]);
        update_rdoq_model(model_cnt.run_rdoq[ctx], &rdoq_model.run_rdoq[ctx]);
    }
    for (ctx = 0; ctx < 12; ctx++) {
        update_rdoq_model(model_cnt.last1[ctx], &rdoq_model.last1[ctx]);
    }
    for (ctx = 0; ctx < 22; ctx++) {
        update_rdoq_model(model_cnt.last2[ctx], &rdoq_model.last2[ctx]);
    }
    for (bin = 0; bin < 2; bin++) {
        for (ctx = 0; ctx < 3; ctx++) {
            rdoq_est_cbf[ctx][bin] = biari_no_bits(bin, rdoq_model.cbf + ctx);
        }
    }
    for (ctx = 0; ctx < 6 * 4; ctx++) {
        for (bin = 0; bin < 2; bin++) {
            rdoq_est_run[ctx][bin] = biari_no_bits(bin, rdoq_model.run_rdoq + ctx);
        }
    }
    for (ctx = 0; ctx < 6 * 4; ctx++) {
        for (bin = 0; bin < 2; bin++) {
            rdoq_est_level[ctx][bin] = biari_no_bits(bin, rdoq_model.level + ctx);
        }
    }
    // luma / chroma
    for (ctx = 0; ctx < 2; ctx++) {
        int i;
        int j;
        int chroma_offset1 = ctx * 6;
        int chroma_offset2 = ctx * 12;
        for (i = 0; i < 6; i++) {
            for (j = 0; j < 12; j++) {
                for (bin = 0; bin < 2; bin++) {
                    rdoq_est_last[ctx][i][j][bin] = biari_no_bitsW(bin, rdoq_model.last1 + i + chroma_offset1,
                        rdoq_model.last2 + j + chroma_offset2);
                }
            }
        }
    }
}
static void init_scan(u16 *scan, int size_x, int size_y, int scan_type)
{
#pragma HLS INLINE off
    int x;
    int y;
    int l;
    int pos;
    int num_line;
    pos = 0;
    num_line = size_x + size_y - 1;
    if (scan_type == 0) {
        /* starting point */
        scan[pos] = ((u16)0);
        pos++;
        /* loop */
        for (l = 1; l < num_line; l++) {

#pragma HLS LOOP_TRIPCOUNT max=127
            /* decreasing loop */
            if (l % 2) {
                x = (l < size_x - 1 ? l : size_x - 1);
                y = (0 > l - (size_x - 1) ? 0 : l - (size_x - 1));
                while (x >= 0 && y < size_y) {

#pragma HLS LOOP_TRIPCOUNT max=64
                    scan[pos] = ((u16)(y * size_x + x));
                    pos++;
                    x--;
                    y++;
                }
            }
            else
                /* increasing loop */
            {
                y = (l < size_y - 1 ? l : size_y - 1);
                x = (0 > l - (size_y - 1) ? 0 : l - (size_y - 1));
                while (y >= 0 && x < size_x) {

#pragma HLS LOOP_TRIPCOUNT max=64
                    scan[pos] = ((u16)(y * size_x + x));
                    pos++;
                    x++;
                    y--;
                }
            }
        }
    }
}
void level_core(u32 level[24], u32 level1[24], int t0[4096], int i, int max_num)
{
#pragma HLS ARRAY_PARTITION variable=level1 complete dim=1
#pragma HLS FUNCTION_INSTANTIATE variable=i
#pragma HLS ARRAY_PARTITION variable=level complete dim=1
    int tmp = 0;
    int pos = 0;
#pragma HLS ARRAY_PARTITION variable=t0 complete dim=1
    for (pos = 0; pos < max_num; pos++)
    {
#pragma HLS UNROLL
        tmp += !(t0[pos] - i);
    }
    level[i] += tmp;
    level1[i + 1] += 8 * tmp;
}

void run_core(u32 run[24], u32 run1[24], int t0[4096], int i, u32 mat_run[4096], int max_num)
{
#pragma HLS ARRAY_PARTITION variable=mat_run complete dim=1
#pragma HLS ARRAY_PARTITION variable=run1 complete dim=1
#pragma HLS FUNCTION_INSTANTIATE variable=i
#pragma HLS ARRAY_PARTITION variable=run complete dim=1
    int tmp = 0;
    int tmp2 = 0;
    int pos = 0;
#pragma HLS ARRAY_PARTITION variable=t0 complete dim=1
    for (pos = 0; pos < max_num; pos++)
    {
#pragma HLS UNROLL
        tmp += !(t0[pos] - i);
        tmp2 += mat_run[pos];
    }
    run[i] += tmp;
    run1[i + 1] += tmp2;
}

void last1_core(u32 last1[12], int t0[4096], int i, int max_num)
{
#pragma HLS ARRAY_PARTITION variable=last1 complete dim=1
#pragma HLS FUNCTION_INSTANTIATE variable=i
    int tmp = 0;
    int pos = 0;
#pragma HLS ARRAY_PARTITION variable=t0 complete dim=1
    for (pos = 0; pos < max_num; pos++)
    {
#pragma HLS UNROLL
        tmp += !(t0[pos] - i);
    }
    last1[i] += tmp;
}

void last2_core(u32 last2[22], int t0[4096], int i, int max_num)
{
#pragma HLS ARRAY_PARTITION variable=last2 complete dim=1
#pragma HLS FUNCTION_INSTANTIATE variable=i
    int tmp = 0;
    int pos = 0;
#pragma HLS ARRAY_PARTITION variable=t0 complete dim=1
    for (pos = 0; pos < max_num; pos++)
    {
#pragma HLS UNROLL
        tmp += !(t0[pos] - i);
    }
    last2[i] += tmp;
}
void model_update(u32 last1[12], u32 last2[22], u32 run[24], u32 level[24], u32 mat_run[4096], u32 mat_prev_level[4096], int ch_type, int max_num)
{
#pragma HLS ARRAY_PARTITION variable=last2 complete dim=1
#pragma HLS ARRAY_PARTITION variable=last1 complete dim=1
#pragma HLS PIPELINE II=1
#pragma HLS ARRAY_PARTITION variable=run complete dim=1
#pragma HLS ARRAY_PARTITION variable=level complete dim=1
#pragma HLS ARRAY_PARTITION variable=mat_prev_level complete dim=1
#pragma HLS ARRAY_PARTITION variable=mat_run complete dim=1
    int scan_pos = 0;
    static int t0[4096] = { 0 };
#pragma HLS ARRAY_PARTITION variable=t0 complete dim=1
    static int last1_pos[4096] = { 0 };
#pragma HLS ARRAY_PARTITION variable=last1_pos complete dim=1
    static int last2_pos[4096] = { 0 };
#pragma HLS ARRAY_PARTITION variable=last2_pos complete dim=1
    u32 last1_tmp[12] = { 0 };
#pragma HLS ARRAY_PARTITION variable=last1_tmp complete dim=1
    u32 last2_tmp[22] = { 0 };
#pragma HLS ARRAY_PARTITION variable=last2_tmp complete dim=1
    u32 level_tmp[24] = { 0 };
#pragma HLS ARRAY_PARTITION variable=level_tmp complete dim=1
    u32 level_tmp1[24] = { 0 };
#pragma HLS ARRAY_PARTITION variable=level_tmp1 complete dim=1
    u32 run_tmp[24] = { 0 };
#pragma HLS ARRAY_PARTITION variable=run_tmp complete dim=1
    u32 run_tmp1[24] = { 0 };
#pragma HLS ARRAY_PARTITION variable=run_tmp1 complete dim=1
    int ace_log2 = 0;

    for (scan_pos = 0; scan_pos < max_num; scan_pos++)
    {
#pragma HLS UNROLL
        if (scan_pos + 1 < 2) {
            ace_log2 = 0;
        }
        else if (scan_pos + 1 < 4) {
            ace_log2 = 1;
        }
        else if (scan_pos + 1 < 8) {
            ace_log2 = 2;
        }
        else if (scan_pos + 1 < 16) {
            ace_log2 = 3;
        }
        else if (scan_pos + 1 < 32) {
            ace_log2 = 4;
        }
        else if (scan_pos + 1 < 64) {
            ace_log2 = 5;
        }
        else if (scan_pos + 1 < 128) {
            ace_log2 = 6;
        }
        else if (scan_pos + 1 < 256) {
            ace_log2 = 7;
        }
        else if (scan_pos + 1 < 512) {
            ace_log2 = 8;
        }
        else if (scan_pos + 1 < 1024) {
            ace_log2 = 9;
        }
        else if (scan_pos + 1 < 2048) {
            ace_log2 = 10;
        }
        else if (scan_pos + 1 < 4096) {
            ace_log2 = 11;
        }
        else {
            ace_log2 = 11;
        }

        t0[scan_pos] = ((COM_MIN(mat_prev_level[scan_pos] - 1, 5)) * 2) + (ch_type == 0 ? 0 : 12);
        last1_pos[scan_pos] = ((COM_MIN(mat_prev_level[scan_pos] - 1, 5))) + (ch_type == 0 ? 0 : 6);
        last2_pos[scan_pos] = ace_log2 + (ch_type == 0 ? 0 : 12);
    }

    for (scan_pos = 0; scan_pos < 24; scan_pos += 2)
    {
#pragma HLS UNROLL
        level_core(level_tmp, level_tmp1, t0, scan_pos, max_num);
        run_core(run_tmp, run_tmp1, t0, scan_pos, mat_run, max_num);
    }

    for (scan_pos = 0; scan_pos < 12; scan_pos++)
    {
#pragma HLS UNROLL
        last1_core(last1_tmp, last1_pos, scan_pos, max_num);
        last1[scan_pos] += last1_tmp[scan_pos];
    }

    for (scan_pos = 0; scan_pos < 22; scan_pos++)
    {
#pragma HLS UNROLL
        last2_core(last2_tmp, last2_pos, scan_pos, max_num);
        last2[scan_pos] += last2_tmp[scan_pos];
    }

    for (scan_pos = 0; scan_pos < 24; scan_pos++)
    {
#pragma HLS UNROLL
        level[scan_pos] += level_tmp[scan_pos] + level_tmp1[scan_pos];
        run[scan_pos] += run[scan_pos] + run_tmp1[scan_pos];
    }
}

//64
#define W 64
#define H 64
#include "rdoq_func_pcnl.h"
#define W 64
#define H 32
#include "rdoq_func_pcnl.h"
#define W 32
#define H 64
#include "rdoq_func_pcnl.h"
#define W 64
#define H 16
#include "rdoq_func_pcnl.h"
#define W 16
#define H 64
#include "rdoq_func_pcnl.h"
#define W 64
#define H 8
#include "rdoq_func_pcnl.h"
#define W 8
#define H 64
#include "rdoq_func_pcnl.h"
//32
#define W 32
#define H 32
#include "rdoq_func_pcnl.h"
#define W 32
#define H 16
#include "rdoq_func_pcnl.h"
#define W 16
#define H 32
#include "rdoq_func_pcnl.h"
#define W 32
#define H 8
#include "rdoq_func_pcnl.h"
#define W 8
#define H 32
#include "rdoq_func_pcnl.h"
#define W 32
#define H 4
#include "rdoq_func_pcnl.h"
#define W 4
#define H 32
#include "rdoq_func_pcnl.h"
//16
#define W 16
#define H 16
#include "rdoq_func_pcnl.h"
#define W 16
#define H 8
#include "rdoq_func_pcnl.h"
#define W 8
#define H 16
#include "rdoq_func_pcnl.h"
#define W 16
#define H 4
#include "rdoq_func_pcnl.h"
#define W 4
#define H 16
#include "rdoq_func_pcnl.h"
//8
#define W 8
#define H 8
#include "rdoq_func_pcnl.h"
#define W 8
#define H 4
#include "rdoq_func_pcnl.h"
#define W 4
#define H 8
#include "rdoq_func_pcnl.h"
#define W 4
#define H 4
#include "rdoq_func_pcnl.h"
#undef W
#undef H
template <const int W, const int H, typename F, F func>
int enc_quant_nnz_with_func(int bit_depth, int qp, s64 lambda, s16* coef, int cu_width_log2, int cu_height_log2, int ch_type)
{
#ifndef __SYNTHESIS__ //{{{
    bool b_wh = W==8 && H==4 && 0;
    static int s_idx = 0;

    if (b_wh) {
        //printf("eq %d: nor %d %d, qp %d, lambda %ld\n", s_idx, W, H, qp, lambda);
        for (int i=0;i<W*H;i++)   printf("eq %d: coef %d: %d\n", s_idx, i, coef[i]);
    }
#endif//__SYNTHESIS__ }}}

#if defined MD_RDOQ_TV_ACTIVE
    md_rdoq_test_module.parameter_cnt++;
    md_rdoq_test_module.writeParameter2File(W, H, coef, W * H * sizeof(*coef), 1);
    md_rdoq_test_module.writeParameter2File(W, H, &qp, sizeof(qp), 1);
    md_rdoq_test_module.writeParameter2File(W, H, &lambda, sizeof(lambda), 1);
#endif//MD_RDOQ_TV_ACTIVE

    int i;
    int j;
    int num_nz_coef = 0;
    int is_intra = 1;
    int scale = quant_scale[qp];
    int width = 1 << cu_width_log2;
    int height = 1 << cu_height_log2;
    s16* p;
    s64 lev;
    s64 offset;
    enc_rdoq_bit_est();
    int shift;
    int tr_shift;
    int log2_size;
    int ns_shift;
    int ns_scale;
    s64 zero_coeff_threshold;
    BOOL is_coded = 0;
    s32 rdoq_est_run_local[24][2];
    s32 rdoq_est_level_local[24][2];
    s32 rdoq_est_last_local[2][6][12][2];

    if (width > 32) {
        p = coef;
        for (i = 0; i < height; i++) {
            for (j = 32; j < width; j++) {
                p[j] = ((s16)0);
            }
            p += width;
        }
    }
    if (height > 32) {
        for (i = 0; i < width * height - 32 * width; i++) {
            coef[i + 32 * width] = ((s16)0);
        }
    }
    log2_size = (cu_width_log2 + cu_height_log2) >> 1;
    ns_shift = cu_width_log2 + (cu_height_log2 & 1) ? 7 : 0;
    ns_scale = cu_width_log2 + (cu_height_log2 & 1) ? 181 : 1;
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 6; j++) {
            for (int k = 0; k < 12; k++) {
                for (int m = 0; m < 2; m++) {
                    rdoq_est_last_local[i][j][k][m] = rdoq_est_last[i][j][k][m];
                }
            }
        }
    }
    for (i = 0; i < 24; i++) {
        for (j = 0; j < 2; j++) {
            rdoq_est_run_local[i][j] = rdoq_est_run[i][j];
            rdoq_est_level_local[i][j] = rdoq_est_level[i][j];
        }
    }
    tr_shift = get_transform_shift(bit_depth, log2_size - ns_shift);
    shift = 14 + tr_shift;
    offset = ((s64)(201) << ((s64)(shift - 9)));
    zero_coeff_threshold = (((s64)1) << ((s64)shift)) - offset;
    for (i = 0; i < 1 << (cu_width_log2 + cu_height_log2); i++) {
        lev = ((s64)(abs((int)coef[i]))) * ((s64)scale) * ((long long)ns_scale);
        if (lev >= zero_coeff_threshold) {
            is_coded = 1;
            break;
        }
    }
    if (!is_coded) {
        for (i = 0; i < 1 << (cu_width_log2 + cu_height_log2); i++) {
            coef[i] = ((s16)0);
        }
        //return num_nz_coef;
    } else {

        s16 coef_tmp[H][W] = { 0 };
        s16 dst_tmp[H][W] = { 0 };
        for (int a = 0; a < height; a++) {
            for (int b = 0; b < width; b++) {
                coef_tmp[a][b] = coef[a * width + b];
            }
        }
        num_nz_coef = func(qp, lambda, is_intra, coef_tmp, dst_tmp,
            cu_width_log2, cu_height_log2, ch_type, bit_depth, rdoq_est_run_local, rdoq_est_level_local, rdoq_est_last_local);
        for (int a = 0; a < height; a++) {
            for (int b = 0; b < width; b++) {
                coef[a * width + b] = dst_tmp[a][b];
            }
        }
    }

#if defined MD_RDOQ_TV_ACTIVE
    md_rdoq_test_module.writeOutputGolden2File(W,H,coef, W * H * sizeof(*coef));
    md_rdoq_test_module.writeOutputGolden2File(W,H,&num_nz_coef, sizeof(num_nz_coef));
#endif //MD_RDOQ_TV_ACTIVE

#ifndef __SYNTHESIS__ //{{{
    if (b_wh) {
        for (int i=0;i<W*H;i++) printf("eq %d: ret %d: %d\n", s_idx, i, coef[i]);
        printf("eq %d: %d\n", s_idx, num_nz_coef);
        s_idx ++;
    }
#endif//__SYNTHESIS__ }}}

    return num_nz_coef;
}


template <const int W, const int H, typename F, F func>
int enc_quant_nnz_with_func_small(int bit_depth, int qp, s64 lambda, s16* coef, int cu_width_log2, int cu_height_log2, int ch_type)
{

#ifndef __SYNTHESIS__ //{{{
    bool b_wh = W==8 && H==4 && 0;
    static int s_idx = 0;

    if (b_wh) {
        //printf("eq %d: sml %d %d, qp %d, lambda %ld\n", s_idx, W, H, qp, lambda);
        for (int i=0;i<W*H;i++)   printf("eq %d: coef %d: %d\n", s_idx, i, coef[i]);
    }
#endif//__SYNTHESIS__ }}}

#if defined MD_RDOQ_TV_ACTIVE
    md_rdoq_test_module.parameter_cnt++;
    md_rdoq_test_module.writeParameter2File(W, H, coef, W * H * sizeof(*coef), 1);
    md_rdoq_test_module.writeParameter2File(W, H, &qp, sizeof(qp), 1);
    md_rdoq_test_module.writeParameter2File(W, H, &lambda, sizeof(lambda), 1);
#endif//MD_RDOQ_TV_ACTIVE

    int i;
    int j;
    int num_nz_coef = 0;
    int is_intra = 1;
    int scale = quant_scale[qp];
    int width = 1 << cu_width_log2;
    int height = 1 << cu_height_log2;
    s16* p;
    s64 lev;
    s64 offset;
    enc_rdoq_bit_est();
    int shift;
    int tr_shift;
    int log2_size;
    int ns_shift;
    int ns_scale;
    s64 zero_coeff_threshold;
    BOOL is_coded = 0;
    s32 rdoq_est_run_local[24][2];
    s32 rdoq_est_level_local[24][2];
    s32 rdoq_est_last_local[2][6][12][2];

    if (width > 32) {
        p = coef;
        for (i = 0; i < height; i++) {
            for (j = 32; j < width; j++) {
                p[j] = ((s16)0);
            }
            p += width;
        }
    }
    if (height > 32) {
        for (i = 0; i < width * height - 32 * width; i++) {
            coef[i + 32 * width] = ((s16)0);
        }
    }
    log2_size = (cu_width_log2 + cu_height_log2) >> 1;
    ns_shift = cu_width_log2 + (cu_height_log2 & 1) ? 7 : 0;
    ns_scale = cu_width_log2 + (cu_height_log2 & 1) ? 181 : 1;
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 6; j++) {
            for (int k = 0; k < 12; k++) {
                for (int m = 0; m < 2; m++) {
                    rdoq_est_last_local[i][j][k][m] = rdoq_est_last[i][j][k][m];
                }
            }
        }
    }
    for (i = 0; i < 24; i++) {
        for (j = 0; j < 2; j++) {
            rdoq_est_run_local[i][j] = rdoq_est_run[i][j];
            rdoq_est_level_local[i][j] = rdoq_est_level[i][j];
        }
    }
    tr_shift = get_transform_shift(bit_depth, log2_size - ns_shift);
    shift = 14 + tr_shift;
    offset = ((s64)(201) << ((s64)(shift - 9)));
    zero_coeff_threshold = (((s64)1) << ((s64)shift)) - offset;
    for (i = 0; i < 1 << (cu_width_log2 + cu_height_log2); i++) {
        lev = ((s64)(abs((int)coef[i]))) * ((s64)scale) * ((long long)ns_scale);
        if (lev >= zero_coeff_threshold) {
            is_coded = 1;
            break;
        }
    }
    if (!is_coded) {
        for (i = 0; i < 1 << (cu_width_log2 + cu_height_log2); i++) {
            coef[i] = ((s16)0);
        }
        //return num_nz_coef;
    } else {
        num_nz_coef = func(qp, lambda, is_intra, coef, coef, cu_width_log2, cu_height_log2, ch_type, bit_depth, rdoq_est_run_local, rdoq_est_level_local, rdoq_est_last_local);    
    }

#if defined MD_RDOQ_TV_ACTIVE
    md_rdoq_test_module.writeOutputGolden2File(W,H,coef, W * H * sizeof(*coef));
    md_rdoq_test_module.writeOutputGolden2File(W,H,&num_nz_coef, sizeof(num_nz_coef));
#endif //MD_RDOQ_TV_ACTIVE

#ifndef __SYNTHESIS__ //{{{
    if (b_wh) {
        for (int i=0;i<W*H;i++) printf("eq %d: ret %d: %d\n", s_idx, i, coef[i]);
        printf("eq %d: %d\n", s_idx, num_nz_coef);
        s_idx ++;
    }
#endif//__SYNTHESIS__ }}}

    return num_nz_coef;
}

template <const int W, const int H>
int enc_quant_nnz1(int bit_depth, int qp, s64 lambda, s16* coef, int cu_width_log2,
    int cu_height_log2, int ch_type);

#define DEFINE_ENC_QUANT_NNZ1(W, H) \
template <> int enc_quant_nnz1<W, H>(int bit_depth, int qp, s64 lambda, s16* coef, int cu_width_log2,\
    int cu_height_log2, int ch_type)\
{\
    return enc_quant_nnz_with_func<W, H, decltype(&RDOQ_FUNC_NAME(rdoq_top, W, H)), &RDOQ_FUNC_NAME(rdoq_top, W, H)>(bit_depth, qp, lambda, coef, cu_width_log2, \
        cu_height_log2, ch_type);\
}

#define DEFINE_ENC_QUANT_NNZ_SMALL(W, H) \
template <> int enc_quant_nnz1<W, H>(int bit_depth, int qp, s64 lambda, s16* coef, int cu_width_log2,\
    int cu_height_log2, int ch_type)\
{\
    return enc_quant_nnz_with_func_small<W, H, decltype(&RDOQ_FUNC_NAME(rdoq_top, W, H)), &RDOQ_FUNC_NAME(rdoq_top, W, H)>(bit_depth, qp, lambda, coef, cu_width_log2, \
        cu_height_log2, ch_type);\
}

#define DEFINE_ENC_QUANT_NNZ_IGNORE(W, H) \
template <> int enc_quant_nnz1<W, H>(int bit_depth, int qp, s64 lambda, s16* coef, int cu_width_log2,\
    int cu_height_log2, int ch_type)\
{\
    assert(0);\
    return 0;\
}

DEFINE_ENC_QUANT_NNZ1(64, 32);
DEFINE_ENC_QUANT_NNZ1(16, 32);
DEFINE_ENC_QUANT_NNZ1(16, 64);
DEFINE_ENC_QUANT_NNZ1(32, 16);
DEFINE_ENC_QUANT_NNZ1(32, 32);
DEFINE_ENC_QUANT_NNZ1(32, 4);
DEFINE_ENC_QUANT_NNZ1(32, 64);
DEFINE_ENC_QUANT_NNZ1(32, 8);
DEFINE_ENC_QUANT_NNZ1(4, 32);
DEFINE_ENC_QUANT_NNZ1(64, 16);
DEFINE_ENC_QUANT_NNZ1(64, 64);
DEFINE_ENC_QUANT_NNZ1(64, 8);
DEFINE_ENC_QUANT_NNZ1(8, 32);
DEFINE_ENC_QUANT_NNZ1(8, 64);

DEFINE_ENC_QUANT_NNZ_SMALL(16, 16);
DEFINE_ENC_QUANT_NNZ_SMALL(16, 4);
DEFINE_ENC_QUANT_NNZ_SMALL(16, 8);
DEFINE_ENC_QUANT_NNZ_SMALL(4, 16);
DEFINE_ENC_QUANT_NNZ_SMALL(4, 8);
DEFINE_ENC_QUANT_NNZ_SMALL(8, 16);
DEFINE_ENC_QUANT_NNZ_SMALL(8, 4);
DEFINE_ENC_QUANT_NNZ_SMALL(8, 8);
DEFINE_ENC_QUANT_NNZ_SMALL(4, 4);

DEFINE_ENC_QUANT_NNZ_IGNORE(2, 4);
DEFINE_ENC_QUANT_NNZ_IGNORE(4, 2);
DEFINE_ENC_QUANT_NNZ_IGNORE(2, 8);
DEFINE_ENC_QUANT_NNZ_IGNORE(8, 2);
DEFINE_ENC_QUANT_NNZ_IGNORE(16, 2);
DEFINE_ENC_QUANT_NNZ_IGNORE(2, 16);

int enc_quant_nnz(int bit_depth, int qp, s64 lambda, s16* coef, int cu_width_log2,
    int cu_height_log2, int ch_type)
{
    int i;
    int j;
#pragma HLS INLINE off
    int num_nz_coef = 0;
    int is_intra = 1;
    int scale = quant_scale[qp];
    int width = 1 << cu_width_log2;
    int height = 1 << cu_height_log2;
    s16* p;
    s64 lev;
    s64 offset;
    enc_rdoq_bit_est();
    int shift;
    int tr_shift;
    int log2_size;
    int ns_shift;
    int ns_scale;
    s64 zero_coeff_threshold;
    BOOL is_coded = 0;
    s32 rdoq_est_run_local[24][2];
    s32 rdoq_est_level_local[24][2];
    s32 rdoq_est_last_local[2][6][12][2];
    if (width > 32) {

#pragma HLS loop_tripcount max=32

        p = coef;
        for (i = 0; i < height; i++) {

#pragma HLS loop_tripcount max=64
            for (j = 32; j < width; j++) {

#pragma HLS loop_tripcount max=64
                p[j] = ((s16)0);
            }
            p += width;
        }
    }
    if (height > 32) {
        for (i = 0; i < width * height - 32 * width; i++) {

#pragma HLS loop_tripcount max=2048
            coef[i + 32 * width] = ((s16)0);
        }
    }

    log2_size = (cu_width_log2 + cu_height_log2) >> 1;
    ns_shift = cu_width_log2 + (cu_height_log2 & 1) ? 7 : 0;
    ns_scale = cu_width_log2 + (cu_height_log2 & 1) ? 181 : 1;
    for (i = 0; i < 2; i++) {

#pragma HLS loop_tripcount max=2
        for (j = 0; j < 6; j++) {

#pragma HLS loop_tripcount max=6
            for (int k = 0; k < 12; k++) {

#pragma HLS loop_tripcount max=12
                for (int m = 0; m < 2; m++) {

#pragma HLS loop_tripcount max=2
                    rdoq_est_last_local[i][j][k][m] = rdoq_est_last[i][j][k][m];
                }
            }
        }
    }
    for (i = 0; i < 24; i++) {

#pragma HLS loop_tripcount max=24
        for (j = 0; j < 2; j++) {

#pragma HLS loop_tripcount max=2
            rdoq_est_run_local[i][j] = rdoq_est_run[i][j];
            rdoq_est_level_local[i][j] = rdoq_est_level[i][j];
        }
    }
    tr_shift = get_transform_shift(bit_depth, log2_size - ns_shift);
    shift = 14 + tr_shift;
//    offset = ((s64)((slice_type == 1 ? 201 : 153))) << ((s64)(shift - 9));
    offset = ((s64)(201) << ((s64)(shift - 9)));
    zero_coeff_threshold = (((s64)1) << ((s64)shift)) - offset;
    for (i = 0; i < 1 << (cu_width_log2 + cu_height_log2); i++) {
#pragma HLS LOOP_TRIPCOUNT max=4096
        lev = ((s64)(abs((int)coef[i]))) * ((s64)scale) * ((long long)ns_scale);
        if (lev >= zero_coeff_threshold) {
            is_coded = 1;
            break;
        }
    }
    if (!is_coded) {
        for (i = 0; i < 1 << (cu_width_log2 + cu_height_log2); i++) {

#pragma HLS LOOP_TRIPCOUNT max=4096
            coef[i] = ((s16)0);
        }
        return num_nz_coef;
    }

    if (cu_width_log2 == 6 && cu_height_log2 == 6)
    {
        s16 coef_tmp[64][64] = { 0 };
        s16 dst_tmp[64][64] = { 0 };
        for (int a = 0; a < height; a++)
        {
            for (int b = 0; b < width; b++)
            {
                coef_tmp[a][b] = coef[a * width + b];
            }
        }
        num_nz_coef = rdoq_top_64x64(qp, lambda, is_intra, coef_tmp, dst_tmp,
            cu_width_log2, cu_height_log2, ch_type, bit_depth, rdoq_est_run_local, rdoq_est_level_local, rdoq_est_last_local);
        for (int a = 0; a < height; a++)
        {
            for (int b = 0; b < width; b++)
            {
                coef[a * width + b] = dst_tmp[a][b];
            }
        }
    }
    else if (cu_width_log2 == 6 && cu_height_log2 == 5)
    {
        s16 coef_tmp[32][64] = { 0 };
        s16 dst_tmp[32][64] = { 0 };
        for (int a = 0; a < height; a++)
        {
            for (int b = 0; b < width; b++)
            {
                coef_tmp[a][b] = coef[a * width + b];
            }
        }
        num_nz_coef = rdoq_top_64x32(qp, lambda, is_intra, coef_tmp, dst_tmp,
            cu_width_log2, cu_height_log2, ch_type, bit_depth, rdoq_est_run_local, rdoq_est_level_local, rdoq_est_last_local);
        for (int a = 0; a < height; a++)
        {
            for (int b = 0; b < width; b++)
            {
                coef[a * width + b] = dst_tmp[a][b];
            }
        }
    }
    else if (cu_width_log2 == 6 && cu_height_log2 == 4)
    {
        s16 coef_tmp[16][64] = { 0 };
        s16 dst_tmp[16][64] = { 0 };
        for (int a = 0; a < height; a++)
        {
            for (int b = 0; b < width; b++)
            {
                coef_tmp[a][b] = coef[a * width + b];
            }
        }
        num_nz_coef = rdoq_top_64x16(qp, lambda, is_intra, coef_tmp, dst_tmp, cu_width_log2, cu_height_log2, ch_type, bit_depth, rdoq_est_run_local, rdoq_est_level_local, rdoq_est_last_local);
        for (int a = 0; a < height; a++)
        {
            for (int b = 0; b < width; b++)
            {
                coef[a * width + b] = dst_tmp[a][b];
            }
        }
    }
    else if (cu_width_log2 == 6 && cu_height_log2 == 3)
    {
        s16 coef_tmp[8][64] = { 0 };
        s16 dst_tmp[8][64] = { 0 };
        for (int a = 0; a < height; a++)
        {
            for (int b = 0; b < width; b++)
            {
                coef_tmp[a][b] = coef[a * width + b];
            }
        }
        num_nz_coef = rdoq_top_64x8(qp, lambda, is_intra, coef_tmp, dst_tmp, cu_width_log2, cu_height_log2, ch_type, bit_depth, rdoq_est_run_local, rdoq_est_level_local, rdoq_est_last_local);
        for (int a = 0; a < height; a++)
        {
            for (int b = 0; b < width; b++)
            {
                coef[a * width + b] = dst_tmp[a][b];
            }
        }
    }
    else if (cu_width_log2 == 5 && cu_height_log2 == 6)
    {
        s16 coef_tmp[64][32] = { 0 };
        s16 dst_tmp[64][32] = { 0 };
        for (int a = 0; a < height; a++)
        {
            for (int b = 0; b < width; b++)
            {
                coef_tmp[a][b] = coef[a * width + b];
            }
        }
        num_nz_coef = rdoq_top_32x64(qp, lambda, is_intra, coef_tmp, dst_tmp, cu_width_log2, cu_height_log2, ch_type, bit_depth, rdoq_est_run_local, rdoq_est_level_local, rdoq_est_last_local);
        for (int a = 0; a < height; a++)
        {
            for (int b = 0; b < width; b++)
            {
                coef[a * width + b] = dst_tmp[a][b];
            }
        }
    }
    else if (cu_width_log2 == 4 && cu_height_log2 == 6)
    {
        s16 coef_tmp[64][16] = { 0 };
        s16 dst_tmp[64][16] = { 0 };
        for (int a = 0; a < height; a++)
        {
            for (int b = 0; b < width; b++)
            {
                coef_tmp[a][b] = coef[a * width + b];
            }
        }
        num_nz_coef = rdoq_top_16x64(qp, lambda, is_intra, coef_tmp, dst_tmp, cu_width_log2, cu_height_log2, ch_type, bit_depth, rdoq_est_run_local, rdoq_est_level_local, rdoq_est_last_local);
        for (int a = 0; a < height; a++)
        {
            for (int b = 0; b < width; b++)
            {
                coef[a * width + b] = dst_tmp[a][b];
            }
        }
    }
    else if (cu_width_log2 == 3 && cu_height_log2 == 6)
    {
        s16 coef_tmp[64][8] = { 0 };
        s16 dst_tmp[64][8] = { 0 };
        for (int a = 0; a < height; a++)
        {
            for (int b = 0; b < width; b++)
            {
                coef_tmp[a][b] = coef[a * width + b];
            }
        }
        num_nz_coef = rdoq_top_8x64(qp, lambda, is_intra, coef_tmp, dst_tmp, cu_width_log2, cu_height_log2, ch_type, bit_depth, rdoq_est_run_local, rdoq_est_level_local, rdoq_est_last_local);
        for (int a = 0; a < height; a++)
        {
            for (int b = 0; b < width; b++)
            {
                coef[a * width + b] = dst_tmp[a][b];
            }
        }
    }
    else if (cu_width_log2 == 5 && cu_height_log2 == 5)
    {
        s16 coef_tmp[32][32] = { 0 };
        s16 dst_tmp[32][32] = { 0 };
        for (int a = 0; a < height; a++)
        {
            for (int b = 0; b < width; b++)
            {
                coef_tmp[a][b] = coef[a * width + b];
            }

        }
        num_nz_coef = rdoq_top_32x32(qp, lambda, is_intra, coef_tmp, dst_tmp, cu_width_log2, cu_height_log2, ch_type, bit_depth, rdoq_est_run_local, rdoq_est_level_local, rdoq_est_last_local);
        for (int a = 0; a < height; a++)
        {
            for (int b = 0; b < width; b++)
            {
                coef[a * width + b] = dst_tmp[a][b];
            }
        }

    }
    else if (cu_width_log2 == 5 && cu_height_log2 == 4)
    {
        s16 coef_tmp[16][32] = { 0 };
        s16 dst_tmp[16][32] = { 0 };
        for (int a = 0; a < height; a++)
        {
            for (int b = 0; b < width; b++)
            {
                coef_tmp[a][b] = coef[a * width + b];
            }
        }

        num_nz_coef = rdoq_top_32x16(qp, lambda, is_intra, coef_tmp, dst_tmp,
            cu_width_log2, cu_height_log2, ch_type, bit_depth, rdoq_est_run_local, rdoq_est_level_local, rdoq_est_last_local);
        for (int a = 0; a < height; a++)
        {
            for (int b = 0; b < width; b++)
            {
                coef[a * width + b] = dst_tmp[a][b];
            }
        }
    }
    else if (cu_width_log2 == 4 && cu_height_log2 == 5)
    {
        s16 coef_tmp[32][16] = { 0 };
        s16 dst_tmp[32][16] = { 0 };
        for (int a = 0; a < height; a++)
        {
            for (int b = 0; b < width; b++)
            {
                coef_tmp[a][b] = coef[a * width + b];
            }
        }

        num_nz_coef = rdoq_top_16x32(qp, lambda, is_intra, coef_tmp, dst_tmp,
            cu_width_log2, cu_height_log2, ch_type, bit_depth, rdoq_est_run_local, rdoq_est_level_local, rdoq_est_last_local);
        for (int a = 0; a < height; a++)
        {
            for (int b = 0; b < width; b++)
            {
                coef[a * width + b] = dst_tmp[a][b];
            }
        }
    }
    else if (cu_width_log2 == 5 && cu_height_log2 == 3)
    {
        s16 coef_tmp[8][32] = { 0 };
        s16 dst_tmp[8][32] = { 0 };
        for (int a = 0; a < height; a++)
        {
            for (int b = 0; b < width; b++)
            {
                coef_tmp[a][b] = coef[a * width + b];
            }
        }

        num_nz_coef = rdoq_top_32x8(qp, lambda, is_intra, coef_tmp, dst_tmp, cu_width_log2, cu_height_log2, ch_type, bit_depth, rdoq_est_run_local, rdoq_est_level_local, rdoq_est_last_local);
        for (int a = 0; a < height; a++)
        {
            for (int b = 0; b < width; b++)
            {
                coef[a * width + b] = dst_tmp[a][b];
            }
        }
    }
    else if (cu_width_log2 == 3 && cu_height_log2 == 5)
    {
        s16 coef_tmp[32][8] = { 0 };
        s16 dst_tmp[32][8] = { 0 };
        for (int a = 0; a < height; a++)
        {
            for (int b = 0; b < width; b++)
            {
                coef_tmp[a][b] = coef[a * width + b];
            }
        }

        num_nz_coef = rdoq_top_8x32(qp, lambda, is_intra, coef_tmp, dst_tmp, cu_width_log2, cu_height_log2, ch_type, bit_depth, rdoq_est_run_local, rdoq_est_level_local, rdoq_est_last_local);
        for (int a = 0; a < height; a++)
        {
            for (int b = 0; b < width; b++)
            {
                coef[a * width + b] = dst_tmp[a][b];
            }
        }
    }
    else if (cu_width_log2 == 5 && cu_height_log2 == 2)
    {
        s16 coef_tmp[4][32] = { 0 };
        s16 dst_tmp[4][32] = { 0 };
        for (int a = 0; a < height; a++)
        {
            for (int b = 0; b < width; b++)
            {
                coef_tmp[a][b] = coef[a * width + b];
            }
        }

        num_nz_coef = rdoq_top_32x4(qp, lambda, is_intra, coef_tmp, dst_tmp, cu_width_log2, cu_height_log2, ch_type, bit_depth, rdoq_est_run_local, rdoq_est_level_local, rdoq_est_last_local);
        for (int a = 0; a < height; a++)
        {
            for (int b = 0; b < width; b++)
            {
                coef[a * width + b] = dst_tmp[a][b];
            }
        }
    }
    else if (cu_width_log2 == 2 && cu_height_log2 == 5)
    {
        s16 coef_tmp[32][4] = { 0 };
        s16 dst_tmp[32][4] = { 0 };
        for (int a = 0; a < height; a++)
        {
            for (int b = 0; b < width; b++)
            {
                coef_tmp[a][b] = coef[a * width + b];
            }
        }

        num_nz_coef = rdoq_top_4x32(qp, lambda, is_intra, coef_tmp, dst_tmp, cu_width_log2, cu_height_log2, ch_type, bit_depth, rdoq_est_run_local, rdoq_est_level_local, rdoq_est_last_local);
        for (int a = 0; a < height; a++)
        {
            for (int b = 0; b < width; b++)
            {
                coef[a * width + b] = dst_tmp[a][b];
            }
        }
    }
    else if (cu_width_log2 == 4 && cu_height_log2 == 4)
    {
        num_nz_coef = rdoq_top_16x16(qp, lambda, is_intra, coef, coef, cu_width_log2, cu_height_log2, ch_type, bit_depth, rdoq_est_run_local, rdoq_est_level_local, rdoq_est_last_local);
    }
    else if (cu_width_log2 == 4 && cu_height_log2 == 3)
    {
        num_nz_coef = rdoq_top_16x8(qp, lambda, is_intra, coef, coef, cu_width_log2, cu_height_log2, ch_type, bit_depth, rdoq_est_run_local, rdoq_est_level_local, rdoq_est_last_local);
    }
    else if (cu_width_log2 == 3 && cu_height_log2 == 4)
    {
        num_nz_coef = rdoq_top_8x16(qp, lambda, is_intra, coef, coef, cu_width_log2, cu_height_log2, ch_type, bit_depth, rdoq_est_run_local, rdoq_est_level_local, rdoq_est_last_local);
    }
    else if (cu_width_log2 == 4 && cu_height_log2 == 2)
    {
        num_nz_coef = rdoq_top_16x4(qp, lambda, is_intra, coef, coef, cu_width_log2, cu_height_log2, ch_type, bit_depth, rdoq_est_run_local, rdoq_est_level_local, rdoq_est_last_local);
    }
    else if (cu_width_log2 == 2 && cu_height_log2 == 4)
    {
        num_nz_coef = rdoq_top_4x16(qp, lambda, is_intra, coef, coef, cu_width_log2, cu_height_log2, ch_type, bit_depth, rdoq_est_run_local, rdoq_est_level_local, rdoq_est_last_local);
    }
    else if (cu_width_log2 == 3 && cu_height_log2 == 3)
    {
        num_nz_coef = rdoq_top_8x8(qp, lambda, is_intra, coef, coef, cu_width_log2, cu_height_log2, ch_type, bit_depth, rdoq_est_run_local, rdoq_est_level_local, rdoq_est_last_local);
    }
    else if (cu_width_log2 == 3 && cu_height_log2 == 2)
    {
        num_nz_coef = rdoq_top_8x4(qp, lambda, is_intra, coef, coef, cu_width_log2, cu_height_log2, ch_type, bit_depth, rdoq_est_run_local, rdoq_est_level_local, rdoq_est_last_local);
    }
    else if (cu_width_log2 == 2 && cu_height_log2 == 3)
    {
        num_nz_coef = rdoq_top_4x8(qp, lambda, is_intra, coef, coef, cu_width_log2, cu_height_log2, ch_type, bit_depth, rdoq_est_run_local, rdoq_est_level_local, rdoq_est_last_local);
    }
    else if (cu_width_log2 == 2 && cu_height_log2 == 2)
    {
        num_nz_coef = rdoq_top_4x4(qp, lambda, is_intra, coef, coef, cu_width_log2, cu_height_log2, ch_type, bit_depth, rdoq_est_run_local, rdoq_est_level_local, rdoq_est_last_local);
    }

    static u16 scan[1 << 14] = { 0 };
    init_scan(scan, 1 << cu_width_log2, 1 << cu_height_log2, 0);
    static u32 mat_prev_level[4096] = { ((u32)0) };
    static u32 mat_run[4096] = { ((u32)0) };
    mat_prev_level[0] = ((u32)6);
    int scan_pos = 0;
    int max_num_coef = 1 << (cu_height_log2 + cu_width_log2);
    for (scan_pos = 1; scan_pos < max_num_coef; scan_pos++) {

#pragma HLS LOOP_TRIPCOUNT max=4096
        u32 blk_pos = (u32)scan[scan_pos - 1];
        if (abs((int)coef[blk_pos])) {
            mat_prev_level[scan_pos] = ((u32)(abs((int)coef[blk_pos])));
            mat_run[scan_pos] = ((u32)0);
        }
        else {
            mat_prev_level[scan_pos] = mat_prev_level[scan_pos - 1];
            mat_run[scan_pos] = mat_run[scan_pos - 1] + ((unsigned int)1);
        }
    }
    //if (ch_type == Y_C)
    //{
    //    model_update(model_cnt.last1, model_cnt.last2, model_cnt.run_rdoq, model_cnt.level, mat_run, mat_prev_level, ch_type, max_num_coef);
    //}
    return num_nz_coef;
}
