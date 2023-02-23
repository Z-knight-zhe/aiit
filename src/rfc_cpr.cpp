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
    //  Filename       : rfc_cpr.cpp
    //  Author         : Li Tingting
    //  Status         : phase 003
    //  Reset          : 2021-10-15
    //  Description    : source file for cpr
    //
//------------------------------------------------------------------------------
    //
    //  Modified       : 2021-11-22 by HLL
    //  Description    : tidied up
    //
//------------------------------------------------------------------------------

//*** INCLUDE ******************************************************************
#include <iostream>
#include "rfc_cpr.hpp"


//*** NAMING SPACE *************************************************************
using namespace std;


//*** FUNCTION *****************************************************************
// CPR
CPR::CPR()
{
    // pixel (pre, res, bit, prm)
    m_datPre = (pxl_t (*)[RFC_SIZE_L0_MAX_Y  ][RFC_SIZE_L0_MAX_X  ])
        new     pxl_t    [RFC_SIZE_L0_MAX_Y  ][RFC_SIZE_L0_MAX_X  ];
    m_datRes = (int   (*)[RFC_SIZE_L0_MAX_Y  ][RFC_SIZE_L0_MAX_X  ])
        new     int      [RFC_SIZE_L0_MAX_Y  ][RFC_SIZE_L0_MAX_X  ];
    m_datBit = (pxl_t (*)[RFC_SIZE_L0_MAX_Y  ][RFC_SIZE_L0_MAX_X  ])
        new     pxl_t    [RFC_SIZE_L0_MAX_Y  ][RFC_SIZE_L0_MAX_X  ];
    m_datPrm = (prm_t (*)[RFC_NUMB_L1_IN_L0_Y][RFC_NUMB_L1_IN_L0_X])
        new     prm_t    [RFC_NUMB_L1_IN_L0_Y][RFC_NUMB_L1_IN_L0_X];
}

// ~CPR
CPR::~CPR()
{
    // delete
    delete [] m_datPre;
    delete [] m_datRes;
    delete [] m_datBit;
}

// connect
void CPR::connect(
const cfg_t &cfg)
{
    m_cfg = &cfg;
}


// proc
void CPR::proc(
pxl_t datOri[RFC_SIZE_L0_MAX_Y][RFC_SIZE_L0_MAX_X],
int idxLaC, int sizX, int sizY)
{
    // variables
    int datShf = idxLaC == 0 ? 0 : DATA_SHF_CH;
    // NOTE: this force should be removed later
    datShf = 0;

    // compress L0
    for (int posL1Y = 0; posL1Y < sizY; posL1Y += RFC_SIZE_L1_Y) {
        for (int posL1X = 0; posL1X < sizX; posL1X += RFC_SIZE_L1_X) {
            // init
            m_bitWithSizL1.siz = 0;
            m_bitWithSizL1.dat.reset();

            // compress L1
            prm_t datPrm = 0;
            for (int posL2Y = 0; posL2Y < RFC_SIZE_L1_Y; posL2Y += RFC_SIZE_L2_Y) {
                for (int posL2X = 0; posL2X < RFC_SIZE_L1_X; posL2X += RFC_SIZE_L2_X) {
                    // get pos
                    int posY = posL1Y + posL2Y;
                    int posX = posL1X + posL2X;

                    // compress L2
                    bool  flgPreFirst = posL2X % (RFC_SIZE_L1_X >> datShf) == 0 && posL2Y == 0 ? 0
                        :                                                                        1
                    ;
                    pxl_t datPreFirst = posL2X % (RFC_SIZE_L1_X >> datShf) == 0 && posL2Y == 0 ? 0
                                      : posL2X % (RFC_SIZE_L1_X >> datShf) == 0                ? datOri[posY - 1][posX]
                                      :                                                          datOri[posY][posX - 1]
                    ;
                    bool flgUseOri = encodeWithL3((pxl_t (*)[RFC_SIZE_L0_MAX_X]) &datOri[posY][posX]
                        ,                         idxLaC, RFC_SIZE_L2_X, RFC_SIZE_L2_Y
                        ,                         flgPreFirst, datPreFirst
                    );

                    // update parameter (flgUseOri)
                    datPrm <<= 1;
                    datPrm |= (uint16_t)flgUseOri;
                }
            }

            // update parameter (siz)
            int siz = (m_bitWithSizL1.siz + RFC_DATA_BUS_WD - 1) / RFC_DATA_BUS_WD - 1;
            datPrm <<= FUNC_LOG2(RFC_SIZE_L1_X * RFC_SIZE_L1_Y * sizeof(pxl_t) * 8 / RFC_DATA_BUS_WD);
            datPrm |= siz;

            // save parameter
            (*m_datPrm)[posL1Y / RFC_SIZE_L1_Y][posL1X / RFC_SIZE_L1_X] = datPrm;

            // save data
            pxl_t *pBitInPxlTyp = (pxl_t *) &(m_bitWithSizL1.dat);
            for (int idxPxl = 0; idxPxl < RFC_SIZE_L1_Y; ++idxPxl)
                memcpy(&(*m_datBit)[posL1Y + idxPxl][posL1X], &pBitInPxlTyp[idxPxl * RFC_SIZE_L1_X], RFC_SIZE_L1_X * sizeof(pxl_t));
        }
    }

    // dump
   /* if ((enmLvlDmp_t)m_cfg->dmpEnmLvlItf >= enmLvlDmp_t::OUTPUT) {
        dmpDatRfcCprOriLcu(   datOri, idxLaC, sizX, sizY);
        dmpDatRfcCprBitLcu(*m_datBit, idxLaC, sizX, sizY);
        dmpDatRfcCprPrmLcu(*m_datPrm, idxLaC, sizX, sizY);
    }*/
}

void CPR::proc_rew(
    pxl_t datOri[RFC_SIZE_L0_MAX_Y][RFC_SIZE_L0_MAX_X],
    int sizX, int sizY, int *bin_size)
{
    // variables
    // NOTE: this force should be removed later
    

    // compress L0
    for (int posL1Y = 0; posL1Y < sizY; posL1Y += RFC_SIZE_L1_Y) {
        for (int posL1X = 0; posL1X < sizX; posL1X += RFC_SIZE_L1_X) {
            // init
            m_bitWithSizL1.siz = 0;
            m_bitWithSizL1.dat.reset();

            // compress L1
            prm_t datPrm = 0;
            for (int posL2Y = 0; posL2Y < RFC_SIZE_L1_Y; posL2Y += RFC_SIZE_L2_Y) {
                for (int posL2X = 0; posL2X < RFC_SIZE_L1_X; posL2X += RFC_SIZE_L2_X) {
                    // get pos
                    int posY = posL1Y + posL2Y;
                    int posX = posL1X + posL2X;

                    // compress L2
                    bool  flgPreFirst = posL2X % (RFC_SIZE_L1_X ) == 0 && posL2Y == 0 ? 0
                        : 1
                        ;
                    pxl_t datPreFirst = posL2X % (RFC_SIZE_L1_X ) == 0 && posL2Y == 0 ? 0
                        : posL2X % (RFC_SIZE_L1_X ) == 0 ? datOri[posY - 1][posX]
                        : datOri[posY][posX - 1]
                        ;
                    bool flgUseOri = encodeWithL3((pxl_t(*)[RFC_SIZE_L0_MAX_X]) & datOri[posY][posX]
                        , 0, RFC_SIZE_L2_X, RFC_SIZE_L2_Y
                        , flgPreFirst, datPreFirst
                    );

                    // update parameter (flgUseOri)
                    datPrm <<= 1;
                    datPrm |= (uint16_t)flgUseOri;
                }
            }

            // update parameter (siz)
            int siz = (m_bitWithSizL1.siz + RFC_DATA_BUS_WD - 1) / RFC_DATA_BUS_WD - 1;
            datPrm <<= FUNC_LOG2(RFC_SIZE_L1_X * RFC_SIZE_L1_Y * sizeof(pxl_t) * 8 / RFC_DATA_BUS_WD);
            datPrm |= siz;
            *bin_size = siz + 1;
            // save parameter
            (*m_datPrm)[posL1Y / RFC_SIZE_L1_Y][posL1X / RFC_SIZE_L1_X] = datPrm;

            // save data
            pxl_t* pBitInPxlTyp = (pxl_t*)&(m_bitWithSizL1.dat);
            for (int idxPxl = 0; idxPxl < RFC_SIZE_L1_Y; ++idxPxl)
                memcpy(&(*m_datBit)[posL1Y + idxPxl][posL1X], &pBitInPxlTyp[idxPxl * RFC_SIZE_L1_X], RFC_SIZE_L1_X * sizeof(pxl_t));
        }
    }

    // dump
   /* if ((enmLvlDmp_t)m_cfg->dmpEnmLvlItf >= enmLvlDmp_t::OUTPUT) {
        dmpDatRfcCprOriLcu(   datOri, idxLaC, sizX, sizY);
        dmpDatRfcCprBitLcu(*m_datBit, idxLaC, sizX, sizY);
        dmpDatRfcCprPrmLcu(*m_datPrm, idxLaC, sizX, sizY);
    }*/
}

void CPR::proc_72x64(
    pxl_t datOri[72][64],
    int idxLaC, int sizX, int sizY)
{
    // variables
    int datShf = idxLaC == 0 ? 0 : DATA_SHF_CH;
    // NOTE: this force should be removed later
    datShf = 0;

    // compress L0
    for (int posL1Y = 0; posL1Y < sizY; posL1Y += RFC_SIZE_L1_Y) {
        for (int posL1X = 0; posL1X < sizX; posL1X += RFC_SIZE_L1_X) {
            // init
            m_bitWithSizL1.siz = 0;
            m_bitWithSizL1.dat.reset();

            // compress L1
            prm_t datPrm = 0;
            for (int posL2Y = 0; posL2Y < RFC_SIZE_L1_Y; posL2Y += RFC_SIZE_L2_Y) {
                for (int posL2X = 0; posL2X < RFC_SIZE_L1_X; posL2X += RFC_SIZE_L2_X) {
                    // get pos
                    int posY = posL1Y + posL2Y;
                    int posX = posL1X + posL2X;

                    // compress L2
                    bool  flgPreFirst = posL2X % (RFC_SIZE_L1_X >> datShf) == 0 && posL2Y == 0 ? 0
                        : 1
                        ;
                    pxl_t datPreFirst = posL2X % (RFC_SIZE_L1_X >> datShf) == 0 && posL2Y == 0 ? 0
                        : posL2X % (RFC_SIZE_L1_X >> datShf) == 0 ? datOri[posY - 1][posX]
                        : datOri[posY][posX - 1]
                        ;
                    bool flgUseOri = encodeWithL3_72x64((pxl_t(*)[64]) & datOri[posY][posX]
                        , idxLaC, RFC_SIZE_L2_X, RFC_SIZE_L2_Y
                        , flgPreFirst, datPreFirst
                    );

                    // update parameter (flgUseOri)
                    datPrm <<= 1;
                    datPrm |= (uint16_t)flgUseOri;
                }
            }

            // update parameter (siz)
            int siz = (m_bitWithSizL1.siz + RFC_DATA_BUS_WD - 1) / RFC_DATA_BUS_WD - 1;
            datPrm <<= FUNC_LOG2(RFC_SIZE_L1_X * RFC_SIZE_L1_Y * sizeof(pxl_t) * 8 / RFC_DATA_BUS_WD);
            datPrm |= siz;

            // save parameter
            (*m_datPrm)[posL1Y / RFC_SIZE_L1_Y][posL1X / RFC_SIZE_L1_X] = datPrm;

            // save data
            pxl_t* pBitInPxlTyp = (pxl_t*)&(m_bitWithSizL1.dat);
            for (int idxPxl = 0; idxPxl < RFC_SIZE_L1_Y; ++idxPxl)
                memcpy(&(*m_datBit)[posL1Y + idxPxl][posL1X], &pBitInPxlTyp[idxPxl * RFC_SIZE_L1_X], RFC_SIZE_L1_X * sizeof(pxl_t));
        }
    }
}

void CPR::proc_36x32(
    pxl_t datOri[36][32],
    int idxLaC, int sizX, int sizY)
{
    // variables
    int datShf = idxLaC == 0 ? 0 : DATA_SHF_CH;
    // NOTE: this force should be removed later
    datShf = 0;

    // compress L0
    for (int posL1Y = 0; posL1Y < sizY; posL1Y += RFC_SIZE_L1_Y) {
        for (int posL1X = 0; posL1X < sizX; posL1X += RFC_SIZE_L1_X) {
            // init
            m_bitWithSizL1.siz = 0;
            m_bitWithSizL1.dat.reset();

            // compress L1
            prm_t datPrm = 0;
            for (int posL2Y = 0; posL2Y < RFC_SIZE_L1_Y; posL2Y += RFC_SIZE_L2_Y) {
                for (int posL2X = 0; posL2X < RFC_SIZE_L1_X; posL2X += RFC_SIZE_L2_X) {
                    // get pos
                    int posY = posL1Y + posL2Y;
                    int posX = posL1X + posL2X;

                    // compress L2
                    bool  flgPreFirst = posL2X % (RFC_SIZE_L1_X >> datShf) == 0 && posL2Y == 0 ? 0
                        : 1
                        ;
                    pxl_t datPreFirst = posL2X % (RFC_SIZE_L1_X >> datShf) == 0 && posL2Y == 0 ? 0
                        : posL2X % (RFC_SIZE_L1_X >> datShf) == 0 ? datOri[posY - 1][posX]
                        : datOri[posY][posX - 1]
                        ;
                    bool flgUseOri = encodeWithL3_36x32((pxl_t(*)[32]) & datOri[posY][posX]
                        , idxLaC, RFC_SIZE_L2_X, RFC_SIZE_L2_Y
                        , flgPreFirst, datPreFirst
                    );

                    // update parameter (flgUseOri)
                    datPrm <<= 1;
                    datPrm |= (uint16_t)flgUseOri;
                }
            }

            // update parameter (siz)
            int siz = (m_bitWithSizL1.siz + RFC_DATA_BUS_WD - 1) / RFC_DATA_BUS_WD - 1;
            datPrm <<= FUNC_LOG2(RFC_SIZE_L1_X * RFC_SIZE_L1_Y * sizeof(pxl_t) * 8 / RFC_DATA_BUS_WD);
            datPrm |= siz;

            // save parameter
            (*m_datPrm)[posL1Y / RFC_SIZE_L1_Y][posL1X / RFC_SIZE_L1_X] = datPrm;

            // save data
            pxl_t* pBitInPxlTyp = (pxl_t*)&(m_bitWithSizL1.dat);
            for (int idxPxl = 0; idxPxl < RFC_SIZE_L1_Y; ++idxPxl)
                memcpy(&(*m_datBit)[posL1Y + idxPxl][posL1X], &pBitInPxlTyp[idxPxl * RFC_SIZE_L1_X], RFC_SIZE_L1_X * sizeof(pxl_t));
        }
    }
}

// encode
bool CPR::encodeWithL3(
pxl_t datOri[RFC_SIZE_L0_MAX_Y][RFC_SIZE_L0_MAX_X],
int idxLaC, int sizX, int sizY, bool flgPreFirst, pxl_t datPreFirst)
{
    // variables
    m_bitWithSizL2Full.siz = 0;
    m_bitWithSizL2Data.siz = 0;
    m_bitWithSizL2Sign.siz = 0;

    // encode first pixel
    if (flgPreFirst) {
        (*m_datPre)[0][0] = datPreFirst;
        (*m_datRes)[0][0] = datOri[0][0] - (*m_datPre)[0][0];
    }
    else {
        pushDat(m_bitWithSizL2Full, datOri[0][0], DATA_PXL_WD);
        (*m_datRes)[0][0] = 0;
    }

    // do DPCM (differential pulse-code modulation)
    for (int j = 1; j < sizY; ++j) {
        (*m_datPre)[j][0] = datOri[j - 1][0];
        (*m_datRes)[j][0] = datOri[j    ][0] - (*m_datPre)[j][0];
    }
    for (int j = 0; j < sizY; ++j) {
        for (int i = 1; i < sizX; ++i) {
            (*m_datPre)[j][i] = datOri[j][i - 1];
            (*m_datRes)[j][i] = datOri[j][i    ] - (*m_datPre)[j][i];
        }
    }

    // encode residual size, data and sign
    for (int idxL3Y = 0; idxL3Y < sizY / RFC_SIZE_L3_Y; ++idxL3Y) {
        for (int idxL3X = 0; idxL3X < sizX / RFC_SIZE_L3_X; ++idxL3X) {
            // size
            int datResMax = (*m_datRes)[idxL3Y * RFC_SIZE_L3_Y][idxL3X * RFC_SIZE_L3_X];
            int datResMin = (*m_datRes)[idxL3Y * RFC_SIZE_L3_Y][idxL3X * RFC_SIZE_L3_X];
            for(int m = 0; m < RFC_SIZE_L3_Y; ++m) {
                for(int n = 0; n < RFC_SIZE_L3_X; ++n) {
                    datResMax = FUNC_MAX2(datResMax, (*m_datRes)[idxL3Y * RFC_SIZE_L3_Y + m][idxL3X * RFC_SIZE_L3_X + n]);
                    datResMin = FUNC_MIN2(datResMin, (*m_datRes)[idxL3Y * RFC_SIZE_L3_Y + m][idxL3X * RFC_SIZE_L3_X + n]);
                }
            }
            int datAbs = FUNC_MAX2(abs(datResMax), abs(datResMin));
            int sizRes = datAbs == 0 ? 0 : (FUNC_LOG2(datAbs) + 1);
            if (   sizRes    != 0
                && datResMax ==   (1 << (sizRes - 1))
                && datResMin == - (1 << (sizRes - 1))
            ) {
                ++sizRes;
            }
            sizRes = FUNC_MIN2(RFC_SIZE_RES_MAX, sizRes);
            pushDat(m_bitWithSizL2Full, sizRes, RFC_DATA_MOD_WD);

            // data
            for(int m = 0; m < RFC_SIZE_L3_Y; ++m) {
                for(int n = 0; n < RFC_SIZE_L3_Y; ++n) {
                    pushRes(m_bitWithSizL2Data, sizRes, (*m_datRes)[idxL3Y * RFC_SIZE_L3_Y + m][idxL3X * RFC_SIZE_L3_X + n],
                                                            datOri [idxL3Y * RFC_SIZE_L3_Y + m][idxL3X * RFC_SIZE_L3_X + n]);
                }
            }

            // sign
            if (sizRes != 0 && sizRes != RFC_SIZE_RES_MAX) {
                if (   datResMax ==   (1 << (sizRes - 1)))
                    pushDat(m_bitWithSizL2Sign, 0, 1);
                if (   datResMin == - (1 << (sizRes - 1)))
                    pushDat(m_bitWithSizL2Sign, 1, 1);
                if (   datResMax ==   (1 << (sizRes - 1))
                    && datResMin == - (1 << (sizRes - 1))
                ) {
                    assert(0);
                }
            }
        }
    }

    // merge data, sign with first pixel, size
    pushBit(m_bitWithSizL2Full, m_bitWithSizL2Data);
    pushBit(m_bitWithSizL2Full, m_bitWithSizL2Sign);

    // debug
    //if ((enmLvlDmp_t)m_cfg->dmpEnmLvlItf >= enmLvlDmp_t::INTERMEDIATE)
      //  dmpSizBit(m_bitWithSizL2Full.siz, idxLaC);

    // save bit or ori
    bool flgUseOri;
    if (m_bitWithSizL2Full.siz >= sizX * sizY * DATA_PXL_WD) {
        flgUseOri = 1;
        for (int j = 0; j < sizY; ++j) {
            for (int i = 0; i < sizX; ++i)
                pushDat(m_bitWithSizL1, datOri[j][i], DATA_PXL_WD);
        }
    }
    else {
        flgUseOri = 0;
        pushBit(m_bitWithSizL1, m_bitWithSizL2Full);
    }

    // return
    return flgUseOri;
}

bool CPR::encodeWithL3_72x64(
    pxl_t datOri[72][64],
    int idxLaC, int sizX, int sizY, bool flgPreFirst, pxl_t datPreFirst)
{
    // variables
    m_bitWithSizL2Full.siz = 0;
    m_bitWithSizL2Data.siz = 0;
    m_bitWithSizL2Sign.siz = 0;

    // encode first pixel
    if (flgPreFirst) {
        (*m_datPre)[0][0] = datPreFirst;
        (*m_datRes)[0][0] = datOri[0][0] - (*m_datPre)[0][0];
    }
    else {
        pushDat(m_bitWithSizL2Full, datOri[0][0], DATA_PXL_WD);
        (*m_datRes)[0][0] = 0;
    }

    // do DPCM (differential pulse-code modulation)
    for (int j = 1; j < sizY; ++j) {
        (*m_datPre)[j][0] = datOri[j - 1][0];
        (*m_datRes)[j][0] = datOri[j][0] - (*m_datPre)[j][0];
    }
    for (int j = 0; j < sizY; ++j) {
        for (int i = 1; i < sizX; ++i) {
            (*m_datPre)[j][i] = datOri[j][i - 1];
            (*m_datRes)[j][i] = datOri[j][i] - (*m_datPre)[j][i];
        }
    }

    // encode residual size, data and sign
    for (int idxL3Y = 0; idxL3Y < sizY / RFC_SIZE_L3_Y; ++idxL3Y) {
        for (int idxL3X = 0; idxL3X < sizX / RFC_SIZE_L3_X; ++idxL3X) {
            // size
            int datResMax = (*m_datRes)[idxL3Y * RFC_SIZE_L3_Y][idxL3X * RFC_SIZE_L3_X];
            int datResMin = (*m_datRes)[idxL3Y * RFC_SIZE_L3_Y][idxL3X * RFC_SIZE_L3_X];
            for (int m = 0; m < RFC_SIZE_L3_Y; ++m) {
                for (int n = 0; n < RFC_SIZE_L3_X; ++n) {
                    datResMax = FUNC_MAX2(datResMax, (*m_datRes)[idxL3Y * RFC_SIZE_L3_Y + m][idxL3X * RFC_SIZE_L3_X + n]);
                    datResMin = FUNC_MIN2(datResMin, (*m_datRes)[idxL3Y * RFC_SIZE_L3_Y + m][idxL3X * RFC_SIZE_L3_X + n]);
                }
            }
            int datAbs = FUNC_MAX2(abs(datResMax), abs(datResMin));
            int sizRes = datAbs == 0 ? 0 : (FUNC_LOG2(datAbs) + 1);
            if (sizRes != 0
                && datResMax == (1 << (sizRes - 1))
                && datResMin == -(1 << (sizRes - 1))
                ) {
                ++sizRes;
            }
            sizRes = FUNC_MIN2(RFC_SIZE_RES_MAX, sizRes);
            pushDat(m_bitWithSizL2Full, sizRes, RFC_DATA_MOD_WD);

            // data
            for (int m = 0; m < RFC_SIZE_L3_Y; ++m) {
                for (int n = 0; n < RFC_SIZE_L3_Y; ++n) {
                    pushRes(m_bitWithSizL2Data, sizRes, (*m_datRes)[idxL3Y * RFC_SIZE_L3_Y + m][idxL3X * RFC_SIZE_L3_X + n],
                        datOri[idxL3Y * RFC_SIZE_L3_Y + m][idxL3X * RFC_SIZE_L3_X + n]);
                }
            }

            // sign
            if (sizRes != 0 && sizRes != RFC_SIZE_RES_MAX) {
                if (datResMax == (1 << (sizRes - 1)))
                    pushDat(m_bitWithSizL2Sign, 0, 1);
                if (datResMin == -(1 << (sizRes - 1)))
                    pushDat(m_bitWithSizL2Sign, 1, 1);
                if (datResMax == (1 << (sizRes - 1))
                    && datResMin == -(1 << (sizRes - 1))
                    ) {
                    assert(0);
                }
            }
        }
    }

    // merge data, sign with first pixel, size
    pushBit(m_bitWithSizL2Full, m_bitWithSizL2Data);
    pushBit(m_bitWithSizL2Full, m_bitWithSizL2Sign);

    // debug
    //if ((enmLvlDmp_t)m_cfg->dmpEnmLvlItf >= enmLvlDmp_t::INTERMEDIATE)
      //  dmpSizBit(m_bitWithSizL2Full.siz, idxLaC);

    // save bit or ori
    bool flgUseOri;
    if (m_bitWithSizL2Full.siz >= sizX * sizY * DATA_PXL_WD) {
        flgUseOri = 1;
        for (int j = 0; j < sizY; ++j) {
            for (int i = 0; i < sizX; ++i)
                pushDat(m_bitWithSizL1, datOri[j][i], DATA_PXL_WD);
        }
    }
    else {
        flgUseOri = 0;
        pushBit(m_bitWithSizL1, m_bitWithSizL2Full);
    }

    // return
    return flgUseOri;
}

bool CPR::encodeWithL3_36x32(
    pxl_t datOri[36][32],
    int idxLaC, int sizX, int sizY, bool flgPreFirst, pxl_t datPreFirst)
{
    // variables
    m_bitWithSizL2Full.siz = 0;
    m_bitWithSizL2Data.siz = 0;
    m_bitWithSizL2Sign.siz = 0;

    // encode first pixel
    if (flgPreFirst) {
        (*m_datPre)[0][0] = datPreFirst;
        (*m_datRes)[0][0] = datOri[0][0] - (*m_datPre)[0][0];
    }
    else {
        pushDat(m_bitWithSizL2Full, datOri[0][0], DATA_PXL_WD);
        (*m_datRes)[0][0] = 0;
    }

    // do DPCM (differential pulse-code modulation)
    for (int j = 1; j < sizY; ++j) {
        (*m_datPre)[j][0] = datOri[j - 1][0];
        (*m_datRes)[j][0] = datOri[j][0] - (*m_datPre)[j][0];
    }
    for (int j = 0; j < sizY; ++j) {
        for (int i = 1; i < sizX; ++i) {
            (*m_datPre)[j][i] = datOri[j][i - 1];
            (*m_datRes)[j][i] = datOri[j][i] - (*m_datPre)[j][i];
        }
    }

    // encode residual size, data and sign
    for (int idxL3Y = 0; idxL3Y < sizY / RFC_SIZE_L3_Y; ++idxL3Y) {
        for (int idxL3X = 0; idxL3X < sizX / RFC_SIZE_L3_X; ++idxL3X) {
            // size
            int datResMax = (*m_datRes)[idxL3Y * RFC_SIZE_L3_Y][idxL3X * RFC_SIZE_L3_X];
            int datResMin = (*m_datRes)[idxL3Y * RFC_SIZE_L3_Y][idxL3X * RFC_SIZE_L3_X];
            for (int m = 0; m < RFC_SIZE_L3_Y; ++m) {
                for (int n = 0; n < RFC_SIZE_L3_X; ++n) {
                    datResMax = FUNC_MAX2(datResMax, (*m_datRes)[idxL3Y * RFC_SIZE_L3_Y + m][idxL3X * RFC_SIZE_L3_X + n]);
                    datResMin = FUNC_MIN2(datResMin, (*m_datRes)[idxL3Y * RFC_SIZE_L3_Y + m][idxL3X * RFC_SIZE_L3_X + n]);
                }
            }
            int datAbs = FUNC_MAX2(abs(datResMax), abs(datResMin));
            int sizRes = datAbs == 0 ? 0 : (FUNC_LOG2(datAbs) + 1);
            if (sizRes != 0
                && datResMax == (1 << (sizRes - 1))
                && datResMin == -(1 << (sizRes - 1))
                ) {
                ++sizRes;
            }
            sizRes = FUNC_MIN2(RFC_SIZE_RES_MAX, sizRes);
            pushDat(m_bitWithSizL2Full, sizRes, RFC_DATA_MOD_WD);

            // data
            for (int m = 0; m < RFC_SIZE_L3_Y; ++m) {
                for (int n = 0; n < RFC_SIZE_L3_Y; ++n) {
                    pushRes(m_bitWithSizL2Data, sizRes, (*m_datRes)[idxL3Y * RFC_SIZE_L3_Y + m][idxL3X * RFC_SIZE_L3_X + n],
                        datOri[idxL3Y * RFC_SIZE_L3_Y + m][idxL3X * RFC_SIZE_L3_X + n]);
                }
            }

            // sign
            if (sizRes != 0 && sizRes != RFC_SIZE_RES_MAX) {
                if (datResMax == (1 << (sizRes - 1)))
                    pushDat(m_bitWithSizL2Sign, 0, 1);
                if (datResMin == -(1 << (sizRes - 1)))
                    pushDat(m_bitWithSizL2Sign, 1, 1);
                if (datResMax == (1 << (sizRes - 1))
                    && datResMin == -(1 << (sizRes - 1))
                    ) {
                    assert(0);
                }
            }
        }
    }

    // merge data, sign with first pixel, size
    pushBit(m_bitWithSizL2Full, m_bitWithSizL2Data);
    pushBit(m_bitWithSizL2Full, m_bitWithSizL2Sign);

    // debug
    //if ((enmLvlDmp_t)m_cfg->dmpEnmLvlItf >= enmLvlDmp_t::INTERMEDIATE)
      //  dmpSizBit(m_bitWithSizL2Full.siz, idxLaC);

    // save bit or ori
    bool flgUseOri;
    if (m_bitWithSizL2Full.siz >= sizX * sizY * DATA_PXL_WD) {
        flgUseOri = 1;
        for (int j = 0; j < sizY; ++j) {
            for (int i = 0; i < sizX; ++i)
                pushDat(m_bitWithSizL1, datOri[j][i], DATA_PXL_WD);
        }
    }
    else {
        flgUseOri = 0;
        pushBit(m_bitWithSizL1, m_bitWithSizL2Full);
    }

    // return
    return flgUseOri;
}

// dmpDatRfcCprOriLcu
void CPR::dmpDatRfcCprOriLcu(
pxl_t datOri[RFC_SIZE_L0_MAX_Y][RFC_SIZE_L0_MAX_X],
int idxLaC, int sizX, int sizY)
{
    string m_name = "RFC";
    string obj    = "datRfcCprOriLcu";
    string idx[2] = {"LU", "CH"};
    string typ    = "S0I8F0";
    string pst    = "dat";
    int msk       = 0xff;
    string fmt    = "%02x";
    int inc       = 1;
    FUNC_DMP_D_2(m_dmpDatRfcCprOriLcuFlg[idxLaC]
        ,        m_dmpDatRfcCprOriLcuFpt[idxLaC]
        ,        obj, idx[idxLaC], typ, pst
        ,        datOri
        ,        msk, fmt
        ,        sizY, inc, sizX, inc
    );
}

// dmpDatRfcCprBitLcu
void CPR::dmpDatRfcCprBitLcu(
pxl_t datBit[RFC_SIZE_L0_MAX_Y][RFC_SIZE_L0_MAX_X],
int idxLaC, int sizX, int sizY)
{
    string m_name = "RFC";
    string obj    = "datRfcCprBitLcu";
    string idx[2] = {"LU", "CH"};
    string typ    = "S0I8F0";
    string pst    = "dat";
    int msk       = 0xff;
    string fmt    = "%02x";
    int inc       = 1;
    FUNC_DMP_D_2(m_dmpDatRfcCprBitLcuFlg[idxLaC]
        ,        m_dmpDatRfcCprBitLcuFpt[idxLaC]
        ,        obj, idx[idxLaC], typ, pst
        ,        datBit
        ,        msk, fmt
        ,        sizY, inc, sizX, inc
    );
}

// dmpDatRfcCprPrmLcu
void CPR::dmpDatRfcCprPrmLcu(
prm_t datPrm[RFC_NUMB_L1_IN_L0_Y][RFC_NUMB_L1_IN_L0_X],
int idxLaC, int sizX, int sizY)
{
    string m_name = "RFC";
    string obj    = "datRfcCprPrmLcu";
    string idx[2] = {"LU", "CH"};
    string typ    = "S0I11F0";
    string pst    = "dat";
    int msk       = 0x7ff;
    string fmt    = "%03x";
    int inc       = 1;
    FUNC_DMP_D_2(m_dmpDatRfcCprPrmLcuFlg[idxLaC]
        ,        m_dmpDatRfcCprPrmLcuFpt[idxLaC]
        ,        obj, idx[idxLaC], typ, pst
        ,        datPrm
        ,        msk, fmt
        ,        sizY / RFC_SIZE_L1_Y, inc
        ,        sizX / RFC_SIZE_L1_X, inc
    );
}

// dmpSizBit
void CPR::dmpSizBit(
int dbgSizBit, int idxLaC)
{
    string m_name = "RFC";
    string obj    = "sizBit";
    string idx[2] = {"LU", "CH"};
    string typ    = "S0I8F0";
    string pst    = "dat";
    int msk       = 0xff;
    string fmt    = "%02x";
    FUNC_DMP_D_0(m_dmpSizBitFlg[idxLaC]
        ,        m_dmpSizBitFpt[idxLaC]
        ,        obj, idx[idxLaC], typ, pst
        ,        dbgSizBit
        ,        msk, fmt
    );
}

