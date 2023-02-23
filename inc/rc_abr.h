#ifndef _RC_ABR_H_
#define _RC_ABR_H_
#include "uratectrl.h"
#include "rate_control.h"
#include "enc_def.h"
//#include "common.h"

#define ABR_BETA1 2

typedef struct rc_abr_ctrl
{
	/* Sequence level const value */
	ENC_PARAM *enc_cfg;
	double m_seq_tar_bit_rate;
	double m_seq_avg_frm_bits;
	double m_seq_frm_rate;
	double m_seq_avg_subgop_bpp;
	int m_seq_pic_pixels;

	/* Sequence level */
	int    last_ip_type;
	double last_ip_qp;
	double last_finished_ip_lambda;
	double last_finished_ip_cplxr;

	double m_shortTermCplxSum;
	double m_shortTermCplxCount;
	double m_cplxrSum;
	double m_total_err_bits;
	i64s_t m_total_bits;
	i64s_t m_total_frms;
	i64s_t m_subgops;

	/* Sub-GOP level */
	int    m_subgop_bfrms;
	double m_bpp_in_subgop;
	double curr_ip_qp;
	double curr_ip_lambda;
} rc_abr_ctrl;

class RC_ABR :public Rate_Control{
	rc_abr_ctrl rc_ctrl;

public:
	RC_ABR(ENC_PARAM *input, unsigned int target_bitrate, int frame_rate, int pic_width, int pic_height){
		rc_init(input, target_bitrate, frame_rate, pic_width, pic_height);
	}
	~RC_ABR(){
	}

	void  rc_init(ENC_PARAM *input,unsigned int target_bitrate, int frame_rate, int pic_width, int pic_height);
	void  rc_get_qp(avs3_rc_get_qp_param_t *param, u8 intra_gop_id);
	void  rc_update(avs3_rc_update_param_t *param, char *ext_info, int info_len);

};
#endif
