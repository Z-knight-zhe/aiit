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
    //  Release        : XK265
    //
//------------------------------------------------------------------------------
    //
    //  Filename       : cfg_type.hpp
    //  Author         : Huang Leilei
    //  Status         : phase 002
    //  Reset          : 2021-04-14
    //  Description    : configuration related header (type)
    //
//------------------------------------------------------------------------------
    //
    //  *** auto-generated, please do NOT modify it mannually ***
    //
//------------------------------------------------------------------------------

#ifndef __XK265_CFG_TYPE_HPP__
#define __XK265_CFG_TYPE_HPP__

//*** INCLUDE ******************************************************************
#include "common.hpp"


//*** TYPE *********************************************************************
struct cfg_t {
    // changeable
    int         idxFra;
    int         idxLcuX;
    int         idxLcuY;
    int         trueSizLcuX;
    int         trueSizLcuY;
    int         enmTyp;
    int         datQpFra;
    int         datQpLcu;

    // system
    string      strFileYuv;
    string      strPrefixDump;
    string      strPrefixLoad;
    int         enmLvlEnc;
    int         enmLvlLog;

    // top
    bool        flgDmpCsv;
    bool        flgDmpYuvRec;
    bool        flgDmpDbg;
    bool        flgDistortion[2];
    bool        flgSizCuIntra[4];
    bool        flgSizCuInter[4];
    bool        flgPrtCuIntra[2];
    bool        flgPrtCuInter[4];
    int         numFra;
    int         idxFraBgn;
    int         numChn;
    int         sizFraX;
    int         sizFraY;
    int         sizSwX;
    int         sizSwY;
    int         sizLcu;
    int         enmStd;
    int         enmModRun;
    int         datLvl;
    int         datFps;
    int         datQpSeq;
    int         datPrdIntra;

    // gop
    int         gopSiz;
    int         gopEnmTyp[32];
    int         gopDatPoc[32];
    int         gopDatQpDlt[32];
    bool        gopFlgRefNeg[32];
    bool        gopFlgRefPos[32];
    int         gopNumRefPos[32];
    int         gopDatPocRefDltNeg[32];
    int         gopDatPocRefDltPos[32][5];

    // roi
    int         roiIdxLcuBgnX[2];
    int         roiIdxLcuBgnY[2];
    int         roiIdxLcuEndX[2];
    int         roiIdxLcuEndY[2];
    bool        roiFlgTyp[2];
    int         roiEnmTyp[2];
    bool        roiFlgQp[2];
    int         roiDatQp[2];

    // r_c
    bool        r_cFlg[8];
    double      r_cDatBps;
    double      r_cDatBpsIntra;
    double      r_cDatBpsInter;
    int         r_cDatQpMinIntra;
    int         r_cDatQpMinInter;
    int         r_cDatQpMaxIntra;
    int         r_cDatQpMaxInter;
    int         r_cDatThrRatiIntra[6];
    int         r_cDatDltRatiIntra[7];
    int         r_cDatThrRatiInter[6];
    int         r_cDatDltRatiInter[7];
    double      r_cDatSclSmthIntra;
    double      r_cDatSclSmthInter;
    int         r_cDatThrSmthIntra;
    int         r_cDatThrSmthInter;
    int         r_cDatDltSmthIntra;
    int         r_cDatDltSmthInter;
    double      r_cDatPrmSmthIntra[3];
    double      r_cDatPrmSmthInter[3];
    int         r_cDatThrSadrIntra[6];
    int         r_cDatDltSadrIntra[7];
    int         r_cDatThrSadrInter[6];
    int         r_cDatDltSadrInter[7];
    int         r_cDatThrSatdIntra[6];
    int         r_cDatDltSatdIntra[7];
    int         r_cDatThrSatdInter[6];
    int         r_cDatDltSatdInter[7];
    int         r_cDatThrSamvInter[6];
    int         r_cDatDltSamvInter[7];

    // iip
    int         iipEnmStg;
    int         iipEnmScl;
    int         iipDatRatio;
    double      iipDatSclOnQp[14];
    double      iipDatSclOnMv[5];

    // mrg
    int         mrgEnmLvl;
    int         mrgNum;

    // skp
    bool        skpFlg[2];
    int         skpEnmScl;
    double      skpDatSclOnChn[2];
    double      skpDatSclOnQp[14];
    double      skpDatSclOnMv[5];

    // rmd
    bool        rmdFlgRun;
    int         rmdEnmModLoad;
    int         rmdNumGrp;
    int         rmdSizGrp;
    int         rmdDatMod[42];

    // ime
    bool        imeFlgRun;
    int         imeEnmModLoad;
    int         imeNumPtn;
    int         imeEnmCtr[8];
    int         imePosCtrX[8];
    int         imePosCtrY[8];
    int         imeSizPtnX[8];
    int         imeSizPtnY[8];
    int         imeEnmSlp[8];
    bool        imeFlgD_s[8];
    double      imeDatSclMvd;

    // fme
    bool        fmeFlgRun;
    int         fmeEnmModLoad;
    int         fmeNumTrv;
    int         fmeNumCtr[2];
    int         fmeEnmCtr[2][4];
    int         fmeEnmItp[2][4];
    double      fmeDatSclMvd;

    // rdo
    bool        rdoFlgRun;
    bool        rdoFlgRdoq[3][4];
    bool        rdoFlgSkpTsf[4];
    int         rdoEnmModLoad;
    int         rdoNumMod;
    double      rdoDatSclLambda[4];
    double      rdoDatFitICu[2];
    double      rdoDatFitIPu[6];
    double      rdoDatFitITuS[7];
    double      rdoDatFitITuI[7];
    double      rdoDatFitPCu;
    double      rdoDatFitPPu[8];
    double      rdoDatFitPTuS[6];
    double      rdoDatFitPTuI[6];

    // rec
    bool        recFlgRun;
    int         recEnmModLoad;

    // ilf
    bool        ilfFlgRun;
    int         ilfEnmModLoad;
    bool        ilfFlgDbf;
    bool        ilfFlgSao[2];
    int         ilfDatOffBtDiv2;
    int         ilfDatOffTcDiv2;
    double      ilfDatSclLambda[4];

    // e_c
    bool        e_cFlgRun;
    bool        e_cFlgSbh;
    int         e_cEnmModLoad;

    // e_d
    bool        e_dFlgRun;
    int         e_dEnmModLoad;

    // itf
    bool        itfFlgRfc;

    // dump
    int         dmpEnmLvlTop;
    int         dmpEnmLvlRmd;
    int         dmpEnmLvlIme;
    int         dmpEnmLvlFme;
    int         dmpEnmLvlRdo;
    int         dmpEnmLvlIlf;
    int         dmpEnmLvlE_c;
    int         dmpEnmLvlE_d;
    int         dmpEnmLvlItf;

    // derived
    int         numLst;
    int         numLcuX;
    int         numLcuY;
    int         numCu;
    int         numPu;
    int         num4x4;
    int         numLaC;
    int         trueSizFraX;
    int         trueSizFraY;
    int         sizSchX;
    int         sizSchY;
    int         trueSizSwX;
    int         trueSizSwY;
    int         sizTuMax;
    int         sizRecExt;
    int         sizRefExt;
    int         sizIipExtX;
    int         sizIipExtY;
    int         sizModExtX;
    int         sizModExtY;
    int         sizFmvExtX;
    int         sizFmvExtY;
    int         sizCbfExtX;
    int         sizCbfExtY;
    int         datDepCu;
    int         gopNumReorderPics;
    int         gopDatPocCur;
    int         gopDatPocCurAbs;
    int         gopDatPocRefDltNegCur;
    int         gopDatPocRefDltPosCur;
    int         gopDatPocRefNegCur;
    int         gopDatPocRefPosCur;
    int         gopDatPocRefNegCurAbs;
    int         gopDatPocRefPosCurAbs;
    int         gopIdxRefLast[33];
    bool        rdoFlgRdoqAny;
    bool        rdoFlgSkpTsfAny;

    // =
    cfg_t &operator=(const cfg_t &cfg) {
        // changeable
        idxFra                      = cfg.idxFra               ;
        idxLcuX                     = cfg.idxLcuX              ;
        idxLcuY                     = cfg.idxLcuY              ;
        trueSizLcuX                 = cfg.trueSizLcuX          ;
        trueSizLcuY                 = cfg.trueSizLcuY          ;
        enmTyp                      = cfg.enmTyp               ;
        datQpFra                    = cfg.datQpFra             ;
        datQpLcu                    = cfg.datQpLcu             ;

        // system
        strFileYuv                  = cfg.strFileYuv           ;
        strPrefixDump               = cfg.strPrefixDump        ;
        strPrefixLoad               = cfg.strPrefixLoad        ;
        enmLvlEnc                   = cfg.enmLvlEnc            ;
        enmLvlLog                   = cfg.enmLvlLog            ;

        // top
        flgDmpCsv                   = cfg.flgDmpCsv            ;
        flgDmpYuvRec                = cfg.flgDmpYuvRec         ;
        flgDmpDbg                   = cfg.flgDmpDbg            ;
        memcpy(flgDistortion        , cfg.flgDistortion        , sizeof(flgDistortion        ));
        memcpy(flgSizCuIntra        , cfg.flgSizCuIntra        , sizeof(flgSizCuIntra        ));
        memcpy(flgSizCuInter        , cfg.flgSizCuInter        , sizeof(flgSizCuInter        ));
        memcpy(flgPrtCuIntra        , cfg.flgPrtCuIntra        , sizeof(flgPrtCuIntra        ));
        memcpy(flgPrtCuInter        , cfg.flgPrtCuInter        , sizeof(flgPrtCuInter        ));
        numFra                      = cfg.numFra               ;
        idxFraBgn                   = cfg.idxFraBgn            ;
        numChn                      = cfg.numChn               ;
        sizFraX                     = cfg.sizFraX              ;
        sizFraY                     = cfg.sizFraY              ;
        sizSwX                      = cfg.sizSwX               ;
        sizSwY                      = cfg.sizSwY               ;
        sizLcu                      = cfg.sizLcu               ;
        enmStd                      = cfg.enmStd               ;
        enmModRun                   = cfg.enmModRun            ;
        datLvl                      = cfg.datLvl               ;
        datFps                      = cfg.datFps               ;
        datQpSeq                    = cfg.datQpSeq             ;
        datPrdIntra                 = cfg.datPrdIntra          ;

        // gop
        gopSiz                      = cfg.gopSiz               ;
        memcpy(gopEnmTyp            , cfg.gopEnmTyp            , sizeof(gopEnmTyp            ));
        memcpy(gopDatPoc            , cfg.gopDatPoc            , sizeof(gopDatPoc            ));
        memcpy(gopDatQpDlt          , cfg.gopDatQpDlt          , sizeof(gopDatQpDlt          ));
        memcpy(gopFlgRefNeg         , cfg.gopFlgRefNeg         , sizeof(gopFlgRefNeg         ));
        memcpy(gopFlgRefPos         , cfg.gopFlgRefPos         , sizeof(gopFlgRefPos         ));
        memcpy(gopNumRefPos         , cfg.gopNumRefPos         , sizeof(gopNumRefPos         ));
        memcpy(gopDatPocRefDltNeg   , cfg.gopDatPocRefDltNeg   , sizeof(gopDatPocRefDltNeg   ));
        memcpy(gopDatPocRefDltPos   , cfg.gopDatPocRefDltPos   , sizeof(gopDatPocRefDltPos   ));

        // roi
        memcpy(roiIdxLcuBgnX        , cfg.roiIdxLcuBgnX        , sizeof(roiIdxLcuBgnX        ));
        memcpy(roiIdxLcuBgnY        , cfg.roiIdxLcuBgnY        , sizeof(roiIdxLcuBgnY        ));
        memcpy(roiIdxLcuEndX        , cfg.roiIdxLcuEndX        , sizeof(roiIdxLcuEndX        ));
        memcpy(roiIdxLcuEndY        , cfg.roiIdxLcuEndY        , sizeof(roiIdxLcuEndY        ));
        memcpy(roiFlgTyp            , cfg.roiFlgTyp            , sizeof(roiFlgTyp            ));
        memcpy(roiEnmTyp            , cfg.roiEnmTyp            , sizeof(roiEnmTyp            ));
        memcpy(roiFlgQp             , cfg.roiFlgQp             , sizeof(roiFlgQp             ));
        memcpy(roiDatQp             , cfg.roiDatQp             , sizeof(roiDatQp             ));

        // r_c
        memcpy(r_cFlg               , cfg.r_cFlg               , sizeof(r_cFlg               ));
        r_cDatBps                   = cfg.r_cDatBps            ;
        r_cDatBpsIntra              = cfg.r_cDatBpsIntra       ;
        r_cDatBpsInter              = cfg.r_cDatBpsInter       ;
        r_cDatQpMinIntra            = cfg.r_cDatQpMinIntra     ;
        r_cDatQpMinInter            = cfg.r_cDatQpMinInter     ;
        r_cDatQpMaxIntra            = cfg.r_cDatQpMaxIntra     ;
        r_cDatQpMaxInter            = cfg.r_cDatQpMaxInter     ;
        memcpy(r_cDatThrRatiIntra   , cfg.r_cDatThrRatiIntra   , sizeof(r_cDatThrRatiIntra   ));
        memcpy(r_cDatDltRatiIntra   , cfg.r_cDatDltRatiIntra   , sizeof(r_cDatDltRatiIntra   ));
        memcpy(r_cDatThrRatiInter   , cfg.r_cDatThrRatiInter   , sizeof(r_cDatThrRatiInter   ));
        memcpy(r_cDatDltRatiInter   , cfg.r_cDatDltRatiInter   , sizeof(r_cDatDltRatiInter   ));
        r_cDatSclSmthIntra          = cfg.r_cDatSclSmthIntra   ;
        r_cDatSclSmthInter          = cfg.r_cDatSclSmthInter   ;
        r_cDatThrSmthIntra          = cfg.r_cDatThrSmthIntra   ;
        r_cDatThrSmthInter          = cfg.r_cDatThrSmthInter   ;
        r_cDatDltSmthIntra          = cfg.r_cDatDltSmthIntra   ;
        r_cDatDltSmthInter          = cfg.r_cDatDltSmthInter   ;
        memcpy(r_cDatPrmSmthIntra   , cfg.r_cDatPrmSmthIntra   , sizeof(r_cDatPrmSmthIntra   ));
        memcpy(r_cDatPrmSmthInter   , cfg.r_cDatPrmSmthInter   , sizeof(r_cDatPrmSmthInter   ));
        memcpy(r_cDatThrSadrIntra   , cfg.r_cDatThrSadrIntra   , sizeof(r_cDatThrSadrIntra   ));
        memcpy(r_cDatDltSadrIntra   , cfg.r_cDatDltSadrIntra   , sizeof(r_cDatDltSadrIntra   ));
        memcpy(r_cDatThrSadrInter   , cfg.r_cDatThrSadrInter   , sizeof(r_cDatThrSadrInter   ));
        memcpy(r_cDatDltSadrInter   , cfg.r_cDatDltSadrInter   , sizeof(r_cDatDltSadrInter   ));
        memcpy(r_cDatThrSatdIntra   , cfg.r_cDatThrSatdIntra   , sizeof(r_cDatThrSatdIntra   ));
        memcpy(r_cDatDltSatdIntra   , cfg.r_cDatDltSatdIntra   , sizeof(r_cDatDltSatdIntra   ));
        memcpy(r_cDatThrSatdInter   , cfg.r_cDatThrSatdInter   , sizeof(r_cDatThrSatdInter   ));
        memcpy(r_cDatDltSatdInter   , cfg.r_cDatDltSatdInter   , sizeof(r_cDatDltSatdInter   ));
        memcpy(r_cDatThrSamvInter   , cfg.r_cDatThrSamvInter   , sizeof(r_cDatThrSamvInter   ));
        memcpy(r_cDatDltSamvInter   , cfg.r_cDatDltSamvInter   , sizeof(r_cDatDltSamvInter   ));

        // iip
        iipEnmStg                   = cfg.iipEnmStg            ;
        iipEnmScl                   = cfg.iipEnmScl            ;
        iipDatRatio                 = cfg.iipDatRatio          ;
        memcpy(iipDatSclOnQp        , cfg.iipDatSclOnQp        , sizeof(iipDatSclOnQp        ));
        memcpy(iipDatSclOnMv        , cfg.iipDatSclOnMv        , sizeof(iipDatSclOnMv        ));

        // mrg
        mrgEnmLvl                   = cfg.mrgEnmLvl            ;
        mrgNum                      = cfg.mrgNum               ;

        // skp
        memcpy(skpFlg               , cfg.skpFlg               , sizeof(skpFlg               ));
        skpEnmScl                   = cfg.skpEnmScl            ;
        memcpy(skpDatSclOnChn       , cfg.skpDatSclOnChn       , sizeof(skpDatSclOnChn       ));
        memcpy(skpDatSclOnQp        , cfg.skpDatSclOnQp        , sizeof(skpDatSclOnQp        ));
        memcpy(skpDatSclOnMv        , cfg.skpDatSclOnMv        , sizeof(skpDatSclOnMv        ));

        // rmd
        rmdFlgRun                   = cfg.rmdFlgRun            ;
        rmdEnmModLoad               = cfg.rmdEnmModLoad        ;
        rmdNumGrp                   = cfg.rmdNumGrp            ;
        rmdSizGrp                   = cfg.rmdSizGrp            ;
        memcpy(rmdDatMod            , cfg.rmdDatMod            , sizeof(rmdDatMod            ));

        // ime
        imeFlgRun                   = cfg.imeFlgRun            ;
        imeEnmModLoad               = cfg.imeEnmModLoad        ;
        imeNumPtn                   = cfg.imeNumPtn            ;
        memcpy(imeEnmCtr            , cfg.imeEnmCtr            , sizeof(imeEnmCtr            ));
        memcpy(imePosCtrX           , cfg.imePosCtrX           , sizeof(imePosCtrX           ));
        memcpy(imePosCtrY           , cfg.imePosCtrY           , sizeof(imePosCtrY           ));
        memcpy(imeSizPtnX           , cfg.imeSizPtnX           , sizeof(imeSizPtnX           ));
        memcpy(imeSizPtnY           , cfg.imeSizPtnY           , sizeof(imeSizPtnY           ));
        memcpy(imeEnmSlp            , cfg.imeEnmSlp            , sizeof(imeEnmSlp            ));
        memcpy(imeFlgD_s            , cfg.imeFlgD_s            , sizeof(imeFlgD_s            ));
        imeDatSclMvd                = cfg.imeDatSclMvd         ;

        // fme
        fmeFlgRun                   = cfg.fmeFlgRun            ;
        fmeEnmModLoad               = cfg.fmeEnmModLoad        ;
        fmeNumTrv                   = cfg.fmeNumTrv            ;
        memcpy(fmeNumCtr            , cfg.fmeNumCtr            , sizeof(fmeNumCtr            ));
        memcpy(fmeEnmCtr            , cfg.fmeEnmCtr            , sizeof(fmeEnmCtr            ));
        memcpy(fmeEnmItp            , cfg.fmeEnmItp            , sizeof(fmeEnmItp            ));
        fmeDatSclMvd                = cfg.fmeDatSclMvd         ;

        // rdo
        rdoFlgRun                   = cfg.rdoFlgRun            ;
        memcpy(rdoFlgRdoq           , cfg.rdoFlgRdoq           , sizeof(rdoFlgRdoq           ));
        memcpy(rdoFlgSkpTsf         , cfg.rdoFlgSkpTsf         , sizeof(rdoFlgSkpTsf         ));
        rdoEnmModLoad               = cfg.rdoEnmModLoad        ;
        rdoNumMod                   = cfg.rdoNumMod            ;
        memcpy(rdoDatSclLambda      , cfg.rdoDatSclLambda      , sizeof(rdoDatSclLambda      ));
        memcpy(rdoDatFitICu         , cfg.rdoDatFitICu         , sizeof(rdoDatFitICu         ));
        memcpy(rdoDatFitIPu         , cfg.rdoDatFitIPu         , sizeof(rdoDatFitIPu         ));
        memcpy(rdoDatFitITuS        , cfg.rdoDatFitITuS        , sizeof(rdoDatFitITuS        ));
        memcpy(rdoDatFitITuI        , cfg.rdoDatFitITuI        , sizeof(rdoDatFitITuI        ));
        rdoDatFitPCu                = cfg.rdoDatFitPCu         ;
        memcpy(rdoDatFitPPu         , cfg.rdoDatFitPPu         , sizeof(rdoDatFitPPu         ));
        memcpy(rdoDatFitPTuS        , cfg.rdoDatFitPTuS        , sizeof(rdoDatFitPTuS        ));
        memcpy(rdoDatFitPTuI        , cfg.rdoDatFitPTuI        , sizeof(rdoDatFitPTuI        ));

        // rec
        recFlgRun                   = cfg.recFlgRun            ;
        recEnmModLoad               = cfg.recEnmModLoad        ;

        // ilf
        ilfFlgRun                   = cfg.ilfFlgRun            ;
        ilfEnmModLoad               = cfg.ilfEnmModLoad        ;
        ilfFlgDbf                   = cfg.ilfFlgDbf            ;
        memcpy(ilfFlgSao            , cfg.ilfFlgSao            , sizeof(ilfFlgSao            ));
        ilfDatOffBtDiv2             = cfg.ilfDatOffBtDiv2      ;
        ilfDatOffTcDiv2             = cfg.ilfDatOffTcDiv2      ;
        memcpy(ilfDatSclLambda      , cfg.ilfDatSclLambda      , sizeof(ilfDatSclLambda      ));

        // e_c
        e_cFlgRun                   = cfg.e_cFlgRun            ;
        e_cFlgSbh                   = cfg.e_cFlgSbh            ;
        e_cEnmModLoad               = cfg.e_cEnmModLoad        ;

        // e_d
        e_dFlgRun                   = cfg.e_dFlgRun            ;
        e_dEnmModLoad               = cfg.e_dEnmModLoad        ;

        // itf
        itfFlgRfc                   = cfg.itfFlgRfc            ;

        // dump
        dmpEnmLvlTop                = cfg.dmpEnmLvlTop         ;
        dmpEnmLvlRmd                = cfg.dmpEnmLvlRmd         ;
        dmpEnmLvlIme                = cfg.dmpEnmLvlIme         ;
        dmpEnmLvlFme                = cfg.dmpEnmLvlFme         ;
        dmpEnmLvlRdo                = cfg.dmpEnmLvlRdo         ;
        dmpEnmLvlIlf                = cfg.dmpEnmLvlIlf         ;
        dmpEnmLvlE_c                = cfg.dmpEnmLvlE_c         ;
        dmpEnmLvlE_d                = cfg.dmpEnmLvlE_d         ;
        dmpEnmLvlItf                = cfg.dmpEnmLvlItf         ;

        // derived
        numLst                      = cfg.numLst               ;
        numLcuX                     = cfg.numLcuX              ;
        numLcuY                     = cfg.numLcuY              ;
        numCu                       = cfg.numCu                ;
        numPu                       = cfg.numPu                ;
        num4x4                      = cfg.num4x4               ;
        numLaC                      = cfg.numLaC               ;
        trueSizFraX                 = cfg.trueSizFraX          ;
        trueSizFraY                 = cfg.trueSizFraY          ;
        sizSchX                     = cfg.sizSchX              ;
        sizSchY                     = cfg.sizSchY              ;
        trueSizSwX                  = cfg.trueSizSwX           ;
        trueSizSwY                  = cfg.trueSizSwY           ;
        sizTuMax                    = cfg.sizTuMax             ;
        sizRecExt                   = cfg.sizRecExt            ;
        sizRefExt                   = cfg.sizRefExt            ;
        sizIipExtX                  = cfg.sizIipExtX           ;
        sizIipExtY                  = cfg.sizIipExtY           ;
        sizModExtX                  = cfg.sizModExtX           ;
        sizModExtY                  = cfg.sizModExtY           ;
        sizFmvExtX                  = cfg.sizFmvExtX           ;
        sizFmvExtY                  = cfg.sizFmvExtY           ;
        sizCbfExtX                  = cfg.sizCbfExtX           ;
        sizCbfExtY                  = cfg.sizCbfExtY           ;
        datDepCu                    = cfg.datDepCu             ;
        gopNumReorderPics           = cfg.gopNumReorderPics    ;
        gopDatPocCur                = cfg.gopDatPocCur         ;
        gopDatPocCurAbs             = cfg.gopDatPocCurAbs      ;
        gopDatPocRefDltNegCur       = cfg.gopDatPocRefDltNegCur;
        gopDatPocRefDltPosCur       = cfg.gopDatPocRefDltPosCur;
        gopDatPocRefNegCur          = cfg.gopDatPocRefNegCur   ;
        gopDatPocRefPosCur          = cfg.gopDatPocRefPosCur   ;
        gopDatPocRefNegCurAbs       = cfg.gopDatPocRefNegCurAbs;
        gopDatPocRefPosCurAbs       = cfg.gopDatPocRefPosCurAbs;
        memcpy(gopIdxRefLast        , cfg.gopIdxRefLast        , sizeof(gopIdxRefLast        ));
        rdoFlgRdoqAny               = cfg.rdoFlgRdoqAny        ;
        rdoFlgSkpTsfAny             = cfg.rdoFlgSkpTsfAny      ;
        return *this;
    }
};

#endif /* __XK265_CFG_TYPE_HPP__ */
