#pragma warning(disable:4334)
#pragma warning(disable:4018)
#if (!defined(W)) || (!defined(H)) 
#error W or H not define
#endif
#define GET_I_COST(rate, lamba)  (rate*lamba)
#define _RDOQ_FUNC_NAME(name, w, h) name##_##w##x##h
#define RDOQ_FUNC_NAME(name, w, h) _RDOQ_FUNC_NAME(name, w, h)
#define _HLS_DIRECTIVE(cmd) (void)"xx_hls_cmd "#cmd;
#define HLS_DIRECTIVE(cmd) _HLS_DIRECTIVE(cmd)
#define SIZE (W * H)
#define MAT_SIZE COM_MIN(W, H)
#if (W <= H)
#define MAT_SIZE_32_1 (2 * W - 1)
#else
#define MAT_SIZE_32_1 (2 * H)
#endif
#if (W <= 16) && (H <= 16)
#define UPDATE_SIZE SIZE
#elif ((W == 32) && (H == 32)) || ((W == 16) && (H == 32)) || ((W == 8) && (H == 32)) || ((W == 32) && (H == 16)) || ((W == 32) && (H == 8)) || ((W == 32) && (H == 4)) || ((W == 4) && (H == 32))
#define UPDATE_SIZE MAT_SIZE_32_1
#else
#define UPDATE_SIZE MAT_SIZE
#endif
void RDOQ_FUNC_NAME(level_core, W, H)(u32 level[24], u32 level1[24], U5 t0[UPDATE_SIZE], int i, int max_num)
{
#pragma HLS ARRAY_PARTITION variable=level1 complete dim=1
#pragma HLS FUNCTION_INSTANTIATE variable=i
#pragma HLS ARRAY_PARTITION variable=level complete dim=1
    int tmp = 0;
    int pos = 0;
#pragma HLS ARRAY_PARTITION variable=t0 complete dim=1
    for (pos = 0; pos < UPDATE_SIZE; pos++)
    {
#pragma HLS UNROLL
        if (pos < max_num)
        {
            tmp += !(t0[pos] - i);
        }
    }
    level[i] += tmp;
    level1[i + 1] += 8 * tmp;
}
void RDOQ_FUNC_NAME(run_core, W, H)(u32 run[24], u32 run1[24], U5 t0[UPDATE_SIZE], int i, U1 mat_run[UPDATE_SIZE], int max_num)
{
#pragma HLS ARRAY_PARTITION variable=mat_run complete dim=1
#pragma HLS ARRAY_PARTITION variable=run1 complete dim=1
#pragma HLS FUNCTION_INSTANTIATE variable=i
#pragma HLS ARRAY_PARTITION variable=run complete dim=1
    int tmp = 0;
    int tmp2 = 0;
    int pos = 0;
#pragma HLS ARRAY_PARTITION variable=t0 complete dim=1
    for (pos = 0; pos < UPDATE_SIZE; pos++)
    {
#pragma HLS UNROLL
        if (pos < max_num)
        {
            tmp += !(t0[pos] - i);
            tmp2 += mat_run[pos];
        }
    }
    run[i] += tmp;
    run1[i + 1] += tmp2;
}
void RDOQ_FUNC_NAME(last1_core, W, H)(u32 last1[12], U4 t0[UPDATE_SIZE], int i, int max_num)
{
#pragma HLS ARRAY_PARTITION variable=last1 complete dim=1
#pragma HLS FUNCTION_INSTANTIATE variable=i
    int tmp = 0;
    int pos = 0;
#pragma HLS ARRAY_PARTITION variable=t0 complete dim=1
    for (pos = 0; pos < UPDATE_SIZE; pos++)
    {
#pragma HLS UNROLL
        if (pos < max_num)
        {
            tmp += !(t0[pos] - i);
        }
    }
    last1[i] += tmp;
}
void RDOQ_FUNC_NAME(last2_core, W, H)(u32 last2[22], U5 t0[UPDATE_SIZE], int i, int max_num)
{
#pragma HLS ARRAY_PARTITION variable=last2 complete dim=1
#pragma HLS FUNCTION_INSTANTIATE variable=i
    int tmp = 0;
    int pos = 0;
#pragma HLS ARRAY_PARTITION variable=t0 complete dim=1
    for (pos = 0; pos < UPDATE_SIZE; pos++)
    {
#pragma HLS UNROLL
        if (pos < max_num)
        {
            tmp += !(t0[pos] - i);
        }
    }
    last2[i] += tmp;
}
void RDOQ_FUNC_NAME(model_update, W, H)(u32 last1[12], u32 last2[22], u32 run[24], u32 level[24], U1 mat_run[UPDATE_SIZE], U24 mat_prev_level[UPDATE_SIZE], int ch_type, int max_num)
{
#pragma HLS ARRAY_PARTITION variable=last2 complete dim=1
#pragma HLS ARRAY_PARTITION variable=last1 complete dim=1
#pragma HLS PIPELINE II=1
#pragma HLS ARRAY_PARTITION variable=run complete dim=1
#pragma HLS ARRAY_PARTITION variable=level complete dim=1
#pragma HLS ARRAY_PARTITION variable=mat_prev_level complete dim=1
#pragma HLS ARRAY_PARTITION variable=mat_run complete dim=1

    int scan_pos = 0;
    static U5 t0[UPDATE_SIZE] = { 0 };
#pragma HLS ARRAY_PARTITION variable=t0 complete dim=1
    static U4 last1_pos[UPDATE_SIZE] = { 0 };
#pragma HLS ARRAY_PARTITION variable=last1_pos complete dim=1
    static U5 last2_pos[UPDATE_SIZE] = { 0 };
#pragma HLS ARRAY_PARTITION variable=last2_pos complete dim=1
    u32 last1_tmp[12] = { 0 };
#pragma HLS ARRAY_PARTITION variable=last1_tmp complete dim=1
    u32 last2_tmp[22] = { 0 };
#pragma HLS ARRAY_PARTITION variable=last2_tmp complete dim=1
    u32 level_tmp[24] = { 0 };
#pragma HLS ARRAY_PARTITION variable=level_tmp complete dim=1
    u32 level_tmp1[24] = { 0 };
#pragma HLS ARRAY_PARTITION variable=level_tmp1 complete dim=1
    u32 run_tmp[24] = { 0 };
#pragma HLS ARRAY_PARTITION variable=run_tmp complete dim=1
    u32 run_tmp1[24] = { 0 };
#pragma HLS ARRAY_PARTITION variable=run_tmp1 complete dim=1
    int ace_log2 = 0;

    for (scan_pos = 0; scan_pos < UPDATE_SIZE; scan_pos++)
    {
#pragma HLS UNROLL
        if (scan_pos < max_num)
        {
            if (scan_pos + 1 < 2) {
                ace_log2 = 0;
            }
            else if (scan_pos + 1 < 4) {
                ace_log2 = 1;
            }
            else if (scan_pos + 1 < 8) {
                ace_log2 = 2;
            }
            else if (scan_pos + 1 < 16) {
                ace_log2 = 3;
            }
            else if (scan_pos + 1 < 32) {
                ace_log2 = 4;
            }
            else if (scan_pos + 1 < 64) {
                ace_log2 = 5;
            }
            else if (scan_pos + 1 < 128) {
                ace_log2 = 6;
            }
            else if (scan_pos + 1 < 256) {
                ace_log2 = 7;
            }
            else if (scan_pos + 1 < 512) {
                ace_log2 = 8;
            }
            else if (scan_pos + 1 < 1024) {
                ace_log2 = 9;
            }
            else if (scan_pos + 1 < 2048) {
                ace_log2 = 10;
            }
            else if (scan_pos + 1 < 4096) {
                ace_log2 = 11;
            }
            else {
                ace_log2 = 11;
            }

            t0[scan_pos] = ((COM_MIN((U24)(mat_prev_level[scan_pos] - 1), (U24)5)) * 2) + (ch_type == 0 ? 0 : 12);
            last1_pos[scan_pos] = ((COM_MIN((U24)(mat_prev_level[scan_pos] - 1), (U24)5))) + (ch_type == 0 ? 0 : 6);
            last2_pos[scan_pos] = ace_log2 + (ch_type == 0 ? 0 : 12);
        }
    }

    for (scan_pos = 0; scan_pos < 24; scan_pos += 2)
    {
#pragma HLS UNROLL
        RDOQ_FUNC_NAME(level_core, W, H)(level_tmp, level_tmp1, t0, scan_pos, max_num);
        RDOQ_FUNC_NAME(run_core, W, H)(run_tmp, run_tmp1, t0, scan_pos, mat_run, max_num);
    }

    for (scan_pos = 0; scan_pos < 12; scan_pos++)
    {
#pragma HLS UNROLL
        RDOQ_FUNC_NAME(last1_core, W, H)(last1_tmp, last1_pos, scan_pos, max_num);
        last1[scan_pos] += last1_tmp[scan_pos];
    }

    for (scan_pos = 0; scan_pos < 22; scan_pos++)
    {
#pragma HLS UNROLL
        RDOQ_FUNC_NAME(last2_core, W, H)(last2_tmp, last2_pos, scan_pos, max_num);
        last2[scan_pos] += last2_tmp[scan_pos];
    }

    for (scan_pos = 0; scan_pos < 24; scan_pos++)
    {
#pragma HLS UNROLL
        level[scan_pos] += level_tmp[scan_pos] + level_tmp1[scan_pos];
        run[scan_pos] += run[scan_pos] + run_tmp1[scan_pos];
    }
}

#if (W <= 16) && (H <= 16)
//small size circuit
#if (W == 8) && (H == 8)
void cal_cost_core(s64 test[64], int scan_pos, s64* d64_base_cost)
{
#pragma HLS FUNCTION_INSTANTIATE variable=scan_pos
#pragma HLS ARRAY_PARTITION variable=test complete dim=1
#pragma HLS PIPELINE II=1

    s64 d64_base_cost_tmp = 0;
    int i = 0;
    if (scan_pos == 0)
    {
        d64_base_cost_tmp += test[0];
    }
    else if (scan_pos == 1)
    {
        for (i = 0; i <= 1; i++)
        {
#pragma HLS UNROLL
            d64_base_cost_tmp += test[i];
        }
    }
    else if (scan_pos == 2)
    {
        for (i = 0; i <= 2; i++)
        {
#pragma HLS UNROLL
            d64_base_cost_tmp += test[i];
        }
    }
    else if (scan_pos == 3)
    {
        for (i = 0; i <= 3; i++)
        {
#pragma HLS UNROLL
            d64_base_cost_tmp += test[i];
        }
    }
    else if (scan_pos == 4)
    {
        for (i = 0; i <= 4; i++)
        {
#pragma HLS UNROLL
            d64_base_cost_tmp += test[i];
        }
    }
    else if (scan_pos == 5)
    {
        for (i = 0; i <= 5; i++)
        {
#pragma HLS UNROLL
            d64_base_cost_tmp += test[i];
        }
    }
    else if (scan_pos == 6)
    {
        for (i = 0; i <= 6; i++)
        {
#pragma HLS UNROLL
            d64_base_cost_tmp += test[i];
        }
    }
    else if (scan_pos == 7)
    {
        for (i = 0; i <= 7; i++)
        {
#pragma HLS UNROLL
            d64_base_cost_tmp += test[i];
        }
    }
    else if (scan_pos == 8)
    {
        for (i = 0; i <= 8; i++)
        {
#pragma HLS UNROLL
            d64_base_cost_tmp += test[i];
        }
    }
    else if (scan_pos == 9)
    {
        for (i = 0; i <= 9; i++)
        {
#pragma HLS UNROLL
            d64_base_cost_tmp += test[i];
        }
    }
    else if (scan_pos == 10)
    {
        for (i = 0; i <= 10; i++)
        {
#pragma HLS UNROLL
            d64_base_cost_tmp += test[i];
        }
    }
    else if (scan_pos == 11)
    {
        for (i = 0; i <= 11; i++)
        {
#pragma HLS UNROLL
            d64_base_cost_tmp += test[i];
        }
    }
    else if (scan_pos == 12)
    {
        for (i = 0; i <= 12; i++)
        {
#pragma HLS UNROLL
            d64_base_cost_tmp += test[i];
        }
    }
    else if (scan_pos == 13)
    {
        for (i = 0; i <= 13; i++)
        {
#pragma HLS UNROLL
            d64_base_cost_tmp += test[i];
        }
    }
    else if (scan_pos == 14)
    {
        for (i = 0; i <= 14; i++)
        {
#pragma HLS UNROLL
            d64_base_cost_tmp += test[i];
        }
    }
    else if (scan_pos == 15)
    {
        for (i = 0; i <= 15; i++)
        {
#pragma HLS UNROLL
            d64_base_cost_tmp += test[i];
        }
    }
    else if (scan_pos == 16)
    {
        for (i = 0; i <= 16; i++)
        {
#pragma HLS UNROLL
            d64_base_cost_tmp += test[i];
        }
    }
    else if (scan_pos == 17)
    {
        for (i = 0; i <= 17; i++)
        {
#pragma HLS UNROLL
            d64_base_cost_tmp += test[i];
        }
    }
    else if (scan_pos == 18)
    {
        for (i = 0; i <= 18; i++)
        {
#pragma HLS UNROLL
            d64_base_cost_tmp += test[i];
        }
    }
    else if (scan_pos == 19)
    {
        for (i = 0; i <= 19; i++)
        {
#pragma HLS UNROLL
            d64_base_cost_tmp += test[i];
        }
    }
    else if (scan_pos == 20)
    {
        for (i = 0; i <= 20; i++)
        {
#pragma HLS UNROLL
            d64_base_cost_tmp += test[i];
        }
    }
    else if (scan_pos == 21)
    {
        for (i = 0; i <= 21; i++)
        {
#pragma HLS UNROLL
            d64_base_cost_tmp += test[i];
        }
    }
    else if (scan_pos == 22)
    {
        for (i = 0; i <= 22; i++)
        {
#pragma HLS UNROLL
            d64_base_cost_tmp += test[i];
        }
    }
    else if (scan_pos == 23)
    {
        for (i = 0; i <= 23; i++)
        {
#pragma HLS UNROLL
            d64_base_cost_tmp += test[i];
        }
    }
    else if (scan_pos == 24)
    {
        for (i = 0; i <= 24; i++)
        {
#pragma HLS UNROLL
            d64_base_cost_tmp += test[i];
        }
    }
    else if (scan_pos == 25)
    {
        for (i = 0; i <= 25; i++)
        {
#pragma HLS UNROLL
            d64_base_cost_tmp += test[i];
        }
    }
    else if (scan_pos == 26)
    {
        for (i = 0; i <= 26; i++)
        {
#pragma HLS UNROLL
            d64_base_cost_tmp += test[i];
        }
    }
    else if (scan_pos == 27)
    {
        for (i = 0; i <= 27; i++)
        {
#pragma HLS UNROLL
            d64_base_cost_tmp += test[i];
        }
    }
    else if (scan_pos == 28)
    {
        for (i = 0; i <= 28; i++)
        {
#pragma HLS UNROLL
            d64_base_cost_tmp += test[i];
        }
    }
    else if (scan_pos == 29)
    {
        for (i = 0; i <= 29; i++)
        {
#pragma HLS UNROLL
            d64_base_cost_tmp += test[i];
        }
    }
    else if (scan_pos == 30)
    {
        for (i = 0; i <= 30; i++)
        {
#pragma HLS UNROLL
            d64_base_cost_tmp += test[i];
        }
    }
    else if (scan_pos == 31)
    {
        for (i = 0; i <= 31; i++)
        {
#pragma HLS UNROLL
            d64_base_cost_tmp += test[i];
        }
    }
    else if (scan_pos == 32)
    {
        for (i = 0; i <= 32; i++)
        {
#pragma HLS UNROLL
            d64_base_cost_tmp += test[i];
        }
    }
    else if (scan_pos == 33)
    {
        for (i = 0; i <= 33; i++)
        {
#pragma HLS UNROLL
            d64_base_cost_tmp += test[i];
        }
    }
    else if (scan_pos == 34)
    {
        for (i = 0; i <= 34; i++)
        {
#pragma HLS UNROLL
            d64_base_cost_tmp += test[i];
        }
    }
    else if (scan_pos == 35)
    {
        for (i = 0; i <= 35; i++)
        {
#pragma HLS UNROLL
            d64_base_cost_tmp += test[i];
        }
    }
    else if (scan_pos == 36)
    {
        for (i = 0; i <= 36; i++)
        {
#pragma HLS UNROLL
            d64_base_cost_tmp += test[i];
        }
    }
    else if (scan_pos == 37)
    {
        for (i = 0; i <= 37; i++)
        {
#pragma HLS UNROLL
            d64_base_cost_tmp += test[i];
        }
    }
    else if (scan_pos == 38)
    {
        for (i = 0; i <= 38; i++)
        {
#pragma HLS UNROLL
            d64_base_cost_tmp += test[i];
        }
    }
    else if (scan_pos == 39)
    {
        for (i = 0; i <= 39; i++)
        {
#pragma HLS UNROLL
            d64_base_cost_tmp += test[i];
        }
    }
    else if (scan_pos == 40)
    {
        for (i = 0; i <= 40; i++)
        {
#pragma HLS UNROLL
            d64_base_cost_tmp += test[i];
        }
    }
    else if (scan_pos == 41)
    {
        for (i = 0; i <= 41; i++)
        {
#pragma HLS UNROLL
            d64_base_cost_tmp += test[i];
        }
    }
    else if (scan_pos == 42)
    {
        for (i = 0; i <= 42; i++)
        {
#pragma HLS UNROLL
            d64_base_cost_tmp += test[i];
        }
    }
    else if (scan_pos == 43)
    {
        for (i = 0; i <= 43; i++)
        {
#pragma HLS UNROLL
            d64_base_cost_tmp += test[i];
        }
    }
    else if (scan_pos == 44)
    {
        for (i = 0; i <= 44; i++)
        {
#pragma HLS UNROLL
            d64_base_cost_tmp += test[i];
        }
    }
    else if (scan_pos == 45)
    {
        for (i = 0; i <= 45; i++)
        {
#pragma HLS UNROLL
            d64_base_cost_tmp += test[i];
        }
    }
    else if (scan_pos == 46)
    {
        for (i = 0; i <= 46; i++)
        {
#pragma HLS UNROLL
            d64_base_cost_tmp += test[i];
        }
    }
    else if (scan_pos == 47)
    {
        for (i = 0; i <= 47; i++)
        {
#pragma HLS UNROLL
            d64_base_cost_tmp += test[i];
        }
    }
    else if (scan_pos == 48)
    {
        for (i = 0; i <= 48; i++)
        {
#pragma HLS UNROLL
            d64_base_cost_tmp += test[i];
        }
    }
    else if (scan_pos == 49)
    {
        for (i = 0; i <= 49; i++)
        {
#pragma HLS UNROLL
            d64_base_cost_tmp += test[i];
        }
    }
    else if (scan_pos == 50)
    {
        for (i = 0; i <= 50; i++)
        {
#pragma HLS UNROLL
            d64_base_cost_tmp += test[i];
        }
    }
    else if (scan_pos == 51)
    {
        for (i = 0; i <= 51; i++)
        {
#pragma HLS UNROLL
            d64_base_cost_tmp += test[i];
        }
    }
    else if (scan_pos == 52)
    {
        for (i = 0; i <= 52; i++)
        {
#pragma HLS UNROLL
            d64_base_cost_tmp += test[i];
        }
    }
    else if (scan_pos == 53)
    {
        for (i = 0; i <= 53; i++)
        {
#pragma HLS UNROLL
            d64_base_cost_tmp += test[i];
        }
    }
    else if (scan_pos == 54)
    {
        for (i = 0; i <= 54; i++)
        {
#pragma HLS UNROLL
            d64_base_cost_tmp += test[i];
        }
    }
    else if (scan_pos == 55)
    {
        for (i = 0; i <= 55; i++)
        {
#pragma HLS UNROLL
            d64_base_cost_tmp += test[i];
        }
    }
    else if (scan_pos == 56)
    {
        for (i = 0; i <= 56; i++)
        {
#pragma HLS UNROLL
            d64_base_cost_tmp += test[i];
        }
    }
    else if (scan_pos == 57)
    {
        for (i = 0; i <= 57; i++)
        {
#pragma HLS UNROLL
            d64_base_cost_tmp += test[i];
        }
    }
    else if (scan_pos == 58)
    {
        for (i = 0; i <= 58; i++)
        {
#pragma HLS UNROLL
            d64_base_cost_tmp += test[i];
        }
    }
    else if (scan_pos == 59)
    {
        for (i = 0; i <= 59; i++)
        {
#pragma HLS UNROLL
            d64_base_cost_tmp += test[i];
        }
    }
    else if (scan_pos == 60)
    {
        for (i = 0; i <= 60; i++)
        {
#pragma HLS UNROLL
            d64_base_cost_tmp += test[i];
        }
    }
    else if (scan_pos == 61)
    {
        for (i = 0; i <= 61; i++)
        {
#pragma HLS UNROLL
            d64_base_cost_tmp += test[i];
        }
    }
    else if (scan_pos == 62)
    {
        for (i = 0; i <= 62; i++)
        {
#pragma HLS UNROLL
            d64_base_cost_tmp += test[i];
        }
    }
    else if (scan_pos == 63)
    {
        for (i = 0; i <= 63; i++)
        {
#pragma HLS UNROLL
            d64_base_cost_tmp += test[i];
        }
    }

    *d64_base_cost = d64_base_cost_tmp;
}
void cal_base_cost(s64 d64_base_cost[64], s64 mat_coded_cost[64], s64 mat_uncoded_cost[64], s64 d64_cost_last_zero[64], U24 lev_opt[64])
{
#pragma HLS PIPELINE II=1
#pragma HLS ARRAY_PARTITION variable=d64_cost_last_zero complete dim=1
#pragma HLS ARRAY_PARTITION variable=mat_coded_cost complete dim=1
#pragma HLS ARRAY_PARTITION variable=d64_base_cost complete dim=1
#pragma HLS ARRAY_PARTITION variable=lev_opt complete dim=1
#pragma HLS ARRAY_PARTITION variable=mat_uncoded_cost complete dim=1
    int scan_pos = 0;
    int i = 0;
    s64 d64_base_cost_tmp[64] = { 0 };
    s64 cost_tmp[64] = { 0 };
    for (i = 0; i < 64; i++) {
#pragma HLS UNROLL
        cost_tmp[i] = mat_coded_cost[i] - mat_uncoded_cost[i] + d64_cost_last_zero[i] * ((!(!lev_opt[i])));
    }
    for (scan_pos = 0; scan_pos < 64; scan_pos+=8) {
#pragma HLS UNROLL
        cal_cost_core(cost_tmp, scan_pos, &d64_base_cost_tmp[scan_pos]);
    }
    for (scan_pos = 1; scan_pos < 64; scan_pos += 8) {
#pragma HLS UNROLL
        d64_base_cost_tmp[scan_pos] += d64_base_cost_tmp[scan_pos - 1] + cost_tmp[scan_pos];
    }
    for (scan_pos = 2; scan_pos < 64; scan_pos += 8) {
#pragma HLS UNROLL
        d64_base_cost_tmp[scan_pos] += d64_base_cost_tmp[scan_pos - 1] + cost_tmp[scan_pos];
    }
    for (scan_pos = 3; scan_pos < 64; scan_pos += 8) {
#pragma HLS UNROLL
        d64_base_cost_tmp[scan_pos] += d64_base_cost_tmp[scan_pos - 1] + cost_tmp[scan_pos];
    }
    for (scan_pos = 4; scan_pos < 64; scan_pos += 8) {
#pragma HLS UNROLL
        d64_base_cost_tmp[scan_pos] += d64_base_cost_tmp[scan_pos - 1] + cost_tmp[scan_pos];
    }
    for (scan_pos = 5; scan_pos < 64; scan_pos += 8) {
#pragma HLS UNROLL
        d64_base_cost_tmp[scan_pos] += d64_base_cost_tmp[scan_pos - 1] + cost_tmp[scan_pos];
    }
    for (scan_pos = 6; scan_pos < 64; scan_pos += 8) {
#pragma HLS UNROLL
        d64_base_cost_tmp[scan_pos] += d64_base_cost_tmp[scan_pos - 1] + cost_tmp[scan_pos];
    }
    for (scan_pos = 7; scan_pos < 64; scan_pos += 8) {
#pragma HLS UNROLL
        d64_base_cost_tmp[scan_pos] += d64_base_cost_tmp[scan_pos - 1] + cost_tmp[scan_pos];
    }

    for (scan_pos = 0; scan_pos < 64; scan_pos++) {
#pragma HLS UNROLL
        d64_base_cost[scan_pos] += d64_base_cost_tmp[scan_pos];
    }
}
void compare_core(int* idx, s64 a[8], int i)
{
#pragma HLS FUNCTION_INSTANTIATE variable=i
#pragma HLS PIPELINE II=1
#pragma HLS ARRAY_PARTITION variable=a complete dim=1
    int tmp[8] = { 0 };
#pragma HLS ARRAY_PARTITION variable=tmp complete dim=1
    int idx_tmp = 0;
    for (int j = 0; j < 8; j++)
    {
#pragma HLS UNROLL
        if (a[j] >= a[i])
        {
            tmp[j] = 1;
        }
        idx_tmp += !!(tmp[j]);
    }
    *idx = idx_tmp;
}
s64 compare8(s64 a[8], int idx_in[8], int* idx_out)
{
#pragma HLS PIPELINE II=1
#pragma HLS ARRAY_PARTITION variable=a complete dim=1
    int idx[8] = { 0 };
#pragma HLS ARRAY_PARTITION variable=idx complete dim=1
    int i;
    s64 smallest = 0;
    for (i = 0; i < 8; i++)
    {
#pragma HLS UNROLL
        compare_core(&idx[i], a, i);
        if (idx[i] == 8)
        {
            smallest = a[i];
            *idx_out = idx_in[i];
        }
    }
    return smallest;
}
int compare_top(s64 data[64], s64* best_data)
{
#pragma HLS ARRAY_PARTITION variable=data complete dim=1
#pragma HLS PIPELINE II=1
    int i = 0;
    s64 best_data_tmp = 0;
    int best_idx = 0;
    int data_idx[64] = { 0 };
#pragma HLS ARRAY_PARTITION variable=data_idx complete dim=1
    for (i = 0; i < 64; i++)
    {
#pragma HLS UNROLL
        data_idx[i] = i;
    }
    s64 tmp_data[8] = { 0 };
    int tmp_idx[8] = { 0 };
#pragma HLS ARRAY_PARTITION variable=tmp_idx complete dim=1
#pragma HLS ARRAY_PARTITION variable=tmp_data complete dim=1
    for (i = 0; i < 8; i++)
    {
#pragma HLS UNROLL
        tmp_data[i] = compare8(data + i * 8, data_idx + i * 8, &tmp_idx[i]);
    }
    best_data_tmp = compare8(tmp_data, tmp_idx, &best_idx);
    *best_data = best_data_tmp;
    return best_idx;
}
int RDOQ_FUNC_NAME(rdoq_top, W, H)(RDOQ_MODEL* model_cnt, int qp, s64 d_lambda, int is_intra, s16 src_coef[SIZE], s16 dst_tmp[SIZE], int cu_width_log2, int cu_height_log2, int ch_type, int bit_depth, int rdoq_est_cbf[NUM_QT_CBF_CTX][2], s32 rdoq_est_run_local[24][2], s32 rdoq_est_level_local[24][2], s32 rdoq_est_last_local[2][6][12][2])
{
#pragma HLS PIPELINE II=1
#pragma HLS ARRAY_PARTITION variable = src_coef complete dim = 0
#pragma HLS ARRAY_PARTITION variable = dst_tmp complete dim = 0
#pragma HLS ARRAY_PARTITION variable = rdoq_est_run_local complete dim = 0
#pragma HLS ARRAY_PARTITION variable = rdoq_est_level_local complete dim = 0
#pragma HLS ARRAY_PARTITION variable = rdoq_est_last_local complete dim = 0
    U14 quant_scale[80] = { (16302), (15024), (13777), (12634), (11626), (10624), (9742), (8958), (8192), (7512), (6889), (6305), (5793), (5303), (4878), (4467), (4091), (3756), (3444), (3161), (2894), (2654), (2435), (2235), (2048), (1878), (1722), (1579), (1449), (1329), (1218), (1117), (1024), (939), (861), (790), (724), (664), (609), (558), (512), (470), (430), (395), (362), (332), (304), (279), (256), (235), (215), (197), (181), (166), (152), (140), (128), (116), (108), (99), (91), (83), (76), (69), (64), (59), (54), (49), (45), (41), (38), (35), (32), (30), (27), (25), (23), (21), (19), (18) };

    const U14 scale = quant_scale[qp];
    const int ns_shift = ((cu_width_log2 + cu_height_log2) & 1) ? 7 : 0;
    const int ns_scale = ((cu_width_log2 + cu_height_log2) & 1) ? 181 : 1;
    const int ns_offset = ((cu_width_log2 + cu_height_log2) & 1) ? (1 << (ns_shift - 1)) : 0;
    const int q_value = (scale * ns_scale + ns_offset) >> ns_shift;
    const int log2_size = (cu_width_log2 + cu_height_log2) >> 1;
    const int tr_shift = get_transform_shift(bit_depth, log2_size);
    const int max_num_coef = SIZE;


    u16 scan[SIZE] = {
    0,1,8,16,9,2,3,10,17,24,32,25,18,11,4,5,12,19,26,33,40,48,41,34,27,20,13,6,7,14,21,28,35,42,49,56,57,50,43,36,29,22,15,23,30,37,44,51,58,59,52,45,38,31,39,46,53,60,61,54,47,55,62,63
    };
    const int ctx_last = (ch_type == Y_C) ? 0 : 1;
    const int q_bits = QUANT_SHIFT + tr_shift;
    int num_nz_coef = 0;
    int scan_pos;
    int last_idx_flag = 0;
    s64 best_data = 0;
    s32 ctx_qt_cbf;
    int best_last_idx_p1 = 0;
    s16 tmp_coef[SIZE] = { 0 };
    U36 tmp_level_double[SIZE] = { 0 };
    s16 tmp_dst_coef[SIZE] = { 0 };
    const s64 lambda = (d_lambda * (1 << 15) + 500)/1024;
    U30 err_scale_tbl[80][6] = { 32932, 65865, 131731, 263462, 526925, 1053850, 35734, 71468, 142936, 285873, 571747, 1143495, 38968, 77937, 155874, 311749, 623498, 1246996, 42494, 84988, 169976, 339953, 679906, 1359812, 46178, 92356, 184713, 369427, 738855, 1477711, 50533, 101067, 202135, 404270, 808540, 1617081, 55108, 110217, 220435, 440871, 881742, 1763484, 59932, 119864, 239728, 479456, 958912, 1917824, 65536, 131072, 262144, 524288, 1048576, 2097152, 71468, 142936, 285873, 571747, 1143495, 2286990, 77931, 155863, 311726, 623452, 1246905, 2493811, 85150, 170300, 340600, 681200, 1362400, 2724800, 92675, 185351, 370703, 741406, 1482812, 2965625, 101239, 202478, 404956, 809912, 1619825, 3239650, 110059, 220119, 440238, 880477, 1760954, 3521908, 120186, 240372, 480744, 961488, 1922976, 3845952, 131232, 262464, 524928, 1049857, 2099715, 4199430, 142936, 285873, 571747, 1143495, 2286990, 4573980, 155885, 311771, 623543, 1247086, 2494173, 4988347, 169842, 339684, 679368, 1358736, 2717473, 5434947, 185511, 371023, 742046, 1484093, 2968187, 5936374, 202287, 404574, 809149, 1618299, 3236599, 6473198, 220480, 440961, 881923, 1763846, 3527693, 7055387, 240210, 480421, 960842, 1921685, 3843371, 7686742, 262144, 524288, 1048576, 2097152, 4194304, 8388608, 285873, 571747, 1143495, 2286990, 4573980, 9147960, 311771, 623543, 1247086, 2494173, 4988347, 9976695, 340006, 680013, 1360027, 2720055, 5440110, 10880221, 370511, 741022, 1482045, 2964090, 5928181, 11856362, 403966, 807932, 1615864, 3231728, 6463457, 12926914, 440780, 881561, 1763122, 3526245, 7052491, 14104982, 480636, 961272, 1922545, 3845091, 7690183, 15380366, 524288, 1048576, 2097152, 4194304, 8388608, 16777216, 571747, 1143495, 2286990, 4573980, 9147960, 18295920, 623543, 1247086, 2494173, 4988347, 9976695, 19953390, 679583, 1359166, 2718333, 5436667, 10873334, 21746669, 741534, 1483068, 2966137, 5932275, 11864550, 23729101, 808540, 1617081, 3234162, 6468324, 12936648, 25873296, 881561, 1763122, 3526245, 7052491, 14104982, 28209965, 962134, 1924268, 3848537, 7697074, 15394148, 30788296, 1048576, 2097152, 4194304, 8388608, 16777216, 33554432, 1142278, 2284557, 4569114, 9138228, 18276456, 36552913, 1248537, 2497074, 4994148, 9988296, 19976592, 39953184, 1359166, 2718333, 5436667, 10873334, 21746669, 43493339, 1483068, 2966137, 5932275, 11864550, 23729101, 47458202, 1617081, 3234162, 6468324, 12936648, 25873296, 51746593, 1766022, 3532045, 7064090, 14128181, 28256363, 56512727, 1924268, 3848537, 7697074, 15394148, 30788296, 61576592, 2097152, 4194304, 8388608, 16777216, 33554432, 67108864, 2284557, 4569114, 9138228, 18276456, 36552913, 73105826, 2497074, 4994148, 9988296, 19976592, 39953184, 79906368, 2725233, 5450466, 10900932, 21801864, 43603728, 87207457, 2966137, 5932275, 11864550, 23729101, 47458202, 94916404, 3234162, 6468324, 12936648, 25873296, 51746593, 103493187, 3532045, 7064090, 14128181, 28256363, 56512727, 113025455, 3834792, 7669584, 15339168, 30678337, 61356675, 122713351, 4194304, 8388608, 16777216, 33554432, 67108864, 134217728, 4628197, 9256395, 18512790, 37025580, 74051160, 148102320, 4971026, 9942053, 19884107, 39768215, 79536431, 159072862, 5422938, 10845877, 21691754, 43383508, 86767016, 173534032, 5899680, 11799360, 23598721, 47197442, 94394885, 188789771, 6468324, 12936648, 25873296, 51746593, 103493187, 206986375, 7064090, 14128181, 28256363, 56512727, 113025455, 226050910, 7780737, 15561475, 31122951, 62245902, 124491805, 248983611, 8388608, 16777216, 33554432, 67108864, 134217728, 268435456, 9099506, 18199013, 36398027, 72796055, 145592111, 291184223, 9942053, 19884107, 39768215, 79536431, 159072862, 318145725, 10956549, 21913098, 43826196, 87652393, 175304787, 350609575, 11930464, 23860929, 47721858, 95443717, 190887435, 381774870, 13094412, 26188824, 52377649, 104755299, 209510599, 419021199, 14128181, 28256363, 56512727, 113025455, 226050910, 452101820, 15339168, 30678337, 61356675, 122713351, 245426702, 490853405, 16777216, 33554432, 67108864, 134217728, 268435456, 536870912, 17895697, 35791394, 71582788, 143165576, 286331153, 572662306, 19884107, 39768215, 79536431, 159072862, 318145725, 636291451, 21474836, 42949672, 85899345, 171798691, 343597383, 687194767, 23342213, 46684427, 93368854, 186737708, 373475417, 746950834, 25565281, 51130563, 102261126, 204522252, 409044504, 818089008, 28256363, 56512727, 113025455, 226050910, 452101820, 904203641, 29826161, 59652323, 119304647, 238609294, 477218588, 954437176 };
    s64 err_scale = err_scale_tbl[qp][log2_size - 1];
    s64 d64_best_cost = 0;
    s64 d64_base_cost[SIZE] = { 0 };
    s64 d64_coded_cost = 0;
    s64 d64_uncoded_cost = 0;

#pragma HLS ARRAY_PARTITION variable=scan complete dim=0
#pragma HLS ARRAY_PARTITION variable=tmp_coef complete dim=0
#pragma HLS ARRAY_PARTITION variable=tmp_level_double complete dim=0
#pragma HLS ARRAY_PARTITION variable=tmp_dst_coef complete dim=0
#pragma HLS ARRAY_PARTITION variable=d64_base_cost complete dim=0

    /* ===== quantization ===== */
    for (scan_pos = 0; scan_pos < max_num_coef; scan_pos++)
    {
#pragma HLS UNROLL
        u32 blk_pos = scan[scan_pos];
        U36 level_double = src_coef[blk_pos];
        U24 max_abs_level;
        U1 lower_int;
        U36 temp_level;
        temp_level = ((s64)COM_ABS(src_coef[blk_pos]) * (s64)q_value);
        level_double = (int)COM_MIN(((s64)temp_level), (s64)COM_INT32_MAX - (1 << (s64)(q_bits - 1)));
        tmp_level_double[blk_pos] = level_double;
        max_abs_level = (u32)(level_double >> q_bits);
        lower_int = ((level_double - ((s64)max_abs_level << q_bits)) < (1 << (s64)(q_bits - 1))) ? 1 : 0;
        if (!lower_int)
        {
            max_abs_level++;
        }
        tmp_coef[blk_pos] = src_coef[blk_pos] > 0 ? (s16)max_abs_level : -(s16)(max_abs_level);
    }

    if (!is_intra && ch_type == Y_C)
    {
        d64_best_cost = 0;
        for (scan_pos = 0; scan_pos < max_num_coef; scan_pos++)
        {
#pragma HLS UNROLL
            d64_base_cost[scan_pos] = 0;
        }
    }
    else
    {
        ctx_qt_cbf = ch_type;
        d64_best_cost = GET_I_COST(rdoq_est_cbf[ctx_qt_cbf][0], lambda);
        for (scan_pos = 0; scan_pos < max_num_coef; scan_pos++)
        {
#pragma HLS UNROLL
            d64_base_cost[scan_pos] = GET_I_COST(rdoq_est_cbf[ctx_qt_cbf][1], lambda);
        }
    }

    U24 mat_prev_level[SIZE] = { 0 };
    U1 mat_run[SIZE] = { 0 };
    mat_prev_level[0] = 6;
#pragma HLS ARRAY_PARTITION variable=mat_prev_level complete dim=0
#pragma HLS ARRAY_PARTITION variable=mat_run complete dim=0

    for (scan_pos = 1; scan_pos < max_num_coef; scan_pos++)
    {
#pragma HLS UNROLL
        u32 blk_pos = scan[scan_pos - 1];

        if (COM_ABS(tmp_coef[blk_pos]))
        {
            mat_prev_level[scan_pos] = COM_ABS(tmp_coef[blk_pos]);
            mat_run[scan_pos] = 0;
        }
        else
        {
            mat_prev_level[scan_pos] = mat_prev_level[scan_pos - 1];
            mat_run[scan_pos] = 1;
        }
    }

    s64 mat_uncoded_cost[SIZE] = { 0 };
    s64 mat_coded_cost[SIZE] = { 0 };
    U24 lev_opt[SIZE] = { 0 };
#pragma HLS ARRAY_PARTITION variable=mat_uncoded_cost complete dim=0
#pragma HLS ARRAY_PARTITION variable=mat_coded_cost complete dim=0
#pragma HLS ARRAY_PARTITION variable=lev_opt complete dim=0

    for (scan_pos = 0; scan_pos < max_num_coef; scan_pos++)
    {
#pragma HLS UNROLL
        u32 blk_pos = scan[scan_pos];
        U24 level;
		U5 ctx_run = ((COM_MIN((U24)(mat_prev_level[scan_pos] - 1), (U24)(5))) << 1) + (ch_type == Y_C ? 0 : 12);

        int iflast = (scan_pos == max_num_coef - 1);
        level = get_coded_level_rl(&d64_uncoded_cost, &d64_coded_cost, tmp_level_double[blk_pos], COM_ABS(tmp_coef[blk_pos]),
            mat_run[scan_pos], (u16)ctx_run, (u16)ctx_run, q_bits, err_scale, lambda, iflast, rdoq_est_run_local, rdoq_est_level_local);
        tmp_dst_coef[blk_pos] = (s16)(tmp_coef[blk_pos] < 0 ? -(s16)(level) : (s16)level);

        mat_uncoded_cost[scan_pos] = d64_uncoded_cost;
        mat_coded_cost[scan_pos] = d64_coded_cost;
        lev_opt[scan_pos] = level;
    }

    s64 d64_cost_last_zero[SIZE] = { 0 };
    d64_cost_last_zero[0] = 0;
    int ace_log2 = 0;
#pragma HLS ARRAY_PARTITION variable=d64_cost_last_zero complete dim=0

    for (scan_pos = 1; scan_pos < max_num_coef; scan_pos++)
    {
#pragma HLS UNROLL
        u32 blk_pos = scan[scan_pos];
        if ((scan_pos) < 2)
        {
            ace_log2 = 0;
        }
        else if ((scan_pos) < 4)
        {
            ace_log2 = 1;
        }
        else if ((scan_pos) < 8)
        {
            ace_log2 = 2;
        }
        else if ((scan_pos) < 16)
        {
            ace_log2 = 3;
        }
        else if ((scan_pos) < 32)
        {
            ace_log2 = 4;
        }
        else if ((scan_pos) < 64)
        {
            ace_log2 = 5;
        }
        else if ((scan_pos) < 128)
        {
            ace_log2 = 6;
        }
        else if ((scan_pos) < 256)
        {
            ace_log2 = 7;
        }
        else if ((scan_pos) < 512)
        {
            ace_log2 = 8;
        }
        else if ((scan_pos) < 1024)
        {
            ace_log2 = 9;
        }
        else if ((scan_pos) < 2048)
        {
            ace_log2 = 10;
        }
        else if ((scan_pos) < 4096)
        {
            ace_log2 = 11;
        }
        else
        {
            ace_log2 = 11;
        }
		d64_cost_last_zero[scan_pos] = GET_I_COST(rdoq_est_last_local[ctx_last][COM_MIN((U24)(mat_prev_level[scan_pos - 1] - 1), (U24)(5))][ace_log2][0], lambda);
    }
    cal_base_cost(d64_base_cost, mat_coded_cost, mat_uncoded_cost, d64_cost_last_zero, lev_opt);
    s64 d64_cur_is_last_cost[SIZE] = { 0 };
#pragma HLS ARRAY_PARTITION variable=d64_cur_is_last_cost complete dim=0
    for (scan_pos = 0; scan_pos < max_num_coef; scan_pos++)
    {
#pragma HLS UNROLL
        u32 blk_pos = scan[scan_pos];
        if ((scan_pos + 1) < 2)
        {
            ace_log2 = 0;
        }
        else if ((scan_pos + 1) < 4)
        {
            ace_log2 = 1;
        }
        else if ((scan_pos + 1) < 8)
        {
            ace_log2 = 2;
        }
        else if ((scan_pos + 1) < 16)
        {
            ace_log2 = 3;
        }
        else if ((scan_pos + 1) < 32)
        {
            ace_log2 = 4;
        }
        else if ((scan_pos + 1) < 64)
        {
            ace_log2 = 5;
        }
        else if ((scan_pos + 1) < 128)
        {
            ace_log2 = 6;
        }
        else if ((scan_pos + 1) < 256)
        {
            ace_log2 = 7;
        }
        else if ((scan_pos + 1) < 512)
        {
            ace_log2 = 8;
        }
        else if ((scan_pos + 1) < 1024)
        {
            ace_log2 = 9;
        }
        else if ((scan_pos + 1) < 2048)
        {
            ace_log2 = 10;
        }
        else if ((scan_pos + 1) < 4096)
        {
            ace_log2 = 11;
        }
        else
        {
            ace_log2 = 11;
        }
        U24 prev_level = mat_prev_level[scan_pos];
		s64 d64_cost_last_one = GET_I_COST(rdoq_est_last_local[ctx_last][COM_MIN((U24)(prev_level - 1), (U24)(5))][ace_log2][1], lambda);
        d64_cur_is_last_cost[scan_pos] = d64_base_cost[scan_pos] + d64_cost_last_one;
    }

    RDOQ_FUNC_NAME(model_update, W, H)(model_cnt->last1, model_cnt->last2, model_cnt->run_rdoq, model_cnt->level, mat_run, mat_prev_level, ch_type, max_num_coef);

    last_idx_flag = compare_top(d64_cur_is_last_cost, &best_data);
    if (best_data < d64_best_cost)
    {
        best_last_idx_p1 = last_idx_flag + 1;
    }
    int mask[SIZE] = { 0 };
#pragma HLS ARRAY_PARTITION variable=mask complete dim=0

    for (scan_pos = 0; scan_pos < SIZE; scan_pos++)
    {
#pragma HLS UNROLL
        u32 blk_pos = scan[scan_pos];
        if (scan_pos < max_num_coef)
        {
            if (scan_pos < best_last_idx_p1)
                mask[blk_pos] = 1;
            else
                mask[blk_pos] = 0;
        }
        else
        {
            mask[blk_pos] = 0;
        }
        dst_tmp[blk_pos] = tmp_dst_coef[blk_pos] * mask[blk_pos];
        num_nz_coef += !!(dst_tmp[blk_pos]);
    }
    return num_nz_coef;
}
#else
int RDOQ_FUNC_NAME(rdoq_top, W, H)(RDOQ_MODEL* model_cnt, int qp, s64 d_lambda, int is_intra, s16 src_coef[SIZE], s16 dst_tmp[SIZE], int cu_width_log2, 
	int cu_height_log2, int ch_type, int bit_depth, RDOQ_ARRAY* rdoq_array)
{
#pragma HLS PIPELINE II=1
#pragma HLS ARRAY_PARTITION variable = src_coef complete dim = 0
#pragma HLS ARRAY_PARTITION variable = dst_tmp complete dim = 0

    U14 quant_scale[80] = { (16302), (15024), (13777), (12634), (11626), (10624), (9742), (8958), (8192), (7512), (6889), (6305), (5793), (5303), (4878), (4467), (4091), (3756), (3444), (3161), (2894), (2654), (2435), (2235), (2048), (1878), (1722), (1579), (1449), (1329), (1218), (1117), (1024), (939), (861), (790), (724), (664), (609), (558), (512), (470), (430), (395), (362), (332), (304), (279), (256), (235), (215), (197), (181), (166), (152), (140), (128), (116), (108), (99), (91), (83), (76), (69), (64), (59), (54), (49), (45), (41), (38), (35), (32), (30), (27), (25), (23), (21), (19), (18) };

    const U14 scale = quant_scale[qp];
    const int ns_shift = ((cu_width_log2 + cu_height_log2) & 1) ? 7 : 0;
    const int ns_scale = ((cu_width_log2 + cu_height_log2) & 1) ? 181 : 1;
    const int ns_offset = ((cu_width_log2 + cu_height_log2) & 1) ? (1 << (ns_shift - 1)) : 0;
    const int q_value = (scale * ns_scale + ns_offset) >> ns_shift;
    const int log2_size = (cu_width_log2 + cu_height_log2) >> 1;
    const int tr_shift = get_transform_shift(bit_depth, log2_size);
#if (W == 16) && (H == 4)
    const int max_num_coef = 32;
#elif (W == 4) && (H == 16)
    const int max_num_coef = 32;
#elif (W == 16) && (H == 8)
    const int max_num_coef = 64;
#elif (W == 8) && (H == 16)
    const int max_num_coef = 64;
#elif (W == 16) && (H == 16)
    const int max_num_coef = 64;
#else
    const int max_num_coef = SIZE;
#endif

    u16 scan[SIZE] = {
#if (W == 4) && (H == 4)
    0,1,4,8,5,2,3,6,9,12,13,10,7,11,14,15
#elif (W == 4) && (H == 8)
    0,1,4,8,5,2,3,6,9,12,16,13,10,7,11,14,17,20,24,21,18,15,19,22,25,28,29,26,23,27,30,31
#elif (W == 8) && (H == 4)
    0,1,8,16,9,2,3,10,17,24,25,18,11,4,5,12,19,26,27,20,13,6,7,14,21,28,29,22,15,23,30,31
#elif (W == 8) && (H == 8)
    0,1,8,16,9,2,3,10,17,24,32,25,18,11,4,5,12,19,26,33,40,48,41,34,27,20,13,6,7,14,21,28,35,42,49,56,57,50,43,36,29,22,15,23,30,37,44,51,58,59,52,45,38,31,39,46,53,60,61,54,47,55,62,63
#elif (W == 16) && (H == 4)
0,1,16,32,17,2,3,18,33,48,49,34,19,4,5,20,35,50,51,36,21,6,7,22,37,52,53,38,23,8,9,24,39,54,55,40,25,10,11,26,41,56,57,42,27,12,13,28,43,58,59,44,29,14,15,30,45,60,61,46,31,47,62,63
#elif (W == 4) && (H == 16)
0,1,4,8,5,2,3,6,9,12,16,13,10,7,11,14,17,20,24,21,18,15,19,22,25,28,32,29,26,23,27,30,33,36,40,37,34,31,35,38,41,44,48,45,42,39,43,46,49,52,56,53,50,47,51,54,57,60,61,58,55,59,62,63
#elif (W == 16) && (H == 8)
0,1,16,32,17,2,3,18,33,48,64,49,34,19,4,5,20,35,50,65,80,96,81,66,51,36,21,6,7,22,37,52,67,82,97,112,113,98,83,68,53,38,23,8,9,24,39,54,69,84,99,114,115,100,85,70,55,40,25,10,11,26,41,56,71,86,101,116,117,102,87,72,57,42,27,12,13,28,43,58,73,88,103,118,119,104,89,74,59,44,29,14,15,30,45,60,75,90,105,120,121,106,91,76,61,46,31,47,62,77,92,107,122,123,108,93,78,63,79,94,109,124,125,110,95,111,126,127
#elif (W == 8) && (H == 16)
0,1,8,16,9,2,3,10,17,24,32,25,18,11,4,5,12,19,26,33,40,48,41,34,27,20,13,6,7,14,21,28,35,42,49,56,64,57,50,43,36,29,22,15,23,30,37,44,51,58,65,72,80,73,66,59,52,45,38,31,39,46,53,60,67,74,81,88,96,89,82,75,68,61,54,47,55,62,69,76,83,90,97,104,112,105,98,91,84,77,70,63,71,78,85,92,99,106,113,120,121,114,107,100,93,86,79,87,94,101,108,115,122,123,116,109,102,95,103,110,117,124,125,118,111,119,126,127
#elif (W == 16) && (H == 16)
0,1,16,32,17,2,3,18,33,48,64,49,34,19,4,5,20,35,50,65,80,96,81,66,51,36,21,6,7,22,37,52,67,82,97,112,128,113,98,83,68,53,38,23,8,9,24,39,54,69,84,99,114,129,144,160,145,130,115,100,85,70,55,40,25,10,11,26,41,56,71,86,101,116,131,146,161,176,192,177,162,147,132,117,102,87,72,57,42,27,12,13,28,43,58,73,88,103,118,133,148,163,178,193,208,224,209,194,179,164,149,134,119,104,89,74,59,44,29,14,15,30,45,60,75,90,105,120,135,150,165,180,195,210,225,240,241,226,211,196,181,166,151,136,121,106,91,76,61,46,31,47,62,77,92,107,122,137,152,167,182,197,212,227,242,243,228,213,198,183,168,153,138,123,108,93,78,63,79,94,109,124,139,154,169,184,199,214,229,244,245,230,215,200,185,170,155,140,125,110,95,111,126,141,156,171,186,201,216,231,246,247,232,217,202,187,172,157,142,127,143,158,173,188,203,218,233,248,249,234,219,204,189,174,159,175,190,205,220,235,250,251,236,221,206,191,207,222,237,252,253,238,223,239,254,255
#else
        0
#endif
};
    const int ctx_last = (ch_type == Y_C) ? 0 : 1;
    const int q_bits = QUANT_SHIFT + tr_shift;
    int num_nz_coef = 0;
    int scan_pos;
    s32 ctx_qt_cbf;
    int best_last_idx_p1 = 0;
    s16 tmp_coef[SIZE] = { 0 };
    U36 tmp_level_double[SIZE] = { 0 };
    s16 tmp_dst_coef[SIZE] = { 0 };
    const s64 lambda = (d_lambda * (1 << 15) + (1 << (LAMBDA_SCALE - 1))) >> LAMBDA_SCALE;
    //const s64 lambda = (s64)(d_lambda * (double)(1 << SCALE_BITS) + 0.5);
    U30 err_scale_tbl[80][6] = { 32932, 65865, 131731, 263462, 526925, 1053850, 35734, 71468, 142936, 285873, 571747, 1143495, 38968, 77937, 155874, 311749, 623498, 1246996, 42494, 84988, 169976, 339953, 679906, 1359812, 46178, 92356, 184713, 369427, 738855, 1477711, 50533, 101067, 202135, 404270, 808540, 1617081, 55108, 110217, 220435, 440871, 881742, 1763484, 59932, 119864, 239728, 479456, 958912, 1917824, 65536, 131072, 262144, 524288, 1048576, 2097152, 71468, 142936, 285873, 571747, 1143495, 2286990, 77931, 155863, 311726, 623452, 1246905, 2493811, 85150, 170300, 340600, 681200, 1362400, 2724800, 92675, 185351, 370703, 741406, 1482812, 2965625, 101239, 202478, 404956, 809912, 1619825, 3239650, 110059, 220119, 440238, 880477, 1760954, 3521908, 120186, 240372, 480744, 961488, 1922976, 3845952, 131232, 262464, 524928, 1049857, 2099715, 4199430, 142936, 285873, 571747, 1143495, 2286990, 4573980, 155885, 311771, 623543, 1247086, 2494173, 4988347, 169842, 339684, 679368, 1358736, 2717473, 5434947, 185511, 371023, 742046, 1484093, 2968187, 5936374, 202287, 404574, 809149, 1618299, 3236599, 6473198, 220480, 440961, 881923, 1763846, 3527693, 7055387, 240210, 480421, 960842, 1921685, 3843371, 7686742, 262144, 524288, 1048576, 2097152, 4194304, 8388608, 285873, 571747, 1143495, 2286990, 4573980, 9147960, 311771, 623543, 1247086, 2494173, 4988347, 9976695, 340006, 680013, 1360027, 2720055, 5440110, 10880221, 370511, 741022, 1482045, 2964090, 5928181, 11856362, 403966, 807932, 1615864, 3231728, 6463457, 12926914, 440780, 881561, 1763122, 3526245, 7052491, 14104982, 480636, 961272, 1922545, 3845091, 7690183, 15380366, 524288, 1048576, 2097152, 4194304, 8388608, 16777216, 571747, 1143495, 2286990, 4573980, 9147960, 18295920, 623543, 1247086, 2494173, 4988347, 9976695, 19953390, 679583, 1359166, 2718333, 5436667, 10873334, 21746669, 741534, 1483068, 2966137, 5932275, 11864550, 23729101, 808540, 1617081, 3234162, 6468324, 12936648, 25873296, 881561, 1763122, 3526245, 7052491, 14104982, 28209965, 962134, 1924268, 3848537, 7697074, 15394148, 30788296, 1048576, 2097152, 4194304, 8388608, 16777216, 33554432, 1142278, 2284557, 4569114, 9138228, 18276456, 36552913, 1248537, 2497074, 4994148, 9988296, 19976592, 39953184, 1359166, 2718333, 5436667, 10873334, 21746669, 43493339, 1483068, 2966137, 5932275, 11864550, 23729101, 47458202, 1617081, 3234162, 6468324, 12936648, 25873296, 51746593, 1766022, 3532045, 7064090, 14128181, 28256363, 56512727, 1924268, 3848537, 7697074, 15394148, 30788296, 61576592, 2097152, 4194304, 8388608, 16777216, 33554432, 67108864, 2284557, 4569114, 9138228, 18276456, 36552913, 73105826, 2497074, 4994148, 9988296, 19976592, 39953184, 79906368, 2725233, 5450466, 10900932, 21801864, 43603728, 87207457, 2966137, 5932275, 11864550, 23729101, 47458202, 94916404, 3234162, 6468324, 12936648, 25873296, 51746593, 103493187, 3532045, 7064090, 14128181, 28256363, 56512727, 113025455, 3834792, 7669584, 15339168, 30678337, 61356675, 122713351, 4194304, 8388608, 16777216, 33554432, 67108864, 134217728, 4628197, 9256395, 18512790, 37025580, 74051160, 148102320, 4971026, 9942053, 19884107, 39768215, 79536431, 159072862, 5422938, 10845877, 21691754, 43383508, 86767016, 173534032, 5899680, 11799360, 23598721, 47197442, 94394885, 188789771, 6468324, 12936648, 25873296, 51746593, 103493187, 206986375, 7064090, 14128181, 28256363, 56512727, 113025455, 226050910, 7780737, 15561475, 31122951, 62245902, 124491805, 248983611, 8388608, 16777216, 33554432, 67108864, 134217728, 268435456, 9099506, 18199013, 36398027, 72796055, 145592111, 291184223, 9942053, 19884107, 39768215, 79536431, 159072862, 318145725, 10956549, 21913098, 43826196, 87652393, 175304787, 350609575, 11930464, 23860929, 47721858, 95443717, 190887435, 381774870, 13094412, 26188824, 52377649, 104755299, 209510599, 419021199, 14128181, 28256363, 56512727, 113025455, 226050910, 452101820, 15339168, 30678337, 61356675, 122713351, 245426702, 490853405, 16777216, 33554432, 67108864, 134217728, 268435456, 536870912, 17895697, 35791394, 71582788, 143165576, 286331153, 572662306, 19884107, 39768215, 79536431, 159072862, 318145725, 636291451, 21474836, 42949672, 85899345, 171798691, 343597383, 687194767, 23342213, 46684427, 93368854, 186737708, 373475417, 746950834, 25565281, 51130563, 102261126, 204522252, 409044504, 818089008, 28256363, 56512727, 113025455, 226050910, 452101820, 904203641, 29826161, 59652323, 119304647, 238609294, 477218588, 954437176 };

    s64 err_scale = err_scale_tbl[qp][log2_size - 1];
    s64 d64_best_cost = 0;
    s64 d64_base_cost[SIZE] = { 0 };
    s64 d64_coded_cost = 0;
    s64 d64_uncoded_cost = 0;

#pragma HLS ARRAY_PARTITION variable=scan complete dim=0
#pragma HLS ARRAY_PARTITION variable=tmp_coef complete dim=0
#pragma HLS ARRAY_PARTITION variable=tmp_level_double complete dim=0
#pragma HLS ARRAY_PARTITION variable=tmp_dst_coef complete dim=0
#pragma HLS ARRAY_PARTITION variable=d64_base_cost complete dim=0

    /* ===== quantization ===== */
    for (scan_pos = 0; scan_pos < max_num_coef; scan_pos++)
    {
#pragma HLS UNROLL
        u32 blk_pos = scan[scan_pos];
        U36 level_double = src_coef[blk_pos];
        U24 max_abs_level;
        U1 lower_int;
        U36 temp_level;
        temp_level = ((s64)COM_ABS(src_coef[blk_pos]) * (s64)q_value);
        level_double = (int)COM_MIN(((s64)temp_level), (s64)COM_INT32_MAX - (s64)(1 << (q_bits - 1)));
        tmp_level_double[blk_pos] = level_double;
        max_abs_level = (u32)(level_double >> q_bits);
        lower_int = ((level_double - ((s64)max_abs_level << q_bits)) < (s64)(1 << (q_bits - 1))) ? 1 : 0;
        if (!lower_int)
        {
            max_abs_level++;
        }
        tmp_coef[blk_pos] = src_coef[blk_pos] > 0 ? (s16)max_abs_level : -(s16)(max_abs_level);
    }

    if (!is_intra && ch_type == Y_C)
    {
        d64_best_cost = 0;
        for (scan_pos = 0; scan_pos < max_num_coef; scan_pos++)
        {
#pragma HLS UNROLL
            d64_base_cost[scan_pos] = 0;
        }
    }
    else
    {
        ctx_qt_cbf = ch_type;
        d64_best_cost = GET_I_COST(rdoq_array->rdoq_est_cbf[ctx_qt_cbf][0], lambda);
		//d64_best_cost = d64_best_cost >> 24;
        for (scan_pos = 0; scan_pos < max_num_coef; scan_pos++)
        {
#pragma HLS UNROLL
            d64_base_cost[scan_pos] = GET_I_COST(rdoq_array->rdoq_est_cbf[ctx_qt_cbf][1], lambda);
			//d64_base_cost[scan_pos] = d64_base_cost[scan_pos] >> 24;
        }
    }

    U24 mat_prev_level[SIZE] = { 0 };
    U1 mat_run[SIZE] = { 0 };
    mat_prev_level[0] = 6;
#pragma HLS ARRAY_PARTITION variable=mat_prev_level complete dim=0
#pragma HLS ARRAY_PARTITION variable=mat_run complete dim=0

    for (scan_pos = 1; scan_pos < max_num_coef; scan_pos++)
    {
#pragma HLS UNROLL
        u32 blk_pos = scan[scan_pos - 1];

        if (COM_ABS(tmp_coef[blk_pos]))
        {
            mat_prev_level[scan_pos] = COM_ABS(tmp_coef[blk_pos]);
            mat_run[scan_pos] = 0;
        }
        else
        {
            mat_prev_level[scan_pos] = mat_prev_level[scan_pos - 1];
            mat_run[scan_pos] = 1;
        }
    }

    s64 mat_uncoded_cost[SIZE] = { 0 };
    s64 mat_coded_cost[SIZE] = { 0 };
    U24 lev_opt[SIZE] = { 0 };
#pragma HLS ARRAY_PARTITION variable=mat_uncoded_cost complete dim=0
#pragma HLS ARRAY_PARTITION variable=mat_coded_cost complete dim=0
#pragma HLS ARRAY_PARTITION variable=lev_opt complete dim=0

    for (scan_pos = 0; scan_pos < max_num_coef; scan_pos++)
    {
#pragma HLS UNROLL
        u32 blk_pos = scan[scan_pos];
        U24 level;
        U5 ctx_run = ((COM_MIN((U24)(mat_prev_level[scan_pos] - 1), (U24)5)) << 1) + (ch_type == Y_C ? 0 : 12);
#if (W == 16) || (H == 16)
        int iflast = 0;
#else
        int iflast = (scan_pos == max_num_coef - 1);
#endif
        level = get_coded_level_rl(&d64_uncoded_cost, &d64_coded_cost, tmp_level_double[blk_pos], COM_ABS(tmp_coef[blk_pos]),
            mat_run[scan_pos], (u16)ctx_run, (u16)ctx_run, q_bits, err_scale, lambda, iflast, rdoq_array->rdoq_est_run, rdoq_array->rdoq_est_level);
        tmp_dst_coef[blk_pos] = (s16)(tmp_coef[blk_pos] < 0 ? -(s16)(level) : (s16)level);

        mat_uncoded_cost[scan_pos] = d64_uncoded_cost;
        mat_coded_cost[scan_pos] = d64_coded_cost;
        lev_opt[scan_pos] = level;
    }

    s64 d64_cost_last_zero[SIZE] = { 0 };
    d64_cost_last_zero[0] = 0;
    int ace_log2 = 0;
#pragma HLS ARRAY_PARTITION variable=d64_cost_last_zero complete dim=0

    for (scan_pos = 1; scan_pos < max_num_coef; scan_pos++)
    {
#pragma HLS UNROLL
        u32 blk_pos = scan[scan_pos];
        if ((scan_pos) < 2)
        {
            ace_log2 = 0;
        }
        else if ((scan_pos) < 4)
        {
            ace_log2 = 1;
        }
        else if ((scan_pos) < 8)
        {
            ace_log2 = 2;
        }
        else if ((scan_pos) < 16)
        {
            ace_log2 = 3;
        }
        else if ((scan_pos) < 32)
        {
            ace_log2 = 4;
        }
        else if ((scan_pos) < 64)
        {
            ace_log2 = 5;
        }
        else if ((scan_pos) < 128)
        {
            ace_log2 = 6;
        }
        else if ((scan_pos) < 256)
        {
            ace_log2 = 7;
        }
        else if ((scan_pos) < 512)
        {
            ace_log2 = 8;
        }
        else if ((scan_pos) < 1024)
        {
            ace_log2 = 9;
        }
        else if ((scan_pos) < 2048)
        {
            ace_log2 = 10;
        }
        else if ((scan_pos) < 4096)
        {
            ace_log2 = 11;
        }
        else
        {
            ace_log2 = 11;
        }
		d64_cost_last_zero[scan_pos] = GET_I_COST(rdoq_array->rdoq_est_last[ctx_last][COM_MIN((U24)(mat_prev_level[scan_pos - 1] - 1), (U24)(5))][ace_log2][0], lambda);
		//d64_cost_last_zero[scan_pos] = d64_cost_last_zero[scan_pos] >> 24;
    }
    for (scan_pos = 0; scan_pos < max_num_coef; scan_pos++)
    {
#pragma HLS UNROLL
        for (int i = 0; i <= scan_pos; i++)
        {
#pragma HLS UNROLL
            d64_base_cost[scan_pos] += mat_coded_cost[i] - mat_uncoded_cost[i] + d64_cost_last_zero[i] * !!(lev_opt[i]);
        }
    }
    for (scan_pos = 0; scan_pos < max_num_coef; scan_pos++)
    {
#pragma HLS UNROLL
        u32 blk_pos = scan[scan_pos];
        if ((scan_pos + 1) < 2)
        {
            ace_log2 = 0;
        }
        else if ((scan_pos + 1) < 4)
        {
            ace_log2 = 1;
        }
        else if ((scan_pos + 1) < 8)
        {
            ace_log2 = 2;
        }
        else if ((scan_pos + 1) < 16)
        {
            ace_log2 = 3;
        }
        else if ((scan_pos + 1) < 32)
        {
            ace_log2 = 4;
        }
        else if ((scan_pos + 1) < 64)
        {
            ace_log2 = 5;
        }
        else if ((scan_pos + 1) < 128)
        {
            ace_log2 = 6;
        }
        else if ((scan_pos + 1) < 256)
        {
            ace_log2 = 7;
        }
        else if ((scan_pos + 1) < 512)
        {
            ace_log2 = 8;
        }
        else if ((scan_pos + 1) < 1024)
        {
            ace_log2 = 9;
        }
        else if ((scan_pos + 1) < 2048)
        {
            ace_log2 = 10;
        }
        else if ((scan_pos + 1) < 4096)
        {
            ace_log2 = 11;
        }
        else
        {
            ace_log2 = 11;
        }
        U24 prev_level = mat_prev_level[scan_pos];
		s64 d64_cost_last_one = GET_I_COST(rdoq_array->rdoq_est_last[ctx_last][COM_MIN((U24)(prev_level - 1), (U24)(5))][ace_log2][1], lambda);
		//d64_cost_last_one = d64_cost_last_one >> 24;
        if (COM_ABS(tmp_dst_coef[blk_pos]))
        {
            s64 d64_cur_is_last_cost = d64_base_cost[scan_pos] + d64_cost_last_one;

            if (d64_cur_is_last_cost < d64_best_cost)
            {
                d64_best_cost = d64_cur_is_last_cost;
                best_last_idx_p1 = scan_pos + 1;
            }
        }
    }
    RDOQ_FUNC_NAME(model_update, W, H)(model_cnt->last1, model_cnt->last2, model_cnt->run_rdoq, model_cnt->level, mat_run, mat_prev_level, ch_type, max_num_coef);

    int mask[SIZE] = { 0 };
#pragma HLS ARRAY_PARTITION variable=mask complete dim=0

    for (scan_pos = 0; scan_pos < SIZE; scan_pos++)
    {
#pragma HLS UNROLL
        u32 blk_pos = scan[scan_pos];
        if (scan_pos < max_num_coef)
        {
            if (scan_pos < best_last_idx_p1)
                mask[blk_pos] = 1;
            else
                mask[blk_pos] = 0;
        }
        else
        {
            mask[blk_pos] = 0;
        }
        dst_tmp[blk_pos] = tmp_dst_coef[blk_pos] * mask[blk_pos];
        num_nz_coef += !!(dst_tmp[blk_pos]);
    }
    return num_nz_coef;
}
#endif
#elif ((W == 32) && (H == 32)) || ((W == 16) && (H == 32)) || ((W == 8) && (H == 32)) || ((W == 32) && (H == 16)) || ((W == 32) && (H == 8)) || ((W == 32) && (H == 4)) || ((W == 4) && (H == 32))

int RDOQ_FUNC_NAME(clean_nz, W, H)(s16 dst_tmp[H][W], int x_pos, int y_pos, int pzb)
{
    int i;
    int j;
    int mask[H][W] = { 0 };
#pragma HLS ARRAY_PARTITION variable = mask complete dim = 0
    int num_nz_coef = 0;
    for (i = 0; i < H; i++)
    {
#pragma HLS UNROLL
        for (j = 0; j < W; j++)
        {
#pragma HLS UNROLL
            if (!pzb)
                mask[i][j] = 0;
            else
            {
                if ((i + j) > x_pos)
                {
                    mask[i][j] = 0;
                }
                else if ((i + j) == x_pos)
                {
                    if (x_pos & 1)
                    {
                        if (i > y_pos)
                        {
                            mask[i][j] = 0;
                        }
                        else
                        {
                            mask[i][j] = 1;
                        }
                    }
                    else
                    {
                        if (i < y_pos)
                        {
                            mask[i][j] = 0;
                        }
                        else
                        {
                            mask[i][j] = 1;
                        }
                    }
                }
                else
                {
                    mask[i][j] = 1;
                }
            }
            dst_tmp[i][j] = dst_tmp[i][j] * mask[i][j];
            num_nz_coef += !!(dst_tmp[i][j]);
        }
    }
    return num_nz_coef;
}
void RDOQ_FUNC_NAME(read_data, W, H)(s16 coef64[MAT_SIZE_32_1], s16 src_coef[H][W], int i)
{
    int a, b;
#if (W <= H)
    if (i == 0)
    {
        coef64[0] = src_coef[0][0];
        coef64[1] = src_coef[0][1];
        coef64[2] = src_coef[1][0];
    }
    else if (i == 1)
    {
        for (a = 2; a >= 0; a--)
        {
#pragma HLS UNROLL
            coef64[2 - a] = src_coef[a][2 - a];
        }
        for (b = 0; b < 4; b++)
        {
#pragma HLS UNROLL
            coef64[3 + b] = src_coef[b][3 - b];
        }
    }
#if (W > 4)
    else if (i == 2)
    {
        for (a = 4; a >= 0; a--)
        {
#pragma HLS UNROLL
            coef64[4 - a] = src_coef[a][4 - a];
        }
        for (b = 0; b < 6; b++)
        {
#pragma HLS UNROLL
            coef64[5 + b] = src_coef[b][5 - b];
        }
    }
    else if (i == 3)
    {
        for (a = 6; a >= 0; a--)
        {
#pragma HLS UNROLL
            coef64[6 - a] = src_coef[a][6 - a];
        }
        for (b = 0; b < 8; b++)
        {
#pragma HLS UNROLL
            coef64[7 + b] = src_coef[b][7 - b];
        }
    }
#if (W > 8)
    else if (i == 4)
    {
        for (a = 8; a >= 0; a--)
        {
#pragma HLS UNROLL
            coef64[8 - a] = src_coef[a][8 - a];
        }
        for (b = 0; b < 10; b++)
        {
#pragma HLS UNROLL
            coef64[9 + b] = src_coef[b][9 - b];
        }
    }
    else if (i == 5)
    {
        for (a = 10; a >= 0; a--)
        {
#pragma HLS UNROLL
            coef64[10 - a] = src_coef[a][10 - a];
        }
        for (b = 0; b < 12; b++)
        {
#pragma HLS UNROLL
            coef64[11 + b] = src_coef[b][11 - b];
        }
    }
    else if (i == 6)
    {
        for (a = 12; a >= 0; a--)
        {
#pragma HLS UNROLL
            coef64[12 - a] = src_coef[a][12 - a];
        }
        for (b = 0; b < 14; b++)
        {
#pragma HLS UNROLL
            coef64[13 + b] = src_coef[b][13 - b];
        }
    }
    else if (i == 7)
    {
        for (a = 14; a >= 0; a--)
        {
#pragma HLS UNROLL
            coef64[14 - a] = src_coef[a][14 - a];
        }
        for (b = 0; b < 16; b++)
        {
#pragma HLS UNROLL
            coef64[15 + b] = src_coef[b][15 - b];
        }
    }
#if (W > 16)
    else if (i == 8)
    {
        for (a = 16; a >= 0; a--)
        {
#pragma HLS UNROLL
            coef64[16 - a] = src_coef[a][16 - a];
        }
        for (b = 0; b < 18; b++)
        {
#pragma HLS UNROLL
            coef64[17 + b] = src_coef[b][17 - b];
        }
    }
    else if (i == 9)
    {
        for (a = 18; a >= 0; a--)
        {
#pragma HLS UNROLL
            coef64[18 - a] = src_coef[a][18 - a];
        }
        for (b = 0; b < 20; b++)
        {
#pragma HLS UNROLL
            coef64[19 + b] = src_coef[b][19 - b];
        }
    }
    else if (i == 10)
    {
        for (a = 20; a >= 0; a--)
        {
#pragma HLS UNROLL
            coef64[20 - a] = src_coef[a][20 - a];
        }
        for (b = 0; b < 22; b++)
        {
#pragma HLS UNROLL
            coef64[21 + b] = src_coef[b][21 - b];
        }
    }
    else if (i == 11)
    {
        for (a = 22; a >= 0; a--)
        {
#pragma HLS UNROLL
            coef64[22 - a] = src_coef[a][22 - a];
        }
        for (b = 0; b < 24; b++)
        {
#pragma HLS UNROLL
            coef64[23 + b] = src_coef[b][23 - b];
        }
    }
    else if (i == 12)
    {
        for (a = 24; a >= 0; a--)
        {
#pragma HLS UNROLL
            coef64[24 - a] = src_coef[a][24 - a];
        }
        for (b = 0; b < 26; b++)
        {
#pragma HLS UNROLL
            coef64[25 + b] = src_coef[b][25 - b];
        }
    }
    else if (i == 13)
    {
        for (a = 26; a >= 0; a--)
        {
#pragma HLS UNROLL
            coef64[26 - a] = src_coef[a][26 - a];
        }
        for (b = 0; b < 28; b++)
        {
#pragma HLS UNROLL
            coef64[27 + b] = src_coef[b][27 - b];
        }
    }
    else if (i == 14)
    {
        for (a = 28; a >= 0; a--)
        {
#pragma HLS UNROLL
            coef64[28 - a] = src_coef[a][28 - a];
        }
        for (b = 0; b < 30; b++)
        {
#pragma HLS UNROLL
            coef64[29 + b] = src_coef[b][29 - b];
        }
    }
    else if (i == 15)
    {
        for (a = 30; a >= 0; a--)
        {
#pragma HLS UNROLL
            coef64[30 - a] = src_coef[a][30 - a];
        }
        for (b = 0; b < 32; b++)
        {
#pragma HLS UNROLL
            coef64[31 + b] = src_coef[b][31 - b];
        }
    }
#endif
#endif
#endif
#else
if (i == 0)
{
    coef64[0] = src_coef[0][0];
    coef64[1] = src_coef[0][1];
    coef64[2] = src_coef[1][0];
}
else if (i == 1)
{
    for (a = 2; a >= 0; a--)
    {
#pragma HLS UNROLL
        coef64[2 - a] = src_coef[a][2 - a];
    }
    for (b = 0; b < 4; b++)
    {
#pragma HLS UNROLL
        coef64[3 + b] = src_coef[b][3 - b];
    }
}
#if (H == 4)
else if (i == 2)
{
    for (a = 3; a >= 0; a--)
    {
#pragma HLS UNROLL
        coef64[3 - a] = src_coef[a][4 - a];
    }
    for (b = 0; b < 4; b++)
    {
#pragma HLS UNROLL
        coef64[4 + b] = src_coef[b][5 - b];
    }
}
else if (i == 3)
{
    for (a = 3; a >= 0; a--)
    {
#pragma HLS UNROLL
        coef64[3 - a] = src_coef[a][6 - a];
    }
    for (b = 0; b < 4; b++)
    {
#pragma HLS UNROLL
        coef64[4 + b] = src_coef[b][7 - b];
    }
}
else if (i == 4)
{
    for (a = 3; a >= 0; a--)
    {
#pragma HLS UNROLL
        coef64[3 - a] = src_coef[a][8 - a];
    }
    for (b = 0; b < 4; b++)
    {
#pragma HLS UNROLL
        coef64[4 + b] = src_coef[b][9 - b];
    }
}
else if (i == 5)
{
    for (a = 3; a >= 0; a--)
    {
#pragma HLS UNROLL
        coef64[3 - a] = src_coef[a][10 - a];
    }
    for (b = 0; b < 4; b++)
    {
#pragma HLS UNROLL
        coef64[4 + b] = src_coef[b][11 - b];
    }
}
else if (i == 6)
{
    for (a = 3; a >= 0; a--)
    {
#pragma HLS UNROLL
        coef64[3 - a] = src_coef[a][12 - a];
    }
    for (b = 0; b < 4; b++)
    {
#pragma HLS UNROLL
        coef64[4 + b] = src_coef[b][13 - b];
    }
}
else if (i == 7)
{
    for (a = 3; a >= 0; a--)
    {
#pragma HLS UNROLL
        coef64[3 - a] = src_coef[a][14 - a];
    }
    for (b = 0; b < 4; b++)
    {
#pragma HLS UNROLL
        coef64[4 + b] = src_coef[b][15 - b];
    }
}
else if (i == 8)
{
    for (a = 3; a >= 0; a--)
    {
#pragma HLS UNROLL
        coef64[3 - a] = src_coef[a][16 - a];
    }
    for (b = 0; b < 4; b++)
    {
#pragma HLS UNROLL
        coef64[4 + b] = src_coef[b][17 - b];
    }
}
else if (i == 9)
{
    for (a = 3; a >= 0; a--)
    {
#pragma HLS UNROLL
        coef64[3 - a] = src_coef[a][18 - a];
    }
    for (b = 0; b < 4; b++)
    {
#pragma HLS UNROLL
        coef64[4 + b] = src_coef[b][19 - b];
    }
}
else if (i == 10)
{
    for (a = 3; a >= 0; a--)
    {
#pragma HLS UNROLL
        coef64[3 - a] = src_coef[a][20 - a];
    }
    for (b = 0; b < 4; b++)
    {
#pragma HLS UNROLL
        coef64[4 + b] = src_coef[b][21 - b];
    }
}
else if (i == 11)
{
    for (a = 3; a >= 0; a--)
    {
#pragma HLS UNROLL
        coef64[3 - a] = src_coef[a][22 - a];
    }
    for (b = 0; b < 4; b++)
    {
#pragma HLS UNROLL
        coef64[4 + b] = src_coef[b][23 - b];
    }
}
else if (i == 12)
{
    for (a = 3; a >= 0; a--)
    {
#pragma HLS UNROLL
        coef64[3 - a] = src_coef[a][24 - a];
    }
    for (b = 0; b < 4; b++)
    {
#pragma HLS UNROLL
        coef64[4 + b] = src_coef[b][25 - b];
    }
}
else if (i == 13)
{
    for (a = 3; a >= 0; a--)
    {
#pragma HLS UNROLL
        coef64[3 - a] = src_coef[a][26 - a];
    }
    for (b = 0; b < 4; b++)
    {
#pragma HLS UNROLL
        coef64[4 + b] = src_coef[b][27 - b];
    }
}
else if (i == 14)
{
    for (a = 3; a >= 0; a--)
    {
#pragma HLS UNROLL
        coef64[3 - a] = src_coef[a][28 - a];
    }
    for (b = 0; b < 4; b++)
    {
#pragma HLS UNROLL
        coef64[4 + b] = src_coef[b][29 - b];
    }
}
else if (i == 15)
{
    for (a = 3; a >= 0; a--)
    {
#pragma HLS UNROLL
        coef64[3 - a] = src_coef[a][30 - a];
    }
    for (b = 0; b < 4; b++)
    {
#pragma HLS UNROLL
        coef64[4 + b] = src_coef[b][31 - b];
    }
}
#else
else if (i == 2)
{
    for (a = 4; a >= 0; a--)
    {
#pragma HLS UNROLL
        coef64[4 - a] = src_coef[a][4 - a];
    }
    for (b = 0; b < 6; b++)
    {
#pragma HLS UNROLL
        coef64[5 + b] = src_coef[b][5 - b];
    }
}
else if (i == 3)
{
    for (a = 6; a >= 0; a--)
    {
#pragma HLS UNROLL
        coef64[6 - a] = src_coef[a][6 - a];
    }
    for (b = 0; b < 8; b++)
    {
#pragma HLS UNROLL
        coef64[7 + b] = src_coef[b][7 - b];
    }
}
#if (H == 8)
else if (i == 4)
{
    for (a = 7; a >= 0; a--)
    {
#pragma HLS UNROLL
        coef64[7 - a] = src_coef[a][8 - a];
    }
    for (b = 0; b < 8; b++)
    {
#pragma HLS UNROLL
        coef64[8 + b] = src_coef[b][9 - b];
    }
}
else if (i == 5)
{
    for (a = 7; a >= 0; a--)
    {
#pragma HLS UNROLL
        coef64[7 - a] = src_coef[a][10 - a];
    }
    for (b = 0; b < 8; b++)
    {
#pragma HLS UNROLL
        coef64[8 + b] = src_coef[b][11 - b];
    }
}
else if (i == 6)
{
    for (a = 7; a >= 0; a--)
    {
#pragma HLS UNROLL
        coef64[7 - a] = src_coef[a][12 - a];
    }
    for (b = 0; b < 8; b++)
    {
#pragma HLS UNROLL
        coef64[8 + b] = src_coef[b][13 - b];
    }
}
else if (i == 7)
{
    for (a = 7; a >= 0; a--)
    {
#pragma HLS UNROLL
        coef64[7 - a] = src_coef[a][14 - a];
    }
    for (b = 0; b < 8; b++)
    {
#pragma HLS UNROLL
        coef64[8 + b] = src_coef[b][15 - b];
    }
}
else if (i == 8)
{
    for (a = 7; a >= 0; a--)
    {
#pragma HLS UNROLL
        coef64[7 - a] = src_coef[a][16 - a];
    }
    for (b = 0; b < 8; b++)
    {
#pragma HLS UNROLL
        coef64[8 + b] = src_coef[b][17 - b];
    }
}
else if (i == 9)
{
    for (a = 7; a >= 0; a--)
    {
#pragma HLS UNROLL
        coef64[7 - a] = src_coef[a][18 - a];
    }
    for (b = 0; b < 8; b++)
    {
#pragma HLS UNROLL
        coef64[8 + b] = src_coef[b][19 - b];
    }
}
else if (i == 10)
{
    for (a = 7; a >= 0; a--)
    {
#pragma HLS UNROLL
        coef64[7 - a] = src_coef[a][20 - a];
    }
    for (b = 0; b < 8; b++)
    {
#pragma HLS UNROLL
        coef64[8 + b] = src_coef[b][21 - b];
    }
}
else if (i == 11)
{
    for (a = 7; a >= 0; a--)
    {
#pragma HLS UNROLL
        coef64[7 - a] = src_coef[a][22 - a];
    }
    for (b = 0; b < 8; b++)
    {
#pragma HLS UNROLL
        coef64[8 + b] = src_coef[b][23 - b];
    }
}
else if (i == 12)
{
    for (a = 7; a >= 0; a--)
    {
#pragma HLS UNROLL
        coef64[7 - a] = src_coef[a][24 - a];
    }
    for (b = 0; b < 8; b++)
    {
#pragma HLS UNROLL
        coef64[8 + b] = src_coef[b][25 - b];
    }
}
else if (i == 13)
{
    for (a = 7; a >= 0; a--)
    {
#pragma HLS UNROLL
        coef64[7 - a] = src_coef[a][26 - a];
    }
    for (b = 0; b < 8; b++)
    {
#pragma HLS UNROLL
        coef64[8 + b] = src_coef[b][27 - b];
    }
}
else if (i == 14)
{
    for (a = 7; a >= 0; a--)
    {
#pragma HLS UNROLL
        coef64[7 - a] = src_coef[a][28 - a];
    }
    for (b = 0; b < 8; b++)
    {
#pragma HLS UNROLL
        coef64[8 + b] = src_coef[b][29 - b];
    }
}
else if (i == 15)
{
    for (a = 7; a >= 0; a--)
    {
#pragma HLS UNROLL
        coef64[7 - a] = src_coef[a][30 - a];
    }
    for (b = 0; b < 8; b++)
    {
#pragma HLS UNROLL
        coef64[8 + b] = src_coef[b][31 - b];
    }
}
#elif (H == 16)
else if (i == 4)
{
for (a = 8; a >= 0; a--)
{
#pragma HLS UNROLL
    coef64[8 - a] = src_coef[a][8 - a];
}
for (b = 0; b < 10; b++)
{
#pragma HLS UNROLL
    coef64[9 + b] = src_coef[b][9 - b];
}
}
else if (i == 5)
{
for (a = 10; a >= 0; a--)
{
#pragma HLS UNROLL
    coef64[10 - a] = src_coef[a][10 - a];
}
for (b = 0; b < 12; b++)
{
#pragma HLS UNROLL
    coef64[11 + b] = src_coef[b][11 - b];
}
}
else if (i == 6)
{
for (a = 12; a >= 0; a--)
{
#pragma HLS UNROLL
    coef64[12 - a] = src_coef[a][12 - a];
}
for (b = 0; b < 14; b++)
{
#pragma HLS UNROLL
    coef64[13 + b] = src_coef[b][13 - b];
}
}
else if (i == 7)
{
for (a = 14; a >= 0; a--)
{
#pragma HLS UNROLL
    coef64[14 - a] = src_coef[a][14 - a];
}
for (b = 0; b < 16; b++)
{
#pragma HLS UNROLL
    coef64[15 + b] = src_coef[b][15 - b];
}
}
else if (i == 8)
{
for (a = 15; a >= 0; a--)
{
#pragma HLS UNROLL
    coef64[15 - a] = src_coef[a][16 - a];
}
for (b = 0; b < 16; b++)
{
#pragma HLS UNROLL
    coef64[16 + b] = src_coef[b][17 - b];
}
}
else if (i == 9)
{
for (a = 15; a >= 0; a--)
{
#pragma HLS UNROLL
    coef64[15 - a] = src_coef[a][18 - a];
}
for (b = 0; b < 16; b++)
{
#pragma HLS UNROLL
    coef64[16 + b] = src_coef[b][19 - b];
}
}
else if (i == 10)
{
for (a = 15; a >= 0; a--)
{
#pragma HLS UNROLL
    coef64[15 - a] = src_coef[a][20 - a];
}
for (b = 0; b < 16; b++)
{
#pragma HLS UNROLL
    coef64[16 + b] = src_coef[b][21 - b];
}
}
else if (i == 11)
{
for (a = 15; a >= 0; a--)
{
#pragma HLS UNROLL
    coef64[15 - a] = src_coef[a][22 - a];
}
for (b = 0; b < 16; b++)
{
#pragma HLS UNROLL
    coef64[16 + b] = src_coef[b][23 - b];
}
}
else if (i == 12)
{
for (a = 15; a >= 0; a--)
{
#pragma HLS UNROLL
    coef64[15 - a] = src_coef[a][24 - a];
}
for (b = 0; b < 16; b++)
{
#pragma HLS UNROLL
    coef64[16 + b] = src_coef[b][25 - b];
}
}
else if (i == 13)
{
for (a = 15; a >= 0; a--)
{
#pragma HLS UNROLL
    coef64[15 - a] = src_coef[a][26 - a];
}
for (b = 0; b < 16; b++)
{
#pragma HLS UNROLL
    coef64[16 + b] = src_coef[b][27 - b];
}
}
else if (i == 14)
{
for (a = 15; a >= 0; a--)
{
#pragma HLS UNROLL
    coef64[15 - a] = src_coef[a][28 - a];
}
for (b = 0; b < 16; b++)
{
#pragma HLS UNROLL
    coef64[16 + b] = src_coef[b][29 - b];
}
}
else if (i == 15)
{
for (a = 15; a >= 0; a--)
{
#pragma HLS UNROLL
    coef64[15 - a] = src_coef[a][30 - a];
}
for (b = 0; b < 16; b++)
{
#pragma HLS UNROLL
    coef64[16 + b] = src_coef[b][31 - b];
}
}
#endif
#endif
#endif
}
void RDOQ_FUNC_NAME(write_data, W, H)(s16 dst_tmp[H][W], s16 dst64[MAT_SIZE_32_1], int i)
{
    int a, b;
#if (W <= H)
    if (i == 0)
    {
        dst_tmp[0][0] = dst64[0];
        dst_tmp[0][1] = dst64[1];
        dst_tmp[1][0] = dst64[2];
    }
    else if (i == 1)
    {
        for (a = 2; a >= 0; a--)
        {
#pragma HLS UNROLL
            dst_tmp[a][2 - a] = dst64[2 - a];
        }
        for (b = 0; b < 4; b++)
        {
#pragma HLS UNROLL
            dst_tmp[b][3 - b] = dst64[3 + b];
        }
    }
#if (W > 4)
    else if (i == 2)
    {
        for (a = 4; a >= 0; a--)
        {
#pragma HLS UNROLL
            dst_tmp[a][4 - a] = dst64[4 - a];
        }
        for (b = 0; b < 6; b++)
        {
#pragma HLS UNROLL
            dst_tmp[b][5 - b] = dst64[5 + b];
        }
    }
    else if (i == 3)
    {
        for (a = 6; a >= 0; a--)
        {
#pragma HLS UNROLL
            dst_tmp[a][6 - a] = dst64[6 - a];
        }
        for (b = 0; b < 8; b++)
        {
#pragma HLS UNROLL
            dst_tmp[b][7 - b] = dst64[7 + b];
        }
    }
#if (W > 8)
    else if (i == 4)
    {
        for (a = 8; a >= 0; a--)
        {
#pragma HLS UNROLL
            dst_tmp[a][8 - a] = dst64[8 - a];
        }
        for (b = 0; b < 10; b++)
        {
#pragma HLS UNROLL
            dst_tmp[b][9 - b] = dst64[9 + b];
        }
    }
    else if (i == 5)
    {
        for (a = 10; a >= 0; a--)
        {
#pragma HLS UNROLL
            dst_tmp[a][10 - a] = dst64[10 - a];
        }
        for (b = 0; b < 12; b++)
        {
#pragma HLS UNROLL
            dst_tmp[b][11 - b] = dst64[11 + b];
        }
    }
    else if (i == 6)
    {
        for (a = 12; a >= 0; a--)
        {
#pragma HLS UNROLL
            dst_tmp[a][12 - a] = dst64[12 - a];
        }
        for (b = 0; b < 14; b++)
        {
#pragma HLS UNROLL
            dst_tmp[b][13 - b] = dst64[13 + b];
        }
    }
    else if (i == 7)
    {
        for (a = 14; a >= 0; a--)
        {
#pragma HLS UNROLL
            dst_tmp[a][14 - a] = dst64[14 - a];
        }
        for (b = 0; b < 16; b++)
        {
#pragma HLS UNROLL
            dst_tmp[b][15 - b] = dst64[15 + b];
        }
    }
#if (W > 16)
    else if (i == 8)
    {
        for (a = 16; a >= 0; a--)
        {
#pragma HLS UNROLL
            dst_tmp[a][16 - a] = dst64[16 - a];
        }
        for (b = 0; b < 18; b++)
        {
#pragma HLS UNROLL
            dst_tmp[b][17 - b] = dst64[17 + b];
        }
    }
    else if (i == 9)
    {
        for (a = 18; a >= 0; a--)
        {
#pragma HLS UNROLL
            dst_tmp[a][18 - a] = dst64[18 - a];
        }
        for (b = 0; b < 20; b++)
        {
#pragma HLS UNROLL
            dst_tmp[b][19 - b] = dst64[19 + b];
        }
    }
    else if (i == 10)
    {
        for (a = 20; a >= 0; a--)
        {
#pragma HLS UNROLL
            dst_tmp[a][20 - a] = dst64[20 - a];
        }
        for (b = 0; b < 22; b++)
        {
#pragma HLS UNROLL
            dst_tmp[b][21 - b] = dst64[21 + b];
        }
    }
    else if (i == 11)
    {
        for (a = 22; a >= 0; a--)
        {
#pragma HLS UNROLL
            dst_tmp[a][22 - a] = dst64[22 - a];
        }
        for (b = 0; b < 24; b++)
        {
#pragma HLS UNROLL
            dst_tmp[b][23 - b] = dst64[23 + b];
        }
    }
    else if (i == 12)
    {
        for (a = 24; a >= 0; a--)
        {
#pragma HLS UNROLL
            dst_tmp[a][24 - a] = dst64[24 - a];
        }
        for (b = 0; b < 26; b++)
        {
#pragma HLS UNROLL
            dst_tmp[b][25 - b] = dst64[25 + b];
        }
    }
    else if (i == 13)
    {
        for (a = 26; a >= 0; a--)
        {
#pragma HLS UNROLL
            dst_tmp[a][26 - a] = dst64[26 - a];
        }
        for (b = 0; b < 28; b++)
        {
#pragma HLS UNROLL
            dst_tmp[b][27 - b] = dst64[27 + b];
        }
    }
    else if (i == 14)
    {
        for (a = 28; a >= 0; a--)
        {
#pragma HLS UNROLL
            dst_tmp[a][28 - a] = dst64[28 - a];
        }
        for (b = 0; b < 30; b++)
        {
#pragma HLS UNROLL
            dst_tmp[b][29 - b] = dst64[29 + b];
        }
    }
    else if (i == 15)
    {
        for (a = 30; a >= 0; a--)
        {
#pragma HLS UNROLL
            dst_tmp[a][30 - a] = dst64[30 - a];
        }
        for (b = 0; b < 32; b++)
        {
#pragma HLS UNROLL
            dst_tmp[b][31 - b] = dst64[31 + b];
        }
    }
#endif
#endif
#endif
#else
if (i == 0)
{
    dst_tmp[0][0] = dst64[0];
    dst_tmp[0][1] = dst64[1];
    dst_tmp[1][0] = dst64[2];
}
else if (i == 1)
{
    for (a = 2; a >= 0; a--)
    {
#pragma HLS UNROLL
        dst_tmp[a][2 - a] = dst64[2 - a];
    }
    for (b = 0; b < 4; b++)
    {
#pragma HLS UNROLL
        dst_tmp[b][3 - b] = dst64[3 + b];
    }
}
#if (H == 4)
else if (i == 2)
{
    for (a = 3; a >= 0; a--)
    {
#pragma HLS UNROLL
        dst_tmp[a][4 - a] = dst64[3 - a];
    }
    for (b = 0; b < 4; b++)
    {
#pragma HLS UNROLL
        dst_tmp[b][5 - b] = dst64[4 + b];
    }
}
else if (i == 3)
{
    for (a = 3; a >= 0; a--)
    {
#pragma HLS UNROLL
        dst_tmp[a][6 - a] = dst64[3 - a];
    }
    for (b = 0; b < 4; b++)
    {
#pragma HLS UNROLL
        dst_tmp[b][7 - b] = dst64[4 + b];
    }
}
else if (i == 4)
{
    for (a = 3; a >= 0; a--)
    {
#pragma HLS UNROLL
        dst_tmp[a][8 - a] = dst64[3 - a];
    }
    for (b = 0; b < 4; b++)
    {
#pragma HLS UNROLL
        dst_tmp[b][9 - b] = dst64[4 + b];
    }
}
else if (i == 5)
{
    for (a = 3; a >= 0; a--)
    {
#pragma HLS UNROLL
        dst_tmp[a][10 - a] = dst64[3 - a];
    }
    for (b = 0; b < 4; b++)
    {
#pragma HLS UNROLL
        dst_tmp[b][11 - b] = dst64[4 + b];
    }
}
else if (i == 6)
{
    for (a = 3; a >= 0; a--)
    {
#pragma HLS UNROLL
        dst_tmp[a][12 - a] = dst64[3 - a];
    }
    for (b = 0; b < 4; b++)
    {
#pragma HLS UNROLL
        dst_tmp[b][13 - b] = dst64[4 + b];
    }
}
else if (i == 7)
{
    for (a = 3; a >= 0; a--)
    {
#pragma HLS UNROLL
        dst_tmp[a][14 - a] = dst64[3 - a];
    }
    for (b = 0; b < 4; b++)
    {
#pragma HLS UNROLL
        dst_tmp[b][15 - b] = dst64[4 + b];
    }
}
else if (i == 8)
{
    for (a = 3; a >= 0; a--)
    {
#pragma HLS UNROLL
        dst_tmp[a][16 - a] = dst64[3 - a];
    }
    for (b = 0; b < 4; b++)
    {
#pragma HLS UNROLL
        dst_tmp[b][17 - b] = dst64[4 + b];
    }
}
else if (i == 9)
{
    for (a = 3; a >= 0; a--)
    {
#pragma HLS UNROLL
        dst_tmp[a][18 - a] = dst64[3 - a];
    }
    for (b = 0; b < 4; b++)
    {
#pragma HLS UNROLL
        dst_tmp[b][19 - b] = dst64[4 + b];
    }
}
else if (i == 10)
{
    for (a = 3; a >= 0; a--)
    {
#pragma HLS UNROLL
        dst_tmp[a][20 - a] = dst64[3 - a];
    }
    for (b = 0; b < 4; b++)
    {
#pragma HLS UNROLL
        dst_tmp[b][21 - b] = dst64[4 + b];
    }
}
else if (i == 11)
{
    for (a = 3; a >= 0; a--)
    {
#pragma HLS UNROLL
        dst_tmp[a][22 - a] = dst64[3 - a];
    }
    for (b = 0; b < 4; b++)
    {
#pragma HLS UNROLL
        dst_tmp[b][23 - b] = dst64[4 + b];
    }
}
else if (i == 12)
{
    for (a = 3; a >= 0; a--)
    {
#pragma HLS UNROLL
        dst_tmp[a][24 - a] = dst64[3 - a];
    }
    for (b = 0; b < 4; b++)
    {
#pragma HLS UNROLL
        dst_tmp[b][25 - b] = dst64[4 + b];
    }
}
else if (i == 13)
{
    for (a = 3; a >= 0; a--)
    {
#pragma HLS UNROLL
        dst_tmp[a][26 - a] = dst64[3 - a];
    }
    for (b = 0; b < 4; b++)
    {
#pragma HLS UNROLL
        dst_tmp[b][27 - b] = dst64[4 + b];
    }
}
else if (i == 14)
{
    for (a = 3; a >= 0; a--)
    {
#pragma HLS UNROLL
        dst_tmp[a][28 - a] = dst64[3 - a];
    }
    for (b = 0; b < 4; b++)
    {
#pragma HLS UNROLL
        dst_tmp[b][29 - b] = dst64[4 + b];
    }
}
else if (i == 15)
{
    for (a = 3; a >= 0; a--)
    {
#pragma HLS UNROLL
        dst_tmp[a][30 - a] = dst64[3 - a];
    }
    for (b = 0; b < 4; b++)
    {
#pragma HLS UNROLL
        dst_tmp[b][31 - b] = dst64[4 + b];
    }
}
#else
else if (i == 2)
{
    for (a = 4; a >= 0; a--)
    {
#pragma HLS UNROLL
        dst_tmp[a][4 - a] = dst64[4 - a];
    }
    for (b = 0; b < 6; b++)
    {
#pragma HLS UNROLL
        dst_tmp[b][5 - b] = dst64[5 + b];
    }
}
else if (i == 3)
{
    for (a = 6; a >= 0; a--)
    {
#pragma HLS UNROLL
        dst_tmp[a][6 - a] = dst64[6 - a];
    }
    for (b = 0; b < 8; b++)
    {
#pragma HLS UNROLL
        dst_tmp[b][7 - b] = dst64[7 + b];
    }
}
#if (H == 8)
else if (i == 4)
{
    for (a = 7; a >= 0; a--)
    {
#pragma HLS UNROLL
        dst_tmp[a][8 - a] = dst64[7 - a];
    }
    for (b = 0; b < 8; b++)
    {
#pragma HLS UNROLL
        dst_tmp[b][9 - b] = dst64[8 + b];
    }
}
else if (i == 5)
{
    for (a = 7; a >= 0; a--)
    {
#pragma HLS UNROLL
        dst_tmp[a][10 - a] = dst64[7 - a];
    }
    for (b = 0; b < 8; b++)
    {
#pragma HLS UNROLL
        dst_tmp[b][11 - b] = dst64[8 + b];
    }
}
else if (i == 6)
{
    for (a = 7; a >= 0; a--)
    {
#pragma HLS UNROLL
        dst_tmp[a][12 - a] = dst64[7 - a];
    }
    for (b = 0; b < 8; b++)
    {
#pragma HLS UNROLL
        dst_tmp[b][13 - b] = dst64[8 + b];
    }
}
else if (i == 7)
{
    for (a = 7; a >= 0; a--)
    {
#pragma HLS UNROLL
        dst_tmp[a][14 - a] = dst64[7 - a];
    }
    for (b = 0; b < 8; b++)
    {
#pragma HLS UNROLL
        dst_tmp[b][15 - b] = dst64[8 + b];
    }
}
else if (i == 8)
{
    for (a = 7; a >= 0; a--)
    {
#pragma HLS UNROLL
        dst_tmp[a][16 - a] = dst64[7 - a];
    }
    for (b = 0; b < 8; b++)
    {
#pragma HLS UNROLL
        dst_tmp[b][17 - b] = dst64[8 + b];
    }
}
else if (i == 9)
{
    for (a = 7; a >= 0; a--)
    {
#pragma HLS UNROLL
        dst_tmp[a][18 - a] = dst64[7 - a];
    }
    for (b = 0; b < 8; b++)
    {
#pragma HLS UNROLL
        dst_tmp[b][19 - b] = dst64[8 + b];
    }
}
else if (i == 10)
{
    for (a = 7; a >= 0; a--)
    {
#pragma HLS UNROLL
        dst_tmp[a][20 - a] = dst64[7 - a];
    }
    for (b = 0; b < 8; b++)
    {
#pragma HLS UNROLL
        dst_tmp[b][21 - b] = dst64[8 + b];
    }
}
else if (i == 11)
{
    for (a = 7; a >= 0; a--)
    {
#pragma HLS UNROLL
        dst_tmp[a][22 - a] = dst64[7 - a];
    }
    for (b = 0; b < 8; b++)
    {
#pragma HLS UNROLL
        dst_tmp[b][23 - b] = dst64[8 + b];
    }
}
else if (i == 12)
{
    for (a = 7; a >= 0; a--)
    {
#pragma HLS UNROLL
        dst_tmp[a][24 - a] = dst64[7 - a];
    }
    for (b = 0; b < 8; b++)
    {
#pragma HLS UNROLL
        dst_tmp[b][25 - b] = dst64[8 + b];
    }
}
else if (i == 13)
{
    for (a = 7; a >= 0; a--)
    {
#pragma HLS UNROLL
        dst_tmp[a][26 - a] = dst64[7 - a];
    }
    for (b = 0; b < 8; b++)
    {
#pragma HLS UNROLL
        dst_tmp[b][27 - b] = dst64[8 + b];
    }
}
else if (i == 14)
{
    for (a = 7; a >= 0; a--)
    {
#pragma HLS UNROLL
        dst_tmp[a][28 - a] = dst64[7 - a];
    }
    for (b = 0; b < 8; b++)
    {
#pragma HLS UNROLL
        dst_tmp[b][29 - b] = dst64[8 + b];
    }
}
else if (i == 15)
{
    for (a = 7; a >= 0; a--)
    {
#pragma HLS UNROLL
        dst_tmp[a][30 - a] = dst64[7 - a];
    }
    for (b = 0; b < 8; b++)
    {
#pragma HLS UNROLL
        dst_tmp[b][31 - b] = dst64[8 + b];
    }
}
#elif (H == 16)
else if (i == 4)
    {
    for (a = 8; a >= 0; a--)
    {
#pragma HLS UNROLL
        dst_tmp[a][8 - a] = dst64[8 - a];
    }
    for (b = 0; b < 10; b++)
    {
#pragma HLS UNROLL
        dst_tmp[b][9 - b] = dst64[9 + b];
    }
    }
else if (i == 5)
    {
    for (a = 10; a >= 0; a--)
    {
#pragma HLS UNROLL
        dst_tmp[a][10 - a] = dst64[10 - a];
    }
    for (b = 0; b < 12; b++)
    {
#pragma HLS UNROLL
        dst_tmp[b][11 - b] = dst64[11 + b];
    }
    }
else if (i == 6)
    {
    for (a = 12; a >= 0; a--)
    {
#pragma HLS UNROLL
        dst_tmp[a][12 - a] = dst64[12 - a];
    }
    for (b = 0; b < 14; b++)
    {
#pragma HLS UNROLL
        dst_tmp[b][13 - b] = dst64[13 + b];
    }
    }
else if (i == 7)
    {
    for (a = 14; a >= 0; a--)
    {
#pragma HLS UNROLL
        dst_tmp[a][14 - a] = dst64[14 - a];
    }
    for (b = 0; b < 16; b++)
    {
#pragma HLS UNROLL
        dst_tmp[b][15 - b] = dst64[15 + b];
    }
    }
else if (i == 8)
{
for (a = 15; a >= 0; a--)
{
#pragma HLS UNROLL
    dst_tmp[a][16 - a] = dst64[15 - a];
}
for (b = 0; b < 16; b++)
{
#pragma HLS UNROLL
    dst_tmp[b][17 - b] = dst64[16 + b];
}
}
else if (i == 9)
{
for (a = 15; a >= 0; a--)
{
#pragma HLS UNROLL
    dst_tmp[a][18 - a] = dst64[15 - a];
}
for (b = 0; b < 16; b++)
{
#pragma HLS UNROLL
    dst_tmp[b][19 - b] = dst64[16 + b];
}
}
else if (i == 10)
{
for (a = 15; a >= 0; a--)
{
#pragma HLS UNROLL
    dst_tmp[a][20 - a] = dst64[15 - a];
}
for (b = 0; b < 16; b++)
{
#pragma HLS UNROLL
    dst_tmp[b][21 - b] = dst64[16 + b];
}
}
else if (i == 11)
{
for (a = 15; a >= 0; a--)
{
#pragma HLS UNROLL
    dst_tmp[a][22 - a] = dst64[15 - a];
}
for (b = 0; b < 16; b++)
{
#pragma HLS UNROLL
    dst_tmp[b][23 - b] = dst64[16 + b];
}
}
else if (i == 12)
{
for (a = 15; a >= 0; a--)
{
#pragma HLS UNROLL
    dst_tmp[a][24 - a] = dst64[15 - a];
}
for (b = 0; b < 16; b++)
{
#pragma HLS UNROLL
    dst_tmp[b][25 - b] = dst64[16 + b];
}
}
else if (i == 13)
{
for (a = 15; a >= 0; a--)
{
#pragma HLS UNROLL
    dst_tmp[a][26 - a] = dst64[15 - a];
}
for (b = 0; b < 16; b++)
{
#pragma HLS UNROLL
    dst_tmp[b][27 - b] = dst64[16 + b];
}
}
else if (i == 14)
{
for (a = 15; a >= 0; a--)
{
#pragma HLS UNROLL
    dst_tmp[a][28 - a] = dst64[15 - a];
}
for (b = 0; b < 16; b++)
{
#pragma HLS UNROLL
    dst_tmp[b][29 - b] = dst64[16 + b];
}
}
else if (i == 15)
{
for (a = 15; a >= 0; a--)
{
#pragma HLS UNROLL
    dst_tmp[a][30 - a] = dst64[15 - a];
}
for (b = 0; b < 16; b++)
{
#pragma HLS UNROLL
    dst_tmp[b][31 - b] = dst64[16 + b];
}
}
#endif
#endif
#endif

}
void RDOQ_FUNC_NAME(rdoq_core, W, H)(RDOQ_MODEL* model_cnt, U24 last_level[W / 2 + 1], U24 prev_level[W / 2 + 1], int* pzb, s64 base_cost, int* x_pos, int* y_pos, s64 best_cost_out[W / 2 + 1], s64 last_cost[W / 2 + 1], int q_value, int cyc, int q_bits, s64 err_scale, s64 lambda, int is_intra, s16 coef64[MAT_SIZE_32_1], s16 dst_coef[MAT_SIZE_32_1], int ch_type, s32 rdoq_est_run_local[24][2], s32 rdoq_est_level_local[24][2], s32 rdoq_est_last_local[2][6][12][2])
{
#pragma HLS PIPELINE II=1
    const int ctx_last = (ch_type == Y_C) ? 0 : 1;
    int num_nz_coef = 0;
    int cnt = 4 * cyc + 3;
    int scan_pos;
    s64 d64_base_cost[MAT_SIZE_32_1] = { 0 };
    s64 d64_coded_cost = 0;
    s64 d64_uncoded_cost = 0;
    s16 tmp_coef[MAT_SIZE_32_1] = { 0 };
    U36 tmp_level_double[MAT_SIZE_32_1] = { 0 };
    s64 best_cost = best_cost_out[cyc];
#pragma HLS ARRAY_PARTITION variable = d64_base_cost complete dim = 0
#pragma HLS ARRAY_PARTITION variable = tmp_coef complete dim = 0
#pragma HLS ARRAY_PARTITION variable = tmp_level_double complete dim = 0

    int base_idx = (2 * cyc + 1) * cyc;

    for (scan_pos = 0; scan_pos < MAT_SIZE_32_1; scan_pos++)
    {
#pragma HLS UNROLL
        U36 level_double = coef64[scan_pos];
        U24 max_abs_level;
        U1 lower_int;
        U36 temp_level;
        temp_level = ((s64)COM_ABS(coef64[scan_pos]) * (s64)q_value);
        level_double = (int)COM_MIN(((s64)temp_level), (s64)COM_INT32_MAX - (s64)(1 << (q_bits - 1)));
        tmp_level_double[scan_pos] = level_double;
        max_abs_level = (u32)(level_double >> q_bits);
        lower_int = ((level_double - ((s64)max_abs_level << q_bits)) < (s64)(1 << (q_bits - 1))) ? 1 : 0;
        if (!lower_int)
        {
            max_abs_level++;
        }
        tmp_coef[scan_pos] = coef64[scan_pos] > 0 ? (s16)max_abs_level : -(s16)(max_abs_level);
    }

    U24 mat_prev_level[MAT_SIZE_32_1] = { 0 };
    U1 mat_run[MAT_SIZE_32_1] = { 0 };
#pragma HLS ARRAY_PARTITION variable = mat_prev_level complete dim = 0
#pragma HLS ARRAY_PARTITION variable = mat_run complete dim = 0

    if (last_level[cyc])
    {
        mat_prev_level[0] = last_level[cyc];
        mat_run[0] = 0;
    }
    else
    {
        mat_prev_level[0] = prev_level[cyc];
        mat_run[0] = 1;
    }

    for (scan_pos = 1; scan_pos < MAT_SIZE_32_1; scan_pos++)
    {
#pragma HLS UNROLL
        if (COM_ABS(tmp_coef[scan_pos - 1]))
        {
            mat_prev_level[scan_pos] = COM_ABS(tmp_coef[scan_pos - 1]);
            mat_run[scan_pos] = 0;
        }
        else
        {
            mat_prev_level[scan_pos] = mat_prev_level[scan_pos - 1];
            mat_run[scan_pos] = 1;
        }
    }

    s64 mat_uncoded_cost[MAT_SIZE_32_1] = { 0 };
    s64 mat_coded_cost[MAT_SIZE_32_1] = { 0 };
    U24 lev_opt[MAT_SIZE_32_1] = { 0 };
    u16 ctx_run = 0;
#pragma HLS ARRAY_PARTITION variable = mat_uncoded_cost complete dim = 0
#pragma HLS ARRAY_PARTITION variable = mat_coded_cost complete dim = 0
#pragma HLS ARRAY_PARTITION variable = lev_opt complete dim = 0

    for (scan_pos = 0; scan_pos < MAT_SIZE_32_1; scan_pos++)
    {
#pragma HLS UNROLL
        U24 level;
        U5 ctx_run = ((COM_MIN((U24)(mat_prev_level[scan_pos] - 1), (U24)5)) << 1) + (ch_type == Y_C ? 0 : 12);

        level = get_coded_level_rl(&d64_uncoded_cost, &d64_coded_cost, tmp_level_double[scan_pos], COM_ABS(tmp_coef[scan_pos]),
            mat_run[scan_pos], (u16)ctx_run, (u16)ctx_run, q_bits, err_scale, lambda, 0, rdoq_est_run_local, rdoq_est_level_local);
        dst_coef[scan_pos] = (s16)(tmp_coef[scan_pos] < 0 ? -(s16)(level) : (s16)level);

        mat_uncoded_cost[scan_pos] = d64_uncoded_cost;
        mat_coded_cost[scan_pos] = d64_coded_cost;
        lev_opt[scan_pos] = level;
    }

    s64 d64_cost_last_zero[MAT_SIZE_32_1] = { 0 };
#pragma HLS ARRAY_PARTITION variable = d64_cost_last_zero complete dim = 0
    int ace_log2 = 0;
    if (cyc == 0)
    {
        d64_cost_last_zero[0] = 0;
    }
    else
    {
        if ((base_idx) < 2)
        {
            ace_log2 = 0;
        }
        else if ((base_idx) < 4)
        {
            ace_log2 = 1;
        }
        else if ((base_idx) < 8)
        {
            ace_log2 = 2;
        }
        else if ((base_idx) < 16)
        {
            ace_log2 = 3;
        }
        else if ((base_idx) < 32)
        {
            ace_log2 = 4;
        }
        else if ((base_idx) < 64)
        {
            ace_log2 = 5;
        }
        else if ((base_idx) < 128)
        {
            ace_log2 = 6;
        }
        else if ((base_idx) < 256)
        {
            ace_log2 = 7;
        }
        else if ((base_idx) < 512)
        {
            ace_log2 = 8;
        }
        else if ((base_idx) < 1024)
        {
            ace_log2 = 9;
        }
        else if ((base_idx) < 2048)
        {
            ace_log2 = 10;
        }
        else if ((base_idx) < 4096)
        {
            ace_log2 = 11;
        }
        else
        {
            ace_log2 = 11;
        }
        d64_cost_last_zero[0] = GET_I_COST(rdoq_est_last_local[ctx_last][COM_MIN((U24)(prev_level[cyc] - 1), (U24)5)][ace_log2][0], lambda);

    }

    for (scan_pos = 1; scan_pos < MAT_SIZE_32_1; scan_pos++)
    {
#pragma HLS UNROLL
        if ((scan_pos + base_idx) < 2)
        {
            ace_log2 = 0;
        }
        else if ((scan_pos + base_idx) < 4)
        {
            ace_log2 = 1;
        }
        else if ((scan_pos + base_idx) < 8)
        {
            ace_log2 = 2;
        }
        else if ((scan_pos + base_idx) < 16)
        {
            ace_log2 = 3;
        }
        else if ((scan_pos + base_idx) < 32)
        {
            ace_log2 = 4;
        }
        else if ((scan_pos + base_idx) < 64)
        {
            ace_log2 = 5;
        }
        else if ((scan_pos + base_idx) < 128)
        {
            ace_log2 = 6;
        }
        else if ((scan_pos + base_idx) < 256)
        {
            ace_log2 = 7;
        }
        else if ((scan_pos + base_idx) < 512)
        {
            ace_log2 = 8;
        }
        else if ((scan_pos + base_idx) < 1024)
        {
            ace_log2 = 9;
        }
        else if ((scan_pos + base_idx) < 2048)
        {
            ace_log2 = 10;
        }
        else if ((scan_pos + base_idx) < 4096)
        {
            ace_log2 = 11;
        }
        else
        {
            ace_log2 = 11;
        }
        d64_cost_last_zero[scan_pos] = GET_I_COST(rdoq_est_last_local[ctx_last][COM_MIN((U24)(mat_prev_level[scan_pos - 1] - 1), (U24)5)][ace_log2][0], lambda);
    }

    if (cyc != 0)
        d64_base_cost[0] = last_cost[cyc];
    else
        d64_base_cost[0] = base_cost;

    d64_base_cost[0] -= mat_uncoded_cost[0];
    d64_base_cost[0] += mat_coded_cost[0];
    d64_base_cost[0] += d64_cost_last_zero[0] * !!(lev_opt[0]);
    for (scan_pos = 1; scan_pos < MAT_SIZE_32_1; scan_pos++)
    {
#pragma HLS UNROLL
        d64_base_cost[scan_pos] = d64_base_cost[0];
        for (int i = 1; i <= scan_pos; i++)
        {
#pragma HLS UNROLL
            d64_base_cost[scan_pos] += mat_coded_cost[i] - mat_uncoded_cost[i] + d64_cost_last_zero[i] * !!(lev_opt[i]);
        }
    }

    for (scan_pos = 0; scan_pos < MAT_SIZE_32_1; scan_pos++)
    {
#pragma HLS UNROLL
        if (scan_pos < cnt)
        {
            if ((scan_pos + base_idx + 1) < 2)
            {
                ace_log2 = 0;
            }
            else if ((scan_pos + base_idx + 1) < 4)
            {
                ace_log2 = 1;
            }
            else if ((scan_pos + base_idx + 1) < 8)
            {
                ace_log2 = 2;
            }
            else if ((scan_pos + base_idx + 1) < 16)
            {
                ace_log2 = 3;
            }
            else if ((scan_pos + base_idx + 1) < 32)
            {
                ace_log2 = 4;
            }
            else if ((scan_pos + base_idx + 1) < 64)
            {
                ace_log2 = 5;
            }
            else if ((scan_pos + base_idx + 1) < 128)
            {
                ace_log2 = 6;
            }
            else if ((scan_pos + base_idx + 1) < 256)
            {
                ace_log2 = 7;
            }
            else if ((scan_pos + base_idx + 1) < 512)
            {
                ace_log2 = 8;
            }
            else if ((scan_pos + base_idx + 1) < 1024)
            {
                ace_log2 = 9;
            }
            else if ((scan_pos + base_idx + 1) < 2048)
            {
                ace_log2 = 10;
            }
            else if ((scan_pos + base_idx + 1) < 4096)
            {
                ace_log2 = 11;
            }
            else
            {
                ace_log2 = 11;
            }
            U24 prev_level = mat_prev_level[scan_pos];
            s64 d64_cost_last_one = GET_I_COST(rdoq_est_last_local[ctx_last][COM_MIN((U24)(prev_level - 1), (U24)5)][ace_log2][1], lambda);

            if (COM_ABS(dst_coef[scan_pos]))
            {
                s64 d64_cur_is_last_cost = d64_base_cost[scan_pos] + d64_cost_last_one;

                if (d64_cur_is_last_cost < best_cost)
                {
                    *pzb = 1;
                    best_cost = d64_cur_is_last_cost;
#if (W <= H)
                    if (scan_pos < (2 * cyc + 1))
                    {
                        *x_pos = 2 * cyc;
                        *y_pos = 2 * cyc - scan_pos;
                    }
                    else
                    {
                        *x_pos = 2 * cyc + 1;
                        *y_pos = scan_pos - 2 * cyc - 1;
                    }
#else
                    if (cyc < H / 2)
                    {
                        if (scan_pos < (2 * cyc + 1))
                        {
                            *x_pos = 2 * cyc;
                            *y_pos = 2 * cyc - scan_pos;
                        }
                        else
                        {
                            *x_pos = 2 * cyc + 1;
                            *y_pos = scan_pos - 2 * cyc - 1;
                        }
                    }
                    else
                    {
                        if (scan_pos < H)
                        {
                            *x_pos = 2 * cyc;
                            *y_pos = H - 1 - scan_pos;
                        }
                        else
                        {
                            *x_pos = 2 * cyc + 1;
                            *y_pos = scan_pos - H;
                        }
                    }
#endif
                }
            }
        }
    }
    RDOQ_FUNC_NAME(model_update, W, H)(model_cnt->last1, model_cnt->last2, model_cnt->run_rdoq, model_cnt->level, mat_run, mat_prev_level, ch_type, cnt);

    if (cnt <= MAT_SIZE_32_1)
    {
        last_cost[cyc + 1] = d64_base_cost[cnt - 1];
        prev_level[cyc + 1] = mat_prev_level[cnt - 1];
        last_level[cyc + 1] = COM_ABS(tmp_coef[cnt - 1]);
    }
    else
    {
        last_cost[cyc + 1] = d64_base_cost[MAT_SIZE_32_1 - 1];
        prev_level[cyc + 1] = mat_prev_level[MAT_SIZE_32_1 - 1];
        last_level[cyc + 1] = COM_ABS(tmp_coef[MAT_SIZE_32_1 - 1]);
    }
    best_cost_out[cyc + 1] = best_cost;
}
int RDOQ_FUNC_NAME(rdoq_top, W, H)(RDOQ_MODEL* model_cnt, int qp, s64 d_lambda, int is_intra, s16 src_coef[H][W], s16 dst_tmp[H][W], int cu_width_log2, int cu_height_log2, int ch_type, int bit_depth, int rdoq_est_cbf[NUM_QT_CBF_CTX][2], s32 rdoq_est_run_local[24][2], s32 rdoq_est_level_local[24][2], s32 rdoq_est_last_local[2][6][12][2])
{
#pragma HLS ARRAY_PARTITION variable = src_coef complete dim = 1
#pragma HLS ARRAY_PARTITION variable = dst_tmp complete dim = 0
#pragma HLS ARRAY_PARTITION variable = rdoq_est_run_local complete dim = 0
#pragma HLS ARRAY_PARTITION variable = rdoq_est_level_local complete dim = 0
#pragma HLS ARRAY_PARTITION variable = rdoq_est_last_local complete dim = 0
    U14 quant_scale[80] = { (16302), (15024), (13777), (12634), (11626), (10624), (9742), (8958), (8192), (7512), (6889), (6305), (5793), (5303), (4878), (4467), (4091), (3756), (3444), (3161), (2894), (2654), (2435), (2235), (2048), (1878), (1722), (1579), (1449), (1329), (1218), (1117), (1024), (939), (861), (790), (724), (664), (609), (558), (512), (470), (430), (395), (362), (332), (304), (279), (256), (235), (215), (197), (181), (166), (152), (140), (128), (116), (108), (99), (91), (83), (76), (69), (64), (59), (54), (49), (45), (41), (38), (35), (32), (30), (27), (25), (23), (21), (19), (18) };

    const U14 scale = quant_scale[qp];
    const int ns_shift = ((cu_width_log2 + cu_height_log2) & 1) ? 7 : 0;
    const int ns_scale = ((cu_width_log2 + cu_height_log2) & 1) ? 181 : 1;
    const int ns_offset = ((cu_width_log2 + cu_height_log2) & 1) ? (1 << (ns_shift - 1)) : 0;
    const int q_value = (scale * ns_scale + ns_offset) >> ns_shift;
    const int log2_size = (cu_width_log2 + cu_height_log2) >> 1;
    const int tr_shift = get_transform_shift(bit_depth, log2_size);
    const int ctx_last = (ch_type == Y_C) ? 0 : 1;
    const int q_bits = QUANT_SHIFT + tr_shift;
    int num_nz_coef = 0;
    //const s64 lambda = (s64)(d_lambda * (double)(1 << SCALE_BITS) + 0.5);
    const s64 lambda = (d_lambda * (1 << 15) + 500) /1024;
    U30 err_scale_tbl[80][6] = { 32932, 65865, 131731, 263462, 526925, 1053850, 35734, 71468, 142936, 285873, 571747, 1143495, 38968, 77937, 155874, 311749, 623498, 1246996, 42494, 84988, 169976, 339953, 679906, 1359812, 46178, 92356, 184713, 369427, 738855, 1477711, 50533, 101067, 202135, 404270, 808540, 1617081, 55108, 110217, 220435, 440871, 881742, 1763484, 59932, 119864, 239728, 479456, 958912, 1917824, 65536, 131072, 262144, 524288, 1048576, 2097152, 71468, 142936, 285873, 571747, 1143495, 2286990, 77931, 155863, 311726, 623452, 1246905, 2493811, 85150, 170300, 340600, 681200, 1362400, 2724800, 92675, 185351, 370703, 741406, 1482812, 2965625, 101239, 202478, 404956, 809912, 1619825, 3239650, 110059, 220119, 440238, 880477, 1760954, 3521908, 120186, 240372, 480744, 961488, 1922976, 3845952, 131232, 262464, 524928, 1049857, 2099715, 4199430, 142936, 285873, 571747, 1143495, 2286990, 4573980, 155885, 311771, 623543, 1247086, 2494173, 4988347, 169842, 339684, 679368, 1358736, 2717473, 5434947, 185511, 371023, 742046, 1484093, 2968187, 5936374, 202287, 404574, 809149, 1618299, 3236599, 6473198, 220480, 440961, 881923, 1763846, 3527693, 7055387, 240210, 480421, 960842, 1921685, 3843371, 7686742, 262144, 524288, 1048576, 2097152, 4194304, 8388608, 285873, 571747, 1143495, 2286990, 4573980, 9147960, 311771, 623543, 1247086, 2494173, 4988347, 9976695, 340006, 680013, 1360027, 2720055, 5440110, 10880221, 370511, 741022, 1482045, 2964090, 5928181, 11856362, 403966, 807932, 1615864, 3231728, 6463457, 12926914, 440780, 881561, 1763122, 3526245, 7052491, 14104982, 480636, 961272, 1922545, 3845091, 7690183, 15380366, 524288, 1048576, 2097152, 4194304, 8388608, 16777216, 571747, 1143495, 2286990, 4573980, 9147960, 18295920, 623543, 1247086, 2494173, 4988347, 9976695, 19953390, 679583, 1359166, 2718333, 5436667, 10873334, 21746669, 741534, 1483068, 2966137, 5932275, 11864550, 23729101, 808540, 1617081, 3234162, 6468324, 12936648, 25873296, 881561, 1763122, 3526245, 7052491, 14104982, 28209965, 962134, 1924268, 3848537, 7697074, 15394148, 30788296, 1048576, 2097152, 4194304, 8388608, 16777216, 33554432, 1142278, 2284557, 4569114, 9138228, 18276456, 36552913, 1248537, 2497074, 4994148, 9988296, 19976592, 39953184, 1359166, 2718333, 5436667, 10873334, 21746669, 43493339, 1483068, 2966137, 5932275, 11864550, 23729101, 47458202, 1617081, 3234162, 6468324, 12936648, 25873296, 51746593, 1766022, 3532045, 7064090, 14128181, 28256363, 56512727, 1924268, 3848537, 7697074, 15394148, 30788296, 61576592, 2097152, 4194304, 8388608, 16777216, 33554432, 67108864, 2284557, 4569114, 9138228, 18276456, 36552913, 73105826, 2497074, 4994148, 9988296, 19976592, 39953184, 79906368, 2725233, 5450466, 10900932, 21801864, 43603728, 87207457, 2966137, 5932275, 11864550, 23729101, 47458202, 94916404, 3234162, 6468324, 12936648, 25873296, 51746593, 103493187, 3532045, 7064090, 14128181, 28256363, 56512727, 113025455, 3834792, 7669584, 15339168, 30678337, 61356675, 122713351, 4194304, 8388608, 16777216, 33554432, 67108864, 134217728, 4628197, 9256395, 18512790, 37025580, 74051160, 148102320, 4971026, 9942053, 19884107, 39768215, 79536431, 159072862, 5422938, 10845877, 21691754, 43383508, 86767016, 173534032, 5899680, 11799360, 23598721, 47197442, 94394885, 188789771, 6468324, 12936648, 25873296, 51746593, 103493187, 206986375, 7064090, 14128181, 28256363, 56512727, 113025455, 226050910, 7780737, 15561475, 31122951, 62245902, 124491805, 248983611, 8388608, 16777216, 33554432, 67108864, 134217728, 268435456, 9099506, 18199013, 36398027, 72796055, 145592111, 291184223, 9942053, 19884107, 39768215, 79536431, 159072862, 318145725, 10956549, 21913098, 43826196, 87652393, 175304787, 350609575, 11930464, 23860929, 47721858, 95443717, 190887435, 381774870, 13094412, 26188824, 52377649, 104755299, 209510599, 419021199, 14128181, 28256363, 56512727, 113025455, 226050910, 452101820, 15339168, 30678337, 61356675, 122713351, 245426702, 490853405, 16777216, 33554432, 67108864, 134217728, 268435456, 536870912, 17895697, 35791394, 71582788, 143165576, 286331153, 572662306, 19884107, 39768215, 79536431, 159072862, 318145725, 636291451, 21474836, 42949672, 85899345, 171798691, 343597383, 687194767, 23342213, 46684427, 93368854, 186737708, 373475417, 746950834, 25565281, 51130563, 102261126, 204522252, 409044504, 818089008, 28256363, 56512727, 113025455, 226050910, 452101820, 904203641, 29826161, 59652323, 119304647, 238609294, 477218588, 954437176 };

    s64 err_scale = err_scale_tbl[qp][log2_size - 1];
    s64 last_cost[W / 2 + 1];
    s64 best_cost[W / 2 + 1];
    int x_pos = 0;
    int y_pos = 0;
    s16 dst64_tmp[MAT_SIZE_32_1];
    s64 d64_base_cost0 = 0;
    s16 tmp_coef[MAT_SIZE_32_1];
    U24 last_level[W / 2 + 1];
    U24 prev_level[W / 2 + 1];
    last_level[0] = 6;
    prev_level[0] = 6;
    int pzb = 0;
    last_cost[0] = 0;
#pragma HLS ARRAY_PARTITION variable = last_cost complete dim = 0
#pragma HLS ARRAY_PARTITION variable = best_cost complete dim = 0
#pragma HLS ARRAY_PARTITION variable = dst64_tmp complete dim = 0
#pragma HLS ARRAY_PARTITION variable = tmp_coef complete dim = 0
#pragma HLS ARRAY_PARTITION variable = dst_coef complete dim = 0
#pragma HLS ARRAY_PARTITION variable = last_level complete dim = 0
#pragma HLS ARRAY_PARTITION variable = prev_level complete dim = 0

    if (!is_intra && ch_type == Y_C)
    {
        d64_base_cost0 = 0;
        best_cost[0] = 0;
    }
    else
    {
        d64_base_cost0 = GET_I_COST(rdoq_est_cbf[ch_type][1], lambda);
        best_cost[0] = GET_I_COST(rdoq_est_cbf[ch_type][0], lambda);
    }

    for (int cyc = 0; cyc < W / 2; cyc++)
    {
#pragma HLS DATAFLOW
        RDOQ_FUNC_NAME(read_data, W, H)(tmp_coef, src_coef, cyc);
        RDOQ_FUNC_NAME(rdoq_core, W, H)(model_cnt, last_level, prev_level, &pzb, d64_base_cost0, &x_pos, &y_pos, best_cost, last_cost, q_value, cyc, q_bits, err_scale, lambda, is_intra, tmp_coef, dst64_tmp, ch_type, rdoq_est_run_local, rdoq_est_level_local, rdoq_est_last_local);
        RDOQ_FUNC_NAME(write_data, W, H)(dst_tmp, dst64_tmp, cyc);
    }
    num_nz_coef = RDOQ_FUNC_NAME(clean_nz, W, H)(dst_tmp, x_pos, y_pos, pzb);
    return num_nz_coef;
}
//big size circuit
#else

void RDOQ_FUNC_NAME(rdoq_data_prepare, W, H)(int ch_type, s64 lambda, int cnt, s16 src_tmp[MAT_SIZE], s16 src_coef[MAT_SIZE], int is_intra)
{
#pragma HLS INLINE
    int scan_pos;
#if (W > H)
#if (H == 8)
    if (cnt == 1)
    {
        for (scan_pos = 0; scan_pos < 1; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[1 - scan_pos - 1];
        }
    }
    else if (cnt == 2)
    {
        for (scan_pos = 0; scan_pos < 2; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 3)
    {
        for (scan_pos = 0; scan_pos < 3; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[3 - scan_pos - 1];
        }
    }
    else if (cnt == 4)
    {
        for (scan_pos = 0; scan_pos < 4; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 5)
    {
        for (scan_pos = 0; scan_pos < 5; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[5 - scan_pos - 1];
        }
    }
    else if (cnt == 6)
    {
        for (scan_pos = 0; scan_pos < 6; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 7)
    {
        for (scan_pos = 0; scan_pos < 7; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[7 - scan_pos - 1];
        }
    }
    else if (cnt == 8)
    {
        for (scan_pos = 0; scan_pos < 8; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 9)
    {
        for (scan_pos = 0; scan_pos < 8; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[8 - scan_pos - 1];
        }
    }
    else if (cnt == 10)
    {
        for (scan_pos = 0; scan_pos < 8; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 11)
    {
        for (scan_pos = 0; scan_pos < 8; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[8 - scan_pos - 1];
        }
    }
    else if (cnt == 12)
    {
        for (scan_pos = 0; scan_pos < 8; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 13)
    {
        for (scan_pos = 0; scan_pos < 8; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[8 - scan_pos - 1];
        }
    }
    else if (cnt == 14)
    {
        for (scan_pos = 0; scan_pos < 8; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 15)
    {
        for (scan_pos = 0; scan_pos < 8; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[8 - scan_pos - 1];
        }
    }
    else if (cnt == 16)
    {
        for (scan_pos = 0; scan_pos < 8; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 17)
    {
        for (scan_pos = 0; scan_pos < 8; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[8 - scan_pos - 1];
        }
    }
    else if (cnt == 18)
    {
        for (scan_pos = 0; scan_pos < 8; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 19)
    {
        for (scan_pos = 0; scan_pos < 8; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[8 - scan_pos - 1];
        }
    }
    else if (cnt == 20)
    {
        for (scan_pos = 0; scan_pos < 8; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 21)
    {
        for (scan_pos = 0; scan_pos < 8; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[8 - scan_pos - 1];
        }
    }
    else if (cnt == 22)
    {
        for (scan_pos = 0; scan_pos < 8; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 23)
    {
        for (scan_pos = 0; scan_pos < 8; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[8 - scan_pos - 1];
        }
    }
    else if (cnt == 24)
    {
        for (scan_pos = 0; scan_pos < 8; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 25)
    {
        for (scan_pos = 0; scan_pos < 8; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[8 - scan_pos - 1];
        }
    }
    else if (cnt == 26)
    {
        for (scan_pos = 0; scan_pos < 8; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 27)
    {
        for (scan_pos = 0; scan_pos < 8; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[8 - scan_pos - 1];
        }
    }
    else if (cnt == 28)
    {
        for (scan_pos = 0; scan_pos < 8; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 29)
    {
        for (scan_pos = 0; scan_pos < 8; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[8 - scan_pos - 1];
        }
    }
    else if (cnt == 30)
    {
        for (scan_pos = 0; scan_pos < 8; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 31)
    {
        for (scan_pos = 0; scan_pos < 8; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[8 - scan_pos - 1];
        }
    }
    else if (cnt == 32)
    {
        for (scan_pos = 0; scan_pos < 8; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 33)
{
for (scan_pos = 0; scan_pos < 8; scan_pos++)
{
#pragma HLS UNROLL
    src_coef[scan_pos] = src_tmp[8 - scan_pos - 1];
}
}
    else if (cnt == 34)
{
for (scan_pos = 0; scan_pos < 8; scan_pos++)
{
#pragma HLS UNROLL
    src_coef[scan_pos] = src_tmp[scan_pos];
}
}
    else if (cnt == 35)
{
for (scan_pos = 0; scan_pos < 8; scan_pos++)
{
#pragma HLS UNROLL
    src_coef[scan_pos] = src_tmp[8 - scan_pos - 1];
}
}
    else if (cnt == 36)
{
for (scan_pos = 0; scan_pos < 8; scan_pos++)
{
#pragma HLS UNROLL
    src_coef[scan_pos] = src_tmp[scan_pos];
}
}
    else if (cnt == 37)
{
for (scan_pos = 0; scan_pos < 8; scan_pos++)
{
#pragma HLS UNROLL
    src_coef[scan_pos] = src_tmp[8 - scan_pos - 1];
}
}
    else if (cnt == 38)
{
for (scan_pos = 0; scan_pos < 8; scan_pos++)
{
#pragma HLS UNROLL
    src_coef[scan_pos] = src_tmp[scan_pos];
}
}
    else if (cnt == 39)
{
for (scan_pos = 0; scan_pos < 8; scan_pos++)
{
#pragma HLS UNROLL
    src_coef[scan_pos] = src_tmp[8 - scan_pos - 1];
}
}
    else if (cnt == 40)
{
for (scan_pos = 0; scan_pos < 8; scan_pos++)
{
#pragma HLS UNROLL
    src_coef[scan_pos] = src_tmp[scan_pos];
}
}
    else if (cnt == 41)
{
for (scan_pos = 0; scan_pos < 8; scan_pos++)
{
#pragma HLS UNROLL
    src_coef[scan_pos] = src_tmp[8 - scan_pos - 1];
}
}
    else if (cnt == 42)
{
for (scan_pos = 0; scan_pos < 8; scan_pos++)
{
#pragma HLS UNROLL
    src_coef[scan_pos] = src_tmp[scan_pos];
}
}
    else if (cnt == 43)
{
for (scan_pos = 0; scan_pos < 8; scan_pos++)
{
#pragma HLS UNROLL
    src_coef[scan_pos] = src_tmp[8 - scan_pos - 1];
}
}
    else if (cnt == 44)
{
for (scan_pos = 0; scan_pos < 8; scan_pos++)
{
#pragma HLS UNROLL
    src_coef[scan_pos] = src_tmp[scan_pos];
}
}
    else if (cnt == 45)
{
for (scan_pos = 0; scan_pos < 8; scan_pos++)
{
#pragma HLS UNROLL
    src_coef[scan_pos] = src_tmp[8 - scan_pos - 1];
}
}
    else if (cnt == 46)
{
for (scan_pos = 0; scan_pos < 8; scan_pos++)
{
#pragma HLS UNROLL
    src_coef[scan_pos] = src_tmp[scan_pos];
}
}
    else if (cnt == 47)
{
for (scan_pos = 0; scan_pos < 8; scan_pos++)
{
#pragma HLS UNROLL
    src_coef[scan_pos] = src_tmp[8 - scan_pos - 1];
}
}
    else if (cnt == 48)
{
for (scan_pos = 0; scan_pos < 8; scan_pos++)
{
#pragma HLS UNROLL
    src_coef[scan_pos] = src_tmp[scan_pos];
}
}
    else if (cnt == 49)
{
for (scan_pos = 0; scan_pos < 8; scan_pos++)
{
#pragma HLS UNROLL
    src_coef[scan_pos] = src_tmp[8 - scan_pos - 1];
}
}
    else if (cnt == 50)
{
for (scan_pos = 0; scan_pos < 8; scan_pos++)
{
#pragma HLS UNROLL
    src_coef[scan_pos] = src_tmp[scan_pos];
}
}
    else if (cnt == 51)
{
for (scan_pos = 0; scan_pos < 8; scan_pos++)
{
#pragma HLS UNROLL
    src_coef[scan_pos] = src_tmp[8 - scan_pos - 1];
}
}
    else if (cnt == 52)
{
for (scan_pos = 0; scan_pos < 8; scan_pos++)
{
#pragma HLS UNROLL
    src_coef[scan_pos] = src_tmp[scan_pos];
}
}
    else if (cnt == 53)
{
for (scan_pos = 0; scan_pos < 8; scan_pos++)
{
#pragma HLS UNROLL
    src_coef[scan_pos] = src_tmp[8 - scan_pos - 1];
}
}
    else if (cnt == 54)
{
for (scan_pos = 0; scan_pos < 8; scan_pos++)
{
#pragma HLS UNROLL
    src_coef[scan_pos] = src_tmp[scan_pos];
}
}
    else if (cnt == 55)
{
for (scan_pos = 0; scan_pos < 8; scan_pos++)
{
#pragma HLS UNROLL
    src_coef[scan_pos] = src_tmp[8 - scan_pos - 1];
}
}
    else if (cnt == 56)
{
for (scan_pos = 0; scan_pos < 8; scan_pos++)
{
#pragma HLS UNROLL
    src_coef[scan_pos] = src_tmp[scan_pos];
}
}
    else if (cnt == 57)
{
for (scan_pos = 0; scan_pos < 8; scan_pos++)
{
#pragma HLS UNROLL
    src_coef[scan_pos] = src_tmp[8 - scan_pos - 1];
}
}
    else if (cnt == 58)
{
for (scan_pos = 0; scan_pos < 8; scan_pos++)
{
#pragma HLS UNROLL
    src_coef[scan_pos] = src_tmp[scan_pos];
}
}
    else if (cnt == 59)
{
for (scan_pos = 0; scan_pos < 8; scan_pos++)
{
#pragma HLS UNROLL
    src_coef[scan_pos] = src_tmp[8 - scan_pos - 1];
}
}
    else if (cnt == 60)
{
for (scan_pos = 0; scan_pos < 8; scan_pos++)
{
#pragma HLS UNROLL
    src_coef[scan_pos] = src_tmp[scan_pos];
}
}
    else if (cnt == 61)
{
for (scan_pos = 0; scan_pos < 8; scan_pos++)
{
#pragma HLS UNROLL
    src_coef[scan_pos] = src_tmp[8 - scan_pos - 1];
}
}
    else if (cnt == 62)
{
for (scan_pos = 0; scan_pos < 8; scan_pos++)
{
#pragma HLS UNROLL
    src_coef[scan_pos] = src_tmp[scan_pos];
}
}
    else if (cnt == 63)
{
for (scan_pos = 0; scan_pos < 8; scan_pos++)
{
#pragma HLS UNROLL
    src_coef[scan_pos] = src_tmp[8 - scan_pos - 1];
}
}
    else if (cnt == 64)
{
for (scan_pos = 0; scan_pos < 8; scan_pos++)
{
#pragma HLS UNROLL
    src_coef[scan_pos] = src_tmp[scan_pos];
}
}
#elif (H == 16)
    if (cnt == 1)
    {
        for (scan_pos = 0; scan_pos < 1; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[1 - scan_pos - 1];
        }
    }
    else if (cnt == 2)
    {
        for (scan_pos = 0; scan_pos < 2; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 3)
    {
        for (scan_pos = 0; scan_pos < 3; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[3 - scan_pos - 1];
        }
    }
    else if (cnt == 4)
    {
        for (scan_pos = 0; scan_pos < 4; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 5)
    {
        for (scan_pos = 0; scan_pos < 5; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[5 - scan_pos - 1];
        }
    }
    else if (cnt == 6)
    {
        for (scan_pos = 0; scan_pos < 6; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 7)
    {
        for (scan_pos = 0; scan_pos < 7; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[7 - scan_pos - 1];
        }
    }
    else if (cnt == 8)
    {
        for (scan_pos = 0; scan_pos < 8; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 9)
    {
        for (scan_pos = 0; scan_pos < 9; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[9 - scan_pos - 1];
        }
    }
    else if (cnt == 10)
    {
        for (scan_pos = 0; scan_pos < 10; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 11)
    {
        for (scan_pos = 0; scan_pos < 11; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[11 - scan_pos - 1];
        }
    }
    else if (cnt == 12)
    {
        for (scan_pos = 0; scan_pos < 12; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 13)
    {
        for (scan_pos = 0; scan_pos < 13; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[13 - scan_pos - 1];
        }
    }
    else if (cnt == 14)
    {
        for (scan_pos = 0; scan_pos < 14; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 15)
    {
        for (scan_pos = 0; scan_pos < 15; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[15 - scan_pos - 1];
        }
    }
    else if (cnt == 16)
    {
        for (scan_pos = 0; scan_pos < 16; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 17)
    {
        for (scan_pos = 0; scan_pos < 16; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[16 - scan_pos - 1];
        }
    }
    else if (cnt == 18)
    {
        for (scan_pos = 0; scan_pos < 16; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 19)
    {
        for (scan_pos = 0; scan_pos < 16; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[16 - scan_pos - 1];
        }
    }
    else if (cnt == 20)
    {
        for (scan_pos = 0; scan_pos < 16; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 21)
    {
        for (scan_pos = 0; scan_pos < 16; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[16 - scan_pos - 1];
        }
    }
    else if (cnt == 22)
    {
        for (scan_pos = 0; scan_pos < 16; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 23)
    {
        for (scan_pos = 0; scan_pos < 16; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[16 - scan_pos - 1];
        }
    }
    else if (cnt == 24)
    {
        for (scan_pos = 0; scan_pos < 16; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 25)
    {
        for (scan_pos = 0; scan_pos < 16; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[16 - scan_pos - 1];
        }
    }
    else if (cnt == 26)
    {
        for (scan_pos = 0; scan_pos < 16; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 27)
    {
        for (scan_pos = 0; scan_pos < 16; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[16 - scan_pos - 1];
        }
    }
    else if (cnt == 28)
    {
        for (scan_pos = 0; scan_pos < 16; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 29)
    {
        for (scan_pos = 0; scan_pos < 16; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[16 - scan_pos - 1];
        }
    }
    else if (cnt == 30)
    {
        for (scan_pos = 0; scan_pos < 16; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 31)
    {
        for (scan_pos = 0; scan_pos < 16; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[16 - scan_pos - 1];
        }
    }
    else if (cnt == 32)
    {
        for (scan_pos = 0; scan_pos < 16; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
#if (W > 32)
    else if (cnt == 33)
    {
        for (scan_pos = 0; scan_pos < 16; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[16 - scan_pos - 1];
        }
    }
    else if (cnt == 34)
    {
        for (scan_pos = 0; scan_pos < 16; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 35)
    {
        for (scan_pos = 0; scan_pos < 16; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[16 - scan_pos - 1];
        }
    }
    else if (cnt == 36)
    {
        for (scan_pos = 0; scan_pos < 16; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 37)
    {
        for (scan_pos = 0; scan_pos < 16; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[16 - scan_pos - 1];
        }
    }
    else if (cnt == 38)
    {
        for (scan_pos = 0; scan_pos < 16; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 39)
    {
        for (scan_pos = 0; scan_pos < 16; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[16 - scan_pos - 1];
        }
    }
    else if (cnt == 40)
    {
        for (scan_pos = 0; scan_pos < 16; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 41)
    {
        for (scan_pos = 0; scan_pos < 16; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[16 - scan_pos - 1];
        }
    }
    else if (cnt == 42)
    {
        for (scan_pos = 0; scan_pos < 16; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 43)
    {
        for (scan_pos = 0; scan_pos < 16; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[16 - scan_pos - 1];
        }
    }
    else if (cnt == 44)
    {
        for (scan_pos = 0; scan_pos < 16; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 45)
    {
        for (scan_pos = 0; scan_pos < 16; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[16 - scan_pos - 1];
        }
    }
    else if (cnt == 46)
    {
        for (scan_pos = 0; scan_pos < 16; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 47)
    {
        for (scan_pos = 0; scan_pos < 16; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[16 - scan_pos - 1];
        }
    }
    else if (cnt == 48)
    {
        for (scan_pos = 0; scan_pos < 16; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 49)
    {
        for (scan_pos = 0; scan_pos < 16; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[16 - scan_pos - 1];
        }
    }
    else if (cnt == 50)
    {
        for (scan_pos = 0; scan_pos < 16; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 51)
    {
        for (scan_pos = 0; scan_pos < 16; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[16 - scan_pos - 1];
        }
    }
    else if (cnt == 52)
    {
        for (scan_pos = 0; scan_pos < 16; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 53)
    {
        for (scan_pos = 0; scan_pos < 16; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[16 - scan_pos - 1];
        }
    }
    else if (cnt == 54)
    {
        for (scan_pos = 0; scan_pos < 16; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 55)
    {
        for (scan_pos = 0; scan_pos < 16; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[16 - scan_pos - 1];
        }
    }
    else if (cnt == 56)
    {
        for (scan_pos = 0; scan_pos < 16; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 57)
    {
        for (scan_pos = 0; scan_pos < 16; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[16 - scan_pos - 1];
        }
    }
    else if (cnt == 58)
    {
        for (scan_pos = 0; scan_pos < 16; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 59)
    {
        for (scan_pos = 0; scan_pos < 16; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[16 - scan_pos - 1];
        }
    }
    else if (cnt == 60)
    {
        for (scan_pos = 0; scan_pos < 16; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 61)
    {
        for (scan_pos = 0; scan_pos < 16; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[16 - scan_pos - 1];
        }
    }
    else if (cnt == 62)
    {
        for (scan_pos = 0; scan_pos < 16; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 63)
    {
        for (scan_pos = 0; scan_pos < 16; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[16 - scan_pos - 1];
        }
    }
    else if (cnt == 64)
    {
        for (scan_pos = 0; scan_pos < 16; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
#endif
#else
    if (cnt == 1)
    {
        for (scan_pos = 0; scan_pos < 1; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[1 - scan_pos - 1];
        }
    }
    else if (cnt == 2)
    {
        for (scan_pos = 0; scan_pos < 2; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 3)
    {
        for (scan_pos = 0; scan_pos < 3; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[3 - scan_pos - 1];
        }
    }
    else if (cnt == 4)
    {
        for (scan_pos = 0; scan_pos < 4; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 5)
    {
        for (scan_pos = 0; scan_pos < 5; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[5 - scan_pos - 1];
        }
    }
    else if (cnt == 6)
    {
        for (scan_pos = 0; scan_pos < 6; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 7)
    {
        for (scan_pos = 0; scan_pos < 7; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[7 - scan_pos - 1];
        }
    }
    else if (cnt == 8)
    {
        for (scan_pos = 0; scan_pos < 8; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 9)
    {
        for (scan_pos = 0; scan_pos < 9; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[9 - scan_pos - 1];
        }
    }
    else if (cnt == 10)
    {
        for (scan_pos = 0; scan_pos < 10; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 11)
    {
        for (scan_pos = 0; scan_pos < 11; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[11 - scan_pos - 1];
        }
    }
    else if (cnt == 12)
    {
        for (scan_pos = 0; scan_pos < 12; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 13)
    {
        for (scan_pos = 0; scan_pos < 13; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[13 - scan_pos - 1];
        }
    }
    else if (cnt == 14)
    {
        for (scan_pos = 0; scan_pos < 14; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 15)
    {
        for (scan_pos = 0; scan_pos < 15; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[15 - scan_pos - 1];
        }
    }
    else if (cnt == 16)
    {
        for (scan_pos = 0; scan_pos < 16; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 17)
    {
        for (scan_pos = 0; scan_pos < 17; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[17 - scan_pos - 1];
        }
    }
    else if (cnt == 18)
    {
        for (scan_pos = 0; scan_pos < 18; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 19)
    {
        for (scan_pos = 0; scan_pos < 19; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[19 - scan_pos - 1];
        }
    }
    else if (cnt == 20)
    {
        for (scan_pos = 0; scan_pos < 20; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 21)
    {
        for (scan_pos = 0; scan_pos < 21; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[21 - scan_pos - 1];
        }
    }
    else if (cnt == 22)
    {
        for (scan_pos = 0; scan_pos < 22; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 23)
    {
        for (scan_pos = 0; scan_pos < 23; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[23 - scan_pos - 1];
        }
    }
    else if (cnt == 24)
    {
        for (scan_pos = 0; scan_pos < 24; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 25)
    {
        for (scan_pos = 0; scan_pos < 25; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[25 - scan_pos - 1];
        }
    }
    else if (cnt == 26)
    {
        for (scan_pos = 0; scan_pos < 26; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 27)
    {
        for (scan_pos = 0; scan_pos < 27; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[27 - scan_pos - 1];
        }
    }
    else if (cnt == 28)
    {
        for (scan_pos = 0; scan_pos < 28; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 29)
    {
        for (scan_pos = 0; scan_pos < 29; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[29 - scan_pos - 1];
        }
    }
    else if (cnt == 30)
    {
        for (scan_pos = 0; scan_pos < 30; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 31)
    {
        for (scan_pos = 0; scan_pos < 31; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[31 - scan_pos - 1];
        }
    }
    else if (cnt == 32)
    {
        for (scan_pos = 0; scan_pos < 32; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 33)
    {
        for (scan_pos = 0; scan_pos < 32; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[32 - scan_pos - 1];
        }
    }
    else if (cnt == 34)
    {
        for (scan_pos = 0; scan_pos < 32; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 35)
    {
        for (scan_pos = 0; scan_pos < 32; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[32 - scan_pos - 1];
        }
    }
    else if (cnt == 36)
    {
        for (scan_pos = 0; scan_pos < 32; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 37)
    {
        for (scan_pos = 0; scan_pos < 32; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[32 - scan_pos - 1];
        }
    }
    else if (cnt == 38)
    {
        for (scan_pos = 0; scan_pos < 32; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 39)
    {
        for (scan_pos = 0; scan_pos < 32; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[32 - scan_pos - 1];
        }
    }
    else if (cnt == 40)
    {
        for (scan_pos = 0; scan_pos < 32; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 41)
    {
        for (scan_pos = 0; scan_pos < 32; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[32 - scan_pos - 1];
        }
    }
    else if (cnt == 42)
    {
        for (scan_pos = 0; scan_pos < 32; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 43)
    {
        for (scan_pos = 0; scan_pos < 32; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[32 - scan_pos - 1];
        }
    }
    else if (cnt == 44)
    {
        for (scan_pos = 0; scan_pos < 32; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 45)
    {
        for (scan_pos = 0; scan_pos < 32; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[32 - scan_pos - 1];
        }
    }
    else if (cnt == 46)
    {
        for (scan_pos = 0; scan_pos < 32; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 47)
    {
        for (scan_pos = 0; scan_pos < 32; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[32 - scan_pos - 1];
        }
    }
    else if (cnt == 48)
    {
        for (scan_pos = 0; scan_pos < 32; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 49)
    {
        for (scan_pos = 0; scan_pos < 32; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[32 - scan_pos - 1];
        }
    }
    else if (cnt == 50)
    {
        for (scan_pos = 0; scan_pos < 32; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 51)
    {
        for (scan_pos = 0; scan_pos < 32; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[32 - scan_pos - 1];
        }
    }
    else if (cnt == 52)
    {
        for (scan_pos = 0; scan_pos < 32; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 53)
    {
        for (scan_pos = 0; scan_pos < 32; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[32 - scan_pos - 1];
        }
    }
    else if (cnt == 54)
    {
        for (scan_pos = 0; scan_pos < 32; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 55)
    {
        for (scan_pos = 0; scan_pos < 32; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[32 - scan_pos - 1];
        }
    }
    else if (cnt == 56)
    {
        for (scan_pos = 0; scan_pos < 32; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 57)
    {
        for (scan_pos = 0; scan_pos < 32; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[32 - scan_pos - 1];
        }
    }
    else if (cnt == 58)
    {
        for (scan_pos = 0; scan_pos < 32; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 59)
    {
        for (scan_pos = 0; scan_pos < 32; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[32 - scan_pos - 1];
        }
    }
    else if (cnt == 60)
    {
        for (scan_pos = 0; scan_pos < 32; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 61)
    {
        for (scan_pos = 0; scan_pos < 32; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[32 - scan_pos - 1];
        }
    }
    else if (cnt == 62)
    {
        for (scan_pos = 0; scan_pos < 32; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
    else if (cnt == 63)
    {
        for (scan_pos = 0; scan_pos < 32; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[32 - scan_pos - 1];
        }
    }
    else if (cnt == 64)
    {
        for (scan_pos = 0; scan_pos < 32; scan_pos++)
        {
#pragma HLS UNROLL
            src_coef[scan_pos] = src_tmp[scan_pos];
        }
    }
#endif
#else
if (cnt == 1)
{
    for (scan_pos = 0; scan_pos < 1; scan_pos++)
    {
#pragma HLS UNROLL
        src_coef[scan_pos] = src_tmp[1 - scan_pos - 1];
    }
}
else if (cnt == 2)
{
    for (scan_pos = 0; scan_pos < 2; scan_pos++)
    {
#pragma HLS UNROLL
        src_coef[scan_pos] = src_tmp[scan_pos];
    }
}
else if (cnt == 3)
{
    for (scan_pos = 0; scan_pos < 3; scan_pos++)
    {
#pragma HLS UNROLL
        src_coef[scan_pos] = src_tmp[3 - scan_pos - 1];
    }
}
else if (cnt == 4)
{
    for (scan_pos = 0; scan_pos < 4; scan_pos++)
    {
#pragma HLS UNROLL
        src_coef[scan_pos] = src_tmp[scan_pos];
    }
}
else if (cnt == 5)
{
    for (scan_pos = 0; scan_pos < 5; scan_pos++)
    {
#pragma HLS UNROLL
        src_coef[scan_pos] = src_tmp[5 - scan_pos - 1];
    }
}
else if (cnt == 6)
{
    for (scan_pos = 0; scan_pos < 6; scan_pos++)
    {
#pragma HLS UNROLL
        src_coef[scan_pos] = src_tmp[scan_pos];
    }
}
else if (cnt == 7)
{
    for (scan_pos = 0; scan_pos < 7; scan_pos++)
    {
#pragma HLS UNROLL
        src_coef[scan_pos] = src_tmp[7 - scan_pos - 1];
    }
}
else if (cnt == 8)
{
    for (scan_pos = 0; scan_pos < 8; scan_pos++)
    {
#pragma HLS UNROLL
        src_coef[scan_pos] = src_tmp[scan_pos];
    }
}
#if (W > 8)
else if (cnt == 9)
{
    for (scan_pos = 0; scan_pos < 9; scan_pos++)
    {
#pragma HLS UNROLL
        src_coef[scan_pos] = src_tmp[9 - scan_pos - 1];
    }
}
else if (cnt == 10)
{
    for (scan_pos = 0; scan_pos < 10; scan_pos++)
    {
#pragma HLS UNROLL
        src_coef[scan_pos] = src_tmp[scan_pos];
    }
}
else if (cnt == 11)
{
    for (scan_pos = 0; scan_pos < 11; scan_pos++)
    {
#pragma HLS UNROLL
        src_coef[scan_pos] = src_tmp[11 - scan_pos - 1];
    }
}
else if (cnt == 12)
{
    for (scan_pos = 0; scan_pos < 12; scan_pos++)
    {
#pragma HLS UNROLL
        src_coef[scan_pos] = src_tmp[scan_pos];
    }
}
else if (cnt == 13)
{
    for (scan_pos = 0; scan_pos < 13; scan_pos++)
    {
#pragma HLS UNROLL
        src_coef[scan_pos] = src_tmp[13 - scan_pos - 1];
    }
}
else if (cnt == 14)
{
    for (scan_pos = 0; scan_pos < 14; scan_pos++)
    {
#pragma HLS UNROLL
        src_coef[scan_pos] = src_tmp[scan_pos];
    }
}
else if (cnt == 15)
{
    for (scan_pos = 0; scan_pos < 15; scan_pos++)
    {
#pragma HLS UNROLL
        src_coef[scan_pos] = src_tmp[15 - scan_pos - 1];
    }
}
else if (cnt == 16)
{
    for (scan_pos = 0; scan_pos < 16; scan_pos++)
    {
#pragma HLS UNROLL
        src_coef[scan_pos] = src_tmp[scan_pos];
    }
}
#if (W > 16)
else if (cnt == 17)
{
    for (scan_pos = 0; scan_pos < 17; scan_pos++)
    {
#pragma HLS UNROLL
        src_coef[scan_pos] = src_tmp[17 - scan_pos - 1];
    }
}
else if (cnt == 18)
{
    for (scan_pos = 0; scan_pos < 18; scan_pos++)
    {
#pragma HLS UNROLL
        src_coef[scan_pos] = src_tmp[scan_pos];
    }
}
else if (cnt == 19)
{
    for (scan_pos = 0; scan_pos < 19; scan_pos++)
    {
#pragma HLS UNROLL
        src_coef[scan_pos] = src_tmp[19 - scan_pos - 1];
    }
}
else if (cnt == 20)
{
    for (scan_pos = 0; scan_pos < 20; scan_pos++)
    {
#pragma HLS UNROLL
        src_coef[scan_pos] = src_tmp[scan_pos];
    }
}
else if (cnt == 21)
{
    for (scan_pos = 0; scan_pos < 21; scan_pos++)
    {
#pragma HLS UNROLL
        src_coef[scan_pos] = src_tmp[21 - scan_pos - 1];
    }
}
else if (cnt == 22)
{
    for (scan_pos = 0; scan_pos < 22; scan_pos++)
    {
#pragma HLS UNROLL
        src_coef[scan_pos] = src_tmp[scan_pos];
    }
}
else if (cnt == 23)
{
    for (scan_pos = 0; scan_pos < 23; scan_pos++)
    {
#pragma HLS UNROLL
        src_coef[scan_pos] = src_tmp[23 - scan_pos - 1];
    }
}
else if (cnt == 24)
{
    for (scan_pos = 0; scan_pos < 24; scan_pos++)
    {
#pragma HLS UNROLL
        src_coef[scan_pos] = src_tmp[scan_pos];
    }
}
else if (cnt == 25)
{
    for (scan_pos = 0; scan_pos < 25; scan_pos++)
    {
#pragma HLS UNROLL
        src_coef[scan_pos] = src_tmp[25 - scan_pos - 1];
    }
}
else if (cnt == 26)
{
    for (scan_pos = 0; scan_pos < 26; scan_pos++)
    {
#pragma HLS UNROLL
        src_coef[scan_pos] = src_tmp[scan_pos];
    }
}
else if (cnt == 27)
{
    for (scan_pos = 0; scan_pos < 27; scan_pos++)
    {
#pragma HLS UNROLL
        src_coef[scan_pos] = src_tmp[27 - scan_pos - 1];
    }
}
else if (cnt == 28)
{
    for (scan_pos = 0; scan_pos < 28; scan_pos++)
    {
#pragma HLS UNROLL
        src_coef[scan_pos] = src_tmp[scan_pos];
    }
}
else if (cnt == 29)
{
    for (scan_pos = 0; scan_pos < 29; scan_pos++)
    {
#pragma HLS UNROLL
        src_coef[scan_pos] = src_tmp[29 - scan_pos - 1];
    }
}
else if (cnt == 30)
{
    for (scan_pos = 0; scan_pos < 30; scan_pos++)
    {
#pragma HLS UNROLL
        src_coef[scan_pos] = src_tmp[scan_pos];
    }
}
else if (cnt == 31)
{
    for (scan_pos = 0; scan_pos < 31; scan_pos++)
    {
#pragma HLS UNROLL
        src_coef[scan_pos] = src_tmp[31 - scan_pos - 1];
    }
}
else if (cnt == 32)
{
    for (scan_pos = 0; scan_pos < 32; scan_pos++)
    {
#pragma HLS UNROLL
        src_coef[scan_pos] = src_tmp[scan_pos];
    }
}
#if (W > 32)
else if (cnt == 33)
{
    for (scan_pos = 0; scan_pos < 33; scan_pos++)
    {
#pragma HLS UNROLL
        src_coef[scan_pos] = src_tmp[33 - scan_pos - 1];
    }
}
else if (cnt == 34)
{
    for (scan_pos = 0; scan_pos < 34; scan_pos++)
    {
#pragma HLS UNROLL
        src_coef[scan_pos] = src_tmp[scan_pos];
    }
}
else if (cnt == 35)
{
    for (scan_pos = 0; scan_pos < 35; scan_pos++)
    {
#pragma HLS UNROLL
        src_coef[scan_pos] = src_tmp[35 - scan_pos - 1];
    }
}
else if (cnt == 36)
{
    for (scan_pos = 0; scan_pos < 36; scan_pos++)
    {
#pragma HLS UNROLL
        src_coef[scan_pos] = src_tmp[scan_pos];
    }
}
else if (cnt == 37)
{
    for (scan_pos = 0; scan_pos < 37; scan_pos++)
    {
#pragma HLS UNROLL
        src_coef[scan_pos] = src_tmp[37 - scan_pos - 1];
    }
}
else if (cnt == 38)
{
    for (scan_pos = 0; scan_pos < 38; scan_pos++)
    {
#pragma HLS UNROLL
        src_coef[scan_pos] = src_tmp[scan_pos];
    }
}
else if (cnt == 39)
{
    for (scan_pos = 0; scan_pos < 39; scan_pos++)
    {
#pragma HLS UNROLL
        src_coef[scan_pos] = src_tmp[39 - scan_pos - 1];
    }
}
else if (cnt == 40)
{
    for (scan_pos = 0; scan_pos < 40; scan_pos++)
    {
#pragma HLS UNROLL
        src_coef[scan_pos] = src_tmp[scan_pos];
    }
}
else if (cnt == 41)
{
    for (scan_pos = 0; scan_pos < 41; scan_pos++)
    {
#pragma HLS UNROLL
        src_coef[scan_pos] = src_tmp[41 - scan_pos - 1];
    }
}
else if (cnt == 42)
{
    for (scan_pos = 0; scan_pos < 42; scan_pos++)
    {
#pragma HLS UNROLL
        src_coef[scan_pos] = src_tmp[scan_pos];
    }
}
else if (cnt == 43)
{
    for (scan_pos = 0; scan_pos < 43; scan_pos++)
    {
#pragma HLS UNROLL
        src_coef[scan_pos] = src_tmp[43 - scan_pos - 1];
    }
}
else if (cnt == 44)
{
    for (scan_pos = 0; scan_pos < 44; scan_pos++)
    {
#pragma HLS UNROLL
        src_coef[scan_pos] = src_tmp[scan_pos];
    }
}
else if (cnt == 45)
{
    for (scan_pos = 0; scan_pos < 45; scan_pos++)
    {
#pragma HLS UNROLL
        src_coef[scan_pos] = src_tmp[45 - scan_pos - 1];
    }
}
else if (cnt == 46)
{
    for (scan_pos = 0; scan_pos < 46; scan_pos++)
    {
#pragma HLS UNROLL
        src_coef[scan_pos] = src_tmp[scan_pos];
    }
}
else if (cnt == 47)
{
    for (scan_pos = 0; scan_pos < 47; scan_pos++)
    {
#pragma HLS UNROLL
        src_coef[scan_pos] = src_tmp[47 - scan_pos - 1];
    }
}
else if (cnt == 48)
{
    for (scan_pos = 0; scan_pos < 48; scan_pos++)
    {
#pragma HLS UNROLL
        src_coef[scan_pos] = src_tmp[scan_pos];
    }
}
else if (cnt == 49)
{
    for (scan_pos = 0; scan_pos < 49; scan_pos++)
    {
#pragma HLS UNROLL
        src_coef[scan_pos] = src_tmp[49 - scan_pos - 1];
    }
}
else if (cnt == 50)
{
    for (scan_pos = 0; scan_pos < 50; scan_pos++)
    {
#pragma HLS UNROLL
        src_coef[scan_pos] = src_tmp[scan_pos];
    }
}
else if (cnt == 51)
{
    for (scan_pos = 0; scan_pos < 51; scan_pos++)
    {
#pragma HLS UNROLL
        src_coef[scan_pos] = src_tmp[51 - scan_pos - 1];
    }
}
else if (cnt == 52)
{
    for (scan_pos = 0; scan_pos < 52; scan_pos++)
    {
#pragma HLS UNROLL
        src_coef[scan_pos] = src_tmp[scan_pos];
    }
}
else if (cnt == 53)
{
    for (scan_pos = 0; scan_pos < 53; scan_pos++)
    {
#pragma HLS UNROLL
        src_coef[scan_pos] = src_tmp[53 - scan_pos - 1];
    }
}
else if (cnt == 54)
{
    for (scan_pos = 0; scan_pos < 54; scan_pos++)
    {
#pragma HLS UNROLL
        src_coef[scan_pos] = src_tmp[scan_pos];
    }
}
else if (cnt == 55)
{
    for (scan_pos = 0; scan_pos < 55; scan_pos++)
    {
#pragma HLS UNROLL
        src_coef[scan_pos] = src_tmp[55 - scan_pos - 1];
    }
}
else if (cnt == 56)
{
    for (scan_pos = 0; scan_pos < 56; scan_pos++)
    {
#pragma HLS UNROLL
        src_coef[scan_pos] = src_tmp[scan_pos];
    }
}
else if (cnt == 57)
{
    for (scan_pos = 0; scan_pos < 57; scan_pos++)
    {
#pragma HLS UNROLL
        src_coef[scan_pos] = src_tmp[57 - scan_pos - 1];
    }
}
else if (cnt == 58)
{
    for (scan_pos = 0; scan_pos < 58; scan_pos++)
    {
#pragma HLS UNROLL
        src_coef[scan_pos] = src_tmp[scan_pos];
    }
}
else if (cnt == 59)
{
    for (scan_pos = 0; scan_pos < 59; scan_pos++)
    {
#pragma HLS UNROLL
        src_coef[scan_pos] = src_tmp[59 - scan_pos - 1];
    }
}
else if (cnt == 60)
{
    for (scan_pos = 0; scan_pos < 60; scan_pos++)
    {
#pragma HLS UNROLL
        src_coef[scan_pos] = src_tmp[scan_pos];
    }
}
else if (cnt == 61)
{
    for (scan_pos = 0; scan_pos < 61; scan_pos++)
    {
#pragma HLS UNROLL
        src_coef[scan_pos] = src_tmp[61 - scan_pos - 1];
    }
}
else if (cnt == 62)
{
    for (scan_pos = 0; scan_pos < 62; scan_pos++)
    {
#pragma HLS UNROLL
        src_coef[scan_pos] = src_tmp[scan_pos];
    }
}
else if (cnt == 63)
{
    for (scan_pos = 0; scan_pos < 63; scan_pos++)
    {
#pragma HLS UNROLL
        src_coef[scan_pos] = src_tmp[63 - scan_pos - 1];
    }
}
else if (cnt == 64)
{
    for (scan_pos = 0; scan_pos < 64; scan_pos++)
    {
#pragma HLS UNROLL
        src_coef[scan_pos] = src_tmp[scan_pos];
    }
}
#endif
#endif
#endif
#endif

}
void RDOQ_FUNC_NAME(out_coef, W, H)(int cnt, s16 dst_coef[MAT_SIZE], s16 dst_tmp[MAT_SIZE])
{
    int scan_pos;
    for (scan_pos = 0; scan_pos < MAT_SIZE; scan_pos++)
    {
#pragma HLS UNROLL
        if (scan_pos < cnt && cnt < MAT_SIZE)
        {
            if (cnt & 1)
            {
                dst_tmp[scan_pos] = dst_coef[cnt - scan_pos - 1];
            }
            else
            {
                dst_tmp[scan_pos] = dst_coef[scan_pos];
            }
        }
        else if (scan_pos < cnt && cnt >= MAT_SIZE)
        {
            if (cnt & 1)
            {
                dst_tmp[scan_pos] = dst_coef[MAT_SIZE - scan_pos - 1];
            }
            else
            {
                dst_tmp[scan_pos] = dst_coef[scan_pos];
            }
        }
    }
}
int RDOQ_FUNC_NAME(clean_nz, W, H)(s16 dst_tmp[H][W], int x_pos, int y_pos, int pzb)
{
    int i;
    int j;
    int mask[H][W] = { 0 };
#pragma HLS ARRAY_PARTITION variable = mask complete dim = 0
    int num_nz_coef = 0;
    for (i = 0; i < H; i++)
    {
#pragma HLS UNROLL
        for (j = 0; j < W; j++)
        {
#pragma HLS UNROLL
            if (!pzb)
                mask[i][j] = 0;
            else
            {
                if ((i + j) > x_pos)
                {
                    mask[i][j] = 0;
                }
                else if ((i + j) == x_pos)
                {
                    if (x_pos & 1)
                    {
                        if (i > y_pos)
                        {
                            mask[i][j] = 0;
                        }
                        else
                        {
                            mask[i][j] = 1;
                        }
                    }
                    else
                    {
                        if (i < y_pos)
                        {
                            mask[i][j] = 0;
                        }
                        else
                        {
                            mask[i][j] = 1;
                        }
                    }
                }
                else
                {
                    mask[i][j] = 1;
                }
            }
            dst_tmp[i][j] = dst_tmp[i][j] * mask[i][j];
            num_nz_coef += !!(dst_tmp[i][j]);
        }
    }
    return num_nz_coef;
}
void RDOQ_FUNC_NAME(read_data, W, H)(s16 coef64[MAT_SIZE], s16 src_coef[H][W], int i)
{
    int j;
#if (W > H)
#if (H == 8)
    if (i == 0)
    {
        coef64[0] = src_coef[0][i];
    }
    else if (i == 1)
    {
        for (j = 0; j <= 1; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 2)
    {
        for (j = 0; j <= 2; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 3)
    {
        for (j = 0; j <= 3; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 4)
    {
        for (j = 0; j <= 4; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 5)
    {
        for (j = 0; j <= 5; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 6)
    {
        for (j = 0; j <= 6; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 7)
    {
        for (j = 0; j <= 7; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 8)
    {
        for (j = 0; j <= 7; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 9)
    {
        for (j = 0; j <= 7; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 10)
    {
        for (j = 0; j <= 7; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 11)
    {
        for (j = 0; j <= 7; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 12)
    {
        for (j = 0; j <= 7; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 13)
    {
        for (j = 0; j <= 7; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 14)
    {
        for (j = 0; j <= 7; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 15)
    {
        for (j = 0; j <= 7; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 16)
    {
        for (j = 0; j <= 7; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 17)
    {
        for (j = 0; j <= 7; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 18)
    {
        for (j = 0; j <= 7; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 19)
    {
        for (j = 0; j <= 7; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 20)
    {
        for (j = 0; j <= 7; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 21)
    {
        for (j = 0; j <= 7; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 22)
    {
        for (j = 0; j <= 7; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 23)
    {
        for (j = 0; j <= 7; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 24)
    {
        for (j = 0; j <= 7; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 25)
    {
        for (j = 0; j <= 7; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 26)
    {
        for (j = 0; j <= 7; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 27)
    {
        for (j = 0; j <= 7; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 28)
    {
        for (j = 0; j <= 7; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 29)
    {
        for (j = 0; j <= 7; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 30)
    {
        for (j = 0; j <= 7; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 31)
    {
        for (j = 0; j <= 7; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 32)
{
for (j = 0; j <= 7; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
    else if (i == 33)
{
for (j = 0; j <= 7; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
    else if (i == 34)
{
for (j = 0; j <= 7; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
    else if (i == 35)
{
for (j = 0; j <= 7; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
    else if (i == 36)
{
for (j = 0; j <= 7; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
    else if (i == 37)
{
for (j = 0; j <= 7; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
    else if (i == 38)
{
for (j = 0; j <= 7; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
    else if (i == 39)
{
for (j = 0; j <= 7; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
    else if (i == 40)
{
for (j = 0; j <= 7; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
    else if (i == 41)
{
for (j = 0; j <= 7; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
    else if (i == 42)
{
for (j = 0; j <= 7; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
    else if (i == 43)
{
for (j = 0; j <= 7; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
    else if (i == 44)
{
for (j = 0; j <= 7; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
    else if (i == 45)
{
for (j = 0; j <= 7; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
    else if (i == 46)
{
for (j = 0; j <= 7; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
    else if (i == 47)
{
for (j = 0; j <= 7; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
    else if (i == 48)
{
for (j = 0; j <= 7; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
    else if (i == 49)
{
for (j = 0; j <= 7; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
    else if (i == 50)
{
for (j = 0; j <= 7; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
    else if (i == 51)
{
for (j = 0; j <= 7; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
    else if (i == 52)
{
for (j = 0; j <= 7; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
    else if (i == 53)
{
for (j = 0; j <= 7; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
    else if (i == 54)
{
for (j = 0; j <= 7; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
    else if (i == 55)
{
for (j = 0; j <= 7; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
    else if (i == 56)
{
for (j = 0; j <= 7; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
    else if (i == 57)
{
for (j = 0; j <= 7; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
    else if (i == 58)
{
for (j = 0; j <= 7; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
    else if (i == 59)
{
for (j = 0; j <= 7; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
    else if (i == 60)
{
for (j = 0; j <= 7; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
    else if (i == 61)
{
for (j = 0; j <= 7; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
    else if (i == 62)
{
for (j = 0; j <= 7; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
    else if (i == 63)
{
for (j = 0; j <= 7; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
#elif (H == 16)
    if (i == 0)
    {
        coef64[0] = src_coef[0][i];
    }
    else if (i == 1)
    {
        for (j = 0; j <= 1; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 2)
    {
        for (j = 0; j <= 2; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 3)
    {
        for (j = 0; j <= 3; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 4)
    {
        for (j = 0; j <= 4; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 5)
    {
        for (j = 0; j <= 5; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 6)
    {
        for (j = 0; j <= 6; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 7)
    {
        for (j = 0; j <= 7; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 8)
    {
        for (j = 0; j <= 8; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 9)
    {
        for (j = 0; j <= 9; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 10)
    {
        for (j = 0; j <= 10; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 11)
    {
        for (j = 0; j <= 11; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 12)
    {
        for (j = 0; j <= 12; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 13)
    {
        for (j = 0; j <= 13; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 14)
    {
        for (j = 0; j <= 14; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 15)
    {
        for (j = 0; j <= 15; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 16)
    {
        for (j = 0; j <= 15; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 17)
    {
        for (j = 0; j <= 15; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 18)
    {
        for (j = 0; j <= 15; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 19)
    {
        for (j = 0; j <= 15; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 20)
    {
        for (j = 0; j <= 15; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 21)
    {
        for (j = 0; j <= 15; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 22)
    {
        for (j = 0; j <= 15; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 23)
    {
        for (j = 0; j <= 15; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 24)
    {
        for (j = 0; j <= 15; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 25)
    {
        for (j = 0; j <= 15; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 26)
    {
        for (j = 0; j <= 15; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 27)
    {
        for (j = 0; j <= 15; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 28)
    {
        for (j = 0; j <= 15; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 29)
    {
        for (j = 0; j <= 15; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 30)
    {
        for (j = 0; j <= 15; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 31)
    {
        for (j = 0; j <= 15; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
#if (W > 32)
    else if (i == 32)
{
for (j = 0; j <= 15; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
    else if (i == 33)
{
for (j = 0; j <= 15; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
    else if (i == 34)
{
for (j = 0; j <= 15; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
    else if (i == 35)
{
for (j = 0; j <= 15; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
    else if (i == 36)
{
for (j = 0; j <= 15; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
    else if (i == 37)
{
for (j = 0; j <= 15; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
    else if (i == 38)
{
for (j = 0; j <= 15; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
    else if (i == 39)
{
for (j = 0; j <= 15; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
    else if (i == 40)
{
for (j = 0; j <= 15; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
    else if (i == 41)
{
for (j = 0; j <= 15; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
    else if (i == 42)
{
for (j = 0; j <= 15; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
    else if (i == 43)
{
for (j = 0; j <= 15; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
    else if (i == 44)
{
for (j = 0; j <= 15; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
    else if (i == 45)
{
for (j = 0; j <= 15; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
    else if (i == 46)
{
for (j = 0; j <= 15; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
    else if (i == 47)
{
for (j = 0; j <= 15; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
    else if (i == 48)
{
for (j = 0; j <= 15; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
    else if (i == 49)
{
for (j = 0; j <= 15; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
    else if (i == 50)
{
for (j = 0; j <= 15; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
    else if (i == 51)
{
for (j = 0; j <= 15; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
    else if (i == 52)
{
for (j = 0; j <= 15; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
    else if (i == 53)
{
for (j = 0; j <= 15; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
    else if (i == 54)
{
for (j = 0; j <= 15; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
    else if (i == 55)
{
for (j = 0; j <= 15; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
    else if (i == 56)
{
for (j = 0; j <= 15; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
    else if (i == 57)
{
for (j = 0; j <= 15; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
    else if (i == 58)
{
for (j = 0; j <= 15; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
    else if (i == 59)
{
for (j = 0; j <= 15; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
    else if (i == 60)
{
for (j = 0; j <= 15; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
    else if (i == 61)
{
for (j = 0; j <= 15; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
    else if (i == 62)
{
for (j = 0; j <= 15; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
    else if (i == 63)
{
for (j = 0; j <= 15; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}

#endif
#else
    if (i == 0)
    {
        coef64[0] = src_coef[0][i];
    }
    else if (i == 1)
    {
        for (j = 0; j <= 1; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 2)
    {
        for (j = 0; j <= 2; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 3)
    {
        for (j = 0; j <= 3; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 4)
    {
        for (j = 0; j <= 4; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 5)
    {
        for (j = 0; j <= 5; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 6)
    {
        for (j = 0; j <= 6; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 7)
    {
        for (j = 0; j <= 7; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 8)
    {
        for (j = 0; j <= 8; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 9)
    {
        for (j = 0; j <= 9; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 10)
    {
        for (j = 0; j <= 10; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 11)
    {
        for (j = 0; j <= 11; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 12)
    {
        for (j = 0; j <= 12; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 13)
    {
        for (j = 0; j <= 13; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 14)
    {
        for (j = 0; j <= 14; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 15)
    {
        for (j = 0; j <= 15; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 16)
    {
        for (j = 0; j <= 16; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 17)
    {
        for (j = 0; j <= 17; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 18)
    {
        for (j = 0; j <= 18; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 19)
    {
        for (j = 0; j <= 19; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 20)
    {
        for (j = 0; j <= 20; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 21)
    {
        for (j = 0; j <= 21; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 22)
    {
        for (j = 0; j <= 22; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 23)
    {
        for (j = 0; j <= 23; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 24)
    {
        for (j = 0; j <= 24; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 25)
    {
        for (j = 0; j <= 25; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 26)
    {
        for (j = 0; j <= 26; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 27)
    {
        for (j = 0; j <= 27; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 28)
    {
        for (j = 0; j <= 28; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 29)
    {
        for (j = 0; j <= 29; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 30)
    {
        for (j = 0; j <= 30; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 31)
    {
        for (j = 0; j <= 31; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 32)
    {
        for (j = 0; j <= 31; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 33)
    {
        for (j = 0; j <= 31; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 34)
    {
        for (j = 0; j <= 31; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 35)
    {
        for (j = 0; j <= 31; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 36)
    {
        for (j = 0; j <= 31; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 37)
    {
        for (j = 0; j <= 31; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 38)
    {
        for (j = 0; j <= 31; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 39)
    {
        for (j = 0; j <= 31; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 40)
    {
        for (j = 0; j <= 31; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 41)
    {
        for (j = 0; j <= 31; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 42)
    {
        for (j = 0; j <= 31; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 43)
    {
        for (j = 0; j <= 31; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 44)
    {
        for (j = 0; j <= 31; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 45)
    {
        for (j = 0; j <= 31; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 46)
    {
        for (j = 0; j <= 31; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 47)
    {
        for (j = 0; j <= 31; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 48)
    {
        for (j = 0; j <= 31; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 49)
    {
        for (j = 0; j <= 31; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 50)
    {
        for (j = 0; j <= 31; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 51)
    {
        for (j = 0; j <= 31; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 52)
    {
        for (j = 0; j <= 31; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 53)
    {
        for (j = 0; j <= 31; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 54)
    {
        for (j = 0; j <= 31; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 55)
    {
        for (j = 0; j <= 31; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 56)
    {
        for (j = 0; j <= 31; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 57)
    {
        for (j = 0; j <= 31; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 58)
    {
        for (j = 0; j <= 31; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 59)
    {
        for (j = 0; j <= 31; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 60)
    {
        for (j = 0; j <= 31; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 61)
    {
        for (j = 0; j <= 31; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 62)
    {
        for (j = 0; j <= 31; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
    else if (i == 63)
    {
        for (j = 0; j <= 31; j++)
        {
#pragma HLS UNROLL
            coef64[j] = src_coef[j][i - j];
        }
    }
#endif
#else
if (i == 0)
{
    coef64[0] = src_coef[0][i];
}
else if (i == 1)
{
    for (j = 0; j <= 1; j++)
    {
#pragma HLS UNROLL
        coef64[j] = src_coef[j][i - j];
    }
}
else if (i == 2)
{
    for (j = 0; j <= 2; j++)
    {
#pragma HLS UNROLL
        coef64[j] = src_coef[j][i - j];
    }
}
else if (i == 3)
{
    for (j = 0; j <= 3; j++)
    {
#pragma HLS UNROLL
        coef64[j] = src_coef[j][i - j];
    }
}
else if (i == 4)
{
    for (j = 0; j <= 4; j++)
    {
#pragma HLS UNROLL
        coef64[j] = src_coef[j][i - j];
    }
}
else if (i == 5)
{
    for (j = 0; j <= 5; j++)
    {
#pragma HLS UNROLL
        coef64[j] = src_coef[j][i - j];
    }
}
else if (i == 6)
{
    for (j = 0; j <= 6; j++)
    {
#pragma HLS UNROLL
        coef64[j] = src_coef[j][i - j];
    }
}
else if (i == 7)
{
    for (j = 0; j <= 7; j++)
    {
#pragma HLS UNROLL
        coef64[j] = src_coef[j][i - j];
    }
}
#if (W > 8)
else if (i == 8)
{
    for (j = 0; j <= 8; j++)
    {
#pragma HLS UNROLL
        coef64[j] = src_coef[j][i - j];
    }
}
else if (i == 9)
{
    for (j = 0; j <= 9; j++)
    {
#pragma HLS UNROLL
        coef64[j] = src_coef[j][i - j];
    }
}
else if (i == 10)
{
    for (j = 0; j <= 10; j++)
    {
#pragma HLS UNROLL
        coef64[j] = src_coef[j][i - j];
    }
}
else if (i == 11)
{
    for (j = 0; j <= 11; j++)
    {
#pragma HLS UNROLL
        coef64[j] = src_coef[j][i - j];
    }
}
else if (i == 12)
{
    for (j = 0; j <= 12; j++)
    {
#pragma HLS UNROLL
        coef64[j] = src_coef[j][i - j];
    }
}
else if (i == 13)
{
    for (j = 0; j <= 13; j++)
    {
#pragma HLS UNROLL
        coef64[j] = src_coef[j][i - j];
    }
}
else if (i == 14)
{
    for (j = 0; j <= 14; j++)
    {
#pragma HLS UNROLL
        coef64[j] = src_coef[j][i - j];
    }
}
else if (i == 15)
{
    for (j = 0; j <= 15; j++)
    {
#pragma HLS UNROLL
        coef64[j] = src_coef[j][i - j];
    }
}
#if (W > 16)
else if (i == 16)
{
    for (j = 0; j <= 16; j++)
    {
#pragma HLS UNROLL
        coef64[j] = src_coef[j][i - j];
    }
}
else if (i == 17)
{
    for (j = 0; j <= 17; j++)
    {
#pragma HLS UNROLL
        coef64[j] = src_coef[j][i - j];
    }
}
else if (i == 18)
{
    for (j = 0; j <= 18; j++)
    {
#pragma HLS UNROLL
        coef64[j] = src_coef[j][i - j];
    }
}
else if (i == 19)
{
    for (j = 0; j <= 19; j++)
    {
#pragma HLS UNROLL
        coef64[j] = src_coef[j][i - j];
    }
}
else if (i == 20)
{
    for (j = 0; j <= 20; j++)
    {
#pragma HLS UNROLL
        coef64[j] = src_coef[j][i - j];
    }
}
else if (i == 21)
{
    for (j = 0; j <= 21; j++)
    {
#pragma HLS UNROLL
        coef64[j] = src_coef[j][i - j];
    }
}
else if (i == 22)
{
    for (j = 0; j <= 22; j++)
    {
#pragma HLS UNROLL
        coef64[j] = src_coef[j][i - j];
    }
}
else if (i == 23)
{
    for (j = 0; j <= 23; j++)
    {
#pragma HLS UNROLL
        coef64[j] = src_coef[j][i - j];
    }
}
else if (i == 24)
{
    for (j = 0; j <= 24; j++)
    {
#pragma HLS UNROLL
        coef64[j] = src_coef[j][i - j];
    }
}
else if (i == 25)
{
    for (j = 0; j <= 25; j++)
    {
#pragma HLS UNROLL
        coef64[j] = src_coef[j][i - j];
    }
}
else if (i == 26)
{
    for (j = 0; j <= 26; j++)
    {
#pragma HLS UNROLL
        coef64[j] = src_coef[j][i - j];
    }
}
else if (i == 27)
{
    for (j = 0; j <= 27; j++)
    {
#pragma HLS UNROLL
        coef64[j] = src_coef[j][i - j];
    }
}
else if (i == 28)
{
    for (j = 0; j <= 28; j++)
    {
#pragma HLS UNROLL
        coef64[j] = src_coef[j][i - j];
    }
}
else if (i == 29)
{
    for (j = 0; j <= 29; j++)
    {
#pragma HLS UNROLL
        coef64[j] = src_coef[j][i - j];
    }
}
else if (i == 30)
{
    for (j = 0; j <= 30; j++)
    {
#pragma HLS UNROLL
        coef64[j] = src_coef[j][i - j];
    }
}
else if (i == 31)
{
    for (j = 0; j <= 31; j++)
    {
#pragma HLS UNROLL
        coef64[j] = src_coef[j][i - j];
    }
}
#if (W > 32)
else if (i == 32)
{
for (j = 0; j <= 32; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
else if (i == 33)
{
for (j = 0; j <= 33; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
else if (i == 34)
{
for (j = 0; j <= 34; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
else if (i == 35)
{
for (j = 0; j <= 35; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
else if (i == 36)
{
for (j = 0; j <= 36; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
else if (i == 37)
{
for (j = 0; j <= 37; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
else if (i == 38)
{
for (j = 0; j <= 38; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
else if (i == 39)
{
for (j = 0; j <= 39; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
else if (i == 40)
{
for (j = 0; j <= 40; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
else if (i == 41)
{
for (j = 0; j <= 41; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
else if (i == 42)
{
for (j = 0; j <= 42; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
else if (i == 43)
{
for (j = 0; j <= 43; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
else if (i == 44)
{
for (j = 0; j <= 44; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
else if (i == 45)
{
for (j = 0; j <= 45; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
else if (i == 46)
{
for (j = 0; j <= 46; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
else if (i == 47)
{
for (j = 0; j <= 47; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
else if (i == 48)
{
for (j = 0; j <= 48; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
else if (i == 49)
{
for (j = 0; j <= 49; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
else if (i == 50)
{
for (j = 0; j <= 50; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
else if (i == 51)
{
for (j = 0; j <= 51; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
else if (i == 52)
{
for (j = 0; j <= 52; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
else if (i == 53)
{
for (j = 0; j <= 53; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
else if (i == 54)
{
for (j = 0; j <= 54; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
else if (i == 55)
{
for (j = 0; j <= 55; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
else if (i == 56)
{
for (j = 0; j <= 56; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
else if (i == 57)
{
for (j = 0; j <= 57; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
else if (i == 58)
{
for (j = 0; j <= 58; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
else if (i == 59)
{
for (j = 0; j <= 59; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
else if (i == 60)
{
for (j = 0; j <= 60; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
else if (i == 61)
{
for (j = 0; j <= 61; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
else if (i == 62)
{
for (j = 0; j <= 62; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
else if (i == 63)
{
for (j = 0; j <= 63; j++)
{
#pragma HLS UNROLL
    coef64[j] = src_coef[j][i - j];
}
}
#endif
#endif
#endif
#endif
}
void RDOQ_FUNC_NAME(write_data, W, H)(s16 dst_tmp[H][W], s16 dst64[MAT_SIZE], int i)
{
    int j;
#if (W > H)
#if (H == 8)
    if (i == 0)
    {
        dst_tmp[0][i] = dst64[0];
    }
    else if (i == 1)
    {
        for (j = 0; j <= 1; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 2)
    {
        for (j = 0; j <= 2; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 3)
    {
        for (j = 0; j <= 3; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 4)
    {
        for (j = 0; j <= 4; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 5)
    {
        for (j = 0; j <= 5; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 6)
    {
        for (j = 0; j <= 6; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 7)
    {
        for (j = 0; j <= 7; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 8)
    {
        for (j = 0; j <= 7; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 9)
    {
        for (j = 0; j <= 7; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 10)
    {
        for (j = 0; j <= 7; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 11)
    {
        for (j = 0; j <= 7; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 12)
    {
        for (j = 0; j <= 7; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 13)
    {
        for (j = 0; j <= 7; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 14)
    {
        for (j = 0; j <= 7; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 15)
    {
        for (j = 0; j <= 7; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 16)
    {
        for (j = 0; j <= 7; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 17)
    {
        for (j = 0; j <= 7; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 18)
    {
        for (j = 0; j <= 7; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 19)
    {
        for (j = 0; j <= 7; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 20)
    {
        for (j = 0; j <= 7; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 21)
    {
        for (j = 0; j <= 7; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 22)
    {
        for (j = 0; j <= 7; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 23)
    {
        for (j = 0; j <= 7; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 24)
    {
        for (j = 0; j <= 7; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 25)
    {
        for (j = 0; j <= 7; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 26)
    {
        for (j = 0; j <= 7; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 27)
    {
        for (j = 0; j <= 7; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 28)
    {
        for (j = 0; j <= 7; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 29)
    {
        for (j = 0; j <= 7; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 30)
    {
        for (j = 0; j <= 7; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 31)
    {
        for (j = 0; j <= 7; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 32)
{
for (j = 0; j <= 7; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
    else if (i == 33)
{
for (j = 0; j <= 7; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
    else if (i == 34)
{
for (j = 0; j <= 7; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
    else if (i == 35)
{
for (j = 0; j <= 7; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
    else if (i == 36)
{
for (j = 0; j <= 7; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
    else if (i == 37)
{
for (j = 0; j <= 7; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
    else if (i == 38)
{
for (j = 0; j <= 7; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
    else if (i == 39)
{
for (j = 0; j <= 7; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
    else if (i == 40)
{
for (j = 0; j <= 7; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
    else if (i == 41)
{
for (j = 0; j <= 7; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
    else if (i == 42)
{
for (j = 0; j <= 7; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
    else if (i == 43)
{
for (j = 0; j <= 7; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
    else if (i == 44)
{
for (j = 0; j <= 7; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
    else if (i == 45)
{
for (j = 0; j <= 7; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
    else if (i == 46)
{
for (j = 0; j <= 7; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
    else if (i == 47)
{
for (j = 0; j <= 7; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
    else if (i == 48)
{
for (j = 0; j <= 7; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
    else if (i == 49)
{
for (j = 0; j <= 7; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
    else if (i == 50)
{
for (j = 0; j <= 7; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
    else if (i == 51)
{
for (j = 0; j <= 7; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
    else if (i == 52)
{
for (j = 0; j <= 7; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
    else if (i == 53)
{
for (j = 0; j <= 7; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
    else if (i == 54)
{
for (j = 0; j <= 7; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
    else if (i == 55)
{
for (j = 0; j <= 7; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
    else if (i == 56)
{
for (j = 0; j <= 7; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
    else if (i == 57)
{
for (j = 0; j <= 7; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
    else if (i == 58)
{
for (j = 0; j <= 7; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
    else if (i == 59)
{
for (j = 0; j <= 7; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
    else if (i == 60)
{
for (j = 0; j <= 7; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
    else if (i == 61)
{
for (j = 0; j <= 7; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
    else if (i == 62)
{
for (j = 0; j <= 7; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
    else if (i == 63)
{
for (j = 0; j <= 7; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
#elif (H == 16)
    if (i == 0)
    {
        dst_tmp[0][i] = dst64[0];
    }
    else if (i == 1)
    {
        for (j = 0; j <= 1; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 2)
    {
        for (j = 0; j <= 2; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 3)
    {
        for (j = 0; j <= 3; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 4)
    {
        for (j = 0; j <= 4; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 5)
    {
        for (j = 0; j <= 5; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 6)
    {
        for (j = 0; j <= 6; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 7)
    {
        for (j = 0; j <= 7; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 8)
    {
        for (j = 0; j <= 8; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 9)
    {
        for (j = 0; j <= 9; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 10)
    {
        for (j = 0; j <= 10; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 11)
    {
        for (j = 0; j <= 11; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 12)
    {
        for (j = 0; j <= 12; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 13)
    {
        for (j = 0; j <= 13; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 14)
    {
        for (j = 0; j <= 14; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 15)
    {
        for (j = 0; j <= 15; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 16)
    {
        for (j = 0; j <= 15; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 17)
    {
        for (j = 0; j <= 15; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 18)
    {
        for (j = 0; j <= 15; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 19)
    {
        for (j = 0; j <= 15; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 20)
    {
        for (j = 0; j <= 15; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 21)
    {
        for (j = 0; j <= 15; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 22)
    {
        for (j = 0; j <= 15; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 23)
    {
        for (j = 0; j <= 15; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 24)
    {
        for (j = 0; j <= 15; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 25)
    {
        for (j = 0; j <= 15; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 26)
    {
        for (j = 0; j <= 15; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 27)
    {
        for (j = 0; j <= 15; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 28)
    {
        for (j = 0; j <= 15; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 29)
    {
        for (j = 0; j <= 15; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 30)
    {
        for (j = 0; j <= 15; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 31)
    {
        for (j = 0; j <= 15; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }

#if (W > 32)
    else if (i == 32)
{
for (j = 0; j <= 15; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
    else if (i == 33)
{
for (j = 0; j <= 15; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
    else if (i == 34)
{
for (j = 0; j <= 15; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
    else if (i == 35)
{
for (j = 0; j <= 15; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
    else if (i == 36)
{
for (j = 0; j <= 15; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
    else if (i == 37)
{
for (j = 0; j <= 15; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
    else if (i == 38)
{
for (j = 0; j <= 15; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
    else if (i == 39)
{
for (j = 0; j <= 15; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
    else if (i == 40)
{
for (j = 0; j <= 15; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
    else if (i == 41)
{
for (j = 0; j <= 15; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
    else if (i == 42)
{
for (j = 0; j <= 15; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
    else if (i == 43)
{
for (j = 0; j <= 15; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
    else if (i == 44)
{
for (j = 0; j <= 15; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
    else if (i == 45)
{
for (j = 0; j <= 15; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
    else if (i == 46)
{
for (j = 0; j <= 15; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
    else if (i == 47)
{
for (j = 0; j <= 15; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
    else if (i == 48)
{
for (j = 0; j <= 15; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
    else if (i == 49)
{
for (j = 0; j <= 15; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
    else if (i == 50)
{
for (j = 0; j <= 15; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
    else if (i == 51)
{
for (j = 0; j <= 15; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
    else if (i == 52)
{
for (j = 0; j <= 15; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
    else if (i == 53)
{
for (j = 0; j <= 15; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
    else if (i == 54)
{
for (j = 0; j <= 15; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
    else if (i == 55)
{
for (j = 0; j <= 15; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
    else if (i == 56)
{
for (j = 0; j <= 15; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
    else if (i == 57)
{
for (j = 0; j <= 15; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
    else if (i == 58)
{
for (j = 0; j <= 15; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
    else if (i == 59)
{
for (j = 0; j <= 15; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
    else if (i == 60)
{
for (j = 0; j <= 15; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
    else if (i == 61)
{
for (j = 0; j <= 15; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
    else if (i == 62)
{
for (j = 0; j <= 15; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
    else if (i == 63)
{
for (j = 0; j <= 15; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
#endif
#else
    if (i == 0)
    {
        dst_tmp[0][i] = dst64[0];
    }
    else if (i == 1)
    {
        for (j = 0; j <= 1; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 2)
    {
        for (j = 0; j <= 2; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 3)
    {
        for (j = 0; j <= 3; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 4)
    {
        for (j = 0; j <= 4; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 5)
    {
        for (j = 0; j <= 5; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 6)
    {
        for (j = 0; j <= 6; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 7)
    {
        for (j = 0; j <= 7; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 8)
    {
        for (j = 0; j <= 8; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 9)
    {
        for (j = 0; j <= 9; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 10)
    {
        for (j = 0; j <= 10; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 11)
    {
        for (j = 0; j <= 11; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 12)
    {
        for (j = 0; j <= 12; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 13)
    {
        for (j = 0; j <= 13; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 14)
    {
        for (j = 0; j <= 14; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 15)
    {
        for (j = 0; j <= 15; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 16)
    {
        for (j = 0; j <= 16; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 17)
    {
        for (j = 0; j <= 17; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 18)
    {
        for (j = 0; j <= 18; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 19)
    {
        for (j = 0; j <= 19; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 20)
    {
        for (j = 0; j <= 20; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 21)
    {
        for (j = 0; j <= 21; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 22)
    {
        for (j = 0; j <= 22; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 23)
    {
        for (j = 0; j <= 23; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 24)
    {
        for (j = 0; j <= 24; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 25)
    {
        for (j = 0; j <= 25; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 26)
    {
        for (j = 0; j <= 26; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 27)
    {
        for (j = 0; j <= 27; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 28)
    {
        for (j = 0; j <= 28; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 29)
    {
        for (j = 0; j <= 29; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 30)
    {
        for (j = 0; j <= 30; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 31)
    {
        for (j = 0; j <= 31; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 32)
    {
        for (j = 0; j <= 31; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 33)
    {
        for (j = 0; j <= 31; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 34)
    {
        for (j = 0; j <= 31; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 35)
    {
        for (j = 0; j <= 31; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 36)
    {
        for (j = 0; j <= 31; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 37)
    {
        for (j = 0; j <= 31; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 38)
    {
        for (j = 0; j <= 31; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 39)
    {
        for (j = 0; j <= 31; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 40)
    {
        for (j = 0; j <= 31; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 41)
    {
        for (j = 0; j <= 31; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 42)
    {
        for (j = 0; j <= 31; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 43)
    {
        for (j = 0; j <= 31; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 44)
    {
        for (j = 0; j <= 31; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 45)
    {
        for (j = 0; j <= 31; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 46)
    {
        for (j = 0; j <= 31; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 47)
    {
        for (j = 0; j <= 31; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 48)
    {
        for (j = 0; j <= 31; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 49)
    {
        for (j = 0; j <= 31; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 50)
    {
        for (j = 0; j <= 31; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 51)
    {
        for (j = 0; j <= 31; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 52)
    {
        for (j = 0; j <= 31; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 53)
    {
        for (j = 0; j <= 31; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 54)
    {
        for (j = 0; j <= 31; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 55)
    {
        for (j = 0; j <= 31; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 56)
    {
        for (j = 0; j <= 31; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 57)
    {
        for (j = 0; j <= 31; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 58)
    {
        for (j = 0; j <= 31; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 59)
    {
        for (j = 0; j <= 31; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 60)
    {
        for (j = 0; j <= 31; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 61)
    {
        for (j = 0; j <= 31; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 62)
    {
        for (j = 0; j <= 31; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
    else if (i == 63)
    {
        for (j = 0; j <= 31; j++)
        {
#pragma HLS UNROLL
            dst_tmp[j][i - j] = dst64[j];
        }
    }
#endif
#else
if (i == 0)
{
    dst_tmp[0][i] = dst64[0];
}
else if (i == 1)
{
    for (j = 0; j <= 1; j++)
    {
#pragma HLS UNROLL
        dst_tmp[j][i - j] = dst64[j];
    }
}
else if (i == 2)
{
    for (j = 0; j <= 2; j++)
    {
#pragma HLS UNROLL
        dst_tmp[j][i - j] = dst64[j];
    }
}
else if (i == 3)
{
    for (j = 0; j <= 3; j++)
    {
#pragma HLS UNROLL
        dst_tmp[j][i - j] = dst64[j];
    }
}
else if (i == 4)
{
    for (j = 0; j <= 4; j++)
    {
#pragma HLS UNROLL
        dst_tmp[j][i - j] = dst64[j];
    }
}
else if (i == 5)
{
    for (j = 0; j <= 5; j++)
    {
#pragma HLS UNROLL
        dst_tmp[j][i - j] = dst64[j];
    }
}
else if (i == 6)
{
    for (j = 0; j <= 6; j++)
    {
#pragma HLS UNROLL
        dst_tmp[j][i - j] = dst64[j];
    }
}
else if (i == 7)
{
    for (j = 0; j <= 7; j++)
    {
#pragma HLS UNROLL
        dst_tmp[j][i - j] = dst64[j];
    }
}
#if (W > 8)
else if (i == 8)
{
    for (j = 0; j <= 8; j++)
    {
#pragma HLS UNROLL
        dst_tmp[j][i - j] = dst64[j];
    }
}
else if (i == 9)
{
    for (j = 0; j <= 9; j++)
    {
#pragma HLS UNROLL
        dst_tmp[j][i - j] = dst64[j];
    }
}
else if (i == 10)
{
    for (j = 0; j <= 10; j++)
    {
#pragma HLS UNROLL
        dst_tmp[j][i - j] = dst64[j];
    }
}
else if (i == 11)
{
    for (j = 0; j <= 11; j++)
    {
#pragma HLS UNROLL
        dst_tmp[j][i - j] = dst64[j];
    }
}
else if (i == 12)
{
    for (j = 0; j <= 12; j++)
    {
#pragma HLS UNROLL
        dst_tmp[j][i - j] = dst64[j];
    }
}
else if (i == 13)
{
    for (j = 0; j <= 13; j++)
    {
#pragma HLS UNROLL
        dst_tmp[j][i - j] = dst64[j];
    }
}
else if (i == 14)
{
    for (j = 0; j <= 14; j++)
    {
#pragma HLS UNROLL
        dst_tmp[j][i - j] = dst64[j];
    }
}
else if (i == 15)
{
    for (j = 0; j <= 15; j++)
    {
#pragma HLS UNROLL
        dst_tmp[j][i - j] = dst64[j];
    }
}
#if (W > 16)
else if (i == 16)
{
for (j = 0; j <= 16; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
else if (i == 17)
{
for (j = 0; j <= 17; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
else if (i == 18)
{
for (j = 0; j <= 18; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
else if (i == 19)
{
for (j = 0; j <= 19; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
else if (i == 20)
{
for (j = 0; j <= 20; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
else if (i == 21)
{
for (j = 0; j <= 21; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
else if (i == 22)
{
for (j = 0; j <= 22; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
else if (i == 23)
{
for (j = 0; j <= 23; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
else if (i == 24)
{
for (j = 0; j <= 24; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
else if (i == 25)
{
for (j = 0; j <= 25; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
else if (i == 26)
{
for (j = 0; j <= 26; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
else if (i == 27)
{
for (j = 0; j <= 27; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
else if (i == 28)
{
for (j = 0; j <= 28; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
else if (i == 29)
{
for (j = 0; j <= 29; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
else if (i == 30)
{
for (j = 0; j <= 30; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
else if (i == 31)
{
for (j = 0; j <= 31; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
#if (W > 32)
else if (i == 32)
{
for (j = 0; j <= 32; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
else if (i == 33)
{
for (j = 0; j <= 33; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
else if (i == 34)
{
for (j = 0; j <= 34; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
else if (i == 35)
{
for (j = 0; j <= 35; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
else if (i == 36)
{
for (j = 0; j <= 36; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
else if (i == 37)
{
for (j = 0; j <= 37; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
else if (i == 38)
{
for (j = 0; j <= 38; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
else if (i == 39)
{
for (j = 0; j <= 39; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
else if (i == 40)
{
for (j = 0; j <= 40; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
else if (i == 41)
{
for (j = 0; j <= 41; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
else if (i == 42)
{
for (j = 0; j <= 42; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
else if (i == 43)
{
for (j = 0; j <= 43; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
else if (i == 44)
{
for (j = 0; j <= 44; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
else if (i == 45)
{
for (j = 0; j <= 45; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
else if (i == 46)
{
for (j = 0; j <= 46; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
else if (i == 47)
{
for (j = 0; j <= 47; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
else if (i == 48)
{
for (j = 0; j <= 48; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
else if (i == 49)
{
for (j = 0; j <= 49; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
else if (i == 50)
{
for (j = 0; j <= 50; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
else if (i == 51)
{
for (j = 0; j <= 51; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
else if (i == 52)
{
for (j = 0; j <= 52; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
else if (i == 53)
{
for (j = 0; j <= 53; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
else if (i == 54)
{
for (j = 0; j <= 54; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
else if (i == 55)
{
for (j = 0; j <= 55; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
else if (i == 56)
{
for (j = 0; j <= 56; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
else if (i == 57)
{
for (j = 0; j <= 57; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
else if (i == 58)
{
for (j = 0; j <= 58; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
else if (i == 59)
{
for (j = 0; j <= 59; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
else if (i == 60)
{
for (j = 0; j <= 60; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
else if (i == 61)
{
for (j = 0; j <= 61; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
else if (i == 62)
{
for (j = 0; j <= 62; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
else if (i == 63)
{
for (j = 0; j <= 63; j++)
{
#pragma HLS UNROLL
    dst_tmp[j][i - j] = dst64[j];
}
}
#endif
#endif
#endif
#endif
}
void RDOQ_FUNC_NAME(rdoq_core, W, H)(RDOQ_MODEL* model_cnt, U24 last_level[W + 1], U24 prev_level[W + 1], int* pzb, s64 base_cost, int* x_pos, int* y_pos, s64 best_cost_out[W + 1], s64 last_cost[W + 1], int q_value, int cnt, int q_bits, s64 err_scale, s64 lambda, int is_intra, s16 coef64[MAT_SIZE], s16 dst_coef[MAT_SIZE], int ch_type, s32 rdoq_est_run_local[24][2], s32 rdoq_est_level_local[24][2], s32 rdoq_est_last_local[2][6][12][2])
{
#pragma HLS PIPELINE II=1
    const int ctx_last = (ch_type == Y_C) ? 0 : 1;
    int num_nz_coef = 0;
    int cyc = cnt - 1;
    int scan_pos;
    s64 d64_base_cost[MAT_SIZE] = { 0 };
    s64 d64_coded_cost = 0;
    s64 d64_uncoded_cost = 0;
    s16 tmp_coef[MAT_SIZE] = { 0 };
    s16 src_coef[MAT_SIZE] = { 0 };
    U36 tmp_level_double[MAT_SIZE] = { 0 };
    s64 best_cost = best_cost_out[cyc];
#pragma HLS ARRAY_PARTITION variable = d64_base_cost complete dim = 0
#pragma HLS ARRAY_PARTITION variable = tmp_coef complete dim = 0
#pragma HLS ARRAY_PARTITION variable = src_coef complete dim = 0
#pragma HLS ARRAY_PARTITION variable = tmp_level_double complete dim = 0

    int last_idx = (((1 + cnt) * cnt) >> 1) - 1;
    int base_idx = ((1 + cyc) * cyc >> 1);

    RDOQ_FUNC_NAME(rdoq_data_prepare, W, H)(ch_type, lambda, cnt, coef64, src_coef, is_intra);

    for (scan_pos = 0; scan_pos < MAT_SIZE; scan_pos++)
    {
#pragma HLS UNROLL
        U36 level_double = src_coef[scan_pos];
        U24 max_abs_level;
        U1 lower_int;
        U36 temp_level;
        temp_level = ((s64)COM_ABS(src_coef[scan_pos]) * (s64)q_value);
        level_double = (int)COM_MIN(((s64)temp_level), (s64)COM_INT32_MAX - (1 << (s64)(q_bits - 1)));
        tmp_level_double[scan_pos] = level_double;
        max_abs_level = (u32)(level_double >> q_bits);
        lower_int = ((level_double - ((s64)max_abs_level << q_bits)) < (1 << (s64)(q_bits - 1))) ? 1 : 0;
        if (!lower_int)
        {
            max_abs_level++;
        }
        tmp_coef[scan_pos] = src_coef[scan_pos] > 0 ? (s16)max_abs_level : -(s16)(max_abs_level);
    }

    U24 mat_prev_level[MAT_SIZE] = { 0 };
    U1 mat_run[MAT_SIZE] = { 0 };
#pragma HLS ARRAY_PARTITION variable = mat_prev_level complete dim = 0
#pragma HLS ARRAY_PARTITION variable = mat_run complete dim = 0

    if (last_level[cyc])
    {
        mat_prev_level[0] = last_level[cyc];
        mat_run[0] = 0;
    }
    else
    {
        mat_prev_level[0] = prev_level[cyc];
        mat_run[0] = 1;
    }

    for (scan_pos = 1; scan_pos < MAT_SIZE; scan_pos++)
    {
#pragma HLS UNROLL
        if (COM_ABS(tmp_coef[scan_pos - 1]))
        {
            mat_prev_level[scan_pos] = COM_ABS(tmp_coef[scan_pos - 1]);
            mat_run[scan_pos] = 0;
        }
        else
        {
            mat_prev_level[scan_pos] = mat_prev_level[scan_pos - 1];
            mat_run[scan_pos] = 1;
        }
    }

    s64 mat_uncoded_cost[MAT_SIZE] = { 0 };
    s64 mat_coded_cost[MAT_SIZE] = { 0 };
    U24 lev_opt[MAT_SIZE] = { 0 };
    u16 ctx_run = 0;
#pragma HLS ARRAY_PARTITION variable = mat_uncoded_cost complete dim = 0
#pragma HLS ARRAY_PARTITION variable = mat_coded_cost complete dim = 0
#pragma HLS ARRAY_PARTITION variable = lev_opt complete dim = 0

    for (scan_pos = 0; scan_pos < MAT_SIZE; scan_pos++)
    {
#pragma HLS UNROLL
        U24 level;
        U5 ctx_run = ((COM_MIN((U24)(mat_prev_level[scan_pos] - 1), (U24)5)) << 1) + (ch_type == Y_C ? 0 : 12);

        level = get_coded_level_rl(&d64_uncoded_cost, &d64_coded_cost, tmp_level_double[scan_pos], COM_ABS(tmp_coef[scan_pos]),
            mat_run[scan_pos], ctx_run, (u16)ctx_run, q_bits, err_scale, lambda, 0, rdoq_est_run_local, rdoq_est_level_local);
        dst_coef[scan_pos] = (s16)(tmp_coef[scan_pos] < 0 ? -(s16)(level) : (s16)level);

        mat_uncoded_cost[scan_pos] = d64_uncoded_cost;
        mat_coded_cost[scan_pos] = d64_coded_cost;
        lev_opt[scan_pos] = level;
    }

    s64 d64_cost_last_zero[MAT_SIZE] = { 0 };
#pragma HLS ARRAY_PARTITION variable = d64_cost_last_zero complete dim = 0
    int ace_log2 = 0;
    if (cyc == 0)
    {
        d64_cost_last_zero[0] = 0;
    }
    else
    {
        if ((base_idx) < 2)
        {
            ace_log2 = 0;
        }
        else if ((base_idx) < 4)
        {
            ace_log2 = 1;
        }
        else if ((base_idx) < 8)
        {
            ace_log2 = 2;
        }
        else if ((base_idx) < 16)
        {
            ace_log2 = 3;
        }
        else if ((base_idx) < 32)
        {
            ace_log2 = 4;
        }
        else if ((base_idx) < 64)
        {
            ace_log2 = 5;
        }
        else if ((base_idx) < 128)
        {
            ace_log2 = 6;
        }
        else if ((base_idx) < 256)
        {
            ace_log2 = 7;
        }
        else if ((base_idx) < 512)
        {
            ace_log2 = 8;
        }
        else if ((base_idx) < 1024)
        {
            ace_log2 = 9;
        }
        else if ((base_idx) < 2048)
        {
            ace_log2 = 10;
        }
        else if ((base_idx) < 4096)
        {
            ace_log2 = 11;
        }
        else
        {
            ace_log2 = 11;
        }
        d64_cost_last_zero[0] = GET_I_COST(rdoq_est_last_local[ctx_last][COM_MIN((U24)(prev_level[cyc] - 1), (U24)5)][ace_log2][0], lambda);

    }

    for (scan_pos = 1; scan_pos < MAT_SIZE; scan_pos++)
    {
#pragma HLS UNROLL
        if ((scan_pos + base_idx) < 2)
        {
            ace_log2 = 0;
        }
        else if ((scan_pos + base_idx) < 4)
        {
            ace_log2 = 1;
        }
        else if ((scan_pos + base_idx) < 8)
        {
            ace_log2 = 2;
        }
        else if ((scan_pos + base_idx) < 16)
        {
            ace_log2 = 3;
        }
        else if ((scan_pos + base_idx) < 32)
        {
            ace_log2 = 4;
        }
        else if ((scan_pos + base_idx) < 64)
        {
            ace_log2 = 5;
        }
        else if ((scan_pos + base_idx) < 128)
        {
            ace_log2 = 6;
        }
        else if ((scan_pos + base_idx) < 256)
        {
            ace_log2 = 7;
        }
        else if ((scan_pos + base_idx) < 512)
        {
            ace_log2 = 8;
        }
        else if ((scan_pos + base_idx) < 1024)
        {
            ace_log2 = 9;
        }
        else if ((scan_pos + base_idx) < 2048)
        {
            ace_log2 = 10;
        }
        else if ((scan_pos + base_idx) < 4096)
        {
            ace_log2 = 11;
        }
        else
        {
            ace_log2 = 11;
        }
        d64_cost_last_zero[scan_pos] = GET_I_COST(rdoq_est_last_local[ctx_last][COM_MIN((U24)(mat_prev_level[scan_pos - 1] - 1), (U24)5)][ace_log2][0], lambda);
    }

    if (cnt != 1)
        d64_base_cost[0] = last_cost[cyc];
    else
        d64_base_cost[0] = base_cost;

    d64_base_cost[0] -= mat_uncoded_cost[0];
    d64_base_cost[0] += mat_coded_cost[0];
    d64_base_cost[0] += d64_cost_last_zero[0] * !!(lev_opt[0]);
    for (scan_pos = 1; scan_pos < MAT_SIZE; scan_pos++)
    {
#pragma HLS UNROLL
        d64_base_cost[scan_pos] = d64_base_cost[0];
        for (int i = 1; i <= scan_pos; i++)
        {
#pragma HLS UNROLL
            d64_base_cost[scan_pos] += mat_coded_cost[i] - mat_uncoded_cost[i] + d64_cost_last_zero[i] * !!(lev_opt[i]);
        }
    }

    for (scan_pos = 0; scan_pos < MAT_SIZE; scan_pos++)
    {
#pragma HLS UNROLL
        if (scan_pos < cnt)
        {
            if ((scan_pos + base_idx + 1) < 2)
            {
                ace_log2 = 0;
            }
            else if ((scan_pos + base_idx + 1) < 4)
            {
                ace_log2 = 1;
            }
            else if ((scan_pos + base_idx + 1) < 8)
            {
                ace_log2 = 2;
            }
            else if ((scan_pos + base_idx + 1) < 16)
            {
                ace_log2 = 3;
            }
            else if ((scan_pos + base_idx + 1) < 32)
            {
                ace_log2 = 4;
            }
            else if ((scan_pos + base_idx + 1) < 64)
            {
                ace_log2 = 5;
            }
            else if ((scan_pos + base_idx + 1) < 128)
            {
                ace_log2 = 6;
            }
            else if ((scan_pos + base_idx + 1) < 256)
            {
                ace_log2 = 7;
            }
            else if ((scan_pos + base_idx + 1) < 512)
            {
                ace_log2 = 8;
            }
            else if ((scan_pos + base_idx + 1) < 1024)
            {
                ace_log2 = 9;
            }
            else if ((scan_pos + base_idx + 1) < 2048)
            {
                ace_log2 = 10;
            }
            else if ((scan_pos + base_idx + 1) < 4096)
            {
                ace_log2 = 11;
            }
            else
            {
                ace_log2 = 11;
            }
            U24 prev_level = mat_prev_level[scan_pos];
			s64 d64_cost_last_one = GET_I_COST(rdoq_est_last_local[ctx_last][COM_MIN((U24)(prev_level - 1), (U24)5)][ace_log2][1], lambda);
            
            if (COM_ABS(dst_coef[scan_pos]))
            {
                s64 d64_cur_is_last_cost = d64_base_cost[scan_pos] + d64_cost_last_one;

                if (d64_cur_is_last_cost < best_cost)
                {
                    *pzb = 1;
                    best_cost = d64_cur_is_last_cost;
                    if (cnt <= MAT_SIZE)
                    {
                        if (cnt & 1)
                        {
                            *y_pos = cnt - scan_pos - 1;
                        }
                        else
                        {
                            *y_pos = scan_pos;
                        }
                    }
                    else
                    {
                        if (cnt & 1)
                        {
                            *y_pos = MAT_SIZE - scan_pos - 1;
                        }
                        else
                        {
                            *y_pos = scan_pos;
                        }
                    }
                    *x_pos = cnt - 1;
                }
            }
        }
    }
    RDOQ_FUNC_NAME(model_update, W, H)(model_cnt->last1, model_cnt->last2, model_cnt->run_rdoq, model_cnt->level, mat_run, mat_prev_level, ch_type, cnt);

    if (cnt <= MAT_SIZE)
    {
        last_cost[cyc + 1] = d64_base_cost[cnt - 1];
        prev_level[cyc + 1] = mat_prev_level[cnt - 1];
        last_level[cyc + 1] = COM_ABS(tmp_coef[cnt - 1]);
    }
    else
    {
        last_cost[cyc + 1] = d64_base_cost[MAT_SIZE - 1];
        prev_level[cyc + 1] = mat_prev_level[MAT_SIZE - 1];
        last_level[cyc + 1] = COM_ABS(tmp_coef[MAT_SIZE - 1]);
    }
    best_cost_out[cyc + 1] = best_cost;
}
int RDOQ_FUNC_NAME(rdoq_top, W, H)(RDOQ_MODEL* model_cnt, int qp, s64 d_lambda, int is_intra, s16 src_coef[H][W], s16 dst_tmp[H][W], int cu_width_log2, int cu_height_log2, int ch_type, int bit_depth, int rdoq_est_cbf[NUM_QT_CBF_CTX][2], s32 rdoq_est_run_local[24][2], s32 rdoq_est_level_local[24][2], s32 rdoq_est_last_local[2][6][12][2])
{
#pragma HLS ARRAY_PARTITION variable = src_coef complete dim = 1
//#pragma HLS STREAM variable=src_coef dim=2
#pragma HLS ARRAY_PARTITION variable = dst_tmp complete dim = 0
#pragma HLS ARRAY_PARTITION variable = rdoq_est_run_local complete dim = 0
#pragma HLS ARRAY_PARTITION variable = rdoq_est_level_local complete dim = 0
#pragma HLS ARRAY_PARTITION variable = rdoq_est_last_local complete dim = 0
    U14 quant_scale[80] = { (16302), (15024), (13777), (12634), (11626), (10624), (9742), (8958), (8192), (7512), (6889), (6305), (5793), (5303), (4878), (4467), (4091), (3756), (3444), (3161), (2894), (2654), (2435), (2235), (2048), (1878), (1722), (1579), (1449), (1329), (1218), (1117), (1024), (939), (861), (790), (724), (664), (609), (558), (512), (470), (430), (395), (362), (332), (304), (279), (256), (235), (215), (197), (181), (166), (152), (140), (128), (116), (108), (99), (91), (83), (76), (69), (64), (59), (54), (49), (45), (41), (38), (35), (32), (30), (27), (25), (23), (21), (19), (18) };

    const U14 scale = quant_scale[qp];
    const int ns_shift = ((cu_width_log2 + cu_height_log2) & 1) ? 7 : 0;
    const int ns_scale = ((cu_width_log2 + cu_height_log2) & 1) ? 181 : 1;
    const int ns_offset = ((cu_width_log2 + cu_height_log2) & 1) ? (1 << (ns_shift - 1)) : 0;
    const int q_value = (scale * ns_scale + ns_offset) >> ns_shift;
    const int log2_size = (cu_width_log2 + cu_height_log2) >> 1;
    const int tr_shift = get_transform_shift(bit_depth, log2_size);
    const int ctx_last = (ch_type == Y_C) ? 0 : 1;
    const int q_bits = QUANT_SHIFT + tr_shift;
    int num_nz_coef = 0;
    const s64 lambda = (d_lambda * (1 << 15) + 500) / 1024;
    //const s64 lambda = (s64)(d_lambda * (double)(1 << SCALE_BITS) + 0.5);
    U30 err_scale_tbl[80][6] = { 32932, 65865, 131731, 263462, 526925, 1053850, 35734, 71468, 142936, 285873, 571747, 1143495, 38968, 77937, 155874, 311749, 623498, 1246996, 42494, 84988, 169976, 339953, 679906, 1359812, 46178, 92356, 184713, 369427, 738855, 1477711, 50533, 101067, 202135, 404270, 808540, 1617081, 55108, 110217, 220435, 440871, 881742, 1763484, 59932, 119864, 239728, 479456, 958912, 1917824, 65536, 131072, 262144, 524288, 1048576, 2097152, 71468, 142936, 285873, 571747, 1143495, 2286990, 77931, 155863, 311726, 623452, 1246905, 2493811, 85150, 170300, 340600, 681200, 1362400, 2724800, 92675, 185351, 370703, 741406, 1482812, 2965625, 101239, 202478, 404956, 809912, 1619825, 3239650, 110059, 220119, 440238, 880477, 1760954, 3521908, 120186, 240372, 480744, 961488, 1922976, 3845952, 131232, 262464, 524928, 1049857, 2099715, 4199430, 142936, 285873, 571747, 1143495, 2286990, 4573980, 155885, 311771, 623543, 1247086, 2494173, 4988347, 169842, 339684, 679368, 1358736, 2717473, 5434947, 185511, 371023, 742046, 1484093, 2968187, 5936374, 202287, 404574, 809149, 1618299, 3236599, 6473198, 220480, 440961, 881923, 1763846, 3527693, 7055387, 240210, 480421, 960842, 1921685, 3843371, 7686742, 262144, 524288, 1048576, 2097152, 4194304, 8388608, 285873, 571747, 1143495, 2286990, 4573980, 9147960, 311771, 623543, 1247086, 2494173, 4988347, 9976695, 340006, 680013, 1360027, 2720055, 5440110, 10880221, 370511, 741022, 1482045, 2964090, 5928181, 11856362, 403966, 807932, 1615864, 3231728, 6463457, 12926914, 440780, 881561, 1763122, 3526245, 7052491, 14104982, 480636, 961272, 1922545, 3845091, 7690183, 15380366, 524288, 1048576, 2097152, 4194304, 8388608, 16777216, 571747, 1143495, 2286990, 4573980, 9147960, 18295920, 623543, 1247086, 2494173, 4988347, 9976695, 19953390, 679583, 1359166, 2718333, 5436667, 10873334, 21746669, 741534, 1483068, 2966137, 5932275, 11864550, 23729101, 808540, 1617081, 3234162, 6468324, 12936648, 25873296, 881561, 1763122, 3526245, 7052491, 14104982, 28209965, 962134, 1924268, 3848537, 7697074, 15394148, 30788296, 1048576, 2097152, 4194304, 8388608, 16777216, 33554432, 1142278, 2284557, 4569114, 9138228, 18276456, 36552913, 1248537, 2497074, 4994148, 9988296, 19976592, 39953184, 1359166, 2718333, 5436667, 10873334, 21746669, 43493339, 1483068, 2966137, 5932275, 11864550, 23729101, 47458202, 1617081, 3234162, 6468324, 12936648, 25873296, 51746593, 1766022, 3532045, 7064090, 14128181, 28256363, 56512727, 1924268, 3848537, 7697074, 15394148, 30788296, 61576592, 2097152, 4194304, 8388608, 16777216, 33554432, 67108864, 2284557, 4569114, 9138228, 18276456, 36552913, 73105826, 2497074, 4994148, 9988296, 19976592, 39953184, 79906368, 2725233, 5450466, 10900932, 21801864, 43603728, 87207457, 2966137, 5932275, 11864550, 23729101, 47458202, 94916404, 3234162, 6468324, 12936648, 25873296, 51746593, 103493187, 3532045, 7064090, 14128181, 28256363, 56512727, 113025455, 3834792, 7669584, 15339168, 30678337, 61356675, 122713351, 4194304, 8388608, 16777216, 33554432, 67108864, 134217728, 4628197, 9256395, 18512790, 37025580, 74051160, 148102320, 4971026, 9942053, 19884107, 39768215, 79536431, 159072862, 5422938, 10845877, 21691754, 43383508, 86767016, 173534032, 5899680, 11799360, 23598721, 47197442, 94394885, 188789771, 6468324, 12936648, 25873296, 51746593, 103493187, 206986375, 7064090, 14128181, 28256363, 56512727, 113025455, 226050910, 7780737, 15561475, 31122951, 62245902, 124491805, 248983611, 8388608, 16777216, 33554432, 67108864, 134217728, 268435456, 9099506, 18199013, 36398027, 72796055, 145592111, 291184223, 9942053, 19884107, 39768215, 79536431, 159072862, 318145725, 10956549, 21913098, 43826196, 87652393, 175304787, 350609575, 11930464, 23860929, 47721858, 95443717, 190887435, 381774870, 13094412, 26188824, 52377649, 104755299, 209510599, 419021199, 14128181, 28256363, 56512727, 113025455, 226050910, 452101820, 15339168, 30678337, 61356675, 122713351, 245426702, 490853405, 16777216, 33554432, 67108864, 134217728, 268435456, 536870912, 17895697, 35791394, 71582788, 143165576, 286331153, 572662306, 19884107, 39768215, 79536431, 159072862, 318145725, 636291451, 21474836, 42949672, 85899345, 171798691, 343597383, 687194767, 23342213, 46684427, 93368854, 186737708, 373475417, 746950834, 25565281, 51130563, 102261126, 204522252, 409044504, 818089008, 28256363, 56512727, 113025455, 226050910, 452101820, 904203641, 29826161, 59652323, 119304647, 238609294, 477218588, 954437176 };

    s64 err_scale = err_scale_tbl[qp][log2_size - 1];
    s64 last_cost[W + 1];
    s64 best_cost[W + 1];
    int x_pos = 0;
    int y_pos = 0;
    s16 dst64_tmp[MAT_SIZE];
    s64 d64_base_cost0 = 0;
    s16 tmp_coef[MAT_SIZE];
    s16 dst_coef[MAT_SIZE];
    U24 last_level[W + 1];
    U24 prev_level[W + 1];
    last_level[0] = 6;
    prev_level[0] = 6;
    int pzb = 0;
    last_cost[0] = 0;
#pragma HLS ARRAY_PARTITION variable = last_cost complete dim = 0
#pragma HLS ARRAY_PARTITION variable = best_cost complete dim = 0
#pragma HLS ARRAY_PARTITION variable = dst64_tmp complete dim = 0
#pragma HLS ARRAY_PARTITION variable = tmp_coef complete dim = 0
#pragma HLS ARRAY_PARTITION variable = dst_coef complete dim = 0
#pragma HLS ARRAY_PARTITION variable = last_level complete dim = 0
#pragma HLS ARRAY_PARTITION variable = prev_level complete dim = 0

    if (!is_intra && ch_type == Y_C)
    {
        d64_base_cost0 = 0;
        best_cost[0] = 0;
    }
    else
    {
        d64_base_cost0 = GET_I_COST(rdoq_est_cbf[ch_type][1], lambda);
        best_cost[0] = GET_I_COST(rdoq_est_cbf[ch_type][0], lambda);
    }

    for (int cyc = 0; cyc < W; cyc++)
    {
#pragma HLS DATAFLOW
        RDOQ_FUNC_NAME(read_data, W, H)(tmp_coef, src_coef, cyc);
        RDOQ_FUNC_NAME(rdoq_core, W, H)(model_cnt, last_level, prev_level, &pzb, d64_base_cost0, &x_pos, &y_pos, best_cost, last_cost, q_value, cyc + 1, q_bits, err_scale, lambda, is_intra, tmp_coef, dst64_tmp, ch_type, rdoq_est_run_local, rdoq_est_level_local, rdoq_est_last_local);
        RDOQ_FUNC_NAME(out_coef, W, H)(cyc + 1, dst64_tmp, dst_coef);
        RDOQ_FUNC_NAME(write_data, W, H)(dst_tmp, dst_coef, cyc);
    }
    num_nz_coef = RDOQ_FUNC_NAME(clean_nz, W, H)(dst_tmp, x_pos, y_pos, pzb);
    return num_nz_coef;
}

void RDOQ_FUNC_NAME(read_data1, W, H)(s16 coef64[64], s16 src_coef[64][64], int i)
{
	int j;
#if (W > H)
#if (H == 8)
	if (i == 0)
	{
		coef64[0] = src_coef[0][i];
	}
	else if (i == 1)
	{
		for (j = 0; j <= 1; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 2)
	{
		for (j = 0; j <= 2; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 3)
	{
		for (j = 0; j <= 3; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 4)
	{
		for (j = 0; j <= 4; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 5)
	{
		for (j = 0; j <= 5; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 6)
	{
		for (j = 0; j <= 6; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 7)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 8)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 9)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 10)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 11)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 12)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 13)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 14)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 15)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 16)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 17)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 18)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 19)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 20)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 21)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 22)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 23)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 24)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 25)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 26)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 27)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 28)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 29)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 30)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 31)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 32)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 33)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 34)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 35)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 36)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 37)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 38)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 39)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 40)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 41)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 42)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 43)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 44)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 45)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 46)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 47)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 48)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 49)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 50)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 51)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 52)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 53)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 54)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 55)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 56)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 57)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 58)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 59)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 60)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 61)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 62)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 63)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
#elif (H == 16)
	if (i == 0)
	{
		coef64[0] = src_coef[0][i];
	}
	else if (i == 1)
	{
		for (j = 0; j <= 1; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 2)
	{
		for (j = 0; j <= 2; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 3)
	{
		for (j = 0; j <= 3; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 4)
	{
		for (j = 0; j <= 4; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 5)
	{
		for (j = 0; j <= 5; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 6)
	{
		for (j = 0; j <= 6; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 7)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 8)
	{
		for (j = 0; j <= 8; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 9)
	{
		for (j = 0; j <= 9; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 10)
	{
		for (j = 0; j <= 10; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 11)
	{
		for (j = 0; j <= 11; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 12)
	{
		for (j = 0; j <= 12; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 13)
	{
		for (j = 0; j <= 13; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 14)
	{
		for (j = 0; j <= 14; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 15)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 16)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 17)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 18)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 19)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 20)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 21)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 22)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 23)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 24)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 25)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 26)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 27)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 28)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 29)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 30)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 31)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
#if (W > 32)
	else if (i == 32)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 33)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 34)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 35)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 36)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 37)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 38)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 39)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 40)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 41)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 42)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 43)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 44)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 45)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 46)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 47)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 48)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 49)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 50)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 51)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 52)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 53)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 54)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 55)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 56)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 57)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 58)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 59)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 60)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 61)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 62)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 63)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}

#endif
#else
	if (i == 0)
	{
		coef64[0] = src_coef[0][i];
	}
	else if (i == 1)
	{
		for (j = 0; j <= 1; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 2)
	{
		for (j = 0; j <= 2; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 3)
	{
		for (j = 0; j <= 3; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 4)
	{
		for (j = 0; j <= 4; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 5)
	{
		for (j = 0; j <= 5; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 6)
	{
		for (j = 0; j <= 6; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 7)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 8)
	{
		for (j = 0; j <= 8; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 9)
	{
		for (j = 0; j <= 9; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 10)
	{
		for (j = 0; j <= 10; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 11)
	{
		for (j = 0; j <= 11; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 12)
	{
		for (j = 0; j <= 12; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 13)
	{
		for (j = 0; j <= 13; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 14)
	{
		for (j = 0; j <= 14; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 15)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 16)
	{
		for (j = 0; j <= 16; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 17)
	{
		for (j = 0; j <= 17; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 18)
	{
		for (j = 0; j <= 18; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 19)
	{
		for (j = 0; j <= 19; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 20)
	{
		for (j = 0; j <= 20; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 21)
	{
		for (j = 0; j <= 21; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 22)
	{
		for (j = 0; j <= 22; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 23)
	{
		for (j = 0; j <= 23; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 24)
	{
		for (j = 0; j <= 24; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 25)
	{
		for (j = 0; j <= 25; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 26)
	{
		for (j = 0; j <= 26; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 27)
	{
		for (j = 0; j <= 27; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 28)
	{
		for (j = 0; j <= 28; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 29)
	{
		for (j = 0; j <= 29; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 30)
	{
		for (j = 0; j <= 30; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 31)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 32)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 33)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 34)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 35)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 36)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 37)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 38)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 39)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 40)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 41)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 42)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 43)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 44)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 45)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 46)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 47)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 48)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 49)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 50)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 51)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 52)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 53)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 54)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 55)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 56)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 57)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 58)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 59)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 60)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 61)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 62)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 63)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
#endif
#else
	if (i == 0)
	{
		coef64[0] = src_coef[0][i];
	}
	else if (i == 1)
	{
		for (j = 0; j <= 1; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 2)
	{
		for (j = 0; j <= 2; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 3)
	{
		for (j = 0; j <= 3; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 4)
	{
		for (j = 0; j <= 4; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 5)
	{
		for (j = 0; j <= 5; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 6)
	{
		for (j = 0; j <= 6; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 7)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
#if (W > 8)
	else if (i == 8)
	{
		for (j = 0; j <= 8; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 9)
	{
		for (j = 0; j <= 9; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 10)
	{
		for (j = 0; j <= 10; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 11)
	{
		for (j = 0; j <= 11; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 12)
	{
		for (j = 0; j <= 12; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 13)
	{
		for (j = 0; j <= 13; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 14)
	{
		for (j = 0; j <= 14; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 15)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
#if (W > 16)
	else if (i == 16)
	{
		for (j = 0; j <= 16; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 17)
	{
		for (j = 0; j <= 17; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 18)
	{
		for (j = 0; j <= 18; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 19)
	{
		for (j = 0; j <= 19; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 20)
	{
		for (j = 0; j <= 20; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 21)
	{
		for (j = 0; j <= 21; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 22)
	{
		for (j = 0; j <= 22; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 23)
	{
		for (j = 0; j <= 23; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 24)
	{
		for (j = 0; j <= 24; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 25)
	{
		for (j = 0; j <= 25; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 26)
	{
		for (j = 0; j <= 26; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 27)
	{
		for (j = 0; j <= 27; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 28)
	{
		for (j = 0; j <= 28; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 29)
	{
		for (j = 0; j <= 29; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 30)
	{
		for (j = 0; j <= 30; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 31)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
#if (W > 32)
	else if (i == 32)
	{
		for (j = 0; j <= 32; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 33)
	{
		for (j = 0; j <= 33; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 34)
	{
		for (j = 0; j <= 34; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 35)
	{
		for (j = 0; j <= 35; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 36)
	{
		for (j = 0; j <= 36; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 37)
	{
		for (j = 0; j <= 37; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 38)
	{
		for (j = 0; j <= 38; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 39)
	{
		for (j = 0; j <= 39; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 40)
	{
		for (j = 0; j <= 40; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 41)
	{
		for (j = 0; j <= 41; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 42)
	{
		for (j = 0; j <= 42; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 43)
	{
		for (j = 0; j <= 43; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 44)
	{
		for (j = 0; j <= 44; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 45)
	{
		for (j = 0; j <= 45; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 46)
	{
		for (j = 0; j <= 46; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 47)
	{
		for (j = 0; j <= 47; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 48)
	{
		for (j = 0; j <= 48; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 49)
	{
		for (j = 0; j <= 49; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 50)
	{
		for (j = 0; j <= 50; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 51)
	{
		for (j = 0; j <= 51; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 52)
	{
		for (j = 0; j <= 52; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 53)
	{
		for (j = 0; j <= 53; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 54)
	{
		for (j = 0; j <= 54; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 55)
	{
		for (j = 0; j <= 55; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 56)
	{
		for (j = 0; j <= 56; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 57)
	{
		for (j = 0; j <= 57; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 58)
	{
		for (j = 0; j <= 58; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 59)
	{
		for (j = 0; j <= 59; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 60)
	{
		for (j = 0; j <= 60; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 61)
	{
		for (j = 0; j <= 61; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 62)
	{
		for (j = 0; j <= 62; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
	else if (i == 63)
	{
		for (j = 0; j <= 63; j++)
		{
#pragma HLS UNROLL
			coef64[j] = src_coef[j][i - j];
		}
	}
#endif
#endif
#endif
#endif
}
void RDOQ_FUNC_NAME(write_data1, W, H)(s16 dst_tmp[64][64], s16 dst64[64], int i)
{
	int j;
#if (W > H)
#if (H == 8)
	if (i == 0)
	{
		dst_tmp[0][i] = dst64[0];
	}
	else if (i == 1)
	{
		for (j = 0; j <= 1; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 2)
	{
		for (j = 0; j <= 2; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 3)
	{
		for (j = 0; j <= 3; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 4)
	{
		for (j = 0; j <= 4; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 5)
	{
		for (j = 0; j <= 5; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 6)
	{
		for (j = 0; j <= 6; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 7)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 8)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 9)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 10)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 11)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 12)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 13)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 14)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 15)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 16)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 17)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 18)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 19)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 20)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 21)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 22)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 23)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 24)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 25)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 26)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 27)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 28)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 29)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 30)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 31)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 32)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 33)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 34)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 35)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 36)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 37)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 38)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 39)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 40)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 41)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 42)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 43)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 44)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 45)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 46)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 47)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 48)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 49)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 50)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 51)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 52)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 53)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 54)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 55)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 56)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 57)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 58)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 59)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 60)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 61)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 62)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 63)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
#elif (H == 16)
	if (i == 0)
	{
		dst_tmp[0][i] = dst64[0];
	}
	else if (i == 1)
	{
		for (j = 0; j <= 1; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 2)
	{
		for (j = 0; j <= 2; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 3)
	{
		for (j = 0; j <= 3; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 4)
	{
		for (j = 0; j <= 4; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 5)
	{
		for (j = 0; j <= 5; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 6)
	{
		for (j = 0; j <= 6; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 7)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 8)
	{
		for (j = 0; j <= 8; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 9)
	{
		for (j = 0; j <= 9; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 10)
	{
		for (j = 0; j <= 10; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 11)
	{
		for (j = 0; j <= 11; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 12)
	{
		for (j = 0; j <= 12; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 13)
	{
		for (j = 0; j <= 13; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 14)
	{
		for (j = 0; j <= 14; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 15)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 16)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 17)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 18)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 19)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 20)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 21)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 22)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 23)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 24)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 25)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 26)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 27)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 28)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 29)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 30)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 31)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}

#if (W > 32)
	else if (i == 32)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 33)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 34)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 35)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 36)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 37)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 38)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 39)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 40)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 41)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 42)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 43)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 44)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 45)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 46)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 47)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 48)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 49)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 50)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 51)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 52)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 53)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 54)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 55)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 56)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 57)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 58)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 59)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 60)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 61)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 62)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 63)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
#endif
#else
	if (i == 0)
	{
		dst_tmp[0][i] = dst64[0];
	}
	else if (i == 1)
	{
		for (j = 0; j <= 1; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 2)
	{
		for (j = 0; j <= 2; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 3)
	{
		for (j = 0; j <= 3; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 4)
	{
		for (j = 0; j <= 4; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 5)
	{
		for (j = 0; j <= 5; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 6)
	{
		for (j = 0; j <= 6; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 7)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 8)
	{
		for (j = 0; j <= 8; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 9)
	{
		for (j = 0; j <= 9; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 10)
	{
		for (j = 0; j <= 10; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 11)
	{
		for (j = 0; j <= 11; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 12)
	{
		for (j = 0; j <= 12; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 13)
	{
		for (j = 0; j <= 13; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 14)
	{
		for (j = 0; j <= 14; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 15)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 16)
	{
		for (j = 0; j <= 16; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 17)
	{
		for (j = 0; j <= 17; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 18)
	{
		for (j = 0; j <= 18; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 19)
	{
		for (j = 0; j <= 19; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 20)
	{
		for (j = 0; j <= 20; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 21)
	{
		for (j = 0; j <= 21; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 22)
	{
		for (j = 0; j <= 22; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 23)
	{
		for (j = 0; j <= 23; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 24)
	{
		for (j = 0; j <= 24; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 25)
	{
		for (j = 0; j <= 25; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 26)
	{
		for (j = 0; j <= 26; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 27)
	{
		for (j = 0; j <= 27; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 28)
	{
		for (j = 0; j <= 28; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 29)
	{
		for (j = 0; j <= 29; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 30)
	{
		for (j = 0; j <= 30; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 31)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 32)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 33)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 34)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 35)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 36)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 37)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 38)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 39)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 40)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 41)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 42)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 43)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 44)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 45)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 46)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 47)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 48)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 49)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 50)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 51)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 52)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 53)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 54)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 55)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 56)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 57)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 58)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 59)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 60)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 61)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 62)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 63)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
#endif
#else
	if (i == 0)
	{
		dst_tmp[0][i] = dst64[0];
	}
	else if (i == 1)
	{
		for (j = 0; j <= 1; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 2)
	{
		for (j = 0; j <= 2; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 3)
	{
		for (j = 0; j <= 3; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 4)
	{
		for (j = 0; j <= 4; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 5)
	{
		for (j = 0; j <= 5; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 6)
	{
		for (j = 0; j <= 6; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 7)
	{
		for (j = 0; j <= 7; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
#if (W > 8)
	else if (i == 8)
	{
		for (j = 0; j <= 8; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 9)
	{
		for (j = 0; j <= 9; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 10)
	{
		for (j = 0; j <= 10; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 11)
	{
		for (j = 0; j <= 11; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 12)
	{
		for (j = 0; j <= 12; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 13)
	{
		for (j = 0; j <= 13; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 14)
	{
		for (j = 0; j <= 14; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 15)
	{
		for (j = 0; j <= 15; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
#if (W > 16)
	else if (i == 16)
	{
		for (j = 0; j <= 16; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 17)
	{
		for (j = 0; j <= 17; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 18)
	{
		for (j = 0; j <= 18; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 19)
	{
		for (j = 0; j <= 19; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 20)
	{
		for (j = 0; j <= 20; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 21)
	{
		for (j = 0; j <= 21; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 22)
	{
		for (j = 0; j <= 22; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 23)
	{
		for (j = 0; j <= 23; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 24)
	{
		for (j = 0; j <= 24; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 25)
	{
		for (j = 0; j <= 25; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 26)
	{
		for (j = 0; j <= 26; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 27)
	{
		for (j = 0; j <= 27; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 28)
	{
		for (j = 0; j <= 28; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 29)
	{
		for (j = 0; j <= 29; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 30)
	{
		for (j = 0; j <= 30; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 31)
	{
		for (j = 0; j <= 31; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
#if (W > 32)
	else if (i == 32)
	{
		for (j = 0; j <= 32; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 33)
	{
		for (j = 0; j <= 33; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 34)
	{
		for (j = 0; j <= 34; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 35)
	{
		for (j = 0; j <= 35; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 36)
	{
		for (j = 0; j <= 36; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 37)
	{
		for (j = 0; j <= 37; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 38)
	{
		for (j = 0; j <= 38; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 39)
	{
		for (j = 0; j <= 39; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 40)
	{
		for (j = 0; j <= 40; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 41)
	{
		for (j = 0; j <= 41; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 42)
	{
		for (j = 0; j <= 42; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 43)
	{
		for (j = 0; j <= 43; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 44)
	{
		for (j = 0; j <= 44; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 45)
	{
		for (j = 0; j <= 45; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 46)
	{
		for (j = 0; j <= 46; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 47)
	{
		for (j = 0; j <= 47; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 48)
	{
		for (j = 0; j <= 48; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 49)
	{
		for (j = 0; j <= 49; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 50)
	{
		for (j = 0; j <= 50; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 51)
	{
		for (j = 0; j <= 51; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 52)
	{
		for (j = 0; j <= 52; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 53)
	{
		for (j = 0; j <= 53; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 54)
	{
		for (j = 0; j <= 54; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 55)
	{
		for (j = 0; j <= 55; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 56)
	{
		for (j = 0; j <= 56; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 57)
	{
		for (j = 0; j <= 57; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 58)
	{
		for (j = 0; j <= 58; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 59)
	{
		for (j = 0; j <= 59; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 60)
	{
		for (j = 0; j <= 60; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 61)
	{
		for (j = 0; j <= 61; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 62)
	{
		for (j = 0; j <= 62; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
	else if (i == 63)
	{
		for (j = 0; j <= 63; j++)
		{
#pragma HLS UNROLL
			dst_tmp[j][i - j] = dst64[j];
		}
	}
#endif
#endif
#endif
#endif
}

#endif

#include "../inc/com_def.h"
//int get_transform_shift(int bit_depth, int tr_size_log2)
//{

//#pragma HLS INLINE off
//	return 15 - bit_depth - tr_size_log2;
//}

static int quant_4(U7 w, U7 h, U3 cu_width_log2, U3 cu_height_log2, U4 bit_depth, s16 coef[4][4], U2 slice_type, U14 scale)
{
    int offset;
    int i, j;
    int shift;
    int sign;
    s16 lev;
    int tr_shift;
    int log2_size = (cu_width_log2 + cu_height_log2) >> 1;
    int ns_shift = ((cu_width_log2 + cu_height_log2) & 1) ? 7 : 0;
    int ns_scale = ((cu_width_log2 + cu_height_log2) & 1) ? 181 : 1;
    int num_nz_coef = 0;
    int scale_x_ns_scale = scale * ns_scale;

    tr_shift = get_transform_shift(bit_depth, log2_size - ns_shift);
    shift = QUANT_SHIFT + tr_shift;
    offset = (s64)((slice_type == SLICE_I) ? 171 : 85) << (s64)(shift - 9);

    for (i = 0; i < h; i++)
    {
#pragma HLS LOOP_TRIPCOUNT max=64
        for (j = 0; j < w; j++)
        {
#pragma HLS LOOP_TRIPCOUNT max=64
            sign = COM_SIGN_GET(coef[i][j]);
            lev = (((u32)(COM_ABS(coef[i][j]) * scale_x_ns_scale + offset)) >> shift);
            coef[i][j] = COM_SIGN_SET(lev, sign);

            //bug fix for rate estimation
            //if ((i + j) >= w)
            //{
            //	coef[i][j] = 0;
            //}

            num_nz_coef += !!(coef[i][j]);
        }
    }
    return num_nz_coef;
}

#if (W <= 16) && (H <= 16)
int RDOQ_FUNC_NAME(quant_nnz, W, H)(RDOQ_MODEL* model_cnt, RDOQ_ARRAY* rdoq_array, int bit_depth, int qp, s64 lambda, int is_intra, s16 coef[SIZE], int cu_width_log2, int cu_height_log2, int ch_type, int slice_type, s16 coef_y_pb_part[SIZE], s16 coef_u[SIZE], s16 coef_v[SIZE])
{
    int i;
#pragma HLS INLINE off
    int num_nz_coef = 0;
    U14 quant_scale[80] = { (16302), (15024), (13777), (12634), (11626), (10624), (9742), (8958), (8192), (7512), (6889), (6305), (5793), (5303), (4878), (4467), (4091), (3756), (3444), (3161), (2894), (2654), (2435), (2235), (2048), (1878), (1722), (1579), (1449), (1329), (1218), (1117), (1024), (939), (861), (790), (724), (664), (609), (558), (512), (470), (430), (395), (362), (332), (304), (279), (256), (235), (215), (197), (181), (166), (152), (140), (128), (116), (108), (99), (91), (83), (76), (69), (64), (59), (54), (49), (45), (41), (38), (35), (32), (30), (27), (25), (23), (21), (19), (18) };

    U14 scale = quant_scale[qp];
    int width = W;
    int height = H;

    for (i = 0; i < SIZE; i++) {
        coef_y_pb_part[i] = 0;
    }

    if (W == 4 && H == 4) {
#if RDOQ_OPEN
    num_nz_coef = RDOQ_FUNC_NAME(rdoq_top, W, H)(model_cnt, qp, lambda, is_intra, coef, coef, cu_width_log2, cu_height_log2, ch_type, bit_depth, rdoq_array);
#else 
        s16 coef_tmp[4][4] = { 0 };
        for (U3 i = 0; i < 4; i++) {
            for (U3 j = 0; j < 4; j++) {
                coef_tmp[i][j] = coef[i * 4 + j];
            }
        }
        num_nz_coef = quant_4(4, 4, cu_width_log2, cu_height_log2, bit_depth, coef_tmp, slice_type, scale);
        for (U3 i = 0; i < 4; i++) {
            for (U3 j = 0; j < 4; j++) {
                coef[i * 4 + j] = coef_tmp[i][j];
            }
        }
#endif 
    }
    else {
        num_nz_coef = RDOQ_FUNC_NAME(rdoq_top, W, H)(model_cnt, qp, lambda, is_intra, coef, coef, cu_width_log2, cu_height_log2, ch_type, bit_depth, rdoq_array);
    }
    
    memcpy_rewrintra(coef_y_pb_part, coef, width * height);
    if (ch_type == 1)memcpy_rewrintra(coef_u, coef, width * height);
    else if (ch_type == 2)memcpy_rewrintra(coef_v, coef, width * height);

    return num_nz_coef;
}
#elif (W == 64) && (H == 64)
int RDOQ_FUNC_NAME(quant, W, H)(int cu_width_log2, int cu_height_log2, int bit_depth, s16 coef[SIZE], int slice_type, U14 scale)
{
    int offset;
    int i, j;
    int w = W;
    int h = H;
    int shift;
    int sign;
    s16 lev;
    int tr_shift;
    int log2_size = (cu_width_log2 + cu_height_log2) >> 1;
    int ns_shift = ((cu_width_log2 + cu_height_log2) & 1) ? 7 : 0;
    int ns_scale = ((cu_width_log2 + cu_height_log2) & 1) ? 181 : 1;
    int num_nz_coef = 0;

    tr_shift = get_transform_shift(bit_depth, log2_size - ns_shift);
    shift = QUANT_SHIFT + tr_shift;
    offset = (s64)((slice_type == SLICE_I) ? 171 : 85) << (s64)(shift - 9);

    for (i = 0; i < h; i++)
    {
        for (j = 0; j < w; j++)
        {
            sign = COM_SIGN_GET(coef[j]);
            lev = ((COM_ABS(coef[j]) * scale * ns_scale + offset) >> shift);
            coef[j] = COM_SIGN_SET(lev, sign);
            num_nz_coef += !!(coef[j]);
        }
        coef += w;
    }
    return num_nz_coef;
}

int RDOQ_FUNC_NAME(quant_nnz, W, H)(RDOQ_MODEL* model_cnt, int rdoq_est_cbf[NUM_QT_CBF_CTX][2], s32 rdoq_est_level[NUM_SBAC_CTX_LEVEL][2], s32 rdoq_est_last[2][NUM_SBAC_CTX_LAST1][NUM_SBAC_CTX_LAST2][2], s32 rdoq_est_run[24][2], int bit_depth, int qp, s64 lambda, int is_intra, s16 coef[SIZE], int cu_width_log2, int cu_height_log2, int ch_type, int slice_type, s16 coef_y_pb_part[SIZE], s16 coef_u[SIZE], s16 coef_v[SIZE])
{
    int i;
    int j;
#pragma HLS INLINE off
    int num_nz_coef = 0;
    U14 quant_scale[80] = { (16302), (15024), (13777), (12634), (11626), (10624), (9742), (8958), (8192), (7512), (6889), (6305), (5793), (5303), (4878), (4467), (4091), (3756), (3444), (3161), (2894), (2654), (2435), (2235), (2048), (1878), (1722), (1579), (1449), (1329), (1218), (1117), (1024), (939), (861), (790), (724), (664), (609), (558), (512), (470), (430), (395), (362), (332), (304), (279), (256), (235), (215), (197), (181), (166), (152), (140), (128), (116), (108), (99), (91), (83), (76), (69), (64), (59), (54), (49), (45), (41), (38), (35), (32), (30), (27), (25), (23), (21), (19), (18) };

    U14 scale = quant_scale[qp];
    int width = W;
    int height = H;
    s16* p;

    for (i = 0; i < SIZE; i++) {
        coef_y_pb_part[i] = 0;
    }
    if (width > 32) {

#pragma HLS loop_tripcount max=32

        p = coef;
        for (i = 0; i < height; i++) {

#pragma HLS loop_tripcount max=64
            for (j = 32; j < width; j++) {

#pragma HLS loop_tripcount max=64
                p[j] = 0;
            }
            p += width;
        }
    }
    if (height > 32) {
        //add by huwq
        for (i = 0; i < width * height - 32 * width; i++) {

#pragma HLS loop_tripcount max=2048
            coef[i + 32 * width] = 0;
        }
    }

    num_nz_coef = RDOQ_FUNC_NAME(quant, W, H)(cu_width_log2, cu_height_log2, bit_depth, coef, slice_type, scale);

    memcpy_rewrintra(coef_y_pb_part, coef, width * height);
    if (ch_type == 1)memcpy_rewrintra(coef_u, coef, width * height);
    else if (ch_type == 2)memcpy_rewrintra(coef_v, coef, width * height);

    return num_nz_coef;
}
#else
int RDOQ_FUNC_NAME(quant_nnz, W, H)(RDOQ_MODEL* model_cnt, int rdoq_est_cbf[NUM_QT_CBF_CTX][2], s32 rdoq_est_level[NUM_SBAC_CTX_LEVEL][2], s32 rdoq_est_last[2][NUM_SBAC_CTX_LAST1][NUM_SBAC_CTX_LAST2][2], s32 rdoq_est_run[24][2], int bit_depth, int qp, s64 lambda, int is_intra, s16 coef[SIZE], int cu_width_log2, int cu_height_log2, int ch_type, int slice_type, s16 coef_y_pb_part[SIZE], s16 coef_u[SIZE], s16 coef_v[SIZE])
{
    int i;
    int j;
#pragma HLS INLINE off
    int num_nz_coef = 0;
    U14 quant_scale[80] = { (16302), (15024), (13777), (12634), (11626), (10624), (9742), (8958), (8192), (7512), (6889), (6305), (5793), (5303), (4878), (4467), (4091), (3756), (3444), (3161), (2894), (2654), (2435), (2235), (2048), (1878), (1722), (1579), (1449), (1329), (1218), (1117), (1024), (939), (861), (790), (724), (664), (609), (558), (512), (470), (430), (395), (362), (332), (304), (279), (256), (235), (215), (197), (181), (166), (152), (140), (128), (116), (108), (99), (91), (83), (76), (69), (64), (59), (54), (49), (45), (41), (38), (35), (32), (30), (27), (25), (23), (21), (19), (18) };

    U14 scale = quant_scale[qp];
    int width = W;
    int height = H;
    s16* p;

    for (i = 0; i < SIZE; i++) {
        coef_y_pb_part[i] = 0;
    }
    if (width > 32) {

#pragma HLS loop_tripcount max=32

        p = coef;
        for (i = 0; i < height; i++) {

#pragma HLS loop_tripcount max=64
            for (j = 32; j < width; j++) {

#pragma HLS loop_tripcount max=64
                p[j] = ((s16)0);
            }
            p += width;
        }
    }
    if (height > 32) {
        //add by huwq
        for (i = 0; i < width * height - 32 * width; i++) {

#pragma HLS loop_tripcount max=2048
            coef[i + 32 * width] = ((s16)0);
        }
    }

    s16 coef_tmp[H][W] = { 0 };
    s16 dst_tmp[H][W] = { 0 };
    for (int a = 0; a < height; a++)
    {
        for (int b = 0; b < width; b++)
        {
            coef_tmp[a][b] = coef[a * width + b];
        }
    }
    num_nz_coef = RDOQ_FUNC_NAME(rdoq_top, W, H)(model_cnt, qp, lambda, is_intra, coef_tmp, dst_tmp, cu_width_log2, cu_height_log2, ch_type, bit_depth,rdoq_est_cbf, rdoq_est_run, rdoq_est_level, rdoq_est_last);

    for (int a = 0; a < height; a++)
    {
        for (int b = 0; b < width; b++)
        {
            coef[a * width + b] = dst_tmp[a][b];
        }
    }

    
    memcpy_rewrintra(coef_y_pb_part, coef, width * height);
    if (ch_type == 1)memcpy_rewrintra(coef_u, coef, width * height);
    else if (ch_type == 2)memcpy_rewrintra(coef_v, coef, width * height);

    return num_nz_coef;
}
#endif


#undef W
#undef H
#undef MAT_SIZE
#undef MAT_SIZE_32_1
#undef SIZE
#undef UPDATE_SIZE
