#if (!defined(W)) || (!defined(H)) 
#error LOG_W or LOG_H not define
#endif
#define COEF_TWO 0
#if COEF_TWO
void  FUNC_NAME(ready_coef, W, H)(s16 coef_in[W*H], s16 coef_out[H][W])
{
	U5 i;
	U5 j;
	U13 n = 0;
	for (i = 0; i < (u8)H; i++)
	{
		for (j = 0; j < (u8)W; j++)
		{
			coef_out[i][j] = coef_in[n];
			n++;
		}
	}
}
#if(W ==4 )&& (H == 4)
int  FUNC_NAME(enc_eco_coef, W, H)(s16 coef[W*H], int log2_w, int log2_h)
{
	U15 enc_est = 0;
	u32 num_coeff;
	U4 scan_pos;
	u16 level;

	U8 run;

	s16 coef_cur;

	FUNC_NAME(ready_coef, W, H)(coef, coef_out);
	U4 scan[W*H] = {
		0, 1, 4, 8, 5, 2, 3, 6, 9, 12, 13, 10, 7, 11, 14, 15
	};
	run = 0;
	for (scan_pos = 0; scan_pos < 16; scan_pos++) {
		U4 a = scan[scan_pos] / W;
		U4 b = scan[scan_pos] % W;
		coef_cur = coef_out[a][b];
		if (coef_cur) {
			level = abs(coef_cur);
			//level = ((((int)coef_cur) ^ ((int)coef_cur) >> 15) - (((int)coef_cur) >> 15));
			enc_est += enc_eco_run_est_3(run);
			enc_est += enc_eco_level_est_3(level);
			enc_est++;
			if (scan_pos == W*H - 1) {
				break;
			}
			run = 0;
			enc_est++;
		}
		else{
			run++;
		}


	}
	return enc_est;
}
#elif((W ==4 )&& (H == 8))||((W ==8 )&& (H == 4))||((W ==4 )&& (H == 16))||((W ==16 )&& (H == 4))
int  FUNC_NAME(enc_eco_coef, W, H)(s16 coef[W*H], int log2_w, int log2_h)
{
	U15 enc_est = 0;

	U5 scan_pos;
	u16 level;

	U8 run;

	s16 coef_cur;

	FUNC_NAME(ready_coef, W, H)(coef, coef_out);
	U6 scan[W*H] = {
#if (W == 4) && (H == 8)
		0, 1, 4, 8, 5, 2, 3, 6, 9, 12, 16, 13, 10, 7, 11, 14, 17, 20, 24, 21, 18, 15, 19, 22, 25, 28, 29, 26, 23, 27, 30, 31
#elif (W == 8) && (H == 4)
		0, 1, 8, 16, 9, 2, 3, 10, 17, 24, 25, 18, 11, 4, 5, 12, 19, 26, 27, 20, 13, 6, 7, 14, 21, 28, 29, 22, 15, 23, 30, 31
#elif (W == 16) && (H == 4)
		0, 1, 16, 32, 17, 2, 3, 18, 33, 48, 49, 34, 19, 4, 5, 20, 35, 50, 51, 36, 21, 6, 7, 22, 37, 52, 53, 38, 23, 8, 9, 24, 39, 54, 55, 40, 25, 10, 11, 26, 41, 56, 57, 42, 27, 12, 13, 28, 43, 58, 59, 44, 29, 14, 15, 30, 45, 60, 61, 46, 31, 47, 62, 63
#elif (W == 4) && (H == 16)
		0, 1, 4, 8, 5, 2, 3, 6, 9, 12, 16, 13, 10, 7, 11, 14, 17, 20, 24, 21, 18, 15, 19, 22, 25, 28, 32, 29, 26, 23, 27, 30, 33, 36, 40, 37, 34, 31, 35, 38, 41, 44, 48, 45, 42, 39, 43, 46, 49, 52, 56, 53, 50, 47, 51, 54, 57, 60, 61, 58, 55, 59, 62, 63
#else
		0
#endif
	};
	run = 0;
	for (scan_pos = 0; scan_pos < 32; scan_pos++) {
		U6 a = scan[scan_pos] / W;
		U6 b = scan[scan_pos] % W;
		coef_cur = coef_out[a][b];
		if (coef_cur) {
			level = abs(coef_cur);
		//	level = ((((int)coef_cur) ^ ((int)coef_cur) >> 15) - (((int)coef_cur) >> 15));
			enc_est += enc_eco_run_est_3(run);
			enc_est += enc_eco_level_est_3(level);
			enc_est++;
			if (scan_pos == W*H - 1) {
				break;
			}
			run = 0;
			enc_est++;
		}
		else{
			run++;
		}


	}
	return enc_est;
}

#elif((W ==8 )&& (H == 8))||((W ==8 )&& (H == 16))||((W ==16 )&& (H == 8))||((W ==16 )&& (H == 16))

int  FUNC_NAME(enc_eco_coef, W, H)(s16 coef[W*H], int log2_w, int log2_h)
{
	U15 enc_est = 0;
	U7 scan_pos;
	u16 level;

	U8 run;

	s16 coef_cur;
	FUNC_NAME(ready_coef, W, H)(coef, coef_out);
	U9 scan[W*H] = {
#if (W == 8) && (H == 8)
		0, 1, 8, 16, 9, 2, 3, 10, 17, 24, 32, 25, 18, 11, 4, 5, 12, 19, 26, 33, 40, 48, 41, 34, 27, 20, 13, 6, 7, 14, 21, 28, 35, 42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37, 44, 51, 58, 59, 52, 45, 38, 31, 39, 46, 53, 60, 61, 54, 47, 55, 62, 63
#elif (W == 16) && (H == 8)
		0, 1, 16, 32, 17, 2, 3, 18, 33, 48, 64, 49, 34, 19, 4, 5, 20, 35, 50, 65, 80, 96, 81, 66, 51, 36, 21, 6, 7, 22, 37, 52, 67, 82, 97, 112, 113, 98, 83, 68, 53, 38, 23, 8, 9, 24, 39, 54, 69, 84, 99, 114, 115, 100, 85, 70, 55, 40, 25, 10, 11, 26, 41, 56, 71, 86, 101, 116, 117, 102, 87, 72, 57, 42, 27, 12, 13, 28, 43, 58, 73, 88, 103, 118, 119, 104, 89, 74, 59, 44, 29, 14, 15, 30, 45, 60, 75, 90, 105, 120, 121, 106, 91, 76, 61, 46, 31, 47, 62, 77, 92, 107, 122, 123, 108, 93, 78, 63, 79, 94, 109, 124, 125, 110, 95, 111, 126, 127
#elif (W == 8) && (H == 16)
		0, 1, 8, 16, 9, 2, 3, 10, 17, 24, 32, 25, 18, 11, 4, 5, 12, 19, 26, 33, 40, 48, 41, 34, 27, 20, 13, 6, 7, 14, 21, 28, 35, 42, 49, 56, 64, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37, 44, 51, 58, 65, 72, 80, 73, 66, 59, 52, 45, 38, 31, 39, 46, 53, 60, 67, 74, 81, 88, 96, 89, 82, 75, 68, 61, 54, 47, 55, 62, 69, 76, 83, 90, 97, 104, 112, 105, 98, 91, 84, 77, 70, 63, 71, 78, 85, 92, 99, 106, 113, 120, 121, 114, 107, 100, 93, 86, 79, 87, 94, 101, 108, 115, 122, 123, 116, 109, 102, 95, 103, 110, 117, 124, 125, 118, 111, 119, 126, 127
#elif (W == 16) && (H == 16)
		0, 1, 16, 32, 17, 2, 3, 18, 33, 48, 64, 49, 34, 19, 4, 5, 20, 35, 50, 65, 80, 96, 81, 66, 51, 36, 21, 6, 7, 22, 37, 52, 67, 82, 97, 112, 128, 113, 98, 83, 68, 53, 38, 23, 8, 9, 24, 39, 54, 69, 84, 99, 114, 129, 144, 160, 145, 130, 115, 100, 85, 70, 55, 40, 25, 10, 11, 26, 41, 56, 71, 86, 101, 116, 131, 146, 161, 176, 192, 177, 162, 147, 132, 117, 102, 87, 72, 57, 42, 27, 12, 13, 28, 43, 58, 73, 88, 103, 118, 133, 148, 163, 178, 193, 208, 224, 209, 194, 179, 164, 149, 134, 119, 104, 89, 74, 59, 44, 29, 14, 15, 30, 45, 60, 75, 90, 105, 120, 135, 150, 165, 180, 195, 210, 225, 240, 241, 226, 211, 196, 181, 166, 151, 136, 121, 106, 91, 76, 61, 46, 31, 47, 62, 77, 92, 107, 122, 137, 152, 167, 182, 197, 212, 227, 242, 243, 228, 213, 198, 183, 168, 153, 138, 123, 108, 93, 78, 63, 79, 94, 109, 124, 139, 154, 169, 184, 199, 214, 229, 244, 245, 230, 215, 200, 185, 170, 155, 140, 125, 110, 95, 111, 126, 141, 156, 171, 186, 201, 216, 231, 246, 247, 232, 217, 202, 187, 172, 157, 142, 127, 143, 158, 173, 188, 203, 218, 233, 248, 249, 234, 219, 204, 189, 174, 159, 175, 190, 205, 220, 235, 250, 251, 236, 221, 206, 191, 207, 222, 237, 252, 253, 238, 223, 239, 254, 255
#else
		0
#endif
	};
	run = 0;


	for (scan_pos = 0; scan_pos < 64; scan_pos++) {
		U9 a = scan[scan_pos] / W;
		U9 b = scan[scan_pos] % W;
		coef_cur = coef_out[a][b];
		if (coef_cur) {
			level = abs(coef_cur);
		//	level = ((((int)coef_cur) ^ ((int)coef_cur) >> 15) - (((int)coef_cur) >> 15));
			enc_est += enc_eco_run_est_3(run);
			enc_est += enc_eco_level_est_3(level);
			enc_est++;
			if (scan_pos == W*H - 1) {
				break;
			}
			run = 0;
			enc_est++;
		}
		else{
			run++;
		}


	}
	return enc_est;
}


#elif (W ==4) && (H ==32)||(W ==8) && (H ==32)||(W ==16) && (H ==32)||(W ==32) && (H ==32)||(W ==8) && (H ==64)||(W ==16) && (H ==64)||(W ==32) && (H ==64)||(W ==64) && (H ==64)
int  FUNC_NAME(enc_eco_coef, W, H)(s16 coef_in[W*H], int log2_w, int log2_h)
{
	U15 enc_est = 0;
	s16 coef_out[W];
	s16 coef[H][W];
	U10 run[W];
#pragma HLS ARRAY_PARTITION variable=coef_out complete dim=1
#pragma HLS ARRAY_PARTITION variable=run complete dim=1

#pragma HLS ARRAY_PARTITION variable=coef complete dim=1
	FUNC_NAME(ready_coef, W, H)(coef_in, coef);
	for (U8 z = 0; z < W; z++){
#pragma HLS UNROLL
		run[z] = 0;
		coef_out[z] = 0;
	}
	//	run_end[0] = 0;
	U16 run_num = 0;
	S8 j;
	for (U8 i = 0; i < W; i++)
	{
#pragma HLS PIPELINE II=1
#pragma HLS LOOP_TRIPCOUNT max=64
		if (i == 0)
		{
			coef_out[0] = coef[0][i];
			if (coef[0][i]) {
				run_num = 0;
			}
			else
				run_num = 1;
		}
		else if (i == 1)
		{
			for (j = 0; j <= 1; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 2)
		{
			for (j = 2; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}

		}
		else if (i == 3)
		{
			for (j = 0; j <= 3; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
#if(W>4)
		else if (i == 4)
		{
			for (j = 4; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 5)
		{
			for (j = 0; j <= 5; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 6)
		{
			for (j = 6; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 7)
		{
			for (j = 0; j <= 7; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
#if (W>8)
		else if (i == 8)
		{
			for (j = 8; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 9)
		{
			for (j = 0; j <= 9; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 10)
		{
			for (j = 10; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 11)
		{
			for (j = 0; j <= 11; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 12)
		{
			for (j = 12; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 13)
		{
			for (j = 0; j <= 13; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 14)
		{
			for (j = 14; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 15)
		{
			for (j = 0; j <= 15; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
#if(W>16)
		else if (i == 16)
		{
			for (j = 16; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 17)
		{
			for (j = 0; j <= 17; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 18)
		{
			for (j = 18; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 19)
		{
			for (j = 0; j <= 19; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 20)
		{
			for (j = 20; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 21)
		{
			for (j = 0; j <= 21; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 22)
		{
			for (j = 22; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 23)
		{
			for (j = 0; j <= 23; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 24)
		{
			for (j = 24; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 25)
		{
			for (j = 0; j <= 25; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 26)
		{
			for (j = 26; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 27)
		{
			for (j = 0; j <= 27; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 28)
		{
			for (j = 28; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}

		}
		else if (i == 29)
		{
			for (j = 0; j <= 29; j++)
			{
#pragma hls unroll

				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;

					run_num++;
				}

			}

		}
		else if (i == 30)
		{
			for (j = 30; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}

		}
		else if (i == 31)
		{
			for (j = 0; j <= 31; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {

					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;

					run_num++;
				}
			}

		}
#if (W>32)
		else if (i == 32)
		{
			for (j = 32; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}

		}
		else if (i == 33)
		{
			for (j = 0; j <= 33; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 34)
		{
			for (j = 34; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}

		}
		else if (i == 35)
		{
			for (j = 0; j <= 35; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					//coef64[j] = src_coef[j*W+i-j];
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					//coef64[j] = 0;
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 36)
		{
			for (j = 36; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					//coef64[i - j] = src_coef[j*W+i-j];
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					//coef64[i - j] = 0;
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 37)
		{
			for (j = 0; j <= 37; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					//coef64[j] = src_coef[j*W+i-j];
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					//coef64[j] = 0;
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 38)
		{
			for (j = 38; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					//coef64[i - j] = src_coef[j*W+i-j];
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					//coef64[i - j] = 0;
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 39)
		{
			for (j = 0; j <= 39; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					//coef64[j] = src_coef[j*W+i-j];
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					//	coef64[j] = 0;
					run[j] = 0;
					run_num++;
				}
			}
		}

		else if (i == 40)
		{
			for (j = 40; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 41)
		{
			for (j = 0; j <= 41; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 42)
		{
			for (j = 42; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 43)
		{
			for (j = 0; j <= 43; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 44)
		{
			for (j = 44; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 45)
		{
			for (j = 0; j <= 45; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 46)
		{
			for (j = 46; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 47)
		{
			for (j = 0; j <= 47; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}

		else if (i == 48)
		{
			for (j = 48; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 49)
		{
			for (j = 0; j <= 49; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 50)
		{
			for (j = 50; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 51)
		{
			for (j = 0; j <= 51; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 52)
		{
			for (j = 52; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 53)
		{
			for (j = 0; j <= 53; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 54)
		{
			for (j = 54; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 55)
		{
			for (j = 0; j <= 55; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 56)
		{
			for (j = 56; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 57)
		{
			for (j = 0; j <= 57; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 58)
		{
			for (j = 58; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 59)
		{
			for (j = 0; j <= 59; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 60)
		{
			for (j = 60; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}

		}
		else if (i == 61)
		{
			for (j = 0; j <= 61; j++)
			{
#pragma hls unroll

				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;

					run_num++;
				}

			}

		}
		else if (i == 62)
		{
			for (j = 62; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}

		}
		else if (i == 63)
		{
			for (j = 0; j <= 63; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {

					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}

		}
#endif
#endif
#endif
#endif
#if(W<=4)
		enc_est += rate_coef_est_4(coef_out, run);
#elif(W<=8)
		enc_est += rate_coef_est_8(coef_out, run);
#elif(W<=16)
		enc_est += rate_coef_est_16(coef_out, run);
#elif(W<=32)
		enc_est += rate_coef_est_32(coef_out, run);
#else
		enc_est += rate_coef_est_64(coef_out, run);
#endif
	}

	return enc_est;
}

#elif (W ==32) && (H ==4)||(W ==32) && (H ==8)||(W ==32) && (H ==16)||(W ==64) && (H ==32)||(W ==64) && (H ==16)||(W ==64) && (H ==8)
int  FUNC_NAME(enc_eco_coef, W, H)(s16 coef_in[W*H], int log2_w, int log2_h)
{
	U15 enc_est = 0;
	s16 coef_out[W];
	s16 coef[H][W];
	U10 run[W];
#pragma HLS ARRAY_PARTITION variable=coef_out complete dim=1
#pragma HLS ARRAY_PARTITION variable=run complete dim=1

#pragma HLS ARRAY_PARTITION variable=coef complete dim=1
	FUNC_NAME(ready_coef, W, H)(coef_in, coef);
	for (U8 z = 0; z < W; z++){
#pragma HLS UNROLL
		run[z] = 0;
		coef_out[z] = 0;
	}
	//	run_end[0] = 0;
	U16 run_num = 0;
	S8 j;
	for (U8 i = 0; i < W; i++)
	{
#pragma HLS PIPELINE II=1
#pragma HLS LOOP_TRIPCOUNT max=64
		if (i == 0)
		{
			coef_out[0] = coef[0][i];
			if (coef[0][i]) {
				run_num = 0;
			}
			else
				run_num = 1;
		}
		else if (i ==1)
		{
			for (j = 0; j <= 1; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 2)
		{
			for (j = 2; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}

		}
		else if (i == 3)
		{
			for (j = 0; j <= 3; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
#if(H==4)
		else if (i & 1)
		{
			for (j = 0; j <= 3; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else //if (!(i &1))
		{
			x = 3;
			for (j = 3; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[x - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[x - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[x - j] = 0;
					run_num++;
				}
			}
		}
#else
		else if (i == 4)
		{
			for (j = 4; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 5)
		{
			for (j = 0; j <= 5; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 6)
		{
			for (j = 6; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 7)
		{
			for (j = 0; j <= 7; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
#if (H==8)
		else if (i & 1)
		{
			for (j = 0; j <= 7; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else //if (!(i &1))
		{
			x = 7;
			for (j = 7; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[x - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[x - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[x - j] = 0;
					run_num++;
				}
			}
		}
#else
		else if (i == 8)
		{
			for (j = 8; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 9)
		{
			for (j = 0; j <= 9; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 10)
		{
			for (j = 10; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 11)
		{
			for (j = 0; j <= 11; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 12)
		{
			for (j = 12; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 13)
		{
			for (j = 0; j <= 13; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 14)
		{
			for (j = 14; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 15)
		{
			for (j = 0; j <= 15; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
#if(H==16)
		else if (i & 1)
		{
			for (j = 0; j <= 15; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else //if (!(i &1))
		{
			x = 15;
			for (j = 15; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[x - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[x - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[x - j] = 0;
					run_num++;
				}
			}
		}
#else
		else if (i == 16)
		{
			for (j = 16; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 17)
		{
			for (j = 0; j <= 17; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 18)
		{
			for (j = 18; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 19)
		{
			for (j = 0; j <= 19; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 20)
		{
			for (j = 20; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 21)
		{
			for (j = 0; j <= 21; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 22)
		{
			for (j = 22; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 23)
		{
			for (j = 0; j <= 23; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 24)
		{
			for (j = 24; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 25)
		{
			for (j = 0; j <= 25; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 26)
		{
			for (j = 26; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 27)
		{
			for (j = 0; j <= 27; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 28)
		{
			for (j = 28; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}

		}
		else if (i == 29)
		{
			for (j = 0; j <= 29; j++)
			{
#pragma hls unroll

				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;

					run_num++;
				}

			}

		}
		else if (i == 30)
		{
			for (j = 30; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}

		}
		else if (i == 31)
		{
			for (j = 0; j <= 31; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {

					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;

					run_num++;
				}
			}

		}
#if (H==32)
		else if (i & 1)
		{
			for (j = 0; j <= 31; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else //if (!(i &1))
		{
			x = 31;
			for (j = 31; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[x - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[x - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[x - j] = 0;
					run_num++;
				}
			}
		}
#endif
#endif
#endif
#endif

#if(H==4)
		enc_est += rate_coef_est_4(coef_out, run);
#elif(H==8)
		enc_est += rate_coef_est_8(coef_out, run);
#elif(H==16)
		enc_est += rate_coef_est_16(coef_out, run);
#elif(H==32)
		enc_est += rate_coef_est_32(coef_out, run);
#else
		enc_est += rate_coef_est_64(coef_out, run);
#endif
	}

	return enc_est;
}
#endif
#else
#if(W ==4 )&& (H == 4)
int  FUNC_NAME(enc_eco_coef, W, H)(s16 coef[W*H], int log2_w, int log2_h)
{
	U15 enc_est = 0;
	U5 scan_pos;
	u16 level;

	U8 run;

	s16 coef_cur;
	//FUNC_NAME(ready_coef, W, H)(coef, coef_out);
	U4 scan[W*H] = {
		0, 1, 4, 8, 5, 2, 3, 6, 9, 12, 13, 10, 7, 11, 14, 15
	};
	run = 0;
	for (scan_pos = 0; scan_pos < 16; scan_pos++) {
		coef_cur = coef[scan[scan_pos]];
		if (coef_cur) {
			level = abs(coef_cur);
			//level = ((((int)coef_cur) ^ ((int)coef_cur) >> 15) - (((int)coef_cur) >> 15));
			enc_est += enc_eco_run_est_3(run);
			enc_est += enc_eco_level_est_3(level);
			enc_est++;
			if (scan_pos == W*H - 1) {
				break;
			}
			run = 0;
			enc_est++;
		}
		else{
			run++;
		}


	}
	return enc_est;
}
#elif((W ==4 )&& (H == 8))||((W ==8 )&& (H == 4))||((W ==4 )&& (H == 16))||((W ==16 )&& (H == 4))
int  FUNC_NAME(enc_eco_coef, W, H)(s16 coef[W*H], int log2_w, int log2_h)
{
	U15 enc_est = 0;
	U6 scan_pos;
	u16 level;

	U8 run;

	s16 coef_cur;

	//FUNC_NAME(ready_coef, W, H)(coef, coef_out);
	u16 scan[W*H] = {
#if (W == 4) && (H == 8)
		0, 1, 4, 8, 5, 2, 3, 6, 9, 12, 16, 13, 10, 7, 11, 14, 17, 20, 24, 21, 18, 15, 19, 22, 25, 28, 29, 26, 23, 27, 30, 31
#elif (W == 8) && (H == 4)
		0, 1, 8, 16, 9, 2, 3, 10, 17, 24, 25, 18, 11, 4, 5, 12, 19, 26, 27, 20, 13, 6, 7, 14, 21, 28, 29, 22, 15, 23, 30, 31
#elif (W == 16) && (H == 4)
		0, 1, 16, 32, 17, 2, 3, 18, 33, 48, 49, 34, 19, 4, 5, 20, 35, 50, 51, 36, 21, 6, 7, 22, 37, 52, 53, 38, 23, 8, 9, 24, 39, 54, 55, 40, 25, 10, 11, 26, 41, 56, 57, 42, 27, 12, 13, 28, 43, 58, 59, 44, 29, 14, 15, 30, 45, 60, 61, 46, 31, 47, 62, 63
#elif (W == 4) && (H == 16)
		0, 1, 4, 8, 5, 2, 3, 6, 9, 12, 16, 13, 10, 7, 11, 14, 17, 20, 24, 21, 18, 15, 19, 22, 25, 28, 32, 29, 26, 23, 27, 30, 33, 36, 40, 37, 34, 31, 35, 38, 41, 44, 48, 45, 42, 39, 43, 46, 49, 52, 56, 53, 50, 47, 51, 54, 57, 60, 61, 58, 55, 59, 62, 63
#else
		0
#endif
	};
	run = 0;
	for (scan_pos = 0; scan_pos < 32; scan_pos++) {
		//u8 a = scan[scan_pos] / W;
		//u8 b = scan[scan_pos] % W;
		//coef_cur = coef_out[a][b];
		coef_cur = coef[scan[scan_pos]];
		if (coef_cur) {
			level = abs(coef_cur);
			//	level = ((((int)coef_cur) ^ ((int)coef_cur) >> 15) - (((int)coef_cur) >> 15));
			enc_est += enc_eco_run_est_3(run);
			enc_est += enc_eco_level_est_3(level);
			enc_est++;
			if (scan_pos == W*H - 1) {
				break;
			}
			run = 0;
			enc_est++;
		}
		else{
			run++;
		}


	}
	return enc_est;
}

#elif((W ==8 )&& (H == 8))||((W ==8 )&& (H == 16))||((W ==16 )&& (H == 8))||((W ==16 )&& (H == 16))

int  FUNC_NAME(enc_eco_coef, W, H)(s16 coef[W*H], int log2_w, int log2_h)
{
	U15 enc_est = 0;

	U7 scan_pos;
	u16 level;

	U8 run;

	s16 coef_cur;

	//FUNC_NAME(ready_coef, W, H)(coef, coef_out);
	U9 scan[W*H] = {
#if (W == 8) && (H == 8)
		0, 1, 8, 16, 9, 2, 3, 10, 17, 24, 32, 25, 18, 11, 4, 5, 12, 19, 26, 33, 40, 48, 41, 34, 27, 20, 13, 6, 7, 14, 21, 28, 35, 42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37, 44, 51, 58, 59, 52, 45, 38, 31, 39, 46, 53, 60, 61, 54, 47, 55, 62, 63
#elif (W == 16) && (H == 8)
		0, 1, 16, 32, 17, 2, 3, 18, 33, 48, 64, 49, 34, 19, 4, 5, 20, 35, 50, 65, 80, 96, 81, 66, 51, 36, 21, 6, 7, 22, 37, 52, 67, 82, 97, 112, 113, 98, 83, 68, 53, 38, 23, 8, 9, 24, 39, 54, 69, 84, 99, 114, 115, 100, 85, 70, 55, 40, 25, 10, 11, 26, 41, 56, 71, 86, 101, 116, 117, 102, 87, 72, 57, 42, 27, 12, 13, 28, 43, 58, 73, 88, 103, 118, 119, 104, 89, 74, 59, 44, 29, 14, 15, 30, 45, 60, 75, 90, 105, 120, 121, 106, 91, 76, 61, 46, 31, 47, 62, 77, 92, 107, 122, 123, 108, 93, 78, 63, 79, 94, 109, 124, 125, 110, 95, 111, 126, 127
#elif (W == 8) && (H == 16)
		0, 1, 8, 16, 9, 2, 3, 10, 17, 24, 32, 25, 18, 11, 4, 5, 12, 19, 26, 33, 40, 48, 41, 34, 27, 20, 13, 6, 7, 14, 21, 28, 35, 42, 49, 56, 64, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37, 44, 51, 58, 65, 72, 80, 73, 66, 59, 52, 45, 38, 31, 39, 46, 53, 60, 67, 74, 81, 88, 96, 89, 82, 75, 68, 61, 54, 47, 55, 62, 69, 76, 83, 90, 97, 104, 112, 105, 98, 91, 84, 77, 70, 63, 71, 78, 85, 92, 99, 106, 113, 120, 121, 114, 107, 100, 93, 86, 79, 87, 94, 101, 108, 115, 122, 123, 116, 109, 102, 95, 103, 110, 117, 124, 125, 118, 111, 119, 126, 127
#elif (W == 16) && (H == 16)
		0, 1, 16, 32, 17, 2, 3, 18, 33, 48, 64, 49, 34, 19, 4, 5, 20, 35, 50, 65, 80, 96, 81, 66, 51, 36, 21, 6, 7, 22, 37, 52, 67, 82, 97, 112, 128, 113, 98, 83, 68, 53, 38, 23, 8, 9, 24, 39, 54, 69, 84, 99, 114, 129, 144, 160, 145, 130, 115, 100, 85, 70, 55, 40, 25, 10, 11, 26, 41, 56, 71, 86, 101, 116, 131, 146, 161, 176, 192, 177, 162, 147, 132, 117, 102, 87, 72, 57, 42, 27, 12, 13, 28, 43, 58, 73, 88, 103, 118, 133, 148, 163, 178, 193, 208, 224, 209, 194, 179, 164, 149, 134, 119, 104, 89, 74, 59, 44, 29, 14, 15, 30, 45, 60, 75, 90, 105, 120, 135, 150, 165, 180, 195, 210, 225, 240, 241, 226, 211, 196, 181, 166, 151, 136, 121, 106, 91, 76, 61, 46, 31, 47, 62, 77, 92, 107, 122, 137, 152, 167, 182, 197, 212, 227, 242, 243, 228, 213, 198, 183, 168, 153, 138, 123, 108, 93, 78, 63, 79, 94, 109, 124, 139, 154, 169, 184, 199, 214, 229, 244, 245, 230, 215, 200, 185, 170, 155, 140, 125, 110, 95, 111, 126, 141, 156, 171, 186, 201, 216, 231, 246, 247, 232, 217, 202, 187, 172, 157, 142, 127, 143, 158, 173, 188, 203, 218, 233, 248, 249, 234, 219, 204, 189, 174, 159, 175, 190, 205, 220, 235, 250, 251, 236, 221, 206, 191, 207, 222, 237, 252, 253, 238, 223, 239, 254, 255
#else
		0
#endif
	};
	run = 0;


	for (scan_pos = 0; scan_pos < 64; scan_pos++) {
		//u8 a = scan[scan_pos] / W;
		//u8 b = scan[scan_pos] % W;
		//coef_cur = coef_out[a][b];
		coef_cur = coef[scan[scan_pos]];
		if (coef_cur) {
			level = abs(coef_cur);
			//	level = ((((int)coef_cur) ^ ((int)coef_cur) >> 15) - (((int)coef_cur) >> 15));
			enc_est += enc_eco_run_est_3(run);
			enc_est += enc_eco_level_est_3(level);
			enc_est++;
			if (scan_pos == W*H - 1) {
				break;
			}
			run = 0;
			enc_est++;
		}
		else{
			run++;
		}


	}
	return enc_est;
}


#elif (W ==4) && (H ==32)||(W ==8) && (H ==32)||(W ==16) && (H ==32)||(W ==32) && (H ==32)||(W ==8) && (H ==64)||(W ==16) && (H ==64)||(W ==32) && (H ==64)||(W ==64) && (H ==64)
int  FUNC_NAME(enc_eco_coef, W, H)(s16 coef[W*H], int log2_w, int log2_h)
{
	U15 enc_est = 0;
	s16 coef_out[W];
	//s16 coef[H][W];
	U10 run[W];
#pragma HLS ARRAY_PARTITION variable=coef_out complete dim=1
#pragma HLS ARRAY_PARTITION variable=run complete dim=1

#pragma HLS ARRAY_PARTITION variable=coef complete dim=1
//	FUNC_NAME(ready_coef, W, H)(coef_in, coef);
	for (U7 z = 0; z < W; z++){
#pragma HLS UNROLL
		run[z] = 0;
		coef_out[z] = 0;
	}
	//	run_end[0] = 0;
	U10 run_num = 0;
	S7 j;
	for (U7 i = 0; i < W; i++)
	{
#pragma HLS PIPELINE II=1
#pragma HLS LOOP_TRIPCOUNT max=64
		if (i == 0)
		{
			coef_out[0] = coef[i];
			if (coef[i]) {
				run_num = 0;
			}
			else
				run_num = 1;
		}
		else if (i == 1)
		{
			for (j = 0; j <= 1; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 2)
		{
			for (j = 2; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}

		}
		else if (i == 3)
		{
			for (j = 0; j <= 3; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
#if(W>4)
		else if (i == 4)
		{
			for (j = 4; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 5)
		{
			for (j = 0; j <= 5; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 6)
		{
			for (j = 6; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 7)
		{
			for (j = 0; j <= 7; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
#if (W>8)
		else if (i == 8)
		{
			for (j = 8; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 9)
		{
			for (j = 0; j <= 9; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 10)
		{
			for (j = 10; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 11)
		{
			for (j = 0; j <= 11; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 12)
		{
			for (j = 12; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 13)
		{
			for (j = 0; j <= 13; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 14)
		{
			for (j = 14; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 15)
		{
			for (j = 0; j <= 15; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
#if(W>16)
		else if (i == 16)
		{
			for (j = 16; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 17)
		{
			for (j = 0; j <= 17; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 18)
		{
			for (j = 18; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 19)
		{
			for (j = 0; j <= 19; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 20)
		{
			for (j = 20; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 21)
		{
			for (j = 0; j <= 21; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 22)
		{
			for (j = 22; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 23)
		{
			for (j = 0; j <= 23; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 24)
		{
			for (j = 24; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 25)
		{
			for (j = 0; j <= 25; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 26)
		{
			for (j = 26; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 27)
		{
			for (j = 0; j <= 27; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 28)
		{
			for (j = 28; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}

		}
		else if (i == 29)
		{
			for (j = 0; j <= 29; j++)
			{
#pragma hls unroll

				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;

					run_num++;
				}

			}

		}
		else if (i == 30)
		{
			for (j = 30; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}

		}
		else if (i == 31)
		{
			for (j = 0; j <= 31; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {

					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;

					run_num++;
				}
			}

		}
#if (W>32)
		else if (i == 32)
		{
			for (j = 32; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}

		}
		else if (i == 33)
		{
			for (j = 0; j <= 33; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 34)
		{
			for (j = 34; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}

		}
		else if (i == 35)
		{
			for (j = 0; j <= 35; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					//coef64[j] = src_coef[j*W+i-j];
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					//coef64[j] = 0;
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 36)
		{
			for (j = 36; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					//coef64[i - j] = src_coef[j*W+i-j];
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					//coef64[i - j] = 0;
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 37)
		{
			for (j = 0; j <= 37; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					//coef64[j] = src_coef[j*W+i-j];
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					//coef64[j] = 0;
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 38)
		{
			for (j = 38; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					//coef64[i - j] = src_coef[j*W+i-j];
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					//coef64[i - j] = 0;
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 39)
		{
			for (j = 0; j <= 39; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					//coef64[j] = src_coef[j*W+i-j];
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					//	coef64[j] = 0;
					run[j] = 0;
					run_num++;
				}
			}
		}

		else if (i == 40)
		{
			for (j = 40; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 41)
		{
			for (j = 0; j <= 41; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 42)
		{
			for (j = 42; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 43)
		{
			for (j = 0; j <= 43; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 44)
		{
			for (j = 44; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 45)
		{
			for (j = 0; j <= 45; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 46)
		{
			for (j = 46; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 47)
		{
			for (j = 0; j <= 47; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}

		else if (i == 48)
		{
			for (j = 48; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 49)
		{
			for (j = 0; j <= 49; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 50)
		{
			for (j = 50; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 51)
		{
			for (j = 0; j <= 51; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 52)
		{
			for (j = 52; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 53)
		{
			for (j = 0; j <= 53; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 54)
		{
			for (j = 54; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 55)
		{
			for (j = 0; j <= 55; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 56)
		{
			for (j = 56; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 57)
		{
			for (j = 0; j <= 57; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 58)
		{
			for (j = 58; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 59)
		{
			for (j = 0; j <= 59; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 60)
		{
			for (j = 60; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}

		}
		else if (i == 61)
		{
			for (j = 0; j <= 61; j++)
			{
#pragma hls unroll

				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;

					run_num++;
				}

			}

		}
		else if (i == 62)
		{
			for (j = 62; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}

		}
		else if (i == 63)
		{
			for (j = 0; j <= 63; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {

					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}

		}
#endif
#endif
#endif
#endif
#if(W<=4)
		enc_est += rate_coef_est_4(coef_out, run);
#elif(W<=8)
		enc_est += rate_coef_est_8(coef_out, run);
#elif(W<=16)
		enc_est += rate_coef_est_16(coef_out, run);
#elif(W<=32)
		enc_est += rate_coef_est_32(coef_out, run);
#else
		enc_est += rate_coef_est_64(coef_out, run);
#endif
	}

	return enc_est;
}

#elif (W ==32) && (H ==4)||(W ==32) && (H ==8)||(W ==32) && (H ==16)||(W ==64) && (H ==32)||(W ==64) && (H ==16)||(W ==64) && (H ==8)
int  FUNC_NAME(enc_eco_coef, W, H)(s16 coef[W*H], int log2_w, int log2_h)
{
	U15 enc_est = 0;
	s16 coef_out[H];
	//s16 coef[H][W];
	U10 run[H];
#pragma HLS ARRAY_PARTITION variable=coef_out complete dim=1
#pragma HLS ARRAY_PARTITION variable=run complete dim=1

#pragma HLS ARRAY_PARTITION variable=coef complete dim=1
	//coef_ready_16x16(coef_in, coef);
	//FUNC_NAME(ready_coef, W, H)(coef_in, coef);
	for (U7 z = 0; z < H; z++){
#pragma HLS UNROLL
		run[z] = 0;
		coef_out[z] = 0;
	}
	U7 x;
	U10 run_num = 0;
	S7 j;
	for (U7 i = 0; i < W; i++)
	{
#pragma HLS PIPELINE II=1
#pragma HLS LOOP_TRIPCOUNT max=64
		if (i == 0)
		{
			coef_out[0] = coef[i];
			if (coef[i]) {
				run_num = 0;
			}
			else
				run_num = 1;
		}
		else if (i == 1)
		{
			for (j = 0; j <= 1; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 2)
		{
			for (j = 2; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}

		}
		else if (i == 3)
		{
			for (j = 0; j <= 3; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
#if(H==4)
		else if (i & 1)
		{
			for (j = 0; j <= 3; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else //if (!(i &1))
		{
			x = 3;
			for (j = 3; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[x - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[x - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[x - j] = 0;
					run_num++;
				}
			}
		}
#else
		else if (i == 4)
		{
			for (j = 4; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 5)
		{
			for (j = 0; j <= 5; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 6)
		{
			for (j = 6; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 7)
		{
			for (j = 0; j <= 7; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
#if (H==8)
		else if (i & 1)
		{
			for (j = 0; j <= 7; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else //if (!(i &1))
		{
			x = 7;
			for (j = 7; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[x - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[x - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[x - j] = 0;
					run_num++;
				}
			}
		}
#else
		else if (i == 8)
		{
			for (j = 8; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 9)
		{
			for (j = 0; j <= 9; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 10)
		{
			for (j = 10; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 11)
		{
			for (j = 0; j <= 11; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 12)
		{
			for (j = 12; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 13)
		{
			for (j = 0; j <= 13; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 14)
		{
			for (j = 14; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 15)
		{
			for (j = 0; j <= 15; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
#if(H==16)
		else if (i & 1)
		{
			for (j = 0; j <= 15; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else //if (!(i &1))
		{
			x = 15;
			for (j = 15; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[x - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[x - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[x - j] = 0;
					run_num++;
				}
			}
		}
#else
		else if (i == 16)
		{
			for (j = 16; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 17)
		{
			for (j = 0; j <= 17; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 18)
		{
			for (j = 18; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 19)
		{
			for (j = 0; j <= 19; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 20)
		{
			for (j = 20; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 21)
		{
			for (j = 0; j <= 21; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 22)
		{
			for (j = 22; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 23)
		{
			for (j = 0; j <= 23; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 24)
		{
			for (j = 24; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 25)
		{
			for (j = 0; j <= 25; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 26)
		{
			for (j = 26; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 27)
		{
			for (j = 0; j <= 27; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else if (i == 28)
		{
			for (j = 28; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}

		}
		else if (i == 29)
		{
			for (j = 0; j <= 29; j++)
			{
#pragma hls unroll

				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;

					run_num++;
				}

			}

		}
		else if (i == 30)
		{
			for (j = 30; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[i - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[i - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[i - j] = 0;
					run_num++;
				}
			}

		}
		else if (i == 31)
		{
			for (j = 0; j <= 31; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {

					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;

					run_num++;
				}
			}

		}
#if (H==32)
		else if (i & 1)
		{
			for (j = 0; j <= 31; j++)
			{
#pragma hls unroll
				coef_out[j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[j] = run_num;
					run_num = 0;
				}
				else
				{
					run[j] = 0;
					run_num++;
				}
			}
		}
		else //if (!(i &1))
		{
			x = 31;
			for (j = 31; j >= 0; j--)
			{
#pragma hls unroll
				coef_out[x - j] = coef[j*W+i-j];
				if (coef[j*W+i-j]) {
					run[x - j] = run_num;
					run_num = 0;
				}
				else
				{
					run[x - j] = 0;
					run_num++;
				}
			}
		}
#endif
#endif
#endif
#endif

#if(H==4)
		enc_est += rate_coef_est_4(coef_out, run);
#elif(H==8)
		enc_est += rate_coef_est_8(coef_out, run);
#elif(H==16)
		enc_est += rate_coef_est_16(coef_out, run);
#elif(H==32)
		enc_est += rate_coef_est_32(coef_out, run);
#else
		enc_est += rate_coef_est_64(coef_out, run);
#endif
	}

	return enc_est;
}
#endif
#endif


int FUNC_NAME(enc_eco_cbf_est_skip, W, H)(RDOQ_MODEL* model_cnt, int tb_avaliable, int num_nz[4][3], u8 pred_mode,
	S8 ipm[4][2], u8 tree_status, int cu_width_log2, int cu_height_log2)
{
	U4 enc_est = 0;
	if ((pred_mode) != 0) {
		if (!((pred_mode) == 3)) {
			if (tree_status == TREE_LC) {
				if (cu_width_log2 > 6 || cu_height_log2 > 6) {
					enc_est++;
				}
				else {
					enc_est++;
				}

				return 0;

			}
		}
		if (tb_avaliable) {
			enc_est++;
		}

		if (tree_status == TREE_LC) {
			enc_est += 2;
			model_cnt->cbf[1]++;
		}
		else {
			//int part_num = get_part_num((PART_SIZE)tb_part_size);

			enc_est += 1;
			model_cnt->cbf[0]++;
		}
	}
	else {
		if (!((ipm[0][0]) == 33)) {
			//int part_num = get_part_num((PART_SIZE)tb_part_size);
			enc_est += 1;
			model_cnt->cbf[0]++;

		}
		if (tree_status == TREE_LC) {
			if (!((ipm[0][0]) == 33 && (ipm[0][1]) == 0)) {
				enc_est += 2;
				model_cnt->cbf[1]++;
			}
		}
	}
	return enc_est;
}
int FUNC_NAME(enc_eco_cbf_est, W, H)(RDOQ_MODEL* model_cnt,  int num_nz[4][3], u8 pred_mode,
	S8 ipm[4][2], u8 tree_status, int cu_width_log2, int cu_height_log2)
{
	U4 enc_est = 0;
	//COM_MODE *mod_info_curr = &core->mod_info_curr;
	U1 ctp_zero_flag = !is_cu_nz(num_nz);
	/* code allcbf */
	if ((pred_mode) != 0) {
		if (!((pred_mode) == 3)) {
			if (tree_status == TREE_LC) {
				if (cu_width_log2 > 6 || cu_height_log2 > 6) {
					enc_est++;
				}
				else {
					enc_est++;
				}
				if (ctp_zero_flag) {
					return 0;
				}
			}
		}

		if (tree_status == TREE_LC) {
			enc_est += 2;
			model_cnt->cbf[1]++;
		}
		if (num_nz[0][1] + num_nz[0][2] == 0  && tree_status == TREE_LC) {
		}
		else {
			//int part_num = get_part_num((PART_SIZE)tb_part_size);

			enc_est += 1;
			model_cnt->cbf[0]++;
		}
	}
	else {
		if (!((ipm[0][0]) == 33)) {
			//int part_num = get_part_num((PART_SIZE)tb_part_size);

			enc_est += 1;
			model_cnt->cbf[0]++;

		}
		if (tree_status == TREE_LC) {
			if (!((ipm[0][0]) == 33 && (ipm[0][1]) == 0)) {
				enc_est += 2;
				model_cnt->cbf[1]++;
			}
		}
	}
	return enc_est;
}

int FUNC_NAME(enc_bit_est_flag, W, H)(RDOQ_MODEL* model_cnt, s32 slice_type, int f_scu, u8 curr_mvr, u8 tree_status, u8 cons_pred_mode, int cu_mode, u8 umve_flag, u8 umve_idx, u8 affine_flag, unsigned char affine_enable_flag, u8 skip_idx, S3 refi[REFP_NUM], u8 mvp_from_hmvp_flag, u8 smvd_flag, u8 num_refp, s16  affine_mvd[REFP_NUM][VER_NUM][MV_D], s16  mvd[REFP_NUM][MV_D], int cu_width_log2, int cu_height_log2,int num_nz[MAX_NUM_TB][N_C], S8   ipm[MAX_NUM_PB][2], int cur_ptr, int refp_ptr[17][2], u8 num_of_hmvp_cand, u8 mpm[4][2], int intra)
{
	U8 enc_est = 0;
	if (intra){
		if (slice_type != 1) {
			if ((cons_pred_mode) != (ONLY_INTRA)) {
				enc_est += 2;
			}
			if (cons_pred_mode == NO_MODE_CONS) {
				enc_est++;
			}
		}
		enc_est += encode_intra_dir_est(ipm[0][0], mpm[0]);
		enc_est++;
	}
	else{
		U2 skip_flag = (cu_mode == 2);
		U2 dir_flag = (cu_mode == 3);
		U1 umve_enable_flag = 1;
		U4 num_of_hmvp_cand = 8;
		U1 amvr_enable_flag = 1;
		U1 emvr_enable_flag = 1;
		U1 smvd_enable_flag = 1;
		enc_est++;
		if (!skip_flag) {
			enc_est++;
		}
		if (skip_flag || dir_flag) {
			if (umve_enable_flag) {
				enc_est++;
			}
			if (umve_flag) {
				enc_est += encode_umve_idx_est((umve_idx));
			}
			else {
				if (cu_width_log2 >= 4 && cu_height_log2 >= 4) {
					enc_est++;
				}
				if (affine_flag) {
					enc_est += sbac_write_truncate_unary_sym_est((skip_idx), 5);
				}
				else {
					enc_est += encode_skip_idx_cu_est((skip_idx), num_of_hmvp_cand, slice_type);
				}
			}
		}
		else {
			if (((cons_pred_mode)) == (NO_MODE_CONS)) {
				enc_est++;
			}
			enc_est += encode_affine_flag_est(((affine_flag)) != 0, affine_enable_flag, cu_width_log2, cu_height_log2);
			if (amvr_enable_flag) {
				// also imply ctx->info.sqh.num_of_hmvp_cand is not zero
				if (emvr_enable_flag && !affine_flag) {
					enc_est++;
				}
				if (affine_flag) {
					enc_est += sbac_write_truncate_unary_sym_est(curr_mvr, 3);
				}
				else {
					enc_est += sbac_write_truncate_unary_sym_est(curr_mvr, 5);
				}
			}
			{
				S8 refi0 = refi[0];
				S8 refi1 = refi[1];
				if ((smvd_enable_flag) && refi0 >= 0 && refi1 >= 0 && cur_ptr - (refp_ptr[0][0]) == (refp_ptr[0][1]) - cur_ptr && (affine_flag) == 0 && !(mvp_from_hmvp_flag)) {
					enc_est++;
				}
				if ((slice_type == 2 || slice_type == 3) && refi0 >= 0) {
					if ((smvd_flag) == 0)
					if ((num_refp) > 1) {
						enc_est += sbac_write_truncate_unary_sym_est(refi0, num_refp);
					}
					if (affine_flag) {
						S16 affine_mvd_real[2];
						U3 amvr_shift = ((curr_mvr) == 0 ? 2 : (((curr_mvr) == 1 ? 4 : 0)));
						affine_mvd_real[0] = (((affine_mvd[0][0][0]) >> (amvr_shift)));
						affine_mvd_real[1] = (((affine_mvd[0][0][1]) >> (amvr_shift)));
						enc_est += encode_mvd_est(affine_mvd_real);
						S16 affine_mvd_real_1[2];
						affine_mvd_real_1[0] = (((affine_mvd[0][1][0]) >> (amvr_shift)));
						affine_mvd_real_1[1] = (((affine_mvd[0][1][1]) >> (amvr_shift)));
						enc_est += encode_mvd_est(affine_mvd_real_1);
					}
					else {
						S16 a_mvd[2];
						a_mvd[0] = (((mvd[0][0]) >> (curr_mvr)));
						a_mvd[1] = (((mvd[0][1]) >> (curr_mvr)));
						enc_est += encode_mvd_est(a_mvd);
					}
				}
			}
		}


		if (tree_status != TREE_C) {
			enc_est += FUNC_NAME(enc_eco_cbf_est, W, H)(model_cnt, num_nz, cu_mode, ipm, tree_status, cu_width_log2, cu_height_log2);
		}
		else if (!((cu_mode) == 0 && (ipm[0][0]) == 33
			&& (ipm[0][1]) == 0)) {
			enc_est += 2;
			model_cnt->cbf[1]++;
		}
	}

	return enc_est;
}

int FUNC_NAME(get_bitcnt_est, W, H)(RDOQ_MODEL* model_cnt, int f_scu, u8 curr_mvr, u8 tree_status,
	s16 coef[W* H], int ch_type, s32 slice_type, u8 cons_pred_mode, int cu_mode, u8 umve_flag, u8 umve_idx, u8 affine_flag, unsigned char affine_enable_flag, u8 skip_idx, S3 refi[REFP_NUM], u8 mvp_from_hmvp_flag, u8 smvd_flag, u8 num_refp, s16  affine_mvd[REFP_NUM][VER_NUM][MV_D], s16  mvd[REFP_NUM][MV_D], int cu_width_log2, int cu_height_log2, int tb_part, int pb_part, int num_nz[MAX_NUM_TB][N_C], S8   ipm[MAX_NUM_PB][2], unsigned char slice_type_md, int cur_ptr, int refp_ptr[17][2], u8 mpm[4][2], int is_intra)
{

	U16 enc_est;
	enc_est = FUNC_NAME(enc_bit_est_flag, W, H)( model_cnt, slice_type, f_scu, curr_mvr, tree_status, cons_pred_mode, cu_mode, umve_flag, umve_idx, affine_flag, affine_enable_flag, skip_idx, refi, mvp_from_hmvp_flag, smvd_flag, num_refp, affine_mvd, mvd, cu_width_log2, cu_height_log2,  num_nz, ipm, cur_ptr, refp_ptr, 8, mpm, is_intra);
	U16 coef_est = FUNC_NAME(enc_eco_coef, W, H)(coef, cu_width_log2, cu_height_log2);
	//int coef_est = enc_eco_coef(coef, cu_width_log2, cu_height_log2, num_nz[0][0], 0);
	return enc_est + coef_est;
}

int FUNC_NAME(enc_bit_est_flag_skip, W, H)(RDOQ_MODEL* model_cnt, s32 slice_type, u8 tree_status, u8 umve_flag, u8 umve_idx, u8 affine_flag, u8 skip_idx, int cu_width_log2, int cu_height_log2, int num_nz[MAX_NUM_TB][N_C], S8   ipm[MAX_NUM_PB][2], u8 num_of_hmvp_cand)
{
	int enc_est = 0;
	int umve_enable_flag = 1;
	enc_est++;
	if (umve_enable_flag) {
		enc_est++;
	}
	if (umve_flag) {
		enc_est += encode_umve_idx_est((umve_idx));
	}
	else {
		if (cu_width_log2 >= 4 && cu_height_log2 >= 4) {
			enc_est++;
		}
		if (affine_flag) {
			enc_est += sbac_write_truncate_unary_sym_est((skip_idx), 5);
		}
		else {
			enc_est += encode_skip_idx_cu_est((skip_idx), num_of_hmvp_cand, slice_type);
		}
	}

	if (tree_status != TREE_C) {
		enc_est += FUNC_NAME(enc_eco_cbf_est_skip,W,H)(model_cnt, 0, num_nz, 2, ipm, tree_status, cu_width_log2, cu_height_log2);
	}
	else if (!((2) == 0 && (ipm[0][0]) == 33
		&& (ipm[0][1]) == 0)) {
		enc_est += 2;
		model_cnt->cbf[1]++;

	}
	return enc_est;
}

//
//void FUNC_NAME(update_rdo_luma, W, H)(RDOQ_MODEL* model_cnt, MD_FW*md_fw_ptr, ENC_CU_DATA_ARRAY *cu_data_temp_ptr, MD_COM_MODE *mod_info_curr_ptr, u8 mode_type, s64 dist_pred[1],
//	s64 *cost_best_temp, MD_COM_MODE *best_info_ptr, COM_PART_INFO *mod_info_curr_pb_info, COM_PART_INFO *mod_info_curr_tb_info, COM_PART_INFO *bst_info_pb_info, COM_PART_INFO *bst_info_tb_info,
//	MD_INPUT *md_input_ptr, s64 *cost_best, u8 curr_mvr, u8 tree_status, u8 cons_pred_mode, s8 coef_rec_write_flag[1], pel rec_y_pb_part[W * H], pel pred_cache[W * H], s16 coef_y_pb_part[W * H], s32 *dist,  s64 lambda_y,
//	int is_intra, ENC_ME_LINE_MAP *me_line_map_ptr, IP_ARRAY *IP_buffer_ptr, IP_MAP_SCU *MAP_SCU_IP_ptr, int tb_w, int tb_h, s64 *cost_pb_best)
//{
//	s64 cost = 0;
//	U7 pb_w = W;
//	U7 pb_h = H;
//	s64 cost_pb_temp = 0;
//	int bin_cnt_luma[1] = { 0 };
//	U13 i;
//	U13 j;
//
//	if (is_intra || mod_info_curr_ptr->num_nz[0][0])
//	{
//		cost = *dist;
//	}
//	U16 flag_est = FUNC_NAME(enc_bit_est_flag, W, H)(model_cnt, md_fw_ptr->slice_type, md_fw_ptr->f_scu, curr_mvr, tree_status, cons_pred_mode, mod_info_curr_ptr->cu_mode, mod_info_curr_ptr->umve_flag, mod_info_curr_ptr->umve_idx, mod_info_curr_ptr->affine_flag, md_fw_ptr->affine_enable_flag, mod_info_curr_ptr->skip_idx, mod_info_curr_ptr->refi, mod_info_curr_ptr->mvp_from_hmvp_flag, mod_info_curr_ptr->smvd_flag, md_fw_ptr->num_refp, mod_info_curr_ptr->affine_mvd, mod_info_curr_ptr->mvd, mod_info_curr_ptr->cu_width_log2, mod_info_curr_ptr->cu_height_log2,  mod_info_curr_ptr->num_nz, mod_info_curr_ptr->ipm, md_fw_ptr->cur_ptr, md_fw_ptr->refp_ptr, 8, mod_info_curr_ptr->mpm, is_intra);
//	U16 coef_est = FUNC_NAME(enc_eco_coef, W, H)(coef_y_pb_part, mod_info_curr_ptr->cu_width_log2, mod_info_curr_ptr->cu_height_log2);
//	U16 bit_cnt = flag_est + coef_est;
//	cost += (bit_cnt * lambda_y + 500)  >>10;
//	if (is_intra == 0){
//		if (mod_info_curr_ptr->num_nz[0][0]){
//			if (cost < *cost_best_temp) {
//				*cost_best_temp = cost;
//			}
//		}
//		if (mode_type == 0) {
//			cost = dist_pred[0];
//			flag_est = FUNC_NAME(enc_bit_est_flag_skip, W, H)(model_cnt, md_fw_ptr->slice_type, tree_status, mod_info_curr_ptr->umve_flag, mod_info_curr_ptr->umve_idx, mod_info_curr_ptr->affine_flag, mod_info_curr_ptr->skip_idx, mod_info_curr_ptr->cu_width_log2, mod_info_curr_ptr->cu_height_log2, mod_info_curr_ptr->num_nz, mod_info_curr_ptr->ipm, 8);
//			cost += (flag_est * lambda_y + 500) >>10;
//			if (cost < *cost_best_temp) {
//				*cost_best_temp = cost;
//				mod_info_curr_ptr->cu_mode = 2;
//				mod_info_curr_ptr->tb_part = (SIZE_2Nx2N);
//				for (i = 0; i < 4; i++){
//					for (j = 0; j < 3; j++){
//						mod_info_curr_ptr->num_nz[i][j] = 0;
//					}
//				}
//			}
//		}
//		
//		U2 lidx;
//		U7 cu_width = W;
//		U7 cu_height = H;
//		U2 vertex;
//		U2 vertex_num;
//
//	
//		static U11 skip_mode_2_thread_s64[64] = { (1000), (1002), (1004), (1005), (1007), (1009), (1010), (1012), (1014), (1016), (1017), (1019), (1020), (1022), (1024), (1026), (1027), (1029), (1030), (1032), (1034), (1036), (1037), (1039), (1040), (1042), (1044), (1046), (1047), (1049), (1050), (1052), (1054), (1056), (1057), (1059), (1060), (1062), (1064), (1066), (1067), (1069), (1070), (1072), (1074), (1076), (1077), (1079), (1080), (1082), (1084), (1086), (1087), (1089), (1090), (1092), (1094), (1096), (1097), (1099), (1100), (1102), (1104), (1106) };
//
//		if (*cost_best_temp < *cost_best) {
//
//			best_info_ptr->cu_mode = mod_info_curr_ptr->cu_mode;
//			best_info_ptr->pb_part = mod_info_curr_ptr->pb_part;
//			best_info_ptr->tb_part = mod_info_curr_ptr->tb_part;
//			bst_info_pb_info->num_sub_part = mod_info_curr_pb_info->num_sub_part;
//			bst_info_tb_info->num_sub_part = mod_info_curr_tb_info->num_sub_part;
//			for (U1 it = 0; it < 1; it++) {
//#pragma HLS UNROLL
//#pragma HLS loop_tripcount max = 1
//				bst_info_pb_info->sub_x[it] = mod_info_curr_pb_info->sub_x[it];
//				bst_info_pb_info->sub_y[it] = mod_info_curr_pb_info->sub_y[it];
//				bst_info_pb_info->sub_w[it] = mod_info_curr_pb_info->sub_w[it];
//				bst_info_pb_info->sub_h[it] = mod_info_curr_pb_info->sub_h[it];
//				bst_info_pb_info->sub_scup[it] = mod_info_curr_pb_info->sub_scup[it];
//				bst_info_tb_info->sub_x[it] = mod_info_curr_tb_info->sub_x[it];
//				bst_info_tb_info->sub_y[it] = mod_info_curr_tb_info->sub_y[it];
//				bst_info_tb_info->sub_w[it] = mod_info_curr_tb_info->sub_w[it];
//				bst_info_tb_info->sub_h[it] = mod_info_curr_tb_info->sub_h[it];
//				bst_info_tb_info->sub_scup[it] = mod_info_curr_tb_info->sub_scup[it];
//			}
//			best_info_ptr->umve_flag = mod_info_curr_ptr->umve_flag;
//			*cost_best = *cost_best_temp;
//			if (best_info_ptr->cu_mode != 0) {
//				if (best_info_ptr->cu_mode == 2 || best_info_ptr->cu_mode == 3) {
//					best_info_ptr->mvr_idx = ((u8)0);
//				}
//				else {
//					best_info_ptr->mvp_from_hmvp_flag = mod_info_curr_ptr->mvp_from_hmvp_flag;
//					best_info_ptr->mvr_idx = curr_mvr;
//				}
//				best_info_ptr->refi[0] = mod_info_curr_ptr->refi[0];
//				best_info_ptr->refi[1] = mod_info_curr_ptr->refi[1];
//				for (lidx = 0; lidx < 2; lidx++) {
//#pragma HLS UNROLL
//#pragma HLS loop_tripcount max = 2
//					best_info_ptr->mv[lidx][0] = mod_info_curr_ptr->mv[lidx][0];
//					best_info_ptr->mv[lidx][1] = mod_info_curr_ptr->mv[lidx][1];
//					best_info_ptr->mvd[lidx][0] = mod_info_curr_ptr->mvd[lidx][0];
//					best_info_ptr->mvd[lidx][1] = mod_info_curr_ptr->mvd[lidx][1];
//				}
//				best_info_ptr->smvd_flag = mod_info_curr_ptr->smvd_flag;
//				best_info_ptr->affine_flag = mod_info_curr_ptr->affine_flag;
//				if (best_info_ptr->affine_flag) {
//
//					vertex_num = (best_info_ptr->affine_flag) + 1;
//					for (lidx = 0; lidx < 2; lidx++) {
//#pragma HLS UNROLL
//#pragma HLS loop_tripcount max = 2
//						for (vertex = 0; vertex < vertex_num; vertex++) {
//#pragma HLS UNROLL
//#pragma HLS loop_tripcount max = 2
//							best_info_ptr->affine_mv[lidx][vertex][0] = mod_info_curr_ptr->affine_mv[lidx][vertex][0];
//							best_info_ptr->affine_mv[lidx][vertex][1] = mod_info_curr_ptr->affine_mv[lidx][vertex][1];
//							best_info_ptr->affine_mvd[lidx][vertex][0] = mod_info_curr_ptr->affine_mvd[lidx][vertex][0];
//							best_info_ptr->affine_mvd[lidx][vertex][1] = mod_info_curr_ptr->affine_mvd[lidx][vertex][1];
//						}
//					}
//				}
//				if (best_info_ptr->cu_mode == 2) {
//					if (((int)(best_info_ptr->umve_flag)) != 0) {
//						best_info_ptr->umve_idx = mod_info_curr_ptr->umve_idx;
//					}
//					else {
//						best_info_ptr->skip_idx = mod_info_curr_ptr->skip_idx;
//					}
//					for (U11 i_p = (0); i_p < W * H; i_p++) {
//						best_info_ptr->pred_y[i_p] = pred_cache[i_p];
//					}
//					for (U3 i_numnz = 0; i_numnz < 4; i_numnz++) {
//#pragma HLS UNROLL
//#pragma HLS loop_tripcount max = 3
//						best_info_ptr->num_nz[i_numnz][0] = 0;
//					}
//					coef_rec_write_flag[0] = (1);
//					for (U11 i_c = (0); i_c < W * H; i_c++) {
//						cu_data_temp_ptr->coef_y[i_c] = 0;
//					}
//					
//				}
//				else {
//					if (best_info_ptr->cu_mode == 3) {
//						if (best_info_ptr->umve_flag) {
//							best_info_ptr->umve_idx = mod_info_curr_ptr->umve_idx;
//						}
//						else {
//							best_info_ptr->skip_idx = mod_info_curr_ptr->skip_idx;
//						}
//					}
//
//					for (U3 i_numnz = 0; i_numnz < 4; i_numnz++) {
//#pragma HLS UNROLL
//#pragma HLS loop_tripcount max = 3
//						best_info_ptr->num_nz[i_numnz][0] = mod_info_curr_ptr->num_nz[i_numnz][0];
//					}
//					for (U11 i_pc = 0; i_pc < W * H; i_pc++) {
//						best_info_ptr->pred_y[i_pc] = pred_cache[i_pc];
//						cu_data_temp_ptr->coef_y[i_pc] = coef_y_pb_part[i_pc];
//					}
//
//					coef_rec_write_flag[0] = 1;
//				}
//			}
//			pel *p;
//			S16 *r;
//			s16 *c;
//			s16 pred;
//			p = pred_cache ;
//			r = cu_data_temp_ptr->reco_y ;
//				if (mod_info_curr_ptr->num_nz[0][0] == 0) {
//					for (i = 0; i < H; i++) {
//#pragma HLS UNROLL
//						for (j = 0; j < W; j++) {
//#pragma HLS UNROLL
//							pred = 255 < p[i * cu_width + j] ? 255 : p[i * cu_width + j];
//							r[i * cu_width + j] = 0>pred?0:pred;
//						}
//					}
//				}
//				else
//				{
//					c = rec_y_pb_part ;
//					for (i = 0; i < H; i++) {
//#pragma HLS UNROLL
//						for (j = 0; j < W; j++) {
//#pragma HLS UNROLL
//							pred = 255 < c[i * cu_width + j] ? 255 : c[i * cu_width + j];
//							r[i * cu_width + j] = 0 > pred ? 0 : pred;
//						}
//					}
//				}
//		//	}
//
//			coef_rec_write_flag[0] = 1;
//		}
//	}
//
//
//	if (is_intra){
//		cost_pb_temp += cost;
//		if (cost_pb_temp < *cost_pb_best){
//			*cost_pb_best = cost_pb_temp;
//		}
//		if (cost_pb_temp < *cost_best) {
//			best_info_ptr->cu_mode = MODE_INTRA;
//			best_info_ptr->affine_flag = 0;
//			*cost_best = cost_pb_temp;
//			memcpy_rewrintra(mod_info_curr_ptr->coef_y, coef_y_pb_part, pb_w * pb_h);
//			memcpy_rewrintra(mod_info_curr_ptr->rec_y, rec_y_pb_part, pb_w*pb_h);
//
//			for (i = 0; i < 4; ++i){
//				for (j = 0; j < 2; ++j){
//					best_info_ptr->mpm[i][j] = mod_info_curr_ptr->mpm[i][j];
//					best_info_ptr->ipm[i][j] = mod_info_curr_ptr->ipm[i][j];
//				}
//			}
//			for (i = 0; i < 4; ++i){
//				for (j = 0; j < 3; ++j){
//					best_info_ptr->num_nz[i][j] = mod_info_curr_ptr->num_nz[i][j];
//				}
//			}
//			for (i = 0; i < ((W)*(H)); ++i){
//				best_info_ptr->coef_y[i] = mod_info_curr_ptr->coef_y[i];
//				best_info_ptr->pred_y[i] = mod_info_curr_ptr->pred_y[i];
//				best_info_ptr->rec_y[i] = mod_info_curr_ptr->rec_y[i];
//			}
//		}
//	}
//
//}
#undef W
#undef H