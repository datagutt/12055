/************************************************************
  Copyright (C), 2008-2010, OPPO Mobile Comm Corp., Ltd
  VENDOR_EDIT
  FileName: mediametadataediter.h
  Author: zouf
  Create Date: 2011-11-7
  Description: 
  History: 
      <author>  <time>   <version >   <desc>
      zouf       2011-11-7     1.0     create this moudle  
***********************************************************/


#ifndef MEDIAMETADATAEDITER_INTERFACE_H
#define MEDIAMETADATAEDITER_INTERFACE_H

#include <utils/Errors.h>  // for status_t
#include <utils/threads.h>
#include <binder/IMemory.h>
#include <private/media/VideoFrame.h>

namespace android {

class MediaMetadataEditerInterface: public RefBase
{
public:
    MediaMetadataEditerInterface(){}
    virtual ~MediaMetadataEditerInterface(){}
    virtual bool flush() = 0;
    virtual status_t setDataSource(const char* dataSourceUrl) = 0;
    virtual status_t setDataSource(int fd, int64_t offset, int64_t length) = 0;
	virtual bool writeAlbumArt(MediaAlbumArt* albumArt) = 0;
	virtual bool writeMetadata(int keyCode, const char* value) = 0;

};

}; // namespace android

#endif // MEDIAMETADATAEDITER_INTERFACE_H