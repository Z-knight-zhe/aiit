#ifndef _URATECONTRL_H_
#define _URATECONTRL_H_

#include <cstdlib>
#include <math.h>
#include<string.h>
//#include "uAVS3lib.h"
#include <memory.h>

#define RC_TYPE_CBR 1
#define RC_TYPE_ABR 2
#define RC_TYPE_CRF 3
typedef signed long long   i64s_t;
typedef signed int         i32s_t;
typedef signed short       i16s_t;
typedef signed char        char_t;
typedef unsigned long long i64u_t;
typedef unsigned int       i32u_t;
typedef unsigned short     i16u_t;
typedef unsigned char      uchar_t;

#define Clip3(min,max,val)  (((val)<(min))?(min):(((val)>(max))?(max):(val)))

/*****************************************************************************
* slice type
*****************************************************************************/
#define COM_ST_UNKNOWN                  (0)
#define COM_ST_I                        (1)
#define COM_ST_P                        (2)
#define COM_ST_B                        (3)

#define SLICE_I                            COM_ST_I
#define SLICE_P                            COM_ST_P
#define SLICE_B                            COM_ST_B

#define IS_INTRA_SLICE(slice_type)       ((slice_type) == SLICE_I))
#define IS_INTER_SLICE(slice_type)      (((slice_type) == SLICE_P) || ((slice_type) == SLICE_B))

#define MAX_QP 63
#define MAXGOP 32
// Quantization parameter range
#define SHIFT_QP                     11
#define LAM_2Level_TU            0.8

#define FP_FLG  0x0A  /* 0000 1010 */
#define BG_FLG  0x20  /* 0010 0000 */
/* ---------------------------------------------------------------------------
*  Rate Control types and callback functions
*/
typedef struct avs3_rc_basic_info_t {
	int type;               // in
	int frameLevel;         // in
	long long poc;          // in
	long long coi;          // in
	int    scene_cut;       // in
	int    qp;              // out/in         
	double lambda;          // out/in       
	double target_bits;     // out/in, only used for log-info
	double pic_cpp;         // out/in  
	double inter_cpp;       // in
	double intra_cpp;       // in
	int    coded_list_idx;  // out
} avs3_rc_basic_info_t;

typedef struct avs3_rc_get_qp_param_t {
	avs3_rc_basic_info_t info;
	int    bfrms_in_subgop;
	double *sub_gop_cpp_list;
	double ref_lambda[2];
} avs3_rc_get_qp_param_t;

typedef struct avs3_rc_update_param_t {
	avs3_rc_basic_info_t info;
	int    header_bits;
	int    data_bits;
	//int    coef_bits;
} avs3_rc_update_param_t;

 

#endif // #ifndef _RATECONTRAL_H_