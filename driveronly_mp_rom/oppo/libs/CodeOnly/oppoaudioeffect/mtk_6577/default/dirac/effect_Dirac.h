/************************************************************************************
** Copyright (C), 2000-2012, OPPO Mobile Comm Corp., Ltd
** All rights reserved.
** 
** VENDOR_EDIT
** 
** Description: - 
**      Types and constants needed by Dirac effect
** 
** 
** --------------------------- Revision History: --------------------------------
** <author>		                      <data> 	<version >  <desc>
** ------------------------------------------------------------------------------
** guole@oppo.com	                     2011-10-26	  1.0       create file
** guole@oppo.com	                     2011-10-26	  1.1       port it to 11071
** Hongye.Jin@Pdt.Audio.AudioEffect&HAL  2012-06-19   1.2	    port it to 12021
** ------------------------------------------------------------------------------
** 
************************************************************************************/
 
#ifndef ANDROID_EFFECT_DIRAC_API_H_
#define ANDROID_EFFECT_DIRAC_API_H_

#include <hardware/audio_effect.h>

#if __cplusplus
extern "C" {
#endif

static const effect_uuid_t _DIRAC_UUID =
    {0x4c6383e0, 0xff7d, 0x11e0, 0xb6d8, {0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b}};
const effect_uuid_t * const DIRAC_UUID = &_DIRAC_UUID;

#if __cplusplus
}  // extern "C"
#endif
#endif /*ANDROID_EFFECT_VISUALIZER_API_H_*/

