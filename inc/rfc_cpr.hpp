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
    //  Filename       : rfc_cpr.hpp
    //  Author         : Li Tingting
    //  Status         : phase 003
    //  Reset          : 2021-10-15
    //  Description    : cpr related header
    //
//------------------------------------------------------------------------------
    //
    //  Modified       : 2021-11-22 by HLL
    //  Description    : tidied up
    //
//------------------------------------------------------------------------------

#ifndef __XKCDC_RFC_CPR_HPP__
#define __XKCDC_RFC_CPR_HPP__

//*** INCLUDE ******************************************************************
    #include "common.hpp"
    #include "rfc.hpp"


//*** DEFINE *******************************************************************
    #define FUNC_MAX2(a, b)    ((a) > (b) ? (a) : (b))
    #define FUNC_MIN2(a, b)    ((a) < (b) ? (a) : (b))


//*** CLASS ********************************************************************
class CPR
{
//--- PRIVATE VARIABLE -----------------
private:
    // connection
        const cfg_t *m_cfg;

    // pixel (pre, res)
        //               [position y       ][position x       ]
        pxl_t (*m_datPre)[RFC_SIZE_L0_MAX_Y][RFC_SIZE_L0_MAX_X];
        int   (*m_datRes)[RFC_SIZE_L0_MAX_Y][RFC_SIZE_L0_MAX_X];

    // buffer (L?)
        bitWithSiz_t m_bitWithSizL1;
        bitWithSiz_t m_bitWithSizL2Full;
        bitWithSiz_t m_bitWithSizL2Data;
        bitWithSiz_t m_bitWithSizL2Sign;

    // data (bit)
        //               [position y       ][position x       ]
        pxl_t (*m_datBit)[RFC_SIZE_L0_MAX_Y][RFC_SIZE_L0_MAX_X];

    // data (parameter)
        // !!! one L1, one parameter
        //               [L1 index y         ][L1 index x         ]
        prm_t (*m_datPrm)[RFC_NUMB_L1_IN_L0_Y][RFC_NUMB_L1_IN_L0_X];


//--- PRIVATE FUNCTION -----------------
private:
    // dump
        void dmpDatRfcCprOriLcu(pxl_t datOri[RFC_SIZE_L0_MAX_Y  ][RFC_SIZE_L0_MAX_X  ], int idxLaC, int sizX, int sizY); bool m_dmpDatRfcCprOriLcuFlg[2] = {0};  FILE *m_dmpDatRfcCprOriLcuFpt[2];
        void dmpDatRfcCprBitLcu(pxl_t datBit[RFC_SIZE_L0_MAX_Y  ][RFC_SIZE_L0_MAX_X  ], int idxLaC, int sizX, int sizY); bool m_dmpDatRfcCprBitLcuFlg[2] = {0};  FILE *m_dmpDatRfcCprBitLcuFpt[2];
        void dmpDatRfcCprPrmLcu(prm_t datPrm[RFC_NUMB_L1_IN_L0_Y][RFC_NUMB_L1_IN_L0_X], int idxLaC, int sizX, int sizY); bool m_dmpDatRfcCprPrmLcuFlg[2] = {0};  FILE *m_dmpDatRfcCprPrmLcuFpt[2];
        void dmpSizBit         (int   sizBit                                          , int idxLaC                    ); bool m_dmpSizBitFlg         [2] = {0};  FILE *m_dmpSizBitFpt         [2];


//--- PUBLIC FUNCTION ------------------
public:
    // basic
        CPR();
        ~CPR();
        void connect(const cfg_t &cfg);
        void proc(pxl_t datOri[RFC_SIZE_L0_MAX_Y][RFC_SIZE_L0_MAX_X], int idxLaC, int sizX, int sizY);

        void proc_rew(pxl_t datOri[RFC_SIZE_L0_MAX_Y][RFC_SIZE_L0_MAX_X], int sizX, int sizY, int *bin_size);

        void proc_72x64(pxl_t datOri[72][64], int idxLaC, int sizX, int sizY);
        void proc_36x32(pxl_t datOri[36][32], int idxLaC, int sizX, int sizY);
    // tool
        bool encodeWithL3(pxl_t datOri[RFC_SIZE_L0_MAX_Y][RFC_SIZE_L0_MAX_X], int idxLaC, int sizX, int sizY, bool flgFirst, pxl_t datFirst);

        bool encodeWithL3_72x64(pxl_t datOri[72][64], int idxLaC, int sizX, int sizY, bool flgFirst, pxl_t datFirst);
        bool encodeWithL3_36x32(pxl_t datOri[36][32], int idxLaC, int sizX, int sizY, bool flgFirst, pxl_t datFirst);
    // clrBit
        void inline clrBit()
        {
            m_bitWithSizL1.siz = 0;
            m_bitWithSizL1.dat.reset();
        }

    // cpyBit
        void inline cpyBit(bitWithSiz_t *dst)
        {
            *dst = m_bitWithSizL1;
        }

    // cpyDatPrm
        void inline cpyDatPrm(prm_t datPrm[SIZE_FRA_MAX_Y / RFC_SIZE_L1_Y][SIZE_FRA_MAX_X / RFC_SIZE_L1_X], int idxX, int idxY, int sizX, int sizY) {
            for (int j = 0; j < sizY / RFC_SIZE_L1_Y; ++j) {
                for (int i = 0; i < sizX / RFC_SIZE_L1_X; ++i)
                    datPrm[idxY / RFC_SIZE_L1_Y + j][idxX / RFC_SIZE_L1_X + i] = (*m_datPrm)[j][i];
            }
        }

    // cpyDatBit
        void inline cpyDatBit(pxl_t datBit[SIZE_FRA_MAX_Y][SIZE_FRA_MAX_X], int idxX, int idxY, int sizX, int sizY) {
            for (int j = 0; j < sizY; ++j) {
                for (int i = 0; i < sizX; ++i)
                    datBit[idxY + j][idxX + i] = (*m_datBit)[j][i];
            }
        }
};




#endif /* __XKCDC_RFC_CPR_HPP__ */
