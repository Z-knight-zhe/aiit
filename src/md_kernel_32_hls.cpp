#include "md_kernel_32.h"
#include "md_kernel_32_hls.h"

FILE * md_kernel_32_file_out = NULL ;

//#ifdef __SYNTHESIS__
#if 0
#define SUB_MODEL_SYN 1
#define NO_SUB_MODEL_SYN 0
#else
#define SUB_MODEL_SYN 0
#define NO_SUB_MODEL_SYN 1
#endif

#define HLS_WRAP 1
namespace md_kernel_32_hls{

static const s8 com_tbl_log2[257] = {
	/* 0, 1 */
	-1, -1,
	/* 2, 3 */
	1, -1,
	/* 4 ~ 7 */
	2, -1, -1, -1,
	/* 8 ~ 15 */
	3, -1, -1, -1, -1, -1, -1, -1,
	/* 16 ~ 31 */
	4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	/* 31 ~ 63 */
	5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	/* 64 ~ 127 */
	6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	/* 128 ~ 255 */
	7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	/* 256 */
	8
};


	static const U16 quant_scale[80] = { (16302), (15024), (13777), (12634), (11626), (10624), (9742), (8958), (8192), (7512), (6889), (6305), (5793), (5303), (4878), (4467), (4091), (3756), (3444), (3161), (2894), (2654), (2435), (2235), (2048), (1878), (1722), (1579), (1449), (1329), (1218), (1117), (1024), (939), (861), (790), (724), (664), (609), (558), (512), (470), (430), (395), (362), (332), (304), (279), (256), (235), (215), (197), (181), (166), (152), (140), (128), (116), (108), (99), (91), (83), (76), (69), (64), (59), (54), (49), (45), (41), (38), (35), (32), (30), (27), (25), (23), (21), (19), (18) };
	static const U30 err_scale_tbl[80][6] = { 32932, 65865, 131731, 263462, 526925, 1053850, 35734, 71468, 142936, 285873, 571747, 1143495, 38968, 77937, 155874, 311749, 623498, 1246996, 42494, 84988, 169976, 339953, 679906, 1359812, 46178, 92356, 184713, 369427, 738855, 1477711, 50533, 101067, 202135, 404270, 808540, 1617081, 55108, 110217, 220435, 440871, 881742, 1763484, 59932, 119864, 239728, 479456, 958912, 1917824, 65536, 131072, 262144, 524288, 1048576, 2097152, 71468, 142936, 285873, 571747, 1143495, 2286990, 77931, 155863, 311726, 623452, 1246905, 2493811, 85150, 170300, 340600, 681200, 1362400, 2724800, 92675, 185351, 370703, 741406, 1482812, 2965625, 101239, 202478, 404956, 809912, 1619825, 3239650, 110059, 220119, 440238, 880477, 1760954, 3521908, 120186, 240372, 480744, 961488, 1922976, 3845952, 131232, 262464, 524928, 1049857, 2099715, 4199430, 142936, 285873, 571747, 1143495, 2286990, 4573980, 155885, 311771, 623543, 1247086, 2494173, 4988347, 169842, 339684, 679368, 1358736, 2717473, 5434947, 185511, 371023, 742046, 1484093, 2968187, 5936374, 202287, 404574, 809149, 1618299, 3236599, 6473198, 220480, 440961, 881923, 1763846, 3527693, 7055387, 240210, 480421, 960842, 1921685, 3843371, 7686742, 262144, 524288, 1048576, 2097152, 4194304, 8388608, 285873, 571747, 1143495, 2286990, 4573980, 9147960, 311771, 623543, 1247086, 2494173, 4988347, 9976695, 340006, 680013, 1360027, 2720055, 5440110, 10880221, 370511, 741022, 1482045, 2964090, 5928181, 11856362, 403966, 807932, 1615864, 3231728, 6463457, 12926914, 440780, 881561, 1763122, 3526245, 7052491, 14104982, 480636, 961272, 1922545, 3845091, 7690183, 15380366, 524288, 1048576, 2097152, 4194304, 8388608, 16777216, 571747, 1143495, 2286990, 4573980, 9147960, 18295920, 623543, 1247086, 2494173, 4988347, 9976695, 19953390, 679583, 1359166, 2718333, 5436667, 10873334, 21746669, 741534, 1483068, 2966137, 5932275, 11864550, 23729101, 808540, 1617081, 3234162, 6468324, 12936648, 25873296, 881561, 1763122, 3526245, 7052491, 14104982, 28209965, 962134, 1924268, 3848537, 7697074, 15394148, 30788296, 1048576, 2097152, 4194304, 8388608, 16777216, 33554432, 1142278, 2284557, 4569114, 9138228, 18276456, 36552913, 1248537, 2497074, 4994148, 9988296, 19976592, 39953184, 1359166, 2718333, 5436667, 10873334, 21746669, 43493339, 1483068, 2966137, 5932275, 11864550, 23729101, 47458202, 1617081, 3234162, 6468324, 12936648, 25873296, 51746593, 1766022, 3532045, 7064090, 14128181, 28256363, 56512727, 1924268, 3848537, 7697074, 15394148, 30788296, 61576592, 2097152, 4194304, 8388608, 16777216, 33554432, 67108864, 2284557, 4569114, 9138228, 18276456, 36552913, 73105826, 2497074, 4994148, 9988296, 19976592, 39953184, 79906368, 2725233, 5450466, 10900932, 21801864, 43603728, 87207457, 2966137, 5932275, 11864550, 23729101, 47458202, 94916404, 3234162, 6468324, 12936648, 25873296, 51746593, 103493187, 3532045, 7064090, 14128181, 28256363, 56512727, 113025455, 3834792, 7669584, 15339168, 30678337, 61356675, 122713351, 4194304, 8388608, 16777216, 33554432, 67108864, 134217728, 4628197, 9256395, 18512790, 37025580, 74051160, 148102320, 4971026, 9942053, 19884107, 39768215, 79536431, 159072862, 5422938, 10845877, 21691754, 43383508, 86767016, 173534032, 5899680, 11799360, 23598721, 47197442, 94394885, 188789771, 6468324, 12936648, 25873296, 51746593, 103493187, 206986375, 7064090, 14128181, 28256363, 56512727, 113025455, 226050910, 7780737, 15561475, 31122951, 62245902, 124491805, 248983611, 8388608, 16777216, 33554432, 67108864, 134217728, 268435456, 9099506, 18199013, 36398027, 72796055, 145592111, 291184223, 9942053, 19884107, 39768215, 79536431, 159072862, 318145725, 10956549, 21913098, 43826196, 87652393, 175304787, 350609575, 11930464, 23860929, 47721858, 95443717, 190887435, 381774870, 13094412, 26188824, 52377649, 104755299, 209510599, 419021199, 14128181, 28256363, 56512727, 113025455, 226050910, 452101820, 15339168, 30678337, 61356675, 122713351, 245426702, 490853405, 16777216, 33554432, 67108864, 134217728, 268435456, 536870912, 17895697, 35791394, 71582788, 143165576, 286331153, 572662306, 19884107, 39768215, 79536431, 159072862, 318145725, 636291451, 21474836, 42949672, 85899345, 171798691, 343597383, 687194767, 23342213, 46684427, 93368854, 186737708, 373475417, 746950834, 25565281, 51130563, 102261126, 204522252, 409044504, 818089008, 28256363, 56512727, 113025455, 226050910, 452101820, 904203641, 29826161, 59652323, 119304647, 238609294, 477218588, 954437176 };
#define GET_I_COST(rate, lamba)  (rate*lamba)
//FILE * file1_in_1 = NULL;
//FILE * file1_in_2 = NULL;
//FILE * file1_in_3 = NULL;
//FILE * file1_in_4 = NULL;
FILE * file1_out_1 = NULL;
FILE * file1_out_2 = NULL;
FILE * file1_out_3 = NULL;
FILE * file1_out_4 = NULL;
FILE * file1_out_5 = NULL;
FILE * file1_out_6 = NULL;
FILE * file1_out_7 = NULL;
FILE * file1_out_8 = NULL;
FILE * file1_out_9 = NULL;
FILE * file1_out_10 = NULL;

static int num_32 = 0;

void stage_h_split_copy(
	s16 src[32][32],
	hls::stream<s512_h> &dst1,
	hls::stream<s512_h> &dst2,
	hls::stream<s512_h> &dst3
	)
{
#pragma HLS INLINE
	ap_int<512> tmp;
	for(int i = 0 ; i < 32 ; i ++)
	{
#pragma HLS PIPELINE
		for(int j = 0 ; j < 32 ; j ++)
		{
			tmp(j*16+15,j*16) = src[i][j];
		}
		dst1.write(tmp);
		dst2.write(tmp);
		dst3.write(tmp);
	}
}

void stage_h_split_copy(
	s16 src[32][32],
	hls::stream<s512_h> &dst1,
	hls::stream<s512_h> &dst2
	)
{
#pragma HLS INLINE
	ap_int<512> tmp;
	for(int i = 0 ; i < 32 ; i ++)
	{
#pragma HLS PIPELINE
		for(int j = 0 ; j < 32 ; j ++)
		{
			tmp(j*16+15,j*16) = src[i][j];
		}
		dst1.write(tmp);
		dst2.write(tmp);
	}
}


void stage_h_split_copy(
	s16 src[32][32],
	s16 dst1[32][32],
	s16 dst2[32][32]
	)
{
#pragma HLS INLINE
	for(int i = 0 ; i < 32 ; i ++)
	{
#pragma HLS PIPELINE
		for(int j = 0 ; j < 32 ; j ++)
		{
			dst1[i][j] = src[i][j];
			dst2[i][j] = src[i][j];
		}
	}
}

void stage_h_split_copy(
	S8 src[4][2],
	S8 dst1[4][2],
	S8 dst2[4][2]
	)
{
#pragma HLS INLINE
	for(int i = 0 ; i < 4 ; i ++)
	{
#pragma HLS PIPELINE
		for(int j = 0 ; j < 2 ; j ++)
		{
			dst1[i][j] = src[i][j];
			dst2[i][j] = src[i][j];
		}
	}
}

void stage_copy(
	S8 src[4][2],
	hls::stream<s64_h> &dst
	)
{
#pragma HLS INLINE
	s64_h tmp;
	tmp(7,0) = src[0][0];
	tmp(15,8) = src[0][1];
	tmp(23,16) = src[1][0];
	tmp(31,24) = src[1][1];
	tmp(39,32) = src[2][0];
	tmp(47,40) = src[2][1];
	tmp(55,48) = src[3][0];
	tmp(63,56) = src[3][1];
	dst.write(tmp);

}

void stage_copy(
	U8 src[4][2],
	hls::stream<u64_h> &dst
	)
{
#pragma HLS INLINE
	u64_h tmp;
	tmp(7,0) = src[0][0];
	tmp(15,8) = src[0][1];
	tmp(23,16) = src[1][0];
	tmp(31,24) = src[1][1];
	tmp(39,32) = src[2][0];
	tmp(47,40) = src[2][1];
	tmp(55,48) = src[3][0];
	tmp(63,56) = src[3][1];
	dst.write(tmp);
}

void stage_copy(
	int src[4][3],
	hls::stream<s384_h> &dst
	)
{
#pragma HLS INLINE
	s384_h tmp;
	tmp(31,0) = src[0][0];
	tmp(63,32) = src[0][1];
	tmp(95,64) = src[0][2];

	tmp(127,96) = src[1][0];
	tmp(159,128) = src[1][1];
	tmp(191,160) = src[1][2];

	tmp(223,192) = src[2][0];
	tmp(255,224) = src[2][1];
	tmp(287,256) = src[2][2];

	tmp(319,288) = src[3][0];
	tmp(351,320) = src[3][1];
	tmp(383,352) = src[3][2];
	dst.write(tmp);
}

void stage_copy(
	s64 src,
	hls::stream<s64_h> &dst
	)
{
#pragma HLS INLINE
	dst.write(src);
}

void stage_h_split_copy(
	u8 src[4][2],
	u8 dst1[4][2],
	u8 dst2[4][2]
	)
{
#pragma HLS INLINE
	for(int i = 0 ; i < 4 ; i ++)
	{
#pragma HLS PIPELINE
		for(int j = 0 ; j < 2 ; j ++)
		{
			dst1[i][j] = src[i][j];
			dst2[i][j] = src[i][j];
		}
	}
}

void stage_h_split_copy(
	hls::stream<s512_h> &src,
	hls::stream<s512_h> &dst1,
	hls::stream<s512_h> &dst2
	)
{
#pragma HLS INLINE
	ap_int<512> tmp;
	for(int i = 0 ; i < 32 ; i ++)
	{
#pragma HLS PIPELINE
		tmp=src.read();
		dst1.write(tmp);
		dst2.write(tmp);
	}
}



void stage_h_copy(s16 src[32][32],hls::stream<s512_h> &dst)
{
#pragma HLS INLINE
	ap_int<512> tmp;
	for(int i = 0 ; i < 32 ; i ++)
	{
#pragma HLS PIPELINE
		for(int j = 0 ; j < 32 ; j ++)
		{
			tmp(j*16+15,j*16) = src[i][j];
		}
		dst.write(tmp);
	}
}

void stage_hv_copy(s16 src1[32][32],s16 src2[32][32],hls::stream<s512_h> &dst1,hls::stream<s512_h> &dst2)
{
#pragma HLS INLINE
	ap_int<512> tmp1;
	ap_int<512> tmp2;
	for(int i = 0 ; i < 32 ; i ++)
	{
#pragma HLS PIPELINE
		for(int j = 0 ; j < 32 ; j ++)
		{
			tmp1(j*16+15,j*16) = src1[i][j];
			tmp2(j*16+15,j*16) = src2[j][i];
		}
		dst1.write(tmp1);
		dst2.write(tmp2);
	}
}


void stage_h_copy(s16 src[32][32],s16 dst[32][32])
{
#pragma HLS INLINE
	for(int i = 0 ; i < 32 ; i ++)
	{
#pragma HLS PIPELINE
		for(int j = 0 ; j < 32 ; j ++)
		{
			dst[i][j] = src[i][j];
		}
	}
}

void stage_h_copy2(s16 src[32][32],s16 dst[32][32])
{
#pragma HLS INLINE
	for(int i = 0 ; i < 32 ; i ++)
	{
#pragma HLS PIPELINE
		for(int j = 0 ; j < 32 ; j ++)
		{
			dst[i][j] = src[i][j];
		}
	}
}

void stage_h_copy(s16 src[32][32],int dst[32][32])
{
#pragma HLS INLINE
	for(int i = 0 ; i < 32 ; i ++)
	{
#pragma HLS PIPELINE
		for(int j = 0 ; j < 32 ; j ++)
		{
			dst[i][j] = src[i][j];
		}
	}
}

void stage_h_copy(hls::stream<s512_h> & src,s16 dst[32][32])
{
#pragma HLS INLINE
	ap_int<512> tmp;
	for(int i = 0 ; i < 32 ; i ++)
	{
#pragma HLS PIPELINE
		tmp = src.read();
		for(int j = 0 ; j < 32 ; j ++)
		{
			dst[i][j] = tmp(j*16+15,j*16);
		}
	}
}

void stage_h_copy(s16 src[32][32],s16 dst1[32][32],s16 dst2[32][32],s16 dst3[32][32],s16 dst4[32][32])
{
#pragma HLS INLINE
	for(int i = 0 ; i < 32 ; i ++)
	{
#pragma HLS PIPELINE
		for(int j = 0 ; j < 32 ; j ++)
		{
			dst1[i][j] = src[i][j];
			dst2[i][j] = src[i][j];
			dst3[i][j] = src[i][j];
			dst4[i][j] = src[i][j];
		}
	}
}

void stage_h_copy(s16 src[32][32],s16 dst1[32][32],s16 dst2[32][32])
{
#pragma HLS INLINE
	for(int i = 0 ; i < 32 ; i ++)
	{
#pragma HLS PIPELINE
		for(int j = 0 ; j < 32 ; j ++)
		{
			dst1[i][j] = src[i][j];
			dst2[i][j] = src[i][j];
		}
	}
}

void stage_v_copy(s16 src[32][32],hls::stream<s512_h> &dst)
{
#pragma HLS INLINE
	ap_int<512> tmp;
	for(int i = 0 ; i < 32 ; i ++)
	{
#pragma HLS PIPELINE
		for(int j = 0 ; j < 32 ; j ++)
		{
			tmp(j*16+15,j*16) = src[j][i];
		}
		dst.write(tmp);
	}
}

void stage_v_copy(s16 src[32][32],s16 dst[32][32])
{
#pragma HLS INLINE
	for(int i = 0 ; i < 32 ; i ++)
	{
#pragma HLS PIPELINE
		for(int j = 0 ; j < 32 ; j ++)
		{
			dst[j][i] = src[j][i];
		}
	}
}

void stage_v_copy(s16 src[32][32],s16 dst1[32][32],s16 dst2[32][32],s16 dst3[32][32],s16 dst4[32][32])
{
#pragma HLS INLINE
	for(int i = 0 ; i < 32 ; i ++)
	{
#pragma HLS PIPELINE
		for(int j = 0 ; j < 32 ; j ++)
		{
			dst1[j][i] = src[j][i];
			dst2[j][i] = src[j][i];
			dst3[j][i] = src[j][i];
			dst4[j][i] = src[j][i];
		}
	}
}

void stage_v_copy(s16 src[32][32],int dst[32][32])
{
#pragma HLS INLINE
	for(int i = 0 ; i < 32 ; i ++)
	{
#pragma HLS PIPELINE
		for(int j = 0 ; j < 32 ; j ++)
		{
			dst[j][i] = src[j][i];
		}
	}
}

void stage_v_copy2(hls::stream<s512_h> & src,s16 dst[32][32])
{
#pragma HLS INLINE
	ap_int<512> tmp;
	for(int i = 0 ; i < 32 ; i ++)
	{
#pragma HLS PIPELINE
		tmp = src.read();
		for(int j = 0 ; j < 32 ; j ++)
		{
			dst[j][i] = tmp(j*16+15,j*16);
		}
	}
}

void stage_v_split_copy(
	s16 src[32][32],
	s16 dst1[32][32],
	s16 dst2[32][32]
	)
{
#pragma HLS INLINE
	ap_int<512> tmp;
	for(int i = 0 ; i < 32 ; i ++)
	{
#pragma HLS PIPELINE
		for(int j = 0 ; j < 32 ; j ++)
		{
			dst1[j][i] = src[j][i];
			dst2[j][i] = src[j][i];
		}
	}
}

void stage_v_split_copy(
	int src[4][3],
	int dst1[4][3],
	int dst2[4][3]
	)
{
#pragma HLS INLINE
	ap_int<512> tmp;
	for(int i = 0 ; i < 4 ; i ++)
	{
#pragma HLS PIPELINE
		for(int j = 0 ; j < 3 ; j ++)
		{
			dst1[i][j] = src[i][j];
			dst2[i][j] = src[i][j];
		}
	}
}

void stage_v_split_copy(
	s16 src[32][32],
	hls::stream<s512_h> &dst1,
	hls::stream<s512_h> &dst2,
	hls::stream<s512_h> &dst3
	)
{
#pragma HLS INLINE
	ap_int<512> tmp;
	for(int i = 0 ; i < 32 ; i ++)
	{
#pragma HLS PIPELINE
		for(int j = 0 ; j < 32 ; j ++)
		{
			tmp(j*16+15,j*16) = src[j][i];
		}
		dst1.write(tmp);
		dst2.write(tmp);
		dst3.write(tmp);
	}
}

void stage_v_split_copy(
	s16 src[32][32],
	hls::stream<s512_h> &dst1,
	hls::stream<s512_h> &dst2
	)
{
#pragma HLS INLINE
	ap_int<512> tmp;
	for(int i = 0 ; i < 32 ; i ++)
	{
#pragma HLS PIPELINE
		for(int j = 0 ; j < 32 ; j ++)
		{
			tmp(j*16+15,j*16) = src[j][i];
		}
		dst1.write(tmp);
		dst2.write(tmp);
	}
}


U4 get_ace_log2(U32 scan_pos)
{
//#pragma HLS INLINE
	U4 ace_log2 = 0;
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
	return ace_log2;
}

/////////////////////quant nnz
void level_core_32(U8 cu_width,U8 cu_height,u32 level[24], u32 level1[24], U5 t0[32], U7 i, int max_num)
{
  //U8 cu_width = 32;
  //U8 cu_height = 32; 
	//int max_pos = 63;
	int tmp = 0;
	int pos = 0;
	for (pos = 0; pos < 32; pos++)
	{
		if (pos < max_num)
		{
			tmp += !(t0[pos] - i);
		}
	}
	level[i] += tmp;
	level1[i + 1] += 8 * tmp;
}
void run_core_32(U8 cu_width, U8 cu_height, u32 run[24], u32 run1[24], U5 t0[32], U7 i, U1 mat_run[32], int max_num)
{
	int tmp = 0;
	int tmp2 = 0;
	int pos = 0;
	//int max_pos = cu_width <= cu_height ? 63 : 64;
	for (pos = 0; pos < 32; pos++)
	{
		if (pos < max_num)
		{
			tmp += !(t0[pos] - i);
			tmp2 += mat_run[pos];
		}
	}
	run[i] += tmp;
	run1[i + 1] += tmp2;
}
void last1_core_32(U8 cu_width, U8 cu_height, u32 last1[12], U4 t0[32], U7 i, int max_num)
{
	int tmp = 0;
	int pos = 0;
	//int max_pos = cu_width <= cu_height ? 63 : 64;
	for (pos = 0; pos < 32; pos++)
	{
		if (pos < max_num)
		{
			tmp += !(t0[pos] - i);
		}
	}
  //printf("tmp = %x\n",tmp);
	last1[i] += tmp;
}
void last2_core_32(U8 cu_width, U8 cu_height, u32 last2[22], U5 t0[32], U7 i, int max_num)
{
	int tmp = 0;
	int pos = 0;
	//int max_pos = cu_width <= cu_height ? 63 : 64;
	for (pos = 0; pos < 32; pos++)
	{
		if (pos < max_num)
		{
			tmp += !(t0[pos] - i);
		}
	}
	last2[i] += tmp;
}

int enc_eco_abs_mvd_est(U16 sym)
{
	U5 enc_est = 0;
	U5 exp_golomb_order = 0;
	// 0, 1, 2
	if (sym < (3)) {
		if (sym == (0)) {
			enc_est++;
		}
		else if (sym == (1)) {
			enc_est += 2;
		}
		else if (sym == (2)) {
			enc_est += 3;
		}
	}
	else {
		U1 offset;
		sym -= (3);
		offset = ((sym & (1)));
		enc_est += 3;
		enc_est++;
		sym = (sym - (offset)) >> 1;
		// exp_golomb part
		if (sym == (0)) {
			exp_golomb_order = 0;
		}
		else if (sym >= (1) && sym < (3)) {
			exp_golomb_order = 2;
		}
		else if (sym >= (3) && sym < (7)) {
			exp_golomb_order = 4;
		}
		else if (sym >= (7) && sym < (15)) {
			exp_golomb_order = 6;
		}
		else if (sym >= (15) && sym < (31)) {
			exp_golomb_order = 8;
		}
		else if (sym >= (31) && sym < (63)) {
			exp_golomb_order = 10;
		}
		else if (sym >= (63) && sym < (127)) {
			exp_golomb_order = 12;
		}
		else if (sym >= (127) && sym < (255)) {
			exp_golomb_order = 14;
		}
		else if (sym >= (255) && sym < (511)) {
			exp_golomb_order = 16;
		}
		else if (sym >= (512) && sym < (1023)) {
			exp_golomb_order = 18;
		}
		else //if (sym >= (1023) && sym < (2047)) 
		{
			exp_golomb_order = 20;
		}
		enc_est += exp_golomb_order;
	}
	return enc_est;
}

int encode_intra_dir_est(u8 ipm, u8 mpm[2])
{
	U3 enc_est = 0;
	S8 ipm_code = (ipm) == (mpm[0]) ? -2 : (((mpm[1]) == (ipm) ? -1 : (((ipm) < (mpm[0]) ? (ipm) : (((ipm) < (mpm[1]) ? (ipm)-1 : (ipm)-2))))));
	if (ipm_code < 0) {
		enc_est += 2;
	}
	else {
		enc_est += 6;
	}
	return enc_est;
}

int encode_mvd_est(S16 mvd[2])
{
	U16 mv;
	U5 enc_est = 0;

	mv = (mvd[0]);
	if ((mvd[0]) < 0) {

		mv = ((-(mvd[0])));
	}
	enc_est += enc_eco_abs_mvd_est(mv);
	if (mv) {
		enc_est++;
	}

	mv = (mvd[1]);
	if ((mvd[1]) < 0) {

		mv = ((-(mvd[1])));
	}
	enc_est += enc_eco_abs_mvd_est(mv);
	if (mv) {
		enc_est++;
	}
	return enc_est;
}

int enc_eco_cbf_est_32(RDOQ_MODEL* model_cnt, int num_nz[4][3], u8 pred_mode,
	S8 ipm[4][2], U2 tree_status, U3 cu_width_log2, U3 cu_height_log2)
{
	U4 enc_est = 0;
	//COM_MODE *mod_info_curr = &core->mod_info_curr;
	U1 ctp_zero_flag = !is_cu_nz(num_nz);
	/* code allcbf */
	if ((pred_mode) != 0) {
		if (!((pred_mode) == 3)) {
			if (tree_status == TREE_LC) {
				enc_est++;
				if (ctp_zero_flag) {
					return 0;
				}
			}
		}

		if (tree_status == TREE_LC) {
			enc_est += 2;
			model_cnt->cbf[1]++;
		}
		if (num_nz[0][1] + num_nz[0][2] == 0 && tree_status == TREE_LC) {
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

int encode_umve_idx_est(u8 umve_idx)
{
	U3 enc_est;
	enc_est = 0;
	if (umve_idx < 20)
		enc_est++;
	else
	{
		enc_est += 2;
	}
	if (umve_idx < 4 || (umve_idx >= 20 && umve_idx < 24))
		enc_est++;
	else
	{
		enc_est += 2;
	}

	enc_est += 2;
	return enc_est;
}

int encode_skip_idx_cu_est(u8  skip_idx, u8 num_hmvp_cands, U2 slice_type)
{

	U8 enc_est = 0;
	U8 val;
	U8 max_skip_num;
	// for P slice, change 3, 4, ..., 13 to 1, 2, ..., 11
	if ((slice_type) == 2 && skip_idx > 0) {
		skip_idx -= 2;
	}
	val = skip_idx;
	max_skip_num = (((slice_type) == 2 ? 2 : (PRED_DIR_NUM)+1)) + num_hmvp_cands;
	enc_est += val;
	if (skip_idx != max_skip_num - 1) {
		enc_est++;
	}
	return enc_est;
}

int encode_affine_flag_est(U1 flag, u8 affine_enable_flag, U3 cu_width_log2, U3 cu_height_log2)
{
	U1 enc_est = 0;
	if (cu_width_log2 >= 4 && cu_height_log2 >= 4) {
		enc_est++;
	}
	return enc_est;
}

int sbac_write_truncate_unary_sym_est(U8 sym, U8 max_num)
{

//#pragma HLS INLINE
	U8 enc_est;
	enc_est = (max_num - (1) > sym + (1) ? sym + (1) : max_num - (1));
	return enc_est;
}


int enc_bit_est_flag_32(RDOQ_MODEL* model_cnt, U2 slice_type, u8 curr_mvr, U2 tree_status, U2 cons_pred_mode, U2 cu_mode, u8 umve_flag, u8 umve_idx, u8 affine_flag, unsigned char affine_enable_flag, u8 skip_idx, S3 refi[REFP_NUM], u8 mvp_from_hmvp_flag, u8 smvd_flag, s16  affine_mvd[REFP_NUM][VER_NUM][MV_D], s16  mvd[REFP_NUM][MV_D], U3 cu_width_log2, U3 cu_height_log2, int num_nz[MAX_NUM_TB][N_C], S8   ipm[MAX_NUM_PB][2], int cur_ptr, int ref_ptr, u8 num_of_hmvp_cand, u8 mpm[4][2], U1 intra)
{
	U8 enc_est = 0;
	if (intra) {
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
	else {
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
			enc_est += encode_affine_flag_est(((affine_flag)) != 0, 1, cu_width_log2, cu_height_log2);
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
				if ((smvd_enable_flag) && refi0 >= 0 && refi1 >= 0 && cur_ptr - 0 == 0 - cur_ptr && (affine_flag) == 0 && !(mvp_from_hmvp_flag)) {
					enc_est++;
				}
				if ((slice_type == 2 || slice_type == 3) && refi0 >= 0) {
					//if ((smvd_flag) == 0)
						/*if ((num_refp) > 1) {
							enc_est += sbac_write_truncate_unary_sym_est(refi0, num_refp);
						}*/
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
			enc_est += enc_eco_cbf_est_32(model_cnt, num_nz, cu_mode, ipm, tree_status, cu_width_log2, cu_height_log2);
		}
		else if (!((cu_mode) == 0 && (ipm[0][0]) == 33
			&& (ipm[0][1]) == 0)) {
			enc_est += 2;
			model_cnt->cbf[1]++;
		}
	}

	return enc_est;
}

int enc_eco_run_est_2_only(U10 sym)
{

	U5 exp_golomb_order = 0;
	U6 enc_est = 0;

	if (sym == 65) {
		enc_est = 1;
	}
	else if (sym == 0) {
		enc_est = 0;
	}
	else if (sym < (16)) {
		enc_est += sym + 1;
	}
	else {
		sym -= (16);
		enc_est += 16;
		if (sym == (0)) {
			exp_golomb_order = 0;
		}
		else if (sym >= (1) && sym < (3)) {
			exp_golomb_order = 2;
		}
		else if (sym >= (3) && sym < (7)) {
			exp_golomb_order = 4;
		}
		else if (sym >= (7) && sym < (15)) {
			exp_golomb_order = 6;
		}
		else if (sym >= (15) && sym < (31)) {
			exp_golomb_order = 8;
		}
		else if (sym >= (31) && sym < (63)) {
			exp_golomb_order = 10;
		}
		else if (sym >= (63) && sym < (127)) {
			exp_golomb_order = 12;
		}
		else if (sym >= (127) && sym < (255)) {
			exp_golomb_order = 14;
		}
		else if (sym >= (255) && sym < (511)) {
			exp_golomb_order = 16;
		}
		else if (sym >= (512) && sym < (1023)) {
			exp_golomb_order = 18;
		}
		else if (sym >= (1023) && sym < (2047)) {
			exp_golomb_order = 20;
		}
		else if (sym >= (2047) && sym < (4095)) {
			exp_golomb_order = 22;
		}
		enc_est += exp_golomb_order;
	}
	return enc_est;
}

int rate_coef_run_est_32(U10 run[32], u8 w)
{
	U12 enc_est = 0;
	U6 i;

	for (i = 0; i < 32; i++)
	{
		if (i < w)
		{
			enc_est += enc_eco_run_est_2_only(run[i]);
		}
	}
	return enc_est;
}

int enc_eco_coef_32_wh_run(s16 coef[32][32]) {
  U8 cu_width = 32; 
  U8 cu_height = 32; 
  U3 log2_w = 5;
  U3 log2_h = 5;
	U15 enc_est = 0;
	U10 run[32];
  //printf("new enc_eco_coef_32_wh_run\n");
#if 0
	//	FUNC_NAME(ready_coef, W, H)(coef_in, coef);
	for (U7 z = 0; z < 32; z++) {
		run[z] = 0;
	}
	//	run_end[0] = 0;
	U10 run_num = 0;
	S7 j;
	for (U7 i = 0; i < cu_width; i++)
	{
		if (i & 0x1)
		{
			for (j = 0; j < 32; j++)
			{
				if (j <= i)
				{
					if (coef[(j << log2_w) + i - j]) {
						if (!run_num) {
							run[j] = 65;
						}
						else {
							run[j] = run_num;
						}
						run_num = 0;
					}
					else
					{
						run[j] = 0;
						run_num++;
					}
				}
			}
		}
		else
		{
			for (j = 32; j >= 0; j--)
			{
				if (j <= i)
				{
					if (coef[(j << log2_w) + i - j]) {
						if (!run_num) {
							run[i - j] = 65;
						}
						else {
							run[i - j] = run_num;
						}
						run_num = 0;
					}
					else
					{
						run[i - j] = 0;
						run_num++;
					}
				}
			}
		}

		enc_est += rate_coef_run_est_32(run, cu_width);
	}

	return enc_est;
#else
	int final_rate = 0;
	int last_x[32];  // the last non-zero position of scan-line.
#pragma HLS ARRAY_PARTITION variable=last_x complete dim=1
	int last_y[32];
#pragma HLS ARRAY_PARTITION variable=last_y complete dim=1
	int first_x[32]; // the first non-zero position of scan-line.
#pragma HLS ARRAY_PARTITION variable=first_x complete dim=1
	int first_y[32];
#pragma HLS ARRAY_PARTITION variable=first_y complete dim=1
	int sumRate[32];

	int final_last_x = -1, final_last_y = -1;
	int final_zig_zag = 0;

	//memset(last_x, -1, sizeof(int) * 32);
	//memset(last_y, -1, sizeof(int) * 32);

	//memset(first_x, -1, sizeof(int) * 32);
	//memset(first_y, -1, sizeof(int) * 32);

	for(int j = 0 ; j < 32 ; j ++){
#pragma HLS UNROLL
		last_x[j] = -1;
		last_y[j] = -1;
		first_x[j] = -1;
		first_y[j] = -1;
		sumRate[j] = 0;
	}

	for (int j = 0; j < 32; j++) // vertical
	{
#pragma HLS PIPELINE
		int zzz = 0;
		for (int i = 0; i < 32; i++) // horizontal
		{
			if (j == 0) // first input row data.
			{
				if (coef[i][j])
				{
					last_x[i] = i;
					last_y[i] = j;

					first_x[i] = i;
					first_y[i] = j;
				}
			}
			else
			{
				if ((i + j) % 2 == 0) // above-right-scan
				{
					if (first_x[i] == -1 && coef[i][j])
					{
						first_x[i] = i;
						first_y[i] = j;
					}

					if (coef[i][j])
					{
						if (last_x[i] != -1)
						{
							int offset = last_x[i] - i;
							if (offset == 1) // previou is non-zero coeff. 
							{
								sumRate[i] += 1;
							}
							else
							{
								sumRate[i] += enc_eco_run_est_2_only(offset - 1); // rate_coef_run_est_32(offset, cu_width);
							}
						}
						last_x[i] = i;
						last_y[i] = j;
					}
				}
				else // above - top scan for (i+j)%2 is even case
				{
					if (coef[i][j] && last_x[i] == -1) // first non-zero position
					{
						last_x[i] = i;
						last_y[i] = j;
					}

					if (coef[i][j] && first_x[i] != -1)
					{
						int offset = first_x[i] - i;
						if (offset == 1) // previou is non-zero coeff. 
						{
							sumRate[i] += 1;
						}
						else
						{
							// should change here.
							sumRate[i] += enc_eco_run_est_2_only(offset - 1); // rate_coef_run_est_32(offset, cu_width);
						}
					}

					if (coef[i][j])
					{
						first_x[i] = i;
						first_y[i] = j;
					}
				}
			}
		}

		// calculate the rate of the last coeff.
		int cur_zig_zag_num;
		if (last_x[0] == -1) // all zeros for current scan-line.
		{
			cur_zig_zag_num = j + 1;
			final_zig_zag += cur_zig_zag_num;
		}
		else
		{
			if (j % 2 == 1) // bottom -left scan
			{
				cur_zig_zag_num = first_x[0];
			}
			else
			{
				cur_zig_zag_num = j - first_x[0];
			}

			final_zig_zag += cur_zig_zag_num;

			// rate _cal:
			// get(final_zig_zag); 
			if (final_zig_zag == 0) // only one non-zero coeff.
			{
				final_rate += 1;
			}
			else
			{
				final_rate += enc_eco_run_est_2_only(final_zig_zag);
			}

			if (j % 2 == 1) // bottom -left scan
			{
				final_zig_zag = j - last_x[0]; // update
			}
			else
			{
				final_zig_zag = j - last_y[0]; // update
			}
			final_rate += sumRate[0];
		}


		// shift register.
		for (int i = 1; i < 32; i++)
		{
			last_x[i - 1] = last_x[i];
			last_y[i - 1] = last_y[i];

			first_x[i - 1] = first_x[i];
			first_y[i - 1] = first_y[i];

			sumRate[i - 1] = sumRate[i];
		}
	}
	return final_rate;
#endif
}

int enc_eco_level_est_2_only(u16 sym)
{

	U5 exp_golomb_order = 0;
	U6 enc_est = 0;

	if (sym == 0)
		enc_est = 0;
	else if (sym < (9)) {
		enc_est = sym;
		enc_est += 2;
	}
	else {
		sym -= (9);
		enc_est = 10;
		if (sym == (0)) {
			exp_golomb_order = 0;
		}
		else if (sym >= (1) && sym < (3)) {
			exp_golomb_order = 2;
		}
		else if (sym >= (3) && sym < (7)) {
			exp_golomb_order = 4;
		}
		else if (sym >= (7) && sym < (15)) {
			exp_golomb_order = 6;
		}
		else if (sym >= (15) && sym < (31)) {
			exp_golomb_order = 8;
		}
		else if (sym >= (31) && sym < (63)) {
			exp_golomb_order = 10;
		}
		else if (sym >= (63) && sym < (127)) {
			exp_golomb_order = 12;
		}
		else if (sym >= (127) && sym < (255)) {
			exp_golomb_order = 14;
		}
		else if (sym >= (255) && sym < (511)) {
			exp_golomb_order = 16;
		}
		else if (sym >= (512) && sym < (1023)) {
			exp_golomb_order = 18;
		}
		else if (sym >= (1023) && sym < (2047)) {
			exp_golomb_order = 20;
		}
		else if (sym >= (2047) && sym < (4095)) {
			exp_golomb_order = 22;
		}
		enc_est += exp_golomb_order;
	}
	return enc_est;
}

int rate_coef_level_est_32(s16 coef[32], u8 w)
{

	U12 enc_est = 0;
	U6 i;

	for (i = 0; i < 32; i++)
	{
#pragma HLS UNROLL
		if (i < w)
		{
			u16  level;
			level = abs(coef[i]);
			enc_est += enc_eco_level_est_2_only(level);
		}
	}
	return enc_est;
}

int enc_eco_coef_32_wh_level(s16 coef[32][32])
{
	  U8 cu_width = 32 ;
	  U8 cu_height = 32;
	  U3 log2_w = 5;
	  U3 log2_h = 5;
#if 1

	U15 enc_est = 0;
	s16 coef_out[32];


	s16 tmp[32] ;
#pragma HLS ARRAY_PARTITION variable=tmp complete dim=1


	//	FUNC_NAME(ready_coef, W, H)(coef_in, coef);
	//for (U7 z = 0; z < 32; z++) {
	//	coef_out[z] = 0;
	//}
	//	run_end[0] = 0;
	U10 run_num = 0;
	S7 j;
	for (U7 i = 0; i < 32; i++)
	{
#pragma HLS PIPELINE
		for(int j = 0 ; j < 32 ; j ++){
			tmp[j] = ((i+j)<32) ? coef[j][i] : (s16) 0;
		}
		enc_est += rate_coef_level_est_32(tmp, cu_width);
	}

	return enc_est;
#else
	U15 enc_est = 0;
	s16 coef_out[32];
	//s16 coef[H][W];
//#pragma HLS ARRAY_PARTITION variable=coef_out complete dim=1
//#pragma HLS ARRAY_PARTITION variable=run complete dim=1

//#pragma HLS ARRAY_PARTITION variable=coef complete dim=1
	//	FUNC_NAME(ready_coef, W, H)(coef_in, coef);
	for (U7 z = 0; z < 32; z++) {
//#pragma HLS UNROLL
		coef_out[z] = 0;
	}
	//	run_end[0] = 0;
	U10 run_num = 0;
	S7 j;
	for (U7 i = 0; i < cu_width; i++)
	{
//#pragma HLS LOOP_TRIPCOUNT max=32
//#pragma HLS PIPELINE II=1
		if (i & 0x1)
		{
			for (j = 0; j < 32; j++)
			{
#pragma hls unroll
				if (j <= i)
				{
					//coef_out[j] = coef[(j << log2_w) + i - j];
					coef_out[j] = coef[i][j];
				}
			}
		}
		else
		{
			for (j = 32; j >= 0; j--)
			{
#pragma hls unroll
				if (j <= i)
				{
					//coef_out[i - j] = coef[(j << log2_w) + i - j];
					coef_out[j] = coef[i][j];
				}
			}
		}

		enc_est += rate_coef_level_est_32(coef_out, cu_width);
	}

	return enc_est;

#endif
}

int enc_eco_coef_32_wh_new(U8 cu_width, U8 cu_height, s16 coef[32][32],s16 coef_run[32][32], U3 log2_w, U3 log2_h)
{
	U15 enc_est_0 = 0;
	U15 enc_est_1 = 0;
	U15 enc_est = 0;
    //s16 tmp_coef[32][32];
	enc_est_0 = enc_eco_coef_32_wh_run(coef);
	enc_est_1 = enc_eco_coef_32_wh_level(coef_run);
	enc_est = enc_est_0 + enc_est_1;
	return enc_est;
}

int enc_eco_cbf_est_skip_32(RDOQ_MODEL* model_cnt, U1 tb_avaliable, u8 pred_mode, S8 ipm[4][2], U2 tree_status, U3 cu_width_log2, U3 cu_height_log2)
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

int enc_bit_est_flag_skip_32(RDOQ_MODEL* model_cnt, U2 slice_type, U2 tree_status, u8 umve_flag, u8 umve_idx, u8 affine_flag, u8 skip_idx, U3 cu_width_log2, U3 cu_height_log2, S8   ipm[MAX_NUM_PB][2], u8 num_of_hmvp_cand)
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
		enc_est += enc_eco_cbf_est_skip_32(model_cnt, 0, 2, ipm, tree_status, cu_width_log2, cu_height_log2);
	}
	else if (!((2) == 0 && (ipm[0][0]) == 33
		&& (ipm[0][1]) == 0)) {
		enc_est += 2;
		model_cnt->cbf[1]++;

	}
	return enc_est;
}


void rate_est_32(
  RDOQ_MODEL* model_cnt, 
  MD_KERNEL_MODE_32* mod_info_curr_ptr, 
  //u8 mode_type,
  //u8* mode_type_update,
  //u8 curr_mvr,
  //u8* curr_mvr_update,
  u8 tree_status, 
  u8 cons_pred_mode, 
  s16 coef_y_pb_part[32][32],
  s16 coef_y_pb_part_run[32][32],
  s64 lambda_y,
  //int is_intra_rate,
 // U1* is_intra_update,
  s64 flag_cost[3],
  int num_nz_rate[MAX_NUM_TB][N_C], 
  MD_KERNEL_INPUT_32* md_kernel_input_32)
{

#if 0
	U16 bit_cnt = 0;
	for(int i = 0 ; i < 32 ; i ++){
#pragma HLS PIPELINE
		for(int j = 0 ; j < 32 ; j++)
		{
			if(coef_y_pb_part[j][i] > coef_y_pb_part_run[j][i])
				bit_cnt++;
		}
	}
	flag_cost[0] = (bit_cnt * lambda_y + 500) >> 10;
#else
  U8 cu_width = 32;
  U8 cu_height = 32; 
  U2 slice_type = 1; 

	// STREAM BUFFER
	u8 mode_type_read ;
	u8 curr_mvr_read ;
	U1 is_intra_read = 1;
#if 0
	if (is_intra_read == 0) {
		if (mode_type_read == 1 || mode_type_read == 0)
		{
			int num_nz_fz[MAX_NUM_TB][N_C];
			cu_nz_cln(num_nz_fz);
			mod_info_curr_ptr->tb_part = ((int)SIZE_2Nx2N);
			if (mode_type == 0)
				mod_info_curr_ptr->cu_mode = 3;
			else {
				flag_cost[2] = 0;
				int bit_cnt = enc_bit_est_flag_32(model_cnt, 2, curr_mvr, tree_status, cons_pred_mode, 
					mod_info_curr_ptr->cu_mode, mod_info_curr_ptr->umve_flag, mod_info_curr_ptr->umve_idx,
					mod_info_curr_ptr->affine_flag, 1, mod_info_curr_ptr->skip_idx, mod_info_curr_ptr->refi,
					0, mod_info_curr_ptr->smvd_flag, mod_info_curr_ptr->affine_mvd,
					mod_info_curr_ptr->mvd, md_kernel_input_32->cu_width_log2, md_kernel_input_32->cu_height_log2,
					num_nz_fz, mod_info_curr_ptr->ipm, 1, 0, 8, mod_info_curr_ptr->mpm, is_intra_read);

				flag_cost[2] += (bit_cnt * lambda_y + 500) >> 10;
			}
		}
	}
#endif

  int flag_est_0 = 0;
  RDOQ_MODEL model_cnt_tmp0;
	//flag_cost[0] = 0;
	flag_est_0 = md_kernel_32_hls::enc_bit_est_flag_32(&model_cnt_tmp0, slice_type, curr_mvr_read, tree_status, cons_pred_mode,
			0, mod_info_curr_ptr->umve_flag, mod_info_curr_ptr->umve_idx, mod_info_curr_ptr->affine_flag,
			1, mod_info_curr_ptr->skip_idx, mod_info_curr_ptr->refi, 0, mod_info_curr_ptr->smvd_flag,
			mod_info_curr_ptr->affine_mvd, mod_info_curr_ptr->mvd, md_kernel_input_32->cu_width_log2, md_kernel_input_32->cu_height_log2,
			num_nz_rate, mod_info_curr_ptr->ipm, 1, 0, 8, mod_info_curr_ptr->mpm, is_intra_read);
	U16 coef_est;

	coef_est = md_kernel_32_hls::enc_eco_coef_32_wh_new(cu_width, cu_height, coef_y_pb_part,coef_y_pb_part_run,
			md_kernel_input_32->cu_width_log2, md_kernel_input_32->cu_height_log2);

	U16 bit_cnt = flag_est_0 + coef_est;
	flag_cost[0] = (bit_cnt * lambda_y + 500) >> 10;
#if 0
	if (is_intra_read == 0) {
		if (mode_type_read == 0) {
			flag_cost[1] = 0;
			flag_est = enc_bit_est_flag_skip_32(model_cnt, slice_type, tree_status, mod_info_curr_ptr->umve_flag, mod_info_curr_ptr->umve_idx, mod_info_curr_ptr->affine_flag, mod_info_curr_ptr->skip_idx, md_kernel_input_32->cu_width_log2, md_kernel_input_32->cu_height_log2, mod_info_curr_ptr->ipm, 8);
			flag_cost[1] += (flag_est * lambda_y + 500) >> 10;
		}
	}

	// WRITE STREAMS
	*curr_mvr_update = curr_mvr_read;
	*mode_type_update = mode_type_read;
#endif
	//*is_intra_update = is_intra_read;
#endif
}

void pre_quant_32_s(
  hls::stream<s512_h> &src_coef,
  U20 q_value,
  S32 q_bits,
  hls::stream<s512_h> &tmp_coef,
  hls::stream<s512_h> &tmp_coef1,
  hls::stream<u1152_h> &tmp_level_double
)
{

	ap_int<512> in;
	ap_int<512> out1;
	ap_int<1152> out2;

	ap_int<16> tmp_1[32];
#pragma HLS ARRAY_PARTITION variable=tmp_1 complete dim=0
	ap_int<16> tmp_2[32];
#pragma HLS ARRAY_PARTITION variable=tmp_2 complete dim=0
	ap_uint<36> tmp_3[32];
#pragma HLS ARRAY_PARTITION variable=tmp_3 complete dim=0

	for (int j = 0; j < 32; j++) // vertical
  {
#pragma HLS PIPELINE
		in = src_coef.read();
		for(int i = 0 ; i < 32 ; i ++)
		{
			 tmp_1[i] = in(i*16+15,i*16);
       //fprintf(file1_out_1,"%x\n",tmp_1[i].VAL);
		}

		for (int i = 0; i < 32; i++)
		{
				S25 max_abs_level;
				U1 lower_int;
				U36 temp_level;
				temp_level = (U36)abs(tmp_1[i]) * q_value;
				U36 level_double = (U36)COM_MIN((temp_level), (U36)(COM_INT32_MAX - (1 << (q_bits - 1))));
				tmp_3[i] = level_double;
				max_abs_level = (S32)(level_double >> q_bits);
				lower_int = ((level_double - (max_abs_level << q_bits)) < (1 << (q_bits - 1))) ? 1 : 0;
				if (!lower_int)
				{
					max_abs_level++;
				}
				tmp_2[i] = tmp_1[i] > 0 ? (S16)max_abs_level : (S16)(-max_abs_level);

		}

    for(int i = 0 ; i < 32 ; i ++)
		{
			  out1(i*16+15,i*16) = tmp_2[i];
			  out2(i*36+35,i*36) = tmp_3[i];
        //fprintf(file1_out_2,"%x\n",tmp_2[i].VAL);
        //fprintf(file1_out_3,"%x\n",tmp_3[i].VAL);
		}

		tmp_coef.write(out1);
		tmp_coef1.write(out1);
		tmp_level_double.write(out2);

    

  }
}

void doc_stage1_s(
		hls::stream<s512_h>  &tmp_coef,
		hls::stream<u768_h> &mat_prev_level_1,
		hls::stream<u768_h> &mat_prev_level_2,
		hls::stream<u768_h> &mat_prev_level_3,
		hls::stream<u32_h>  &mat_run_1,
		hls::stream<u32_h>  &mat_run_2
		)
{


	ap_int<512>  in;
	ap_uint<768> out1;
	ap_uint<32>  out2;

	ap_int<16>  tmp_1[32];
#pragma HLS ARRAY_PARTITION variable=tmp_1 complete dim=0
	ap_uint<24> tmp_2[32];
#pragma HLS ARRAY_PARTITION variable=tmp_2 complete dim=0
	ap_uint<1>  tmp_3[32];
#pragma HLS ARRAY_PARTITION variable=tmp_3 complete dim=0

	for (int j = 0; j < 32; j++) // horizontal
	{
#pragma HLS PIPELINE
		in = tmp_coef.read();
		for(int i = 0 ; i < 32 ; i ++)
		{
			 tmp_1[i] = in(i*16+15,i*16);
		}

		for (int i = 0; i < 32; i++)
		{
			//U32 blk_pos = scan[scan_pos - 1];
				if (tmp_1[i])
				{
					tmp_2[i] = abs(tmp_1[i]);
					//tmp = abs(tmp_coef[blk_pos]);
					tmp_3[i] = 0;
				}
				else
				{
					tmp_2[i] = 1;
					tmp_3[i] = 1;
				}
		}

		for(int i = 0 ; i < 32 ; i ++)
		{
			  out1(i*24+23,i*24) = tmp_2[i];
			  out2(i,i) = tmp_3[i];

        //fprintf(file1_out_4,"%x\n",tmp_2[i].VAL);
        //fprintf(file1_out_5,"%x\n",tmp_3[i].VAL);
		}
		mat_prev_level_1.write(out1);
		mat_prev_level_2.write(out1);
		mat_prev_level_3.write(out1);
		mat_run_1.write(out2);
		mat_run_2.write(out2);
	}
}

void doc_stage2_s(
	hls::stream<s512_h> &tmp_coef,
    hls::stream<u1152_h> &tmp_level_double,
    hls::stream<u768_h> &mat_prev_level,
    hls::stream<u32_h> &mat_run,
	int ch_type,
    S32 q_bits,
    S64 err_scale,
    S64 lambda,
    S32 rdoq_est_run_local[24][2],
    S32 rdoq_est_level_local[24][2],
	hls::stream<s512_h> &tmp_dst_coef_out,
    hls::stream<s2048_h> &mat_coded_cost,
    hls::stream<s2048_h> &mat_uncoded_cost,
    hls::stream<u768_h> &lev_opt
	)
{
#pragma HLS ARRAY_PARTITION variable=rdoq_est_run_local complete dim=0
#pragma HLS ARRAY_PARTITION variable=rdoq_est_level_local complete dim=0

	ap_int<512>   in1;
	ap_uint<1152> in2;
	ap_uint<768>  in3;
	ap_uint<32>	  in4;

	ap_int<512>  out1;
	ap_int<2048> out2;
	ap_int<2048> out3;
	ap_uint<768> out4;

	ap_int<16>  tmp_1[32];
#pragma HLS ARRAY_PARTITION variable=tmp_1 complete dim=0
	ap_uint<36> tmp_2[32];
#pragma HLS ARRAY_PARTITION variable=tmp_2 complete dim=0
	ap_uint<24>	tmp_3[32];
#pragma HLS ARRAY_PARTITION variable=tmp_3 complete dim=0
	ap_uint<1>  tmp_4[32];
#pragma HLS ARRAY_PARTITION variable=tmp_4 complete dim=0

	ap_int<16> tmp_5[32];
#pragma HLS ARRAY_PARTITION variable=tmp_5 complete dim=0
	ap_int<64> tmp_6[32];
#pragma HLS ARRAY_PARTITION variable=tmp_6 complete dim=0
	ap_int<64>	tmp_7[32];
#pragma HLS ARRAY_PARTITION variable=tmp_7 complete dim=0
	ap_uint<24> tmp_8[32];
#pragma HLS ARRAY_PARTITION variable=tmp_8 complete dim=0

	for (int j = 0; j < 32; j++) // horizontal
	{
	#pragma HLS PIPELINE
		in1 = tmp_coef.read();
		in2 = tmp_level_double.read();
		in3 = mat_prev_level.read();
		in4 = mat_run.read();

		for(int i = 0 ; i < 32 ; i ++)
		{
			 tmp_1[i] = in1(i*16+15,i*16);
			 tmp_2[i] = in2(i*36+35,i*36);
			 tmp_3[i] = in3(i*24+23,i*24);
			 tmp_4[i] = in4(i,i);
		}

		for (int i = 0; i < 32; i++)
		{

			S16 tmp_dst_coef;
			s64 d64_uncoded_cost;
			s64 d64_coded_cost;
			U24 level;
			U5 ctx_run = ((COM_MIN((U24)(tmp_3[i] - 1), (U24)(5))) << 1) + (ch_type == Y_C ? 0 : 12);
			int iflast = (i * 32 + j == 1023);
				level = get_coded_level_rl(&d64_uncoded_cost, &d64_coded_cost, tmp_2[i], abs(tmp_1[i]),
						tmp_4[i], ctx_run, ctx_run, q_bits, err_scale, lambda, iflast, rdoq_est_run_local, rdoq_est_level_local);
			S17 level_cbf = level;
			tmp_dst_coef = (S16)(tmp_1[i] < 0 ? (S16)(-level_cbf) : (S16)level_cbf);

			tmp_5[i] = tmp_dst_coef;
			tmp_6[i] = d64_coded_cost;
			tmp_7[i] = d64_uncoded_cost;
			tmp_8[i] = level;
		}

		for(int i = 0 ; i < 32 ; i ++)
		{
			 out1(i*16+15,i*16) = tmp_5[i];
			 out2(i*64+63,i*64) = tmp_6[i];
			 out3(i*64+63,i*64) = tmp_7[i];
			 out4(i*24+23,i*24) = tmp_8[i];
		}

		tmp_dst_coef_out.write(out1);
		mat_coded_cost.write(out2),
		mat_uncoded_cost.write(out3),
		lev_opt.write(out4);
	}
}

void module_update_s(
    hls::stream<u768_h> &mat_prev_level,
    hls::stream<u32_h> & mat_run,
	int ch_type,
	U32 last1[12],
    U32 last2[22], 
    U32 run[24], 
    U32 level[24])
{

#ifndef __SYNTHESIS__
#pragma HLS ARRAY_PARTITION variable=last1 complete dim=1
#pragma HLS ARRAY_PARTITION variable=last2 complete dim=1
#pragma HLS ARRAY_PARTITION variable=run complete dim=1
#pragma HLS ARRAY_PARTITION variable=level complete dim=1

	U5 t0[32] = {0};
#pragma HLS ARRAY_PARTITION variable=t0 complete dim=1
	U4 last1_pos[32] = {0};
#pragma HLS ARRAY_PARTITION variable=last1_pos complete dim=1
	U5 last2_pos[32] = {0};
#pragma HLS ARRAY_PARTITION variable=last2_pos complete dim=1
	U32 last1_tmp[12];
#pragma HLS ARRAY_PARTITION variable=last1_tmp complete dim=1
	U32 last2_tmp[22];
#pragma HLS ARRAY_PARTITION variable=last2_tmp complete dim=1
	U32 level_tmp[24];
#pragma HLS ARRAY_PARTITION variable=level_tmp complete dim=1
	U32 level_tmp1[24];
#pragma HLS ARRAY_PARTITION variable=level_tmp1 complete dim=1
	U32 run_tmp[24];
#pragma HLS ARRAY_PARTITION variable=run_tmp complete dim=1
	U32 run_tmp1[24];
#pragma HLS ARRAY_PARTITION variable=run_tmp1 complete dim=1
	U1 mat_run_tmp[32];
#pragma HLS ARRAY_PARTITION variable=mat_run_tmp complete dim=1
	int scan_pos;
	U4 ace_log2;
	U10 pos[32];
#pragma HLS ARRAY_PARTITION variable=pos complete dim=1


	ap_uint<768> in1;
	ap_uint<32>	 in2;

	ap_uint<24> tmp_1[32];
#pragma HLS ARRAY_PARTITION variable=tmp_1 complete dim=1
	ap_uint<1>  tmp_2[32];
#pragma HLS ARRAY_PARTITION variable=tmp_2 complete dim=1

	for (int j = 0; j < 32; j++) // horizontal
	{
	#pragma HLS PIPELINE


		in1 = mat_prev_level.read();
		in2 = mat_run.read();

		for(int i = 0 ; i < 32 ; i ++)
		{
			 tmp_1[i] = in1(i*24+23,i*24);
			 tmp_2[i] = in2(i,i);
		}

  
    for(int i = 0 ; i < 12 ; i ++){
	    last1_tmp[i] = 0;
    }

    for(int i = 0 ; i < 22 ; i ++){
	    last2_tmp[i] = 0;
    }

    for(int i = 0 ; i < 24 ; i ++){
	    level_tmp[i] = 0 ;
	    level_tmp1[i] = 0;
	    run_tmp[i] = 0;
	    run_tmp1[i] = 0;
    }
  

		for (int i = 0; i < 32; i++)
		{

			if (i + j <= 31) {
				if ((i + j) % 2 == 1) {
					pos[i] = (i + j + 1)*(i + j) / 2 + i;
				}
				else {
					pos[i] = (i + j + 1)*(i + j) / 2 + j;
				}
				ace_log2 = get_ace_log2(pos[i] + 1);
				t0[i] = ((COM_MIN((U24)(tmp_1[i] - 1), (U24)(5))) << 1) + (ch_type == 0 ? 0 : 12);
				last1_pos[i] = ((COM_MIN((U24)(tmp_1[i] - 1), (U24)(5)))) + (ch_type == 0 ? 0 : 6);
				last2_pos[i] = ace_log2 + (ch_type == 0 ? 0 : 12);
				mat_run_tmp[i] = tmp_2[i];
			}
		}

		for (scan_pos = 0; scan_pos < 24; scan_pos += 2)
		{
	#pragma HLS UNROLL
			level_core_32(32, 32, level_tmp, level_tmp1, t0, scan_pos, 32 - j);
			run_core_32(32, 32, run_tmp, run_tmp1, t0, scan_pos, mat_run_tmp, 32 - j);
		}

		for (scan_pos = 0; scan_pos < 12; scan_pos++)
		{
	#pragma HLS UNROLL
			last1_core_32(32, 32, last1_tmp, last1_pos, scan_pos, 32 - j);
			last1[scan_pos] += last1_tmp[scan_pos];
		}

		for (scan_pos = 0; scan_pos < 22; scan_pos++)
		{
	#pragma HLS UNROLL
			last2_core_32(32, 32, last2_tmp, last2_pos, scan_pos, 32 - j);
			last2[scan_pos] += last2_tmp[scan_pos];
		}

		for (scan_pos = 0; scan_pos < 24; scan_pos++)
		{
	#pragma HLS UNROLL
			level[scan_pos] += level_tmp[scan_pos] + level_tmp1[scan_pos];
			run[scan_pos] += run_tmp[scan_pos] + run_tmp1[scan_pos];
		}
	}
#else
	ap_uint<768> in1;
	ap_uint<32>	 in2;

	for (int j = 0; j < 32; j++) // horizontal
	{
	#pragma HLS PIPELINE
		in1 = mat_prev_level.read();
		in2 = mat_run.read();
	}
#endif
}


void DLPN_s(
  hls::stream<u768_h> &mat_prev_level,
  hls::stream<s2048_h> &mat_coded_cost,
  hls::stream<s2048_h> &mat_uncoded_cost,
  hls::stream<u768_h> &lev_opt,
  S32 rdoq_est_cbf[3][2],
  S32 rdoq_est_last_local[2][6][12][2], 
  U1 ctx_last,
  S64 lambda,
  int* final_x,
  int* final_y)
{
	ap_uint<4> ace_log2 = 0;
	ap_uint<10> pos[32];
#pragma HLS ARRAY_PARTITION variable=pos complete dim=1

	ap_int<64> d64_cost_last_zero[32];
#pragma HLS ARRAY_PARTITION variable=d64_cost_last_zero complete dim=1
	ap_int<64> base_cost_buffer_tmp[32];
#pragma HLS ARRAY_PARTITION variable=base_cost_buffer_tmp complete dim=1
	ap_int<64> d64_cost_last_one_tmp[32];
#pragma HLS ARRAY_PARTITION variable=d64_cost_last_one_tmp complete dim=1

	ap_int<64> rdoqD64LastOne[32];
#pragma HLS ARRAY_PARTITION variable=rdoqD64LastOne complete dim=1
	ap_int<64> tempCost[32];
#pragma HLS ARRAY_PARTITION variable=tempCost complete dim=1
	ap_int<64> endPosCost[32];
#pragma HLS ARRAY_PARTITION variable=endPosCost complete dim=1
	int last_x[32] ;
#pragma HLS ARRAY_PARTITION variable=last_x complete dim=1
	int last_y[32] ;
#pragma HLS ARRAY_PARTITION variable=last_y complete dim=1
	int rdoq_last_x[32] ;
#pragma HLS ARRAY_PARTITION variable=rdoq_last_x complete dim=1
	int rdoq_last_y[32] ;
#pragma HLS ARRAY_PARTITION variable=rdoq_last_y complete dim=1
	int final_last_x = -1, final_last_y = -1;
	int final_rdoq_last_x = -1, final_rdoq_last_y = -1;
	ap_int<64> finalRdoqCost = 0;
	//ap_int<64> d64_best_cost_tmp = 0;



	ap_int<64> d64_best_cost = rdoq_est_cbf[0][0] * lambda;
	ap_int<64> cbf_cost = rdoq_est_cbf[0][1] * lambda;
	d64_best_cost = d64_best_cost - cbf_cost;
	ap_int<64> d64_best_cost_tmp = d64_best_cost;


	ap_uint<768> in1;
	ap_int<2048> in2;
	ap_int<2048> in3;
	ap_uint<768> in4;

	ap_uint<24> tmp_1[32];
#pragma HLS ARRAY_PARTITION variable=tmp_1 complete dim=1
 	ap_int<64>  tmp_2[32];
#pragma HLS ARRAY_PARTITION variable=tmp_2 complete dim=1
	ap_int<64>  tmp_3[32];
#pragma HLS ARRAY_PARTITION variable=tmp_3 complete dim=1
	ap_uint<24> tmp_4[32];
#pragma HLS ARRAY_PARTITION variable=tmp_4 complete dim=1


	for(int i = 0 ; i < 32 ; i++)
	{
#pragma HLS UNROLL
		rdoqD64LastOne[i] = -1; // shifter buffer the d64_cost_last_one_tmp
		last_x[i] = -1;
		last_y[i] = -1;
		rdoq_last_x[i] = -1;
		rdoq_last_y[i] = -1;
	}

	//dlnp prepare
	for (int j = 0; j < 32; j++) // horizontal
	{
	#pragma HLS PIPELINE

		in1 = mat_prev_level.read();
		in2 = mat_coded_cost.read();
		in3 = mat_uncoded_cost.read();
		in4 = lev_opt.read();
		for (int i = 0; i < 32 ; i++) // horizontal
		{
			tmp_1[i] = in1(i*24+23,i*24);
			tmp_2[i] = in2(i*64+63,i*64);
			tmp_3[i] = in3(i*64+63,i*64);
			tmp_4[i] = in4(i*24+23,i*24);
		}


		for (int i = 0; i < 32; i++)
		{
			if (i + j <= 31) {
				if ((i + j) % 2 == 1) {
					pos[i] = (i + j + 1)*(i + j) / 2 + i;
				}
				else {
					pos[i] = (i + j + 1)*(i + j) / 2 + j;
				}
			}

			//U8 blk_pos = scan[scan_pos];
			if (i + j < 32) {
				ace_log2 = get_ace_log2(pos[i]);
				U4 flag = 0;
				if (tmp_1[i] - 1 < 5)
				{
					flag = tmp_1[i] - 1;
				}
				else
				{
					flag = 5;
				}
				d64_cost_last_zero[i] = rdoq_est_last_local[ctx_last][flag][ace_log2][0] * lambda;
			}
		}

		for (int i = 0; i < 32; i++)
		{
			//int blk_pos = scan[scan_pos];
			if (i < 32 - j) {
				if (tmp_4[i])
				{
					base_cost_buffer_tmp[i] = tmp_2[i] - tmp_3[i] + d64_cost_last_zero[i];
				}
				else
				{
					base_cost_buffer_tmp[i] = tmp_2[i] - tmp_3[i];
				}
			}
		}
		for (int i = 0; i < 32; i++)
		{
			if (i < 32 - j) {
				ace_log2 = get_ace_log2(pos[i]);
				U24 prev_level = tmp_1[i];
				U4 flag = 0;
				if (prev_level - 1 < 5)
				{
					flag = prev_level - 1;
				}
				else
				{
					flag = 5;
				}
				d64_cost_last_one_tmp[i] = GET_I_COST(rdoq_est_last_local[ctx_last][flag][ace_log2][1], lambda); // block scan order
			}
		}

		//dlnp core
		for (int i = 0; i < 32; i++) // horizontal
		{
				if (j == 0)
				{
					// tempCost[j] = base_cost_buffer_tmp[j];
					if (tmp_4[i])
					{

						rdoq_last_x[i] = i;
						rdoq_last_y[i] = j;
						rdoqD64LastOne[i] = d64_cost_last_one_tmp[i];
					}
					tempCost[i] = base_cost_buffer_tmp[i];
					if ((i + j) % 2 == 0) // bottom - left scan for (i+j)%2 is odd case
					{
						if (tmp_4[i]) endPosCost[i] = base_cost_buffer_tmp[i];
						else endPosCost[i] = 0;
					}
					else // above - top scan for (i+j)%2 is even case
					{
						if (tmp_4[i]) endPosCost[i] = 0;
						else endPosCost[i] = base_cost_buffer_tmp[i];

						// a special case (0, 0) position.
						if (i == 0)  // substract here is to get the actual endPosCost
						{
							endPosCost[i] = tempCost[i] - endPosCost[i];
						}
					}
				}
				else
				{
					if ((i + j) % 2 == 0) // bottom - left scan for (i+j)%2 is odd case
					{

						if (tmp_4[i] && rdoq_last_y[i] != -1) // compare for intialized case.
						{
							// add the rdoqD64LastOne consider here.
							// compare the best LNP for bottom-right scan
							if (tempCost[i] + base_cost_buffer_tmp[i] + d64_cost_last_one_tmp[i] < endPosCost[i] + rdoqD64LastOne[i])
							{
								rdoq_last_x[i] = i;
								rdoq_last_y[i] = j;
								endPosCost[i] = tempCost[i] + base_cost_buffer_tmp[i];
								rdoqD64LastOne[i] = d64_cost_last_one_tmp[i];
							}
						}
						if (tmp_4[i] && rdoq_last_y[i] == -1) // cost initial. purpose.
						{
							rdoq_last_x[i] = i;
							rdoq_last_y[i] = j;
							endPosCost[i] = tempCost[i] + base_cost_buffer_tmp[i];
							rdoqD64LastOne[i] = d64_cost_last_one_tmp[i];
						}

						tempCost[i] += base_cost_buffer_tmp[i];
					}
					else // above - top scan for (i+j)%2 is even case
					{
						ap_int<64> curCost = base_cost_buffer_tmp[i];  // care the bit-width. not int.


						if (tmp_4[i] && rdoq_last_y[i] != -1) // the previous non-zero position
						{
							// consider d64_cost_last_one_tmp part cost.
							if (tempCost[i] - endPosCost[i] + rdoqD64LastOne[i] - d64_cost_last_one_tmp[i] > 0) // previous position is better
							{
								rdoq_last_x[i] = i;
								rdoq_last_y[i] = j;
								endPosCost[i] = tempCost[i];
								rdoqD64LastOne[i] = d64_cost_last_one_tmp[i];
							}
						}

						if (tmp_4[i] && rdoq_last_y[i] == -1) // first non-zero position
						{
							rdoq_last_x[i] = i;
							rdoq_last_y[i] = j;
							// from start to current position (except current pos.)
							endPosCost[i] = tempCost[i];
							rdoqD64LastOne[i] = d64_cost_last_one_tmp[i];
						}

						tempCost[i] += curCost; // from the start point to current pos cost.

						if (i == 0)  // substract here is to get the actual endPosCost
						{
							endPosCost[i] = tempCost[i] - endPosCost[i];
						}
					}
				}
			}

			// each row assign.

			ap_int<64> temp_RdoqCost = finalRdoqCost + endPosCost[0];

			//int index = rdoq_last_x[0] * 32 + rdoq_last_y[0];

			// cross check.

			if (rdoq_last_y[0] != -1 && temp_RdoqCost + rdoqD64LastOne[0] < d64_best_cost_tmp)
			{
				d64_best_cost_tmp = temp_RdoqCost + rdoqD64LastOne[0];
				final_rdoq_last_x = rdoq_last_x[0];
				final_rdoq_last_y = rdoq_last_y[0];
			}
			finalRdoqCost += tempCost[0];

			// shift register.
			for (int i = 1; i < 32; i++)
			{
				last_x[i - 1] = last_x[i];
				last_y[i - 1] = last_y[i];
				rdoq_last_x[i - 1] = rdoq_last_x[i];
				rdoq_last_y[i - 1] = rdoq_last_y[i];
				tempCost[i - 1] = tempCost[i];
				endPosCost[i - 1] = endPosCost[i];
				rdoqD64LastOne[i - 1] = rdoqD64LastOne[i];
			}
	}


	*final_x = final_rdoq_last_x;
	*final_y = final_rdoq_last_y;
	//*best_last_idx_p1 = final_rdoq_last_x * 8 + final_rdoq_last_y;
}

#if NO_SUB_MODEL_SYN
void rdoq_32_32_hardware_s(
  s16 src_coef[32][32],
  U6 qp,
  s32 q_bits,
  s16 tmp_dst_coef_out[32][32],
  u32 last1[12],
  u32 last2[22],
  u32 run[24],
  u32 level[24],
  s32 rdoq_est_cbf[3][2],
  s32 rdoq_est_run_local[24][2],
  s32 rdoq_est_level_local[24][2],
  s32 rdoq_est_last_local[2][6][12][2],
  U1 is_intra,
  s64 lambda,
  U4 bit_depth,
  int* final_x,
  int* final_y)
{
#pragma HLS DATAFLOW
int ch_type = 0;
U3 cu_width_log2 = 5;
U3 cu_height_log2 = 5;
const U14 scale = quant_scale[qp];
const int ns_shift = ((cu_width_log2 + cu_height_log2) & 1) ? 7 : 0;
const int ns_scale = ((cu_width_log2 + cu_height_log2) & 1) ? 181 : 1;
const int ns_offset = ((cu_width_log2 + cu_height_log2) & 1) ? (1 << (ns_shift - 1)) : 0;
const int q_value = (scale * ns_scale + ns_offset) >> ns_shift;
const int log2_size = (cu_width_log2 + cu_height_log2) >> 1;
const int tr_shift = get_transform_shift(bit_depth, log2_size);
const int ctx_last = (ch_type == Y_C) ? 0 : 1;
s64 err_scale = err_scale_tbl[qp][log2_size - 1];

	hls::stream<s512_h> tmp_coef_stream;
#pragma HLS STREAM variable=tmp_coef_stream depth=4096
	hls::stream<s512_h> tmp_coef_stream_1;
#pragma HLS STREAM variable=tmp_coef_stream_1 depth=4096
	hls::stream<s512_h> tmp_coef_stream_2;
#pragma HLS STREAM variable=tmp_coef_stream_2 depth=4096
	hls::stream<u1152_h> tmp_level_double_stream;
#pragma HLS STREAM variable=tmp_level_double_stream depth=4096

	hls::stream<u768_h> mat_prev_level_1;
#pragma HLS STREAM variable=mat_prev_level_1 depth=4096
	hls::stream<u768_h> mat_prev_level_2;
#pragma HLS STREAM variable=mat_prev_level_2 depth=4096
	hls::stream<u768_h> mat_prev_level_3;
#pragma HLS STREAM variable=mat_prev_level_3 depth=4096


	hls::stream<u32_h> mat_run_1;
#pragma HLS STREAM variable=mat_run_1 depth=4096
	hls::stream<u32_h> mat_run_2;
#pragma HLS STREAM variable=mat_run_2 depth=4096

	hls::stream<s2048_h> mat_coded_cost;
#pragma HLS STREAM variable=mat_coded_cost depth=4096
	hls::stream<s2048_h> mat_uncoded_cost;
#pragma HLS STREAM variable=mat_uncoded_cost depth=4096
	hls::stream<u768_h> lev_opt;
#pragma HLS STREAM variable=lev_opt depth=4096

	hls::stream<s512_h> dst_coef_out;
#pragma HLS STREAM variable=dst_coef_out depth=4096

  //
  //printf("pre_quant_32_s_start\n");
  stage_v_copy(src_coef,tmp_coef_stream);

  pre_quant_32_s(tmp_coef_stream,q_value,q_bits,tmp_coef_stream_1,tmp_coef_stream_2,tmp_level_double_stream);
 	doc_stage1_s(tmp_coef_stream_1,mat_prev_level_1,mat_prev_level_2,mat_prev_level_3,mat_run_1,mat_run_2);
	doc_stage2_s(
			tmp_coef_stream_2,tmp_level_double_stream,mat_prev_level_1,mat_run_1,
			ch_type,q_bits,err_scale,lambda,rdoq_est_run_local,rdoq_est_level_local,
			dst_coef_out,mat_coded_cost,mat_uncoded_cost,lev_opt
		);
  module_update_s(mat_prev_level_2,mat_run_2,ch_type,last1, last2,run,level);
  DLPN_s(mat_prev_level_3,mat_coded_cost,mat_uncoded_cost,lev_opt,
		rdoq_est_cbf,rdoq_est_last_local,ctx_last,lambda,final_x,final_y);

  //printf("final_x = %d final_y = %d\n",*final_x,*final_y);
 	stage_v_copy2(dst_coef_out,tmp_dst_coef_out);
}
#endif

void rdoq_32_32_hardware(S16 src_coef[32][32], U6 qp, S32 q_bits, S16 tmp_dst_coef_out[1024], U32 last1[12], U32 last2[22],
	U32 run[24], U32 level[24],
	S32 rdoq_est_cbf[3][2], S32 rdoq_est_run_local[24][2], S32 rdoq_est_level_local[24][2],
	S32 rdoq_est_last_local[2][6][12][2],  U1 is_intra, S64 lambda, U4 bit_depth,int* final_x,int* final_y)
{
  U3 ch_type = 0;
	U3 cu_width_log2 = 5;
	U3 cu_height_log2 = 5;
	const U14 scale = quant_scale[qp];
	const int ns_shift = ((cu_width_log2 + cu_height_log2) & 1) ? 7 : 0;
	const int ns_scale = ((cu_width_log2 + cu_height_log2) & 1) ? 181 : 1;
	const int ns_offset = ((cu_width_log2 + cu_height_log2) & 1) ? (1 << (ns_shift - 1)) : 0;
	const int q_value = (scale * ns_scale + ns_offset) >> ns_shift;
	const int log2_size = (cu_width_log2 + cu_height_log2) >> 1;
	const int tr_shift = get_transform_shift(bit_depth, log2_size);
	const int ctx_last = (ch_type == Y_C) ? 0 : 1;
	s64 err_scale = err_scale_tbl[qp][log2_size - 1];
	S16 tmp_dst_coef[32];
	U36 tmp_level_double[32];
	S16 tmp_coef[32];
	S64 d64_cost_last_zero[32] = { 0 };
	U24 lev_opt[32];
	S64 mat_coded_cost[32];
	S64 mat_uncoded_cost[32];
	d64_cost_last_zero[0] = 0;
	U4 ace_log2 = 0;
	U32 scan_pos = 0;
	S64 d64_best_cost = 0;
	S64 d64_base_cost[1024] = { 0 };
	S64 base_cost_buffer[1024] = { 0 };
	// added by xfhuang.
	S64 base_cost_buffer_tmp[32] = { 0 };
	S64 d64_best_cost_tmp = 0;
	int best_last_idx_p1_tmp = 0;
	U24 mat_prev_level[32] = { 0 };
	U1 mat_run[32] = { 0 };
	s64 d64_uncoded_cost = 0;
	s64 d64_coded_cost = 0;
	S64 d64_cost_last_one_tmp[32] = { 0 };
	S64 finalRdoqCost = 0;
	S64 tempCost[32];   // all scan line cost
	S64 endPosCost[32]; // as the last position cost.
	S64 rdoqD64LastOne[32] = { -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 }; // shifter buffer the d64_cost_last_one_tmp
	int last_x[32] = { -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 };
	int last_y[32] = { -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 };
	int rdoq_last_x[32] = { -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 };
	int rdoq_last_y[32] = { -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 };
	int final_last_x = -1, final_last_y = -1;
	int final_rdoq_last_x = -1, final_rdoq_last_y = -1;
	U5 t0[32] = { 0 };
	U4 last1_pos[32] = { 0 };
	U5 last2_pos[32] = { 0 };


	if (!is_intra && ch_type == Y_C)
	{
		d64_best_cost = 0;
		d64_best_cost_tmp = 0;
	}
	else
	{
		d64_best_cost = rdoq_est_cbf[ch_type][0] * lambda;
		S64 cbf_cost = rdoq_est_cbf[ch_type][1] * lambda;
		d64_best_cost = d64_best_cost - cbf_cost;
		d64_best_cost_tmp = d64_best_cost;
	}


#if 1

	for (int j = 0; j < 32; j++) // vertical
	{
		U32 last1_tmp[12] = { 0 };
		U32 last2_tmp[22] = { 0 };
		U32 level_tmp[24] = { 0 };
		U32 level_tmp1[24] = { 0 };
		U32 run_tmp[24] = { 0 };
		U32 run_tmp1[24] = { 0 };
		U10 pos[32];
		//pre_quant
		for (int i = 0; i < 32; i++)
		{
				U36 level_double = src_coef[i][j];
				S25 max_abs_level;
				U1 lower_int;
				U36 temp_level;
				temp_level = (U36)abs(src_coef[i][j]) * q_value;
				level_double = (U36)COM_MIN((temp_level), (U36)(COM_INT32_MAX - (1 << (q_bits - 1))));
				tmp_level_double[i] = level_double;
				max_abs_level = (S32)(level_double >> q_bits);
				lower_int = ((level_double - (max_abs_level << q_bits)) < (1 << (q_bits - 1))) ? 1 : 0;
				if (!lower_int)
				{
					max_abs_level++;
				}
				tmp_coef[i] = src_coef[i][j] > 0 ? (S16)max_abs_level : (S16)(-max_abs_level);
		}
		//doc
		for (int i = 0; i < 32; i++)
		{
			//if (i < 32 - j) {
				if (tmp_coef[i])
				{
					mat_prev_level[i] = abs(tmp_coef[i]);
					mat_run[i] = 0;
				}
				else
				{
					mat_prev_level[i] = 1;
					mat_run[i] = 1;
				}
		}

		for (int i = 0; i < 32; i++)
		{
			//if (i < 32 - j) {
				U24 level;
				U5 ctx_run = ((COM_MIN((U24)(mat_prev_level[i] - 1), (U24)(5))) << 1) + (ch_type == Y_C ? 0 : 12);

				int iflast = (i * 32 + j == 1023);
				level = get_coded_level_rl(&d64_uncoded_cost, &d64_coded_cost, tmp_level_double[i], abs(tmp_coef[i]),
					mat_run[i], ctx_run, ctx_run, q_bits, err_scale, lambda, iflast, rdoq_est_run_local, rdoq_est_level_local);
				S17 level_cbf = level;
				tmp_dst_coef[i] = (S16)(tmp_coef[i] < 0 ? (S16)(-level_cbf) : (S16)level_cbf);
				tmp_dst_coef_out[i * 32 + j] = tmp_dst_coef[i];
				mat_uncoded_cost[i] = d64_uncoded_cost;
				mat_coded_cost[i] = d64_coded_cost;
				lev_opt[i] = level;
		}
		//model update
		for (int i = 0; i < 32; i++)
		{
			if (i + j <= 31) {
				if ((i + j) % 2 == 1) {
					pos[i] = (i + j + 1)*(i + j) / 2 + i;
				}
				else {
					pos[i] = (i + j + 1)*(i + j) / 2 + j;
				}
				ace_log2 = get_ace_log2(pos[i] + 1);
				t0[i] = ((COM_MIN((U24)(mat_prev_level[i] - 1), (U24)(5))) << 1) + (ch_type == 0 ? 0 : 12);
				last1_pos[i] = ((COM_MIN((U24)(mat_prev_level[i] - 1), (U24)(5)))) + (ch_type == 0 ? 0 : 6);
				last2_pos[i] = ace_log2 + (ch_type == 0 ? 0 : 12);
			}
		}
		for (scan_pos = 0; scan_pos < 24; scan_pos += 2)
		{
			level_core_32(32, 32, level_tmp, level_tmp1, t0, scan_pos, 32 - j);
			run_core_32(32, 32, run_tmp, run_tmp1, t0, scan_pos, mat_run, 32 - j);
		}

		for (scan_pos = 0; scan_pos < 12; scan_pos++)
		{
			last1_core_32(32, 32, last1_tmp, last1_pos, scan_pos, 32 - j);
			last1[scan_pos] += last1_tmp[scan_pos];
		}

		for (scan_pos = 0; scan_pos < 22; scan_pos++)
		{
			last2_core_32(32, 32, last2_tmp, last2_pos, scan_pos, 32 - j);
			last2[scan_pos] += last2_tmp[scan_pos];
		}

		for (scan_pos = 0; scan_pos < 24; scan_pos++)
		{
			level[scan_pos] += level_tmp[scan_pos] + level_tmp1[scan_pos];
			run[scan_pos] += run_tmp[scan_pos] + run_tmp1[scan_pos];
		}


		//dlnp prepare
		for (int i = 0; i < 32; i++)
		{
			if (i + j <= 31) {
				ace_log2 = get_ace_log2(pos[i]);
				U4 flag = 0;
				if (mat_prev_level[i] - 1 < 5)
				{
					flag = mat_prev_level[i] - 1;
				}
				else
				{
					flag = 5;
				}
				d64_cost_last_zero[i] = rdoq_est_last_local[ctx_last][flag][ace_log2][0] * lambda;
			}
		}

		for (int i = 0; i < 32; i++)
		{
			if (i < 32 - j) {
				if (lev_opt[i])
				{
					base_cost_buffer_tmp[i] = mat_coded_cost[i] - mat_uncoded_cost[i] + d64_cost_last_zero[i];
				}
				else
				{
					base_cost_buffer_tmp[i] = mat_coded_cost[i] - mat_uncoded_cost[i];
				}
			}
		}
		for (int i = 0; i < 32; i++)
		{
			if (i < 32 - j) {
				ace_log2 = get_ace_log2(pos[i]);
				U24 prev_level = mat_prev_level[i];
				U4 flag = 0;
				if (prev_level - 1 < 5)
				{
					flag = prev_level - 1;
				}
				else
				{
					flag = 5;
				}
				d64_cost_last_one_tmp[i] = GET_I_COST(rdoq_est_last_local[ctx_last][flag][ace_log2][1], lambda); // block scan order
			}
		}

		//dlnp core
		for (int i = 0; i < 32; i++) // horizontal
		{
			if (j == 0)
			{
				if (lev_opt[i])
				{
					rdoq_last_x[i] = i;
					rdoq_last_y[i] = j;
					rdoqD64LastOne[i] = d64_cost_last_one_tmp[i];
				}
				tempCost[i] = base_cost_buffer_tmp[i];
				if ((i + j) % 2 == 0) // bottom - left scan for (i+j)%2 is odd case
				{
					if (lev_opt[i]) endPosCost[i] = base_cost_buffer_tmp[i];
					else endPosCost[i] = 0;
				}
				else // above - top scan for (i+j)%2 is even case
				{
					if (lev_opt[i]) endPosCost[i] = 0;
					else endPosCost[i] = base_cost_buffer_tmp[i];

					// a special case (0, 0) position.
					if (i == 0)  // substract here is to get the actual endPosCost
					{
						endPosCost[i] = tempCost[i] - endPosCost[i];
					}
				}
			}
			else
			{
				if ((i + j) % 2 == 0) // bottom - left scan for (i+j)%2 is odd case
				{

					if (lev_opt[i] && rdoq_last_y[i] != -1) // compare for intialized case.
					{
						// add the rdoqD64LastOne consider here.
						// compare the best LNP for bottom-right scan
						if (tempCost[i] + base_cost_buffer_tmp[i] + d64_cost_last_one_tmp[i] < endPosCost[i] + rdoqD64LastOne[i])
						{
							rdoq_last_x[i] = i;
							rdoq_last_y[i] = j;
							endPosCost[i] = tempCost[i] + base_cost_buffer_tmp[i];
							rdoqD64LastOne[i] = d64_cost_last_one_tmp[i];
						}
					}
					if (lev_opt[i] && rdoq_last_y[i] == -1) // cost initial. purpose.
					{
						rdoq_last_x[i] = i;
						rdoq_last_y[i] = j;
						endPosCost[i] = tempCost[i] + base_cost_buffer_tmp[i];
						rdoqD64LastOne[i] = d64_cost_last_one_tmp[i];
					}

					tempCost[i] += base_cost_buffer_tmp[i];
				}
				else // above - top scan for (i+j)%2 is even case
				{
					S64 curCost = base_cost_buffer_tmp[i];  // care the bit-width. not int.


					if (lev_opt[i] && rdoq_last_y[i] != -1) // the previous non-zero position
					{
						// consider d64_cost_last_one_tmp part cost.
						if (tempCost[i] - endPosCost[i] + rdoqD64LastOne[i] - d64_cost_last_one_tmp[i] > 0) // previous position is better
						{
							rdoq_last_x[i] = i;
							rdoq_last_y[i] = j;
							endPosCost[i] = tempCost[i];
							rdoqD64LastOne[i] = d64_cost_last_one_tmp[i];
						}
					}

					if (lev_opt[i] && rdoq_last_y[i] == -1) // first non-zero position
					{
						rdoq_last_x[i] = i;
						rdoq_last_y[i] = j;
						// from start to current position (except current pos.)
						endPosCost[i] = tempCost[i];
						rdoqD64LastOne[i] = d64_cost_last_one_tmp[i];
					}

					tempCost[i] += curCost; // from the start point to current pos cost.

					if (i == 0)  // substract here is to get the actual endPosCost
					{
						endPosCost[i] = tempCost[i] - endPosCost[i];
					}
				}
			}
		}

		// each row assign.

		S64 temp_RdoqCost = finalRdoqCost + endPosCost[0];

		int index = rdoq_last_x[0] * 32 + rdoq_last_y[0];

		// cross check.

		if (rdoq_last_y[0] != -1 && temp_RdoqCost + rdoqD64LastOne[0] < d64_best_cost_tmp)
		{
			d64_best_cost_tmp = temp_RdoqCost + rdoqD64LastOne[0];
			final_rdoq_last_x = rdoq_last_x[0];
			final_rdoq_last_y = rdoq_last_y[0];
		}
		finalRdoqCost += tempCost[0];

		// shift register.
		for (int i = 1; i < 32; i++)
		{
			last_x[i - 1] = last_x[i];
			last_y[i - 1] = last_y[i];
			rdoq_last_x[i - 1] = rdoq_last_x[i];
			rdoq_last_y[i - 1] = rdoq_last_y[i];
			tempCost[i - 1] = tempCost[i];
			endPosCost[i - 1] = endPosCost[i];
			rdoqD64LastOne[i - 1] = rdoqD64LastOne[i];
		}
	}

	///////////////////////////////////////////////////
	// back to zig-zag scan order this can be optimized later.
#endif
	*final_x = final_rdoq_last_x;
	*final_y = final_rdoq_last_y;

  //printf("final_x = %d final_y = %d\n",*final_x,*final_y);  
}

void CleanNz_32_32( int num_nz[MAX_NUM_TB][N_C], S16 dst_tmp[32][32], S16 tmp_dst_coef[32][32], int final_x, int final_y)
{

	int num_nz_t = 0;
	for (int i = 0; i < 32; i++)
	{
#pragma HLS PIPELINE II=1
		for (int j = 0; j < 32; j++)
		{
			if (i + j < final_x + final_y) {
				dst_tmp[j][i] = tmp_dst_coef[j][i];
			}
			else if (i + j == final_x + final_y) {
				if ((final_x + final_y) % 2 == 1) {
					if (j<final_x && i>final_y) {
						dst_tmp[j][i] = 0;
					}
					else {
						dst_tmp[j][i] = tmp_dst_coef[j][i];
					}
				}
				else {
					if (j > final_x && i < final_y) {
						dst_tmp[j][i] = 0;
					}
					else {
						dst_tmp[j][i] = tmp_dst_coef[j][i];
					}
				}
			}
			else {
				dst_tmp[j][i] = 0;
			}
			num_nz_t += !!(dst_tmp[j][i]);
		}
	}
	num_nz[0][0] = num_nz_t;
}

void inv_dct_col_32_lite_s(U8 cu_width, U8 cu_height, hls::stream<s512_h> &coef, s16 block[32][32], U4 shift)
{
	U7 size = 32;
	U8 size_b = size - 1;


			int E[16];
#pragma HLS ARRAY_PARTITION variable=E complete dim=1
			int O[16];
#pragma HLS ARRAY_PARTITION variable=O complete dim=1
			int EE[8];
#pragma HLS ARRAY_PARTITION variable=EE complete dim=1
			int EO[8];
#pragma HLS ARRAY_PARTITION variable=EO complete dim=1
			int EEE[4];
#pragma HLS ARRAY_PARTITION variable=EEE complete dim=1
			int EEO[4];
#pragma HLS ARRAY_PARTITION variable=EEO complete dim=1
			int EEEE[2];
#pragma HLS ARRAY_PARTITION variable=EEEE complete dim=1
			int EEEO[2];
#pragma HLS ARRAY_PARTITION variable=EEEO complete dim=1

			int rnd_factor = 1 << (shift - 1);
			int k;
      
      ap_int<512> in;
      s16 coeff[32];

	  for (int i = 0; i < 32; i++) {
#pragma HLS PIPELINE
        in = coef.read();      

        for(int j = 0 ; j < 32 ; j ++)
        {
          coeff[j] = in(16*j+15,16*j);
        }
        
				O[0] = (45) * coeff[1] + (45) * coeff[3] + (44) * coeff[5] + (43) * coeff[7] + (41) * coeff[9] + (39) * coeff[11] + (36) * coeff[13] + (34) * coeff[15] + (30) * coeff[17] + (27) * coeff[19] + (23) * coeff[21] + (19) * coeff[23] + (15) * coeff[25] + (11) * coeff[27] + (7) * coeff[29] + (2) * coeff[31];
				O[1] = (45) * coeff[1] + (41) * coeff[3] + (34) * coeff[5] + (23) * coeff[7] + (11) * coeff[9] + (-2) * coeff[11] + (-15) * coeff[13] + (-27) * coeff[15] + (-36) * coeff[17] + (-43) * coeff[19] + (-45) * coeff[21] + (-44) * coeff[23] + (-39) * coeff[25] + (-30) * coeff[27] + (-19) * coeff[29] + (-7) * coeff[31];
				O[2] = (44) * coeff[1] + (34) * coeff[3] + (15) * coeff[5] + (-7) * coeff[7] + (-27) * coeff[9] + (-41) * coeff[11] + (-45) * coeff[13] + (-39) * coeff[15] + (-23) * coeff[17] + (-2) * coeff[19] + (19) * coeff[21] + (36) * coeff[23] + (45) * coeff[25] + (43) * coeff[27] + (30) * coeff[29] + (11) * coeff[31];
				O[3] = (43) * coeff[1] + (23) * coeff[3] + (-7) * coeff[5] + (-34) * coeff[7] + (-45) * coeff[9] + (-36) * coeff[11] + (-11) * coeff[13] + (19) * coeff[15] + (41) * coeff[17] + (44) * coeff[19] + (27) * coeff[21] + (-2) * coeff[23] + (-30) * coeff[25] + (-45) * coeff[27] + (-39) * coeff[29] + (-15) * coeff[31];
				O[4] = (41) * coeff[1] + (11) * coeff[3] + (-27) * coeff[5] + (-45) * coeff[7] + (-30) * coeff[9] + (7) * coeff[11] + (39) * coeff[13] + (43) * coeff[15] + (15) * coeff[17] + (-23) * coeff[19] + (-45) * coeff[21] + (-34) * coeff[23] + (2) * coeff[25] + (36) * coeff[27] + (44) * coeff[29] + (19) * coeff[31];
				O[5] = (39) * coeff[1] + (-2) * coeff[3] + (-41) * coeff[5] + (-36) * coeff[7] + (7) * coeff[9] + (43) * coeff[11] + (34) * coeff[13] + (-11) * coeff[15] + (-44) * coeff[17] + (-30) * coeff[19] + (15) * coeff[21] + (45) * coeff[23] + (27) * coeff[25] + (-19) * coeff[27] + (-45) * coeff[29] + (-23) * coeff[31];
				O[6] = (36) * coeff[1] + (-15) * coeff[3] + (-45) * coeff[5] + (-11) * coeff[7] + (39) * coeff[9] + (34) * coeff[11] + (-19) * coeff[13] + (-45) * coeff[15] + (-7) * coeff[17] + (41) * coeff[19] + (30) * coeff[21] + (-23) * coeff[23] + (-44) * coeff[25] + (-2) * coeff[27] + (43) * coeff[29] + (27) * coeff[31];
				O[7] = (34) * coeff[1] + (-27) * coeff[3] + (-39) * coeff[5] + (19) * coeff[7] + (43) * coeff[9] + (-11) * coeff[11] + (-45) * coeff[13] + (2) * coeff[15] + (45) * coeff[17] + (7) * coeff[19] + (-44) * coeff[21] + (-15) * coeff[23] + (41) * coeff[25] + (23) * coeff[27] + (-36) * coeff[29] + (-30) * coeff[31];
				O[8] = (30) * coeff[1] + (-36) * coeff[3] + (-23) * coeff[5] + (41) * coeff[7] + (15) * coeff[9] + (-44) * coeff[11] + (-7) * coeff[13] + (45) * coeff[15] + (-2) * coeff[17] + (-45) * coeff[19] + (11) * coeff[21] + (43) * coeff[23] + (-19) * coeff[25] + (-39) * coeff[27] + (27) * coeff[29] + (34) * coeff[31];
				O[9] = (27) * coeff[1] + (-43) * coeff[3] + (-2) * coeff[5] + (44) * coeff[7] + (-23) * coeff[9] + (-30) * coeff[11] + (41) * coeff[13] + (7) * coeff[15] + (-45) * coeff[17] + (19) * coeff[19] + (34) * coeff[21] + (-39) * coeff[23] + (-11) * coeff[25] + (45) * coeff[27] + (-15) * coeff[29] + (-36) * coeff[31];
				O[10] = (23) * coeff[1] + (-45) * coeff[3] + (19) * coeff[5] + (27) * coeff[7] + (-45) * coeff[9] + (15) * coeff[11] + (30) * coeff[13] + (-44) * coeff[15] + (11) * coeff[17] + (34) * coeff[19] + (-43) * coeff[21] + (7) * coeff[23] + (36) * coeff[25] + (-41) * coeff[27] + (2) * coeff[29] + (39) * coeff[31];
				O[11] = (19) * coeff[1] + (-44) * coeff[3] + (36) * coeff[5] + (-2) * coeff[7] + (-34) * coeff[9] + (45) * coeff[11] + (-23) * coeff[13] + (-15) * coeff[15] + (43) * coeff[17] + (-39) * coeff[19] + (7) * coeff[21] + (30) * coeff[23] + (-45) * coeff[25] + (27) * coeff[27] + (11) * coeff[29] + (-41) * coeff[31];
				O[12] = (15) * coeff[1] + (-39) * coeff[3] + (45) * coeff[5] + (-30) * coeff[7] + (2) * coeff[9] + (27) * coeff[11] + (-44) * coeff[13] + (41) * coeff[15] + (-19) * coeff[17] + (-11) * coeff[19] + (36) * coeff[21] + (-45) * coeff[23] + (34) * coeff[25] + (-7) * coeff[27] + (-23) * coeff[29] + (43) * coeff[31];
				O[13] = (11) * coeff[1] + (-30) * coeff[3] + (43) * coeff[5] + (-45) * coeff[7] + (36) * coeff[9] + (-19) * coeff[11] + (-2) * coeff[13] + (23) * coeff[15] + (-39) * coeff[17] + (45) * coeff[19] + (-41) * coeff[21] + (27) * coeff[23] + (-7) * coeff[25] + (-15) * coeff[27] + (34) * coeff[29] + (-44) * coeff[31];
				O[14] = (7) * coeff[1] + (-19) * coeff[3] + (30) * coeff[5] + (-39) * coeff[7] + (44) * coeff[9] + (-45) * coeff[11] + (43) * coeff[13] + (-36) * coeff[15] + (27) * coeff[17] + (-15) * coeff[19] + (2) * coeff[21] + (11) * coeff[23] + (-23) * coeff[25] + (34) * coeff[27] + (-41) * coeff[29] + (45) * coeff[31];
				O[15] = (2) * coeff[1] + (-7) * coeff[3] + (11) * coeff[5] + (-15) * coeff[7] + (19) * coeff[9] + (-23) * coeff[11] + (27) * coeff[13] + (-30) * coeff[15] + (34) * coeff[17] + (-36) * coeff[19] + (39) * coeff[21] + (-41) * coeff[23] + (43) * coeff[25] + (-44) * coeff[27] + (45) * coeff[29] + (-45) * coeff[31];

				EO[0] = (45) * coeff[2] + (43) * coeff[6] + (40) * coeff[10] + (35) * coeff[14] + (29) * coeff[18] + (21) * coeff[22] + (13) * coeff[26] + (4) * coeff[30];
				EO[1] = (43) * coeff[2] + (29) * coeff[6] + (4) * coeff[10] + (-21) * coeff[14] + (-40) * coeff[18] + (-45) * coeff[22] + (-35) * coeff[26] + (-13) * coeff[30];
				EO[2] = (40) * coeff[2] + (4) * coeff[6] + (-35) * coeff[10] + (-43) * coeff[14] + (-13) * coeff[18] + (29) * coeff[22] + (45) * coeff[26] + (21) * coeff[30];
				EO[3] = (35) * coeff[2] + (-21) * coeff[6] + (-43) * coeff[10] + (4) * coeff[14] + (45) * coeff[18] + (13) * coeff[22] + (-40) * coeff[26] + (-29) * coeff[30];
				EO[4] = (29) * coeff[2] + (-40) * coeff[6] + (-13) * coeff[10] + (45) * coeff[14] + (-4) * coeff[18] + (-43) * coeff[22] + (21) * coeff[26] + (35) * coeff[30];
				EO[5] = (21) * coeff[2] + (-45) * coeff[6] + (29) * coeff[10] + (13) * coeff[14] + (-43) * coeff[18] + (35) * coeff[22] + (4) * coeff[26] + (-40) * coeff[30];
				EO[6] = (13) * coeff[2] + (-35) * coeff[6] + (45) * coeff[10] + (-40) * coeff[14] + (21) * coeff[18] + (4) * coeff[22] + (-29) * coeff[26] + (43) * coeff[30];
				EO[7] = (4) * coeff[2] + (-13) * coeff[6] + (21) * coeff[10] + (-29) * coeff[14] + (35) * coeff[18] + (-40) * coeff[22] + (43) * coeff[26] + (-45) * coeff[30];

				EEO[0] = (44) * coeff[4] + (38) * coeff[12] + (25) * coeff[20] + (9) * coeff[28];
				EEO[1] = (38) * coeff[4] + (-9) * coeff[12] + (-44) * coeff[20] + (-25) * coeff[28];
				EEO[2] = (25) * coeff[4] + (-44) * coeff[12] + (9) * coeff[20] + (38) * coeff[28];
				EEO[3] = (9) * coeff[4] + (-25) * coeff[12] + (38) * coeff[20] + (-44) * coeff[28];

				EEEO[0] = (42) * coeff[8] + (17) * coeff[24];
				EEEO[1] = (17) * coeff[8] + (-42) * coeff[24];
				EEEE[0] = (32) * coeff[0] + (32) * coeff[16];
				EEEE[1] = (32) * coeff[0] + (-32) * coeff[16];

				EEE[0] = EEEE[0] + EEEO[0];
				EEE[3] = EEEE[0] - EEEO[0];
				EEE[1] = EEEE[1] + EEEO[1];
				EEE[2] = EEEE[1] - EEEO[1];

				for (k = 0; k < 4; k++) {
					EE[k] = EEE[k] + EEO[k];
					EE[k + 4] = EEE[3 - k] - EEO[3 - k];
				}
				for (k = 0; k < 8; k++) {
					E[k] = EE[k] + EO[k];
					E[k + 8] = EE[7 - k] - EO[7 - k];
				}


				if(i == 0){
				block[0][0] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] + O[0] + rnd_factor) >> shift);
				block[16][16] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15] - O[15] + rnd_factor) >> shift);
				block[1][1] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] + O[1] + rnd_factor) >> shift);
				block[17][17] = Clip3(-(1 << 15), (1 << 15) - 1, (E[14] - O[14] + rnd_factor) >> shift);
				block[2][2] = Clip3(-(1 << 15), (1 << 15) - 1, (E[2] + O[2] + rnd_factor) >> shift);
				block[18][18] = Clip3(-(1 << 15), (1 << 15) - 1, (E[13] - O[13] + rnd_factor) >> shift);
				block[3][3] = Clip3(-(1 << 15), (1 << 15) - 1, (E[3] + O[3] + rnd_factor) >> shift);
				block[19][19] = Clip3(-(1 << 15), (1 << 15) - 1, (E[12] - O[12] + rnd_factor) >> shift);
				block[4][4] = Clip3(-(1 << 15), (1 << 15) - 1, (E[4] + O[4] + rnd_factor) >> shift);
				block[20][20] = Clip3(-(1 << 15), (1 << 15) - 1, (E[11] - O[11] + rnd_factor) >> shift);
				block[5][5] = Clip3(-(1 << 15), (1 << 15) - 1, (E[5] + O[5] + rnd_factor) >> shift);
				block[21][21] = Clip3(-(1 << 15), (1 << 15) - 1, (E[10] - O[10] + rnd_factor) >> shift);
				block[6][6] = Clip3(-(1 << 15), (1 << 15) - 1, (E[6] + O[6] + rnd_factor) >> shift);
				block[22][22] = Clip3(-(1 << 15), (1 << 15) - 1, (E[9] - O[9] + rnd_factor) >> shift);
				block[7][7] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7] + O[7] + rnd_factor) >> shift);
				block[23][23] = Clip3(-(1 << 15), (1 << 15) - 1, (E[8] - O[8] + rnd_factor) >> shift);
				block[8][8] = Clip3(-(1 << 15), (1 << 15) - 1, (E[8] + O[8] + rnd_factor) >> shift);
				block[24][24] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7] - O[7] + rnd_factor) >> shift);
				block[9][9] = Clip3(-(1 << 15), (1 << 15) - 1, (E[9] + O[9] + rnd_factor) >> shift);
				block[25][25] = Clip3(-(1 << 15), (1 << 15) - 1, (E[6] - O[6] + rnd_factor) >> shift);
				block[10][10] = Clip3(-(1 << 15), (1 << 15) - 1, (E[10] + O[10] + rnd_factor) >> shift);
				block[26][26] = Clip3(-(1 << 15), (1 << 15) - 1, (E[5] - O[5] + rnd_factor) >> shift);
				block[11][11] = Clip3(-(1 << 15), (1 << 15) - 1, (E[11] + O[11] + rnd_factor) >> shift);
				block[27][27] = Clip3(-(1 << 15), (1 << 15) - 1, (E[4] - O[4] + rnd_factor) >> shift);
				block[12][12] = Clip3(-(1 << 15), (1 << 15) - 1, (E[12] + O[12] + rnd_factor) >> shift);
				block[28][28] = Clip3(-(1 << 15), (1 << 15) - 1, (E[3] - O[3] + rnd_factor) >> shift);
				block[13][13] = Clip3(-(1 << 15), (1 << 15) - 1, (E[13] + O[13] + rnd_factor) >> shift);
				block[29][29] = Clip3(-(1 << 15), (1 << 15) - 1, (E[2] - O[2] + rnd_factor) >> shift);
				block[14][14] = Clip3(-(1 << 15), (1 << 15) - 1, (E[14] + O[14] + rnd_factor) >> shift);
				block[30][30] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] - O[1] + rnd_factor) >> shift);
				block[15][15] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15] + O[15] + rnd_factor) >> shift);
				block[31][31] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] - O[0] + rnd_factor) >> shift);
				}
				else if(i == 1){
				block[1][0] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] + O[0] + rnd_factor) >> shift);
				block[17][16] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15] - O[15] + rnd_factor) >> shift);
				block[2][1] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] + O[1] + rnd_factor) >> shift);
				block[18][17] = Clip3(-(1 << 15), (1 << 15) - 1, (E[14] - O[14] + rnd_factor) >> shift);
				block[3][2] = Clip3(-(1 << 15), (1 << 15) - 1, (E[2] + O[2] + rnd_factor) >> shift);
				block[19][18] = Clip3(-(1 << 15), (1 << 15) - 1, (E[13] - O[13] + rnd_factor) >> shift);
				block[4][3] = Clip3(-(1 << 15), (1 << 15) - 1, (E[3] + O[3] + rnd_factor) >> shift);
				block[20][19] = Clip3(-(1 << 15), (1 << 15) - 1, (E[12] - O[12] + rnd_factor) >> shift);
				block[5][4] = Clip3(-(1 << 15), (1 << 15) - 1, (E[4] + O[4] + rnd_factor) >> shift);
				block[21][20] = Clip3(-(1 << 15), (1 << 15) - 1, (E[11] - O[11] + rnd_factor) >> shift);
				block[6][5] = Clip3(-(1 << 15), (1 << 15) - 1, (E[5] + O[5] + rnd_factor) >> shift);
				block[22][21] = Clip3(-(1 << 15), (1 << 15) - 1, (E[10] - O[10] + rnd_factor) >> shift);
				block[7][6] = Clip3(-(1 << 15), (1 << 15) - 1, (E[6] + O[6] + rnd_factor) >> shift);
				block[23][22] = Clip3(-(1 << 15), (1 << 15) - 1, (E[9] - O[9] + rnd_factor) >> shift);
				block[8][7] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7] + O[7] + rnd_factor) >> shift);
				block[24][23] = Clip3(-(1 << 15), (1 << 15) - 1, (E[8] - O[8] + rnd_factor) >> shift);
				block[9][8] = Clip3(-(1 << 15), (1 << 15) - 1, (E[8] + O[8] + rnd_factor) >> shift);
				block[25][24] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7] - O[7] + rnd_factor) >> shift);
				block[10][9] = Clip3(-(1 << 15), (1 << 15) - 1, (E[9] + O[9] + rnd_factor) >> shift);
				block[26][25] = Clip3(-(1 << 15), (1 << 15) - 1, (E[6] - O[6] + rnd_factor) >> shift);
				block[11][10] = Clip3(-(1 << 15), (1 << 15) - 1, (E[10] + O[10] + rnd_factor) >> shift);
				block[27][26] = Clip3(-(1 << 15), (1 << 15) - 1, (E[5] - O[5] + rnd_factor) >> shift);
				block[12][11] = Clip3(-(1 << 15), (1 << 15) - 1, (E[11] + O[11] + rnd_factor) >> shift);
				block[28][27] = Clip3(-(1 << 15), (1 << 15) - 1, (E[4] - O[4] + rnd_factor) >> shift);
				block[13][12] = Clip3(-(1 << 15), (1 << 15) - 1, (E[12] + O[12] + rnd_factor) >> shift);
				block[29][28] = Clip3(-(1 << 15), (1 << 15) - 1, (E[3] - O[3] + rnd_factor) >> shift);
				block[14][13] = Clip3(-(1 << 15), (1 << 15) - 1, (E[13] + O[13] + rnd_factor) >> shift);
				block[30][29] = Clip3(-(1 << 15), (1 << 15) - 1, (E[2] - O[2] + rnd_factor) >> shift);
				block[15][14] = Clip3(-(1 << 15), (1 << 15) - 1, (E[14] + O[14] + rnd_factor) >> shift);
				block[31][30] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] - O[1] + rnd_factor) >> shift);
				block[16][15] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15] + O[15] + rnd_factor) >> shift);
				block[0][31] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] - O[0] + rnd_factor) >> shift);
				}
				else if(i == 2){
				block[2][0] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] + O[0] + rnd_factor) >> shift);
				block[18][16] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15] - O[15] + rnd_factor) >> shift);
				block[3][1] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] + O[1] + rnd_factor) >> shift);
				block[19][17] = Clip3(-(1 << 15), (1 << 15) - 1, (E[14] - O[14] + rnd_factor) >> shift);
				block[4][2] = Clip3(-(1 << 15), (1 << 15) - 1, (E[2] + O[2] + rnd_factor) >> shift);
				block[20][18] = Clip3(-(1 << 15), (1 << 15) - 1, (E[13] - O[13] + rnd_factor) >> shift);
				block[5][3] = Clip3(-(1 << 15), (1 << 15) - 1, (E[3] + O[3] + rnd_factor) >> shift);
				block[21][19] = Clip3(-(1 << 15), (1 << 15) - 1, (E[12] - O[12] + rnd_factor) >> shift);
				block[6][4] = Clip3(-(1 << 15), (1 << 15) - 1, (E[4] + O[4] + rnd_factor) >> shift);
				block[22][20] = Clip3(-(1 << 15), (1 << 15) - 1, (E[11] - O[11] + rnd_factor) >> shift);
				block[7][5] = Clip3(-(1 << 15), (1 << 15) - 1, (E[5] + O[5] + rnd_factor) >> shift);
				block[23][21] = Clip3(-(1 << 15), (1 << 15) - 1, (E[10] - O[10] + rnd_factor) >> shift);
				block[8][6] = Clip3(-(1 << 15), (1 << 15) - 1, (E[6] + O[6] + rnd_factor) >> shift);
				block[24][22] = Clip3(-(1 << 15), (1 << 15) - 1, (E[9] - O[9] + rnd_factor) >> shift);
				block[9][7] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7] + O[7] + rnd_factor) >> shift);
				block[25][23] = Clip3(-(1 << 15), (1 << 15) - 1, (E[8] - O[8] + rnd_factor) >> shift);
				block[10][8] = Clip3(-(1 << 15), (1 << 15) - 1, (E[8] + O[8] + rnd_factor) >> shift);
				block[26][24] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7] - O[7] + rnd_factor) >> shift);
				block[11][9] = Clip3(-(1 << 15), (1 << 15) - 1, (E[9] + O[9] + rnd_factor) >> shift);
				block[27][25] = Clip3(-(1 << 15), (1 << 15) - 1, (E[6] - O[6] + rnd_factor) >> shift);
				block[12][10] = Clip3(-(1 << 15), (1 << 15) - 1, (E[10] + O[10] + rnd_factor) >> shift);
				block[28][26] = Clip3(-(1 << 15), (1 << 15) - 1, (E[5] - O[5] + rnd_factor) >> shift);
				block[13][11] = Clip3(-(1 << 15), (1 << 15) - 1, (E[11] + O[11] + rnd_factor) >> shift);
				block[29][27] = Clip3(-(1 << 15), (1 << 15) - 1, (E[4] - O[4] + rnd_factor) >> shift);
				block[14][12] = Clip3(-(1 << 15), (1 << 15) - 1, (E[12] + O[12] + rnd_factor) >> shift);
				block[30][28] = Clip3(-(1 << 15), (1 << 15) - 1, (E[3] - O[3] + rnd_factor) >> shift);
				block[15][13] = Clip3(-(1 << 15), (1 << 15) - 1, (E[13] + O[13] + rnd_factor) >> shift);
				block[31][29] = Clip3(-(1 << 15), (1 << 15) - 1, (E[2] - O[2] + rnd_factor) >> shift);
				block[16][14] = Clip3(-(1 << 15), (1 << 15) - 1, (E[14] + O[14] + rnd_factor) >> shift);
				block[0][30] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] - O[1] + rnd_factor) >> shift);
				block[17][15] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15] + O[15] + rnd_factor) >> shift);
				block[1][31] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] - O[0] + rnd_factor) >> shift);
				}
				else if(i == 3){
				block[3][0] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] + O[0] + rnd_factor) >> shift);
				block[19][16] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15] - O[15] + rnd_factor) >> shift);
				block[4][1] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] + O[1] + rnd_factor) >> shift);
				block[20][17] = Clip3(-(1 << 15), (1 << 15) - 1, (E[14] - O[14] + rnd_factor) >> shift);
				block[5][2] = Clip3(-(1 << 15), (1 << 15) - 1, (E[2] + O[2] + rnd_factor) >> shift);
				block[21][18] = Clip3(-(1 << 15), (1 << 15) - 1, (E[13] - O[13] + rnd_factor) >> shift);
				block[6][3] = Clip3(-(1 << 15), (1 << 15) - 1, (E[3] + O[3] + rnd_factor) >> shift);
				block[22][19] = Clip3(-(1 << 15), (1 << 15) - 1, (E[12] - O[12] + rnd_factor) >> shift);
				block[7][4] = Clip3(-(1 << 15), (1 << 15) - 1, (E[4] + O[4] + rnd_factor) >> shift);
				block[23][20] = Clip3(-(1 << 15), (1 << 15) - 1, (E[11] - O[11] + rnd_factor) >> shift);
				block[8][5] = Clip3(-(1 << 15), (1 << 15) - 1, (E[5] + O[5] + rnd_factor) >> shift);
				block[24][21] = Clip3(-(1 << 15), (1 << 15) - 1, (E[10] - O[10] + rnd_factor) >> shift);
				block[9][6] = Clip3(-(1 << 15), (1 << 15) - 1, (E[6] + O[6] + rnd_factor) >> shift);
				block[25][22] = Clip3(-(1 << 15), (1 << 15) - 1, (E[9] - O[9] + rnd_factor) >> shift);
				block[10][7] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7] + O[7] + rnd_factor) >> shift);
				block[26][23] = Clip3(-(1 << 15), (1 << 15) - 1, (E[8] - O[8] + rnd_factor) >> shift);
				block[11][8] = Clip3(-(1 << 15), (1 << 15) - 1, (E[8] + O[8] + rnd_factor) >> shift);
				block[27][24] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7] - O[7] + rnd_factor) >> shift);
				block[12][9] = Clip3(-(1 << 15), (1 << 15) - 1, (E[9] + O[9] + rnd_factor) >> shift);
				block[28][25] = Clip3(-(1 << 15), (1 << 15) - 1, (E[6] - O[6] + rnd_factor) >> shift);
				block[13][10] = Clip3(-(1 << 15), (1 << 15) - 1, (E[10] + O[10] + rnd_factor) >> shift);
				block[29][26] = Clip3(-(1 << 15), (1 << 15) - 1, (E[5] - O[5] + rnd_factor) >> shift);
				block[14][11] = Clip3(-(1 << 15), (1 << 15) - 1, (E[11] + O[11] + rnd_factor) >> shift);
				block[30][27] = Clip3(-(1 << 15), (1 << 15) - 1, (E[4] - O[4] + rnd_factor) >> shift);
				block[15][12] = Clip3(-(1 << 15), (1 << 15) - 1, (E[12] + O[12] + rnd_factor) >> shift);
				block[31][28] = Clip3(-(1 << 15), (1 << 15) - 1, (E[3] - O[3] + rnd_factor) >> shift);
				block[16][13] = Clip3(-(1 << 15), (1 << 15) - 1, (E[13] + O[13] + rnd_factor) >> shift);
				block[0][29] = Clip3(-(1 << 15), (1 << 15) - 1, (E[2] - O[2] + rnd_factor) >> shift);
				block[17][14] = Clip3(-(1 << 15), (1 << 15) - 1, (E[14] + O[14] + rnd_factor) >> shift);
				block[1][30] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] - O[1] + rnd_factor) >> shift);
				block[18][15] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15] + O[15] + rnd_factor) >> shift);
				block[2][31] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] - O[0] + rnd_factor) >> shift);
				}
				else if(i == 4){
				block[4][0] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] + O[0] + rnd_factor) >> shift);
				block[20][16] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15] - O[15] + rnd_factor) >> shift);
				block[5][1] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] + O[1] + rnd_factor) >> shift);
				block[21][17] = Clip3(-(1 << 15), (1 << 15) - 1, (E[14] - O[14] + rnd_factor) >> shift);
				block[6][2] = Clip3(-(1 << 15), (1 << 15) - 1, (E[2] + O[2] + rnd_factor) >> shift);
				block[22][18] = Clip3(-(1 << 15), (1 << 15) - 1, (E[13] - O[13] + rnd_factor) >> shift);
				block[7][3] = Clip3(-(1 << 15), (1 << 15) - 1, (E[3] + O[3] + rnd_factor) >> shift);
				block[23][19] = Clip3(-(1 << 15), (1 << 15) - 1, (E[12] - O[12] + rnd_factor) >> shift);
				block[8][4] = Clip3(-(1 << 15), (1 << 15) - 1, (E[4] + O[4] + rnd_factor) >> shift);
				block[24][20] = Clip3(-(1 << 15), (1 << 15) - 1, (E[11] - O[11] + rnd_factor) >> shift);
				block[9][5] = Clip3(-(1 << 15), (1 << 15) - 1, (E[5] + O[5] + rnd_factor) >> shift);
				block[25][21] = Clip3(-(1 << 15), (1 << 15) - 1, (E[10] - O[10] + rnd_factor) >> shift);
				block[10][6] = Clip3(-(1 << 15), (1 << 15) - 1, (E[6] + O[6] + rnd_factor) >> shift);
				block[26][22] = Clip3(-(1 << 15), (1 << 15) - 1, (E[9] - O[9] + rnd_factor) >> shift);
				block[11][7] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7] + O[7] + rnd_factor) >> shift);
				block[27][23] = Clip3(-(1 << 15), (1 << 15) - 1, (E[8] - O[8] + rnd_factor) >> shift);
				block[12][8] = Clip3(-(1 << 15), (1 << 15) - 1, (E[8] + O[8] + rnd_factor) >> shift);
				block[28][24] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7] - O[7] + rnd_factor) >> shift);
				block[13][9] = Clip3(-(1 << 15), (1 << 15) - 1, (E[9] + O[9] + rnd_factor) >> shift);
				block[29][25] = Clip3(-(1 << 15), (1 << 15) - 1, (E[6] - O[6] + rnd_factor) >> shift);
				block[14][10] = Clip3(-(1 << 15), (1 << 15) - 1, (E[10] + O[10] + rnd_factor) >> shift);
				block[30][26] = Clip3(-(1 << 15), (1 << 15) - 1, (E[5] - O[5] + rnd_factor) >> shift);
				block[15][11] = Clip3(-(1 << 15), (1 << 15) - 1, (E[11] + O[11] + rnd_factor) >> shift);
				block[31][27] = Clip3(-(1 << 15), (1 << 15) - 1, (E[4] - O[4] + rnd_factor) >> shift);
				block[16][12] = Clip3(-(1 << 15), (1 << 15) - 1, (E[12] + O[12] + rnd_factor) >> shift);
				block[0][28] = Clip3(-(1 << 15), (1 << 15) - 1, (E[3] - O[3] + rnd_factor) >> shift);
				block[17][13] = Clip3(-(1 << 15), (1 << 15) - 1, (E[13] + O[13] + rnd_factor) >> shift);
				block[1][29] = Clip3(-(1 << 15), (1 << 15) - 1, (E[2] - O[2] + rnd_factor) >> shift);
				block[18][14] = Clip3(-(1 << 15), (1 << 15) - 1, (E[14] + O[14] + rnd_factor) >> shift);
				block[2][30] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] - O[1] + rnd_factor) >> shift);
				block[19][15] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15] + O[15] + rnd_factor) >> shift);
				block[3][31] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] - O[0] + rnd_factor) >> shift);
				}
				else if(i == 5){
				block[5][0] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] + O[0] + rnd_factor) >> shift);
				block[21][16] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15] - O[15] + rnd_factor) >> shift);
				block[6][1] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] + O[1] + rnd_factor) >> shift);
				block[22][17] = Clip3(-(1 << 15), (1 << 15) - 1, (E[14] - O[14] + rnd_factor) >> shift);
				block[7][2] = Clip3(-(1 << 15), (1 << 15) - 1, (E[2] + O[2] + rnd_factor) >> shift);
				block[23][18] = Clip3(-(1 << 15), (1 << 15) - 1, (E[13] - O[13] + rnd_factor) >> shift);
				block[8][3] = Clip3(-(1 << 15), (1 << 15) - 1, (E[3] + O[3] + rnd_factor) >> shift);
				block[24][19] = Clip3(-(1 << 15), (1 << 15) - 1, (E[12] - O[12] + rnd_factor) >> shift);
				block[9][4] = Clip3(-(1 << 15), (1 << 15) - 1, (E[4] + O[4] + rnd_factor) >> shift);
				block[25][20] = Clip3(-(1 << 15), (1 << 15) - 1, (E[11] - O[11] + rnd_factor) >> shift);
				block[10][5] = Clip3(-(1 << 15), (1 << 15) - 1, (E[5] + O[5] + rnd_factor) >> shift);
				block[26][21] = Clip3(-(1 << 15), (1 << 15) - 1, (E[10] - O[10] + rnd_factor) >> shift);
				block[11][6] = Clip3(-(1 << 15), (1 << 15) - 1, (E[6] + O[6] + rnd_factor) >> shift);
				block[27][22] = Clip3(-(1 << 15), (1 << 15) - 1, (E[9] - O[9] + rnd_factor) >> shift);
				block[12][7] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7] + O[7] + rnd_factor) >> shift);
				block[28][23] = Clip3(-(1 << 15), (1 << 15) - 1, (E[8] - O[8] + rnd_factor) >> shift);
				block[13][8] = Clip3(-(1 << 15), (1 << 15) - 1, (E[8] + O[8] + rnd_factor) >> shift);
				block[29][24] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7] - O[7] + rnd_factor) >> shift);
				block[14][9] = Clip3(-(1 << 15), (1 << 15) - 1, (E[9] + O[9] + rnd_factor) >> shift);
				block[30][25] = Clip3(-(1 << 15), (1 << 15) - 1, (E[6] - O[6] + rnd_factor) >> shift);
				block[15][10] = Clip3(-(1 << 15), (1 << 15) - 1, (E[10] + O[10] + rnd_factor) >> shift);
				block[31][26] = Clip3(-(1 << 15), (1 << 15) - 1, (E[5] - O[5] + rnd_factor) >> shift);
				block[16][11] = Clip3(-(1 << 15), (1 << 15) - 1, (E[11] + O[11] + rnd_factor) >> shift);
				block[0][27] = Clip3(-(1 << 15), (1 << 15) - 1, (E[4] - O[4] + rnd_factor) >> shift);
				block[17][12] = Clip3(-(1 << 15), (1 << 15) - 1, (E[12] + O[12] + rnd_factor) >> shift);
				block[1][28] = Clip3(-(1 << 15), (1 << 15) - 1, (E[3] - O[3] + rnd_factor) >> shift);
				block[18][13] = Clip3(-(1 << 15), (1 << 15) - 1, (E[13] + O[13] + rnd_factor) >> shift);
				block[2][29] = Clip3(-(1 << 15), (1 << 15) - 1, (E[2] - O[2] + rnd_factor) >> shift);
				block[19][14] = Clip3(-(1 << 15), (1 << 15) - 1, (E[14] + O[14] + rnd_factor) >> shift);
				block[3][30] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] - O[1] + rnd_factor) >> shift);
				block[20][15] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15] + O[15] + rnd_factor) >> shift);
				block[4][31] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] - O[0] + rnd_factor) >> shift);
				}
				else if(i == 6){
				block[6][0] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] + O[0] + rnd_factor) >> shift);
				block[22][16] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15] - O[15] + rnd_factor) >> shift);
				block[7][1] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] + O[1] + rnd_factor) >> shift);
				block[23][17] = Clip3(-(1 << 15), (1 << 15) - 1, (E[14] - O[14] + rnd_factor) >> shift);
				block[8][2] = Clip3(-(1 << 15), (1 << 15) - 1, (E[2] + O[2] + rnd_factor) >> shift);
				block[24][18] = Clip3(-(1 << 15), (1 << 15) - 1, (E[13] - O[13] + rnd_factor) >> shift);
				block[9][3] = Clip3(-(1 << 15), (1 << 15) - 1, (E[3] + O[3] + rnd_factor) >> shift);
				block[25][19] = Clip3(-(1 << 15), (1 << 15) - 1, (E[12] - O[12] + rnd_factor) >> shift);
				block[10][4] = Clip3(-(1 << 15), (1 << 15) - 1, (E[4] + O[4] + rnd_factor) >> shift);
				block[26][20] = Clip3(-(1 << 15), (1 << 15) - 1, (E[11] - O[11] + rnd_factor) >> shift);
				block[11][5] = Clip3(-(1 << 15), (1 << 15) - 1, (E[5] + O[5] + rnd_factor) >> shift);
				block[27][21] = Clip3(-(1 << 15), (1 << 15) - 1, (E[10] - O[10] + rnd_factor) >> shift);
				block[12][6] = Clip3(-(1 << 15), (1 << 15) - 1, (E[6] + O[6] + rnd_factor) >> shift);
				block[28][22] = Clip3(-(1 << 15), (1 << 15) - 1, (E[9] - O[9] + rnd_factor) >> shift);
				block[13][7] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7] + O[7] + rnd_factor) >> shift);
				block[29][23] = Clip3(-(1 << 15), (1 << 15) - 1, (E[8] - O[8] + rnd_factor) >> shift);
				block[14][8] = Clip3(-(1 << 15), (1 << 15) - 1, (E[8] + O[8] + rnd_factor) >> shift);
				block[30][24] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7] - O[7] + rnd_factor) >> shift);
				block[15][9] = Clip3(-(1 << 15), (1 << 15) - 1, (E[9] + O[9] + rnd_factor) >> shift);
				block[31][25] = Clip3(-(1 << 15), (1 << 15) - 1, (E[6] - O[6] + rnd_factor) >> shift);
				block[16][10] = Clip3(-(1 << 15), (1 << 15) - 1, (E[10] + O[10] + rnd_factor) >> shift);
				block[0][26] = Clip3(-(1 << 15), (1 << 15) - 1, (E[5] - O[5] + rnd_factor) >> shift);
				block[17][11] = Clip3(-(1 << 15), (1 << 15) - 1, (E[11] + O[11] + rnd_factor) >> shift);
				block[1][27] = Clip3(-(1 << 15), (1 << 15) - 1, (E[4] - O[4] + rnd_factor) >> shift);
				block[18][12] = Clip3(-(1 << 15), (1 << 15) - 1, (E[12] + O[12] + rnd_factor) >> shift);
				block[2][28] = Clip3(-(1 << 15), (1 << 15) - 1, (E[3] - O[3] + rnd_factor) >> shift);
				block[19][13] = Clip3(-(1 << 15), (1 << 15) - 1, (E[13] + O[13] + rnd_factor) >> shift);
				block[3][29] = Clip3(-(1 << 15), (1 << 15) - 1, (E[2] - O[2] + rnd_factor) >> shift);
				block[20][14] = Clip3(-(1 << 15), (1 << 15) - 1, (E[14] + O[14] + rnd_factor) >> shift);
				block[4][30] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] - O[1] + rnd_factor) >> shift);
				block[21][15] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15] + O[15] + rnd_factor) >> shift);
				block[5][31] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] - O[0] + rnd_factor) >> shift);
				}
				else if(i == 7){
				block[7][0] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] + O[0] + rnd_factor) >> shift);
				block[23][16] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15] - O[15] + rnd_factor) >> shift);
				block[8][1] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] + O[1] + rnd_factor) >> shift);
				block[24][17] = Clip3(-(1 << 15), (1 << 15) - 1, (E[14] - O[14] + rnd_factor) >> shift);
				block[9][2] = Clip3(-(1 << 15), (1 << 15) - 1, (E[2] + O[2] + rnd_factor) >> shift);
				block[25][18] = Clip3(-(1 << 15), (1 << 15) - 1, (E[13] - O[13] + rnd_factor) >> shift);
				block[10][3] = Clip3(-(1 << 15), (1 << 15) - 1, (E[3] + O[3] + rnd_factor) >> shift);
				block[26][19] = Clip3(-(1 << 15), (1 << 15) - 1, (E[12] - O[12] + rnd_factor) >> shift);
				block[11][4] = Clip3(-(1 << 15), (1 << 15) - 1, (E[4] + O[4] + rnd_factor) >> shift);
				block[27][20] = Clip3(-(1 << 15), (1 << 15) - 1, (E[11] - O[11] + rnd_factor) >> shift);
				block[12][5] = Clip3(-(1 << 15), (1 << 15) - 1, (E[5] + O[5] + rnd_factor) >> shift);
				block[28][21] = Clip3(-(1 << 15), (1 << 15) - 1, (E[10] - O[10] + rnd_factor) >> shift);
				block[13][6] = Clip3(-(1 << 15), (1 << 15) - 1, (E[6] + O[6] + rnd_factor) >> shift);
				block[29][22] = Clip3(-(1 << 15), (1 << 15) - 1, (E[9] - O[9] + rnd_factor) >> shift);
				block[14][7] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7] + O[7] + rnd_factor) >> shift);
				block[30][23] = Clip3(-(1 << 15), (1 << 15) - 1, (E[8] - O[8] + rnd_factor) >> shift);
				block[15][8] = Clip3(-(1 << 15), (1 << 15) - 1, (E[8] + O[8] + rnd_factor) >> shift);
				block[31][24] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7] - O[7] + rnd_factor) >> shift);
				block[16][9] = Clip3(-(1 << 15), (1 << 15) - 1, (E[9] + O[9] + rnd_factor) >> shift);
				block[0][25] = Clip3(-(1 << 15), (1 << 15) - 1, (E[6] - O[6] + rnd_factor) >> shift);
				block[17][10] = Clip3(-(1 << 15), (1 << 15) - 1, (E[10] + O[10] + rnd_factor) >> shift);
				block[1][26] = Clip3(-(1 << 15), (1 << 15) - 1, (E[5] - O[5] + rnd_factor) >> shift);
				block[18][11] = Clip3(-(1 << 15), (1 << 15) - 1, (E[11] + O[11] + rnd_factor) >> shift);
				block[2][27] = Clip3(-(1 << 15), (1 << 15) - 1, (E[4] - O[4] + rnd_factor) >> shift);
				block[19][12] = Clip3(-(1 << 15), (1 << 15) - 1, (E[12] + O[12] + rnd_factor) >> shift);
				block[3][28] = Clip3(-(1 << 15), (1 << 15) - 1, (E[3] - O[3] + rnd_factor) >> shift);
				block[20][13] = Clip3(-(1 << 15), (1 << 15) - 1, (E[13] + O[13] + rnd_factor) >> shift);
				block[4][29] = Clip3(-(1 << 15), (1 << 15) - 1, (E[2] - O[2] + rnd_factor) >> shift);
				block[21][14] = Clip3(-(1 << 15), (1 << 15) - 1, (E[14] + O[14] + rnd_factor) >> shift);
				block[5][30] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] - O[1] + rnd_factor) >> shift);
				block[22][15] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15] + O[15] + rnd_factor) >> shift);
				block[6][31] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] - O[0] + rnd_factor) >> shift);
				}
				else if(i == 8){
				block[8][0] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] + O[0] + rnd_factor) >> shift);
				block[24][16] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15] - O[15] + rnd_factor) >> shift);
				block[9][1] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] + O[1] + rnd_factor) >> shift);
				block[25][17] = Clip3(-(1 << 15), (1 << 15) - 1, (E[14] - O[14] + rnd_factor) >> shift);
				block[10][2] = Clip3(-(1 << 15), (1 << 15) - 1, (E[2] + O[2] + rnd_factor) >> shift);
				block[26][18] = Clip3(-(1 << 15), (1 << 15) - 1, (E[13] - O[13] + rnd_factor) >> shift);
				block[11][3] = Clip3(-(1 << 15), (1 << 15) - 1, (E[3] + O[3] + rnd_factor) >> shift);
				block[27][19] = Clip3(-(1 << 15), (1 << 15) - 1, (E[12] - O[12] + rnd_factor) >> shift);
				block[12][4] = Clip3(-(1 << 15), (1 << 15) - 1, (E[4] + O[4] + rnd_factor) >> shift);
				block[28][20] = Clip3(-(1 << 15), (1 << 15) - 1, (E[11] - O[11] + rnd_factor) >> shift);
				block[13][5] = Clip3(-(1 << 15), (1 << 15) - 1, (E[5] + O[5] + rnd_factor) >> shift);
				block[29][21] = Clip3(-(1 << 15), (1 << 15) - 1, (E[10] - O[10] + rnd_factor) >> shift);
				block[14][6] = Clip3(-(1 << 15), (1 << 15) - 1, (E[6] + O[6] + rnd_factor) >> shift);
				block[30][22] = Clip3(-(1 << 15), (1 << 15) - 1, (E[9] - O[9] + rnd_factor) >> shift);
				block[15][7] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7] + O[7] + rnd_factor) >> shift);
				block[31][23] = Clip3(-(1 << 15), (1 << 15) - 1, (E[8] - O[8] + rnd_factor) >> shift);
				block[16][8] = Clip3(-(1 << 15), (1 << 15) - 1, (E[8] + O[8] + rnd_factor) >> shift);
				block[0][24] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7] - O[7] + rnd_factor) >> shift);
				block[17][9] = Clip3(-(1 << 15), (1 << 15) - 1, (E[9] + O[9] + rnd_factor) >> shift);
				block[1][25] = Clip3(-(1 << 15), (1 << 15) - 1, (E[6] - O[6] + rnd_factor) >> shift);
				block[18][10] = Clip3(-(1 << 15), (1 << 15) - 1, (E[10] + O[10] + rnd_factor) >> shift);
				block[2][26] = Clip3(-(1 << 15), (1 << 15) - 1, (E[5] - O[5] + rnd_factor) >> shift);
				block[19][11] = Clip3(-(1 << 15), (1 << 15) - 1, (E[11] + O[11] + rnd_factor) >> shift);
				block[3][27] = Clip3(-(1 << 15), (1 << 15) - 1, (E[4] - O[4] + rnd_factor) >> shift);
				block[20][12] = Clip3(-(1 << 15), (1 << 15) - 1, (E[12] + O[12] + rnd_factor) >> shift);
				block[4][28] = Clip3(-(1 << 15), (1 << 15) - 1, (E[3] - O[3] + rnd_factor) >> shift);
				block[21][13] = Clip3(-(1 << 15), (1 << 15) - 1, (E[13] + O[13] + rnd_factor) >> shift);
				block[5][29] = Clip3(-(1 << 15), (1 << 15) - 1, (E[2] - O[2] + rnd_factor) >> shift);
				block[22][14] = Clip3(-(1 << 15), (1 << 15) - 1, (E[14] + O[14] + rnd_factor) >> shift);
				block[6][30] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] - O[1] + rnd_factor) >> shift);
				block[23][15] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15] + O[15] + rnd_factor) >> shift);
				block[7][31] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] - O[0] + rnd_factor) >> shift);
				}
				else if(i == 9){
				block[9][0] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] + O[0] + rnd_factor) >> shift);
				block[25][16] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15] - O[15] + rnd_factor) >> shift);
				block[10][1] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] + O[1] + rnd_factor) >> shift);
				block[26][17] = Clip3(-(1 << 15), (1 << 15) - 1, (E[14] - O[14] + rnd_factor) >> shift);
				block[11][2] = Clip3(-(1 << 15), (1 << 15) - 1, (E[2] + O[2] + rnd_factor) >> shift);
				block[27][18] = Clip3(-(1 << 15), (1 << 15) - 1, (E[13] - O[13] + rnd_factor) >> shift);
				block[12][3] = Clip3(-(1 << 15), (1 << 15) - 1, (E[3] + O[3] + rnd_factor) >> shift);
				block[28][19] = Clip3(-(1 << 15), (1 << 15) - 1, (E[12] - O[12] + rnd_factor) >> shift);
				block[13][4] = Clip3(-(1 << 15), (1 << 15) - 1, (E[4] + O[4] + rnd_factor) >> shift);
				block[29][20] = Clip3(-(1 << 15), (1 << 15) - 1, (E[11] - O[11] + rnd_factor) >> shift);
				block[14][5] = Clip3(-(1 << 15), (1 << 15) - 1, (E[5] + O[5] + rnd_factor) >> shift);
				block[30][21] = Clip3(-(1 << 15), (1 << 15) - 1, (E[10] - O[10] + rnd_factor) >> shift);
				block[15][6] = Clip3(-(1 << 15), (1 << 15) - 1, (E[6] + O[6] + rnd_factor) >> shift);
				block[31][22] = Clip3(-(1 << 15), (1 << 15) - 1, (E[9] - O[9] + rnd_factor) >> shift);
				block[16][7] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7] + O[7] + rnd_factor) >> shift);
				block[0][23] = Clip3(-(1 << 15), (1 << 15) - 1, (E[8] - O[8] + rnd_factor) >> shift);
				block[17][8] = Clip3(-(1 << 15), (1 << 15) - 1, (E[8] + O[8] + rnd_factor) >> shift);
				block[1][24] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7] - O[7] + rnd_factor) >> shift);
				block[18][9] = Clip3(-(1 << 15), (1 << 15) - 1, (E[9] + O[9] + rnd_factor) >> shift);
				block[2][25] = Clip3(-(1 << 15), (1 << 15) - 1, (E[6] - O[6] + rnd_factor) >> shift);
				block[19][10] = Clip3(-(1 << 15), (1 << 15) - 1, (E[10] + O[10] + rnd_factor) >> shift);
				block[3][26] = Clip3(-(1 << 15), (1 << 15) - 1, (E[5] - O[5] + rnd_factor) >> shift);
				block[20][11] = Clip3(-(1 << 15), (1 << 15) - 1, (E[11] + O[11] + rnd_factor) >> shift);
				block[4][27] = Clip3(-(1 << 15), (1 << 15) - 1, (E[4] - O[4] + rnd_factor) >> shift);
				block[21][12] = Clip3(-(1 << 15), (1 << 15) - 1, (E[12] + O[12] + rnd_factor) >> shift);
				block[5][28] = Clip3(-(1 << 15), (1 << 15) - 1, (E[3] - O[3] + rnd_factor) >> shift);
				block[22][13] = Clip3(-(1 << 15), (1 << 15) - 1, (E[13] + O[13] + rnd_factor) >> shift);
				block[6][29] = Clip3(-(1 << 15), (1 << 15) - 1, (E[2] - O[2] + rnd_factor) >> shift);
				block[23][14] = Clip3(-(1 << 15), (1 << 15) - 1, (E[14] + O[14] + rnd_factor) >> shift);
				block[7][30] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] - O[1] + rnd_factor) >> shift);
				block[24][15] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15] + O[15] + rnd_factor) >> shift);
				block[8][31] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] - O[0] + rnd_factor) >> shift);
				}
				else if(i == 10){
				block[10][0] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] + O[0] + rnd_factor) >> shift);
				block[26][16] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15] - O[15] + rnd_factor) >> shift);
				block[11][1] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] + O[1] + rnd_factor) >> shift);
				block[27][17] = Clip3(-(1 << 15), (1 << 15) - 1, (E[14] - O[14] + rnd_factor) >> shift);
				block[12][2] = Clip3(-(1 << 15), (1 << 15) - 1, (E[2] + O[2] + rnd_factor) >> shift);
				block[28][18] = Clip3(-(1 << 15), (1 << 15) - 1, (E[13] - O[13] + rnd_factor) >> shift);
				block[13][3] = Clip3(-(1 << 15), (1 << 15) - 1, (E[3] + O[3] + rnd_factor) >> shift);
				block[29][19] = Clip3(-(1 << 15), (1 << 15) - 1, (E[12] - O[12] + rnd_factor) >> shift);
				block[14][4] = Clip3(-(1 << 15), (1 << 15) - 1, (E[4] + O[4] + rnd_factor) >> shift);
				block[30][20] = Clip3(-(1 << 15), (1 << 15) - 1, (E[11] - O[11] + rnd_factor) >> shift);
				block[15][5] = Clip3(-(1 << 15), (1 << 15) - 1, (E[5] + O[5] + rnd_factor) >> shift);
				block[31][21] = Clip3(-(1 << 15), (1 << 15) - 1, (E[10] - O[10] + rnd_factor) >> shift);
				block[16][6] = Clip3(-(1 << 15), (1 << 15) - 1, (E[6] + O[6] + rnd_factor) >> shift);
				block[0][22] = Clip3(-(1 << 15), (1 << 15) - 1, (E[9] - O[9] + rnd_factor) >> shift);
				block[17][7] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7] + O[7] + rnd_factor) >> shift);
				block[1][23] = Clip3(-(1 << 15), (1 << 15) - 1, (E[8] - O[8] + rnd_factor) >> shift);
				block[18][8] = Clip3(-(1 << 15), (1 << 15) - 1, (E[8] + O[8] + rnd_factor) >> shift);
				block[2][24] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7] - O[7] + rnd_factor) >> shift);
				block[19][9] = Clip3(-(1 << 15), (1 << 15) - 1, (E[9] + O[9] + rnd_factor) >> shift);
				block[3][25] = Clip3(-(1 << 15), (1 << 15) - 1, (E[6] - O[6] + rnd_factor) >> shift);
				block[20][10] = Clip3(-(1 << 15), (1 << 15) - 1, (E[10] + O[10] + rnd_factor) >> shift);
				block[4][26] = Clip3(-(1 << 15), (1 << 15) - 1, (E[5] - O[5] + rnd_factor) >> shift);
				block[21][11] = Clip3(-(1 << 15), (1 << 15) - 1, (E[11] + O[11] + rnd_factor) >> shift);
				block[5][27] = Clip3(-(1 << 15), (1 << 15) - 1, (E[4] - O[4] + rnd_factor) >> shift);
				block[22][12] = Clip3(-(1 << 15), (1 << 15) - 1, (E[12] + O[12] + rnd_factor) >> shift);
				block[6][28] = Clip3(-(1 << 15), (1 << 15) - 1, (E[3] - O[3] + rnd_factor) >> shift);
				block[23][13] = Clip3(-(1 << 15), (1 << 15) - 1, (E[13] + O[13] + rnd_factor) >> shift);
				block[7][29] = Clip3(-(1 << 15), (1 << 15) - 1, (E[2] - O[2] + rnd_factor) >> shift);
				block[24][14] = Clip3(-(1 << 15), (1 << 15) - 1, (E[14] + O[14] + rnd_factor) >> shift);
				block[8][30] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] - O[1] + rnd_factor) >> shift);
				block[25][15] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15] + O[15] + rnd_factor) >> shift);
				block[9][31] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] - O[0] + rnd_factor) >> shift);
				}
				else if(i == 11){
				block[11][0] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] + O[0] + rnd_factor) >> shift);
				block[27][16] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15] - O[15] + rnd_factor) >> shift);
				block[12][1] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] + O[1] + rnd_factor) >> shift);
				block[28][17] = Clip3(-(1 << 15), (1 << 15) - 1, (E[14] - O[14] + rnd_factor) >> shift);
				block[13][2] = Clip3(-(1 << 15), (1 << 15) - 1, (E[2] + O[2] + rnd_factor) >> shift);
				block[29][18] = Clip3(-(1 << 15), (1 << 15) - 1, (E[13] - O[13] + rnd_factor) >> shift);
				block[14][3] = Clip3(-(1 << 15), (1 << 15) - 1, (E[3] + O[3] + rnd_factor) >> shift);
				block[30][19] = Clip3(-(1 << 15), (1 << 15) - 1, (E[12] - O[12] + rnd_factor) >> shift);
				block[15][4] = Clip3(-(1 << 15), (1 << 15) - 1, (E[4] + O[4] + rnd_factor) >> shift);
				block[31][20] = Clip3(-(1 << 15), (1 << 15) - 1, (E[11] - O[11] + rnd_factor) >> shift);
				block[16][5] = Clip3(-(1 << 15), (1 << 15) - 1, (E[5] + O[5] + rnd_factor) >> shift);
				block[0][21] = Clip3(-(1 << 15), (1 << 15) - 1, (E[10] - O[10] + rnd_factor) >> shift);
				block[17][6] = Clip3(-(1 << 15), (1 << 15) - 1, (E[6] + O[6] + rnd_factor) >> shift);
				block[1][22] = Clip3(-(1 << 15), (1 << 15) - 1, (E[9] - O[9] + rnd_factor) >> shift);
				block[18][7] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7] + O[7] + rnd_factor) >> shift);
				block[2][23] = Clip3(-(1 << 15), (1 << 15) - 1, (E[8] - O[8] + rnd_factor) >> shift);
				block[19][8] = Clip3(-(1 << 15), (1 << 15) - 1, (E[8] + O[8] + rnd_factor) >> shift);
				block[3][24] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7] - O[7] + rnd_factor) >> shift);
				block[20][9] = Clip3(-(1 << 15), (1 << 15) - 1, (E[9] + O[9] + rnd_factor) >> shift);
				block[4][25] = Clip3(-(1 << 15), (1 << 15) - 1, (E[6] - O[6] + rnd_factor) >> shift);
				block[21][10] = Clip3(-(1 << 15), (1 << 15) - 1, (E[10] + O[10] + rnd_factor) >> shift);
				block[5][26] = Clip3(-(1 << 15), (1 << 15) - 1, (E[5] - O[5] + rnd_factor) >> shift);
				block[22][11] = Clip3(-(1 << 15), (1 << 15) - 1, (E[11] + O[11] + rnd_factor) >> shift);
				block[6][27] = Clip3(-(1 << 15), (1 << 15) - 1, (E[4] - O[4] + rnd_factor) >> shift);
				block[23][12] = Clip3(-(1 << 15), (1 << 15) - 1, (E[12] + O[12] + rnd_factor) >> shift);
				block[7][28] = Clip3(-(1 << 15), (1 << 15) - 1, (E[3] - O[3] + rnd_factor) >> shift);
				block[24][13] = Clip3(-(1 << 15), (1 << 15) - 1, (E[13] + O[13] + rnd_factor) >> shift);
				block[8][29] = Clip3(-(1 << 15), (1 << 15) - 1, (E[2] - O[2] + rnd_factor) >> shift);
				block[25][14] = Clip3(-(1 << 15), (1 << 15) - 1, (E[14] + O[14] + rnd_factor) >> shift);
				block[9][30] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] - O[1] + rnd_factor) >> shift);
				block[26][15] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15] + O[15] + rnd_factor) >> shift);
				block[10][31] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] - O[0] + rnd_factor) >> shift);
				}
				else if(i == 12){
				block[12][0] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] + O[0] + rnd_factor) >> shift);
				block[28][16] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15] - O[15] + rnd_factor) >> shift);
				block[13][1] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] + O[1] + rnd_factor) >> shift);
				block[29][17] = Clip3(-(1 << 15), (1 << 15) - 1, (E[14] - O[14] + rnd_factor) >> shift);
				block[14][2] = Clip3(-(1 << 15), (1 << 15) - 1, (E[2] + O[2] + rnd_factor) >> shift);
				block[30][18] = Clip3(-(1 << 15), (1 << 15) - 1, (E[13] - O[13] + rnd_factor) >> shift);
				block[15][3] = Clip3(-(1 << 15), (1 << 15) - 1, (E[3] + O[3] + rnd_factor) >> shift);
				block[31][19] = Clip3(-(1 << 15), (1 << 15) - 1, (E[12] - O[12] + rnd_factor) >> shift);
				block[16][4] = Clip3(-(1 << 15), (1 << 15) - 1, (E[4] + O[4] + rnd_factor) >> shift);
				block[0][20] = Clip3(-(1 << 15), (1 << 15) - 1, (E[11] - O[11] + rnd_factor) >> shift);
				block[17][5] = Clip3(-(1 << 15), (1 << 15) - 1, (E[5] + O[5] + rnd_factor) >> shift);
				block[1][21] = Clip3(-(1 << 15), (1 << 15) - 1, (E[10] - O[10] + rnd_factor) >> shift);
				block[18][6] = Clip3(-(1 << 15), (1 << 15) - 1, (E[6] + O[6] + rnd_factor) >> shift);
				block[2][22] = Clip3(-(1 << 15), (1 << 15) - 1, (E[9] - O[9] + rnd_factor) >> shift);
				block[19][7] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7] + O[7] + rnd_factor) >> shift);
				block[3][23] = Clip3(-(1 << 15), (1 << 15) - 1, (E[8] - O[8] + rnd_factor) >> shift);
				block[20][8] = Clip3(-(1 << 15), (1 << 15) - 1, (E[8] + O[8] + rnd_factor) >> shift);
				block[4][24] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7] - O[7] + rnd_factor) >> shift);
				block[21][9] = Clip3(-(1 << 15), (1 << 15) - 1, (E[9] + O[9] + rnd_factor) >> shift);
				block[5][25] = Clip3(-(1 << 15), (1 << 15) - 1, (E[6] - O[6] + rnd_factor) >> shift);
				block[22][10] = Clip3(-(1 << 15), (1 << 15) - 1, (E[10] + O[10] + rnd_factor) >> shift);
				block[6][26] = Clip3(-(1 << 15), (1 << 15) - 1, (E[5] - O[5] + rnd_factor) >> shift);
				block[23][11] = Clip3(-(1 << 15), (1 << 15) - 1, (E[11] + O[11] + rnd_factor) >> shift);
				block[7][27] = Clip3(-(1 << 15), (1 << 15) - 1, (E[4] - O[4] + rnd_factor) >> shift);
				block[24][12] = Clip3(-(1 << 15), (1 << 15) - 1, (E[12] + O[12] + rnd_factor) >> shift);
				block[8][28] = Clip3(-(1 << 15), (1 << 15) - 1, (E[3] - O[3] + rnd_factor) >> shift);
				block[25][13] = Clip3(-(1 << 15), (1 << 15) - 1, (E[13] + O[13] + rnd_factor) >> shift);
				block[9][29] = Clip3(-(1 << 15), (1 << 15) - 1, (E[2] - O[2] + rnd_factor) >> shift);
				block[26][14] = Clip3(-(1 << 15), (1 << 15) - 1, (E[14] + O[14] + rnd_factor) >> shift);
				block[10][30] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] - O[1] + rnd_factor) >> shift);
				block[27][15] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15] + O[15] + rnd_factor) >> shift);
				block[11][31] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] - O[0] + rnd_factor) >> shift);
				}
				else if(i == 13){
				block[13][0] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] + O[0] + rnd_factor) >> shift);
				block[29][16] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15] - O[15] + rnd_factor) >> shift);
				block[14][1] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] + O[1] + rnd_factor) >> shift);
				block[30][17] = Clip3(-(1 << 15), (1 << 15) - 1, (E[14] - O[14] + rnd_factor) >> shift);
				block[15][2] = Clip3(-(1 << 15), (1 << 15) - 1, (E[2] + O[2] + rnd_factor) >> shift);
				block[31][18] = Clip3(-(1 << 15), (1 << 15) - 1, (E[13] - O[13] + rnd_factor) >> shift);
				block[16][3] = Clip3(-(1 << 15), (1 << 15) - 1, (E[3] + O[3] + rnd_factor) >> shift);
				block[0][19] = Clip3(-(1 << 15), (1 << 15) - 1, (E[12] - O[12] + rnd_factor) >> shift);
				block[17][4] = Clip3(-(1 << 15), (1 << 15) - 1, (E[4] + O[4] + rnd_factor) >> shift);
				block[1][20] = Clip3(-(1 << 15), (1 << 15) - 1, (E[11] - O[11] + rnd_factor) >> shift);
				block[18][5] = Clip3(-(1 << 15), (1 << 15) - 1, (E[5] + O[5] + rnd_factor) >> shift);
				block[2][21] = Clip3(-(1 << 15), (1 << 15) - 1, (E[10] - O[10] + rnd_factor) >> shift);
				block[19][6] = Clip3(-(1 << 15), (1 << 15) - 1, (E[6] + O[6] + rnd_factor) >> shift);
				block[3][22] = Clip3(-(1 << 15), (1 << 15) - 1, (E[9] - O[9] + rnd_factor) >> shift);
				block[20][7] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7] + O[7] + rnd_factor) >> shift);
				block[4][23] = Clip3(-(1 << 15), (1 << 15) - 1, (E[8] - O[8] + rnd_factor) >> shift);
				block[21][8] = Clip3(-(1 << 15), (1 << 15) - 1, (E[8] + O[8] + rnd_factor) >> shift);
				block[5][24] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7] - O[7] + rnd_factor) >> shift);
				block[22][9] = Clip3(-(1 << 15), (1 << 15) - 1, (E[9] + O[9] + rnd_factor) >> shift);
				block[6][25] = Clip3(-(1 << 15), (1 << 15) - 1, (E[6] - O[6] + rnd_factor) >> shift);
				block[23][10] = Clip3(-(1 << 15), (1 << 15) - 1, (E[10] + O[10] + rnd_factor) >> shift);
				block[7][26] = Clip3(-(1 << 15), (1 << 15) - 1, (E[5] - O[5] + rnd_factor) >> shift);
				block[24][11] = Clip3(-(1 << 15), (1 << 15) - 1, (E[11] + O[11] + rnd_factor) >> shift);
				block[8][27] = Clip3(-(1 << 15), (1 << 15) - 1, (E[4] - O[4] + rnd_factor) >> shift);
				block[25][12] = Clip3(-(1 << 15), (1 << 15) - 1, (E[12] + O[12] + rnd_factor) >> shift);
				block[9][28] = Clip3(-(1 << 15), (1 << 15) - 1, (E[3] - O[3] + rnd_factor) >> shift);
				block[26][13] = Clip3(-(1 << 15), (1 << 15) - 1, (E[13] + O[13] + rnd_factor) >> shift);
				block[10][29] = Clip3(-(1 << 15), (1 << 15) - 1, (E[2] - O[2] + rnd_factor) >> shift);
				block[27][14] = Clip3(-(1 << 15), (1 << 15) - 1, (E[14] + O[14] + rnd_factor) >> shift);
				block[11][30] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] - O[1] + rnd_factor) >> shift);
				block[28][15] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15] + O[15] + rnd_factor) >> shift);
				block[12][31] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] - O[0] + rnd_factor) >> shift);
				}
				else if(i == 14){
				block[14][0] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] + O[0] + rnd_factor) >> shift);
				block[30][16] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15] - O[15] + rnd_factor) >> shift);
				block[15][1] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] + O[1] + rnd_factor) >> shift);
				block[31][17] = Clip3(-(1 << 15), (1 << 15) - 1, (E[14] - O[14] + rnd_factor) >> shift);
				block[16][2] = Clip3(-(1 << 15), (1 << 15) - 1, (E[2] + O[2] + rnd_factor) >> shift);
				block[0][18] = Clip3(-(1 << 15), (1 << 15) - 1, (E[13] - O[13] + rnd_factor) >> shift);
				block[17][3] = Clip3(-(1 << 15), (1 << 15) - 1, (E[3] + O[3] + rnd_factor) >> shift);
				block[1][19] = Clip3(-(1 << 15), (1 << 15) - 1, (E[12] - O[12] + rnd_factor) >> shift);
				block[18][4] = Clip3(-(1 << 15), (1 << 15) - 1, (E[4] + O[4] + rnd_factor) >> shift);
				block[2][20] = Clip3(-(1 << 15), (1 << 15) - 1, (E[11] - O[11] + rnd_factor) >> shift);
				block[19][5] = Clip3(-(1 << 15), (1 << 15) - 1, (E[5] + O[5] + rnd_factor) >> shift);
				block[3][21] = Clip3(-(1 << 15), (1 << 15) - 1, (E[10] - O[10] + rnd_factor) >> shift);
				block[20][6] = Clip3(-(1 << 15), (1 << 15) - 1, (E[6] + O[6] + rnd_factor) >> shift);
				block[4][22] = Clip3(-(1 << 15), (1 << 15) - 1, (E[9] - O[9] + rnd_factor) >> shift);
				block[21][7] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7] + O[7] + rnd_factor) >> shift);
				block[5][23] = Clip3(-(1 << 15), (1 << 15) - 1, (E[8] - O[8] + rnd_factor) >> shift);
				block[22][8] = Clip3(-(1 << 15), (1 << 15) - 1, (E[8] + O[8] + rnd_factor) >> shift);
				block[6][24] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7] - O[7] + rnd_factor) >> shift);
				block[23][9] = Clip3(-(1 << 15), (1 << 15) - 1, (E[9] + O[9] + rnd_factor) >> shift);
				block[7][25] = Clip3(-(1 << 15), (1 << 15) - 1, (E[6] - O[6] + rnd_factor) >> shift);
				block[24][10] = Clip3(-(1 << 15), (1 << 15) - 1, (E[10] + O[10] + rnd_factor) >> shift);
				block[8][26] = Clip3(-(1 << 15), (1 << 15) - 1, (E[5] - O[5] + rnd_factor) >> shift);
				block[25][11] = Clip3(-(1 << 15), (1 << 15) - 1, (E[11] + O[11] + rnd_factor) >> shift);
				block[9][27] = Clip3(-(1 << 15), (1 << 15) - 1, (E[4] - O[4] + rnd_factor) >> shift);
				block[26][12] = Clip3(-(1 << 15), (1 << 15) - 1, (E[12] + O[12] + rnd_factor) >> shift);
				block[10][28] = Clip3(-(1 << 15), (1 << 15) - 1, (E[3] - O[3] + rnd_factor) >> shift);
				block[27][13] = Clip3(-(1 << 15), (1 << 15) - 1, (E[13] + O[13] + rnd_factor) >> shift);
				block[11][29] = Clip3(-(1 << 15), (1 << 15) - 1, (E[2] - O[2] + rnd_factor) >> shift);
				block[28][14] = Clip3(-(1 << 15), (1 << 15) - 1, (E[14] + O[14] + rnd_factor) >> shift);
				block[12][30] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] - O[1] + rnd_factor) >> shift);
				block[29][15] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15] + O[15] + rnd_factor) >> shift);
				block[13][31] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] - O[0] + rnd_factor) >> shift);
				}
				else if(i == 15){
				block[15][0] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] + O[0] + rnd_factor) >> shift);
				block[31][16] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15] - O[15] + rnd_factor) >> shift);
				block[16][1] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] + O[1] + rnd_factor) >> shift);
				block[0][17] = Clip3(-(1 << 15), (1 << 15) - 1, (E[14] - O[14] + rnd_factor) >> shift);
				block[17][2] = Clip3(-(1 << 15), (1 << 15) - 1, (E[2] + O[2] + rnd_factor) >> shift);
				block[1][18] = Clip3(-(1 << 15), (1 << 15) - 1, (E[13] - O[13] + rnd_factor) >> shift);
				block[18][3] = Clip3(-(1 << 15), (1 << 15) - 1, (E[3] + O[3] + rnd_factor) >> shift);
				block[2][19] = Clip3(-(1 << 15), (1 << 15) - 1, (E[12] - O[12] + rnd_factor) >> shift);
				block[19][4] = Clip3(-(1 << 15), (1 << 15) - 1, (E[4] + O[4] + rnd_factor) >> shift);
				block[3][20] = Clip3(-(1 << 15), (1 << 15) - 1, (E[11] - O[11] + rnd_factor) >> shift);
				block[20][5] = Clip3(-(1 << 15), (1 << 15) - 1, (E[5] + O[5] + rnd_factor) >> shift);
				block[4][21] = Clip3(-(1 << 15), (1 << 15) - 1, (E[10] - O[10] + rnd_factor) >> shift);
				block[21][6] = Clip3(-(1 << 15), (1 << 15) - 1, (E[6] + O[6] + rnd_factor) >> shift);
				block[5][22] = Clip3(-(1 << 15), (1 << 15) - 1, (E[9] - O[9] + rnd_factor) >> shift);
				block[22][7] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7] + O[7] + rnd_factor) >> shift);
				block[6][23] = Clip3(-(1 << 15), (1 << 15) - 1, (E[8] - O[8] + rnd_factor) >> shift);
				block[23][8] = Clip3(-(1 << 15), (1 << 15) - 1, (E[8] + O[8] + rnd_factor) >> shift);
				block[7][24] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7] - O[7] + rnd_factor) >> shift);
				block[24][9] = Clip3(-(1 << 15), (1 << 15) - 1, (E[9] + O[9] + rnd_factor) >> shift);
				block[8][25] = Clip3(-(1 << 15), (1 << 15) - 1, (E[6] - O[6] + rnd_factor) >> shift);
				block[25][10] = Clip3(-(1 << 15), (1 << 15) - 1, (E[10] + O[10] + rnd_factor) >> shift);
				block[9][26] = Clip3(-(1 << 15), (1 << 15) - 1, (E[5] - O[5] + rnd_factor) >> shift);
				block[26][11] = Clip3(-(1 << 15), (1 << 15) - 1, (E[11] + O[11] + rnd_factor) >> shift);
				block[10][27] = Clip3(-(1 << 15), (1 << 15) - 1, (E[4] - O[4] + rnd_factor) >> shift);
				block[27][12] = Clip3(-(1 << 15), (1 << 15) - 1, (E[12] + O[12] + rnd_factor) >> shift);
				block[11][28] = Clip3(-(1 << 15), (1 << 15) - 1, (E[3] - O[3] + rnd_factor) >> shift);
				block[28][13] = Clip3(-(1 << 15), (1 << 15) - 1, (E[13] + O[13] + rnd_factor) >> shift);
				block[12][29] = Clip3(-(1 << 15), (1 << 15) - 1, (E[2] - O[2] + rnd_factor) >> shift);
				block[29][14] = Clip3(-(1 << 15), (1 << 15) - 1, (E[14] + O[14] + rnd_factor) >> shift);
				block[13][30] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] - O[1] + rnd_factor) >> shift);
				block[30][15] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15] + O[15] + rnd_factor) >> shift);
				block[14][31] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] - O[0] + rnd_factor) >> shift);
				}
				else if(i == 16){
				block[16][0] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] + O[0] + rnd_factor) >> shift);
				block[0][16] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15] - O[15] + rnd_factor) >> shift);
				block[17][1] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] + O[1] + rnd_factor) >> shift);
				block[1][17] = Clip3(-(1 << 15), (1 << 15) - 1, (E[14] - O[14] + rnd_factor) >> shift);
				block[18][2] = Clip3(-(1 << 15), (1 << 15) - 1, (E[2] + O[2] + rnd_factor) >> shift);
				block[2][18] = Clip3(-(1 << 15), (1 << 15) - 1, (E[13] - O[13] + rnd_factor) >> shift);
				block[19][3] = Clip3(-(1 << 15), (1 << 15) - 1, (E[3] + O[3] + rnd_factor) >> shift);
				block[3][19] = Clip3(-(1 << 15), (1 << 15) - 1, (E[12] - O[12] + rnd_factor) >> shift);
				block[20][4] = Clip3(-(1 << 15), (1 << 15) - 1, (E[4] + O[4] + rnd_factor) >> shift);
				block[4][20] = Clip3(-(1 << 15), (1 << 15) - 1, (E[11] - O[11] + rnd_factor) >> shift);
				block[21][5] = Clip3(-(1 << 15), (1 << 15) - 1, (E[5] + O[5] + rnd_factor) >> shift);
				block[5][21] = Clip3(-(1 << 15), (1 << 15) - 1, (E[10] - O[10] + rnd_factor) >> shift);
				block[22][6] = Clip3(-(1 << 15), (1 << 15) - 1, (E[6] + O[6] + rnd_factor) >> shift);
				block[6][22] = Clip3(-(1 << 15), (1 << 15) - 1, (E[9] - O[9] + rnd_factor) >> shift);
				block[23][7] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7] + O[7] + rnd_factor) >> shift);
				block[7][23] = Clip3(-(1 << 15), (1 << 15) - 1, (E[8] - O[8] + rnd_factor) >> shift);
				block[24][8] = Clip3(-(1 << 15), (1 << 15) - 1, (E[8] + O[8] + rnd_factor) >> shift);
				block[8][24] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7] - O[7] + rnd_factor) >> shift);
				block[25][9] = Clip3(-(1 << 15), (1 << 15) - 1, (E[9] + O[9] + rnd_factor) >> shift);
				block[9][25] = Clip3(-(1 << 15), (1 << 15) - 1, (E[6] - O[6] + rnd_factor) >> shift);
				block[26][10] = Clip3(-(1 << 15), (1 << 15) - 1, (E[10] + O[10] + rnd_factor) >> shift);
				block[10][26] = Clip3(-(1 << 15), (1 << 15) - 1, (E[5] - O[5] + rnd_factor) >> shift);
				block[27][11] = Clip3(-(1 << 15), (1 << 15) - 1, (E[11] + O[11] + rnd_factor) >> shift);
				block[11][27] = Clip3(-(1 << 15), (1 << 15) - 1, (E[4] - O[4] + rnd_factor) >> shift);
				block[28][12] = Clip3(-(1 << 15), (1 << 15) - 1, (E[12] + O[12] + rnd_factor) >> shift);
				block[12][28] = Clip3(-(1 << 15), (1 << 15) - 1, (E[3] - O[3] + rnd_factor) >> shift);
				block[29][13] = Clip3(-(1 << 15), (1 << 15) - 1, (E[13] + O[13] + rnd_factor) >> shift);
				block[13][29] = Clip3(-(1 << 15), (1 << 15) - 1, (E[2] - O[2] + rnd_factor) >> shift);
				block[30][14] = Clip3(-(1 << 15), (1 << 15) - 1, (E[14] + O[14] + rnd_factor) >> shift);
				block[14][30] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] - O[1] + rnd_factor) >> shift);
				block[31][15] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15] + O[15] + rnd_factor) >> shift);
				block[15][31] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] - O[0] + rnd_factor) >> shift);
				}
				else if(i == 17){
				block[17][0] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] + O[0] + rnd_factor) >> shift);
				block[1][16] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15] - O[15] + rnd_factor) >> shift);
				block[18][1] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] + O[1] + rnd_factor) >> shift);
				block[2][17] = Clip3(-(1 << 15), (1 << 15) - 1, (E[14] - O[14] + rnd_factor) >> shift);
				block[19][2] = Clip3(-(1 << 15), (1 << 15) - 1, (E[2] + O[2] + rnd_factor) >> shift);
				block[3][18] = Clip3(-(1 << 15), (1 << 15) - 1, (E[13] - O[13] + rnd_factor) >> shift);
				block[20][3] = Clip3(-(1 << 15), (1 << 15) - 1, (E[3] + O[3] + rnd_factor) >> shift);
				block[4][19] = Clip3(-(1 << 15), (1 << 15) - 1, (E[12] - O[12] + rnd_factor) >> shift);
				block[21][4] = Clip3(-(1 << 15), (1 << 15) - 1, (E[4] + O[4] + rnd_factor) >> shift);
				block[5][20] = Clip3(-(1 << 15), (1 << 15) - 1, (E[11] - O[11] + rnd_factor) >> shift);
				block[22][5] = Clip3(-(1 << 15), (1 << 15) - 1, (E[5] + O[5] + rnd_factor) >> shift);
				block[6][21] = Clip3(-(1 << 15), (1 << 15) - 1, (E[10] - O[10] + rnd_factor) >> shift);
				block[23][6] = Clip3(-(1 << 15), (1 << 15) - 1, (E[6] + O[6] + rnd_factor) >> shift);
				block[7][22] = Clip3(-(1 << 15), (1 << 15) - 1, (E[9] - O[9] + rnd_factor) >> shift);
				block[24][7] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7] + O[7] + rnd_factor) >> shift);
				block[8][23] = Clip3(-(1 << 15), (1 << 15) - 1, (E[8] - O[8] + rnd_factor) >> shift);
				block[25][8] = Clip3(-(1 << 15), (1 << 15) - 1, (E[8] + O[8] + rnd_factor) >> shift);
				block[9][24] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7] - O[7] + rnd_factor) >> shift);
				block[26][9] = Clip3(-(1 << 15), (1 << 15) - 1, (E[9] + O[9] + rnd_factor) >> shift);
				block[10][25] = Clip3(-(1 << 15), (1 << 15) - 1, (E[6] - O[6] + rnd_factor) >> shift);
				block[27][10] = Clip3(-(1 << 15), (1 << 15) - 1, (E[10] + O[10] + rnd_factor) >> shift);
				block[11][26] = Clip3(-(1 << 15), (1 << 15) - 1, (E[5] - O[5] + rnd_factor) >> shift);
				block[28][11] = Clip3(-(1 << 15), (1 << 15) - 1, (E[11] + O[11] + rnd_factor) >> shift);
				block[12][27] = Clip3(-(1 << 15), (1 << 15) - 1, (E[4] - O[4] + rnd_factor) >> shift);
				block[29][12] = Clip3(-(1 << 15), (1 << 15) - 1, (E[12] + O[12] + rnd_factor) >> shift);
				block[13][28] = Clip3(-(1 << 15), (1 << 15) - 1, (E[3] - O[3] + rnd_factor) >> shift);
				block[30][13] = Clip3(-(1 << 15), (1 << 15) - 1, (E[13] + O[13] + rnd_factor) >> shift);
				block[14][29] = Clip3(-(1 << 15), (1 << 15) - 1, (E[2] - O[2] + rnd_factor) >> shift);
				block[31][14] = Clip3(-(1 << 15), (1 << 15) - 1, (E[14] + O[14] + rnd_factor) >> shift);
				block[15][30] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] - O[1] + rnd_factor) >> shift);
				block[0][15] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15] + O[15] + rnd_factor) >> shift);
				block[16][31] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] - O[0] + rnd_factor) >> shift);
				}
				else if(i == 18){
				block[18][0] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] + O[0] + rnd_factor) >> shift);
				block[2][16] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15] - O[15] + rnd_factor) >> shift);
				block[19][1] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] + O[1] + rnd_factor) >> shift);
				block[3][17] = Clip3(-(1 << 15), (1 << 15) - 1, (E[14] - O[14] + rnd_factor) >> shift);
				block[20][2] = Clip3(-(1 << 15), (1 << 15) - 1, (E[2] + O[2] + rnd_factor) >> shift);
				block[4][18] = Clip3(-(1 << 15), (1 << 15) - 1, (E[13] - O[13] + rnd_factor) >> shift);
				block[21][3] = Clip3(-(1 << 15), (1 << 15) - 1, (E[3] + O[3] + rnd_factor) >> shift);
				block[5][19] = Clip3(-(1 << 15), (1 << 15) - 1, (E[12] - O[12] + rnd_factor) >> shift);
				block[22][4] = Clip3(-(1 << 15), (1 << 15) - 1, (E[4] + O[4] + rnd_factor) >> shift);
				block[6][20] = Clip3(-(1 << 15), (1 << 15) - 1, (E[11] - O[11] + rnd_factor) >> shift);
				block[23][5] = Clip3(-(1 << 15), (1 << 15) - 1, (E[5] + O[5] + rnd_factor) >> shift);
				block[7][21] = Clip3(-(1 << 15), (1 << 15) - 1, (E[10] - O[10] + rnd_factor) >> shift);
				block[24][6] = Clip3(-(1 << 15), (1 << 15) - 1, (E[6] + O[6] + rnd_factor) >> shift);
				block[8][22] = Clip3(-(1 << 15), (1 << 15) - 1, (E[9] - O[9] + rnd_factor) >> shift);
				block[25][7] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7] + O[7] + rnd_factor) >> shift);
				block[9][23] = Clip3(-(1 << 15), (1 << 15) - 1, (E[8] - O[8] + rnd_factor) >> shift);
				block[26][8] = Clip3(-(1 << 15), (1 << 15) - 1, (E[8] + O[8] + rnd_factor) >> shift);
				block[10][24] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7] - O[7] + rnd_factor) >> shift);
				block[27][9] = Clip3(-(1 << 15), (1 << 15) - 1, (E[9] + O[9] + rnd_factor) >> shift);
				block[11][25] = Clip3(-(1 << 15), (1 << 15) - 1, (E[6] - O[6] + rnd_factor) >> shift);
				block[28][10] = Clip3(-(1 << 15), (1 << 15) - 1, (E[10] + O[10] + rnd_factor) >> shift);
				block[12][26] = Clip3(-(1 << 15), (1 << 15) - 1, (E[5] - O[5] + rnd_factor) >> shift);
				block[29][11] = Clip3(-(1 << 15), (1 << 15) - 1, (E[11] + O[11] + rnd_factor) >> shift);
				block[13][27] = Clip3(-(1 << 15), (1 << 15) - 1, (E[4] - O[4] + rnd_factor) >> shift);
				block[30][12] = Clip3(-(1 << 15), (1 << 15) - 1, (E[12] + O[12] + rnd_factor) >> shift);
				block[14][28] = Clip3(-(1 << 15), (1 << 15) - 1, (E[3] - O[3] + rnd_factor) >> shift);
				block[31][13] = Clip3(-(1 << 15), (1 << 15) - 1, (E[13] + O[13] + rnd_factor) >> shift);
				block[15][29] = Clip3(-(1 << 15), (1 << 15) - 1, (E[2] - O[2] + rnd_factor) >> shift);
				block[0][14] = Clip3(-(1 << 15), (1 << 15) - 1, (E[14] + O[14] + rnd_factor) >> shift);
				block[16][30] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] - O[1] + rnd_factor) >> shift);
				block[1][15] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15] + O[15] + rnd_factor) >> shift);
				block[17][31] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] - O[0] + rnd_factor) >> shift);
				}
				else if(i == 19){
				block[19][0] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] + O[0] + rnd_factor) >> shift);
				block[3][16] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15] - O[15] + rnd_factor) >> shift);
				block[20][1] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] + O[1] + rnd_factor) >> shift);
				block[4][17] = Clip3(-(1 << 15), (1 << 15) - 1, (E[14] - O[14] + rnd_factor) >> shift);
				block[21][2] = Clip3(-(1 << 15), (1 << 15) - 1, (E[2] + O[2] + rnd_factor) >> shift);
				block[5][18] = Clip3(-(1 << 15), (1 << 15) - 1, (E[13] - O[13] + rnd_factor) >> shift);
				block[22][3] = Clip3(-(1 << 15), (1 << 15) - 1, (E[3] + O[3] + rnd_factor) >> shift);
				block[6][19] = Clip3(-(1 << 15), (1 << 15) - 1, (E[12] - O[12] + rnd_factor) >> shift);
				block[23][4] = Clip3(-(1 << 15), (1 << 15) - 1, (E[4] + O[4] + rnd_factor) >> shift);
				block[7][20] = Clip3(-(1 << 15), (1 << 15) - 1, (E[11] - O[11] + rnd_factor) >> shift);
				block[24][5] = Clip3(-(1 << 15), (1 << 15) - 1, (E[5] + O[5] + rnd_factor) >> shift);
				block[8][21] = Clip3(-(1 << 15), (1 << 15) - 1, (E[10] - O[10] + rnd_factor) >> shift);
				block[25][6] = Clip3(-(1 << 15), (1 << 15) - 1, (E[6] + O[6] + rnd_factor) >> shift);
				block[9][22] = Clip3(-(1 << 15), (1 << 15) - 1, (E[9] - O[9] + rnd_factor) >> shift);
				block[26][7] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7] + O[7] + rnd_factor) >> shift);
				block[10][23] = Clip3(-(1 << 15), (1 << 15) - 1, (E[8] - O[8] + rnd_factor) >> shift);
				block[27][8] = Clip3(-(1 << 15), (1 << 15) - 1, (E[8] + O[8] + rnd_factor) >> shift);
				block[11][24] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7] - O[7] + rnd_factor) >> shift);
				block[28][9] = Clip3(-(1 << 15), (1 << 15) - 1, (E[9] + O[9] + rnd_factor) >> shift);
				block[12][25] = Clip3(-(1 << 15), (1 << 15) - 1, (E[6] - O[6] + rnd_factor) >> shift);
				block[29][10] = Clip3(-(1 << 15), (1 << 15) - 1, (E[10] + O[10] + rnd_factor) >> shift);
				block[13][26] = Clip3(-(1 << 15), (1 << 15) - 1, (E[5] - O[5] + rnd_factor) >> shift);
				block[30][11] = Clip3(-(1 << 15), (1 << 15) - 1, (E[11] + O[11] + rnd_factor) >> shift);
				block[14][27] = Clip3(-(1 << 15), (1 << 15) - 1, (E[4] - O[4] + rnd_factor) >> shift);
				block[31][12] = Clip3(-(1 << 15), (1 << 15) - 1, (E[12] + O[12] + rnd_factor) >> shift);
				block[15][28] = Clip3(-(1 << 15), (1 << 15) - 1, (E[3] - O[3] + rnd_factor) >> shift);
				block[0][13] = Clip3(-(1 << 15), (1 << 15) - 1, (E[13] + O[13] + rnd_factor) >> shift);
				block[16][29] = Clip3(-(1 << 15), (1 << 15) - 1, (E[2] - O[2] + rnd_factor) >> shift);
				block[1][14] = Clip3(-(1 << 15), (1 << 15) - 1, (E[14] + O[14] + rnd_factor) >> shift);
				block[17][30] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] - O[1] + rnd_factor) >> shift);
				block[2][15] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15] + O[15] + rnd_factor) >> shift);
				block[18][31] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] - O[0] + rnd_factor) >> shift);
				}
				else if(i == 20){
				block[20][0] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] + O[0] + rnd_factor) >> shift);
				block[4][16] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15] - O[15] + rnd_factor) >> shift);
				block[21][1] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] + O[1] + rnd_factor) >> shift);
				block[5][17] = Clip3(-(1 << 15), (1 << 15) - 1, (E[14] - O[14] + rnd_factor) >> shift);
				block[22][2] = Clip3(-(1 << 15), (1 << 15) - 1, (E[2] + O[2] + rnd_factor) >> shift);
				block[6][18] = Clip3(-(1 << 15), (1 << 15) - 1, (E[13] - O[13] + rnd_factor) >> shift);
				block[23][3] = Clip3(-(1 << 15), (1 << 15) - 1, (E[3] + O[3] + rnd_factor) >> shift);
				block[7][19] = Clip3(-(1 << 15), (1 << 15) - 1, (E[12] - O[12] + rnd_factor) >> shift);
				block[24][4] = Clip3(-(1 << 15), (1 << 15) - 1, (E[4] + O[4] + rnd_factor) >> shift);
				block[8][20] = Clip3(-(1 << 15), (1 << 15) - 1, (E[11] - O[11] + rnd_factor) >> shift);
				block[25][5] = Clip3(-(1 << 15), (1 << 15) - 1, (E[5] + O[5] + rnd_factor) >> shift);
				block[9][21] = Clip3(-(1 << 15), (1 << 15) - 1, (E[10] - O[10] + rnd_factor) >> shift);
				block[26][6] = Clip3(-(1 << 15), (1 << 15) - 1, (E[6] + O[6] + rnd_factor) >> shift);
				block[10][22] = Clip3(-(1 << 15), (1 << 15) - 1, (E[9] - O[9] + rnd_factor) >> shift);
				block[27][7] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7] + O[7] + rnd_factor) >> shift);
				block[11][23] = Clip3(-(1 << 15), (1 << 15) - 1, (E[8] - O[8] + rnd_factor) >> shift);
				block[28][8] = Clip3(-(1 << 15), (1 << 15) - 1, (E[8] + O[8] + rnd_factor) >> shift);
				block[12][24] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7] - O[7] + rnd_factor) >> shift);
				block[29][9] = Clip3(-(1 << 15), (1 << 15) - 1, (E[9] + O[9] + rnd_factor) >> shift);
				block[13][25] = Clip3(-(1 << 15), (1 << 15) - 1, (E[6] - O[6] + rnd_factor) >> shift);
				block[30][10] = Clip3(-(1 << 15), (1 << 15) - 1, (E[10] + O[10] + rnd_factor) >> shift);
				block[14][26] = Clip3(-(1 << 15), (1 << 15) - 1, (E[5] - O[5] + rnd_factor) >> shift);
				block[31][11] = Clip3(-(1 << 15), (1 << 15) - 1, (E[11] + O[11] + rnd_factor) >> shift);
				block[15][27] = Clip3(-(1 << 15), (1 << 15) - 1, (E[4] - O[4] + rnd_factor) >> shift);
				block[0][12] = Clip3(-(1 << 15), (1 << 15) - 1, (E[12] + O[12] + rnd_factor) >> shift);
				block[16][28] = Clip3(-(1 << 15), (1 << 15) - 1, (E[3] - O[3] + rnd_factor) >> shift);
				block[1][13] = Clip3(-(1 << 15), (1 << 15) - 1, (E[13] + O[13] + rnd_factor) >> shift);
				block[17][29] = Clip3(-(1 << 15), (1 << 15) - 1, (E[2] - O[2] + rnd_factor) >> shift);
				block[2][14] = Clip3(-(1 << 15), (1 << 15) - 1, (E[14] + O[14] + rnd_factor) >> shift);
				block[18][30] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] - O[1] + rnd_factor) >> shift);
				block[3][15] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15] + O[15] + rnd_factor) >> shift);
				block[19][31] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] - O[0] + rnd_factor) >> shift);
				}
				else if(i == 21){
				block[21][0] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] + O[0] + rnd_factor) >> shift);
				block[5][16] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15] - O[15] + rnd_factor) >> shift);
				block[22][1] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] + O[1] + rnd_factor) >> shift);
				block[6][17] = Clip3(-(1 << 15), (1 << 15) - 1, (E[14] - O[14] + rnd_factor) >> shift);
				block[23][2] = Clip3(-(1 << 15), (1 << 15) - 1, (E[2] + O[2] + rnd_factor) >> shift);
				block[7][18] = Clip3(-(1 << 15), (1 << 15) - 1, (E[13] - O[13] + rnd_factor) >> shift);
				block[24][3] = Clip3(-(1 << 15), (1 << 15) - 1, (E[3] + O[3] + rnd_factor) >> shift);
				block[8][19] = Clip3(-(1 << 15), (1 << 15) - 1, (E[12] - O[12] + rnd_factor) >> shift);
				block[25][4] = Clip3(-(1 << 15), (1 << 15) - 1, (E[4] + O[4] + rnd_factor) >> shift);
				block[9][20] = Clip3(-(1 << 15), (1 << 15) - 1, (E[11] - O[11] + rnd_factor) >> shift);
				block[26][5] = Clip3(-(1 << 15), (1 << 15) - 1, (E[5] + O[5] + rnd_factor) >> shift);
				block[10][21] = Clip3(-(1 << 15), (1 << 15) - 1, (E[10] - O[10] + rnd_factor) >> shift);
				block[27][6] = Clip3(-(1 << 15), (1 << 15) - 1, (E[6] + O[6] + rnd_factor) >> shift);
				block[11][22] = Clip3(-(1 << 15), (1 << 15) - 1, (E[9] - O[9] + rnd_factor) >> shift);
				block[28][7] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7] + O[7] + rnd_factor) >> shift);
				block[12][23] = Clip3(-(1 << 15), (1 << 15) - 1, (E[8] - O[8] + rnd_factor) >> shift);
				block[29][8] = Clip3(-(1 << 15), (1 << 15) - 1, (E[8] + O[8] + rnd_factor) >> shift);
				block[13][24] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7] - O[7] + rnd_factor) >> shift);
				block[30][9] = Clip3(-(1 << 15), (1 << 15) - 1, (E[9] + O[9] + rnd_factor) >> shift);
				block[14][25] = Clip3(-(1 << 15), (1 << 15) - 1, (E[6] - O[6] + rnd_factor) >> shift);
				block[31][10] = Clip3(-(1 << 15), (1 << 15) - 1, (E[10] + O[10] + rnd_factor) >> shift);
				block[15][26] = Clip3(-(1 << 15), (1 << 15) - 1, (E[5] - O[5] + rnd_factor) >> shift);
				block[0][11] = Clip3(-(1 << 15), (1 << 15) - 1, (E[11] + O[11] + rnd_factor) >> shift);
				block[16][27] = Clip3(-(1 << 15), (1 << 15) - 1, (E[4] - O[4] + rnd_factor) >> shift);
				block[1][12] = Clip3(-(1 << 15), (1 << 15) - 1, (E[12] + O[12] + rnd_factor) >> shift);
				block[17][28] = Clip3(-(1 << 15), (1 << 15) - 1, (E[3] - O[3] + rnd_factor) >> shift);
				block[2][13] = Clip3(-(1 << 15), (1 << 15) - 1, (E[13] + O[13] + rnd_factor) >> shift);
				block[18][29] = Clip3(-(1 << 15), (1 << 15) - 1, (E[2] - O[2] + rnd_factor) >> shift);
				block[3][14] = Clip3(-(1 << 15), (1 << 15) - 1, (E[14] + O[14] + rnd_factor) >> shift);
				block[19][30] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] - O[1] + rnd_factor) >> shift);
				block[4][15] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15] + O[15] + rnd_factor) >> shift);
				block[20][31] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] - O[0] + rnd_factor) >> shift);
				}
				else if(i == 22){
				block[22][0] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] + O[0] + rnd_factor) >> shift);
				block[6][16] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15] - O[15] + rnd_factor) >> shift);
				block[23][1] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] + O[1] + rnd_factor) >> shift);
				block[7][17] = Clip3(-(1 << 15), (1 << 15) - 1, (E[14] - O[14] + rnd_factor) >> shift);
				block[24][2] = Clip3(-(1 << 15), (1 << 15) - 1, (E[2] + O[2] + rnd_factor) >> shift);
				block[8][18] = Clip3(-(1 << 15), (1 << 15) - 1, (E[13] - O[13] + rnd_factor) >> shift);
				block[25][3] = Clip3(-(1 << 15), (1 << 15) - 1, (E[3] + O[3] + rnd_factor) >> shift);
				block[9][19] = Clip3(-(1 << 15), (1 << 15) - 1, (E[12] - O[12] + rnd_factor) >> shift);
				block[26][4] = Clip3(-(1 << 15), (1 << 15) - 1, (E[4] + O[4] + rnd_factor) >> shift);
				block[10][20] = Clip3(-(1 << 15), (1 << 15) - 1, (E[11] - O[11] + rnd_factor) >> shift);
				block[27][5] = Clip3(-(1 << 15), (1 << 15) - 1, (E[5] + O[5] + rnd_factor) >> shift);
				block[11][21] = Clip3(-(1 << 15), (1 << 15) - 1, (E[10] - O[10] + rnd_factor) >> shift);
				block[28][6] = Clip3(-(1 << 15), (1 << 15) - 1, (E[6] + O[6] + rnd_factor) >> shift);
				block[12][22] = Clip3(-(1 << 15), (1 << 15) - 1, (E[9] - O[9] + rnd_factor) >> shift);
				block[29][7] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7] + O[7] + rnd_factor) >> shift);
				block[13][23] = Clip3(-(1 << 15), (1 << 15) - 1, (E[8] - O[8] + rnd_factor) >> shift);
				block[30][8] = Clip3(-(1 << 15), (1 << 15) - 1, (E[8] + O[8] + rnd_factor) >> shift);
				block[14][24] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7] - O[7] + rnd_factor) >> shift);
				block[31][9] = Clip3(-(1 << 15), (1 << 15) - 1, (E[9] + O[9] + rnd_factor) >> shift);
				block[15][25] = Clip3(-(1 << 15), (1 << 15) - 1, (E[6] - O[6] + rnd_factor) >> shift);
				block[0][10] = Clip3(-(1 << 15), (1 << 15) - 1, (E[10] + O[10] + rnd_factor) >> shift);
				block[16][26] = Clip3(-(1 << 15), (1 << 15) - 1, (E[5] - O[5] + rnd_factor) >> shift);
				block[1][11] = Clip3(-(1 << 15), (1 << 15) - 1, (E[11] + O[11] + rnd_factor) >> shift);
				block[17][27] = Clip3(-(1 << 15), (1 << 15) - 1, (E[4] - O[4] + rnd_factor) >> shift);
				block[2][12] = Clip3(-(1 << 15), (1 << 15) - 1, (E[12] + O[12] + rnd_factor) >> shift);
				block[18][28] = Clip3(-(1 << 15), (1 << 15) - 1, (E[3] - O[3] + rnd_factor) >> shift);
				block[3][13] = Clip3(-(1 << 15), (1 << 15) - 1, (E[13] + O[13] + rnd_factor) >> shift);
				block[19][29] = Clip3(-(1 << 15), (1 << 15) - 1, (E[2] - O[2] + rnd_factor) >> shift);
				block[4][14] = Clip3(-(1 << 15), (1 << 15) - 1, (E[14] + O[14] + rnd_factor) >> shift);
				block[20][30] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] - O[1] + rnd_factor) >> shift);
				block[5][15] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15] + O[15] + rnd_factor) >> shift);
				block[21][31] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] - O[0] + rnd_factor) >> shift);
				}
				else if(i == 23){
				block[23][0] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] + O[0] + rnd_factor) >> shift);
				block[7][16] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15] - O[15] + rnd_factor) >> shift);
				block[24][1] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] + O[1] + rnd_factor) >> shift);
				block[8][17] = Clip3(-(1 << 15), (1 << 15) - 1, (E[14] - O[14] + rnd_factor) >> shift);
				block[25][2] = Clip3(-(1 << 15), (1 << 15) - 1, (E[2] + O[2] + rnd_factor) >> shift);
				block[9][18] = Clip3(-(1 << 15), (1 << 15) - 1, (E[13] - O[13] + rnd_factor) >> shift);
				block[26][3] = Clip3(-(1 << 15), (1 << 15) - 1, (E[3] + O[3] + rnd_factor) >> shift);
				block[10][19] = Clip3(-(1 << 15), (1 << 15) - 1, (E[12] - O[12] + rnd_factor) >> shift);
				block[27][4] = Clip3(-(1 << 15), (1 << 15) - 1, (E[4] + O[4] + rnd_factor) >> shift);
				block[11][20] = Clip3(-(1 << 15), (1 << 15) - 1, (E[11] - O[11] + rnd_factor) >> shift);
				block[28][5] = Clip3(-(1 << 15), (1 << 15) - 1, (E[5] + O[5] + rnd_factor) >> shift);
				block[12][21] = Clip3(-(1 << 15), (1 << 15) - 1, (E[10] - O[10] + rnd_factor) >> shift);
				block[29][6] = Clip3(-(1 << 15), (1 << 15) - 1, (E[6] + O[6] + rnd_factor) >> shift);
				block[13][22] = Clip3(-(1 << 15), (1 << 15) - 1, (E[9] - O[9] + rnd_factor) >> shift);
				block[30][7] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7] + O[7] + rnd_factor) >> shift);
				block[14][23] = Clip3(-(1 << 15), (1 << 15) - 1, (E[8] - O[8] + rnd_factor) >> shift);
				block[31][8] = Clip3(-(1 << 15), (1 << 15) - 1, (E[8] + O[8] + rnd_factor) >> shift);
				block[15][24] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7] - O[7] + rnd_factor) >> shift);
				block[0][9] = Clip3(-(1 << 15), (1 << 15) - 1, (E[9] + O[9] + rnd_factor) >> shift);
				block[16][25] = Clip3(-(1 << 15), (1 << 15) - 1, (E[6] - O[6] + rnd_factor) >> shift);
				block[1][10] = Clip3(-(1 << 15), (1 << 15) - 1, (E[10] + O[10] + rnd_factor) >> shift);
				block[17][26] = Clip3(-(1 << 15), (1 << 15) - 1, (E[5] - O[5] + rnd_factor) >> shift);
				block[2][11] = Clip3(-(1 << 15), (1 << 15) - 1, (E[11] + O[11] + rnd_factor) >> shift);
				block[18][27] = Clip3(-(1 << 15), (1 << 15) - 1, (E[4] - O[4] + rnd_factor) >> shift);
				block[3][12] = Clip3(-(1 << 15), (1 << 15) - 1, (E[12] + O[12] + rnd_factor) >> shift);
				block[19][28] = Clip3(-(1 << 15), (1 << 15) - 1, (E[3] - O[3] + rnd_factor) >> shift);
				block[4][13] = Clip3(-(1 << 15), (1 << 15) - 1, (E[13] + O[13] + rnd_factor) >> shift);
				block[20][29] = Clip3(-(1 << 15), (1 << 15) - 1, (E[2] - O[2] + rnd_factor) >> shift);
				block[5][14] = Clip3(-(1 << 15), (1 << 15) - 1, (E[14] + O[14] + rnd_factor) >> shift);
				block[21][30] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] - O[1] + rnd_factor) >> shift);
				block[6][15] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15] + O[15] + rnd_factor) >> shift);
				block[22][31] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] - O[0] + rnd_factor) >> shift);
				}
				else if(i == 24){
				block[24][0] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] + O[0] + rnd_factor) >> shift);
				block[8][16] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15] - O[15] + rnd_factor) >> shift);
				block[25][1] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] + O[1] + rnd_factor) >> shift);
				block[9][17] = Clip3(-(1 << 15), (1 << 15) - 1, (E[14] - O[14] + rnd_factor) >> shift);
				block[26][2] = Clip3(-(1 << 15), (1 << 15) - 1, (E[2] + O[2] + rnd_factor) >> shift);
				block[10][18] = Clip3(-(1 << 15), (1 << 15) - 1, (E[13] - O[13] + rnd_factor) >> shift);
				block[27][3] = Clip3(-(1 << 15), (1 << 15) - 1, (E[3] + O[3] + rnd_factor) >> shift);
				block[11][19] = Clip3(-(1 << 15), (1 << 15) - 1, (E[12] - O[12] + rnd_factor) >> shift);
				block[28][4] = Clip3(-(1 << 15), (1 << 15) - 1, (E[4] + O[4] + rnd_factor) >> shift);
				block[12][20] = Clip3(-(1 << 15), (1 << 15) - 1, (E[11] - O[11] + rnd_factor) >> shift);
				block[29][5] = Clip3(-(1 << 15), (1 << 15) - 1, (E[5] + O[5] + rnd_factor) >> shift);
				block[13][21] = Clip3(-(1 << 15), (1 << 15) - 1, (E[10] - O[10] + rnd_factor) >> shift);
				block[30][6] = Clip3(-(1 << 15), (1 << 15) - 1, (E[6] + O[6] + rnd_factor) >> shift);
				block[14][22] = Clip3(-(1 << 15), (1 << 15) - 1, (E[9] - O[9] + rnd_factor) >> shift);
				block[31][7] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7] + O[7] + rnd_factor) >> shift);
				block[15][23] = Clip3(-(1 << 15), (1 << 15) - 1, (E[8] - O[8] + rnd_factor) >> shift);
				block[0][8] = Clip3(-(1 << 15), (1 << 15) - 1, (E[8] + O[8] + rnd_factor) >> shift);
				block[16][24] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7] - O[7] + rnd_factor) >> shift);
				block[1][9] = Clip3(-(1 << 15), (1 << 15) - 1, (E[9] + O[9] + rnd_factor) >> shift);
				block[17][25] = Clip3(-(1 << 15), (1 << 15) - 1, (E[6] - O[6] + rnd_factor) >> shift);
				block[2][10] = Clip3(-(1 << 15), (1 << 15) - 1, (E[10] + O[10] + rnd_factor) >> shift);
				block[18][26] = Clip3(-(1 << 15), (1 << 15) - 1, (E[5] - O[5] + rnd_factor) >> shift);
				block[3][11] = Clip3(-(1 << 15), (1 << 15) - 1, (E[11] + O[11] + rnd_factor) >> shift);
				block[19][27] = Clip3(-(1 << 15), (1 << 15) - 1, (E[4] - O[4] + rnd_factor) >> shift);
				block[4][12] = Clip3(-(1 << 15), (1 << 15) - 1, (E[12] + O[12] + rnd_factor) >> shift);
				block[20][28] = Clip3(-(1 << 15), (1 << 15) - 1, (E[3] - O[3] + rnd_factor) >> shift);
				block[5][13] = Clip3(-(1 << 15), (1 << 15) - 1, (E[13] + O[13] + rnd_factor) >> shift);
				block[21][29] = Clip3(-(1 << 15), (1 << 15) - 1, (E[2] - O[2] + rnd_factor) >> shift);
				block[6][14] = Clip3(-(1 << 15), (1 << 15) - 1, (E[14] + O[14] + rnd_factor) >> shift);
				block[22][30] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] - O[1] + rnd_factor) >> shift);
				block[7][15] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15] + O[15] + rnd_factor) >> shift);
				block[23][31] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] - O[0] + rnd_factor) >> shift);
				}
				else if(i == 25){
				block[25][0] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] + O[0] + rnd_factor) >> shift);
				block[9][16] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15] - O[15] + rnd_factor) >> shift);
				block[26][1] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] + O[1] + rnd_factor) >> shift);
				block[10][17] = Clip3(-(1 << 15), (1 << 15) - 1, (E[14] - O[14] + rnd_factor) >> shift);
				block[27][2] = Clip3(-(1 << 15), (1 << 15) - 1, (E[2] + O[2] + rnd_factor) >> shift);
				block[11][18] = Clip3(-(1 << 15), (1 << 15) - 1, (E[13] - O[13] + rnd_factor) >> shift);
				block[28][3] = Clip3(-(1 << 15), (1 << 15) - 1, (E[3] + O[3] + rnd_factor) >> shift);
				block[12][19] = Clip3(-(1 << 15), (1 << 15) - 1, (E[12] - O[12] + rnd_factor) >> shift);
				block[29][4] = Clip3(-(1 << 15), (1 << 15) - 1, (E[4] + O[4] + rnd_factor) >> shift);
				block[13][20] = Clip3(-(1 << 15), (1 << 15) - 1, (E[11] - O[11] + rnd_factor) >> shift);
				block[30][5] = Clip3(-(1 << 15), (1 << 15) - 1, (E[5] + O[5] + rnd_factor) >> shift);
				block[14][21] = Clip3(-(1 << 15), (1 << 15) - 1, (E[10] - O[10] + rnd_factor) >> shift);
				block[31][6] = Clip3(-(1 << 15), (1 << 15) - 1, (E[6] + O[6] + rnd_factor) >> shift);
				block[15][22] = Clip3(-(1 << 15), (1 << 15) - 1, (E[9] - O[9] + rnd_factor) >> shift);
				block[0][7] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7] + O[7] + rnd_factor) >> shift);
				block[16][23] = Clip3(-(1 << 15), (1 << 15) - 1, (E[8] - O[8] + rnd_factor) >> shift);
				block[1][8] = Clip3(-(1 << 15), (1 << 15) - 1, (E[8] + O[8] + rnd_factor) >> shift);
				block[17][24] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7] - O[7] + rnd_factor) >> shift);
				block[2][9] = Clip3(-(1 << 15), (1 << 15) - 1, (E[9] + O[9] + rnd_factor) >> shift);
				block[18][25] = Clip3(-(1 << 15), (1 << 15) - 1, (E[6] - O[6] + rnd_factor) >> shift);
				block[3][10] = Clip3(-(1 << 15), (1 << 15) - 1, (E[10] + O[10] + rnd_factor) >> shift);
				block[19][26] = Clip3(-(1 << 15), (1 << 15) - 1, (E[5] - O[5] + rnd_factor) >> shift);
				block[4][11] = Clip3(-(1 << 15), (1 << 15) - 1, (E[11] + O[11] + rnd_factor) >> shift);
				block[20][27] = Clip3(-(1 << 15), (1 << 15) - 1, (E[4] - O[4] + rnd_factor) >> shift);
				block[5][12] = Clip3(-(1 << 15), (1 << 15) - 1, (E[12] + O[12] + rnd_factor) >> shift);
				block[21][28] = Clip3(-(1 << 15), (1 << 15) - 1, (E[3] - O[3] + rnd_factor) >> shift);
				block[6][13] = Clip3(-(1 << 15), (1 << 15) - 1, (E[13] + O[13] + rnd_factor) >> shift);
				block[22][29] = Clip3(-(1 << 15), (1 << 15) - 1, (E[2] - O[2] + rnd_factor) >> shift);
				block[7][14] = Clip3(-(1 << 15), (1 << 15) - 1, (E[14] + O[14] + rnd_factor) >> shift);
				block[23][30] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] - O[1] + rnd_factor) >> shift);
				block[8][15] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15] + O[15] + rnd_factor) >> shift);
				block[24][31] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] - O[0] + rnd_factor) >> shift);
				}
				else if(i == 26){
				block[26][0] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] + O[0] + rnd_factor) >> shift);
				block[10][16] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15] - O[15] + rnd_factor) >> shift);
				block[27][1] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] + O[1] + rnd_factor) >> shift);
				block[11][17] = Clip3(-(1 << 15), (1 << 15) - 1, (E[14] - O[14] + rnd_factor) >> shift);
				block[28][2] = Clip3(-(1 << 15), (1 << 15) - 1, (E[2] + O[2] + rnd_factor) >> shift);
				block[12][18] = Clip3(-(1 << 15), (1 << 15) - 1, (E[13] - O[13] + rnd_factor) >> shift);
				block[29][3] = Clip3(-(1 << 15), (1 << 15) - 1, (E[3] + O[3] + rnd_factor) >> shift);
				block[13][19] = Clip3(-(1 << 15), (1 << 15) - 1, (E[12] - O[12] + rnd_factor) >> shift);
				block[30][4] = Clip3(-(1 << 15), (1 << 15) - 1, (E[4] + O[4] + rnd_factor) >> shift);
				block[14][20] = Clip3(-(1 << 15), (1 << 15) - 1, (E[11] - O[11] + rnd_factor) >> shift);
				block[31][5] = Clip3(-(1 << 15), (1 << 15) - 1, (E[5] + O[5] + rnd_factor) >> shift);
				block[15][21] = Clip3(-(1 << 15), (1 << 15) - 1, (E[10] - O[10] + rnd_factor) >> shift);
				block[0][6] = Clip3(-(1 << 15), (1 << 15) - 1, (E[6] + O[6] + rnd_factor) >> shift);
				block[16][22] = Clip3(-(1 << 15), (1 << 15) - 1, (E[9] - O[9] + rnd_factor) >> shift);
				block[1][7] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7] + O[7] + rnd_factor) >> shift);
				block[17][23] = Clip3(-(1 << 15), (1 << 15) - 1, (E[8] - O[8] + rnd_factor) >> shift);
				block[2][8] = Clip3(-(1 << 15), (1 << 15) - 1, (E[8] + O[8] + rnd_factor) >> shift);
				block[18][24] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7] - O[7] + rnd_factor) >> shift);
				block[3][9] = Clip3(-(1 << 15), (1 << 15) - 1, (E[9] + O[9] + rnd_factor) >> shift);
				block[19][25] = Clip3(-(1 << 15), (1 << 15) - 1, (E[6] - O[6] + rnd_factor) >> shift);
				block[4][10] = Clip3(-(1 << 15), (1 << 15) - 1, (E[10] + O[10] + rnd_factor) >> shift);
				block[20][26] = Clip3(-(1 << 15), (1 << 15) - 1, (E[5] - O[5] + rnd_factor) >> shift);
				block[5][11] = Clip3(-(1 << 15), (1 << 15) - 1, (E[11] + O[11] + rnd_factor) >> shift);
				block[21][27] = Clip3(-(1 << 15), (1 << 15) - 1, (E[4] - O[4] + rnd_factor) >> shift);
				block[6][12] = Clip3(-(1 << 15), (1 << 15) - 1, (E[12] + O[12] + rnd_factor) >> shift);
				block[22][28] = Clip3(-(1 << 15), (1 << 15) - 1, (E[3] - O[3] + rnd_factor) >> shift);
				block[7][13] = Clip3(-(1 << 15), (1 << 15) - 1, (E[13] + O[13] + rnd_factor) >> shift);
				block[23][29] = Clip3(-(1 << 15), (1 << 15) - 1, (E[2] - O[2] + rnd_factor) >> shift);
				block[8][14] = Clip3(-(1 << 15), (1 << 15) - 1, (E[14] + O[14] + rnd_factor) >> shift);
				block[24][30] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] - O[1] + rnd_factor) >> shift);
				block[9][15] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15] + O[15] + rnd_factor) >> shift);
				block[25][31] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] - O[0] + rnd_factor) >> shift);
				}
				else if(i == 27){
				block[27][0] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] + O[0] + rnd_factor) >> shift);
				block[11][16] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15] - O[15] + rnd_factor) >> shift);
				block[28][1] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] + O[1] + rnd_factor) >> shift);
				block[12][17] = Clip3(-(1 << 15), (1 << 15) - 1, (E[14] - O[14] + rnd_factor) >> shift);
				block[29][2] = Clip3(-(1 << 15), (1 << 15) - 1, (E[2] + O[2] + rnd_factor) >> shift);
				block[13][18] = Clip3(-(1 << 15), (1 << 15) - 1, (E[13] - O[13] + rnd_factor) >> shift);
				block[30][3] = Clip3(-(1 << 15), (1 << 15) - 1, (E[3] + O[3] + rnd_factor) >> shift);
				block[14][19] = Clip3(-(1 << 15), (1 << 15) - 1, (E[12] - O[12] + rnd_factor) >> shift);
				block[31][4] = Clip3(-(1 << 15), (1 << 15) - 1, (E[4] + O[4] + rnd_factor) >> shift);
				block[15][20] = Clip3(-(1 << 15), (1 << 15) - 1, (E[11] - O[11] + rnd_factor) >> shift);
				block[0][5] = Clip3(-(1 << 15), (1 << 15) - 1, (E[5] + O[5] + rnd_factor) >> shift);
				block[16][21] = Clip3(-(1 << 15), (1 << 15) - 1, (E[10] - O[10] + rnd_factor) >> shift);
				block[1][6] = Clip3(-(1 << 15), (1 << 15) - 1, (E[6] + O[6] + rnd_factor) >> shift);
				block[17][22] = Clip3(-(1 << 15), (1 << 15) - 1, (E[9] - O[9] + rnd_factor) >> shift);
				block[2][7] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7] + O[7] + rnd_factor) >> shift);
				block[18][23] = Clip3(-(1 << 15), (1 << 15) - 1, (E[8] - O[8] + rnd_factor) >> shift);
				block[3][8] = Clip3(-(1 << 15), (1 << 15) - 1, (E[8] + O[8] + rnd_factor) >> shift);
				block[19][24] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7] - O[7] + rnd_factor) >> shift);
				block[4][9] = Clip3(-(1 << 15), (1 << 15) - 1, (E[9] + O[9] + rnd_factor) >> shift);
				block[20][25] = Clip3(-(1 << 15), (1 << 15) - 1, (E[6] - O[6] + rnd_factor) >> shift);
				block[5][10] = Clip3(-(1 << 15), (1 << 15) - 1, (E[10] + O[10] + rnd_factor) >> shift);
				block[21][26] = Clip3(-(1 << 15), (1 << 15) - 1, (E[5] - O[5] + rnd_factor) >> shift);
				block[6][11] = Clip3(-(1 << 15), (1 << 15) - 1, (E[11] + O[11] + rnd_factor) >> shift);
				block[22][27] = Clip3(-(1 << 15), (1 << 15) - 1, (E[4] - O[4] + rnd_factor) >> shift);
				block[7][12] = Clip3(-(1 << 15), (1 << 15) - 1, (E[12] + O[12] + rnd_factor) >> shift);
				block[23][28] = Clip3(-(1 << 15), (1 << 15) - 1, (E[3] - O[3] + rnd_factor) >> shift);
				block[8][13] = Clip3(-(1 << 15), (1 << 15) - 1, (E[13] + O[13] + rnd_factor) >> shift);
				block[24][29] = Clip3(-(1 << 15), (1 << 15) - 1, (E[2] - O[2] + rnd_factor) >> shift);
				block[9][14] = Clip3(-(1 << 15), (1 << 15) - 1, (E[14] + O[14] + rnd_factor) >> shift);
				block[25][30] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] - O[1] + rnd_factor) >> shift);
				block[10][15] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15] + O[15] + rnd_factor) >> shift);
				block[26][31] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] - O[0] + rnd_factor) >> shift);
				}
				else if(i == 28){
				block[28][0] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] + O[0] + rnd_factor) >> shift);
				block[12][16] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15] - O[15] + rnd_factor) >> shift);
				block[29][1] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] + O[1] + rnd_factor) >> shift);
				block[13][17] = Clip3(-(1 << 15), (1 << 15) - 1, (E[14] - O[14] + rnd_factor) >> shift);
				block[30][2] = Clip3(-(1 << 15), (1 << 15) - 1, (E[2] + O[2] + rnd_factor) >> shift);
				block[14][18] = Clip3(-(1 << 15), (1 << 15) - 1, (E[13] - O[13] + rnd_factor) >> shift);
				block[31][3] = Clip3(-(1 << 15), (1 << 15) - 1, (E[3] + O[3] + rnd_factor) >> shift);
				block[15][19] = Clip3(-(1 << 15), (1 << 15) - 1, (E[12] - O[12] + rnd_factor) >> shift);
				block[0][4] = Clip3(-(1 << 15), (1 << 15) - 1, (E[4] + O[4] + rnd_factor) >> shift);
				block[16][20] = Clip3(-(1 << 15), (1 << 15) - 1, (E[11] - O[11] + rnd_factor) >> shift);
				block[1][5] = Clip3(-(1 << 15), (1 << 15) - 1, (E[5] + O[5] + rnd_factor) >> shift);
				block[17][21] = Clip3(-(1 << 15), (1 << 15) - 1, (E[10] - O[10] + rnd_factor) >> shift);
				block[2][6] = Clip3(-(1 << 15), (1 << 15) - 1, (E[6] + O[6] + rnd_factor) >> shift);
				block[18][22] = Clip3(-(1 << 15), (1 << 15) - 1, (E[9] - O[9] + rnd_factor) >> shift);
				block[3][7] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7] + O[7] + rnd_factor) >> shift);
				block[19][23] = Clip3(-(1 << 15), (1 << 15) - 1, (E[8] - O[8] + rnd_factor) >> shift);
				block[4][8] = Clip3(-(1 << 15), (1 << 15) - 1, (E[8] + O[8] + rnd_factor) >> shift);
				block[20][24] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7] - O[7] + rnd_factor) >> shift);
				block[5][9] = Clip3(-(1 << 15), (1 << 15) - 1, (E[9] + O[9] + rnd_factor) >> shift);
				block[21][25] = Clip3(-(1 << 15), (1 << 15) - 1, (E[6] - O[6] + rnd_factor) >> shift);
				block[6][10] = Clip3(-(1 << 15), (1 << 15) - 1, (E[10] + O[10] + rnd_factor) >> shift);
				block[22][26] = Clip3(-(1 << 15), (1 << 15) - 1, (E[5] - O[5] + rnd_factor) >> shift);
				block[7][11] = Clip3(-(1 << 15), (1 << 15) - 1, (E[11] + O[11] + rnd_factor) >> shift);
				block[23][27] = Clip3(-(1 << 15), (1 << 15) - 1, (E[4] - O[4] + rnd_factor) >> shift);
				block[8][12] = Clip3(-(1 << 15), (1 << 15) - 1, (E[12] + O[12] + rnd_factor) >> shift);
				block[24][28] = Clip3(-(1 << 15), (1 << 15) - 1, (E[3] - O[3] + rnd_factor) >> shift);
				block[9][13] = Clip3(-(1 << 15), (1 << 15) - 1, (E[13] + O[13] + rnd_factor) >> shift);
				block[25][29] = Clip3(-(1 << 15), (1 << 15) - 1, (E[2] - O[2] + rnd_factor) >> shift);
				block[10][14] = Clip3(-(1 << 15), (1 << 15) - 1, (E[14] + O[14] + rnd_factor) >> shift);
				block[26][30] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] - O[1] + rnd_factor) >> shift);
				block[11][15] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15] + O[15] + rnd_factor) >> shift);
				block[27][31] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] - O[0] + rnd_factor) >> shift);
				}
				else if(i == 29){
				block[29][0] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] + O[0] + rnd_factor) >> shift);
				block[13][16] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15] - O[15] + rnd_factor) >> shift);
				block[30][1] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] + O[1] + rnd_factor) >> shift);
				block[14][17] = Clip3(-(1 << 15), (1 << 15) - 1, (E[14] - O[14] + rnd_factor) >> shift);
				block[31][2] = Clip3(-(1 << 15), (1 << 15) - 1, (E[2] + O[2] + rnd_factor) >> shift);
				block[15][18] = Clip3(-(1 << 15), (1 << 15) - 1, (E[13] - O[13] + rnd_factor) >> shift);
				block[0][3] = Clip3(-(1 << 15), (1 << 15) - 1, (E[3] + O[3] + rnd_factor) >> shift);
				block[16][19] = Clip3(-(1 << 15), (1 << 15) - 1, (E[12] - O[12] + rnd_factor) >> shift);
				block[1][4] = Clip3(-(1 << 15), (1 << 15) - 1, (E[4] + O[4] + rnd_factor) >> shift);
				block[17][20] = Clip3(-(1 << 15), (1 << 15) - 1, (E[11] - O[11] + rnd_factor) >> shift);
				block[2][5] = Clip3(-(1 << 15), (1 << 15) - 1, (E[5] + O[5] + rnd_factor) >> shift);
				block[18][21] = Clip3(-(1 << 15), (1 << 15) - 1, (E[10] - O[10] + rnd_factor) >> shift);
				block[3][6] = Clip3(-(1 << 15), (1 << 15) - 1, (E[6] + O[6] + rnd_factor) >> shift);
				block[19][22] = Clip3(-(1 << 15), (1 << 15) - 1, (E[9] - O[9] + rnd_factor) >> shift);
				block[4][7] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7] + O[7] + rnd_factor) >> shift);
				block[20][23] = Clip3(-(1 << 15), (1 << 15) - 1, (E[8] - O[8] + rnd_factor) >> shift);
				block[5][8] = Clip3(-(1 << 15), (1 << 15) - 1, (E[8] + O[8] + rnd_factor) >> shift);
				block[21][24] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7] - O[7] + rnd_factor) >> shift);
				block[6][9] = Clip3(-(1 << 15), (1 << 15) - 1, (E[9] + O[9] + rnd_factor) >> shift);
				block[22][25] = Clip3(-(1 << 15), (1 << 15) - 1, (E[6] - O[6] + rnd_factor) >> shift);
				block[7][10] = Clip3(-(1 << 15), (1 << 15) - 1, (E[10] + O[10] + rnd_factor) >> shift);
				block[23][26] = Clip3(-(1 << 15), (1 << 15) - 1, (E[5] - O[5] + rnd_factor) >> shift);
				block[8][11] = Clip3(-(1 << 15), (1 << 15) - 1, (E[11] + O[11] + rnd_factor) >> shift);
				block[24][27] = Clip3(-(1 << 15), (1 << 15) - 1, (E[4] - O[4] + rnd_factor) >> shift);
				block[9][12] = Clip3(-(1 << 15), (1 << 15) - 1, (E[12] + O[12] + rnd_factor) >> shift);
				block[25][28] = Clip3(-(1 << 15), (1 << 15) - 1, (E[3] - O[3] + rnd_factor) >> shift);
				block[10][13] = Clip3(-(1 << 15), (1 << 15) - 1, (E[13] + O[13] + rnd_factor) >> shift);
				block[26][29] = Clip3(-(1 << 15), (1 << 15) - 1, (E[2] - O[2] + rnd_factor) >> shift);
				block[11][14] = Clip3(-(1 << 15), (1 << 15) - 1, (E[14] + O[14] + rnd_factor) >> shift);
				block[27][30] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] - O[1] + rnd_factor) >> shift);
				block[12][15] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15] + O[15] + rnd_factor) >> shift);
				block[28][31] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] - O[0] + rnd_factor) >> shift);
				}
				else if(i == 30){
				block[30][0] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] + O[0] + rnd_factor) >> shift);
				block[14][16] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15] - O[15] + rnd_factor) >> shift);
				block[31][1] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] + O[1] + rnd_factor) >> shift);
				block[15][17] = Clip3(-(1 << 15), (1 << 15) - 1, (E[14] - O[14] + rnd_factor) >> shift);
				block[0][2] = Clip3(-(1 << 15), (1 << 15) - 1, (E[2] + O[2] + rnd_factor) >> shift);
				block[16][18] = Clip3(-(1 << 15), (1 << 15) - 1, (E[13] - O[13] + rnd_factor) >> shift);
				block[1][3] = Clip3(-(1 << 15), (1 << 15) - 1, (E[3] + O[3] + rnd_factor) >> shift);
				block[17][19] = Clip3(-(1 << 15), (1 << 15) - 1, (E[12] - O[12] + rnd_factor) >> shift);
				block[2][4] = Clip3(-(1 << 15), (1 << 15) - 1, (E[4] + O[4] + rnd_factor) >> shift);
				block[18][20] = Clip3(-(1 << 15), (1 << 15) - 1, (E[11] - O[11] + rnd_factor) >> shift);
				block[3][5] = Clip3(-(1 << 15), (1 << 15) - 1, (E[5] + O[5] + rnd_factor) >> shift);
				block[19][21] = Clip3(-(1 << 15), (1 << 15) - 1, (E[10] - O[10] + rnd_factor) >> shift);
				block[4][6] = Clip3(-(1 << 15), (1 << 15) - 1, (E[6] + O[6] + rnd_factor) >> shift);
				block[20][22] = Clip3(-(1 << 15), (1 << 15) - 1, (E[9] - O[9] + rnd_factor) >> shift);
				block[5][7] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7] + O[7] + rnd_factor) >> shift);
				block[21][23] = Clip3(-(1 << 15), (1 << 15) - 1, (E[8] - O[8] + rnd_factor) >> shift);
				block[6][8] = Clip3(-(1 << 15), (1 << 15) - 1, (E[8] + O[8] + rnd_factor) >> shift);
				block[22][24] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7] - O[7] + rnd_factor) >> shift);
				block[7][9] = Clip3(-(1 << 15), (1 << 15) - 1, (E[9] + O[9] + rnd_factor) >> shift);
				block[23][25] = Clip3(-(1 << 15), (1 << 15) - 1, (E[6] - O[6] + rnd_factor) >> shift);
				block[8][10] = Clip3(-(1 << 15), (1 << 15) - 1, (E[10] + O[10] + rnd_factor) >> shift);
				block[24][26] = Clip3(-(1 << 15), (1 << 15) - 1, (E[5] - O[5] + rnd_factor) >> shift);
				block[9][11] = Clip3(-(1 << 15), (1 << 15) - 1, (E[11] + O[11] + rnd_factor) >> shift);
				block[25][27] = Clip3(-(1 << 15), (1 << 15) - 1, (E[4] - O[4] + rnd_factor) >> shift);
				block[10][12] = Clip3(-(1 << 15), (1 << 15) - 1, (E[12] + O[12] + rnd_factor) >> shift);
				block[26][28] = Clip3(-(1 << 15), (1 << 15) - 1, (E[3] - O[3] + rnd_factor) >> shift);
				block[11][13] = Clip3(-(1 << 15), (1 << 15) - 1, (E[13] + O[13] + rnd_factor) >> shift);
				block[27][29] = Clip3(-(1 << 15), (1 << 15) - 1, (E[2] - O[2] + rnd_factor) >> shift);
				block[12][14] = Clip3(-(1 << 15), (1 << 15) - 1, (E[14] + O[14] + rnd_factor) >> shift);
				block[28][30] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] - O[1] + rnd_factor) >> shift);
				block[13][15] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15] + O[15] + rnd_factor) >> shift);
				block[29][31] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] - O[0] + rnd_factor) >> shift);
				}
				else{
				block[31][0] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] + O[0] + rnd_factor) >> shift);
				block[15][16] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15] - O[15] + rnd_factor) >> shift);
				block[0][1] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] + O[1] + rnd_factor) >> shift);
				block[16][17] = Clip3(-(1 << 15), (1 << 15) - 1, (E[14] - O[14] + rnd_factor) >> shift);
				block[1][2] = Clip3(-(1 << 15), (1 << 15) - 1, (E[2] + O[2] + rnd_factor) >> shift);
				block[17][18] = Clip3(-(1 << 15), (1 << 15) - 1, (E[13] - O[13] + rnd_factor) >> shift);
				block[2][3] = Clip3(-(1 << 15), (1 << 15) - 1, (E[3] + O[3] + rnd_factor) >> shift);
				block[18][19] = Clip3(-(1 << 15), (1 << 15) - 1, (E[12] - O[12] + rnd_factor) >> shift);
				block[3][4] = Clip3(-(1 << 15), (1 << 15) - 1, (E[4] + O[4] + rnd_factor) >> shift);
				block[19][20] = Clip3(-(1 << 15), (1 << 15) - 1, (E[11] - O[11] + rnd_factor) >> shift);
				block[4][5] = Clip3(-(1 << 15), (1 << 15) - 1, (E[5] + O[5] + rnd_factor) >> shift);
				block[20][21] = Clip3(-(1 << 15), (1 << 15) - 1, (E[10] - O[10] + rnd_factor) >> shift);
				block[5][6] = Clip3(-(1 << 15), (1 << 15) - 1, (E[6] + O[6] + rnd_factor) >> shift);
				block[21][22] = Clip3(-(1 << 15), (1 << 15) - 1, (E[9] - O[9] + rnd_factor) >> shift);
				block[6][7] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7] + O[7] + rnd_factor) >> shift);
				block[22][23] = Clip3(-(1 << 15), (1 << 15) - 1, (E[8] - O[8] + rnd_factor) >> shift);
				block[7][8] = Clip3(-(1 << 15), (1 << 15) - 1, (E[8] + O[8] + rnd_factor) >> shift);
				block[23][24] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7] - O[7] + rnd_factor) >> shift);
				block[8][9] = Clip3(-(1 << 15), (1 << 15) - 1, (E[9] + O[9] + rnd_factor) >> shift);
				block[24][25] = Clip3(-(1 << 15), (1 << 15) - 1, (E[6] - O[6] + rnd_factor) >> shift);
				block[9][10] = Clip3(-(1 << 15), (1 << 15) - 1, (E[10] + O[10] + rnd_factor) >> shift);
				block[25][26] = Clip3(-(1 << 15), (1 << 15) - 1, (E[5] - O[5] + rnd_factor) >> shift);
				block[10][11] = Clip3(-(1 << 15), (1 << 15) - 1, (E[11] + O[11] + rnd_factor) >> shift);
				block[26][27] = Clip3(-(1 << 15), (1 << 15) - 1, (E[4] - O[4] + rnd_factor) >> shift);
				block[11][12] = Clip3(-(1 << 15), (1 << 15) - 1, (E[12] + O[12] + rnd_factor) >> shift);
				block[27][28] = Clip3(-(1 << 15), (1 << 15) - 1, (E[3] - O[3] + rnd_factor) >> shift);
				block[12][13] = Clip3(-(1 << 15), (1 << 15) - 1, (E[13] + O[13] + rnd_factor) >> shift);
				block[28][29] = Clip3(-(1 << 15), (1 << 15) - 1, (E[2] - O[2] + rnd_factor) >> shift);
				block[13][14] = Clip3(-(1 << 15), (1 << 15) - 1, (E[14] + O[14] + rnd_factor) >> shift);
				block[29][30] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] - O[1] + rnd_factor) >> shift);
				block[14][15] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15] + O[15] + rnd_factor) >> shift);
				block[30][31] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] - O[0] + rnd_factor) >> shift);
				}
			}
}

void inv_dct_col_32(U8 cu_width, U8 cu_height, s16 coeff[32][32], s16 block[32][32], U4 shift)
{
	U7 size = 32;
	U8 size_b = size - 1;

	if (cu_height == 4)
	{

		int E[2], O[2];
		int rnd_factor = 1 << (shift - 1);

		for (int i = 0; i < 32; i++) {
			O[0] = (42) * coeff[1][i] + (17) * coeff[3][i];
			O[1] = (17) * coeff[1][i] + (-42) * coeff[3][i];
			E[0] = (32) * coeff[0][i] + (32) * coeff[2][i];
			E[1] = (32) * coeff[0][i] + (-32) * coeff[2][i];
			block[i][0] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] + O[0] + rnd_factor) >> shift);
			block[i][1] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] + O[1] + rnd_factor) >> shift);
			block[i][2] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] - O[1] + rnd_factor) >> shift);
			block[i][3] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] - O[0] + rnd_factor) >> shift);
		}
	}
	else if (cu_height == 8)
	{


		int E[4], O[4];
		int EE[2], EO[2];

		int rnd_factor = 1 << (shift - 1);
		int k;
		for (int i = 0; i < 32; i++) {
			O[0] = (44) * coeff[1][i] + (38) * coeff[3][i] + (25) * coeff[5][i] + (9) * coeff[7][i];
			O[1] = (38) * coeff[1][i] + (-9) * coeff[3][i] + (-44) * coeff[5][i] + (-25) * coeff[7][i];
			O[2] = (25) * coeff[1][i] + (-44) * coeff[3][i] + (9) * coeff[5][i] + (38) * coeff[7][i];
			O[3] = (9) * coeff[1][i] + (-25) * coeff[3][i] + (38) * coeff[5][i] + (-44) * coeff[7][i];

			EO[0] = (42) * coeff[2][i] + (17) * coeff[6][i];
			EO[1] = (17) * coeff[2][i] + (-42) * coeff[6][i];
			EE[0] = (32) * coeff[0][i] + (32) * coeff[4][i];
			EE[1] = (32) * coeff[0][i] + (-32) * coeff[4][i];

			E[0] = EE[0] + EO[0];
			E[3] = EE[0] - EO[0];
			E[1] = EE[1] + EO[1];
			E[2] = EE[1] - EO[1];
			for (k = 0; k < 4; k++) {
				block[i][k] = Clip3(-(1 << 15), (1 << 15) - 1, ((int)(E[k] + O[k] + rnd_factor)) >> shift);
				block[i][k + 4] = Clip3(-(1 << 15), (1 << 15) - 1, ((int)(E[3 - k] - O[3 - k] + rnd_factor)) >> shift);

			}
		}
	}
	else if (cu_height == 16)
	{
		int E[8], O[8];
		int EE[4], EO[4];
		int EEE[2], EEO[2];

		int rnd_factor = 1 << (shift - 1);
		int k;
		for (int i = 0; i < 32; i++) {
			O[0] = (45) * coeff[1][i] + (43) * coeff[3][i] + (40) * coeff[5][i] + (35) * coeff[7][i] + (29) * coeff[9][i] + (21) * coeff[11][i] + (13) * coeff[13][i] + (4) * coeff[15][i];
			O[1] = (43) * coeff[1][i] + (29) * coeff[3][i] + (4) * coeff[5][i] + (-21) * coeff[7][i] + (-40) * coeff[9][i] + (-45) * coeff[11][i] + (-35) * coeff[13][i] + (-13) * coeff[15][i];
			O[2] = (40) * coeff[1][i] + (4) * coeff[3][i] + (-35) * coeff[5][i] + (-43) * coeff[7][i] + (-13) * coeff[9][i] + (29) * coeff[11][i] + (45) * coeff[13][i] + (21) * coeff[15][i];
			O[3] = (35) * coeff[1][i] + (-21) * coeff[3][i] + (-43) * coeff[5][i] + (4) * coeff[7][i] + (45) * coeff[9][i] + (13) * coeff[11][i] + (-40) * coeff[13][i] + (-29) * coeff[15][i];
			O[4] = (29) * coeff[1][i] + (-40) * coeff[3][i] + (-13) * coeff[5][i] + (45) * coeff[7][i] + (-4) * coeff[9][i] + (-43) * coeff[11][i] + (21) * coeff[13][i] + (35) * coeff[15][i];
			O[5] = (21) * coeff[1][i] + (-45) * coeff[3][i] + (29) * coeff[5][i] + (13) * coeff[7][i] + (-43) * coeff[9][i] + (35) * coeff[11][i] + (4) * coeff[13][i] + (-40) * coeff[15][i];
			O[6] = (13) * coeff[1][i] + (-35) * coeff[3][i] + (45) * coeff[5][i] + (-40) * coeff[7][i] + (21) * coeff[9][i] + (4) * coeff[11][i] + (-29) * coeff[13][i] + (43) * coeff[15][i];
			O[7] = (4) * coeff[1][i] + (-13) * coeff[3][i] + (21) * coeff[5][i] + (-29) * coeff[7][i] + (35) * coeff[9][i] + (-40) * coeff[11][i] + (43) * coeff[13][i] + (-45) * coeff[15][i];

			EO[0] = (44) * coeff[2][i] + (38) * coeff[6][i] + (25) * coeff[10][i] + (9) * coeff[14][i];
			EO[1] = (38) * coeff[2][i] + (-9) * coeff[6][i] + (-44) * coeff[10][i] + (-25) * coeff[14][i];
			EO[2] = (25) * coeff[2][i] + (-44) * coeff[6][i] + (9) * coeff[10][i] + (38) * coeff[14][i];
			EO[3] = (9) * coeff[2][i] + (-25) * coeff[6][i] + (38) * coeff[10][i] + (-44) * coeff[14][i];

			EEO[0] = (42) * coeff[4][i] + (17) * coeff[12][i];
			EEE[0] = (32) * coeff[0][i] + (32) * coeff[8][i];
			EEO[1] = (17) * coeff[4][i] + (-42) * coeff[12][i];
			EEE[1] = (32) * coeff[0][i] + (-32) * coeff[8][i];

			for (k = 0; k < 2; k++) {
				EE[k] = EEE[k] + EEO[k];
				EE[k + 2] = EEE[1 - k] - EEO[1 - k];
			}
			for (k = 0; k < 4; k++) {
				E[k] = EE[k] + EO[k];
				E[k + 4] = EE[3 - k] - EO[3 - k];
			}
			for (k = 0; k < 8; k++) {
				block[i][k] = Clip3(-(1 << 15), (1 << 15) - 1, (E[k] + O[k] + rnd_factor) >> shift);
				block[i][k + 8] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7 - k] - O[7 - k] + rnd_factor) >> shift);
			}
		}
	}
	else {
		int E[16], O[16];
		int EE[8], EO[8];
		int EEE[4], EEO[4];
		int EEEE[2], EEEO[2];

		int rnd_factor = 1 << (shift - 1);
		int k;
		for (int i = 0; i < 32; i++) {
			O[0] = (45) * coeff[1][i] + (45) * coeff[3][i] + (44) * coeff[5][i] + (43) * coeff[7][i] + (41) * coeff[9][i] + (39) * coeff[11][i] + (36) * coeff[13][i] + (34) * coeff[15][i] + (30) * coeff[17][i] + (27) * coeff[19][i] + (23) * coeff[21][i] + (19) * coeff[23][i] + (15) * coeff[25][i] + (11) * coeff[27][i] + (7) * coeff[29][i] + (2) * coeff[31][i];
			O[1] = (45) * coeff[1][i] + (41) * coeff[3][i] + (34) * coeff[5][i] + (23) * coeff[7][i] + (11) * coeff[9][i] + (-2) * coeff[11][i] + (-15) * coeff[13][i] + (-27) * coeff[15][i] + (-36) * coeff[17][i] + (-43) * coeff[19][i] + (-45) * coeff[21][i] + (-44) * coeff[23][i] + (-39) * coeff[25][i] + (-30) * coeff[27][i] + (-19) * coeff[29][i] + (-7) * coeff[31][i];
			O[2] = (44) * coeff[1][i] + (34) * coeff[3][i] + (15) * coeff[5][i] + (-7) * coeff[7][i] + (-27) * coeff[9][i] + (-41) * coeff[11][i] + (-45) * coeff[13][i] + (-39) * coeff[15][i] + (-23) * coeff[17][i] + (-2) * coeff[19][i] + (19) * coeff[21][i] + (36) * coeff[23][i] + (45) * coeff[25][i] + (43) * coeff[27][i] + (30) * coeff[29][i] + (11) * coeff[31][i];
			O[3] = (43) * coeff[1][i] + (23) * coeff[3][i] + (-7) * coeff[5][i] + (-34) * coeff[7][i] + (-45) * coeff[9][i] + (-36) * coeff[11][i] + (-11) * coeff[13][i] + (19) * coeff[15][i] + (41) * coeff[17][i] + (44) * coeff[19][i] + (27) * coeff[21][i] + (-2) * coeff[23][i] + (-30) * coeff[25][i] + (-45) * coeff[27][i] + (-39) * coeff[29][i] + (-15) * coeff[31][i];
			O[4] = (41) * coeff[1][i] + (11) * coeff[3][i] + (-27) * coeff[5][i] + (-45) * coeff[7][i] + (-30) * coeff[9][i] + (7) * coeff[11][i] + (39) * coeff[13][i] + (43) * coeff[15][i] + (15) * coeff[17][i] + (-23) * coeff[19][i] + (-45) * coeff[21][i] + (-34) * coeff[23][i] + (2) * coeff[25][i] + (36) * coeff[27][i] + (44) * coeff[29][i] + (19) * coeff[31][i];
			O[5] = (39) * coeff[1][i] + (-2) * coeff[3][i] + (-41) * coeff[5][i] + (-36) * coeff[7][i] + (7) * coeff[9][i] + (43) * coeff[11][i] + (34) * coeff[13][i] + (-11) * coeff[15][i] + (-44) * coeff[17][i] + (-30) * coeff[19][i] + (15) * coeff[21][i] + (45) * coeff[23][i] + (27) * coeff[25][i] + (-19) * coeff[27][i] + (-45) * coeff[29][i] + (-23) * coeff[31][i];
			O[6] = (36) * coeff[1][i] + (-15) * coeff[3][i] + (-45) * coeff[5][i] + (-11) * coeff[7][i] + (39) * coeff[9][i] + (34) * coeff[11][i] + (-19) * coeff[13][i] + (-45) * coeff[15][i] + (-7) * coeff[17][i] + (41) * coeff[19][i] + (30) * coeff[21][i] + (-23) * coeff[23][i] + (-44) * coeff[25][i] + (-2) * coeff[27][i] + (43) * coeff[29][i] + (27) * coeff[31][i];
			O[7] = (34) * coeff[1][i] + (-27) * coeff[3][i] + (-39) * coeff[5][i] + (19) * coeff[7][i] + (43) * coeff[9][i] + (-11) * coeff[11][i] + (-45) * coeff[13][i] + (2) * coeff[15][i] + (45) * coeff[17][i] + (7) * coeff[19][i] + (-44) * coeff[21][i] + (-15) * coeff[23][i] + (41) * coeff[25][i] + (23) * coeff[27][i] + (-36) * coeff[29][i] + (-30) * coeff[31][i];
			O[8] = (30) * coeff[1][i] + (-36) * coeff[3][i] + (-23) * coeff[5][i] + (41) * coeff[7][i] + (15) * coeff[9][i] + (-44) * coeff[11][i] + (-7) * coeff[13][i] + (45) * coeff[15][i] + (-2) * coeff[17][i] + (-45) * coeff[19][i] + (11) * coeff[21][i] + (43) * coeff[23][i] + (-19) * coeff[25][i] + (-39) * coeff[27][i] + (27) * coeff[29][i] + (34) * coeff[31][i];
			O[9] = (27) * coeff[1][i] + (-43) * coeff[3][i] + (-2) * coeff[5][i] + (44) * coeff[7][i] + (-23) * coeff[9][i] + (-30) * coeff[11][i] + (41) * coeff[13][i] + (7) * coeff[15][i] + (-45) * coeff[17][i] + (19) * coeff[19][i] + (34) * coeff[21][i] + (-39) * coeff[23][i] + (-11) * coeff[25][i] + (45) * coeff[27][i] + (-15) * coeff[29][i] + (-36) * coeff[31][i];
			O[10] = (23) * coeff[1][i] + (-45) * coeff[3][i] + (19) * coeff[5][i] + (27) * coeff[7][i] + (-45) * coeff[9][i] + (15) * coeff[11][i] + (30) * coeff[13][i] + (-44) * coeff[15][i] + (11) * coeff[17][i] + (34) * coeff[19][i] + (-43) * coeff[21][i] + (7) * coeff[23][i] + (36) * coeff[25][i] + (-41) * coeff[27][i] + (2) * coeff[29][i] + (39) * coeff[31][i];
			O[11] = (19) * coeff[1][i] + (-44) * coeff[3][i] + (36) * coeff[5][i] + (-2) * coeff[7][i] + (-34) * coeff[9][i] + (45) * coeff[11][i] + (-23) * coeff[13][i] + (-15) * coeff[15][i] + (43) * coeff[17][i] + (-39) * coeff[19][i] + (7) * coeff[21][i] + (30) * coeff[23][i] + (-45) * coeff[25][i] + (27) * coeff[27][i] + (11) * coeff[29][i] + (-41) * coeff[31][i];
			O[12] = (15) * coeff[1][i] + (-39) * coeff[3][i] + (45) * coeff[5][i] + (-30) * coeff[7][i] + (2) * coeff[9][i] + (27) * coeff[11][i] + (-44) * coeff[13][i] + (41) * coeff[15][i] + (-19) * coeff[17][i] + (-11) * coeff[19][i] + (36) * coeff[21][i] + (-45) * coeff[23][i] + (34) * coeff[25][i] + (-7) * coeff[27][i] + (-23) * coeff[29][i] + (43) * coeff[31][i];
			O[13] = (11) * coeff[1][i] + (-30) * coeff[3][i] + (43) * coeff[5][i] + (-45) * coeff[7][i] + (36) * coeff[9][i] + (-19) * coeff[11][i] + (-2) * coeff[13][i] + (23) * coeff[15][i] + (-39) * coeff[17][i] + (45) * coeff[19][i] + (-41) * coeff[21][i] + (27) * coeff[23][i] + (-7) * coeff[25][i] + (-15) * coeff[27][i] + (34) * coeff[29][i] + (-44) * coeff[31][i];
			O[14] = (7) * coeff[1][i] + (-19) * coeff[3][i] + (30) * coeff[5][i] + (-39) * coeff[7][i] + (44) * coeff[9][i] + (-45) * coeff[11][i] + (43) * coeff[13][i] + (-36) * coeff[15][i] + (27) * coeff[17][i] + (-15) * coeff[19][i] + (2) * coeff[21][i] + (11) * coeff[23][i] + (-23) * coeff[25][i] + (34) * coeff[27][i] + (-41) * coeff[29][i] + (45) * coeff[31][i];
			O[15] = (2) * coeff[1][i] + (-7) * coeff[3][i] + (11) * coeff[5][i] + (-15) * coeff[7][i] + (19) * coeff[9][i] + (-23) * coeff[11][i] + (27) * coeff[13][i] + (-30) * coeff[15][i] + (34) * coeff[17][i] + (-36) * coeff[19][i] + (39) * coeff[21][i] + (-41) * coeff[23][i] + (43) * coeff[25][i] + (-44) * coeff[27][i] + (45) * coeff[29][i] + (-45) * coeff[31][i];

			EO[0] = (45) * coeff[2][i] + (43) * coeff[6][i] + (40) * coeff[10][i] + (35) * coeff[14][i] + (29) * coeff[18][i] + (21) * coeff[22][i] + (13) * coeff[26][i] + (4) * coeff[30][i];
			EO[1] = (43) * coeff[2][i] + (29) * coeff[6][i] + (4) * coeff[10][i] + (-21) * coeff[14][i] + (-40) * coeff[18][i] + (-45) * coeff[22][i] + (-35) * coeff[26][i] + (-13) * coeff[30][i];
			EO[2] = (40) * coeff[2][i] + (4) * coeff[6][i] + (-35) * coeff[10][i] + (-43) * coeff[14][i] + (-13) * coeff[18][i] + (29) * coeff[22][i] + (45) * coeff[26][i] + (21) * coeff[30][i];
			EO[3] = (35) * coeff[2][i] + (-21) * coeff[6][i] + (-43) * coeff[10][i] + (4) * coeff[14][i] + (45) * coeff[18][i] + (13) * coeff[22][i] + (-40) * coeff[26][i] + (-29) * coeff[30][i];
			EO[4] = (29) * coeff[2][i] + (-40) * coeff[6][i] + (-13) * coeff[10][i] + (45) * coeff[14][i] + (-4) * coeff[18][i] + (-43) * coeff[22][i] + (21) * coeff[26][i] + (35) * coeff[30][i];
			EO[5] = (21) * coeff[2][i] + (-45) * coeff[6][i] + (29) * coeff[10][i] + (13) * coeff[14][i] + (-43) * coeff[18][i] + (35) * coeff[22][i] + (4) * coeff[26][i] + (-40) * coeff[30][i];
			EO[6] = (13) * coeff[2][i] + (-35) * coeff[6][i] + (45) * coeff[10][i] + (-40) * coeff[14][i] + (21) * coeff[18][i] + (4) * coeff[22][i] + (-29) * coeff[26][i] + (43) * coeff[30][i];
			EO[7] = (4) * coeff[2][i] + (-13) * coeff[6][i] + (21) * coeff[10][i] + (-29) * coeff[14][i] + (35) * coeff[18][i] + (-40) * coeff[22][i] + (43) * coeff[26][i] + (-45) * coeff[30][i];

			EEO[0] = (44) * coeff[4][i] + (38) * coeff[12][i] + (25) * coeff[20][i] + (9) * coeff[28][i];
			EEO[1] = (38) * coeff[4][i] + (-9) * coeff[12][i] + (-44) * coeff[20][i] + (-25) * coeff[28][i];
			EEO[2] = (25) * coeff[4][i] + (-44) * coeff[12][i] + (9) * coeff[20][i] + (38) * coeff[28][i];
			EEO[3] = (9) * coeff[4][i] + (-25) * coeff[12][i] + (38) * coeff[20][i] + (-44) * coeff[28][i];

			EEEO[0] = (42) * coeff[8][i] + (17) * coeff[24][i];
			EEEO[1] = (17) * coeff[8][i] + (-42) * coeff[24][i];
			EEEE[0] = (32) * coeff[0][i] + (32) * coeff[16][i];
			EEEE[1] = (32) * coeff[0][i] + (-32) * coeff[16][i];

			EEE[0] = EEEE[0] + EEEO[0];
			EEE[3] = EEEE[0] - EEEO[0];
			EEE[1] = EEEE[1] + EEEO[1];
			EEE[2] = EEEE[1] - EEEO[1];

			for (k = 0; k < 4; k++) {
				EE[k] = EEE[k] + EEO[k];
				EE[k + 4] = EEE[3 - k] - EEO[3 - k];
			}
			for (k = 0; k < 8; k++) {
				E[k] = EE[k] + EO[k];
				E[k + 8] = EE[7 - k] - EO[7 - k];
			}
			for (k = 0; k < 16; k++) {
				block[(i + k) & size_b][k] = Clip3(-(1 << 15), (1 << 15) - 1, (E[k] + O[k] + rnd_factor) >> shift);
				block[(i + k + 16) & size_b][k + 16] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15 - k] - O[15 - k] + rnd_factor) >> shift);
			}
		}
	}
}



void dquant_32_s(U6 qp, s16 coef[32][32], hls::stream<s512_h> &coef_out,U4 bit_depth)
{
//#pragma HLS INLINE
  U8 cu_width = 32; 
  U8 cu_height= 32; 
  U3 log2_w = 5;
  U3 log2_h = 5;
	int i;
	int j;
	int w = 1 << log2_w;
	int h = 1 << log2_h;
	int log2_size = (log2_w + log2_h) >> 1;
	int refix = (log2_w + log2_h) & 1;
	const int com_tbl_dq_scale[80] = { // [64 + 16]
		32768, 36061, 38968, 42495, 46341, 50535, 55437, 60424,
		32932, 35734, 38968, 42495, 46177, 50535, 55109, 59933,
		65535, 35734, 38968, 42577, 46341, 50617, 55027, 60097,
		32809, 35734, 38968, 42454, 46382, 50576, 55109, 60056,
		65535, 35734, 38968, 42495, 46320, 50515, 55109, 60076,
		65535, 35744, 38968, 42495, 46341, 50535, 55099, 60087,
		65535, 35734, 38973, 42500, 46341, 50535, 55109, 60097,
		32771, 35734, 38965, 42497, 46341, 50535, 55109, 60099,
		32768, 36061, 38968, 42495, 46341, 50535, 55437, 60424,
		32932, 35734, 38968, 42495, 46177, 50535, 55109, 59933
	};
	const int com_tbl_dq_shift[80] = { // [64 + 16]
		14, 14, 14, 14, 14, 14, 14, 14,  //15, 15, 15, 15, 15, 15, 15, 15,
		13, 13, 13, 13, 13, 13, 13, 13,  //14, 14, 14, 14, 14, 14, 14, 14,
		13, 12, 12, 12, 12, 12, 12, 12,  //14, 13, 13, 13, 13, 13, 13, 13,
		11, 11, 11, 11, 11, 11, 11, 11,  //12, 12, 12, 12, 12, 12, 12, 12,
		11, 10, 10, 10, 10, 10, 10, 10,  //12, 11, 11, 11, 11, 11, 11, 11,
		10, 9, 9, 9, 9, 9, 9, 9,  //11, 10, 10, 10, 10, 10, 10, 10,
		9, 8, 8, 8, 8, 8, 8, 8,  //10,  9,  9,  9,  9,  9,  9,  9,
		7, 7, 7, 7, 7, 7, 7, 7,  // 8,  8,  8,  8,  8,  8,  8,  8,
		6, 6, 6, 6, 6, 6, 6, 6,  // 7,  7,  7,  7,  7,  7,  7,  7,
		5, 5, 5, 5, 5, 5, 5, 5,  // 6,  6,  6,  6,  6,  6,  6,  6
	};
	int scale = com_tbl_dq_scale[qp];
	// +1 is used to compensate for the mismatching of shifts in quantization and inverse quantization
	int shift = com_tbl_dq_shift[qp] - get_transform_shift(bit_depth, log2_size) + 1;
	int offset = shift == 0 ? 0 : 1 << (shift - 1);

	ap_int<512> out;
	for (i = 0; i < 32; i++) {
#pragma HLS PIPELINE II=1
	  for (j = 0; j < 32; j++) {
		int weight = (i | j) & 0xE0 ? 0 : coef[j][i];
		int lev = (weight * scale + offset) >> shift;
		int lev2 = (lev > 32767 ? 32767 : ((lev < -32768 ? -32768 : lev)));
					//coef_out[i][j] = (lev);
        out(16*j+15,16*j) = (s16)lev2;
	  }
      coef_out.write(out);
	}
}

void itrans_v_32_lite_s(hls::stream<s512_h> &coef_dq, s16 coef_temp[32][32], U4 bit_depth)
{
  U8 cu_width = 32;
  U8 cu_height = 32;
  U3 cu_width_log2 = 5;
	U4 shift1 = com_get_inverse_trans_shift(cu_width_log2, 0, bit_depth);
	s16 block1[32][32];

	U7 size = 32;
	U8 size_b = size - 1;

	inv_dct_col_32_lite_s(cu_width, cu_height, coef_dq, coef_temp, shift1);

}


void inv_dct_col_32_lite(U8 cu_width, U8 cu_height, s16 coeff[32][32], s16 block[32][32], U4 shift)
{
#pragma HLS ARRAY_PARTITION variable=coeff complete dim=1
#pragma HLS ARRAY_PARTITION variable=block complete dim=2
	U7 size = 32;
	U8 size_b = size - 1;
		if (cu_height == 16)
		{

			int E[8], O[8];
			int EE[4], EO[4];
			int EEE[2], EEO[2];
#pragma HLS ARRAY_PARTITION variable=E complete dim=0
#pragma HLS ARRAY_PARTITION variable=O complete dim=0
#pragma HLS ARRAY_PARTITION variable=EE complete dim=0
#pragma HLS ARRAY_PARTITION variable=EO complete dim=0
#pragma HLS ARRAY_PARTITION variable=EEE complete dim=0
#pragma HLS ARRAY_PARTITION variable=EEO complete dim=0
			int rnd_factor = 1 << (shift - 1);
			int k;
			for (int i = 0; i < 32; i++) {
#pragma HLS PIPELINE II=1
				O[0] = (45) * coeff[1][i] + (43) * coeff[3][i] + (40) * coeff[5][i] + (35) * coeff[7][i] + (29) * coeff[9][i] + (21) * coeff[11][i] + (13) * coeff[13][i] + (4) * coeff[15][i];
				O[1] = (43) * coeff[1][i] + (29) * coeff[3][i] + (4) * coeff[5][i] + (-21) * coeff[7][i] + (-40) * coeff[9][i] + (-45) * coeff[11][i] + (-35) * coeff[13][i] + (-13) * coeff[15][i];
				O[2] = (40) * coeff[1][i] + (4) * coeff[3][i] + (-35) * coeff[5][i] + (-43) * coeff[7][i] + (-13) * coeff[9][i] + (29) * coeff[11][i] + (45) * coeff[13][i] + (21) * coeff[15][i];
				O[3] = (35) * coeff[1][i] + (-21) * coeff[3][i] + (-43) * coeff[5][i] + (4) * coeff[7][i] + (45) * coeff[9][i] + (13) * coeff[11][i] + (-40) * coeff[13][i] + (-29) * coeff[15][i];
				O[4] = (29) * coeff[1][i] + (-40) * coeff[3][i] + (-13) * coeff[5][i] + (45) * coeff[7][i] + (-4) * coeff[9][i] + (-43) * coeff[11][i] + (21) * coeff[13][i] + (35) * coeff[15][i];
				O[5] = (21) * coeff[1][i] + (-45) * coeff[3][i] + (29) * coeff[5][i] + (13) * coeff[7][i] + (-43) * coeff[9][i] + (35) * coeff[11][i] + (4) * coeff[13][i] + (-40) * coeff[15][i];
				O[6] = (13) * coeff[1][i] + (-35) * coeff[3][i] + (45) * coeff[5][i] + (-40) * coeff[7][i] + (21) * coeff[9][i] + (4) * coeff[11][i] + (-29) * coeff[13][i] + (43) * coeff[15][i];
				O[7] = (4) * coeff[1][i] + (-13) * coeff[3][i] + (21) * coeff[5][i] + (-29) * coeff[7][i] + (35) * coeff[9][i] + (-40) * coeff[11][i] + (43) * coeff[13][i] + (-45) * coeff[15][i];

				EO[0] = (44) * coeff[2][i] + (38) * coeff[6][i] + (25) * coeff[10][i] + (9) * coeff[14][i];
				EO[1] = (38) * coeff[2][i] + (-9) * coeff[6][i] + (-44) * coeff[10][i] + (-25) * coeff[14][i];
				EO[2] = (25) * coeff[2][i] + (-44) * coeff[6][i] + (9) * coeff[10][i] + (38) * coeff[14][i];
				EO[3] = (9) * coeff[2][i] + (-25) * coeff[6][i] + (38) * coeff[10][i] + (-44) * coeff[14][i];

				EEO[0] = (42) * coeff[4][i] + (17) * coeff[12][i];
				EEE[0] = (32) * coeff[0][i] + (32) * coeff[8][i];
				EEO[1] = (17) * coeff[4][i] + (-42) * coeff[12][i];
				EEE[1] = (32) * coeff[0][i] + (-32) * coeff[8][i];

				for (k = 0; k < 2; k++) {
#pragma HLS UNROLL
					EE[k] = EEE[k] + EEO[k];
					EE[k + 2] = EEE[1 - k] - EEO[1 - k];
				}
				for (k = 0; k < 4; k++) {
#pragma HLS UNROLL
					E[k] = EE[k] + EO[k];
					E[k + 4] = EE[3 - k] - EO[3 - k];
				}
				for (k = 0; k < 8; k++) {
#pragma HLS UNROLL
					block[i][k] = Clip3(-(1 << 15), (1 << 15) - 1, (E[k] + O[k] + rnd_factor) >> shift);
					block[i][k + 8] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7 - k] - O[7 - k] + rnd_factor) >> shift);
				}
			}
		}
		else {

			int E[16], O[16];
			int EE[8], EO[8];
			int EEE[4], EEO[4];
			int EEEE[2], EEEO[2];
#pragma HLS ARRAY_PARTITION variable=E complete dim=0
#pragma HLS ARRAY_PARTITION variable=O complete dim=0
#pragma HLS ARRAY_PARTITION variable=EE complete dim=0
#pragma HLS ARRAY_PARTITION variable=EO complete dim=0
#pragma HLS ARRAY_PARTITION variable=EEE complete dim=0
#pragma HLS ARRAY_PARTITION variable=EEO complete dim=0
#pragma HLS ARRAY_PARTITION variable=EEEE complete dim=0
#pragma HLS ARRAY_PARTITION variable=EEEO complete dim=0
			int rnd_factor = 1 << (shift - 1);
			int k;
			for (int i = 0; i < 32; i++) {
#pragma HLS PIPELINE II=1
				O[0] = (45) * coeff[1][i] + (45) * coeff[3][i] + (44) * coeff[5][i] + (43) * coeff[7][i] + (41) * coeff[9][i] + (39) * coeff[11][i] + (36) * coeff[13][i] + (34) * coeff[15][i] + (30) * coeff[17][i] + (27) * coeff[19][i] + (23) * coeff[21][i] + (19) * coeff[23][i] + (15) * coeff[25][i] + (11) * coeff[27][i] + (7) * coeff[29][i] + (2) * coeff[31][i];
				O[1] = (45) * coeff[1][i] + (41) * coeff[3][i] + (34) * coeff[5][i] + (23) * coeff[7][i] + (11) * coeff[9][i] + (-2) * coeff[11][i] + (-15) * coeff[13][i] + (-27) * coeff[15][i] + (-36) * coeff[17][i] + (-43) * coeff[19][i] + (-45) * coeff[21][i] + (-44) * coeff[23][i] + (-39) * coeff[25][i] + (-30) * coeff[27][i] + (-19) * coeff[29][i] + (-7) * coeff[31][i];
				O[2] = (44) * coeff[1][i] + (34) * coeff[3][i] + (15) * coeff[5][i] + (-7) * coeff[7][i] + (-27) * coeff[9][i] + (-41) * coeff[11][i] + (-45) * coeff[13][i] + (-39) * coeff[15][i] + (-23) * coeff[17][i] + (-2) * coeff[19][i] + (19) * coeff[21][i] + (36) * coeff[23][i] + (45) * coeff[25][i] + (43) * coeff[27][i] + (30) * coeff[29][i] + (11) * coeff[31][i];
				O[3] = (43) * coeff[1][i] + (23) * coeff[3][i] + (-7) * coeff[5][i] + (-34) * coeff[7][i] + (-45) * coeff[9][i] + (-36) * coeff[11][i] + (-11) * coeff[13][i] + (19) * coeff[15][i] + (41) * coeff[17][i] + (44) * coeff[19][i] + (27) * coeff[21][i] + (-2) * coeff[23][i] + (-30) * coeff[25][i] + (-45) * coeff[27][i] + (-39) * coeff[29][i] + (-15) * coeff[31][i];
				O[4] = (41) * coeff[1][i] + (11) * coeff[3][i] + (-27) * coeff[5][i] + (-45) * coeff[7][i] + (-30) * coeff[9][i] + (7) * coeff[11][i] + (39) * coeff[13][i] + (43) * coeff[15][i] + (15) * coeff[17][i] + (-23) * coeff[19][i] + (-45) * coeff[21][i] + (-34) * coeff[23][i] + (2) * coeff[25][i] + (36) * coeff[27][i] + (44) * coeff[29][i] + (19) * coeff[31][i];
				O[5] = (39) * coeff[1][i] + (-2) * coeff[3][i] + (-41) * coeff[5][i] + (-36) * coeff[7][i] + (7) * coeff[9][i] + (43) * coeff[11][i] + (34) * coeff[13][i] + (-11) * coeff[15][i] + (-44) * coeff[17][i] + (-30) * coeff[19][i] + (15) * coeff[21][i] + (45) * coeff[23][i] + (27) * coeff[25][i] + (-19) * coeff[27][i] + (-45) * coeff[29][i] + (-23) * coeff[31][i];
				O[6] = (36) * coeff[1][i] + (-15) * coeff[3][i] + (-45) * coeff[5][i] + (-11) * coeff[7][i] + (39) * coeff[9][i] + (34) * coeff[11][i] + (-19) * coeff[13][i] + (-45) * coeff[15][i] + (-7) * coeff[17][i] + (41) * coeff[19][i] + (30) * coeff[21][i] + (-23) * coeff[23][i] + (-44) * coeff[25][i] + (-2) * coeff[27][i] + (43) * coeff[29][i] + (27) * coeff[31][i];
				O[7] = (34) * coeff[1][i] + (-27) * coeff[3][i] + (-39) * coeff[5][i] + (19) * coeff[7][i] + (43) * coeff[9][i] + (-11) * coeff[11][i] + (-45) * coeff[13][i] + (2) * coeff[15][i] + (45) * coeff[17][i] + (7) * coeff[19][i] + (-44) * coeff[21][i] + (-15) * coeff[23][i] + (41) * coeff[25][i] + (23) * coeff[27][i] + (-36) * coeff[29][i] + (-30) * coeff[31][i];
				O[8] = (30) * coeff[1][i] + (-36) * coeff[3][i] + (-23) * coeff[5][i] + (41) * coeff[7][i] + (15) * coeff[9][i] + (-44) * coeff[11][i] + (-7) * coeff[13][i] + (45) * coeff[15][i] + (-2) * coeff[17][i] + (-45) * coeff[19][i] + (11) * coeff[21][i] + (43) * coeff[23][i] + (-19) * coeff[25][i] + (-39) * coeff[27][i] + (27) * coeff[29][i] + (34) * coeff[31][i];
				O[9] = (27) * coeff[1][i] + (-43) * coeff[3][i] + (-2) * coeff[5][i] + (44) * coeff[7][i] + (-23) * coeff[9][i] + (-30) * coeff[11][i] + (41) * coeff[13][i] + (7) * coeff[15][i] + (-45) * coeff[17][i] + (19) * coeff[19][i] + (34) * coeff[21][i] + (-39) * coeff[23][i] + (-11) * coeff[25][i] + (45) * coeff[27][i] + (-15) * coeff[29][i] + (-36) * coeff[31][i];
				O[10] = (23) * coeff[1][i] + (-45) * coeff[3][i] + (19) * coeff[5][i] + (27) * coeff[7][i] + (-45) * coeff[9][i] + (15) * coeff[11][i] + (30) * coeff[13][i] + (-44) * coeff[15][i] + (11) * coeff[17][i] + (34) * coeff[19][i] + (-43) * coeff[21][i] + (7) * coeff[23][i] + (36) * coeff[25][i] + (-41) * coeff[27][i] + (2) * coeff[29][i] + (39) * coeff[31][i];
				O[11] = (19) * coeff[1][i] + (-44) * coeff[3][i] + (36) * coeff[5][i] + (-2) * coeff[7][i] + (-34) * coeff[9][i] + (45) * coeff[11][i] + (-23) * coeff[13][i] + (-15) * coeff[15][i] + (43) * coeff[17][i] + (-39) * coeff[19][i] + (7) * coeff[21][i] + (30) * coeff[23][i] + (-45) * coeff[25][i] + (27) * coeff[27][i] + (11) * coeff[29][i] + (-41) * coeff[31][i];
				O[12] = (15) * coeff[1][i] + (-39) * coeff[3][i] + (45) * coeff[5][i] + (-30) * coeff[7][i] + (2) * coeff[9][i] + (27) * coeff[11][i] + (-44) * coeff[13][i] + (41) * coeff[15][i] + (-19) * coeff[17][i] + (-11) * coeff[19][i] + (36) * coeff[21][i] + (-45) * coeff[23][i] + (34) * coeff[25][i] + (-7) * coeff[27][i] + (-23) * coeff[29][i] + (43) * coeff[31][i];
				O[13] = (11) * coeff[1][i] + (-30) * coeff[3][i] + (43) * coeff[5][i] + (-45) * coeff[7][i] + (36) * coeff[9][i] + (-19) * coeff[11][i] + (-2) * coeff[13][i] + (23) * coeff[15][i] + (-39) * coeff[17][i] + (45) * coeff[19][i] + (-41) * coeff[21][i] + (27) * coeff[23][i] + (-7) * coeff[25][i] + (-15) * coeff[27][i] + (34) * coeff[29][i] + (-44) * coeff[31][i];
				O[14] = (7) * coeff[1][i] + (-19) * coeff[3][i] + (30) * coeff[5][i] + (-39) * coeff[7][i] + (44) * coeff[9][i] + (-45) * coeff[11][i] + (43) * coeff[13][i] + (-36) * coeff[15][i] + (27) * coeff[17][i] + (-15) * coeff[19][i] + (2) * coeff[21][i] + (11) * coeff[23][i] + (-23) * coeff[25][i] + (34) * coeff[27][i] + (-41) * coeff[29][i] + (45) * coeff[31][i];
				O[15] = (2) * coeff[1][i] + (-7) * coeff[3][i] + (11) * coeff[5][i] + (-15) * coeff[7][i] + (19) * coeff[9][i] + (-23) * coeff[11][i] + (27) * coeff[13][i] + (-30) * coeff[15][i] + (34) * coeff[17][i] + (-36) * coeff[19][i] + (39) * coeff[21][i] + (-41) * coeff[23][i] + (43) * coeff[25][i] + (-44) * coeff[27][i] + (45) * coeff[29][i] + (-45) * coeff[31][i];

				EO[0] = (45) * coeff[2][i] + (43) * coeff[6][i] + (40) * coeff[10][i] + (35) * coeff[14][i] + (29) * coeff[18][i] + (21) * coeff[22][i] + (13) * coeff[26][i] + (4) * coeff[30][i];
				EO[1] = (43) * coeff[2][i] + (29) * coeff[6][i] + (4) * coeff[10][i] + (-21) * coeff[14][i] + (-40) * coeff[18][i] + (-45) * coeff[22][i] + (-35) * coeff[26][i] + (-13) * coeff[30][i];
				EO[2] = (40) * coeff[2][i] + (4) * coeff[6][i] + (-35) * coeff[10][i] + (-43) * coeff[14][i] + (-13) * coeff[18][i] + (29) * coeff[22][i] + (45) * coeff[26][i] + (21) * coeff[30][i];
				EO[3] = (35) * coeff[2][i] + (-21) * coeff[6][i] + (-43) * coeff[10][i] + (4) * coeff[14][i] + (45) * coeff[18][i] + (13) * coeff[22][i] + (-40) * coeff[26][i] + (-29) * coeff[30][i];
				EO[4] = (29) * coeff[2][i] + (-40) * coeff[6][i] + (-13) * coeff[10][i] + (45) * coeff[14][i] + (-4) * coeff[18][i] + (-43) * coeff[22][i] + (21) * coeff[26][i] + (35) * coeff[30][i];
				EO[5] = (21) * coeff[2][i] + (-45) * coeff[6][i] + (29) * coeff[10][i] + (13) * coeff[14][i] + (-43) * coeff[18][i] + (35) * coeff[22][i] + (4) * coeff[26][i] + (-40) * coeff[30][i];
				EO[6] = (13) * coeff[2][i] + (-35) * coeff[6][i] + (45) * coeff[10][i] + (-40) * coeff[14][i] + (21) * coeff[18][i] + (4) * coeff[22][i] + (-29) * coeff[26][i] + (43) * coeff[30][i];
				EO[7] = (4) * coeff[2][i] + (-13) * coeff[6][i] + (21) * coeff[10][i] + (-29) * coeff[14][i] + (35) * coeff[18][i] + (-40) * coeff[22][i] + (43) * coeff[26][i] + (-45) * coeff[30][i];

				EEO[0] = (44) * coeff[4][i] + (38) * coeff[12][i] + (25) * coeff[20][i] + (9) * coeff[28][i];
				EEO[1] = (38) * coeff[4][i] + (-9) * coeff[12][i] + (-44) * coeff[20][i] + (-25) * coeff[28][i];
				EEO[2] = (25) * coeff[4][i] + (-44) * coeff[12][i] + (9) * coeff[20][i] + (38) * coeff[28][i];
				EEO[3] = (9) * coeff[4][i] + (-25) * coeff[12][i] + (38) * coeff[20][i] + (-44) * coeff[28][i];

				EEEO[0] = (42) * coeff[8][i] + (17) * coeff[24][i];
				EEEO[1] = (17) * coeff[8][i] + (-42) * coeff[24][i];
				EEEE[0] = (32) * coeff[0][i] + (32) * coeff[16][i];
				EEEE[1] = (32) * coeff[0][i] + (-32) * coeff[16][i];

				EEE[0] = EEEE[0] + EEEO[0];
				EEE[3] = EEEE[0] - EEEO[0];
				EEE[1] = EEEE[1] + EEEO[1];
				EEE[2] = EEEE[1] - EEEO[1];

				for (k = 0; k < 4; k++) {
#pragma HLS UNROLL
					EE[k] = EEE[k] + EEO[k];
					EE[k + 4] = EEE[3 - k] - EEO[3 - k];
				}
				for (k = 0; k < 8; k++) {
#pragma HLS UNROLL
					E[k] = EE[k] + EO[k];
					E[k + 8] = EE[7 - k] - EO[7 - k];
				}
				for (k = 0; k < 16; k++) {
#pragma HLS UNROLL
					block[(i + k) & size_b][k] = Clip3(-(1 << 15), (1 << 15) - 1, (E[k] + O[k] + rnd_factor) >> shift);
					block[(i + k + 16) & size_b][k + 16] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15 - k] - O[15 - k] + rnd_factor) >> shift);
				}
			}


		}
}

void itrans_v_32_lite(s16 coef_dq[32][32], s16 coef_temp[32][32], U4 bit_depth)
{
//#pragma HLS INLINE
	U8 cu_width = 32;
	U8 cu_height = 32;
	U3 cu_width_log2 = 5;

	U4 shift1 = com_get_inverse_trans_shift(cu_width_log2, 0, bit_depth);
	s16 block1[32][32];

	U7 size = 32;
	U8 size_b = size - 1;

	inv_dct_col_32_lite(cu_width, cu_height, coef_dq, coef_temp, shift1);

}





void inv_dct_row_32_lite_s(U8 cu_width, U8 cu_height, s16 coeff[32][32], hls::stream<s512_h> &block, U4 shift)
{
	  U7 size = 32;
	  U8 size_b = size - 1;

		int E[16];
#pragma HLS ARRAY_PARTITION variable=E complete dim=0
		int O[16];
#pragma HLS ARRAY_PARTITION variable=O complete dim=0
		int EE[8];
#pragma HLS ARRAY_PARTITION variable=EE complete dim=0
		int EO[8];
#pragma HLS ARRAY_PARTITION variable=EO complete dim=0
		int EEE[4];
#pragma HLS ARRAY_PARTITION variable=EEE complete dim=0
		int EEO[4];
#pragma HLS ARRAY_PARTITION variable=EEO complete dim=0
		int EEEE[2];
#pragma HLS ARRAY_PARTITION variable=EEEE complete dim=0
		int EEEO[2];
#pragma HLS ARRAY_PARTITION variable=EEEO complete dim=0

		int rnd_factor = 1 << (shift - 1);
		int k;

		ap_int<512> out;

		for (int i = 0; i < 32; i++) {
#pragma HLS PIPELINE II=1
				O[0] = (45) * coeff[(1 + i) & size_b][i] + (45) * coeff[(3 + i) & size_b][i] + (44) * coeff[(5 + i) & size_b][i] + (43) * coeff[(7 + i) & size_b][i] + (41) * coeff[(9 + i) & size_b][i] + (39) * coeff[(11 + i) & size_b][i] + (36) * coeff[(13 + i) & size_b][i] + (34) * coeff[(15 + i) & size_b][i] + (30) * coeff[(17 + i) & size_b][i] + (27) * coeff[(19 + i) & size_b][i] + (23) * coeff[(21 + i) & size_b][i] + (19) * coeff[(23 + i) & size_b][i] + (15) * coeff[(25 + i) & size_b][i] + (11) * coeff[(27 + i) & size_b][i] + (7) * coeff[(29 + i) & size_b][i] + (2) * coeff[(31 + i) & size_b][i];
				O[1] = (45) * coeff[(1 + i) & size_b][i] + (41) * coeff[(3 + i) & size_b][i] + (34) * coeff[(5 + i) & size_b][i] + (23) * coeff[(7 + i) & size_b][i] + (11) * coeff[(9 + i) & size_b][i] + (-2) * coeff[(11 + i) & size_b][i] + (-15) * coeff[(13 + i) & size_b][i] + (-27) * coeff[(15 + i) & size_b][i] + (-36) * coeff[(17 + i) & size_b][i] + (-43) * coeff[(19 + i) & size_b][i] + (-45) * coeff[(21 + i) & size_b][i] + (-44) * coeff[(23 + i) & size_b][i] + (-39) * coeff[(25 + i) & size_b][i] + (-30) * coeff[(27 + i) & size_b][i] + (-19) * coeff[(29 + i) & size_b][i] + (-7) * coeff[(31 + i) & size_b][i];
				O[2] = (44) * coeff[(1 + i) & size_b][i] + (34) * coeff[(3 + i) & size_b][i] + (15) * coeff[(5 + i) & size_b][i] + (-7) * coeff[(7 + i) & size_b][i] + (-27) * coeff[(9 + i) & size_b][i] + (-41) * coeff[(11 + i) & size_b][i] + (-45) * coeff[(13 + i) & size_b][i] + (-39) * coeff[(15 + i) & size_b][i] + (-23) * coeff[(17 + i) & size_b][i] + (-2) * coeff[(19 + i) & size_b][i] + (19) * coeff[(21 + i) & size_b][i] + (36) * coeff[(23 + i) & size_b][i] + (45) * coeff[(25 + i) & size_b][i] + (43) * coeff[(27 + i) & size_b][i] + (30) * coeff[(29 + i) & size_b][i] + (11) * coeff[(31 + i) & size_b][i];
				O[3] = (43) * coeff[(1 + i) & size_b][i] + (23) * coeff[(3 + i) & size_b][i] + (-7) * coeff[(5 + i) & size_b][i] + (-34) * coeff[(7 + i) & size_b][i] + (-45) * coeff[(9 + i) & size_b][i] + (-36) * coeff[(11 + i) & size_b][i] + (-11) * coeff[(13 + i) & size_b][i] + (19) * coeff[(15 + i) & size_b][i] + (41) * coeff[(17 + i) & size_b][i] + (44) * coeff[(19 + i) & size_b][i] + (27) * coeff[(21 + i) & size_b][i] + (-2) * coeff[(23 + i) & size_b][i] + (-30) * coeff[(25 + i) & size_b][i] + (-45) * coeff[(27 + i) & size_b][i] + (-39) * coeff[(29 + i) & size_b][i] + (-15) * coeff[(31 + i) & size_b][i];
				O[4] = (41) * coeff[(1 + i) & size_b][i] + (11) * coeff[(3 + i) & size_b][i] + (-27) * coeff[(5 + i) & size_b][i] + (-45) * coeff[(7 + i) & size_b][i] + (-30) * coeff[(9 + i) & size_b][i] + (7) * coeff[(11 + i) & size_b][i] + (39) * coeff[(13 + i) & size_b][i] + (43) * coeff[(15 + i) & size_b][i] + (15) * coeff[(17 + i) & size_b][i] + (-23) * coeff[(19 + i) & size_b][i] + (-45) * coeff[(21 + i) & size_b][i] + (-34) * coeff[(23 + i) & size_b][i] + (2) * coeff[(25 + i) & size_b][i] + (36) * coeff[(27 + i) & size_b][i] + (44) * coeff[(29 + i) & size_b][i] + (19) * coeff[(31 + i) & size_b][i];
				O[5] = (39) * coeff[(1 + i) & size_b][i] + (-2) * coeff[(3 + i) & size_b][i] + (-41) * coeff[(5 + i) & size_b][i] + (-36) * coeff[(7 + i) & size_b][i] + (7) * coeff[(9 + i) & size_b][i] + (43) * coeff[(11 + i) & size_b][i] + (34) * coeff[(13 + i) & size_b][i] + (-11) * coeff[(15 + i) & size_b][i] + (-44) * coeff[(17 + i) & size_b][i] + (-30) * coeff[(19 + i) & size_b][i] + (15) * coeff[(21 + i) & size_b][i] + (45) * coeff[(23 + i) & size_b][i] + (27) * coeff[(25 + i) & size_b][i] + (-19) * coeff[(27 + i) & size_b][i] + (-45) * coeff[(29 + i) & size_b][i] + (-23) * coeff[(31 + i) & size_b][i];
				O[6] = (36) * coeff[(1 + i) & size_b][i] + (-15) * coeff[(3 + i) & size_b][i] + (-45) * coeff[(5 + i) & size_b][i] + (-11) * coeff[(7 + i) & size_b][i] + (39) * coeff[(9 + i) & size_b][i] + (34) * coeff[(11 + i) & size_b][i] + (-19) * coeff[(13 + i) & size_b][i] + (-45) * coeff[(15 + i) & size_b][i] + (-7) * coeff[(17 + i) & size_b][i] + (41) * coeff[(19 + i) & size_b][i] + (30) * coeff[(21 + i) & size_b][i] + (-23) * coeff[(23 + i) & size_b][i] + (-44) * coeff[(25 + i) & size_b][i] + (-2) * coeff[(27 + i) & size_b][i] + (43) * coeff[(29 + i) & size_b][i] + (27) * coeff[(31 + i) & size_b][i];
				O[7] = (34) * coeff[(1 + i) & size_b][i] + (-27) * coeff[(3 + i) & size_b][i] + (-39) * coeff[(5 + i) & size_b][i] + (19) * coeff[(7 + i) & size_b][i] + (43) * coeff[(9 + i) & size_b][i] + (-11) * coeff[(11 + i) & size_b][i] + (-45) * coeff[(13 + i) & size_b][i] + (2) * coeff[(15 + i) & size_b][i] + (45) * coeff[(17 + i) & size_b][i] + (7) * coeff[(19 + i) & size_b][i] + (-44) * coeff[(21 + i) & size_b][i] + (-15) * coeff[(23 + i) & size_b][i] + (41) * coeff[(25 + i) & size_b][i] + (23) * coeff[(27 + i) & size_b][i] + (-36) * coeff[(29 + i) & size_b][i] + (-30) * coeff[(31 + i) & size_b][i];
				O[8] = (30) * coeff[(1 + i) & size_b][i] + (-36) * coeff[(3 + i) & size_b][i] + (-23) * coeff[(5 + i) & size_b][i] + (41) * coeff[(7 + i) & size_b][i] + (15) * coeff[(9 + i) & size_b][i] + (-44) * coeff[(11 + i) & size_b][i] + (-7) * coeff[(13 + i) & size_b][i] + (45) * coeff[(15 + i) & size_b][i] + (-2) * coeff[(17 + i) & size_b][i] + (-45) * coeff[(19 + i) & size_b][i] + (11) * coeff[(21 + i) & size_b][i] + (43) * coeff[(23 + i) & size_b][i] + (-19) * coeff[(25 + i) & size_b][i] + (-39) * coeff[(27 + i) & size_b][i] + (27) * coeff[(29 + i) & size_b][i] + (34) * coeff[(31 + i) & size_b][i];
				O[9] = (27) * coeff[(1 + i) & size_b][i] + (-43) * coeff[(3 + i) & size_b][i] + (-2) * coeff[(5 + i) & size_b][i] + (44) * coeff[(7 + i) & size_b][i] + (-23) * coeff[(9 + i) & size_b][i] + (-30) * coeff[(11 + i) & size_b][i] + (41) * coeff[(13 + i) & size_b][i] + (7) * coeff[(15 + i) & size_b][i] + (-45) * coeff[(17 + i) & size_b][i] + (19) * coeff[(19 + i) & size_b][i] + (34) * coeff[(21 + i) & size_b][i] + (-39) * coeff[(23 + i) & size_b][i] + (-11) * coeff[(25 + i) & size_b][i] + (45) * coeff[(27 + i) & size_b][i] + (-15) * coeff[(29 + i) & size_b][i] + (-36) * coeff[(31 + i) & size_b][i];
				O[10] = (23) * coeff[(1 + i) & size_b][i] + (-45) * coeff[(3 + i) & size_b][i] + (19) * coeff[(5 + i) & size_b][i] + (27) * coeff[(7 + i) & size_b][i] + (-45) * coeff[(9 + i) & size_b][i] + (15) * coeff[(11 + i) & size_b][i] + (30) * coeff[(13 + i) & size_b][i] + (-44) * coeff[(15 + i) & size_b][i] + (11) * coeff[(17 + i) & size_b][i] + (34) * coeff[(19 + i) & size_b][i] + (-43) * coeff[(21 + i) & size_b][i] + (7) * coeff[(23 + i) & size_b][i] + (36) * coeff[(25 + i) & size_b][i] + (-41) * coeff[(27 + i) & size_b][i] + (2) * coeff[(29 + i) & size_b][i] + (39) * coeff[(31 + i) & size_b][i];
				O[11] = (19) * coeff[(1 + i) & size_b][i] + (-44) * coeff[(3 + i) & size_b][i] + (36) * coeff[(5 + i) & size_b][i] + (-2) * coeff[(7 + i) & size_b][i] + (-34) * coeff[(9 + i) & size_b][i] + (45) * coeff[(11 + i) & size_b][i] + (-23) * coeff[(13 + i) & size_b][i] + (-15) * coeff[(15 + i) & size_b][i] + (43) * coeff[(17 + i) & size_b][i] + (-39) * coeff[(19 + i) & size_b][i] + (7) * coeff[(21 + i) & size_b][i] + (30) * coeff[(23 + i) & size_b][i] + (-45) * coeff[(25 + i) & size_b][i] + (27) * coeff[(27 + i) & size_b][i] + (11) * coeff[(29 + i) & size_b][i] + (-41) * coeff[(31 + i) & size_b][i];
				O[12] = (15) * coeff[(1 + i) & size_b][i] + (-39) * coeff[(3 + i) & size_b][i] + (45) * coeff[(5 + i) & size_b][i] + (-30) * coeff[(7 + i) & size_b][i] + (2) * coeff[(9 + i) & size_b][i] + (27) * coeff[(11 + i) & size_b][i] + (-44) * coeff[(13 + i) & size_b][i] + (41) * coeff[(15 + i) & size_b][i] + (-19) * coeff[(17 + i) & size_b][i] + (-11) * coeff[(19 + i) & size_b][i] + (36) * coeff[(21 + i) & size_b][i] + (-45) * coeff[(23 + i) & size_b][i] + (34) * coeff[(25 + i) & size_b][i] + (-7) * coeff[(27 + i) & size_b][i] + (-23) * coeff[(29 + i) & size_b][i] + (43) * coeff[(31 + i) & size_b][i];
				O[13] = (11) * coeff[(1 + i) & size_b][i] + (-30) * coeff[(3 + i) & size_b][i] + (43) * coeff[(5 + i) & size_b][i] + (-45) * coeff[(7 + i) & size_b][i] + (36) * coeff[(9 + i) & size_b][i] + (-19) * coeff[(11 + i) & size_b][i] + (-2) * coeff[(13 + i) & size_b][i] + (23) * coeff[(15 + i) & size_b][i] + (-39) * coeff[(17 + i) & size_b][i] + (45) * coeff[(19 + i) & size_b][i] + (-41) * coeff[(21 + i) & size_b][i] + (27) * coeff[(23 + i) & size_b][i] + (-7) * coeff[(25 + i) & size_b][i] + (-15) * coeff[(27 + i) & size_b][i] + (34) * coeff[(29 + i) & size_b][i] + (-44) * coeff[(31 + i) & size_b][i];
				O[14] = (7) * coeff[(1 + i) & size_b][i] + (-19) * coeff[(3 + i) & size_b][i] + (30) * coeff[(5 + i) & size_b][i] + (-39) * coeff[(7 + i) & size_b][i] + (44) * coeff[(9 + i) & size_b][i] + (-45) * coeff[(11 + i) & size_b][i] + (43) * coeff[(13 + i) & size_b][i] + (-36) * coeff[(15 + i) & size_b][i] + (27) * coeff[(17 + i) & size_b][i] + (-15) * coeff[(19 + i) & size_b][i] + (2) * coeff[(21 + i) & size_b][i] + (11) * coeff[(23 + i) & size_b][i] + (-23) * coeff[(25 + i) & size_b][i] + (34) * coeff[(27 + i) & size_b][i] + (-41) * coeff[(29 + i) & size_b][i] + (45) * coeff[(31 + i) & size_b][i];
				O[15] = (2) * coeff[(1 + i) & size_b][i] + (-7) * coeff[(3 + i) & size_b][i] + (11) * coeff[(5 + i) & size_b][i] + (-15) * coeff[(7 + i) & size_b][i] + (19) * coeff[(9 + i) & size_b][i] + (-23) * coeff[(11 + i) & size_b][i] + (27) * coeff[(13 + i) & size_b][i] + (-30) * coeff[(15 + i) & size_b][i] + (34) * coeff[(17 + i) & size_b][i] + (-36) * coeff[(19 + i) & size_b][i] + (39) * coeff[(21 + i) & size_b][i] + (-41) * coeff[(23 + i) & size_b][i] + (43) * coeff[(25 + i) & size_b][i] + (-44) * coeff[(27 + i) & size_b][i] + (45) * coeff[(29 + i) & size_b][i] + (-45) * coeff[(31 + i) & size_b][i];

				EO[0] = (45) * coeff[(2 + i) & size_b][i] + (43) * coeff[(6 + i) & size_b][i] + (40) * coeff[(10 + i) & size_b][i] + (35) * coeff[(14 + i) & size_b][i] + (29) * coeff[(18 + i) & size_b][i] + (21) * coeff[(22 + i) & size_b][i] + (13) * coeff[(26 + i) & size_b][i] + (4) * coeff[(30 + i) & size_b][i];
				EO[1] = (43) * coeff[(2 + i) & size_b][i] + (29) * coeff[(6 + i) & size_b][i] + (4) * coeff[(10 + i) & size_b][i] + (-21) * coeff[(14 + i) & size_b][i] + (-40) * coeff[(18 + i) & size_b][i] + (-45) * coeff[(22 + i) & size_b][i] + (-35) * coeff[(26 + i) & size_b][i] + (-13) * coeff[(30 + i) & size_b][i];
				EO[2] = (40) * coeff[(2 + i) & size_b][i] + (4) * coeff[(6 + i) & size_b][i] + (-35) * coeff[(10 + i) & size_b][i] + (-43) * coeff[(14 + i) & size_b][i] + (-13) * coeff[(18 + i) & size_b][i] + (29) * coeff[(22 + i) & size_b][i] + (45) * coeff[(26 + i) & size_b][i] + (21) * coeff[(30 + i) & size_b][i];
				EO[3] = (35) * coeff[(2 + i) & size_b][i] + (-21) * coeff[(6 + i) & size_b][i] + (-43) * coeff[(10 + i) & size_b][i] + (4) * coeff[(14 + i) & size_b][i] + (45) * coeff[(18 + i) & size_b][i] + (13) * coeff[(22 + i) & size_b][i] + (-40) * coeff[(26 + i) & size_b][i] + (-29) * coeff[(30 + i) & size_b][i];
				EO[4] = (29) * coeff[(2 + i) & size_b][i] + (-40) * coeff[(6 + i) & size_b][i] + (-13) * coeff[(10 + i) & size_b][i] + (45) * coeff[(14 + i) & size_b][i] + (-4) * coeff[(18 + i) & size_b][i] + (-43) * coeff[(22 + i) & size_b][i] + (21) * coeff[(26 + i) & size_b][i] + (35) * coeff[(30 + i) & size_b][i];
				EO[5] = (21) * coeff[(2 + i) & size_b][i] + (-45) * coeff[(6 + i) & size_b][i] + (29) * coeff[(10 + i) & size_b][i] + (13) * coeff[(14 + i) & size_b][i] + (-43) * coeff[(18 + i) & size_b][i] + (35) * coeff[(22 + i) & size_b][i] + (4) * coeff[(26 + i) & size_b][i] + (-40) * coeff[(30 + i) & size_b][i];
				EO[6] = (13) * coeff[(2 + i) & size_b][i] + (-35) * coeff[(6 + i) & size_b][i] + (45) * coeff[(10 + i) & size_b][i] + (-40) * coeff[(14 + i) & size_b][i] + (21) * coeff[(18 + i) & size_b][i] + (4) * coeff[(22 + i) & size_b][i] + (-29) * coeff[(26 + i) & size_b][i] + (43) * coeff[(30 + i) & size_b][i];
				EO[7] = (4) * coeff[(2 + i) & size_b][i] + (-13) * coeff[(6 + i) & size_b][i] + (21) * coeff[(10 + i) & size_b][i] + (-29) * coeff[(14 + i) & size_b][i] + (35) * coeff[(18 + i) & size_b][i] + (-40) * coeff[(22 + i) & size_b][i] + (43) * coeff[(26 + i) & size_b][i] + (-45) * coeff[(30 + i) & size_b][i];

				EEO[0] = (44) * coeff[(4 + i) & size_b][i] + (38) * coeff[(12 + i) & size_b][i] + (25) * coeff[(20 + i) & size_b][i] + (9) * coeff[(28 + i) & size_b][i];
				EEO[1] = (38) * coeff[(4 + i) & size_b][i] + (-9) * coeff[(12 + i) & size_b][i] + (-44) * coeff[(20 + i) & size_b][i] + (-25) * coeff[(28 + i) & size_b][i];
				EEO[2] = (25) * coeff[(4 + i) & size_b][i] + (-44) * coeff[(12 + i) & size_b][i] + (9) * coeff[(20 + i) & size_b][i] + (38) * coeff[(28 + i) & size_b][i];
				EEO[3] = (9) * coeff[(4 + i) & size_b][i] + (-25) * coeff[(12 + i) & size_b][i] + (38) * coeff[(20 + i) & size_b][i] + (-44) * coeff[(28 + i) & size_b][i];

				EEEO[0] = (42) * coeff[(8 + i) & size_b][i] + (17) * coeff[(24 + i) & size_b][i];
				EEEO[1] = (17) * coeff[(8 + i) & size_b][i] + (-42) * coeff[(24 + i) & size_b][i];
				EEEE[0] = (32) * coeff[(0 + i) & size_b][i] + (32) * coeff[(16 + i) & size_b][i];
				EEEE[1] = (32) * coeff[(0 + i) & size_b][i] + (-32) * coeff[(16 + i) & size_b][i];
				EEE[0] = EEEE[0] + EEEO[0];
				EEE[3] = EEEE[0] - EEEO[0];
				EEE[1] = EEEE[1] + EEEO[1];
				EEE[2] = EEEE[1] - EEEO[1];
				for (k = 0; k < 4; k++) {
					EE[k] = EEE[k] + EEO[k];
					EE[k + 4] = EEE[3 - k] - EEO[3 - k];
				}
				for (k = 0; k < 8; k++) {
					E[k] = EE[k] + EO[k];
					E[k + 8] = EE[7 - k] - EO[7 - k];
				}
				for (k = 0; k < 16; k++) {
					//block[i][k] = Clip3(-(1 << 15), (1 << 15) - 1, (E[k] + O[k] + rnd_factor) >> shift);
					//block[i][k + 16] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15 - k] - O[15 - k] + rnd_factor) >> shift);
					out(16*k+15,16*k) = Clip3(-(1 << 15), (1 << 15) - 1, (E[k] + O[k] + rnd_factor) >> shift);
					out(256+16*k+15,256+16*k) = Clip3(-(1 << 15), (1 << 15) - 1, (E[15 - k] - O[15 - k] + rnd_factor) >> shift);
				}
				block.write(out);
			}
}



//void itrans_h_32_lite(U8 cu_width, U8 cu_height, s16 resi[32][32], s16 coef_temp[32][32], U3 cu_height_log2, U4 bit_depth)
void itrans_h_32_lite_s(hls::stream<s512_h> &resi,s16 coef_temp[32][32],U4 bit_depth)
{
	U4 shift2;
	s16 block1[32][32];
	U7 size = 32;
	U8 size_b = size - 1;
  U8 cu_width = 32; 
  U8 cu_height = 32;
  U3 cu_height_log2 = 5;

	{
		shift2 = com_get_inverse_trans_shift(cu_height_log2, 1, bit_depth);
		inv_dct_row_32_lite_s(cu_width, cu_height, coef_temp, resi, shift2);
	}
}

void recon_32_lite_s(
  hls::stream<s512_h> &resi,
  hls::stream<s512_h> &pred,
  int is_coef,
  U3 plane, 
  hls::stream<s512_h> &rec,
  U4 bit_depth)
{
	int i;
	int j;
	pel p;
	U7 cu_width = 32;
	U7 cu_height = 32;
	ap_int<512> in1;
	ap_int<512> in2;
	ap_int<512> out;
	U1 is_coef_not_zero = is_coef != 0;
	for (i = 0; i < cu_height; i++) {
#pragma HLS PIPELINE II=1
		in1 = resi.read();
		in2 = pred.read();
		for (j = 0; j < cu_width; j++) {
			p = in2(16*j+15,16*j);
			if (is_coef_not_zero)
				p += in1(16*j+15,16*j);
			 out(16*j+15,16*j) = (((0 > (((1 << bit_depth) - 1 < p ? (1 << bit_depth) - 1 : p)) ? 0 : (((1 << bit_depth) - 1 < p ? (1 << bit_depth) - 1 : p)))));
		}
		rec.write(out);
	}
}

s64 enc_ssd_16b_1_32_s(
  hls::stream<s512_h> &src1,
  hls::stream<s512_h> &src2,
  U4 bit_depth)
{
	int offset2 = 0;
	U3 w_log2 = 5;
	U3 h_log2 = 5;
	U8 w = 1 << w_log2;
	U8 h = 1 << h_log2;
	const int shift = (bit_depth - 8) << 1;
	s64 ssd = 0;
	U8 i, j;
	int diff;

	ap_int<512> in1;
	ap_int<512> in2;

	for (i = 0; i < 32; i++) {
#pragma HLS PIPELINE II=1
		in1 = src1.read();
		in2 = src2.read();
		for (j = 0; j < 32; j++) {
				diff = in1(16*j+15,16*j) - in2(16*j+15,16*j);
				ssd += ((diff * diff >> shift));
		}
	}
	return ssd;
	//add by xujch
}


void enc_ssd_16b_1_32_s(
  hls::stream<s512_h> &src1,
  hls::stream<s512_h> &src2,
  U4 bit_depth,
  s32 dist_t[1])
{
	int offset2 = 0;
	U3 w_log2 = 5;
	U3 h_log2 = 5;
	U8 w = 1 << w_log2;
	U8 h = 1 << h_log2;
	const int shift = (bit_depth - 8) << 1;
	s64 ssd = 0;
	U8 i, j;
	int diff;

	ap_int<512> in1;
	ap_int<512> in2;

	for (i = 0; i < 32; i++) {
#pragma HLS PIPELINE II=1
		in1 = src1.read();
		in2 = src2.read();
		for (j = 0; j < 32; j++) {
				diff = in1(16*j+15,16*j) - in2(16*j+15,16*j);
				ssd += ((diff * diff >> shift));
		}
	}
	dist_t[0] = (s32)ssd;
	//add by xujch
}
void enc_ssd_16b_1_32_s(
  hls::stream<s512_h> &src1,
  hls::stream<s512_h> &src2,
  U4 bit_depth,
  s64 *dist)
{
	int offset2 = 0;
	U3 w_log2 = 5;
	U3 h_log2 = 5;
	U8 w = 1 << w_log2;
	U8 h = 1 << h_log2;
	const int shift = (bit_depth - 8) << 1;
	s64 ssd = 0;
	U8 i, j;
	int diff;

	ap_int<512> in1;
	ap_int<512> in2;

	for (i = 0; i < 32; i++) {
#pragma HLS PIPELINE II=1
		in1 = src1.read();
		in2 = src2.read();
		for (j = 0; j < 32; j++) {
				diff = in1(16*j+15,16*j) - in2(16*j+15,16*j);
				ssd += ((diff * diff >> shift));
		}
	}
	* dist = ssd;
	//add by xujch
}



#if NO_SUB_MODEL_SYN
int rdoq_top_32_32(
  RDOQ_MODEL* model_cnt, 
  U6 qp, 
  s64 d_lambda, 
  U1 is_intra, 
  s16 src_coef[32][32], 
  s16 dst_tmp[32][32],
  U4 bit_depth, 
  RDOQ_ARRAY* rdoq_array,
  int *final_x,
  int *final_y)
{

  U2 ch_type = 0; 
  U8 cu_width = 32; 
  U8 cu_height = 32; 
  U3 cu_width_log2 = 5;
  U3 cu_height_log2 = 5; 
	const int log2_size = (cu_width_log2 + cu_height_log2) >> 1;
	const int tr_shift = get_transform_shift(bit_depth, log2_size);

	const U8 w = 1 << cu_width_log2;
	const U8 h = 1 << cu_height_log2;
	const U14 max_num_coef = 528;
	const s64 lambda = (d_lambda * (1 << 15) + 500) >> 10;
	U30 err_scale_tbl[80][6] = { 32932, 65865, 131731, 263462, 526925, 1053850, 35734, 71468, 142936, 285873, 571747, 1143495, 38968, 77937, 155874, 311749, 623498, 1246996, 42494, 84988, 169976, 339953, 679906, 1359812, 46178, 92356, 184713, 369427, 738855, 1477711, 50533, 101067, 202135, 404270, 808540, 1617081, 55108, 110217, 220435, 440871, 881742, 1763484, 59932, 119864, 239728, 479456, 958912, 1917824, 65536, 131072, 262144, 524288, 1048576, 2097152, 71468, 142936, 285873, 571747, 1143495, 2286990, 77931, 155863, 311726, 623452, 1246905, 2493811, 85150, 170300, 340600, 681200, 1362400, 2724800, 92675, 185351, 370703, 741406, 1482812, 2965625, 101239, 202478, 404956, 809912, 1619825, 3239650, 110059, 220119, 440238, 880477, 1760954, 3521908, 120186, 240372, 480744, 961488, 1922976, 3845952, 131232, 262464, 524928, 1049857, 2099715, 4199430, 142936, 285873, 571747, 1143495, 2286990, 4573980, 155885, 311771, 623543, 1247086, 2494173, 4988347, 169842, 339684, 679368, 1358736, 2717473, 5434947, 185511, 371023, 742046, 1484093, 2968187, 5936374, 202287, 404574, 809149, 1618299, 3236599, 6473198, 220480, 440961, 881923, 1763846, 3527693, 7055387, 240210, 480421, 960842, 1921685, 3843371, 7686742, 262144, 524288, 1048576, 2097152, 4194304, 8388608, 285873, 571747, 1143495, 2286990, 4573980, 9147960, 311771, 623543, 1247086, 2494173, 4988347, 9976695, 340006, 680013, 1360027, 2720055, 5440110, 10880221, 370511, 741022, 1482045, 2964090, 5928181, 11856362, 403966, 807932, 1615864, 3231728, 6463457, 12926914, 440780, 881561, 1763122, 3526245, 7052491, 14104982, 480636, 961272, 1922545, 3845091, 7690183, 15380366, 524288, 1048576, 2097152, 4194304, 8388608, 16777216, 571747, 1143495, 2286990, 4573980, 9147960, 18295920, 623543, 1247086, 2494173, 4988347, 9976695, 19953390, 679583, 1359166, 2718333, 5436667, 10873334, 21746669, 741534, 1483068, 2966137, 5932275, 11864550, 23729101, 808540, 1617081, 3234162, 6468324, 12936648, 25873296, 881561, 1763122, 3526245, 7052491, 14104982, 28209965, 962134, 1924268, 3848537, 7697074, 15394148, 30788296, 1048576, 2097152, 4194304, 8388608, 16777216, 33554432, 1142278, 2284557, 4569114, 9138228, 18276456, 36552913, 1248537, 2497074, 4994148, 9988296, 19976592, 39953184, 1359166, 2718333, 5436667, 10873334, 21746669, 43493339, 1483068, 2966137, 5932275, 11864550, 23729101, 47458202, 1617081, 3234162, 6468324, 12936648, 25873296, 51746593, 1766022, 3532045, 7064090, 14128181, 28256363, 56512727, 1924268, 3848537, 7697074, 15394148, 30788296, 61576592, 2097152, 4194304, 8388608, 16777216, 33554432, 67108864, 2284557, 4569114, 9138228, 18276456, 36552913, 73105826, 2497074, 4994148, 9988296, 19976592, 39953184, 79906368, 2725233, 5450466, 10900932, 21801864, 43603728, 87207457, 2966137, 5932275, 11864550, 23729101, 47458202, 94916404, 3234162, 6468324, 12936648, 25873296, 51746593, 103493187, 3532045, 7064090, 14128181, 28256363, 56512727, 113025455, 3834792, 7669584, 15339168, 30678337, 61356675, 122713351, 4194304, 8388608, 16777216, 33554432, 67108864, 134217728, 4628197, 9256395, 18512790, 37025580, 74051160, 148102320, 4971026, 9942053, 19884107, 39768215, 79536431, 159072862, 5422938, 10845877, 21691754, 43383508, 86767016, 173534032, 5899680, 11799360, 23598721, 47197442, 94394885, 188789771, 6468324, 12936648, 25873296, 51746593, 103493187, 206986375, 7064090, 14128181, 28256363, 56512727, 113025455, 226050910, 7780737, 15561475, 31122951, 62245902, 124491805, 248983611, 8388608, 16777216, 33554432, 67108864, 134217728, 268435456, 9099506, 18199013, 36398027, 72796055, 145592111, 291184223, 9942053, 19884107, 39768215, 79536431, 159072862, 318145725, 10956549, 21913098, 43826196, 87652393, 175304787, 350609575, 11930464, 23860929, 47721858, 95443717, 190887435, 381774870, 13094412, 26188824, 52377649, 104755299, 209510599, 419021199, 14128181, 28256363, 56512727, 113025455, 226050910, 452101820, 15339168, 30678337, 61356675, 122713351, 245426702, 490853405, 16777216, 33554432, 67108864, 134217728, 268435456, 536870912, 17895697, 35791394, 71582788, 143165576, 286331153, 572662306, 19884107, 39768215, 79536431, 159072862, 318145725, 636291451, 21474836, 42949672, 85899345, 171798691, 343597383, 687194767, 23342213, 46684427, 93368854, 186737708, 373475417, 746950834, 25565281, 51130563, 102261126, 204522252, 409044504, 818089008, 28256363, 56512727, 113025455, 226050910, 452101820, 904203641, 29826161, 59652323, 119304647, 238609294, 477218588, 954437176 };

	s64 err_scale = err_scale_tbl[qp][log2_size - 1];

	const int q_bits = 14 + tr_shift;



	/* ===== quantization ===== */
	rdoq_32_32_hardware_s(src_coef, qp, q_bits, dst_tmp, model_cnt->last1, model_cnt->last2, model_cnt->run_rdoq, model_cnt->level,
		rdoq_array->rdoq_est_cbf, rdoq_array->rdoq_est_run, rdoq_array->rdoq_est_level,
		rdoq_array->rdoq_est_last, is_intra, lambda, bit_depth,final_x,final_y);
}
#endif
void memcpy_rewrintra_2d_to_1d_32(s16* des, s16 src[32][32])
{
	int w = 1 << 5;
	int h = 1 << 5;

	int i, j;
	for (i = 0; i < h; i++) {
		for (j = 0; j < w; j++) {
			des[(i << 5) + j] = src[i][j];
		}
	}
}

#if NO_SUB_MODEL_SYN
void quant_nnz_32_lite(
		//int num_nz_rate[MAX_NUM_TB][N_C],
		//int num_nz_update[MAX_NUM_TB][N_C],
		RDOQ_MODEL* model_cnt,
		RDOQ_ARRAY* rdoq_array,
		U4 bit_depth,
		U6 qp,
		s64 lambda,
		//U1 is_intra_quant,
		//U1* is_intra_rate,
		s16 coef[32][32],
		//s16 coef_y_pb_part[SIZE_32],
		s16 coef_tmp_v_to_it[32][32],
		//s16 coef_y_pb_part_update[SIZE_32],
		int *final_x,
		int *final_y)
{
#if 0
	stage_v_copy(coef,coef_tmp_v_to_it);
#else
	// STREAM BUFFER
	U1 is_intra_read = 1;
	int num_nz_write;

	int i;
	U14 quant_scale[80] = { (16302), (15024), (13777), (12634), (11626), (10624), (9742), (8958), (8192), (7512), (6889), (6305), (5793), (5303), (4878), (4467), (4091), (3756), (3444), (3161), (2894), (2654), (2435), (2235), (2048), (1878), (1722), (1579), (1449), (1329), (1218), (1117), (1024), (939), (861), (790), (724), (664), (609), (558), (512), (470), (430), (395), (362), (332), (304), (279), (256), (235), (215), (197), (181), (166), (152), (140), (128), (116), (108), (99), (91), (83), (76), (69), (64), (59), (54), (49), (45), (41), (38), (35), (32), (30), (27), (25), (23), (21), (19), (18) };

	U14 scale = quant_scale[qp];
    U2 ch_type = 0;

    num_nz_write = rdoq_top_32_32(model_cnt, qp, lambda, is_intra_read, coef, coef_tmp_v_to_it,bit_depth, rdoq_array,final_x,final_y);

	//memcpy_rewrintra_2d_to_1d_32(coef_y_pb_part, coef_tmp_v_to_it);
	//memcpy_rewrintra_2d_to_1d_32(coef_y_pb_part_update, coef_tmp_v_to_it);
	// WRITE STREAMS
	//*is_intra_rate = is_intra_read;
	//*num_nz_itv = num_nz_write;

	//num_nz_rate[0][ch_type] = num_nz_write;
	//num_nz_update[0][ch_type] = num_nz_write;
#endif
}
#endif
void memcpy_1d_2_1d_32(s16 des[64 * 64], s16 src[32 * 32], int n, int flag)
{
	int z;
	if (flag) {
		if (n == 128)
			for (z = 0; z < 128; z++)
				des[z] = src[z];
		else if (n == 256)
			for (z = 0; z < 256; z++)
				des[z] = src[z];
		else if (n == 512)
			for (z = 0; z < 512; z++)
				des[z] = src[z];
		else if (n == 1024)
			for (z = 0; z < 1024; z++)
				des[z] = src[z];
	}
	else {
		if (n == 128)
			for (z = 0; z < 128; z++)
				des[z] = 0;
		else if (n == 256)
			for (z = 0; z < 256; z++)
				des[z] = 0;
		else if (n == 512)
			for (z = 0; z < 512; z++)
				des[z] = 0;
		else if (n == 1024)
			for (z = 0; z < 1024; z++)
				des[z] = 0;
	}
}
void memcpy_2d_2_1d_32(s16 des[64 * 64], s16 src[32][32], int w, int h)
{
	if (w == 4 && h == 32) {
		for (int i = 0; i < 32; i++)
			for (int j = 0; j < 4; j++)
				des[i * 4 + j] = src[i][j];
	}
	else if (w == 32 && h == 4) {
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 32; j++)
				des[i * 32 + j] = src[i][j];
	}
	else if (w == 8 && h == 32) {
		for (int i = 0; i < 32; i++)
			for (int j = 0; j < 8; j++)
				des[i * 8 + j] = src[i][j];
	}
	else if (w == 32 && h == 8) {
		for (int i = 0; i < 8; i++)
			for (int j = 0; j < 32; j++)
				des[i * 32 + j] = src[i][j];
	}
	else if (w == 16 && h == 32) {
		for (int i = 0; i < 32; i++)
			for (int j = 0; j < 16; j++)
				des[i * 16 + j] = src[i][j];
	}
	else if (w == 32 && h == 16) {
		for (int i = 0; i < 16; i++)
			for (int j = 0; j < 32; j++)
				des[i * 32 + j] = src[i][j];
	}
	else if (w == 32 && h == 32) {
		for (int i = 0; i < 32; i++)
			for (int j = 0; j < 32; j++)
				des[i * 32 + j] = src[i][j];
	}
}
void update_rdo_luma_32(
	pel rec_y_pb_part[32][32],
	s16 coef_y_pb_part[32][32],
	u8 mpm[4][2],
	s8 ipm[4][2],
	int num_nz_update[MAX_NUM_TB][N_C],
	s32 dist_rec[1],
	s64 flag_cost[3],
	hls::stream<s64_h>  &cost_s,
	hls::stream<s512_h> &rec_y_s,
	hls::stream<s512_h> &coef_y_s,
	hls::stream<u64_h> &mpm_s,
	hls::stream<s64_h> &ipm_s,
	hls::stream<s384_h> &num_nz_s
	)
{

	U8 cu_width = 32;
	U8 cu_height = 32;
	s64 cost = 0;
	U7 pb_w = cu_width;
	U7 pb_h = cu_height;
	s64 cost_pb_temp = 0;
	int bin_cnt_luma[1] = { 0 };
	U13 i;
	U13 j;
	s16 wxh = s16(cu_width) * s16(cu_height);

	//*cost_best_temp = MAX_S64;

	//if (is_intra || num_nz_update[0][0])
	//{
	cost = dist_rec[0];
	//}
	cost += flag_cost[0];
	cost_pb_temp += cost;
	//if (cost_pb_temp < *cost_pb_best) {
	//	*cost_pb_best = cost_pb_temp;
	//}
#if 0
	if (cost_pb_temp < *cost_best) {
		//*cost_pb_best = cost_pb_temp;
		*cost_best = cost_pb_temp;
		best_info_ptr->cu_mode = MODE_INTRA;
		best_info_ptr->affine_flag = 0;
	}

		for(int i = 0 ; i < 32 ; i ++)
		{
	#pragma HLS PIPELINE
			for(int j = 0 ; j < 32 ; j ++)
			{
				cu_data_temp_ptr->coef_y_t[j][i] = coef_y_pb_part[j][i];
				cu_data_temp_ptr->reco_y_t[i][j] = rec_y_pb_part[i][j];
			}
		}


		for (i = 0; i < 4; ++i) {
#pragma HLS UNROLL
			for (j = 0; j < 2; ++j) {
#pragma HLS UNROLL
				best_info_ptr->mpm[i][j] = mod_info_curr_ptr->mpm[i][j];
				best_info_ptr->ipm[i][j] = mod_info_curr_ptr->ipm[i][j];
			}
		}
		for (i = 0; i < 4; ++i) {
#pragma HLS UNROLL
			for (j = 0; j < 3; ++j) {
#pragma HLS UNROLL
				best_info_ptr->num_nz[i][j] = num_nz_update[i][j];
			}
		}
#else
		stage_copy(cost_pb_temp,cost_s);
		stage_copy(mpm,mpm_s);
		stage_copy(ipm,ipm_s);
		stage_copy(num_nz_update,num_nz_s);
		stage_hv_copy(rec_y_pb_part,coef_y_pb_part,rec_y_s,coef_y_s);
		//stage_v_copy(coef_y_pb_part,coef_y_s);
#endif

}

void update_rdo_luma_32_final(
		hls::stream<s64_h>  &cost_s,
		hls::stream<s512_h> &rec_y_s,
		hls::stream<s512_h> &coef_y_s,
		hls::stream<u64_h> &mpm_s,
		hls::stream<s64_h> &ipm_s,
		hls::stream<s384_h> &num_nz_s,

		pel rec_y_pb_part[32][32],
		s16 coef_y_pb_part[32][32],
		u8 mpm[4][2],
		s8 ipm[4][2],
		int num_nz_update[MAX_NUM_TB][N_C],
		s64 *cost_best
	)
{

	int o_vld = 0;


//	if (cost_pb_temp < *cost_best) {
//		//*cost_pb_best = cost_pb_temp;
//		*cost_best = cost_pb_temp;
//		best_info_ptr->cu_mode = MODE_INTRA;
//		best_info_ptr->affine_flag = 0;
//	}

		s64_h cost = cost_s.read();
		u64_h mpm_t = mpm_s.read();
		s64_h ipm_t = ipm_s.read();
		s384_h num_nz = num_nz_s.read();

    
		if(cost < * cost_best){
			*cost_best = cost;
			o_vld = 1;
		}

    if(o_vld){
      mpm[0][0] = mpm_t(7,0);
      mpm[0][1] = mpm_t(15,8);
      mpm[1][0] = mpm_t(23,16);
      mpm[1][1] = mpm_t(31,24);
      mpm[2][0] = mpm_t(39,32);
      mpm[2][1] = mpm_t(47,40);
      mpm[3][0] = mpm_t(55,48);
      mpm[3][1] = mpm_t(63,56);

      ipm[0][0] = ipm_t(7,0);
      ipm[0][1] = ipm_t(15,8);
      ipm[1][0] = ipm_t(23,16);
      ipm[1][1] = ipm_t(31,24);
      ipm[2][0] = ipm_t(39,32);
      ipm[2][1] = ipm_t(47,40);
      ipm[3][0] = ipm_t(55,48);
      ipm[3][1] = ipm_t(63,56);

	    num_nz_update[0][0] = num_nz(31,0)    ;
	    num_nz_update[0][1] = num_nz(63,32)   ;
	    num_nz_update[0][2] = num_nz(95,64)   ;
	    num_nz_update[1][0] = num_nz(127,96)  ;
	    num_nz_update[1][1] = num_nz(159,128) ;
	    num_nz_update[1][2] = num_nz(191,160) ;
	    num_nz_update[2][0] = num_nz(223,192) ;
	    num_nz_update[2][1] = num_nz(255,224) ;
	    num_nz_update[2][2] = num_nz(287,256) ;
	    num_nz_update[3][0] = num_nz(319,288) ;
	    num_nz_update[3][1] = num_nz(351,320) ;
	    num_nz_update[3][2] = num_nz(383,352) ;
    }

		//stage_v_copy(coef_y_pb_part_s,coef_y_pb_part);
		//stage_h_copy(rec_y_pb_part_s,rec_y_pb_part);
		s512_h coef_y_pb_tmp;
		s512_h rec_y_pb_tmp;
		for(int i = 0 ; i < 32 ; i ++)
		{
	#pragma HLS PIPELINE
			coef_y_pb_tmp = coef_y_s.read();
			rec_y_pb_tmp = rec_y_s.read();
			if(o_vld)
			{
				for(int j = 0 ; j < 32 ; j ++)
				{
				//printf("%d \n",tmp);
					rec_y_pb_part[i][j]  = rec_y_pb_tmp(j*16+15,j*16);
					coef_y_pb_part[j][i] = coef_y_pb_tmp(j*16+15,j*16);
				//printf("copy_h0 (%d %d) %d\n",i,j);
				}
			}
		}

		//if(cost_s.empty() && rec_y_s.empty() && coef_y_s.empty() && mpm_s.empty() && ipm_s.empty() && num_nz_s.empty());
		//	printf("empty\n");

//		hls::stream<s64_h>  &cost_s,
//		hls::stream<s512_h> &rec_y_s,
//		hls::stream<s512_h> &coef_y_s,
//		hls::stream<u64_h> &mpm_s,
//		hls::stream<s64_h> &ipm_s,
//		hls::stream<s384_h> &num_nz_s,
}
#if 0
void update_rdo_luma_32(
	U1 valid_flag,
	ENC_CU_DATA_ARRAY* cu_data_temp_ptr,
	MD_KERNEL_MODE_32* mod_info_curr_ptr,
	u8 mode_type,
	s64 dist_pred[1],
	s64* cost_best_temp,
	MD_COM_MODE_BEST* best_info_ptr,
	s64* cost_best,
	u8 curr_mvr,
	pel rec_y_pb_part[32][32],
	pel pred_cache[32][32],
	s16 coef_y_pb_part[32*32],
	s32* dist,
	U1 is_intra,
	s64* cost_pb_best,
	s64 flag_cost[3],
	int num_nz_update[MAX_NUM_TB][N_C],
	MD_KERNEL_INPUT_32* md_kernel_input)
{
#if HLS_WRAP
	for(int i = 0 ; i < 32 ; i ++)
	{
#pragma HLS PIPELINE
		for(int j = 0 ; j < 32 ; j ++)
		{
			cu_data_temp_ptr->coef_y_t[i][j] = coef_y_pb_part[i][j];
			cu_data_temp_ptr->reco_y_t[i][j] = rec_y_pb_part[i][j];
		}
	}
#else
#pragma HLS ARRAY_PARTITION variable=coef_y_pb_part complete dim=1
#pragma HLS ARRAY_PARTITION variable=rec_y_pb_part complete dim=2
#pragma HLS ARRAY_PARTITION variable=cu_data_temp_ptr->coef_y_t complete dim=1
#pragma HLS ARRAY_PARTITION variable=cu_data_temp_ptr->reco_y_t complete dim=2

#pragma HLS ARRAY_PARTITION variable=best_info_ptr->mpm complete dim=2
#pragma HLS ARRAY_PARTITION variable=best_info_ptr->ipm complete dim=2
#pragma HLS ARRAY_PARTITION variable=best_info_ptr->num_nz complete dim=2
#pragma HLS ARRAY_PARTITION variable=mod_info_curr_ptr->mpm complete dim=2
#pragma HLS ARRAY_PARTITION variable=mod_info_curr_ptr->ipm complete dim=2
#pragma HLS ARRAY_PARTITION variable=num_nz_update dim=2
	U8 cu_width = 32;
	U8 cu_height = 32;
	s64 cost = 0;
	U7 pb_w = cu_width;
	U7 pb_h = cu_height;
	s64 cost_pb_temp = 0;
	int bin_cnt_luma[1] = { 0 };
	U13 i;
	U13 j;
	s16 wxh = s16(cu_width) * s16(cu_height);

	*cost_best_temp = MAX_S64;

	if (is_intra || num_nz_update[0][0])
	{
		 cost = *dist;
	}
	cost = cost + flag_cost[0];
	cost_pb_temp = cost_pb_temp + cost;
	//if (cost_pb_temp < *cost_pb_best) {
	//	*cost_pb_best = cost_pb_temp;
	//}
	if (cost_pb_temp < *cost_best) {
		//*cost_pb_best = cost_pb_temp;
		*cost_best = cost_pb_temp;
		best_info_ptr->cu_mode = MODE_INTRA;
		best_info_ptr->affine_flag = 0;
#ifdef __SYNTHESIS__
		//md_kernel_32_hls::stage_v_copy(coef_y_pb_part,cu_data_temp_ptr->coef_y_t);
		//md_kernel_32_hls::stage_h_copy(rec_y_pb_part,cu_data_temp_ptr->reco_y_t);



			for(int i = 0 ; i < 32 ; i ++)
			{
		#pragma HLS PIPELINE
				for(int j = 0 ; j < 32 ; j ++)
				{
					cu_data_temp_ptr->coef_y_t[j][i] = coef_y_pb_part[j][i];
					cu_data_temp_ptr->reco_y_t[i][j] = rec_y_pb_part[i][j];
				}
			}

#else
    memcpy_rewrintra(cu_data_temp_ptr->coef_y, coef_y_pb_part, wxh);
    memcpy_2d_2_1d_32(cu_data_temp_ptr->reco_y, rec_y_pb_part, cu_width, cu_height);
#endif

		for (i = 0; i < 4; ++i) {
#pragma HLS PIPELINE
			for (j = 0; j < 2; ++j) {
				best_info_ptr->mpm[i][j] = mod_info_curr_ptr->mpm[i][j];
				best_info_ptr->ipm[i][j] = mod_info_curr_ptr->ipm[i][j];
			}
			for (j = 0; j < 3; ++j) {
				best_info_ptr->num_nz[i][j] = num_nz_update[i][j];
			}
		}
	}
#endif
}
#endif

void Ipred_vert_32(
  pel src_up[32*2+3], 
  hls::stream<s512_h> &orgY_s,
  hls::stream<s512_h> &pred_cache_s,
  hls::stream<s512_h> &coef_tmp_s)
{
  ap_int<512> orgY_32;
  ap_int<512> pred_cache_32;
  ap_int<512> coef_tmp_32;

  for (int i = 0; i < 32; i++) {
#pragma HLS PIPELINE
    orgY_32 = orgY_s.read();
	for (int j = 0; j < 32; j++) {
		pred_cache_32(j*16+15,j*16) = src_up[j+3];
		coef_tmp_32(j*16+15,j*16) = orgY_32(j*16+15,j*16) - src_up[j+3];
	}
    
    pred_cache_s.write(pred_cache_32); 
    coef_tmp_s.write(coef_tmp_32);
	}
}

void Ipred_hor_32(
  pel src_le[32*2+3], 
  hls::stream<s512_h> &orgY_s,
  hls::stream<s512_h> &pred_cache_s,
  hls::stream<s512_h> &coef_tmp_s)
{

  ap_int<512> orgY_32;
  ap_int<512> pred_cache_32;
  ap_int<512> coef_tmp_32;

	for (int i = 0; i < 32; i++) {
#pragma HLS PIPELINE
		orgY_32 = orgY_s.read();
		for (int j = 0; j < 32; j++) {
			pred_cache_32(j*16+15,j*16) = src_le[i+3];
			coef_tmp_32(j*16+15,j*16) =  orgY_32(j*16+15,j*16) - src_le[i+3];
		}
		pred_cache_s.write(pred_cache_32);
		coef_tmp_s.write(coef_tmp_32);
	}
}

void Ipred_dc_32(
  pel src_le[32*2+3], 
  pel src_up[32*2+3],
  U4 bit_depth,
  u16 avail_cu, 
  hls::stream<s512_h> &orgY_s,
  hls::stream<s512_h> &pred_cache_s,
  hls::stream<s512_h> &coef_tmp_s)
{

  ap_int<512> orgY_32;
  ap_int<512> pred_cache_32;
  ap_int<512> coef_tmp_32;

  U8 w = 32;
  U8 h = 32; 
	int dc = 0;
	//	int wh;
	int i;
	int j;
	int offset_s1 = 0;
	int s_src2 = w;
	int s_diff = w;
if (((avail_cu) & 1 << 1) == 1 << 1) {
		for (i = 0; i < 32; i++) {
#pragma HLS UNROLL
				dc += (src_le[i+3]);
		}
		if (((avail_cu) & 1 << 0) == 1 << 0) {
			for (j = 0; j < 32; j++) {
#pragma HLS UNROLL
					dc += (src_up[j+3]);
			}
			if (w == h)
				dc = (dc + ((64) >> 1)) * (4096 / (64)) >> 12;
			if ((w + h) == 48)
				dc = (dc + ((48) >> 1)) * (4096 / (48)) >> 12;
			if ((w + h) == 40)
				dc = (dc + ((40) >> 1)) * (4096 / (40)) >> 12;
		}
		else {
			dc = (dc + (h >> 1)) >> (com_tbl_log2[h]);
		}
	}
	else if (((avail_cu) & 1 << 0) == 1 << 0) {
		for (j = 0; j < 32; j++) {
#pragma HLS UNROLL
				dc += (src_up[j+3]);
		}
		dc = (dc + (w >> 1)) >> (com_tbl_log2[w]);
	}
	else {
		dc = 1 << (bit_depth - 1);
	}

	for (i = 0; i < 32; i++) {
#pragma HLS PIPELINE
		orgY_32 = orgY_s.read();
		for (j = 0; j < 32; j++) {
			pred_cache_32(j*16+15,j*16) = dc;
			coef_tmp_32(j*16+15,j*16) =  orgY_32(j*16+15,j*16) - dc;
		}
		pred_cache_s.write(pred_cache_32);
		coef_tmp_s.write(coef_tmp_32);
	}

}
void Ipred_plane_32(
  pel src_le[32*2+3], 
  pel src_up[32*2+3],
  U4 bit_depth, 
  hls::stream<s512_h> &orgY_s,
  hls::stream<s512_h> &pred_cache_s,
  hls::stream<s512_h> &coef_tmp_s)
{
  U8 w = 32;
  U8 h = 32;

  ap_int<512> orgY_32;
  ap_int<512> pred_cache_32;
  ap_int<512> coef_tmp_32;

	int coef_h = 0;
	int coef_v = 0;
	int a;
	int b;
	int c;
	int x;
	int y;
	int w2 = w >> 1;
	int h2 = h >> 1;
	int ib_mult[5] = { (13), (17), (5), (11), (23) };
	int ib_shift[5] = { (7), (10), (11), (15), (19) };
	int idx_w = (com_tbl_log2[w]) - 2;
	int idx_h = (com_tbl_log2[h]) - 2;
	int im_h;
	int is_h;
	int im_v;
	int is_v;
	int temp;

	int offset_s1 = 0;
	int offset_s2 = 0;
	int offset_diff = 0;
	int s_src2 = w;
	int s_diff = w;
	int pred_cache;

	im_h = ib_mult[idx_w];
	is_h = ib_shift[idx_w];
	im_v = ib_mult[idx_h];
	is_v = ib_shift[idx_h];

	for (x = 1; x < 17; x++) {
#pragma HLS UNROLL
		if (x < w2 + 1){
			coef_h += x * ((src_up[w2 + x+2]) - (src_up[w2 - x+2]));
		}
	}

	for (y = 1; y < 17; y++) {
#pragma HLS UNROLL
		if (y < h2 + 1){
			coef_v += y * ((src_le[h2 + y+2]) - (src_le[h2 - y+2]));
		}
	}
	a = (src_le[h+2] + src_up[w+2]) << 4;
	b = ((coef_h << 5) * im_h + (1 << (is_h - 1))) >> is_h;
	c = ((coef_v << 5) * im_v + (1 << (is_v - 1))) >> is_v;
	temp = a - (h2 - 1) * c - (w2 - 1) * b + 16;

	for (int i = 0; i < 32; i++) {
#pragma HLS PIPELINE
		orgY_32 = orgY_s.read();
		for (int j = 0; j < 32; j++) {
				pred_cache = ((temp +i*c + j*b) >> 5);
				pred_cache_32(j*16+15,j*16) = (((0 >(((1 << bit_depth) - 1 < (pred_cache) ? (1 << bit_depth) - 1 : (pred_cache))) ? 0 : (((1 << bit_depth) - 1 < (pred_cache) ? (1 << bit_depth) - 1 : (pred_cache))))));
				coef_tmp_32(j*16+15,j*16) = orgY_32(j*16+15,j*16) - pred_cache_32(j*16+15,j*16);
		}
		pred_cache_s.write(pred_cache_32);
		coef_tmp_s.write(coef_tmp_32);
	}

}
void Ipred_bi_32(
  pel* src_le, 
  pel* src_up, 
  U4 bit_depth, 
  hls::stream<s512_h> &orgY_s,
  hls::stream<s512_h> &pred_cache_s,
  hls::stream<s512_h> &coef_tmp_s)
{
  U8 w = 32 ; 
  U8 h = 32 ; 
  ap_int<512> orgY_32;
  ap_int<512> pred_cache_32;
  ap_int<512> coef_tmp_32;
   const s8 com_tbl_log2[257] = {
  	/* 0, 1 */
  	-1, -1,
  	/* 2, 3 */
  	1, -1,
  	/* 4 ~ 7 */
  	2, -1, -1, -1,
  	/* 8 ~ 15 */
  	3, -1, -1, -1, -1, -1, -1, -1,
  	/* 16 ~ 31 */
  	4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  	/* 31 ~ 63 */
  	5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  	/* 64 ~ 127 */
  	6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  	/* 128 ~ 255 */
  	7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  	/* 256 */
  	8
  };
#pragma HLS ARRAY_PARTITION variable=com_tbl_log2 complete dim=0

	int x;
	int y;
	int ishift_x = com_tbl_log2[w];
	int ishift_y = com_tbl_log2[h];
	int ishift = ishift_x < ishift_y ? ishift_x : ishift_y;
	int ishift_xy = ishift_x + ishift_y + 1;
	int offset = 1 << (ishift_x + ishift_y);
	int a;
	int b;
	int c;
	int wt;
	int wxy,wxya,wxyb,wxyc;
	int tmp;
	int predx,predxa,predxb,predxc;
	int ref_up[128 >> 1];
#pragma HLS ARRAY_PARTITION variable=ref_up complete dim=0
	int ref_le[128 >> 1];
#pragma HLS ARRAY_PARTITION variable=ref_le complete dim=0
	int up[128 >> 1];
#pragma HLS ARRAY_PARTITION variable=up complete dim=0
	int le[128 >> 1];
#pragma HLS ARRAY_PARTITION variable=le complete dim=0
	int wy[128 >> 1];
#pragma HLS ARRAY_PARTITION variable=wy complete dim=0
	int wc;
	int tbl_wc[6] = { (-1), (21), (13), (7), (4), (2) };
#pragma HLS ARRAY_PARTITION variable=tbl_wc complete dim=0
	int offset_s1 = 0;
	int offset_s2 = 0;
	int offset_diff = 0;
	int s_src2 = w;
	int s_diff = w;

	wc = (ishift_x > ishift_y ? ishift_x - ishift_y : ishift_y - ishift_x);
	wc = tbl_wc[wc];
	a = (src_up[w - 1 + 3]);
	b = (src_le[h - 1 + 3]);
	c = (w == h ? (a + b + 1) >> 1 : (((a << ishift_x) + (b << ishift_y)) * wc + (1 << (ishift + 5))) >> (ishift + 6));
	wt = (c << 1) - a - b;
	for (x = 0; x < 32; x++) {
#pragma HLS UNROLL
		if (x < w)
		{
			up[x] = b - src_up[x + 3];
			ref_up[x] = src_up[x + 3];
			ref_up[x] <<= ishift_y;
		}
	}
	tmp = 0;
	for (y = 0; y < 32; y++) {
#pragma HLS UNROLL
		if (y < h)
		{
			le[y] = a - src_le[y+3];
			ref_le[y] = src_le[y+3];
			ref_le[y] <<= ishift_x;
			wy[y] = wt * y;
		}
	}
	int i, j; int temp,tempa,tempb,tempc;
	for (i = 0; i < 32; i++) {
#pragma HLS PIPELINE
		orgY_32 = orgY_s.read();
		for (j = 0; j < 32; j++) {
				predx = ref_le[i] + (j + 1) * le[i];
				ref_up[j] += up[j];
				wxy = j * wy[i];
				temp = ((predx << ishift_y) + (ref_up[j] << ishift_x) + wxy + offset) >> ishift_xy;
				pred_cache_32(j*16+15,j*16) = (((0 > (((1 << bit_depth) - 1 < (temp) ? (1 << bit_depth) - 1 : (temp))) ? 0 : (((1 << bit_depth) - 1 < (temp) ? (1 << bit_depth) - 1 : (temp))))));
				coef_tmp_32(j*16+15,j*16)  = orgY_32(j*16+15,j*16) - pred_cache_32(j*16+15,j*16);
		}
		pred_cache_s.write(pred_cache_32);
		coef_tmp_s.write(coef_tmp_32);
	}

}
void ipred_ang1_32( 
  pel src_le[32*2+3], 
  pel src_up[32*2+3], 
  int ipm, 
  hls::stream<s512_h> &orgY_s,
  hls::stream<s512_h> &pred_cache_s,
  hls::stream<s512_h> &coef_tmp_s)
{
      U8 w = 32;
      U8 h = 32;
      ap_int<512> orgY_32;
      ap_int<512> pred_cache_32;
      ap_int<512> coef_tmp_32;
static const int ipred_dxdy[9][2] = { { (2816), (372) }, { (2048), (512) }, { (1408), (744) }, { (1024), (1024) }, { (744), (1408) }, { (512), (2048) }, { (372), (2816) }, { (256), (4096) }, { (128), (8192) } };
#pragma HLS ARRAY_PARTITION variable=ipred_dxdy complete dim=0
int i, j, t_dxa,t_dxb,t_dxc,t_dxd, offseta,offsetb,offsetc,offsetd;
			int pa, pna, pn_n1a, pn_p2a, pos_maxa;
			int xa = 2147483647;

			for (i = 0; i < 32; i++)
			{
#pragma HLS PIPELINE
				orgY_32 = orgY_s.read();
				for (j = 0; j < 32; j++)
				{
						/* case x-line */
						t_dxa = (i + 1) * ipred_dxdy[ipm - 3][0] >> 10;
						offseta = ((i + 1) * ipred_dxdy[ipm - 3][0] >> 5) - (t_dxa << 5);
						xa = j + t_dxa;

						pn_n1a = xa - 1;
						pa = xa;
						pna = xa + 1;
						pn_p2a = xa + 2;
						pos_maxa = w * 2 - 1;

						pn_n1a = (pn_n1a < pos_maxa ? pn_n1a : pos_maxa);
						pa = (pa < pos_maxa ? pa : pos_maxa);
						pna = (pna < pos_maxa ? pna : pos_maxa);
						pn_p2a = (pn_p2a < pos_maxa ? pn_p2a : pos_maxa);

						pred_cache_32(j*16+15,j*16) = ((src_up[pn_n1a + 3]) * (32 - offseta) + (src_up[pa + 3]) * (64 - offseta) + (src_up[pna + 3]) * (32 + offseta) + (src_up[pn_p2a + 3]) * offseta + 64) >> 7;
						coef_tmp_32(j*16+15,j*16)  =  orgY_32(j*16+15,j*16) - pred_cache_32(j*16+15,j*16);
				}
				pred_cache_s.write(pred_cache_32);
				coef_tmp_s.write(coef_tmp_32);
			}

}
void ipred_ang2_32(
  pel src_le[32*2+3], 
  pel src_up[32*2+3], 
  int ipm, 
  hls::stream<s512_h> &orgY_s,
  hls::stream<s512_h> &pred_cache_s,
  hls::stream<s512_h> &coef_tmp_s)
{
  U8 w = 32;
  U8 h = 32; 

  ap_int<512> orgY_32;
  ap_int<512> pred_cache_32;
  ap_int<512> coef_tmp_32;
	static const int ipred_dxdy[8][2] = { { (8192), (128) }, { (4096), (256) }, { (2816), (372) }, { (2048), (512) }, { (1408), (744) }, { (1024), (1024) }, { (744), (1408) }, { (512), (2048) } };
#pragma HLS ARRAY_PARTITION variable=ipred_dxdy complete dim=0
	int i, j, t_dya, offseta;
	int pa, pna, pn_n1a, pn_p2a, pos_maxa;
	int ya = 2147483647;

	for (i = 0; i < 32; i++)
	{
#pragma HLS PIPELINE
		orgY_32 = orgY_s.read();
		for (j = 0; j < 32; j++)
		{
				t_dya = (j + 1) * ipred_dxdy[ipm - 25][1] >> 10;
				offseta = ((j + 1) * ipred_dxdy[ipm - 25][1] >> 5) - (t_dya << 5);
				ya = i + t_dya;
				pn_n1a = ya - 1;
				pa = ya;
				pna = ya + 1;
				pn_p2a = ya+ 2;
				pos_maxa = h * 2 - 1;
				pn_n1a = (pn_n1a < pos_maxa ? pn_n1a : pos_maxa);
				pa = (pa < pos_maxa ? pa : pos_maxa);
				pna = (pna < pos_maxa ? pna : pos_maxa);
				pn_p2a = (pn_p2a < pos_maxa ? pn_p2a : pos_maxa);
				pred_cache_32(j*16+15,j*16) = ((src_le[pn_n1a + 3]) * (32 - offseta) + (src_le[pa + 3]) * (64 - offseta) + (src_le[pna + 3]) * (32 + offseta) + (src_le[pn_p2a + 3]) * offseta + 64) >> 7;
				coef_tmp_32(j*16+15,j*16) = orgY_32(j*16+15,j*16) - pred_cache_32(j*16+15,j*16);
		}
		pred_cache_s.write(pred_cache_32);
		coef_tmp_s.write(coef_tmp_32);
	}
}
void ipred_ang3_32(
  pel src_le[32*2+3], 
  pel src_up[32*2+3], 
  int ipm,
  hls::stream<s512_h> &orgY_s,
  hls::stream<s512_h> &pred_cache_s,
  hls::stream<s512_h> &coef_tmp_s)
{

  U8 w = 32;
  U8 h = 32; 
  ap_int<512> orgY_32;
  ap_int<512> pred_cache_32;
  ap_int<512> coef_tmp_32;
	static const int ipred_dxdy[11][2] = { { (128), (8192) }, { (256), (4096) }, { (372), (2816) }, { (512), (2048) }, { (744), (1408) }, { (1024), (1024) }, { (1408), (744) }, { (2048), (512) }, { (2816), (372) }, { (4096), (256) }, { (8192), (128) } };
#pragma HLS ARRAY_PARTITION variable=ipred_dxdy complete dim=0
	int i,j;
	int offseta, offset_xa, offset_ya, xxa, yya, t_dxa, t_dya;
	int pa, pna, pn_n1a, pn_p2a, pos_maxa;
	pel temp_pela = 0;
	for (i = 0; i < 32; i++)
	{
#pragma HLS PIPELINE
		orgY_32 = orgY_s.read();
		for (j = 0; j < 32; j++)
		{
				t_dxa = (i + 1) * ipred_dxdy[ipm - 13][0] >> 10;
				offset_xa = ((i + 1) * ipred_dxdy[ipm - 13][0] >> 5) - (t_dxa << 5);
				t_dxa = -t_dxa;
				xxa = j + t_dxa;
				t_dya = (j + 1) * ipred_dxdy[ipm - 13][1] >> 10;
				offset_ya = ((j + 1) * ipred_dxdy[ipm - 13][1] >> 5) - (t_dya << 5);
				t_dya = -t_dya;
				yya = i + t_dya;

				if (yya <= -1) {
					offseta = offset_xa;
					pn_n1a = xxa+ 1;
					pa = xxa;
					pna = xxa - 1;
					pn_p2a = xxa - 2;
					pos_maxa = w * 2 - 1;
					temp_pela = ((src_up[pn_n1a + 3]) * (32 - offseta) + (src_up[pa + 3]) * (64 - offseta) + ((
						int)src_up[pna + 3]) * (32 + offseta) + (src_up[pn_p2a + 3]) * offseta + 64) >> 7;

				}
				else {
					offseta = offset_ya;
					pn_n1a = yya + 1;
					pa = yya;
					pna = yya - 1;
					pn_p2a = yya - 2;
					pos_maxa = h * 2 - 1;
					temp_pela = ((src_le[pn_n1a + 3]) * (32 - offseta) + (src_le[pa + 3]) * (64 - offseta) + (src_le[pna + 3]) * (32 + offseta) + (src_le[pn_p2a + 3]) * offseta + 64) >> 7;

				}
				pred_cache_32(j*16+15,j*16) = temp_pela;
				coef_tmp_32(j*16+15,j*16)= orgY_32(j*16+15,j*16) - temp_pela;
		}
		pred_cache_s.write(pred_cache_32);
		coef_tmp_s.write(coef_tmp_32);
	}
}



void intra_inter_pred_32(
  MD_KERNEL_MODE_32* mod_info_nz_32, 
  hls::stream<s512_h> &orgY_s,//i
  s8 &inter_mode_idx,
  hls::stream<s512_h> &coef_tmp_s,//o
  //U1 *valid_flag,
  hls::stream<s512_h> &pred_cache_s,//o
  //U1* is_intra_quant,
  u8 intra_mode_list[5], 
  pel src_le_temp[32 * 2 + 3], 
  pel src_up_temp[32 * 2 + 3], 
  u16 avail_tb, 
  u8 rdo_list[5],
  u8 rdo_idx,
  u8 mpm[MAX_NUM_PB][2],
  U4 bit_depth)
{
//#pragma HLS ARRAY_PARTITION variable=src_le_temp complete dim=0
//#pragma HLS ARRAY_PARTITION variable=src_up_temp complete dim=0
//#pragma HLS ARRAY_PARTITION variable=mpm complete dim=0
//#pragma HLS ARRAY_PARTITION variable=mod_info_nz_32->ipm complete dim=0
//#pragma HLS ARRAY_PARTITION variable=mod_info_nz_32->mpm complete dim=0
#if 0
	U7 w = 32;
	U7 h = 32;
	  ap_int<512> orgY_32;
	  ap_int<512> pred_cache_32;
	  ap_int<512> coef_tmp_32;
	  for (int i = 0; i < 32; i++) {
	#pragma HLS PIPELINE
	    orgY_32 = orgY_s.read();
		for (int j = 0; j < 32; j++) {
			pred_cache_32(j*16+15,j*16) = src_up_temp[j+3];
			coef_tmp_32(j*16+15,j*16) = orgY_32(j*16+15,j*16) - src_up_temp[j+3];
		}

	    pred_cache_s.write(pred_cache_32);
	    coef_tmp_s.write(coef_tmp_32);
	  }

#else
	U7 w = 32;
	U7 h = 32;
	inter_mode_idx = rdo_list[rdo_idx];
	s8 intra_mode_idx;
//	MD_KERNEL_MODE_32 mode_info_write_32;
//#pragma HLS ARRAY_PARTITION variable=mode_info_write_32.ipm complete dim=0
	U1 is_intra_write;
	U1 valid_flag_write = 0;
	is_intra_write = 1;
	intra_mode_idx = intra_mode_list[inter_mode_idx - 17];
  //printf("intra_mode_idx = %d\n",intra_mode_idx);

	{
		if (intra_mode_idx == 12)
			Ipred_vert_32(src_up_temp, orgY_s, pred_cache_s, coef_tmp_s);
		else if (intra_mode_idx == 24)
			Ipred_hor_32(src_le_temp, orgY_s, pred_cache_s, coef_tmp_s);
		else if (intra_mode_idx == 0)
			Ipred_dc_32(src_le_temp, src_up_temp, bit_depth, avail_tb, orgY_s, pred_cache_s, coef_tmp_s);
		else if (intra_mode_idx == 1)
			Ipred_plane_32(src_le_temp, src_up_temp, bit_depth, orgY_s, pred_cache_s, coef_tmp_s);
		else if (intra_mode_idx == 2)
			Ipred_bi_32(src_le_temp, src_up_temp, bit_depth, orgY_s, pred_cache_s, coef_tmp_s);
		else if (intra_mode_idx >= 3 && intra_mode_idx <= 11)
			ipred_ang1_32(src_le_temp, src_up_temp, intra_mode_idx, orgY_s, pred_cache_s, coef_tmp_s);
		else if (intra_mode_idx >= 25 && intra_mode_idx <= 32)
			ipred_ang2_32(src_le_temp, src_up_temp, intra_mode_idx, orgY_s, pred_cache_s, coef_tmp_s);
		else
			ipred_ang3_32(src_le_temp, src_up_temp, intra_mode_idx, orgY_s, pred_cache_s, coef_tmp_s);


	}
	//mode_info_write_32.cu_mode = 0;
	//mode_info_write_32.
	//mode_info_write_32.
	//valid_flag_write = 1;
	s8 ipm[MAX_NUM_PB][2];
	ipm[0][0] = intra_mode_idx;
	ipm[0][1] = -1;
	//*valid_flag = valid_flag_write;
	//*is_intra_quant = is_intra_write;

	//mod_info_nz_32->cu_mode = 0;

	for (int i = 0; i < MAX_NUM_PB; ++i)
	{
#pragma HLS UNROLL
		for (int j = 0; j < 2; ++j)
		{
#pragma HLS UNROLL
			mod_info_nz_32->ipm[i][j] = ipm[i][j];
			mod_info_nz_32->mpm[i][j] = mpm[i][j];
		}
	}
	//mod_info_nz_32->tb_part = mode_info_write_32.tb_part;
	//mod_info_nz_32->pb_part = 0;
#endif
}


void dct_row_32_lite(U8 cu_width, U8 cu_height, s16 src[32][32], s16 dst[32][32], U4 shift)
{
	U7 size = 32;
	U8 size_b = size - 1;

	int j, k;
	int E[32][16];
#pragma HLS ARRAY_PARTITION variable=E complete dim=0
	int O[32][16];
#pragma HLS ARRAY_PARTITION variable=O complete dim=0
	int EE[32][8];
#pragma HLS ARRAY_PARTITION variable=EE complete dim=0
	int EO[32][8];
#pragma HLS ARRAY_PARTITION variable=EO complete dim=0
	int EEE[32][4];
#pragma HLS ARRAY_PARTITION variable=EEE complete dim=0
	int EEO[32][4];
#pragma HLS ARRAY_PARTITION variable=EEO complete dim=0
	int EEEE[32][2];
#pragma HLS ARRAY_PARTITION variable=EEEE complete dim=0
	int EEEO[32][2];
#pragma HLS ARRAY_PARTITION variable=EEEO complete dim=0
	int tmp[32];
#pragma HLS ARRAY_PARTITION variable=tmp complete dim=0
	int add;

	add = 1 << (shift - 1);

	for (j = 0; j < cu_height; j++)
	{
#pragma HLS PIPELINE
		/* E and O*/
		for (k = 0; k < 16; k++)
		{
			E[j][k] = src[j][k] + src[j][31 - k];
			O[j][k] = src[j][k] - src[j][31 - k];
		}
		/* EE and EO */
		for (k = 0; k < 8; k++)
		{
			EE[j][k] = E[j][k] + E[j][15 - k];
			EO[j][k] = E[j][k] - E[j][15 - k];
		}

		/* EEE and EEO */
		for (k = 0; k < 4; k++)
		{
			EEE[j][k] = EE[j][k] + EE[j][7 - k];
			EEO[j][k] = EE[j][k] - EE[j][7 - k];
		}

		EEEE[j][0] = EEE[j][0] + EEE[j][3];
		EEEO[j][0] = EEE[j][0] - EEE[j][3];
		EEEE[j][1] = EEE[j][1] + EEE[j][2];
		EEEO[j][1] = EEE[j][1] - EEE[j][2];

		tmp[0] = ((32 * EEEE[j][0] + 32 * EEEE[j][1] + add) >> shift);
		tmp[1] = ((32 * EEEE[j][0] - 32 * EEEE[j][1] + add) >> shift);
		tmp[2] = ((42 * EEEO[j][0] + 17 * EEEO[j][1] + add) >> shift);
		tmp[3] = ((17 * EEEO[j][0] - 42 * EEEO[j][1] + add) >> shift);
		tmp[4] = (((44) * EEO[j][0] + (38) * EEO[j][1] + (25) * EEO[j][2] + (9) * EEO[j][3] + add) >> shift);
		tmp[5] = (((38) * EEO[j][0] + (-9) * EEO[j][1] + (-44) * EEO[j][2] + (-25) * EEO[j][3] + add) >> shift);
		tmp[6] = (((25) * EEO[j][0] + (-44) * EEO[j][1] + (9) * EEO[j][2] + (38) * EEO[j][3] + add) >> shift);
		tmp[7] = (((9) * EEO[j][0] + (-25) * EEO[j][1] + (38) * EEO[j][2] + (-44) * EEO[j][3] + add) >> shift);
		tmp[8] = (((45) * EO[j][0] + (43) * EO[j][1] + (40) * EO[j][2] + (35) * EO[j][3] + (29) * EO[j][4] + (21) * EO[j][5] + (13) * EO[j][6] + (4) * EO[j][7] + add) >> shift);
		tmp[9] = (((43) * EO[j][0] + (29) * EO[j][1] + (4) * EO[j][2] + (-21) * EO[j][3] + (-40) * EO[j][4] + (-45) * EO[j][5] + (-35) * EO[j][6] + (-13) * EO[j][7] + add) >> shift);
		tmp[10] = (((40) * EO[j][0] + (4) * EO[j][1] + (-35) * EO[j][2] + (-43) * EO[j][3] + (-13) * EO[j][4] + (29) * EO[j][5] + (45) * EO[j][6] + (21) * EO[j][7] + add) >> shift);
		tmp[11] = (((35) * EO[j][0] + (-21) * EO[j][1] + (-43) * EO[j][2] + (4) * EO[j][3] + (45) * EO[j][4] + (13) * EO[j][5] + (-40) * EO[j][6] + (-29) * EO[j][7] + add) >> shift);
		tmp[12] = (((29) * EO[j][0] + (-40) * EO[j][1] + (-13) * EO[j][2] + (45) * EO[j][3] + (-4) * EO[j][4] + (-43) * EO[j][5] + (21) * EO[j][6] + (35) * EO[j][7] + add) >> shift);
		tmp[13] = (((21) * EO[j][0] + (-45) * EO[j][1] + (29) * EO[j][2] + (13) * EO[j][3] + (-43) * EO[j][4] + (35) * EO[j][5] + (4) * EO[j][6] + (-40) * EO[j][7] + add) >> shift);
		tmp[14] = (((13) * EO[j][0] + (-35) * EO[j][1] + (45) * EO[j][2] + (-40) * EO[j][3] + (21) * EO[j][4] + (4) * EO[j][5] + (-29) * EO[j][6] + (43) * EO[j][7] + add) >> shift);
		tmp[15] = (((4) * EO[j][0] + (-13) * EO[j][1] + (21) * EO[j][2] + (-29) * EO[j][3] + (35) * EO[j][4] + (-40) * EO[j][5] + (43) * EO[j][6] + (-45) * EO[j][7] + add) >> shift);
		tmp[16] = (((45) * O[j][0] + (45) * O[j][1] + (44) * O[j][2] + (43) * O[j][3] + (41) * O[j][4] + (39) * O[j][5] + (36) * O[j][6] + (34) * O[j][7] + (30) * O[j][8] + (27) * O[j][9] + (23) * O[j][10] + (19) * O[j][11] + (15) * O[j][12] + (11) * O[j][13] + (7) * O[j][14] + (2) * O[j][15] + add) >> shift);
		tmp[17] = (((45) * O[j][0] + (41) * O[j][1] + (34) * O[j][2] + (23) * O[j][3] + (11) * O[j][4] + (-2) * O[j][5] + (-15) * O[j][6] + (-27) * O[j][7] + (-36) * O[j][8] + (-43) * O[j][9] + (-45) * O[j][10] + (-44) * O[j][11] + (-39) * O[j][12] + (-30) * O[j][13] + (-19) * O[j][14] + (-7) * O[j][15] + add) >> shift);
		tmp[18] = (((44) * O[j][0] + (34) * O[j][1] + (15) * O[j][2] + (-7) * O[j][3] + (-27) * O[j][4] + (-41) * O[j][5] + (-45) * O[j][6] + (-39) * O[j][7] + (-23) * O[j][8] + (-2) * O[j][9] + (19) * O[j][10] + (36) * O[j][11] + (45) * O[j][12] + (43) * O[j][13] + (30) * O[j][14] + (11) * O[j][15] + add) >> shift);
		tmp[19] = (((43) * O[j][0] + (23) * O[j][1] + (-7) * O[j][2] + (-34) * O[j][3] + (-45) * O[j][4] + (-36) * O[j][5] + (-11) * O[j][6] + (19) * O[j][7] + (41) * O[j][8] + (44) * O[j][9] + (27) * O[j][10] + (-2) * O[j][11] + (-30) * O[j][12] + (-45) * O[j][13] + (-39) * O[j][14] + (-15) * O[j][15] + add) >> shift);
		tmp[20] = (((41) * O[j][0] + (11) * O[j][1] + (-27) * O[j][2] + (-45) * O[j][3] + (-30) * O[j][4] + (7) * O[j][5] + (39) * O[j][6] + (43) * O[j][7] + (15) * O[j][8] + (-23) * O[j][9] + (-45) * O[j][10] + (-34) * O[j][11] + (2) * O[j][12] + (36) * O[j][13] + (44) * O[j][14] + (19) * O[j][15] + add) >> shift);
		tmp[21] = (((39) * O[j][0] + (-2) * O[j][1] + (-41) * O[j][2] + (-36) * O[j][3] + (7) * O[j][4] + (43) * O[j][5] + (34) * O[j][6] + (-11) * O[j][7] + (-44) * O[j][8] + (-30) * O[j][9] + (15) * O[j][10] + (45) * O[j][11] + (27) * O[j][12] + (-19) * O[j][13] + (-45) * O[j][14] + (-23) * O[j][15] + add) >> shift);
		tmp[22] = (((36) * O[j][0] + (-15) * O[j][1] + (-45) * O[j][2] + (-11) * O[j][3] + (39) * O[j][4] + (34) * O[j][5] + (-19) * O[j][6] + (-45) * O[j][7] + (-7) * O[j][8] + (41) * O[j][9] + (30) * O[j][10] + (-23) * O[j][11] + (-44) * O[j][12] + (-2) * O[j][13] + (43) * O[j][14] + (27) * O[j][15] + add) >> shift);
		tmp[23] = (((34) * O[j][0] + (-27) * O[j][1] + (-39) * O[j][2] + (19) * O[j][3] + (43) * O[j][4] + (-11) * O[j][5] + (-45) * O[j][6] + (2) * O[j][7] + (45) * O[j][8] + (7) * O[j][9] + (-44) * O[j][10] + (-15) * O[j][11] + (41) * O[j][12] + (23) * O[j][13] + (-36) * O[j][14] + (-30) * O[j][15] + add) >> shift);
		tmp[24] = (((30) * O[j][0] + (-36) * O[j][1] + (-23) * O[j][2] + (41) * O[j][3] + (15) * O[j][4] + (-44) * O[j][5] + (-7) * O[j][6] + (45) * O[j][7] + (-2) * O[j][8] + (-45) * O[j][9] + (11) * O[j][10] + (43) * O[j][11] + (-19) * O[j][12] + (-39) * O[j][13] + (27) * O[j][14] + (34) * O[j][15] + add) >> shift);
		tmp[25] = (((27) * O[j][0] + (-43) * O[j][1] + (-2) * O[j][2] + (44) * O[j][3] + (-23) * O[j][4] + (-30) * O[j][5] + (41) * O[j][6] + (7) * O[j][7] + (-45) * O[j][8] + (19) * O[j][9] + (34) * O[j][10] + (-39) * O[j][11] + (-11) * O[j][12] + (45) * O[j][13] + (-15) * O[j][14] + (-36) * O[j][15] + add) >> shift);
		tmp[26] = (((23) * O[j][0] + (-45) * O[j][1] + (19) * O[j][2] + (27) * O[j][3] + (-45) * O[j][4] + (15) * O[j][5] + (30) * O[j][6] + (-44) * O[j][7] + (11) * O[j][8] + (34) * O[j][9] + (-43) * O[j][10] + (7) * O[j][11] + (36) * O[j][12] + (-41) * O[j][13] + (2) * O[j][14] + (39) * O[j][15] + add) >> shift);
		tmp[27] = (((19) * O[j][0] + (-44) * O[j][1] + (36) * O[j][2] + (-2) * O[j][3] + (-34) * O[j][4] + (45) * O[j][5] + (-23) * O[j][6] + (-15) * O[j][7] + (43) * O[j][8] + (-39) * O[j][9] + (7) * O[j][10] + (30) * O[j][11] + (-45) * O[j][12] + (27) * O[j][13] + (11) * O[j][14] + (-41) * O[j][15] + add) >> shift);
		tmp[28] = (((15) * O[j][0] + (-39) * O[j][1] + (45) * O[j][2] + (-30) * O[j][3] + (2) * O[j][4] + (27) * O[j][5] + (-44) * O[j][6] + (41) * O[j][7] + (-19) * O[j][8] + (-11) * O[j][9] + (36) * O[j][10] + (-45) * O[j][11] + (34) * O[j][12] + (-7) * O[j][13] + (-23) * O[j][14] + (43) * O[j][15] + add) >> shift);
		tmp[29] = (((11) * O[j][0] + (-30) * O[j][1] + (43) * O[j][2] + (-45) * O[j][3] + (36) * O[j][4] + (-19) * O[j][5] + (-2) * O[j][6] + (23) * O[j][7] + (-39) * O[j][8] + (45) * O[j][9] + (-41) * O[j][10] + (27) * O[j][11] + (-7) * O[j][12] + (-15) * O[j][13] + (34) * O[j][14] + (-44) * O[j][15] + add) >> shift);
		tmp[30] = (((7) * O[j][0] + (-19) * O[j][1] + (30) * O[j][2] + (-39) * O[j][3] + (44) * O[j][4] + (-45) * O[j][5] + (43) * O[j][6] + (-36) * O[j][7] + (27) * O[j][8] + (-15) * O[j][9] + (2) * O[j][10] + (11) * O[j][11] + (-23) * O[j][12] + (34) * O[j][13] + (-41) * O[j][14] + (45) * O[j][15] + add) >> shift);
		tmp[31] = (((2) * O[j][0] + (-7) * O[j][1] + (11) * O[j][2] + (-15) * O[j][3] + (19) * O[j][4] + (-23) * O[j][5] + (27) * O[j][6] + (-30) * O[j][7] + (34) * O[j][8] + (-36) * O[j][9] + (39) * O[j][10] + (-41) * O[j][11] + (43) * O[j][12] + (-44) * O[j][13] + (45) * O[j][14] + (-45) * O[j][15] + add) >> shift);

		dst[0][(j + 0) & size_b] = tmp[0];
		dst[16][(j + 16) & size_b] = tmp[1];
		dst[8][(j + 8) & size_b] = tmp[2];
		dst[24][(j + 24) & size_b] = tmp[3];
		dst[4][(j + 4) & size_b] = tmp[4];
		dst[12][(j + 12) & size_b] = tmp[5];
		dst[20][(j + 20) & size_b] = tmp[6];
		dst[28][(j + 28) & size_b] = tmp[7];
		dst[2][(j + 2) & size_b] = tmp[8];
		dst[6][(j + 6) & size_b] = tmp[9];
		dst[10][(j + 10) & size_b] = tmp[10];
		dst[14][(j + 14) & size_b] = tmp[11];
		dst[18][(j + 18) & size_b] = tmp[12];
		dst[22][(j + 22) & size_b] = tmp[13];
		dst[26][(j + 26) & size_b] = tmp[14];
		dst[30][(j + 30) & size_b] = tmp[15];
		dst[1][(j + 1) & size_b] = tmp[16];
		dst[3][(j + 3) & size_b] = tmp[17];
		dst[5][(j + 5) & size_b] = tmp[18];
		dst[7][(j + 7) & size_b] = tmp[19];
		dst[9][(j + 9) & size_b] = tmp[20];
		dst[11][(j + 11) & size_b] = tmp[21];
		dst[13][(j + 13) & size_b] = tmp[22];
		dst[15][(j + 15) & size_b] = tmp[23];
		dst[17][(j + 17) & size_b] = tmp[24];
		dst[19][(j + 19) & size_b] = tmp[25];
		dst[21][(j + 21) & size_b] = tmp[26];
		dst[23][(j + 23) & size_b] = tmp[27];
		dst[25][(j + 25) & size_b] = tmp[28];
		dst[27][(j + 27) & size_b] = tmp[29];
		dst[29][(j + 29) & size_b] = tmp[30];
		dst[31][(j + 31) & size_b] = tmp[31];
	}
}

void dct_col_32_lite(U8 cu_width, U8 cu_height,s16 src[32][32], s16 dst[32][32], U4 shift)
{
  	U7 size = 32;
  	U8 size_b = size - 1;
  
  	int j, k;
  	int E[32][16];
#pragma HLS ARRAY_PARTITION variable=E complete dim=0
  	int O[32][16];
#pragma HLS ARRAY_PARTITION variable=O complete dim=0
  	int EE[32][8];
#pragma HLS ARRAY_PARTITION variable=EE complete dim=0
  	int EO[32][8];
#pragma HLS ARRAY_PARTITION variable=EO complete dim=0
  	int EEE[32][4];
#pragma HLS ARRAY_PARTITION variable=EEE complete dim=0
  	int EEO[32][4];
#pragma HLS ARRAY_PARTITION variable=EEO complete dim=0
  	int EEEE[32][2];
#pragma HLS ARRAY_PARTITION variable=EEEE complete dim=0
  	int EEEO[32][2];
#pragma HLS ARRAY_PARTITION variable=EEEO complete dim=0
  	int tmp[32];
#pragma HLS ARRAY_PARTITION variable=tmp complete dim=0
  	int add;

  	add = 1 << (shift - 1);
  	for (j = 0; j < cu_width; j++)
  	{
#pragma HLS PIPELINE
  		/* E and O*/
  		for (k = 0; k < 16; k++)
  		{
  			E[j][k] = src[j][(k + j) & size_b] + src[j][(31 - k + j) & size_b];
  			O[j][k] = src[j][(k + j) & size_b] - src[j][(31 - k + j) & size_b];
  		}
  		/* EE and EO */
  		for (k = 0; k < 8; k++)
  		{
  			EE[j][k] = E[j][k] + E[j][15 - k];
  			EO[j][k] = E[j][k] - E[j][15 - k];
  		}
  
  		/* EEE and EEO */
  		for (k = 0; k < 4; k++)
  		{
  			EEE[j][k] = EE[j][k] + EE[j][7 - k];
  			EEO[j][k] = EE[j][k] - EE[j][7 - k];
  		}
  
  
  				/* EEEE and EEEO */
  		EEEE[j][0] = EEE[j][0] + EEE[j][3];
  		EEEO[j][0] = EEE[j][0] - EEE[j][3];
  		EEEE[j][1] = EEE[j][1] + EEE[j][2];
  		EEEO[j][1] = EEE[j][1] - EEE[j][2];

  		tmp[0] = ((32 * EEEE[j][0] + 32 * EEEE[j][1] + add) >> shift);
  		tmp[1] = ((32 * EEEE[j][0] - 32 * EEEE[j][1] + add) >> shift);
  		tmp[2] = ((42 * EEEO[j][0] + 17 * EEEO[j][1] + add) >> shift);
  		tmp[3] = ((17 * EEEO[j][0] - 42 * EEEO[j][1] + add) >> shift);
  		tmp[4] = (((44) * EEO[j][0] + (38) * EEO[j][1] + (25) * EEO[j][2] + (9) * EEO[j][3] + add) >> shift);
  		tmp[5] = (((38) * EEO[j][0] + (-9) * EEO[j][1] + (-44) * EEO[j][2] + (-25) * EEO[j][3] + add) >> shift);
  		tmp[6] = (((25) * EEO[j][0] + (-44) * EEO[j][1] + (9) * EEO[j][2] + (38) * EEO[j][3] + add) >> shift);
  		tmp[7] = (((9) * EEO[j][0] + (-25) * EEO[j][1] + (38) * EEO[j][2] + (-44) * EEO[j][3] + add) >> shift);
  		tmp[8] = (((45) * EO[j][0] + (43) * EO[j][1] + (40) * EO[j][2] + (35) * EO[j][3] + (29) * EO[j][4] + (21) * EO[j][5] + (13) * EO[j][6] + (4) * EO[j][7] + add) >> shift);
  		tmp[9] = (((43) * EO[j][0] + (29) * EO[j][1] + (4) * EO[j][2] + (-21) * EO[j][3] + (-40) * EO[j][4] + (-45) * EO[j][5] + (-35) * EO[j][6] + (-13) * EO[j][7] + add) >> shift);
  		tmp[10] = (((40) * EO[j][0] + (4) * EO[j][1] + (-35) * EO[j][2] + (-43) * EO[j][3] + (-13) * EO[j][4] + (29) * EO[j][5] + (45) * EO[j][6] + (21) * EO[j][7] + add) >> shift);
  		tmp[11] = (((35) * EO[j][0] + (-21) * EO[j][1] + (-43) * EO[j][2] + (4) * EO[j][3] + (45) * EO[j][4] + (13) * EO[j][5] + (-40) * EO[j][6] + (-29) * EO[j][7] + add) >> shift);
  		tmp[12] = (((29) * EO[j][0] + (-40) * EO[j][1] + (-13) * EO[j][2] + (45) * EO[j][3] + (-4) * EO[j][4] + (-43) * EO[j][5] + (21) * EO[j][6] + (35) * EO[j][7] + add) >> shift);
  		tmp[13] = (((21) * EO[j][0] + (-45) * EO[j][1] + (29) * EO[j][2] + (13) * EO[j][3] + (-43) * EO[j][4] + (35) * EO[j][5] + (4) * EO[j][6] + (-40) * EO[j][7] + add) >> shift);
  		tmp[14] = (((13) * EO[j][0] + (-35) * EO[j][1] + (45) * EO[j][2] + (-40) * EO[j][3] + (21) * EO[j][4] + (4) * EO[j][5] + (-29) * EO[j][6] + (43) * EO[j][7] + add) >> shift);
  		tmp[15] = (((4) * EO[j][0] + (-13) * EO[j][1] + (21) * EO[j][2] + (-29) * EO[j][3] + (35) * EO[j][4] + (-40) * EO[j][5] + (43) * EO[j][6] + (-45) * EO[j][7] + add) >> shift);
  		tmp[16] = (((45) * O[j][0] + (45) * O[j][1] + (44) * O[j][2] + (43) * O[j][3] + (41) * O[j][4] + (39) * O[j][5] + (36) * O[j][6] + (34) * O[j][7] + (30) * O[j][8] + (27) * O[j][9] + (23) * O[j][10] + (19) * O[j][11] + (15) * O[j][12] + (11) * O[j][13] + (7) * O[j][14] + (2) * O[j][15] + add) >> shift);
  		tmp[17] = (((45) * O[j][0] + (41) * O[j][1] + (34) * O[j][2] + (23) * O[j][3] + (11) * O[j][4] + (-2) * O[j][5] + (-15) * O[j][6] + (-27) * O[j][7] + (-36) * O[j][8] + (-43) * O[j][9] + (-45) * O[j][10] + (-44) * O[j][11] + (-39) * O[j][12] + (-30) * O[j][13] + (-19) * O[j][14] + (-7) * O[j][15] + add) >> shift);
  		tmp[18] = (((44) * O[j][0] + (34) * O[j][1] + (15) * O[j][2] + (-7) * O[j][3] + (-27) * O[j][4] + (-41) * O[j][5] + (-45) * O[j][6] + (-39) * O[j][7] + (-23) * O[j][8] + (-2) * O[j][9] + (19) * O[j][10] + (36) * O[j][11] + (45) * O[j][12] + (43) * O[j][13] + (30) * O[j][14] + (11) * O[j][15] + add) >> shift);
  		tmp[19] = (((43) * O[j][0] + (23) * O[j][1] + (-7) * O[j][2] + (-34) * O[j][3] + (-45) * O[j][4] + (-36) * O[j][5] + (-11) * O[j][6] + (19) * O[j][7] + (41) * O[j][8] + (44) * O[j][9] + (27) * O[j][10] + (-2) * O[j][11] + (-30) * O[j][12] + (-45) * O[j][13] + (-39) * O[j][14] + (-15) * O[j][15] + add) >> shift);
  		tmp[20] = (((41) * O[j][0] + (11) * O[j][1] + (-27) * O[j][2] + (-45) * O[j][3] + (-30) * O[j][4] + (7) * O[j][5] + (39) * O[j][6] + (43) * O[j][7] + (15) * O[j][8] + (-23) * O[j][9] + (-45) * O[j][10] + (-34) * O[j][11] + (2) * O[j][12] + (36) * O[j][13] + (44) * O[j][14] + (19) * O[j][15] + add) >> shift);
  		tmp[21] = (((39) * O[j][0] + (-2) * O[j][1] + (-41) * O[j][2] + (-36) * O[j][3] + (7) * O[j][4] + (43) * O[j][5] + (34) * O[j][6] + (-11) * O[j][7] + (-44) * O[j][8] + (-30) * O[j][9] + (15) * O[j][10] + (45) * O[j][11] + (27) * O[j][12] + (-19) * O[j][13] + (-45) * O[j][14] + (-23) * O[j][15] + add) >> shift);
  		tmp[22] = (((36) * O[j][0] + (-15) * O[j][1] + (-45) * O[j][2] + (-11) * O[j][3] + (39) * O[j][4] + (34) * O[j][5] + (-19) * O[j][6] + (-45) * O[j][7] + (-7) * O[j][8] + (41) * O[j][9] + (30) * O[j][10] + (-23) * O[j][11] + (-44) * O[j][12] + (-2) * O[j][13] + (43) * O[j][14] + (27) * O[j][15] + add) >> shift);
  		tmp[23] = (((34) * O[j][0] + (-27) * O[j][1] + (-39) * O[j][2] + (19) * O[j][3] + (43) * O[j][4] + (-11) * O[j][5] + (-45) * O[j][6] + (2) * O[j][7] + (45) * O[j][8] + (7) * O[j][9] + (-44) * O[j][10] + (-15) * O[j][11] + (41) * O[j][12] + (23) * O[j][13] + (-36) * O[j][14] + (-30) * O[j][15] + add) >> shift);
  		tmp[24] = (((30) * O[j][0] + (-36) * O[j][1] + (-23) * O[j][2] + (41) * O[j][3] + (15) * O[j][4] + (-44) * O[j][5] + (-7) * O[j][6] + (45) * O[j][7] + (-2) * O[j][8] + (-45) * O[j][9] + (11) * O[j][10] + (43) * O[j][11] + (-19) * O[j][12] + (-39) * O[j][13] + (27) * O[j][14] + (34) * O[j][15] + add) >> shift);
  		tmp[25] = (((27) * O[j][0] + (-43) * O[j][1] + (-2) * O[j][2] + (44) * O[j][3] + (-23) * O[j][4] + (-30) * O[j][5] + (41) * O[j][6] + (7) * O[j][7] + (-45) * O[j][8] + (19) * O[j][9] + (34) * O[j][10] + (-39) * O[j][11] + (-11) * O[j][12] + (45) * O[j][13] + (-15) * O[j][14] + (-36) * O[j][15] + add) >> shift);
  		tmp[26] = (((23) * O[j][0] + (-45) * O[j][1] + (19) * O[j][2] + (27) * O[j][3] + (-45) * O[j][4] + (15) * O[j][5] + (30) * O[j][6] + (-44) * O[j][7] + (11) * O[j][8] + (34) * O[j][9] + (-43) * O[j][10] + (7) * O[j][11] + (36) * O[j][12] + (-41) * O[j][13] + (2) * O[j][14] + (39) * O[j][15] + add) >> shift);
  		tmp[27] = (((19) * O[j][0] + (-44) * O[j][1] + (36) * O[j][2] + (-2) * O[j][3] + (-34) * O[j][4] + (45) * O[j][5] + (-23) * O[j][6] + (-15) * O[j][7] + (43) * O[j][8] + (-39) * O[j][9] + (7) * O[j][10] + (30) * O[j][11] + (-45) * O[j][12] + (27) * O[j][13] + (11) * O[j][14] + (-41) * O[j][15] + add) >> shift);
  		tmp[28] = (((15) * O[j][0] + (-39) * O[j][1] + (45) * O[j][2] + (-30) * O[j][3] + (2) * O[j][4] + (27) * O[j][5] + (-44) * O[j][6] + (41) * O[j][7] + (-19) * O[j][8] + (-11) * O[j][9] + (36) * O[j][10] + (-45) * O[j][11] + (34) * O[j][12] + (-7) * O[j][13] + (-23) * O[j][14] + (43) * O[j][15] + add) >> shift);
  		tmp[29] = (((11) * O[j][0] + (-30) * O[j][1] + (43) * O[j][2] + (-45) * O[j][3] + (36) * O[j][4] + (-19) * O[j][5] + (-2) * O[j][6] + (23) * O[j][7] + (-39) * O[j][8] + (45) * O[j][9] + (-41) * O[j][10] + (27) * O[j][11] + (-7) * O[j][12] + (-15) * O[j][13] + (34) * O[j][14] + (-44) * O[j][15] + add) >> shift);
  		tmp[30] = (((7) * O[j][0] + (-19) * O[j][1] + (30) * O[j][2] + (-39) * O[j][3] + (44) * O[j][4] + (-45) * O[j][5] + (43) * O[j][6] + (-36) * O[j][7] + (27) * O[j][8] + (-15) * O[j][9] + (2) * O[j][10] + (11) * O[j][11] + (-23) * O[j][12] + (34) * O[j][13] + (-41) * O[j][14] + (45) * O[j][15] + add) >> shift);
  		tmp[31] = (((2) * O[j][0] + (-7) * O[j][1] + (11) * O[j][2] + (-15) * O[j][3] + (19) * O[j][4] + (-23) * O[j][5] + (27) * O[j][6] + (-30) * O[j][7] + (34) * O[j][8] + (-36) * O[j][9] + (39) * O[j][10] + (-41) * O[j][11] + (43) * O[j][12] + (-44) * O[j][13] + (45) * O[j][14] + (-45) * O[j][15] + add) >> shift);
  

		dst[0][j] = tmp[0];
		dst[16][j] = tmp[1];
		dst[8][j] = tmp[2];
		dst[24][j] = tmp[3];
		dst[4][j] = tmp[4];
		dst[12][j] = tmp[5];
		dst[20][j] = tmp[6];
		dst[28][j] = tmp[7];
		dst[2][j] = tmp[8];
		dst[6][j] = tmp[9];
		dst[10][j] = tmp[10];
		dst[14][j] = tmp[11];
		dst[18][j] = tmp[12];
		dst[22][j] = tmp[13];
		dst[26][j] = tmp[14];
		dst[30][j] = tmp[15];
		dst[1][j] = tmp[16];
		dst[3][j] = tmp[17];
		dst[5][j] = tmp[18];
		dst[7][j] = tmp[19];
		dst[9][j] = tmp[20];
		dst[11][j] = tmp[21];
		dst[13][j] = tmp[22];
		dst[15][j] = tmp[23];
		dst[17][j] = tmp[24];
		dst[19][j] = tmp[25];
		dst[21][j] = tmp[26];
		dst[23][j] = tmp[27];
		dst[25][j] = tmp[28];
		dst[27][j] = tmp[29];
		dst[29][j] = tmp[30];
		dst[31][j] = tmp[31];
  	}
  }



void prepare_luma_inter_32(U8 w, U8 h, MD_FW* md_fw_ptr, ENC_ME_LINE_MAP* me_line_map_ptr, ENC_ME_NEB_INFO inner_nebs[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN], ENC_ME_NEB_INFO above_nebs_line[ABOVE_LINE_LEN],
	SKIP_MERGE_INPUT* in, MD_COM_MODE* mod_info_curr,
	SKIP_RDO* skip_rdo, SKIP_ELEMENT skip_rdo_mode[MAX_INTER_SKIP_RDO], MERGE_RDO* merge_rdo, MD_INPUT* md_input_ptr,
	int num_amvr, u32 lambda_mv,
#if ENABLE_BFRAME
	s8 num_refp_cnt[2],
#else
	s8 num_refp_cnt,
#endif
	int ptr, int refp_ptr[17][2], s16 mvp_all[2][5][2],
	U1 valid_mode[17], int amvr_enable_flag, ENC_FME_INFO_ALL* fme_mv_info, pel RefWindowLCU[SWW * SWH], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_ref1[SWW * SWH], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS], ME_MV_DIR* me_mv_dir, MV_DIR* mv_dir,
#endif
	S14 sub_pel_mv[MV_D], SKIP_ELEMENT mode[SKIP_MODE_NUM], strFetch_ref_window* Fetch_Ref_window_ptr)
{
	s8 refi_cur = 0;
	U13 x_pos = mod_info_curr->x_pos;
	U13 y_pos = mod_info_curr->y_pos;
	U7 cu_width = mod_info_curr->cu_width;
	U7 cu_height = mod_info_curr->cu_height;
	U10 x_scu = x_pos >> 2;
	U10 y_scu = y_pos >> 2;

	U3 cu_width_log2 = mod_info_curr->cu_width_log2;
	U3 cu_height_log2 = mod_info_curr->cu_height_log2;
	U11 x0_scu;
	U11 y0_scu;
	x0_scu = PEL2SCU(x_pos - md_input_ptr->pix_x);
	y0_scu = PEL2SCU(y_pos - md_input_ptr->pix_y);

	U1 curr_ctu_idx = me_line_map_ptr->curr_ctu_idx;
	U1 above_line_idx = me_line_map_ptr->above_line_idx;
	U12 pix_x_scu = in->pix_x >> MIN_CU_LOG2;
	//SKIP_ELEMENT mode[SKIP_MODE_NUM];
	for (int i = 0; i < SKIP_MODE_NUM; ++i)
	{
		mode[i].valid = 0;
		mode[i].skip_flag = 0;
		mode[i].skip_idx = 0;
		mode[i].umve_flag = 0;
		mode[i].umve_idx = 0;
		mode[i].affine_flag = 0;
		mode[i].mvp[REFP_0][0] = 0;
		mode[i].mvp[REFP_0][1] = 0;
		mode[i].refi[REFP_0] = -1;
		mode[i].mvp[REFP_1][0] = 0;
		mode[i].mvp[REFP_1][1] = 0;
		mode[i].refi[REFP_1] = -1;
	}
	U6 num_cands_all = 0;
	U11 pic_width_in_scu = md_fw_ptr->pic_width_in_scu;
	U11 pic_height_in_scu = md_fw_ptr->pic_height_in_scu;
	U1 allow_affine = 0;
	U1 run_direct_skip = 1/*cu_width * cu_height >= 64*/;
	U1 run_affine_merge = allow_affine && cu_width >= 16 && cu_height >= 16;
	NEB_CU neb[NUM_SKIP_SPATIAL_MV];
	for (int i = 0; i < 6; ++i)
	{
		neb[i].x_scu_of_ctu = 0;
		neb[i].y_scu_of_ctu = 0;
		neb[i].valid_flag = 0;
	}
	SKIP_MERGE_MAP_SCU neb_map[NUM_SKIP_SPATIAL_MV];
	for (int i = 0; i < 6; ++i)
	{
		neb_map[i].map_mv[REFP_0][0] = 0;
		neb_map[i].map_mv[REFP_0][1] = 0;
		neb_map[i].map_refi[REFP_0] = REFI_INVALID;
		neb_map[i].map_mv[REFP_1][0] = 0;
		neb_map[i].map_mv[REFP_1][1] = 0;
		neb_map[i].map_refi[REFP_1] = REFI_INVALID;
		neb_map[i].map_scu = 0;
		neb_map[i].map_cu_mode = 0;
	}
	S7 mode_list[MAX_INTER_SKIP_RDO];

	init_skip_merge_rdo(skip_rdo, merge_rdo, mode_list);
	derive_neb_cu(pic_width_in_scu, x_scu, y_scu, cu_width, cu_height, neb, neb_map, inner_nebs, me_line_map_ptr->map_scu_inner, me_line_map_ptr->map_cu_mode_inner, above_nebs_line + pix_x_scu - me_line_map_ptr->min_scu_x + SCUX_OFFSET - 1, me_line_map_ptr->new_map_scu_above_line + pix_x_scu - me_line_map_ptr->min_scu_x + SCUX_OFFSET - 1, me_line_map_ptr->new_map_cu_mode_above_line + pix_x_scu - me_line_map_ptr->min_scu_x + SCUX_OFFSET - 1, curr_ctu_idx);
	derive_inter_cands(in, (U4*)&skip_rdo->num_rdo, neb, neb_map, x_scu, y_scu, pic_width_in_scu, pic_height_in_scu, mode, run_direct_skip, Fetch_Ref_window_ptr);
	derive_affine_skip_cands(cu_width, cu_height, x_scu, y_scu, pic_width_in_scu, pic_height_in_scu, merge_rdo, in, neb, neb_map, me_line_map_ptr, inner_nebs, above_nebs_line, pix_x_scu, run_affine_merge, Fetch_Ref_window_ptr);
	//make_cand_list_32(w, h, md_fw_ptr, mode_list, mod_info_curr, in, mode, refw, org);
#if SKIP_RESTRI
	check_skip_mv_in_200(mode, x_pos, y_pos, CtrPos, cu_width, cu_height, mod_info_curr);
#endif
	for (int i = 0, j = 0; i < SKIP_MODE_NUM && j < 4; ++i) {
		if (mode[i].valid && (mode[i].refi[REFP_0] >= 0 || REFI_IS_VALID(mode[i].refi[REFP_1])))
		{
			mode_list[j++] = i;
		}
	}
	output_skip_mode(skip_rdo, skip_rdo_mode, mode, mode_list);

	mod_info_curr->affine_flag = 0;
	mod_info_curr->mvp_from_hmvp_flag = 0;

	if (cu_width == 32 && cu_height == 32)
	{
		sub_pel_mv[MV_X] = fme_mv_info->sadTree32x32Fmv[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2][MV_X];
		sub_pel_mv[MV_Y] = fme_mv_info->sadTree32x32Fmv[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2][MV_Y];
#if ENABLE_BFRAME
		mv_dir->cur_refi = me_mv_dir->block32x32MvDir[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2].cur_refi;
		mv_dir->lidx = me_mv_dir->block32x32MvDir[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2].lidx;
#endif
	}
	else if (cu_width == 32 && cu_height == 16)
	{
		sub_pel_mv[MV_X] = fme_mv_info->sadTree32x16Fmv[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2][MV_X];
		sub_pel_mv[MV_Y] = fme_mv_info->sadTree32x16Fmv[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2][MV_Y];
#if ENABLE_BFRAME
		mv_dir->cur_refi = me_mv_dir->block32x16MvDir[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2].cur_refi;
		mv_dir->lidx = me_mv_dir->block32x16MvDir[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2].lidx;
#endif
	}
	else if (cu_width == 16 && cu_height == 32)
	{
		sub_pel_mv[MV_X] = fme_mv_info->sadTree16x32Fmv[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2][MV_X];
		sub_pel_mv[MV_Y] = fme_mv_info->sadTree16x32Fmv[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2][MV_Y];
#if ENABLE_BFRAME
		mv_dir->cur_refi = me_mv_dir->block16x32MvDir[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2].cur_refi;
		mv_dir->lidx = me_mv_dir->block16x32MvDir[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2].lidx;
#endif
	}
#if ENABLE_BFRAME
	for (refi_cur = 0; refi_cur < num_refp_cnt[mv_dir->lidx]; refi_cur++) {
		derive_mvp_line_all(md_fw_ptr, me_line_map_ptr, inner_nebs, above_nebs_line, mod_info_curr, ptr, mv_dir->lidx, (int)refi_cur, refp_ptr, num_amvr, mvp_all[refi_cur]);
	}
#else
	for (refi_cur = 0; refi_cur < num_refp_cnt; refi_cur++) {
		derive_mvp_line_all(md_fw_ptr, me_line_map_ptr, inner_nebs, above_nebs_line, mod_info_curr, ptr, 0, (int)refi_cur, refp_ptr, num_amvr, mvp_all[refi_cur]);
	}
#endif
	


	for (int x = 0; x < 4; x++) {
		if (x < skip_rdo->num_rdo)
		{
			valid_mode[x] = 1;
		}
		else
		{
			valid_mode[x] = 0;
		}
	}

	for (int y = 0; y < 5; y++) {
		if (y < merge_rdo->num_cands)
		{
			valid_mode[y + 4] = 1;
		}
		else
		{
			valid_mode[y + 4] = 0;
		}
	}

	for (int z = 0; z < 5; z++) {
		//if (amvr_enable_flag)
		//	valid_mode[z + 9] = 1;
		//else
		{
			valid_mode[9] = 1;
			valid_mode[10] = 0;
			valid_mode[11] = 0;
			valid_mode[12] = 0;
			valid_mode[13] = 0;
		}
	}

	for (int p = 0; p < 3; p++) {
		valid_mode[p + 14] = 0;
	}


	//else if (cu_width == 32 && cu_height == 8)
	//{
	//	sub_pel_mv[MV_X] = fme_mv_info->sadTree32x8Fmv[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2][MV_X];
	//	sub_pel_mv[MV_Y] = fme_mv_info->sadTree32x8Fmv[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2][MV_Y];
	//}
	//else if (cu_width == 8 && cu_height == 32)
	//{
	//	sub_pel_mv[MV_X] = fme_mv_info->sadTree8x32Fmv[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2][MV_X];
	//	sub_pel_mv[MV_Y] = fme_mv_info->sadTree8x32Fmv[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2][MV_Y];
	//}
	//else if (cu_width == 32 && cu_height == 4)
	//{
	//	sub_pel_mv[MV_X] = fme_mv_info->sadTree32x4Fmv[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2][MV_X];
	//	sub_pel_mv[MV_Y] = fme_mv_info->sadTree32x4Fmv[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2][MV_Y];
	//}
	//else //if (cu_width == 4 && cu_height == 32)
	//{
	//	sub_pel_mv[MV_X] = fme_mv_info->sadTree4x32Fmv[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2][MV_X];
	//	sub_pel_mv[MV_Y] = fme_mv_info->sadTree4x32Fmv[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2][MV_Y];
	//}

}

#if 1
void get_IP_pixel_32(U8 w, U8 h, u16 avail_cu, U11 pic_width_in_scu, U11 pic_height_in_scu,
	U4 bit_depth, U2 ch_type, pel left[2 * 32 + 3], pel up[2 * 32 + 3], U14 x, U14 y, ENC_ME_LINE_MAP* me_line_ptr, IP_ARRAY* IP_buffer_ptr, IP_MAP_SCU* MAP_SCU_IP_ptr)
{
	int i;
	int j;
	int width_in_scu = ch_type == 0 ? w >> 2 : w >> (2 - 1);
	int height_in_scu = ch_type == 0 ? h >> 2 : h >> (2 - 1);
	//4
	U4 unit_size = ch_type == 0 ? 1 << 2 : 1 << 2 >> 1;
	U2 unit_size_bit = ch_type == 0 ? 2 : 1;
	//x/4
	int x_scu = ((ch_type == 0 ? x : x << 1)) >> 2;
	//y/4
	int y_scu = ((ch_type == 0 ? y : y << 1)) >> 2;
	//number of padding pixel in the left column   64
	int pad_le = h;
	//number of padding pixel in the upper row     64
	int pad_up = w;
	//16
	int pad_le_in_scu = height_in_scu;
	//16
	int pad_up_in_scu = width_in_scu;
	int pic_x = x % 64;
	int pic_y = y % 64;
	int pic_x_scu = x_scu - MAP_SCU_IP_ptr->min_scu_x;
	int pic_y_scu = y_scu - MAP_SCU_IP_ptr->min_scu_y;
	int flag;

	S7 ip_array_size_idx = get_ip_array_size_idx(w, h);
#if MD_PRINT
	assert(ip_array_size_idx != -1);
#endif
	if ((((int)avail_cu) & 1 << 0) == 1 << 0) {
		if (pic_y == 0) {
			for (i = 0; i < w; i++) {
#pragma HLS LOOP_TRIPCOUNT max=8
				up[i + 3] = IP_buffer_ptr->up_temp_size[ip_array_size_idx][pic_x + i];
			}
			for (i = 0; i < pad_up_in_scu; i++) {

#pragma HLS LOOP_TRIPCOUNT max=2
				if (x_scu + width_in_scu + i < pic_width_in_scu) {
					flag = ((int)(MAP_SCU_IP_ptr->map_scu_above_line[MAP_SCU_IP_ptr->above_line_idx][x_scu + width_in_scu + i] >> 31 & ((
						unsigned int)1)));
				}
				else {
					flag = 0;
				}
				if (x_scu + width_in_scu + i < pic_width_in_scu && flag) {
					for (j = 0; j < unit_size; j++) {

#pragma HLS LOOP_TRIPCOUNT max=4
						up[w + (i << unit_size_bit) + j + 3] = IP_buffer_ptr->up_temp_size[ip_array_size_idx][pic_x + w + (i << unit_size_bit) + j];
					}
				}
				else {
					for (j = 0; j < unit_size; j++) {

#pragma HLS LOOP_TRIPCOUNT max=4
						up[w + (i << unit_size_bit) + j + 3] = up[w + (i << unit_size_bit) + 2];
					}
				}
			}
		}
		else {
			for (i = 0; i < w; i++) {

#pragma HLS LOOP_TRIPCOUNT max=8
				up[i + 3] = IP_buffer_ptr->up_innerLB_size[ip_array_size_idx][(pic_y >> 2) - 1][pic_x + i];

			}
			for (i = 0; i < pad_up_in_scu; i++) {

#pragma HLS LOOP_TRIPCOUNT max=2
				if (pic_x_scu + width_in_scu + i < 16) {
					flag = ((int)(MAP_SCU_IP_ptr->map_scu_inner[MAP_SCU_IP_ptr->curr_ctu_idx][pic_x_scu + width_in_scu + i][pic_y_scu - 1]
						>> 31 & ((unsigned int)1)));
				}
				else {
					flag = 0;
				}
				if (x_scu + width_in_scu + i < pic_width_in_scu && flag) {
					for (j = 0; j < unit_size; j++) {

#pragma HLS LOOP_TRIPCOUNT max=4

						up[w + (i << unit_size_bit) + j + 3] = IP_buffer_ptr->up_innerLB_size[ip_array_size_idx][(pic_y >> 2) - 1][pic_x + w + (i << unit_size_bit) + j];
					}
				}
				else {
					for (j = 0; j < unit_size; j++) {

#pragma HLS LOOP_TRIPCOUNT max=4
						up[w + (i << unit_size_bit) + j + 3] = up[w + (i << unit_size_bit) + 2];
					}
				}
			}
		}
	}
	if ((((int)avail_cu) & 1 << 1) == 1 << 1) {
		if (pic_x == 0) {
			for (i = 0; i < h; i++) {

#pragma HLS LOOP_TRIPCOUNT max=8
				left[i + 3] = IP_buffer_ptr->left_size[ip_array_size_idx][pic_y + i];
			}
			for (i = 0; i < pad_le_in_scu; i++) {

#pragma HLS LOOP_TRIPCOUNT max=2
				if (pic_y_scu + height_in_scu + i < 16) {
					flag = ((int)(MAP_SCU_IP_ptr->map_scu_inner[1 - MAP_SCU_IP_ptr->curr_ctu_idx][15][pic_y_scu + height_in_scu + i] >> 31
						& ((unsigned int)1)));
				}
				else {
					flag = 0;
				}
				if (y_scu + height_in_scu + i < pic_height_in_scu && flag) {
					for (j = 0; j < unit_size; j++) {

#pragma HLS LOOP_TRIPCOUNT max=4
						left[h + (i << unit_size_bit) + j + 3] = IP_buffer_ptr->left_size[ip_array_size_idx][pic_y + h + (i << unit_size_bit) + j];
					}
				}
				else {
					for (j = 0; j < unit_size; j++) {

#pragma HLS LOOP_TRIPCOUNT max=4
						left[h + (i << unit_size_bit) + j + 3] = left[h + (i << unit_size_bit) + 2];
					}
				}
			}
		}
		else {
			for (i = 0; i < h; i++) {

#pragma HLS LOOP_TRIPCOUNT max=8
				left[i + 3] = IP_buffer_ptr->left_innerLB_size[ip_array_size_idx][pic_y + i][(pic_x >> 2) - 1];
			}
			for (i = 0; i < pad_le_in_scu; i++) {

#pragma HLS LOOP_TRIPCOUNT max=2
				if (pic_y_scu + height_in_scu + i < 16) {
					flag = ((int)(MAP_SCU_IP_ptr->map_scu_inner[MAP_SCU_IP_ptr->curr_ctu_idx][pic_x_scu - 1][pic_y_scu + height_in_scu + i]
						>> 31 & ((unsigned int)1)));
				}
				else {
					flag = 0;
				}
				if (y_scu + height_in_scu + i < pic_height_in_scu && flag) {
					for (j = 0; j < unit_size; j++) {

#pragma HLS LOOP_TRIPCOUNT max=4
						left[h + (i << unit_size_bit) + j + 3] = IP_buffer_ptr->left_innerLB_size[ip_array_size_idx][h + (i << unit_size_bit) + j + pic_y][(pic_x >> 2) - 1];
					}
				}
				else {
					for (j = 0; j < unit_size; j++) {

#pragma HLS LOOP_TRIPCOUNT max=4
						left[h + (i << unit_size_bit) + j + 3] = left[h + (i << unit_size_bit) + 2];
					}
				}
			}
		}
	}
	if ((((int)avail_cu) & 1 << 2) == 1 << 2) {
		if (pic_y == 0 && pic_x == 0) {
			up[2] = left[2] = ((pel)(IP_buffer_ptr->top_left));
		}
		else if (pic_y == 0) {
			up[2] = left[2] = IP_buffer_ptr->up_temp_size[ip_array_size_idx][pic_x - 1];
		}
		else if (pic_x == 0) {
			up[2] = left[2] = IP_buffer_ptr->left_size[ip_array_size_idx][pic_y - 1];
		}
		else {
			up[2] = left[2] = IP_buffer_ptr->up_innerLB_size[ip_array_size_idx][(pic_y >> 2) - 1][pic_x - 1];
		}
	}
	else if ((((int)avail_cu) & 1 << 0) == 1 << 0) {
		up[2] = left[2] = up[3];
	}
	else if ((((int)avail_cu) & 1 << 1) == 1 << 1) {
		up[2] = left[2] = left[3];
	}
	up[1] = left[3];
	left[1] = up[3];
	up[0] = left[4];
	left[0] = up[4];
}
#else
void get_IP_pixel_32(U8 w, U8 h, u16 avail_cu, U11 pic_width_in_scu, U11 pic_height_in_scu,
	U4 bit_depth, U2 ch_type, pel left[2 * 32 + 3], pel up[2 * 32 + 3], U14 x, U14 y, ENC_ME_LINE_MAP* me_line_ptr, IP_ARRAY* IP_buffer_ptr, IP_MAP_SCU* MAP_SCU_IP_ptr)
{
	int i;
	int j;
	int width_in_scu = ch_type == 0 ? w >> 2 : w >> (2 - 1);//8
	int height_in_scu = ch_type == 0 ? h >> 2 : h >> (2 - 1);//8
	//4
	U4 unit_size = ch_type == 0 ? 1 << 2 : 1 << 2 >> 1;//4
	U2 unit_size_bit = ch_type == 0 ? 2 : 1;//2
	//x/4
	int x_scu = ((ch_type == 0 ? x : x << 1)) >> 2;// 1 2 3 4
	//y/4
	int y_scu = ((ch_type == 0 ? y : y << 1)) >> 2;// 1 2 3 4
	//number of padding pixel in the left column   64
	int pad_le = h;
	//number of padding pixel in the upper row     64
	int pad_up = w;
	//16
	int pad_le_in_scu = height_in_scu;//8
	//16
	int pad_up_in_scu = width_in_scu;//8
	int pic_x = x % 64;
	int pic_y = y % 64;
	int pic_x_scu = x_scu - MAP_SCU_IP_ptr->min_scu_x;
	int pic_y_scu = y_scu - MAP_SCU_IP_ptr->min_scu_y;
	int flag;

	S7 ip_array_size_idx = get_ip_array_size_idx(w, h);
#if MD_PRINT
	assert(ip_array_size_idx != -1);
#endif
	if ((((int)avail_cu) & 1 << 0) == 1 << 0) {
		if (pic_y == 0) {
			//top
			for (i = 0; i < w; i++) {
#pragma HLS UNROLL
				up[i + 3] = IP_buffer_ptr->up_temp_size[ip_array_size_idx][pic_x + i];
			}

			//top right
			for (i = 0; i < pad_up_in_scu; i++) {
#pragma HLS PIPELINE
				if (x_scu + width_in_scu + i < pic_width_in_scu) {
					for (j = 0; j < unit_size; j++) {
#pragma HLS UNROLL
						up[w + (i << unit_size_bit) + j + 3] = IP_buffer_ptr->up_temp_size[ip_array_size_idx][pic_x + w + (i << unit_size_bit) + j];
					}
				}
				else {
					for (j = 0; j < unit_size; j++) {
#pragma HLS UNROLL
						up[w + (i << unit_size_bit) + j + 3] = up[w + (i << unit_size_bit) + 2];
					}
				}
			}
		}
		else {

			//top
			for (i = 0; i < w; i++) {
#pragma HLS UNROLL
				up[i + 3] = IP_buffer_ptr->up_innerLB_size[ip_array_size_idx][(pic_y >> 2) - 1][pic_x + i];

			}

			//top right
			for (i = 0; i < pad_up_in_scu; i++) {
#pragma HLS PIPELINE
				if (x_scu + width_in_scu + i < pic_width_in_scu) {
					for (j = 0; j < unit_size; j++) {
						up[w + (i << unit_size_bit) + j + 3] = IP_buffer_ptr->up_innerLB_size[ip_array_size_idx][(pic_y >> 2) - 1][pic_x + w + (i << unit_size_bit) + j];
					}
				}
				else {
					for (j = 0; j < unit_size; j++) {
						up[w + (i << unit_size_bit) + j + 3] = up[w + (i << unit_size_bit) + 2];
					}
				}
			}
		}
	}
#if 1
	if ((((int)avail_cu) & 1 << 1) == 1 << 1) {
		if (pic_x == 0) {
			//left
			for (i = 0; i < h; i++) {
#pragma HLS UNROLL
				left[i + 3] = IP_buffer_ptr->left_size[ip_array_size_idx][pic_y + i];
			}
			//left bottom
			for (i = 0; i < pad_le_in_scu; i++) {
#pragma HLS PIPELINE
				if (y_scu + height_in_scu + i < pic_height_in_scu) {
					for (j = 0; j < unit_size; j++) {
						left[h + (i << unit_size_bit) + j + 3] = IP_buffer_ptr->left_size[ip_array_size_idx][pic_y + h + (i << unit_size_bit) + j];
					}
				}
				else {
					for (j = 0; j < unit_size; j++) {
						left[h + (i << unit_size_bit) + j + 3] = left[h + (i << unit_size_bit) + 2];
					}
				}
			}
		}
		else {
			for (i = 0; i < h; i++) {
#pragma HLS UNROLL
				left[i + 3] = IP_buffer_ptr->left_innerLB_size[ip_array_size_idx][pic_y + i][(pic_x >> 2) - 1];
			}
			for (i = 0; i < pad_le_in_scu; i++) {
#pragma HLS PIPELINE
				if (y_scu + height_in_scu + i < pic_height_in_scu) {
					for (j = 0; j < unit_size; j++) {
						left[h + (i << unit_size_bit) + j + 3] = IP_buffer_ptr->left_innerLB_size[ip_array_size_idx][h + (i << unit_size_bit) + j + pic_y][(pic_x >> 2) - 1];
					}
				}
				else {
					for (j = 0; j < unit_size; j++) {
						left[h + (i << unit_size_bit) + j + 3] = left[h + (i << unit_size_bit) + 2];
					}
				}
			}
		}
	}

	if ((((int)avail_cu) & 1 << 2) == 1 << 2) {
		if (pic_y == 0 && pic_x == 0) {
			up[2] = left[2] = ((pel)(IP_buffer_ptr->top_left));
		}
		else if (pic_y == 0) {
			up[2] = left[2] = IP_buffer_ptr->up_temp_size[ip_array_size_idx][pic_x - 1];
		}
		else if (pic_x == 0) {
			up[2] = left[2] = IP_buffer_ptr->left_size[ip_array_size_idx][pic_y - 1];
		}
		else {
			up[2] = left[2] = IP_buffer_ptr->up_innerLB_size[ip_array_size_idx][(pic_y >> 2) - 1][pic_x - 1];
		}
	}
	else if ((((int)avail_cu) & 1 << 0) == 1 << 0) {
		up[2] = left[2] = up[3];
	}
	else if ((((int)avail_cu) & 1 << 1) == 1 << 1) {
		up[2] = left[2] = left[3];
	}
	up[1] = left[3];
	left[1] = up[3];
	up[0] = left[4];
	left[0] = up[4];
#endif
}
#endif
int get_rmd_result_32(U3 cu_width_log2, U3 cu_heght_log2, U13 x, U13 y, u8 ipred_list[5], u8 rmd_mode[IPD_RDO_CNT])
{
	//int i;
	//int w = cu_width_log2 - 2;
	//int h = cu_heght_log2 - 2;
	//int x0 = x >> 2;
	//int y0 = y >> 2;
	//int idx = get_g_map_idx(w, h, x0, y0);
	//int cnt = ((((cu_width_log2 - cu_heght_log2) >= 2) || ((cu_heght_log2 - cu_width_log2) >= 2)) ? (IPD_RDO_CNT - 1) : IPD_RDO_CNT);
  int cnt = 5;
	for (int i = 0; i < IPD_RDO_CNT; i++) {
#pragma HLS PIPELINE
		ipred_list[i] = rmd_mode[i];
	}

	return cnt;
}



void prepare_intra_info_32(U8 w, U8 h, MD_FW* md_fw_ptr, ENC_ME_LINE_MAP* me_line_map_ptr, MD_COM_MODE* mod_info_curr_ptr,
	IP_ARRAY* IP_buffer_ptr, MPM_ARRAY* MPM_buffer_ptr, IP_MAP_SCU* MAP_SCU_IP_ptr, U3 cu_width_log2, U3 cu_height_log2,
	U13 pix_x, U13 pix_y, U14 sub_x, U14 sub_y, u8 ipred_list[33], int* pred_cnt, pel src_le_temp[2 * 32 + 3], pel src_up_temp[2 * 32 + 3],
	u16* avail_tb, u8 rmd_mode[IPD_RDO_CNT])
{
	int pb_x_scu = mod_info_curr_ptr->x_pos >> 2;
	int pb_y_scu = mod_info_curr_ptr->y_pos >> 2;
	int tb_x_scu;
	int tb_y_scu;
	int i;
	int bit_depth;

	com_get_mpm(pb_x_scu, pb_y_scu, mod_info_curr_ptr->mpm[0], MPM_buffer_ptr, me_line_map_ptr);

	get_rmd_result_32(cu_width_log2, cu_height_log2, pix_x, pix_y, ipred_list, rmd_mode);

	bit_depth = md_fw_ptr->bit_depth_internal;
	tb_x_scu = sub_x >> 2;
	tb_y_scu = sub_y >> 2;
	*avail_tb = com_get_avail_intra_ip(MAP_SCU_IP_ptr, tb_x_scu, tb_y_scu);

	for (i = 0; i < (2 * h + 3); i++) {
#pragma HLS UNROLL
		src_le_temp[i] = (1 << (bit_depth - 1));
	}
	for (i = 0; i < (2 * w + 3); i++) {
#pragma HLS UNROLL
		src_up_temp[i] = (1 << (bit_depth - 1));
	}


	get_IP_pixel_32(w, h, *avail_tb, md_fw_ptr->pic_width_in_scu, md_fw_ptr->pic_height_in_scu, bit_depth, 0, src_le_temp, src_up_temp, sub_x, sub_y, me_line_map_ptr, IP_buffer_ptr, MAP_SCU_IP_ptr);
}

#if NO_SUB_MODEL_SYN
void transform_hor_32_lite(U4 bit_depth, s16 resi[32][32], s16 coef_temp[32][32])
{
//#pragma HLS ARRAY_PARTITION variable=coef_temp complete dim=2
//#pragma HLS ARRAY_PARTITION variable=resi complete dim=2
#if 0
		stage_v_copy(resi,coef_temp);
#else
  U8 cu_width = 32;
  U8 cu_height = 32;
  U3 tb_width_log2 = com_tbl_log2[cu_width];
  U3 tb_height_log2 = com_tbl_log2[cu_height];
  U4 shift1 = com_get_forward_trans_shift(tb_width_log2, 0, bit_depth);
  dct_row_32_lite(cu_width, cu_height, resi, coef_temp, shift1);
#endif
}

void transform_ver_32_lite(U4 bit_depth, s16 coef_tmp[32][32], s16 coef[32][32])
{
//#pragma HLS ARRAY_PARTITION variable=coef_tmp complete dim=2
//#pragma HLS ARRAY_PARTITION variable=coef complete dim=1
#if 0
	stage_v_copy(coef_tmp,coef);
#else
	U8 cu_width = 32;
	U8 cu_height = 32;
	U3 tb_width_log2 = com_tbl_log2[cu_width];
	U3 tb_height_log2 = com_tbl_log2[cu_height];
	U4 shift2 = com_get_forward_trans_shift(tb_height_log2, 1, bit_depth);
	dct_col_32_lite(cu_width, cu_height, coef_tmp, coef, shift2);
#endif
}

void inv_transform_ver_32_lite(U4 bit_depth, U6 qp_y, s16 coef_tmp[32][32],s16 coef_tmp_h_to_it[32][32])
{
#if 0
	stage_v_copy(coef_tmp, coef_tmp_h_to_it);
#else
#pragma HLS DATAFLOW
#pragma HLS ARRAY_PARTITION variable=coef_tmp complete dim=1
#pragma HLS ARRAY_PARTITION variable=coef_tmp_h_to_it complete dim=1
	hls::stream<s512_h> coef_dq;
#pragma HLS STREAM variable=coef_dq depth=4096
	dquant_32_s(qp_y, coef_tmp, coef_dq,bit_depth);
	itrans_v_32_lite_s(coef_dq, coef_tmp_h_to_it,bit_depth);
#endif
}

void inv_transform_hor_32_lite(
  U4 bit_depth,
  pel pred_cache[32][32],
  s32 dist_rec[1],
  s64* dist_fz,
  pel rec[32][32],
  s16 coef_temp[32][32],
  pel org[32][32]
)
{
#if 0
	stage_v_copy(coef_temp, rec);
#else
#pragma HLS DATAFLOW
#pragma HLS ARRAY_PARTITION variable=coef_temp complete dim=1
#pragma HLS ARRAY_PARTITION variable=rec complete dim=2
#pragma HLS ARRAY_PARTITION variable=org complete dim=2
#pragma HLS ARRAY_PARTITION variable=pred_cache complete dim=2
//#pragma HLS ARRAY_PARTITION variable=pred_cache_32x32_update complete dim=2
	  U8 cu_width = 32;
	  U8 cu_height = 32;
	  U3 cu_width_log2 = 5;
	  U3 cu_height_log2 = 5;
		// STREAM BUFFER
	  s32 dist_write = 0;
	  s64 dist_fz_write = MAX_S64;

	  hls::stream<s512_h> org_s1;
#pragma HLS STREAM variable=org_s1 depth=4096
	  hls::stream<s512_h> org_s2;
#pragma HLS STREAM variable=org_s2 depth=4096
	  hls::stream<s512_h> resi_s;
#pragma HLS STREAM variable=resi_s depth=4096
	  hls::stream<s512_h> pred_cache_s1;
#pragma HLS STREAM variable=pred_cache_s1 depth=4096
	  hls::stream<s512_h> pred_cache_s2;
#pragma HLS STREAM variable=pred_cache_s2 depth=4096
	  hls::stream<s512_h> pred_cache_s3;
#pragma HLS STREAM variable=pred_cache_s3 depth=4096
	  hls::stream<s512_h> rec_s1;
#pragma HLS STREAM variable=rec_s1 depth=4096
	  hls::stream<s512_h> rec_s2;
#pragma HLS STREAM variable=rec_s2 depth=4096
	  hls::stream<s512_h> rec_s3;
#pragma HLS STREAM variable=rec_s3 depth=4096
	  s32 cost = 0;
	  s64 cost_fz = 0;


	  stage_h_copy(pred_cache,pred_cache_s1);
	  stage_h_copy(org,org_s1);
	  //stage_h_copy2(pred_cache_s3,pred_cache_32x32_update);//ram2s s2ram
	  //stage_h_copy(pred_cache,pred_cache_s1);//ram2s
	  //stage_h_copy(pred_cache,pred_cache_s2);//ram2s

	  //stage_h_copy(org,org_s1);//ram2s
	  //stage_h_copy(org,org_s2);//ram2s

	  itrans_h_32_lite_s(resi_s, coef_temp,bit_depth);
	  recon_32_lite_s(resi_s, pred_cache_s1,1, 0,rec_s1, bit_depth);

	  stage_h_split_copy(rec_s1,rec_s2,rec_s3);
	  stage_h_copy(rec_s3,rec);

	  s64 dist_tmp;
	  enc_ssd_16b_1_32_s(rec_s2, org_s1, bit_depth,dist_rec);
	 // dist_write = cost;
	 // enc_ssd_16b_1_32_s(pred_cache_s2, org_s2, bit_depth,dist_fz);
	 // dist_fz_write = cost_fz;
#endif
}

void rdo_prepare_32(
  RDOQ_MODEL* model_cnt,
  MD_FW* md_fw_ptr,
  ENC_ME_LINE_MAP* me_line_map_ptr,
  ENC_ME_NEB_INFO inner_nebs[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN],
  ENC_ME_NEB_INFO above_nebs_line[ABOVE_LINE_LEN],
  SKIP_MERGE_INPUT* skip_in,
  MD_COM_MODE* mod_info_curr_ptr,
  MERGE_RDO* merge_rdo,
  MD_INPUT* md_input_ptr,
  U1 valid_mode[17],
  U1 amvr_enable_flag,
  ENC_FME_INFO_ALL* fme_mv_info,
  IP_ARRAY* IP_buffer_ptr,
  MPM_ARRAY* MPM_buffer_ptr,
  IP_MAP_SCU* MAP_SCU_IP_ptr,
  u8 ipred_list[5],
  pel src_le_temp[2 * 32 + 3],
  pel src_up_temp[2 * 32 + 3],
  u16* avail_tb,
  u8 rmd_mode[IPD_RDO_CNT],
  U5 all_inter_mode_num,
  pel RefWindowLCU[SWW * SWH],
  S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
  u8 curr_cons_pred_mode,
  u8 inter_num[1],
  u8 intra_num[1],
  u8 rdo_list[5],
  SKIP_ELEMENT skip_mode[SKIP_MODE_NUM],
  SKIP_RDO* skip_rdo,
  SKIP_ELEMENT skip_rdo_mode[MAX_INTER_SKIP_RDO],
  S14 sub_pel_mv[MV_D],
  s16 mvp_all[2][5][2],
  U4 bit_depth,
  strFetch_ref_window* Fetch_Ref_window_ptr)
{

#if 0
	   //src_le_temp[2 * 32 + 3],
	  for(int i = 0 ; i < 64 ; i ++)
	  {
#pragma HLS UNROLL
		  src_up_temp[3+i] = IP_buffer_ptr->up_innerLB_size[0][0][i];
	  }
	  src_up_temp[0] = src_up_temp[3];
	  src_up_temp[1] = src_up_temp[3];
	  src_up_temp[2] = src_up_temp[3];
#else
  U7 w = 32;
  U7 h = 32;
  U3 cu_width_log2 = 5;
  U3 cu_height_log2 = 5;

	U14 sub_x = mod_info_curr_ptr->x_pos;
	U14 sub_y = mod_info_curr_ptr->y_pos;

	U13 pix_x = sub_x - md_input_ptr->pix_x;
	U13 pix_y = sub_y - md_input_ptr->pix_y;


	inter_num[0] = 0;
	intra_num[0] = 0;

	int pred_cnt[1] = { 5 };

	prepare_intra_info_32(w, h, md_fw_ptr, me_line_map_ptr, mod_info_curr_ptr, IP_buffer_ptr, MPM_buffer_ptr, MAP_SCU_IP_ptr,
			cu_width_log2, cu_height_log2, pix_x, pix_y, sub_x, sub_y, ipred_list, pred_cnt, src_le_temp, src_up_temp, avail_tb, rmd_mode);

	for (int intra_index = 0; intra_index < 5; intra_index++)
	{
#pragma HLS PIPELINE
		rdo_list[inter_num[0] + intra_index] = all_inter_mode_num + intra_index;
	}
		//}
#endif
}

#endif
}

s64 rdo_luma_32_32(pel orgY_32[32][32], s8 inter_mode_idx, u8 ipred_list[5],
		pel src_le_temp[2 * 32 + 3], pel src_up_temp[2 * 32 + 3], u16 avail_tb,
		u8 rdo_list[5], U4 bit_depth, s64 lambda_y, U2 tree_status,
		U2 curr_cons_pred_mode,
		MD_COM_MODE* mod_info_curr_ptr, RDOQ_MODEL* model_cnt,
		RDOQ_ARRAY* rdoq_array, MD_INPUT* md_input_ptr,
		MD_KERNEL_INPUT_32* md_kernel_input_32,
		ENC_CU_DATA_ARRAY* cu_data_temp_ptr, MD_COM_MODE_BEST* best_info_ptr,s64 *cost_best_return) {
#pragma HLS DATAFLOW
	//valid_flag = 1;
	//is_intra_quant = 1;
	s64 dist_pred;
	s64 cost_best = MAX_S64;
	s32 dist_rec[1];
	s64 flag_cost[3];
	hls::stream<s512_h> orgY_s;
	hls::stream<s512_h> orgY_s_1;
	hls::stream<s512_h> orgY_s_2;
	hls::stream<s512_h> pred_cache_32_s;
	hls::stream<s512_h> coef_tmp_32_s;
	s16 orgY_32_0[32][32];
	s16 orgY_32_1[32][32];
	s16 orgY_32_2[32][32];
	pel pred_cache_32x32[32][32];
	pel pred_cache_32x32_update[32][32];
	s16 coef_tmp_32x32[32][32];
	s16 coef_tmp_h[32][32];
	s16 coef_tmp_v[32][32];
	s16 coef_tmp_v_to_clean[32][32];
	s16 coef_tmp_v_to_it[32][32];
	s16 coef_tmp_v_to_it_1[32][32];
	s16 coef_tmp_v_to_it_2[32][32];
	s16 coef_tmp_v_to_it_3[32][32];
	s16 coef_tmp_v_to_it_4[32][32];
	s16 coef_tmp_h_to_it[32][32];
	pel rec_y_pb_part[32][32];
	int final_x;
	int final_y;
	int num_nz_rate[MAX_NUM_TB][N_C];
	int num_nz_rate_1[MAX_NUM_TB][N_C];
	int num_nz_rate_2[MAX_NUM_TB][N_C];
	MD_KERNEL_MODE_32 md_info_nz_32;
	MD_KERNEL_MODE_32 md_info_nz_32_1;
	MD_KERNEL_MODE_32 md_info_nz_32_2;
#pragma HLS STREAM variable=dist_rec off depth=10
#pragma HLS ARRAY_PARTITION variable=dist_rec complete dim=0
#pragma HLS STREAM variable=flag_cost off depth=10
#pragma HLS ARRAY_PARTITION variable=flag_cost complete dim=0
#pragma HLS STREAM variable=orgY_s depth=4096
#pragma HLS STREAM variable=orgY_s_1 depth=4096
#pragma HLS STREAM variable=orgY_s_2 depth=4096
#pragma HLS STREAM variable=pred_cache_32_s depth=4096
#pragma HLS STREAM variable=coef_tmp_32_s depth=4096
#pragma HLS STREAM variable=orgY_32_0 off depth=10
#pragma HLS ARRAY_PARTITION variable=orgY_32_0 complete dim=2
#pragma HLS STREAM variable=orgY_32_1 off depth=10
#pragma HLS ARRAY_PARTITION variable=orgY_32_1 complete dim=2
#pragma HLS STREAM variable=orgY_32_2 off depth=10
#pragma HLS ARRAY_PARTITION variable=orgY_32_2 complete dim=2
#pragma HLS STREAM variable=pred_cache_32x32 off depth=10
#pragma HLS ARRAY_PARTITION variable=pred_cache_32x32 complete dim=2
#pragma HLS STREAM variable=pred_cache_32x32_update off depth=10
#pragma HLS ARRAY_PARTITION variable=pred_cache_32x32_update complete dim=2
#pragma HLS STREAM variable=coef_tmp_32x32 off depth=10
#pragma HLS ARRAY_PARTITION variable=coef_tmp_32x32 complete dim=2
#pragma HLS STREAM variable=coef_tmp_h off depth=10
#pragma HLS ARRAY_PARTITION variable=coef_tmp_h complete dim=2
#pragma HLS STREAM variable=coef_tmp_v off depth=10
#pragma HLS ARRAY_PARTITION variable=coef_tmp_v complete dim=1
#pragma HLS STREAM variable=coef_tmp_v_to_clean off depth=10
#pragma HLS ARRAY_PARTITION variable=coef_tmp_v_to_clean complete dim=1
#pragma HLS STREAM variable=coef_tmp_v_to_it off depth=10
#pragma HLS ARRAY_PARTITION variable=coef_tmp_v_to_it complete dim=1
#pragma HLS STREAM variable=coef_tmp_v_to_it_1 off depth=10
#pragma HLS ARRAY_PARTITION variable=coef_tmp_v_to_it_1 complete dim=1
#pragma HLS STREAM variable=coef_tmp_v_to_it_2 off depth=10
#pragma HLS ARRAY_PARTITION variable=coef_tmp_v_to_it_2 complete dim=1
#pragma HLS STREAM variable=coef_tmp_v_to_it_3 off depth=10
#pragma HLS ARRAY_PARTITION variable=coef_tmp_v_to_it_3 complete dim=1
#pragma HLS STREAM variable=coef_tmp_v_to_it_4 off depth=10
#pragma HLS ARRAY_PARTITION variable=coef_tmp_v_to_it_4 complete dim=1
#pragma HLS STREAM variable=coef_tmp_h_to_it off depth=10
#pragma HLS ARRAY_PARTITION variable=coef_tmp_h_to_it complete dim=1
#pragma HLS STREAM variable=rec_y_pb_part off depth=10
#pragma HLS ARRAY_PARTITION variable=rec_y_pb_part complete dim=2
#pragma HLS STREAM variable=num_nz_rate off depth=10
#pragma HLS ARRAY_PARTITION variable=num_nz_rate complete dim=0
#pragma HLS STREAM variable=num_nz_rate_1 off depth=10
#pragma HLS ARRAY_PARTITION variable=num_nz_rate_1 complete dim=0
#pragma HLS STREAM variable=num_nz_rate_2 off depth=10
#pragma HLS ARRAY_PARTITION variable=num_nz_rate_2 complete dim=0
#pragma HLS STREAM variable=md_info_nz_32.ipm off depth=10
#pragma HLS ARRAY_PARTITION variable=md_info_nz_32.ipm complete dim=0
#pragma HLS STREAM variable=md_info_nz_32.mpm  off depth=10
#pragma HLS ARRAY_PARTITION variable=md_info_nz_32.mpm complete dim=0
#pragma HLS STREAM variable=md_info_nz_32_1.ipm off depth=10
#pragma HLS ARRAY_PARTITION variable=md_info_nz_32_1.ipm complete dim=0
#pragma HLS STREAM variable=md_info_nz_32_1.mpm  off depth=10
#pragma HLS ARRAY_PARTITION variable=md_info_nz_32_1.mpm complete dim=0
#pragma HLS STREAM variable=md_info_nz_32_2.ipm off depth=10
#pragma HLS ARRAY_PARTITION variable=md_info_nz_32_2.ipm complete dim=0
#pragma HLS STREAM variable=md_info_nz_32_2.mpm  off depth=10
#pragma HLS ARRAY_PARTITION variable=md_info_nz_32_2.mpm complete dim=0


	hls::stream<s64_h>  cost_s;
#pragma HLS STREAM variable=cost_s depth=4096
	hls::stream<s512_h> rec_y_s;
#pragma HLS STREAM variable=rec_y_s depth=4096
	hls::stream<s512_h> coef_y_s;
#pragma HLS STREAM variable=coef_y_s depth=4096
	hls::stream<u64_h> mpm_s;
#pragma HLS STREAM variable=mpm_s depth=4096
	hls::stream<s64_h> ipm_s;
#pragma HLS STREAM variable=ipm_s depth=4096
	hls::stream<s384_h> num_nz_s;
#pragma HLS STREAM variable=num_nz_s depth=4096

	for (u8 rdo_idx = 0; rdo_idx < 5; rdo_idx++) {
		// md_kernel_32_hls::stage_h_split_copy(orgY_32,orgY_32_1,orgY_32_2);
		md_kernel_32_hls::stage_h_split_copy(orgY_32, orgY_s_1, orgY_s_2);
	}
	for (u8 rdo_idx = 0; rdo_idx < 5; rdo_idx++) {
#pragma HLS DATAFLOW
		md_kernel_32_hls::intra_inter_pred_32(&md_info_nz_32, orgY_s_1,
				inter_mode_idx, coef_tmp_32_s, pred_cache_32_s, ipred_list,
				src_le_temp, src_up_temp, avail_tb, rdo_list, rdo_idx,
				mod_info_curr_ptr->mpm, bit_depth);
		md_kernel_32_hls::stage_h_split_copy(md_info_nz_32.ipm,
				md_info_nz_32_1.ipm, md_info_nz_32_2.ipm);
		md_kernel_32_hls::stage_h_split_copy(md_info_nz_32.mpm,
				md_info_nz_32_1.mpm, md_info_nz_32_2.mpm);
		//md_kernel_32_hls::stream2ram_copy(pred_cache_32_s,pred_cache_32x32);
		//md_kernel_32_hls::stream2ram_copy(coef_tmp_32_s,coef_tmp_32x32);
		md_kernel_32_hls::stage_h_copy(orgY_s_2, orgY_32_2);
		md_kernel_32_hls::stage_h_copy(pred_cache_32_s, pred_cache_32x32);
		md_kernel_32_hls::stage_h_copy(coef_tmp_32_s, coef_tmp_32x32);
		//md_kernel_32_hls::stage_h_copy(orgY_s, orgY_32_1);
		md_kernel_32_hls::transform_hor_32_lite(bit_depth, coef_tmp_32x32,
				coef_tmp_h);
		md_kernel_32_hls::transform_ver_32_lite(bit_depth, coef_tmp_h,
				coef_tmp_v);
		md_kernel_32_hls::quant_nnz_32_lite(model_cnt, rdoq_array, bit_depth,
				md_input_ptr->qp_y, lambda_y, coef_tmp_v, coef_tmp_v_to_clean,
				&final_x, &final_y);
		md_kernel_32_hls::CleanNz_32_32(num_nz_rate, coef_tmp_v_to_it,
				coef_tmp_v_to_clean, final_x, final_y);
		//memcpy_rewrintra_2d_to_1d_32(coef_y_pb_part_update, coef_tmp_v_to_it,5,5);
		md_kernel_32_hls::stage_v_split_copy(num_nz_rate, num_nz_rate_1,
				num_nz_rate_2);
		md_kernel_32_hls::stage_v_copy(coef_tmp_v_to_it, coef_tmp_v_to_it_1,
				coef_tmp_v_to_it_2, coef_tmp_v_to_it_3, coef_tmp_v_to_it_4);
		md_kernel_32_hls::rate_est_32(model_cnt, &md_info_nz_32_1, tree_status,
				curr_cons_pred_mode, coef_tmp_v_to_it_1, coef_tmp_v_to_it_2,
				lambda_y, flag_cost, num_nz_rate_1, md_kernel_input_32);
		md_kernel_32_hls::inv_transform_ver_32_lite(bit_depth,
				md_input_ptr->qp_y, coef_tmp_v_to_it_3, coef_tmp_h_to_it);
		md_kernel_32_hls::inv_transform_hor_32_lite(bit_depth, pred_cache_32x32,
				dist_rec, &dist_pred, rec_y_pb_part, coef_tmp_h_to_it,
				orgY_32_2);


    //if(md_kernel_32_file_out == NULL)
    //  md_kernel_32_file_out = fopen("md_kernel_32_file_out_test.dat","w");
    //fwrite(rec_y_pb_part ,sizeof(S16),1*CU_SIZE*16,md_kernel_32_file_out);

		//md_kernel_32_hls::update_rdo_luma_32(cu_data_temp_ptr, &md_info_nz_32_2,
		//		&dist_pred, best_info_ptr, &cost_best, rec_y_pb_part,
		//		coef_tmp_v_to_it_4, dist_rec, flag_cost, num_nz_rate_2);
		md_kernel_32_hls::update_rdo_luma_32(rec_y_pb_part,coef_tmp_v_to_it_4,md_info_nz_32_2.mpm,
				md_info_nz_32_2.ipm,num_nz_rate_2,dist_rec,flag_cost,
				cost_s,rec_y_s,coef_y_s,mpm_s,ipm_s,num_nz_s);
	}

	for(int i = 0 ; i < 5 ; i ++){
		md_kernel_32_hls::update_rdo_luma_32_final(cost_s,rec_y_s,coef_y_s,mpm_s,ipm_s,num_nz_s,
				cu_data_temp_ptr->reco_y_t,cu_data_temp_ptr->coef_y_t,
				best_info_ptr->mpm,
				best_info_ptr->ipm,
				best_info_ptr->num_nz,
			    &cost_best
		);
	}

	*cost_best_return = cost_best;
}

s64 rdo_luma_32_lite3(U1 isNS_allow, U8 w, U8 h, RDOQ_MODEL* model_cnt, RDOQ_ARRAY* rdoq_array, MD_FW* md_fw_ptr, ENC_ME_LINE_MAP* me_line_map_ptr,
	ENC_ME_NEB_INFO inner_nebs[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN], ENC_ME_NEB_INFO above_nebs_line[ABOVE_LINE_LEN], SKIP_MERGE_INPUT* skip_in,
	U2 tree_status, MD_COM_MODE_BEST* best_info_ptr, MD_INPUT* md_input_ptr,pel orgY_32[32][32], pel RefWindowLCU[SWW * SWH], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_ref1[SWW * SWH], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS], ME_MV_DIR* me_mv_dir, pel refwin_md_ref1[88][88], S14 CtrPosFME_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS],
#endif
	MD_COM_MODE* mod_info_curr_ptr, ENC_CU_DATA_ARRAY* cu_data_temp_ptr, U2 curr_cons_pred_mode, U3 qt_depth,
	ENC_FME_INFO_ALL* fme_mv_info,
	u8 rmd_mode[IPD_RDO_CNT], IP_ARRAY* IP_buffer_ptr, MPM_ARRAY* MPM_buffer_ptr, IP_MAP_SCU* MAP_SCU_IP_ptr,
	MD_KERNEL_INPUT_32* md_kernel_input_32, pel refwin_md[88][88], S14 CtrPosFME[RANGE_NUM][DIR_DIM][MAP_DIMS],
	strFetch_ref_window* Fetch_Ref_window_ptr
	)
{

#pragma HLS ARRAY_PARTITION variable=orgY_32 complete dim=2
//#pragma HLS ARRAY_PARTITION variable=coef_tmp_v complete dim=2
#pragma HLS ARRAY_PARTITION variable=cu_data_temp_ptr->coef_y_t complete dim=1
#pragma HLS ARRAY_PARTITION variable=cu_data_temp_ptr->reco_y_t complete dim=2

#pragma HLS ARRAY_PARTITION variable=IP_buffer_ptr->up_temp_size complete dim=2
#pragma HLS ARRAY_PARTITION variable=IP_buffer_ptr->up_innerLB_size complete dim=3//
//#pragma HLS DATAFLOW
#pragma HLS ARRAY_PARTITION variable=model_cnt->last1 complete dim=1
#pragma HLS ARRAY_PARTITION variable=model_cnt->last2 complete dim=1
#pragma HLS ARRAY_PARTITION variable=model_cnt->run_rdoq complete dim=1
#pragma HLS ARRAY_PARTITION variable=model_cnt->level complete dim=1

#pragma HLS ARRAY_PARTITION variable=rdoq_array->rdoq_est_cbf complete dim=0
#pragma HLS ARRAY_PARTITION variable=rdoq_array->rdoq_est_run_local complete dim=0
#pragma HLS ARRAY_PARTITION variable=rdoq_array->rdoq_est_level_local complete dim=0
#pragma HLS ARRAY_PARTITION variable=rdoq_array->rdoq_est_last_local complete dim=0


	U4 bit_depth = md_fw_ptr->bit_depth_internal;
	s64 cost_best = MAX_S64;
	U3 cu_width_log2 = mod_info_curr_ptr->cu_width_log2;
	U3 cu_height_log2 = mod_info_curr_ptr->cu_height_log2;
	U7 cu_width = (1 << cu_width_log2);
	U7 cu_height = (1 << cu_height_log2);
	U1 emvr_enable_flag = 1;
	U1 amvr_enable_flag = 1;
	U1 affine_enable_flag = 1;


	U14 sub_x = mod_info_curr_ptr->x_pos;
	U14 sub_y = mod_info_curr_ptr->y_pos;

	U13 pix_x = sub_x - md_input_ptr->pix_x;
	U13 pix_y = sub_y - md_input_ptr->pix_y;

	u8 mode_type, mode_type_update;
	u8 curr_mvr, curr_mvr_update;

	int  num_nz_itv, num_nz_ith, num_nz_update[MAX_NUM_TB][N_C];


	U1 valid_mode[17];   //{SKIP:0,1,2,3;Skip_Merge:4,5,6,7,8;Mode1:9,10,11,12,13;Mode2:14,15,16};    //{Mode 0:(skip mode:fix 4, skip_merger:0~5), Mode1:(ME:AMVR:FIX 1/5); Mode 2(AFF_ME::FIX 1/3)
	U1 valid_flag =  0;

	int bit_cnt = 0;
	int stride_org = 64;

#if 0
	pel orgY[32 * 32];
	pel orgY_32[32][32];
	pel orgY_32_1[32][32];
	pel orgY_32_2[32][32];
	for (int i = 0; i < cu_height; i++){
#pragma HLS LOOP_TRIPCOUNT max=32
		for (int j = 0; j < cu_width; j++){
#pragma HLS LOOP_TRIPCOUNT max=32
			orgY[(i << 5) + j] = p_fenc_LCU_Y[(mod_info_curr_ptr->y_pos - md_input_ptr->pix_y + i) * 64 + (mod_info_curr_ptr->x_pos - md_input_ptr->pix_x + j)];
	    orgY_32[i][j] = orgY[(i<<5) + j];
    }
  }
#endif
	s64 cost_best_temp = MAX_S64;
	s64 dist_pred = 0;
	s64 lambda_y = md_input_ptr->lambda1;
	U13 pic_height = md_fw_ptr->pic_height_in_scu << 2;
	U13 pic_width = md_fw_ptr->pic_width_in_scu << 2;
	U2 slice_type = md_fw_ptr->slice_type;



	u8 ipred_list[5];
	int pred_cnt = 5;
	s16 mvp_all[2][5][2];
	pel src_le_temp[2 * 32 + 3];//SIZE_32
#pragma HLS ARRAY_PARTITION variable=src_le_temp complete dim=0
	pel src_up_temp[2 * 32 + 3];
#pragma HLS ARRAY_PARTITION variable=src_up_temp complete dim=0
	//pel src_le_temp[128 + 3];
	//pel src_up_temp[128 + 3];
	u16 avail_tb;
	s64 cost_pb_best = MAX_S64;

	u8 rdo_list[5] = { 0 };
	u8 inter_rdo_num = 0;

	u8 intra_rdo_num = 0;

	SKIP_ELEMENT skip_mode[SKIP_MODE_NUM];
	SKIP_RDO skip_rdo;
	SKIP_ELEMENT skip_rdo_mode[MAX_INTER_SKIP_RDO];
	S14 sub_pel_mv[MV_D];
	MERGE_RDO merge_rdo;
	s8 inter_mode_idx;
	u8 rdo_idx;
	U1 is_intra_quant, is_intra_rate, is_intra_update;

	md_kernel_32_hls::rdo_prepare_32(model_cnt, md_fw_ptr, me_line_map_ptr, inner_nebs, above_nebs_line, skip_in, mod_info_curr_ptr,
		&merge_rdo, md_input_ptr, valid_mode, amvr_enable_flag, fme_mv_info,
		IP_buffer_ptr, MPM_buffer_ptr, MAP_SCU_IP_ptr,ipred_list, src_le_temp, src_up_temp, &avail_tb, rmd_mode,
		17,RefWindowLCU, CtrPos,curr_cons_pred_mode,&inter_rdo_num, &intra_rdo_num, rdo_list, skip_mode, &skip_rdo,
		skip_rdo_mode, sub_pel_mv, mvp_all, bit_depth, Fetch_Ref_window_ptr);


	//valid_flag = 1;
	//is_intra_quant = 1;

	rdo_luma_32_32(orgY_32, inter_mode_idx, ipred_list, src_le_temp,
			src_up_temp, avail_tb, rdo_list, bit_depth, lambda_y, tree_status,
			curr_cons_pred_mode, mod_info_curr_ptr,
			model_cnt, rdoq_array, md_input_ptr, md_kernel_input_32,
			cu_data_temp_ptr, best_info_ptr,&cost_best);

  //printf("%d\n",cost_best);
#if 0
  if(cu_width == 32 && cu_height == 32){
    if(md_kernel_32_file_out == NULL)
      md_kernel_32_file_out = fopen("md_kernel_32_file_out_test.dat","w");
	  
      //fwrite(&isNS_allow,sizeof(U1),1,md_kernel_32_file_out);
      //fwrite(&w,sizeof(U8),1,md_kernel_32_file_out);
      //fwrite(&h,sizeof(U8),1,md_kernel_32_file_out);
      //fwrite( model_cnt->cbf ,sizeof(U32),1*3,md_kernel_32_file_out);
      //fwrite( model_cnt->run_rdoq ,sizeof(U32),1*24,md_kernel_32_file_out);
      //fwrite( model_cnt->level ,sizeof(U32),1*24,md_kernel_32_file_out);
      //fwrite( model_cnt->last1 ,sizeof(U32),1*12,md_kernel_32_file_out);
      //fwrite( model_cnt->last2 ,sizeof(U32),1*22,md_kernel_32_file_out);
      //fwrite( rdoq_array->rdoq_est_run ,sizeof(s32),1*24*2,md_kernel_32_file_out);
      //fwrite( rdoq_array->rdoq_est_cbf ,sizeof(int),1*3*2,md_kernel_32_file_out);
      //fwrite( rdoq_array->rdoq_est_level ,sizeof(s32),1*24*2,md_kernel_32_file_out);
      //fwrite( rdoq_array->rdoq_est_last ,sizeof(s32),1*2*6*12*2,md_kernel_32_file_out);
      //fwrite(&md_fw_ptr->slice_type,sizeof(U2),1,md_kernel_32_file_out);
      //fwrite(&md_fw_ptr->pic_width_in_scu,sizeof(U11),1,md_kernel_32_file_out);
      //fwrite(&md_fw_ptr->pic_height_in_scu,sizeof(U11),1,md_kernel_32_file_out);
      //fwrite(&md_fw_ptr->bit_depth_internal,sizeof(U4),1,md_kernel_32_file_out);
      //fwrite( md_fw_ptr->refp_ptr ,sizeof(int),1*17*2,md_kernel_32_file_out);
      //fwrite(&md_fw_ptr->num_refp,sizeof(U5),1,md_kernel_32_file_out);
      //fwrite(&md_fw_ptr->cur_ptr,sizeof(U9),1,md_kernel_32_file_out);
      //fwrite( me_line_map_ptr->new_map_scu_above_line ,sizeof(U32),1*ABOVE_LINE_LEN,md_kernel_32_file_out);
      //fwrite( me_line_map_ptr->new_map_cu_mode_above_line ,sizeof(U32),1*ABOVE_LINE_LEN,md_kernel_32_file_out);
      //fwrite(&me_line_map_ptr->min_scu_x,sizeof(U11),1,md_kernel_32_file_out);
      //fwrite(&me_line_map_ptr->min_scu_y,sizeof(U11),1,md_kernel_32_file_out);
      //fwrite(&me_line_map_ptr->curr_ctu_idx,sizeof(U1),1,md_kernel_32_file_out);
      //fwrite( me_line_map_ptr->map_scu_inner ,sizeof(U32),1*2*SCU_IN_LCU_NUMS_LEN*SCU_IN_LCU_NUMS_LEN,md_kernel_32_file_out);
      //fwrite( me_line_map_ptr->map_cu_mode_inner ,sizeof(U32),1*2*SCU_IN_LCU_NUMS_LEN*SCU_IN_LCU_NUMS_LEN,md_kernel_32_file_out);
      //fwrite( skip_in->refp_map_mv ,sizeof(S16),1*MAX_CU_IN_SCU*MAX_CU_IN_SCU*MV_D,md_kernel_32_file_out);
      //fwrite( skip_in->refp_map_refi ,sizeof(S2),1*MAX_CU_IN_SCU*MAX_CU_IN_SCU,md_kernel_32_file_out);
      //fwrite(&skip_in->pix_x,sizeof(U13),1,md_kernel_32_file_out);
      //fwrite(&tree_status,sizeof(U2),1,md_kernel_32_file_out);
      //fwrite(&best_info_ptr->cu_mode,sizeof(U2),1,md_kernel_32_file_out);
      //fwrite(&best_info_ptr->pb_part,sizeof(int),1,md_kernel_32_file_out);
      //fwrite(&best_info_ptr->tb_part,sizeof(int),1,md_kernel_32_file_out);
      //fwrite( best_info_ptr->num_nz ,sizeof(int),1*MAX_NUM_TB*N_C,md_kernel_32_file_out);

      //fwrite( best_info_ptr->refi ,sizeof(S3),1*REFP_NUM,md_kernel_32_file_out);
      //fwrite(&best_info_ptr->mvr_idx,sizeof(U8),1,md_kernel_32_file_out);
      //fwrite(&best_info_ptr->umve_flag,sizeof(U8),1,md_kernel_32_file_out);
      //fwrite(&best_info_ptr->umve_idx,sizeof(U8),1,md_kernel_32_file_out);
      //fwrite(&best_info_ptr->skip_idx,sizeof(U8),1,md_kernel_32_file_out);
      //fwrite(&best_info_ptr->mvp_from_hmvp_flag,sizeof(U8),1,md_kernel_32_file_out);
      //fwrite( best_info_ptr->mvd ,sizeof(s16),1*REFP_NUM*MV_D,md_kernel_32_file_out);
      //fwrite( best_info_ptr->mv ,sizeof(S14),1*REFP_NUM*MV_D,md_kernel_32_file_out);
      //fwrite(&best_info_ptr->affine_flag,sizeof(U8),1,md_kernel_32_file_out);
      //fwrite( best_info_ptr->affine_mv ,sizeof(int),1*REFP_NUM*VER_NUM*MV_D,md_kernel_32_file_out);
      //fwrite( best_info_ptr->affine_mvd ,sizeof(s16),1*REFP_NUM*VER_NUM*MV_D,md_kernel_32_file_out);
      //fwrite(&best_info_ptr->smvd_flag,sizeof(U8),1,md_kernel_32_file_out);
      //fwrite( best_info_ptr->mpm ,sizeof(U8),1*MAX_NUM_PB*2,md_kernel_32_file_out);
      //fwrite( best_info_ptr->ipm ,sizeof(S8),1*MAX_NUM_PB*2,md_kernel_32_file_out);
      //fwrite(&md_input_ptr->pix_x,sizeof(U13),1,md_kernel_32_file_out);
      //fwrite(&md_input_ptr->pix_y,sizeof(U13),1,md_kernel_32_file_out);
      //fwrite( md_input_ptr->lambda ,sizeof(s64),1*3,md_kernel_32_file_out);
      //fwrite(&md_input_ptr->qp_y,sizeof(U6),1,md_kernel_32_file_out);
      //fwrite(p_fenc_LCU_Y,sizeof(pel),1*MAX_CU_SIZE_FETCH*MAX_CU_SIZE_FETCH,md_kernel_32_file_out);
      //fwrite(RefWindowLCU,sizeof(pel),1*SWW*SWH,md_kernel_32_file_out);
      //fwrite(CtrPos,sizeof(S14),1*RANGE_NUM*DIR_DIM*MAP_DIMS,md_kernel_32_file_out);
      //fwrite(&mod_info_curr_ptr->x_scu,sizeof(U11),1,md_kernel_32_file_out);
      //fwrite(&mod_info_curr_ptr->y_scu,sizeof(U11),1,md_kernel_32_file_out);
      //fwrite(&mod_info_curr_ptr->cu_width,sizeof(U7),1,md_kernel_32_file_out);
      //fwrite(&mod_info_curr_ptr->cu_height,sizeof(U7),1,md_kernel_32_file_out);
      //fwrite(&mod_info_curr_ptr->cu_width_log2,sizeof(U3),1,md_kernel_32_file_out);
      //fwrite(&mod_info_curr_ptr->cu_height_log2,sizeof(U3),1,md_kernel_32_file_out);
      //fwrite(&mod_info_curr_ptr->x_pos,sizeof(U13),1,md_kernel_32_file_out);
      //fwrite(&mod_info_curr_ptr->y_pos,sizeof(U13),1,md_kernel_32_file_out);
      //fwrite(&mod_info_curr_ptr->mvp_from_hmvp_flag,sizeof(U8),1,md_kernel_32_file_out);
      //fwrite(&mod_info_curr_ptr->affine_flag,sizeof(U8),1,md_kernel_32_file_out);
      //fwrite( mod_info_curr_ptr->mpm ,sizeof(U8),1*2,md_kernel_32_file_out);
      //fwrite(&mod_info_curr_ptr->chroma_motion,sizeof(U1),1,md_kernel_32_file_out);
      //fwrite( cu_data_temp_ptr->coef_y_t ,sizeof(S16),1*CU_SIZE*16,md_kernel_32_file_out);
      //fwrite( cu_data_temp_ptr->reco_y_t ,sizeof(S16),1*CU_SIZE*16,md_kernel_32_file_out);
      //fwrite(&curr_cons_pred_mode,sizeof(U2),1,md_kernel_32_file_out);
      //fwrite( fme_mv_info->sadTree32x32Fmv ,sizeof(S14),1*2*2*2,md_kernel_32_file_out);
      //fwrite( fme_mv_info->sadTree32x16Fmv ,sizeof(S14),1*2*4*2,md_kernel_32_file_out);
      //fwrite( fme_mv_info->sadTree16x32Fmv ,sizeof(S14),1*4*2*2,md_kernel_32_file_out);
      //fwrite(rmd_mode,sizeof(u8),1*IPD_RDO_CNT,md_kernel_32_file_out);
      //fwrite( IP_buffer_ptr->up_innerLB_size ,sizeof(pel),1*23*16*64,md_kernel_32_file_out);
      //fwrite( IP_buffer_ptr->left_innerLB_size ,sizeof(pel),1*23*64*16,md_kernel_32_file_out);
      //fwrite( IP_buffer_ptr->up_temp_size ,sizeof(pel),1*23*128,md_kernel_32_file_out);
      //fwrite( IP_buffer_ptr->left_size ,sizeof(pel),1*23*64,md_kernel_32_file_out);
      //fwrite(&IP_buffer_ptr->top_left,sizeof(pel),1,md_kernel_32_file_out);
      //fwrite( MPM_buffer_ptr->buffer ,sizeof(pel),1*16*16,md_kernel_32_file_out);
      //fwrite( MPM_buffer_ptr->up ,sizeof(pel),1*960,md_kernel_32_file_out);
      //fwrite( MPM_buffer_ptr->left_buffer ,sizeof(pel),1*16,md_kernel_32_file_out);
      //fwrite(&MAP_SCU_IP_ptr->min_scu_x,sizeof(U10),1,md_kernel_32_file_out);
      //fwrite(&MAP_SCU_IP_ptr->min_scu_y,sizeof(U10),1,md_kernel_32_file_out);
      //fwrite(&MAP_SCU_IP_ptr->curr_ctu_idx,sizeof(U1),1,md_kernel_32_file_out);
      //fwrite(&MAP_SCU_IP_ptr->above_line_idx,sizeof(U1),1,md_kernel_32_file_out);
      //fwrite( MAP_SCU_IP_ptr->map_scu_inner ,sizeof(U32),1*2*SCU_IN_LCU_NUMS_LEN*SCU_IN_LCU_NUMS_LEN,md_kernel_32_file_out);
      //fwrite( MAP_SCU_IP_ptr->map_scu_above_line ,sizeof(U32),1*2*960,md_kernel_32_file_out);
      //fwrite(&md_kernel_input_32->x_pos,sizeof(U13),1,md_kernel_32_file_out);
      //fwrite(&md_kernel_input_32->y_pos,sizeof(U13),1,md_kernel_32_file_out);
      //fwrite(&md_kernel_input_32->x_chroma,sizeof(U12),1,md_kernel_32_file_out);
      //fwrite(&md_kernel_input_32->y_chroma,sizeof(U12),1,md_kernel_32_file_out);
      //fwrite(&md_kernel_input_32->chroma_motion,sizeof(U1),1,md_kernel_32_file_out);
      //fwrite(&md_kernel_input_32->cu_width,sizeof(U7),1,md_kernel_32_file_out);
      //fwrite(&md_kernel_input_32->cu_height,sizeof(U7),1,md_kernel_32_file_out);
      //fwrite(&md_kernel_input_32->cu_width_log2,sizeof(U3),1,md_kernel_32_file_out);
      //fwrite(&md_kernel_input_32->cu_height_log2,sizeof(U3),1,md_kernel_32_file_out);
      //fwrite(refwin_md,sizeof(pel),1*88*88,md_kernel_32_file_out);
      //fwrite(CtrPosFME,sizeof(S14),1*RANGE_NUM*DIR_DIM*MAP_DIMS,md_kernel_32_file_out);

      //printf("x = %d y = %d cost_best = %d \n",md_input_ptr->pix_x,md_input_ptr->pix_y,cost_best);
      fflush(md_kernel_32_file_out);
      exit(0) ;
  }
#endif

  	if (!isNS_allow)
  		return 0;
  	else
  		return cost_best;
}



