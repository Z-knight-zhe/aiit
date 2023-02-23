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
    //  Filename       : rfc_dcp.hpp
    //  Author         : Li Tingting
    //  Status         : phase 003
    //  Reset          : 2021-10-15
    //  Description    : dcp related header
    //
//------------------------------------------------------------------------------
    //
    //  Modified       : 2021-11-22 by HLL
    //  Description    : tidied up
    //
//------------------------------------------------------------------------------

#ifndef __XKCDC_RFC_DCP_HPP__
#define __XKCDC_RFC_DCP_HPP__

//*** INCLUDE ******************************************************************
    #include "rfc.hpp"


//*** CLASS ********************************************************************
class DCP
{
//--- PRIVATE VARIABLE -----------------
private:
    // buffer
        bitWithSiz_t m_bitWithSizL1;

    // pixel (ori, res)
        //               [position y       ][position x       ]
        pxl_t (*m_datOri)[RFC_SIZE_L0_MAX_Y][RFC_SIZE_L0_MAX_X];
        int   (*m_datRes)[RFC_SIZE_L0_MAX_Y][RFC_SIZE_L0_MAX_X];

        //             [L3 index y         ][L3 index x         ]
        int (*m_sizRes)[RFC_NUMB_L3_IN_L0_Y][RFC_NUMB_L3_IN_L0_X];


//--- PUBLIC FUNCTION ------------------
public:
    // basic
        DCP();
        ~DCP();
        void proc(pxl_t datBit[RFC_SIZE_L0_MAX_Y][RFC_SIZE_L0_MAX_X], prm_t datPrm[RFC_NUMB_L1_IN_L0_Y][RFC_NUMB_L1_IN_L0_X], int idxLaC, int sizX, int sizY);

    // tool
        void decodeWithL3(pxl_t datOri[RFC_SIZE_L0_MAX_Y][RFC_SIZE_L0_MAX_X], int sizX, int sizY, bool flgUseOri, bool flgFirst, pxl_t datFirst);

    // setBit
        void inline setBit(bitWithSiz_t *src)
        {
            m_bitWithSizL1 = *src;
            // !!! this is not a typo
            m_bitWithSizL1.siz = 0;
        }

    // cpyDatOri
        void inline cpyDatOri(void *dst)
        {
            memcpy(dst, *m_datOri, sizeof(*m_datOri));
        }

    // cpyDatOri
        void inline cpyDatOri(pxl_t datOri[RFC_SIZE_L0_MAX_Y][RFC_SIZE_L0_MAX_X], int idxChn, int sizX, int sizY)
        {
            int datScl = idxChn == 0 ? 0 : RFC_SIZE_L1_X >> 1;
            int datOff = idxChn != 2 ? 0 : RFC_SIZE_L1_X >> 1;
            for (int j = 0; j < sizY; ++j) {
                for (int i = 0; i < sizX; ++i) {
                    datOri[j][i] = (*m_datOri)[j][datOff + i / (RFC_SIZE_L1_X >> 1) * datScl + i];
                }
            }
        }
};

#endif /* __XKCDC_RFC_DCP_HPP__ */
