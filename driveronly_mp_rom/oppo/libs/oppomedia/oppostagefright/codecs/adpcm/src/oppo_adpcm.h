/******************************************************************************
** Copyright (C), 2011-2013, OPPO Mobile Comm Corp., Ltd
** VENDOR_EDIT 
** FileName: oppo_adpcm.h
** Author: luodexiang
** Create Date: 2012-6-19
** Description: Adpcm header file.
** Version: 1.0
** ------------------Revision History: ------------------------
** <author>       <time>    <version >    <desc>
** luodexiang 2012-6-19  Revision 2.1   create this moudle  
******************************************************************************/

#ifndef OPPO_ADPCM_H
#define OPPO_ADPCM_H

enum CodecID {
    CODEC_ID_NONE,

    /* various ADPCM codecs */
    CODEC_ID_ADPCM_MS,
    CODEC_ID_ADPCM_IMA_WAV,
    CODEC_ID_ADPCM_IMA_DK3,
    CODEC_ID_ADPCM_IMA_DK4,
    CODEC_ID_ADPCM_SWF,    
/* Not Support Now 

    CODEC_ID_ADPCM_IMA_QT,
    CODEC_ID_ADPCM_IMA_WS,
    CODEC_ID_ADPCM_IMA_SMJPEG,
    CODEC_ID_ADPCM_4XM,
    CODEC_ID_ADPCM_XA,
    CODEC_ID_ADPCM_ADX,
    CODEC_ID_ADPCM_EA,
    CODEC_ID_ADPCM_G726,
    CODEC_ID_ADPCM_CT,  
    CODEC_ID_ADPCM_YAMAHA,
    CODEC_ID_ADPCM_SBPRO_4,
    CODEC_ID_ADPCM_SBPRO_3,
    CODEC_ID_ADPCM_SBPRO_2,
    CODEC_ID_ADPCM_THP,
    CODEC_ID_ADPCM_IMA_AMV,
    CODEC_ID_ADPCM_EA_R1,
    CODEC_ID_ADPCM_EA_R3,
    CODEC_ID_ADPCM_EA_R2,
    CODEC_ID_ADPCM_IMA_EA_SEAD,
    CODEC_ID_ADPCM_IMA_EA_EACS,
    CODEC_ID_ADPCM_EA_XAS,
    CODEC_ID_ADPCM_EA_MAXIS_XA,
    CODEC_ID_ADPCM_IMA_ISS,
    CODEC_ID_ADPCM_G722,
    CODEC_ID_ADPCM_IMA_APC,
*/
};

typedef struct AdpcmCodecContext
{
	int channels;
	int frame_size;
	int block_align;
	int sample_rate;
	void *priv_data;
	char name[256];
	enum CodecID codecid;
	
}AdpcmCodecContext;

int adpcm_decode_init(AdpcmCodecContext * avctx);
int adpcm_decode_frame(AdpcmCodecContext *avctx,
                            void *data, int *data_size,
                            uint8_t *src_data,int src_size);
int adpcm_decode_close(AdpcmCodecContext * avctx);

#endif/* OPPO_ADPCM_H */

