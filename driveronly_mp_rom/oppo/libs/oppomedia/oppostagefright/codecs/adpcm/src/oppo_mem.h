/******************************************************************************
** Copyright (C), 2011-2013, OPPO Mobile Comm Corp., Ltd
** VENDOR_EDIT 
** FileName: oppo_mem.h
** Author: luodexiang
** Create Date: 2012-6-19
** Description: Memory operation interface header file from ffmpeg.
** Version: 1.0
** ------------------Revision History: ------------------------
** <author>       <time>    <version >    <desc>
** luodexiang 2012-6-19  Revision 2.1   create this moudle  
******************************************************************************/

#ifndef OPPO_UTIL_MEM_H
#define OPPO_UTIL_MEM_H

#include "oppo_attributes.h"

#define DECLARE_ALIGNED(n,t,v)      t v
#define DECLARE_ASM_CONST(n,t,v)    static const t v
#define av_malloc_attrib
#define av_alloc_size(n)
/**
 * Allocates a block of size bytes with alignment suitable for all
 * memory accesses (including vectors if available on the CPU).
 * @param size Size in bytes for the memory block to be allocated.
 * @return Pointer to the allocated block, NULL if the block cannot
 * be allocated.
 * @see av_mallocz()
 */
void *av_malloc(unsigned int size) av_malloc_attrib av_alloc_size(1);

/**
 * Allocates or reallocates a block of memory.
 * If ptr is NULL and size > 0, allocates a new block. If
 * size is zero, frees the memory block pointed to by ptr.
 * @param size Size in bytes for the memory block to be allocated or
 * reallocated.
 * @param ptr Pointer to a memory block already allocated with
 * av_malloc(z)() or av_realloc() or NULL.
 * @return Pointer to a newly reallocated block or NULL if the block
 * cannot be reallocated or the function is used to free the memory block.
 * @see av_fast_realloc()
 */
void *av_realloc(void *ptr, unsigned int size) av_alloc_size(2);

/**
 * Frees a memory block which has been allocated with av_malloc(z)() or
 * av_realloc().
 * @param ptr Pointer to the memory block which should be freed.
 * @note ptr = NULL is explicitly allowed.
 * @note It is recommended that you use av_freep() instead.
 * @see av_freep()
 */
void av_free(void *ptr);

/**
 * Allocates a block of size bytes with alignment suitable for all
 * memory accesses (including vectors if available on the CPU) and
 * zeroes all the bytes of the block.
 * @param size Size in bytes for the memory block to be allocated.
 * @return Pointer to the allocated block, NULL if it cannot be allocated.
 * @see av_malloc()
 */
void *av_mallocz(unsigned int size) av_malloc_attrib av_alloc_size(1);

/**
 * Duplicates the string s.
 * @param s string to be duplicated
 * @return Pointer to a newly allocated string containing a
 * copy of s or NULL if the string cannot be allocated.
 */
char *av_strdup(const char *s) av_malloc_attrib;

/**
 * Frees a memory block which has been allocated with av_malloc(z)() or
 * av_realloc() and set the pointer pointing to it to NULL.
 * @param ptr Pointer to the pointer to the memory block which should
 * be freed.
 * @see av_free()
 */
void av_freep(void *ptr);

#endif /* OPPO_UTIL_MEM_H */


