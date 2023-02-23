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

#include <math.h>
#include <stdio.h>
#include "ratectrl.h"
#define FRAM_DEPTH_0 0
#define FRAM_DEPTH_1 1
/* input/output qp is for bit_depth of 8 */

#define SHIFT_QP 11
#define QCompress 0.6
#define CRF_DEFAULT_C 15.0
typedef struct uavs3e_enc_aqp_param_t {
    int qp_offset_layer;
    double qp_offset_model_offset;
    double qp_offset_model_scale;
} enc_aqp_param_t;


/*! macro to determine maximum */
#define COM_MAX(a,b)                   (((a) > (b)) ? (a) : (b))
/*! macro to determine minimum */
#define COM_MIN(a,b)                   (((a) < (b)) ? (a) : (b))
#define COM_CLIP3(min_x, max_x, value)   COM_MAX((min_x), COM_MIN((max_x), (value)))

#if defined(_MSC_VER)
#define COM_LOG2F(x) (logf((float)(x)) * 1.44269504088896405f)
#define COM_LOG2(x) (log((double)(x)) * 1.4426950408889640513713538072172)
#else
#define COM_LOG2F(x) log2f(x)
#define COM_LOG2(x)  log2(x)
#endif
double enc_get_hgop_qp(double base_qp, int frm_depth, int is_ld)
{
    static const enc_aqp_param_t tbl_qp_adapt_param_ra[8] = {
        { -4,  0,      0 },
        {  1,  0,      0 },
        {  1, -4.0604, 0.154575 },
        {  5, -4.8332, 0.17145 },
        {  7, -4.9668, 0.174975 },
        {  8, -5.9444, 0.225 },
        {  9, -5.9444, 0.225 },
        { 10, -5.9444, 0.225 }
    };
    static const enc_aqp_param_t tbl_qp_adapt_param_ld[8] = {
        { -1,  0.0000, 0.0000},
        { 1,  0.0000, 0.0000},
        { 4, -6.5000, 0.2590},
        { 5, -6.5000, 0.2590},
        { 6, -6.5000, 0.2590},
        { 7, -6.5000, 0.2590},
        { 8, -6.5000, 0.2590},
        { 9, -6.5000, 0.2590},
    };

    const enc_aqp_param_t *qp_adapt_param = is_ld ? tbl_qp_adapt_param_ld : tbl_qp_adapt_param_ra;
    base_qp += qp_adapt_param[frm_depth].qp_offset_layer;

    double dqp_offset = COM_MAX(0, base_qp) * qp_adapt_param[frm_depth].qp_offset_model_scale + qp_adapt_param[frm_depth].qp_offset_model_offset;
    return base_qp + COM_CLIP3(0.0, 4.0, dqp_offset);
}


static __inline double qScale2qp(double qScale)
{
    return 13.131 + 5.661 * COM_LOG2(qScale);
}

static __inline double uavs3e_qp2qScale(double qp)
{
    return pow(2.0, (qp - 13.131) / 5.661);
}

void rc_init(enc_rc_t* p, enc_cfg_t *param)
{
    memset(p, 0, sizeof(enc_rc_t));
    printf("rc init \n");
    /* const data */
    p->type = RC_TYPE_ABR;//param->rc_type;
    p->rfConstant     = pow(CRF_DEFAULT_C, 1 - QCompress) / uavs3e_qp2qScale(param->rc_crf);
    p->target_bitrate = param->rc_bitrate * 1000.0;
    p->max_bitrate    = param->rc_max_bitrate * 1000.0;
    if (p->max_bitrate && p->max_bitrate < p->target_bitrate) {
        p->max_bitrate = p->target_bitrate;
    }
    
    p->pred = (predictor_t*)com_malloc_fast(5 * sizeof(predictor_t) * 1);
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 1; j++)
        {
            p->pred[i + j * 5].coeff_min = 1 / 2;
            p->pred[i + j * 5].coeff = 1;
            p->pred[i + j * 5].count = 1.0;
            p->pred[i + j * 5].decay = 0.5;
            p->pred[i + j * 5].offset = 0.0;
        }
    }
    //p->frame_rate     = param->fps_num * 1.0 / param->fps_den;
    p->frame_rate     = param->fps;
    p->i_period = param->i_period;
    p->buffer_size    = param->rc_vbv_buf_size * 1000.0;
    p->frame_pixels   = param->pic_width * param->pic_height;
    p->min_qp         = param->rc_min_qp;
    p->max_qp         = param->rc_max_qp;
    p->low_delay      = !param->max_b_frames;
    p->win_size       = ((int)(p->frame_rate + 0.5) + param->max_b_frames) / (param->max_b_frames + 1) * (param->max_b_frames + 1);

    p->total_subgops  = p->low_delay ? -1 : -2;
    p->win_idx        = 0;
    p->win_bits       = 0;
    p->win_frames     = 0;
    p->win_bits_list  = (int*)com_malloc(sizeof(int) * p->win_size);
    memset(p->win_bits_list, 0, sizeof(int) * p->win_size);
    p->shortTermCplxSum = CRF_DEFAULT_C;
    p->shortTermCplxCount = 1;

    p->buffer_fill_final =
    p->buffer_fill_final_min = p->buffer_size * 0.9 * p->frame_rate * 2;
    p->buffer_rate = param->rc_max_bitrate * 1000.0 / p->frame_rate;
    p->single_frame_vbv = p->buffer_rate * 1.1 > p->buffer_size;
    p->vbv_max_rate = param->rc_max_bitrate * 1000.0;
    p->b_vbv_min_rate = p->type == RC_TYPE_ABR
        && param->rc_max_bitrate <= param->rc_bitrate;
    p->i_mb_count = p->frame_pixels / 256;
    if (param->rc_max_bitrate > 0 && param->rc_vbv_buf_size > 0)
        p->b_vbv = 1;
}

void rc_destroy(enc_rc_t *p)
{
    if (p->win_bits_list) {
        com_mfree(p->win_bits_list);
    }
   
}

#define X264_MAX(a,b) ( (a)>(b) ? (a) : (b) )
static  double x264_clip3f(double v, double f_min, double f_max)
{
    return ((v < f_min) ? f_min : (v > f_max) ? f_max : v);
}
static float predict_size(predictor_t *p, float q, float var)
{
    return (p->coeff*var + p->offset) / (q*p->count);
}

static void update_predictor(predictor_t *p, float q, float var, float bits)
{
    float range = 1.5;
    if (var < 10)
        return;
    float old_coeff = p->coeff / p->count;
    float old_offset = p->offset / p->count;
    float new_coeff = X264_MAX((bits*q - old_offset) / var, p->coeff_min);
    float new_coeff_clipped = x264_clip3f(new_coeff, old_coeff / range, old_coeff*range);
    float new_offset = bits*q - new_coeff_clipped * var;
    if (new_offset >= 0)
        new_coeff = new_coeff_clipped;
    else
        new_offset = 0;
    p->count *= p->decay;
    p->coeff *= p->decay;
    p->offset *= p->decay;
    p->count++;
    p->coeff += new_coeff;
    p->offset += new_offset;
}
// update VBV after encoding a frame
static int update_vbv(enc_rc_t *p, int bits)
{
    int filler = 0;
    int bitrate = p->target_bitrate; 
   
    long buffer_size = p->buffer_size *  p->frame_rate * 2;

    if (p->last_satd*p->frame_pixels >= p->i_mb_count)
        update_predictor(&p->pred[p->pic_type], uavs3e_qp2qScale(p->qp), p->last_satd*p->frame_pixels, bits);
    long buffer_diff = (long)bits * p->frame_rate * 2;
    p->buffer_fill_final -= buffer_diff;
    p->buffer_fill_final_min -= buffer_diff;

    if (p->buffer_fill_final_min < 0)
    {
        double underflow = (double)p->buffer_fill_final_min / (p->frame_rate * 2);
        
        printf("VBV underflow  \n");
        p->buffer_fill_final =
        p->buffer_fill_final_min = 0;
    }

   
    buffer_diff = (long)bitrate  * 2;
    p->buffer_fill_final += buffer_diff;
    p->buffer_fill_final_min += buffer_diff;

    if (p->buffer_fill_final > buffer_size)
    {
        
#define X264_MIN(a,b) ( (a)<(b) ? (a) : (b) )

        p->buffer_fill_final = X264_MIN(p->buffer_fill_final, buffer_size);
        p->buffer_fill_final_min = X264_MIN(p->buffer_fill_final_min, buffer_size);
       
    }

    return filler;
}

 void update_vbv_plan(enc_rc_t *p, int overhead)
{
   
    p->buffer_fill = p->buffer_fill_final_min / (p->frame_rate * 2);
    
    p->buffer_fill = X264_MIN(p->buffer_fill, p->buffer_size);
    p->buffer_fill -= overhead;
}


// apply VBV constraints and clip qscale to between lmin and lmax
static double clip_qscale(enc_rc_t *p, int pict_type, double q)
{
   
    double lmin = uavs3e_qp2qScale(p->min_qp);
    double lmax = uavs3e_qp2qScale(p->max_qp);
   
    double q0 = q;
    p->pic_type = pict_type;
    /* B-frames are not directly subject to VBV,
    * since they are controlled by the P-frames' QPs. */

    if (p->b_vbv && p->last_satd > 0)
    {
        double fenc_cpb_duration = (double)2 / p->frame_rate ;
        /* Lookahead VBV: raise the quantizer as necessary such that no frames in
        * the lookahead overflow and such that the buffer is in a reasonable state
        * by the end of the lookahead. */
       
        /* Fallback to old purely-reactive algorithm: no lookahead. */
      
            if ((pict_type == SLICE_P ||
                (pict_type == SLICE_I && p->last_non_b_pict_type == SLICE_I)) &&
                p->buffer_fill / p->buffer_size < 0.5)
            {
                printf("last i\n");
                q /= x264_clip3f(2.0*p->buffer_fill / p->buffer_size, 0.5, 1.0);
            }

            /* Now a hard threshold to make sure the frame fits in VBV.
            * This one is mostly for I-frames. */
            double bits = predict_size(&p->pred[pict_type], q, p->last_satd*p->frame_pixels);
            /* For small VBVs, allow the frame to use up the entire VBV. */
            double max_fill_factor = p->buffer_size >= 5 * p->max_bitrate / p->frame_rate ? 2 : 1;
            /* For single-frame VBVs, request that the frame use up the entire VBV. */
            double min_fill_factor = p->single_frame_vbv ? 1 : 2;

            if (bits > p->buffer_fill / max_fill_factor)
            {
                double qf = x264_clip3f(p->buffer_fill / (max_fill_factor*bits), 0.2, 1.0);
                q /= qf;
                bits *= qf;
            }
            if (bits < p->buffer_rate / min_fill_factor)
            {
                double qf = x264_clip3f(bits*min_fill_factor / p->buffer_rate, 0.001, 1.0);
                q *= qf;
            }
            q = X264_MAX(q0, q);
        

        /* Check B-frame complexity, and use up any bits that would
        * overflow before the next P-frame. */
            if (pict_type == SLICE_P && !p->single_frame_vbv)
        {
            int nb = 0;
            double bits = predict_size(&p->pred[pict_type], q, p->last_satd*p->frame_pixels);
            double pbbits = bits;
            double space;
            double minigop_cpb_duration;
       
            minigop_cpb_duration =  fenc_cpb_duration;
            space = p->buffer_fill + minigop_cpb_duration*p->vbv_max_rate - p->buffer_size;
            if (pbbits < space)
            {
                q *= X264_MAX(pbbits / space, bits / (0.5 * p->buffer_size));
                
            }
            
            q = X264_MAX(q0 / 2, q);
        }


        if (!p->b_vbv_min_rate)
            q = X264_MAX(q0, q);
    }
    p->last_non_b_pict_type = pict_type;
    if (lmin == lmax)
        return lmin;
    
    return x264_clip3f(q, lmin, lmax);
}

void rc_update(enc_rc_t *p, COM_PIC *pic, char *ext_info, int info_buf_size)
{   
    int layer_id = pic->temporal_id;


    if (layer_id <= 1) {
        if (p->total_subgops >= 0) {
            p->total_factor += (p->subgop_bits / p->subgop_frms) * p->subgop_qscale / p->subgop_cplx;
        }
        p->total_subgops++;
        p->subgop_frms = 0;
        p->subgop_bits = 0;
        p->subgop_cplx = pow(pic->picture_satd, 1 - QCompress);
        p->subgop_qscale = uavs3e_qp2qScale(pic->picture_qp_real);
    }
    p->subgop_frms++;
    p->subgop_bits += pic->picture_bits;
   
    p->total_frms++;
    p->total_bits += pic->picture_bits;

    p->win_bits  -= p->win_bits_list[p->win_idx];
    p->win_bits  += pic->picture_bits;

    p->win_bits_list[p->win_idx] = pic->picture_bits;
    p->win_idx = (p->win_idx + 1) % p->win_size;
    update_vbv(p, pic->picture_bits);
    p->win_frames = COM_MIN(p->win_frames + 1, p->win_size);
    printf( "brTal:%9.2f \n",  p->total_bits / (p->total_frms / p->frame_rate) / 1000);
    //if (p->win_frames == p->win_size) {
        printf(" brCur: %9.2f \n", p->win_bits * p->frame_rate / p->win_frames / 1000);
    //}
   // printf(" brCur: %9.2f \n", pic->picture_bits / 1000);
    if (ext_info) {
        sprintf(ext_info, "layer:%d cost:%5.2f brTal:%9.2fkbps", pic->temporal_id, pic->picture_satd, p->total_bits / (p->total_frms / p->frame_rate) / 1000);

        if (p->total_subgops > 0) {
            sprintf(ext_info, "%s factor:%9.2f", ext_info, p->total_factor / p->total_subgops);
        }
        if (p->win_frames == p->win_size) {
            sprintf(ext_info, "%s brCur:%9.2fkbps", ext_info, p->win_bits * p->frame_rate / p->win_frames / 1000);
            printf(" brCur: %9.2f \n", p->win_bits * p->frame_rate / p->win_frames / 1000);
        }
    }
}
double fix_qscale(enc_rc_t *p,double qScale )
{
    int pred_size = 0;
    int count = 1000;
    int add = 0;
    int sub = 0;
    while (count--)
    {

        pred_size = predict_size(&p->pred[p->pic_type], qScale, p->last_satd*p->frame_pixels);
        if (pred_size > p->buffer_rate)
        {
            qScale = qScale + 1;
            add = 1;
        }
        if (pred_size < p->buffer_rate)
        {
            qScale = qScale - 1;
            sub = 1;
        }
        if (add && sub)
            break;
    }
    return qScale;
}
/*************************************************************************************************/
//
//  CRF:  baseCplx^(1-QCompress)/uavs3e_qp2qScale(crf) == currCplx^(1-QCompress)/uavs3e_qp2qScale(qp) 
//
//  ABR:  Bits * qScale / (complex^(1 - QCompress))= C, calculate factor C through historical data
//
/*************************************************************************************************/
int rc_get_qp(enc_rc_t *p,  COM_PIC *pic, int qp_l0, int qp_l1)
{
    double qp;
    double min_qp = p->min_qp;
    double max_qp = p->max_qp;
    int layer_id = pic->lay_id;
   // long long ptr = pic->img->ptr;
    int ai_mode = (p->i_period == 1);
    int pic_type = layer_id + 1;
    long long ptr = 0;
    if (layer_id > FRAM_DEPTH_1) {
        com_assert(qp_l0 >= 0);

        if (p->low_delay) {
            qp = enc_get_hgop_qp(qp_l0, layer_id, 1);
        } else {
            int weighted_qp = qp_l0;

            if (qp_l1 > 0) {
                weighted_qp = (qp_l0 + qp_l1 * 3) / 4;
            }
            qp = enc_get_hgop_qp(weighted_qp, layer_id, 0);
        }
        return (int)(COM_CLIP3(min_qp, max_qp, (qp + 0.5)));
    }

    /*** frames in top layer ***/

    if ((layer_id + ai_mode) == FRAM_DEPTH_1) {
        p->shortTermCplxSum *= 0.5;
        p->shortTermCplxCount *= 0.5;
        p->shortTermCplxSum += pic->picture_satd;
        p->shortTermCplxCount++;
    }

    double blurredComplexity = pow(p->shortTermCplxSum / p->shortTermCplxCount, 1 - QCompress);

   // if (p->max_bitrate != 0) {
    if (0) {
        int sub_win = 16;

        if (p->win_frames == p->win_size) { // calculate min_qp
            int shift_out_bits = 0;
            int idx = p->win_idx;

            for (int i = 0; i < sub_win; i++) {
                shift_out_bits += p->win_bits_list[idx];
                idx = COM_MIN(idx + 1, p->win_size);
            }
            double max_bits = p->max_bitrate / p->frame_rate * p->win_size - p->win_bits + shift_out_bits;
            double frame_bits = COM_MAX(max_bits / sub_win, p->target_bitrate / p->frame_rate / 2);
            double qScale = blurredComplexity * (p->total_factor / p->total_subgops) / frame_bits;

            min_qp = qScale2qp(qScale) - (layer_id == 0 ? 3 : 0);
            min_qp = COM_MIN(min_qp, max_qp);
        }
    }

    if (p->type == RC_TYPE_CRF) {
        qp = qScale2qp(blurredComplexity / p->rfConstant);
        if (layer_id == FRAM_DEPTH_0) {
            qp -= 1;
        }
    } else if(p->type == RC_TYPE_ABR) {
        if (p->total_factor == 0) {
            if (layer_id == FRAM_DEPTH_0) { // first I frame, r-lambda model
                double cpp = pow(pic->picture_satd, 1.31);
                double bpp = p->target_bitrate / p->frame_rate / p->frame_pixels;
                double alpha = bpp < 0.025 ? 0.25 : 0.3;
                double ratio = 0.7 * alpha* pow(pow(cpp, 1.0 / 1.31) * 4.0 / bpp, 0.5582);
                double lambda = 6.7542 / 256 * pow(bpp * ratio / cpp, -1.7860);
                qp = (int)(5.661 * log(lambda) + 13.131 + 0.5);
                double qScale = uavs3e_qp2qScale(qp);
                qScale = clip_qscale(p, pic_type, qScale);
                if (p->single_frame_vbv)
                    qScale = fix_qscale(p, qScale);
                qp = qScale2qp(qScale);
            } else {
                com_assert(qp_l0 > 0);
                qp = enc_get_hgop_qp(qp_l0, layer_id, p->low_delay);
                double qScale = uavs3e_qp2qScale(qp);
                qScale = clip_qscale(p, pic_type, qScale);
                if (p->single_frame_vbv)
                    qScale = fix_qscale(p, qScale);
                qp = qScale2qp(qScale);
            }
        } else {
            if (!p->b_vbv_min_rate && p->last_satd)
            {
                double timeDone = p->total_frms / p->frame_rate;
                double abrBuffer = p->target_bitrate * 2 * COM_MAX(sqrt(timeDone), 1);
                double err_bits = p->total_bits - p->target_bitrate * timeDone;
                double frame_bits = p->target_bitrate / p->frame_rate / (COM_CLIP3(0.5, 2.0, (abrBuffer + err_bits) / abrBuffer));

                double qScale = blurredComplexity * (p->total_factor / p->total_subgops) / frame_bits;
                qScale = clip_qscale(p, pic_type, qScale);
                if (p->single_frame_vbv)
                    qScale = fix_qscale(p,qScale);
               
                qp = qScale2qp(qScale);
            }
            else
            {
                double frame_bits = p->target_bitrate / p->frame_rate;

                double qScale = blurredComplexity * (p->total_factor / p->total_subgops) / frame_bits;
                qScale = clip_qscale(p, pic_type, qScale);
                if (p->single_frame_vbv)
                    qScale = fix_qscale(p, qScale);
                
                qp = qScale2qp(qScale);
               
                  
            }
        }
       
    }
    if (layer_id == FRAM_DEPTH_0 && !p->single_frame_vbv&& !ai_mode) {
        qp -= 3;
    }
    
    return (int)(COM_CLIP3(min_qp, max_qp, (qp + 0.5)));
}

