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

#ifndef RATE_CONTROL_PICTURE_H
#define RATE_CONTROL_PICTURE_H
//#define LOWDELAY_RC_OPEN
#include "base_type.h"

#ifdef VSB_TEMP_TEST
#include "video_statistic.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif



  //#include "enccommon.h"
  //#include "H264Sei.h"

  enum
  {
    VCENCRC_OVERFLOW = -1
  };

#define RC_CBR_HRD  0   /* 1 = Constant bit rate model. Must use filler
  * data to conform */

#define CTRL_LEVELS          7  /* DO NOT CHANGE THIS */
#define CHECK_POINTS_MAX    10  /* DO NOT CHANGE THIS */
#define RC_TABLE_LENGTH     10  /* DO NOT CHANGE THIS */
#define INTRA_QPDELTA_DEFAULT  (-5)
#define CTBRC_STRENGTH

#ifndef CTBRC_STRENGTH 
#define QP_FRACTIONAL_BITS  0
#else
#define QP_FRACTIONAL_BITS  8
#endif

#define TOL_CTB_RC_FIX_POINT 7

#define RC_MOVING_AVERAGE_FRAMES 60

#define LEAST_MONITOR_FRAME       3
#define I32_MAX                  (0x7FFFFFFF)

#define IS_CTBRC(x) ((x)&3)
#define IS_CTBRC_FOR_QUALITY(x) ((x)&1)
#define IS_CTBRC_FOR_BITRATE(x) ((x)&2)
#define CLR_CTBRC_FOR_BITRATE(x) {(x)&=(~2);}

#define CTB_RC_QP_STEP_FIXP 16
#define CTB_RC_ROW_FACTOR_FIXP 16

#define VCENC_DEFAULT_QP          26
  typedef enum
  {
      ENCHW_NOK = -1,
      ENCHW_OK = 0
  } bool_e;

  typedef enum
  {
      ENCHW_NO = 0,
      ENCHW_YES = 1
  } true_e;
  enum slice_type
  {
      B_SLICE,
      P_SLICE,
      I_SLICE
  };
#define CU_TREE_QP 10
  typedef enum
  {
      ASIC_HEVC = 1,
      ASIC_H264 = 2,
      ASIC_AV1 = 3,
      ASIC_JPEG = 4,
      ASIC_MJPEG = 5,
      ASIC_CUTREE = 6,
      ASIC_VP9 = 7
  } asicCodingType_e;
  typedef struct {
      i32 frame[120];
      i32 length;
      i32 count;
      i32 pos;
      i32 frameRateNumer;
      i32 frameRateDenom;
  } rc_ma_s;
  typedef struct {
      u32 intraCu8Num[120];
      u32 skipCu8Num[120];
      u32 PBFrame4NRdCost[120];
      i32 length;
      i32 count;
      i32 pos;
      u32 average_intraCu8Num;
      u32 average_skipCu8Num;
      u32 average_PBFrame4NRdCost;
  } rc_ma_char;
  typedef struct
  {
    i64  a1;               /* model parameter */
    i64  a2;               /* model parameter */
    i32  qp_prev;          /* previous QP */
    i32  qs[RC_TABLE_LENGTH + 1]; /* quantization step size */
    i32  bits[RC_TABLE_LENGTH + 1]; /* Number of bits needed to code residual */
    i32  pos;              /* current position */
    i32  len;              /* current lenght */
    i32  zero_div;         /* a1 divisor is 0 */
    i32  cbr;
    i32  weight;
	i32  frameBitCntLast;
    i32  targetPicSizeLast;
} linReg_s;

  typedef struct
  {
    i32 wordError[CTRL_LEVELS]; /* Check point error bit */
    i32 qpChange[CTRL_LEVELS];  /* Check point qp difference */
    i32 wordCntTarget[CHECK_POINTS_MAX];    /* Required bit count */
    i32 wordCntPrev[CHECK_POINTS_MAX];  /* Real bit count */
    i32 checkPointDistance;
    i32 checkPoints;
  } hevcQpCtrl_s;

  /* Virtual buffer */
  typedef struct
  {
    i32 bufferSize;          /* size of the virtual buffer */
    i32 minBitRate;
    i32 maxBitRate;
    i32 bitRate;             /* input bit rate per second */
    i32 bitPerPic;           /* average number of bits per picture */
    i32 picTimeInc;          /* timeInc since last coded picture */
    i32 timeScale;           /* input frame rate numerator */
    i32 unitsInTic;          /* input frame rate denominator */
    i32 virtualBitCnt;       /* virtual (channel) bit count */
    i32 realBitCnt;          /* real bit count */
    i32 bufferOccupancy;     /* number of bits in the buffer */
    i32 skipFrameTarget;     /* how many frames should be skipped in a row */
    i32 skippedFrames;       /* how many frames have been skipped in a row */
    i32 nonZeroTarget;
    i32 bucketFullness;      /* Leaky Bucket fullness */
    i32 bucketLevel;         /* Leaky Bucket fullness + virtualBitCnt */
    /* new rate control */
    i32 windowRem;
    i32 seconds;             /* Full seconds elapsed */
    i32 averageBitRate;      /* This buffer average bitrate for full seconds */
  } rcVirtualBuffer_s;

  typedef struct
  {
    i32 x0;
    i32 x1;
    i32 xMin;
    i32 started;
    i32 preFrameMad;
    ptr_t ctbMemPreAddr;
    u32* ctbMemPreVirtualAddr;
  } ctbRcModel_s;

  typedef struct
  {
    ctbRcModel_s models[3];
    i32 qpSumForRc;
    i32 qpStep;
    i32 rowFactor;
    ptr_t ctbMemCurAddr;
    u32* ctbMemCurVirtualAddr;
  } ctbRateControl_s;

  typedef struct
  {
      double coeffMin;
      double coeff;
      double count;
      double decay;
      double offset;
      i32 qp;
  } rcPredictor;
  struct sw_picture{
      int type;
      int poc;
      struct sw_picture * rpl[2][16];
      int x265Type;
      double curQpRc;
      int cost;
  };
  typedef struct
  {
    true_e picRc;
    u32    ctbRc;            /* ctb header qp can vary, check point rc */
    true_e picSkip;          /* Frame Skip enable */
    true_e hrd;              /* HRD restrictions followed or not */
    true_e vbr;              /* Variable Bit Rate Control by qpMin */
    u32 fillerIdx;
    i32 picArea;
    i32 ctbPerPic;            /* Number of macroblock per picture */
    i32 ctbRows;              /* ctb rows in picture */
    i32 ctbCols;              /* ctb columns in picture */
    i32 ctbSize;              /* ctb size */
    i32 coeffCntMax;         /* Number of coeff per picture */
    i32 nonZeroCnt;
    i32 srcPrm;              /* Source parameter */
    i32 qpSum;               /* Qp sum counter */
    i32 qpNum;
    float averageQp;
    u32 sliceTypeCur;
    u32 sliceTypePrev;
    true_e frameCoded;       /* Pic coded information */
    i32 fixedQp;             /* Pic header qp when fixed */
    i32 qpHdr;               /* Pic header qp of current voded picture */
    i32 qpMin;               /* Pic header minimum qp for current picture */
    i32 qpMax;               /* Pic header maximum qp for current picture */
    i32 qpMinI;              /* Pic header minimum qp for I frame, user set */
    i32 qpMaxI;              /* Pic header maximum qp for I frame, user set */
    i32 qpMinPB;             /* Pic header minimum qp for P/B frame, user set */
    i32 qpMaxPB;             /* Pic header maximum qp for P/B frame, user set */
    i32 qpHdrPrev;           /* Pic header qp of previous coded picture */
    i32 qpLastCoded;         /* Quantization parameter of last coded mb */
    i32 qpTarget;            /* Target quantrization parameter */
    u32 estTimeInc;
    i32 outRateNum;
    i32 outRateDenom;
    i32 gDelaySum;
    i32 gInitialDelay;
    i32 gInitialDoffs;
    hevcQpCtrl_s qpCtrl;
    rcVirtualBuffer_s virtualBuffer;
    
    i32 gBufferMin, gBufferMax;
    /* new rate control */
    linReg_s linReg[4];       /* Data for R-Q model for inter frames */
    linReg_s rError[4];       /* Rate prediction error for inter frames(bits) */
    linReg_s intra;        /* Data for intra frames */
    linReg_s intraError;   /* Prediction error for intra frames */
    linReg_s gop;          /* Data for GOP */
    //linReg_s linReg_BFrame;       /* Data for R-Q model for B frames */
    //linReg_s rError_BFrame;       /* Rate prediction error for B frames(bits) */
    i32 targetPicSize;
    i32 minPicSizeI;
    i32 maxPicSizeI;
    i32 minPicSizeP;
    i32 maxPicSizeP;
    i32 minPicSizeB;
    i32 maxPicSizeB;
    i32 frameBitCnt;
    i32 tolMovingBitRate;
    float f_tolMovingBitRate;
    i32 monitorFrames;
    float tolCtbRcInter;
    float tolCtbRcIntra;
    /* for gop rate control */
    i32 gopQpSum;           /* Sum of current GOP inter frame QPs */
    i32 gopQpDiv;
    i32 gopPQpSum;           /* Sum of current GOP inter frame QPs */
    i32 gopPQpNum;
    i32 gopBitCnt;          /* Current GOP bit count so far */
    i32 gopAvgBitCnt;       /* Previous GOP average bit count */
    u32 frameCnt;
    i32 bitrateWindow;
    i32 windowLen;          /* Bitrate window which tries to match target */
    i32 intraInterval;      /* Distance between two previous I-frames */
    i32 intraIntervalCtr;
    i32 intraQpDelta;
    i32 longTermQpDelta; /* QP delta of the frame using long term reference */
    i32 frameQpDelta;
    u32 fixedIntraQp;
    i32 bpp;
    i32 hierarchial_bit_allocation_total_weight;
    i32 hierarchial_bit_allocation_map[8][8];
    i32 hierarchial_sse[8][8];
    i32 smooth_psnr_in_gop;
    i32 hierarchial_alg_map[8][8];
    i32 hierarchial_decoding_order[8][8];
    i32 hierarchial_bit_allocation_bitmap[8];
    i32 hierarchial_bit_allocation_GOP_size;
    i32 hierarchial_bit_allocation_GOP_size_previous;
    i32 encoded_frame_number;
    u32 gopPoc;
    //CTB_RC
    u32 ctbRcQpDeltaReverse;
    u32 ctbRcBitsMin; 
    u32 ctbRcBitsMax; 
    u32 ctbRctotalLcuBit; 
    u32 bitsRatio; 
    u32 ctbRcThrdMin; 
    u32 ctbRcThrdMax; 
    i32 seqStart;
    u32 rcQpDeltaRange;
    u32 rcBaseMBComplexity;
    i32 picQpDeltaMin;
    i32 picQpDeltaMax;
    i32 resetIntraPicQp;
    i32 inputSceneChange;

    rc_ma_s ma;
    rc_ma_char ma_char;
    
    u32 rcPicComplexity;
    float complexity;

    i32 minIQp;
    i32 finiteQp;
    i32 gopMulti;
    i32 gopLastBitCnt;
    i32 intraframeBitCnt;
    
    u32 intraCu8Num;
    u32 skipCu8Num;
    u32 PBFrame4NRdCost;
    double reciprocalOfNumBlk8;  /* Multiply this factor to get blk8x8 avg */

    u32 codingType;

    i32 i32MaxPicSize;
    u32 u32PFrameByQPStep;
    i32 qpHdrPrevGop;
    i32 qpHdrPrevI;
    u32 u32PrevIBitCnt;
    u32 u32StaticSceneIbitPercent;

    ctbRateControl_s ctbRateCtrl;

    i32 crf; /*CRF constant*/
    double pbOffset;
    double ipOffset;
    unsigned long long crf_iCostAvg;
    double crf_iQpAvg;
    int crf_iFrames;

    i32 pass;
    double pass1CurCost;
    double pass1GopCost[4];
    double pass1AvgCost[4];
    i32 pass1GopFrameNum[4];
    i32 pass1FrameNum[4];
    rcPredictor rcPred[4];
    i32 predId;
    struct sw_picture pic;
    struct sw_picture prevRefPic;
    struct sw_picture nextRefPic;
  } vcencRateControl_s;
 
  i32 getFramePredId(i32 type);
  /*------------------------------------------------------------------------------
      Function prototypes
  ------------------------------------------------------------------------------*/
  bool_e VCEncInitRc(vcencRateControl_s *rc, u32 newStream);
  void VCEncBeforePicRc(vcencRateControl_s *rc, u32 timeInc, u32 sliceType, bool use_ltr_cur, struct sw_picture*);
  i32 VCEncAfterPicRc(vcencRateControl_s *rc, u32 nonZeroCnt, u32 byteCnt,
                     u32 qpSum,u32 qpNum);
  u32 HevcFillerRc(vcencRateControl_s *rc, u32 frameCnt);
  i32 rcCalculate(i32 a, i32 b, i32 c);
  i32 getPredId(vcencRateControl_s *rc);
  double qp2qScale(double qp);


#ifdef __cplusplus
}
#endif
#include "enc_def.h"
#if RATECONTROL
#include <list>
using namespace std;

#define MIN_QP                       4
#define MAX_QP                       63
#define SHIFT_QP                     18
#define LAM_2Level_TU            0.8

#if HEVC_RC
typedef int   Int;
typedef int   Int64;
typedef       double              Double;
typedef unsigned char byte;    //!< byte type definition
#define pel_t byte


#define ALPHA     6.7542;
#define BETA1     1.2517
#define BETA2     1.7860
#define QP_Limit  10

const Int g_RCInvalidQPValue = -999;
const Int g_RCMaxPicListSize = 32;
const Int g_SmoothPicInterval = 60;
const Int g_RCSmoothWindowSize = 40;
const Int g_RCIterationNum = 20;
const Double g_RCWeightPicTargetBitInGOP = 0.9;
const Double g_RCWeightPicRargetBitInBuffer = 1.0 - g_RCWeightPicTargetBitInGOP;
const Double g_RCAlphaMinValue = 0.05;
const Double g_RCAlphaMaxValue = 500.0;
const Double g_RCBetaMinValue = -3.0;
const Double g_RCBetaMaxValue = -0.1;
const Double g_RCWeightHistoryLambda = 0.5;
const Double g_RCWeightCurrentLambda = 1.0 - g_RCWeightHistoryLambda;

typedef struct {
    Double m_alpha;
    Double m_beta;
} TRCParameter;

typedef struct {
    Int     imgtype;
    Int     m_picQP;
    Int     m_framelevel;
    Int     m_picActualHeaderBits;
    Double  lastLevelLambda;
    float   blurred_complexity;
} RCPreviousPictures;

#endif

// function type
typedef void(*vpp_ipred_t)(pel_t* p_pred, pel_t* p_top, pel_t* p_left);
//typedef int(*pixel_cmp_t)(const pel_t *pix1, intptr_t i_pix1, const pel_t *pix2, intptr_t i_pix2);
//typedef void(*pixel_cmp_x3_t)(const pel_t *fenc, const pel_t *pix0, const pel_t *pix1, const pel_t *pix2, intptr_t i_stride, int scores[3]);
//typedef void(*pixel_cmp_x4_t)(const pel_t *fenc, const pel_t *pix0, const pel_t *pix1, const pel_t *pix2, const pel_t *pix3, intptr_t i_stride, int scores[4]);
//typedef void(*copy_pp_t)(pel_t *dst, intptr_t dstStride, const pel_t *src, intptr_t srcStride); // dst is aligned

/* ---------------------------------------------------------------------------
* motion vector
*/
union mv_t {
    uint32_t    v;                    // v = ((y << 16) | (x & 0xFFFF)), 32-bit
    struct {
        int16_t x;                    // x, low  16-bit
        int16_t y;                    // y, high 16-bit
    };
};

/* ---------------------------------------------------------------------------
* starvpp_me_t
*/
typedef struct vpp_me_t {
    int             mv_min[2];        /* full pel MV range for motion search (min) */
    int             mv_max[2];        /* full pel MV range for motion search (max) */
    mv_t            bmv;              /* [OUT] best motion vector */
    mv_t            pmv;              /* pred motion vector for the current block */
    uint16_t* mvbits;           /* used for getting the mv bits */
    //pixel_cmp_t     sad_8x8;          /* function handle for cal sad of 8x8 block */
    //pixel_cmp_x3_t  sad_8x8_x3;       /* function handle for cal sad of 8x8 block (X3) */
    //pixel_cmp_x4_t  sad_8x8_x4;       /* function handle for cal sad of 8x8 block (X4) */

    //pixel_cmp_t     satd_8x8;
    //pixel_cmp_x3_t  satd_8x8_x3;
    //pixel_cmp_x4_t  satd_8x8_x4;
} vpp_me_t;

/* ---------------------------------------------------------------------------
* image complexity
*/
typedef struct complex_t {
    long long   i_best_cost;          /* sum best  cost of whole frame */
    long long   i_inter_cost;         /* sum inter cost of whole frame */
    long long   i_intra_cost;         /* sum intra cost of whole frame */
    int         i_sum_intras;         /* number of intra blocks in frame */
    int         i_slice_type;         /* slice type of frame, or -1 for uncertain */
    U32         i_min_rate;
} complex_t;

typedef struct RCLCU {
    int     GlobeLCUNum;
    int     CurBits;
    int     PreBits;
    double  PreBpp;
    double  CurBpp;
    double  LCUTBL;
    double  LCUTBLdelta;
    double  LCUBuffer;
    double  LCUBufferError;
    double  LCUBufferDifError;
    double  LCUPreBufferError;
    int     LCUPreLCUQP;
    int     LCUCurLCUQP;
    int     LCUdeltaQP;
    double  LCUCurLambda;
} RCLCU;

typedef struct LCU_RC {
    Int m_actualBits;
    Int m_targetBits;
    Int m_numberOfPixel;
    Double m_lambda;
    Int m_costIntra;
    Int m_targetBitsLeft;
    Double m_bitWeight;
    Int m_QP;
} LCU_RC;

typedef struct RateControl {
    int     LCUbaseQP;
    int     RConoff;
    int     LCURConoff;
    int     IntraPeriod;
    int     TotalFrames;
    int     GlobalImageNumber;
    int     CodedFrameNumber;
    int     ImageType;
    int     ImageQP;
    int     DeltaQP;
    int     GopLeaderDeltaQP;
    int     RcQP;
    int     RcMBQP;
    int     SumMBQP;
    int     NumMB;
    int     ImageBits;
    int     FrameWidth;
    int     FrameHeight;
    double  ImageBpp;
    double  Belta;
    double  GopBpp;
    int     GopAvgBaseQP;
    int     GopAvgBaseQPCount;
    double  GopAllKeyBpp;
    double  FirstBufferSizeLevel;
    double  IntraFrameBpp;
    double  InterFrameBpp;
    double  TargetBitrate;
    double  bpp;
    double  TargetBufferLevel;
    double  DeltaBufferLevel;
    double  CurrentBufferSizeBpp;
    double  BufferError;
    double  BufferDifError;
    double  PreBufferError;
    double  LowestBufferSizeBpp;
    double  HighestBufferSizeBpp;
    int     GopFlag;
    FILE* FReportFrame;
    FILE* FReportLCU;
    RCLCU* prclcu;
#if HEVC_RC

    bool m_useLCUSeparateModel;
    //GOP Level
    Int  bitsRatio[4];
    Int  GOPLevel[4];
    Int64 m_SeqbitsLeft;
    Int64 m_GOPbitsLeft;
    Double  m_alphaUpdate;
    Double  m_betaUpdate;
    Int  m_picTargetBitInGOP[4];          // every frame target bits
    TRCParameter  m_picPara[5];           // LDP frame level(3) + intra picture(1) + non-reference picture(1)
    TRCParameter  m_LCUPara[5][2040];      // max : 1920x1080
    //Pic Level
    Int m_frameQp;                        // current frame Qp
    Double m_frameLambda;                 // current frame Lambda
    Int m_frameLevel;                     // current frame level in a GOP
    Int m_numberOfPixel;                  // current frame pixels number
    Int m_targetBits;                     // current frame target bits
    Int m_adaptiveBit;
    Int m_framesLeft;
    Int64 m_bitsLeft;
    Int iGOPid;                           // frame index in a GOP
    Int numPic;                           // frame number in a GOP

    Double m_totalCostIntra;
    Int m_picActualHeaderBits;            // every frame actual header bits
    Int m_picActualBits;                  // the whole frame, including header
    Int m_picQP;                          // in integer form
    long long int total_distortion;
    Int m_totalactualbits;                // total actual bits in a second
#if U0132_TARGET_BITS_SATURATION
    bool       m_CpbSaturationEnabled;    // Enable target bits saturation to avoid CPB overflow and underflow
    Int        m_cpbState;                // CPB State 
    UInt       m_cpbSize;                 // CPB size
    UInt       m_bufferingRate;           // Buffering rate
    Double     m_RCInitialCpbFullness;    // initial CPB fullness
#if V0078_ADAPTIVE_LOWER_BOUND
    Int        m_lowerBound;
#endif
#endif
#endif
    complex_t complxs;
    pel_t* ref_yuv;
    pel_t* cur_yuv;

    //pixel_funcs_t  pixf;
    vpp_me_t       me;                    /* search on previous frame */
    vpp_ipred_t    intra_pred[4];         /* intra predications */
    //pixel_cmp_t    block_cost;

    double  short_term_cplxsum;
    double  short_term_cplxcount;
    float   blurred_complexity;
    //#if rc_algorithm_2
    Int  m_picTargetBits;
    //#endif
#if rc_algorithm_3
    int     b_abr;
    double  qcompress;
    double  cplxr_sum;             /* sum of bits*qscale/rceq */
    double  wanted_bits_window;    /* target bitrate * window */
    double  rate_factor_constant;
    float   last_rceq;
    float   last_qscale;
    float   qpa_rc;
    float   lstep;
#endif
#if rc_algorithm_4
    bool rc_two_pass;
#endif
#if rc_algorithm_6
    Double target_bpp;
    Double lastpic_bpp;
    bool   current_pic_update;
    bool   current_pic_reset_0;
#endif
#if rc_algorithm_9
    Int  qp_num[64];
    Int  qp_val[64];
#endif
#if LCU_RATECONTROL
    Int m_Lcu_idx;
    Int m_numberOfLCU;
    Int m_remainingCostIntra;
    Int m_LCULeft;
    LCU_RC m_lcu[2280];
#endif

} RateControl;

void Init_LCURateControl(RateControl* prc, int NumUnitsLCU);
int  CalculateLCUDeltaQP(RateControl* prc);
void UpdataLCURateControl(RateControl* prc, int qp, double lambda, int bits, int NumLCU);
void Init_RateControl(RateControl* prc);
void Updata_RateControl(RateControl* prc, int framebits, int frameqp, int imgtype, int framenumber, int goplength);
int  CalculateGopDeltaQP_RateControl(RateControl* prc, int imgtype, int framenumber, int goplength);
void Init_FuzzyController(double ScaFactor);
void updateAfterLCU(Int ActualBits);
void getLCUInitTargetBits();
double getLCUTargetBpp(int LCU_nr);
double getLCUEstLambda(int bpp);
Double getLCUEstLambdaAndQP(Double bpp, Int clipPicQP, int* estQP);
Int getLCUEstQP(Double lambda, Int clipPicQP);
extern RateControl* pRC;
extern RCLCU* pRCLCU;

#if HEVC_RC
void rate_control();
void gop_level_ratecontrol();
void frame_level_ratecontrol();
Int  xEstGOPTargetBits();
void xCalEquaCoeff(RateControl* prc, Double* lambdaRatio, Double* equaCoeffA, Double* equaCoeffB, Int GOPSize);
Double xSolveEqua(Double targetBpp, Double* equaCoeffA, Double* equaCoeffB, Int GOPSize);
void setAllBitRatio(Double basicLambda, Double* equaCoeffA, Double* equaCoeffB);
Int xEstPicTargetBits(RateControl* pRC);
void updata_ratecontrol_parameters(int bits);
#if V0078_ADAPTIVE_LOWER_BOUND
Int xEstPicLowerBound(Int targetBits);
#endif
#endif


/*------------------vpp-------------------*/
#if defined(_MSC_VER)
#define DECLARE_ALIGNED(var, n) __declspec(align(n)) var
#elif defined(__INTEL_COMPILER)
#define DECLARE_ALIGNED(var, n) var __declspec(align(n))
#else
#define DECLARE_ALIGNED(var, n) var __attribute__((aligned(n)))
#endif
#define ALIGN32(var)            DECLARE_ALIGNED(var, 32)
#define ALIGN16(var)            DECLARE_ALIGNED(var, 16)

#define VPP_BLK_SIZE            8     /* VPP block size: 8x8 */
#define VPP_PAD        VPP_BLK_SIZE   /* number of pixels padded around the reference frame */
#define MAX_DISTORTION     (1 << 30)  /* maximum distortion (1 << bitdepth)^2 * (MAX_CU_SIZE)^2 */
#define STARAVS_MAX_GOP_SIZE    16    /* max length of mini GOP */

void starvpp_estimate_frame_complexity();
void starvpp_me_init();
void starvpp_vpp_intra_pred_init(uint32_t cpu, vpp_ipred_t* ipred);
void ratecontral_vpp();
void transform_8x8_h_vpp(U8 orgY_8x8[8][8], U8 predY_8x8[8][8], S23 dst[8][8], U1 NoResi_flag[1], U1 valid_flag[1]);
void transform_8x8_v_vpp(S23 src[8][8], S17 dst[8][8], U16 qp_table, U4 cutype[1], U1 NoResi_flag[1], U1 valid_flag[1], U6 cbp[1]);
U32 rate_luma_8x8_vpp(S17 quant_out[8][8], U3 img_type, S13 curr_mvd_x, S13 curr_mvd_y, U1 useDQP, U4 mode, U6 uiDirMode, S7 mostProbableMode[2], U6 cbp, S2 delta_qp);
#endif

#endif

