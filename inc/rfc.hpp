
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
    //  Filename       : rfc.hpp
    //  Author         : Li Tingting
    //  Status         : phase 003
    //  Reset          : 2021-10-15
    //  Description    : rfc related header
    //
//------------------------------------------------------------------------------
    //
    //  Modified       : 2021-11-22 by HLL
    //  Description    : tidied up
    //
//------------------------------------------------------------------------------

#ifndef __XKCDC_RFC_HPP__
#define __XKCDC_RFC_HPP__

//*** INCLUDE ******************************************************************
    #include <bitset>
    #include "common.hpp"


//*** NAMING SPACE *************************************************************
    using std::bitset;


//*** DEFINE *******************************************************************
    
    #define RFC_SIZE_L0_MAX_X        SIZE_SCH_MAX_X
    #define RFC_SIZE_L0_MAX_Y        SIZE_SCH_MAX_Y
    #define RFC_SIZE_L1_X            32
    #define RFC_SIZE_L1_Y            4
    #define RFC_SIZE_L2_X            8
    #define RFC_SIZE_L2_Y            2
    #define RFC_SIZE_L3_X            2
    #define RFC_SIZE_L3_Y            2
    #define RFC_SIZE_RES_MAX         7

    #define RFC_NUMB_BIT_MAX         (RFC_SIZE_L1_X * RFC_SIZE_L1_Y * DATA_PXL_WD)
    #define RFC_NUMB_L1_IN_L0_X      (RFC_SIZE_L0_MAX_X / RFC_SIZE_L1_X)
    #define RFC_NUMB_L1_IN_L0_Y      (RFC_SIZE_L0_MAX_Y / RFC_SIZE_L1_Y)
    #define RFC_NUMB_L2_IN_L1        ((RFC_SIZE_L1_X * RFC_SIZE_L1_Y) / (RFC_SIZE_L2_X * RFC_SIZE_L2_Y))
    #define RFC_NUMB_L3_IN_L0_X      (RFC_SIZE_L0_MAX_X / RFC_SIZE_L3_X)
    #define RFC_NUMB_L3_IN_L0_Y      (RFC_SIZE_L0_MAX_Y / RFC_SIZE_L3_Y)


    #define RFC_DATA_MOD_WD          3
    #define RFC_DATA_BUS_WD          128


//*** TYPEDEF ******************************************************************
    // prm_t
        typedef uint16_t prm_t;


//*** STRUCT *******************************************************************
    // bitWithSiz_t
        struct bitWithSiz_t {
            // variable
            bitset<RFC_NUMB_BIT_MAX> dat;
            int                      siz;

            // operation
            bitWithSiz_t &operator=(const bitWithSiz_t &arg)
            {
                dat = arg.dat;
                siz = arg.siz;
                return *this;
            }
        };


//*** INLINE *******************************************************************
    // pushDat
        inline void pushDat(bitWithSiz_t &dst, pxl_t dat, int siz)
        {
            bitset<DATA_PXL_WD> bitTmp = dat;
            for (int i = 0; i < siz; ++i)
                dst.dat[dst.siz + i] = bitTmp[siz - 1 - i];
            dst.siz += siz;
        }

    // pushBit
        inline void pushBit(bitWithSiz_t &dst, bitWithSiz_t &src)
        {
            for (int i = 0; i < src.siz; ++i)
                dst.dat[dst.siz + i] = src.dat[i];
            dst.siz += src.siz;
        }

    // pushRes: in an semi-fixed length coding way
        inline void pushRes(bitWithSiz_t &dst, int sizRes, int datRes, pxl_t datOri)
        {
            if (sizRes == RFC_SIZE_RES_MAX)
                pushDat(dst, datOri, DATA_PXL_WD);
            else if (datRes >= 0)
                pushDat(dst, datRes, sizRes);
            else
                pushDat(dst, datRes + (1 << sizRes), sizRes);
        }

    // popData
        inline pxl_t popData(bitWithSiz_t &src, int siz)
        {
            bitset<DATA_PXL_WD> bitTmp;
            for (int i = 0; i < siz; ++i)
                bitTmp[siz - 1 - i] = src.dat[src.siz + i];
            src.siz += siz;
            return (pxl_t) bitTmp.to_ulong();
        }

    // popSign
        inline pxl_t popSign(bitWithSiz_t &src, int bias)
        {
            bitset<1> bitTmp;
            bitTmp[0] = src.dat[bias];
            return bitTmp[0] == 0 ? 0 : 1;
        }

    // getRes
        inline int getRes(pxl_t dat, int siz, int sgn)
        {
            if (siz == 0)
                return 0;
            if (siz == 7)
                return dat;
            if (dat == (pxl_t)(1 << (siz - 1)))
                return sgn ? - dat : dat;
            if (dat < (pxl_t)(1 << (siz-1)))
                return dat;
            return (dat - (1 << siz));
        }

#endif // __XKCDC_RFC_HPP__
