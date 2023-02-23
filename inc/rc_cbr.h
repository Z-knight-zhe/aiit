#ifndef _RC_CBR_H_
#define _RC_CBR_H_
#include "uratectrl.h"
#include "rate_control.h"
#include "enc_def.h"
 
//#include<iostream>
//using namespace std;

//extern bool RC_DEBUG;

#define g_RCMaxPicListSize 40
#define COST_BETA_I 1.33
#define MIN_INTER_CPP 0.1

//const int g_RCInvalidQPValue = -999;
//const double g_RCAlphaMinValue = 0.01;
//const double g_RCAlphaMaxValue = 500.0;
//const double g_RCBetaMinValue = -3.0;
//const double g_RCBetaMaxValue = -0.2;

typedef struct coded_pic_info {
    int frameLevel;
    double lambda;
} coded_pic_info;

typedef struct rlambda_param
{
    double m_alpha;
    double m_beta;
} rlambda_param;

typedef struct rc_rlambda_ctrl
{
    /* Sequence level const value */
    //cfg_param_t *enc_cfg;
    ENC_PARAM *enc_cfg;

    double m_seq_tar_bit_rate;
    double m_seq_alpha_ratio;
    double m_seq_beta_ratio;
    double m_seq_pic_pixels;
    double m_seq_avg_bits;
    double m_seq_frm_rate;
    int    m_seq_smooth_window;
    int    m_seq_gopid_2_level[8];

    /* Sequence level */
    coded_pic_info m_seq_coded_frm_list[g_RCMaxPicListSize + 1];
    int m_seq_coded_frm_idx;
    int m_seq_coded_frm_list_size;
    rlambda_param m_seq_pic_params[5];
    rlambda_param m_seq_pic_params_old[5];
    double       m_seq_bits_error;
    double       m_seq_last_lambda;

    /* GOP level info data */
    int m_gop_coded_bits;
    int m_gop_coded_frms;

    /* sub-GOP level */
    double m_sub_gop_pic_bits[8];
    int m_sub_gop_pic;
    int m_sub_gop_pic_left;
    double m_sub_gop_bits_left;
    int m_scene_cut_flag;
} rc_rlambda_ctrl;

class RC_CBR :public Rate_Control
{

    int target_bitrate;
    rc_rlambda_ctrl rc_ctrl;
    
public:
    RC_CBR(ENC_PARAM *input, int target_bitrate0){
        rc_init(input, target_bitrate0);
    }
    ~RC_CBR(){

    }

    void rc_init(ENC_PARAM *input, int target_bitrate0);

    void rc_get_qp(avs3_rc_get_qp_param_t* param, u8 intra_gop_id);

    void rc_update(avs3_rc_update_param_t *param, char *ext_info, int info_len);
    void rlambda_get_subgop_target_bits(int numPic, double* cpp_list, int max_cpp_idx,int frame_level);

    void rlambda_updateAfterPicture(int actualHeaderBits, int actualTotalBits, double averageQP, double averageLambda, int frameLevel, double cpp);

    double rlambda_getRefineBitsForIntra(double cpp);
    double rlambda_solve_equa(double targetBpp, double* equaCoeffA, double* equaCoeffB, double GOPSize);
    double rlambda_estimatePicLambda(double pic_tar_bits, double cpp, double intra_cpp, int frameLevel, int scene_cut, int *codec_list_idx, double *ref_lambda);
     
};

#endif


