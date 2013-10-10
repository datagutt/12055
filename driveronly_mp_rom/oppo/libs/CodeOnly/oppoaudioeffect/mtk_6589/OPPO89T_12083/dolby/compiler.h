/******************************************************************************
TDAS Implementation Kit

  Confidential Information - Limited distribution to authorized persons only.
  This material is protected under international copyright laws as an 
  unpublished work. Do not copy. Copyright (C) 2002-2006 Audistry LLC, Dolby
  Laboratories Inc. All rights reserved.

	File:           compiler.h
	Contributors:	Quang Thai
	Description:    C reference for DSPs

******************************************************************************/

#ifndef COMPILER_H
#define COMPILER_H 1

#include <stdlib.h>					/* for malloc, etc */
#include <string.h>					/* for memset, etc */
#include <assert.h>

#define __RVCT__

#define INT16_TYPE short
#define INT32_TYPE int 
#define INT64_TYPE long long 
#define UINT16_TYPE unsigned short 
#define UINT32_TYPE unsigned int
#define UINT64_TYPE unsigned long long
#define FLOAT_TYPE float
#define DOUBLE_TYPE double 

#define __max(a,b)    (((a) > (b)) ? (a) : (b))
#define __min(a,b)    (((a) < (b)) ? (a) : (b))

/* Platform word alignment boundary */
#define ALIGN_BYTES sizeof(int)
/* SIZE_ALIGN rounds upwards to an ALIGN_BYTES sized alignment boundary */
#define SIZE_ALIGN(a) \
    ((a + ALIGN_BYTES-1)/ALIGN_BYTES)*ALIGN_BYTES

#define ALIGN  __attribute__ ((aligned (4)))

/* 
 * This macro ensures that sizeof() returns the exact size when less than that of 
 * an int (4), but otherwise, rounds up to the next word-width (4) multiple.
 */
#define sizeof(X) (((sizeof(X)) > (sizeof(int))) ?	(((sizeof(X)+3)>>2)<<2) : (sizeof(X)) )


#endif /* COMPILER_H */
