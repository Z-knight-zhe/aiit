#ifndef _RC_TOP_
#define _RC_TOP_

#include"rate_control.h"
#include "enc_def.h"
#include"rc_cbr.h"
#include"rc_crf.h"
#include"rc_abr.h"

//#include<iostream>
//using namespace std;

extern u8 rate_ctrl;
extern Rate_Control *rc_ctrl;
extern int op_target_bitrate;////kbps
extern u8 GOP_SIZE;


class RC_TOP{
	u8 is_initialized = 0;
	int  pic_width;
	int pic_height;
	int pic_height_in_lcu;
	int max_cuwh;
public:
    
	u8 img_org_initQP = 23;//////////////////////
	double img_org_initLambda;
	double img_org_target_bits;
	int bitdepth_qp_offset = 0;

	double  img_org_cpp;
	int img_org_coded_list_idx;

	//double frame_buffer_cpp[MAXGOP];
	int frames_cpps = 0;

	double img_org_intra_cpp = 0; //////SATD
	double img_org_inter_cpp = 0;
	double *intra_cpp_list = NULL; ////for I frame
	//double *inter_cpp_list = NULL; ///for P/B frame

	u32 header_bits = 0;
	u32 data_bits = 0;

	RC_TOP(){}
	RC_TOP(ENC_PARAM*ctx_param, int pic_width0, int pic_height0, int pic_height_in_lcu0, int max_cuwh0, int fps){
		init(ctx_param,pic_width0,pic_height0, pic_height_in_lcu0, max_cuwh0,fps);
	}
	void init(ENC_PARAM*ctx_param,int pic_width0, int pic_height0, int pic_height_in_lcu0,int max_cuwh0,int fps){
		pic_width = pic_width0;
		pic_height = pic_height0;
		pic_height_in_lcu = pic_height_in_lcu0;
		max_cuwh = max_cuwh0;

		img_org_initQP = 23;//////////////////////
		bitdepth_qp_offset = 0;

		//double frame_buffer_cpp[MAXGOP];
		frames_cpps = 0;

		img_org_intra_cpp = 0; //////SATD
		img_org_inter_cpp = 0;
		intra_cpp_list = NULL; ////for I frame
		//double *inter_cpp_list = NULL; ///for P/B frame

		header_bits = 0;
		data_bits = 0;
		
		int lcu_rows = (pic_height + max_cuwh - 1) / max_cuwh;
		// inter_cpp_list=(double*)malloc(sizeof(double) * lcu_rows);
		intra_cpp_list = (double*)malloc(sizeof(double) * lcu_rows);
		//memset(inter_cpp_list, 0, sizeof(double) * lcu_rows);
		memset(intra_cpp_list, 0, sizeof(double) * lcu_rows);


			switch (rate_ctrl){
			case RC_TYPE_CBR:
				rc_ctrl = new RC_CBR(ctx_param, op_target_bitrate);
				break;
			case RC_TYPE_ABR:
				rc_ctrl = new RC_ABR(ctx_param, op_target_bitrate, fps, pic_width, pic_height);
				break;
			case RC_TYPE_CRF:
				rc_ctrl = new RC_CRF(ctx_param);// , target_bitrate);
				break;
			}

 
			is_initialized = 1;///////////////////////////

		}
	 

	~RC_TOP(){
		if(intra_cpp_list)free(intra_cpp_list);///////
	}

	bool isInitialized(){
		return is_initialized;
	}
	void group_id_inc(){
	 rc_ctrl->intra_gop_id=(rc_ctrl->intra_gop_id+1)%GOP_SIZE;
	}
	int get_rc_QP(ENC_PARAM &ctx_param, COM_PIC*ctx_pic_org, u8 ctx_slice_type, int fps, int ctx_poc);
	void  refine_pic_qp(ENC_PARAM&ctx_param, int initQP, int ctx_slice_type);
	double  pre_cal_I_cost(u8 *p_org, int i_org, int width, int height, int bit_depth, double* cpp_list);
	int   getQP(int b_IFrm, int bit_depth_input, int bit_depth_internal, COM_PIC*ctx_pic_org, u8 ctx_slice_type, int ctx_poc,int intra_gop_id);
	void  rc_proprocess(int bit_depth_internal, u8 ctx_slice_type, int ctx_poc);

};

#endif
