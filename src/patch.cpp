#include <math.h>
#include "com_def.h"
#include "patch.h"
#include "aec.h"

extern AEC_input  AEC_DATA_input;
extern AEC_COM_SBAC_CTX aec_ctx;
extern AEC_output AEC_DATA_output;
extern PATCH_INFO* patch;



void sbac_carry_propagate1(ENC_SBAC* sbac, COM_BSW* bs, int* bs_cur)
{
	u32 leadByte = (sbac->code) >> (24 - sbac->left_bits);
	sbac->left_bits += 8;
	(sbac->code) &= (0xffffffffu >> sbac->left_bits);
	if (leadByte < 0xFF)
	{
		while (sbac->stacked_ff != 0)
		{
			sbac_put_byte1(0xFF, sbac, bs, bs_cur);
			sbac->stacked_ff--;
		}
		sbac_put_byte1((u8)leadByte, sbac, bs, bs_cur);
	}
	else if (leadByte > 0xFF)
	{
		sbac->pending_byte++; //! add carry bit to pending_byte
		while (sbac->stacked_ff != 0)
		{
			sbac_put_byte1(0x00, sbac, bs, bs_cur); //! write pending_tyte
			sbac->stacked_ff--;
		}
		sbac_put_byte1((u8)leadByte & 0xFF, sbac, bs, bs_cur);
	}
	else //! leadByte == 0xff
	{
		sbac->stacked_ff++;
	}
}

void aec_enc_sbac_encode_bin_trm1(u32 bin, ENC_SBAC* sbac, COM_BSW* bs, int* bs_cur)
{
	int s_flag = (sbac->range == QUAR_HALF_PROB);
	u32 rMPS = (sbac->range - 1) | 0x100;
	(sbac->range) -= 2;
	if (bin)
	{
		sbac->range = QUAR_HALF_PROB;
		sbac->code = ((sbac->code << s_flag) + rMPS) << 8;
		sbac->left_bits -= (8 + s_flag);
		if (sbac->left_bits < 12)
		{
			sbac_carry_propagate1(sbac, bs, bs_cur);
		}
	}
	else
	{
		if (s_flag)
		{
			sbac->code <<= 1;
			if (--sbac->left_bits < 12)
			{
				sbac_carry_propagate1(sbac, bs, bs_cur);
			}
		}
		sbac->range = rMPS;
	}
}

int aec_com_bsw_write_test(COM_BSW* bs, u32 val, int len, int* bs_cur) /* len(1 ~ 32) */
{
#pragma HLS INLINE off
	int leftbits;
	com_assert(bs);
	leftbits = bs->leftbits;
	val <<= (32 - len);
	bs->code |= (val >> (32 - leftbits));
	if (len < leftbits) {
		bs->leftbits -= len;
	}
	else {
		bs->leftbits = 0;
		//bs->fn_flush(bs);
		aec_com_bsw_flush1(bs, bs_cur);
#if defined(X86F)
		/* on X86 machine, shift operation works properly when the value of the
		   right operand is less than the number of bits of the left operand. */
		bs->code = (leftbits < 32 ? val << leftbits : 0);
#else
		bs->code = (val << leftbits);
#endif
		bs->leftbits = 32 - (len - leftbits);
	}
	return 0;
}

int aec_com_bsw_flush1(COM_BSW* bs, int* bs_cur)
{
	int bytes = COM_BSW_GET_SINK_BYTE(bs);
	while (bytes--)
	{
#ifdef SKIP_MERLIN_SEP
		bs->cur_temp[bs->cur_size] = (bs->code >> 24) & 0xFF;
		bs->cur_size++;
#else
		//int i = *bs_cur;
		bs->beg[*bs_cur] = (bs->code >> 24) & 0xFF;
		*bs_cur += 1;
#endif
		bs->code <<= 8;
	}
	return 0;
}

void aec_enc_sbac_finish1(ENC_SBAC* sbac, COM_BSW* bs, int* bs_cur)
{
	int is_ipcm = 0;
	//ENC_SBAC *sbac = &AEC_DATA_input.sbac;
	if (sbac->code >> (32 - sbac->left_bits))
	{
		assert(sbac->pending_byte != 0xff);
		aec_com_bsw_write_test(bs, sbac->pending_byte + 1, 8, bs_cur);
		while (sbac->stacked_ff != 0)
		{
			aec_com_bsw_write_test(bs, 0x00, 8, bs_cur);
			sbac->stacked_ff--;
		}
		sbac->code -= 1 << (32 - sbac->left_bits);
	}
	else
	{
		if (sbac->is_pending_byte)
		{
			aec_com_bsw_write_test(bs, sbac->pending_byte, 8, bs_cur);
		}

		while (sbac->stacked_ff != 0)
		{
			aec_com_bsw_write_test(bs, 0xFF, 8, bs_cur);
			sbac->stacked_ff--;
		}
	}
	sbac->code |= (1 << 7);
	aec_com_bsw_write_test(bs, sbac->code >> 8, 24 - sbac->left_bits, bs_cur);

	//if ((23 - sbac->left_bits) % 8)
	if (is_ipcm || (24 - sbac->left_bits) % 8) // write the last byte of low in the end of CABAC, if the number of used bits (23 - left_bits) + 1 is not exactly bytes (Nx8), corresponding to bits_Needed != 0
	{
		aec_com_bsw_write_test(bs, sbac->code, 8, bs_cur);
	}

	if (!is_ipcm)
	{
		//add termination slice padding bits
		aec_com_bsw_write_test(bs, 1, 1, bs_cur);
	}
	while (!COM_BSR_IS_BYTE_ALIGN(bs))
	{
		aec_com_bsw_write_test(bs, 0, 1, bs_cur);
	}

	//aec_com_bsw_flush1(bs, bs_cur);

	//bs->leftbits = 32;
	//bs->code = 0;
}

int aec_enc_eco_send(COM_BSW* bs, COM_BSW* ctx_bs, int* bs_cur)
{
	aec_com_bsw_write_test(bs, 0x000001, 24, bs_cur);
	aec_com_bsw_write_test(bs, 0x8F, 8, bs_cur);

	return COM_OK;
}

int aec_com_bsw_write1(COM_BSW* bs, int val, int* bs_cur)
{
	com_assert(bs);
	bs->leftbits--;
	bs->code |= ((val & 0x1) << bs->leftbits);
	if (bs->leftbits == 0) {
		com_assert_rv(bs->cur <= bs->end, -1);
		//bs->fn_flush(bs);
		aec_com_bsw_flush1(bs, bs_cur);
		bs->code = 0;
		bs->leftbits = 32;
	}
	return 0;
}

int aec_enc_eco_patch_header(COM_BSW* bs, COM_BSW* ctx_bs, COM_SQH* sqh, COM_PIC_HEADER* ph, COM_SH_EXT* sh, u8 patch_idx, PATCH_INFO* patch, int* bs_cur)
{
	//patch_start_code_prefix
	aec_com_bsw_write_test(bs, 0x000001, 24, bs_cur);
	//patch_index
	aec_com_bsw_write_test(bs, patch_idx, 8, bs_cur);

	if (!ph->fixed_picture_qp_flag) {
		aec_com_bsw_write1(bs, sh->fixed_slice_qp_flag, bs_cur);
		aec_com_bsw_write_test(bs, sh->slice_qp, 7, bs_cur);
	}
	s8* sao_enable_patch = patch->patch_sao_enable + patch_idx * N_C;
	if (sqh->sample_adaptive_offset_enable_flag) {
		aec_com_bsw_write1(bs, sao_enable_patch[Y_C], bs_cur);
		aec_com_bsw_write1(bs, sao_enable_patch[U_C], bs_cur);
		aec_com_bsw_write1(bs, sao_enable_patch[V_C], bs_cur);
	}

	/* byte align */
	while (!COM_BSR_IS_BYTE_ALIGN(bs)) {
		aec_com_bsw_write1(bs, 1, bs_cur);
	}

	return COM_OK;
}

void Demulate1(COM_BSW* bs, COM_BSW* ctx_bs, int* bs_cur, int bs_start_pos)
{
	unsigned int uiZeroCount = 0;
	unsigned char ucHeaderType = 0;
	unsigned int uiStartPos = bs_start_pos;
	//bs->fn_flush(bs);
	//int start_pos = *bs_cur;
	aec_com_bsw_flush1(bs, bs_cur);
	//for (int i = 0; i < *bs_cur - start_pos; i++)
	//{
	//	ctx_bs->beg[start_pos + i + patch->len] = bs->beg[start_pos + i];
	//}
	bs->leftbits = bs->leftbits % 8;
	//unsigned int current_bytes_size = COM_BSW_GET_WRITE_BYTE(bs);
	unsigned int current_bytes_size = *bs_cur;
	//unsigned int ctx_current_bytes_size = *bs_cur + patch->len;
	int plus = 0;

	//stuffing bit '1'
	int stuffbitnum = bs->leftbits;
	if (stuffbitnum > 0) {
		//ctx_bs->beg[ctx_current_bytes_size - 1] = bs->beg[current_bytes_size - 1] & (~(1 << (8 - stuffbitnum))) << stuffbitnum;
		//ctx_bs->beg[ctx_current_bytes_size - 1] = bs->beg[current_bytes_size - 1] | (1 << (stuffbitnum - 1));
		bs->beg[current_bytes_size - 1] = bs->beg[current_bytes_size - 1] & (~(1 << (8 - stuffbitnum))) << stuffbitnum;
		bs->beg[current_bytes_size - 1] = bs->beg[current_bytes_size - 1] | (1 << (stuffbitnum - 1));
	}
	else {
		//ctx_bs->beg[ctx_current_bytes_size++] = 0x80;
		plus = 1;
		bs->beg[current_bytes_size++] = 0x80;
	}

	//if (bs->beg[uiStartPos] == 0x00 && bs->beg[uiStartPos + 1] == 0x00 && bs->beg[uiStartPos + 2] == 0x01) {
	//	uiStartPos += 3;
	//	ucHeaderType = bs->beg[uiStartPos++]; //HeaderType
	//	if (ucHeaderType == 0x00) {
	//		uiZeroCount++;
	//	}
	//}
	//else {
	//	printf("Wrong start code!");
	//	exit(1);
	//}

	//unsigned int uiWriteOffset = uiStartPos - bs_start_pos;
	//unsigned int uiBitsWriteOffset = 0;

	//switch (ucHeaderType) {
	//case 0xb5:
	//	if ((bs->beg[uiStartPos] >> 4) == 0x0d) {
	//		break;
	//	}
	//	else {
	//	}
	//case 0xb0: //
	//case 0xb2: //
	//	return;
	//default:
	//	break;
	//}
	///*write head info*/
	//bs->buftmp[0] = 0x00;
	//bs->buftmp[1] = 0x00;
	//bs->buftmp[2] = 0x01;
	//bs->buftmp[3] = ucHeaderType;
	/*demulate*/
#if 1
	//for (unsigned int uiReadOffset = uiStartPos; uiReadOffset < current_bytes_size; uiReadOffset++) {
	//	unsigned char ucCurByte = (bs->beg[uiReadOffset - 1] << (8 - uiBitsWriteOffset)) | (bs->beg[uiReadOffset] >>
	//		uiBitsWriteOffset);
	//	if (2 <= uiZeroCount && 0 == (ucCurByte & 0xfc)) {
	//		bs->buftmp[uiWriteOffset++] = 0x02;
	//		uiBitsWriteOffset += 2;
	//		uiZeroCount = 0;
	//		if (uiBitsWriteOffset >= 8) {
	//			uiBitsWriteOffset = 0;
	//			uiReadOffset--;
	//		}
	//		continue;
	//	}
	//	bs->buftmp[uiWriteOffset++] = ucCurByte;

	//	if (0 == ucCurByte) {
	//		uiZeroCount++;
	//	}
	//	else {
	//		uiZeroCount = 0;
	//	}
	//}

	//if (uiBitsWriteOffset != 0) {
	//	/*get the last several bits*/
	//	unsigned char  ucCurByte = bs->beg[current_bytes_size - 1] << (8 - uiBitsWriteOffset);
	//	bs->buftmp[uiWriteOffset++] = ucCurByte;
	//}

	//for (unsigned int i = 0; i < uiWriteOffset; i++) {
	//	bs->beg[bs_start_pos + i] = bs->buftmp[i];
	//}
#endif
	* bs_cur = *bs_cur + plus;
	//bs->cur = bs->beg + uiWriteOffset;
	bs->code = 0;
	bs->leftbits = 32;
}

void sbac_put_byte1(u8 writing_byte, ENC_SBAC* sbac, COM_BSW* bs, int* bs_cur)
{
	if (sbac->is_pending_byte)
	{
		if (sbac->is_bitcount)
			com_bsw_write_est(sbac, 8);
		else
			aec_com_bsw_write_test(bs, sbac->pending_byte, 8, bs_cur);

		//com_bsw_write(bs, sbac->pending_byte, 8);
	}
	sbac->pending_byte = writing_byte;
	sbac->is_pending_byte = 1;
}

