#ifndef __PATCH
#define __PATCH
#include "enc_def.h"
void sbac_carry_propagate1(ENC_SBAC* sbac, COM_BSW* bs, int* bs_cur);

void aec_enc_sbac_encode_bin_trm1(u32 bin, ENC_SBAC* sbac, COM_BSW* bs, int* bs_cur);

int aec_com_bsw_write(COM_BSW* bs, u32 val, int len);

int aec_com_bsw_write_test(COM_BSW* bs, u32 val, int len, int* bs_cur);

int aec_com_bsw_flush1(COM_BSW* bs, int* bs_cur);

int aec_com_bsw_flush(COM_BSW* bs);

void aec_enc_sbac_finish1(ENC_SBAC* sbac, COM_BSW* bs, int* bs_cur);

int aec_enc_eco_send(COM_BSW* bs, COM_BSW* ctx_bs, int* bs_cur);

int aec_enc_eco_patch_header(COM_BSW* bs, COM_BSW* ctx_bs, COM_SQH* sqh, COM_PIC_HEADER* ph, COM_SH_EXT* sh, u8 patch_idx, PATCH_INFO* patch, int* bs_cur);

void Demulate1(COM_BSW* bs, COM_BSW* ctx_bs, int* bs_cur, int bs_start_pos);

void sbac_put_byte1(u8 writing_byte, ENC_SBAC* sbac, COM_BSW* bs, int* bs_cur);

#endif // !__PATCH



