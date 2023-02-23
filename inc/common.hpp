//------------------------------------------------------------------------------
    //
    //  The confidential and proprietary information contained in this file may
    //  only be used by a person authorised under and to the extent permitted
    //  by a subsisting licensing agreement from XK Silicon.
    //
    //                   (C) COPYRIGHT 2020 XK Silicon.
    //                       ALL RIGHTS RESERVED
    //
    //  This entire notice must be reproduced on all copies of this file
    //  and copies of this file may only be made by a person if such person is
    //  permitted to do so under the terms of a subsisting license agreement
    //  from XK Silicon.
    //
    //  Revision       : 112933
    //  Release        : XKCDC
    //
//------------------------------------------------------------------------------
    //
    //  Filename       : common.hpp
    //  Author         : Huang Leilei
    //  Status         : phase 003
    //  Reset          : 2021-10-15
    //  Description    : common related headers
    //
//------------------------------------------------------------------------------
#pragma warning(disable:4005)

#ifndef __XKCDC_COMMON__
#define __XKCDC_COMMON__

//*** INCLUDE ******************************************************************
    #include <stdint.h>
    #include <stdio.h>
    #ifdef   NDEBUG
    #undef   NDEBUG
    #include <assert.h>
    #define  NDEBUG
    #else
    #include <assert.h>
    #endif
    #include <string>
    #include <cstring>
    #include <iostream>
    #include <fstream>
    #include <math.h>


//*** NAMESPACE ****************************************************************
    using std::string;
    using std::cout;
    using std::endl;
    using std::fstream;


//*** DEFINITION ***************************************************************
//--- SYS ------------------------------
    #define _CRT_SECURE_NO_WARNINGS


//--- TOP ------------------------------
    // KNOB
        //#define KNOB_TIME

    // NUMB
        #define NUMB_CHN            3
        #define NUMB_LAC            (NUMB_CHN == 1 ? 1 : 2)

    // SIZE
        #define SIZE_FRA_MAX_X      4096
        #define SIZE_FRA_MAX_Y      4096
        #define SIZE_S_W_MAX_X      128
        #define SIZE_S_W_MAX_Y      128
        #define SIZE_SCH_MAX_X      (SIZE_LCU + SIZE_S_W_MAX_X)
        #define SIZE_SCH_MAX_Y      (SIZE_LCU + SIZE_S_W_MAX_Y)
        #define SIZE_LCU            64

    // DATA
        #define DATA_PXL_WD     16
        #define DATA_PXL_MAX    ((1 << (DATA_PXL_WD)) - 1)
        #define DATA_SHF_CH     1
        #define DATA_SCL_CH     (1 << DATA_SHF_CH)    // this is an out-of-date definition

    // FUNC
        #define FUNC_LOG2(dat)          \
            ( (dat <= 1 <<  0 ) ?  0    \
            : (dat <= 1 <<  1 ) ?  1    \
            : (dat <= 1 <<  2 ) ?  2    \
            : (dat <= 1 <<  3 ) ?  3    \
            : (dat <= 1 <<  4 ) ?  4    \
            : (dat <= 1 <<  5 ) ?  5    \
            : (dat <= 1 <<  6 ) ?  6    \
            : (dat <= 1 <<  7 ) ?  7    \
            : (dat <= 1 <<  8 ) ?  8    \
            : (dat <= 1 <<  9 ) ?  9    \
            : (dat <= 1 << 10 ) ? 10    \
            : (dat <= 1 << 11 ) ? 11    \
            : (dat <= 1 << 12 ) ? 12    \
            : (dat <= 1 << 13 ) ? 13    \
            : (dat <= 1 << 14 ) ? 14    \
            : (dat <= 1 << 15 ) ? 15    \
            : (dat <= 1 << 16 ) ? 16    \
            : (dat <= 1 << 17 ) ? 17    \
            : (dat <= 1 << 18 ) ? 18    \
            : (dat <= 1 << 19 ) ? 19    \
            : (dat <= 1 << 20 ) ? 20    \
            : (dat <= 1 << 21 ) ? 21    \
            : (dat <= 1 << 22 ) ? 22    \
            : (dat <= 1 << 23 ) ? 23    \
            : (dat <= 1 << 24 ) ? 24    \
            : (dat <= 1 << 25 ) ? 25    \
            : (dat <= 1 << 26 ) ? 26    \
            : (dat <= 1 << 27 ) ? 27    \
            : (dat <= 1 << 28 ) ? 28    \
            : (dat <= 1 << 29 ) ? 29    \
            : (dat <= 1 << 30 ) ? 30    \
            : (dat <= 1 << 31 ) ? 31    \
            :                     32    \
            )
        #define FUNC_INI_FPT(flg, fpt, obj, idx, typ, pst)                                    \
            if (!flg) {                                                                       \
                flg = 1;                                                                      \
                string strFul = m_cfg->strPrefixDump                                          \
                              + string((enmTyp_t)m_cfg->enmTyp == enmTyp_t::I ? "I" : "P")    \
                              + "_"                                                           \
                              + m_name                                                        \
                              + "_"                                                           \
                              + obj                                                           \
                              + "_"                                                           \
                              + idx                                                           \
                              + "_"                                                           \
                              + typ                                                           \
                              + "."                                                           \
                              + pst                                                           \
                ;                                                                             \
                fpt = fopen(strFul.c_str(), "w");                                             \
            }
        #define FUNC_DMP_D_0(flg, fpt, obj, idx, typ, pst, dat, msk, fmt)    \
            FUNC_INI_FPT(flg, fpt, obj, idx, typ, pst)                       \
            string fmtFul = fmt + "\n";                                      \
            fprintf(fpt, fmtFul.c_str(), dat & msk);
        #define FUNC_DMP_D_1(flg, fpt, obj, idx, typ, pst, dat, msk, fmt, siz0, inc0)    \
            FUNC_INI_FPT(flg, fpt, obj, idx, typ, pst)                                   \
            string fmtFul = " " + fmt;                                                   \
            for (int i0 = 0; i0 < siz0; i0 += inc0) {                                    \
                if (i0 == 0)                                                             \
                    fprintf(fpt,    fmt.c_str(), dat[i0] & msk);                         \
                else                                                                     \
                    fprintf(fpt, fmtFul.c_str(), dat[i0] & msk);                         \
            }                                                                            \
            fprintf(fpt, "\n");
        #define FUNC_DMP_D_2(flg, fpt, obj, idx, typ, pst, dat, msk, fmt, siz1, inc1, siz0, inc0)    \
            FUNC_INI_FPT(flg, fpt, obj, idx, typ, pst)                                               \
            string fmtFul = " " + fmt;                                                               \
            for (int i1 = 0; i1 < siz1; i1+=inc1) {                                                  \
                for (int i0 = 0; i0 < siz0; i0+=inc0) {                                              \
                    if (i0 == 0)                                                                     \
                        fprintf(fpt,    fmt.c_str(), (int)dat[i1][i0] & msk);                        \
                    else                                                                             \
                        fprintf(fpt, fmtFul.c_str(), (int)dat[i1][i0] & msk);                        \
                }                                                                                    \
                fprintf(fpt, "\n");                                                                  \
            }
        // TODO: [HLL] remove it
        #define FUNC_DMP_D_2_RVS(flg, fpt, obj, idx, typ, pst, dat, msk, fmt, siz1, inc1, siz0, inc0)    \
            FUNC_INI_FPT(flg, fpt, obj, idx, typ, pst)                                                   \
            string fmtFul = " " + fmt;                                                                   \
            for (int i = 0; i < siz1/4; ++i)                                                             \
                for (int j = 0; j < siz0/32; ++j)                                                        \
                    for (int i1 = 0; i1 < 4; i1 += inc1) {                                               \
                        for (int i0 = 0; i0 < 32; i0 += inc0) {                                          \
                            pxl_t tmp = dat[i1+4*i][i0+32*j] & msk;                                      \
                            tmp = ((tmp & 0x80) >> 7)                                                    \
                                | ((tmp & 0x40) >> 5)                                                    \
                                | ((tmp & 0x20) >> 3)                                                    \
                                | ((tmp & 0x10) >> 1)                                                    \
                                | ((tmp & 0x08) << 1)                                                    \
                                | ((tmp & 0x04) << 3)                                                    \
                                | ((tmp & 0x02) << 5)                                                    \
                                | ((tmp & 0x01) << 7)                                                    \
                            ;                                                                            \
                            if (i0 == 0)                                                                 \
                                fprintf(fpt,    fmt.c_str(), tmp);                                       \
                            else                                                                         \
                                fprintf(fpt, fmtFul.c_str(), tmp);                                       \
                        }                                                                                \
                        fprintf(fpt, "\n");                                                              \
                    }
        #define FUNC_DMP_D_3(flg, fpt, obj, idx, typ, pst, dat, msk, fmt, siz2, inc2, siz1, inc1, siz0, inc0)    \
            FUNC_INI_FPT(flg, fpt, obj, idx, typ, pst)                                                           \
            string fmtFul = " " + fmt;                                                                           \
            for (int i2 = 0; i2 < siz2; i2 += inc2) {                                                            \
                for (int i1 = 0; i1 < siz1; i1 += inc1) {                                                        \
                    for (int i0 = 0; i0 < siz0; i0 += inc0) {                                                    \
                        if (i0 == 0)                                                                             \
                            fprintf(fpt,    fmt.c_str(), dat[i2][i1][i0] & msk);                                 \
                        else                                                                                     \
                            fprintf(fpt, fmtFul.c_str(), dat[i2][i1][i0] & msk);                                 \
                    }                                                                                            \
                    fprintf(fpt, "\n");                                                                          \
                }                                                                                                \
            }
        #define FUNC_DMP_D_4(flg, fpt, obj, idx, typ, pst, dat, msk, fmt, siz3, inc3, siz2, inc2, siz1, inc1, siz0, inc0)    \
            FUNC_INI_FPT(flg, fpt, obj, idx, typ, pst)                                                                       \
            string fmtFul = " " + fmt;                                                                                       \
            for (int i3 = 0; i3 < siz3; i3 += inc3) {                                                                        \
                for (int i2 = 0; i2 < siz2; i2 += inc2) {                                                                    \
                    for (int i1 = 0; i1 < siz1; i1 += inc1) {                                                                \
                        for (int i0 = 0; i0 < siz0; i0 += inc0) {                                                            \
                            if (i0 == 0)                                                                                     \
                                fprintf(fpt,    fmt.c_str(), dat[i3][i2][i1][i0] & msk);                                     \
                            else                                                                                             \
                                fprintf(fpt, fmtFul.c_str(), dat[i3][i2][i1][i0] & msk);                                     \
                        }                                                                                                    \
                        fprintf(fpt, "\n");                                                                                  \
                    }                                                                                                        \
                }                                                                                                            \
            }


        #if DATA_PXL_WD > 8
        typedef signed  short  pxl_t;
        #else
        typedef uint8_t     pxl_t;
        #endif



        typedef enum {
            NONE = 0,
            OUTPUT,
            INTERMEDIATE
        } enmLvlDmp_t;


        typedef enum {
            I = 0,
            P,
            B
        } enmTyp_t;



//*** STRUCT *******************************************************************
//--- SYS ------------------------------
    // cfg_t
        #include "cfg_type.hpp"



#endif /* __XKCDC_COMMON__ */
