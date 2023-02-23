#ifndef _FW_REG_DEF_H_
#define _FW_REG_DEF_H_

#define FW_DEBUG 0

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int   uint32;
 
extern uint32 fw_read_reg(uint32 regaddr);
extern void fw_write_reg(uint32 regaddr, uint32 val);

#define _F_MASK(ss,bw)              (((1UL<<(bw))-1)<<(ss))
#define _F_NORMALIZE(v,ss,bw)       (((v)&_F_MASK((ss),(bw)))>>(ss))
#define _F_DENORMALIZE(v,ss,bw)     (((v)<<(ss))&_F_MASK((ss),(bw)))
#define REG_FIELD_GET(v,ss,bw)      (_F_NORMALIZE((v),(ss),(bw)))
#define REG_FIELD_SET(rr,v,ss,bw)   (((rr)&~_F_MASK((ss),(bw)))|_F_DENORMALIZE((v),(ss),(bw)))

#define VID_ENC_BASE_ADDR  0x00000000

#endif

