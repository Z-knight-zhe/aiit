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

#ifndef _COM_UTIL_H_
#define _COM_UTIL_H_

//#ifdef __cplusplus
//extern "C"
//{
//#endif

#include "com_def.h"
#include <stdlib.h>
#include "type_def.h"

/*! macro to determine maximum */
#define COM_MAX(a,b)                   (((int)(a) > (int)(b)) ? (int)(a) : (int)(b))

/*! macro to determine minimum */
#define COM_MIN(a,b)                   (((int)(a) < (int)(b)) ? (int)(a) : (int)(b))

/*! macro to absolute a value */
#define COM_ABS(a)                     abs(a)

/*! macro to absolute a 64-bit value */
#define COM_ABS64(a)                   (((a)^((a)>>63)) - ((a)>>63))

/*! macro to absolute a 32-bit value */
#define COM_ABS32(a)                   (((a)^((a)>>31)) - ((a)>>31))

/*! macro to absolute a 16-bit value */
#define COM_ABS16(a)                   (((a)^((a)>>15)) - ((a)>>15))

/*! macro to clipping within avs3_min and avs3_max */
#define COM_CLIP3(min_x, max_x, value)   COM_MAX((min_x), COM_MIN((max_x), (value)))

/*! macro to clipping within avs3_min and avs3_max */
#define COM_CLIP(n,avs3_min,avs3_max)            (((n)>(avs3_max))? (int)(avs3_max) : (((n)<(avs3_min))? (int)(avs3_min) : (int)(n)))

#define COM_SIGN(x)                    (((x) < 0) ? -1 : 1)

/*! macro to get a sign from a 16-bit value.\n
operation: if(val < 0) return 1, else return 0 */
#define COM_SIGN_GET(val)              ((val<0)? 1: 0)

/*! macro to set sign into a value.\n
operation: if(sign == 0) return val, else if(sign == 1) return -val */
#define COM_SIGN_SET(val, sign)        ((sign)? -val : val)

/*! macro to get a sign from a 16-bit value.\n
operation: if(val < 0) return 1, else return 0 */
#define COM_SIGN_GET16(val)            (((val)>>15) & 1)

/*! macro to set sign into a 16-bit value.\n
operation: if(sign == 0) return val, else if(sign == 1) return -val */
#define COM_SIGN_SET16(val, sign)      (((val) ^ ((s16)((sign)<<15)>>15)) + (sign))

#define COM_ALIGN(val, align)          ((((val) + (align) - 1) / (align)) * (align))


typedef struct _COM_MOTION
{
    s32 mv[REFP_NUM][MV_D];
    s32 ref_idx[REFP_NUM];
} COM_MOTION;

typedef struct _NEB_CU
{
	int x_scu_of_ctu;
	int y_scu_of_ctu;
	int valid_flag;
	//SKIP_MERGE_MAP_SCU map;
} NEB_CU;

#define SAME_MV(MV0, MV1) ((MV0[MV_X] == MV1[MV_X]) && (MV0[MV_Y] == MV1[MV_Y]))
COM_PIC * com_pic_alloc(PICBUF_ALLOCATOR * pa, int * ret);
void com_pic_free(PICBUF_ALLOCATOR *pa, COM_PIC *pic);
COM_PIC* com_picbuf_alloc(int w, int h, int pad_l, int pad_c, int *err);
void com_picbuf_free(COM_PIC *pic);
void com_picbuf_expand(COM_PIC *pic, int exp_l, int exp_c);

#define SPLIT_MAX_PART_COUNT  4


typedef struct _COM_SPLIT_STRUCT
{
    U3       part_count;
    U4       cud;
    U8      width[SPLIT_MAX_PART_COUNT];
    U8      height[SPLIT_MAX_PART_COUNT];
    U3       log_cuw[SPLIT_MAX_PART_COUNT];
    U3       log_cuh[SPLIT_MAX_PART_COUNT];
    U14      x_pos[SPLIT_MAX_PART_COUNT];
    U14      y_pos[SPLIT_MAX_PART_COUNT];
    U12       cup[SPLIT_MAX_PART_COUNT];
	U1      split_allow[SPLIT_MAX_PART_COUNT][NUM_SPLIT_MODE];
	U3      num_split[SPLIT_MAX_PART_COUNT];
} COM_SPLIT_STRUCT;


int com_scan_tbl_init();
int com_scan_tbl_delete();
//void init_scan(u16 *scan, int size_x, int size_y, int scan_type);



//void com_mv_rounding_s32(s32 hor, s32 ver, s32 * rounded_hor, s32 * rounded_ver, int right_shift, int left_shift);
//void com_mv_rounding_s16(s32 hor, s32 ver, s16 * rounded_hor, s16 * rounded_ver, int right_shift, int left_shift);



/*void com_get_affine_mvp_scaling_line(ENC_ME_LINE_MAP* me_line_ptr, COM_MODE * mod_info_curr, int refp_ptr[MAX_NUM_REF_PICS][REFP_NUM], int ptr, int lidx, \
	CPMV mvp[VER_NUM][MV_D]
#if BD_AFFINE_AMVR
	, u8 curr_mvr
#endif
	,CTU_Level_Parameters* fetch_output_ptr);

//void check_affine_mvp_availability(COM_INFO *info, COM_MODE * mod_info_curr, COM_REFP refp[MAX_NUM_REF_PICS][REFP_NUM], COM_MAP *pic_map, int neb_addr_lt[AFFINE_MAX_NUM_LT], int valid_flag_lt[AFFINE_MAX_NUM_LT], int neb_addr_rt[AFFINE_MAX_NUM_RT], int valid_flag_rt[AFFINE_MAX_NUM_RT]);
void com_get_affine_mvp_re(COM_MODE * mod_info_curr, COM_REFP refp[MAX_NUM_REF_PICS][REFP_NUM], int ptr, int lidx, \
	CPMV mvp[VER_NUM][MV_D], int vertex_num
#if BD_AFFINE_AMVR
	, u8 curr_mvr
#endif
	, int neb_addr_lt[AFFINE_MAX_NUM_LT], int valid_flag_lt[AFFINE_MAX_NUM_LT], int neb_addr_rt[AFFINE_MAX_NUM_RT], int valid_flag_rt[AFFINE_MAX_NUM_RT],
	s16(*chosen_mv_lt)[REFP_NUM][MV_D], s8(*chosen_refi_lt)[REFP_NUM], s16(*chosen_mv_rt)[REFP_NUM][MV_D], s8(*chosen_refi_rt)[REFP_NUM]);


int com_get_affine_memory_access(CPMV mv[VER_NUM][MV_D], int cu_width, int cu_height);
*/

#if UMVE_ENH 
void derive_umve_final_motions(int umve_idx, COM_REFP(*refp)[REFP_NUM], int cur_poc, s16 umve_base_pmv[UMVE_BASE_NUM][REFP_NUM][MV_D], s8 umve_base_refi[UMVE_BASE_NUM][REFP_NUM], s16 umve_final_pmv[UMVE_BASE_NUM * UMVE_MAX_REFINE_NUM_SEC_SET][REFP_NUM][MV_D], s8 umve_final_refi[UMVE_BASE_NUM * UMVE_MAX_REFINE_NUM_SEC_SET][REFP_NUM], BOOL isUMVESecSet);
#else
void derive_umve_final_motions(int umve_idx, COM_REFP(*refp)[REFP_NUM], int cur_poc, s16 umve_base_pmv[UMVE_BASE_NUM][REFP_NUM][MV_D], s8 umve_base_refi[UMVE_BASE_NUM][REFP_NUM], s16 umve_final_pmv[UMVE_BASE_NUM * UMVE_MAX_REFINE_NUM][REFP_NUM][MV_D], s8 umve_final_refi[UMVE_BASE_NUM * UMVE_MAX_REFINE_NUM][REFP_NUM]);
#endif

/* MD5 structure */
typedef struct _COM_MD5
{
    u32     h[4]; /* hash state ABCD */
    u8      msg[64]; /*input buffer (chunk message) */
    u32     bits[2]; /* number of bits, modulo 2^64 (lsb first)*/
} COM_MD5;

/* MD5 Functions */
void com_md5_init(COM_MD5 * md5);
void com_md5_update(COM_MD5 * md5, void * buf, u32 len);
void com_md5_update_16(COM_MD5 * md5, void * buf, u32 len);
void com_md5_finish(COM_MD5 * md5, u8 digest[16]);
int com_md5_imgb(COM_IMGB * imgb, u8 digest[16]);

int com_picbuf_signature(COM_PIC * pic, u8 * md5_out);

int com_atomic_inc(volatile int * pcnt);
int com_atomic_dec(volatile int * pcnt);

//#ifdef __cplusplus
//}
//#endif

void com_sbac_ctx_init(COM_SBAC_CTX *sbac_ctx);
#if LIBVC_ON
void init_libvcdata(LibVCData* libvc_data);
void delete_libvcdata(LibVCData* libvc_data);
int get_libidx(LibVCData* libvc_data, int cur_ptr);
#endif
#endif /* _COM_UTIL_H_ */
