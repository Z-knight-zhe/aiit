// -----------------------------------------------------------------------------------------------+
//                                                                                                |
// Copyright (c) 2018~2022 Peng Cheng Laboratory (PCL)                                            |
//                                                                                                |
// All Rights Reserved.                                                                           |
//                                                                                                |
// THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION WHICH IS THE PROPERTY OF           |
// PENG CHENG LABORATORY OR ITS LICENSORS AND IS SUBJECT TO LICENSE TERMS.                        |
// -----------------------------------------------------------------------------------------------+
#ifndef  VU440

#include <stdint.h>
#define DMA_IN_ADDR0		0x88000000
#define DMA_IN_ADDR1		0x90000000
#define DMA_IN_SIZE			0x00020000

#define FW_DEBUG_BASE            (0x10000000)
#define SYNC_READ_ADDR           (FW_DEBUG_BASE+0x0000ffc0)
#define SYNC_WRITE_ADDR          (FW_DEBUG_BASE+0x0000ffd0)

#define FW_VPU_ADDR_BASE         0x10000000
#define SYNC_VPU_READ_ADDR       (FW_VPU_ADDR_BASE+0x0000ffe0)

#define TO_RISCV                 (1)
#define TO_PCIE                  (2)

#define ARRAY_NUM                (2)

#define FROM_DEVICE_NAME_DEFAULT "/dev/xdma0_c2h_0"
#define TO_DEVICE_NAME_DEFAULT   "/dev/xdma0_h2c_0"
#define BYPASS_DEFAULT			 "/dev/xdma0_bypass"

#define DDR_BASE_ADDR		0x80000000
#define ORI_YUV_Y0			(DDR_BASE_ADDR+0x00000000)
#define ORI_YUV_U0			(DDR_BASE_ADDR+0x00C00000)
#define ORI_YUV_V0			(DDR_BASE_ADDR+0x01000000)
#define ORI_YUV_Y1			(DDR_BASE_ADDR+0x01400000)
#define ORI_YUV_U1			(DDR_BASE_ADDR+0x02000000)
#define ORI_YUV_V1			(DDR_BASE_ADDR+0x02400000)
#define REC_YUV_Y0			(DDR_BASE_ADDR+0x02800000)
#define REC_YUV_U0			(DDR_BASE_ADDR+0x03400000)
#define REC_YUV_V0			(DDR_BASE_ADDR+0x03800000)
#define REC_YUV_Y1			(DDR_BASE_ADDR+0x03C00000)
#define REC_YUV_U1			(DDR_BASE_ADDR+0x04800000)
#define REC_YUV_V1			(DDR_BASE_ADDR+0x04C00000)
#define ENC_BITSTREAM		(DDR_BASE_ADDR+0x05000000)
#define GOLDEN_TV0			(DDR_BASE_ADDR+0x06000000)
#define GOLDEN_TV1			(DDR_BASE_ADDR+0x06C00000)

#define BS_HEAD_ADDR		0x87800000L

enum system_log_level
{
	SYSTEM_LOG_TRC = 0,
	SYSTEM_LOG_DBG = 1,
	SYSTEM_LOG_INFO = 2,
	SYSTEM_LOG_WARN = 3,
	SYSTEM_LOG_ERR = 4,
	SYSTEM_LOG_FATAL = 5
};

extern int g_pingpong_odd;
static int g_ffplay_log_level = SYSTEM_LOG_DBG;

#define COLOR_NONE "\033[0m"
#define COLOR_BLACK "\033[0;30m"
#define COLOR_LIGHT_GRAY "\033[0;37m"
#define COLOR_DARK_GRAY "\033[1;30m"
#define COLOR_BLUE "\033[0;34m"
#define COLOR_LIGHT_BLUE "\033[1;34m"
#define COLOR_GREEN "\033[0;32m"
#define COLOR_LIGHT_GREEN "\033[1;32m"
#define COLOR_RED "\033[0;31m"
#define COLOR_LIGHT_RED "\033[1;31m"
#define COLOR_PURPLE "\033[0;35m"
#define COLOR_LIGHT_PURPLE "\033[1;35m"
#define COLOR_YELLOW "\033[0;33m"
#define COLOR_LIGHT_YELLOW "\033[1;33m"
#define COLOR_WHITE "\033[1;37m"

#define LOG_TRCPR(fmt,...) do{ \
	if(g_ffplay_log_level<=SYSTEM_LOG_TRC)\
	{\
		printf(COLOR_NONE "[TRACE] %s:%d%s: " fmt, __FUNCTION__, __LINE__,COLOR_NONE,##__VA_ARGS__);\
	}\
}while(0);\

#define LOG_DBGPR(fmt,...) do{ \
	if(g_ffplay_log_level<=SYSTEM_LOG_DBG)\
	{\
		printf(COLOR_LIGHT_GREEN "[DEBUG] %s:%d%s: " fmt, __FUNCTION__, __LINE__,COLOR_NONE,##__VA_ARGS__);\
	}\
}while(0);\
	
#define LOG_INFOPR(fmt,...) do{ \
	if(g_ffplay_log_level<=SYSTEM_LOG_INFO)\
	{\
		printf(COLOR_LIGHT_BLUE "[INFO] %s:%d%s: " fmt, __FUNCTION__, __LINE__,COLOR_NONE,##__VA_ARGS__);\
	}\
}while(0);\

#define LOG_WARNPR(fmt,...) do{ \
	if(g_ffplay_log_level<=SYSTEM_LOG_WARN)\
	{\
		printf(COLOR_LIGHT_YELLOW "[WARN] %s:%d%s: " fmt, __FUNCTION__, __LINE__,COLOR_NONE,##__VA_ARGS__);\
	}\
}while(0);\

#define LOG_ERRPR(fmt,...) do{ \
	if(g_ffplay_log_level<=SYSTEM_LOG_ERR)\
	{\
		printf(COLOR_LIGHT_RED "[ERROR] %s:%d%s: " fmt, __FUNCTION__, __LINE__,COLOR_NONE,##__VA_ARGS__);\
	}\
}while(0);\

#define LOG_FATALPR(fmt,...) do{ \
	if(g_ffplay_log_level<=SYSTEM_LOG_FATAL)\
	{\
		printf(COLOR_LIGHT_PURPLE "[FATAL] %s:%d%s: " fmt, __FUNCTION__, __LINE__,COLOR_NONE,##__VA_ARGS__);\
	}\
}while(0);\

#define FPGA_DEBUG
#ifdef FPGA_DEBUG
#define tracef LOG_TRCPR
#define debugf LOG_DBGPR
#define infof LOG_INFOPR
#define warnf LOG_WARNPR
#define errorf LOG_ERRPR
#define fatalf LOG_FATALPR
#else
#define tracef 
#define debugf 
#define infof 
#define warnf 
#define errorf LOG_ERRPR
#define fatalf LOG_FATALPR
#endif

typedef union {
	unsigned char byte[8];
	unsigned int len;
} t_pcie_type;

void sync_read(uint32_t udId, uint32_t udAddr, uint32_t udSize);
void wait_read(uint32_t udId, uint32_t *pudAddr, uint32_t *pudSize);
void sync_write(uint32_t udId, uint32_t udAddr, uint32_t udSize);
void wait_write(uint32_t udId, uint32_t *pudAddr, uint32_t *pudSize);
int reg_read(uint32_t udAddr, uint32_t *pRData);
int reg_write(uint32_t udAddr, uint32_t udWData);
int write_to_device(uint32_t udAddr, uint32_t udSize, int dInFileFd, uint32_t udBase);
ssize_t read_to_buffer(char* fname, int fd, char* buffer, uint64_t size, uint64_t base);
ssize_t write_from_buffer(char* fname, int fd, char* buffer, uint64_t size, uint64_t base);
int write_to_tmpfile(const char *fname, uint32_t udSize, int dInFileFd, uint32_t udBase);
int write_yuv420(int fp, int w, int h, int bit_depth_input, int bit_depth_internal, int pingpong);
int file_type_support(const char *fname);
int write_reg32(void* address, unsigned int val, unsigned int offset);

#endif // ! WIN32
