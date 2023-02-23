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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "pcie_utils.h"
#define MINI_SIZE           8
#define IFVCA_CLIP(n,min,max) (((n)>(max))? (max) : (((n)<(min))? (min) : (n)))

/*
 * man 2 write:
 * On Linux, write() (and similar system calls) will transfer at most
 * 	0x7ffff000 (2,147,479,552) bytes, returning the number of bytes
 *	actually transferred.  (This is true on both 32-bit and 64-bit
 *	systems.)
 */

#define RW_MAX_SIZE	0x7ffff000

void sync_read(uint32_t udId, uint32_t udAddr, uint32_t udSize)
{
    uint32_t udRData = 0xffffffff;

    do
    {
        reg_read(SYNC_READ_ADDR, &udRData);
        usleep(5);
    } while (udRData != 0);
    reg_write(SYNC_READ_ADDR+0xc, 0);
    reg_write(SYNC_READ_ADDR+0x8, udSize);
    reg_write(SYNC_READ_ADDR+0x4, udAddr);
    reg_write(SYNC_READ_ADDR,     udId);
    debugf("sync_read SYNC_READ_ADDR:0x%x id:0x%x addr:0x%x, size:0x%x\n", SYNC_READ_ADDR, udId, udAddr, udSize);

    return;
}

void wait_read(uint32_t udId, uint32_t *pudAddr, uint32_t *pudSize)
{
    uint32_t udRData = 0xffffffff;

    do
    {
        reg_read(SYNC_READ_ADDR, &udRData);
        usleep(5);
    } while (udRData != udId);
    reg_read(SYNC_READ_ADDR+0x8, pudSize);
    reg_read(SYNC_READ_ADDR+0x4, pudAddr);
	debugf("wait_read SYNC_READ_ADDR:0x%x id:0x%x addr:0x%x, size:%u\n", SYNC_READ_ADDR, udId, *pudAddr, *pudSize);

    reg_write(SYNC_READ_ADDR+0xc, 0);
    reg_write(SYNC_READ_ADDR+0x8, 0);
    reg_write(SYNC_READ_ADDR+0x4, 0);
    reg_write(SYNC_READ_ADDR,     0);

    return;
}

void sync_write(uint32_t udId, uint32_t udAddr, uint32_t udSize)
{
    uint32_t udRData = 0xffffffff;

    do
    {
        reg_read(SYNC_WRITE_ADDR, &udRData);
        usleep(5);
    } while (udRData != 0);
    reg_write(SYNC_WRITE_ADDR+0xc, 0);
    reg_write(SYNC_WRITE_ADDR+0x8, udSize);
    reg_write(SYNC_WRITE_ADDR+0x4, udAddr);
    reg_write(SYNC_WRITE_ADDR,     udId);
	debugf("sync_write SYNC_WRITE_ADDR:0x%x id:0x%x addr:0x%x, size:0x%x\n", SYNC_WRITE_ADDR, udId, udAddr, udSize);

    return;
}

void wait_write(uint32_t udId, uint32_t *pudAddr, uint32_t *pudSize)
{
    uint32_t udRData = 0xffffffff;

    do
    {
        reg_read(SYNC_WRITE_ADDR, &udRData);
        usleep(5);
    } while (udRData != udId);
    reg_read(SYNC_WRITE_ADDR+0x8, pudSize);
    reg_read(SYNC_WRITE_ADDR+0x4, pudAddr);
	debugf("wait_write SYNC_WRITE_ADDR:0x%x id:0x%x addr:0x%x, size:0x%x\n", SYNC_WRITE_ADDR, udId, *pudAddr, *pudSize);

    reg_write(SYNC_WRITE_ADDR+0xc, 0);
    reg_write(SYNC_WRITE_ADDR+0x8, 0);
    reg_write(SYNC_WRITE_ADDR+0x4, 0);
    reg_write(SYNC_WRITE_ADDR,     0);

    return;
}

int reg_read(uint32_t udAddr, uint32_t *pRData)
{
    uint32_t udSize = 4;
    size_t   sBytesDone = 0;
    ssize_t  rc;
    char     *pcBuffer = NULL;
    char     *pcAllocated = NULL;
    int      dFpgaFd = open(FROM_DEVICE_NAME_DEFAULT, O_RDWR);
    int      dUnderFlow = 0;
    int      ret = -1;

    if (dFpgaFd < 0) {
        errorf("unable to open from_device %s, %d.\n", FROM_DEVICE_NAME_DEFAULT, dFpgaFd);
        perror("open from_device");
        return -EINVAL;
    }

    ret = posix_memalign((void **)&pcAllocated, 4096 /*alignment */ , udSize + 4096);
    if (ret || !pcAllocated) {
        errorf("OOM %u.\n", udSize + 4096);
        rc = -ENOMEM;
        goto out;
    }
    pcBuffer = pcAllocated;

    rc = read_to_buffer(FROM_DEVICE_NAME_DEFAULT, dFpgaFd, pcBuffer, udSize, udAddr);
    *pRData = *((uint32_t*)pcBuffer);
    if(*pRData != 0 && rc != udSize)
    {
        errorf("rc = read_to_buffer() = %ld, udSize = %u, udAddr = %x rdata = 0x%x\n", rc, udSize, udAddr, *pRData);
    }
    if (rc < 0)
        goto out;
    sBytesDone = rc;

    if (!dUnderFlow && sBytesDone < udSize)
        dUnderFlow = 1;

out:
    close(dFpgaFd);
    free(pcAllocated);

    if (rc < 0)
        return rc;

    /* treat dUnderFlow as error */
    return dUnderFlow ? -EIO : 0;
}

int reg_write(uint32_t udAddr, uint32_t udWData)
{
    uint32_t udSize = 4;
    size_t   sBytesDone = 0;
    ssize_t  rc;
    char     *pcBuffer = NULL;
    char     *pcAllocated = NULL;
    int      dFpgaFd = open(TO_DEVICE_NAME_DEFAULT, O_RDWR);
    int      dUnderFlow = 0;
    int      ret = -1;

    if (dFpgaFd < 0) {
        errorf("unable to open device %s, %d.\n", TO_DEVICE_NAME_DEFAULT, dFpgaFd);
        perror("open device");
        return -EINVAL;
    }

    ret = posix_memalign((void **)&pcAllocated, 4096 /*alignment */ , udSize + 4096);
    if (ret || !pcAllocated) {
        errorf("OOM %u.\n", udSize + 4096);
        rc = -ENOMEM;
        goto out;
    }
    pcBuffer = pcAllocated;

    *((uint32_t*)pcBuffer) = udWData;
    rc = write_from_buffer(TO_DEVICE_NAME_DEFAULT, dFpgaFd, pcBuffer, udSize, udAddr);
    //debugf("rc = write_from_buffer() = %d, udSize = %d, udAddr = %x, udWData = 0x%x\r\n", rc, udSize, udAddr, udWData);
    if (rc < 0)
        goto out;

    sBytesDone = rc;
    if (!dUnderFlow && sBytesDone < udSize)
        dUnderFlow = 1;

out:
    close(dFpgaFd);
    free(pcAllocated);

    if (rc < 0)
        return rc;

    /* treat dUnderFlow as error */
    return dUnderFlow ? -EIO : 0;
}

int write_to_device(uint32_t udAddr, uint32_t udSize, int dInFileFd, uint32_t udBase)
{
    ssize_t rc = -1;
    size_t  sBytesDone   = 0;
    char    *pcBuffer    = NULL;
    char    *pcAllocated = NULL;
    int     dFpgaFd      = open(TO_DEVICE_NAME_DEFAULT, O_RDWR);
    int     dUnderFlow   = 0;
    int     ret = -1;

    if (dInFileFd <= 0) {
        errorf("file fd=%d error.\n", dInFileFd);
        return -1;
    }

    if (dFpgaFd < 0) {
        errorf("unable to open device %s, %d.\n", TO_DEVICE_NAME_DEFAULT, dFpgaFd);
        perror("open device");
        return -EINVAL;
    }

    ret = posix_memalign((void **)&pcAllocated, 4096 /*alignment */ , udSize + 4096);
    if (ret || !pcAllocated) {
        errorf("OOM %u.\n", udSize + 4096);
        rc = -ENOMEM;
        goto out;
    }
    pcBuffer = pcAllocated;

    rc = read_to_buffer("Demo", dInFileFd, pcBuffer, udSize, udBase);
    if (rc < 0 || rc < udSize)
    {
        errorf("write_to_device, rc = read_to_buffer() = %ld, udSize = %u, udAddr = 0x%x\n", rc, udSize, udAddr);
        goto out;
    }

    rc = write_from_buffer(TO_DEVICE_NAME_DEFAULT, dFpgaFd, pcBuffer, udSize, udAddr);
    if (rc < 0)
    {
        errorf("write_to_device, rc = write_from_buffer = %ld, udSize = %u, udAddr = 0x%x\n", rc, udSize, udAddr);
        goto out;
    }

    sBytesDone = rc;
    if (!dUnderFlow && sBytesDone < udSize)
        dUnderFlow = 1;

out:
    close(dFpgaFd);
    free(pcAllocated);

    //if (rc < 0)
        return rc;

    /* treat dUnderFlow as error */
    return dUnderFlow ? -EIO : 0;
}

//int write_to_tmpfile(const char *fname, uint32_t udSize, int dInFileFd, uint32_t udBase)
//{
//    ssize_t rc = -1;
//    size_t  sBytesDone   = 0;
//    char    *pcBuffer    = NULL;
//    char    *pcAllocated = NULL;
//    int     dFpgaFd      = open(fname, O_RDWR);
//    int     dUnderFlow   = 0;
//    int     ret = -1;
//
//    if (dInFileFd <= 0) {
//        errorf("file fd=%d error.\n", dInFileFd);
//        return -1;
//    }
//
//    if (dFpgaFd < 0) {
//        errorf("unable to open device %s, %d.\n", fname, dFpgaFd);
//        perror("open device");
//        return -EINVAL;
//    }
//
//    ret = posix_memalign((void **)&pcAllocated, 4096 /*alignment */ , udSize + 4096);
//    if (ret || !pcAllocated) {
//        errorf("OOM %u.\n", udSize + 4096);
//        rc = -ENOMEM;
//        goto out;
//    }
//    pcBuffer = pcAllocated;
//
//    rc = read_to_buffer(fname, dInFileFd, pcBuffer, udSize, udBase);
//    //debugf("write_to_tmpfile, rc = read_to_buffer() = %ld, udSize = %u, udBase = %x\n", rc, udSize, udBase);
//    if (rc < 0)
//        goto out;
//
//    rc = write_from_buffer(fname, dFpgaFd, pcBuffer, rc, 0);
//    //debugf("write_to_tmpfile, rc = write_from_buffer = %ld, udSize = %u, udBase = %x\n", rc, udSize, udBase);
//    if (rc < 0)
//        goto out;
//
//    sBytesDone = rc;
//    if (!dUnderFlow && sBytesDone < udSize)
//        dUnderFlow = 1;
//
//out:
//    close(dFpgaFd);
//    free(pcAllocated);
//
//    //if (rc < 0)
//        return rc;
//
//    /* treat dUnderFlow as error */
//    return dUnderFlow ? -EIO : 0;
//}

//ssize_t read_to_buffer(const char *fname, int fd, char *buffer, uint64_t size, uint64_t base)
//{
//	ssize_t rc;
//	uint64_t count = 0;
//	char *buf = buffer;
//	off_t offset = base;
//	int loop = 0;
//
//	while (count < size) {
//		ssize_t bytes = size - count;
//
//		if (bytes > RW_MAX_SIZE)
//			bytes = RW_MAX_SIZE;
//
//		if (offset) {
//			rc = lseek(fd, offset, SEEK_SET);
//            //debugf("read_to_buffer: fd = %d, rc = lseek() = 0x%lx, 0ffset = 0x%lx, bytes = 0x%lx\n",
//            //    fd, rc, offset, bytes);
//			if (rc != offset) {
//				errorf("%s, seek off 0x%lx != 0x%lx.\n",
//					fname, rc, offset);
//				perror("seek file");
//				return -EIO;
//			}
//		}
//
//		/* read data from file into memory buffer */
//		rc = read(fd, buf, bytes);
//		if (rc < 0) {
//			errorf("%s, read 0x%lx @ 0x%lx failed %ld.\n",
//				fname, bytes, offset, rc);
//			perror("read file");
//			return -EIO;
//		}
//
//		count += rc;
//		if (rc != bytes) {
//			errorf("%s, read underflow 0x%lx/0x%lx @ 0x%lx.\n",
//				fname, rc, bytes, offset);
//			break;
//		}
//
//		buf += bytes;
//		offset += bytes;
//		loop++;
//	}
//
//	if (count != size && loop)
//		errorf("%s, read underflow 0x%lx/0x%lx.\n",
//			fname, count, size);
//	return count;
//}

ssize_t read_to_buffer(char* fname, int fd, char* buffer, uint64_t size,
    uint64_t base)
{
    ssize_t rc;
    uint64_t count = 0;
    char* buf = buffer;
    off_t offset = base;
    int loop = 0;

    while (count < size) {
        uint64_t bytes = size - count;

        if (bytes > RW_MAX_SIZE)
            bytes = RW_MAX_SIZE;

        if (offset) {
            rc = lseek(fd, offset, SEEK_SET);
            if (rc != offset) {
                fprintf(stderr, "%s, seek off 0x%lx != 0x%lx.\n",
                    fname, rc, offset);
                perror("seek file");
                return -EIO;
            }
        }

        /* read data from file into memory buffer */
        rc = read(fd, buf, bytes);
        if (rc < 0) {
            fprintf(stderr, "%s, read 0x%lx @ 0x%lx failed %ld.\n",
                fname, bytes, offset, rc);
            perror("read file");
            return -EIO;
        }

        count += rc;
        if (rc != bytes) {
            fprintf(stderr, "%s, read underflow 0x%lx/0x%lx @ 0x%lx.\n",
                fname, rc, bytes, offset);
            break;
        }

        buf += bytes;
        offset += bytes;
        loop++;
    }

    if (count != size && loop)
        fprintf(stderr, "%s, read underflow 0x%lx/0x%lx.\n",
            fname, count, size);
    return count;
}

int write_reg32(void* address, unsigned int val, unsigned int offset)
{
    	//usleep(1);
	*((unsigned int*)address + offset) = val;
}
ssize_t write_from_buffer(char* fname, int fd, char* buffer, uint64_t size,
    uint64_t base)
{
    ssize_t rc;
    uint64_t count = 0;
    char* buf = buffer;
    off_t offset = base;
    int loop = 0;
   // printf("addr = 0x%x, size = %d\n", base, size);
    while (count < size) {
        uint64_t bytes = size - count;

        if (bytes > RW_MAX_SIZE)
            bytes = RW_MAX_SIZE;

        if (offset) {
            rc = lseek(fd, offset, SEEK_SET);
            if (rc != offset) {
                fprintf(stderr, "%s, seek off 0x%lx != 0x%lx.\n",
                    fname, rc, offset);
                perror("seek file");
                return -EIO;
            }
        }

        /* write data to file from memory buffer */
        rc = write(fd, buf, bytes);
        if (rc < 0) {
            fprintf(stderr, "%s, write 0x%lx @ 0x%lx failed %ld.\n",
                fname, bytes, offset, rc);
            perror("write file");
            return -EIO;
        }

        count += rc;
        if (rc != bytes) {
            fprintf(stderr, "%s, write underflow 0x%lx/0x%lx @ 0x%lx.\n",
                fname, rc, bytes, offset);
            break;
        }
        buf += bytes;
        offset += bytes;

        loop++;
    }

    if (count != size && loop)
        fprintf(stderr, "%s, write underflow 0x%lx/0x%lx.\n",
            fname, count, size);

    return count;
}


//ssize_t write_from_buffer(const char *fname, int fd, char *buffer, uint64_t size, uint64_t base)
//{
//	ssize_t rc;
//	uint64_t count = 0;
//	char *buf = buffer;
//	off_t offset = base;
//	int loop = 0;
//
//	while (count < size) {
//
//        //debugf("### dma_utils.c write_from_buffer()......\r\n");
//        //debugf("fname =%s, fd= %d, buffer=0x%x, size=%d, base= %x\r\n", fname, fd, buffer, size, base);
//		ssize_t bytes = size - count;
//
//		if (bytes > RW_MAX_SIZE)
//			bytes = RW_MAX_SIZE;
//
//		if (offset) {
//			rc = lseek(fd, offset, SEEK_SET);
//			if (rc != offset) {
//				errorf("%s, seek off 0x%lx != 0x%lx.\n",
//					fname, rc, offset);
//				perror("seek file");
//				return -EIO;
//			}
//		}
//
//		/* write data to file from memory buffer */
//        //debugf("### dma_utils.c -- BEFORE rc = write(fd, buf, bytes)\r\n");
//        //getBufVal = *((int*)buf);
//        //debugf("### getBufVal = 0x%x  ###########################\r\n",getBufVal);
//        //debugf("fd= %d, buf=0x%x, bytes= %d, --> rc = Nodefine\r\n", fd, buf, bytes);
//        //debugf("--------------------------------------------------------------\r\n");
//
//        rc = write(fd, buf, bytes);
//#if 0
//        // check input
//        ///////////////////////////////
//        int tmpfd = open("test.h264", O_CREAT|O_RDWR|O_APPEND, S_IRWXU);
//        if (tmpfd)
//        {
//            write(tmpfd, buf, bytes);
//            syncfs(tmpfd);
//            close(tmpfd);
//        }
//        ///////////////////////////////
//#endif
//        //debugf("### dma_utils.c write(fd, buf, bytes)......\r\n");
//        //debugf("write_from_buffer: fd = %d, buf=0x%lx, bytes= 0x%lx, --> rc = %ld\n",
//        //    fd, (long unsigned int)buf, bytes, rc);
//		if (rc < 0) {
//			errorf("%s, write 0x%lx @ 0x%lx failed %ld.\n",
//				fname, bytes, offset, rc);
//			perror("write file");
//			return -EIO;
//		}
//
//		count += rc;
//		if (rc != bytes) {
//			errorf("%s, write underflow 0x%lx/0x%lx @ 0x%lx.\n",
//				fname, rc, bytes, offset);
//			break;
//		}
//		buf += bytes;
//		offset += bytes;
//
//		loop++;
//	}	
//
//	if (count != size && loop)
//		errorf("%s, write underflow 0x%lx/0x%lx.\n",
//			fname, count, size);
//
//	return count;
//}

int write_yuv420(int fp, int w, int h, int bit_depth_input, int bit_depth_internal, int pingpong)
{
    int ret = 0;
    uint32_t udAddr = 0;
    int dFpgaFd = open(TO_DEVICE_NAME_DEFAULT, O_RDWR);
    if (dFpgaFd < 0) {
        errorf("unable to open device %s, %d.\n", TO_DEVICE_NAME_DEFAULT, dFpgaFd);
        perror("open device");
        return -EINVAL;
    }
    //printf("pingpong: %d\n", pingpong);

    int horizontal_size = w;
    int vertical_size   = h;
    int pic_width  = (horizontal_size + MINI_SIZE - 1) / MINI_SIZE * MINI_SIZE;
    int pic_height = (vertical_size   + MINI_SIZE - 1) / MINI_SIZE * MINI_SIZE;
    int width[3] = {pic_width, pic_width>>1, pic_width>>1};
    int height[3] = {pic_height, pic_height>>1, pic_height>>1};
    int size_yuv[3];
    unsigned char*dst1;
 
    int scale = (bit_depth_input == 10) ? 2 : 1;//////////////////////////////////////

    int bit_shift = bit_depth_internal - bit_depth_input;
    const short minval = 0;
    const short maxval = (1 << bit_depth_internal) - 1;

    short *temp = (short*)malloc(pic_width * 2 * height[0]);
    int num_comp = 3;
    size_yuv[0] = pic_width * pic_height;
    size_yuv[1] = size_yuv[2] = (pic_width >> 1) * (pic_height >> 1);
    for (int comp = 0; comp < num_comp; comp++)
    {
        int padding_w = (pic_width - horizontal_size) >> (comp > 0 ? 1 : 0);
        int padding_h = (height[0] - vertical_size) >> (comp > 0 ? 1 : 0);
        int size_byte = ((horizontal_size * vertical_size) >> (comp > 0 ? 2 : 0)) * scale;///////////////////////
        unsigned char * buf = (unsigned char*)malloc(size_byte);
        if (read(fp, buf, size_byte) != (unsigned)size_byte)
        {
            errorf("Error: read %d.\n", size_byte);
            if(buf)free(buf);
            ret = -1;
            goto out;
        }

        dst1 = (unsigned char*)malloc(width[comp] *(bit_depth_internal == 8 ? 1 : 2)*height[comp]);
         
        if (bit_depth_input == 10)
        {
            for( int y = 0; y < height[comp] - padding_h; y++ )
            {
                int ind_src = y * (width[comp] - padding_w);
                int ind_dst = y * width[comp];
                for( int x = 0; x < width[comp] - padding_w; x++ )
                {
                    int i_dst = ind_dst + x;
                    int i_src = ind_src + x;
                    temp[i_dst] = (buf[i_src * 2] | (buf[i_src * 2 + 1] << 8));

                    if (temp[i_dst] > 1023 || temp[i_dst] < 0)
                    {
                        errorf("Error: input pixel value %d beyond [0, 1023], please check bit-depth %d of the input yuv file.\n", temp[i_dst], bit_depth_input);
                        if(buf)free(buf);
                        ret = -1;
                        goto out;
                    }
                }
                //padding right
                for( int x = width[comp] - padding_w; x < width[comp]; x++ )
                {
                    int i_dst = ind_dst + x;
                    temp[i_dst] = temp[i_dst - 1];
                }
            }
            //padding bottom
            for( int y = height[comp] - padding_h; y < height[comp]; y++ )
            {
                int ind_dst = y * width[comp];
                for( int x = 0; x < width[comp]; x++ )
                {
                    int i_dst = ind_dst + x;
                    temp[i_dst] = temp[i_dst - width[comp]];
                }
            }
        }
        else
        {
            for( int y = 0; y < height[comp] - padding_h; y++ )
            {
                int ind_src = y * (width[comp] - padding_w);
                int ind_dst = y * width[comp];
                for( int x = 0; x < width[comp] - padding_w; x++ )
                {
                    int i_dst = ind_dst + x;
                    int i_src = ind_src + x;
                    temp[i_dst] = buf[i_src];
                }
                //padding right
                for( int x = width[comp] - padding_w; x < width[comp]; x++ )
                {
                    int i_dst = ind_dst + x;
                    temp[i_dst] = temp[i_dst - 1];
                }
            }
            //padding bottom
            for( int y = height[comp] - padding_h; y < height[comp]; y++ )
            {
                int ind_dst = y * width[comp];
                for( int x = 0; x < width[comp]; x++ )
                {
                    int i_dst = ind_dst + x;
                    temp[i_dst] = temp[i_dst - width[comp]];
                }
            }
        }
         
        if (bit_shift == 0){
            for (int i = 0; i < size_yuv[comp]; i++)
            {
                dst1[i] = (unsigned char)(temp[i]);
            }
        }

        if (bit_shift > 0)
        {
            for (int i = 0; i < size_yuv[comp]; i++)
            {
                dst1[i] = temp[i] << bit_shift;
            }
        }
        else if (bit_shift < 0)
        {
            int rounding = 1 << ((-bit_shift) - 1);
            for (int i = 0; i < size_yuv[comp]; i++)
            {
                dst1[i] = IFVCA_CLIP((temp[i] + rounding) >> (-bit_shift), minval, maxval);/////clip the value to the bit depth of internal_bit_depth
            }
        }

#if 0
        static int cnt = 0;
        if (cnt<6 || (cnt/3>40&&cnt/3<46))
        {
            char filename[64] = {0};
            sprintf(filename, "yuv420_%d_%d.dat", cnt/3, comp);
            int tmpfd = open(filename, O_CREAT|O_RDWR|O_TRUNC, S_IRWXU);
            if (tmpfd)
            {
                write(tmpfd, buf, size_byte);
                //syncfs(tmpfd);
                close(tmpfd);
            }
        }
        ++cnt;
#endif
        int base = 64;
        int horizontal = pic_width;
        int vertical = pic_height;
        switch (comp)
        {
        case 0:
            {
                if (pingpong)
                {
                    udAddr = ORI_YUV_Y0;
                }
                else
                {
                    udAddr = ORI_YUV_Y1;
                }
                break;
            }
        case 1:
            {
                base = 32;
                horizontal = pic_width/2;
                vertical = pic_height/2;
                if (pingpong)
                {
                    udAddr = ORI_YUV_U0;
                }
                else
                {
                    udAddr = ORI_YUV_U1;
                }
                break;
            }
        case 2:
            {
                base = 32;
                horizontal = pic_width/2;
                vertical = pic_height/2;
                if (pingpong)
                {
                    udAddr = ORI_YUV_V0;
                }
                else
                {
                    udAddr = ORI_YUV_V1;
                }
                break;
            }
        default:
            {
                debugf("error %d\n", comp);
                break;
            }
        }
        int offset_w = ((horizontal + base - 1) / base) * base;
        int offset_h = ((vertical + base - 1) / base) * base;
        int offset_size = offset_w * offset_h;
        int tmplen = offset_w - horizontal;
        char* ddrbuf = (char*)malloc(offset_size);
        char* tmpdat = (char*)malloc(tmplen);
        memset(ddrbuf, 0, offset_size);
        memset(tmpdat, 0, tmplen);
        for (int i = 0; i < vertical; ++i)
        {
            //wret = fwrite(dst1 + i * horizontal, horizontal, 1, tmpfd);
            memcpy(ddrbuf + i * horizontal+i*tmplen, dst1 + i * horizontal, horizontal);
            //memcpy(tmpdat, dst1 + i * pic_width + 16 * 7, tmplen);
            //wret = fwrite(tmpdat, tmplen, 1, tmpfd);
            memcpy(ddrbuf + (i+1)*horizontal+i*tmplen, tmpdat, tmplen);
        }
#if 0
        static int cntddr = 0;
        if (cntddr<6 || (cntddr/3>40&&cntddr/3<46))
        {
            char filename[64] = {0};
            sprintf(filename, "yuv420ddr_%d_%d.dat", cntddr/3, comp);
            int tmpfd = open(filename, O_CREAT|O_RDWR|O_TRUNC, S_IRWXU);
            if (tmpfd)
            {
                write(tmpfd, ddrbuf, offset_size);
                //syncfs(tmpfd);
                close(tmpfd);
            }
        }
        ++cntddr;
#endif
        ssize_t rc = write_from_buffer(TO_DEVICE_NAME_DEFAULT, dFpgaFd, ddrbuf, offset_size, udAddr);
        //debugf("write_to_device, rc = write_from_buffer = %ld, udSize = %u, udAddr = 0x%x\n", rc, size_byte, udAddr);
        if (rc != offset_size)
        {
            errorf("write_to_device, rc = write_from_buffer = %ld, udSize = %u, udAddr = 0x%x\n", rc, offset_size, udAddr);
        }

        if(buf)free(buf);
        if(ddrbuf)free(ddrbuf);
        if(tmpdat)free(tmpdat);
    }
    
out:
    if(temp)free(temp);
    close(dFpgaFd);

    return ret;
}

int file_type_support(const char *fname)
{
    if (!(strstr(fname, ".avs3") || strstr(fname, ".yuv")
        || strstr(fname, ".AVS3") || strstr(fname, ".YUV")))
    {
        return -1;
    }
    int w = 0, h = 0, fps = 0;
    char suffix[16] = {0};
    int ret = 0;
    if(strchr(fname, '_'))
    {
        ret = sscanf(strchr(fname, '_'), "_%dx%d_%d.%s", &w, &h, &fps, suffix);
    }
    if (ret < 4)
    {
        //debugf("file not support %d\n", ret);
        return -1;
    }
    
    return 0;
}

#endif // ! WIN32
