/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MEDIA_DEFS_H_

#define MEDIA_DEFS_H_

namespace android {

extern const char *MEDIA_MIMETYPE_IMAGE_JPEG;

extern const char *MEDIA_MIMETYPE_VIDEO_VPX;
extern const char *MEDIA_MIMETYPE_VIDEO_AVC;
extern const char *MEDIA_MIMETYPE_VIDEO_MPEG4;
extern const char *MEDIA_MIMETYPE_VIDEO_H263;
extern const char *MEDIA_MIMETYPE_VIDEO_MPEG2;
extern const char *MEDIA_MIMETYPE_VIDEO_RAW;

extern const char *MEDIA_MIMETYPE_AUDIO_AMR_NB;
extern const char *MEDIA_MIMETYPE_AUDIO_AMR_WB;
extern const char *MEDIA_MIMETYPE_AUDIO_MPEG;           // layer III
extern const char *MEDIA_MIMETYPE_AUDIO_MPEG_LAYER_I;
extern const char *MEDIA_MIMETYPE_AUDIO_MPEG_LAYER_II;
extern const char *MEDIA_MIMETYPE_AUDIO_AAC;
extern const char *MEDIA_MIMETYPE_AUDIO_QCELP;
extern const char *MEDIA_MIMETYPE_AUDIO_VORBIS;
extern const char *MEDIA_MIMETYPE_AUDIO_G711_ALAW;
extern const char *MEDIA_MIMETYPE_AUDIO_G711_MLAW;
extern const char *MEDIA_MIMETYPE_AUDIO_RAW;
extern const char *MEDIA_MIMETYPE_AUDIO_FLAC;
extern const char *MEDIA_MIMETYPE_AUDIO_AAC_ADTS;

extern const char *MEDIA_MIMETYPE_CONTAINER_MPEG4;
extern const char *MEDIA_MIMETYPE_CONTAINER_WAV;
extern const char *MEDIA_MIMETYPE_CONTAINER_OGG;
extern const char *MEDIA_MIMETYPE_CONTAINER_MATROSKA;
extern const char *MEDIA_MIMETYPE_CONTAINER_MPEG2TS;
extern const char *MEDIA_MIMETYPE_CONTAINER_AVI;
extern const char *MEDIA_MIMETYPE_CONTAINER_MPEG2PS;

extern const char *MEDIA_MIMETYPE_CONTAINER_WVM;

extern const char *MEDIA_MIMETYPE_TEXT_3GPP;
extern const char *MEDIA_MIMETYPE_TEXT_SUBRIP;

extern const char *MEDIA_MIMETYPE_CONTAINER_RMVB;
extern const char *MEDIA_MIMETYPE_VIDEO_REAL;
extern const char *MEDIA_MIMETYPE_AUDIO_REAL;
extern const char *MEDIA_MIMETYPE_VIDEO_FLV1;
extern const char *MEDIA_MIMETYPE_VIDEO_VP6F;
extern const char *MEDIA_MIMETYPE_VIDEO_RV10;
extern const char *MEDIA_MIMETYPE_VIDEO_RV20;
extern const char *MEDIA_MIMETYPE_VIDEO_RV30;
extern const char *MEDIA_MIMETYPE_VIDEO_RV40;
extern const char *MEDIA_MIMETYPE_AUDIO_COOK;
extern const char *MEDIA_MIMETYPE_AUDIO_RA144;
extern const char *MEDIA_MIMETYPE_AUDIO_RA288;
extern const char *MEDIA_MIMETYPE_VIDEO_WMV;
extern const char *MEDIA_MIMETYPE_VIDEO_WMV1;
extern const char *MEDIA_MIMETYPE_VIDEO_WMV2;
extern const char *MEDIA_MIMETYPE_VIDEO_WMV3;
extern const char *MEDIA_MIMETYPE_VIDEO_VC1;
extern const char *MEDIA_MIMETYPE_AUDIO_WMA1;
extern const char *MEDIA_MIMETYPE_AUDIO_WMA2;
extern const char *MEDIA_MIMETYPE_AUDIO_WMAPRO;
extern const char *MEDIA_MIMETYPE_VIDEO_MPEG1;
extern const char *MEDIA_MIMETYPE_AUDIO_MPEG1;
extern const char *MEDIA_MIMETYPE_AUDIO_MPEG2;
extern const char *MEDIA_MIMETYPE_VIDEO_SVQ1;
extern const char *MEDIA_MIMETYPE_VIDEO_SVQ3;
extern const char *MEDIA_MIMETYPE_AUDIO_QDM2;
extern const char *MEDIA_MIMETYPE_VIDEO_MJPEG;
extern const char *MEDIA_MIMETYPE_VIDEO_MSMPEG4V1;
extern const char *MEDIA_MIMETYPE_VIDEO_MSMPEG4V2;
extern const char *MEDIA_MIMETYPE_VIDEO_MSMPEG4V3;
extern const char *MEDIA_MIMETYPE_AUDIO_PCM_U8;
extern const char *MEDIA_MIMETYPE_AUDIO_ADPCM_IMA_QT;
extern const char *MEDIA_MIMETYPE_AUDIO_ADPCM_IMA_WAV;
extern const char *MEDIA_MIMETYPE_AUDIO_ADPCM_IMA_DK3;
extern const char *MEDIA_MIMETYPE_AUDIO_ADPCM_IMA_DK4;
extern const char *MEDIA_MIMETYPE_AUDIO_ADPCM_IMA_WS;
extern const char *MEDIA_MIMETYPE_AUDIO_ADPCM_IMA_SMJPEG;
extern const char *MEDIA_MIMETYPE_AUDIO_ADPCM_MS;
extern const char *MEDIA_MIMETYPE_AUDIO_ADPCM_4XM;
extern const char *MEDIA_MIMETYPE_AUDIO_ADPCM_XA;
extern const char *MEDIA_MIMETYPE_AUDIO_ADPCM_ADX;
extern const char *MEDIA_MIMETYPE_AUDIO_ADPCM_EA;
extern const char *MEDIA_MIMETYPE_AUDIO_ADPCM_G726;
extern const char *MEDIA_MIMETYPE_AUDIO_ADPCM_CT;
extern const char *MEDIA_MIMETYPE_AUDIO_ADPCM_SWF;
extern const char *MEDIA_MIMETYPE_AUDIO_ADPCM_YAMAHA;
extern const char *MEDIA_MIMETYPE_AUDIO_ADPCM_SBPRO_4;
extern const char *MEDIA_MIMETYPE_AUDIO_ADPCM_SBPRO_3;
extern const char *MEDIA_MIMETYPE_AUDIO_ADPCM_SBPRO_2;
extern const char *MEDIA_MIMETYPE_AUDIO_ADPCM_THP;
extern const char *MEDIA_MIMETYPE_AUDIO_ADPCM_IMA_AMV;
extern const char *MEDIA_MIMETYPE_AUDIO_ADPCM_EA_R1;
extern const char *MEDIA_MIMETYPE_AUDIO_ADPCM_EA_R3;
extern const char *MEDIA_MIMETYPE_AUDIO_ADPCM_EA_R2;
extern const char *MEDIA_MIMETYPE_AUDIO_ADPCM_IMA_EA_SEAD;
extern const char *MEDIA_MIMETYPE_AUDIO_ADPCM_IMA_EA_EACS;
extern const char *MEDIA_MIMETYPE_AUDIO_ADPCM_EA_XAS;
extern const char *MEDIA_MIMETYPE_AUDIO_ADPCM_EA_MAXIS_XA;
extern const char *MEDIA_MIMETYPE_AUDIO_ADPCM_IMA_ISS;
extern const char *MEDIA_MIMETYPE_AUDIO_DTS;
extern const char *MEDIA_MIMETYPE_CONTAINER_FFMPEG;
extern const char *MEDIA_MIMETYPE_CONTAINER_FLV;
extern const char *MEDIA_MIMETYPE_AUDIO_WMA;
extern const char *MEDIA_MIMETYPE_CONTAINER_AAC;
extern const char *MEDIA_MIMETYPE_CONTAINER_QCP;
extern const char *MEDIA_MIMETYPE_CONTAINER_ASF;
extern const char *MEDIA_MIMETYPE_VIDEO_DIVX;
extern const char *MEDIA_MIMETYPE_VIDEO_DIVX311;
extern const char *MEDIA_MIMETYPE_VIDEO_DIVX4;
extern const char *MEDIA_MIMETYPE_AUDIO_AC3;
extern const char *MEDIA_MIMETYPE_AUDIO_EAC3;
extern const char *MEDIA_MIMETYPE_VIDEO_SPARK;
extern const char *MEDIA_MIMETYPE_VIDEO_VP6;
extern const char *MEDIA_MIMETYPE_AUDIO_EVRC;
extern const char *MEDIA_MIMETYPE_AUDIO_APE;
extern const char *MEDIA_MIMETYPE_CONTAINER_MIDI;
extern const char *MEDIA_MIMETYPE_VIDEO_INDEO3;
extern const char *MEDIA_MIMETYPE_AUDIO_PCM_F32BE;
extern const char *MEDIA_MIMETYPE_AUDIO_PCM_F32LE;
extern const char *MEDIA_MIMETYPE_AUDIO_SIPR;
extern const char *MEDIA_MIMETYPE_AUDIO_PCM_F64LE;
extern const char *MEDIA_MIMETYPE_AUDIO_PCM_S24LE;
extern const char *MEDIA_MIMETYPE_AUDIO_PCM_S32LE;
extern const char *MEDIA_MIMETYPE_AUDIO_PCM_ZORK;
extern const char *MEDIA_MIMETYPE_AUDIO_DLPCM;
extern const char *MEDIA_MIMETYPE_AUDIO_TRUEHD;
extern const char *MEDIA_MIMETYPE_AUDIO_WMALOSS;

}  // namespace android

#endif  // MEDIA_DEFS_H_
