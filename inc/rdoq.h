#pragma once
template <const int W, const int H>
int enc_quant_nnz1(int bit_depth, int qp, s64 lambda, s16* coef, int cu_width_log2, int cu_height_log2, int ch_type);