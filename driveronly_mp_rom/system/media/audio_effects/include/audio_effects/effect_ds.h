/******************************************************************************
 *  This program is protected under international and U.S. copyright laws as
 *  an unpublished work. This program is confidential and proprietary to the
 *  copyright owners. Reproduction or disclosure, in whole or in part, or the
 *  production of derivative works therefrom without the express permission of
 *  the copyright owners is prohibited.
 *
 *               Copyright (C) 2011-2012 by Dolby Laboratories,
 *                             All rights reserved.
 ******************************************************************************/

#ifndef ANDROID_EFFECT_DS_H_
#define ANDROID_EFFECT_DS_H_

#include <hardware/audio_effect.h>

#if __cplusplus
extern "C" {
#endif

/*
    Type-UUID and UUID for Ds Effect.
    IMPORTANT NOTES: Do not change these numbers without updating their counterparts
    in DsEffect.java
*/
static const effect_uuid_t EFFECT_SL_IID_DS_ =
    { 0x46d279d9, 0x9be7, 0x453d, 0x9d7c, {0xef, 0x93, 0x7f, 0x67, 0x55, 0x87} };
const effect_uuid_t * const EFFECT_SL_IID_DS = &EFFECT_SL_IID_DS_;
static const effect_uuid_t EFFECT_UUID_DS =
    { 0x9d4921da, 0x8225, 0x4f29, 0xaefa, {0x39, 0x53, 0x7a, 0x04, 0xbc, 0xaa} };

/*
    Parameters for the EFFECT_CMD_SET_PARAM command.
    IMPORTANT NOTES: Do not change these DS parameters without updating their counterparts
    in DsEffect.java
*/
typedef enum
{
    DS_PARAM_TUNING = 0,
    DS_PARAM_DEFINE_SETTINGS = 1,
    DS_PARAM_ALL_VALUES = 2,
    DS_PARAM_SINGLE_DEVICE_VALUE = 3,
    DS_PARAM_VISUALIZER_DATA = 4,
    DS_PARAM_DEFINE_PARAMS = 5,
    DS_PARAM_VERSION = 6,
    DS_PARAM_VISUALIZER_ENABLE = 7
} t_ds_params;

#if __cplusplus
}  // extern "C"
#endif

#endif /*ANDROID_EFFECT_DS_H_*/

