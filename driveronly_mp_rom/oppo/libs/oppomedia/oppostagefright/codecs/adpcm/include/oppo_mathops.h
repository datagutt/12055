/******************************************************************************
** Copyright (C), 2011-2013, OPPO Mobile Comm Corp., Ltd
** VENDOR_EDIT 
** FileName: oppo_mathops.h
** Author: luodexiang
** Create Date: 2012-6-19
** Description: Math operation.
** Version: 1.0
** ------------------Revision History: ------------------------
** <author>       <time>    <version >    <desc>
** luodexiang 2012-6-19  Revision 2.1   create this moudle  
******************************************************************************/

#ifndef OPPO_CODEC_MATHOPS_H
#define OPPO_CODEC_MATHOPS_H

#include <stdint.h>
#include "oppo_common.h"

#define CHAR_BIT      8         /* number of bits in a char */
#define INT_BIT (CHAR_BIT * sizeof(int))

/* generic implementation */

#ifndef MULL
#   define MULL(a,b,s) (((int64_t)(a) * (int64_t)(b)) >> (s))
#endif

#ifndef MULH
//gcc 3.4 creates an incredibly bloated mess out of this
//#    define MULH(a,b) (((int64_t)(a) * (int64_t)(b))>>32)

static av_always_inline int MULH(int a, int b){
    return ((int64_t)(a) * (int64_t)(b))>>32;
}
#endif

#ifndef UMULH
static av_always_inline unsigned UMULH(unsigned a, unsigned b){
    return ((uint64_t)(a) * (uint64_t)(b))>>32;
}
#endif

#ifndef MUL64
#   define MUL64(a,b) ((int64_t)(a) * (int64_t)(b))
#endif

#ifndef MAC64
#   define MAC64(d, a, b) ((d) += MUL64(a, b))
#endif

#ifndef MLS64
#   define MLS64(d, a, b) ((d) -= MUL64(a, b))
#endif

/* signed 16x16 -> 32 multiply add accumulate */
#ifndef MAC16
#   define MAC16(rt, ra, rb) rt += (ra) * (rb)
#endif

/* signed 16x16 -> 32 multiply */
#ifndef MUL16
#   define MUL16(ra, rb) ((ra) * (rb))
#endif

#ifndef MLS16
#   define MLS16(rt, ra, rb) ((rt) -= (ra) * (rb))
#endif

/* median of 3 */
#ifndef mid_pred
#define mid_pred mid_pred
static inline av_const int mid_pred(int a, int b, int c)
{
#if 0
    int t= (a-b)&((a-b)>>31);
    a-=t;
    b+=t;
    b-= (b-c)&((b-c)>>31);
    b+= (a-b)&((a-b)>>31);

    return b;
#else
    if(a>b){
        if(c>b){
            if(c>a) b=a;
            else    b=c;
        }
    }else{
        if(b>c){
            if(c>a) b=c;
            else    b=a;
        }
    }
    return b;
#endif
}
#endif

#ifndef sign_extend
static inline av_const int sign_extend(int val, unsigned bits)
{
    return (val << (INT_BIT - bits)) >> (INT_BIT - bits);
}
#endif

#ifndef zero_extend
static inline av_const unsigned zero_extend(unsigned val, unsigned bits)
{
    return (val << (INT_BIT - bits)) >> (INT_BIT - bits);
}
#endif

#ifndef COPY3_IF_LT
#define COPY3_IF_LT(x, y, a, b, c, d)\
if ((y) < (x)) {\
    (x) = (y);\
    (a) = (b);\
    (c) = (d);\
}
#endif

#ifndef NEG_SSR32
#   define NEG_SSR32(a,s) ((( int32_t)(a))>>(32-(s)))
#endif

#ifndef NEG_USR32
#   define NEG_USR32(a,s) (((uint32_t)(a))>>(32-(s)))
#endif


#endif /* OPPO_CODEC_MATHOPS_H */

