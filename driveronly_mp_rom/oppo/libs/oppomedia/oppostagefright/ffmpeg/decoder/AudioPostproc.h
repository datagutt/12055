/************************************************************
  Copyright (C), 2012-2013, OPPO Mobile Comm Corp., Ltd
  FileName: AudioPostproc.h
  Author: zys
  Create Date: 2012-04-06
  Description: audio post-processing
  History: 
      <author>  <time>   <version >   <desc>
      zys       2012-04-06     1.0     create this moudle  
***********************************************************/


#ifndef __AUDIOPOSTPROC_H__
#define __AUDIOPOSTPROC_H__
#include <stdint.h>

#ifndef UINT64_C
#define UINT64_C(c)  __UINT64_C(c)
#endif
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavcodec/audioconvert.h"
}

/********************************************************************
                     Format convert
*********************************************************************/
#define SampleFormat AVSampleFormat

class AudioConvert
{
public:
    AudioConvert(enum SampleFormat out_fmt, int8_t out_channels,
                           enum SampleFormat in_fmt, int8_t in_channels);
    virtual ~AudioConvert();
    void Convert(void* buf, int32_t* size);
private:
    AVAudioConvert *mAudioConvert;
    enum SampleFormat mOutFormat;
    enum SampleFormat mInFormat;
    int8_t *mAudioBuffer;
};

/********************************************************************
                     Downmix
*********************************************************************/
typedef void (*DOWNMIX)(int16_t *buf, int32_t buf_size, int32_t len, int8_t channels);

typedef struct {
    enum CodecID  codeId;
    DOWNMIX downmix;
}DownmixInfo;

class AudioDownmix
{
public:
    AudioDownmix(enum CodecID codec_id, enum SampleFormat fmt, int8_t in_channels);
    virtual ~AudioDownmix();
    void Downmix(int16_t *buf, int buf_size, int *size);
private:
    enum CodecID mCodecId;
    enum SampleFormat mFmt;
    int8_t mInChannels;
    int8_t mBytesPerSample;
    DOWNMIX mDownmix;
};

/********************************************************************
                     resample
*********************************************************************/
class AudioResample
{
public:
    AudioResample( enum SampleFormat in_fmt, int output_channels, int input_channels,
                                     int output_rate, int input_rate);
    virtual ~AudioResample();
    int Resample(void* buf, int32_t size, void** output_buf);
private:
    ReSampleContext *mAudioResample;
	enum SampleFormat mInFormat;
	int mInputChannels;
    int8_t *mAudioBuffer;
};

#endif