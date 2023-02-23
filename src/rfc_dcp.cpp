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
    //  Filename       : rfc_dcp.cpp
    //  Author         : Li Tingting
    //  Status         : phase 003
    //  Reset          : 2021-10-15
    //  Description    : source file for dcp
    //
//------------------------------------------------------------------------------
    //
    //  Modified       : 2021-11-22 by HLL
    //  Description    : tidied up
    //
//------------------------------------------------------------------------------

//*** INCLUDE ******************************************************************
#include "rfc_dcp.hpp"


//*** FUNCTION *****************************************************************
// DCP
DCP::DCP()
{
    // pixel (ori, res)
    m_datOri = (pxl_t (*)[RFC_SIZE_L0_MAX_Y][RFC_SIZE_L0_MAX_X])
        new     pxl_t    [RFC_SIZE_L0_MAX_Y][RFC_SIZE_L0_MAX_X];
    m_datRes = (int   (*)[RFC_SIZE_L0_MAX_Y][RFC_SIZE_L0_MAX_X])
        new     int      [RFC_SIZE_L0_MAX_Y][RFC_SIZE_L0_MAX_X];

    m_sizRes = (int (*)[RFC_NUMB_L3_IN_L0_Y][RFC_NUMB_L3_IN_L0_X])
        new     int    [RFC_NUMB_L3_IN_L0_Y][RFC_NUMB_L3_IN_L0_X];
}

// ~DCP
DCP::~DCP()
{
    // delete
    delete [] m_datOri;
    delete [] m_datRes;

    delete [] m_sizRes;
}

// proc
void DCP::proc(
pxl_t datBit[RFC_SIZE_L0_MAX_Y  ][RFC_SIZE_L0_MAX_X  ],
prm_t datPrm[RFC_NUMB_L1_IN_L0_Y][RFC_NUMB_L1_IN_L0_X],
int idxLaC, int sizX, int sizY)
{
    // variables
    int datShf = idxLaC == 0 ? 0 : DATA_SHF_CH;
    // NOTE: this force should be removed later
    datShf = 0;

    // decompress L0
    for (int posL1Y = 0; posL1Y < sizY; posL1Y += RFC_SIZE_L1_Y) {
        for (int posL1X = 0; posL1X < sizX; posL1X += RFC_SIZE_L1_X) {
            // init
            m_bitWithSizL1.siz = 0;

            // extract data
            pxl_t *pBitInPxlTyp = (pxl_t *) &(m_bitWithSizL1.dat);
            for (int idxPxl = 0; idxPxl < RFC_SIZE_L1_Y; ++idxPxl)
                memcpy(&pBitInPxlTyp[idxPxl * RFC_SIZE_L1_X], &datBit[posL1Y + idxPxl][posL1X], RFC_SIZE_L1_X * sizeof(pxl_t));

            // decompress L1
            prm_t datPrmL2 = datPrm[posL1Y / RFC_SIZE_L1_Y][posL1X / RFC_SIZE_L1_X];

            prm_t flgUseOriL2 = datPrmL2 >> FUNC_LOG2(RFC_SIZE_L1_X * RFC_SIZE_L1_Y * sizeof(pxl_t) * 8 / RFC_DATA_BUS_WD);
            flgUseOriL2 &= 0xff;
            for (int posL2Y = 0; posL2Y < RFC_SIZE_L1_Y; posL2Y += RFC_SIZE_L2_Y) {
                for (int posL2X = 0; posL2X < RFC_SIZE_L1_X; posL2X += RFC_SIZE_L2_X) {
                    // get pos
                    int posY = posL1Y + posL2Y;
                    int posX = posL1X + posL2X;

                    // get flgL2UseOri
                    bool flgUseOri = (flgUseOriL2 >> 7) & 1;
                    flgUseOriL2 <<= 1;

                    // decompress L2
                    bool  flgPreFirst = posL2X % (RFC_SIZE_L1_X >> datShf) == 0 && posL2Y == 0 ? 0
                        :                                                                        1
                    ;
                    pxl_t datPreFirst = posL2X % (RFC_SIZE_L1_X >> datShf) == 0 && posL2Y == 0 ? 0
                                      : posL2X % (RFC_SIZE_L1_X >> datShf) == 0                ? (*m_datOri)[posY - 1][posX]
                                      :                                                          (*m_datOri)[posY][posX - 1]
                    ;
                    decodeWithL3((pxl_t (*) [RFC_SIZE_L0_MAX_X]) &(*m_datOri)[posY][posX]
                        ,        RFC_SIZE_L2_X, RFC_SIZE_L2_Y
                        ,        flgUseOri
                        ,        flgPreFirst, datPreFirst
                    );
                }
            }
        }
    }
}

// decode
void DCP::decodeWithL3(pxl_t datOri[RFC_SIZE_L0_MAX_Y][RFC_SIZE_L0_MAX_X],
int sizX, int sizY, bool flgUseOri, bool flgPreFirst, pxl_t datPreFirst)
{
    // use ori
    if (flgUseOri) {
        for (int j = 0; j < sizY; ++j) {
            for (int i = 0; i < sizX; ++i)
                datOri[j][i] = popData(m_bitWithSizL1, DATA_PXL_WD);
        }
    }

    // decompress bit
    else {
        // decode first pixel (if flgPreFirst is false)
        if (!flgPreFirst)
            datOri[0][0] = popData(m_bitWithSizL1, DATA_PXL_WD);

        // decode residual size
        int sizResL2 = 0;
        for (int j = 0; j < sizY / RFC_SIZE_L3_Y; ++j) {
            for (int i = 0; i < sizX / RFC_SIZE_L3_X; ++i) {
                (*m_sizRes)[j][i] = popData(m_bitWithSizL1, RFC_DATA_MOD_WD);
                int sizRes = (*m_sizRes)[j][i] == RFC_SIZE_RES_MAX ? DATA_PXL_WD : (*m_sizRes)[j][i];
                sizResL2 += RFC_SIZE_L3_X * RFC_SIZE_L3_Y * sizRes;
            }
        }

        // decode residual data and sign
        int offset = m_bitWithSizL1.siz + sizResL2;
        int cntT = 0;
        for (int j = 0; j < sizY / RFC_SIZE_L3_Y; ++j) {
            for (int i = 0; i < sizX / RFC_SIZE_L3_X; ++i) {
                // data
                int datRes[RFC_SIZE_L3_Y][RFC_SIZE_L3_X];
                int sizRes = (*m_sizRes)[j][i] == RFC_SIZE_RES_MAX ? DATA_PXL_WD : (*m_sizRes)[j][i];
                int flgSign = 0;
                for (int m = 0; m < RFC_SIZE_L3_Y; ++m) {
                    for (int n = 0; n < RFC_SIZE_L3_X; ++n) {
                        datRes[m][n] = popData(m_bitWithSizL1, sizRes);
                        if (datRes[m][n] == 1 << ((*m_sizRes)[j][i] - 1))
                            flgSign = 1;
                    }
                }

                // sign
                int datSign = 0;
                if ((*m_sizRes)[j][i] && (*m_sizRes)[j][i] != RFC_SIZE_RES_MAX && flgSign)
                    datSign = popSign(m_bitWithSizL1, offset + (cntT++));
                for (int m = 0; m < RFC_SIZE_L3_Y; ++m) {
                    for (int n = 0; n < RFC_SIZE_L3_X; ++n)
                        (*m_datRes)[j * RFC_SIZE_L3_Y + m][i * RFC_SIZE_L3_X + n] = getRes(datRes[m][n], (*m_sizRes)[j][i], datSign);
                }
            }
        }
        m_bitWithSizL1.siz += cntT;

        // decode first pixel (if flgPreFirst is true)
        if (flgPreFirst)
            datOri[0][0] = (*m_sizRes)[0][0] == RFC_SIZE_RES_MAX ? (*m_datRes)[0][0] : (*m_datRes)[0][0] + datPreFirst ;

        // do DPCM (differential pulse-code modulation)
        for (int j = 1; j < sizY; ++j) {
            if ((*m_sizRes)[j / RFC_SIZE_L3_Y][0] == RFC_SIZE_RES_MAX)
                datOri[j][0] = (*m_datRes)[j][0];
            else
                datOri[j][0] = (*m_datRes)[j][0] + datOri[j - 1][0];
        }
        for (int j = 0; j < sizY; ++j) {
            for (int i = 1; i < sizX; ++i) {
                if ((*m_sizRes)[j / RFC_SIZE_L3_Y][i / RFC_SIZE_L3_X] == RFC_SIZE_RES_MAX)
                    datOri[j][i] = (*m_datRes)[j][i];
                else
                    datOri[j][i] = (*m_datRes)[j][i] + datOri[j][i - 1];
            }
        }
    }
}
