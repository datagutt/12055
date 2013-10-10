/************************************************************
  Copyright (C), 2008-2010, OPPO Mobile Comm Corp., Ltd
  FileName: FfmpegDecoder.h
  Author: zouf
  Create Date: 2010-11-9
  Description: 
  History: 
      <author>  <time>   <version >   <desc>
      zouf       2010-11-9     1.0     create this moudle  
***********************************************************/

#ifndef __FFMPEG_DECODER_H__

#define __FFMPEG_DECODER_H__

#include <OMX_Component.h>

#include <media/oppostagefright/MediaBuffer.h>
#include <media/oppostagefright/MediaSource.h>
//#include <media/oppostagefright/MediaDebug.h>
#include <media/oppostagefright/foundation/ADebug.h>
#include <media/oppostagefright/MediaDefs.h>
#include <media/oppostagefright/MediaErrors.h>
#include <media/oppostagefright/MetaData.h>
#include <media/oppostagefright/Utils.h>

#include <utils/threads.h>
#include <utils/List.h>

#include <stdint.h>

#ifndef UINT64_C
#define UINT64_C(c)  __UINT64_C(c)
#endif

extern "C"
{
#include "libavformat/avformat.h"
}

#define OUTPUT 0

namespace android 
{


struct FfmpegDecoder : public MediaSource,
						public MediaBufferObserver 
{
	FfmpegDecoder(const sp<MediaSource> &source);

	virtual status_t start(MetaData *params);
	virtual status_t stop();
	virtual sp<MetaData> getFormat();
	virtual status_t read(MediaBuffer **buffer, const ReadOptions *options)= 0;

	virtual void signalBufferReturned(MediaBuffer *buffer);

	virtual void initDecodeParam() = 0;

protected:
	bool inited;	
	virtual bool CheckInit();

	sp<MediaSource> mSource;
	bool mStarted;

	sp<MetaData> mFormat;
	AVCodec *mCodec;
	AVCodecContext *mCodecContex;

	MediaBuffer *mInputBuffer;

	int64_t mNumSamplesOutput;
	
	virtual ~FfmpegDecoder();
	AVFrame *mFrame;

private:

	FfmpegDecoder(const FfmpegDecoder &);
	FfmpegDecoder &operator=(const FfmpegDecoder &);
};

}  // namespace android

#endif  

