/************************************************************
  Copyright (C), 2008-2010, OPPO Mobile Comm Corp., Ltd
  VENDOR_EDIT
  FileName: ID3Writer.h
  Author: zouf
  Create Date: 2011-10-24
  Description: 
  History: 
      <author>  <time>   <version >   <desc>
      zouf       2011-10-24     1.0     create this moudle  
***********************************************************/

#ifndef METADATAWRITERINTERFACE_H_

#define METADATAWRITERINTERFACE_H_
#include <media/mediametadataretriever.h>

#include <utils/KeyedVector.h>
#include "FileSource.h"

namespace android {


class MetadataWriterInterface {
public:
    MetadataWriterInterface(){}
    virtual ~MetadataWriterInterface(){}


	virtual int writeAlbumArt(const char *albumart, size_t length, const char *mime) = 0;
	virtual int writeString(int keyCode, const char *value) = 0;
	virtual bool flush() = 0;
};

}  // namespace android

#endif  // ID3WRITER_H_

