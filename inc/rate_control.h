#ifndef _RATE_CONTROL_H_
#define _RATE_CONTROL_H_
#include "uratectrl.h"
#include "com_port.h"

class Rate_Control{

public:
	u8 intra_gop_id=0;
	//virtual void rc_init(ENC_PARAM *input, int target_bitrate0)=0;
	virtual void rc_get_qp(avs3_rc_get_qp_param_t* param, u8 intra_gop_id) = 0;
	virtual void rc_update(avs3_rc_update_param_t *param, char *ext_info, int info_len) = 0;

	i32u_t xCalcHADs8x8_I(u8 *p_org, int i_org);
};

#endif
