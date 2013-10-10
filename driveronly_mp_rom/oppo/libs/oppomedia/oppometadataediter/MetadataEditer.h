/************************************************************
  Copyright (C), 2008-2010, OPPO Mobile Comm Corp., Ltd
  VENDOR_EDIT
  FileName: MetadataWriter.h
  Author: zouf
  Create Date: 2011-10-24
  Description: 
  History: 
      <author>  <time>   <version >   <desc>
      zouf       2011-10-24     1.0     create this moudle  
***********************************************************/
#ifndef METADATA_EDITER_H_
	
#define METADATA_EDITER_H_
	
#include <stdio.h>
#include <utils/Errors.h>
#include <utils/threads.h>
#include "MediaMetadataEditerInterface.h"	
#include <media/stagefright/DataSource.h>
#include <media/stagefright/FileSource.h>


class ID3FileSource;
class MetadataWriterInterface;
namespace android {
	
class MetadataEditer : public MediaMetadataEditerInterface 
{
public:
    MetadataEditer();
    virtual  ~MetadataEditer();

    virtual bool flush();
    virtual status_t setDataSource(const char* dataSourceUrl);
    virtual status_t setDataSource(int fd, int64_t offset, int64_t length);
	virtual bool writeAlbumArt(MediaAlbumArt* albumArt);
	virtual bool writeMetadata(int keyCode, const char* value);
	
private:
	int  parserId3( sp<ID3FileSource> dataSource);
	
    bool probeMp3Writer(sp<ID3FileSource> dataSource);
		
	sp<ID3FileSource>  mFileSource;  	
	MetadataWriterInterface   *mMetaWriter;
};

} // namespace android

#endif  // METADATA_EDITER_H_
