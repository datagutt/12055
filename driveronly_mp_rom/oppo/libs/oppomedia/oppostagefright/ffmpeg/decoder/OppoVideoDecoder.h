/************************************************************
  Copyright (C), 2008-2010, OPPO Mobile Comm Corp., Ltd
  FileName: VideoDecoder.h
  Author: zouf
  Create Date: 2010-11-22
  Description: 
  History: 
      <author>  <time>   <version >   <desc>
      zouf       2010-11-22     1.0     create this moudle  
***********************************************************/

#ifndef __OPPOVIDEODECODER_H__
#define __OPPOVIDEODECODER_H__

#include "FfmpegDecoder.h"
#include <pthread.h>


namespace android {

class OppoVideoDecoder : public FfmpegDecoder
{
public:
	OppoVideoDecoder(const sp<MediaSource> &source);
	virtual status_t start(MetaData *params);
	virtual status_t stop();
	virtual status_t read(MediaBuffer **buffer, const ReadOptions *options);
	virtual void initDecodeParam();
	
	/*OPPO 2012-1-14 luodexiang Add begin for  */
	virtual void signalBufferReturned(MediaBuffer *buffer);
	/*OPPO 2012-1-14 luodexiang Add end	*/

private:

	void allocateFrames(int32_t width, int32_t height);
	void releaseFrames();

#define OUTPUT_FRAME_NUM 6
	int32_t mWidth, mHeight;
	MediaBuffer *mFrames[OUTPUT_FRAME_NUM];
	AVFrame *mPicture;
	
	bool mFormatChanged;
	MediaBuffer *BackupBuffer;
	int mThreadCount;

private:
	OppoVideoDecoder(const OppoVideoDecoder &);
	OppoVideoDecoder &operator=(const OppoVideoDecoder &);	
};

}
#endif


