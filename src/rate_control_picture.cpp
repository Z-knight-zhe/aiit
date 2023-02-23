/*------------------------------------------------------------------------------
--                                                                                                                               --
--       This software is confidential and proprietary and may be used                                   --
--        only as expressly authorized by a licensing agreement from                                     --
--                                                                                                                               --
--                            Verisilicon.                                                                                    --
--                                                                                                                               --
--                   (C) COPYRIGHT 2014 VERISILICON                                                            --
--                            ALL RIGHTS RESERVED                                                                    --
--                                                                                                                               --
--                 The entire notice above must be reproduced                                                 --
--                  on all copies and should not be removed.                                                    --
--                                                                                                                               --
--------------------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include "base_type.h"
#include "rate_control_picture.h"
//#include "tools.h"
//#include "sw_slice.h"
#include <math.h>
//#include "enccfg.h"
//#include "sw_cu_tree.h"

/*
 * Using only one leaky bucket (Multible buckets is supported by std).
 * Constant bit rate (CBR) operation, ie. leaky bucket drain rate equals
 * average rate of the stream, is enabled if RC_CBR_HRD = 1. Frame skiping and
 * filler data are minimum requirements for the CBR conformance.
 *
 * Constant HRD parameters:
 *   low_delay_hrd_flag = 0, assumes constant delay mode.
 *       cpb_cnt_minus1 = 0, only one leaky bucket used.
 *         (cbr_flag[0] = RC_CBR_HRD, CBR mode.)
 */

/*------------------------------------------------------------------------------
  Module defines
------------------------------------------------------------------------------*/

/* Define this if strict bitrate control is needed, each window has a strict
 * bit budget. Otherwise a moving window is used for smoother quality.
*/
//#define RC_WINDOW_STRICT
#define TRACE_RC
#ifdef TRACE_RC
#include <stdio.h>
FILE *fpRcTrc = NULL;
/* Select debug output: fpRcTrc or stdout */
#define DBGOUTPUT fpRcTrc
/* Select debug level: 0 = minimum, 2 = maximum */
#define DBG_LEVEL 2
#define DBG(l, str) if (l <= DBG_LEVEL) fprintf str
#else
#define DBG(l, str)
#endif

#define INITIAL_BUFFER_FULLNESS   80    /* Decoder Buffer in procents */
#define MIN_PIC_SIZE              50    /* Procents from picPerPic */

#define DIV(a, b)       (((b)!=0) ? ((a) + (SIGN(a) * (b)) / 2) / (b) : (a))
#define DSCY                      32 /* n * 32 */
#define DSCBITPERMB               128 //128 /* bitPerMb scaler  */
#define I32_MIN           (-1 << 31)
#define I64_MAX           9223372036854775807 /* 2 ^ 63 - 1 */
//#define QP_DELTA          2.0     /* Limit QP change between consecutive frames.*/
#define QP_DELTA          0x200     /* Limit QP change between consecutive frames.*/

//#define QP_DELTA_LIMIT    6.0     /* Threshold when above limit is abandoned. */
#define QP_DELTA_LIMIT    0x600     /* Threshold when above limit is abandoned. */



#define QP_FIXPOINT_0_POINT_1  0x1a 


#define INTRA_QP_DELTA    (0)
#define WORD_CNT_MAX      65535

#define CLIP3(x, y, z)  ((z) < (x) ? (x) : ((z) > (y) ? (y) : (z)))
#define ABS(x)          ((x) < (0) ? -(x) : (x))
#define MAX(a, b)       ((a) > (b) ?  (a) : (b))
#define MIN(a, b)       ((a) < (b) ?  (a) : (b))
#define SIGN(a)         ((a) < (0) ? (-1) : (1))

#if defined(_MSC_VER)
#define X265_LOG2F(x) (logf((float)(x)) * 1.44269504088896405f)
#define X265_LOG2(x) (log((double)(x)) * 1.4426950408889640513713538072172)
#else
#define X265_LOG2F(x) log2f(x)
#define X265_LOG2(x)  log2(x)
#endif
#define HANTRO_FALSE 0
#define HANTRO_TRUE  1

void *EWLmemset(void *d, i32 c, u32 n)
{
    return memset(d, (int)c, (size_t)n);
}
/*------------------------------------------------------------------------------
  Local structures
------------------------------------------------------------------------------*/
#if 0
/* q_step values scaled up by 4 and evenly rounded */
static const i32 q_step[53] = { 3, 3, 3, 4, 4, 5, 5, 6, 7, 7, 8, 9, 10, 11,
                                13, 14, 16, 18, 20, 23, 25, 28, 32, 36, 40, 45, 51, 57, 64, 72, 80, 90,
                                101, 114, 128, 144, 160, 180, 203, 228, 256, 288, 320, 360, 405, 456,
                                513, 577, 640, 720, 810, 896
                              };
#else
#if 0
static const i32 q_step[53] = { 2, 3, 3, 4, 4, 4, 5, 6, 6, 7, 8, 9, 10, 11,
                                13, 14, 16, 18, 20, 22, 25, 28, 31, 35, 39, 44, 50, 56, 63, 71, 79, 88,
                                100, 112, 126, 141, 157, 177, 200, 224, 252, 283, 314, 354, 401, 448,
                                503, 566, 629, 707, 802, 896
                              };
#else
#if 0
static const float q_step[511] ={
  2.475138122,2.503897841,2.532991733,2.562423679,2.592197608,2.622317493,2.652787355,2.683611259,2.71479332 ,2.746337699,
2.778248606,2.8105303  ,2.843187089,2.876223331,2.909643437,2.943451865,2.977653128,3.012251791,3.047252471,3.082659839,
3.118478621,3.154713597,3.191369603,3.228451532,3.265964332,3.303913009,3.342302629,3.381138315,3.42042525 ,3.460168677,
3.5003739  ,3.541046286,3.582191262,3.623814319,3.665921013,3.708516964,3.751607855,3.795199439,3.839297532,3.883908021,
3.929036858,3.974690067,4.020873741,4.067594043,4.11485721 ,4.162669547,4.211037438,4.259967336,4.309465772,4.359539352,
4.410194759,4.461438754,4.513278175,4.565719942,4.618771052,4.672438587,4.726729708,4.781651661,4.837211777,4.893417471,
4.950276243,5.007795682,5.065983465,5.124847358,5.184395216,5.244634986,5.30557471 ,5.367222518,5.42958664 ,5.492675398,
5.556497212,5.621060599,5.686374177,5.752446663,5.819286873,5.88690373 ,5.955306256,6.024503581,6.094504941,6.165319678,
6.236957241,6.309427194,6.382739206,6.456903064,6.531928663,6.607826018,6.684605258,6.76227663 ,6.8408505  ,6.920337354,
7.0007478  ,7.082092572,7.164382523,7.247628638,7.331842027,7.417033928,7.50321571 ,7.590398877,7.678595064,7.767816041,
7.858073716,7.949380134,8.041747482,8.135188087,8.229714419,8.325339095,8.422074875,8.519934672,8.618931544,8.719078704,
8.820389519,8.922877508,9.026556351,9.131439883,9.237542104,9.344877173,9.453459416,9.563303323,9.674423555,9.786834942,
9.900552486,10.01559136,10.13196693,10.24969472,10.36879043,10.48926997,10.61114942,10.73444504,10.85917328,10.9853508 ,
11.11299442,11.2421212 ,11.37274835,11.50489333,11.63857375,11.77380746,11.91061251,12.04900716,12.18900988,12.33063936,
12.47391448,12.61885439,12.76547841,12.91380613,13.06385733,13.21565204,13.36921052,13.52455326,13.681701  ,13.84067471,
14.0014956 ,14.16418514,14.32876505,14.49525728,14.66368405,14.83406786,15.00643142,15.18079775,15.35719013,15.53563208,
15.71614743,15.89876027,16.08349496,16.27037617,16.45942884,16.65067819,16.84414975,17.03986934,17.23786309,17.43815741,
17.64077904,17.84575502,18.0531127 ,18.26287977,18.47508421,18.68975435,18.90691883,19.12660665,19.34884711,19.57366988,
19.80110497,20.03118273,20.26393386,20.49938943,20.73758086,20.97853995,21.22229884,21.46889007,21.71834656,21.97070159,
22.22598885,22.4842424 ,22.74549671,23.00978665,23.27714749,23.54761492,23.82122502,24.09801433,24.37801976,24.66127871,
24.94782897,25.23770878,25.53095683,25.82761225,26.12771465,26.43130407,26.73842103,27.04910652,27.363402  ,27.68134941,
28.0029912 ,28.32837029,28.65753009,28.99051455,29.32736811,29.66813571,30.01286284,30.36159551,30.71438026,31.07126416,
31.43229486,31.79752054,32.16698993,32.54075235,32.91885768,33.30135638,33.6882995 ,34.07973869,34.47572618,34.87631482,
35.28155808,35.69151003,36.1062254 ,36.52575953,36.95016842,37.37950869,37.81383766,38.25321329,38.69769422,39.14733977,
39.60220994,40.06236546,40.52786772,40.99877886,41.47516173,41.95707989,42.44459768,42.93778015,43.43669312,43.94140318,
44.45197769,44.96848479,45.49099342,46.0195733 ,46.55429498,47.09522984,47.64245005,48.19602865,48.75603953,49.32255742,
49.89565793,50.47541755,51.06191365,51.65522451,52.25542931,52.86260815,53.47684207,54.09821304,54.726804  ,55.36269883,
56.0059824 ,56.65674057,57.31506019,57.98102911,58.65473621,59.33627142,60.02572568,60.72319102,61.42876051,62.14252833,
62.86458973,63.59504107,64.33397986,65.0815047 ,65.83771535,66.60271276,67.376599  ,68.15947737,68.95145235,69.75262963,
70.56311615,71.38302007,72.21245081,73.05151907,73.90033683,74.75901738,75.62767532,76.50642658,77.39538844,78.29467953,
79.20441989,80.12473092,81.05573545,81.99755772,82.95032345,83.91415978,84.88919535,85.87556029,86.87338624,87.88280636,
88.90395538,89.93696959,90.98198684,92.0391466 ,93.10858997,94.19045967,95.2849001 ,96.3920573 ,97.51207906,98.64511484,
99.79131586,100.9508351,102.1238273,103.310449 ,104.5108586,105.7252163,106.9536841,108.1964261,109.453608 ,110.7253977,
112.0119648,113.3134811,114.6301204,115.9620582,117.3094724,118.6725428,120.0514514,121.446382 ,122.857521 ,124.2850567,
125.7291795,127.1900821,128.6679597,130.1630094,131.6754307,133.2054255,134.753198 ,136.3189547,137.9029047,139.5052593,
141.1262323,142.7660401,144.4249016,146.1030381,147.8006737,149.5180348,151.2553506,153.0128532,154.7907769,156.5893591,
158.4088398,160.2494618,162.1114709,163.9951154,165.9006469,167.8283196,169.7783907,171.7511206,173.7467725,175.7656127,
177.8079108,179.8739392,181.9639737,184.0782932,186.2171799,188.3809193,190.5698002,192.7841146,195.0241581,197.2902297,
199.5826317,201.9016702,204.2476546,206.620898 ,209.0217172,211.4504326,213.9073683,216.3928522,218.907216 ,221.4507953,
224.0239296,226.6269623,229.2602408,231.9241164,234.6189449,237.3450857,240.1029027,242.8927641,245.7150421,248.5701133,
251.4583589,254.3801643,257.3359194,260.3260188,263.3508614,266.410851 ,269.506396 ,272.6379095,275.8058094,279.0105185,
282.2524646,285.5320803,288.8498032,292.2060763,295.6013473,299.0360695,302.5107013,306.0257063,309.5815538,313.1787181,
316.8176796,320.4989237,324.2229418,327.9902309,331.8012938,335.6566391,339.5567814,343.5022412,347.4935449,351.5312254,
355.6158215,359.7478783,363.9279473,368.1565864,372.4343599,376.7618387,381.1396004,385.5682292,390.0483162,394.5804594,
399.1652635,403.8033404,408.4953092,413.2417961,418.0434344,422.9008652,427.8147365,432.7857043,437.814432 ,442.9015906,
448.0478592,453.2539246,458.5204815,463.8482329,469.2378897,474.6901714,480.2058055,485.7855282,491.4300841,497.1402266,
502.9167178,508.7603286,514.6718389,520.6520376,526.7017228,532.8217021,539.012792 ,545.275819 ,551.6116188,558.0210371,
564.5049292,571.0641605,577.6996065,584.4121525,591.2026947,598.0721391,605.0214026,612.0514127,619.1631075,626.3574363,
633.6353591,640.9978473,648.4458836,655.9804618,663.6025876,671.3132783,679.1135628,687.0044823,694.9870899,703.0624509,
711.2316431,719.4957567,727.8558947,736.3131728,744.8687198,753.5236774,762.2792008,771.1364584,780.0966325,789.1609187,
798.3305269,807.6066808,816.9906184,826.4835921,836.0868689,845.8017304,855.6294731,865.5714087,875.628864 ,885.8031813,
896.0957185

};
#else
static const i32 q_step[511] ={

0x279  ,0x280  ,0x288  ,0x28f  ,0x297  ,0x29f  ,0x2a7  ,0x2af  ,0x2b6  ,0x2bf  ,
0x2c7  ,0x2cf  ,0x2d7  ,0x2e0  ,0x2e8  ,0x2f1  ,0x2fa  ,0x303  ,0x30c  ,0x315  ,
0x31e  ,0x327  ,0x330  ,0x33a  ,0x344  ,0x34d  ,0x357  ,0x361  ,0x36b  ,0x375  ,
0x380  ,0x38a  ,0x395  ,0x39f  ,0x3aa  ,0x3b5  ,0x3c0  ,0x3cb  ,0x3d6  ,0x3e2  ,
0x3ed  ,0x3f9  ,0x405  ,0x411  ,0x41d  ,0x429  ,0x436  ,0x442  ,0x44f  ,0x45c  ,
0x469  ,0x476  ,0x483  ,0x490  ,0x49e  ,0x4ac  ,0x4ba  ,0x4c8  ,0x4d6  ,0x4e4  ,
0x4f3  ,0x501  ,0x510  ,0x51f  ,0x52f  ,0x53e  ,0x54e  ,0x55e  ,0x56d  ,0x57e  ,
0x58e  ,0x59e  ,0x5af  ,0x5c0  ,0x5d1  ,0x5e3  ,0x5f4  ,0x606  ,0x618  ,0x62a  ,
0x63c  ,0x64f  ,0x661  ,0x674  ,0x688  ,0x69b  ,0x6af  ,0x6c3  ,0x6d7  ,0x6eb  ,
0x700  ,0x715  ,0x72a  ,0x73f  ,0x754  ,0x76a  ,0x780  ,0x797  ,0x7ad  ,0x7c4  ,
0x7db  ,0x7f3  ,0x80a  ,0x822  ,0x83a  ,0x853  ,0x86c  ,0x885  ,0x89e  ,0x8b8  ,
0x8d2  ,0x8ec  ,0x906  ,0x921  ,0x93c  ,0x958  ,0x974  ,0x990  ,0x9ac  ,0x9c9  ,
0x9e6  ,0xa03  ,0xa21  ,0xa3f  ,0xa5e  ,0xa7d  ,0xa9c  ,0xabc  ,0xadb  ,0xafc  ,
0xb1c  ,0xb3d  ,0xb5f  ,0xb81  ,0xba3  ,0xbc6  ,0xbe9  ,0xc0c  ,0xc30  ,0xc54  ,
0xc79  ,0xc9e  ,0xcc3  ,0xce9  ,0xd10  ,0xd37  ,0xd5e  ,0xd86  ,0xdae  ,0xdd7  ,
0xe00  ,0xe2a  ,0xe54  ,0xe7e  ,0xea9  ,0xed5  ,0xf01  ,0xf2e  ,0xf5b  ,0xf89  ,
0xfb7  ,0xfe6  ,0x1015 ,0x1045 ,0x1075 ,0x10a6 ,0x10d8 ,0x110a ,0x113c ,0x1170 ,
0x11a4 ,0x11d8 ,0x120d ,0x1243 ,0x1279 ,0x12b0 ,0x12e8 ,0x1320 ,0x1359 ,0x1392 ,
0x13cd ,0x1407 ,0x1443 ,0x147f ,0x14bc ,0x14fa ,0x1538 ,0x1578 ,0x15b7 ,0x15f8 ,
0x1639 ,0x167b ,0x16be ,0x1702 ,0x1746 ,0x178c ,0x17d2 ,0x1819 ,0x1860 ,0x18a9 ,
0x18f2 ,0x193c ,0x1987 ,0x19d3 ,0x1a20 ,0x1a6e ,0x1abd ,0x1b0c ,0x1b5d ,0x1bae ,
0x1c00 ,0x1c54 ,0x1ca8 ,0x1cfd ,0x1d53 ,0x1dab ,0x1e03 ,0x1e5c ,0x1eb6 ,0x1f12 ,
0x1f6e ,0x1fcc ,0x202a ,0x208a ,0x20eb ,0x214d ,0x21b0 ,0x2214 ,0x2279 ,0x22e0 ,
0x2348 ,0x23b1 ,0x241b ,0x2486 ,0x24f3 ,0x2561 ,0x25d0 ,0x2640 ,0x26b2 ,0x2725 ,
0x279a ,0x280f ,0x2887 ,0x28ff ,0x2979 ,0x29f5 ,0x2a71 ,0x2af0 ,0x2b6f ,0x2bf0 ,
0x2c73 ,0x2cf7 ,0x2d7d ,0x2e05 ,0x2e8d ,0x2f18 ,0x2fa4 ,0x3032 ,0x30c1 ,0x3152 ,
0x31e5 ,0x3279 ,0x330f ,0x33a7 ,0x3441 ,0x34dc ,0x357a ,0x3619 ,0x36ba ,0x375c ,
0x3801 ,0x38a8 ,0x3950 ,0x39fb ,0x3aa7 ,0x3b56 ,0x3c06 ,0x3cb9 ,0x3d6d ,0x3e24 ,
0x3edd ,0x3f98 ,0x4055 ,0x4114 ,0x41d6 ,0x429a ,0x4360 ,0x4428 ,0x44f3 ,0x45c0 ,
0x4690 ,0x4762 ,0x4836 ,0x490d ,0x49e6 ,0x4ac2 ,0x4ba0 ,0x4c81 ,0x4d65 ,0x4e4b ,
0x4f34 ,0x501f ,0x510e ,0x51ff ,0x52f3 ,0x53ea ,0x54e3 ,0x55e0 ,0x56df ,0x57e1 ,
0x58e7 ,0x59ef ,0x5afb ,0x5c0a ,0x5d1b ,0x5e30 ,0x5f48 ,0x6064 ,0x6183 ,0x62a5 ,
0x63ca ,0x64f3 ,0x661f ,0x674f ,0x6882 ,0x69b9 ,0x6af4 ,0x6c32 ,0x6d74 ,0x6eb9 ,
0x7003 ,0x7150 ,0x72a1 ,0x73f6 ,0x754f ,0x76ac ,0x780d ,0x7972 ,0x7adb ,0x7c48 ,
0x7dba ,0x7f30 ,0x80aa ,0x8229 ,0x83ac ,0x8534 ,0x86c0 ,0x8851 ,0x89e7 ,0x8b81 ,
0x8d20 ,0x8ec4 ,0x906c ,0x921a ,0x93cc ,0x9584 ,0x9741 ,0x9903 ,0x9aca ,0x9c96 ,
0x9e68 ,0xa03f ,0xa21c ,0xa3fe ,0xa5e6 ,0xa7d4 ,0xa9c7 ,0xabc0 ,0xadbf ,0xafc3 ,
0xb1ce ,0xb3df ,0xb5f6 ,0xb814 ,0xba37 ,0xbc61 ,0xbe91 ,0xc0c8 ,0xc306 ,0xc54a ,
0xc795 ,0xc9e6 ,0xcc3f ,0xce9e ,0xd105 ,0xd373 ,0xd5e8 ,0xd864 ,0xdae8 ,0xdd73 ,
0xe006 ,0xe2a0 ,0xe542 ,0xe7ec ,0xea9e ,0xed58 ,0xf01a ,0xf2e4 ,0xf5b7 ,0xf891 ,
0xfb75 ,0xfe61 ,0x10155,0x10453,0x10759,0x10a69,0x10d81,0x110a3,0x113ce,0x11702,
0x11a40,0x11d88,0x120d9,0x12434,0x12799,0x12b09,0x12e82,0x13206,0x13594,0x1392d,
0x13cd1,0x1407f,0x14439,0x147fd,0x14bcd,0x14fa8,0x1538e,0x15780,0x15b7e,0x15f87,
0x1639d,0x167bf,0x16bed,0x17028,0x1746f,0x178c3,0x17d23,0x18191,0x1860c,0x18a94,
0x18f2a,0x193cd,0x1987e,0x19d3d,0x1a20b,0x1a6e6,0x1abd0,0x1b0c9,0x1b5d0,0x1bae6,
0x1c00c,0x1c541,0x1ca85,0x1cfd9,0x1d53c,0x1dab0,0x1e034,0x1e5c9,0x1eb6e,0x1f123,
0x1f6ea,0x1fcc2,0x202ab,0x208a6,0x20eb3,0x214d2,0x21b03,0x22146,0x2279c,0x22e05,
0x23481,0x23b10,0x241b3,0x24869,0x24f33,0x25612,0x25d05,0x2640d,0x26b29,0x2725b,
0x279a2,0x280ff,0x28872,0x28ffb,0x2979a,0x29f50,0x2a71d,0x2af01,0x2b6fc,0x2bf0f,
0x2c73b,0x2cf7e,0x2d7db,0x2e050,0x2e8de,0x2f186,0x2fa47,0x30322,0x30c18,0x31529,
0x31e54,0x3279b,0x330fd,0x33a7b,0x34416,0x34dcd,0x357a1,0x36192,0x36ba0,0x375cd,
0x38018
};
#endif
#endif
#endif
static const double reciprocal[] = {
  /* compute float reciprocal of rc->hierarchial_bit_allocation_total_weight, max (20*8=160)
   * for(int i = 0; i <= 160; i++)
   *   reciprocal[i] = 1.0/i;
   */
  0.000000000, 1.000000000, 0.500000000, 0.333333333, 0.250000000, 0.200000000, 0.166666667, 0.142857143, 0.125000000, 0.111111111, 
  0.100000000, 0.090909091, 0.083333333, 0.076923077, 0.071428571, 0.066666667, 0.062500000, 0.058823529, 0.055555556, 0.052631579, 
  0.050000000, 0.047619048, 0.045454545, 0.043478261, 0.041666667, 0.040000000, 0.038461538, 0.037037037, 0.035714286, 0.034482759, 
  0.033333333, 0.032258065, 0.031250000, 0.030303030, 0.029411765, 0.028571429, 0.027777778, 0.027027027, 0.026315789, 0.025641026, 
  0.025000000, 0.024390244, 0.023809524, 0.023255814, 0.022727273, 0.022222222, 0.021739130, 0.021276596, 0.020833333, 0.020408163, 
  0.020000000, 0.019607843, 0.019230769, 0.018867925, 0.018518519, 0.018181818, 0.017857143, 0.017543860, 0.017241379, 0.016949153, 
  0.016666667, 0.016393443, 0.016129032, 0.015873016, 0.015625000, 0.015384615, 0.015151515, 0.014925373, 0.014705882, 0.014492754, 
  0.014285714, 0.014084507, 0.013888889, 0.013698630, 0.013513514, 0.013333333, 0.013157895, 0.012987013, 0.012820513, 0.012658228, 
  0.012500000, 0.012345679, 0.012195122, 0.012048193, 0.011904762, 0.011764706, 0.011627907, 0.011494253, 0.011363636, 0.011235955, 
  0.011111111, 0.010989011, 0.010869565, 0.010752688, 0.010638298, 0.010526316, 0.010416667, 0.010309278, 0.010204082, 0.010101010, 
  0.010000000, 0.009900990, 0.009803922, 0.009708738, 0.009615385, 0.009523810, 0.009433962, 0.009345794, 0.009259259, 0.009174312, 
  0.009090909, 0.009009009, 0.008928571, 0.008849558, 0.008771930, 0.008695652, 0.008620690, 0.008547009, 0.008474576, 0.008403361, 
  0.008333333, 0.008264463, 0.008196721, 0.008130081, 0.008064516, 0.008000000, 0.007936508, 0.007874016, 0.007812500, 0.007751938, 
  0.007692308, 0.007633588, 0.007575758, 0.007518797, 0.007462687, 0.007407407, 0.007352941, 0.007299270, 0.007246377, 0.007194245, 
  0.007142857, 0.007092199, 0.007042254, 0.006993007, 0.006944444, 0.006896552, 0.006849315, 0.006802721, 0.006756757, 0.006711409, 
  0.006666667, 0.006622517, 0.006578947, 0.006535948, 0.006493506, 0.006451613, 0.006410256, 0.006369427, 0.006329114, 0.006289308, 
  0.006250000
};
/*------------------------------------------------------------------------------
  Local function prototypes
------------------------------------------------------------------------------*/

static i32 InitialQp(i32 bits, i32 pels);
static void MbQuant(vcencRateControl_s *rc);
static void LinearModel(vcencRateControl_s *rc);
static void AdaptiveModel(vcencRateControl_s *rc);
static void SourceParameter(vcencRateControl_s *rc, i32 nonZeroCnt);
static void PicSkip(vcencRateControl_s *rc);
static void PicQuantLimit(vcencRateControl_s *rc);
static i32 VirtualBuffer(rcVirtualBuffer_s *vb, i32 timeInc, true_e hrd);
static void PicQuant(vcencRateControl_s *rc);
static i32 avg_rc_error(linReg_s *p);
static i32 avg_rc_error_jpeg(linReg_s *p);
static void update_rc_error(linReg_s *p, i32 bits, i32 windowLength);
static i32 gop_avg_qp(vcencRateControl_s *rc);
static i32 new_pic_quant(linReg_s *p, vcencRateControl_s *rc, i32 bits, true_e useQpDeltaLimit,u32 complexity,i32 lastActualBits, i32 lastTargetBits, i32 currentTargetBits);
static i32 get_avg_bits(linReg_s *p, i32 n);
static void update_tables(linReg_s *p, i32 qp, i32 bits);
static void update_model(linReg_s *p);
static i64 lin_sy(i32 *qp, i32 *r, i32 n);
static i64 lin_sx(i32 *qp, i32 n);
static i64 lin_sxy(i32 *qp, i32 *r, i32 n);
static i64 lin_nsxx(i32 *qp, i32 n);
static void init_hierarchial_rc (vcencRateControl_s *rc);
static void MaAddFrame(rc_ma_s *ma, i32 frameSizeBits);
static i32 Ma(rc_ma_s *ma);
static i32 moving_totalBit_window_minus1(rc_ma_s *ma);
static void MaCharAddFrame(rc_ma_char *ma, u32 intraCu8Num, u32 skipCu8Num, u32 PBFrame4NRdCost);
static void MaChar(rc_ma_char *ma);
static i32 frame_moving_bits(rc_ma_s *ma, i32 maxBits, i32 winLen);
static void initFramePredictors(vcencRateControl_s *rc);
static double predictSize(rcPredictor *p, double q, double var);
static void updatePredictor(rcPredictor *p, double q, double var, double bits, i32 qp);
static double qScale2qp(double qScale);
static double getNormQp (i32 qp);
static i32 getPass2Qp(vcencRateControl_s *rc);

#define IS_CTBRC_FOR_BITRATE(x) ((x)&2)

/*------------------------------------------------------------------------------

  VCEncInitRc() Initialize rate control.

------------------------------------------------------------------------------*/
bool_e VCEncInitRc(vcencRateControl_s *rc, u32 newStream)
{
  rcVirtualBuffer_s *vb = &rc->virtualBuffer;
  i32 i;
  rc->codingType = ASIC_HEVC;
  rc->monitorFrames = MAX(LEAST_MONITOR_FRAME, rc->outRateNum / rc->outRateDenom);
  rc->picRc = ENCHW_YES;
  rc->picSkip = ENCHW_NO;
  rc->qpHdr = -1 << QP_FRACTIONAL_BITS;
  rc->ctbRcQpDeltaReverse = 0;
  
  rc->hrd = ENCHW_YES;
 
  rc->bitrateWindow = 150;
  rc->vbr = ENCHW_NO;
  rc->crf = -1;
  rc->pbOffset = 6.0 * 0.378512;
  rc->ipOffset = 6.0 * 0.485427;
  rc->intraQpDelta = INTRA_QPDELTA_DEFAULT << QP_FRACTIONAL_BITS;
  rc->fixedIntraQp = 0 << QP_FRACTIONAL_BITS;
  rc->tolMovingBitRate = 2000;
  rc->tolCtbRcInter = 0.0;
  rc->tolCtbRcIntra = -1.0;
  rc->longTermQpDelta = 0;
  rc->f_tolMovingBitRate = 2000.0;
  rc->smooth_psnr_in_gop = 0;
  rc->maxPicSizeI = MIN((((i64)rcCalculate(rc->virtualBuffer.bitRate, rc->outRateDenom, rc->outRateNum))*(1 + 20)), I32_MAX);
  rc->maxPicSizeP = MIN((((i64)rcCalculate(rc->virtualBuffer.bitRate, rc->outRateDenom, rc->outRateNum))*(1 + 20)), I32_MAX);
  rc->maxPicSizeB = MIN((((i64)rcCalculate(rc->virtualBuffer.bitRate, rc->outRateDenom, rc->outRateNum))*(1 + 20)), I32_MAX);
  rc->minPicSizeI = rcCalculate(rc->virtualBuffer.bitRate, rc->outRateDenom, rc->outRateNum) / (1 + 20);
  rc->minPicSizeP = rcCalculate(rc->virtualBuffer.bitRate, rc->outRateDenom, rc->outRateNum) / (1 + 20);
  rc->minPicSizeB = rcCalculate(rc->virtualBuffer.bitRate, rc->outRateDenom, rc->outRateNum) / (1 + 20);
  
  rc->rcQpDeltaRange = 10;
  rc->rcBaseMBComplexity = 15;
  rc->picQpDeltaMin = -2;
  rc->picQpDeltaMax = 3;
 
  

  // ctbRc for rate purpose
  
  rc->pass =  0;
 
#if defined(TRACE_RC) && (DBGOUTPUT == fpRcTrc)
    if (!fpRcTrc) fpRcTrc = fopen("rc.trc", "wt");
#endif

  if ((rc->qpMax > (51 << QP_FRACTIONAL_BITS)))
  {
   // return ENCHW_NOK;
  }
  //bitPerPic
  rc->bpp = (((i64)rcCalculate(vb->bitRate, rc->outRateDenom, rc->outRateNum))*100 + rc->picArea / 2)/rc->picArea;
  //printf("bpp=%d\n",rc->bpp);
  init_hierarchial_rc (rc);
  if(rc->hierarchial_bit_allocation_GOP_size>8)
    rc->hierarchial_bit_allocation_GOP_size=8;
  else if(rc->hierarchial_bit_allocation_GOP_size<=0)
    rc->hierarchial_bit_allocation_GOP_size=1;

  rc->hierarchial_bit_allocation_GOP_size_previous=rc->hierarchial_bit_allocation_GOP_size;
  rc->hierarchial_bit_allocation_total_weight=0;
  for(i=0;i<rc->hierarchial_bit_allocation_GOP_size;i++)
  {
    rc->hierarchial_bit_allocation_total_weight+=rc->hierarchial_bit_allocation_map[rc->hierarchial_bit_allocation_GOP_size-1][i];
  }
  
  //CTB_RC
  rc->seqStart = 1;
  
  
  /* QP -1: Initial QP estimation done by RC */
  if (rc->qpHdr == (-1 << QP_FRACTIONAL_BITS))
  {
    i32 tmp = rcCalculate(vb->bitRate, rc->outRateDenom, rc->outRateNum);
    rc->qpHdr = InitialQp(tmp, rc->picArea/*rc->ctbPerPic*rc->ctbSize*rc->ctbSize*/);
    PicQuantLimit(rc);
    rc->finiteQp = rc->qpHdr;
    if((rc->finiteQp)>=(18<<QP_FRACTIONAL_BITS))
    {
      rc->finiteQp -= (18<<QP_FRACTIONAL_BITS);
    }
    else
    {
      rc->finiteQp = 0;
    }
  }
  else
  {
    i32 tmp = rcCalculate(vb->bitRate, rc->outRateDenom, rc->outRateNum);
    rc->finiteQp = InitialQp(tmp, rc->picArea);
    if((rc->finiteQp)>=(18<<QP_FRACTIONAL_BITS))
    {
      rc->finiteQp -= (18<<QP_FRACTIONAL_BITS);
    }
    else
    {
      rc->finiteQp = 0;
    }
  }

  if ((rc->qpHdr > rc->qpMax) || (rc->qpHdr < rc->qpMin))
  {
    return ENCHW_NOK;
  }

  /* HRD needs frame RC and macroblock RC*/
  if (rc->hrd == ENCHW_YES)
  {
    rc->picRc = ENCHW_YES;
  }

  rc->coeffCntMax = rc->ctbPerPic * rc->ctbSize * rc->ctbSize * 3 / 2;
  rc->frameCoded = ENCHW_YES;
  rc->sliceTypeCur = I_SLICE;
  rc->sliceTypePrev = P_SLICE;

  rc->qpHdrPrev  = rc->qpHdr;
  rc->fixedQp    = rc->qpHdr;

  vb->bitPerPic = rcCalculate(vb->bitRate, rc->outRateDenom, rc->outRateNum);
  rc->qpCtrl.checkPointDistance = 0;
  rc->qpCtrl.checkPoints = 0;

  rc->u32PFrameByQPStep  = 0;
  rc->qpHdrPrevI   = rc->qpHdr;
  rc->u32PrevIBitCnt = 0;

  /* new rate control algorithm */
  /* API parameter is named bitrateWindow but the actual usage is rate controlling
   * window in frames. RC tries to match the target bitrate inside the
   * window. Each window can contain multiple GOPs and the RC adapts to the
   * intra rate by calculating intraInterval. */

  DBG(0, (DBGOUTPUT, "\nInitRc: picRc\t\t%i  hrd\t%i  picSkip\t%i\n",
          rc->picRc, rc->hrd, rc->picSkip));
  DBG(0, (DBGOUTPUT, "  mbRc\t\t\t%i  qpHdr\t%i  Min,Max\t%i,%i\n",
          rc->ctbRc, rc->qpHdr, rc->qpMin, rc->qpMax));
  DBG(0, (DBGOUTPUT, "  checkPointDistance\t%i\n",
          rc->qpCtrl.checkPointDistance));

  DBG(0, (DBGOUTPUT, "  CPBsize\t%i\n BitRate\t%i\n BitPerPic\t%i\n",
          vb->bufferSize, vb->bitRate, vb->bitPerPic));

  /* If changing QP between frames don't reset GOP RC.
   * Changing bitrate resets RC the same way as new stream. */
  if (!newStream) return ENCHW_OK;
  
  /* new rate control algorithm */
  //init error for P B B B
  for(i=0;i<4;i++)
  {
    update_rc_error(&rc->rError[i], 0x7fffffff, 0);
  }
  //init error for I
  update_rc_error(&rc->intraError, 0x7fffffff, 0);
  //init model for P B B B
  for(i=0;i<4;i++)
  {
    EWLmemset(&rc->linReg[i], 0, sizeof(linReg_s));
    rc->linReg[i].qs[0]   = q_step[510];
    rc->linReg[i].weight = 10;
    rc->linReg[i].qp_prev = rc->qpHdr;
    rc->linReg[i].frameBitCntLast = 0;
    rc->linReg[i].targetPicSizeLast = 0;
  }
  //init model for I
  EWLmemset(&rc->intra, 0, sizeof(linReg_s));
  rc->intra.qs[0]   = q_step[510];
  rc->intra.weight = 10;
  rc->intra.qp_prev = rc->qpHdr;
  rc->intra.frameBitCntLast = 0;
  rc->intra.targetPicSizeLast = 0;
    /* API parameter is named bitrateWindow but the actual usage is rate controlling
   * window in frames. RC tries to match the target bitrate inside the
   * window. Each window can contain multiple GOPs and the RC adapts to the
   * intra rate by calculating intraInterval. */
  rc->windowLen = rc->bitrateWindow;
  vb->windowRem = rc->bitrateWindow;
  rc->intraIntervalCtr = rc->intraInterval = rc->bitrateWindow;
  rc->gopMulti = rc->bitrateWindow*rc->outRateDenom/rc->outRateNum;
  if(rc->gopMulti<1)
    rc->gopMulti = 1;
  rc->targetPicSize = 0;
  rc->frameBitCnt = 0;
  rc->gopQpSum = 0;
  rc->gopQpDiv = 0;
  rc->gopPQpSum = 0;
  rc->gopPQpNum = 0;

  vb->picTimeInc      = 0;
  vb->realBitCnt      = 0;
  vb->virtualBitCnt   = 0;
  
  rc->ma.pos = rc->ma.count = 0;
  rc->ma.frameRateNumer = rc->outRateNum;
  rc->ma.frameRateDenom = rc->outRateDenom;
  if (rc->outRateDenom)
    rc->ma.length = MAX(LEAST_MONITOR_FRAME, MIN(rc->monitorFrames, RC_MOVING_AVERAGE_FRAMES));
  else
    rc->ma.length = RC_MOVING_AVERAGE_FRAMES;

  rc->ma_char.pos = rc->ma_char.count =0;
  rc->ma_char.length = MAX(LEAST_MONITOR_FRAME, MIN(rc->monitorFrames/2, RC_MOVING_AVERAGE_FRAMES));

  rc->virtualBuffer.maxBitRate = rcCalculate(rc->virtualBuffer.bitRate, 100 + rc->f_tolMovingBitRate, 100);
  rc->virtualBuffer.minBitRate = rcCalculate(rc->virtualBuffer.bitRate, 100, 100 + rc->f_tolMovingBitRate);

  if (rc->hrd)
  {
    vb->bucketFullness =
      rcCalculate(vb->bufferSize, INITIAL_BUFFER_FULLNESS, 100);
    rc->gDelaySum = rcCalculate(90000, vb->bufferSize, vb->bitRate);
    rc->gInitialDelay = rcCalculate(90000, vb->bucketFullness, vb->bitRate);
    rc->gInitialDoffs = rc->gDelaySum - rc->gInitialDelay;
    vb->bucketFullness = vb->bucketLevel =
                           vb->bufferSize - vb->bucketFullness;
#ifdef TRACE_RC
    rc->gBufferMin = vb->bufferSize;
    rc->gBufferMax = 0;
#endif
  

    DBG(1, (DBGOUTPUT, "\n InitialDelay\t%i\n Offset\t\t%i\n",
            rc->gInitialDelay, rc->gInitialDoffs));
    rc->gopLastBitCnt = vb->bitPerPic;
    rc->intraframeBitCnt = vb->bitPerPic;
    rc->minIQp     = rc->qpHdr-(8<<QP_FRACTIONAL_BITS);
    if(rc->minIQp<rc->qpMin)
      rc->minIQp=rc->qpMin;
    printf("rc->minIQp=%d\n",rc->minIQp);
  }

  rc->complexity = 12;
  rc->rcPicComplexity = rc->complexity * rc->ctbPerPic * (rc->ctbSize/8)*(rc->ctbSize/8);
  rc->resetIntraPicQp = 0;
  rc->inputSceneChange = 0;

  /* init ctbRc for bit rate mode */
  if (IS_CTBRC_FOR_BITRATE(rc->ctbRc))
  {
    ctbRateControl_s *pCtbRateCtrl = &(rc->ctbRateCtrl);    
    i32 maxQpStep = (1<<18) - 1;
    i32 maxRowFactor = (1<<16) - 1;

    for (i = 0; i < 3; i ++)
      pCtbRateCtrl->models[i].xMin = 2048;

    pCtbRateCtrl->rowFactor = ((1 << CTB_RC_ROW_FACTOR_FIXP) + rc->ctbCols/2) / rc->ctbCols;
    pCtbRateCtrl->qpStep = MIN(maxQpStep, pCtbRateCtrl->qpStep);
    pCtbRateCtrl->rowFactor = MIN(maxRowFactor, pCtbRateCtrl->rowFactor);
  }

  initFramePredictors(rc);

  return ENCHW_OK;
}
#define X265_TYPE_IDR           0x0001
#define X265_TYPE_I             0x0002
#define X265_TYPE_P             0x0003
#define X265_TYPE_BREF          0x0004  /* Non-disposable B-frame */
#define X265_TYPE_B             0x0005
#define X265_TYPE_BLDY          0x0006
i32 getFramePredId(i32 type)
{
    i32 idc = 0;
    switch (type)
    {
    case X265_TYPE_I:
    case 'I':
        idc = I_SLICE; break;

    case X265_TYPE_P:
    case X265_TYPE_BLDY:
    case 'P':
    case 'L':
        idc = P_SLICE; break;

    case X265_TYPE_B:
    case 'b':
        idc = B_SLICE; break;

    case X265_TYPE_BREF:
    case 'B':
        idc = 3; break;

    default: break;
    }
    return idc;
}
/*------------------------------------------------------------------------------

  InitialQp()  Returns sequence initial quantization parameter.

------------------------------------------------------------------------------*/
#define OFFSET_QP 0
static i32 InitialQp(i32 bits, i32 pels)
{
  const i32 qp_tbl[2][36] =
  {
    /*{27, 32, 36, 40, 44, 51, 58, 65, 72, 84, 96, 108, 119, 138, 156, 174, 192, 223, 253, 285, 314, 349, 384, 419, 453, 503, 553, 603, 653, 719, 784, 864, 0x7FFFFFFF},*/
    {16,19,23, 27, 32, 36, 40, 44, 51, 58, 65, 72, 84, 96, 108, 119, 138, 156, 174, 192, 223, 253, 285, 314, 349, 384, 419, 453, 503, 553, 603, 653, 719, 784, 864, 0x7FFFFFFF},
    /*{26, 38, 59, 96, 173, 305, 545, 0x7FFFFFFF},*/
    {51,50,49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16}
  };
  const i32 upscale = 20000;
  i32 i = -1;
  //avoid overflow
  i64 bits64=(i64)bits;
  
  /* prevents overflow, QP would anyway be 17 with this high bitrate
     for all resolutions under and including 1920x1088 */
  //    if (bits > 1000000)
  //      return 17;

  /* Make room for multiplication */
  pels >>= 8;
  bits64 >>= 5;

  /* Use maximum QP if bitrate way too low. */
  if (!bits64)
    return 51<<QP_FRACTIONAL_BITS;

  /* Adjust the bits value for the current resolution */
  bits64 *= pels + 250;
  ASSERT(pels > 0);
  ASSERT(bits64 > 0);
  bits64 /= 350 + (3 * pels) / 4;
  bits64 = rcCalculate((i32)bits64, upscale, pels << 6);

  while ((qp_tbl[0][++i]-OFFSET_QP) < bits64);

  DBG(0, (DBGOUTPUT, "BPP\t\t%lld\n", bits64));

  return qp_tbl[1][i]<<QP_FRACTIONAL_BITS;
}

/*------------------------------------------------------------------------------

      HevcFillerRc

      Stream watermarking. Insert filler NAL unit of certain size after each
      Nth frame.

------------------------------------------------------------------------------*/
u32 HevcFillerRc(vcencRateControl_s *rc, u32 frameCnt)
{
  const u8 filler[] = { 0, 9, 0, 9, 9, 9, 0, 2, 2, 0 };
  u32 idx;

  if (rc->fillerIdx == (u32)(-1))
  {
    rc->fillerIdx = sizeof(filler) / sizeof(*filler) - 1;
  }

  idx = rc->fillerIdx;
  if (frameCnt != 0 && ((frameCnt % 128) == 0))
  {
    idx++;
  }
  idx %= sizeof(filler) / sizeof(*filler);

  if (idx != rc->fillerIdx)
  {
    rc->fillerIdx = idx;
    return filler[idx] + 1;
  }
  return 0;
}
/*------------------------------------------------------------------------------
  VirtualBuffer()  Return difference of target and real buffer fullness.
  Virtual buffer and real bit count grow until one second.  After one second
  output bit rate per second is removed from virtualBitCnt and realBitCnt. Bit
  drifting has been taken care.

  If the leaky bucket in VBR mode becomes empty (e.g. underflow), those R * T_e
  bits are lost and must be decremented from virtualBitCnt. (NOTE: Drift
  calculation will mess virtualBitCnt up, so the loss is added to realBitCnt)
------------------------------------------------------------------------------*/
static i32 VirtualBuffer(rcVirtualBuffer_s *vb, i32 timeInc, true_e hrd)
{
  i32 target;

  vb->picTimeInc    += timeInc;

  /* picTimeInc must be in range of [0, timeScale) */
  while (vb->picTimeInc >= vb->timeScale)
  {
    vb->picTimeInc    -= vb->timeScale;
    if (vb->realBitCnt < (I32_MIN + vb->bitRate))
        vb->realBitCnt = I32_MIN;
    else
        vb->realBitCnt -= vb->bitRate;

    if (vb->bucketLevel < (I32_MIN + vb->bitRate))
        vb->bucketLevel = I32_MIN;
    else
        vb->bucketLevel -= vb->bitRate;

    vb->seconds++;
    vb->averageBitRate = vb->bitRate + vb->realBitCnt / vb->seconds;
  }
  vb->virtualBitCnt = rcCalculate(vb->bitRate, vb->picTimeInc, vb->timeScale);
  //printf("vb->virtualBitCnt=%d,vb->picTimeInc=%d,vb->timeScale=%d,vb->bitRate=%d\n",vb->virtualBitCnt,vb->picTimeInc,vb->timeScale,vb->bitRate);

  if (hrd)
  {
#if RC_CBR_HRD
    /* In CBR mode, bucket _must not_ underflow. Insert filler when
     * needed. */
    vb->bucketFullness = vb->bucketLevel - vb->virtualBitCnt;
#else
    if (vb->bucketLevel >= vb->virtualBitCnt)
    {
      vb->bucketFullness = vb->bucketLevel - vb->virtualBitCnt;
    }
    else
    {
      vb->bucketFullness = 0;
      vb->realBitCnt += vb->virtualBitCnt - vb->bucketLevel;
      vb->bucketLevel = vb->virtualBitCnt;
    }
#endif
  }

  /* Saturate realBitCnt, this is to prevent overflows caused by much greater
     bitrate setting than is really possible to reach */
  if (vb->realBitCnt > 0x1FFFFFFF)
    vb->realBitCnt = 0x1FFFFFFF;
  if (vb->realBitCnt < -0x1FFFFFFF)
    vb->realBitCnt = -0x1FFFFFFF;

  target = vb->virtualBitCnt - vb->realBitCnt;
  //printf("target=%d,vb->realBitCnt=%d\n",target,vb->realBitCnt);
  /* Saturate target, prevents rc going totally out of control.
     This situation should never happen. */
  if (target > 0x1FFFFFFF)
    target = 0x1FFFFFFF;
  if (target < -0x1FFFFFFF)
    target = -0x1FFFFFFF;


  DBG(1, (DBGOUTPUT, "virtualBitCnt:\t%8i  realBitCnt:\t%8i",
          vb->virtualBitCnt, vb->realBitCnt));
  DBG(1, (DBGOUTPUT, "  diff bits:\t%8i  avg bitrate:\t%8i\n", target,
          vb->averageBitRate));
  return target;
}

static i32 rcModelErrorEst(linReg_s *p, vcencRateControl_s *rc)
{
  i64 modelBits = rc->targetPicSize;
  i32 qp = rc->qpHdrPrev;
  i32 err = rc->frameBitCnt - (i32)modelBits;

  if (rc->codingType == ASIC_JPEG)
    return err;

  if (p->a1 == 0 && p->a2 == 0)
    return err;

  modelBits  = DIV(p->a1, q_step[((qp*10)>>QP_FRACTIONAL_BITS)]);
  modelBits += DIV(p->a2, (i64)q_step[((qp*10)>>QP_FRACTIONAL_BITS)] * q_step[((qp*10)>>QP_FRACTIONAL_BITS)]);      
  modelBits >>= QP_FRACTIONAL_BITS;
  modelBits = MIN((modelBits * (rc->ctbPerPic * rc->ctbSize * rc->ctbSize / 16 / 16) / DSCBITPERMB),rc->i32MaxPicSize);

  err = rc->frameBitCnt - (i32)modelBits;
  return err;
}

i32 VCEncAfterSceneChangeCheck(vcencRateControl_s *rc, linReg_s *rcModel, i32 normBits, i32 prevFrameBitCnt)
{
  i32 sceneChanged = 0;

  if (rc->codingType == ASIC_JPEG)
    return 0;

  /* Only support IPPP pattern now. */
  if (rc->hierarchial_bit_allocation_GOP_size > 1)
    return 0;

  if (rc->inputSceneChange)
    return 1;

  if (rc->sliceTypeCur == P_SLICE)
  {
    double dIntraPercentage = rc->intraCu8Num/(double)(rc->picArea/8/8);
    i32 bitsOutCtrl = (rc->frameBitCnt > rc->targetPicSize*3/2) &&
                      (rc->frameBitCnt > rc->virtualBuffer.bitPerPic*2);
    if (rc->sliceTypePrev == P_SLICE)
      bitsOutCtrl = bitsOutCtrl && (rc->frameBitCnt > prevFrameBitCnt*2);

    if((dIntraPercentage > 0.35) && bitsOutCtrl)
      sceneChanged = 1;
  }
  else if ((rc->sliceTypeCur == I_SLICE) && (rc->sliceTypePrev != I_SLICE))
  {
    if ((rc->resetIntraPicQp == 0) && rcModel->len)
    {
      i32 pos = (rcModel->pos > 0) ? (rcModel->pos - 1) : (RC_TABLE_LENGTH - 1);
      i32 prevINormBits = rcModel->bits[pos] * rcModel->qs[pos] / q_step[((rc->qpHdrPrev*10)>>QP_FRACTIONAL_BITS)];

      if ((rc->frameBitCnt > 15 * rc->virtualBuffer.bitPerPic) &&
          (rc->frameBitCnt > 15 * prevFrameBitCnt) &&
          (normBits > 5 * prevINormBits))
      {
        sceneChanged = 1;
      }
    }
  }
  return sceneChanged;
}
#define SWAP(a,b,type) {type tmp; tmp=a; a=b; b=tmp;}
/*------------------------------------------------------------------------------
  VCEncAfterPicRc()  Update source model, bit rate target error and linear
  regression model for frame QP calculation. If HRD enabled, check leaky bucket
  status and return RC_OVERFLOW if coded frame must be skipped. Otherwise
  returns number of required filler payload bytes.
------------------------------------------------------------------------------*/
i32 VCEncAfterPicRc(vcencRateControl_s *rc, u32 nonZeroCnt, u32 byteCnt,
                   u32 qpSum,u32 qpNum)
{
  rcVirtualBuffer_s *vb = &rc->virtualBuffer;
  i32 bitPerPic = rc->virtualBuffer.bitPerPic;
  i32 tmp, stat, bitCnt = (i32)byteCnt * 8, normBits;
  i32 movingBitRate,bestBits,movingBitBase,realBitCnt,targetBitCnt;
  i32 imin, imax, i;
  i32 algNum;
  linReg_s *rcModel = NULL, *rcError = NULL;
  i32 bitCntError;
  i32 prevFrameBitCnt = rc->frameBitCnt;
  i32 sceneChange =0;
  rc->qpSum = (i32)qpSum;
  rc->qpNum = (i32)qpNum;
  rc->averageQp = qpNum ? (float)qpSum/qpNum : (float)qpSum/(rc->ctbPerPic * rc->ctbSize * rc->ctbSize / 8 / 8);
  //printf("average QP= %f\n",rc->averageQp);
  rc->frameBitCnt = bitCnt;
  rc->nonZeroCnt = nonZeroCnt;
  rc->gopBitCnt += bitCnt;
  rc->frameCnt++;
  normBits = rcCalculate(bitCnt, DSCBITPERMB, rc->ctbPerPic * rc->ctbSize * rc->ctbSize / 16 / 16);
  algNum = (rc->sliceTypeCur==I_SLICE) ? 0 : rc->hierarchial_alg_map[rc->hierarchial_bit_allocation_GOP_size-1][rc->gopPoc];

  if (IS_CTBRC_FOR_BITRATE(rc->ctbRc))
  {
    float avgQpForRc = (rc->ctbRateCtrl.qpSumForRc * 1.0 / rc->ctbPerPic) * (1 << QP_FRACTIONAL_BITS);
    i32 fracQp = rc->qpHdrPrev & ((1 << QP_FRACTIONAL_BITS) - 1);
    rc->qpHdrPrev = (i32)avgQpForRc + fracQp;
    rc->qpHdrPrev = MIN(rc->qpHdrPrev, 51 << QP_FRACTIONAL_BITS);
    
    SWAP(rc->ctbRateCtrl.ctbMemCurAddr,rc->ctbRateCtrl.models[rc->sliceTypeCur].ctbMemPreAddr,ptr_t);
    SWAP(rc->ctbRateCtrl.ctbMemCurVirtualAddr,rc->ctbRateCtrl.models[rc->sliceTypeCur].ctbMemPreVirtualAddr,u32*);
  }

  if (rc->pass == 2)
  {
    i32 predId = getPredId(rc);
    updatePredictor(&rc->rcPred[predId], qp2qScale(getNormQp(rc->qpHdrPrev)), rc->pass1CurCost, rc->frameBitCnt, rc->qpHdrPrev);
    if (predId == P_SLICE)
    {
      rc->gopPQpSum += rc->qpHdrPrev;
      rc->gopPQpNum ++;
    }

    /*Update I frame average qp and complexity info in CRF mode*/
    if (rc->crf >= 0)
    {
      //printf("Average QP used by CMODEL: %f\n", rc->averageQp);
      if (rc->sliceTypeCur == I_SLICE)
      {
        int intQp = rc->qpHdr >> QP_FRACTIONAL_BITS;
        double fracQp = (rc->qpHdr & 255)/(1 << QP_FRACTIONAL_BITS);
        double realQp = intQp+fracQp;
        if(rc->crf_iCostAvg > 0)
        {
            rc->crf_iCostAvg = (rc->crf_iCostAvg*rc->crf_iFrames + (int)(rc->pass1CurCost))/(rc->crf_iFrames + 1);
            rc->crf_iQpAvg = (rc->crf_iQpAvg*rc->crf_iFrames + realQp)/(rc->crf_iFrames+1);
        }else
        {
            rc->crf_iCostAvg = (int)(rc->pass1CurCost);
            rc->crf_iQpAvg = realQp;
        }
        rc->crf_iFrames++;
      }
    }
  }

  if (rc->sliceTypeCur != I_SLICE)
  {
      rc->u32PFrameByQPStep   = (2*rc->frameBitCnt * (q_step[(rc->qpHdr*10) >> QP_FRACTIONAL_BITS] >> QP_FRACTIONAL_BITS)) >>2;
  }
  else
  {
      rc->qpHdrPrevI   = rc->qpHdr;
      rc->u32PrevIBitCnt = bitCnt;
  }
  
  if (rc->targetPicSize)
  {
    tmp = ((bitCnt - rc->targetPicSize) * 100) / rc->targetPicSize;
  }
  else
  {
    tmp = 0;
  }
  DBG(1, (DBGOUTPUT, "\nAFTER PIC RC:\n"));
  DBG(0, (DBGOUTPUT, "BitCnt\t\t%8d\n", bitCnt));
  DBG(1, (DBGOUTPUT, "BitErr/avg\t%7d%%  ", ((bitCnt - bitPerPic) * 100) / (bitPerPic + 1)));
  DBG(1, (DBGOUTPUT, "BitErr/target\t%7i%%  qpHdr %2i  avgQp %2i\n", tmp, rc->qpHdr, rc->qpSum / rc->qpNum));
  DBG(1, (DBGOUTPUT, "RC QP(qpHdr %d avgQp %d) BITS %d  BitErr/target\t%7i%%\n", rc->qpHdr, rc->qpSum, bitCnt, tmp));

  if(rc->sliceTypeCur != I_SLICE && rc->sliceTypePrev != I_SLICE)
  {
    if((rc->linReg[algNum].qp_prev < rc->qpHdr)&&(rc->frameBitCnt<rc->linReg[algNum].frameBitCntLast)&&(rc->linReg[algNum].frameBitCntLast!=0)&&(rc->frameBitCnt!=0)&&((((rc->qpHdr-rc->linReg[algNum].qp_prev)*10)>>QP_FRACTIONAL_BITS)!=0))
    {
      rc->linReg[algNum].weight = (((((i64)(rc->linReg[algNum].frameBitCntLast-rc->frameBitCnt))*100/rc->frameBitCnt))*q_step[(rc->qpHdr*10)>>QP_FRACTIONAL_BITS]/q_step[(rc->linReg[algNum].qp_prev*10)>>QP_FRACTIONAL_BITS]/(((rc->qpHdr-rc->linReg[algNum].qp_prev)*10)>>QP_FRACTIONAL_BITS));
    }
    else if((rc->linReg[algNum].qp_prev > rc->qpHdr)&&(rc->frameBitCnt>rc->linReg[algNum].frameBitCntLast)&&(rc->linReg[algNum].frameBitCntLast!=0)&&(rc->frameBitCnt!=0)&&((((rc->linReg[algNum].qp_prev - rc->qpHdr)*10)>>QP_FRACTIONAL_BITS)!=0))
    {
      rc->linReg[algNum].weight = (((((i64)(rc->frameBitCnt-rc->linReg[algNum].frameBitCntLast))*100/rc->linReg[algNum].frameBitCntLast))*q_step[(rc->linReg[algNum].qp_prev*10)>>QP_FRACTIONAL_BITS]/q_step[(rc->qpHdr*10)>>QP_FRACTIONAL_BITS]/(((rc->linReg[algNum].qp_prev - rc->qpHdr)*10)>>QP_FRACTIONAL_BITS));
    }

    if(ABS(tmp)<5&&(rc->targetPicSize !=0))
    {
      rc->linReg[algNum].weight = 50;
    }
    if(rc->linReg[algNum].weight < 10)
      rc->linReg[algNum].weight = 10;
  }

  if((rc->sliceTypeCur == I_SLICE)  &&
    ((rc->codingType != ASIC_JPEG) || ((rc->codingType == ASIC_JPEG) && (ABS(tmp)> ABS(rc->tolMovingBitRate - 100)) && (rc->targetPicSize !=0))))
  {
    rcModel = &rc->intra;
    rcError = &rc->intraError;

    if (rc->hrd == ENCHW_YES) 
    {
      //printf("old rc->minIQp=%x,rc->qpHdrPrev=%x \n",rc->minIQp,rc->qpHdrPrev);
      if(rc->frameBitCnt<(rc->virtualBuffer.bitPerPic)&&(rc->minIQp==rc->qpHdrPrev))
      {        
        rc->minIQp -= 1<<QP_FRACTIONAL_BITS;
        if(rc->minIQp < rc->finiteQp)
          rc->minIQp = rc->finiteQp;
        if(rc->minIQp<rc->qpMin)
          rc->minIQp = rc->qpMin;
        //printf("new -- rc->minIQp=%x,rc->frameBitCnt=%d,buffersize=%d \n",rc->minIQp,rc->frameBitCnt,rc->virtualBuffer.bufferSize);
      }

      else if((rc->gopLastBitCnt<(rc->virtualBuffer.bitRate*rc->gopMulti)*9/10)&&(rc->frameBitCnt<(rc->virtualBuffer.bitRate*(3+(rc->gopMulti-1))/4)))
      {
        rc->minIQp -= 1<<QP_FRACTIONAL_BITS;
        if(rc->minIQp < rc->finiteQp)
          rc->minIQp = rc->finiteQp;
        if(rc->minIQp<rc->qpMin)
          rc->minIQp = rc->qpMin;
        //printf("new --- rc->minIQp=%x,rc->frameBitCnt=%d,buffersize=%d \n",rc->minIQp,rc->frameBitCnt,rc->virtualBuffer.bufferSize);
      }
      else if((rc->frameBitCnt>((rc->virtualBuffer.bufferSize- rc->virtualBuffer.bitRate + rc->intraframeBitCnt)))||
              (rc->frameBitCnt>(rc->virtualBuffer.bitRate*(3+(rc->gopMulti-1))/4)))
      {
        rc->minIQp += 1<<QP_FRACTIONAL_BITS;
        if(rc->minIQp>rc->qpMax)
          rc->minIQp = rc->qpMax;
        //printf("new ++ rc->minIQp=%x,rc->frameBitCnt=%d,buffersize=%d, standard=%d\n",rc->minIQp,rc->frameBitCnt,rc->virtualBuffer.bufferSize,(rc->virtualBuffer.bitRate*(3+(rc->gopMulti-1))/4));
      }
      //rc->intraframeBitCnt = rc->frameBitCnt;
    }
  }
  else
  {
    rcModel = &rc->linReg[algNum];
    rcError = &rc->rError[algNum];

    if (rc->sliceTypeCur == B_SLICE && rc->hierarchial_decoding_order[rc->hierarchial_bit_allocation_GOP_size-1][rc->gopPoc])
    {
      if(rc->smooth_psnr_in_gop && (i32)rc->gopPoc == rc->hierarchial_bit_allocation_GOP_size - 2) {
#if 0      
        imin = imax = 0;
        for(i = 0; i < rc->hierarchial_bit_allocation_GOP_size; i++) {
          if(rc->hierarchial_sse[rc->hierarchial_bit_allocation_GOP_size-1][i] > rc->hierarchial_sse[rc->hierarchial_bit_allocation_GOP_size-1][imax])
            imax = i;
          if(rc->hierarchial_sse[rc->hierarchial_bit_allocation_GOP_size-1][i] < rc->hierarchial_sse[rc->hierarchial_bit_allocation_GOP_size-1][imin])
            imin = i;
        }
        if(rc->hierarchial_bit_allocation_map[rc->hierarchial_bit_allocation_GOP_size-1][imin] > 1) 
          rc->hierarchial_bit_allocation_map[rc->hierarchial_bit_allocation_GOP_size-1][imin]--;
        if(rc->hierarchial_bit_allocation_map[rc->hierarchial_bit_allocation_GOP_size-1][imax] < 20)
          rc->hierarchial_bit_allocation_map[rc->hierarchial_bit_allocation_GOP_size-1][imax]++;
#else
        for(i = 0; i < rc->hierarchial_bit_allocation_GOP_size-1; i++) {
          if(rc->hierarchial_sse[rc->hierarchial_bit_allocation_GOP_size-1][rc->hierarchial_bit_allocation_GOP_size-1] > rc->hierarchial_sse[rc->hierarchial_bit_allocation_GOP_size-1][i])
          {
            if(rc->hierarchial_bit_allocation_map[rc->hierarchial_bit_allocation_GOP_size-1][i] > 1) 
                rc->hierarchial_bit_allocation_map[rc->hierarchial_bit_allocation_GOP_size-1][i]--;
          }
          else if(rc->hierarchial_sse[rc->hierarchial_bit_allocation_GOP_size-1][rc->hierarchial_bit_allocation_GOP_size-1] < rc->hierarchial_sse[rc->hierarchial_bit_allocation_GOP_size-1][i])
          {
            if(rc->hierarchial_bit_allocation_map[rc->hierarchial_bit_allocation_GOP_size-1][i] < 20)
                rc->hierarchial_bit_allocation_map[rc->hierarchial_bit_allocation_GOP_size-1][i]++;
            
          }
        }
#endif          
      } 
    }
  }

  bitCntError = rcModelErrorEst(rcModel, rc);
  
  //printf("I frame qp=%d,bitPerPixel=%f\n",rc->qpHdr,(float)bitCnt/rc->picArea);
  MaAddFrame(&rc->ma, bitCnt);

  if (rc->sliceTypeCur != I_SLICE)
  {
    MaCharAddFrame(&rc->ma_char, rc->intraCu8Num, rc->skipCu8Num, rc->PBFrame4NRdCost);

    /* trace the QP over GOP, excluding Intra QP */
    rc->gopQpSum += rc->qpHdrPrev;
    rc->gopQpDiv++;
  }

  /* Scene change process. Clean RC Model for P frames. */
  /* Only support IPPP pattern now. */
  if (VCEncAfterSceneChangeCheck(rc, rcModel, normBits, prevFrameBitCnt))
  {
    tmp = rc->linReg[algNum].qp_prev;
    EWLmemset(&rc->rError[algNum], 0, sizeof(linReg_s));
    EWLmemset(&rc->linReg[algNum], 0, sizeof(linReg_s));
    rc->linReg[algNum].weight = 10;

    if (rc->sliceTypeCur == P_SLICE)
    {
      /* clean rcError. reset next I frame QP */
      rc->resetIntraPicQp = 1;
      bitCntError = 0;
    }
    else if (rc->sliceTypeCur == I_SLICE)
    {
      tmp = rc->qpHdrPrev;
      /* if qpHdrPrev is not reset for this scene change frame, get next P frame QP from model */
      if (!rc->inputSceneChange)
      {
        update_tables(&rc->linReg[algNum], rc->qpHdrPrev, normBits);
        update_model(&rc->linReg[algNum]);
      }
    }
    rc->linReg[algNum].qp_prev = tmp;
  }
  /* clean scene change related flags */
  rc->inputSceneChange = 0;
  if (rc->sliceTypeCur == I_SLICE)
    rc->resetIntraPicQp = 0;

  /* Calculate the source parameter only for INTER frames */
  if (rc->sliceTypeCur != I_SLICE)
    SourceParameter(rc, rc->nonZeroCnt);

  /* Store the error between target and actual frame size in percentage.
   * Saturate the error to avoid inter frames with mostly intra MBs
   * to affect too much. */
  if (rcError)
    update_rc_error(rcError, bitCntError, rc->windowLen);

  /* Update number of bits used for residual, inter or intra */
  if (rcModel)
  {
    update_tables(rcModel, rc->qpHdrPrev, normBits);
    update_model(rcModel);
  }

  /* Post-frame skip if HRD buffer overflow */
  if ((rc->hrd == ENCHW_YES) && (bitCnt > (vb->bufferSize - vb->bucketFullness)))
  {
    DBG(1, (DBGOUTPUT, "Be: %7i  ", vb->bucketFullness));
    DBG(1, (DBGOUTPUT, "fillerBits %5i  ", 0));
    DBG(1, (DBGOUTPUT, "bitCnt %d  spaceLeft %d  ",
            bitCnt, (vb->bufferSize - vb->bucketFullness)));
    DBG(1, (DBGOUTPUT, "bufSize %d  bucketFullness %d  bitPerPic %d\n",
            vb->bufferSize, vb->bucketFullness, bitPerPic));
    DBG(0, (DBGOUTPUT, "HRD overflow, frame discard\n"));
    rc->frameCoded = ENCHW_NO;

    /*update sliceTypePre even HRD overflow, otherwise, 
      if IFrame overflow happen, next followed IFrame has no chance to use RC decide new qp in PicQuant()
       then leading to HRD overflow forever
     */
    rc->sliceTypePrev = rc->sliceTypeCur;
    
    return VCENCRC_OVERFLOW;
  }
  else
  {
    i32 vbBitCnt = bitCnt;
    /* do not consider bit rate loss if qp==qpMin in VBR force mode */
    if ( rc->vbr &&
        (rc->qpHdr <= rc->qpMin) &&
        (vbBitCnt < rc->virtualBuffer.bitPerPic))
    {
      vbBitCnt = rc->virtualBuffer.bitPerPic;
    }

    if((rc->sliceTypeCur != I_SLICE) &&
        sceneChange &&
        (bitCnt > (rc->targetPicSize*(100+sceneChange)/100)))
    {
        i64 tmpBits;
        tmpBits = (rc->targetPicSize*(100 + sceneChange) / 100);

        if (vb->bucketLevel > ((i32)I32_MAX - tmpBits))
            vb->bucketLevel = (i32)I32_MAX;
        else
            vb->bucketLevel += (i32)tmpBits;

        if (vb->bucketFullness > ((i32)I32_MAX - tmpBits))
            vb->bucketFullness = (i32)I32_MAX;
        else
            vb->bucketFullness += (i32)tmpBits;

        if (vb->realBitCnt > ((i32)I32_MAX - tmpBits))
            vb->realBitCnt = (i32)I32_MAX;
        else
            vb->realBitCnt += (i32)tmpBits;
    }
    else if (rc->codingType != ASIC_JPEG)
    {
        if (vb->bucketLevel > ((i32)I32_MAX - vbBitCnt))
            vb->bucketLevel = (i32)I32_MAX;
        else
            vb->bucketLevel += vbBitCnt;

        if (vb->bucketFullness > ((i32)I32_MAX - vbBitCnt))
            vb->bucketFullness = (i32)I32_MAX;
        else
            vb->bucketFullness += vbBitCnt;

        if (vb->realBitCnt > ((i32)I32_MAX - vbBitCnt))
            vb->realBitCnt = (i32)I32_MAX;
        else
            vb->realBitCnt += vbBitCnt;
    }
  }

  if(rc->sliceTypeCur != I_SLICE)
  {
      rc->linReg[algNum].frameBitCntLast = rc->frameBitCnt;
      rc->linReg[algNum].targetPicSizeLast = rc->targetPicSize;
  }
  rc->sliceTypePrev = rc->sliceTypeCur;
  DBG(1, (DBGOUTPUT, "plot\t%4i\t%4i\t%8i\t%8i\t%8i\t%8i\t%8i\t%8i\t%8i\n",
          rc->frameCnt, rc->qpHdr, rc->targetPicSize, bitCnt,
          bitPerPic, rc->gopAvgBitCnt, vb->realBitCnt - vb->virtualBitCnt,
          vb->bitRate, vb->bucketFullness));

  if (rc->hrd == ENCHW_NO)
  {
    return 0;
  }

  tmp = 0;
#if RC_CBR_HRD
  /* Bits needed to prevent bucket underflow */
  tmp = bitPerPic - vb->bucketFullness;

  if (tmp > 0)
  {
    tmp = (tmp + 7) / 8;
    vb->bucketFullness += tmp * 8;
    vb->realBitCnt += tmp * 8;
  }
  else
  {
    tmp = 0;
  }
#endif

  /* Update Buffering Info */
  stat = vb->bufferSize - vb->bucketFullness;

  rc->gInitialDelay = rcCalculate(90000, stat, vb->bitRate);
  rc->gInitialDoffs = rc->gDelaySum - rc->gInitialDelay;



  

  DBG(1, (DBGOUTPUT, "initialDelay: %5i  ", rc->gInitialDelay));
  DBG(1, (DBGOUTPUT, "initialDoffs: %5i\n", rc->gInitialDoffs));
  DBG(1, (DBGOUTPUT, "Be: %7i  ", vb->bucketFullness));
  DBG(1, (DBGOUTPUT, "fillerBits %5i\n", tmp * 8));

#ifdef TRACE_RC
  if (vb->bucketFullness < rc->gBufferMin)
  {
    rc->gBufferMin = vb->bucketFullness;
  }
  if (vb->bucketFullness > rc->gBufferMax)
  {
    rc->gBufferMax = vb->bucketFullness;
  }
  DBG(1, (DBGOUTPUT, "\nLeaky Bucket Min: %i (%d%%)  Max: %i (%d%%)\n",
          rc->gBufferMin, rc->gBufferMin * 100 / vb->bufferSize,
          rc->gBufferMax, rc->gBufferMax * 100 / vb->bufferSize));
#endif
  return tmp;
}

static void init_hierarchial_rc (vcencRateControl_s *rc)
{
  //if(rc->hierarchial_bit_allocation_GOP_size==1)
  {
    if(rc->bpp>20)
    {
      rc->hierarchial_bit_allocation_map[0][0]=6;
    }
    else if(rc->bpp>10)
    {
      rc->hierarchial_bit_allocation_map[0][0]=10;
    }
    else if(rc->bpp>5)
    {
      rc->hierarchial_bit_allocation_map[0][0]=12;
    }
    else
    {
      rc->hierarchial_bit_allocation_map[0][0]=14;
    }
    rc->hierarchial_alg_map[0][0]=0;
    rc->hierarchial_decoding_order[0][0]=0;  // poc order : decoding order
  }
  //else if(rc->hierarchial_bit_allocation_GOP_size==2)
  {
    if(rc->bpp>20)
    {
      rc->hierarchial_bit_allocation_map[1][1]=6;
      rc->hierarchial_bit_allocation_map[1][0]=2;
    }
    else if(rc->bpp>10)
    {
      rc->hierarchial_bit_allocation_map[1][1]=10;
      rc->hierarchial_bit_allocation_map[1][0]=2;
    }
    else if(rc->bpp>5)
    {
      rc->hierarchial_bit_allocation_map[1][1]=12;
      rc->hierarchial_bit_allocation_map[1][0]=2;
    }
    else
    {
      rc->hierarchial_bit_allocation_map[1][1]=14;
      rc->hierarchial_bit_allocation_map[1][0]=2;
    }
    rc->hierarchial_alg_map[1][1]=0;
    rc->hierarchial_alg_map[1][0]=1;
    rc->hierarchial_decoding_order[1][1]=0;
    rc->hierarchial_decoding_order[1][0]=1;
  }
  //else if(rc->hierarchial_bit_allocation_GOP_size==3)
  {
    if(rc->bpp>20)
    {
      rc->hierarchial_bit_allocation_map[2][2]=6;
      rc->hierarchial_bit_allocation_map[2][0]=2;
      rc->hierarchial_bit_allocation_map[2][1]=3;
    }
    else if(rc->bpp>10)
    {
      rc->hierarchial_bit_allocation_map[2][2]=10;
      rc->hierarchial_bit_allocation_map[2][0]=2;
      rc->hierarchial_bit_allocation_map[2][1]=3;
    }
    else if(rc->bpp>5)
    {
      rc->hierarchial_bit_allocation_map[2][2]=12;
      rc->hierarchial_bit_allocation_map[2][0]=2;
      rc->hierarchial_bit_allocation_map[2][1]=3;
    }
    else
    {
      rc->hierarchial_bit_allocation_map[2][2]=14;
      rc->hierarchial_bit_allocation_map[2][0]=2;
      rc->hierarchial_bit_allocation_map[2][1]=3;
    }
    rc->hierarchial_alg_map[2][2]=0;
    rc->hierarchial_alg_map[2][1]=1;
    rc->hierarchial_alg_map[2][0]=1;
    rc->hierarchial_decoding_order[2][2]=0;
    rc->hierarchial_decoding_order[2][0]=1;
    rc->hierarchial_decoding_order[2][1]=2;
  }
  //else if(rc->hierarchial_bit_allocation_GOP_size==4)
  {
    if(rc->bpp>20)
    {
      rc->hierarchial_bit_allocation_map[3][3] = 12;
      rc->hierarchial_bit_allocation_map[3][1] = 5;
      rc->hierarchial_bit_allocation_map[3][0] = 3;
      rc->hierarchial_bit_allocation_map[3][2] = 3;
    }
    else if(rc->bpp>10)
    {
      rc->hierarchial_bit_allocation_map[3][3] = 15;
      rc->hierarchial_bit_allocation_map[3][1] = 6;
      rc->hierarchial_bit_allocation_map[3][0] = 3;
      rc->hierarchial_bit_allocation_map[3][2] = 3;
    }
    else if(rc->bpp>5)
    {
      rc->hierarchial_bit_allocation_map[3][3] = 19;
      rc->hierarchial_bit_allocation_map[3][1] = 7;
      rc->hierarchial_bit_allocation_map[3][0] = 3;
      rc->hierarchial_bit_allocation_map[3][2] = 3;
    }
    else
    {
      rc->hierarchial_bit_allocation_map[3][3] = 23;
      rc->hierarchial_bit_allocation_map[3][1] = 8;
      rc->hierarchial_bit_allocation_map[3][0] = 3;
      rc->hierarchial_bit_allocation_map[3][2] = 3;
    }
    rc->hierarchial_alg_map[3][3]=0;
    rc->hierarchial_alg_map[3][1]=1;
    rc->hierarchial_alg_map[3][0]=1;
    rc->hierarchial_alg_map[3][2]=1;

    rc->hierarchial_decoding_order[3][3]=0; // poc order: decoding order
    rc->hierarchial_decoding_order[3][1]=1;
    rc->hierarchial_decoding_order[3][0]=2;
    rc->hierarchial_decoding_order[3][2]=3;
  }  
  //else if(rc->hierarchial_bit_allocation_GOP_size==5)
  {
    if(rc->bpp>20)
    {
      rc->hierarchial_bit_allocation_map[4][4] = 12;
      rc->hierarchial_bit_allocation_map[4][1] = 6;
      rc->hierarchial_bit_allocation_map[4][0] = 4;
      rc->hierarchial_bit_allocation_map[4][2] = 3;
      rc->hierarchial_bit_allocation_map[4][3] = 3;
    }
    else if(rc->bpp>10)
    {
      rc->hierarchial_bit_allocation_map[4][4] = 15;
      rc->hierarchial_bit_allocation_map[4][1] = 7;
      rc->hierarchial_bit_allocation_map[4][0] = 4;
      rc->hierarchial_bit_allocation_map[4][2] = 3;
      rc->hierarchial_bit_allocation_map[4][3] = 3;
    }
    else if(rc->bpp>5)
    {
      rc->hierarchial_bit_allocation_map[4][4] = 19;
      rc->hierarchial_bit_allocation_map[4][1] = 8;
      rc->hierarchial_bit_allocation_map[4][0] = 4;
      rc->hierarchial_bit_allocation_map[4][2] = 3;
      rc->hierarchial_bit_allocation_map[4][3] = 3;
    }
    else
    {
      rc->hierarchial_bit_allocation_map[4][4] = 23;
      rc->hierarchial_bit_allocation_map[4][1] = 9;
      rc->hierarchial_bit_allocation_map[4][0] = 4;
      rc->hierarchial_bit_allocation_map[4][2] = 3;
      rc->hierarchial_bit_allocation_map[4][3] = 3;
    }
    rc->hierarchial_alg_map[4][4]=0;
    rc->hierarchial_alg_map[4][1]=1;
    rc->hierarchial_alg_map[4][0]=1;
    rc->hierarchial_alg_map[4][2]=1;
    rc->hierarchial_alg_map[4][3]=1;

    rc->hierarchial_decoding_order[4][4]=0; // poc order: decoding order
    rc->hierarchial_decoding_order[4][1]=1;
    rc->hierarchial_decoding_order[4][0]=2;
    rc->hierarchial_decoding_order[4][2]=3;
    rc->hierarchial_decoding_order[4][3]=4;
  }
  //else if(rc->hierarchial_bit_allocation_GOP_size==6)
  {
    if(rc->bpp>20)
    {
      rc->hierarchial_bit_allocation_map[5][5] = 12;
      rc->hierarchial_bit_allocation_map[5][2] = 6;
      rc->hierarchial_bit_allocation_map[5][0] = 3;
      rc->hierarchial_bit_allocation_map[5][1] = 4;
      rc->hierarchial_bit_allocation_map[5][3] = 3;
      rc->hierarchial_bit_allocation_map[5][4] = 3;
    }
    else if(rc->bpp>10)
    {
      rc->hierarchial_bit_allocation_map[5][5] = 15;
      rc->hierarchial_bit_allocation_map[5][2] = 7;
      rc->hierarchial_bit_allocation_map[5][0] = 3;
      rc->hierarchial_bit_allocation_map[5][1] = 4;
      rc->hierarchial_bit_allocation_map[5][3] = 3;
      rc->hierarchial_bit_allocation_map[5][4] = 3;
    }
    else if(rc->bpp>5)
    {
      rc->hierarchial_bit_allocation_map[5][5] = 19;
      rc->hierarchial_bit_allocation_map[5][2] = 8;
      rc->hierarchial_bit_allocation_map[5][0] = 3;
      rc->hierarchial_bit_allocation_map[5][1] = 4;
      rc->hierarchial_bit_allocation_map[5][3] = 3;
      rc->hierarchial_bit_allocation_map[5][4] = 3;
    }
    else
    {
      rc->hierarchial_bit_allocation_map[5][5] = 23;
      rc->hierarchial_bit_allocation_map[5][2] = 9;
      rc->hierarchial_bit_allocation_map[5][0] = 3;
      rc->hierarchial_bit_allocation_map[5][1] = 4;
      rc->hierarchial_bit_allocation_map[5][3] = 3;
      rc->hierarchial_bit_allocation_map[5][4] = 3;
    }
    rc->hierarchial_alg_map[5][5]=0;
    rc->hierarchial_alg_map[5][2]=1;
    rc->hierarchial_alg_map[5][0]=1;
    rc->hierarchial_alg_map[5][1]=1;
    rc->hierarchial_alg_map[5][3]=1;
    rc->hierarchial_alg_map[5][4]=1;

    rc->hierarchial_decoding_order[5][5]=0; // poc order: decoding order
    rc->hierarchial_decoding_order[5][2]=1;
    rc->hierarchial_decoding_order[5][0]=2;
    rc->hierarchial_decoding_order[5][1]=3;
    rc->hierarchial_decoding_order[5][3]=4;    
    rc->hierarchial_decoding_order[5][4]=5;  
  }
  //else if(rc->hierarchial_bit_allocation_GOP_size==7)
  {
    if(rc->bpp>20)
    {
      rc->hierarchial_bit_allocation_map[6][6] = 12;
      rc->hierarchial_bit_allocation_map[6][2] = 6;
      rc->hierarchial_bit_allocation_map[6][0] = 3;
      rc->hierarchial_bit_allocation_map[6][1] = 3;
      rc->hierarchial_bit_allocation_map[6][4] = 5;
      rc->hierarchial_bit_allocation_map[6][3] = 3;
      rc->hierarchial_bit_allocation_map[6][5] = 3;
    }
    else if(rc->bpp>10)
    {
      rc->hierarchial_bit_allocation_map[6][6] = 15;
      rc->hierarchial_bit_allocation_map[6][2] = 7;
      rc->hierarchial_bit_allocation_map[6][0] = 3;
      rc->hierarchial_bit_allocation_map[6][1] = 3;
      rc->hierarchial_bit_allocation_map[6][4] = 5;
      rc->hierarchial_bit_allocation_map[6][3] = 3;
      rc->hierarchial_bit_allocation_map[6][5] = 3;
    }
    else if(rc->bpp>5)
    {
      rc->hierarchial_bit_allocation_map[6][6] = 19;
      rc->hierarchial_bit_allocation_map[6][2] = 8;
      rc->hierarchial_bit_allocation_map[6][0] = 3;
      rc->hierarchial_bit_allocation_map[6][1] = 3;
      rc->hierarchial_bit_allocation_map[6][4] = 5;
      rc->hierarchial_bit_allocation_map[6][3] = 3;
      rc->hierarchial_bit_allocation_map[6][5] = 3;
    }
    else
    {
      rc->hierarchial_bit_allocation_map[6][6] = 23;
      rc->hierarchial_bit_allocation_map[6][2] = 9;
      rc->hierarchial_bit_allocation_map[6][0] = 3;
      rc->hierarchial_bit_allocation_map[6][1] = 3;
      rc->hierarchial_bit_allocation_map[6][4] = 5;
      rc->hierarchial_bit_allocation_map[6][3] = 3;
      rc->hierarchial_bit_allocation_map[6][5] = 3;
    }
    rc->hierarchial_alg_map[6][6]=0;
    rc->hierarchial_alg_map[6][2]=1;
    rc->hierarchial_alg_map[6][0]=1;
    rc->hierarchial_alg_map[6][1]=1;
    rc->hierarchial_alg_map[6][4]=1;
    rc->hierarchial_alg_map[6][3]=1;
    rc->hierarchial_alg_map[6][5]=1;

    rc->hierarchial_decoding_order[6][6]=0; // poc order: decoding order
    rc->hierarchial_decoding_order[6][2]=1;
    rc->hierarchial_decoding_order[6][0]=2;
    rc->hierarchial_decoding_order[6][1]=3;
    rc->hierarchial_decoding_order[6][4]=4;    
    rc->hierarchial_decoding_order[6][3]=5;  
    rc->hierarchial_decoding_order[6][5]=6; 
    
  }
  //else if(rc->hierarchial_bit_allocation_GOP_size==8)
  {
    if(rc->bpp>20)
    {
      rc->hierarchial_bit_allocation_map[7][7] = 12;
      rc->hierarchial_bit_allocation_map[7][3] = 6;
      rc->hierarchial_bit_allocation_map[7][1] = 5;
      rc->hierarchial_bit_allocation_map[7][0] = 3;
      rc->hierarchial_bit_allocation_map[7][2] = 3;
      rc->hierarchial_bit_allocation_map[7][5] = 5;
      rc->hierarchial_bit_allocation_map[7][4] = 3;
      rc->hierarchial_bit_allocation_map[7][6] = 3;
    }
    else if(rc->bpp>10)
    {
      rc->hierarchial_bit_allocation_map[7][7] = 15;
      rc->hierarchial_bit_allocation_map[7][3] = 7;
      rc->hierarchial_bit_allocation_map[7][1] = 5;
      rc->hierarchial_bit_allocation_map[7][0] = 3;
      rc->hierarchial_bit_allocation_map[7][2] = 3;
      rc->hierarchial_bit_allocation_map[7][5] = 5;
      rc->hierarchial_bit_allocation_map[7][4] = 3;
      rc->hierarchial_bit_allocation_map[7][6] = 3;
    }
    else if(rc->bpp>5)
    {
      rc->hierarchial_bit_allocation_map[7][7] = 19;
      rc->hierarchial_bit_allocation_map[7][3] = 8;
      rc->hierarchial_bit_allocation_map[7][1] = 5;
      rc->hierarchial_bit_allocation_map[7][0] = 3;
      rc->hierarchial_bit_allocation_map[7][2] = 3;
      rc->hierarchial_bit_allocation_map[7][5] = 5;
      rc->hierarchial_bit_allocation_map[7][4] = 3;
      rc->hierarchial_bit_allocation_map[7][6] = 3;
    }
    else
    {
      rc->hierarchial_bit_allocation_map[7][7] = 23;
      rc->hierarchial_bit_allocation_map[7][3] = 9;
      rc->hierarchial_bit_allocation_map[7][1] = 5;
      rc->hierarchial_bit_allocation_map[7][0] = 3;
      rc->hierarchial_bit_allocation_map[7][2] = 3;
      rc->hierarchial_bit_allocation_map[7][5] = 5;
      rc->hierarchial_bit_allocation_map[7][4] = 3;
      rc->hierarchial_bit_allocation_map[7][6] = 3;
    }
    rc->hierarchial_alg_map[7][7]=0;
    rc->hierarchial_alg_map[7][3]=1;
    rc->hierarchial_alg_map[7][1]=1;
    rc->hierarchial_alg_map[7][0]=1;
    rc->hierarchial_alg_map[7][2]=1;
    rc->hierarchial_alg_map[7][5]=1;
    rc->hierarchial_alg_map[7][4]=1;
    rc->hierarchial_alg_map[7][6]=1;

    rc->hierarchial_decoding_order[7][7]=0; // poc order: decoding order
    rc->hierarchial_decoding_order[7][3]=1;
    rc->hierarchial_decoding_order[7][1]=2;
    rc->hierarchial_decoding_order[7][0]=3;
    rc->hierarchial_decoding_order[7][2]=4;    
    rc->hierarchial_decoding_order[7][5]=5;  
    rc->hierarchial_decoding_order[7][4]=6; 
    rc->hierarchial_decoding_order[7][6]=7; 
    
  }
  
  if (rc->pass == 2) 
  {
    i32 i, j;
    for (j = 0; j < 8; j ++)
      for (i = 0; i < 8; i ++)
        rc->hierarchial_bit_allocation_map[j][i] = 1;
  }
}

/*------------------------------------------------------------------------------
  crfPicQuant() Calculate qp for current frame for CRF mode
------------------------------------------------------------------------------*/
void crfPicQuant(vcencRateControl_s *rc, struct sw_picture* pic)
{
  i32 predId = rc->predId;
  unsigned long long curCost = rc->pass1CurCost;
  double q = rc->crf;
  double qConst = rc->crf;
  int intQp = q;
  //printf("POC: %d;  Cost: %d;  Type: %d\n",pic->poc, (int)curCost, predId);
  if(predId == B_SLICE || predId == 3)
  {
    /*For B slice and BREF slice, QP is mainly determined by reference 2 frames*/
    struct sw_picture *prevRefPic = pic->rpl[0][0];
    struct sw_picture *nextRefPic = pic->rpl[1][0];
    double q0 = prevRefPic->curQpRc;
    double q1 = nextRefPic->curQpRc;
    unsigned long long c0 = prevRefPic->cost;
    unsigned long long c1 = nextRefPic->cost;
    int i0 = prevRefPic->type == I_SLICE?1:0;
    int i1 = nextRefPic->type == I_SLICE?1:0;
    int dt0 = (pic->poc - prevRefPic->poc) >= 0? pic->poc - prevRefPic->poc : prevRefPic->poc - pic->poc;
    int dt1 = (pic->poc - nextRefPic->poc) >= 0? pic->poc - nextRefPic->poc : nextRefPic->poc - pic->poc;

    /*Initial qp is affected by reference slice type and distance*/
    if (i0 && i1)
      q = (q0 + q1) / 2 + rc->ipOffset;
    else if (i0)
      q = q1;
    else if (i1)
      q = q0;
    else
      q = (q0 * dt1 + q1 * dt0) / (dt0 + dt1);
    /*Unless both reference slices are B slice, reduce QP by pbOffset*/
    if (prevRefPic->x265Type != B_SLICE && nextRefPic->x265Type != B_SLICE)
      q += rc->pbOffset / 2;
    /*For BREF slice, modify qp based on complexity(cost) of current slice and reference slice*/
    if(predId == 3)
    {
      if (((1.5*curCost) < c0 && q >= q0) && ((1.5*curCost) < c1 && q >= q1))
      {
          double avgCost = (c0+c1)/2;
          double avgQp = (q0+q1)/2;
          q = CLIP3(0, 51, avgQp - (avgCost/curCost - 1)*rc->pbOffset/4);
      }else if(1.5*curCost < c0 && q >= q0)
          q = CLIP3(0, 51, q0 - (c0/curCost - 1)*rc->pbOffset/4);
      else if(1.5*curCost < c1 && q >= q1)
          q = CLIP3(0, 51, q1 - (c1/curCost - 1)*rc->pbOffset/4);
    }
    else
    {
      if((curCost > (1.5*c0) && q <= q0) && (curCost > (1.5*c1) && q <= q1))
      {
          double avgCost = (c0+c1)/2;
          double avgQp = (q0+q1)/2;
          q = CLIP3(0, 51, avgQp + (curCost/avgCost - 1)*rc->pbOffset/4);
      }else if(curCost > (1.5*c0) && q <= q0)
          q = CLIP3(0, 51, q0 + (curCost/c0 - 1)*rc->pbOffset/4);
      else if(curCost > (1.5*c1) && q <= q1)
          q = CLIP3(0, 51, q1 + (curCost/c1 - 1)*rc->pbOffset/4);
    }

    /*Restrict final qp within +-3 of crf constant*/
    q = CLIP3(qConst-3, qConst+3, q);
  }
  else if(predId == P_SLICE)
  {
    /*Almost same logic of B slice*/
    struct sw_picture *prevRefPic = pic->rpl[0][0];
    struct sw_picture *nextRefPic = pic->rpl[1][0];
    double q0 = prevRefPic->curQpRc;
    double q1 = -1;
    int dt0 = (pic->poc - prevRefPic->poc) >= 0? pic->poc - prevRefPic->poc : prevRefPic->poc - pic->poc;
    int dt1 = 0;
    int i0 = prevRefPic->type == I_SLICE?1:0;
    int i1 = 0;
    unsigned long long c0 = prevRefPic->cost;
    unsigned long long c1 = 0;

    if (pic->type == B_SLICE)
    {
        q1 = nextRefPic->curQpRc;
        if (nextRefPic->x265Type == B_SLICE)
            q1 -= rc->pbOffset / 2;


        dt1 = (pic->poc - nextRefPic->poc) >= 0? pic->poc - nextRefPic->poc : nextRefPic->poc - pic->poc;
        i1 = nextRefPic->type == I_SLICE?1:0;
        c1 = nextRefPic->cost;
    } 

    if(prevRefPic->x265Type == B_SLICE)
        q0 -= rc->pbOffset/2;


    if(pic->type == B_SLICE)
    {
        if (i0 && i1)
            q = (q0 + q1) / 2 + rc->ipOffset;
        else if (i0)
            q = q1;
        else if (i1)
            q = q0;
        else
            q = (q0 * dt1 + q1 * dt0) / (dt0 + dt1);
    }else
    {
        q = q0;
    }

    if(pic->type == B_SLICE)
    {
        if ((1.5*curCost < c0 && q >= q0) && (1.5*curCost < c1 && q >= q1))
        {
            double avgCost = (c0+c1)/2;
            double avgQp = (q0+q1)/2;
            q = CLIP3(0, 51, avgQp - (avgCost/curCost - 1)*rc->pbOffset/4);
        }else if(1.5*curCost < c0 && q >= q0)
            q = CLIP3(0, 51, q0 - (c0/curCost - 1)*rc->pbOffset/4);
        else if(1.5*curCost < c1 && q >= q1)
            q = CLIP3(0, 51, q1 - (c1/curCost - 1)*rc->pbOffset/4);


        if((curCost > 1.5*c0 && q <= q0) && (curCost > 1.5*c1 && q <= q1))
        {
            double avgCost = (c0+c1)/2;
            double avgQp = (q0+q1)/2;
            q = CLIP3(0, 51, avgQp + (curCost/avgCost - 1)*rc->pbOffset/4);
        }else if(curCost > 1.5*c0 && q <= q0)
            q = CLIP3(0, 51, q0 + (curCost/c0 - 1)*rc->pbOffset/4);
        else if(curCost > 1.5*c1 && q <= q1)
            q = CLIP3(0, 51, q1 + (curCost/c1 - 1)*rc->pbOffset/4);
    }
    else
    {
        if(1.5*curCost < c0 && q >= q0)
            q = CLIP3(0, 51, q0 - (c0/curCost - 1)*rc->pbOffset/4);

        if(curCost > 1.5*c0 && q <= q0)
            q = CLIP3(0, 51, q0 + (curCost/c0 - 1)*rc->pbOffset/4);
    }

    if(pic->type == B_SLICE)
        q = CLIP3(qConst-2, qConst+2, q);
    else
        q = CLIP3(qConst-3, qConst+3, q);
  }
  else
  {
    /*QP of I frame is mainly determined by complexity(cost) of current frame compared with average complexity of all I frames*/
    if(rc->crf_iCostAvg > 0)
    {
        if((curCost > rc->crf_iCostAvg))
        {
            q = CLIP3(0, 51, rc->crf - (curCost/rc->crf_iCostAvg - 1)*MIN(q - rc->crf_iQpAvg, 6));
        }
    }
    //QP for I frame needs to be smaller to get better quality
    if(q >= qConst)
        q -= 3;
    q = CLIP3(qConst-3, qConst+3,q);
  }
  pic->curQpRc = CLIP3(0, 51, q);
  //x265Type is mainly to store predId since sliceInst->type stores real reference type
  pic->x265Type = predId;
  pic->cost = curCost;
  intQp = (int)pic->curQpRc;
  rc->qpHdr = (intQp << QP_FRACTIONAL_BITS) + (pic->curQpRc - intQp)*(1 << QP_FRACTIONAL_BITS);
}

/*------------------------------------------------------------------------------
  VCEncBeforePicRc()  Update virtual buffer, and calculate picInitQp for current
  picture , and coded status.
------------------------------------------------------------------------------*/
void VCEncBeforePicRc(vcencRateControl_s *rc, u32 timeInc, u32 sliceType, bool use_ltr_cur, struct sw_picture* pic)
{
  rcVirtualBuffer_s *vb = &rc->virtualBuffer;
  i32 i, rcWindow,intraBits = 0, tmp = 0, qp_move = 0;
  bool bTargetBitsExtremSmall = HANTRO_FALSE;

  rc->frameCoded = ENCHW_YES;
  rc->sliceTypeCur = sliceType;

  if(rc->hierarchial_bit_allocation_GOP_size_previous!=rc->hierarchial_bit_allocation_GOP_size)
  {
    init_hierarchial_rc (rc);
    rc->hierarchial_bit_allocation_GOP_size_previous=rc->hierarchial_bit_allocation_GOP_size;
    rc->hierarchial_bit_allocation_total_weight=0;
    for(i=0;i<rc->hierarchial_bit_allocation_GOP_size;i++)
    {
      rc->hierarchial_bit_allocation_total_weight+=rc->hierarchial_bit_allocation_map[rc->hierarchial_bit_allocation_GOP_size-1][i];
      //printf("rc->hierarchial_bit_allocation_map[%d]=%d\n",i,rc->hierarchial_bit_allocation_map[i]);
    }
  }
  //printf("rc->hierarchial_bit_allocation_total_weight=%d\n",rc->hierarchial_bit_allocation_total_weight);

  //printf("rc->encoded_frame_number=%d\n",rc->encoded_frame_number);
  
  DBG(1, (DBGOUTPUT, "\nBEFORE PIC RC: pic=%d\n", rc->frameCnt));
  DBG(1, (DBGOUTPUT, "Frame type:\t%8i  timeInc:\t%8i\n",
          sliceType, timeInc));

  tmp = VirtualBuffer(&rc->virtualBuffer, (i32) timeInc, rc->hrd);
  //printf("\n VirtualBuffer delta=%d\n",tmp);
  for (i = 0; i < CHECK_POINTS_MAX; i++)
  {
    rc->qpCtrl.wordCntTarget[i] = 0;
  }

  if (vb->windowRem == 0)
  {
    vb->windowRem = rc->windowLen - 1;
    /* New bitrate window, reset error counters. */
    for(i=0;i<4;i++)
    {
      //rc->windowLen need to be adjusted  for different model.
      update_rc_error(&rc->rError[i], 0x7fffffff, rc->windowLen);
    }
     /* New bitrate window, reset error counters. */
    //update_rc_error(&rc->rError_BFrame, 0x7fffffff, 18/*rc->windowLen*/);
    /* Don't reset intra error in case of intra-only, it would cause step. */
    if (rc->sliceTypeCur != rc->sliceTypePrev)
      update_rc_error(&rc->intraError, 0x7fffffff, rc->windowLen);
    
  }
  else
  {
    vb->windowRem--;
  }
  //printf("rc->encoded_frame_number=%d\n",rc->encoded_frame_number);
  /* Calculate target size for this picture. Adjust the target bitPerPic
   * with the cumulated error between target and actual bitrates (tmp).
   * Also take into account the bits used by intra frame starting the GOP. */
  if (rc->sliceTypeCur != I_SLICE &&
      rc->intraInterval > 1)
  {
    /* GOP bits that are used by intra frame. Amount of bits
     * "stolen" by intra from each inter frame in the GOP. */
    intraBits = vb->bitPerPic * rc->intraInterval * get_avg_bits(&rc->gop, 10) / 100;
    intraBits -= vb->bitPerPic;
    intraBits /= (rc->intraInterval - 1);
    intraBits = MAX(0, intraBits);
    
  }

  /* Compensate the virtual bit buffer for intra frames "stealing" bits
   * from inter frames because after intra frame the bit buffer seems
   * to overflow but the following inters will level the buffer. */
  tmp += intraBits * (rc->intraInterval - rc->intraIntervalCtr);

#ifdef RC_WINDOW_STRICT
  /* In the end of window don't be too strict with matching the error
   * otherwise the end of window tends to twist QP. */
  rcWindow = MAX(MAX(3, rc->windowLen / 6), vb->windowRem);
  //printf("\n rcWindow=%d\n",rcWindow);
#else
  /* Actually we can be fairly easy with this one, let's make it
   * a moving window to smoothen the changes. */
  rcWindow = MAX(1, rc->windowLen);
#endif
  if(rc->sliceTypeCur!=I_SLICE)
  {

    if(rc->encoded_frame_number>=1)
    {
      float sum=0;
      for(i=0;i<rc->encoded_frame_number;i++)
      {
        
        sum+=vb->bitPerPic*rc->hierarchial_bit_allocation_GOP_size*(float)rc->hierarchial_bit_allocation_map[rc->hierarchial_bit_allocation_GOP_size-1][i];
      }
      //sum=sum/rc->hierarchial_bit_allocation_total_weight;
      sum=sum*reciprocal[rc->hierarchial_bit_allocation_total_weight];
      tmp+=sum-vb->bitPerPic*rc->encoded_frame_number;
    }
    
    //rc->targetPicSize = ((vb->bitPerPic - intraBits + DIV(tmp, rcWindow) + qp_move)*rc->hierarchial_bit_allocation_GOP_size*(float)rc->hierarchial_bit_allocation_map[rc->hierarchial_bit_allocation_GOP_size-1][rc->gopPoc]/rc->hierarchial_bit_allocation_total_weight);
    rc->targetPicSize = ((vb->bitPerPic - intraBits + DIV(tmp, rcWindow) + qp_move)*rc->hierarchial_bit_allocation_GOP_size*(float)rc->hierarchial_bit_allocation_map[rc->hierarchial_bit_allocation_GOP_size-1][rc->gopPoc]*reciprocal[rc->hierarchial_bit_allocation_total_weight]);
    //printf("GOP frame targetPicSize=%d\n",rc->targetPicSize);

    /* try to catch scene change */
    //previous MUST not I Slice
    if(rc->sliceTypePrev != I_SLICE)
    {
      //double dIntraPercentage = rc->intraCu8Num/(double)(rc->picArea/8/8);
      //disable the simple scenechane detect
      //if((dIntraPercentage > 0.35) && 0)
      if(0)
      {
        //printf("SceneChange detected: IntraPercentage=%f, targetPicSize from %d to %d\n", dIntraPercentage, rc->targetPicSize, 2*rc->targetPicSize);
        if(rc->targetPicSize > 0)
        {
            rc->targetPicSize *= 2;
        }
      }
    }
  }
  else
  {
    //printf("I frame bitPerPic=%d\n",vb->bitPerPic);
    rc->targetPicSize = vb->bitPerPic - intraBits + DIV(tmp, rcWindow) + qp_move;
  }
  /* Limit the target to a realistic minimum that can be reached.
   * Setting target lower than this will confuse RC because it can never
   * be reached. Frame with only skipped mbs == 96 bits. */// carl 96 should be changed to what number
  bTargetBitsExtremSmall = ((96 + rc->ctbRows * rc->ctbSize) > rc->targetPicSize)? HANTRO_TRUE: HANTRO_FALSE;
  rc->targetPicSize = MAX(96 + rc->ctbRows * rc->ctbSize, rc->targetPicSize);
  //printf("\n vb->bitPerPic=%d, rc->targetPicSize=%d bTargetBitsExtremSmall=%d\n", vb->bitPerPic, rc->targetPicSize, bTargetBitsExtremSmall);
  DBG(1, (DBGOUTPUT, "intraRatio: %3i%%\tintraBits: %7i\tbufferComp: %7i\n",
          get_avg_bits(&rc->gop, 10), intraBits, DIV(tmp, rcWindow)));
  DBG(1, (DBGOUTPUT, "WndRem: %4i  ", vb->windowRem));
  if (rc->sliceTypeCur == I_SLICE)
  {
    DBG(1, (DBGOUTPUT, "Rd: %6d  ", avg_rc_error(&rc->intraError)));
  }
  else
  {
    //DBG(1, (DBGOUTPUT, "Rd: %6d  ", avg_rc_error(&rc->rError)));
  }
  DBG(1, (DBGOUTPUT, "Tr: %7d  ", rc->targetPicSize));
  if (rc->sliceTypeCur == I_SLICE)
  {
    DBG(1, (DBGOUTPUT, "Td: %7d\n",
            CLIP3(rc->targetPicSize - avg_rc_error(&rc->intraError),
                  0, 2 * rc->targetPicSize)));
  }
  else
  {
    //DBG(1, (DBGOUTPUT, "Td: %7d\n",
    //        CLIP3(rc->targetPicSize - avg_rc_error(&rc->rError),
    //              0, 2 * rc->targetPicSize)));
  }

  if (rc->picSkip)
    PicSkip(rc);

  /* determine initial quantization parameter for current picture */
  PicQuant(rc);
  if(bTargetBitsExtremSmall && (rc->qpHdr < rc->qpHdrPrev)&&(rc->sliceTypeCur!=I_SLICE) && 1)
  {
      rc->qpHdr = MAX(rc->qpHdrPrevGop, rc->qpHdr);
  }
  /* quantization parameter user defined limitations */
  PicQuantLimit(rc);

  if (rc->sliceTypeCur == I_SLICE)
  {
    if (rc->fixedIntraQp)
      rc->qpHdr = rc->fixedIntraQp;
    else if (rc->sliceTypePrev != I_SLICE)
    {
      if(IS_CTBRC_FOR_QUALITY(rc->ctbRc))
        rc->qpHdr += rc->intraQpDelta + (((i32)MAX(rc->complexity, 9)- 17)<<8) - (INTRA_QPDELTA_DEFAULT << QP_FRACTIONAL_BITS);
      else
        rc->qpHdr += rc->intraQpDelta;

      if (rc->hrd == ENCHW_YES) 
      {
        if(rc->qpHdr<rc->minIQp)
        {
          rc->qpHdr=rc->minIQp;
        }
      }

      if( rc->picRc && (rc->ma_char.count != 0))
      {
        MaChar(&rc->ma_char);      
      
        //double dPBFrame4NRdCost = rc->ma_char.average_PBFrame4NRdCost/(double)(rc->picArea/8/8);
        //double dIntraPercentage = rc->ma_char.average_intraCu8Num/(double)(rc->picArea/8/8);
        //double dSkipCu8Percentage =  rc->ma_char.average_skipCu8Num/(double)(rc->picArea/8/8);

        /*Static scene: adj. I Frame qpOffset to smooth bits allocation*/
        //if(dPBFrame4NRdCost <=10 && dIntraPercentage<= 0.0016)
        if((i32)rc->ma_char.average_PBFrame4NRdCost <= (rc->picArea>>6)*10 && rc->ma_char.average_intraCu8Num <= (rc->picArea>>6) * 0.0016 && (!rc->inputSceneChange))
        {
            //user set u32StaticSceneIbitPercent to try to control static scene I-Frame bits percentage
            if(rc->u32StaticSceneIbitPercent)
            {
                bool bAdd = HANTRO_FALSE;
                bool bSub = HANTRO_FALSE;
                u32 u32IBitRation = (u32)((((unsigned long long)rc->u32PrevIBitCnt*rc->outRateNum)/rc->outRateDenom/rc->intraInterval)*100/rc->virtualBuffer.bitRate);
                if(u32IBitRation > (rc->u32StaticSceneIbitPercent + 15) )
                {
                    rc->qpHdr = rc->qpHdrPrevI + (2 << QP_FRACTIONAL_BITS);
                    bAdd = HANTRO_TRUE;
                }
                else if(u32IBitRation > (rc->u32StaticSceneIbitPercent + 5) )
                {
                    rc->qpHdr = rc->qpHdrPrevI + (1 << QP_FRACTIONAL_BITS);
                    bAdd = HANTRO_TRUE;
                }
                else if(u32IBitRation >= (rc->u32StaticSceneIbitPercent-5) )
                    rc->qpHdr = rc->qpHdrPrevI;
                else if((u32IBitRation > (rc->u32StaticSceneIbitPercent - 15)) && (rc->qpHdr >= rc->qpHdrPrevI))
                {
                    rc->qpHdr = rc->qpHdrPrevI - (1 << QP_FRACTIONAL_BITS);
                    bSub = HANTRO_TRUE;
                }
                else if(rc->qpHdr >= (rc->qpHdrPrevI - (1 << QP_FRACTIONAL_BITS)))
                {
                    rc->qpHdr = rc->qpHdrPrevI - (2 << QP_FRACTIONAL_BITS);
                    bSub = HANTRO_TRUE;
                }
                if(((rc->virtualBuffer.realBitCnt - rc->virtualBuffer.virtualBitCnt) > rc->virtualBuffer.bitRate) && (bAdd == HANTRO_FALSE))
                    rc->qpHdr = rc->qpHdr + (1 << QP_FRACTIONAL_BITS);
                else if(((rc->virtualBuffer.virtualBitCnt - rc->virtualBuffer.realBitCnt) > rc->virtualBuffer.bitRate) && (bSub == HANTRO_FALSE))
                    rc->qpHdr = rc->qpHdr - (1 << QP_FRACTIONAL_BITS);
            }
        }
      }
    }

    if (rc->intraIntervalCtr > 1)
      rc->intraInterval = rc->intraIntervalCtr;
    rc->intraIntervalCtr = 1;
  }
  else
  {
    rc->intraIntervalCtr++;

    /* Check that interval is repeating */
    if (rc->intraIntervalCtr > rc->intraInterval)
      rc->intraInterval = rc->intraIntervalCtr;
#if 1
    if(rc->sliceTypeCur != I_SLICE)
    {
         //printf("model b qp=%d\n ",rc->qpHdr);
         //B frm not referenced
         if (rc->picRc != ENCHW_YES)
            rc->qpHdr += rc->frameQpDelta;
         if (use_ltr_cur)
           rc->qpHdr += rc->longTermQpDelta;
         if (rc->qpHdr > (51<<QP_FRACTIONAL_BITS))
           rc->qpHdr = (51<<QP_FRACTIONAL_BITS);
         //printf("final  b qp=%d\n ",rc->qpHdr);
    }
#endif    
  }

  /* quantization parameter user defined limitations */
  PicQuantLimit(rc);
    
  if (rc->pass == 1)
  {
    rc->qpHdr = CU_TREE_QP << QP_FRACTIONAL_BITS;
  }

  if (rc->pass == 2 && rc->crf >= 0)
  {
    crfPicQuant(rc, pic);
  }

  /* Store the start QP, before ROI adjustment */
  rc->qpHdrPrev = rc->qpHdr;

  /* mb rate control (check point rate control) */
   //CTB_RC
#if 0
    if (rc->ctbRc)
    {
      //MbQuant(rc);
      if(rc->seqStart)
      {
        rc->bitsRatio = ((unsigned long long)rc->targetPicSize<<16)/rc->ctbPerPic;
        //printf("rc->targetPicSize=%d,rc->ctbPerPic=%d,rc->bitsRatio=0x%x\n",rc->targetPicSize,rc->ctbPerPic,rc->bitsRatio);
        rc->ctbRcThrdMin = rc->ctbRcThrdMax = 0;
        rc->seqStart = 0;
      }
      else
      {
        u32 avg = rc->ctbRctotalLcuBit/rc->ctbPerPic;
        if(rc->ctbRctotalLcuBit==0)
        {
          u16* ptr = (u16*)rc->ctbMemCurVirtualAddr;
          i32 i;
          rc->ctbRctotalLcuBit = rc->ctbPerPic;
          for(i=0;i<rc->ctbPerPic;i++)
          {
            ptr[i] = 1;
          }
        }

        rc->bitsRatio = ((unsigned long long)rc->targetPicSize<<16)/rc->ctbRctotalLcuBit;
        rc->ctbRcThrdMin = MIN(0xffff,   (((unsigned long long)(rc->ctbRcBitsMin + (avg-rc->ctbRcBitsMin)/5)*rc->bitsRatio)>>16));
        rc->ctbRcThrdMax = MIN(0xffff,   (((unsigned long long)(rc->ctbRcBitsMax - (rc->ctbRcBitsMax-avg)/5)*rc->bitsRatio)>>16));

      }
    }
#endif
  /* reset counters */
  rc->qpSum = 0;
  rc->qpLastCoded = rc->qpHdr;
  rc->qpTarget = rc->qpHdr;
  rc->nonZeroCnt = 0;

  DBG(0, (DBGOUTPUT, "Frame coded\t%8d  ", rc->frameCoded));
  DBG(0, (DBGOUTPUT, "Frame qpHdr\t%8d  ", rc->qpHdr));
  DBG(0, (DBGOUTPUT, "GopRem:\t%8d  ", vb->windowRem));
  DBG(0, (DBGOUTPUT, "Target bits:\t%8d  ", rc->targetPicSize));
  DBG(1, (DBGOUTPUT, "\nintraBits:\t%8d  ", intraBits));
  DBG(1, (DBGOUTPUT, "bufferComp:\t%8d  ", DIV(tmp, rcWindow)));
  //DBG(1, (DBGOUTPUT, "Rd:\t\t%8d\n", avg_rc_error(&rc->rError)));

  for (i = 0; i < CHECK_POINTS_MAX; i++)
  {
    DBG(1, (DBGOUTPUT, "CP %i  mbNum %4i  wTarg %5i\n", i,
            (rc->qpCtrl.checkPointDistance * (i + 1)),
            rc->qpCtrl.wordCntTarget[i] * 32));
  }

}

/*------------------------------------------------------------------------------

  MbQuant()

------------------------------------------------------------------------------*/
void MbQuant(vcencRateControl_s *rc)
{
  i32 nonZeroTarget;

  /* Disable Mb Rc for Intra Slices, because coeffTarget will be wrong */
  if (rc->sliceTypeCur == I_SLICE ||
      rc->srcPrm == 0)
  {
    return;
  }

  /* Required zero cnt */
  nonZeroTarget = rcCalculate(rc->targetPicSize, 256, rc->srcPrm);
  nonZeroTarget = MIN(rc->coeffCntMax, MAX(0, nonZeroTarget));

  nonZeroTarget = MIN(0x7FFFFFFFU / 1024U, (u32)nonZeroTarget);

  rc->virtualBuffer.nonZeroTarget = nonZeroTarget;

  /* Use linear model when previous frame can't be used for prediction */
  if ((rc->sliceTypeCur != rc->sliceTypePrev) || (rc->nonZeroCnt == 0))
  {
    LinearModel(rc);
  }
  else
  {
    AdaptiveModel(rc);
  }
}

/*------------------------------------------------------------------------------

  LinearModel()

------------------------------------------------------------------------------*/
void LinearModel(vcencRateControl_s *rc)
{
  const i32 sscale = 256;
  hevcQpCtrl_s *qc = &rc->qpCtrl;
  i32 scaler;
  i32 i;
  i32 tmp, nonZeroTarget = rc->virtualBuffer.nonZeroTarget;

  ASSERT(nonZeroTarget < (0x7FFFFFFF / sscale));

  if (nonZeroTarget > 0)
  {
    scaler = rcCalculate(nonZeroTarget, sscale, (i32) rc->ctbPerPic * rc->ctbSize * rc->ctbSize / 16 / 16);
  }
  else
  {
    return;
  }

  DBG(1, (DBGOUTPUT, " Linear Target: %8d prevCnt:\t %6d Scaler:\t %6d\n",
          nonZeroTarget, rc->nonZeroCnt, scaler / sscale));

  for (i = 0; i < rc->qpCtrl.checkPoints; i++)
  {
    tmp = (scaler * (qc->checkPointDistance * (i + 1) + 1)) / sscale;
    tmp = MIN(WORD_CNT_MAX, tmp / 32 + 1);
    if (tmp < 0) tmp = WORD_CNT_MAX;    /* Detect overflow */
    qc->wordCntTarget[i] = tmp; /* div32 for regs */
  }

  /* calculate nz count for avg. bits per frame */
  tmp = rcCalculate(rc->virtualBuffer.bitPerPic, rc->ctbSize * rc->ctbSize, rc->srcPrm);

  DBG(1, (DBGOUTPUT, "Error Limit:\t %8d SrcPrm:\t %6d\n",
          tmp, rc->srcPrm / 256));

  qc->wordError[0] = -tmp * 3;
  qc->qpChange[0] = -3;
  qc->wordError[1] = -tmp * 2;
  qc->qpChange[1] = -2;
  qc->wordError[2] = -tmp * 1;
  qc->qpChange[2] = -1;
  qc->wordError[3] = tmp * 1;
  qc->qpChange[3] = 0;
  qc->wordError[4] = tmp * 2;
  qc->qpChange[4] = 1;
  qc->wordError[5] = tmp * 3;
  qc->qpChange[5] = 2;
  qc->wordError[6] = tmp * 4;
  qc->qpChange[6] = 3;

  for (i = 0; i < CTRL_LEVELS; i++)
  {
    tmp = qc->wordError[i];
    tmp = CLIP3(-32768, 32767, tmp / 4);
    qc->wordError[i] = tmp;
  }
}


/*------------------------------------------------------------------------------

  AdaptiveModel()

------------------------------------------------------------------------------*/
void AdaptiveModel(vcencRateControl_s *rc)
{
  const i32 sscale = rc->ctbSize * rc->ctbSize;
  hevcQpCtrl_s *qc = &rc->qpCtrl;
  i32 i;
  i32 tmp, nonZeroTarget = rc->virtualBuffer.nonZeroTarget;
  i32 scaler;

  ASSERT(nonZeroTarget < (0x7FFFFFFF / sscale));

  if ((nonZeroTarget > 0) && (rc->nonZeroCnt > 0))
  {
    scaler = rcCalculate(nonZeroTarget, sscale, rc->nonZeroCnt);
  }
  else
  {
    return;
  }
  DBG(1, (DBGOUTPUT, "Adaptive Target: %8d prevCnt:\t %6d Scaler:\t %6d\n",
          nonZeroTarget, rc->nonZeroCnt, scaler / sscale));

  for (i = 0; i < rc->qpCtrl.checkPoints; i++)
  {
    tmp = (i32)(qc->wordCntPrev[i] * scaler) / sscale;
    tmp = MIN(WORD_CNT_MAX, tmp / 32 + 1);
    if (tmp < 0) tmp = WORD_CNT_MAX;    /* Detect overflow */
    qc->wordCntTarget[i] = tmp; /* div32 for regs */
    DBG(2, (DBGOUTPUT, " CP %i  wordCntPrev %6i  wordCntTarget_div32 %6i\n",
            i, qc->wordCntPrev[i], qc->wordCntTarget[i]));
  }

  /* Qp change table */

  /* calculate nz count for avg. bits per frame */
  tmp = rcCalculate(rc->virtualBuffer.bitPerPic, rc->ctbSize * rc->ctbSize, (rc->srcPrm * 3));

  DBG(1, (DBGOUTPUT, "Error Limit:\t %8d SrcPrm:\t %6d\n",
          tmp, rc->srcPrm / (rc->ctbSize * rc->ctbSize)));

  qc->wordError[0] = -tmp * 3;
  qc->qpChange[0] = -3;
  qc->wordError[1] = -tmp * 2;
  qc->qpChange[1] = -2;
  qc->wordError[2] = -tmp * 1;
  qc->qpChange[2] = -1;
  qc->wordError[3] = tmp * 1;
  qc->qpChange[3] = 0;
  qc->wordError[4] = tmp * 2;
  qc->qpChange[4] = 1;
  qc->wordError[5] = tmp * 3;
  qc->qpChange[5] = 2;
  qc->wordError[6] = tmp * 4;
  qc->qpChange[6] = 3;

  for (i = 0; i < CTRL_LEVELS; i++)
  {
    tmp = qc->wordError[i];
    tmp = CLIP3(-32768, 32767, tmp / 4);
    qc->wordError[i] = tmp;
  }
}

/*------------------------------------------------------------------------------

  SourceParameter()  Source parameter of last coded frame. Parameters
  has been scaled up by factor 256.

------------------------------------------------------------------------------*/
void SourceParameter(vcencRateControl_s *rc, i32 nonZeroCnt)
{
  //ASSERT(rc->qpSum <= 51 * rc->ctbPerPic * rc->ctbSize * rc->ctbSize / 16 / 16);
  ASSERT(nonZeroCnt <= rc->coeffCntMax);
  ASSERT(nonZeroCnt >= 0 && rc->coeffCntMax >= 0);

  /* AVOID division by zero */
  if (nonZeroCnt == 0)
  {
    nonZeroCnt = 1;
  }

  rc->srcPrm = rcCalculate(rc->frameBitCnt, rc->ctbSize * rc->ctbSize, nonZeroCnt);

  DBG(1, (DBGOUTPUT, "nonZeroCnt %6i, srcPrm %i\n",
          nonZeroCnt, rc->srcPrm / (rc->ctbSize * rc->ctbSize)));

}

/*------------------------------------------------------------------------------
  PicSkip()  Decrease framerate if not enough bits available.
------------------------------------------------------------------------------*/
void PicSkip(vcencRateControl_s *rc)
{
  rcVirtualBuffer_s *vb = &rc->virtualBuffer;
  i32 bitAvailable = vb->virtualBitCnt - vb->realBitCnt;
  i32 skipIncLimit = -vb->bitPerPic / 3;
  i32 skipDecLimit = vb->bitPerPic / 3;

  /* When frameRc is enabled, skipFrameTarget is not allowed to be > 1
   * This makes sure that not too many frames is skipped and lets
   * the frameRc adjust QP instead of skipping many frames */
  if (((rc->picRc == ENCHW_NO) || (vb->skipFrameTarget == 0)) &&
      (bitAvailable < skipIncLimit))
  {
    vb->skipFrameTarget++;
  }

  if ((bitAvailable > skipDecLimit) && vb->skipFrameTarget > 0)
  {
    vb->skipFrameTarget--;
  }

  if (vb->skippedFrames < vb->skipFrameTarget)
  {
    vb->skippedFrames++;
    rc->frameCoded = ENCHW_NO;
  }
  else
  {
    vb->skippedFrames = 0;
  }
}

/*------------------------------------------------------------------------------
  PicQuant()  Calculate quantization parameter for next frame. In the beginning
                of window use previous GOP average QP and otherwise find new QP
                using the target size and previous frames QPs and bit counts.
------------------------------------------------------------------------------*/
void PicQuant(vcencRateControl_s *rc)
{
  i32 normBits, targetBits;
  true_e useQpDeltaLimit = ENCHW_YES;
  i32 i, predictOrder,predictBits,predictQP_Err,bestIQP,bestBits,bestI,bitError,total_count;
  i32 bpp_accurate;
  i32 movingBitMax,movingBitMin,movingBitBase;
  i32 algNum=rc->hierarchial_alg_map[rc->hierarchial_bit_allocation_GOP_size-1][rc->gopPoc];
  bestI = 0;

  if (rc->picRc != ENCHW_YES)
  {
    rc->qpHdr = rc->fixedQp;
    DBG(1, (DBGOUTPUT, "R/cx:  xxxx  QP: xx xx D:  xxxx newQP: xx\n"));
    return;
  }

  /* If HRD is enabled we must make sure this frame fits in buffer */
  if (rc->hrd == ENCHW_YES)
  {
    i32 bitsAvailable =
      (rc->virtualBuffer.bufferSize - rc->virtualBuffer.bucketFullness);

    /* If the previous frame didn't fit the buffer we don't limit QP change */
    if (rc->frameBitCnt > bitsAvailable)
    {
      useQpDeltaLimit = ENCHW_NO;
    }
  }

  /* determine initial quantization parameter for current picture */
  if (rc->sliceTypeCur == I_SLICE)
  {
    /* Default intra QP == average of prev frame and prev GOP average */
    rc->qpHdrPrevGop = gop_avg_qp(rc);
    rc->qpHdr = (rc->qpHdrPrev + rc->qpHdrPrevGop)/2;
    
    /* reset intra QP if scene changed */
    if (rc->resetIntraPicQp || rc->inputSceneChange)
    {
      i32 resetQp = InitialQp(rc->virtualBuffer.bitPerPic, rc->picArea);
      rc->qpHdr = MAX(resetQp, rc->qpHdr);
    }

    i32 pass2Reset = (rc->frameCnt == 0) && (rc->pass == 2) ;
    if (pass2Reset) useQpDeltaLimit = ENCHW_NO;
      
    /* If all frames are intra we calculate new QP for intra the same way as for inter */
    if (rc->sliceTypePrev == I_SLICE || pass2Reset)
    {
      if(rc->codingType != ASIC_JPEG)
      {
        targetBits = rc->targetPicSize - avg_rc_error(&rc->intraError);
        targetBits = CLIP3(0, 2 * rc->targetPicSize, targetBits);
        normBits = rcCalculate(targetBits, DSCBITPERMB, rc->ctbPerPic * rc->ctbSize * rc->ctbSize / 16 / 16);
        rc->qpHdr = new_pic_quant(&rc->intra, rc, normBits, useQpDeltaLimit,(u32)rc->complexity,0,0,0);

        if (pass2Reset)
        {
          for (i = 0; i < 4; i ++)
            rc->rcPred[i].qp = rc->qpHdr;
        }
      }
      else
      {
        useQpDeltaLimit = ENCHW_NO;
        targetBits = rc->targetPicSize - avg_rc_error_jpeg(&rc->intraError);
        targetBits = CLIP3(rc->minPicSizeI , 2 * rc->targetPicSize, targetBits);
        normBits = rcCalculate(targetBits, DSCBITPERMB, rc->ctbPerPic * rc->ctbSize * rc->ctbSize / 16 / 16);
        rc->qpHdr = new_pic_quant(&rc->intra, rc, normBits, useQpDeltaLimit,(u32)rc->complexity,0,0,0);
        DBG(1, (DBGOUTPUT, "rc_qp (%d, %d, 0x%x) rc_er %d %d <-> %d %d %d  \n", targetBits, rc->qpHdr>>QP_FRACTIONAL_BITS, rc->qpHdr, 
                                      rc->targetPicSize,  avg_rc_error_jpeg(&rc->intraError), 
                                      rc->intraError.bits[2], rc->intraError.bits[1], rc->intraError.bits[0]));                                              
      }
    }
    else
    {
      DBG(1, (DBGOUTPUT, "R/cx:  xxxx  QP: xx xx D:  xxxx newQP: xx\n"));
    }
  }
  else
  {    
    i32 algNum = rc->hierarchial_alg_map[rc->hierarchial_bit_allocation_GOP_size-1][rc->gopPoc];
    double weight = rc->hierarchial_bit_allocation_GOP_size *
                    (float)rc->hierarchial_bit_allocation_map[rc->hierarchial_bit_allocation_GOP_size-1][rc->gopPoc] *
                    reciprocal[rc->hierarchial_bit_allocation_total_weight];

    movingBitBase = MIN(((((i64)rc->virtualBuffer.bitRate * rc->ma.frameRateDenom) / rc->ma.frameRateNumer)* weight),rc->i32MaxPicSize);
    movingBitBase = (i32)MIN(rc->u32PFrameByQPStep,(u32)movingBitBase);
    movingBitMin = movingBitBase / 10;

    //movingBitMax=((float)rc->virtualBuffer.maxBitRate*rc->ma.frameRateDenom/rc->ma.frameRateNumer)*rc->ma.count-moving_totalBit_window_minus1(&rc->ma);
    //movingBitMax=(movingBitMax+movingBitBase+1)/2;
    movingBitMax = MIN((((i64)rc->virtualBuffer.maxBitRate * rc->ma.frameRateDenom) / rc->ma.frameRateNumer),rc->i32MaxPicSize);
    movingBitMax = (i32)(frame_moving_bits(&rc->ma, movingBitMax, rc->monitorFrames) * weight);
    movingBitMax = MAX(movingBitMin, movingBitMax);

    rc->targetPicSize=CLIP3(movingBitMin, movingBitMax, rc->targetPicSize);
    if(rc->sliceTypeCur == P_SLICE)
      rc->targetPicSize=CLIP3(rc->minPicSizeP, rc->maxPicSizeP, rc->targetPicSize);
    else
      rc->targetPicSize=CLIP3(rc->minPicSizeB, rc->maxPicSizeB, rc->targetPicSize);

    /* Calculate new QP by matching to previous inter frames R-Q curve */
    targetBits = rc->targetPicSize - avg_rc_error(&rc->rError[algNum]);
    targetBits = CLIP3(rc->targetPicSize/10, rc->targetPicSize*2, targetBits);
    normBits = rcCalculate(targetBits, DSCBITPERMB, rc->ctbPerPic * rc->ctbSize * rc->ctbSize / 16 / 16);
    rc->qpHdr = new_pic_quant(&rc->linReg[algNum], rc, normBits, useQpDeltaLimit,(u32)rc->complexity,rc->linReg[algNum].frameBitCntLast,rc->linReg[algNum].targetPicSizeLast,rc->targetPicSize);
    if (rc->hrd == ENCHW_YES) 
    {
      //printf("rc->qpHdr=%x,rc->minIQp=%x\n",rc->qpHdr,rc->minIQp);
      if(rc->qpHdr<(rc->minIQp-(3<<QP_FRACTIONAL_BITS)))
        rc->qpHdr=rc->minIQp-(3<<QP_FRACTIONAL_BITS);
    }
  }
}

/*------------------------------------------------------------------------------

  PicQuantLimit()

------------------------------------------------------------------------------*/
void PicQuantLimit(vcencRateControl_s *rc)
{
  rc->qpHdr = MIN(rc->qpMax, MAX(rc->qpMin, rc->qpHdr));
}

/*------------------------------------------------------------------------------

  Calculate()  I try to avoid overflow and calculate good enough result of a*b/c

------------------------------------------------------------------------------*/
i32 rcCalculate(i32 a, i32 b, i32 c)
{
  u32 left = 32;
  u32 right = 0;
  u32 shift;
  i32 sign = 1;
  i32 tmp;

  if (a == 0 || b == 0)
  {
    return 0;
  }
  //else if ((a * b / b) == a && c != 0)
  else if ((i64)a*(i64)b < (1LL<<31) && c != 0)
  {
    return (a * b / c);
  }
  if (a < 0)
  {
    sign = -1;
    a = -a;
  }
  if (b < 0)
  {
    sign *= -1;
    b = -b;
  }
  if (c < 0)
  {
    sign *= -1;
    c = -c;
  }

  if (c == 0)
  {
    return 0x7FFFFFFF * sign;
  }

  if (b > a)
  {
    tmp = b;
    b = a;
    a = tmp;
  }

  for (--left; (((u32)a << left) >> left) != (u32)a; --left);
  left--; /* unsigned values have one more bit on left,
               we want signed accuracy. shifting signed values gives
               lint warnings */

  while (((u32)b >> right) > (u32)c)
  {
    right++;
  }

  if (right > left)
  {
    return 0x7FFFFFFF * sign;
  }
  else
  {
    shift = left - right;
    return (i32)((((u32)a << shift) / (u32)c * (u32)b) >> shift) * sign;
  }
}
#if 1

/*------------------------------------------------------------------------------
  avg_rc_error()  PI(D)-control for rate prediction error.
------------------------------------------------------------------------------*/
static i32 avg_rc_error(linReg_s *p)
{
  if (ABS(p->bits[2]) < 0xFFFFFFF && ABS(p->bits[1]) < 0xFFFFFFF)
    return DIV(p->bits[2] * 8 + p->bits[1] * 4 + p->bits[0] * 0, 100);

  /* Avoid overflow */
  return rcCalculate(p->bits[2], 8, 100) +
         rcCalculate(p->bits[1], 4, 100);
}

/*------------------------------------------------------------------------------
  update_overhead()  Update PI(D)-control values
------------------------------------------------------------------------------*/
static void update_rc_error(linReg_s *p, i32 bits, i32 windowLen)
{
  p->len = 3;

  if (bits == (i32)0x7fffffff)
  {
    /* RESET */
    p->bits[0] = 0;
    if (windowLen)  /* Store the average error of previous GOP. */
      p->bits[1] = p->bits[1] / windowLen;
    else
      p->bits[1] = 0;
    p->bits[2] = 0;
    return;
  }
  p->bits[0] = bits - p->bits[2]; /* Derivative */
  if ((bits > 0) && (bits + p->bits[1] > p->bits[1]))
    p->bits[1] = bits + p->bits[1]; /* Integral */
  if ((bits < 0) && (bits + p->bits[1] < p->bits[1]))
    p->bits[1] = bits + p->bits[1]; /* Integral */
  p->bits[2] = bits;              /* Proportional */
  DBG(1, (DBGOUTPUT, "P %6d I %7d D %7d\n", p->bits[2],  p->bits[1], p->bits[0]));
}
#else
/*------------------------------------------------------------------------------
  avg_rc_error()  PI(D)-control for rate prediction error.
------------------------------------------------------------------------------*/
static i32 avg_rc_error(linReg_s *p)
{
  /* Avoid overflow */
  i32 i;
  i32 biterror=0;
  for(i=0;i<p->len;i++)
  {
    biterror += p->bits[i];
  }
  return biterror/p->len;
}

/*------------------------------------------------------------------------------
  update_overhead()  Update PI(D)-control values
------------------------------------------------------------------------------*/
static void update_rc_error(linReg_s *p, i32 bits, i32 windowLen)
{
  p->len = 4;

  if (bits == (i32)0x7fffffff)
  {
    /* RESET */
    p->bits[0] = 0;
    p->bits[1] = 0;
    p->bits[2] = 0;
    p->bits[3] = 0;
    p->bits[4] = 0;
    p->pos=0;
    return;
  }
  p->bits[p->pos++] = bits; 
  if(p->pos==p->len)
    p->pos=0;
}

#endif

/*------------------------------------------------------------------------------
  avg_rc_error()  PI(D)-control for rate prediction error.
------------------------------------------------------------------------------*/
static i32 avg_rc_error_jpeg(linReg_s *p)
{
  if (ABS(p->bits[2]) < 0xFFFFFFF && ABS(p->bits[1]) < 0xFFFFFFF)
    return DIV(p->bits[2] * 8 + p->bits[1] * 4 + p->bits[0] * 0, 12);

  /* Avoid overflow */
  return rcCalculate(p->bits[2], 8, 10) +
         rcCalculate(p->bits[1], 4, 10);
}

/*------------------------------------------------------------------------------
  gop_avg_qp()  Average quantization parameter of P frames since previous I.
------------------------------------------------------------------------------*/
i32 gop_avg_qp(vcencRateControl_s *rc)
{
  i32 tmp = rc->qpHdrPrev;
  i32 maxIntraBitRatio = 95;  /* Percentage of total GOP bits. */

  rc->gopLastBitCnt=rc->gopBitCnt;

  /* Average QP of previous GOP inter frames */  
  if ((rc->pass==2) && rc->gopPQpSum && rc->gopPQpNum)
    tmp = DIV(rc->gopPQpSum, rc->gopPQpNum);
  else if (rc->gopQpSum && rc->gopQpDiv)
  {
    tmp = DIV(rc->gopQpSum, rc->gopQpDiv);
  }
  
  /* Average bit count per frame for previous GOP (intra + inter) */
  rc->gopAvgBitCnt = DIV(rc->gopBitCnt, (rc->gopQpDiv + 1));
  /* Ratio of intra_frame_bits/all_gop_bits % for previous GOP */
  if (rc->gopBitCnt)
  {
    i32 gopIntraBitRatio =
      rcCalculate(get_avg_bits(&rc->intra, 1), rc->ctbPerPic * rc->ctbSize * rc->ctbSize / 16 / 16, DSCBITPERMB) * 100;
    gopIntraBitRatio = DIV(gopIntraBitRatio, rc->gopBitCnt);
    /* Limit GOP intra bit ratio to leave room for inters. */
    gopIntraBitRatio = MIN(maxIntraBitRatio, gopIntraBitRatio);
    update_tables(&rc->gop, tmp, gopIntraBitRatio);
  }
  rc->gopQpSum = 0;
  rc->gopQpDiv = 0;
  rc->gopBitCnt = 0;
  rc->gopPQpSum = 0;
  rc->gopPQpNum = 0;

  return tmp;
}

/*------------------------------------------------------------------------------
  new_pic_quant()  Calculate new quantization parameter from the 2nd degree R-Q
  equation. Further adjust Qp for "smoother" visual quality.
------------------------------------------------------------------------------*/
static i32 new_pic_quant(linReg_s *p, vcencRateControl_s *rc, i32 bits, true_e useQpDeltaLimit,u32 complexity,i32 lastActualBits, i32 lastTargetBits, i32 currentTargetBits)
{
  i64 diff_prev = 0, tmp = 0, diff, diff_best = 0x7FFFFFFFFF;

  i32 qp_prev = 0, qp_best = p->qp_prev, qp = p->qp_prev;
  i32 resetQp = 0;

  DBG(1, (DBGOUTPUT, "R/cx:%6d ", bits));

  if (rc->pass == 2) 
  {
    i32 idc = getPredId(rc);
    qp = getPass2Qp(rc);
    p->qp_prev = (rc->rcPred[idc].qp >= 0) ? rc->rcPred[idc].qp : qp;

    double bits = predictSize(&(rc->rcPred[idc]), qp2qScale((qp>>QP_FRACTIONAL_BITS)*1.0), rc->pass1CurCost);
    if (bits > 0) rc->targetPicSize = bits;

    if ((idc == B_SLICE) || (idc == 3))
      useQpDeltaLimit = ENCHW_NO;
    goto new_pic_quant_end;
  }

  if (p->a1 == 0 && p->a2 == 0)
  {
    //printf("p->a1 == 0 && p->a2 == 0\n");
    DBG(1, (DBGOUTPUT, " QP: xx xx D:  ==== newQP: %2d\n", qp));
    p->qp_prev = qp;
    return qp;
  }

    /* Find the qp that has the best match on fitted curve */
  do {
      tmp  = DIV(p->a1, q_step[((qp*10)>>QP_FRACTIONAL_BITS)]);
      tmp += DIV(p->a2, (i64)q_step[((qp*10)>>QP_FRACTIONAL_BITS)] * q_step[((qp*10)>>QP_FRACTIONAL_BITS)]);
      //diff = ABS(tmp*p->weight/10 - (bits << QP_FRACTIONAL_BITS));
      diff = ABS(tmp - ((i64)bits << QP_FRACTIONAL_BITS));
     
      if (diff < diff_best) {
          if (diff_best == 0x7FFFFFFFFF) {
              diff_prev = diff;
              qp_prev   = qp;
          } else {
              diff_prev = diff_best;
              qp_prev   = qp_best;
          }
          diff_best = diff;
          qp_best   = qp;
          if ((tmp - (bits << QP_FRACTIONAL_BITS)) <= 0) {

              qp -= QP_FIXPOINT_0_POINT_1;
              if (qp < 0) {
                  qp_best =  0;
                  break;
              }
          } else {
              qp += QP_FIXPOINT_0_POINT_1;

              if (qp >(51 << QP_FRACTIONAL_BITS)) {
                  qp_best = (51 << QP_FRACTIONAL_BITS);
                break;
             }
          }
      } else {
          break;
      }
  }
  while ((qp >= 0) && (qp <= (51 << QP_FRACTIONAL_BITS)));
  
  qp = qp_best;
  
  if(lastActualBits && lastTargetBits)
  {
#if 1
    /* This condition means that R(Q) history conflict with R(Q) of last frame. 
           Just make Qp same as that of last frame. */
    if (((currentTargetBits >= lastActualBits) && (qp > p->qp_prev)) ||
        ((currentTargetBits <= lastActualBits) && (qp < p->qp_prev)))
      qp = p->qp_prev;
#else
    if(currentTargetBits>=lastTargetBits)
    {
      if(lastActualBits <= lastTargetBits)
      {
        if(qp<=p->qp_prev)
        {
          //printf("correct qp result\n");
        }
        else
        {
          //printf("error qp result\n");
          qp = p->qp_prev;
        }
      }
    }
    if(currentTargetBits <= lastTargetBits)
    {
      if(lastActualBits >= lastTargetBits)
      {
        if(qp >= p->qp_prev)
        {
          //printf("correct qp result\n");
        }
        else
        {
          //printf("error qp result\n");
          qp = p->qp_prev;
        }
      }
    }
#endif
  }

new_pic_quant_end:

  qp = CLIP3(0, 0x3300, qp);
  DBG(1,(DBGOUTPUT, " QP: %2d %2d D: %5d", qp, qp_prev, (i32)(diff_prev - diff_best)));
  DBG(1, (DBGOUTPUT, " newQP: %2d\n", qp));

  if (rc->inputSceneChange)
    resetQp = InitialQp(rc->virtualBuffer.bitPerPic, rc->picArea);

  /* Limit Qp change for smoother visual quality */
  if (useQpDeltaLimit)
  {
    i32 minQp = p->qp_prev + (rc->picQpDeltaMin << QP_FRACTIONAL_BITS);
    i32 maxQp = p->qp_prev + (rc->picQpDeltaMax << QP_FRACTIONAL_BITS);
    i32 delta = qp - p->qp_prev;
    tmp = ABS(delta);

    //scale qpDelta
    if(tmp > ((1<<QP_FRACTIONAL_BITS) + (1<<(QP_FRACTIONAL_BITS-1))))
      tmp >>= 1;
    else if(tmp <= (1<<(QP_FRACTIONAL_BITS-2)))
      tmp = tmp;    
    else
      tmp = tmp *2 /3;
    
    qp = (delta >= 0) ? (p->qp_prev + tmp) : (p->qp_prev - tmp);
    qp = MAX(qp, resetQp);
    qp = CLIP3(minQp, maxQp, qp);
  }
  else
  {
    qp = MAX(qp, resetQp);
  }

  if ((rc->codingType!= ASIC_JPEG) && (rc->codingType!= ASIC_MJPEG) && (IS_CTBRC_FOR_QUALITY(rc->ctbRc) == 0))
    qp = ((qp + (1 << (QP_FRACTIONAL_BITS-1))) >> QP_FRACTIONAL_BITS) << QP_FRACTIONAL_BITS;

  return qp;
}

/*------------------------------------------------------------------------------
  get_avg_bits()
------------------------------------------------------------------------------*/
static i32 get_avg_bits(linReg_s *p, i32 n)
{
  i32 i;
  i64 sum = 0;
  i32 pos = p->pos;

  if (!p->len) return 0;

  if (n == -1 || n > p->len)
    n = p->len;

  i = n;
  while (i--)
  {
    if (pos) pos--;
    else pos = p->len - 1;
    sum += p->bits[pos];
    if (sum < 0)
    {
      return I64_MAX / (n - i);
    }
  }
  return DIV(sum, n);
}

/*------------------------------------------------------------------------------
  update_tables()  only statistics of PSLICE, please.
------------------------------------------------------------------------------*/
static void update_tables(linReg_s *p, i32 qp, i32 bits)
{
  const i32 clen = RC_TABLE_LENGTH;
  i32 tmp = p->pos;

  p->qp_prev   = qp;
  p->qs[tmp]   = q_step[((qp*10)>>QP_FRACTIONAL_BITS)];
  p->bits[tmp] = bits;

  if (++p->pos >= clen)
  {
    p->pos = 0;
  }
  if (p->len < clen)
  {
    p->len++;
  }
}

/*------------------------------------------------------------------------------
            update_model()  Update model parameter by Linear Regression.
------------------------------------------------------------------------------*/
static void update_model(linReg_s *p)
{
  i32 i, n = p->len,*r = p->bits;
  i32 *qs = p->qs ;
  i64  a1, a2, sx = lin_sx(qs, n), sy = lin_sy(qs, r, n);
  
  for (i = 0; i < n; i++)
  {
    DBG(2, (DBGOUTPUT, "model: qs %i  r %i\n", qs[i], r[i]));
  }

  a1 = lin_sxy(qs, r, n);
  //printf("lin_sxy a1=%d,n=%d\n",a1,n);
  a1 = (a1 < (I64_MAX / n)) ? (a1 * n) : I64_MAX;
  //printf("\n r[0]=%d,sy=%d,sx=%d,a1=%d\n",r[0],sy,sx,a1);
  if (sy == 0)
  {
    a1 = 0;
    //printf("if a1=0");
  }
  else
  {
    a1 -= (sx < (I64_MAX / sy)) ? (sx * sy) : I64_MAX;
    //printf("else a1=%d",a1);
  }
  //printf("before a2 calc, a1=%d\n",a1);
  a2 = (lin_nsxx(qs, n) - (sx * sx));
  //printf("a2=%d\n",a2);
  if (a2 == 0)
  {
    if (p->a1 == 0)
    {
      /* If encountered in the beginning */
      a1 = 0;
    }
    else
    {
      a1 = (p->a1 * 2) / 3;
    }
  }
  else
  {
    //printf("a1=%d,a2=%d\n",a1,a2);
    a1 = a1*DSCY/a2;
    //printf("a1*DSCY_64/a2=%d\n",a1);
  }
  //printf("before ceiling a1=%d\n",a1);
  /* Value of a1 shouldn't be excessive (small) */
   a1 = MAX(a1, ((i64)-262144*256*256));      //????
   a1 = MIN(a1, ((i64)262144 * 256 * 256 - 1));      //????

  ASSERT(ABS(a1) * sx >= 0);
  ASSERT(sx * DSCY >= 0);
  a2 = DIV((sy * DSCY), n) - DIV((a1 * sx), n);

  DBG(2, (DBGOUTPUT, "model: a2:%9d  a1:%8d\n", (i32)a2, (i32)a1));

  if (p->len > 0)
  {
    p->a1 = a1;
    p->a2 = a2;
  }
  //printf("rc update_model p->a1=%ld,p->a2=%ld\n",p->a1,p->a2);
}

/*------------------------------------------------------------------------------
  lin_sy()  calculate value of Sy for n points.
------------------------------------------------------------------------------*/
static i64 lin_sy(i32 *qp, i32 *r, i32 n)
{
  i64 sum = 0;

  while (n--)
  {
    sum += (i64)qp[n] * qp[n] * (r[n] << QP_FRACTIONAL_BITS);
    if (sum < 0)
    {
      return I64_MAX / DSCY;
    }
  }
  return (DIV(sum, DSCY));
}

/*------------------------------------------------------------------------------
  lin_sx()  calculate value of Sx for n points.
------------------------------------------------------------------------------*/
static i64 lin_sx(i32 *qp, i32 n)
{
  i64 tmp = 0;

  while (n--)
  {
    ASSERT(qp[n]);
    tmp += qp[n];
  }
  return tmp;
}

/*------------------------------------------------------------------------------
  lin_sxy()  calculate value of Sxy for n points.
------------------------------------------------------------------------------*/
static i64 lin_sxy(i32 *qp, i32 *r, i32 n)
{
  i64 tmp, sum = 0;

  while (n--)
  {
    tmp = (i64)qp[n] * (i64)qp[n] * (i64)qp[n];
    //tmp =>> (QP_FRACTIONAL_BITS*3);
    if (tmp > (r[n] << (3*QP_FRACTIONAL_BITS)))
    {
      sum += DIV(tmp, DSCY) * (r[n] << (QP_FRACTIONAL_BITS));
    }
    else
    {
      sum += tmp * DIV((r[n] << (QP_FRACTIONAL_BITS)), DSCY);
    }
    if (sum < 0)
    {
      return I64_MAX;
    }
  }
  return sum;
}

/*------------------------------------------------------------------------------
  lin_nsxx()  calculate value of n * Sxy for n points.
------------------------------------------------------------------------------*/
static i64 lin_nsxx(i32 *qp, i32 n)
{
  i64 d = n;
  i64 tmp = 0, sum = 0;

  while (n--)
  {
    tmp = qp[n];
    tmp *= tmp;
    sum += d * tmp;
  }
  return sum;
}

static void MaAddFrame(rc_ma_s *ma, i32 frameSizeBits)
{
    ma->frame[ma->pos++] = frameSizeBits;

    if (ma->pos == ma->length)
        ma->pos = 0;

    if (ma->count < ma->length)
        ma->count++;
}

/*------------------------------------------------------------------------------
    Calculate average bitrate of moving window
------------------------------------------------------------------------------*/
static i32 Ma(rc_ma_s *ma)
{
    i32 i;
    unsigned long long sum = 0;     /* Using 64-bits to avoid overflow */

    for (i = 0; i < ma->count; i++)
    {
        sum += ma->frame[i];
        //printf("i=%d,ma->frame[i]=%d\n",i,ma->frame[i]);
    }

    if (!ma->frameRateDenom)
        return 0;

    
    sum = sum / ma->count;


    return sum * (ma->frameRateNumer+ma->frameRateDenom-1) / ma->frameRateDenom;
}

static void MaCharAddFrame(rc_ma_char *ma, u32 intraCu8Num, u32 skipCu8Num, u32 PBFrame4NRdCost)
{
  ma->intraCu8Num[ma->pos] = intraCu8Num;
  ma->skipCu8Num[ma->pos] = skipCu8Num;
  ma->PBFrame4NRdCost[ma->pos] = PBFrame4NRdCost;

  ma->pos++;

  if (ma->pos == ma->length)
      ma->pos = 0;
  
  if (ma->count < ma->length)
      ma->count++;
}

static void MaChar(rc_ma_char *ma)
{
    i32 i;
    unsigned long long sum0 = 0, sum1 = 0, sum2 = 0;     

    for (i = 0; i < ma->count; i++)
    {
        sum0 += ma->intraCu8Num[i];
        sum1 += ma->skipCu8Num[i];
        sum2 += ma->PBFrame4NRdCost[i];        
    }
    
    ma->average_intraCu8Num = sum0 / ma->count;
    ma->average_skipCu8Num= sum1 / ma->count;
    ma->average_PBFrame4NRdCost= sum2 / ma->count;    
}

/*------------------------------------------------------------------------------
    Calculate average bitrate of moving window -1
------------------------------------------------------------------------------*/
static i32 moving_totalBit_window_minus1(rc_ma_s *ma)
{
    i32 i=0;
    i32 sum = 0;     /* Using 64-bits to avoid overflow */
    i32 readptr;
    readptr=ma->pos-1;
    if(readptr<0)
      readptr=ma->length-1;
    while(1)
    {
      if(i==ma->count-1)
        break;
      sum += ma->frame[readptr];
      readptr--;
      if(readptr<0)
      readptr=ma->length-1;
      i++;
    }
    return sum;
}

/*------------------------------------------------------------------------------
    Calculate the minimum remaining bits per frame for all ongoing slide windows
------------------------------------------------------------------------------*/
static i32 frame_moving_bits(rc_ma_s *ma, i32 maxBits, i32 winLen)
{
  i32 i = 0;
  i32 sum = 0;
  i32 len = winLen - 1;
  i32 bits = maxBits;
  i32 totalBits = maxBits * winLen;
  i32 pos = ma->pos;
  i32 tmp;
  //len = MIN(len, ma->count);

  for (i = len; i > 0; i --)
  {
    if (ma->count < (len-i+1))
      break;

    pos --;
    if (pos < 0)
      pos = ma->length - 1;

    sum += ma->frame[pos];
    tmp = (totalBits - sum) / i;
    bits = MIN(tmp, bits);
  }
  bits = MAX(100, bits);
  return bits;
}

/******************************************************************
  -Pass2 RC
*******************************************************************/
void initFramePredictors(vcencRateControl_s *rc)
{
    rc->rcPred[B_SLICE].coeff = 0.2;
    rc->rcPred[P_SLICE].coeff = 0.8;
    rc->rcPred[I_SLICE].coeff = 1.2;
    rc->rcPred[3      ].coeff = 0.35;
    for (int i = 0; i < 4; i++)
    {
        rc->rcPred[i].coeffMin = rc->rcPred[i].coeff / 4;
        rc->rcPred[i].count = 1.0;
        rc->rcPred[i].decay = 0.5;
        rc->rcPred[i].offset = 0.0;
        rc->rcPred[i].qp = -1;
    }
}

void updatePredictor(rcPredictor *p, double q, double var, double bits, i32 qp)
{
    if (var < 10)
        return;
    const double range = 2;
    double old_coeff = p->coeff / p->count;
    double old_offset = p->offset / p->count;
    double new_coeff = MAX((bits * q - old_offset) / var, p->coeffMin );
    double new_coeff_clipped = CLIP3(old_coeff / range, old_coeff * range, new_coeff);
    double new_offset = bits * q - new_coeff_clipped * var;
    if (new_offset >= 0)
        new_coeff = new_coeff_clipped;
    else
        new_offset = 0;
    p->count  *= p->decay;
    p->coeff  *= p->decay;
    p->offset *= p->decay;
    p->count++;
    p->coeff  += new_coeff;
    p->offset += new_offset;
    p->qp = qp;
}

double predictSize(rcPredictor *p, double q, double var)
{
    return (p->coeff * var + p->offset) / (q * p->count);
}

double qScale2qp(double qScale)
{
    return 12.0 + 6.0 * (double)X265_LOG2(qScale / 0.85);
}

double qp2qScale(double qp)
{
    return 0.85 * pow(2.0, (qp - 12.0) / 6.0);
}

double getNormQp (i32 qp)
{
   return qp * 1.0 / (1 << QP_FRACTIONAL_BITS);
}

i32 getPredId(vcencRateControl_s *rc)
{
   return rc->predId;
    
   i32 idc = rc->sliceTypeCur;

   if (rc->sliceTypeCur == B_SLICE)
   {
      if ((i32)rc->gopPoc == (rc->hierarchial_bit_allocation_GOP_size - 1))
      {
        idc = P_SLICE;
      }
      else if ((rc->hierarchial_bit_allocation_GOP_size > 2) &&
               ((i32)rc->gopPoc == (rc->hierarchial_bit_allocation_GOP_size/2 -1 )))
      {
        idc = 3;  //bref
      }
   }
   
   return idc;
}

double predictWindowSize(vcencRateControl_s *rc, double *costs, i32 *frameNum, i32 q, double targetBits)
{
  i32 i, frms = 0;
  double bits = 0, diff = 0;

  if (rc->sliceTypeCur == I_SLICE)
  {
    double bitsIntra = 0, bitsInter = 0;
    i32 frmsInter = 0, frmsIntra = 0;
    for (i = 0; i < 4; i++)
    {
      frms += frameNum[i];
      if (frameNum[i])
      {
        double tmpBits = predictSize(&(rc->rcPred[i]), qp2qScale(q*1.0), costs[i]) * frameNum[i];
        if (i == I_SLICE)
        {
          bitsIntra += tmpBits;
          frmsIntra += frameNum[i];
        }
        else
        {
          bitsInter += tmpBits;
          frmsInter += frameNum[i];
        }
      }
    }

    //TODO: explicit setting intraInterval by API, not initialized by bitrateWindowSize
    if (frmsInter && (frmsInter < frmsIntra * (rc->intraInterval - 1)))
    {
      if (frmsIntra) bits = bitsIntra / frmsIntra;
      bits += (bitsInter / frmsInter) * (rc->intraInterval - 1);
      frms = rc->intraInterval;
    }
    else
      bits = bitsIntra + bitsInter;
  }
  else
  {
    for (i = 0; i < 4; i++)
    {
      if (frameNum[i] && (i != I_SLICE))
      {
        frms += frameNum[i];
        bits += predictSize(&(rc->rcPred[i]), qp2qScale(q*1.0), costs[i]) * frameNum[i];
      }
    }
  }

  targetBits *= frms;
  if (targetBits) diff = bits - targetBits;

  return diff;
}

i32 predictQp(vcencRateControl_s *rc, double *costs, i32 *frameNum, double targetBits)
{
  double deltaBits;
  double diff, diff0;
  i32 i;
  i32 q = (rc->sliceTypeCur == I_SLICE) ? rc->rcPred[I_SLICE].qp : rc->rcPred[P_SLICE].qp;
  q = (q < 0) ? 26 : (q>>QP_FRACTIONAL_BITS);

  deltaBits = predictWindowSize(rc, costs, frameNum, q, targetBits);
  diff0 = ABS(deltaBits);
  if (diff0 == 0) return q;
  
  i32 step = (deltaBits > 0) ? 1 : -1;
  for (q = q + step; (q >= 0) && (q <= 51); q += step)
  {
    deltaBits = predictWindowSize(rc, costs, frameNum, q, targetBits);
    diff = ABS(deltaBits);
    if (diff >= diff0)
    {
      q -= step;
      break;
    }
    diff0 = diff;
  }

  q = CLIP3(0, 51, q);
  return q;
}

i32 getPass2Qp(vcencRateControl_s *rc)
{
  i32 q;
  i32 idc = getPredId(rc);
  i32 bSlice = (idc == B_SLICE) || (idc == 3);
  i32 firstP = (idc == P_SLICE) && (rc->frameCnt == 1);

  if (bSlice)
  {
    q = rc->rcPred[P_SLICE].qp;
    goto get_pass2_qp_end;
  }

  q  = predictQp(rc, rc->pass1AvgCost, rc->pass1FrameNum, rc->targetPicSize);
  if (idc == P_SLICE)
  {
    i32 shortQp = predictQp(rc, rc->pass1GopCost, rc->pass1GopFrameNum, rc->targetPicSize);    
    q = (q >= shortQp) ? q : (q + shortQp + 1) >> 1;
  }

  q <<= QP_FRACTIONAL_BITS;  
  if (bSlice || firstP)
  {
    q = MAX(q, rc->rcPred[P_SLICE].qp);
  }

get_pass2_qp_end:

#if 0
  for (i = 0; i < 4; i ++)
  {
    printf ("================= predictor %d coeff = %f \n", i, rc->rcPred[i].coeff);
  }
#endif

  return q;
}

#if RATECONTROL

//extern fetch_output fw_param;
RateControl* pRC;
RCLCU* pRCLCU;
#if HEVC_RC
static RCPreviousPictures lastlevelpic[2];
static RCPreviousPictures lastpic;
list<RCPreviousPictures*> m_piclist;
#endif
double QueryTable[13][13] = {
    { -4.80, -4.80, -4.80, -4.80, -3.57, -3.57, -3.17, -3.17, -2.00, -2.00, -0.25, -0.25, 0.00, },
    { -4.80, -4.80, -4.80, -4.80, -3.57, -3.57, -3.17, -3.17, -2.00, -2.00, -0.25, -0.25, 0.00, },
    { -4.80, -4.80, -3.57, -3.57, -3.57, -3.57, -2.00, -2.00, -1.10, -1.10, 0.00, 0.00, 0.25, },
    { -4.80, -4.80, -3.57, -3.57, -3.57, -3.57, -2.00, -2.00, -1.10, -1.10, 0.00, 0.00, 0.25, },
    { -3.57, -3.57, -3.57, -3.57, -2.00, -2.00, -1.10, -1.10, 0.00, 0.00, 1.10, 1.10, 2.00, },
    { -3.57, -3.57, -3.57, -3.57, -2.00, -2.00, -1.10, -1.10, 0.00, 0.00, 1.10, 1.10, 2.00, },
    { -3.17, -3.17, -2.00, -2.00, -1.10, -1.10, 0.00, 0.00, 1.10, 1.10, 2.00, 2.00, 3.17, },
    { -3.17, -3.17, -2.00, -2.00, -1.10, -1.10, 0.00, 0.00, 1.10, 1.10, 2.00, 2.00, 3.17, },
    { -2.00, -2.00, -1.10, -1.10, 0.00, 0.00, 1.10, 1.10, 2.00, 2.00, 3.57, 3.57, 3.57, },
    { -2.00, -2.00, -1.10, -1.10, 0.00, 0.00, 1.10, 1.10, 2.00, 2.00, 3.57, 3.57, 3.57, },
    { -0.25, -0.25, 0.00, 0.00, 1.10, 1.10, 2.44, 2.44, 3.57, 3.57, 3.57, 3.57, 4.80, },
    { -0.25, -0.25, 0.00, 0.00, 1.10, 1.10, 2.44, 2.44, 3.57, 3.57, 3.57, 3.57, 4.80, },
    { 0.00, 0.00, 0.25, 0.25, 2.00, 2.00, 3.57, 3.57, 3.86, 3.86, 4.80, 4.80, 4.80, },
};

char RateControlQueryTable[13][13];

void Init_FuzzyController(double ScaFactor)
{
    int i, j;
    for (i = 0; i < 13; i++)
        for (j = 0; j < 13; j++) {
            RateControlQueryTable[i][j] = QueryTable[i][j] > 0.0F ? (char)ceil(QueryTable[i][j] * ScaFactor) : (char)floor(
                QueryTable[i][j] * ScaFactor);
        }
}

int  GetNewFrameDeltaQP_FuzzyController(double ActualValue, double DeltaValue, double Amax, double Amin, double Bmax,
    double Bmin)
{
    double dFuzAct, dFuzDel;
    int iFuzAct, iFuzDel, ConVal;

    dFuzAct = (12.0 / (Amax - Amin)) * (ActualValue - (Amax + Amin) / 2.0);
    dFuzDel = (12.0 / (Bmax - Bmin)) * (DeltaValue - (Bmax + Bmin) / 2.0);

    dFuzAct = fabs(dFuzAct) > 6 ? 6.0 * dFuzAct / fabs(dFuzAct) : dFuzAct;
    dFuzDel = fabs(dFuzDel) > 6 ? 6.0 * dFuzDel / fabs(dFuzDel) : dFuzDel;

    iFuzAct = (int)((dFuzAct < 0 ? floor(dFuzAct + 0.5) : ceil(dFuzAct - 0.5)) + 6);
    iFuzDel = (int)((dFuzDel < 0 ? floor(dFuzDel + 0.5) : ceil(dFuzDel - 0.5)) + 6);

    ConVal = RateControlQueryTable[iFuzAct][iFuzDel];
    return ConVal;
}
#define STARVPP_ALIGN(x, a)     (((x) + ((a) - 1)) & (~((a) - 1)))
int align_stride(int x, int align, int disalign)
{
    x = STARVPP_ALIGN(x, align);
    if (!(x & (disalign - 1))) {
        x += align;
    }

    return x;
}

int align_plane_size(int x, int disalign)
{
    if (!(x & (disalign - 1))) {
        x += 128;
    }

    return x;
}
extern FRAME_LEVEL_FW frame_level_fw;
extern Slice_Level_Parameters ctu_ctrl_input;
void Init_RateControl(RateControl* prc)
{
    int i, j;
    //RateControl *prc;
    //RCLCU       RClcu;
    //RateControl RC;
    int w = frame_level_fw.pic_width;
    int h = frame_level_fw.pic_height;
    int IniQP = (frame_level_fw.EncControl == 0 || frame_level_fw.ECInitialQP == 0) ? frame_level_fw.qp_y : frame_level_fw.ECInitialQP;

    //pRCLCU = &RClcu;
    //memset(pRCLCU, 0, sizeof(RCLCU));
    //if (fw_param.EncControl == 1 && input->intra_period > 1 && input->useDQP == 1) {
    //	input->useDQP = 0;
    //	printf("LCU Rate Control does not support RA. DeltaQP is reset to 0. \n");
    //}

    //pRC = &RC;
    //memset(pRC, 0, sizeof(RateControl));

    //prc->prclcu = pRCLCU;
    prc->RConoff = frame_level_fw.EncControl;
    prc->LCURConoff = frame_level_fw.useDQP;
    prc->IntraPeriod = frame_level_fw.intra_period;
    prc->TotalFrames = frame_level_fw.TotalFrames;
    prc->CodedFrameNumber = 0;
    prc->ImageQP = prc->RcQP = prc->RcMBQP = IniQP;
    prc->FrameWidth = frame_level_fw.pic_width;
    prc->FrameHeight = frame_level_fw.pic_height;
    prc->GopBpp = 0.0F;
    prc->GopAllKeyBpp = 0.0F;
    prc->GopAvgBaseQP = 0;
    prc->GopAvgBaseQPCount = 0;
    prc->TargetBitrate = frame_level_fw.TargetBitRate;
    prc->bpp = (double)(1.0) * frame_level_fw.TargetBitRate / frame_level_fw.framerate / w / h;
    prc->TargetBufferLevel = 0.0F;
    prc->FirstBufferSizeLevel = 0.0F;
    prc->PreBufferError = prc->BufferError = 0.0F;
    prc->CurrentBufferSizeBpp = 0.0;
    prc->LowestBufferSizeBpp = prc->HighestBufferSizeBpp = 0.0F;
    prc->GopFlag = -100;
    prc->DeltaQP = 0;
    frame_level_fw.intra_period == 1 ? Init_FuzzyController(0.50) : Init_FuzzyController(0.25);

#if HEVC_RC

    prc->bitsRatio[0] = 1;
    prc->bitsRatio[1] = 1;
    prc->bitsRatio[2] = 1;
    prc->bitsRatio[3] = 1;

    prc->GOPLevel[0] = 1;
    prc->GOPLevel[1] = 1;
    prc->GOPLevel[2] = 1;
    prc->GOPLevel[3] = 1;

    if (prc->bpp < 0.03) {
        prc->m_alphaUpdate = 0.01;
        prc->m_betaUpdate = 0.005;
    }
    else if (prc->bpp < 0.08) {
        prc->m_alphaUpdate = 0.05;
        prc->m_betaUpdate = 0.025;
    }
    else if (prc->bpp < 0.2) {
        prc->m_alphaUpdate = 0.1;
        prc->m_betaUpdate = 0.05;
    }
    else if (prc->bpp < 0.5) {
        prc->m_alphaUpdate = 0.2;
        prc->m_betaUpdate = 0.1;
    }
    else {
        prc->m_alphaUpdate = 0.4;
        prc->m_betaUpdate = 0.2;
    }

    for (i = 0; i < 5; i++) {
        if (i > 0) {
            prc->m_picPara[i].m_alpha = 3.2003;
            prc->m_picPara[i].m_beta = -1.250;
        }
        else {
#if rc_algorithm_10
            prc->m_picPara[i].m_alpha = ALPHA;
            prc->m_picPara[i].m_beta = BETA2;
#else
            prc->m_picPara[i].m_alpha = 3.2003;
            prc->m_picPara[i].m_beta = -1.250;
#endif

        }
    }
#if LCU_RATECONTROL
    prc->m_useLCUSeparateModel = true;
    Int picWidthInLCU = ((w % 32 == 0) ? (w / 32) : w / 32 + 1);
    Int picHeightInLCU = ((h % 32 == 0) ? (h / 32) : h / 32 + 1);
    prc->m_numberOfLCU = picWidthInLCU * picHeightInLCU;
    pRC->m_LCULeft = prc->m_numberOfLCU;
    if (prc->m_useLCUSeparateModel) {
        for (i = 0; i < 5; i++) {
            for (j = 0; j < 510; j++) {
                prc->m_LCUPara[i][j].m_alpha = prc->m_picPara[i].m_alpha;
                prc->m_LCUPara[i][j].m_beta = prc->m_picPara[i].m_beta;
            }
        }
    }

    //for (i= 0;i< prc->m_numberOfLCU;i++)
    for (i = 0; i < picHeightInLCU; i++)
    {
        for (j = 0; j < picWidthInLCU; j++)
        {
            int LCUIdx = i * picWidthInLCU + j;
            pRC->m_lcu[LCUIdx].m_actualBits = 0;
            pRC->m_lcu[LCUIdx].m_QP = 0;
            pRC->m_lcu[LCUIdx].m_lambda = 0.0;
            pRC->m_lcu[LCUIdx].m_targetBits = 0;
            pRC->m_lcu[LCUIdx].m_bitWeight = 1.0;
            Int currWidth = ((i == picWidthInLCU - 1) ? w - 32 * (picWidthInLCU - 1) : 32);
            Int currHeight = ((j == picHeightInLCU - 1) ? h - 32 * (picHeightInLCU - 1) : 32);
            pRC->m_lcu[LCUIdx].m_numberOfPixel = currWidth * currHeight;
        }
    }
#endif
#if U0132_TARGET_BITS_SATURATION
    prc->m_CpbSaturationEnabled = true;
    prc->m_cpbSize = prc->TargetBitrate * 0.3f;             // MaxCpbSize <= 240000000(proflie = main, level = 6.2, tier = main)
    prc->m_RCInitialCpbFullness = 0.9f;
    prc->m_cpbState = 0/*(UInt)(prc->m_cpbSize * prc->m_RCInitialCpbFullness)*/;
    prc->m_bufferingRate = (Int)(prc->TargetBitrate / img->framerate);
#endif
    prc->m_frameQp = prc->ImageQP;
    prc->m_frameLambda = 0;
    prc->m_numberOfPixel = prc->FrameWidth * prc->FrameHeight;
    prc->m_targetBits = (Int)(prc->TargetBitrate / frame_level_fw.framerate);
    if (prc->IntraPeriod == 1)
    {
        prc->m_SeqbitsLeft = prc->TotalFrames * prc->m_targetBits;
        prc->m_framesLeft = prc->TotalFrames;
    }
    else {
        prc->m_SeqbitsLeft = prc->IntraPeriod * prc->m_targetBits;
        prc->m_framesLeft = prc->IntraPeriod;
    }
    prc->m_adaptiveBit = 1;
    prc->iGOPid = 0;
    prc->numPic = 0;
    prc->m_totalactualbits = 0;
#endif
    int img_length = (prc->FrameWidth / 2 + VPP_PAD * 2) * (prc->FrameHeight / 2 + VPP_PAD * 2);
    prc->cur_yuv = (pel_t*)(malloc(img_length * sizeof(pel_t)));
    prc->ref_yuv = (pel_t*)(malloc(img_length * sizeof(pel_t)));

    memset(prc->ref_yuv, 0, img_length * sizeof(pel_t));
    memset(prc->cur_yuv, 0, img_length * sizeof(pel_t));

    //staravs_pixel_init(0, &pRC->pixf);

    //starvpp_me_init();

    //pRC->me.sad_8x8 = pRC->pixf.sad[1];
    //pRC->me.sad_8x8_x3 = pRC->pixf.sad_x3[1];
    //pRC->me.sad_8x8_x4 = pRC->pixf.sad_x4[1];
    //pRC->me.satd_8x8 = pRC->pixf.satd[1];
    //pRC->me.satd_8x8_x3 = pRC->pixf.satd_x3[1];
    //pRC->me.satd_8x8_x4 = pRC->pixf.satd_x4[1];

    //starvpp_vpp_intra_pred_init(0, pRC->intra_pred);

    pRC->short_term_cplxsum = 0;
    pRC->short_term_cplxcount = 0;
    pRC->blurred_complexity = 0;

#if rc_algorithm_3
    pRC->b_abr = 1;
    pRC->qcompress = 0.6;
#endif

    lastlevelpic[0].m_picQP = -1;
    lastlevelpic[0].m_framelevel = 0;
    lastlevelpic[0].m_picActualHeaderBits = 0;
    lastlevelpic[0].lastLevelLambda = -1;
    lastlevelpic[1].m_picQP = -1;
    lastlevelpic[1].m_framelevel = 0;
    lastlevelpic[1].m_picActualHeaderBits = 0;
    lastlevelpic[1].lastLevelLambda = -1;
    lastpic.m_picQP = -1;
    lastpic.m_framelevel = 0;
    lastpic.m_picActualHeaderBits = 0;
    lastpic.lastLevelLambda = -1;

#if rc_algorithm_6
    prc->target_bpp = (double)prc->TargetBitrate / frame_level_fw.framerate / w / h;
    prc->lastpic_bpp = prc->target_bpp;
    prc->current_pic_update = 1;
    prc->current_pic_reset_0 = 0;
#endif
    lastpic.imgtype = frame_level_fw.slice_type;

#if rc_algorithm_9
    for (i = 0; i < 64; i++) {
        prc->qp_num[i] = 0;
        prc->qp_val[i] = i;
    }
#endif
}

void Updata_RateControl(RateControl* prc, int framebits, int frameqp, int imgtype, int framenumber, int goplength)
{
    int LevelLength = 0;
    double PI = 3.1415926;
    prc->ImageQP = frameqp;
    prc->ImageType = imgtype;
    prc->GlobalImageNumber = framenumber;
    prc->ImageBits = framebits;
    prc->ImageBpp = (double)(1.0) * framebits / prc->FrameWidth / prc->FrameHeight;
    prc->IntraFrameBpp = imgtype == 0 ? prc->ImageBpp : prc->IntraFrameBpp;
    prc->InterFrameBpp = imgtype == 1 || imgtype == 4 || imgtype == 0 ? prc->ImageBpp : prc->InterFrameBpp;
    if (imgtype == 0 || imgtype == 4) {
        prc->GopAvgBaseQP = imgtype == 0 ? prc->RcQP : prc->GopAvgBaseQP + prc->RcQP - 1;
        prc->GopAllKeyBpp = imgtype == 0 ? prc->ImageBpp : prc->GopAllKeyBpp + prc->ImageBpp;
        prc->GopAvgBaseQPCount = imgtype == 0 ? 1 : prc->GopAvgBaseQPCount + 1;
    }

    prc->CodedFrameNumber++;

    if (imgtype != 2) {
        prc->GopBpp = prc->ImageBpp;
    }
    else {
        prc->GopBpp += prc->ImageBpp;
    }

    prc->CurrentBufferSizeBpp += prc->ImageBpp - prc->bpp;

    prc->HighestBufferSizeBpp = prc->CurrentBufferSizeBpp > prc->HighestBufferSizeBpp ? prc->CurrentBufferSizeBpp :
        prc->HighestBufferSizeBpp;
    prc->LowestBufferSizeBpp = prc->CurrentBufferSizeBpp < prc->LowestBufferSizeBpp ? prc->CurrentBufferSizeBpp :
        prc->LowestBufferSizeBpp;

    if (prc->IntraPeriod == 1) {
        prc->TargetBufferLevel = prc->DeltaBufferLevel = 0.0;
    }
    else if (prc->IntraPeriod == 0) {
        if (imgtype == 0) {
            prc->TargetBufferLevel = prc->CurrentBufferSizeBpp;
            prc->DeltaBufferLevel = prc->TargetBufferLevel / (prc->TotalFrames + MAX(0, MIN(150, (prc->TotalFrames - 150) / 3)));
        }
        else {
            prc->TargetBufferLevel = prc->TargetBufferLevel - prc->DeltaBufferLevel;
        }
    }
    else if (prc->IntraPeriod == 3 || prc->IntraPeriod == 2) {
        if (imgtype == 0 && prc->CodedFrameNumber < 2) {
            prc->FirstBufferSizeLevel = prc->CurrentBufferSizeBpp;
            prc->TargetBufferLevel = prc->CurrentBufferSizeBpp;
        }
        else {
            prc->TargetBufferLevel = prc->FirstBufferSizeLevel * cos(PI / 2 * prc->CodedFrameNumber / prc->TotalFrames);
        }
    }
    else {
        if (imgtype == 0 && prc->CodedFrameNumber < 2) {
            prc->TargetBufferLevel = prc->CurrentBufferSizeBpp;
            LevelLength = prc->CodedFrameNumber < 2 ? (prc->IntraPeriod - 1) * goplength : prc->IntraPeriod * goplength - 1;
            prc->DeltaBufferLevel = prc->TargetBufferLevel / (prc->IntraPeriod == 0 ? prc->TotalFrames : MIN(LevelLength,
                prc->TotalFrames - framenumber));
        }
        else if (imgtype == 0 && prc->CodedFrameNumber >= 2 &&
            ((prc->TotalFrames - prc->CodedFrameNumber) > (prc->IntraPeriod * goplength + goplength))) {
            prc->GopFlag = prc->CodedFrameNumber;
            prc->TargetBufferLevel = prc->DeltaBufferLevel = 0.0;
        }
        else if (prc->GopFlag == prc->CodedFrameNumber - goplength) {
            prc->TargetBufferLevel = prc->CurrentBufferSizeBpp;
            if ((prc->TotalFrames - prc->CodedFrameNumber) <= (prc->IntraPeriod * goplength)) {
                LevelLength = prc->TotalFrames - prc->CodedFrameNumber;
            }
            else {
                LevelLength = (prc->IntraPeriod - 1) * goplength - 1;
            }
            prc->DeltaBufferLevel = prc->TargetBufferLevel / LevelLength;
            prc->GopFlag = -100;
        }
        else if (imgtype == 0 && prc->CodedFrameNumber >= 2 &&
            ((prc->TotalFrames - prc->CodedFrameNumber) <= (prc->IntraPeriod * goplength + goplength))) {
            prc->TargetBufferLevel = prc->CurrentBufferSizeBpp;
            prc->DeltaBufferLevel = prc->TargetBufferLevel / (prc->TotalFrames - prc->CodedFrameNumber);
        }
        else {
            prc->TargetBufferLevel = prc->TargetBufferLevel - prc->DeltaBufferLevel;
        }
    }
}

int  CalculateGopDeltaQP_RateControl(RateControl* prc, int imgtype, int framenumber, int goplength)
{
    int deltaQP = 0;
    double BufferRange, deltaBufferRange;
    double Tbpp;
    prc->Belta = imgtype == 0 ? 0.12F : 0.12F;

    prc->BufferError = prc->CurrentBufferSizeBpp - prc->TargetBufferLevel;

    if ((prc->CodedFrameNumber % goplength == 1) || (pRC->IntraPeriod == 1)) {
        prc->BufferDifError = prc->BufferError - prc->PreBufferError;
        prc->PreBufferError = prc->BufferError;
    }
    if (prc->IntraPeriod == 1) {
        Tbpp = prc->IntraFrameBpp;
        BufferRange = Tbpp * prc->Belta * 2;
        BufferRange = prc->CodedFrameNumber < 2 ? Tbpp * 4 : BufferRange;
        BufferRange = BufferRange < 0.0001 ? 0.0001 : BufferRange;
        deltaBufferRange = BufferRange * 2;
        deltaQP = GetNewFrameDeltaQP_FuzzyController(prc->BufferError, prc->BufferDifError, BufferRange, -BufferRange,
            deltaBufferRange, -deltaBufferRange);
        prc->DeltaQP = deltaQP;
        return deltaQP;
    }

    if (prc->IntraPeriod == 0) {
        Tbpp = prc->InterFrameBpp;
        BufferRange = Tbpp * prc->Belta * 2;
        BufferRange = prc->CodedFrameNumber < 2 ? Tbpp * 4 : BufferRange;
        BufferRange = BufferRange < 0.0001 ? 0.0001 : BufferRange;
        deltaBufferRange = BufferRange * 2;
        deltaQP = GetNewFrameDeltaQP_FuzzyController(prc->BufferError, prc->BufferDifError, BufferRange, -BufferRange,
            deltaBufferRange, -deltaBufferRange);
        prc->DeltaQP = deltaQP;
        return deltaQP;
    }

    if (prc->IntraPeriod > 1) {
        if (imgtype == 0) {
            Tbpp = prc->GopAllKeyBpp;
            BufferRange = Tbpp * prc->Belta * 2;
            BufferRange = prc->CodedFrameNumber < 2 ? Tbpp * 4 : BufferRange;
            BufferRange = BufferRange < 0.0001 ? 0.0001 : BufferRange;
            deltaBufferRange = BufferRange * 2;
            deltaQP = GetNewFrameDeltaQP_FuzzyController(prc->BufferError, prc->BufferDifError, BufferRange, -BufferRange,
                deltaBufferRange, -deltaBufferRange);
            prc->DeltaQP = deltaQP;
            return deltaQP;
        }
        else {
            Tbpp = prc->GopBpp;
            BufferRange = Tbpp * prc->Belta * 2;
            BufferRange = prc->CodedFrameNumber < 2 ? Tbpp * 4 : BufferRange;
            BufferRange = BufferRange < 0.0001 ? 0.0001 : BufferRange;
            deltaBufferRange = BufferRange / 2;
            deltaQP = GetNewFrameDeltaQP_FuzzyController(prc->BufferError, prc->BufferDifError, BufferRange, -BufferRange,
                deltaBufferRange, -deltaBufferRange);
            prc->DeltaQP = deltaQP;
            return deltaQP;
        }
    }
    return 0; // never goes here
}

void Init_LCURateControl(RateControl* prc, int NumUnitsLCU)
{
    prc->prclcu->LCUTBL = prc->TargetBufferLevel;
    prc->prclcu->LCUBuffer = prc->CurrentBufferSizeBpp;
    prc->prclcu->LCUTBLdelta = prc->DeltaBufferLevel / NumUnitsLCU;
    prc->prclcu->LCUBufferError = 0;
    prc->prclcu->LCUBufferDifError = 0;
    prc->prclcu->LCUPreBufferError = 0;
}
int  CalculateLCUDeltaQP(RateControl* prc)
{
    int deltaQP = 0;
    double BufferRange, deltaBufferRange;
    double Tbpp;
    prc->Belta = 0.12F;

    if (prc->IntraPeriod <= 1) { // lcu level RC does not support RA now.
        Tbpp = prc->bpp;
        BufferRange = Tbpp * prc->Belta * 2;
        BufferRange = BufferRange < 0.0001 ? 0.0001 : BufferRange;
        deltaBufferRange = BufferRange * 2;
        deltaQP = GetNewFrameDeltaQP_FuzzyController(prc->prclcu->LCUBufferError, prc->prclcu->LCUBufferDifError, BufferRange,
            -BufferRange, deltaBufferRange, -deltaBufferRange);
    }
    return deltaQP;
}
void UpdataLCURateControl(RateControl* prc, int qp, double lambda, int bits, int NumLCU)
{
    prc->prclcu->PreBpp = prc->prclcu->CurBpp;
    prc->prclcu->CurBpp = 1.0 * bits / (prc->FrameHeight * prc->FrameWidth);
    prc->prclcu->LCUTBL -= prc->prclcu->LCUTBLdelta;
    prc->prclcu->LCUBuffer = prc->prclcu->LCUBuffer + prc->prclcu->CurBpp - prc->bpp / NumLCU;
    prc->prclcu->LCUBufferError = prc->prclcu->LCUBuffer - prc->prclcu->LCUTBL;
    prc->prclcu->LCUBufferDifError = prc->prclcu->LCUBufferError - prc->prclcu->LCUPreBufferError;
    prc->prclcu->LCUPreBufferError = prc->prclcu->LCUBufferError;
    prc->prclcu->LCUPreLCUQP = prc->prclcu->LCUCurLCUQP;
    prc->prclcu->LCUCurLCUQP = qp;
    prc->prclcu->LCUCurLambda = lambda;
    return;
}

#if HEVC_RC
Int xEstGOPTargetBits()
{
    Int realInfluencePicture = MIN(g_RCSmoothWindowSize, pRC->m_framesLeft);
    Int averageTargetBitsPerPic = (Int)(pRC->TargetBitrate / frame_level_fw.framerate);
    Int currentTargetBitsPerPic = (Int)((pRC->m_SeqbitsLeft - averageTargetBitsPerPic * (pRC->m_framesLeft - realInfluencePicture)) / realInfluencePicture);
    Int targetBits = currentTargetBitsPerPic * pRC->numPic;

    if (targetBits < 200) {
        targetBits = 200;   // at least allocate 200 bits for one GOP
    }

    return targetBits;
}
#endif

#if HEVC_RC
void xCalEquaCoeff(RateControl* prc, Double* lambdaRatio, Double* equaCoeffA, Double* equaCoeffB, Int GOPSize)
{
    for (Int i = 0; i < GOPSize; i++) {
        Int frameLevel = prc->GOPLevel[i];
        Double alpha = prc->m_picPara[frameLevel].m_alpha;
        Double beta = prc->m_picPara[frameLevel].m_beta;
        equaCoeffA[i] = pow(1.0 / alpha, 1.0 / beta) * pow(lambdaRatio[i], 1.0 / beta);
        equaCoeffB[i] = 1.0 / beta;
    }
}
#endif

#if HEVC_RC
Double xSolveEqua(Double targetBpp, Double* equaCoeffA, Double* equaCoeffB, Int GOPSize)
{
    Double solution = 100.0;
    Double minNumber = 0.1;
    Double maxNumber = 10000.0;
    for (Int i = 0; i < g_RCIterationNum; i++) {
        Double fx = 0.0;
        for (Int j = 0; j < GOPSize; j++) {
            fx += equaCoeffA[j] * pow(solution, equaCoeffB[j]);
        }

        if (fabs(fx - targetBpp) < 0.000001) {
            break;
        }

        if (fx > targetBpp) {
            minNumber = solution;
            solution = (solution + maxNumber) / 2.0;
        }
        else {
            maxNumber = solution;
            solution = (solution + minNumber) / 2.0;
        }
    }

    solution = Clip3(0.1, 10000.0, solution);
    return solution;
}
#endif

#if HEVC_RC
void setAllBitRatio(Double basicLambda, Double* equaCoeffA, Double* equaCoeffB)
{
    for (Int i = 0; i < 4; i++) {
        pRC->bitsRatio[i] = (Int)(equaCoeffA[i] * pow(basicLambda, equaCoeffB[i]) * pRC->FrameWidth * pRC->FrameHeight);
    }
}
#endif
#if 0
void downsampling_image(pel_t* cur_yuv, pel_t* ref_yuv) {
#define FILTER(a,b,c,d) ((((a + b + 1) >> 1) + ((c + d + 1) >> 1) + 1) >> 1)
    int x, y;

    for (y = 0; y < img->height / 2; y++) {
        for (x = 0; x < frame_level_fw.pic_width / 2; x++) {
            cur_yuv[(img->width / 2 + VPP_PAD * 2) * 8 + y * (img->width / 2 + VPP_PAD * 2) + x + VPP_PAD] = FILTER(img->imgY_org[2 * y][2 * x], img->imgY_org[2 * y + 1][2 * x], img->imgY_org[2 * y][2 * x + 1], img->imgY_org[2 * y + 1][2 * x + 1]);
            ref_yuv[(img->width / 2 + VPP_PAD * 2) * 8 + y * (img->width / 2 + VPP_PAD * 2) + x + VPP_PAD] = FILTER(img->ref_pixel_buffer[0][0][2 * y][2 * x], img->ref_pixel_buffer[0][0][2 * y + 1][2 * x], img->ref_pixel_buffer[0][0][2 * y][2 * x + 1], img->ref_pixel_buffer[0][0][2 * y + 1][2 * x + 1]);
        }
    }
#undef FILTER
}

void plane_expand_border(pel_t* p_pix, int i_stride, int i_width, int i_height, int i_padh, int i_padv, int b_pad_top, int b_pad_bottom)
{
    pel_t* pix = p_pix + 8 * i_stride;
    pel_t* row;
    int x, y;

    /* --- horizontal ----------------------------------------------
    */
    for (y = 0; y < i_height; y++) {
        for (x = 0; x < i_padh; x++) {
            pix[x] = pix[i_padh];
            //memset(pix, pix[i_padh], i_padh);                                     /* left  band */
            pix[i_width + i_padh + x] = pix[i_width + i_padh - 1];
            //memset(pix + i_width + i_padh, pix[i_width + i_padh - 1], i_padh);    /* right band */
        }
        pix += i_stride;
    }

    /* --- vertical ------------------------------------------------
    */

    /* upper band */
    pix = p_pix + 8 * i_stride;
    if (b_pad_top) {
        row = pix - i_stride;   /* start row position */
        for (y = 0; y < i_padv; y++) {
            memcpy(row, pix, i_stride * sizeof(pel_t));
            row -= i_stride;
        }
    }

    /* lower band */
    pix = p_pix + (i_height - 1 + 8) * i_stride;
    if (b_pad_bottom) {
        row = pix + i_stride;
        for (y = 0; y < i_padv; y++) {
            memcpy(row, pix, i_stride * sizeof(pel_t));
            row += i_stride;
        }
    }
}

void ratecontral_vpp()
{
    downsampling_image(pRC->cur_yuv, pRC->ref_yuv);

    plane_expand_border(pRC->cur_yuv, img->width / 2 + 2 * VPP_PAD, img->width / 2, img->height / 2, VPP_PAD, VPP_PAD, 1, 1);

    plane_expand_border(pRC->ref_yuv, img->width / 2 + 2 * VPP_PAD, img->width / 2, img->height / 2, VPP_PAD, VPP_PAD, 1, 1);

    starvpp_estimate_frame_complexity();

    if (img->type != SLICE_I) {
        //pRC->short_term_cplxsum *= 0.5;
        //pRC->short_term_cplxcount *= 0.5;
        //pRC->short_term_cplxsum += pRC->complxs.i_best_cost * 1.0;
        //pRC->short_term_cplxcount++;
        //pRC->blurred_complexity = pRC->short_term_cplxsum / pRC->short_term_cplxcount;

        pRC->blurred_complexity = pRC->complxs.i_best_cost;
    }
    printf(" [Cplx %10.2f ]", pRC->blurred_complexity);
    //printf(" [REst %7d ]", pRC->complxs.i_min_rate);
}
#endif
void gop_level_ratecontrol()
{
#if HEVC_RC
    if (frame_level_fw.EncControl == 1)
    {
        if (pRC->iGOPid != pRC->numPic) {
            return;
        }

        if (frame_level_fw.frame_id % pRC->IntraPeriod == 0) {
            pRC->numPic = 1;
        }
        else if (pRC->IntraPeriod - frame_level_fw.frame_id % pRC->IntraPeriod < 4) {
            pRC->numPic = pRC->IntraPeriod - frame_level_fw.frame_id % pRC->IntraPeriod;
        }
        else {
            pRC->numPic = 4;
        }

        Int targetBits = xEstGOPTargetBits();

#if rc_algorithm_0
        int a = 1.3 * (0.725 * log(120) + 0.5793);
        int b = (0.725 * log(120) + 0.5793);
        int c = log(120);

        if (pRC->m_adaptiveBit > 0 && lastpic.lastLevelLambda > 0.1 && lastpic.m_framelevel == 1) {
            Double targetBpp = (Double)targetBits / (pRC->FrameWidth * pRC->FrameHeight);
            Double basicLambda = 0.0;
            Double* lambdaRatio = new Double[4];
            Double* equaCoeffA = new Double[4];
            Double* equaCoeffB = new Double[4];

            // for GOP size = 4, low delay case
            //if (lastpic.lastLevelLambda < 120.0) {
            //	//lambdaRatio[3] = 0.725 * log(lastpic.lastLevelLambda) + 0.5793;
            //	//lambdaRatio[2] = 0.725 * log(lastpic.lastLevelLambda) + 0.5793;
            //    //lambdaRatio[1] = 1.3 * lambdaRatio[3];
            //	//lambdaRatio[0] = 1.3 * lambdaRatio[3];
            //	lambdaRatio[3] = 1.0;
            //	lambdaRatio[2] = 2.0;
            //	lambdaRatio[1] = 2.0;
            //	lambdaRatio[0] = 1.0;
            //}
            //else {
            lambdaRatio[3] = 1.0;
            lambdaRatio[2] = 2.0;
            lambdaRatio[1] = 2.0;
            lambdaRatio[0] = 1.0;
            //}

            xCalEquaCoeff(pRC, lambdaRatio, equaCoeffA, equaCoeffB, 4);
            basicLambda = xSolveEqua(targetBpp, equaCoeffA, equaCoeffB, 4);
            setAllBitRatio(basicLambda, equaCoeffA, equaCoeffB);

            delete[]lambdaRatio;
            delete[]equaCoeffA;
            delete[]equaCoeffB;
        }
#endif

        Int i;
        Int totalPicRatio = 0;
        Int currPicRatio = 0;
        for (i = 0; i < pRC->numPic; i++) {
            totalPicRatio += pRC->bitsRatio[i];
        }
        for (i = 0; i < pRC->numPic; i++) {
            currPicRatio = pRC->bitsRatio[i];
            pRC->m_picTargetBitInGOP[i] = (Int)(((Double)targetBits) * currPicRatio / totalPicRatio);
        }

        pRC->iGOPid = 0;
        pRC->m_GOPbitsLeft = targetBits;
    }
#endif
}

#if HEVC_RC
Int xEstPicTargetBits(RateControl* pRC)
{
    Int targetBits = 0;
    Int GOPbitsLeft = pRC->m_GOPbitsLeft;

    Int i;
    Int currPicPosition = pRC->iGOPid;
    Int currPicRatio = pRC->bitsRatio[currPicPosition];
    Int totalPicRatio = 0;
    for (i = currPicPosition; i < pRC->numPic; i++) {
        totalPicRatio += pRC->bitsRatio[i];
    }

    targetBits = Int(((Double)GOPbitsLeft) * currPicRatio / totalPicRatio);

#if rc_algorithm_5
    Int realInfluencePicture = MIN(g_RCSmoothWindowSize, pRC->m_framesLeft);
    Int averageTargetBitsPerPic = (Int)(pRC->TargetBitrate / frame_level_fw.framerate);
    targetBits = (Int)((pRC->m_SeqbitsLeft - averageTargetBitsPerPic * (pRC->m_framesLeft - realInfluencePicture)) / realInfluencePicture);
#endif

    //use blur_complex
#if rc_algorithm_1
    Int Target_Bits = pRC->m_picTargetBitInGOP[currPicPosition];
    if (img->type != I_IMG && lastpic.blurred_complexity != 0) {
        //if (lastpic.blurred_complexity >= pRC->blurred_complexity) {
        //	targetBits = (Int)(pRC->blurred_complexity * pRC->m_picActualBits / lastpic.blurred_complexity);
        //	targetBits = Clip3(100, 2 * Target_Bits, targetBits);
        //}
        if ((2 * lastpic.blurred_complexity < pRC->blurred_complexity) && (targetBits < 2 * pRC->m_picActualBits)) {
            //targetBits = 0.5 * targetBits;
            pRC->m_picPara[pRC->m_frameLevel].m_alpha = pRC->m_picPara[pRC->m_frameLevel].m_alpha * 2;
        }
        if (8 * lastpic.blurred_complexity < pRC->blurred_complexity) {
            //pRC->m_picPara[pRC->m_frameLevel].m_alpha = 3.2003;
            //pRC->m_picPara[pRC->m_frameLevel].m_beta = -1.250;
            pRC->m_picPara[pRC->m_frameLevel].m_alpha = pRC->m_picPara[pRC->m_frameLevel].m_alpha * 2;
        }
    }
#endif

    if (targetBits < 100) {
        targetBits = 100;   // at least allocate 100 bits for one picture
    }

    //if (pRC->m_framesLeft > 16) {
    //    targetBits = Int(g_RCWeightPicRargetBitInBuffer * targetBits + g_RCWeightPicTargetBitInGOP * pRC->m_picTargetBitInGOP[currPicPosition]);
    //}

    return targetBits;
}

Int xEstPicHeaderBits(list<RCPreviousPictures*>& listPreviousPictures, Int frameLevel)
{
    Int numPreviousPics = 0;
    Int totalPreviousBits = 0;
    Int estHeaderBits = 0;

    estHeaderBits = lastlevelpic[pRC->m_frameLevel].m_picActualHeaderBits;

    return estHeaderBits;
}
#endif

#if V0078_ADAPTIVE_LOWER_BOUND
Int xEstPicLowerBound(Int targetBits)
{
    Int lowerBound = 0;
    Int GOPbitsLeft = pRC->m_GOPbitsLeft;

    const Int nextPicPosition = (pRC->iGOPid + 1) % pRC->numPic;
    const Int nextPicRatio = pRC->bitsRatio[nextPicPosition];

    Int totalPicRatio = 0;
    for (Int i = nextPicPosition; i < pRC->numPic; i++) {
        totalPicRatio += pRC->bitsRatio[i];
    }

    if (nextPicPosition == 0) {
        GOPbitsLeft = pRC->m_picTargetBitInGOP[0] + pRC->m_picTargetBitInGOP[1] + pRC->m_picTargetBitInGOP[2] + pRC->m_picTargetBitInGOP[3];
    }
    else {
        GOPbitsLeft -= targetBits;
    }

    lowerBound = Int(((Double)GOPbitsLeft) * nextPicRatio / totalPicRatio);

    if (lowerBound < 100) {
        lowerBound = 100;   // at least allocate 100 bits for one picture
    }

    if (pRC->m_framesLeft > 16) {
        lowerBound = Int(g_RCWeightPicRargetBitInBuffer * lowerBound + g_RCWeightPicTargetBitInGOP * pRC->m_picTargetBitInGOP[nextPicPosition]);
    }

    return lowerBound;
}
#endif

#if HEVC_RC
Int xCalcHADs8x8_ISlice(U8 piOrg[8][8])
{
    Int k, l, i, j, jj;
    Int diff[64], m1[8][8], m2[8][8], m3[8][8], iSumHad = 0;

    for (l = 0; l < 8; l++) {
        for (k = 0; k < 8; k++) {
            diff[l * 8 + k] = piOrg[l][k];
        }
    }

    //horizontal
    for (j = 0; j < 8; j++)
    {
        jj = j << 3;
        m2[j][0] = diff[jj] + diff[jj + 4];
        m2[j][1] = diff[jj + 1] + diff[jj + 5];
        m2[j][2] = diff[jj + 2] + diff[jj + 6];
        m2[j][3] = diff[jj + 3] + diff[jj + 7];
        m2[j][4] = diff[jj] - diff[jj + 4];
        m2[j][5] = diff[jj + 1] - diff[jj + 5];
        m2[j][6] = diff[jj + 2] - diff[jj + 6];
        m2[j][7] = diff[jj + 3] - diff[jj + 7];

        m1[j][0] = m2[j][0] + m2[j][2];
        m1[j][1] = m2[j][1] + m2[j][3];
        m1[j][2] = m2[j][0] - m2[j][2];
        m1[j][3] = m2[j][1] - m2[j][3];
        m1[j][4] = m2[j][4] + m2[j][6];
        m1[j][5] = m2[j][5] + m2[j][7];
        m1[j][6] = m2[j][4] - m2[j][6];
        m1[j][7] = m2[j][5] - m2[j][7];

        m2[j][0] = m1[j][0] + m1[j][1];
        m2[j][1] = m1[j][0] - m1[j][1];
        m2[j][2] = m1[j][2] + m1[j][3];
        m2[j][3] = m1[j][2] - m1[j][3];
        m2[j][4] = m1[j][4] + m1[j][5];
        m2[j][5] = m1[j][4] - m1[j][5];
        m2[j][6] = m1[j][6] + m1[j][7];
        m2[j][7] = m1[j][6] - m1[j][7];
    }

    //vertical
    for (i = 0; i < 8; i++)
    {
        m3[0][i] = m2[0][i] + m2[4][i];
        m3[1][i] = m2[1][i] + m2[5][i];
        m3[2][i] = m2[2][i] + m2[6][i];
        m3[3][i] = m2[3][i] + m2[7][i];
        m3[4][i] = m2[0][i] - m2[4][i];
        m3[5][i] = m2[1][i] - m2[5][i];
        m3[6][i] = m2[2][i] - m2[6][i];
        m3[7][i] = m2[3][i] - m2[7][i];

        m1[0][i] = m3[0][i] + m3[2][i];
        m1[1][i] = m3[1][i] + m3[3][i];
        m1[2][i] = m3[0][i] - m3[2][i];
        m1[3][i] = m3[1][i] - m3[3][i];
        m1[4][i] = m3[4][i] + m3[6][i];
        m1[5][i] = m3[5][i] + m3[7][i];
        m1[6][i] = m3[4][i] - m3[6][i];
        m1[7][i] = m3[5][i] - m3[7][i];

        m2[0][i] = m1[0][i] + m1[1][i];
        m2[1][i] = m1[0][i] - m1[1][i];
        m2[2][i] = m1[2][i] + m1[3][i];
        m2[3][i] = m1[2][i] - m1[3][i];
        m2[4][i] = m1[4][i] + m1[5][i];
        m2[5][i] = m1[4][i] - m1[5][i];
        m2[6][i] = m1[6][i] + m1[7][i];
        m2[7][i] = m1[6][i] - m1[7][i];
    }

    for (i = 0; i < 8; i++)
    {
        for (j = 0; j < 8; j++)
        {
            iSumHad += abs(m2[i][j]);
        }
    }
    iSumHad -= abs(m2[0][0]);
    iSumHad = (iSumHad + 2) >> 2;
    return(iSumHad);
}
#endif

#if HEVC_RC
Int calCostSliceI()
{
#if !rc_I_slice_cost
#if rc_algorithm_9
    if (frame_level_fw.intra_period > 1)
        return 0;
#endif
#endif

    int i, j, y, x, w, h;
    U8 orgY[8][8];
    Int iSumHad = 0;
#if rc_I_slice_cost
    if (img->width < 352 && img->height < 288) {
#endif
#if LCU_RATECONTROL
        Int bk_iSumHad = 0;
        int istride = 0;
        int lcuidx = 0;
        Int picWidthInLCU = ((frame_level_fw.pic_width % 32 == 0) ? (frame_level_fw.pic_width / 32) : frame_level_fw.pic_width / 32 + 1);
        Int picHeightInLCU = ((frame_level_fw.pic_height % 32 == 0) ? (frame_level_fw.pic_height / 32) : frame_level_fw.pic_height / 32 + 1);

        for (y = 0; y < picHeightInLCU; y++) {
            for (x = 0; x < picWidthInLCU; x++) {

                Int currWidth = ((x == picWidthInLCU - 1) ? frame_level_fw.pic_width - 32 * (picWidthInLCU - 1) : 32);
                Int currHeight = ((y == picHeightInLCU - 1) ? frame_level_fw.pic_height - 32 * (picHeightInLCU - 1) : 32);
                for (h = 0; h < currHeight; h += 8) {
                    for (w = 0; w < currWidth; w += 8) {

                        for (j = 0; j < 8; j++) {
                            for (i = 0; i < 8; i++) {
                                //orgY[j][i] = img->imgY_org[y * 32 + h + j][x * 32 + w + i];
                            }
                        }
                        bk_iSumHad = xCalcHADs8x8_ISlice(orgY);
                        pRC->m_lcu[lcuidx].m_costIntra += bk_iSumHad;
                        iSumHad += bk_iSumHad;
                    }
                }
                lcuidx++;
            }
        }

#else
        for (y = 0; y < frame_level_fw.pic_height; y += 8) {
            for (x = 0; x < frame_level_fw.pic_width; x += 8) {
                for (j = 0; j < 8; j++) {
                    for (i = 0; i < 8; i++) {
                        //orgY[j][i] = img->imgY_org[y + j][x + i];
                    }
                }
                iSumHad += xCalcHADs8x8_ISlice(orgY);

            }
        }
#endif
#if rc_I_slice_cost
    }
    else
        return 0;
#endif
    return iSumHad;
}
#endif

#if HEVC_RC
Int getRefineBitsForIntra(Int orgBits, Int iSumHadPic)
{
#if rc_algorithm_9
    return 0;
#endif

    Int K;
    Double alpha = 0.25, beta = 0.5582;
    Int iIntraBits;

    if (pRC->m_numberOfPixel > 832 * 480) {
        if (orgBits * 40 < pRC->m_numberOfPixel) {
            alpha = 0.25;
        }
        else {
            alpha = 0.30;
        }
    }

    if (pRC->bpp < 0.03) {
        K = 1.0;
    }
    else if (pRC->bpp < 0.08) {
        K = 2.0;
    }
    else {
        K = 4.0;
    }

    iIntraBits = (Int)(alpha * pow(iSumHadPic * K / (Double)orgBits, beta) * (Double)orgBits + 0.5);

    if (iIntraBits * frame_level_fw.framerate > 100000000) {
        iIntraBits = MIN(pRC->TargetBitrate * 0.9, (Int)(100000000 / frame_level_fw.framerate));
    }

    return iIntraBits;
}

Double calculateLambdaIntra(Double alpha, Double beta, Double MADPerPixel, Double bitsPerPixel)
{
    //printf(" [MADPerPixel %f]", MADPerPixel);
    return ((alpha / 95.0) * pow(MADPerPixel / bitsPerPixel, beta));
}

Double estimatePicLambda(Int targetBits, Int iSumHadPic)
{
#if rc_algorithm_9
    if (frame_level_fw.slice_type == SLICE_I && frame_level_fw.intra_period > 1) {
        return 0;
    }
#endif

    int i;
    Double alpha = pRC->m_picPara[pRC->m_frameLevel].m_alpha;
    Double beta = pRC->m_picPara[pRC->m_frameLevel].m_beta;
    Double bpp = (Double)targetBits / (Double)pRC->m_numberOfPixel;
    Double estLambda;
#if rc_I_slice_cost
    if (img->type == I_IMG && img->intra_period == 1 && img->width < 352 && img->height < 288) {
#else
#if rc_algorithm_10
    if (img->type == I_IMG) {
#else
    if (frame_level_fw.slice_type == SLICE_I && frame_level_fw.intra_period > 1) {
#endif
#endif
        estLambda = calculateLambdaIntra(alpha, beta, pow(iSumHadPic / (Double)pRC->m_numberOfPixel, BETA1), bpp);
        //printf("estLambda=%lf\n",estLambda);
    }
    else {
        estLambda = alpha * pow(bpp, beta);
    }

    Double lastLevelLambda = -1.0;
    Double lastPicLambda = -1.0;

    lastLevelLambda = lastlevelpic[pRC->m_frameLevel].lastLevelLambda;
    lastPicLambda = lastpic.lastLevelLambda;

#if !rc_algorithm_7
    if (lastLevelLambda > 0.0) {
        lastLevelLambda = Clip3(0.1, 10000.0, lastLevelLambda);
        estLambda = Clip3(lastLevelLambda * pow(2.0, -3.0 / 3.0), lastLevelLambda * pow(2.0, 3.0 / 3.0), estLambda);
    }
#endif

#if !rc_algorithm_8
    if (lastPicLambda > 0.0) {
        lastPicLambda = Clip3(0.1, 2000.0, lastPicLambda);
        estLambda = Clip3(lastPicLambda * pow(2.0, -QP_Limit / 3.0), lastPicLambda * pow(2.0, QP_Limit / 3.0), estLambda);
    }
    else {
        estLambda = Clip3(0.1, 10000.0, estLambda);
    }
#endif

#if rc_algorithm_8
    if (lastPicLambda > 0.0) {
        lastPicLambda = Clip3(0.1, 2000.0, lastPicLambda);
        if (lastpic.imgtype == I_IMG && img->type == P_IMG) {
            if (lastLevelLambda > 0) {
                estLambda = Clip3(lastLevelLambda * pow(2.0, -3 / 3.0), lastLevelLambda * pow(2.0, 3 / 3.0), estLambda);
            }
            else {
                estLambda = Clip3(lastPicLambda * pow(2.0, -3 / 3.0), lastPicLambda * pow(2.0, 3 / 3.0), estLambda);
            }
        }
        else if (lastpic.imgtype == P_IMG && img->type == I_IMG) {
            estLambda = Clip3(lastPicLambda * pow(2.0, -3 / 3.0), lastPicLambda /** pow(2.0, 5 / 3.0)*/, estLambda);
        }
        else {
            estLambda = Clip3(lastPicLambda * pow(2.0, -3 / 3.0), lastPicLambda * pow(2.0, 3 / 3.0), estLambda);
        }
    }
    else {
        estLambda = Clip3(0.1, 10000.0, estLambda);
    }
#endif

    if (estLambda < 0.1) {
        estLambda = 0.1;
    }
#if LCU_RATECONTROL
    Double totalWeight = 0.0;
    // initial BU bit allocation weight
    for (Int i = 0; i < pRC->m_numberOfLCU; i++)
    {
        Double alphaLCU, betaLCU;
        if (pRC->m_useLCUSeparateModel)
        {
            alphaLCU = pRC->m_LCUPara[pRC->m_frameLevel][pRC->m_Lcu_idx].m_alpha;
            betaLCU = pRC->m_LCUPara[pRC->m_frameLevel][pRC->m_Lcu_idx].m_beta;
        }
        else
        {
            alphaLCU = pRC->m_picPara[pRC->m_frameLevel].m_alpha;
            betaLCU = pRC->m_picPara[pRC->m_frameLevel].m_beta;
        }

        pRC->m_lcu[i].m_bitWeight = pRC->m_lcu[i].m_numberOfPixel * pow(estLambda / alphaLCU, 1.0 / betaLCU);

        if (pRC->m_lcu[i].m_bitWeight < 0.01)
        {
            pRC->m_lcu[i].m_bitWeight = 0.01;
        }
        totalWeight += pRC->m_lcu[i].m_bitWeight;
    }
    for (Int i = 0; i < pRC->m_numberOfLCU; i++)
    {
        Double BUTargetBits = pRC->m_targetBits * pRC->m_lcu[i].m_bitWeight / totalWeight;
        pRC->m_lcu[i].m_bitWeight = BUTargetBits;
    }
#endif
    return estLambda;
    }

Int estimatePicQP(Double lambda)
{
    Int QP;
    Int i, j, K;
#if rc_algorithm_9
    if (frame_level_fw.slice_type == SLICE_I && frame_level_fw.intra_period > 1) {
        if (frame_level_fw.frame_id == 1) {
            if (pRC->bpp < 0.03) {
                QP = 45;
            }
            else if (pRC->bpp < 0.08) {
                QP = 35;
            }
            else {
                QP = 25;
            }
        }
        else {
            //bubble_sort
            for (i = 0; i < 63; i++) {
                for (int j = 63; j > i; j--) {
                    if (pRC->qp_num[j - 1] < pRC->qp_num[j]) {
                        swap(pRC->qp_num[j - 1], pRC->qp_num[j]);
                        swap(pRC->qp_val[j - 1], pRC->qp_val[j]);
                    }
                }
            }
            //cal_qp
            QP = Int((pRC->qp_num[0] * pRC->qp_val[0] + pRC->qp_num[1] * pRC->qp_val[1] + pRC->qp_num[2] * pRC->qp_val[2] + pRC->qp_num[3] * pRC->qp_val[3] + pRC->qp_num[4] * pRC->qp_val[4])
                / (pRC->qp_num[0] + pRC->qp_num[1] + pRC->qp_num[2] + pRC->qp_num[3] + pRC->qp_num[4]) + 0.5);

            if (pRC->bpp < 0.03) {
                K = 1.0;
            }
            else if (pRC->bpp < 0.08) {
                K = 4.0;
            }
            else {
                K = 8.0;
            }
            QP -= K;
            //reset array
            for (i = 0; i < 64; i++) {
                pRC->qp_num[i] = 0;
                pRC->qp_val[i] = i;
            }
        }
    }
    if (frame_level_fw.slice_type != SLICE_I || frame_level_fw.intra_period == 1) {
        QP = Int(5.5464 * log(lambda) + 11.604 + 0.5);
        //QP = Int(5.7736 * log(lambda) + 13.909 + 0.5);
    }
#endif
#if !rc_algorithm_9
    QP = Int(5.5464 * log(lambda) + 11.604 + 0.5);
#endif

    Int lastLevelQP = lastlevelpic[pRC->m_frameLevel].m_picQP;
    Int lastPicQP = lastpic.m_picQP;

#if !rc_algorithm_7
    if (lastLevelQP > 0) {
        QP = Clip3(lastLevelQP - 3, lastLevelQP + 3, QP);
    }
#endif

#if !rc_algorithm_8
    if (lastPicQP >= 0) {
        QP = Clip3(lastPicQP - QP_Limit, lastPicQP + QP_Limit, QP);
    }
#endif

#if rc_algorithm_8
    if (lastPicQP >= 0) {
        if (lastpic.imgtype == I_IMG && img->type == P_IMG) {
            if (lastLevelQP > 0) {
                QP = Clip3(lastLevelQP - 3, lastLevelQP + 3, QP);
            }
            else {
                QP = Clip3(lastPicQP - 3, lastPicQP + 3, QP);
            }
        }
        else if (lastpic.imgtype == P_IMG && img->type == I_IMG) {
            QP = Clip3(lastPicQP - 3, lastPicQP, QP);
        }
        else {
            QP = Clip3(lastPicQP - 3, lastPicQP + 3, QP);
        }
    }
#endif

#if rc_algorithm_7  || rc_algorithm_8
    pRC->m_frameLambda = exp((QP - 11.604) / 5.5464);
#endif
#if LCU_RATECONTROL
    for (int i = 0; i < pRC->m_numberOfLCU; i++)
    {
        pRC->m_lcu[i].m_QP = QP;
    }
#endif

    if (frame_level_fw.intra_period == 1)
    {
        int qp = (double)QP - SHIFT_QP;
        pRC->m_frameLambda = 0.85 * pow(2, qp / 4.0) * LAM_2Level_TU;
    }
    else
    {
        pRC->m_frameLambda = exp((QP - 11.604) / 5.5464);
    }
#if rc_algorithm_9
    pRC->qp_num[QP]++;
#endif

    return QP;
}
#if rc_algorithm_3
int rate_estimate_qscale()
{
    float q = 0.0;
    //int nmb = (img->width / 16) * (img->height / 16);
    //pRC->cplxr_sum = .01 * pow(7.0e5, pRC->qcompress) * pow(nmb, 0.5);
    //pRC->wanted_bits_window = 1.0 * pRC->m_targetBits / pRC->f_framerate;
    q = pow(pRC->blurred_complexity, 1 - pRC->qcompress);
    return q;
}
#endif

static float qscale2qp_LUKE(float qscale)
{
    return 12.0f + 6.0f * log2f(qscale / 0.85f);
}

void frame_level_ratecontrol()
{
    if (frame_level_fw.EncControl == 1) {
        int InitQP = pRC->m_frameQp;
#if rc_algorithm_1 || rc_algorithm_3
        ratecontral_vpp();
#endif

        pRC->m_frameLevel = pRC->GOPLevel[pRC->iGOPid];
        if (frame_level_fw.slice_type == SLICE_I) {
            pRC->m_frameLevel = 0;
        }
        Int targetBits = xEstPicTargetBits(pRC);
        Int estHeaderBits = xEstPicHeaderBits(m_piclist, pRC->m_frameLevel);

        //printf("[targetBits = %d]", targetBits);
        if (targetBits < estHeaderBits + 100) {
            targetBits = estHeaderBits + 100;   // at least allocate 100 bits for picture data
        }
        //pRC->m_bitsLeft -= estHeaderBits;

#if V0078_ADAPTIVE_LOWER_BOUND
        pRC->m_lowerBound = xEstPicLowerBound(targetBits);
#endif

#if U0132_TARGET_BITS_SATURATION
        if (pRC->m_CpbSaturationEnabled == 1 && pRC->m_frameLevel != 0)
        {
            Int estimatedCpbFullness = pRC->m_cpbState - pRC->m_bufferingRate;

            //prevent overflow
            if (estimatedCpbFullness + targetBits > (Int)(pRC->m_cpbSize * 0.9f)) {
                targetBits = -estimatedCpbFullness + (Int)(pRC->m_cpbSize * 0.9f);
            }
            if (estimatedCpbFullness > (Int)(pRC->m_cpbSize * 0.9f)) {
                targetBits = 200;
            }

            // prevent underflow
#if V0078_ADAPTIVE_LOWER_BOUND
            if (estimatedCpbFullness + targetBits < pRC->m_lowerBound)
            {
                targetBits = MAX(200, -estimatedCpbFullness + pRC->m_lowerBound);
            }
#endif			
        }
#endif

        Int estimatedBits = 0;
        Int iSumHadPic = 0;

        if (pRC->m_frameLevel == 0) {
#if rc_algorithm_3
#define STARAVS_MAX(a, b)       ((a) > (b)? (a) : (b))
#define STARAVS_CLIP3(L, H, v)  (((v) < (L)) ? (L) : (((v) > (H)) ? (H) : (v)))
            float q = 0.0;
            double wanted_bits, overflow = 1;
            double abr_buffer = 1 * pRC->TargetBitrate;
            int64_t total_bits = stat->bit_ctr_0;

            iSumHadPic = pRC->complxs.i_best_cost;

            double time_done = (double)img->frame_num / (double)img->framerate;
            wanted_bits = time_done * pRC->TargetBitrate;

            pRC->short_term_cplxsum *= 0.5;
            pRC->short_term_cplxcount *= 0.5;
            pRC->short_term_cplxsum += pRC->complxs.i_best_cost /** 0.7*/ /*/ (CLIP_DURATION(h->fenc->f_duration) / BASE_FRAME_DURATION)*/;
            pRC->short_term_cplxcount++;

            pRC->blurred_complexity = pRC->short_term_cplxsum / pRC->short_term_cplxcount;

            if (img->frame_num == 0) {
                int nmb = (img->width / 16) * (img->height / 16);
                pRC->wanted_bits_window = targetBits;
                pRC->cplxr_sum = .01 * pow(7.0e5, pRC->qcompress) * pow(nmb, 0.5);
                pRC->lstep = pow(2, 4 / 6.0);
            }

            q = rate_estimate_qscale();
            pRC->last_rceq = q;
            q /= (double)(pRC->wanted_bits_window / pRC->cplxr_sum);


            if (wanted_bits > 0)
            {
                abr_buffer *= STARAVS_MAX(1, sqrt(time_done));

                overflow = STARAVS_CLIP3(0.5, 2, 1.0 + (total_bits - wanted_bits) / abr_buffer);

                q *= overflow;
            }
            if (img->frame_num > 0)
            {
                double lmin = pRC->last_qscale / pRC->lstep;
                double lmax = pRC->last_qscale * pRC->lstep;
                if (overflow > 1.1 && img->frame_num > 3)
                    lmax *= pRC->lstep;
                else if (overflow < 0.9)
                    lmin /= pRC->lstep;

                q = STARAVS_CLIP3(lmin, lmax, q);
            }
            pRC->last_qscale = q;
            pRC->m_frameQp = qscale2qp_LUKE(q);

            pRC->qpa_rc = pRC->m_frameQp;
#if !rc_algorithm_7
            if (lastpic.m_picQP > 0) {
                pRC->m_frameQp = Clip3(lastpic.m_picQP - 4, lastpic.m_picQP + 4, pRC->m_frameQp);
            }
#endif
            pRC->m_frameLambda = exp((pRC->m_frameQp - 11.604) / 5.5464);
#else
            iSumHadPic = calCostSliceI();

            pRC->m_totalCostIntra = iSumHadPic;
#if LCU_RATECONTROL
            pRC->m_bitsLeft = pRC->m_targetBits - estHeaderBits;
            getLCUInitTargetBits();
#endif
            if (frame_level_fw.intra_period != 1) {
                Int bits = (Int)(pRC->m_SeqbitsLeft / pRC->m_framesLeft);
                targetBits = getRefineBitsForIntra(bits, iSumHadPic);

#if U0132_TARGET_BITS_SATURATION
                if (pRC->m_CpbSaturationEnabled) {
                    Int estimatedCpbFullness = pRC->m_cpbState - pRC->m_bufferingRate;

                    // prevent overflow
                    if (estimatedCpbFullness + targetBits > (Int)(pRC->m_cpbSize * 0.9f)) {
                        targetBits = -estimatedCpbFullness + (Int)(pRC->m_cpbSize * 0.9f);
                    }

                    // prevent underflow
#if V0078_ADAPTIVE_LOWER_BOUND
                    if (estimatedCpbFullness + targetBits < pRC->m_lowerBound) {
                        targetBits = -estimatedCpbFullness + pRC->m_lowerBound;
                    }
#endif
                }
#endif     
                if (targetBits < 200) {
                    targetBits = 200;
                }

            }
            pRC->m_frameLambda = estimatePicLambda(targetBits, iSumHadPic);
            pRC->m_frameQp = estimatePicQP(pRC->m_frameLambda);
#endif
        }
        else {    // normal case
            pRC->m_frameLambda = estimatePicLambda(targetBits, iSumHadPic);
            pRC->m_frameQp = estimatePicQP(pRC->m_frameLambda);
        }
#if rc_algorithm_2
        pRC->m_picTargetBits = targetBits;
#endif
        printf(" [%8d targetBits]", targetBits);

        if (frame_level_fw.intra_period > 1)
        {
            if (frame_level_fw.frame_id != 0)
                pRC->m_frameQp = Clip3(4, MAX_QP, pRC->m_frameQp);
            else
                pRC->m_frameQp = Clip3(4, MAX_QP, InitQP);
        }
        else {
            pRC->m_frameQp = Clip3(4, MAX_QP, pRC->m_frameQp);
        }
        pRC->m_targetBits = targetBits;
        pRC->iGOPid++;

        frame_level_fw.qp_y = pRC->m_frameQp;
        ctu_ctrl_input.qp_y = pRC->m_frameQp;
        //printf("rc_qp_y=%d\n", ctu_ctrl_input.qp_y);
        //img->lambda_mode = pRC->m_frameLambda;
        //img->qp = pRC->m_frameQp;
#if LCU_RATECONTROL
        for (int i = 0; i < pRC->m_numberOfLCU; i++)
        {
            pRC->m_lcu[i].m_QP = frame_level_fw.qp_y;
            pRC->m_lcu[i].m_lambda = pRC->m_frameLambda;
        }
#endif
        //img->lambda_motion = 1024 * sqrt(img->lambda_mode);
        //img->lambda_mode *= 1024;
        //img->qpI = img->qp;
        //img->lambda_motion = (1 << LAMBDA_SCALE) * sqrt(img->lambda_mode);
        //img->lambda_mode *= (1 << LAMBDA_SCALE);
    }
}

void updateAlphaBetaIntra(Double * alpha, Double * beta)
{
    Double lnbpp = log(pow(pRC->m_totalCostIntra / (Double)pRC->m_numberOfPixel, BETA1));
    Double diffLambda = (*beta) * (log((Double)pRC->m_picActualBits) - log((Double)pRC->m_targetBits));

    diffLambda = Clip3(-0.125, 0.125, 0.25 * diffLambda);

    lnbpp = abs(lnbpp);

    double beta_prev = *beta;
    *alpha = (*alpha) * exp(diffLambda);
    *beta = (*beta) + diffLambda / lnbpp;

    if (*beta < 1 && (beta_prev - *beta)>1)
    {
        *alpha = ALPHA;
        *beta = BETA2;
    }


    //printf("alpha=%lf, beta=%lf, diffLambda=%lf\n", *alpha, *beta, diffLambda);
}

static float qp2qscale_LUKE(float qp)
{
    return 0.85f * powf(2.0f, (qp - 12.0f) / 6.0f);
}

void updateAfterPicture()
{
#if rc_algorithm_6
    pRC->current_pic_reset_0 = 0;
    pRC->current_pic_update = 1;
#endif

    Double alpha = pRC->m_picPara[pRC->m_frameLevel].m_alpha;
    Double beta = pRC->m_picPara[pRC->m_frameLevel].m_beta;
#if rc_algorithm_10
    if (img->type == I_IMG) {
#else
    if (frame_level_fw.slice_type == SLICE_I && frame_level_fw.intra_period > 1) {
#endif
#if rc_algorithm_3
        pRC->cplxr_sum += (stat->bit_ctr - stat->bit_ctr_n) * qp2qscale_LUKE(pRC->qpa_rc) / pRC->last_rceq;
        pRC->cplxr_sum *= 0.8;
        pRC->wanted_bits_window += 1.0 / img->framerate * pRC->TargetBitrate;
        pRC->wanted_bits_window *= 0.8;
#else
        updateAlphaBetaIntra(&alpha, &beta);
#endif
    }
    else {
        // update parameters
        Double picActualBits = (Double)pRC->m_picActualBits;
        Double picActualBpp = picActualBits / (Double)pRC->m_numberOfPixel;
        Double picActualDis = (Double)pRC->total_distortion / (Double)pRC->m_numberOfPixel;
        Double calLambda = alpha * pow(picActualBpp, beta);
        Double inputLambda = pRC->m_frameLambda;

#if rc_algorithm_6
        if (pRC->lastpic_bpp > picActualBpp * 10 || pRC->m_picTargetBits == 240) {
            pRC->lastpic_bpp = picActualBpp;
            pRC->current_pic_update = 0;
            return;
        }
        pRC->lastpic_bpp = picActualBpp;
#endif

        if (inputLambda < 0.01 || calLambda < 0.01 || picActualBpp < 0.0001) {
            alpha *= (1.0 - pRC->m_alphaUpdate / 2.0);
            beta *= (1.0 - pRC->m_betaUpdate / 2.0);

            alpha = Clip3(g_RCAlphaMinValue, g_RCAlphaMaxValue, alpha);
            beta = Clip3(g_RCBetaMinValue, g_RCBetaMaxValue, beta);

            TRCParameter rcPara;
            rcPara.m_alpha = alpha;
            rcPara.m_beta = beta;

            pRC->m_picPara[pRC->m_frameLevel].m_alpha = rcPara.m_alpha;
            pRC->m_picPara[pRC->m_frameLevel].m_beta = rcPara.m_beta;

            return;
        }
#if !rc_algorithm_2
        calLambda = Clip3(inputLambda / 10.0, inputLambda * 10.0, calLambda);
        alpha += pRC->m_alphaUpdate * (log(inputLambda) - log(calLambda)) * alpha;
        Double lnbpp = log(picActualBpp);
        lnbpp = Clip3(-5.0, -0.1, lnbpp);
        beta += pRC->m_betaUpdate * (log(inputLambda) - log(calLambda)) * lnbpp;
#endif
#if rc_algorithm_2

        Double K_old = -1 - beta;
        if (K_old <= 0.001) {
            K_old = 0.001;
        }
        Double C_old = alpha / K_old;
        Double D_old = C_old * pow(pRC->m_picTargetBits / (Double)pRC->m_numberOfPixel, -K_old);
        Double D_real = C_old * pow(picActualBpp, -K_old);
        Double temp = (log(D_real) - log(D_old))/**0.5*/;
        Double lnbpp = log(picActualBpp);
        Double K_new, C_new;
        if (lnbpp <= 0 && lnbpp >= -0.001) {
            lnbpp = -0.001;
        }
        if (lnbpp > 0 && lnbpp <= 0.001) {
            lnbpp = 0.001;
        }
        if (frame_level_fw.intra_period == 1 && (picActualBpp > 1 && temp < 0))
        {

            K_new = K_old + (abs(temp) / lnbpp);

        }
        else {
            K_new = K_old + (temp / lnbpp);
        }


        //K_new = K_old + (temp / lnbpp);
        C_new = C_old * (1 - temp);
        alpha = C_new * K_new;
        beta = -K_new - 1;
#endif

        alpha = Clip3(g_RCAlphaMinValue, g_RCAlphaMaxValue, alpha);
        beta = Clip3(g_RCBetaMinValue, g_RCBetaMaxValue, beta);

#if rc_algorithm_6
        if (pRC->target_bpp * 8 < picActualBpp) {
            pRC->current_pic_reset_0 = 1;
            alpha = 3.2003;
            beta = -1.250;
        }
#endif
    }

    pRC->m_picPara[pRC->m_frameLevel].m_alpha = alpha;
    pRC->m_picPara[pRC->m_frameLevel].m_beta = beta;
    }
#if LCU_RATECONTROL
void getLCUInitTargetBits()
{
    unsigned int iAvgBits = 0;
    unsigned int avgbits = 0;

    pRC->m_remainingCostIntra = pRC->m_totalCostIntra;
    for (Int i = pRC->m_numberOfLCU - 1; i >= 0; i--)
        //for (Int i = 0; i < pRC->m_numberOfLCU - 1; i++)
    {
        avgbits = (unsigned int)(pRC->m_targetBits * (double)(pRC->m_lcu[i].m_costIntra / pRC->m_totalCostIntra));
        iAvgBits += avgbits;
        pRC->m_lcu[i].m_targetBitsLeft = iAvgBits;
    }



}

double getLCUTargetBpp(int LCU_nr)
{
    pRC->m_Lcu_idx = LCU_nr;
    Int   LCUIdx = pRC->m_Lcu_idx;
    Double bpp = -1.0;
    Int avgBits = 0;

    if (frame_level_fw.slice_type == SLICE_I)
    {
        Int noOfLCUsLeft = pRC->m_numberOfLCU - LCUIdx + 1;
        Int bitrateWindow = min(4, noOfLCUsLeft);
        Double MAD = pRC->m_lcu[LCUIdx].m_costIntra;

        if (pRC->m_remainingCostIntra > 0.1)
        {
            Double weightedBitsLeft = (pRC->m_bitsLeft * bitrateWindow + (pRC->m_bitsLeft - pRC->m_lcu[LCUIdx].m_targetBitsLeft) * noOfLCUsLeft) / (Double)bitrateWindow;
            avgBits = Int(MAD * weightedBitsLeft / pRC->m_remainingCostIntra);
        }
        else
        {
            avgBits = Int(pRC->m_bitsLeft / pRC->m_LCULeft);
        }
        pRC->m_remainingCostIntra -= MAD;
    }
    else
    {
        Double totalWeight = 0;
        for (Int i = LCUIdx; i < pRC->m_numberOfLCU; i++)
        {
            totalWeight += pRC->m_lcu[LCUIdx].m_bitWeight;
        }
        Int realInfluenceLCU = min(4, pRC->m_LCULeft);
        avgBits = (Int)(pRC->m_lcu[LCUIdx].m_bitWeight - (totalWeight - pRC->m_bitsLeft) / realInfluenceLCU + 0.5);
    }

    if (avgBits < 1)
    {
        avgBits = 1;
    }

    bpp = (Double)avgBits / (Double)pRC->m_lcu[LCUIdx].m_numberOfPixel;
    pRC->m_lcu[LCUIdx].m_targetBits = avgBits;

    return bpp;
}

Double getLCUEstLambdaAndQP(Double bpp, Int clipPicQP, int * estQP)
{
    Int   LCUIdx = pRC->m_Lcu_idx;
    //Int estQP;

    double alpha = pRC->m_picPara[pRC->m_frameLevel].m_alpha;
    double beta = pRC->m_picPara[pRC->m_frameLevel].m_beta;

    Double costPerPixel = pRC->m_lcu[LCUIdx].m_costIntra / (Double)pRC->m_lcu[LCUIdx].m_numberOfPixel;
    costPerPixel = pow(costPerPixel, BETA1);
    Double estLambda = calculateLambdaIntra(alpha, beta, costPerPixel, bpp);

    Int clipNeighbourQP = g_RCInvalidQPValue;
    for (Int i = LCUIdx - 1; i >= 0; i--)
    {
        if (pRC->m_lcu[LCUIdx].m_QP > g_RCInvalidQPValue)
        {
            clipNeighbourQP = pRC->m_lcu[LCUIdx].m_QP;
            break;
        }
    }

    Int minQP = clipPicQP - 2;
    Int maxQP = clipPicQP + 2;

    if (clipNeighbourQP > g_RCInvalidQPValue)
    {
        maxQP = min(clipNeighbourQP + 1, maxQP);
        minQP = max(clipNeighbourQP - 1, minQP);
    }

    Double maxLambda = exp(((Double)(maxQP + 0.49) - 11.604) / 5.5464);
    Double minLambda = exp(((Double)(minQP - 0.49) - 11.604) / 5.5464);

    estLambda = Clip3(minLambda, maxLambda, estLambda);

    *estQP = (U6)(5.5464 * log(estLambda) + 11.604 + 0.5);
    //*estQP = (U6)(5.7736 * log(estLambda) + 13.909 + 0.5);
    *estQP = Clip3(minQP, maxQP, *estQP);
    //printf("%d %d %lf %lf %lf %lf\n", LCUIdx,estQP, costPerPixel,bpp, alpha, beta);

    //img->qp = *estQP;
    pRC->m_lcu[LCUIdx].m_lambda = estLambda;
    //int qp = (double)*estQP - SHIFT_QP;
    //ctu_ctrl_input.lambda=
    //img->lambda_mode = 0.85 * pow(2, qp / 4.0) * LAM_2Level_TU;
    //img->lambda_motion = (1 << LAMBDA_SCALE) * sqrt(img->lambda_mode);
    //img->lambda_mode *= (1 << LAMBDA_SCALE);
    return estLambda;
}

double getLCUEstLambda(int bpp)
{
    Double alpha;
    Double beta;
    if (pRC->m_useLCUSeparateModel)
    {
        alpha = pRC->m_LCUPara[pRC->m_frameLevel][pRC->m_Lcu_idx].m_alpha;
        beta = pRC->m_LCUPara[pRC->m_frameLevel][pRC->m_Lcu_idx].m_beta;
    }
    else
    {
        alpha = pRC->m_picPara[pRC->m_frameLevel].m_alpha;
        beta = pRC->m_picPara[pRC->m_frameLevel].m_beta;
    }

    Double estLambda = alpha * pow(bpp, beta);
    //for Lambda clip, picture level clip
    Double clipPicLambda = pRC->m_frameLambda;

    //for Lambda clip, LCU level clip
    Double clipNeighbourLambda = -1.0;
    for (Int i = pRC->m_Lcu_idx - 1; i >= 0; i--)
    {
        if (pRC->m_lcu[i].m_lambda > 0)
        {
            clipNeighbourLambda = pRC->m_lcu[i].m_lambda;
            break;
        }
    }

    if (clipNeighbourLambda > 0.0)
    {
        estLambda = Clip3(clipNeighbourLambda * pow(2.0, -1.0 / 3.0), clipNeighbourLambda * pow(2.0, 1.0 / 3.0), estLambda);
    }

    if (clipPicLambda > 0.0)
    {
        estLambda = Clip3(clipPicLambda * pow(2.0, -2.0 / 3.0), clipPicLambda * pow(2.0, 2.0 / 3.0), estLambda);
    }
    else
    {
        estLambda = Clip3(10.0, 1000.0, estLambda);
    }

    if (estLambda < 0.1)
    {
        estLambda = 0.1;
    }

    return estLambda;

}

Int getLCUEstQP(Double lambda, Int clipPicQP)
{
    Int LCUIdx = pRC->m_Lcu_idx;
    Int estQP = Int(5.5464 * log(lambda) + 11.604 + 0.5);

    //for Lambda clip, LCU level clip
    Int clipNeighbourQP = g_RCInvalidQPValue;
    for (Int i = LCUIdx - 1; i >= 0; i--)
    {
        if (pRC->m_lcu[i].m_QP > g_RCInvalidQPValue)
        {
            clipNeighbourQP = pRC->m_lcu[i].m_QP;
            break;
        }
    }

    if (clipNeighbourQP > g_RCInvalidQPValue)
    {
        estQP = Clip3(clipNeighbourQP - 1, clipNeighbourQP + 1, estQP);
    }

    estQP = Clip3(clipPicQP - 2, clipPicQP + 2, estQP);

    int qp = (double)estQP - SHIFT_QP;
    //int RPS_idx = ((img->coding_order - 1) % img->gop_size_all);
    //img->lambda_mode = 0.68 * pow(2, qp / 4.0) * 0.8;
    //{
    //    if ((RPS_idx + 1) % img->gop_size_all != 0) {
    //        img->lambda_mode *= MAX(2.00, MIN(4.00, qp / 8.0)) * 0.8;
    //    }
    //}
    //img->lambda_motion = (1 << LAMBDA_SCALE) * sqrt(img->lambda_mode);
    //img->lambda_mode *= (1 << LAMBDA_SCALE);

    return estQP;
}


void updateAfterLCU(Int ActualBits)
{
    pRC->m_bitsLeft -= ActualBits;
    pRC->m_lcu[pRC->m_Lcu_idx].m_actualBits = ActualBits;
    //pRC->prclcu[pRC->m_Lcu_idx].m_numberOfPixel = 1024;
    if (frame_level_fw.slice_type == SLICE_I)
    {
        return;
    }
    Double alpha = pRC->m_LCUPara[pRC->m_frameLevel][pRC->m_Lcu_idx].m_alpha;
    Double beta = pRC->m_LCUPara[pRC->m_frameLevel][pRC->m_Lcu_idx].m_beta;

    Int LCUActualBits = pRC->m_lcu[pRC->m_Lcu_idx].m_actualBits;
    Int LCUTotalPixels = pRC->m_lcu[pRC->m_Lcu_idx].m_numberOfPixel;
    Double bpp = (Double)LCUActualBits / (Double)LCUTotalPixels;
    Double calLambda = alpha * pow(bpp, beta);
    Double inputLambda = pRC->m_lcu[pRC->m_Lcu_idx].m_lambda;

    if (inputLambda < 0.01 || calLambda < 0.01 || bpp < 0.0001)
    {
        alpha *= (1.0 - pRC->m_alphaUpdate / 2.0);
        beta *= (1.0 - pRC->m_betaUpdate / 2.0);

        alpha = Clip3(g_RCAlphaMinValue, g_RCAlphaMaxValue, alpha);
        beta = Clip3(g_RCBetaMinValue, g_RCBetaMaxValue, beta);

        pRC->m_LCUPara[pRC->m_frameLevel][pRC->m_Lcu_idx].m_alpha = alpha;
        pRC->m_LCUPara[pRC->m_frameLevel][pRC->m_Lcu_idx].m_beta = beta;
        return;
    }

    calLambda = Clip3(inputLambda / 10.0, inputLambda * 10.0, calLambda);
    alpha += pRC->m_alphaUpdate * (log(inputLambda) - log(calLambda)) * alpha;
    Double lnbpp = log(bpp);
    lnbpp = Clip3(-5.0, -0.1, lnbpp);
    beta += pRC->m_betaUpdate * (log(inputLambda) - log(calLambda)) * lnbpp;

    alpha = Clip3(g_RCAlphaMinValue, g_RCAlphaMaxValue, alpha);
    beta = Clip3(g_RCBetaMinValue, g_RCBetaMaxValue, beta);

    pRC->m_LCUPara[pRC->m_frameLevel][pRC->m_Lcu_idx].m_alpha = alpha;
    pRC->m_LCUPara[pRC->m_frameLevel][pRC->m_Lcu_idx].m_beta = beta;
}
#endif
void addToPictureLsit()
{
#if rc_algorithm_6
    if (pRC->current_pic_update == 0) {
        return;
    }
#endif
    if (pRC->m_frameLevel == 0) {
        lastlevelpic[0].imgtype = frame_level_fw.slice_type;
        lastlevelpic[0].m_picQP = pRC->m_frameQp;
        lastlevelpic[0].m_framelevel = pRC->m_frameLevel;
        lastlevelpic[0].m_picActualHeaderBits = pRC->m_picActualHeaderBits;
        lastlevelpic[0].lastLevelLambda = pRC->m_frameLambda;
    }
    else {
        lastlevelpic[1].imgtype = frame_level_fw.slice_type;
        lastlevelpic[1].m_picQP = pRC->m_frameQp;
        lastlevelpic[1].m_framelevel = pRC->m_frameLevel;
        lastlevelpic[1].m_picActualHeaderBits = pRC->m_picActualHeaderBits;
        lastlevelpic[1].lastLevelLambda = pRC->m_frameLambda;
    }
    lastpic.imgtype = frame_level_fw.slice_type;
    lastpic.m_picQP = pRC->m_frameQp;
    lastpic.m_framelevel = pRC->m_frameLevel;
    lastpic.m_picActualHeaderBits = pRC->m_picActualHeaderBits;
    lastpic.lastLevelLambda = pRC->m_frameLambda;
#if rc_algorithm_1
    lastpic.blurred_complexity = pRC->blurred_complexity;
#endif
#if rc_algorithm_6
    if (pRC->current_pic_reset_0 == 1) {
        lastlevelpic[pRC->m_frameLevel].imgtype = frame_level_fw.slice_type;
        lastlevelpic[pRC->m_frameLevel].m_picQP = -1;
        lastlevelpic[pRC->m_frameLevel].m_framelevel = 0;
        lastlevelpic[pRC->m_frameLevel].m_picActualHeaderBits = 0;
        lastlevelpic[pRC->m_frameLevel].lastLevelLambda = -1;
        lastpic.imgtype = frame_level_fw.slice_type;
        lastpic.m_picQP = -1;
        lastpic.m_framelevel = 0;
        lastpic.m_picActualHeaderBits = 0;
        lastpic.lastLevelLambda = -1;
    }
#endif
}

#if U0132_TARGET_BITS_SATURATION
Int updateCpbState(Int actualBits)
{
    //Int cpbState = 1;  

    pRC->m_cpbState += actualBits;
    //if (pRC->m_cpbState < 0) {
    //	cpbState = -1;
    //}

    pRC->m_cpbState -= pRC->m_bufferingRate;
    //if (pRC->m_cpbState > pRC->m_cpbSize) {
    //	cpbState = 0;
    //}

    return 0;
}

#endif

void updata_ratecontrol_parameters(int bits)
{
    pRC->m_picActualBits = bits;// (int)(stat->bit_ctr - stat->bit_ctr_n);
    Double averageQP;
    Double averageLambda;

    //frame level RC
    //averageQP = pRC->m_frameQp;

    //if (averageQP > 0.0) {
    //    pRC->m_frameQp = Int(averageQP + 0.5);
    //}
    //else {
    //    pRC->m_frameQp = g_RCInvalidQPValue;
    //}

    updateAfterPicture();
    addToPictureLsit();

    pRC->m_totalactualbits += pRC->m_picActualBits;
    pRC->m_SeqbitsLeft -= pRC->m_picActualBits;
    pRC->m_GOPbitsLeft -= pRC->m_picActualBits;
    pRC->m_framesLeft--;
    printf(" [TotalActualBits %10d bits]", pRC->m_totalactualbits);

    if (pRC->m_framesLeft == 0) {
        pRC->m_framesLeft = pRC->IntraPeriod;
        pRC->m_SeqbitsLeft = pRC->IntraPeriod * (Int)(pRC->TargetBitrate / frame_level_fw.framerate);
        pRC->m_totalactualbits = 0;
    }


#if U0132_TARGET_BITS_SATURATION
    if (pRC->m_CpbSaturationEnabled == 1)
    {
        updateCpbState(pRC->m_picActualBits);
        printf(" [CPB %7d bits]", pRC->m_cpbState);
    }
#endif
#if !U0132_TARGET_BITS_SATURATION
    static Int m_cpbState = 0;
    m_cpbState += pRC->m_picActualBits;
    m_cpbState -= (Int)(pRC->TargetBitrate / frame_level_fw.framerate);
    //printf(" [CPB %7d bits]", m_cpbState);
#endif

    //reset
    pRC->m_picActualHeaderBits = 0;
    //pRC->m_picActualBits = 0;
}
#endif

void rate_control()
{
#if HEVC_RC
    gop_level_ratecontrol();

    frame_level_ratecontrol();
#endif
}

static int FrameNumberInFile;       // The current frame number in the input file

void RC_init_frame()
{
#if RATECONTROL
    if (frame_level_fw.EncControl == 1) {
        if (pRC->IntraPeriod == 0) {
            if (FrameNumberInFile % frame_level_fw.gop_size == 1) {
                pRC->DeltaQP = CalculateGopDeltaQP_RateControl(pRC, frame_level_fw.slice_type, FrameNumberInFile, frame_level_fw.gop_size);
            }
        }
        else if (pRC->IntraPeriod == 1) {
            if ((FrameNumberInFile % frame_level_fw.gop_size == 0) && (FrameNumberInFile != 0)) {
                pRC->DeltaQP = CalculateGopDeltaQP_RateControl(pRC, frame_level_fw.slice_type, FrameNumberInFile, frame_level_fw.gop_size);
            }
        }
        else {
            if ((frame_level_fw.slice_type == 0) && ((pRC->TotalFrames - pRC->CodedFrameNumber) <= (2 * pRC->IntraPeriod * frame_level_fw.gop_size))) {
                Init_FuzzyController(0.50);//enhance adjusting strength of the last GOP group by lmk
            }
            if ((FrameNumberInFile % frame_level_fw.gop_size == 0) && (FrameNumberInFile != 0)) {
                pRC->DeltaQP = CalculateGopDeltaQP_RateControl(pRC, frame_level_fw.slice_type, FrameNumberInFile, frame_level_fw.gop_size);
            }
            else if (pRC->TotalFrames - pRC->CodedFrameNumber == (pRC->TotalFrames - 1) % frame_level_fw.gop_size) {
                pRC->DeltaQP = CalculateGopDeltaQP_RateControl(pRC, frame_level_fw.slice_type, FrameNumberInFile, frame_level_fw.gop_size);
            }
        }

        if ((pRC->CodedFrameNumber % frame_level_fw.gop_size == 1) || (pRC->IntraPeriod == 1)) {
            if (pRC->IntraPeriod > 1 && frame_level_fw.slice_type == 0) {
                int remainGOPnum;
                if ((pRC->TotalFrames - pRC->CodedFrameNumber) > (pRC->IntraPeriod * frame_level_fw.gop_size + frame_level_fw.gop_size)) {
                    remainGOPnum = pRC->IntraPeriod;
                }
                else {
                    remainGOPnum = (int)ceil(1.0F * (pRC->TotalFrames - pRC->CodedFrameNumber) / frame_level_fw.gop_size);
                }

                // handle final GOP by ymzhou
                if ((pRC->TotalFrames - pRC->CodedFrameNumber) <= (pRC->IntraPeriod * frame_level_fw.gop_size + frame_level_fw.gop_size)) {
                    if ((1.0 * remainGOPnum / pRC->IntraPeriod) <= 1.0 / 3) {
                        pRC->DeltaQP += 8;    //as bitrate halve
                    }
                    else if ((1.0 * remainGOPnum / pRC->IntraPeriod) <= 1.0 / 2) {
                        pRC->DeltaQP += 3;
                    }
                    else if ((1.0 * remainGOPnum / pRC->IntraPeriod) <= 2.0 / 3) {
                        pRC->DeltaQP += 4;
                    }
                    else if ((1.0 * remainGOPnum / pRC->IntraPeriod) <= 3.0 / 4) {
                        pRC->DeltaQP += 0;
                    }
                    else if ((1.0 * remainGOPnum / pRC->IntraPeriod) <= 4.0 / 4) {
                        pRC->DeltaQP += 2;
                    }
                    else {
                        pRC->DeltaQP += 0;
                    }
                }

                frame_level_fw.qp_y = pRC->GopAvgBaseQP / pRC->GopAvgBaseQPCount + pRC->DeltaQP;
                //img->qpI = img->qp;
            }
            else {
                frame_level_fw.qp_y += pRC->DeltaQP;
                //img->qpI += pRC->DeltaQP;
            }
            frame_level_fw.qp_y = MAX(MIN_QP, MIN(frame_level_fw.qp_y, MAX_QP + (frame_level_fw.bit_depth_internal - 8) * 8 - 5));
            //img->qpI = MAX(MIN_QP, minn(img->qpI, MAX_QP + (img->sample_bit_depth - 8) * 8 - 5));
            //img->qpP = img->qpI + 1;
        }
        pRC->RcQP = frame_level_fw.qp_y;
#if HEVC_RC
        pRC->m_picActualHeaderBits = 0;
#endif
    }
#endif
}
#if 0
/*---------------------------------vpp----------------------------------*/

/**
* ===========================================================================
* macro defines
* ===========================================================================
*/

#define pack16to32_mask2(mx,my) (((mx) << 16) | ((my) & 0x7FFF))
#define CHECK_MV_RANGE(mx,my)   (!(((pack16to32_mask2(mx,my) + mv_min) | (mv_max - pack16to32_mask2(mx,my))) & 0x80004000))


/* ---------------------------------------------------------------------------
*/
#define COPY3_IF_LT(x, y, a, b, c, d) \
if ((y) < (x)) {\
	(x) = (y); \
	(a) = (b); \
	(c) = (d); \
}

#define COPY4_IF_LT(x, y, a, b, c, d, e, f) \
if ((y) < (x)) {\
	(x) = (y); \
	(a) = (b); \
	(c) = (d); \
	(e) = (f); \
}

/* ---------------------------------------------------------------------------
*/
#define STARAVS_MAX(a, b)       ((a) > (b)? (a) : (b))
#define STARAVS_CLIP3(L, H, v)  (((v) < (L)) ? (L) : (((v) > (H)) ? (H) : (v)))

#define MAKEDWORD(mx, my)       (((my) << 16) | ((mx) & 0xFFFF))
#define STARAVS_SWAP_PTR(x, y)  {void *_t = (x); (x) = (y); (y) = _t;}

/* ---------------------------------------------------------------------------
* weight cost of mvd/ref
*/
#define LAMBDA_ACCURACY_BITS    16
#define LAMBDA_FACTOR(lambda)   ((int)((double)(1<<LAMBDA_ACCURACY_BITS)*lambda+0.5))
#define WEIGHTED_COST(f, bits)  (((f)*(bits))>>LAMBDA_ACCURACY_BITS)

/* ---------------------------------------------------------------------------
* MV cost */
#define MV_COST_IPEL(mx,my)     (WEIGHTED_COST(lambda, p_cost_mvx[(mx) << 2] + p_cost_mvy[(my) << 2]))
#define MV_COST_FPEL(mx,my)     (WEIGHTED_COST(lambda, p_cost_mvx[mx] + p_cost_mvy[my]))
#define MV_COST_FPEL_BID(mx,my) (WEIGHTED_COST(lambda, p_cost_bix[mx] + p_cost_biy[my]))

/* ---------------------------------------------------------------------------
*/
#define VPP_CAL_COST(mx, my) \
	p_me->sad_8x8(p_org, VPP_BLK_SIZE, \
	p_fref + (my)* i_fref + (mx), i_fref) + MV_COST_IPEL(mx, my)

/* ---------------------------------------------------------------------------
*/
#define VPP_COST(mx, my) \
if (CHECK_MV_RANGE(mx, my)) {\
	int cost = p_me->sad_8x8(p_org, VPP_BLK_SIZE, \
	p_fref + (my)* i_fref + (mx), i_fref) + MV_COST_IPEL(mx, my); \
	COPY3_IF_LT(bcost, cost, bmx, mx, bmy, my); \
}

/* ---------------------------------------------------------------------------
*/
#define VPP_COST_X3_DIR(m0x, m0y, d0, m1x, m1y, d1, m2x, m2y, d2) \
{\
	pel_t *pix_base = p_fref + omy * i_fref + omx; \
	p_me->sad_8x8_x3(p_org, \
	pix_base + (m0y)* i_fref + (m0x), \
	pix_base + (m1y)* i_fref + (m1x), \
	pix_base + (m2y)* i_fref + (m2x), \
	i_fref, costs); \
	costs[0] += MV_COST_IPEL(omx + (m0x), omy + (m0y)); \
	costs[1] += MV_COST_IPEL(omx + (m1x), omy + (m1y)); \
	costs[2] += MV_COST_IPEL(omx + (m2x), omy + (m2y)); \
	COPY4_IF_LT(bcost, costs[0], bmx, omx + (m0x), bmy, omy + (m0y), dir, d0); \
	COPY4_IF_LT(bcost, costs[1], bmx, omx + (m1x), bmy, omy + (m1y), dir, d1); \
	COPY4_IF_LT(bcost, costs[2], bmx, omx + (m2x), bmy, omy + (m2y), dir, d2); \
}

/* ---------------------------------------------------------------------------
*/
#define VPP_COST_X4_DIR(m0x, m0y, d0, m1x, m1y, d1, m2x, m2y, d2, m3x, m3y, d3) \
{\
	pel_t *pix_base = p_fref + omy * i_fref + omx; \
	p_me->sad_8x8_x4(p_org, \
	pix_base + (m0y)* i_fref + (m0x), \
	pix_base + (m1y)* i_fref + (m1x), \
	pix_base + (m2y)* i_fref + (m2x), \
	pix_base + (m3y)* i_fref + (m3x), i_fref, costs); \
	costs[0] += MV_COST_IPEL(omx + (m0x), omy + (m0y)); \
	costs[1] += MV_COST_IPEL(omx + (m1x), omy + (m1y)); \
	costs[2] += MV_COST_IPEL(omx + (m2x), omy + (m2y)); \
	costs[3] += MV_COST_IPEL(omx + (m3x), omy + (m3y)); \
	COPY4_IF_LT(bcost, costs[0], bmx, omx + (m0x), bmy, omy + (m0y), dir, d0); \
	COPY4_IF_LT(bcost, costs[1], bmx, omx + (m1x), bmy, omy + (m1y), dir, d1); \
	COPY4_IF_LT(bcost, costs[2], bmx, omx + (m2x), bmy, omy + (m2y), dir, d2); \
	COPY4_IF_LT(bcost, costs[3], bmx, omx + (m3x), bmy, omy + (m3y), dir, d3); \
}

#define VPP_ME_RANGE  64        // max search range in VPP

/* ---------------------------------------------------------------------------
* neighbour properties
*/
enum vpp_block_position_e {
    VPP_BLK_T = 0x01,         /* top  is available */
    VPP_BLK_L = 0x02,         /* left is available */
    VPP_BLK_TR = 0x04,         /* top-right is available */
    VPP_BLK_DL = 0x08          /* down-left is available */
};

/* ---------------------------------------------------------------------------
* intra modes */
enum vpp_intra_pred_e {
    VPP_PRED_HV = 0,
    VPP_PRED_V = 1,
    VPP_PRED_H = 2,
    VPP_PRED_DC = 3
};

/**
* ===========================================================================
* global & local variables
* ===========================================================================
*/

/* ---------------------------------------------------------------------------
* COI  0  1  2  3  4  5  6  7  8  9  10  11  12  13  14  15  16
* REF0    0  0  0  2  0  3  2  4  0  5   3   6   2   7   4   8
* REF1    x  1  2  1  3  2  4  1  5  3   6   2   7   4   8   1
*/
static int refs_in_gop[STARAVS_MAX_GOP_SIZE + 1][2] = {
    // index in GOP:
    // 0
    {-1,-1 },
    // 1        2         3         4         5         6         7         8
    { 0,-1 }, { 0, 1 }, { 0, 2 }, { 2, 1 }, { 0, 3 }, { 3, 2 }, { 2, 4 }, { 4, 1 },
    // 9        10        11        12        13        14        15        16
    { 0, 5 }, { 5, 3 }, { 3, 6 }, { 6, 2 }, { 2, 7 }, { 7, 4 }, { 4, 8 }, { 8, 1 }
};

/* ---------------------------------------------------------------------------
* available intra modes for 8x8 block */
static const int8_t VPP_INTRA_MODES[4][5] = {
    { VPP_PRED_DC,         -1,          -1,         -1, -1 },    /* NULL */
    { VPP_PRED_DC, VPP_PRED_V,          -1,         -1, -1 },    /* TOP  */
    { VPP_PRED_DC, VPP_PRED_H,          -1,         -1, -1 },    /* LEFT */
    { VPP_PRED_DC, VPP_PRED_HV, VPP_PRED_V, VPP_PRED_H, -1 }     /* TOP & LEFT */
};

/* ---------------------------------------------------------------------------
* radius 2 hexagon
* repeated entries are to avoid having to compute mod6 every time */
static const int8_t VPP_HEX2[8][2] = {
    {-1,-2 },       /* 0,       0(6)   5        */
    {-2, 0 },       /* 1,                       */
    {-1, 2 },       /* 2,                       */
    { 1, 2 },       /* 3,   1(7)   *       4    */
    { 2, 0 },       /* 4,                       */
    { 1,-2 },       /* 5,                       */
    {-1,-2 },       /* 6,       2      3        */
    {-2, 0 }        /* 7,                       */
};

/* ---------------------------------------------------------------------------
* (x - 1) % 6 */
static const S8 VPP_M1MOD6[8] = { 5, 0, 1, 2, 3, 4, 5, 0 };

/* ---------------------------------------------------------------------------
* radius 1 diamond
* repeated entries are to avoid having to compute mod4 every time */
static const int8_t VPP_DIA1[6][2] = {
    { 0,-1 },       /* 0,                       */
    {-1, 0 },       /* 1,           0(4)        */
    { 0, 1 },       /* 2,                       */
    { 1, 0 },       /* 3,      1(5) *   3       */
    { 0,-1 },       /* 4,                       */
    {-1, 0 }        /* 5,           2           */
};

/* ---------------------------------------------------------------------------
* (x - 1) % 4 */
static const S8 VPP_M1MOD4[6] = { 3, 0, 1, 2, 3, 0 };

static int estimate_intra_block_cost(pel_t * p_org, pel_t * p_fenc, int i_fenc, int neighbour, U32 * min_rate)
{
    unsigned short Q_TAB[80] = {
        32768, 29775, 27554, 25268, 23170, 21247, 19369, 17770,
        16302, 15024, 13777, 12634, 11626, 10624, 9742, 8958,
        8192, 7512, 6889, 6305, 5793, 5303, 4878, 4467,
        4091, 3756, 3444, 3161, 2894, 2654, 2435, 2235,
        2048, 1878, 1722, 1579, 1449, 1329, 1218, 1117,
        1024, 939, 861, 790, 724, 664, 609, 558,
        512, 470, 430, 395, 362, 332, 304, 279,
        256, 235, 215, 197, 181, 166, 152, 140,
        128, 116, 108, 99, 91, 83, 76, 69,
        64, 59, 54, 49, 45, 41, 38, 35
    };

    pixel_cmp_t block_cost = pRC->pixf.sad[1];
    const int8_t* pred_mode = VPP_INTRA_MODES[neighbour & (VPP_BLK_T | VPP_BLK_L)];
    int mcost = MAX_DISTORTION;
    pel_t p_pred[8][8];
    pel_t p_top[8];
    pel_t p_left[8];
    pel_t* p_tmp;
    int y, cost;

    //for R-Est
    int m, n;
    U8  orgY_8x8[8][8];
    U8  predY_8x8[8][8];
    S23 dst[8][8];
    S17 quant_out[8][8];
    U1 NoResi_flag[1];
    U1 valid_flag[1];
    U4 cutype[1];
    NoResi_flag[0] = 0;
    valid_flag[0] = 1;
    cutype[0] = 10;
    U3 img_type = img->type;
    U1 useDQP = fw_param.useDQP;
    S7 mostProbableMode[2] = { -1, -1 };
    U6 cpb[1];
    S2 delta_qp = 0;
    U32 rate = 0x7FFFFFFF;

    /* prepare the buffers of top and left pixels */
    if (neighbour & VPP_BLK_T) {
        /* copy pixels in top row */
        memcpy(p_top, p_fenc - i_fenc, VPP_BLK_SIZE * sizeof(pel_t));
    }
    if (neighbour & VPP_BLK_L) {
        p_tmp = p_fenc - 1;
        /* copy pixels in left col */
        for (y = 0; y < VPP_BLK_SIZE; y++) {
            p_left[y] = *p_tmp;
            p_tmp += i_fenc;
        }
    }

    /* compute min cost */
    for (; *pred_mode >= 0; pred_mode++) {
        /* get predication block */
        pRC->intra_pred[*pred_mode](&p_pred[0][0], p_top, p_left);

        /* compute cost */
        //cost = block_cost(p_org, VPP_BLK_SIZE, &p_pred[0][0], VPP_BLK_SIZE);
        int sum = 0;
        for (int j = 0; j < VPP_BLK_SIZE; j++) {
            for (int i = 0; i < VPP_BLK_SIZE; i++)
            {
                sum += abs(p_org[i + j * VPP_BLK_SIZE] - p_pred[i][j]);
            }
        }
        cost = sum;
        if (cost < mcost) {
            mcost = cost;
        }

        for (m = 0; m < 8; m++) {
            for (n = 0; n < 8; n++) {
                orgY_8x8[m][n] = (U8)p_org[m * VPP_BLK_SIZE + n];
                predY_8x8[m][n] = (U8)p_pred[m][n];
            }
        }
        transform_8x8_h_vpp(orgY_8x8, predY_8x8, dst, NoResi_flag, valid_flag);
        transform_8x8_v_vpp(dst, quant_out, Q_TAB[pRC->m_frameQp - MIN_QP], cutype, NoResi_flag, valid_flag, cpb);
        rate = rate_luma_8x8_vpp(quant_out, img_type, (S13)0, (S13)0, useDQP, cutype[0], 0, mostProbableMode, cpb[0], delta_qp);
        if (rate < *min_rate) {
            *min_rate = rate;
        }
    }

    return mcost;
}

/* ---------------------------------------------------------------------------
* return minimum motion cost after search
*/
static int estimate_inter_block_cost(pel_t org[8][8], vpp_me_t * p_me, pel_t * p_org, pel_t * p_fref, int i_fref, U32 * min_rate)
{
    unsigned short Q_TAB[80] = {
        32768, 29775, 27554, 25268, 23170, 21247, 19369, 17770,
        16302, 15024, 13777, 12634, 11626, 10624, 9742, 8958,
        8192, 7512, 6889, 6305, 5793, 5303, 4878, 4467,
        4091, 3756, 3444, 3161, 2894, 2654, 2435, 2235,
        2048, 1878, 1722, 1579, 1449, 1329, 1218, 1117,
        1024, 939, 861, 790, 724, 664, 609, 558,
        512, 470, 430, 395, 362, 332, 304, 279,
        256, 235, 215, 197, 181, 166, 152, 140,
        128, 116, 108, 99, 91, 83, 76, 69,
        64, 59, 54, 49, 45, 41, 38, 35
    };

    ALIGN16(int costs[8]);
    int mv_x_min = p_me->mv_min[0];
    int mv_y_min = p_me->mv_min[1];
    int mv_x_max = p_me->mv_max[0];
    int mv_y_max = p_me->mv_max[1];
    const int lambda = 99999;    // factor for determining Lagrangian's motion cost
    const uint32_t mv_min = pack16to32_mask2(-mv_x_min, -mv_y_min);
    const uint32_t mv_max = pack16to32_mask2(mv_x_max, mv_y_max) | 0x8000;
    const uint16_t* p_cost_mvx = p_me->mvbits - p_me->pmv.x;
    const uint16_t* p_cost_mvy = p_me->mvbits - p_me->pmv.y;
    uint32_t pmv;
    int bcost;
    int bmx, bmy, omx, omy;
    int i, dir, idx;

    //for R-Est
    int m, n;
    U8  orgY_8x8[8][8];
    U8  predY_8x8[8][8];
    S23 dst[8][8];
    S17 quant_out[8][8];
    U1 NoResi_flag[1];
    U1 valid_flag[1];
    U4 cutype[1];
    NoResi_flag[0] = 0;
    valid_flag[0] = 1;
    cutype[0] = 1;
    U3 img_type = img->type;
    U1 useDQP = fw_param.useDQP;
    S7 mostProbableMode[2] = { 0, 0 };
    U6 cpb[1];
    S2 delta_qp = 0;
    U32 rate = 0x7FFFFFFF;

    /* -------------------------------------------------------------
    * 1, select a start search point */

    /* calculate and check the fullpel MVP first */
    bmx = STARAVS_CLIP3(mv_x_min, mv_x_max, p_me->pmv.x);
    bmy = STARAVS_CLIP3(mv_y_min, mv_y_max, p_me->pmv.y);
    pmv = MAKEDWORD(bmx, bmy);

    if (CHECK_MV_RANGE(bmx, bmy)) {
        bcost = VPP_CAL_COST(bmx, bmy);
        for (m = 0; m < 8; m++) {
            for (n = 0; n < 8; n++) {
                orgY_8x8[m][n] = org[m][n];
                predY_8x8[m][n] = p_fref[(bmy + m) * i_fref + bmx + n];
            }
        }
        transform_8x8_h_vpp(orgY_8x8, predY_8x8, dst, NoResi_flag, valid_flag);
        transform_8x8_v_vpp(dst, quant_out, Q_TAB[pRC->m_frameQp - MIN_QP], cutype, NoResi_flag, valid_flag, cpb);
        rate = rate_luma_8x8_vpp(quant_out, img_type, (S13)bmx, (S13)bmy, useDQP, cutype[0], 0, mostProbableMode, cpb[0], delta_qp);
        if (rate < *min_rate) {
            *min_rate = rate;
        }
    }
    else {
        return MAX_DISTORTION;
    }

    /* same as above, except the condition is simpler */
    if (pmv) {
        VPP_COST(0, 0);
        for (m = 0; m < 8; m++) {
            for (n = 0; n < 8; n++) {
                orgY_8x8[m][n] = org[m][n];
                predY_8x8[m][n] = p_fref[(0 + m) * i_fref + 0 + n];
            }
        }
        transform_8x8_h_vpp(orgY_8x8, predY_8x8, dst, NoResi_flag, valid_flag);
        transform_8x8_v_vpp(dst, quant_out, Q_TAB[pRC->m_frameQp - MIN_QP], cutype, NoResi_flag, valid_flag, cpb);
        rate = rate_luma_8x8_vpp(quant_out, img_type, (S13)0, (S13)0, useDQP, cutype[0], 0, mostProbableMode, cpb[0], delta_qp);
        if (rate < *min_rate) {
            *min_rate = rate;
        }
    }

    if (!CHECK_MV_RANGE(bmx, bmy)) {
        return MAX_DISTORTION;
    }

    /* -------------------------------------------------------------
    * 2, start search... */
    /* 2.1, use hexagon search */
    dir = 0;                                          /*   6   5   */
    omx = bmx;                                        /*           */
    omy = bmy;                                        /* 1   *   4 */
    VPP_COST_X3_DIR(-1, -2, 6, 1, -2, 5, -2, 0, 1);   /*           */
    VPP_COST_X3_DIR(2, 0, 4, -1, 2, 2, 1, 2, 3);      /*   2   3   */

    /*(-1, -2, 6)*/
    for (m = 0; m < 8; m++) {
        for (n = 0; n < 8; n++) {
            orgY_8x8[m][n] = org[m][n];
            predY_8x8[m][n] = p_fref[(-2 + omy + m) * i_fref + (-1 + omx) + n];
        }
    }
    transform_8x8_h_vpp(orgY_8x8, predY_8x8, dst, NoResi_flag, valid_flag);
    transform_8x8_v_vpp(dst, quant_out, Q_TAB[pRC->m_frameQp - MIN_QP], cutype, NoResi_flag, valid_flag, cpb);
    rate = rate_luma_8x8_vpp(quant_out, img_type, (S13)(omx - 1), (S13)(omy - 2), useDQP, cutype[0], 0, mostProbableMode, cpb[0], delta_qp);
    if (rate < *min_rate) {
        *min_rate = rate;
    }
    /*(1, -2, 5)*/
    for (m = 0; m < 8; m++) {
        for (n = 0; n < 8; n++) {
            orgY_8x8[m][n] = org[m][n];
            predY_8x8[m][n] = p_fref[(omy - 2 + m) * i_fref + omx + 1 + n];
        }
    }
    transform_8x8_h_vpp(orgY_8x8, predY_8x8, dst, NoResi_flag, valid_flag);
    transform_8x8_v_vpp(dst, quant_out, Q_TAB[pRC->m_frameQp - MIN_QP], cutype, NoResi_flag, valid_flag, cpb);
    rate = rate_luma_8x8_vpp(quant_out, img_type, (S13)(1 + omx), (S13)(omy - 2), useDQP, cutype[0], 0, mostProbableMode, cpb[0], delta_qp);
    if (rate < *min_rate) {
        *min_rate = rate;
    }
    /*(-2, 0, 1)*/
    for (m = 0; m < 8; m++) {
        for (n = 0; n < 8; n++) {
            orgY_8x8[m][n] = org[m][n];
            predY_8x8[m][n] = p_fref[(omy + 0 + m) * i_fref + (omx - 2) + n];
        }
    }
    transform_8x8_h_vpp(orgY_8x8, predY_8x8, dst, NoResi_flag, valid_flag);
    transform_8x8_v_vpp(dst, quant_out, Q_TAB[pRC->m_frameQp - MIN_QP], cutype, NoResi_flag, valid_flag, cpb);
    rate = rate_luma_8x8_vpp(quant_out, img_type, (S13)(omx - 2), (S13)(omx + 0), useDQP, cutype[0], 0, mostProbableMode, cpb[0], delta_qp);
    if (rate < *min_rate) {
        *min_rate = rate;
    }
    /*(2, 0, 4)*/
    for (m = 0; m < 8; m++) {
        for (n = 0; n < 8; n++) {
            orgY_8x8[m][n] = org[m][n];
            predY_8x8[m][n] = p_fref[(omy + 0 + m) * i_fref + omx + 2 + n];
        }
    }
    transform_8x8_h_vpp(orgY_8x8, predY_8x8, dst, NoResi_flag, valid_flag);
    transform_8x8_v_vpp(dst, quant_out, Q_TAB[pRC->m_frameQp - MIN_QP], cutype, NoResi_flag, valid_flag, cpb);
    rate = rate_luma_8x8_vpp(quant_out, img_type, (S13)(omx + 2), (S13)(omy + 0), useDQP, cutype[0], 0, mostProbableMode, cpb[0], delta_qp);
    if (rate < *min_rate) {
        *min_rate = rate;
    }
    /*(-1, 2, 2)*/
    for (m = 0; m < 8; m++) {
        for (n = 0; n < 8; n++) {
            orgY_8x8[m][n] = org[m][n];
            predY_8x8[m][n] = p_fref[(omy + 2 + m) * i_fref + (omx - 1) + n];
        }
    }
    transform_8x8_h_vpp(orgY_8x8, predY_8x8, dst, NoResi_flag, valid_flag);
    transform_8x8_v_vpp(dst, quant_out, Q_TAB[pRC->m_frameQp - MIN_QP], cutype, NoResi_flag, valid_flag, cpb);
    rate = rate_luma_8x8_vpp(quant_out, img_type, (S13)(omx - 1), (S13)(omy + 2), useDQP, cutype[0], 0, mostProbableMode, cpb[0], delta_qp);
    if (rate < *min_rate) {
        *min_rate = rate;
    }
    /*(1, 2, 3)*/
    for (m = 0; m < 8; m++) {
        for (n = 0; n < 8; n++) {
            orgY_8x8[m][n] = org[m][n];
            predY_8x8[m][n] = p_fref[(omy + 2 + m) * i_fref + omx + 1 + n];
        }
    }
    transform_8x8_h_vpp(orgY_8x8, predY_8x8, dst, NoResi_flag, valid_flag);
    transform_8x8_v_vpp(dst, quant_out, Q_TAB[pRC->m_frameQp - MIN_QP], cutype, NoResi_flag, valid_flag, cpb);
    rate = rate_luma_8x8_vpp(quant_out, img_type, (S13)(omx + 1), (S13)(omy + 2), useDQP, cutype[0], 0, mostProbableMode, cpb[0], delta_qp);
    if (rate < *min_rate) {
        *min_rate = rate;
    }


    if (dir) {
        const int8_t(*hex)[2];

        idx = dir - 1;      /* start array index */
        /* half hexagon, not overlapping the previous iteration */
        for (i = 0; i < VPP_ME_RANGE && CHECK_MV_RANGE(bmx, bmy); i++) {
            dir = 0;
            omx = bmx;
            omy = bmy;
            hex = &VPP_HEX2[idx];
            VPP_COST_X3_DIR(hex[0][0], hex[0][1], 1, hex[1][0], hex[1][1], 2, hex[2][0], hex[2][1], 3);

            for (m = 0; m < 8; m++) {
                for (n = 0; n < 8; n++) {
                    orgY_8x8[m][n] = org[m][n];
                    predY_8x8[m][n] = p_fref[(omy + hex[0][1] + m) * i_fref + omx + hex[0][0] + n];
                }
            }
            transform_8x8_h_vpp(orgY_8x8, predY_8x8, dst, NoResi_flag, valid_flag);
            transform_8x8_v_vpp(dst, quant_out, Q_TAB[pRC->m_frameQp - MIN_QP], cutype, NoResi_flag, valid_flag, cpb);
            rate = rate_luma_8x8_vpp(quant_out, img_type, (S13)(omx + hex[0][0]), (S13)(omy + hex[0][1]), useDQP, cutype[0], 0, mostProbableMode, cpb[0], delta_qp);
            if (rate < *min_rate) {
                *min_rate = rate;
            }

            for (m = 0; m < 8; m++) {
                for (n = 0; n < 8; n++) {
                    orgY_8x8[m][n] = org[m][n];
                    predY_8x8[m][n] = p_fref[(hex[1][1] + omy + m) * i_fref + omx + hex[1][0] + n];
                }
            }
            transform_8x8_h_vpp(orgY_8x8, predY_8x8, dst, NoResi_flag, valid_flag);
            transform_8x8_v_vpp(dst, quant_out, Q_TAB[pRC->m_frameQp - MIN_QP], cutype, NoResi_flag, valid_flag, cpb);
            rate = rate_luma_8x8_vpp(quant_out, img_type, (S13)(omx + hex[1][0]), (S13)(omy + hex[1][1]), useDQP, cutype[0], 0, mostProbableMode, cpb[0], delta_qp);
            if (rate < *min_rate) {
                *min_rate = rate;
            }

            for (m = 0; m < 8; m++) {
                for (n = 0; n < 8; n++) {
                    orgY_8x8[m][n] = org[m][n];
                    predY_8x8[m][n] = p_fref[(hex[2][1] + omy + m) * i_fref + hex[2][0] + omx + n];
                }
            }
            transform_8x8_h_vpp(orgY_8x8, predY_8x8, dst, NoResi_flag, valid_flag);
            transform_8x8_v_vpp(dst, quant_out, Q_TAB[pRC->m_frameQp - MIN_QP], cutype, NoResi_flag, valid_flag, cpb);
            rate = rate_luma_8x8_vpp(quant_out, img_type, (S13)(omx + hex[2][0]), (S13)(omy + hex[2][1]), useDQP, cutype[0], 0, mostProbableMode, cpb[0], delta_qp);
            if (rate < *min_rate) {
                *min_rate = rate;
            }

            if (!dir) {
                break;      /* early terminate */
            }
            idx = VPP_M1MOD6[dir + idx - 1];    /* next start array index */
        }
    }

    /* 2.2, use diamond search */
    dir = 0;
    if (CHECK_MV_RANGE(bmx, bmy)) {
        omx = bmx;                                              /*    4    */
        omy = bmy;                                              /*  1 * 3  */
        VPP_COST_X4_DIR(0, -1, 4, -1, 0, 1, 1, 0, 3, 0, 1, 2);  /*    2    */
        /*(0, -1, 4)*/
        for (m = 0; m < 8; m++) {
            for (n = 0; n < 8; n++) {
                orgY_8x8[m][n] = org[m][n];
                predY_8x8[m][n] = p_fref[(omy - 1 + m) * i_fref + omx + 0 + n];
            }
        }
        transform_8x8_h_vpp(orgY_8x8, predY_8x8, dst, NoResi_flag, valid_flag);
        transform_8x8_v_vpp(dst, quant_out, Q_TAB[pRC->m_frameQp - MIN_QP], cutype, NoResi_flag, valid_flag, cpb);
        rate = rate_luma_8x8_vpp(quant_out, img_type, (S13)(omx + 0), (S13)(omy - 1), useDQP, cutype[0], 0, mostProbableMode, cpb[0], delta_qp);
        if (rate < *min_rate) {
            *min_rate = rate;
        }
        /*(-1, 0, 1)*/
        for (m = 0; m < 8; m++) {
            for (n = 0; n < 8; n++) {
                orgY_8x8[m][n] = org[m][n];
                predY_8x8[m][n] = p_fref[(omy + 0 + m) * i_fref + omx - 1 + n];
            }
        }
        transform_8x8_h_vpp(orgY_8x8, predY_8x8, dst, NoResi_flag, valid_flag);
        transform_8x8_v_vpp(dst, quant_out, Q_TAB[pRC->m_frameQp - MIN_QP], cutype, NoResi_flag, valid_flag, cpb);
        rate = rate_luma_8x8_vpp(quant_out, img_type, (S13)(omx - 1), (S13)(omy + 0), useDQP, cutype[0], 0, mostProbableMode, cpb[0], delta_qp);
        if (rate < *min_rate) {
            *min_rate = rate;
        }
        /*(1, 0, 3)*/
        for (m = 0; m < 8; m++) {
            for (n = 0; n < 8; n++) {
                orgY_8x8[m][n] = org[m][n];
                predY_8x8[m][n] = p_fref[(omy + 0 + m) * i_fref + omx + 1 + n];
            }
        }
        transform_8x8_h_vpp(orgY_8x8, predY_8x8, dst, NoResi_flag, valid_flag);
        transform_8x8_v_vpp(dst, quant_out, Q_TAB[pRC->m_frameQp - MIN_QP], cutype, NoResi_flag, valid_flag, cpb);
        rate = rate_luma_8x8_vpp(quant_out, img_type, (S13)(omx + 1), (S13)(omy + 0), useDQP, cutype[0], 0, mostProbableMode, cpb[0], delta_qp);
        if (rate < *min_rate) {
            *min_rate = rate;
        }
        /*(0, 1, 2)*/
        for (m = 0; m < 8; m++) {
            for (n = 0; n < 8; n++) {
                orgY_8x8[m][n] = org[m][n];
                predY_8x8[m][n] = p_fref[(omy + 1 + m) * i_fref + omx + 0 + n];
            }
        }
        transform_8x8_h_vpp(orgY_8x8, predY_8x8, dst, NoResi_flag, valid_flag);
        transform_8x8_v_vpp(dst, quant_out, Q_TAB[pRC->m_frameQp - MIN_QP], cutype, NoResi_flag, valid_flag, cpb);
        rate = rate_luma_8x8_vpp(quant_out, img_type, (S13)(omx + 0), (S13)(omy + 1), useDQP, cutype[0], 0, mostProbableMode, cpb[0], delta_qp);
        if (rate < *min_rate) {
            *min_rate = rate;
        }
    }

    if (dir) {
        const int8_t(*dia)[2];
        idx = dir - 1;      /* start array index */
        /* half diamond, not overlapping the previous iteration */
        for (i = 0; i < VPP_ME_RANGE && CHECK_MV_RANGE(bmx, bmy); i++) {
            dir = 0;
            omx = bmx;
            omy = bmy;
            dia = &VPP_DIA1[idx];
            VPP_COST_X3_DIR(dia[0][0], dia[0][1], 1, dia[1][0], dia[1][1], 2, dia[2][0], dia[2][1], 3);
            for (m = 0; m < 8; m++) {
                for (n = 0; n < 8; n++) {
                    orgY_8x8[m][n] = org[m][n];
                    predY_8x8[m][n] = p_fref[(omy + dia[0][1] + m) * i_fref + omx + dia[0][0] + n];
                }
            }
            transform_8x8_h_vpp(orgY_8x8, predY_8x8, dst, NoResi_flag, valid_flag);
            transform_8x8_v_vpp(dst, quant_out, Q_TAB[pRC->m_frameQp - MIN_QP], cutype, NoResi_flag, valid_flag, cpb);
            rate = rate_luma_8x8_vpp(quant_out, img_type, (S13)(omx + dia[0][0]), (S13)(omy + dia[0][1]), useDQP, cutype[0], 0, mostProbableMode, cpb[0], delta_qp);
            if (rate < *min_rate) {
                *min_rate = rate;
            }

            for (m = 0; m < 8; m++) {
                for (n = 0; n < 8; n++) {
                    orgY_8x8[m][n] = org[m][n];
                    predY_8x8[m][n] = p_fref[(dia[1][1] + omy + m) * i_fref + omx + dia[1][0] + n];
                }
            }
            transform_8x8_h_vpp(orgY_8x8, predY_8x8, dst, NoResi_flag, valid_flag);
            transform_8x8_v_vpp(dst, quant_out, Q_TAB[pRC->m_frameQp - MIN_QP], cutype, NoResi_flag, valid_flag, cpb);
            rate = rate_luma_8x8_vpp(quant_out, img_type, (S13)(omx + dia[1][0]), (S13)(omy + dia[1][1]), useDQP, cutype[0], 0, mostProbableMode, cpb[0], delta_qp);
            if (rate < *min_rate) {
                *min_rate = rate;
            }

            for (m = 0; m < 8; m++) {
                for (n = 0; n < 8; n++) {
                    orgY_8x8[m][n] = org[m][n];
                    predY_8x8[m][n] = p_fref[(dia[2][1] + omy + m) * i_fref + dia[2][0] + omx + n];
                }
            }
            transform_8x8_h_vpp(orgY_8x8, predY_8x8, dst, NoResi_flag, valid_flag);
            transform_8x8_v_vpp(dst, quant_out, Q_TAB[pRC->m_frameQp - MIN_QP], cutype, NoResi_flag, valid_flag, cpb);
            rate = rate_luma_8x8_vpp(quant_out, img_type, (S13)(omx + dia[2][0]), (S13)(omy + dia[2][1]), useDQP, cutype[0], 0, mostProbableMode, cpb[0], delta_qp);
            if (rate < *min_rate) {
                *min_rate = rate;
            }

            if (!dir) {
                break;      /* early terminate */
            }
            idx = VPP_M1MOD4[dir + idx - 1];    /* next start array index */
        }
    }

    /* -------------------------------------------------------------
    * 3, store the results of fullpel search */
    p_me->bmv.v = MAKEDWORD(bmx, bmy);

    return bcost;
}

static void estimate_frame_cost()
{
    copy_pp_t block_copy = pRC->pixf.copy_pp[1];
    const int height = img->height / 2;
    const int width = img->width / 2;
    const int i_fenc = width + 2 * VPP_PAD;
    const int i_fref = width + 2 * VPP_PAD;
    int64_t best_cost_sum = 0; /* sum cost of all blocks */
    int64_t inter_cost_sum = 0; /* sum cost of all blocks */
    int64_t intra_cost_sum = 0; /* sum cost of all blocks */
    pel_t org[8][8];
    pel_t* p_org = &org[0][0];
    pel_t* p_fref;              /* forward  reference frame */
    pel_t* p_fenc;
    vpp_me_t* p_me = &pRC->me;
    // int   *p_row_cost = frm->complxs.i_row_cost;
    // int row_cost_sum = 0;
    int intra_num = 0;
    int inter_cost, intra_cost;
    int neighbour;
    int x, y;
    U32 min_rate;
    U32 min_rate_sum = 0;

    /* compute the complexity (each block is an intra mode) */
    p_me->bmv.v = 0;
    for (y = 0; y < height; y += VPP_BLK_SIZE) {
        neighbour = (y > 0) ? VPP_BLK_T : 0;
        p_fref = pRC->ref_yuv + y * i_fref + 8 * i_fref + VPP_BLK_SIZE;
        //p_fenc = pRC->cur_yuv + y * i_fenc + 8 * i_fenc + VPP_BLK_SIZE;
        p_fenc = pRC->cur_yuv + y * i_fenc + VPP_BLK_SIZE;
        for (x = 0; x < width; x += VPP_BLK_SIZE) {
            /* init */
            min_rate = 0x7FFFFFFF;
            neighbour |= (x > 0) ? VPP_BLK_L : 0;

            /* copy pixel block from current original source */
            //block_copy(p_org, VPP_BLK_SIZE, p_fenc, i_fenc);

            for (int j = 0; j < VPP_BLK_SIZE; j++)
            {
                for (int i = 0; i < VPP_BLK_SIZE; i++)
                {
                    p_org[i + j * VPP_BLK_SIZE] = p_fenc[i + j * i_fenc];
                }
            }
            /* calculate max allowed MV range */
            p_me->mv_min[0] = (-VPP_BLK_SIZE - x) + 2;
            p_me->mv_min[1] = (-VPP_BLK_SIZE - y) + 2;
            p_me->mv_max[0] = (width + VPP_BLK_SIZE - x - VPP_BLK_SIZE) - 2;
            p_me->mv_max[1] = (height + VPP_BLK_SIZE - y - VPP_BLK_SIZE) - 2;

            /* set PMV */
            if (x > 0) {
                p_me->pmv = p_me->bmv;// using left mv as PMV
            }
            else {
                p_me->pmv.v = 0;
            }

            /* estimate and sum the cost of current block */
            if (img->type != I_IMG) {
                inter_cost = estimate_inter_block_cost(org, p_me, p_org, p_fref, i_fref, &min_rate);
            }
            intra_cost = estimate_intra_block_cost(p_org, p_fenc, i_fenc, neighbour, &min_rate);

            inter_cost_sum += inter_cost;
            intra_cost_sum += intra_cost;
            min_rate_sum += min_rate;
            if (inter_cost < intra_cost) {
                // row_cost_sum  += inter_cost;
                best_cost_sum += inter_cost;
            }
            else {
                intra_num++;
                // row_cost_sum  += intra_cost;
                best_cost_sum += intra_cost;
            }

            /* update */
            p_fref += VPP_BLK_SIZE;
            p_fenc += VPP_BLK_SIZE;
        }

        /* update row */
        // p_row_cost[y] = row_cost_sum;
        // row_cost_sum  = 0;
    }

    /* store the results */
    pRC->complxs.i_best_cost = best_cost_sum;
    pRC->complxs.i_inter_cost = inter_cost_sum;
    pRC->complxs.i_intra_cost = intra_cost_sum;
    pRC->complxs.i_sum_intras = intra_num;
    pRC->complxs.i_min_rate = min_rate_sum;
}

void starvpp_estimate_frame_complexity()
{
    /* estimate the frame complexity */
    if (img->type != I_IMG) {
        estimate_frame_cost();
    }
    else {
        /* estimate like P-frame */
        estimate_frame_cost();

        /* change i_best_cost equal to i_intra_cost */
        pRC->complxs.i_best_cost = pRC->complxs.i_intra_cost;
    }
}

/* ---------------------------------------------------------------------------
*/
static void vpp_intra_pred_hv_c(pel_t * p_pred, pel_t * p_top, pel_t * p_left)
{
    int x, y;

    for (y = 0; y < VPP_BLK_SIZE; y++) {
        for (x = 0; x < VPP_BLK_SIZE; x++) {
            p_pred[x] = (pel_t)((p_top[x] + p_left[y] + 1) >> 1);
        }
        p_pred += VPP_BLK_SIZE;
    }
}

/* ---------------------------------------------------------------------------
*/
static void vpp_intra_pred_v_c(pel_t * p_pred, pel_t * p_top, pel_t * p_left)
{
    int y;

    for (y = 0; y < VPP_BLK_SIZE; y++) {
        memcpy(p_pred, p_top, VPP_BLK_SIZE * sizeof(pel_t));
        p_pred += VPP_BLK_SIZE;
    }
}

/* ---------------------------------------------------------------------------
*/
static void vpp_intra_pred_h_c(pel_t * p_pred, pel_t * p_top, pel_t * p_left)
{
    int x, y;

    for (y = 0; y < VPP_BLK_SIZE; y++) {
        //memset(p_pred, p_left[y], VPP_BLK_SIZE);
        for (x = 0; x < VPP_BLK_SIZE; x++) {
            p_pred[x] = p_left[y];
        }
        p_pred += VPP_BLK_SIZE;
    }
}

/* ---------------------------------------------------------------------------
*/
static void vpp_intra_pred_dc_c(pel_t * p_pred, pel_t * p_top, pel_t * p_left)
{
    //memset(p_pred, 128, VPP_BLK_SIZE * VPP_BLK_SIZE);
    int x;
    for (x = 0; x < VPP_BLK_SIZE * VPP_BLK_SIZE; x++) {
        p_pred[x] = 128;
    }
}

void starvpp_vpp_intra_pred_init(uint32_t cpu, vpp_ipred_t * ipred)
{
    ipred[VPP_PRED_HV] = vpp_intra_pred_hv_c;
    ipred[VPP_PRED_V] = vpp_intra_pred_v_c;
    ipred[VPP_PRED_H] = vpp_intra_pred_h_c;
    ipred[VPP_PRED_DC] = vpp_intra_pred_dc_c;

#if HAVE_MMX1
    if (cpu & STARAVS_CPU_SSE4) {
        ipred[VPP_PRED_HV] = staravs_vpp_intra_pred_hv_sse4;
        ipred[VPP_PRED_V] = staravs_vpp_intra_pred_v_sse4;
        ipred[VPP_PRED_H] = staravs_vpp_intra_pred_h_sse4;
    }

    if (cpu & STARAVS_CPU_SSE2) {
        ipred[VPP_PRED_DC] = staravs_vpp_intra_pred_dc_sse2;
    }
#endif
}

void starvpp_me_init()
{
    int me_range = VPP_ME_RANGE;
    int subpel_num = 4 * (2 * me_range + 3);
    int max_mv_bits = 5 + 2 * (int)ceil(log(subpel_num + 1) / log(2) + 1e-10);
    int max_mvd = (1 << ((max_mv_bits >> 1))) - 1;
    int bits, i, imin, imax;

    /* set pointer of mvbits */
    pRC->me.mvbits = (uint16_t*)(malloc((max_mvd * 2 + 1) * sizeof(uint16_t)));
    pRC->me.mvbits += max_mvd;       // reset the array offset

    // init array of motion vector bits
    pRC->me.mvbits[0] = 1;
    for (bits = 3; bits <= max_mv_bits; bits += 2) {
        imax = 1 << (bits >> 1);
        imin = imax >> 1;

        for (i = imin; i < imax; i++) {
            pRC->me.mvbits[-i] = pRC->me.mvbits[i] = (uint16_t)bits;
        }
    }
}

void transform_8x8_h_vpp(U8 orgY_8x8[8][8], U8 predY_8x8[8][8], S23 dst[8][8], U1 NoResi_flag[1], U1 valid_flag[1])
{
#pragma HLS ARRAY_PARTITION variable=orgY_8x8 dim=2
#pragma HLS ARRAY_PARTITION variable=orgY_8x8 dim=1
#pragma HLS ARRAY_PARTITION variable=predY_8x8 dim=2
#pragma HLS ARRAY_PARTITION variable=predY_8x8 dim=1
#pragma HLS ARRAY_PARTITION variable=dst dim=2
#pragma HLS ARRAY_PARTITION variable=dst dim=1
    int i, j, k;
    int E[4];
    int O[4];
#pragma HLS ARRAY_PARTITION variable=O dim=1
#pragma HLS ARRAY_PARTITION variable=E dim=1
    int EE[2];
#pragma HLS ARRAY_PARTITION variable=EE dim=1
    int EO[2];
#pragma HLS ARRAY_PARTITION variable=EO dim=1

    if (valid_flag[0] == 1) {
        if (NoResi_flag[0] == 0) {
            for (i = 0; i < 4; i++) {
#pragma HLS PIPELINE
                for (j = 0; j < 2; j++) {
#pragma HLS UNROLL
                    /* E and O*/
                    for (k = 0; k < 4; k++) {
#pragma HLS UNROLL
                        E[k] = orgY_8x8[2 * i + j][k] - predY_8x8[2 * i + j][k] + orgY_8x8[2 * i + j][7 - k] - predY_8x8[2 * i + j][7 - k];
                        O[k] = orgY_8x8[2 * i + j][k] - predY_8x8[2 * i + j][k] - orgY_8x8[2 * i + j][7 - k] + predY_8x8[2 * i + j][7 - k];
                    }
                    /* EE and EO */
                    EE[0] = E[0] + E[3];
                    EO[0] = E[0] - E[3];
                    EE[1] = E[1] + E[2];
                    EO[1] = E[1] - E[2];

                    dst[0][2 * i + j] = (32 * EE[0] + 32 * EE[1] + 1) >> 1;
                    dst[4][2 * i + j] = (32 * EE[0] - 32 * EE[1] + 1) >> 1;
                    dst[2][2 * i + j] = (42 * EO[0] + 17 * EO[1] + 1) >> 1;
                    dst[6][2 * i + j] = (17 * EO[0] - 42 * EO[1] + 1) >> 1;

                    dst[1][2 * i + j] = (44 * O[0] + 38 * O[1] + 25 * O[2] + 9 * O[3] + 1) >> 1;
                    dst[3][2 * i + j] = (38 * O[0] - 9 * O[1] - 44 * O[2] - 25 * O[3] + 1) >> 1;
                    dst[5][2 * i + j] = (25 * O[0] - 44 * O[1] + 9 * O[2] + 38 * O[3] + 1) >> 1;
                    dst[7][2 * i + j] = (9 * O[0] - 25 * O[1] + 38 * O[2] - 44 * O[3] + 1) >> 1;
                }
            }
        }
    }
}

void transform_8x8_v_vpp(S23 src[8][8], S17 dst[8][8], U16 qp_table, U4 cutype[1], U1 NoResi_flag[1], U1 valid_flag[1], U6 cbp[1])
{
#pragma HLS ARRAY_PARTITION variable=dst dim=2
#pragma HLS ARRAY_PARTITION variable=src dim=2
#pragma HLS ARRAY_PARTITION variable=dst dim=1
#pragma HLS ARRAY_PARTITION variable=src dim=1

    cbp[0] = 0;
    int i, j, k;
    int E[4];
#pragma HLS ARRAY_PARTITION variable=E dim=1
    int O[4];
#pragma HLS ARRAY_PARTITION variable=O dim=1
    int EE[2];
#pragma HLS ARRAY_PARTITION variable=EE dim=1
    int EO[2];
#pragma HLS ARRAY_PARTITION variable=EO dim=1
    int Sign[8][8];
#pragma HLS ARRAY_PARTITION variable=Sign dim=2
#pragma HLS ARRAY_PARTITION variable=Sign dim=1
    int temp[8][8];
#pragma HLS ARRAY_PARTITION variable=temp dim=2
#pragma HLS ARRAY_PARTITION variable=temp dim=1
    int abs_val[8][8];
#pragma HLS ARRAY_PARTITION variable=abs_val dim=2
#pragma HLS ARRAY_PARTITION variable=abs_val dim=1
    int qp_const;
    int dct_dst[8][8];
#pragma HLS ARRAY_PARTITION variable=dct_dst dim=2
#pragma HLS ARRAY_PARTITION variable=dct_dst dim=1

    qp_const = cutype[0] == I16MB ? 169125 : 84562;

    if (valid_flag[0] == 1) {
        if (NoResi_flag[0] == 0) {
            for (i = 0; i < 4; i++) {
#pragma HLS PIPELINE
                for (j = 0; j < 2; j++) {
#pragma HLS UNROLL
                    /* E and O*/
                    for (k = 0; k < 4; k++) {
#pragma HLS UNROLL
                        E[k] = src[2 * i + j][k] + src[2 * i + j][7 - k];
                        O[k] = src[2 * i + j][k] - src[2 * i + j][7 - k];
                    }
                    /* EE and EO */
                    EE[0] = E[0] + E[3];
                    EO[0] = E[0] - E[3];
                    EE[1] = E[1] + E[2];
                    EO[1] = E[1] - E[2];

                    dct_dst[0][2 * i + j] = (32 * EE[0] + 32 * EE[1] + 128) >> 8;
                    Sign[0][2 * i + j] = dct_dst[0][2 * i + j] < 0 ? -1 : 1;
                    abs_val[0][2 * i + j] = Sign[0][2 * i + j] == 1 ? dct_dst[0][2 * i + j] : -dct_dst[0][2 * i + j];
                    temp[0][2 * i + j] = (abs_val[0][2 * i + j] * qp_table + qp_const) >> 19;
                    dst[0][2 * i + j] = Sign[0][2 * i + j] == 1 ? temp[0][2 * i + j] : -temp[0][2 * i + j];
                    dst[0][2 * i + j] = Clip3((S17)-32768, (S17)32767, dst[0][2 * i + j]);

                    dct_dst[4][2 * i + j] = (32 * EE[0] - 32 * EE[1] + 128) >> 8;
                    Sign[4][2 * i + j] = dct_dst[4][2 * i + j] < 0 ? -1 : 1;
                    abs_val[4][2 * i + j] = Sign[4][2 * i + j] == 1 ? dct_dst[4][2 * i + j] : -dct_dst[4][2 * i + j];
                    temp[4][2 * i + j] = (abs_val[4][2 * i + j] * qp_table + qp_const) >> 19;
                    dst[4][2 * i + j] = Sign[4][2 * i + j] == 1 ? temp[4][2 * i + j] : -temp[4][2 * i + j];
                    dst[4][2 * i + j] = Clip3((S17)-32768, (S17)32767, dst[4][2 * i + j]);

                    dct_dst[2][2 * i + j] = (42 * EO[0] + 17 * EO[1] + 128) >> 8;
                    Sign[2][2 * i + j] = dct_dst[2][2 * i + j] < 0 ? -1 : 1;
                    abs_val[2][2 * i + j] = Sign[2][2 * i + j] == 1 ? dct_dst[2][2 * i + j] : -dct_dst[2][2 * i + j];
                    temp[2][2 * i + j] = (abs_val[2][2 * i + j] * qp_table + qp_const) >> 19;
                    dst[2][2 * i + j] = Sign[2][2 * i + j] == 1 ? temp[2][2 * i + j] : -temp[2][2 * i + j];
                    dst[2][2 * i + j] = Clip3((S17)-32768, (S17)32767, dst[2][2 * i + j]);

                    dct_dst[6][2 * i + j] = (17 * EO[0] - 42 * EO[1] + 128) >> 8;
                    Sign[6][2 * i + j] = dct_dst[6][2 * i + j] < 0 ? -1 : 1;
                    abs_val[6][2 * i + j] = Sign[6][2 * i + j] == 1 ? dct_dst[6][2 * i + j] : -dct_dst[6][2 * i + j];
                    temp[6][2 * i + j] = (abs_val[6][2 * i + j] * qp_table + qp_const) >> 19;
                    dst[6][2 * i + j] = Sign[6][2 * i + j] == 1 ? temp[6][2 * i + j] : -temp[6][2 * i + j];
                    dst[6][2 * i + j] = Clip3((S17)-32768, (S17)32767, dst[6][2 * i + j]);

                    dct_dst[1][2 * i + j] = (44 * O[0] + 38 * O[1] + 25 * O[2] + 9 * O[3] + 128) >> 8;
                    Sign[1][2 * i + j] = dct_dst[1][2 * i + j] < 0 ? -1 : 1;
                    abs_val[1][2 * i + j] = Sign[1][2 * i + j] == 1 ? dct_dst[1][2 * i + j] : -dct_dst[1][2 * i + j];
                    temp[1][2 * i + j] = (abs_val[1][2 * i + j] * qp_table + qp_const) >> 19;
                    dst[1][2 * i + j] = Sign[1][2 * i + j] == 1 ? temp[1][2 * i + j] : -temp[1][2 * i + j];
                    dst[1][2 * i + j] = Clip3((S17)-32768, (S17)32767, dst[1][2 * i + j]);

                    dct_dst[3][2 * i + j] = (38 * O[0] - 9 * O[1] - 44 * O[2] - 25 * O[3] + 128) >> 8;
                    Sign[3][2 * i + j] = dct_dst[3][2 * i + j] < 0 ? -1 : 1;
                    abs_val[3][2 * i + j] = Sign[3][2 * i + j] == 1 ? dct_dst[3][2 * i + j] : -dct_dst[3][2 * i + j];
                    temp[3][2 * i + j] = (abs_val[3][2 * i + j] * qp_table + qp_const) >> 19;
                    dst[3][2 * i + j] = Sign[3][2 * i + j] == 1 ? temp[3][2 * i + j] : -temp[3][2 * i + j];
                    dst[3][2 * i + j] = Clip3((S17)-32768, (S17)32767, dst[3][2 * i + j]);

                    dct_dst[5][2 * i + j] = (25 * O[0] - 44 * O[1] + 9 * O[2] + 38 * O[3] + 128) >> 8;
                    Sign[5][2 * i + j] = dct_dst[5][2 * i + j] < 0 ? -1 : 1;
                    abs_val[5][2 * i + j] = Sign[5][2 * i + j] == 1 ? dct_dst[5][2 * i + j] : -dct_dst[5][2 * i + j];
                    temp[5][2 * i + j] = (abs_val[5][2 * i + j] * qp_table + qp_const) >> 19;
                    dst[5][2 * i + j] = Sign[5][2 * i + j] == 1 ? temp[5][2 * i + j] : -temp[5][2 * i + j];
                    dst[5][2 * i + j] = Clip3((S17)-32768, (S17)32767, dst[5][2 * i + j]);

                    dct_dst[7][2 * i + j] = (9 * O[0] - 25 * O[1] + 38 * O[2] - 44 * O[3] + 128) >> 8;
                    Sign[7][2 * i + j] = dct_dst[7][2 * i + j] < 0 ? -1 : 1;
                    abs_val[7][2 * i + j] = Sign[7][2 * i + j] == 1 ? dct_dst[7][2 * i + j] : -dct_dst[7][2 * i + j];
                    temp[7][2 * i + j] = (abs_val[7][2 * i + j] * qp_table + qp_const) >> 19;
                    dst[7][2 * i + j] = Sign[7][2 * i + j] == 1 ? temp[7][2 * i + j] : -temp[7][2 * i + j];
                    dst[7][2 * i + j] = Clip3((S17)-32768, (S17)32767, dst[7][2 * i + j]);


                    for (int m = 0; m < 8; m++) {
                        if (dst[m][2 * i + j] != 0) {
                            cbp[0] = 15;
                        }
                    }
                }
            }
        }
        else {
            for (j = 0; j < 4; j++) {
#pragma HLS UNROLL
                for (i = 0; i < 8; i++) {
#pragma HLS UNROLL
                    dst[2 * j][i] = 0;
                    dst[2 * j + 1][i] = 0;
                }
            }
        }
    }
}

U5 cutype_bin_8x8_vpp(U4 mode, U3 img_type, U6 cbp)
{
    const int MapPCUTypeMin[14] = { 1, 2, 3, 4, 3, 3, 4, 4, -1, 5, 5, 5, 5, 5 };
    int act_sym;
    U5 bin_count = 0;

    //CuType
    if (img_type != INTRA_IMG) {
        // act_sym calculation
        act_sym = MapPCUTypeMin[mode];
        if (mode == PSKIPDIRECT && cbp == 0) {
            act_sym = 0;
        }

        if (act_sym >= 1) {
            bin_count += act_sym;
        }
        if (act_sym < 5) {
            bin_count++;
        }
    }
    return bin_count;
}

U4 tr_pmode_bin_8x8_luma_vpp(U4 mode, U6 uiDirMode, S7 mostProbableMode[2])
{
    S7 intra_pred_mode;
    U4 bin_count = 0;

    if (mode == I16MB) {
        //Tr size
        bin_count++;

        //luma mode
        intra_pred_mode = (mostProbableMode[0] == (S7)uiDirMode) ? (S7)(-2) : ((mostProbableMode[1] == (S7)uiDirMode) ?
            (S7)(-1) : (uiDirMode < mostProbableMode[0] ? (S7)uiDirMode : ((S7)uiDirMode < mostProbableMode[1] ? (S7)(uiDirMode - 1) :
                (S7)(uiDirMode - 2))));

        if (intra_pred_mode >= 0) {
            bin_count += 6;
        }
        else {
            bin_count += 2;
        }
    }
    return bin_count;
}

U7 rdo_mvd_8x8_vpp(S13 curr_mvd)
{
    S13 act_sym;
    U7 bin_count = 0;

    if (curr_mvd > 0)
        act_sym = curr_mvd;
    else
        act_sym = -curr_mvd;

    if (act_sym < 3) {   // 0, 1, 2
        if (act_sym == 0) {
            bin_count++;
        }
        else if (act_sym == 1) {
            bin_count += 3;
        }
        else if (act_sym == 2) {
            bin_count += 4;
        }
    }
    else {
        if (act_sym % 2 == 1) {   //odds >3
            act_sym = (act_sym - 3) / 2;
        }
        else { //even >3
            act_sym = (act_sym - 4) / 2;
        }

        if (act_sym == 0) {
            bin_count += 6;
        }
        else if (act_sym > 0 && act_sym <= 2) {
            bin_count += 8;
        }
        else if (act_sym > 2 && act_sym <= 6) {
            bin_count += 10;
        }
        else if (act_sym > 6 && act_sym <= 14) {
            bin_count += 12;
        }
        else if (act_sym > 14 && act_sym <= 30) {
            bin_count += 14;
        }
        else if (act_sym > 30 && act_sym <= 62) {
            bin_count += 16;
        }
        else if (act_sym > 62 && act_sym <= 126) {
            bin_count += 18;
        }
        else if (act_sym > 126 && act_sym <= 254) {
            bin_count += 20;
        }
        else if (act_sym > 254 && act_sym <= 510) {
            bin_count += 22;
        }
        else if (act_sym > 510 && act_sym <= 1022) {
            bin_count += 24;
        }
        else if (act_sym > 1022 && act_sym <= 2046) {
            bin_count += 26;
        }
    }
    return bin_count;
}

U8 mvd_bin_8x8_vpp(U4 mode, S13 curr_mvd_x, S13 curr_mvd_y)
{
    U7 mvd_bin_x = 0;
    U7 mvd_bin_y = 0;

    if (mode == P2NX2N) {
        mvd_bin_x = rdo_mvd_8x8_vpp(curr_mvd_x);
        mvd_bin_y = rdo_mvd_8x8_vpp(curr_mvd_y);
    }
    return mvd_bin_x + mvd_bin_y;
}

U4 RDO_CBP_8x8_luma_vpp(U4 mode, U6 cbp, U3 img_type)
{
    U4 bin_count = 0;

    if (mode != I16MB) {
        if (!(mode == PSKIPDIRECT && img_type == P_IMG)) {
            bin_count++;
        }
        if (cbp != 0) {
            bin_count += 2;   //tr: +1 ; luma or chroma: +1
        }

    }
    else {
        bin_count++;
    }
    return bin_count;
}

#if MB_DQP
U8 RDO_Dqp_8x8_vpp(S2 delta_qp)
{
    int act_sym;
    int dquant;
    U8 bin_count = 0;
    dquant = delta_qp;

    if (dquant > 0) {
        act_sym = 2 * dquant - 1;
    }
    else {
        act_sym = -2 * dquant;
    }

    if (act_sym == 0) {
        bin_count++;
    }
    else {
        bin_count++;
        if (act_sym == 1) {
            bin_count++;
        }
        else {
            bin_count++;
            while (act_sym > 2) {
                bin_count++;
                act_sym--;
            }
            bin_count++;
        }
    }

    return bin_count;
}
#endif

U9 cbp_dqp_bin_8x8_luma_vpp(U1 useDQP, U4 mode, U6 cbp, U3 img_type, S2 delta_qp)
{
    U9 bincount = 0;

    if (mode || (mode == 0 && cbp != 0)) {

        bincount += RDO_CBP_8x8_luma_vpp(mode, cbp, img_type);

#if MB_DQP
        if (cbp != 0 && useDQP) {
            bincount += RDO_Dqp_8x8_vpp(delta_qp);
        }
#endif

    }
    return bincount;
}

U32 rate_luma_8x8_vpp(S17 quant_out[8][8], U3 img_type, S13 curr_mvd_x, S13 curr_mvd_y, U1 useDQP,
    U4 mode, U6 uiDirMode, S7 mostProbableMode[2], U6 cbp, S2 delta_qp)
{
    int pos_8x8[8][8] = { 1, 2, 6, 7, 1, 2, 6, 7,
        3, 5, 8, 13, 3, 5, 8, 13,
        4, 9, 12, 14, 4, 9, 12, 14,
        10, 11, 15, 16, 10, 11, 15, 16,
        1, 2, 6, 7, 1, 2, 6, 7,
        3, 5, 8, 13, 3, 5, 8, 13,
        4, 9, 12, 14, 4, 9, 12, 14,
        10, 11, 15, 16, 10, 11, 15, 16 };

    U5 cutype_bin = 0;
    U4 tr_pmode_bin = 0;
    U8 mvd_bin = 0;
    U9 cbp_dqp_bin = 0;
    U10 all_bin = 0;
    int rate_est = 0;
    int rate_coeff = 0;
    U22 coeff_sum = 0;

    cutype_bin = cutype_bin_8x8_vpp(mode, img_type, cbp);

    tr_pmode_bin = tr_pmode_bin_8x8_luma_vpp(mode, uiDirMode, mostProbableMode);

    mvd_bin = mvd_bin_8x8_vpp(mode, curr_mvd_x, curr_mvd_y);

    cbp_dqp_bin = cbp_dqp_bin_8x8_luma_vpp(useDQP, mode, cbp, img_type, delta_qp);

    int curr_val;
    int xx, yy;
    int abslevel;
    int coeff_pix;

    for (xx = 0; xx < 8; xx++) {
#pragma HLS UNROLL
        for (yy = 0; yy < 8; yy++) {
#pragma HLS UNROLL
            curr_val = quant_out[xx][yy];
            abslevel = (curr_val < 0) ? (-curr_val) : curr_val;
            coeff_pix = (curr_val != 0) ? (abslevel + pos_8x8[xx][yy]) : 0;
            coeff_sum += coeff_pix;
        }
    }

    all_bin = cutype_bin + mvd_bin + tr_pmode_bin + cbp_dqp_bin;

    rate_est = (9182 * all_bin + 4947) >> 13;
    if (coeff_sum == 0) {
        rate_coeff = 0;
    }
    else {
        rate_coeff = (4085 * coeff_sum + 32768) >> 13;
    }
    return rate_est + rate_coeff;
}
#endif

#endif
