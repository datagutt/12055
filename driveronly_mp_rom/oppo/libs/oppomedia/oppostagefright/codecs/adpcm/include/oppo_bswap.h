/******************************************************************************
** Copyright (C), 2011-2013, OPPO Mobile Comm Corp., Ltd
** VENDOR_EDIT 
** FileName: oppo_bswap.h
** Author: luodexiang
** Create Date: 2012-6-19
** Description: Exchange of byte locations basic interface (small end storage mode).
** Version: 1.0
** ------------------Revision History: ------------------------
** <author>       <time>    <version >    <desc>
** luodexiang 2012-6-19  Revision 2.1   create this moudle  
******************************************************************************/

#ifndef OPPO_UTIL_BSWAP_H
#define OPPO_UTIL_BSWAP_H

#include <stdint.h>
#include "oppo_attributes.h"


#define bswap_32 bswap_32
static av_always_inline av_const uint32_t bswap_32(uint32_t x)
{
    uint32_t t;
    __asm__ ("eor %1, %0, %0, ror #16 \n\t"
             "bic %1, %1, #0xFF0000   \n\t"
             "mov %0, %0, ror #8      \n\t"
             "eor %0, %0, %1, lsr #8  \n\t"
             : "+r"(x), "=&r"(t));
             
    return x;
}

#ifndef bswap_16
static av_always_inline av_const uint16_t bswap_16(uint16_t x){
    x= (x>>8) | (x<<8);
    return x;
}
#endif

#ifndef bswap_32
static av_always_inline av_const uint32_t bswap_32(uint32_t x){
    x= ((x<<8)&0xFF00FF00) | ((x>>8)&0x00FF00FF);
    x= (x>>16) | (x<<16);
    return x;
}
#endif

#ifndef bswap_64
static inline uint64_t av_const bswap_64(uint64_t x){
#if 0
    x= ((x<< 8)&0xFF00FF00FF00FF00ULL) | ((x>> 8)&0x00FF00FF00FF00FFULL);
    x= ((x<<16)&0xFFFF0000FFFF0000ULL) | ((x>>16)&0x0000FFFF0000FFFFULL);
    return (x>>32) | (x<<32);
#else
    union {
        uint64_t ll;
        uint32_t l[2];
    } w, r;
    w.ll = x;
    r.l[0] = bswap_32 (w.l[1]);
    r.l[1] = bswap_32 (w.l[0]);
    return r.ll;
#endif
}
#endif


#define be2me_16(x) bswap_16(x)
#define be2me_32(x) bswap_32(x)
#define be2me_64(x) bswap_64(x)
#define le2me_16(x) (x)
#define le2me_32(x) (x)
#define le2me_64(x) (x)

#endif /* OPPO_UTIL_BSWAP_H */

