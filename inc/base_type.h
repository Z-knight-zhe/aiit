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

#ifndef BASE_TYPE_H
#define BASE_TYPE_H

#include <stdint.h>
#include <stdio.h>
#ifndef NDEBUG
#include <assert.h>
#endif

#if defined(_MSC_VER)
/* MSVS doesn't define off_t, and uses _f{seek,tell}i64. */
typedef __int64 offi_t;
#define fseeko _fseeki64
#define ftello _ftelli64
#elif defined(_WIN32)
/* MinGW defines off_t as long and uses f{seek,tell}o64/off64_t for large
* files. */
#define fseeko fseeko64
#define ftello ftello64
#define offi_t off64_t
#endif  /* _WIN32 */

typedef int8_t    i8;
typedef int16_t   i16;
typedef int32_t   i32;
typedef int64_t   i64;
typedef size_t    ptr_t;
#ifndef __SYNTHESIS__
typedef uint8_t   u8;
typedef uint16_t  u16;
typedef uint32_t  u32;
//typedef uint64_t  u64;
#endif

#ifndef INLINE
#define INLINE inline
#endif

#ifndef NULL
#ifdef  __cplusplus
#define NULL    0
#else /*  */
#define NULL    ((void *)0)
#endif /*  */
#endif

typedef       short               Short;
typedef       int                 Int;
typedef       unsigned int        UInt;


#ifndef __cplusplus

#define HANTRO_FALSE 0
#define HANTRO_TRUE  1

#ifndef bool
typedef enum
{
  false = HANTRO_FALSE,
  true  = HANTRO_TRUE
} bool;
#endif

enum
{
  OK  = 0,
  NOK = -1
};
#endif

/* ASSERT */
#ifndef NDEBUG
#define ASSERT(x) assert(x)
#else
#define ASSERT(x)
#endif

#endif
