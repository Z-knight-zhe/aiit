#ifndef _COM_LK_H_
#define _COM_LK_H_
#include "com_port.h"
#include "ratectrl.h"

void inter_search_init(inter_search_t *pi, COM_INFO *info, int is_padding);
int inter_search_create(u8 **pptab, COM_INFO *info);
void inter_search_free(u8 *tab_mvbits, int tab_mvbits_offset);

double loka_estimate_coding_cost(inter_search_t *pi, COM_PIC *img_org, 
    COM_IMGB **ref_l0, COM_IMGB **ref_l1, int  num_ref[2],
    int bit_depth, double *icost, double icost_uv[2], float* map_dqp);
int com_had(int w, int h, s16 *addr_org, s16 *addr_curr, int s_org, int s_cur, int bit_depth);

void com_mc_blk_luma(COM_PIC *pic, pel *dst, int dst_stride, int x_pos, int y_pos, int width, int height, int widx, int max_posx, int max_posy, int max_val, int hp_flag);
#endif