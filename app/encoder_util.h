#ifndef __ENC_UTIL_H__
#define __ENC_UTIL_H__
#include "stdlib.h"
#include "com_typedef.h"
#include "com_def.h"
#include "enc_def.h"
#include <memory>
#include "app_util.h"
#if defined(__linux__)
#include <string.h>
#include <unistd.h>
#else
#include <string>
#include <corecrt_io.h>
#include <Windows.h>
#endif
typedef enum _STATES
{
	STATE_ENCODING,
	STATE_BUMPING,
	STATE_SKIPPING
} STATES;

typedef struct _IMGB_LIST
{
	COM_IMGB* imgb;
	int            used;
	COM_MTIME     ts;
} IMGB_LIST;

enum PatchModeDefine
{
	NONE_SPLIT = 0,
	HOR_BI = 1,
	VER_BI = 2,
	QUARD = 3,
	CUSTOM_HOR = 4,
	CUSTOM_VER = 5,
	HEAD_INFO = 8,

	PATCH_CNT = 8,   //support max to 8 patch
};

enum BitstreamDefine
{
	SEQ_HEAD = 0,
	PIC_HEAD = 1,
	CTU_INFO = 2,
	SEQ_END = 3,
	ERR_INFO = 4
};

#define MAX_BUMP_FRM_CNT           (8 <<1)


class Encoder
{
public:
	Encoder();
	~Encoder();
	int encoder_init(int argc, const char** argv);
	int encode_one_frame(
#if PATCH_DIVIDE_ENABLE
		int multislice_mode
#endif
	);
	int encoder_end();
	void run(
#if PATCH_DIVIDE_ENABLE
		int multislice_mode,
#endif
		int argc, const char** argv);
	int  frame_end(int* end);
	int finish_encode_one_frame();
	void get_patch_size(int patch_idx);
	void encoder_info_config();
	void run_generate_libpic(LibVCData* libvc_data, int* list_libpic_idx);
	void run_decide_pic_referencing_libpic(LibVCData* libvc_data, int* list_libpic_idx);
	int compute_LibVCData(ENC_PARAM param_in, LibVCData* libvc_data);
	int AllocateBitstream();
	int firmware_init();
	int init_img();
	int malloc_picture();
	void parameter_init();
	void information_init();
	void free_picture();
	int img_buffer_init();
	int state_encoding();
	int frame_init();
	void rec_pictures_store();
	void report();
	void checkSize(const int mode);

	unsigned char* get_bs_addr() { return bs_buf_tmp; };
	unsigned char* get_bs_lib_addr() { return bs_buf_lib_tmp; };
	int get_bs_size() { return bs_tmp_size; };
	int get_bs_lib_size() { return bs_tmp_size; };
	int get_patch_num() { return patch_num; };
	int get_split_mode() { return split_mode; };
	int get_patch_width() { return patch_width; };
	int get_patch_height() { return patch_height; };
	int get_pic_width() { return pic_width; };
	int get_pic_height() { return pic_height; };
	int get_bit_depth() { return bit_depth_out; };
	int get_frame_num() { return frame_num; };
	char* get_fname_inp() { return fname_inp; };
	char* get_fname_out() { return fname_out; };
	char* get_fname_rec() { return fname_rec; };
	

	void set_patch_idx(int idx) { patch_idx = idx; };

private:
	unsigned char* bs_buf_lib_tmp = NULL;
	unsigned char* bs_buf_tmp = NULL;
	int bs_lib_size = 0;
	int bs_tmp_size = 0;
	int pic_width = 0;
	int pic_height = 0;
	int patch_width = 0;
	int patch_height = 0;
	int patch_idx = 0;
	int split_mode = 0;
	int patch_num = 2;
	int head_info_flag = 0;
	int bit_depth_out = 0;
	int frame_num = 0;
	char* fname_inp = NULL;
	char* fname_out = NULL;
	char* fname_rec = NULL;

	/*******parameters for libpic***********************/
// reserve the data from ori config
	int ori_flag;
	int ori_iperiod;
	int ori_max_b_frames;
	int ori_qp;
	int err;
	FILE* fp_inp_lib = NULL;
	unsigned char* bs_buf_lib = NULL;
	unsigned char* bs_buf_lib2 = NULL;
	STATES              state_lib = STATE_ENCODING;
	int                 ret_lib;
	ENC                 id_lib;
	COM_BITB            bitb_lib;
	COM_IMGB* imgb_enc_lib = NULL;
	COM_IMGB* imgb_rec_lib = NULL;
	IMGB_LIST           ilist_org_lib[MAX_BUMP_FRM_CNT];
	IMGB_LIST           ilist_rec_lib[MAX_BUMP_FRM_CNT];
	IMGB_LIST* ilist_t_lib = NULL;
	int                 udata_size_lib = 0;
	ENC_STAT            stat_lib;
	double              bitrate_lib;
	double              bits_libpic;
	COM_MTIME           pic_icnt_lib, pic_ocnt_lib, pic_skip_lib;
	COM_CLK             clk_beg_lib, clk_end_lib, clk_tot_lib;
	int                 num_encoded_frames_lib = 0;
	double              psnr_lib[3] = { 0, };
	double              psnr_avg_lib[3] = { 0, };
	int                 size_lib;
	/*******************common parameters fot lib and maib************/
	ENC_STAT            stat_c;
	ENC                 id_c;
	COM_BITB            bitb_c;
	int                 encoder_change_condition;
	/*******************parameters for main_pic***********************/
	STATES              state = STATE_ENCODING;

	unsigned char* bs_buf = NULL;
	unsigned char* bs_buf2 = NULL;
	FILE* fp_inp = NULL;
	ENC                id;
	ENC_PARAM          param_input;

	COM_BITB           bitb;
	COM_IMGB* imgb_enc = NULL; // used in the real encoding process (always 16-bit)
	COM_IMGB* imgb_rec = NULL; // point to the real reconstructed picture (always 16-bit)
	ENC_STAT           stat1;
	int                udata_size = 0;
	int                i, ret = 0, size;
	COM_CLK            clk_beg, clk_end, clk_tot;
	COM_MTIME          pic_icnt, pic_ocnt, pic_skip;
	int                libpic_icnt;
	int                num_encoded_frames = 0;
	double             bitrate;
	double             psnr[3] = { 0, 0, 0 };
	double             psnr_avg[3] = { 0, 0, 0 };

	IMGB_LIST          ilist_org[MAX_BUMP_FRM_CNT]; // always 10-bit depth
	IMGB_LIST          ilist_rec[MAX_BUMP_FRM_CNT]; // which is only used for storing the reconstructed pictures to be written out, so the output bit depth is used for it

	IMGB_LIST* ilist_t = NULL;
	int         is_first_enc = 1;
#if LIBVC_ON
	LibVCData libvc_data;
#endif
#if !CALC_SSIM
	double             seq_header_bit = 0;
#endif
#if AEC_STAGE1_DATA
	FILE* fp_stage1_InStream = NULL;
	FILE* fp_stage1_OutStream = NULL;
#endif

#if AEC_STAGE2_DATA
	FILE* fp_stage2_InStream = NULL;
	FILE* fp_stage2_OutStream = NULL;
#endif

};

class Multislice
{
public:
	Multislice();
	~Multislice();
	void run(
		int argc, 
		const char** argv
	);
	void bs_process(int split_mode, int frame_num, const int patch_num);
	void yuv_split(
		const char* org_file,
		const int pic_width,
		const int pic_height,
		const int bit_depth,
		const int patch_width,
		const int patch_height,
		const int patch_idx,
		const int split_mode,
		const int frame_num,
		const int is_last_patch
	);
	void yuv_merge(
		const char* org_rec_file,
		const int pic_width,
		const int pic_height,
		const int bit_depth,
		const int split_mode,
		const int frame_num,
		const int patch_num
	);
	void get_patch_size(
		const int split_mode,
		const int patch_idx,
		const int pic_width,
		const int pic_height,
		const int patch_num,
		int& patch_width,
		int& patch_height
	);
	int parse_multislice_mode(const int split_mode);


	void bs_process_one_frame(int split_mode_in, const int patch_num);
	int detect_bs_type();



private:
	Encoder encoder[PATCH_CNT + 1];

	unsigned char* bs_fp[PATCH_CNT] = { NULL };
	unsigned char* bs_head_fp = NULL;

	unsigned char* bs_final = NULL;
	unsigned char* bs_tmp = NULL;

	int bs_read_pos = 0;
	int bs_read_pos_tmp = 0;
	int bs_read_pos_patch[PATCH_CNT] = { 0 };
	int bs_read_pos_head = 0;

	int bs_tmp_size[PATCH_CNT] = { 0 };
	int bs_lib_size[PATCH_CNT] = { 0 };
	int bs_head_size = 0;

};

#endif
