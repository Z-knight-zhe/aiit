/* ====================================================================================================================

  The copyright in this software is being made available under the License included below.
  This software may be subject to other third party and contributor rights, including patent rights, and no such
  rights are granted under this license.

  Copyright (c) 2018, HUAWEI TECHNOLOGIES CO., LTD. All rights reserved.
  Copyright (c) 2018, SAMSUNG ELECTRONICS CO., LTD. All rights reserved.
  Copyright (c) 2018, PEKING UNIVERSITY SHENZHEN GRADUATE SCHOOL. All rights reserved.
  Copyright (c) 2018, PENGCHENG LABORATORY. All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted only for
  the purpose of developing standards within Audio and Video Coding Standard Workgroup of China (AVS) and for testing and
  promoting such standards. The following conditions are required to be met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and
      the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
      the following disclaimer in the documentation and/or other materials provided with the distribution.
    * The name of HUAWEI TECHNOLOGIES CO., LTD. or SAMSUNG ELECTRONICS CO., LTD. may not be used to endorse or promote products derived from
      this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

* ====================================================================================================================
*/

////////////////////rmd.h////////////////////
//#ifndef _RMD_H_
//#define _RMD_H_


#include "type_def.h"
#include "com_def.h"

#define RMD_CU_SIZE_NUM 1293



#define COLLECT_RMD_DATA     0

#if COLLECT_RMD_DATA
#define RMD_CU_NUM_TV           200
#define	RMD_INPUT_pix_x          "rmd_input_pix_x.dat"
#define	RMD_INPUT_pix_y          "rmd_input_pix_y.dat"
#define	RMD_INPUT_LCU_Y          "rmd_input_LCU_Y.dat"
#define	RMD_OUTPUT_modes         "rmd_output_modes.dat"
#endif



//
//typedef struct {
//#if RMD_ARRAY
//    u8 mode4x8[1293][IPD_RDO_CNT];
//    u8 mode8x4[1293][IPD_RDO_CNT];
//    u8 mode4x16[1293][IPD_RDO_CNT];
//    u8 mode16x4[1293][IPD_RDO_CNT];
//    u8 mode8x8[1293][IPD_RDO_CNT];
//    u8 mode4x32[1293][IPD_RDO_CNT];
//    u8 mode32x4[1293][IPD_RDO_CNT];
//    u8 mode8x16[1293][IPD_RDO_CNT];
//    u8 mode16x8[1293][IPD_RDO_CNT];
//    u8 mode8x32[1293][IPD_RDO_CNT];
//    u8 mode32x8[1293][IPD_RDO_CNT];
//    u8 mode8x64[1293][IPD_RDO_CNT];
//    u8 mode64x8[1293][IPD_RDO_CNT];
//    u8 mode16x16[1293][IPD_RDO_CNT];
//    u8 mode16x32[1293][IPD_RDO_CNT];
//    u8 mode32x16[1293][IPD_RDO_CNT];
//    u8 mode16x64[1293][IPD_RDO_CNT];
//    u8 mode64x16[1293][IPD_RDO_CNT];
//    u8 mode32x32[1293][IPD_RDO_CNT];
//    u8 mode32x64[1293][IPD_RDO_CNT];
//    u8 mode64x32[1293][IPD_RDO_CNT];
//    u8 mode64x64[1293][IPD_RDO_CNT];
//    u8 mode[RMD_CU_SIZE_NUM][IPD_RDO_CNT];
//#endif
//
//} RMD_OUTPUT;

//typedef struct {
//#if RMD_ARRAY
//    U6 mode4x8[1293][IPD_RDO_CNT];
//    U6 mode8x4[1293][IPD_RDO_CNT];
//    U6 mode4x16[1293][IPD_RDO_CNT];
//    U6 mode16x4[1293][IPD_RDO_CNT];
//    U6 mode8x8[1293][IPD_RDO_CNT];
//    U6 mode4x32[1293][IPD_RDO_CNT];
//    U6 mode32x4[1293][IPD_RDO_CNT];
//    U6 mode8x16[1293][IPD_RDO_CNT];
//    U6 mode16x8[1293][IPD_RDO_CNT];
//    U6 mode8x32[1293][IPD_RDO_CNT];
//    U6 mode32x8[1293][IPD_RDO_CNT];
//    U6 mode8x64[1293][IPD_RDO_CNT];
//    U6 mode64x8[1293][IPD_RDO_CNT];
//    U6 mode16x16[1293][IPD_RDO_CNT];
//    U6 mode16x32[1293][IPD_RDO_CNT];
//    U6 mode32x16[1293][IPD_RDO_CNT];
//    U6 mode16x64[1293][IPD_RDO_CNT];
//    U6 mode64x16[1293][IPD_RDO_CNT];
//    U6 mode32x32[1293][IPD_RDO_CNT];
//    U6 mode32x64[1293][IPD_RDO_CNT];
//    U6 mode64x32[1293][IPD_RDO_CNT];
//    U6 mode64x64[1293][IPD_RDO_CNT];
//    U6 mode[RMD_CU_SIZE_NUM][IPD_RDO_CNT];
//#endif
//
//} RMD_OUTPUT_BW;

//typedef struct _RMD_BUF{
//    U8 up[max_buffer_width];
//    U8 left[MAX_CU_SIZE2];
//    U8 top_left;
//}RMD_BUF;
//#if RMD_ARRAY
//void rmd_run(pel cu_org[16][16][4 * 4], int pix_x, int pix_y, RMD_OUTPUT* rmd_output);
//#else
//void rmd_run(int pix_x, int pix_y, pel fenc_lcu_y[MAX_CU_SIZE2 * MAX_CU_SIZE2], RMD_OUTPUT* rmd_output);
//#endif
//#endif /* _RMD_H_ */
///////////////////////////rmd.h//////////////////////////////////////////



