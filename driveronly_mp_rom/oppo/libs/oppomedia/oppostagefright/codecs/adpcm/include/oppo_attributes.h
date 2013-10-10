/******************************************************************************
** Copyright (C), 2011-2013, OPPO Mobile Comm Corp., Ltd
** VENDOR_EDIT 
** FileName: oppo_attributes.h
** Author: luodexiang
** Create Date: 2012-6-19
** Description: Compile time the basic need of a macro definition
** Version: 1.0
** ------------------Revision History: ------------------------
** <author>       <time>    <version >    <desc>
** luodexiang 2012-6-19  Revision 2.1   create this moudle  
******************************************************************************/

#ifndef OPPO_UTIL_ATTRIBUTES_H
#define OPPO_UTIL_ATTRIBUTES_H

#define AV_GCC_VERSION_AT_LEAST(x,y) 0
#define av_always_inline inline
#define av_noinline
#define av_pure
#define av_const
#define av_cold
#define av_flatten
#define attribute_deprecated
#define av_unused
#define av_uninit(x) x
#define av_builtin_constant_p(x) 0

#define HAVE_ATTRIBUTE_PACKED 1

#define UINT32_MAX  0xffffffff

#endif /* OPPO_UTIL_ATTRIBUTES_H */

