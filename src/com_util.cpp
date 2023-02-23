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

#include "com_def.h"
#include "enc_def.h"
#include "type_def.h"
extern int imgb_release(COM_IMGB * imgb);
#if IP_map_scu
//extern IP_MAP_SCU *MAP_SCU_IP_ptr;
#endif

#if ENC_DEC_TRACE
FILE* fp_trace;
#if TRACE_RDO
#if TRACE_RDO_EXCLUDE_I
int fp_trace_print = 0;
#else
int fp_trace_print = 1;
#endif
#else
int fp_trace_print = 1;
#endif
int fp_trace_counter = 0;
int aec_is_this_line = 0;
#endif

//extern MD_FW *md_fw_ptr;

//extern CTU_Level_Parameters fetch_output;  //CTX_CONTROL_LUKE
extern u16 *com_scan_tbl[COEF_SCAN_TYPE_NUM][MAX_CU_LOG2][MAX_CU_LOG2];

static const s8 com_tbl_log2[257] = {
    /* 0, 1 */
    -1, -1,
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

#define CONV_LOG2(v)                    (com_tbl_log2[v])

void com_mv_rounding_s32(s32 hor, int ver, CPMV *rounded_hor, CPMV *rounded_ver, s32 right_shift, int left_shift)
{
	int add = (right_shift > 0) ? (1 << (right_shift - 1)) : 0;
	*rounded_hor = (hor >= 0) ? (((hor + add) >> right_shift) << left_shift) : -(((-hor + add) >> right_shift) <<
		left_shift);
	*rounded_ver = (ver >= 0) ? (((ver + add) >> right_shift) << left_shift) : -(((-ver + add) >> right_shift) <<
		left_shift);
}

void com_mv_rounding_s16(s32 hor, s32 ver, s16 * rounded_hor, s16 * rounded_ver, int right_shift, int left_shift)
{
    int add = (right_shift > 0) ? (1 << (right_shift - 1)) : 0;
    *rounded_hor = (hor >= 0) ? (((hor + add) >> right_shift) << left_shift) : -(((-hor + add) >> right_shift) <<
                   left_shift);
    *rounded_ver = (ver >= 0) ? (((ver + add) >> right_shift) << left_shift) : -(((-ver + add) >> right_shift) <<
                   left_shift);
}

void memcpy_rewrintra2(s16* des, s16* src, int n)
{
#pragma HLS INLINE off
    int z;
    for (z = 0; z < n; z++) {
#pragma HLS LOOP_TRIPCOUNT max=4
        des[z] = src[z];
    }
}

COM_PIC * com_pic_alloc(PICBUF_ALLOCATOR * pa, int * ret)
{
    return com_picbuf_alloc(pa->width, pa->height, pa->pad_l, pa->pad_c, ret);
}

void com_pic_free(PICBUF_ALLOCATOR *pa, COM_PIC *pic)
{
    com_picbuf_free(pic);
}

int com_atomic_inc(volatile int *pcnt)
{
    int ret;
    ret = *pcnt;
    ret++;
    *pcnt = ret;
    return ret;
}

int com_atomic_dec(volatile int *pcnt)
{
    int ret;
    ret = *pcnt;
    ret--;
    *pcnt = ret;
    return ret;
}

COM_PIC * com_picbuf_alloc(int width, int height, int pad_l, int pad_c, int *err)
{
    COM_PIC *pic = NULL;
    COM_IMGB *imgb = NULL;
    int ret, opt, align[COM_IMGB_MAX_PLANE], pad[COM_IMGB_MAX_PLANE];
    int pic_width_in_scu, h_scu, f_scu, size;
    /* allocate PIC structure */
    pic = (COM_PIC *)com_malloc(sizeof(COM_PIC));
    com_assert_gv(pic != NULL, ret, COM_ERR_OUT_OF_MEMORY, ERR);
    com_mset(pic, 0, sizeof(COM_PIC));
    opt = COM_IMGB_OPT_NONE;
    /* set align value*/
    align[0] = MIN_CU_SIZE;
    align[1] = MIN_CU_SIZE >> 1;
    align[2] = MIN_CU_SIZE >> 1;
    /* set padding value*/
    pad[0] = pad_l;
    pad[1] = pad_c;
    pad[2] = pad_c;
    imgb = com_imgb_create(width, height, COM_COLORSPACE_YUV420, pad, align);
    com_assert_gv(imgb != NULL, ret, COM_ERR_OUT_OF_MEMORY, ERR);
    /* set COM_PIC */
    pic->y = (pel*)imgb->addr_plane[0];
    pic->u = (pel*)imgb->addr_plane[1];
    pic->v = (pel*)imgb->addr_plane[2];
    pic->y_rfcBin = (pel*)imgb->addr_plane_rfc[0];
    pic->y_rfcPrm = (u16*)imgb->addr_plane_rfc[1];
    pic->u_rfcBin = (pel*)imgb->addr_plane_rfc[2];
    pic->u_rfcPrm = (u16*)imgb->addr_plane_rfc[3];
    pic->v_rfcBin = (pel*)imgb->addr_plane_rfc[4];
    pic->v_rfcPrm = (u16*)imgb->addr_plane_rfc[5];
    pic->width_luma   = imgb->width[0];
    pic->height_luma   = imgb->height[0];
    pic->width_chroma   = imgb->width[1];
    pic->height_chroma   = imgb->height[1];
    pic->stride_luma   = STRIDE_IMGB2PIC(imgb->stride[0]);
    pic->stride_chroma   = STRIDE_IMGB2PIC(imgb->stride[1]);
    pic->padsize_luma = pad_l;
    pic->padsize_chroma = pad_c;
    pic->imgb  = imgb;
    /* allocate maps */
    pic_width_in_scu = (pic->width_luma + ((1 << MIN_CU_LOG2) - 1)) >> MIN_CU_LOG2;
    h_scu = (pic->height_luma + ((1 << MIN_CU_LOG2) - 1)) >> MIN_CU_LOG2;
    f_scu = pic_width_in_scu * h_scu;
    size = sizeof(s32) * f_scu * 2;
    pic->map_refi = (s32 (*)[2])com_malloc_fast(size);
    com_assert_gv(pic->map_refi, ret, COM_ERR_OUT_OF_MEMORY, ERR);
    com_mset_x64a(pic->map_refi, -1, size);
    size = sizeof(s32) * f_scu * REFP_NUM * MV_D;
    pic->map_mv = (s32 (*)[2][2])com_malloc_fast(size);
    com_assert_gv(pic->map_mv, ret, COM_ERR_OUT_OF_MEMORY, ERR);
    com_mset_x64a(pic->map_mv, 0, size);
    if (err) {
        *err = COM_OK;
    }
    return pic;
ERR:
    if (pic) {
        com_mfree(pic->map_mv);
        com_mfree(pic->map_refi);
        com_mfree(pic);
    }
    if (err) {
        *err = ret;
    }
    return NULL;
}

void com_picbuf_free(COM_PIC *pic)
{
    COM_IMGB *imgb;
    if (pic) {
        imgb = pic->imgb;
        if (imgb) {
            imgb_release(imgb);
            pic->y = NULL;
            pic->u = NULL;
            pic->v = NULL;
            pic->width_luma = 0;
            pic->height_luma = 0;
            pic->width_chroma = 0;
            pic->height_chroma = 0;
            pic->stride_luma = 0;
            pic->stride_chroma = 0;
        }
        com_mfree(pic->map_mv);
        com_mfree(pic->map_refi);
        com_mfree(pic);
    }
}

static void picbuf_expand(pel *a, int s, int width, int height, int exp)
{
    int i, j;
    pel pixel;
    pel *src, *dst;
    /* left */
    src = a;
    dst = a - exp;
    for (i = 0; i < height; i++) {
        pixel = *src; /* get boundary pixel */
        for (j = 0; j < exp; j++) {
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
        for (j = 0; j < exp; j++) {
            dst[j] = pixel;
        }
        dst += s;
        src += s;
    }
    /* upper */
    src = a - exp;
    dst = a - exp - (exp * s);
    for (i = 0; i < exp; i++) {
        com_mcpy(dst, src, s * sizeof(pel));
        dst += s;
    }
    /* below */
    src = a + ((height - 1) * s) - exp;
    dst = a + ((height - 1) * s) - exp + s;
    for (i = 0; i < exp; i++) {
        com_mcpy(dst, src, s * sizeof(pel));
        dst += s;
    }
}

void com_picbuf_expand(COM_PIC *pic, int exp_l, int exp_c)
{
    picbuf_expand(pic->y, pic->stride_luma, pic->width_luma, pic->height_luma, exp_l);
    picbuf_expand(pic->u, pic->stride_chroma, pic->width_chroma, pic->height_chroma, exp_c);
    picbuf_expand(pic->v, pic->stride_chroma, pic->width_chroma, pic->height_chroma, exp_c);
}

int get_colocal_scup(int scup, int pic_width_in_scu, int pic_height_in_scu)
{
    const int mask = (-1) ^ 3;
    int bx = scup % pic_width_in_scu;
    int by = scup / pic_width_in_scu;
    int xpos = (bx & mask) + 2;
    int ypos = (by & mask) + 2;

    if (ypos >= pic_height_in_scu) {
        ypos = ((by & mask) + pic_height_in_scu) >> 1;
    }
    if (xpos >= pic_width_in_scu) {
        xpos = ((bx & mask) + pic_width_in_scu) >> 1;
    }

    return ypos * pic_width_in_scu + xpos;
}


extern int  g_table_t0[17];
void scaling_mv1(int ptr_cur, int ptr_cur_ref, int ptr_neb, int ptr_neb_ref, s32 mvp[MV_D], s32 mv[MV_D])
{
#pragma HLS ARRAY_PARTITION variable=g_table_t0 complete dim=0
#pragma HLS INLINE off
#pragma HLS allocation instances=ratio  limit=16  operation
#pragma HLS allocation instances=tmp_mv   limit=32  operation


    int ratio;
    int t0 = ptr_neb * 2 - ptr_neb_ref * 2;
    int t1 = ptr_cur * 2 - ptr_cur_ref * 2;
    
    assert(t0 != 0 && t1 != 0);
     // note: divide first for constraining bit-depth
    int diff = ptr_neb - ptr_neb_ref;
    if (diff >= 0)
        ratio = g_table_t0[diff] * t1;
    else
        ratio = -g_table_t0[-diff] * t1;
    
   
    int offset = 1 << (MV_SCALE_PREC - 1);
    s64 tmp_mv;
    tmp_mv = (s64)mvp[MV_X] * ratio;
    tmp_mv = tmp_mv == 0 ? 0 : tmp_mv > 0 ? ((tmp_mv + offset) >> MV_SCALE_PREC) : -((-tmp_mv + offset) >> MV_SCALE_PREC);
    mv[MV_X] = (s16)COM_CLIP3(COM_INT16_MIN, COM_INT16_MAX, tmp_mv);

    tmp_mv = (s64)mvp[MV_Y] * ratio;
    tmp_mv = tmp_mv == 0 ? 0 : tmp_mv > 0 ? ((tmp_mv + offset) >> MV_SCALE_PREC) : -((-tmp_mv + offset) >> MV_SCALE_PREC);
    mv[MV_Y] = (s16)COM_CLIP3(COM_INT16_MIN, COM_INT16_MAX, tmp_mv);
}

void check_mvp_motion_availability(COM_INFO *info, COM_MODE* mod_info_curr, COM_MAP *pic_map,
                                   int neb_addr[NUM_AVS2_SPATIAL_MV], int valid_flag[NUM_AVS2_SPATIAL_MV], int lidx)
{
    int scup = mod_info_curr->scup;
    int x_scu = mod_info_curr->x_scu;
    int y_scu = mod_info_curr->y_scu;
    int cu_width = mod_info_curr->cu_width;
    int cu_height = mod_info_curr->cu_height;
    int pic_width_in_scu = info->pic_width_in_scu;
    int h_scu = info->pic_height_in_scu;

    u32* map_scu = pic_map->map_scu;
    s32(*map_refi)[REFP_NUM] = pic_map->map_refi;


    int cu_width_in_scu = cu_width >> MIN_CU_LOG2;
    neb_addr[0] = scup - 1;                                     // A
    neb_addr[1] = scup - pic_width_in_scu;                      // B
    neb_addr[2] = scup - pic_width_in_scu + cu_width_in_scu;    // C
    valid_flag[0] = x_scu > 0 && MCU_GET_CODED_FLAG(map_scu[neb_addr[0]]) && !MCU_GET_INTRA_FLAG(map_scu[neb_addr[0]])
                    && REFI_IS_VALID(map_refi[neb_addr[0]][lidx]);
    valid_flag[1] = y_scu > 0 && MCU_GET_CODED_FLAG(map_scu[neb_addr[1]]) && !MCU_GET_INTRA_FLAG(map_scu[neb_addr[1]])
                    && REFI_IS_VALID(map_refi[neb_addr[1]][lidx]);
    valid_flag[2] = y_scu > 0 && x_scu + cu_width_in_scu < pic_width_in_scu && MCU_GET_CODED_FLAG(map_scu[neb_addr[2]])
                    && !MCU_GET_INTRA_FLAG(map_scu[neb_addr[2]]) && REFI_IS_VALID(map_refi[neb_addr[2]][lidx]);
    if (!valid_flag[2]) {
        neb_addr[2] = scup - pic_width_in_scu - 1;              // D
        valid_flag[2] = x_scu > 0 && y_scu > 0 && MCU_GET_CODED_FLAG(map_scu[neb_addr[2]])
                        && !MCU_GET_INTRA_FLAG(map_scu[neb_addr[2]]) && REFI_IS_VALID(map_refi[neb_addr[2]][lidx]);
    }
}

void copy_mv(s32 dst[MV_D], const s32 src[MV_D])
{
    dst[MV_X] = src[MV_X];
    dst[MV_Y] = src[MV_Y];
}

void init_motion(COM_MOTION *motion_dst)
{
    int i;
    for (i = 0; i < PRED_BI; i++) {
#pragma HLS loop_tripcount max = 2
        motion_dst->ref_idx[i] = -1;
        motion_dst->mv[i][MV_X] = 0;
        motion_dst->mv[i][MV_Y] = 0;
    }
}

void create_motion(COM_MOTION *motion_dst, s32 mv_new[REFP_NUM][MV_D], s32 refi_new[REFP_NUM])
{
    int i;
    for (i = 0; i < PRED_BI; i++) {
#pragma HLS loop_tripcount max = 2
        copy_mv(motion_dst->mv[i], mv_new[i]);
        motion_dst->ref_idx[i] = refi_new[i];
    }
}

void copy_motion(COM_MOTION *motion_dst, COM_MOTION motion_src)
{
    int i;
    for (i = 0; i < PRED_BI; i++) {
#pragma HLS loop_tripcount max=2
        copy_mv(motion_dst->mv[i], motion_src.mv[i]);
        motion_dst->ref_idx[i] = motion_src.ref_idx[i];
    }
}

int same_motion(COM_MOTION motion1, COM_MOTION motion2)
{
    if ( motion1.ref_idx[PRED_L0] != motion2.ref_idx[PRED_L0] ) {
        return 0;
    }
    if ( REFI_IS_VALID( motion1.ref_idx[PRED_L0] ) && !SAME_MV( motion1.mv[PRED_L0], motion2.mv[PRED_L0] ) ) {
        return 0;
    }

    if ( motion1.ref_idx[PRED_L1] != motion2.ref_idx[PRED_L1] ) {
        return 0;
    }
    if ( REFI_IS_VALID( motion1.ref_idx[PRED_L1] ) && !SAME_MV( motion1.mv[PRED_L1], motion2.mv[PRED_L1] ) ) {
        return 0;
    }
    return 1;
}

void create_motion_refp_0(COM_MOTION *motion_dst, s32 mv_new[MV_D], s32 refi_new)
{
    copy_mv(motion_dst->mv[REFP_0], mv_new);
    motion_dst->ref_idx[REFP_0] = refi_new;
    motion_dst->ref_idx[REFP_1] = REFI_INVALID;
}
//void check_umve_motion_availability(NEB_CU neb[NUM_SKIP_SPATIAL_MV], int valid_flag[NUM_AVS2_SPATIAL_MV])
//{
//
//    COM_MOTION m0, m1, m2;
//    //F
//    valid_flag[0] = neb[0].valid_flag;
//    //G
//    if (neb[0].valid_flag && neb[1].valid_flag) {
//        create_motion_refp_0(&m0, neb[1].map.map_mv, neb[1].map.map_refi);
//        create_motion_refp_0(&m1, neb[0].map.map_mv, neb[0].map.map_refi);
//        valid_flag[1] = !same_motion( m0, m1 );
//    } else if (!neb[0].valid_flag && neb[1].valid_flag) {
//        valid_flag[1] = 1;
//    }
//    //C
//    if (neb[1].valid_flag && neb[2].valid_flag) {
//        create_motion_refp_0(&m0, neb[2].map.map_mv, neb[2].map.map_refi);
//        create_motion_refp_0(&m1, neb[1].map.map_mv, neb[1].map.map_refi);
//        valid_flag[2] = !same_motion( m0, m1 );
//    } else if (!neb[1].valid_flag && neb[2].valid_flag) {
//        valid_flag[2] = 1;
//    }
//    //A
//    if (neb[0].valid_flag && neb[3].valid_flag) {
//        create_motion_refp_0(&m0, neb[3].map.map_mv, neb[3].map.map_refi);
//        create_motion_refp_0(&m1, neb[0].map.map_mv, neb[0].map.map_refi);
//        valid_flag[3] = !same_motion( m0, m1 );
//    } else if (!neb[0].valid_flag && neb[3].valid_flag) {
//        valid_flag[3] = 1;
//    }
//    //D
//    if (neb[5].valid_flag) {
//        create_motion_refp_0(&m0, neb[5].map.map_mv, neb[5].map.map_refi);
//        if (neb[3].valid_flag) {
//            create_motion_refp_0(&m1, neb[3].map.map_mv, neb[3].map.map_refi);
//        } else {
//            init_motion( &m1 );
//        }
//        if (neb[1].valid_flag) {
//            create_motion_refp_0(&m2, neb[1].map.map_mv, neb[1].map.map_refi);
//        } else {
//            init_motion( &m2 );
//        }
//        valid_flag[4] = (!neb[3].valid_flag || !same_motion(m0, m1)) && (!neb[1].valid_flag || !same_motion(m0, m2));
//    }
//}

#if EXT_AMVR_HMVP
void com_get_mvp_hmvp(COM_MOTION motion, int lidx, int ptr_cur, s8 cur_refi, s32 mvp[MV_D],
                      COM_REFP refp[MAX_NUM_REF_PICS][REFP_NUM], int amvr_idx)
{
    int ptr_hmvp_ref;
    s8  refi_hmvp = motion.ref_idx[lidx];
    int ptr_cur_ref = refp[cur_refi][lidx].ptr;
    if (REFI_IS_VALID(refi_hmvp)) {
        ptr_hmvp_ref = refp[refi_hmvp][lidx].ptr;
        scaling_mv1(ptr_cur, ptr_cur_ref, ptr_cur, ptr_hmvp_ref, motion.mv[lidx], mvp);
    } else {
        int lidx1 = (lidx == PRED_L0) ? PRED_L1 : PRED_L0;
        refi_hmvp = motion.ref_idx[lidx1];
        ptr_hmvp_ref = refp[refi_hmvp][lidx1].ptr;
        scaling_mv1(ptr_cur, ptr_cur_ref, ptr_cur, ptr_hmvp_ref, motion.mv[lidx1], mvp);
    }

    // clip MVP after rounding (rounding process might result in 32768)
    int mvp_x, mvp_y;
    com_mv_rounding_s32((s32)mvp[MV_X], (s32)mvp[MV_Y], &mvp_x, &mvp_y, amvr_idx, amvr_idx);
    mvp[MV_X] = (s16)COM_CLIP3(COM_INT16_MIN, COM_INT16_MAX, mvp_x);
    mvp[MV_Y] = (s16)COM_CLIP3(COM_INT16_MIN, COM_INT16_MAX, mvp_y);
}
#endif


void com_get_mvp_default(COM_INFO *info, COM_MODE *mod_info_curr, COM_REFP refp[MAX_NUM_REF_PICS][REFP_NUM],
                         COM_MAP *pic_map, int ptr_cur, int lidx, s32 cur_refi,
                         u8 amvr_idx, s32 mvp[MV_D])
{
    int scup = mod_info_curr->scup;
    int cu_width = mod_info_curr->cu_width;
    int cu_height = mod_info_curr->cu_height;

    int pic_w = info->pic_width;
    int pic_h = info->pic_height;
    int pic_width_in_scu = info->pic_width_in_scu;
    int h_scu = info->pic_height_in_scu;
    COM_PIC_HEADER * sh = &info->pic_header;

    s32(*map_mv)[REFP_NUM][MV_D] = pic_map->map_mv;
    s32(*map_refi)[REFP_NUM] = pic_map->map_refi;
    u32* map_scu = pic_map->map_scu;

    int cnt, hv, ptr_cur_ref;
    int mvPredType = MVPRED_xy_MIN;
    int rFrameL, rFrameU, rFrameUR;
    int neb_addr[NUM_AVS2_SPATIAL_MV], valid_flag[NUM_AVS2_SPATIAL_MV];
    s32 refi[NUM_AVS2_SPATIAL_MV];
    s32 MVPs[NUM_AVS2_SPATIAL_MV][MV_D];

    check_mvp_motion_availability(info, mod_info_curr, pic_map, neb_addr, valid_flag, lidx);
    ptr_cur_ref = refp[cur_refi][lidx].ptr;
    for (cnt = 0; cnt < NUM_AVS2_SPATIAL_MV; cnt++) {
        if (valid_flag[cnt]) {
            refi[cnt] = map_refi[neb_addr[cnt]][lidx];
            assert(REFI_IS_VALID(refi[cnt]));
#if !LIBVC_BLOCKDISTANCE_BY_LIBPTR
            //if (refp[refi[cnt]][lidx].is_library_picture || refp[cur_refi][lidx].is_library_picture)
            if (0) {
                refi[cnt] = REFI_INVALID;
                MVPs[cnt][MV_X] = 0;
                MVPs[cnt][MV_Y] = 0;
            } else
#endif
            {
                int ptr_neb_ref = refp[refi[cnt]][lidx].ptr;
                scaling_mv1(ptr_cur, ptr_cur_ref, ptr_cur, ptr_neb_ref, map_mv[neb_addr[cnt]][lidx], MVPs[cnt]);
            }
        } else {
            refi[cnt] = REFI_INVALID;
            MVPs[cnt][MV_X] = 0;
            MVPs[cnt][MV_Y] = 0;
        }
    }
    rFrameL = refi[0];
    rFrameU = refi[1];
    rFrameUR = refi[2];
    if ((rFrameL != REFI_INVALID) && (rFrameU == REFI_INVALID) && (rFrameUR == REFI_INVALID)) {
        mvPredType = MVPRED_L;
    } else if ((rFrameL == REFI_INVALID) && (rFrameU != REFI_INVALID) && (rFrameUR == REFI_INVALID)) {
        mvPredType = MVPRED_U;
    } else if ((rFrameL == REFI_INVALID) && (rFrameU == REFI_INVALID) && (rFrameUR != REFI_INVALID)) {
        mvPredType = MVPRED_UR;
    }

    for (hv = 0; hv < MV_D; hv++) {
        s32 mva = (s32)MVPs[0][hv], mvb = (s32)MVPs[1][hv], mvc = (s32)MVPs[2][hv];
        switch (mvPredType) {
            case MVPRED_xy_MIN:
                if ((mva < 0 && mvb > 0 && mvc > 0) || (mva > 0 && mvb < 0 && mvc < 0)) {
                    mvp[hv] = (s16)((mvb + mvc) / 2);
                } else if ((mvb < 0 && mva > 0 && mvc > 0) || (mvb > 0 && mva < 0 && mvc < 0)) {
                    mvp[hv] = (s16)((mvc + mva) / 2);
                } else if ((mvc < 0 && mva > 0 && mvb > 0) || (mvc > 0 && mva < 0 && mvb < 0)) {
                    mvp[hv] = (s16)((mva + mvb) / 2);
                } else {
                    s32 mva_ext = abs(mva - mvb);
                    s32 mvb_ext = abs(mvb - mvc);
                    s32 mvc_ext = abs(mvc - mva);
                    s32 pred_vec = avs3_min(mva_ext, avs3_min(mvb_ext, mvc_ext));
                    if (pred_vec == mva_ext) {
                        mvp[hv] = (s16)((mva + mvb) / 2);
                    } else if (pred_vec == mvb_ext) {
                        mvp[hv] = (s16)((mvb + mvc) / 2);
                    } else {
                        mvp[hv] = (s16)((mvc + mva) / 2);
                    }
                }
                break;
            case MVPRED_L:
                mvp[hv] = (s16)mva;
                break;
            case MVPRED_U:
                mvp[hv] = (s16)mvb;
                break;
            case MVPRED_UR:
                mvp[hv] = (s16)mvc;
                break;
            default:
                assert(0);
                break;
        }
    }

    // clip MVP after rounding (rounding process might result in 32768)
    int mvp_x, mvp_y;
    com_mv_rounding_s32((s32)mvp[MV_X], (s32)mvp[MV_Y], &mvp_x, &mvp_y, amvr_idx, amvr_idx);
    mvp[MV_X] = (s16)COM_CLIP3(COM_INT16_MIN, COM_INT16_MAX, mvp_x);
    mvp[MV_Y] = (s16)COM_CLIP3(COM_INT16_MIN, COM_INT16_MAX, mvp_y);
}

void com_derive_mvp(COM_INFO info, COM_MODE *mod_info_curr, int ptr, int ref_list, int ref_idx, int cnt_hmvp_cands,
                    COM_MOTION *motion_cands, COM_MAP *map, COM_REFP refp[MAX_NUM_REF_PICS][REFP_NUM], int mvr_idx, s32 mvp[MV_D],
                    int map_scu_depth)
{
    int scup = mod_info_curr->scup;
    int emvp_flag = mod_info_curr->mvp_from_hmvp_flag;
    int cu_width = mod_info_curr->cu_width;
    int cu_height = mod_info_curr->cu_height;

#if EXT_AMVR_HMVP
    if (!emvp_flag) {
#endif
        com_get_mvp_default(&info, mod_info_curr, refp, map, ptr, ref_list, ref_idx, mvr_idx, mvp);
#if EXT_AMVR_HMVP
    } else {
        if (cnt_hmvp_cands == 0) {
            mvp[MV_X] = 0;
            mvp[MV_Y] = 0;
        } else if (cnt_hmvp_cands < mvr_idx + 1) {
            COM_MOTION motion = motion_cands[cnt_hmvp_cands - 1];
            com_get_mvp_hmvp(motion, ref_list, ptr, ref_idx, mvp, refp, mvr_idx);
        } else {
            COM_MOTION motion = motion_cands[cnt_hmvp_cands - 1 - mvr_idx];
            com_get_mvp_hmvp(motion, ref_list, ptr, ref_idx, mvp, refp, mvr_idx);
        }
    }
#endif
}


void com_get_mvp_default_re(COM_INFO *info, COM_MODE *mod_info_curr, COM_REFP refp[MAX_NUM_REF_PICS][REFP_NUM],
                            int ptr_cur, int lidx, s8 cur_refi,
                            u8 amvr_idx, s16 mvp[MV_D], s32(*chosen_mv)[REFP_NUM][MV_D], s32(*chosen_refi)[REFP_NUM],
                            int neb_addr[NUM_AVS2_SPATIAL_MV], int valid_flag[NUM_AVS2_SPATIAL_MV])
{
    int scup = mod_info_curr->scup;
    int cu_width = mod_info_curr->cu_width;
    int cu_height = mod_info_curr->cu_height;
    int pic_w = info->pic_width;
    int pic_h = info->pic_height;
    int pic_width_in_scu = info->pic_width_in_scu;
    int h_scu = info->pic_height_in_scu;
    COM_PIC_HEADER * sh = &info->pic_header;
    int cnt, hv, ptr_cur_ref;
    int mvPredType = MVPRED_xy_MIN;
    int rFrameL, rFrameU, rFrameUR;
    s32 refi[NUM_AVS2_SPATIAL_MV];
    s32 MVPs[NUM_AVS2_SPATIAL_MV][MV_D];
    ptr_cur_ref = refp[cur_refi][lidx].ptr;
    for (cnt = 0; cnt < NUM_AVS2_SPATIAL_MV; cnt++) {
        if (valid_flag[cnt]) {
            refi[cnt] = chosen_refi[cnt][lidx];
            assert(REFI_IS_VALID(refi[cnt]));
#if !LIBVC_BLOCKDISTANCE_BY_LIBPTR
            if (0) {
                refi[cnt] = REFI_INVALID;
                MVPs[cnt][MV_X] = 0;
                MVPs[cnt][MV_Y] = 0;
            } else
#endif
            {
                int ptr_neb_ref = refp[refi[cnt]][lidx].ptr;
                scaling_mv1(ptr_cur, ptr_cur_ref, ptr_cur, ptr_neb_ref, chosen_mv[cnt][lidx], MVPs[cnt]);
            }
        } else {
            refi[cnt] = REFI_INVALID;
            MVPs[cnt][MV_X] = 0;
            MVPs[cnt][MV_Y] = 0;
        }
    }
    rFrameL = refi[0];
    rFrameU = refi[1];
    rFrameUR = refi[2];
    if ((rFrameL != REFI_INVALID) && (rFrameU == REFI_INVALID) && (rFrameUR == REFI_INVALID)) {
        mvPredType = MVPRED_L;
    } else if ((rFrameL == REFI_INVALID) && (rFrameU != REFI_INVALID) && (rFrameUR == REFI_INVALID)) {
        mvPredType = MVPRED_U;
    } else if ((rFrameL == REFI_INVALID) && (rFrameU == REFI_INVALID) && (rFrameUR != REFI_INVALID)) {
        mvPredType = MVPRED_UR;
    }
    for (hv = 0; hv < MV_D; hv++) {
        s32 mva = (s32)MVPs[0][hv], mvb = (s32)MVPs[1][hv], mvc = (s32)MVPs[2][hv];
        switch (mvPredType) {
            case MVPRED_xy_MIN:
                if ((mva < 0 && mvb > 0 && mvc > 0) || (mva > 0 && mvb < 0 && mvc < 0)) {
                    mvp[hv] = (s16)((mvb + mvc) / 2);
                } else if ((mvb < 0 && mva > 0 && mvc > 0) || (mvb > 0 && mva < 0 && mvc < 0)) {
                    mvp[hv] = (s16)((mvc + mva) / 2);
                } else if ((mvc < 0 && mva > 0 && mvb > 0) || (mvc > 0 && mva < 0 && mvb < 0)) {
                    mvp[hv] = (s16)((mva + mvb) / 2);
                } else {
                    s32 mva_ext = abs(mva - mvb);
                    s32 mvb_ext = abs(mvb - mvc);
                    s32 mvc_ext = abs(mvc - mva);
                    s32 pred_vec = avs3_min(mva_ext, avs3_min(mvb_ext, mvc_ext));
                    if (pred_vec == mva_ext) {
                        mvp[hv] = (s16)((mva + mvb) / 2);
                    } else if (pred_vec == mvb_ext) {
                        mvp[hv] = (s16)((mvb + mvc) / 2);
                    } else {
                        mvp[hv] = (s16)((mvc + mva) / 2);
                    }
                }
                break;
            case MVPRED_L:
                mvp[hv] = (s16)mva;
                break;
            case MVPRED_U:
                mvp[hv] = (s16)mvb;
                break;
            case MVPRED_UR:
                mvp[hv] = (s16)mvc;
                break;
            default:
                assert(0);
                break;
        }
    }
    // clip MVP after rounding (rounding process might result in 32768)
    int mvp_x, mvp_y;
    com_mv_rounding_s32((s32)mvp[MV_X], (s32)mvp[MV_Y], &mvp_x, &mvp_y, amvr_idx, amvr_idx);
    mvp[MV_X] = (s16)COM_CLIP3(COM_INT16_MIN, COM_INT16_MAX, mvp_x);
    mvp[MV_Y] = (s16)COM_CLIP3(COM_INT16_MIN, COM_INT16_MAX, mvp_y);
}

void print_motion(COM_MOTION motion)
{
    printf(" %d %d %d", motion.mv[PRED_L0][MV_X], motion.mv[PRED_L0][MV_Y], motion.ref_idx[PRED_L0]);
    printf(" %d %d %d", motion.mv[PRED_L1][MV_X], motion.mv[PRED_L1][MV_Y], motion.ref_idx[PRED_L1]);
    printf("\n");
}

// text 9.5.8.4.3
void get_col_mv(COM_REFP refp[REFP_NUM], int ptr_curr, int scup_co, s32 mvp[REFP_NUM][MV_D])
{
    s32 mvc[MV_D];
    assert(REFI_IS_VALID(refp[REFP_1].map_refi[scup_co][REFP_0]));

    mvc[MV_X] = refp[REFP_1].map_mv[scup_co][0][MV_X];
    mvc[MV_Y] = refp[REFP_1].map_mv[scup_co][0][MV_Y];
    int ptr_col = refp[REFP_1].ptr;
    int ptr_col_ref = refp[REFP_1].list_ptr[refp[REFP_1].map_refi[scup_co][REFP_0]];
    scaling_mv1(ptr_curr, refp[REFP_0].ptr, ptr_col, ptr_col_ref, mvc, mvp[REFP_0]);
    scaling_mv1(ptr_curr, refp[REFP_1].ptr, ptr_col, ptr_col_ref, mvc, mvp[REFP_1]);
}

//u16 com_get_avail_intra(int x_scu, int y_scu,  ENC_ME_LINE_MAP* me_line_ptr)
//{
//#pragma HLS INLINE off
//    u16 avail = 0;
//    int pic_x_scu = x_scu - me_line_ptr->min_scu_x;
//    int pic_y_scu = y_scu - me_line_ptr->min_scu_y;
//    if (x_scu == me_line_ptr->min_scu_x) {
//        if (x_scu > 0 && MCU_GET_CODED_FLAG(me_line_ptr->map_scu_inner[1 - me_line_ptr->curr_ctu_idx][15][pic_y_scu])) {
//            SET_AVAIL(avail, AVAIL_LE);
//        }
//    } else {
//        if (x_scu > 0 && MCU_GET_CODED_FLAG(me_line_ptr->map_scu_inner[me_line_ptr->curr_ctu_idx][pic_x_scu - 1][pic_y_scu])) {
//            SET_AVAIL(avail, AVAIL_LE);
//        }
//    }
//    if (y_scu > 0) {
//        if (y_scu == me_line_ptr->min_scu_y) {
//			if (MCU_GET_CODED_FLAG(me_line_ptr->new_map_scu_above_line[x_scu - me_line_ptr->min_scu_x + SCUX_OFFSET])) {
//                SET_AVAIL(avail, AVAIL_UP);
//            }
//        } else {
//            if (MCU_GET_CODED_FLAG(me_line_ptr->map_scu_inner[me_line_ptr->curr_ctu_idx][pic_x_scu][pic_y_scu - 1])) {
//                SET_AVAIL(avail, AVAIL_UP);
//            }
//        }
//    }
//    if (y_scu == me_line_ptr->min_scu_y) {
//		if (x_scu > 0 && MCU_GET_CODED_FLAG(me_line_ptr->new_map_scu_above_line[x_scu - me_line_ptr->min_scu_x - 1 + SCUX_OFFSET])) {
//            SET_AVAIL(avail, AVAIL_UP_LE);
//        }
//    } else if (x_scu == me_line_ptr->min_scu_x) {
//        if (x_scu > 0 && MCU_GET_CODED_FLAG(me_line_ptr->map_scu_inner[1 - me_line_ptr->curr_ctu_idx][15][pic_y_scu - 1])) {
//            SET_AVAIL(avail, AVAIL_UP_LE);
//        }
//    } else {
//        if (x_scu > 0
//                && MCU_GET_CODED_FLAG(me_line_ptr->map_scu_inner[me_line_ptr->curr_ctu_idx][pic_x_scu - 1][pic_y_scu - 1])) {
//            SET_AVAIL(avail, AVAIL_UP_LE);
//        }
//    }
//    return avail;
//}
#if IP_map_scu
//u16 com_get_avail_intra_ip(int x_scu, int y_scu)
//{
//#pragma HLS INLINE off
//    u16 avail = 0;
//    int pic_x_scu = x_scu - MAP_SCU_IP_ptr->min_scu_x;
//    int pic_y_scu = y_scu - MAP_SCU_IP_ptr->min_scu_y;
//    if (x_scu == MAP_SCU_IP_ptr->min_scu_x) {
//        //assert(MAP_SCU_IP_ptr->map_scu_inner[1 - MAP_SCU_IP_ptr->curr_ctu_idx][15][pic_y_scu] == me_line_ptr->map_scu_inner[1 - me_line_ptr->curr_ctu_idx][15][pic_y_scu]);
//        if (x_scu > 0 && MCU_GET_CODED_FLAG(MAP_SCU_IP_ptr->map_scu_inner[1 - MAP_SCU_IP_ptr->curr_ctu_idx][15][pic_y_scu])) {
//            SET_AVAIL(avail, AVAIL_LE);
//        }
//    } else {
//        //assert(MAP_SCU_IP_ptr->map_scu_inner[1 - MAP_SCU_IP_ptr->curr_ctu_idx][pic_x_scu - 1][pic_y_scu] == me_line_ptr->map_scu_inner[1 - me_line_ptr->curr_ctu_idx][pic_x_scu - 1][pic_y_scu]);
//
//        if (x_scu > 0
//                && MCU_GET_CODED_FLAG(MAP_SCU_IP_ptr->map_scu_inner[MAP_SCU_IP_ptr->curr_ctu_idx][pic_x_scu - 1][pic_y_scu])) {
//            SET_AVAIL(avail, AVAIL_LE);
//        }
//    }
//    if (y_scu > 0) {
//        //assert(MAP_SCU_IP_ptr->map_scu_above_line[MAP_SCU_IP_ptr->above_line_idx][x_scu] == me_line_ptr->map_scu_above_line[me_line_ptr->above_line_idx][x_scu]);
//        if (y_scu == MAP_SCU_IP_ptr->min_scu_y) {
//            if (MCU_GET_CODED_FLAG(MAP_SCU_IP_ptr->map_scu_above_line[MAP_SCU_IP_ptr->above_line_idx][x_scu])) {
//                SET_AVAIL(avail, AVAIL_UP);
//            }
//        } else {
//            //assert(MAP_SCU_IP_ptr->map_scu_inner[1 - MAP_SCU_IP_ptr->curr_ctu_idx][pic_x_scu][pic_y_scu - 1] == me_line_ptr->map_scu_inner[1 - me_line_ptr->curr_ctu_idx][pic_x_scu][pic_y_scu - 1]);
//
//            if (MCU_GET_CODED_FLAG(MAP_SCU_IP_ptr->map_scu_inner[MAP_SCU_IP_ptr->curr_ctu_idx][pic_x_scu][pic_y_scu - 1])) {
//                SET_AVAIL(avail, AVAIL_UP);
//            }
//        }
//    }
//    if (y_scu == MAP_SCU_IP_ptr->min_scu_y) {
//        if (x_scu > 0)
//            //assert(MAP_SCU_IP_ptr->map_scu_above_line[MAP_SCU_IP_ptr->above_line_idx][x_scu - 1] == me_line_ptr->map_scu_above_line[me_line_ptr->above_line_idx][x_scu - 1]);
//
//            if (x_scu > 0 && MCU_GET_CODED_FLAG(MAP_SCU_IP_ptr->map_scu_above_line[MAP_SCU_IP_ptr->above_line_idx][x_scu - 1])) {
//                SET_AVAIL(avail, AVAIL_UP_LE);
//            }
//    } else if (x_scu == MAP_SCU_IP_ptr->min_scu_x) {
//        //assert(MAP_SCU_IP_ptr->map_scu_inner[1 - MAP_SCU_IP_ptr->curr_ctu_idx][15][pic_y_scu - 1] == me_line_ptr->map_scu_inner[1 - me_line_ptr->curr_ctu_idx][15][pic_y_scu - 1]);
//
//        if (x_scu > 0
//                && MCU_GET_CODED_FLAG(MAP_SCU_IP_ptr->map_scu_inner[1 - MAP_SCU_IP_ptr->curr_ctu_idx][15][pic_y_scu - 1])) {
//            SET_AVAIL(avail, AVAIL_UP_LE);
//        }
//    } else {
//        //assert(MAP_SCU_IP_ptr->map_scu_inner[1 - MAP_SCU_IP_ptr->curr_ctu_idx][pic_x_scu - 1][pic_y_scu - 1] == me_line_ptr->map_scu_inner[1 - me_line_ptr->curr_ctu_idx][pic_x_scu - 1][pic_y_scu - 1]);
//
//        if (x_scu > 0
//                && MCU_GET_CODED_FLAG(MAP_SCU_IP_ptr->map_scu_inner[MAP_SCU_IP_ptr->curr_ctu_idx][pic_x_scu - 1][pic_y_scu - 1])) {
//            SET_AVAIL(avail, AVAIL_UP_LE);
//        }
//    }
//    return avail;
//}
#endif
int com_picbuf_signature(COM_PIC *pic, u8 *signature)
{
    return com_md5_imgb(pic->imgb, signature);
}

/* MD5 functions */
#define MD5FUNC(f, w, x, y, z, msg1, s,msg2 ) \
     ( w += f(x, y, z) + msg1 + msg2,  w = w<<s | w>>(32-s),  w += x )
#define FF(x, y, z) (z ^ (x & (y ^ z)))
#define GG(x, y, z) (y ^ (z & (x ^ y)))
#define HH(x, y, z) (x ^ y ^ z)
#define II(x, y, z) (y ^ (x | ~z))

static void com_md5_trans(u32 *buf, u32 *msg)
{
    register u32 a, b, c, d;
    a = buf[0];
    b = buf[1];
    c = buf[2];
    d = buf[3];
    MD5FUNC(FF, a, b, c, d, msg[ 0],  7, 0xd76aa478); /* 1 */
    MD5FUNC(FF, d, a, b, c, msg[ 1], 12, 0xe8c7b756); /* 2 */
    MD5FUNC(FF, c, d, a, b, msg[ 2], 17, 0x242070db); /* 3 */
    MD5FUNC(FF, b, c, d, a, msg[ 3], 22, 0xc1bdceee); /* 4 */
    MD5FUNC(FF, a, b, c, d, msg[ 4],  7, 0xf57c0faf); /* 5 */
    MD5FUNC(FF, d, a, b, c, msg[ 5], 12, 0x4787c62a); /* 6 */
    MD5FUNC(FF, c, d, a, b, msg[ 6], 17, 0xa8304613); /* 7 */
    MD5FUNC(FF, b, c, d, a, msg[ 7], 22, 0xfd469501); /* 8 */
    MD5FUNC(FF, a, b, c, d, msg[ 8],  7, 0x698098d8); /* 9 */
    MD5FUNC(FF, d, a, b, c, msg[ 9], 12, 0x8b44f7af); /* 10 */
    MD5FUNC(FF, c, d, a, b, msg[10], 17, 0xffff5bb1); /* 11 */
    MD5FUNC(FF, b, c, d, a, msg[11], 22, 0x895cd7be); /* 12 */
    MD5FUNC(FF, a, b, c, d, msg[12],  7, 0x6b901122); /* 13 */
    MD5FUNC(FF, d, a, b, c, msg[13], 12, 0xfd987193); /* 14 */
    MD5FUNC(FF, c, d, a, b, msg[14], 17, 0xa679438e); /* 15 */
    MD5FUNC(FF, b, c, d, a, msg[15], 22, 0x49b40821); /* 16 */
    /* Round 2 */
    MD5FUNC(GG, a, b, c, d, msg[ 1],  5, 0xf61e2562); /* 17 */
    MD5FUNC(GG, d, a, b, c, msg[ 6],  9, 0xc040b340); /* 18 */
    MD5FUNC(GG, c, d, a, b, msg[11], 14, 0x265e5a51); /* 19 */
    MD5FUNC(GG, b, c, d, a, msg[ 0], 20, 0xe9b6c7aa); /* 20 */
    MD5FUNC(GG, a, b, c, d, msg[ 5],  5, 0xd62f105d); /* 21 */
    MD5FUNC(GG, d, a, b, c, msg[10],  9,  0x2441453); /* 22 */
    MD5FUNC(GG, c, d, a, b, msg[15], 14, 0xd8a1e681); /* 23 */
    MD5FUNC(GG, b, c, d, a, msg[ 4], 20, 0xe7d3fbc8); /* 24 */
    MD5FUNC(GG, a, b, c, d, msg[ 9],  5, 0x21e1cde6); /* 25 */
    MD5FUNC(GG, d, a, b, c, msg[14],  9, 0xc33707d6); /* 26 */
    MD5FUNC(GG, c, d, a, b, msg[ 3], 14, 0xf4d50d87); /* 27 */
    MD5FUNC(GG, b, c, d, a, msg[ 8], 20, 0x455a14ed); /* 28 */
    MD5FUNC(GG, a, b, c, d, msg[13],  5, 0xa9e3e905); /* 29 */
    MD5FUNC(GG, d, a, b, c, msg[ 2],  9, 0xfcefa3f8); /* 30 */
    MD5FUNC(GG, c, d, a, b, msg[ 7], 14, 0x676f02d9); /* 31 */
    MD5FUNC(GG, b, c, d, a, msg[12], 20, 0x8d2a4c8a); /* 32 */
    /* Round 3 */
    MD5FUNC(HH, a, b, c, d, msg[ 5],  4, 0xfffa3942); /* 33 */
    MD5FUNC(HH, d, a, b, c, msg[ 8], 11, 0x8771f681); /* 34 */
    MD5FUNC(HH, c, d, a, b, msg[11], 16, 0x6d9d6122); /* 35 */
    MD5FUNC(HH, b, c, d, a, msg[14], 23, 0xfde5380c); /* 36 */
    MD5FUNC(HH, a, b, c, d, msg[ 1],  4, 0xa4beea44); /* 37 */
    MD5FUNC(HH, d, a, b, c, msg[ 4], 11, 0x4bdecfa9); /* 38 */
    MD5FUNC(HH, c, d, a, b, msg[ 7], 16, 0xf6bb4b60); /* 39 */
    MD5FUNC(HH, b, c, d, a, msg[10], 23, 0xbebfbc70); /* 40 */
    MD5FUNC(HH, a, b, c, d, msg[13],  4, 0x289b7ec6); /* 41 */
    MD5FUNC(HH, d, a, b, c, msg[ 0], 11, 0xeaa127fa); /* 42 */
    MD5FUNC(HH, c, d, a, b, msg[ 3], 16, 0xd4ef3085); /* 43 */
    MD5FUNC(HH, b, c, d, a, msg[ 6], 23,  0x4881d05); /* 44 */
    MD5FUNC(HH, a, b, c, d, msg[ 9],  4, 0xd9d4d039); /* 45 */
    MD5FUNC(HH, d, a, b, c, msg[12], 11, 0xe6db99e5); /* 46 */
    MD5FUNC(HH, c, d, a, b, msg[15], 16, 0x1fa27cf8); /* 47 */
    MD5FUNC(HH, b, c, d, a, msg[ 2], 23, 0xc4ac5665); /* 48 */
    /* Round 4 */
    MD5FUNC(II, a, b, c, d, msg[ 0],  6, 0xf4292244); /* 49 */
    MD5FUNC(II, d, a, b, c, msg[ 7], 10, 0x432aff97); /* 50 */
    MD5FUNC(II, c, d, a, b, msg[14], 15, 0xab9423a7); /* 51 */
    MD5FUNC(II, b, c, d, a, msg[ 5], 21, 0xfc93a039); /* 52 */
    MD5FUNC(II, a, b, c, d, msg[12],  6, 0x655b59c3); /* 53 */
    MD5FUNC(II, d, a, b, c, msg[ 3], 10, 0x8f0ccc92); /* 54 */
    MD5FUNC(II, c, d, a, b, msg[10], 15, 0xffeff47d); /* 55 */
    MD5FUNC(II, b, c, d, a, msg[ 1], 21, 0x85845dd1); /* 56 */
    MD5FUNC(II, a, b, c, d, msg[ 8],  6, 0x6fa87e4f); /* 57 */
    MD5FUNC(II, d, a, b, c, msg[15], 10, 0xfe2ce6e0); /* 58 */
    MD5FUNC(II, c, d, a, b, msg[ 6], 15, 0xa3014314); /* 59 */
    MD5FUNC(II, b, c, d, a, msg[13], 21, 0x4e0811a1); /* 60 */
    MD5FUNC(II, a, b, c, d, msg[ 4],  6, 0xf7537e82); /* 61 */
    MD5FUNC(II, d, a, b, c, msg[11], 10, 0xbd3af235); /* 62 */
    MD5FUNC(II, c, d, a, b, msg[ 2], 15, 0x2ad7d2bb); /* 63 */
    MD5FUNC(II, b, c, d, a, msg[ 9], 21, 0xeb86d391); /* 64 */
    buf[0] += a;
    buf[1] += b;
    buf[2] += c;
    buf[3] += d;
}

void com_md5_init(COM_MD5 *md5)
{
    md5->h[0] = 0x67452301;
    md5->h[1] = 0xefcdab89;
    md5->h[2] = 0x98badcfe;
    md5->h[3] = 0x10325476;
    md5->bits[0] = 0;
    md5->bits[1] = 0;
}

void com_md5_update(COM_MD5 *md5, void *buf_t, u32 len)
{
    u8 *buf;
    u32 i, idx, part_len;
    buf = (u8*)buf_t;
    idx = (u32)((md5->bits[0] >> 3) & 0x3f);
    md5->bits[0] += (len << 3);
    if (md5->bits[0] < (len << 3)) {
        (md5->bits[1])++;
    }
    md5->bits[1] += (len >> 29);
    part_len = 64 - idx;
    if (len >= part_len) {
        com_mcpy(md5->msg + idx, buf, part_len);
        com_md5_trans(md5->h, (u32 *)md5->msg);
        for (i = part_len; i + 63 < len; i += 64) {
            com_md5_trans(md5->h, (u32 *)(buf + i));
        }
        idx = 0;
    } else {
        i = 0;
    }
    if (len - i > 0) {
        com_mcpy(md5->msg + idx, buf + i, len - i);
    }
}

void com_md5_update_16(COM_MD5 *md5, void *buf_t, u32 len)
{
    u16 *buf;
    u32 i, idx, part_len, j;
    u8 t[512];
    buf = (u16 *)buf_t;
    idx = (u32)((md5->bits[0] >> 3) & 0x3f);
    len = len * 2;
    for (j = 0; j < len; j += 2) {
        t[j] = (u8)(*(buf));
        t[j + 1] = *(buf) >> 8;
        buf++;
    }
    md5->bits[0] += (len << 3);
    if (md5->bits[0] < (len << 3)) {
        (md5->bits[1])++;
    }
    md5->bits[1] += (len >> 29);
    part_len = 64 - idx;
    if (len >= part_len) {
        com_mcpy(md5->msg + idx, t, part_len);
        com_md5_trans(md5->h, (u32 *)md5->msg);
        for (i = part_len; i + 63 < len; i += 64) {
            com_md5_trans(md5->h, (u32 *)(t + i));
        }
        idx = 0;
    } else {
        i = 0;
    }
    if (len - i > 0) {
        com_mcpy(md5->msg + idx, t + i, len - i);
    }
}

void com_md5_finish(COM_MD5 *md5, u8 digest[16])
{
    u8 *pos;
    int cnt;
    cnt = (md5->bits[0] >> 3) & 0x3F;
    pos = md5->msg + cnt;
    *pos++ = 0x80;
    cnt = 64 - 1 - cnt;
    if (cnt < 8) {
        com_mset(pos, 0, cnt);
        com_md5_trans(md5->h, (u32 *)md5->msg);
        com_mset(md5->msg, 0, 56);
    } else {
        com_mset(pos, 0, cnt - 8);
    }
    com_mcpy((md5->msg + 14 * sizeof(u32)), &md5->bits[0], sizeof(u32));
    com_mcpy((md5->msg + 15 * sizeof(u32)), &md5->bits[1], sizeof(u32));
    com_md5_trans(md5->h, (u32 *)md5->msg);
    com_mcpy(digest, md5->h, 16);
    com_mset(md5, 0, sizeof(COM_MD5));
}

int com_md5_imgb(COM_IMGB *imgb, u8 digest[16])
{
    COM_MD5 md5;
    int i, j;
    com_md5_init(&md5);

    for (i = 0; i < imgb->np; i++) {
        for (j = 0; j < imgb->height[i]; j++) {
            com_md5_update(&md5, ((u8 *)imgb->addr_plane[i]) + j * imgb->stride[i], imgb->width[i] * 2);
        }
    }

    com_md5_finish(&md5, digest);
    return COM_OK;
}

void init_scan(u16 *scan, int size_x, int size_y, int scan_type)
{
#pragma HLS INLINE off
    int x, y, l, pos, num_line;
    pos = 0;
    num_line = size_x + size_y - 1;
    if (scan_type == COEF_SCAN_ZIGZAG) {
        /* starting point */
        scan[pos] = 0;
        pos++;
        /* loop */
        for (l = 1; l < num_line; l++) {
#pragma HLS LOOP_TRIPCOUNT max=127
            if (l % 2) { /* decreasing loop */
                x = COM_MIN(l, size_x - 1);
                y = COM_MAX(0, l - (size_x - 1));
                while (x >= 0 && y < size_y) {
#pragma HLS LOOP_TRIPCOUNT max=64
                    scan[pos] = (u16)(y * size_x + x);
                    pos++;
                    x--;
                    y++;
                }
            } else { /* increasing loop */
                y = COM_MIN(l, size_y - 1);
                x = COM_MAX(0, l - (size_y - 1));
                while (y >= 0 && x < size_x) {
#pragma HLS LOOP_TRIPCOUNT max=64
                    scan[pos] = (u16)(y * size_x + x);
                    pos++;
                    x++;
                    y--;
                }
            }
        }
    }
}

int com_scan_tbl_init()
{
    int x, y, scan_type;
    int size_y, size_x;
    for (scan_type = 0; scan_type < COEF_SCAN_TYPE_NUM; scan_type++) {
        for (y = 0; y < MAX_CU_LOG2; y++) {
            size_y = 1 << (y + 1);
            for (x = 0; x < MAX_CU_LOG2; x++) {
                size_x = 1 << (x + 1);
                com_scan_tbl[scan_type][x][y] = (u16*)com_malloc_fast(size_y * size_x * sizeof(u16));
                init_scan(com_scan_tbl[scan_type][x][y], size_x, size_y, scan_type);
            }
        }
    }
    return COM_OK;
}

int com_scan_tbl_delete()
{
    int x, y, scan_type;
    for (scan_type = 0; scan_type < COEF_SCAN_TYPE_NUM; scan_type++) {
        for (y = 0; y < MAX_CU_LOG2 - 1; y++) {
            for (x = 0; x < MAX_CU_LOG2 - 1; x++) {
                if (com_scan_tbl[scan_type][x][y] != NULL) {
                    free(com_scan_tbl[scan_type][x][y]);
                }
            }
        }
    }
    return COM_OK;
}

/*******************************************/
/* Neighbor location: Graphical indication */
/*                                         */
/*           D B_____________G C           */
/*           A|               |            */
/*            |               |            */
/*            |               |            */
/*            |      cu    cuh|            */
/*            |               |            */
/*            |               |            */
/*           F|_____cuw______H|            */
/*                                         */
/*                                         */
/*******************************************/


#define COM_MAX(a,b)                   (((a) > (b)) ? (a) : (b))

/*! macro to determine minimum */
#define COM_MIN(a,b)                   (((a) < (b)) ? (a) : (b))
#define MV_X 0
#define MV_Y 1

CPMV com_get_affine_memory_access(CPMV mv[VER_NUM][MV_D], U7 cu_width, U7 cu_height)
{
#if BD_AFFINE_AMVR
	CPMV mv_tmp[4][2] = {};

    for (int i = 0; i < VER_NUM; i++) {
#pragma HLS loop_tripcount max = 4
        mv_tmp[i][MV_X] = mv[i][MV_X] >> 2;
        mv_tmp[i][MV_Y] = mv[i][MV_Y] >> 2;
    }
	CPMV max_x = COM_MAX(mv_tmp[0][MV_X], COM_MAX(CPMV(mv_tmp[1][MV_X] + cu_width), COM_MAX(mv_tmp[2][MV_X], CPMV(mv_tmp[3][MV_X] + cu_width)))) >> 2;
	CPMV min_x = COM_MIN(mv_tmp[0][MV_X], COM_MIN(CPMV(mv_tmp[1][MV_X] + cu_width), COM_MIN(mv_tmp[2][MV_X], CPMV(mv_tmp[3][MV_X] + cu_width)))) >> 2;
	CPMV max_y = COM_MAX(mv_tmp[0][MV_Y], COM_MAX(mv_tmp[1][MV_Y], COM_MAX(CPMV(mv_tmp[2][MV_Y] + cu_height), CPMV(mv_tmp[3][MV_Y] + cu_height)))) >> 2;
	CPMV min_y = COM_MIN(mv_tmp[0][MV_Y], COM_MIN(mv_tmp[1][MV_Y], COM_MIN(CPMV(mv_tmp[2][MV_Y] + cu_height), CPMV(mv_tmp[3][MV_Y] + cu_height)))) >> 2;
#else
    int max_x = avs3_max(mv[0][MV_X], avs3_max(mv[1][MV_X] + cu_width, avs3_max(mv[2][MV_X], mv[3][MV_X] + cu_width))) >> 2;
    int min_x = avs3_min(mv[0][MV_X], avs3_min(mv[1][MV_X] + cu_width, avs3_min(mv[2][MV_X], mv[3][MV_X] + cu_width))) >> 2;
    int max_y = avs3_max(mv[0][MV_Y], avs3_max(mv[1][MV_Y], avs3_max(mv[2][MV_Y] + cu_height, mv[3][MV_Y] + cu_height))) >> 2;
    int min_y = avs3_min(mv[0][MV_Y], avs3_min(mv[1][MV_Y], avs3_min(mv[2][MV_Y] + cu_height, mv[3][MV_Y] + cu_height))) >> 2;
#endif
    return (abs(max_x - min_x) + 7) *  (abs(max_y - min_y) + 7);
}

//BOOL com_derive_affine_model_mv(int cur_x_scu_of_ctu, int cur_y_scu_of_ctu, int neb_x_scu_of_ctu, int neb_y_scu_of_ctu,
//                                SKIP_MERGE_MAP_SCU map_aff_left_ctu[MAX_CU_IN_SCU][MAX_CU_IN_SCU], SKIP_MERGE_MAP_SCU map_top_line[MAX_CU_IN_SCU + 2],
//                                SKIP_MERGE_MAP_SCU map_aff_left_top_line[MAX_CU_IN_SCU], SKIP_MERGE_MAP_SCU map_aff_right_top_line[MAX_CU_IN_SCU],
//                                int cu_width, int cu_height, int pic_width_in_scu, CPMV mvp[VER_NUM][MV_D], int * vertex_num,
//                                ENC_ME_LINE_MAP* me_line_map_ptr)
//{
//    s16 neb_mv[VER_NUM][MV_D] = {{0, }, };
//    int i;
//
//    BOOL is_top_ctu_boundary = neb_y_scu_of_ctu < 0 ? TRUE : FALSE;
//    BOOL is_left_ctu_boundary = neb_x_scu_of_ctu < 0 ? TRUE : FALSE;
//    BOOL is_right_ctu_boundary = neb_x_scu_of_ctu >= MAX_CU_IN_SCU ? TRUE : FALSE;
//    int neb_log_w, neb_log_h;
//    if (is_top_ctu_boundary) {
//        if (is_left_ctu_boundary) {
//            neb_log_w = MCU_GET_LOGW(map_aff_left_top_line[neb_x_scu_of_ctu + MAX_CU_IN_SCU].map_cu_mode);
//            neb_log_h = MCU_GET_LOGH(map_aff_left_top_line[neb_x_scu_of_ctu + MAX_CU_IN_SCU].map_cu_mode);
//        } else if (is_right_ctu_boundary) {
//            neb_log_w = MCU_GET_LOGW(map_aff_right_top_line[neb_x_scu_of_ctu - MAX_CU_IN_SCU].map_cu_mode);
//            neb_log_h = MCU_GET_LOGH(map_aff_right_top_line[neb_x_scu_of_ctu - MAX_CU_IN_SCU].map_cu_mode);
//        } else {
//            neb_log_w = MCU_GET_LOGW(map_top_line[neb_x_scu_of_ctu + 1].map_cu_mode);
//            neb_log_h = MCU_GET_LOGH(map_top_line[neb_x_scu_of_ctu + 1].map_cu_mode);
//        }
//    } else {
//        if (is_left_ctu_boundary) {
//            neb_log_w = MCU_GET_LOGW(map_aff_left_ctu[neb_y_scu_of_ctu][neb_x_scu_of_ctu + MAX_CU_IN_SCU].map_cu_mode);
//            neb_log_h = MCU_GET_LOGH(map_aff_left_ctu[neb_y_scu_of_ctu][neb_x_scu_of_ctu + MAX_CU_IN_SCU].map_cu_mode);
//        } else {
//            neb_log_w = MCU_GET_LOGW(
//                            me_line_map_ptr->map_cu_mode_inner[me_line_map_ptr->curr_ctu_idx][neb_x_scu_of_ctu][neb_y_scu_of_ctu]);
//            neb_log_h = MCU_GET_LOGH(
//                            me_line_map_ptr->map_cu_mode_inner[me_line_map_ptr->curr_ctu_idx][neb_x_scu_of_ctu][neb_y_scu_of_ctu]);
//        }
//    }
//    int neb_w = 1 << neb_log_w;
//    int neb_h = 1 << neb_log_h;
//
//    int x_scu_of_ctu[VER_NUM], y_scu_of_ctu[VER_NUM];
//    x_scu_of_ctu[0] = neb_x_scu_of_ctu;
//    x_scu_of_ctu[1] = neb_x_scu_of_ctu + (neb_w >> MIN_CU_LOG2) - 1;
//    x_scu_of_ctu[2] = neb_x_scu_of_ctu;
//    x_scu_of_ctu[3] = neb_x_scu_of_ctu + (neb_w >> MIN_CU_LOG2) - 1;
//    y_scu_of_ctu[0] = is_top_ctu_boundary ? neb_y_scu_of_ctu + (neb_h >> MIN_CU_LOG2) - 1 : neb_y_scu_of_ctu;
//    y_scu_of_ctu[1] = is_top_ctu_boundary ? neb_y_scu_of_ctu + (neb_h >> MIN_CU_LOG2) - 1 : neb_y_scu_of_ctu;
//    y_scu_of_ctu[2] = neb_y_scu_of_ctu + (neb_h >> MIN_CU_LOG2) - 1;
//    y_scu_of_ctu[3] = neb_y_scu_of_ctu + (neb_h >> MIN_CU_LOG2) - 1;
//
//    for (i = 0; i < VER_NUM; i++) {
//#pragma HLS loop_tripcount max = 4
//        if (is_top_ctu_boundary) {
//            if (is_left_ctu_boundary) {
//                neb_mv[i][MV_X] = map_aff_left_top_line[x_scu_of_ctu[i] + MAX_CU_IN_SCU].map_mv[MV_X];
//                neb_mv[i][MV_Y] = map_aff_left_top_line[x_scu_of_ctu[i] + MAX_CU_IN_SCU].map_mv[MV_Y];
//            } else if (is_right_ctu_boundary) {
//                neb_mv[i][MV_X] = map_aff_right_top_line[x_scu_of_ctu[i] - MAX_CU_IN_SCU].map_mv[MV_X];
//                neb_mv[i][MV_Y] = map_aff_right_top_line[x_scu_of_ctu[i] - MAX_CU_IN_SCU].map_mv[MV_Y];
//            } else {
//                neb_mv[i][MV_X] = map_top_line[x_scu_of_ctu[i] + 1].map_mv[MV_X];
//                neb_mv[i][MV_Y] = map_top_line[x_scu_of_ctu[i] + 1].map_mv[MV_Y];
//            }
//        } else {
//            if (is_left_ctu_boundary) {
//                neb_mv[i][MV_X] = map_aff_left_ctu[y_scu_of_ctu[i]][x_scu_of_ctu[i] + MAX_CU_IN_SCU].map_mv[MV_X];
//                neb_mv[i][MV_Y] = map_aff_left_ctu[y_scu_of_ctu[i]][x_scu_of_ctu[i] + MAX_CU_IN_SCU].map_mv[MV_Y];
//            } else {
//                neb_mv[i][MV_X] =
//                    me_line_map_ptr->inner_nebs[me_line_map_ptr->curr_ctu_idx][x_scu_of_ctu[i]][y_scu_of_ctu[i]].mv[REFP_0][MV_X];
//                neb_mv[i][MV_Y] =
//                    me_line_map_ptr->inner_nebs[me_line_map_ptr->curr_ctu_idx][x_scu_of_ctu[i]][y_scu_of_ctu[i]].mv[REFP_0][MV_Y];
//            }
//        }
//    }
//    int cur_x = cur_x_scu_of_ctu << MIN_CU_LOG2;
//    int cur_y = cur_y_scu_of_ctu << MIN_CU_LOG2;
//    int neb_x = neb_x_scu_of_ctu << MIN_CU_LOG2;
//    int neb_y = is_top_ctu_boundary ? cur_y : neb_y_scu_of_ctu << MIN_CU_LOG2;
//    int diff_w = 7 - neb_log_w;
//    int diff_h = 7 - neb_log_h;
//    s32 dmv_hor_x, dmv_hor_y, dmv_ver_x, dmv_ver_y, hor_base, ver_base;
//    s32 tmp_hor, tmp_ver;
//
//    dmv_hor_x = (s32)(neb_mv[1][MV_X] - neb_mv[0][MV_X]) << diff_w;      // deltaMvHor
//    dmv_hor_y = (s32)(neb_mv[1][MV_Y] - neb_mv[0][MV_Y]) << diff_w;
//    if (*vertex_num == 3 && !is_top_ctu_boundary) {
//        dmv_ver_x = (s32)(neb_mv[2][MV_X] - neb_mv[0][MV_X]) << diff_h;  // deltaMvVer
//        dmv_ver_y = (s32)(neb_mv[2][MV_Y] - neb_mv[0][MV_Y]) << diff_h;
//    } else {
//        dmv_ver_x = -dmv_hor_y;                                          // deltaMvVer
//        dmv_ver_y = dmv_hor_x;
//        *vertex_num = 2;
//    }
//    hor_base = (s32)neb_mv[0][MV_X] << 7;
//    ver_base = (s32)neb_mv[0][MV_Y] << 7;
//
//    // derive CPMV 0
//    tmp_hor = dmv_hor_x * (cur_x - neb_x) + dmv_ver_x * (cur_y - neb_y) + hor_base;
//    tmp_ver = dmv_hor_y * (cur_x - neb_x) + dmv_ver_y * (cur_y - neb_y) + ver_base;
//    com_mv_rounding_s32(tmp_hor, tmp_ver, &tmp_hor, &tmp_ver, 7, 0);
//#if BD_AFFINE_AMVR
//    tmp_hor <<= 2;
//    tmp_ver <<= 2;
//#endif
//    mvp[0][MV_X] = (CPMV)COM_CLIP3(COM_CPMV_MIN, COM_CPMV_MAX, tmp_hor);
//    mvp[0][MV_Y] = (CPMV)COM_CLIP3(COM_CPMV_MIN, COM_CPMV_MAX, tmp_ver);
//
//    // derive CPMV 1
//    tmp_hor = dmv_hor_x * (cur_x - neb_x + cu_width) + dmv_ver_x * (cur_y - neb_y) + hor_base;
//    tmp_ver = dmv_hor_y * (cur_x - neb_x + cu_width) + dmv_ver_y * (cur_y - neb_y) + ver_base;
//    com_mv_rounding_s32(tmp_hor, tmp_ver, &tmp_hor, &tmp_ver, 7, 0);
//#if BD_AFFINE_AMVR
//    tmp_hor <<= 2;
//    tmp_ver <<= 2;
//#endif
//    mvp[1][MV_X] = (CPMV)COM_CLIP3(COM_CPMV_MIN, COM_CPMV_MAX, tmp_hor);
//    mvp[1][MV_Y] = (CPMV)COM_CLIP3(COM_CPMV_MIN, COM_CPMV_MAX, tmp_ver);
//
//    // derive CPMV 2
//    if (*vertex_num == 3) {
//        tmp_hor = dmv_hor_x * (cur_x - neb_x) + dmv_ver_x * (cur_y - neb_y + cu_height) + hor_base;
//        tmp_ver = dmv_hor_y * (cur_x - neb_x) + dmv_ver_y * (cur_y - neb_y + cu_height) + ver_base;
//        com_mv_rounding_s32(tmp_hor, tmp_ver, &tmp_hor, &tmp_ver, 7, 0);
//#if BD_AFFINE_AMVR
//        tmp_hor <<= 2;
//        tmp_ver <<= 2;
//#endif
//        mvp[2][MV_X] = (CPMV)COM_CLIP3(COM_CPMV_MIN, COM_CPMV_MAX, tmp_hor);
//        mvp[2][MV_Y] = (CPMV)COM_CLIP3(COM_CPMV_MIN, COM_CPMV_MAX, tmp_ver);
//    }
//
//    for (i = 0; i < *vertex_num; ++i) {
//#pragma HLS loop_tripcount max = 3
//        if ((abs(mvp[i][MV_X]) >> 4) > SRW) {
//            return FALSE;
//        }
//        if ((abs(mvp[i][MV_Y]) >> 4) > SRH) {
//            return FALSE;
//        }
//    }
//    return TRUE;
//}


/*************************************************
Copyright:
Author: zhuxz
Date:2020-05-26
Description:	仿射运动估计运动矢量预测和缩放
**************************************************/

/*************************************************
Function:		com_affine_mc
Description:	仿射运动估计运动矢量预测和缩放
Calls:			com_affine_mc_lc_sim
Input:			COM_INFO *info 当前编码器部分信息
				COM_MODE *mod_info_curr 当前的模式决策信息
				COM_REFP(*refp)[REFP_NUM] 参考帧列表
				COM_MAP *pic_map pic信息映射
				int vertex_num ControlPoint数量
				int ptr 当前帧编号
				int lidx 参考列表号
				s16 mvp[MV_D] 预测的运动矢量
				u8 curr_mvr 当前mvr
				int map_scu_depth scu映射信息depth
Output:         pmv
Return:        	void
Others:         None
*************************************************/

/* inter affine mode */



int check_mv_valid(s16 mvp_scu[MV_D], s16 map_line[MV_D])
{
    if (mvp_scu[MV_X] != map_line[MV_X] || mvp_scu[MV_Y] != map_line[MV_Y]) {
        return 0;
    }
    return 1;
}

int cnt = 0;



/*zxz*/
void check_affine_mvp_availability(COM_INFO *info, COM_MODE * mod_info_curr, COM_REFP refp[MAX_NUM_REF_PICS][REFP_NUM],
                                   COM_MAP *pic_map, int neb_addr_lt[AFFINE_MAX_NUM_LT], int valid_flag_lt[AFFINE_MAX_NUM_LT],
                                   int neb_addr_rt[AFFINE_MAX_NUM_RT], int valid_flag_rt[AFFINE_MAX_NUM_RT])
{
    int scup = mod_info_curr->scup;
    int cu_width = mod_info_curr->cu_width;
    int cu_height = mod_info_curr->cu_height;

    int pic_width_in_scu = info->pic_width_in_scu;
    int h_scu = info->pic_height_in_scu;

    s32(*map_mv)[REFP_NUM][MV_D] = pic_map->map_mv;
    s32(*map_refi)[REFP_NUM] = pic_map->map_refi;
    u32* map_scu = pic_map->map_scu;

    int x_scu = scup % pic_width_in_scu;
    int y_scu = scup / pic_width_in_scu;
    int cu_width_in_scu = cu_width >> MIN_CU_LOG2;

    //-------------------  LT  -------------------//
    neb_addr_lt[0] = scup - 1;                     // A
    neb_addr_lt[1] = scup - pic_width_in_scu;      // B
    neb_addr_lt[2] = scup - pic_width_in_scu - 1;  // D
    valid_flag_lt[0] = x_scu > 0 && MCU_GET_CODED_FLAG(map_scu[neb_addr_lt[0]])
                       && !MCU_GET_INTRA_FLAG(map_scu[neb_addr_lt[0]]);
    valid_flag_lt[1] = y_scu > 0 && MCU_GET_CODED_FLAG(map_scu[neb_addr_lt[1]])
                       && !MCU_GET_INTRA_FLAG(map_scu[neb_addr_lt[1]]);
    valid_flag_lt[2] = x_scu > 0 && y_scu > 0 && MCU_GET_CODED_FLAG(map_scu[neb_addr_lt[2]])
                       && !MCU_GET_INTRA_FLAG(map_scu[neb_addr_lt[2]]);

    //-------------------  RT  -------------------//
    neb_addr_rt[0] = scup - pic_width_in_scu + cu_width_in_scu - 1;     // G
    neb_addr_rt[1] = scup - pic_width_in_scu + cu_width_in_scu;         // C
    valid_flag_rt[0] = y_scu > 0 && MCU_GET_CODED_FLAG(map_scu[neb_addr_rt[0]])
                       && !MCU_GET_INTRA_FLAG(map_scu[neb_addr_rt[0]]);
    valid_flag_rt[1] = y_scu > 0 && x_scu + cu_width_in_scu < pic_width_in_scu
                       && MCU_GET_CODED_FLAG(map_scu[neb_addr_rt[1]]) && !MCU_GET_INTRA_FLAG(map_scu[neb_addr_rt[1]]);

}

/*zxz*/
void com_get_affine_mvp_re(COM_MODE * mod_info_curr, COM_REFP refp[MAX_NUM_REF_PICS][REFP_NUM], int ptr, int lidx, \
                           CPMV mvp[VER_NUM][MV_D], int vertex_num
#if BD_AFFINE_AMVR
                           , u8 curr_mvr
#endif
                           , int neb_addr_lt[AFFINE_MAX_NUM_LT], int valid_flag_lt[AFFINE_MAX_NUM_LT], int neb_addr_rt[AFFINE_MAX_NUM_RT],
                           int valid_flag_rt[AFFINE_MAX_NUM_RT],
                           s32(*chosen_mv_lt)[REFP_NUM][MV_D], s8(*chosen_refi_lt)[REFP_NUM], s32(*chosen_mv_rt)[REFP_NUM][MV_D],
                           s32(*chosen_refi_rt)[REFP_NUM])
{

    s8 cur_refi = mod_info_curr->refi[lidx];

    int ptr_cur_ref;
    int cnt = 0, cnt_lt = 0, cnt_rt = 0;
    int i, j, k;
    int refi_tmp;

    s32 mvp_lt[MV_D], mvp_rt[MV_D];


    ptr_cur_ref = refp[cur_refi][lidx].ptr;
    for (j = 0; j < VER_NUM; j++) {
        mvp[j][MV_X] = 0;
        mvp[j][MV_Y] = 0;
    }

    //-------------------  LT  -------------------//

    for (k = 0; k < AFFINE_MAX_NUM_LT; k++) {
        if (valid_flag_lt[k] && REFI_IS_VALID(chosen_refi_lt[k][lidx])) {
            //refi_tmp = map_refi[neb_addr_lt[k]][lidx];
            refi_tmp = chosen_refi_lt[k][lidx];
#if !LIBVC_BLOCKDISTANCE_BY_LIBPTR
            //if (refp[refi_tmp][lidx].is_library_picture || refp[cur_refi][lidx].is_library_picture)
            if (0) {
                continue;
            } else
#endif
            {
                int ptr_neb_ref = refp[refi_tmp][lidx].ptr;
                //scaling_mv1(ptr, ptr_cur_ref, ptr, ptr_neb_ref, map_mv[neb_addr_lt[k]][lidx], mvp_lt);
                scaling_mv1(ptr, ptr_cur_ref, ptr, ptr_neb_ref, chosen_mv_lt[k][lidx], mvp_lt);
            }
            cnt_lt++;
            break;
        }
    }

    if (cnt_lt == 0) {
        mvp_lt[MV_X] = 0;
        mvp_lt[MV_Y] = 0;
        cnt_lt++;
    }

    //-------------------  RT  -------------------//

    for (k = 0; k < AFFINE_MAX_NUM_RT; k++) {
        if (valid_flag_rt[k] && REFI_IS_VALID(chosen_refi_rt[k][lidx])) {
            //refi_tmp = map_refi[neb_addr_rt[k]][lidx];
            refi_tmp = chosen_refi_rt[k][lidx];
#if !LIBVC_BLOCKDISTANCE_BY_LIBPTR
            //if (refp[refi_tmp][lidx].is_library_picture || refp[cur_refi][lidx].is_library_picture)
            if (0) {
                continue;
            } else
#endif
            {
                int ptr_neb_ref = refp[refi_tmp][lidx].ptr;
                //scaling_mv1(ptr, ptr_cur_ref, ptr, ptr_neb_ref, map_mv[neb_addr_rt[k]][lidx], mvp_rt);
                scaling_mv1(ptr, ptr_cur_ref, ptr, ptr_neb_ref, chosen_mv_rt[k][lidx], mvp_rt);
            }
            cnt_rt++;
            break;
        }
    }

    if (cnt_rt == 0) {
        mvp_rt[MV_X] = 0;
        mvp_rt[MV_Y] = 0;
        cnt_rt++;
    }

    mvp[0][MV_X] = mvp_lt[MV_X];
    mvp[0][MV_Y] = mvp_lt[MV_Y];
    mvp[1][MV_X] = mvp_rt[MV_X];
    mvp[1][MV_Y] = mvp_rt[MV_Y];

#if BD_AFFINE_AMVR
    for (i = 0; i < 2; i++) {
        // convert to 1/16 precision
        s32 mvp_x = (s32)mvp[i][MV_X] << 2;
        s32 mvp_y = (s32)mvp[i][MV_Y] << 2;

        // rounding
        int amvr_shift = Tab_Affine_AMVR(curr_mvr);
        com_mv_rounding_s32(mvp_x, mvp_y, &mvp_x, &mvp_y, amvr_shift, amvr_shift);

        // clipping
        mvp[i][MV_X] = (CPMV)COM_CLIP3(COM_CPMV_MIN, COM_CPMV_MAX, mvp_x);
        mvp[i][MV_Y] = (CPMV)COM_CLIP3(COM_CPMV_MIN, COM_CPMV_MAX, mvp_y);
    }
#endif
}


void com_sbac_ctx_init(COM_SBAC_CTX *sbac_ctx)
{
#if USE_ENH
    int i, num;
    //SBAC_CTX_MODEL *p;
    
    //com_mset(sbac_ctx, 0x00, sizeof(*sbac_ctx));
    
    ///* Initialization of the context models */
    //num = sizeof(COM_SBAC_CTX) / sizeof(SBAC_CTX_MODEL);
    //p = (SBAC_CTX_MODEL*)sbac_ctx;
    
    //for (i = 0; i < num; i++)
    //{
    //    p[i] = PROB_INIT;
    //}

    sbac_ctx->sp_or_ibc_flag[0] = PROB_INIT;
    sbac_ctx->sp_flag[0] = PROB_INIT;
    sbac_ctx->sp_copy_direct_flag[0] = PROB_INIT;
    sbac_ctx->sp_is_matched_flag[0] = PROB_INIT;
    sbac_ctx->sp_above_offset[0] = PROB_INIT;
    sbac_ctx->sp_offset_y_zero[0] = PROB_INIT;
    sbac_ctx->sp_offset_x_zero[0] = PROB_INIT;
    sbac_ctx->sp_n_recent_flag[0] = PROB_INIT;
    sbac_ctx->sp_pixel_is_matched_flag[0] = PROB_INIT;
    sbac_ctx->sp_cs2_flag[0] = PROB_INIT;
    sbac_ctx->sp_mode_context[0] = PROB_INIT;
    sbac_ctx->sp_str_type_context[0] = PROB_INIT;
    sbac_ctx->sp_str_scanmode_context[0] = PROB_INIT;
    sbac_ctx->sp_evs_present_flag_context[0] = PROB_INIT;
    sbac_ctx->sp_SRB_lo_ref_color_flag_context[0] = PROB_INIT;
    sbac_ctx->bgc_flag = PROB_INIT;
    sbac_ctx->bgc_idx = PROB_INIT;
    sbac_ctx->umve_awp_flag = PROB_INIT;
    sbac_ctx->umve_base_idx[0] = PROB_INIT;
    sbac_ctx->umve_step_idx[0] = PROB_INIT;
    sbac_ctx->affine_umve_flag = PROB_INIT;
    sbac_ctx->affine_umve_step_idx[0] = PROB_INIT;
    sbac_ctx->cons_mode[0] = PROB_INIT;
    sbac_ctx->iip_flag[0] = PROB_INIT;
    sbac_ctx->ipf_flag[0] = PROB_INIT;
    sbac_ctx->bvr_idx[0] = PROB_INIT;
    sbac_ctx->mvp_from_hmvp_flag[0] = PROB_INIT;
    sbac_ctx->tb_split[0] = PROB_INIT;
    sbac_ctx->affine_flag[0] = PROB_INIT;
    sbac_ctx->etmvp_flag[0] = PROB_INIT;
    sbac_ctx->awp_flag[0] = PROB_INIT;
    sbac_ctx->sawp_flag[0] = PROB_INIT;
    sbac_ctx->awp_mvr_flag = PROB_INIT;
    sbac_ctx->awp_mvr_step_idx[0] = PROB_INIT;
    sbac_ctx->smvd_flag[0] = PROB_INIT;
    sbac_ctx->ets_flag[0] = PROB_INIT;
    sbac_ctx->ts_flag[0] = PROB_INIT;
    sbac_ctx->est_flag[0] = PROB_INIT;
    sbac_ctx->st_chroma_flag[0] = PROB_INIT;
    sbac_ctx->sao_mode[0] = PROB_INIT;
    sbac_ctx->sao_offset[0] = PROB_INIT;
    sbac_ctx->alf_lcu_enable[0] = PROB_INIT;
    sbac_ctx->esao_lcu_enable[0] = PROB_INIT;
    sbac_ctx->esao_offset[0] = PROB_INIT;
    sbac_ctx->esao_chroma_mode_flag[0] = PROB_INIT;
    sbac_ctx->ccsao_lcu_flag[0] = PROB_INIT;
    sbac_ctx->ccsao_offset[0] = PROB_INIT;

    for (i = 0; i < 2; i++) {
        sbac_ctx->direct_flag[i] = PROB_INIT;
        sbac_ctx->inter_filter_flag[i] = PROB_INIT;
        sbac_ctx->umve_dir_idx[i] = PROB_INIT;
        sbac_ctx->affine_umve_dir_idx[i] = PROB_INIT;
        sbac_ctx->affine_mvr_idx[i] = PROB_INIT;
        sbac_ctx->ctp_zero_flag[i] = PROB_INIT;
        sbac_ctx->awp_mvr_dir_idx[i] = PROB_INIT;
    }

    for (i = 0; i < 3; i++) {
        sbac_ctx->ibc_flag[i] = PROB_INIT;
        sbac_ctx->sp_string_length[i] = PROB_INIT;
        sbac_ctx->sp_n_index[i] = PROB_INIT;
        sbac_ctx->sp_SRB_copy_toprun_context[i] = PROB_INIT;
        sbac_ctx->ipc_flag[i] = PROB_INIT;
        sbac_ctx->inter_dir[i] = PROB_INIT;
        sbac_ctx->refi[i] = PROB_INIT;
        sbac_ctx->cbf[i] = PROB_INIT;
        sbac_ctx->split_mode[i] = PROB_INIT;
        sbac_ctx->awp_idx[i] = PROB_INIT;
        sbac_ctx->sao_merge_flag[i] = PROB_INIT;
        sbac_ctx->nnlf_lcu_enable[i] = PROB_INIT;
    }

    for (i = 0; i < 4; i++) {
        sbac_ctx->skip_flag[i] = PROB_INIT;
        sbac_ctx->sp_lo_ref_maxlength_context[i] = PROB_INIT;
        sbac_ctx->mvr_idx[i] = PROB_INIT;
        sbac_ctx->cu_qp_delta_abs[i] = PROB_INIT;
        sbac_ctx->split_flag[i] = PROB_INIT;
        sbac_ctx->affine_mrg_idx[i] = PROB_INIT;
        sbac_ctx->etmvp_idx[i] = PROB_INIT;
        sbac_ctx->delta_qp[i] = PROB_INIT;
    }

    for (i = 0; i < 5; i++) {
        sbac_ctx->split_dir[i] = PROB_INIT;
    }

    for (i = 0; i < 6; i++) {
        sbac_ctx->pred_mode[i] = PROB_INIT;
        sbac_ctx->cbvp_idx[i] = PROB_INIT;
        sbac_ctx->mvd[2][i] = PROB_INIT;
        sbac_ctx->part_size[i] = PROB_INIT;
    }

    for (i = 0; i < 7; i++) {
        sbac_ctx->sbt_info[i] = PROB_INIT;
    }

    for (i = 0; i < 9; i++) {
        sbac_ctx->bt_split_flag[i] = PROB_INIT;
        sbac_ctx->nnlf_lcu_set[i] = PROB_INIT;
    }

    for (i = 0; i < 11; i++) {
        sbac_ctx->skip_idx_ctx[i] = PROB_INIT;
    }

    for (i = 0; i < 12; i++) {
        sbac_ctx->last1[i] = PROB_INIT;
    }

    for (i = 0; i < 14; i++) {
        sbac_ctx->intra_dir[i] = PROB_INIT;
        sbac_ctx->svd[2][i] = PROB_INIT;
    }

    for (i = 0; i < 16; i++) {
        sbac_ctx->bvd[2][i] = PROB_INIT;
    }

    for (i = 0; i < 22; i++) {
        sbac_ctx->cc_gt1[i] = PROB_INIT;
        sbac_ctx->last2[i] = PROB_INIT;
    }

    for (i = 0; i < 24; i++) {
        sbac_ctx->run[i] = PROB_INIT;
        sbac_ctx->run_rdoq[i] = PROB_INIT;
        sbac_ctx->level[i] = PROB_INIT;
    }

    for (i = 0; i < 28; i++) {
        sbac_ctx->cc_scanr_x[i] = PROB_INIT;
        sbac_ctx->cc_scanr_y[i] = PROB_INIT;
    }

    for (i = 0; i < 60; i++) {
        sbac_ctx->cc_gt0[i] = PROB_INIT;
    }
#else
    add by yanyy
    int i;
#if !AWP
    sbac_ctx->umve_flag = PROB_INIT;
#endif
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
#endif
}

U8 com_split_get_part_size(U3 split_mode, U3 part_num, U8 length)
{
	U8 ans = length;
    switch (split_mode) {
        case SPLIT_QUAD:
        case SPLIT_BI_HOR:
        case SPLIT_BI_VER:
            ans = length >> 1;
            break;
#if EQT
        case SPLIT_EQT_HOR:
        case SPLIT_EQT_VER:
            if ((part_num == 1) || (part_num == 2)) {
                ans = length >> 1;
            } else {
                ans = length >> 2;
            }
            break;
#endif
    }
    return ans;
}


U3 com_split_get_part_size_idx(U3 split_mode, U3 part_num, U3 length_idx)
{
    int ans = length_idx;
    switch (split_mode) {
        case SPLIT_QUAD:
        case SPLIT_BI_HOR:
        case SPLIT_BI_VER:
            ans = length_idx - 1;
            break;
#if EQT
        case SPLIT_EQT_HOR:
        case SPLIT_EQT_VER:
            if ((part_num == 1) || (part_num == 2)) {
                ans = length_idx - 1;
            } else {
                ans = length_idx - 2;
            }
            break;
#endif
    }
    return ans;
}
int com_split_part_count(U3 split_mode)
{
    switch (split_mode) {
		case NO_SPLIT:
			return 1;
        case SPLIT_BI_VER:
            return 2;
        case SPLIT_BI_HOR:
            return 2;
        case SPLIT_EQT_VER:
            return 4;
        case SPLIT_EQT_HOR:
            return 4;
        case SPLIT_QUAD:
            return 4;
        default:
            return 0;
    }
}
SPLIT_DIR com_split_get_direction(U3 mode)
{
    switch (mode) {
        case SPLIT_BI_HOR:
#if EQT
        case SPLIT_EQT_HOR:
#endif
            return SPLIT_HOR;
        case SPLIT_BI_VER:
#if EQT
        case SPLIT_EQT_VER:
            return SPLIT_VER;
#endif
        default:
            return SPLIT_QT;
    }
}

int com_split_is_vertical(U3 mode)
{
    return com_split_get_direction(mode) == SPLIT_VER ? 1 : 0;
}

int com_split_is_horizontal(U3 mode)
{
    return com_split_get_direction(mode) == SPLIT_HOR ? 1 : 0;
}


int  com_split_is_BT(U3 mode)
{
    return (mode == SPLIT_BI_HOR) || (mode == SPLIT_BI_VER) ? 1 : 0;
}

void md_com_split_get_part_structure(U3 split_mode, U14 x0, U14 y0, U8 cu_width, U8 cu_height, U12 cup, U4 cud,
	U3 log2_culine, COM_SPLIT_STRUCT* split_struct)
{
#pragma HLS array_partition variable=split_struct->cup complete dim=1
#pragma HLS array_partition variable=split_struct->x_pos complete dim=1
#pragma HLS array_partition variable=split_struct->y_pos complete dim=1
	// U3 i;
	U3 log_cuw, log_cuh;
	/*split_struct->part_count = com_split_part_count(split_mode);*/
	U3 part_count_array[6] = { 1, 2, 2, 4, 4, 4 };
	split_struct->part_count = part_count_array[split_mode];
	log_cuw = CONV_LOG2(cu_width);
	log_cuh = CONV_LOG2(cu_height);
	split_struct->x_pos[0] = x0;
	split_struct->y_pos[0] = y0;
	split_struct->cup[0] = cup;
	if (split_mode == NO_SPLIT){
		split_struct->width[0] = cu_width;
		split_struct->height[0] = cu_height;
		split_struct->log_cuw[0] = log_cuw;
		split_struct->log_cuh[0] = log_cuh;
		split_struct->cud = cud;
	}
	else if (split_mode == SPLIT_BI_VER){
		for (U3 i = 0; i < split_struct->part_count; ++i) {
#pragma HLS pipeline
#pragma HLS loop_tripcount max=4
			split_struct->width[i] = cu_width >> 1;
			split_struct->height[i] = cu_height;
            split_struct->log_cuw[i] = log_cuw - 1;
			split_struct->log_cuh[i] = log_cuh;
			if (i>0){
				split_struct->x_pos[i] = split_struct->x_pos[i - 1] + split_struct->width[i - 1];
				split_struct->y_pos[i] = split_struct->y_pos[i - 1];
				split_struct->cup[i] = split_struct->cup[i - 1] + (split_struct->width[i - 1] >> MIN_CU_LOG2);
			}
		}
		split_struct->cud = cud + ((cu_width == cu_height || cu_width < cu_height) ? 0 : 1);
	}
	else{
		for (U3 i = 0; i < split_struct->part_count; ++i) {
#pragma HLS pipeline
#pragma HLS loop_tripcount max=4
			split_struct->width[i] = cu_width;
            split_struct->height [i] = cu_height >> 1;
			split_struct->log_cuw[i] = log_cuw;
			split_struct->log_cuh[i] = log_cuh - 1;
			if (i>0){
				split_struct->y_pos[i] = split_struct->y_pos[i - 1] + split_struct->height[i - 1];
				split_struct->x_pos[i] = split_struct->x_pos[i - 1];
				split_struct->cup[i] = split_struct->cup[i - 1] + ((split_struct->height[i - 1] >> MIN_CU_LOG2) << log2_culine);
			}
		}
		split_struct->cud = cud + ((cu_width == cu_height || cu_width > cu_height) ? 0 : 1);
	}
}
void com_split_get_part_structure_only_bt(U3 split_mode, U14 x0, U14 y0, U8 cu_width, U8 cu_height, U12 cup, U4 cud,
	U3 log2_culine, COM_SPLIT_STRUCT* split_struct)
{
	U3 i;
	U3 log_cuw, log_cuh;
	/*split_struct->part_count = com_split_part_count(split_mode);*/
	//U3 part_count_array[6] = { 1, 2, 2, 4, 4, 4 };
	split_struct->part_count = 2;
	log_cuw = CONV_LOG2(cu_width);
	log_cuh = CONV_LOG2(cu_height);
	split_struct->x_pos[0] = x0;
	split_struct->y_pos[0] = y0;
	split_struct->cup[0] = cup;
	if (split_mode == SPLIT_BI_VER){
		for (i = 0; i < split_struct->part_count; ++i) {
#pragma HLS loop_tripcount max=4
			split_struct->width[i] = cu_width >> 1;
			split_struct->log_cuw[i] = log_cuw - 1;
			split_struct->height[i] = cu_height;
			split_struct->log_cuh[i] = log_cuh;
			if (i){
				split_struct->x_pos[i] = split_struct->x_pos[i - 1] + split_struct->width[i - 1];
				split_struct->y_pos[i] = split_struct->y_pos[i - 1];
				split_struct->cup[i] = split_struct->cup[i - 1] + (split_struct->width[i - 1] >> MIN_CU_LOG2);
			}
		}
		split_struct->cud = cud + ((cu_width == cu_height || cu_width < cu_height) ? 0 : 1);
	}
	else{
		for (i = 0; i < split_struct->part_count; ++i) {
#pragma HLS loop_tripcount max=4
			split_struct->width[i] = cu_width;
			split_struct->log_cuw[i] = log_cuw;
			if (i){
				split_struct->y_pos[i] = split_struct->y_pos[i - 1] + split_struct->height[i - 1];
				split_struct->x_pos[i] = split_struct->x_pos[i - 1];
				split_struct->cup[i] = split_struct->cup[i - 1] + ((split_struct->height[i - 1] >> MIN_CU_LOG2) << log2_culine);
			}
			split_struct->height[i] = cu_height >> 1;
			split_struct->log_cuh[i] = log_cuh - 1;
		}
		split_struct->cud = cud + ((cu_width == cu_height || cu_width > cu_height) ? 0 : 1);
	}
}
void com_split_get_part_structure_only_qt(U14 x0, U14 y0, U8 cu_width, U8 cu_height, U12 cup, U4 cud,
	U3 log2_culine, COM_SPLIT_STRUCT* split_struct)
{
	U3 i;
	U3 log_cuw, log_cuh;
	U12 cup_w, cup_h;
	split_struct->part_count = 4;
	log_cuw = CONV_LOG2(cu_width);
	log_cuh = CONV_LOG2(cu_height);
	split_struct->x_pos[0] = x0;
	split_struct->y_pos[0] = y0;
	split_struct->cup[0] = cup;
	split_struct->width[0] = cu_width >> 1;
	split_struct->height[0] = cu_height >> 1;
	split_struct->log_cuw[0] = log_cuw - 1;
	split_struct->log_cuh[0] = log_cuh - 1;
	for (i = 1; i < split_struct->part_count; ++i) {
#pragma HLS loop_tripcount max=3
#pragma HLS pipeline
#pragma HLS unroll
		split_struct->width[i] = split_struct->width[0];
		split_struct->height[i] = split_struct->height[0];
		split_struct->log_cuw[i] = split_struct->log_cuw[0];
		split_struct->log_cuh[i] = split_struct->log_cuh[0];
	}
	split_struct->x_pos[1] = x0 + split_struct->width[0];
	split_struct->y_pos[1] = y0;
	split_struct->x_pos[2] = x0;
	split_struct->y_pos[2] = y0 + split_struct->height[0];
	split_struct->x_pos[3] = split_struct->x_pos[1];
	split_struct->y_pos[3] = split_struct->y_pos[2];
	cup_w = (split_struct->width[0] >> MIN_CU_LOG2);
	cup_h = (((U12)(split_struct->height[0] >> MIN_CU_LOG2)) << log2_culine);
	split_struct->cup[1] = cup + cup_w;
	split_struct->cup[2] = cup + cup_h;
	split_struct->cup[3] = split_struct->cup[1] + cup_h;
	split_struct->cud = cud + 1;

	return;
}

void com_split_get_part_structure(U3 split_mode, U14 x0, U14 y0, U8 cu_width, U8 cu_height, U12 cup, U4 cud,
                                  U3 log2_culine, COM_SPLIT_STRUCT* split_struct)
{
    U3 i;
    U3 log_cuw, log_cuh;
    U12 cup_w, cup_h;
    split_struct->part_count = com_split_part_count(split_mode);
    log_cuw = CONV_LOG2(cu_width);
    log_cuh = CONV_LOG2(cu_height);
    split_struct->x_pos[0] = x0;
    split_struct->y_pos[0] = y0;
    split_struct->cup[0] = cup;
    switch (split_mode) {
        case NO_SPLIT: {
            split_struct->width[0] = cu_width;
            split_struct->height[0] = cu_height;
            split_struct->log_cuw[0] = log_cuw;
            split_struct->log_cuh[0] = log_cuh;
			split_struct->cud = cud;
        }
        break;
        case SPLIT_QUAD: {
            split_struct->width[0] = cu_width >> 1;
            split_struct->height[0] = cu_height >> 1;
            split_struct->log_cuw[0] = log_cuw - 1;
            split_struct->log_cuh[0] = log_cuh - 1;
            for (i = 1; i < split_struct->part_count; ++i) {
#pragma HLS loop_tripcount max=3
                split_struct->width[i] = split_struct->width[0];
                split_struct->height[i] = split_struct->height[0];
                split_struct->log_cuw[i] = split_struct->log_cuw[0];
                split_struct->log_cuh[i] = split_struct->log_cuh[0];
            }
            split_struct->x_pos[1] = x0 + split_struct->width[0];
            split_struct->y_pos[1] = y0;
            split_struct->x_pos[2] = x0;
            split_struct->y_pos[2] = y0 + split_struct->height[0];
            split_struct->x_pos[3] = split_struct->x_pos[1];
            split_struct->y_pos[3] = split_struct->y_pos[2];
            cup_w = (split_struct->width[0] >> MIN_CU_LOG2);
			cup_h = (((U12)(split_struct->height[0] >> MIN_CU_LOG2)) << log2_culine);
            split_struct->cup[1] = cup + cup_w;
            split_struct->cup[2] = cup + cup_h;
            split_struct->cup[3] = split_struct->cup[1] + cup_h;
            split_struct->cud = cud + 1;
        }
        break;
        default: {
            if (com_split_is_vertical(split_mode)) {
                for (i = 0; i < split_struct->part_count; ++i) {
#pragma HLS loop_tripcount max=4
                    split_struct->width[i] = com_split_get_part_size(split_mode, i, cu_width);
                    split_struct->log_cuw[i] = com_split_get_part_size_idx(split_mode, i, log_cuw);

                    if (split_mode == SPLIT_EQT_VER) {
                        if (i == 0 || i == 3) {
                            split_struct->height[i] = cu_height;
                            split_struct->log_cuh[i] = log_cuh;
                        }
                        else {
                            split_struct->height[i] = cu_height >> 1;
                            split_struct->log_cuh[i] = log_cuh - 1;
                        }
                    }
                    else {
                        split_struct->height[i] = cu_height;
                        split_struct->log_cuh[i] = log_cuh;
                        if (i) {
                            split_struct->x_pos[i] = split_struct->x_pos[i - 1] + split_struct->width[i - 1];
                            split_struct->y_pos[i] = split_struct->y_pos[i - 1];
                            split_struct->cup[i] = split_struct->cup[i - 1] + (split_struct->width[i - 1] >> MIN_CU_LOG2);
                        }
                    }

                }
#if EQT
                if (split_mode == SPLIT_EQT_VER) {
                    split_struct->x_pos[1] = split_struct->x_pos[0] + split_struct->width[0];
                    split_struct->y_pos[1] = split_struct->y_pos[0];
                    split_struct->cup[1] = split_struct->cup[0] + (split_struct->width[0] >> MIN_CU_LOG2);
                    cup_h = ((split_struct->height[1] >> MIN_CU_LOG2) << log2_culine);
                    split_struct->x_pos[2] = split_struct->x_pos[1];
                    split_struct->y_pos[2] = split_struct->y_pos[1] + split_struct->height[1];
                    split_struct->cup[2] = split_struct->cup[1] + cup_h;
                    split_struct->x_pos[3] = split_struct->x_pos[1] + split_struct->width[1];
                    split_struct->y_pos[3] = split_struct->y_pos[1];
                    split_struct->cup[3] = split_struct->cup[1] + (split_struct->width[1] >> MIN_CU_LOG2);
                }
#endif
            } else {
                for (i = 0; i < split_struct->part_count; ++i) {
#pragma HLS loop_tripcount max=4
                    if (split_mode == SPLIT_EQT_HOR) {
                        if (i == 0 || i == 3) {
                            split_struct->width[i] = cu_width;
                            split_struct->log_cuw[i] = log_cuw;
                        } else {
                            split_struct->width[i] = cu_width >> 1;
                            split_struct->log_cuw[i] = log_cuw - 1;
                        }
                    } else {
                        split_struct->width[i] = cu_width;
                        split_struct->log_cuw[i] = log_cuw;
                        if (i) {
                            split_struct->y_pos[i] = split_struct->y_pos[i - 1] + split_struct->height[i - 1];
                            split_struct->x_pos[i] = split_struct->x_pos[i - 1];
                            split_struct->cup[i] = split_struct->cup[i - 1] + ((split_struct->height[i - 1] >> MIN_CU_LOG2) << log2_culine);
                        }
                    }

                    split_struct->height[i] = com_split_get_part_size(split_mode, i, cu_height);
                    split_struct->log_cuh[i] = com_split_get_part_size_idx(split_mode, i, log_cuh);
                }
#if EQT
                if (split_mode == SPLIT_EQT_HOR) {
                    split_struct->y_pos[1] = split_struct->y_pos[0] + split_struct->height[0];
                    split_struct->x_pos[1] = split_struct->x_pos[0];
                    split_struct->cup[1] = split_struct->cup[0] + ((split_struct->height[0] >> MIN_CU_LOG2) << log2_culine);
                    split_struct->y_pos[2] = split_struct->y_pos[1];
                    split_struct->x_pos[2] = split_struct->x_pos[1] + split_struct->width[1];
                    split_struct->cup[2] = split_struct->cup[1] + (split_struct->width[1] >> MIN_CU_LOG2);
                    split_struct->y_pos[3] = split_struct->y_pos[1] + split_struct->height[1];
                    split_struct->x_pos[3] = split_struct->x_pos[1];
                    split_struct->cup[3] = split_struct->cup[1] + ((split_struct->height[1] >> MIN_CU_LOG2) << log2_culine);
                }
#endif
            }
            switch (split_mode) {
                case SPLIT_BI_VER:
                    split_struct->cud = cud + ((cu_width == cu_height || cu_width < cu_height) ? 0 : 1);
                    break;
                case SPLIT_BI_HOR:
                    split_struct->cud = cud + ((cu_width == cu_height || cu_width > cu_height) ? 0 : 1);
                    break;
                default:
                    // Triple tree case
                    split_struct->cud = cud + (cu_width == cu_height ? 0 : 1);
                    break;
            }
        }
        break;
    }

}

void com_split_get_part_structure_pcnl(int split_mode, int x0, int y0, int cu_width, int cu_height, int cup, int cud,
    int log2_culine, COM_SPLIT_STRUCT* split_struct)
{
    int i;
    int log_cuw, log_cuh;
    int cup_w, cup_h;
    split_struct->part_count = com_split_part_count(split_mode);
    log_cuw = CONV_LOG2(cu_width);
    log_cuh = CONV_LOG2(cu_height);
    split_struct->x_pos[0] = x0;
    split_struct->y_pos[0] = y0;
    split_struct->cup[0] = cup;
    switch (split_mode) {
    case NO_SPLIT: {
        split_struct->width[0] = cu_width;
        split_struct->height[0] = cu_height;
        split_struct->log_cuw[0] = log_cuw;
        split_struct->log_cuh[0] = log_cuh;
    }
                 break;
    case SPLIT_QUAD: {
        split_struct->width[0] = cu_width >> 1;
        split_struct->height[0] = cu_height >> 1;
        split_struct->log_cuw[0] = log_cuw - 1;
        split_struct->log_cuh[0] = log_cuh - 1;
        for (i = 1; i < split_struct->part_count; ++i) {
#pragma HLS loop_tripcount max=3
            split_struct->width[i] = split_struct->width[0];
            split_struct->height[i] = split_struct->height[0];
            split_struct->log_cuw[i] = split_struct->log_cuw[0];
            split_struct->log_cuh[i] = split_struct->log_cuh[0];
        }
        split_struct->x_pos[1] = x0 + split_struct->width[0];
        split_struct->y_pos[1] = y0;
        split_struct->x_pos[2] = x0;
        split_struct->y_pos[2] = y0 + split_struct->height[0];
        split_struct->x_pos[3] = split_struct->x_pos[1];
        split_struct->y_pos[3] = split_struct->y_pos[2];
        cup_w = (split_struct->width[0] >> MIN_CU_LOG2);
        cup_h = ((split_struct->height[0] >> MIN_CU_LOG2) << log2_culine);
        split_struct->cup[1] = cup + cup_w;
        split_struct->cup[2] = cup + cup_h;
        split_struct->cup[3] = split_struct->cup[1] + cup_h;
        split_struct->cud = cud + 1;
    }
                   break;
    default: {
        if (com_split_is_vertical((SPLIT_MODE)split_mode)) {
            for (i = 0; i < split_struct->part_count; ++i) {
#pragma HLS loop_tripcount max=4
                split_struct->width[i] = com_split_get_part_size(split_mode, i, cu_width);
                split_struct->log_cuw[i] = com_split_get_part_size_idx(split_mode, i, log_cuw);

                if (split_mode == SPLIT_EQT_VER) {
                    if (i == 0 || i == 3) {
                        split_struct->height[i] = cu_height;
                        split_struct->log_cuh[i] = log_cuh;
                    }
                    else {
                        split_struct->height[i] = cu_height >> 1;
                        split_struct->log_cuh[i] = log_cuh - 1;
                    }
                }
                else {
                    split_struct->height[i] = cu_height;
                    split_struct->log_cuh[i] = log_cuh;
                    if (i) {
                        split_struct->x_pos[i] = split_struct->x_pos[i - 1] + split_struct->width[i - 1];
                        split_struct->y_pos[i] = split_struct->y_pos[i - 1];
                        split_struct->cup[i] = split_struct->cup[i - 1] + (split_struct->width[i - 1] >> MIN_CU_LOG2);
                    }
                }

            }
#if EQT
            if (split_mode == SPLIT_EQT_VER) {
                split_struct->x_pos[1] = split_struct->x_pos[0] + split_struct->width[0];
                split_struct->y_pos[1] = split_struct->y_pos[0];
                split_struct->cup[1] = split_struct->cup[0] + (split_struct->width[0] >> MIN_CU_LOG2);
                cup_h = ((split_struct->height[1] >> MIN_CU_LOG2) << log2_culine);
                split_struct->x_pos[2] = split_struct->x_pos[1];
                split_struct->y_pos[2] = split_struct->y_pos[1] + split_struct->height[1];
                split_struct->cup[2] = split_struct->cup[1] + cup_h;
                split_struct->x_pos[3] = split_struct->x_pos[1] + split_struct->width[1];
                split_struct->y_pos[3] = split_struct->y_pos[1];
                split_struct->cup[3] = split_struct->cup[1] + (split_struct->width[1] >> MIN_CU_LOG2);
            }
#endif
        }
        else {
            for (i = 0; i < split_struct->part_count; ++i) {
#pragma HLS loop_tripcount max=4
                if (split_mode == SPLIT_EQT_HOR) {
                    if (i == 0 || i == 3) {
                        split_struct->width[i] = cu_width;
                        split_struct->log_cuw[i] = log_cuw;
                    }
                    else {
                        split_struct->width[i] = cu_width >> 1;
                        split_struct->log_cuw[i] = log_cuw - 1;
                    }
                }
                else {
                    split_struct->width[i] = cu_width;
                    split_struct->log_cuw[i] = log_cuw;
                    if (i) {
                        split_struct->y_pos[i] = split_struct->y_pos[i - 1] + split_struct->height[i - 1];
                        split_struct->x_pos[i] = split_struct->x_pos[i - 1];
                        split_struct->cup[i] = split_struct->cup[i - 1] + ((split_struct->height[i - 1] >> MIN_CU_LOG2) << log2_culine);
                    }
                }

                split_struct->height[i] = com_split_get_part_size(split_mode, i, cu_height);
                split_struct->log_cuh[i] = com_split_get_part_size_idx(split_mode, i, log_cuh);
            }
#if EQT
            if (split_mode == SPLIT_EQT_HOR) {
                split_struct->y_pos[1] = split_struct->y_pos[0] + split_struct->height[0];
                split_struct->x_pos[1] = split_struct->x_pos[0];
                split_struct->cup[1] = split_struct->cup[0] + ((split_struct->height[0] >> MIN_CU_LOG2) << log2_culine);
                split_struct->y_pos[2] = split_struct->y_pos[1];
                split_struct->x_pos[2] = split_struct->x_pos[1] + split_struct->width[1];
                split_struct->cup[2] = split_struct->cup[1] + (split_struct->width[1] >> MIN_CU_LOG2);
                split_struct->y_pos[3] = split_struct->y_pos[1] + split_struct->height[1];
                split_struct->x_pos[3] = split_struct->x_pos[1];
                split_struct->cup[3] = split_struct->cup[1] + ((split_struct->height[1] >> MIN_CU_LOG2) << log2_culine);
            }
#endif
        }
        switch (split_mode) {
        case SPLIT_BI_VER:
            split_struct->cud = cud + ((cu_width == cu_height || cu_width < cu_height) ? 0 : 1);
            break;
        case SPLIT_BI_HOR:
            split_struct->cud = cud + ((cu_width == cu_height || cu_width > cu_height) ? 0 : 1);
            break;
        default:
            // Triple tree case
            split_struct->cud = cud + (cu_width == cu_height ? 0 : 1);
            break;
        }
    }
           break;
    }
}

int com_set_split_mode_pcnl(s8 split_mode, int cud, int cup, int cu_width, int cu_height, int lcu_s,
    u8 split_mode_buf[MAX_CU_DEPTH][NUM_BLOCK_SHAPE][(1 << 7 >> 1) * (1 << 7 >> 1) / ((1 << 2) * (1 << 2))])
{
    int ret = 0;
    int pos = cup + ((cu_height >> 1 >> 2) * (lcu_s >> 2) + (cu_width >> 1 >> 2));
    int shape = ((int)SQUARE) + (com_tbl_log2[cu_width] - com_tbl_log2[cu_height]);
    shape >= 0
        && shape < ((int)NUM_BLOCK_SHAPE) ? ((void)0) : __assert_HLS("shape >= 0 && shape < NUM_BLOCK_SHAPE",
            "../separate/com_util.c", (unsigned int)1970, __FUNCTION__);
    if (cu_width < 8 && cu_height < 8) {
        return ret;
    }
    split_mode_buf[cud][shape][pos] = split_mode;
    return ret;
}

void com_split_get_split_rdo_order(U8 cu_width, U8 cu_height, U3 splits[MAX_SPLIT_NUM])
{
    splits[0] = NO_SPLIT;
    //qt must be tried first; otherwise, due to the split save & load fast algorithm, qt will be never tried in RDO (previous split decision is made base on bt/eqt)
    splits[1] = SPLIT_QUAD;
    splits[2] = cu_width < cu_height ? SPLIT_BI_HOR : SPLIT_BI_VER;
    splits[3] = cu_width < cu_height ? SPLIT_BI_VER : SPLIT_BI_HOR;
    splits[4] = cu_width < cu_height ? SPLIT_EQT_HOR : SPLIT_EQT_VER;
    splits[5] = cu_width < cu_height ? SPLIT_EQT_VER : SPLIT_EQT_HOR;

}


#if DT_SYNTAX
int com_dt_allow(int cu_w, int cu_h, int pred_mode, int max_dt_size)
{
#pragma HLS INLINE off
    //only allow intra DT
    if (pred_mode != MODE_INTRA) {
        return 0;
    }

    int max_size = max_dt_size;
    int min_size = 16;
    int hori_allow = cu_h >= min_size && (cu_w <= max_size && cu_h <= max_size) && cu_w < cu_h * 4;
    int vert_allow = cu_w >= min_size && (cu_w <= max_size && cu_h <= max_size) && cu_h < cu_w * 4;

    return hori_allow + (vert_allow << 1);
}
#endif


#if TB_SPLIT_EXT


void set_pb_part(COM_MODE *mod_info_curr, int part_size)
{
    mod_info_curr->pb_part = part_size;
}

void set_tb_part(COM_MODE *mod_info_curr, int part_size)
{
    mod_info_curr->tb_part = part_size;
}
/****************************
		Function: get_part_info
		Description: 根据帧间PU尺寸不同获得相关的预测信息
		Calls: NULL
		Inputs: PU坐标信息和宽高信息,存储相关信息的数组
		Outputs: PU大小和位置
		Return: NULL
		Others: none
		****************************/
void get_part_info_init(U11 pic_width_in_scu, U13 x, U13 y, U7 w, U7 h, U4 part_size, COM_PART_INFO* sub_info)
{
#pragma HLS ARRAY_PARTITION variable=sub_info->sub_x complete dim=0
#pragma HLS ARRAY_PARTITION variable=sub_info->sub_y complete dim=0
#pragma HLS ARRAY_PARTITION variable=sub_info->sub_w complete dim=0
#pragma HLS ARRAY_PARTITION variable=sub_info->sub_h complete dim=0
#pragma HLS ARRAY_PARTITION variable=sub_info->sub_scup complete dim=0
#pragma HLS INLINE
	U8 i;
	U5 qw = w >> 2;
	U5 qh = h >> 2;
	U12 x_scu, y_scu;

	for (i = 0; i < MAX_NUM_PB; i++) {
#pragma HLS unroll
#pragma HLS LOOP_TRIPCOUNT max=4
		sub_info->num_sub_part = 0;
		sub_info->sub_x[i] = 0;
		sub_info->sub_y[i] = 0;
		sub_info->sub_w[i] = 0;
		sub_info->sub_h[i] = 0;
		sub_info->sub_scup[i] = 0;
	}

	//derive sub_part x, y, w, h
	if (part_size == SIZE_2Nx2N) {
		sub_info->num_sub_part = 1;
		sub_info->sub_x[0] = x;
		sub_info->sub_y[0] = y;
		sub_info->sub_w[0] = w;
		sub_info->sub_h[0] = h;
	}
	else {
		assert(0);
	}

	//derive sub_part scup
	for (i = 0; i < sub_info->num_sub_part; i++) {
#pragma HLS unroll
#pragma HLS LOOP_TRIPCOUNT max=4
		x_scu = PEL2SCU(sub_info->sub_x[i]);
		y_scu = PEL2SCU(sub_info->sub_y[i]);
		sub_info->sub_scup[i] = x_scu + y_scu * pic_width_in_scu;
	}
}

void get_part_info(U11 pic_width_in_scu, U13 x, U13 y, U7 w, U7 h, U4 part_size, COM_PART_INFO* sub_info)
{
#pragma HLS ARRAY_PARTITION variable=sub_info->sub_x complete dim=0
#pragma HLS ARRAY_PARTITION variable=sub_info->sub_y complete dim=0
#pragma HLS ARRAY_PARTITION variable=sub_info->sub_w complete dim=0
#pragma HLS ARRAY_PARTITION variable=sub_info->sub_h complete dim=0
#pragma HLS ARRAY_PARTITION variable=sub_info->sub_scup complete dim=0
#pragma HLS INLINE
    U8 i;
    U5 qw = w >> 2;
    U5 qh = h >> 2;
    U12 x_scu, y_scu;

    for (i = 0; i < MAX_NUM_PB; i++) {
#pragma HLS unroll
#pragma HLS LOOP_TRIPCOUNT max=4
        sub_info->num_sub_part = 0;
        sub_info->sub_x[i] = 0;
        sub_info->sub_y[i] = 0;
        sub_info->sub_w[i] = 0;
        sub_info->sub_h[i] = 0;
        sub_info->sub_scup[i] = 0;
    }

    //derive sub_part x, y, w, h
    if (part_size == SIZE_2Nx2N) {
        sub_info->num_sub_part = 1;
        sub_info->sub_x[0] = x;
        sub_info->sub_y[0] = y;
        sub_info->sub_w[0] = w;
        sub_info->sub_h[0] = h;
    } else if (part_size == SIZE_2NxhN) {
        sub_info->num_sub_part = 4;
        for (i = 0; i < 4; i++) {
#pragma HLS unroll
#pragma HLS LOOP_TRIPCOUNT max=4
            sub_info->sub_x[i] = x;
            sub_info->sub_y[i] = qh * i + y;
            sub_info->sub_w[i] = w;
            sub_info->sub_h[i] = qh;
        }
    } else if (part_size == SIZE_2NxnU) {
        sub_info->num_sub_part = 2;
        for (i = 0; i < 2; i++) {
#pragma HLS unroll
#pragma HLS LOOP_TRIPCOUNT max=2
            sub_info->sub_x[i] = x;
            sub_info->sub_y[i] = qh * (i == 0 ? 0 : 1) + y;
            sub_info->sub_w[i] = w;
            sub_info->sub_h[i] = qh * (i == 0 ? 1 : 3);
        }
    } else if (part_size == SIZE_2NxnD) {
        sub_info->num_sub_part = 2;
        for (i = 0; i < 2; i++) {
#pragma HLS unroll
#pragma HLS LOOP_TRIPCOUNT max=2
            sub_info->sub_x[i] = x;
            sub_info->sub_y[i] = qh * (i == 0 ? 0 : 3) + y;
            sub_info->sub_w[i] = w;
            sub_info->sub_h[i] = qh * (i == 0 ? 3 : 1);
        }
    } else if (part_size == SIZE_hNx2N) {
        sub_info->num_sub_part = 4;
        for (i = 0; i < 4; i++) {
#pragma HLS unroll
#pragma HLS LOOP_TRIPCOUNT max=4
            sub_info->sub_x[i] = qw * i + x;
            sub_info->sub_y[i] = y;
            sub_info->sub_w[i] = qw;
            sub_info->sub_h[i] = h;
        }
    } else if (part_size == SIZE_nLx2N) {
        sub_info->num_sub_part = 2;
        for (i = 0; i < 2; i++) {
#pragma HLS unroll
#pragma HLS LOOP_TRIPCOUNT max=2
            sub_info->sub_x[i] = qw * (i == 0 ? 0 : 1) + x;
            sub_info->sub_y[i] = y;
            sub_info->sub_w[i] = qw * (i == 0 ? 1 : 3);
            sub_info->sub_h[i] = h;
        }
    } else if (part_size == SIZE_nRx2N) {
        sub_info->num_sub_part = 2;
        for (i = 0; i < 2; i++) {
#pragma HLS unroll
#pragma HLS LOOP_TRIPCOUNT max=2
            sub_info->sub_x[i] = qw * (i == 0 ? 0 : 3) + x;
            sub_info->sub_y[i] = y;
            sub_info->sub_w[i] = qw * (i == 0 ? 3 : 1);
            sub_info->sub_h[i] = h;
        }
    } else if (part_size == SIZE_NxN) {
        sub_info->num_sub_part = 4;
        for (i = 0; i < 4; i++) {
#pragma HLS unroll
#pragma HLS LOOP_TRIPCOUNT max=4
            sub_info->sub_x[i] = qw * (i == 0 || i == 2 ? 0 : 2) + x;
            sub_info->sub_y[i] = qh * (i == 0 || i == 1 ? 0 : 2) + y;
            sub_info->sub_w[i] = qw * 2;
            sub_info->sub_h[i] = qh * 2;
        }
    } else {
        assert(0);
    }

    //derive sub_part scup
	//for (i = 0; i < sub_info->num_sub_part; i++) {
	for (i = 0; i < 4; i++) {
#pragma HLS unroll
#pragma HLS LOOP_TRIPCOUNT max=4
        x_scu = PEL2SCU(sub_info->sub_x[i]);
        y_scu = PEL2SCU(sub_info->sub_y[i]);
        sub_info->sub_scup[i] = x_scu + y_scu * pic_width_in_scu;
    }
}

U2  get_part_idx(PART_SIZE part_size, U7 x, U7 y, U7 w, U7 h)
{
    U2 idx = 0;
    U6 hw = w >> 1; //half width
	U6 hh = h >> 1; //half height
	U6 qw = w >> 2; //quarter width
	U6 qh = h >> 2; //quarter height

    assert(x < w);
    assert(y < h);
    assert(w >= 4);
    assert(h >= 4);

    if (part_size == SIZE_2Nx2N) {
        idx = 0;
    } else if (part_size == SIZE_NxN) {
        if (x < hw && y < hh) {
            idx = 0;
        } else if (x >= hw && y < hh) {
            idx = 1;
        } else if (x < hw && y >= hh) {
            idx = 2;
        } else {
            idx = 3;
        }
    } else if (part_size == SIZE_2NxhN) {
        if (y < qh) {         // 1/4
            idx = 0;
        } else if (y < hh) {  // 2/4
            idx = 1;
        } else if (y < qh + hh) { // 3/4
            idx = 2;
        } else {
            idx = 3;
        }
    } else if (part_size == SIZE_hNx2N) {
        if (x < qw) {         // 1/4
            idx = 0;
        } else if (x < hw) {  // 2/4
            idx = 1;
        } else if (x < qw + hw) { // 3/4
            idx = 2;
        } else {
            idx = 3;
        }
    } else if (part_size == SIZE_2NxnU) {
        if (y < qh) {
            idx = 0;
        } else {
            idx = 1;
        }
    } else if (part_size == SIZE_2NxnD) {
        if (y < qh + hh) {
            idx = 0;
        } else {
            idx = 1;
        }
    } else if (part_size == SIZE_nLx2N) {
        if (x < qw) {
            idx = 0;
        } else {
            idx = 1;
        }
    } else if (part_size == SIZE_nRx2N) {
        if (x < qw + hw) {
            idx = 0;
        } else {
            idx = 1;
        }
    } else {
        //printf("\nError: part_size not expected");
        assert(0);
    }
    return idx;
}
//me_line_map_ptr =fetch_output.me_line_map_ptr
//void update_intra_info_map_scu(ENC_ME_LINE_MAP *me_line_map_ptr, int tb_x, int tb_y, int tb_w, int tb_h,
//                               int pic_width_in_scu, int ipm)
//{
//#pragma HLS INLINE off
//    int scu_x = PEL2SCU(tb_x);
//    int scu_y = PEL2SCU(tb_y);
//    int scu_w = PEL2SCU(tb_w);
//    int scu_h = PEL2SCU(tb_h);
//#if IME_LINE_ALL
//    int scu_x_line = scu_x - me_line_map_ptr->min_scu_x;
//    int scu_y_line = scu_y - me_line_map_ptr->min_scu_y;
//#endif
//#if Use_MPM
//    int mpm_x = scu_x % 16;
//    int mpm_y = scu_y % 16;
//#endif
////   map_scu = map_scu + scu_y * pic_width_in_scu + scu_x;
////   map_ipm = map_ipm + scu_y * pic_width_in_scu + scu_x;
//
//    for (int j = 0; j < scu_h; j++) {
//#pragma HLS LOOP_TRIPCOUNT max=3
//        for (int i = 0; i < scu_w; i++) {
//#pragma HLS LOOP_TRIPCOUNT max=3
//            //     MCU_SET_CODED_FLAG(map_scu[i]);
//            //     MCU_SET_INTRA_FLAG(map_scu[i]);
//#if IME_LINE_ALL
//            MCU_SET_CODED_FLAG(me_line_map_ptr->map_scu_inner[me_line_map_ptr->curr_ctu_idx][scu_x_line + i][scu_y_line + j]);
//            MCU_SET_INTRA_FLAG(me_line_map_ptr->map_scu_inner[me_line_map_ptr->curr_ctu_idx][scu_x_line + i][scu_y_line + j]);
//#endif
//#if IP_map_scu
//            MCU_SET_CODED_FLAG(MAP_SCU_IP_ptr->map_scu_inner[MAP_SCU_IP_ptr->curr_ctu_idx][scu_x_line + i][scu_y_line + j]);
//            MCU_SET_INTRA_FLAG(MAP_SCU_IP_ptr->map_scu_inner[MAP_SCU_IP_ptr->curr_ctu_idx][scu_x_line + i][scu_y_line + j]);
//#endif
//#if Use_MPM
////			MPM_buffer_ptr->buffer[j + mpm_y][i + mpm_x] = ipm;
//#else
//
//            map_ipm[i] = ipm;
//#endif
//        }
//        //    map_scu += pic_width_in_scu;
//        //   map_ipm += pic_width_in_scu;
//    }
//}
#endif

int get_part_num(PART_SIZE size)
{
#pragma HLS INLINE off
    switch (size) {
        case SIZE_2Nx2N:
            return 1;
        case SIZE_NxN:
            return 4;
        case SIZE_2NxnU:
            return 2;
        case SIZE_2NxnD:
            return 2;
        case SIZE_2NxhN:
            return 4;
        case SIZE_nLx2N:
            return 2;
        case SIZE_nRx2N:
            return 2;
        case SIZE_hNx2N:
            return 4;
        default:
            assert(0);
            return -1;
    }
}

int get_part_num_tb_in_pb(PART_SIZE pb_part_size, int pb_part_idx)
{
#pragma HLS INLINE off
    switch (pb_part_size) {
        case SIZE_2NxnU:
        case SIZE_nLx2N:
            return pb_part_idx == 0 ? 1 : 3;
        case SIZE_2NxnD:
        case SIZE_nRx2N:
            return pb_part_idx == 0 ? 3 : 1;
        case SIZE_2NxhN:
        case SIZE_hNx2N:
        case SIZE_2Nx2N:
            return 1;
        default:
            assert(0);
            return -1;
    }
}

int get_tb_idx_offset(PART_SIZE pb_part_size, int pb_part_idx)
{
    switch (pb_part_size) {
        case SIZE_2NxnU:
        case SIZE_nLx2N:
            assert(pb_part_idx <= 1);
            return pb_part_idx == 0 ? 0 : 1;
        case SIZE_2NxnD:
        case SIZE_nRx2N:
            assert(pb_part_idx <= 1);
            return pb_part_idx == 0 ? 0 : 3;
        case SIZE_2NxhN:
        case SIZE_hNx2N:
            assert(pb_part_idx <= 3);
            return pb_part_idx;
        case SIZE_2Nx2N:
            assert(pb_part_idx == 0);
            return 0;
        default:
            assert(0);
            return -1;
    }
}


//note: this function only works for DT intra
void get_tb_width_height_in_pb(int pb_w, int pb_h, PART_SIZE pb_part_size, int pb_part_idx, int *tb_w, int *tb_h)
{
#pragma HLS INLINE off
    switch (pb_part_size) {
        case SIZE_2NxnU:
            *tb_w = pb_w;
            *tb_h = pb_part_idx == 0 ? pb_h : pb_h / 3;
            break;
        case SIZE_2NxnD:
            *tb_w = pb_w;
            *tb_h = pb_part_idx == 1 ? pb_h : pb_h / 3;
            break;
        case SIZE_nLx2N:
            *tb_w = pb_part_idx == 0 ? pb_w : pb_w / 3;
            *tb_h = pb_h;
            break;
        case SIZE_nRx2N:
            *tb_w = pb_part_idx == 1 ? pb_w : pb_w / 3;
            *tb_h = pb_h;
            break;
        case SIZE_2NxhN:
        case SIZE_hNx2N:
        case SIZE_2Nx2N:
            *tb_w = pb_w;
            *tb_h = pb_h;
            break;
        default:
            assert(0);
            break;
    }
}

//note: this function only works for DT intra
void get_tb_pos_in_pb(int pb_x, int pb_y, PART_SIZE pb_part_size, int tb_w, int tb_h, int tb_part_idx, int *tb_x,
                      int *tb_y)
{
#pragma HLS INLINE off
    switch (pb_part_size) {
        case SIZE_2NxnU:
        case SIZE_2NxnD:
        case SIZE_2NxhN:
            *tb_x = pb_x;
            *tb_y = pb_y + tb_part_idx * tb_h;
            break;
        case SIZE_nLx2N:
        case SIZE_nRx2N:
        case SIZE_hNx2N:
            *tb_x = pb_x + tb_part_idx * tb_w;
            *tb_y = pb_y;
            break;
        case SIZE_2Nx2N:
            *tb_x = pb_x;
            *tb_y = pb_y;
            break;
        default:
            assert(0);
            break;
    }
}

PART_SIZE get_tb_part_size_by_pb(PART_SIZE pb_part, int pred_mode)
{
    PART_SIZE tb_part = SIZE_2Nx2N;

    switch (pb_part) {
        case SIZE_2Nx2N:
            tb_part = pred_mode == MODE_INTRA ? SIZE_2Nx2N : SIZE_NxN;
            break;
        case SIZE_2NxnU:
        case SIZE_2NxnD:
        case SIZE_2NxhN:
            tb_part = SIZE_2NxhN;
            break;
        case SIZE_nLx2N:
        case SIZE_nRx2N:
        case SIZE_hNx2N:
            tb_part = SIZE_hNx2N;
            break;
        case SIZE_NxN:
            assert(0);
            tb_part = SIZE_NxN;
            break;
        default:
            assert(0);
            break;
    }

    return tb_part;
}

void get_tb_width_height_log2(int log2_w, int log2_h, PART_SIZE part, int *log2_tb_w, int *log2_tb_h)
{
#pragma HLS INLINE off
    switch (part) {
        case SIZE_2Nx2N:
            break;
        case SIZE_NxN:
            log2_w--;
            log2_h--;
            break;
        case SIZE_2NxhN:
            log2_h -= 2;
            break;
        case SIZE_hNx2N:
            log2_w -= 2;
            break;
        default:
            assert(0);
            break;
    }

    *log2_tb_w = log2_w;
    *log2_tb_h = log2_h;
}

void get_tb_width_height(int w, int h, PART_SIZE part, int *tb_w, int *tb_h)
{
#pragma HLS INLINE off
    switch (part) {
        case SIZE_2Nx2N:
            break;
        case SIZE_NxN:
            w >>= 1;
            h >>= 1;
            break;
        case SIZE_2NxhN:
            h >>= 2;
            break;
        case SIZE_hNx2N:
            w >>= 2;
            break;
        default:
            assert(0);
            break;
    }

    *tb_w = w;
    *tb_h = h;
}

void get_tb_start_pos(int w, int h, PART_SIZE part, int idx, int *pos_x, int *pos_y)
{
#pragma HLS INLINE off
    int x = 0, y = 0;

    switch (part) {
        case SIZE_2Nx2N:
            x = y = 0;
            break;
        case SIZE_NxN:
            y = (idx / 2) * h / 2;
            x = (idx % 2) * w / 2;
            break;
        case SIZE_2NxhN:
            x = 0;
            y = idx * (h / 4);
            break;
        case SIZE_hNx2N:
            y = 0;
            x = idx * (w / 4);
            break;
        default:
            assert(0);
            break;
    }
    *pos_x = x;
    *pos_y = y;
}

int get_coef_offset_tb(int cu_x, int cu_y, int tb_x, int tb_y, int cu_w, int cu_h, int tb_part_size)
{
    int offset = 0;
    switch (tb_part_size) {
        case SIZE_2Nx2N:
            offset = 0;
            break;
        case SIZE_NxN:
            if (tb_x == cu_x && tb_y == cu_y) {
                offset = 0;
            } else if (tb_x > cu_x && tb_y == cu_y) {
                offset = (cu_w * cu_h) / 4;
            } else if (tb_x == cu_x && tb_y > cu_y) {
                offset = (cu_w * cu_h) / 2;
            } else {
                offset = (cu_w * cu_h * 3) / 4;
            }
            break;
        case SIZE_2NxhN:
            offset = (tb_y - cu_y) * cu_w;
            break;
        case SIZE_hNx2N:
            offset = (tb_x - cu_x) * cu_h;
            break;
        default:
            assert(0);
            break;
    }

    return offset;
}

int is_tb_avaliable(int log2_w, int log2_h, PART_SIZE pb_part_size, int pred_mode,
                    u8 position_based_transform_enable_flag)
{
    //intra: always infer TB part
    if (pred_mode == MODE_INTRA) {
        return 0;
    }

    //inter: signal for DT and PBT cases
    int avaliable = 0;
    if (position_based_transform_enable_flag && (pb_part_size == SIZE_2Nx2N && abs(log2_w - log2_h) <= 1 && log2_w <= 5
            && log2_w >= 3 && log2_h <= 5 && log2_h >= 3)) {
        avaliable = 0;
    }
    return avaliable;
}

//int is_cu_nz(int nz[MAX_NUM_TB][N_C])
//{
//    int cu_nz = 0;
//    int i, j;
//
//    for (i = 0; i < MAX_NUM_TB; i++) {
//#pragma HLS LOOP_TRIPCOUNT max=4
//        for (j = 0; j < N_C; j++) {
//#pragma HLS LOOP_TRIPCOUNT max=3
//            cu_nz |= nz[i][j];
//        }
//    }
//    return cu_nz ? 1 : 0;
//}

int is_cu_plane_nz(int nz[MAX_NUM_TB][N_C], int plane)
{
    int cu_nz = 0;
    int i;

    for (i = 0; i < MAX_NUM_TB; i++) {
#pragma HLS loop_tripcount max = 4
        cu_nz |= nz[i][plane];
    }
    return cu_nz ? 1 : 0;
}

void cu_plane_nz_cpy(int dst[MAX_NUM_TB][N_C], int src[MAX_NUM_TB][N_C], int plane)
{
    int i;

    for (i = 0; i < MAX_NUM_TB; i++) {
#pragma HLS loop_tripcount max = 4
        dst[i][plane] = src[i][plane];
    }
}

//void cu_plane_nz_cln(int dst[MAX_NUM_TB][N_C], int plane)
//{
//#pragma HLS INLINE off
//    int i;
//
//    for (i = 0; i < MAX_NUM_TB; i++) {
//#pragma HLS LOOP_TRIPCOUNT max=4
//        dst[i][plane] = 0;
//    }
//}

int is_cu_nz_equ(int dst[MAX_NUM_TB][N_C], int src[MAX_NUM_TB][N_C])
{
    int i, j;
    int equ = 1;

    for (i = 0; i < N_C; i++) {
#pragma HLS loop_tripcount max = 3
        for (j = 0; j < MAX_NUM_TB; j++) {
#pragma HLS loop_tripcount max = 4
            if (dst[j][i] != src[j][i]) {
                equ = 0;
                break;
            }
        }
    }
    return equ;
}


void check_set_tb_part(COM_MODE *mode)
{
    int res = is_cu_plane_nz(mode->num_nz, Y_C);
    if (!res) {
        mode->tb_part = SIZE_2Nx2N;
    }
}

void check_tb_part(COM_MODE *mode)
{
    int res = is_cu_plane_nz(mode->num_nz, Y_C);
    if (!res && mode->tb_part != SIZE_2Nx2N) {
        com_assert(0);
    }
}

void copy_rec_y_to_pic(pel* src, int x, int y, int w, int h, int stride, COM_PIC *pic)
{
#pragma HLS INLINE off
    pel* dst;
    int  j, s_pic;

    s_pic = pic->stride_luma;
    dst = pic->y + x + y * s_pic;
    for (j = 0; j < h; j++) {
#pragma HLS LOOP_TRIPCOUNT max=4
        memcpy_rewrintra2(dst, src,  w);
        src += stride;
        dst += s_pic;
    }
}


void* memcpy_re(void *dest, const void *src, size_t count)
{
    void *ret = dest;
    int offset = 0;
    while (count--) {
#pragma HLS loop_tripcount max = 32
        *((char *)dest + offset) = *((const char *)src + offset);
        offset++;
    }
    return ((char *)dest + offset - 1);
}

void* memset_re(void *dest, char c, size_t count)
{
    void *ret = dest;
    size_t offset = 0;
    while (count--) {
#pragma HLS loop_tripcount max = 32
        *((char *)dest + offset) = c;
        offset++;
    }

    return dest;
}

#if LIBVC_ON
void init_libvcdata(LibVCData* libvc_data)
{
    libvc_data->bits_dependencyFile = 0;
    libvc_data->bits_libpic = 0;

    libvc_data->library_picture_enable_flag = 0;
    libvc_data->is_libpic_processing = 0;
    libvc_data->is_libpic_prepared = 0;

    libvc_data->num_candidate_pic = 0;
    libvc_data->num_lib_pic = 0;
    libvc_data->num_RLpic = 0;
#if M5422
    libvc_data->last_libpic_index = -1;
    libvc_data->end_of_intra_period = 0;
#endif
    libvc_data->num_libpic_outside = 0;

    for (int i = 0; i < MAX_CANDIDATE_PIC; i++)
    {
        libvc_data->list_poc_of_candidate_pic[i] = -1;
        libvc_data->list_candidate_pic[i] = NULL;

        libvc_data->list_hist_feature_of_candidate_pic[i].num_component = 0;
        libvc_data->list_hist_feature_of_candidate_pic[i].num_of_hist_interval = 0;
        libvc_data->list_hist_feature_of_candidate_pic[i].length_of_interval = 0;
        for (int j = 0; j < MAX_NUM_COMPONENT; j++)
        {
            libvc_data->list_hist_feature_of_candidate_pic[i].list_hist_feature[j] = NULL;
        }

        libvc_data->list_poc_of_RLpic[i] = -1;
        libvc_data->list_libidx_for_RLpic[i] = -1;

    }
    for (int i = 0; i < MAX_NUM_LIBPIC; i++)
    {
        libvc_data->list_poc_of_libpic[i] = -1;
        libvc_data->list_libpic_outside[i] = NULL;
        libvc_data->list_library_index_outside[i] = -1;
    }
}

void delete_libvcdata(LibVCData* libvc_data)
{
    /* remove allocated picture and picture store buffer */
    for (int i = 0; i < MAX_NUM_LIBPIC; i++)
    {
        if (libvc_data->list_libpic_outside[i])
        {
            com_picbuf_free(libvc_data->list_libpic_outside[i]);
            libvc_data->list_libpic_outside[i] = NULL;
        }
    }

}

int get_libidx(LibVCData* libvc_data, int cur_ptr)
{
    int lib_idx;
    lib_idx = -1;
    for (int j = 0; j < libvc_data->num_RLpic; j++)
    {
        if (cur_ptr == libvc_data->list_poc_of_RLpic[j])
        {
            lib_idx = libvc_data->list_libidx_for_RLpic[j];
            break;
        }
    }

    return lib_idx;
}
#endif

