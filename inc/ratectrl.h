
#ifndef _COM_RC_H_
#define _COM_RC_H_
//#define RC_OPEN  
#define RC_TYPE_NULL 0
#define RC_TYPE_CRF  1
#define RC_TYPE_ABR  2
#define RC_TYPE_CBR  3


#define MV_RANGE_MIN           0
#define MV_RANGE_MAX           1
#define MV_RANGE_DIM           2
#include "com_def.h"
#include "com_define.h"

typedef struct
{
    float coeff_min;
    float coeff;
    float count;
    float decay;
    float offset;
} predictor_t;

typedef struct enc_inter_data_t {
    /*** sequence level ***/
    int bit_depth;
    int gop_size;                       /* gop size           */
    int max_search_range;
    int is_padding;
    s16 min_mv[MV_D];                   /* min mv relative to current coordinate */
    s16 max_mv[MV_D];                   /* max mv relative to current coordinate */
    s16 max_coord[MV_D];                /* max coordinate */
    int subpel_cost_type;

    /*** picture level ***/
    s64  ptr;                           /* current frame numbser */
    COM_PIC *ref_pic;

    /*** local data ***/
    u8   num_refp;
    u8   curr_mvr;
    pel *org;
    int  i_org;
    u32  lambda_mv;

    s32  mot_bits         [REFP_NUM];
    s16  mvp_scale        [REFP_NUM][MAX_NUM_ACTIVE_REF_FRAME][MV_D];
    s16  mv_scale         [REFP_NUM][MAX_NUM_ACTIVE_REF_FRAME][MV_D];
    s16  mv_ipel          [REFP_NUM][MAX_NUM_ACTIVE_REF_FRAME][MV_D];

    u32  hpel_satd        [REFP_NUM][MAX_NUM_ACTIVE_REF_FRAME][9];
    s16  hpel_start_mv    [REFP_NUM][MAX_NUM_ACTIVE_REF_FRAME][MV_D];
    u32  qpel_satd        [REFP_NUM][MAX_NUM_ACTIVE_REF_FRAME][9];
    s16  qpel_start_mv    [REFP_NUM][MAX_NUM_ACTIVE_REF_FRAME][MV_D];

    CPMV affine_mvp_scale [REFP_NUM][MAX_NUM_ACTIVE_REF_FRAME][VER_NUM][MV_D];
    CPMV affine_mv_scale  [REFP_NUM][MAX_NUM_ACTIVE_REF_FRAME][VER_NUM][MV_D];
    int  best_mv_uni      [REFP_NUM][MAX_NUM_ACTIVE_REF_FRAME][MV_D];

    u8  *tab_mvbits;
    int  tab_mvbits_offset;

    int adaptive_raster_range;
} inter_search_t;


typedef struct enc_rc_t
{
    /* Sequence level const data */
    int       type;
    int       low_delay;
    double    target_bitrate;
    double    max_bitrate;
    double    frame_rate;
    int       frame_pixels;
    double    rfConstant;
    int       win_size;
    int       min_qp;
    int       max_qp;

    /* Sequence level global data */
    double    total_factor;
    long long total_subgops;
    long long total_bits;
    long long total_frms;

    /* Sequence level local data */
    double    shortTermCplxSum;
    double    shortTermCplxCount;

    int      *win_bits_list;
    int       win_bits;
    int       win_idx;
    int       win_frames;

    /* Sub-GOP level */
    int       subgop_frms;
    long long subgop_bits;
    double    subgop_cplx;   // we use top-level frame's cplx   as subgop_cplx
    double    subgop_qscale; // we use top-level frame's qscale as subgop_qscale

    /* VBV stuff */
    int b_vbv;
    double buffer_size;
    long  buffer_fill_final;
    long  buffer_fill_final_min;
    double buffer_fill;         /* planned buffer, if all in-progress frames hit their bit budget */
    double buffer_rate;         /* # of bits added to buffer_fill after each frame */
    double vbv_max_rate;        /* # of bits added to buffer_fill per second */
    predictor_t *pred;
    int single_frame_vbv;
    float rate_factor_max_increment; /* Don't allow RF above (CRF + this value). */
    int            last_satd;
    int last_non_b_pict_type;
    double frame_size_maximum;
    int b_vbv_min_rate;
    int pic_type;
    double qp;
    int i_mb_count;
    int i_period;
} enc_rc_t;

typedef struct enc_cfg_t
{
 int            pic_width;
 int            pic_height;
 int            fps_num;
 int            fps_den;
 int            fps;
//#======== Coding Structure =======================
 int            i_period;
 int            max_b_frames;
 int            close_gop;
 int            scenecut;
 int            scenecut_histogram;
 int            adaptive_gop;
 int            lookahead;

//#========== Rate Control =========================
 int            rc_type;
 int            rc_bitrate;
 int            rc_max_bitrate;
 int            rc_min_qp;
 int            rc_max_qp;
 int            rc_crf;
 int            rc_vbv_buf_size;

 int            qp;
 int            qp_offset_cb;
 int            qp_offset_cr;
}enc_cfg_t;


void rc_init(enc_rc_t* p, enc_cfg_t *param);
void rc_update(enc_rc_t *p, COM_PIC *pic, char *ext_info, int info_buf_size);
int  rc_get_qp(enc_rc_t *p, COM_PIC *pic, int qp_l0, int qp_l1);
void rc_destroy(enc_rc_t *p);
void update_vbv_plan(enc_rc_t *p, int overhead);
#endif /* _COM_RC_H_ */
