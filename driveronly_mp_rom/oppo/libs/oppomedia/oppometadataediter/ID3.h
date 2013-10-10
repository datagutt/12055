/************************************************************
  Copyright (C), 2008-2010, OPPO Mobile Comm Corp., Ltd
  VENDOR_EDIT
  FileName: ID3.h
  Author: zouf
  Create Date: 2011-10-24
  Description: 
  History: 
      <author>  <time>   <version >   <desc>
      zouf       2011-10-24     1.0     create this moudle  
***********************************************************/

#ifndef ID3_H_

#define ID3_H_

#include <utils/RefBase.h>

namespace android {

struct String8;

struct ID3 {
    enum Version {
        ID3_UNKNOWN,
        ID3_V1,
        ID3_V1_1,
        ID3_V2_2,
        ID3_V2_3,
        ID3_V2_4,
    };

    ID3(const sp<ID3FileSource> &source);
    ~ID3();

    bool isValid() const;

    Version version() const;

    const void *getAlbumArt(size_t *length, String8 *mime) const;
	bool reLoad(int version = ID3_V1);

    struct Iterator {
        Iterator(const ID3 &parent, const char *id);
        ~Iterator();

        bool done() const;
        void getID(String8 *id) const;
        void getString(String8 *s) const;
        const uint8_t *getData(size_t *length) const;
        void next();

    private:
        const ID3 &mParent;
        char *mID;
        size_t mOffset;

        const uint8_t *mFrameData;
        size_t mFrameSize;

        void findFrame();

        size_t getHeaderLength() const;

        Iterator(const Iterator &);
        Iterator &operator=(const Iterator &);
    };

private:
    bool mIsValid;
    uint8_t *mData;
    size_t mSize;
    size_t mFirstFrameOffset;
    Version mVersion;
	sp<ID3FileSource> mFileSource;

    bool parseV1(const sp<ID3FileSource> &source);
    bool parseV2(const sp<ID3FileSource> &source);
    void removeUnsynchronization();
    bool removeUnsynchronizationV2_4();

    static bool ParseSyncsafeInteger(const uint8_t encoded[4], size_t *x);

    ID3(const ID3 &);
    ID3 &operator=(const ID3 &);
};

}  // namespace android

#endif  // ID3_H_

