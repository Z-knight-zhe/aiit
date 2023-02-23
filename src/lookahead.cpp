
#include "com_def.h"
#include "lookahead.h"
#include "math.h"


/************************** ME ************************************************/
/* Define the Search Range for int-pel */
#define SEARCH_RANGE_IPEL_RA               384
#define SEARCH_RANGE_IPEL_LD               64

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
typedef union uavs3e_union16_t {
    u16 i;
    u8  c[2];
} union16_t;

typedef union uavs3e_union32_t {
    u32 i;
    u16 b[2];
    u8  c[4];
} union32_t;

typedef union uavs3e_union64_t {
    u64 i;
    u32 a[2];
    u16 b[4];
    u8  c[8];
} union64_t;

typedef struct uavs3e_uint128_t {
    u64    i[2];
} uint128_t;

typedef union uavs3e_union128_t {
    uint128_t i;
    u64 a[2];
    u32 b[4];
    u16 c[8];
    u8  d[16];
} union128_t;
#define M16(src)                (((union16_t *)(src))->i)
#define M32(src)                (((union32_t *)(src))->i)
#define M64(src)                (((union64_t *)(src))->i)
#define M128(src)               (((union128_t*)(src))->i)

#define CP16(dst,src)           M16 (dst) = M16 (src)
#define CP32(dst,src)           M32 (dst) = M32 (src)
#define CP64(dst,src)           M64 (dst) = M64 (src)
#define CP128(dst,src)          M128(dst) = M128(src)



const  char tbl_refi_bits[17][16] = {
    {0,                                                             },
    {0,                                                             },
    {1,  1,                                                         },
    {1,  2,  2,                                                     },
    {1,  2,  3,  3,                                                 },
    {1,  2,  3,  4,  4,                                             },
    {1,  2,  3,  4,  5,  5,                                         },
    {1,  2,  3,  4,  5,  6,  6,                                     },
    {1,  2,  3,  4,  5,  6,  7,  7,                                 },
    {1,  2,  3,  4,  5,  6,  7,  8,  8,                             },
    {1,  2,  3,  4,  5,  6,  7,  8,  9,  9,                         },
    {1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 10,                     },
    {1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 11,                 },
    {1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 12,             },
    {1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 13,         },
    {1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 14,     },
    {1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 15, },
};

#define GET_MVBITS_X(mv) (tab_mvbits_x[(mv) >> mvr_idx])
#define GET_MVBITS_Y(mv) (tab_mvbits_y[(mv) >> mvr_idx])
#define GET_MVBITS_IPEL_X(mv) (tab_mvbits_x[((mv) << 2) >> mvr_idx])
#define GET_MVBITS_IPEL_Y(mv) (tab_mvbits_y[((mv) << 2) >> mvr_idx])
#define CONV_LOG2(v)                   (com_tbl_log2[v])

void com_intra_pred(pel *src, pel *dst, int ipm, int w, int h, int bit_depth, u16 avail_cu, u8 ipf_flag);

funs_handle_t avs3_funs_handle;


u32 com_had(int w, int h, pel *org, int s_org, pel *cur, int s_cur, int bit_depth)
{
    int  x, y;
    int sum = 0;
    u32(*satd)(pel *p_org, int i_org, pel *p_pred, int i_pred);
    int shift = bit_depth - 8;

    if (w == h) {
        if (w & 7) { // must be 4x4
            return avs3_funs_handle.cost_satd[0][0](org, s_org, cur, s_cur) >> shift;
        }
        else if (w & 15) { // must be 8x8
            return avs3_funs_handle.cost_satd[1][1](org, s_org, cur, s_cur) >> shift;
        }
        else {
            int  offset_org = s_org << 3;
            int  offset_cur = s_cur << 3;
            satd = avs3_funs_handle.cost_satd[1][1];

            for (y = 0; y < h; y += 8) {
                for (x = 0; x < w; x += 8) {
                    sum += satd(&org[x], s_org, &cur[x], s_cur);
                }
                org += offset_org;
                cur += offset_cur;
            }
            return sum >> shift;
        }
    }
    else if (w > h) {
        if (h == 4) {
            satd = avs3_funs_handle.cost_satd[1][0];
            for (x = 0; x < w; x += 8) {
                sum += satd(&org[x], s_org, &cur[x], s_cur);
            }
            return sum >> shift;
        }
        else if (h & 7) {
            int  offset_org = s_org << 2;
            int  offset_cur = s_cur << 2;
            satd = avs3_funs_handle.cost_satd[1][0];

            for (y = 0; y < h; y += 4) {
                for (x = 0; x < w; x += 8) {
                    sum += satd(&org[x], s_org, &cur[x], s_cur);
                }
                org += offset_org;
                cur += offset_cur;
            }
            return sum >> shift;
        }
        else {
            int  offset_org = s_org << 3;
            int  offset_cur = s_cur << 3;
            satd = avs3_funs_handle.cost_satd[2][1];

            for (y = 0; y < h; y += 8) {
                for (x = 0; x < w; x += 16) {
                    sum += satd(&org[x], s_org, &cur[x], s_cur);
                }
                org += offset_org;
                cur += offset_cur;
            }
            return sum >> shift;
        }
    }
    else {
        if (w == 4) {
            int  offset_org = s_org << 3;
            int  offset_cur = s_cur << 3;
            satd = avs3_funs_handle.cost_satd[0][1];

            for (y = 0; y < h; y += 8) {
                sum += satd(org, s_org, cur, s_cur);
                org += offset_org;
                cur += offset_cur;
            }
            return sum >> shift;
        }
        else if (w & 7) {
            int  offset_org = s_org << 3;
            int  offset_cur = s_cur << 3;
            satd = avs3_funs_handle.cost_satd[0][1];

            for (y = 0; y < h; y += 8) {
                sum += satd(org, s_org, cur, s_cur);
                sum += satd(org + 4, s_org, cur + 4, s_cur);
                sum += satd(org + 8, s_org, cur + 8, s_cur);
                org += offset_org;
                cur += offset_cur;
            }
            return sum >> shift;
        }
        else {
            int  offset_org = s_org << 4;
            int  offset_cur = s_cur << 4;
            satd = avs3_funs_handle.cost_satd[1][2];

            for (y = 0; y < h; y += 16) {
                for (x = 0; x < w; x += 8) {
                    sum += satd(&org[x], s_org, &cur[x], s_cur);
                }
                org += offset_org;
                cur += offset_cur;
            }
            return sum >> shift;
        }
    }
}

static  void com_mset_pel(pel *dst, s16 v, int cnt)
{
#if (BIT_DEPTH == 8)
    memset(dst, v, cnt);
#else
    int i;
    for (i = 0; i < cnt; i++) {
        dst[i] = v;
    }
#endif
}


static void get_ipred_neighbor(pel *dst, int x, int y, int w, int h, int pic_width, int pic_height, pel *src, int s_src, int bit_depth)
{
    pel *srcT  = src - s_src;
    pel *srcTL = srcT - 1;
    pel *srcL  = src - 1;
    pel *left  = dst - 1;
    pel *up    = dst + 1;
    int default_val = 1 << (bit_depth - 1);
    int pad_range = COM_MAX(w, h) * 4 + 4;

    if (y) {
        int size = COM_MIN(2 * w, pic_width - x);
        com_mcpy(up, srcT, size * sizeof(pel));
        up += size, srcT += size;
        com_mset_pel(up, up[-1], pad_range - size);
    } else {
        com_mset_pel(up, default_val, pad_range);
    }
    if (x) {
        pel *src = srcL;
        int size = COM_MIN(2 * h, pic_height - y);

        for (int i = 0; i < size; ++i) {
            *left-- = *src;
            src += s_src;
        }
        int pads = pad_range - size;
        com_mset_pel(left - pads + 1, left[1], pad_range - size);
    } else {
        com_mset_pel(left - pad_range + 1, default_val, pad_range);
    }
    if (x && y) {
        dst[0] = srcTL[0];
    } else if (y) {
        dst[0] = srcTL[1];
    } else if (x) {
        dst[0] = srcL[0];
    } else {
        dst[0] = default_val;
    }
}

static void com_mv_mvr_check(s16* mv, int mvr)
{
    int mvr_mask = (1 << mvr) - 1;
    if ((mv[MV_X] & mvr_mask) || (mv[MV_Y] & mvr_mask)) {
        com_assert(0);
    }
}
static void  com_ipel_range_rounding(s16 mv[MV_D], s32 mvr)
{
    if (mvr > 2) {
        int shift = mvr - 2;
        mv[0] = (mv[0] >= 0) ? ((mv[0] >> shift) << shift) : -(((-mv[0]) >> shift) << shift);
        mv[1] = (mv[1] >= 0) ? ((mv[1] >> shift) << shift) : -(((-mv[1]) >> shift) << shift);
    }
}

static int get_max_search_range(inter_search_t *pi, const s16 mvp[2], int x, int y, int w, int h, int pic_width, int pic_height)
{
    int max_table_offset = (pi->tab_mvbits_offset << pi->curr_mvr) >> 2;
    int mvp_x = mvp[0] >> 2;
    int mvp_y = mvp[1] >> 2;

    if (pi->is_padding) {
        pi->min_mv[0] = COM_MAX(-MAX_CU_SIZE + 1 - x, mvp_x - max_table_offset + 4);
        pi->min_mv[1] = COM_MAX(-MAX_CU_SIZE + 1 - y, mvp_y - max_table_offset + 4);
        pi->max_mv[0] = COM_MIN(pic_width  - 1 - x,   mvp_x + max_table_offset - 4);
        pi->max_mv[1] = COM_MIN(pic_height - 1 - y,   mvp_y + max_table_offset - 4);
    } else {
        pi->min_mv[0] = COM_MAX(- x, mvp_x - max_table_offset + 4);
        pi->min_mv[1] = COM_MAX(- y, mvp_y - max_table_offset + 4);
        pi->max_mv[0] = COM_MIN(pic_width  - 1 - x - w, mvp_x + max_table_offset - 4);
        pi->max_mv[1] = COM_MIN(pic_height - 1 - y - h, mvp_y + max_table_offset - 4);
    }

    com_ipel_range_rounding(pi->min_mv, pi->curr_mvr); 
    com_ipel_range_rounding(pi->max_mv, pi->curr_mvr);

    if (pi->min_mv[MV_X] > pi->max_mv[MV_X] || pi->min_mv[MV_Y] > pi->max_mv[MV_Y]) {
        return 0;
    }
    return 1;
}

static void get_raster_range(inter_search_t *pi, int lidx, int refi, s16 center[MV_D], s16 range[MV_RANGE_DIM][MV_D], int search_step)
{
    COM_PIC *ref_pic = pi->ref_pic;
    int mvr = pi->curr_mvr;
    int dist = COM_ABS((int)(pi->ptr - ref_pic->ptr));
    int max_sr = pi->max_search_range >> COM_MAX(3 - mvr, 0);
    int range_xy = COM_CLIP3(max_sr >> 2, max_sr, (max_sr * dist + (pi->gop_size >> 1)) / pi->gop_size);
    int range_x = range_xy;
    int range_y = range_xy;

    if (mvr && pi->adaptive_raster_range) {
        range_x = COM_ABS(pi->mv_ipel[lidx][refi][0] - center[0]) * 4;
        range_y = COM_ABS(pi->mv_ipel[lidx][refi][1] - center[1]) * 2;
        range_x = COM_MAX(range_x, max_sr >> 2);
        range_y = COM_MAX(range_y, max_sr >> 2);
        range_x = COM_MAX(range_x, search_step * 8);
        range_y = COM_MAX(range_y, search_step * 4);
        range_x = COM_MIN(range_x, range_xy);
        range_y = COM_MIN(range_y, range_xy);
    }

    if (mvr > 2) {
        int shift = mvr - 2;
        range_x = (range_x >> shift) << shift;
        range_y = (range_y >> shift) << shift;
    }

    /* define search range for int-pel search and clip it if needs */
    range[MV_RANGE_MIN][MV_X] = COM_CLIP3(pi->min_mv[MV_X], pi->max_mv[MV_X], center[MV_X] - (s16)range_x);
    range[MV_RANGE_MAX][MV_X] = COM_CLIP3(pi->min_mv[MV_X], pi->max_mv[MV_X], center[MV_X] + (s16)range_x);
    range[MV_RANGE_MIN][MV_Y] = COM_CLIP3(pi->min_mv[MV_Y], pi->max_mv[MV_Y], center[MV_Y] - (s16)range_y);
    range[MV_RANGE_MAX][MV_Y] = COM_CLIP3(pi->min_mv[MV_Y], pi->max_mv[MV_Y], center[MV_Y] + (s16)range_y);

    com_assert(range[MV_RANGE_MIN][MV_X] <= range[MV_RANGE_MAX][MV_X]);
    com_assert(range[MV_RANGE_MIN][MV_Y] <= range[MV_RANGE_MAX][MV_Y]);
}
static int search_diamond(inter_search_t *pi, int x, int y, int w, int h, s8 refi, int lidx, const s16 mvp[MV_D], s16 mv[MV_D], u64* cost_best, int bi, int max_step)
{
    int best_step      = 0;
    int bit_depth      = pi->bit_depth;
    int i_org          = pi->i_org;
    pel *org           = pi->org;
    COM_PIC *ref_pic = pi->ref_pic;
    u8 *tab_mvbits_x   = pi->tab_mvbits - (mvp[MV_X] >> pi->curr_mvr);
    u8 *tab_mvbits_y   = pi->tab_mvbits - (mvp[MV_Y] >> pi->curr_mvr);
    int mvr_idx        = pi->curr_mvr;
    u32 lambda_mv      = pi->lambda_mv;
    int i_ref          = ref_pic->stride_luma;
    pel *ref           = ref_pic->y + y * i_ref + x;
    int shift          = 16 - bi - (bit_depth - 8); // sad << 16 for u64 cost
    int core_size      = (bi ? 5 : 2);
    int core_step      = 1;
    const s16 center_x = mv[MV_X];
    const s16 center_y = mv[MV_Y];
    const int widx     = CONV_LOG2(w) - 2;
    u32 sad;
    u64 cost;
	
    if (bi) {
        if (pi->curr_mvr > 2) {
            core_size++;
        }
        int ipel_mvp_x = (mvp[0] + 2) >> 2;
        int ipel_mvp_y = (mvp[1] + 2) >> 2;
        int diff_x = abs(ipel_mvp_x - center_x);
        int diff_y = abs(ipel_mvp_y - center_y);
        int diff = COM_MAX(diff_x, diff_y) >> COM_MAX(0, pi->curr_mvr - 2);
        core_size = COM_CLIP3(2, core_size, diff);
    }
    if (pi->curr_mvr > 2) {
        core_size <<= (pi->curr_mvr - 2);
        core_step <<= (pi->curr_mvr - 2);
    }

    int min_cmv_x = center_x - core_size;
    int min_cmv_y = center_y - core_size;
    int max_cmv_x = center_x + core_size;
    int max_cmv_y = center_y + core_size;

    while (min_cmv_x <= pi->min_mv[MV_X]) min_cmv_x += core_step;
    while (min_cmv_y <= pi->min_mv[MV_Y]) min_cmv_y += core_step;
    while (max_cmv_x >= pi->max_mv[MV_X]) max_cmv_x -= core_step;
    while (max_cmv_y >= pi->max_mv[MV_Y]) max_cmv_y -= core_step;

    /* 1. try center pointer first */
    pel *p = ref + center_y * i_ref + center_x;

    sad = avs3_funs_handle.cost_sad[widx](org, i_org, p, i_ref, h);
    cost = ((u64)sad << shift) + (GET_MVBITS_IPEL_Y(center_y) + GET_MVBITS_IPEL_X(center_x)) * lambda_mv;

    if (cost < *cost_best) {
        mv[MV_X] = center_x;
        mv[MV_Y] = center_y;
        best_step = 0;
        *cost_best = cost;
    }

    /* 2. try core squre */
    if (max_step) {
        u32 cost_mvx[13];
        u32* pcmvx = cost_mvx;
        int core_step_x2 = core_step << 1;
        int core_step_x3 = core_step_x2 + core_step;
        int core_step_x4 = core_step << 2;
        int max_cmv_x_x4 = max_cmv_x - 3 * core_step;

        for (s16 mv_x = min_cmv_x; mv_x <= max_cmv_x; mv_x += core_step) {
            *pcmvx++ = GET_MVBITS_IPEL_X(mv_x) * lambda_mv;
        }
        for (s16 mv_y = min_cmv_y; mv_y <= max_cmv_y; mv_y += core_step) {
            u32 cost_mvy = GET_MVBITS_IPEL_Y(mv_y) * lambda_mv;
            pel *p = ref + mv_y * i_ref;
            s16 mv_x = min_cmv_x;
            u32 sad_x4[4];
            s64 tmp_best = *cost_best - cost_mvy;

            pcmvx = cost_mvx;

            for (; mv_x <= max_cmv_x_x4; mv_x += core_step_x4) {
                pel *p0 = p + mv_x;
                avs3_funs_handle.cost_sad_x4[widx](org, i_org, p0, p0 + core_step, p0 + core_step_x2, p0 + core_step_x3, i_ref, sad_x4, h);
                s64 cost0 = (*pcmvx++) + ((s64)sad_x4[0] << shift);
                s64 cost1 = (*pcmvx++) + ((s64)sad_x4[1] << shift);
                s64 cost2 = (*pcmvx++) + ((s64)sad_x4[2] << shift);
                s64 cost3 = (*pcmvx++) + ((s64)sad_x4[3] << shift);
                if (cost0 < tmp_best) { mv[0] = mv_x, mv[1] = mv_y, tmp_best = cost0; }
                if (cost1 < tmp_best) { mv[0] = mv_x + core_step, mv[1] = mv_y, tmp_best = cost1; }
                if (cost2 < tmp_best) { mv[0] = mv_x + core_step_x2, mv[1] = mv_y, tmp_best = cost2; }
                if (cost3 < tmp_best) { mv[0] = mv_x + core_step_x3, mv[1] = mv_y, tmp_best = cost3; }
            }
            if (mv_x + 2 * core_step <= max_cmv_x) {
                pel *p0 = p + mv_x;
                avs3_funs_handle.cost_sad_x3[widx](org, i_org, p0, p0 + core_step, p0 + core_step_x2, i_ref, sad_x4, h);
                s64 cost0 = (*pcmvx++) + ((s64)sad_x4[0] << shift);
                s64 cost1 = (*pcmvx++) + ((s64)sad_x4[1] << shift);
                s64 cost2 = (*pcmvx++) + ((s64)sad_x4[2] << shift);
                if (cost0 < tmp_best) { mv[0] = mv_x, mv[1] = mv_y, tmp_best = cost0; }
                if (cost1 < tmp_best) { mv[0] = mv_x + core_step, mv[1] = mv_y, tmp_best = cost1; }
                if (cost2 < tmp_best) { mv[0] = mv_x + core_step_x2, mv[1] = mv_y, tmp_best = cost2; }
                mv_x  += 3 * core_step;
            }
            for (; mv_x <= max_cmv_x; mv_x += core_step) {
                cost = (*pcmvx++) + ((u64)(avs3_funs_handle.cost_sad[widx](org, i_org, p + mv_x, i_ref, h)) << shift);
                if ((s64)cost < tmp_best) { mv[0] = mv_x; mv[1] = mv_y; tmp_best = cost; }
            }

            *cost_best = tmp_best + cost_mvy;
        }
    }

    if (mv[0] != center_x || mv[1] != center_y) {
        best_step = 2;
    }

    if (bi) {
        return best_step;
    }

    /* 3. try multilayer diamond */
    int mvr_scale = COM_MAX(pi->curr_mvr - 2, 0);

    for (int step = 4, round = 0; step < max_step; step <<= 1, round++) {
        int step_scale = (step << mvr_scale) >> 1;
        int search_num = round < 2 ? 8 : 16;
        int search_add = round < 1 ? 2 :  1;
        int search_idx = round < 2 ? 0 :  1;

        static signed char tbl_diapos_partial[2][16][2] = {
            { { -2, 0}, { -1, 1}, {  0, 2}, {  1, 1}, {2, 0}, {1, -1}, {0, -2}, { -1, -1} }, 
            { { -4, 0}, { -3, 1}, { -2, 2}, { -1, 3}, {0, 4}, {1,  3}, {2,  2}, {  3,  1}, {4, 0}, {3, -1}, {2, -2}, {1, -3}, {0, -4}, { -1, -3}, { -2, -2}, { -3, -1}}
        };

        for (int i = 0; i < search_num; i += search_add) {
            s16 mv_x = center_x + ((s16)(step_scale >> search_idx) * tbl_diapos_partial[search_idx][i][MV_X]);
            s16 mv_y = center_y + ((s16)(step_scale >> search_idx) * tbl_diapos_partial[search_idx][i][MV_Y]);

            if (mv_x <= pi->max_mv[MV_X] && mv_x >= pi->min_mv[MV_X] &&
                mv_y <= pi->max_mv[MV_Y] && mv_y >= pi->min_mv[MV_Y]) {
                u64 cost = ((u64)avs3_funs_handle.cost_sad[widx](org, i_org, ref + mv_x + mv_y * i_ref, i_ref, h) << shift)
                         + (GET_MVBITS_IPEL_X(mv_x) + GET_MVBITS_IPEL_Y(mv_y)) * lambda_mv;

                if (cost < *cost_best) {
                    mv[MV_X] = mv_x;
                    mv[MV_Y] = mv_y;
                    best_step = step;
                    *cost_best = cost;
                }
            }
        }
    }

    return best_step;
}

static void search_raster(inter_search_t *pi, int x, int y, int w, int h, s8 refi, int lidx, const s16 mvp[MV_D], s16 mv[MV_D], u64 *cost_best)
{
    static const int step_base[5] = { 5, 5, 5, 6, 8 };
    static const int step_min[5] = { 1, 1, 1, 2, 4 };
    s16 range[MV_RANGE_DIM][MV_D];

    int bit_depth = pi->bit_depth;
    int i_org = pi->i_org;
    pel *org = pi->org;
    COM_PIC *ref_pic = pi->ref_pic;
    u8 *tab_mvbits_x = pi->tab_mvbits - (mvp[MV_X] >> pi->curr_mvr);
    u8 *tab_mvbits_y = pi->tab_mvbits - (mvp[MV_Y] >> pi->curr_mvr);
    int mvr_idx = pi->curr_mvr;
    u32 lambda_mv = pi->lambda_mv;
    int i_ref = ref_pic->stride_luma;
    pel *ref = ref_pic->y + y * i_ref + x;
    int shift = 16 - (bit_depth - 8); // sad << 16 for u64 cost
    const int widx = CONV_LOG2(w) - 2;
    int search_step = COM_MAX(step_base[pi->curr_mvr], COM_MIN(w >> 1, h >> 1)) * (refi + 1);
    int round = 0;
    int step_shift = pi->curr_mvr - 2;

    get_raster_range(pi, lidx, refi, mv, range, search_step);

    while (search_step >= step_min[pi->curr_mvr]) {
        s16 min_cmv_x, min_cmv_y;
        s16 max_cmv_x, max_cmv_y;

        if (round++ == 0) {
            min_cmv_x = range[MV_RANGE_MIN][MV_X];
            min_cmv_y = range[MV_RANGE_MIN][MV_Y];
            max_cmv_x = range[MV_RANGE_MAX][MV_X];
            max_cmv_y = range[MV_RANGE_MAX][MV_Y];
        }
        else {
            min_cmv_x = COM_MAX(mv[MV_X] - search_step, pi->min_mv[MV_X]);
            min_cmv_y = COM_MAX(mv[MV_Y] - search_step, pi->min_mv[MV_Y]);
            max_cmv_x = COM_MIN(mv[MV_X] + search_step, pi->max_mv[MV_X]);
            max_cmv_y = COM_MIN(mv[MV_Y] + search_step, pi->max_mv[MV_Y]);
        }

        u32 cost_mvx[(SEARCH_RANGE_IPEL_RA * 2 + 1) / 5 + 1];
        u32* pcmvx = cost_mvx;
        int search_step_x2 = search_step << 1;
        int search_step_x3 = search_step_x2 + search_step;
        int search_step_x4 = search_step << 2;
        int max_cmv_x_x4 = max_cmv_x - 3 * search_step;

        for (s16 mv_x = min_cmv_x; mv_x <= max_cmv_x; mv_x += search_step) {
            *pcmvx++ = GET_MVBITS_IPEL_X(mv_x) * lambda_mv;
        }
        for (s16 mv_y = min_cmv_y; mv_y <= max_cmv_y; mv_y += search_step) {
            pel* p = ref + mv_y * i_ref;
            u32 cost_mvy = GET_MVBITS_IPEL_Y(mv_y) * lambda_mv;
            s16 mv_x = min_cmv_x;
            u32 sad[4];
            s64 tmp_best = *cost_best - cost_mvy;

            pcmvx = cost_mvx;

            for (; mv_x <= max_cmv_x_x4; mv_x += search_step_x4) {
                pel *p0 = p + mv_x;
                avs3_funs_handle.cost_sad_x4[widx](org, i_org, p0, p0 + search_step, p0 + search_step_x2, p0 + search_step_x3, i_ref, sad, h);
                s64 cost0 = (*pcmvx++) + ((s64)sad[0] << shift);
                s64 cost1 = (*pcmvx++) + ((s64)sad[1] << shift);
                s64 cost2 = (*pcmvx++) + ((s64)sad[2] << shift);
                s64 cost3 = (*pcmvx++) + ((s64)sad[3] << shift);
                if (cost0 < tmp_best) { mv[0] = mv_x, mv[1] = mv_y, tmp_best = cost0; }
                if (cost1 < tmp_best) { mv[0] = mv_x + search_step, mv[1] = mv_y, tmp_best = cost1; }
                if (cost2 < tmp_best) { mv[0] = mv_x + search_step_x2, mv[1] = mv_y, tmp_best = cost2; }
                if (cost3 < tmp_best) { mv[0] = mv_x + search_step_x3, mv[1] = mv_y, tmp_best = cost3; }
            }
            if (mv_x + 2 * search_step <= max_cmv_x) {
                pel *p0 = p + mv_x;
                avs3_funs_handle.cost_sad_x3[widx](org, i_org, p0, p0 + search_step, p0 + search_step_x2, i_ref, sad, h);
                s64 cost0 = (*pcmvx++) + ((s64)sad[0] << shift);
                s64 cost1 = (*pcmvx++) + ((s64)sad[1] << shift);
                s64 cost2 = (*pcmvx++) + ((s64)sad[2] << shift);
                if (cost0 < tmp_best) { mv[0] = mv_x, mv[1] = mv_y, tmp_best = cost0; }
                if (cost1 < tmp_best) { mv[0] = mv_x + search_step, mv[1] = mv_y, tmp_best = cost1; }
                if (cost2 < tmp_best) { mv[0] = mv_x + search_step_x2, mv[1] = mv_y, tmp_best = cost2; }
                mv_x += 3 * search_step;
            }
            for (; mv_x <= max_cmv_x; mv_x += search_step) {
                s64 cost = (*pcmvx++) + ((u64)avs3_funs_handle.cost_sad[widx](org, i_org, p + mv_x, i_ref, h) << shift);
                if (cost < tmp_best) { mv[MV_X] = mv_x; mv[MV_Y] = mv_y; tmp_best = cost; }
            }
            *cost_best = tmp_best + cost_mvy;
        }
        search_step >>= 1;

        if (pi->curr_mvr > 2) {
            search_step = (search_step >> step_shift) << step_shift;
        }
    }
}
u64 me_search_tz(inter_search_t *pi, int x, int y, int w, int h, int pic_width, int pic_height, s8 refi, int lidx, const s16 mvp[MV_D], s16 mv[MV_D], int bi)
{
    if (!get_max_search_range(pi, mvp, x, y, w, h, pic_width, pic_height)) {
        return COM_UINT64_MAX;
    }
    if (!bi) {
        CP32(mv, mvp);
    }

    com_mv_mvr_check(mv, pi->curr_mvr);

    mv[MV_X] = COM_CLIP3(pi->min_mv[MV_X], pi->max_mv[MV_X], (mv[MV_X] + 2) >> 2);
    mv[MV_Y] = COM_CLIP3(pi->min_mv[MV_Y], pi->max_mv[MV_Y], (mv[MV_Y] + 2) >> 2);

    u64 cost_best = COM_UINT64_MAX;
    int best_step = search_diamond(pi, x, y, w, h, refi, lidx, mvp, mv, &cost_best, bi, MAX_FIRST_SEARCH_STEP);

    if (!bi && (abs(mvp[MV_X] - (mv[MV_X] << 2)) > 2 || abs(mvp[MV_Y] - (mv[MV_Y] << 2)) > 2)) {
        if (best_step > RASTER_SEARCH_THD) {
            search_raster(pi, x, y, w, h, refi, lidx, mvp, mv, &cost_best);
        }
        search_diamond(pi, x, y, w, h, refi, lidx, mvp, mv, &cost_best, bi, MAX_REFINE_SEARCH_STEP);
    }
    if (!bi) {
        CP32(pi->mv_ipel[lidx][refi], mv);
    }
    mv[0] <<= 2, mv[1] <<= 2;

//    if (pi->curr_mvr < 2) {
//        cost_best = me_sub_pel_search(pi, x, y, w, h,  refi, lidx, mvp, mv, bi); // this cost is satd
//    }
    int extra_bits = tbl_refi_bits[pi->num_refp][refi] + pi->curr_mvr + (pi->curr_mvr < MAX_NUM_MVR - 1) +
                    (bi ? pi->mot_bits[(lidx == REFP_0) ? REFP_1 : REFP_0] + 1 : 2);

    cost_best += pi->lambda_mv * (extra_bits);

    com_mv_mvr_check(mv, pi->curr_mvr);

    return (u32)((cost_best + (1 << 15)) >> 16);
}

void inter_search_init(inter_search_t *pi, COM_INFO *info, int is_padding)
{
    pi->tab_mvbits = info->tab_mvbits;
    pi->bit_depth = info->bit_depth_internal;
    pi->gop_size = info->gop_size;
    pi->max_search_range = info->sqh.low_delay ? SEARCH_RANGE_IPEL_LD : SEARCH_RANGE_IPEL_RA;
    pi->max_coord[MV_X] = (s16)info->pic_width + 4;
    pi->max_coord[MV_Y] = (s16)info->pic_height + 4;
    pi->is_padding = is_padding;

    pi->tab_mvbits_offset = (COM_MAX(info->pic_width, info->pic_height) << 2) + 3; // max abs(MV)
    pi->lambda_mv = 0;
}
int inter_search_create(u8 **pptab, COM_INFO *info)
{
    int tab_mvbits_offset;
    u8 *tab_mvbits;

    tab_mvbits_offset = (COM_MAX(info->pic_width, info->pic_height) << 2) + 3; // max abs(MV)
    tab_mvbits = (u8 *)com_malloc(sizeof(u8) * (tab_mvbits_offset * 2 + 1)) + tab_mvbits_offset;

    tab_mvbits[0] = 1;
    tab_mvbits[-1] = tab_mvbits[1] = 2;
    tab_mvbits[-2] = tab_mvbits[2] = 3;

    for (int exp_bits = 2;; exp_bits++) {
        int imax = (1 << exp_bits) - 1;
        int imin = (1 << (exp_bits - 1)) - 1;
        int bits = exp_bits << 1;

        imax = COM_MIN(imax, tab_mvbits_offset);

        for (int i = imin; i < imax; i++) {
            tab_mvbits[-i] = tab_mvbits[i] = bits;
        }
        if (imax == tab_mvbits_offset) {
            break;
        }
    }

    *pptab = tab_mvbits;
    return tab_mvbits_offset;
}

void inter_search_free(u8 *tab_mvbits, int tab_mvbits_offset)
{
    u8 *addr = tab_mvbits - tab_mvbits_offset;
    com_mfree(addr);
}


double loka_estimate_coding_cost(inter_search_t *pi, COM_PIC *img_org, 
                                 COM_IMGB **ref_l0, COM_IMGB **ref_l1, int  num_ref[2],
                                 int bit_depth, double *icost, double icost_uv[2], float* map_dqp)
{
    const int    base_qp = 32;
    const double base_lambda = 1.43631 * pow(2.0, (base_qp - 16.0) / 4.0);
    const int is_bi = (num_ref[0] && num_ref[1]);

    int pic_width  = img_org->imgb->width[0];
    int pic_height = img_org->imgb->height[0];

    double total_cost = 0;
    double total_icost = 0;
    double total_icost_u = 0;
    double total_icost_v = 0;
    double total_var = 0;

    int i_org = STRIDE_IMGB2PIC(img_org->imgb->stride[0]);
    int i_org_c = STRIDE_IMGB2PIC(img_org->imgb->stride[1]);
   
    COM_PIC pic = { 0 };
   
    pi->ptr       = img_org->ptr;
    pi->curr_mvr  = 2;
    pi->lambda_mv = (u32)floor(65536.0 * sqrt(base_lambda));
    pi->i_org     = i_org;
    pi->adaptive_raster_range   = 0;
    pi->subpel_cost_type        = 0;

#define WRITE_REC_PIC 0

#if WRITE_REC_PIC
    static pel *buf = NULL;
    if (buf == NULL) buf = malloc(pic_width * pic_height);
    static FILE *fp = NULL;
    if (fp == NULL) fp = fopen("preprocess.yuv", "wb");
    static FILE *fp_org = NULL;
    if (fp_org == NULL) fp_org = fopen("preprocess_org.yuv", "wb");
#endif

#define UNIT_DIM (1 << (UNIT_SIZE_LOG2 << 1))
    for (int y = 0; y < pic_height - UNIT_SIZE + 1; y += UNIT_SIZE) {
        float *var = map_dqp + (y / UNIT_SIZE) * (pic_width / UNIT_SIZE);

        for (int x = 0; x < pic_width - UNIT_SIZE + 1; x += UNIT_SIZE) {
            ALIGNED_32(pel pred_buf[UNIT_DIM]);

            u64 min_cost = COM_UINT64_MAX;
            pel *org = (pel*)img_org->imgb->addr_plane[0] + y * i_org + x;

            pi->org = org;

            for (int lidx = 0; lidx < 1; lidx++) { // uni-prediction (L0 or L1)
                pi->num_refp = num_ref[lidx];

                u64 min_cost_cur_dir = COM_UINT64_MAX;
                for (int refi = 0; refi < num_ref[lidx]; refi++) {
                    COM_IMGB *ref_img = (lidx ? ref_l1 : ref_l0)[refi];
                    pic.ptr = ref_img->ptr;
                   // pic.img = ref_img;
                    pic.stride_luma = STRIDE_IMGB2PIC(ref_img->stride[0]);
                    pic.y = (pel*)ref_img->addr_plane[0];
                    pic.subpel.imgs[0][0] = ref_img;

                    COM_PIC *ref_pic = pi->ref_pic = &pic;
                    s16 mv[2], mvp[2] = { 0 };

                    me_search_tz(pi, x, y, UNIT_SIZE, UNIT_SIZE, pic_width, 
                    pic_height, refi, lidx, mvp, mv, 0);
                    com_mc_blk_luma(ref_pic, pred_buf, UNIT_SIZE, (x << 2) + 
                    mv[MV_X], (y << 2) + mv[MV_Y], UNIT_SIZE, UNIT_SIZE, UNIT_WIDX, pi->max_coord[
                    MV_X], pi->max_coord[MV_Y], (1 << bit_depth) - 1, 0);

                    u32 cost = com_had(UNIT_SIZE, UNIT_SIZE, org, i_org, pred_buf, UNIT_SIZE, bit_depth);
                    if (cost < min_cost_cur_dir) {
#if WRITE_REC_PIC 
                        avs3_funs_handle.ipcpy[UNIT_WIDX](pred_buf, 
                        UNIT_SIZE, buf + y * pic_width + x, pic_width, UNIT_SIZE, UNIT_SIZE);
#endif
                        min_cost_cur_dir = cost;
                        if (cost < min_cost) {
                            min_cost = cost;
                        }
                        
                    }
                }
            }

           
            pel nb_buf[INTRA_NEIB_SIZE];
            get_ipred_neighbor(nb_buf + INTRA_NEIB_MID, x, y, UNIT_SIZE, 
UNIT_SIZE, pic_width, pic_height, org, i_org, bit_depth);

            int avaliable_nb = (x ? AVAIL_LE : 0) | (y ? AVAIL_UP : 0) | ((x 
&& y) ? AVAIL_UP_LE : 0);
            static signed char ipm_tab[] = { 0, 1, 2, 4, 8, 12, 16, 20, 24, 28, 32 
};
            u32 min_icost = u32(COM_UINT64_MAX);
            int best_mode;

            for (int i = 0; i < sizeof(ipm_tab); i++) {
                com_intra_pred(nb_buf + INTRA_NEIB_MID, pred_buf, ipm_tab[i], 
UNIT_SIZE, UNIT_SIZE, bit_depth, avaliable_nb, 0);
                u32 cost = com_had(UNIT_SIZE, UNIT_SIZE, org, i_org, pred_buf, UNIT_SIZE, bit_depth);
 

                if (cost < min_cost) {
#if WRITE_REC_PIC 
                    avs3_funs_handle.ipcpy[UNIT_WIDX](pred_buf, UNIT_SIZE, 
buf + y * pic_width + x, pic_width, UNIT_SIZE, UNIT_SIZE);
#endif
                    min_cost = cost;
                }
                if (cost < min_icost) {
                    min_icost = cost;
                    best_mode = ipm_tab[i];
                }
            }
            if (icost_uv) {
                int xc = x >> 1;
                int yc = y >> 1;
                pel *orgu = (pel*)img_org->imgb->addr_plane[1] + yc * i_org_c + xc;
                pel *orgv = (pel*)img_org->imgb->addr_plane[2] + yc * i_org_c + xc;

                get_ipred_neighbor(nb_buf + INTRA_NEIB_MID, xc, yc, UNITC_SIZE
, UNITC_SIZE, pic_width / 2, pic_height / 2, orgu, i_org_c, bit_depth);
                com_intra_pred(nb_buf + INTRA_NEIB_MID, pred_buf, best_mode, 
UNITC_SIZE, UNITC_SIZE, bit_depth, avaliable_nb, 0);
                total_icost_u += com_had(UNIT_SIZE, UNIT_SIZE, org, i_org, pred_buf, UNIT_SIZE, bit_depth);


                get_ipred_neighbor(nb_buf + INTRA_NEIB_MID, xc, yc, UNITC_SIZE
, UNITC_SIZE, pic_width / 2, pic_height / 2, orgv, i_org_c, bit_depth);
                com_intra_pred(nb_buf + INTRA_NEIB_MID, pred_buf, best_mode, 
UNITC_SIZE, UNITC_SIZE, bit_depth, avaliable_nb, 0);
                total_icost_v += com_had(UNIT_SIZE, UNIT_SIZE, org, i_org, pred_buf, UNIT_SIZE, bit_depth);

            }

            total_cost  += min_cost;
            total_icost += min_icost;

            if (map_dqp) {
                u64 energy = 0;
                int xc = x >> 1;
                int yc = y >> 1;
                pel *orgu = (pel*)img_org->imgb->addr_plane[1] + yc * i_org_c + xc;
                pel *orgv = (pel*)img_org->imgb->addr_plane[2] + yc * i_org_c + xc;

                energy += avs3_funs_handle.cost_var[UNIT_WIDX](org, i_org);
                energy += avs3_funs_handle.cost_var[UNITC_WIDX](orgu, 
i_org_c);
                energy += avs3_funs_handle.cost_var[UNITC_WIDX](orgv, 
i_org_c);
                energy >>= (bit_depth - 8) * 2;

                var[x / UNIT_SIZE] = (float)(0.8 * log2((double)COM_MAX(energy
, 1)));
                total_var += var[x / UNIT_SIZE];
            }
        }        
    }
#undef UNIT_DIM

#if WRITE_REC_PIC
    fwrite(buf, 1, pic_width * pic_height, fp);
    fwrite(img_org->planes[0], 1, pic_width * pic_height, fp_org);
#endif

#undef WRITE_REC_PIC

    int blk_num = (pic_width / UNIT_SIZE) * (pic_height / UNIT_SIZE);

    if (icost) {
        *icost = total_icost / blk_num / UNIT_SIZE / UNIT_SIZE;
    }
    if (icost_uv) {
        icost_uv[0] = total_icost_u / blk_num / UNITC_SIZE / UNITC_SIZE;
        icost_uv[1] = total_icost_v / blk_num / UNITC_SIZE / UNITC_SIZE;
    }

    if (map_dqp) {
        float avg_var = (float)(total_var / blk_num);
        for (int i = 0; i < blk_num; i++) {
            map_dqp[i] -= avg_var;
        }
    }
    return total_cost / blk_num / UNIT_SIZE / UNIT_SIZE;
}






