#ifndef _SKY_MM_BASE_TYPES_H
#define _SKY_MM_BASE_TYPES_H

#include <stdlib.h>

#define MMVoid				void

typedef signed char			MMInt8;
typedef unsigned char		MMUInt8;
typedef short int			MMInt16;
typedef unsigned short		MMUInt16;
typedef int					MMInt32;
typedef unsigned int		MMUInt32;
typedef double				MMDouble;

#define MMNULL	0
#ifndef INT_MAX                                 /*loki*/
#define INT_MAX 0x7fffffff
#endif

//#ifndef UINT_MAX
//#define UINT_MAX      0xffffffff
//#endif
    
#define MMAssert(x)			/*my_assert((x)) */      /*loki*/
#define FFMIN(a,b) ((a) > (b) ? (b) : (a))
#define FFMAX(a,b) ((a) > (b) ? (a) : (b))
#define FFABS(a) ((a) >= 0 ? (a) : (-(a)))
#define STRIDE_ALIGN 4
#define INT32_ALIGN(x)		(((x)+3)>>2<<2)



#endif	//_SKY_MM_BASE_TYPES_H