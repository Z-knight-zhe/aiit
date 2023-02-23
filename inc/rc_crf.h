#ifndef _RC_CRF_H_
#define _RC_CRF_H_

#include "uratectrl.h"
#include "rate_control.h"
#include "enc_def.h"

#include <math.h>
#include <stdio.h>
//#include "common.h"

#define CRF_BETA1 2
#define QCompress 0.6

typedef struct rc_crf_ctrl
{
	/* Sequence level const value */
	ENC_PARAM *enc_cfg;
	double m_rfConst;
	int m_anchor_qp;

	/* Sequence level */
	int    last_ip_type;
	double last_ip_qp;

	double m_shortTermCplxSum;
	double m_shortTermCplxCount;
	i64s_t m_subgops;

	/* Sub-GOP level */
	double curr_ip_qp;
	double curr_ip_lambda;
} rc_crf_ctrl;

class RC_CRF :public Rate_Control{
	rc_crf_ctrl  rc_ctrl;
	int bitdepth_qp_offset=0;// (input->bit_depth - 8) * 8;
public:
	RC_CRF(ENC_PARAM*enc_param){
		rc_init(enc_param);
	}
	~RC_CRF(){
	}
	void rc_init(ENC_PARAM*enc_param);
	void  rc_get_qp(avs3_rc_get_qp_param_t *param, u8 intra_gop_id);
	void  rc_update(avs3_rc_update_param_t *param, char *ext_info, int info_len);

};


 

#endif
