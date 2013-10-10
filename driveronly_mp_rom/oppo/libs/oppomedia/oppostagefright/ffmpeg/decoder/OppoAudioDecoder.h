/************************************************************
  Copyright (C), 2008-2010, OPPO Mobile Comm Corp., Ltd
  FileName: AudioDecoder.h
  Author: zouf
  Create Date: 2010-11-22
  Description: 
  History: 
      <author>  <time>   <version >   <desc>
      zouf       2010-11-22     1.0     create this moudle  
***********************************************************/

#ifndef __OPPOAUDIODECODER_H__
#define __OPPOAUDIODECODER_H__
#include <media/oppostagefright/MediaBufferGroup.h>

#include "FfmpegDecoder.h"
#include "AudioPostproc.h"

namespace android{

class OppoAudioDecoder : public FfmpegDecoder
{
public:
	OppoAudioDecoder(const sp<MediaSource> &source);
	virtual status_t start(MetaData *params);
	virtual status_t stop();
	virtual status_t read(
			MediaBuffer **buffer, const ReadOptions *options);
	virtual void initDecodeParam();

private:
	int mOutSize;
	MediaBufferGroup *mBufferGroup;
	int32_t mNumChannels, mSampleRate, mBitrate, mBlockAlign,mSamplefmt,mBitSample;
	int64_t mFirstPts;
	AudioConvert *mAudioConvert;
	AudioDownmix *mAudioDownmix;
	
private:
	OppoAudioDecoder(const OppoAudioDecoder &);
	OppoAudioDecoder &operator=(const OppoAudioDecoder &);	
};

}
#endif


