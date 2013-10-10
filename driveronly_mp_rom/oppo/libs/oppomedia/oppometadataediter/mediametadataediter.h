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


#ifndef MEDIAMETADATAEDITER_H
#define MEDIAMETADATAEDITER_H

#include <utils/Errors.h>  // for status_t
#include <utils/threads.h>
#include <binder/IMemory.h>
#include "MediaMetadataEditerInterface.h"
#include <media/mediametadataretriever.h>
#include <private/media/VideoFrame.h>

namespace android {

class MediaMetadataEditer: public RefBase
{
public:
    MediaMetadataEditer();
    ~MediaMetadataEditer();
    bool flush();
    status_t setDataSource(const char* dataSourceUrl);
    status_t setDataSource(int fd, int64_t offset, int64_t length);
	bool writeAlbumArt(MediaAlbumArt* albumArt);
	bool writeMetadata(int keyCode, const char* value);
	
private:

    Mutex                                     mLock;
	sp<MediaMetadataEditerInterface> 		  mEditer;

};

}; // namespace android

#endif // MEDIAMETADATAEDITER_H
