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

#ifndef ID3WRITER_H_

#define ID3WRITER_H_
#include <media/mediametadataretriever.h>

#include <utils/KeyedVector.h>
#include "FileSource.h"
#include "MetadataWriterInterface.h"

namespace android {


class ID3Writer : public MetadataWriterInterface {
    enum Version {
        ID3_UNKNOWN,
        ID3_V1,
        ID3_V1_1,
        ID3_V2_2 = 1<<2,
        ID3_V2_3 = 8,
        ID3_V2_4 = 12,
    };
public:
    ID3Writer(sp<ID3FileSource> &source);
    virtual ~ID3Writer();


	virtual int writeAlbumArt(const char *albumart, size_t length, const char *mime);
	virtual int writeString(int keyCode, const char *value);
	virtual bool flush();

private:    
	sp<ID3FileSource> mFileSource;
    size_t mFirstFrameOffset;
    Version mSrcVer;
	Version mDstcVer;
	char *mAlbumArt;
	size_t mAlbumArtLen;
	char  mMime[16];
	KeyedVector<int, char*> mDataVec;
	bool mFlushed;

    bool parseV1(sp<ID3FileSource> source);
    bool parseV2(sp<ID3FileSource> source, size_t *size);
	bool writeV1(sp<ID3FileSource> source);
    bool writeV2(sp<ID3FileSource> source);
    static bool ParseSyncsafeInteger(const uint8_t encoded[4], size_t *x);

    ID3Writer(const ID3Writer &);
    ID3Writer &operator=(const ID3Writer &);
};

}  // namespace android

#endif  // ID3WRITER_H_

