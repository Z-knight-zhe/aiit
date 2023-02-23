/**
* ===========================================================================
* type_def.h
* ---------------------------------------------------------------------------
*
* StarAVS, the Chinese AVS2 video encoder library.
*
* ===========================================================================
*/

//#define     VIVADO_HLS

#ifdef 		VIVADO_HLS

#define _XKEYCHECK_H
#if 0
#include "/home/ubuntu/software/Vivado/2019.2/include/gmp.h"
#include <ap_int.h>
#else
#include "ap_int.h"
#endif

//unsigned types
typedef  ap_uint<1>						U1;
typedef  ap_uint<2>						U2;
typedef  ap_uint<3>						U3;
typedef  ap_uint<4>						U4;
typedef  ap_uint<5>						U5;
typedef  ap_uint<6>						U6;
typedef  ap_uint<7>						U7;
typedef  ap_uint<8>						U8;
typedef  ap_uint<9>						U9;
typedef  ap_uint<10>					U10;
typedef  ap_uint<11>					U11;
typedef  ap_uint<12>					U12;
typedef  ap_uint<13>					U13;
typedef  ap_uint<14>					U14;
typedef  ap_uint<15>					U15;
typedef  ap_uint<16>					U16;
typedef  ap_uint<17>				    U17;
typedef  ap_uint<18>					U18;
typedef  ap_uint<19>					U19;
typedef  ap_uint<20>					U20;
typedef  ap_uint<21>					U21;
typedef  ap_uint<22>					U22;
typedef  ap_uint<23>					U23;
typedef  ap_uint<24>					U24;
typedef  ap_uint<25>					U25;
typedef  ap_uint<26>					U26;
typedef  ap_uint<27>				    U27;
typedef  ap_uint<28>					U28;
typedef  ap_uint<29>					U29;
typedef  ap_uint<30>					U30;
typedef  ap_uint<31>					U31;
typedef  ap_uint<32>					U32;
typedef  ap_uint<33>					U33;
typedef  ap_uint<34>					U34;
typedef  ap_uint<35>					U35;
typedef  ap_uint<36>					U36;
typedef  ap_uint<37>				    U37;
typedef  ap_uint<38>					U38;
typedef  ap_uint<39>					U39;
typedef  ap_uint<40>					U40;
typedef  ap_uint<41>					U41;
typedef  ap_uint<42>					U42;
typedef  ap_uint<43>					U43;
typedef  ap_uint<44>					U44;
typedef  ap_uint<45>					U45;
typedef  ap_uint<46>					U46;
typedef  ap_uint<47>				    U47;
typedef  ap_uint<48>					U48;
typedef  ap_uint<49>					U49;
typedef  ap_uint<50>					U50;
typedef  ap_uint<51>					U51;
typedef  ap_uint<52>					U52;
typedef  ap_uint<53>					U53;
typedef  ap_uint<54>					U54;
typedef  ap_uint<55>					U55;
typedef  ap_uint<56>					U56;
typedef  ap_uint<57>				    U57;
typedef  ap_uint<58>					U58;
typedef  ap_uint<59>					U59;
typedef  ap_uint<60>					U60;
typedef  ap_uint<61>					U61;
typedef  ap_uint<62>					U62;
typedef  ap_uint<63>					U63;
typedef  ap_uint<64>					U64;

//signed types

typedef  ap_int<1>						S1;
typedef  ap_int<2>						S2;
typedef  ap_int<3>						S3;
typedef  ap_int<4>						S4;
typedef  ap_int<5>						S5;
typedef  ap_int<6>						S6;
typedef  ap_int<7>						S7;
typedef  ap_int<8>						S8;
typedef  ap_int<9>						S9;
typedef  ap_int<10>						S10;
typedef  ap_int<11>						S11;
typedef  ap_int<12>						S12;
typedef  ap_int<13>						S13;
typedef  ap_int<14>						S14;
typedef  ap_int<15>						S15;
typedef  ap_int<16>						S16;
typedef  ap_int<17> 			  	    S17;
typedef  ap_int<18>						S18;
typedef  ap_int<19>						S19;
typedef  ap_int<20>						S20;
typedef  ap_int<21>						S21;
typedef  ap_int<22>						S22;
typedef  ap_int<23>						S23;
typedef  ap_int<24>						S24;
typedef  ap_int<25>						S25;
typedef  ap_int<26>						S26;
typedef  ap_int<27>				  	    S27;
typedef  ap_int<28>						S28;
typedef  ap_int<29>						S29;
typedef  ap_int<30>						S30;
typedef  ap_int<31>						S31;
typedef  ap_int<32>						S32;
typedef  ap_int<33>						S33;
typedef  ap_int<34>						S34;
typedef  ap_int<35>						S35;
typedef  ap_int<36>						S36;
typedef  ap_int<37>				  	    S37;
typedef  ap_int<38>						S38;
typedef  ap_int<39>						S39;
typedef  ap_int<40>						S40;
typedef  ap_int<41>						S41;
typedef  ap_int<42>						S42;
typedef  ap_int<43>						S43;
typedef  ap_int<44>						S44;
typedef  ap_int<45>						S45;
typedef  ap_int<46>						S46;
typedef  ap_int<47>				  	    S47;
typedef  ap_int<48>						S48;
typedef  ap_int<49>						S49;
typedef  ap_int<50>						S50;
typedef  ap_int<51>						S51;
typedef  ap_int<52>						S52;
typedef  ap_int<53>						S53;
typedef  ap_int<54>						S54;
typedef  ap_int<55>						S55;
typedef  ap_int<56>						S56;
typedef  ap_int<57>				  	    S57;
typedef  ap_int<58>						S58;
typedef  ap_int<59>						S59;
typedef  ap_int<60>						S60;
typedef  ap_int<61>						S61;
typedef  ap_int<62>						S62;
typedef  ap_int<63>						S63;
typedef  signed long long						S64;

// #include <hls_stream.h>
// 
// using namespace hls;
// 
// #define  hls_stream stream


#else

//unsigned types
typedef  unsigned char				    U1;
typedef  unsigned char				    U2;
typedef  unsigned char				    U3;
typedef  unsigned char				    U4;
typedef  unsigned char				    U5;
typedef  unsigned char				    U6;
typedef  unsigned char				    U7;
typedef  unsigned char				    U8;
typedef  unsigned short				    U9;
typedef  unsigned short				    U10;
typedef  unsigned short				    U11;
typedef  unsigned short				    U12;
typedef  unsigned short				    U13;
typedef  unsigned short				    U14;
typedef  unsigned short				    U15;
typedef  unsigned short				    U16;
typedef  unsigned int			  	    U17;
typedef  unsigned int					U18;
typedef  unsigned int					U19;
typedef  unsigned int					U20;
typedef  unsigned int					U21;
typedef  unsigned int					U22;
typedef  unsigned int					U23;
typedef  unsigned int					U24;
typedef  unsigned int					U25;
typedef  unsigned int					U26;
typedef  unsigned int			  	    U27;
typedef  unsigned int					U28;
typedef  unsigned int					U29;
typedef  unsigned int					U30;
typedef  unsigned int					U31;
typedef  unsigned int					U32;
typedef  unsigned long long		        U33;
typedef  unsigned long long		        U34;
typedef  unsigned long long		        U35;
typedef  unsigned long long		        U36;
typedef  unsigned long long		        U37;
typedef  unsigned long long		        U38;
typedef  unsigned long long		        U39;
typedef  unsigned long long		        U40;
typedef  unsigned long long		        U41;
typedef  unsigned long long		        U42;
typedef  unsigned long long		        U43;
typedef  unsigned long long		        U44;
typedef  unsigned long long		        U45;
typedef  unsigned long long		        U46;
typedef  unsigned long long		        U47;
typedef  unsigned long long		        U48;
typedef  unsigned long long		        U49;
typedef  unsigned long long		        U50;
typedef  unsigned long long		        U51;
typedef  unsigned long long		        U52;
typedef  unsigned long long		        U53;
typedef  unsigned long long		        U54;
typedef  unsigned long long		        U55;
typedef  unsigned long long		        U56;
typedef  unsigned long long		        U57;
typedef  unsigned long long		        U58;
typedef  unsigned long long		        U59;
typedef  unsigned long long		        U60;
typedef  unsigned long long		        U61;
typedef  unsigned long long		        U62;
typedef  unsigned long long		        U63;
typedef  unsigned long long		        U64;

//signed types

typedef  signed char  				    S1;
typedef  signed char  				    S2;
typedef  signed char  				    S3;
typedef  signed char  				    S4;
typedef  signed char  				    S5;
typedef  signed char  				    S6;
typedef  signed char  				    S7;
typedef  signed char  				    S8;
typedef  signed short					S9;
typedef  signed short					S10;
typedef  signed short					S11;
typedef  signed short					S12;
typedef  signed short					S13;
typedef  signed short					S14;
typedef  signed short					S15;
typedef  signed short					S16;
typedef  signed int 			  	    S17;
typedef  signed int 					S18;
typedef  signed int 					S19;
typedef  signed int 					S20;
typedef  signed int 					S21;
typedef  signed int 					S22;
typedef  signed int 					S23;
typedef  signed int 					S24;
typedef  signed int 					S25;
typedef  signed int 					S26;
typedef  signed int 			  	    S27;
typedef  signed int 					S28;
typedef  signed int 					S29;
typedef  signed int 					S30;
typedef  signed int 					S31;
typedef  signed int 					S32;
typedef  signed long long               S33;
typedef  signed long long               S34;
typedef  signed long long               S35;
typedef  signed long long               S36;
typedef  signed long long               S37;
typedef  signed long long               S38;
typedef  signed long long               S39;
typedef  signed long long               S40;
typedef  signed long long               S41;
typedef  signed long long               S42;
typedef  signed long long               S43;
typedef  signed long long               S44;
typedef  signed long long               S45;
typedef  signed long long               S46;
typedef  signed long long               S47;
typedef  signed long long               S48;
typedef  signed long long               S49;
typedef  signed long long               S50;
typedef  signed long long               S51;
typedef  signed long long               S52;
typedef  signed long long               S53;
typedef  signed long long               S54;
typedef  signed long long               S55;
typedef  signed long long               S56;
typedef  signed long long               S57;
typedef  signed long long               S58;
typedef  signed long long               S59;
typedef  signed long long               S60;
typedef  signed long long               S61;
typedef  signed long long               S62;
typedef  signed long long               S63;
typedef  signed long long               S64;

#endif
