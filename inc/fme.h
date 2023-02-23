#ifndef _FME_H_
#define _FME_H_

#include "iime.h"

#define COM_INT14_MAX           ((S14)(8191))
#define COM_INT14_MIN           ((S14)(-8192))



#define MAC_8TAP_FME(filter, buf, offset)	((filter)[1] * (buf)[(offset) + 1] +\
						(filter)[2] * (buf)[(offset) + 2] +\
						(filter)[3] * (buf)[(offset) + 3] +\
						(filter)[4] * (buf)[(offset) + 4] +\
						(filter)[5] * (buf)[(offset) + 5] +\
						(filter)[6] * (buf)[(offset) + 6]+ \
                                                (filter)[0] * ((buf)[(offset)] + (buf)[(offset) + 7]))

#define MAC_8TAP_0FME(buf, offset)	((buf)[(offset) + 3]<<6)

#define MAC_8TAP_1FME(buf, offset)	  (((buf)[(offset) + 1]<<2) \
									 -(((buf)[(offset) + 2]<<3) + ((buf)[(offset) + 2]<<1)) \
								     +(((buf)[(offset) + 3]<<6) - ((buf)[(offset) + 3]<<3)  + (buf)[(offset) + 3]) \
									 +(((buf)[(offset) + 4]<<4) + ((buf)[(offset) + 4]<<1)  + (buf)[(offset) + 4]) \
									 -(((buf)[(offset) + 5]<<3) - (buf)[(offset) + 5]) \
									 +(((buf)[(offset) + 6]<<1) + (buf)[(offset) + 6]) \
									  -((buf)[(offset)] + (buf)[(offset) + 7]))

#define MAC_8TAP_2FME(buf, offset)	 (((buf)[(offset) + 1]<<2) -\
		        					 (((buf)[(offset) + 2]<<3) + ((buf)[(offset) + 2]<<1) + (buf)[(offset) + 2]) + \
	                                 (((buf)[(offset) + 3]<<5) + ((buf)[(offset) + 3]<<3)) + \
									 (((buf)[(offset) + 4]<<5) + ((buf)[(offset) + 4]<<3))  -\
									 (((buf)[(offset) + 5]<<3) + ((buf)[(offset) + 5]<<1) + (buf)[(offset) + 5])+\
		        					  ((buf)[(offset) + 6]<<2) - \
                                      ((buf)[(offset)] + (buf)[(offset) + 7]))

#define MAC_8TAP_3FME(buf, offset)	((((buf)[(offset) + 1]<<1) +  (buf)[(offset) + 1]) -\
									 (((buf)[(offset) + 2]<<3) - (buf)[(offset) + 2])+\
	                                 (((buf)[(offset) + 3]<<4) + ((buf)[(offset) + 3]<<1) + (buf)[(offset) + 3]) +\
									 (((buf)[(offset) + 4]<<6) - ((buf)[(offset) + 4]<<3) + (buf)[(offset) + 4]) -\
			                         (((buf)[(offset) + 5]<<3) + ((buf)[(offset) + 5]<<1)) + \
									  ((buf)[(offset) + 6]<<2) - \
                                       (buf)[(offset)] - (buf)[(offset) + 7])


#define FME_INTERFACE        1
#define FME_PRAGMA_ON        1
#define COLLECT_FME_DATA     0

#if COLLECT_FME_DATA
#define FME_CU_NUM           200
#define	FME_INPUT_F          "fme_input_info.dat"
#define	FME_OUTPUT_F         "fme_output_info.dat"
#endif


void fme_run(pel RefWindowLCU_2d[FME_SWH_SIZE][FME_SWW_SIZE], /*U8 Fetch_Refwin_ptr_refwin_md[FME_SW_SIZE][FME_SW_SIZE], pel ime_out_para_set_y_org[64][64],*/
	ME_MV_INFO_BW* me_mv_info, ME_PMV_INFO_BW* me_pmv_info,
	ENC_FME_INFO_ALL* fme_mv_info, IME_OUT_PARA_SET* ime_out_para_set, FME_OUT_PARA_SET* fme_out_para_set, pel fme2mdrefbuf[2560][8][2][16], FME_OUT_PARA_SET_CHROMA* fme_out_para_set_chroma
#if ENABLE_BFRAME
	, pel RefWindowLCU_2d_ref1[FME_SWH_SIZE][FME_SWW_SIZE],
	ME_MV_DIR* me_mv_dir, ME_MV_DIR* me_mv_dir_fme2md,
	pel RefWindowLCU_2d_ref1_tmp[FME_SW_SIZE][FME_SW_SIZE]
#endif
);

#endif
