/************************************************************
  Copyright (C), 2008-2010, OPPO Mobile Comm Corp., Ltd
  FileName: APExtractor.h
  Author: zouf
  Create Date: 2011-12-21
  Description: 
  History: 
      <author>  <time>   <version >   <desc>
      zouf       2011-12-21     1.0     create this moudle  
***********************************************************/


#ifndef APE_EXTRACTOR_H_
#define APE_EXTRACTOR_H_

#include <media/oppostagefright/DataSource.h>
#include <media/oppostagefright/MediaExtractor.h>
#include <utils/String8.h>
#include "Shared/All.h"
#include "MACLib.h"
#include "APETag.h"

namespace android {


class APExtractor : public MediaExtractor {

public:
    // Extractor assumes ownership of source
    APExtractor(const sp<DataSource> &source,  const sp<AMessage> &meta);

    virtual size_t countTracks();
    virtual sp<MediaSource> getTrack(size_t index);
    virtual sp<MetaData> getTrackMetaData(size_t index, uint32_t flags);

    virtual sp<MetaData> getMetaData();

protected:
    virtual ~APExtractor();

private:
    sp<DataSource> mDataSource;
    status_t mInitCheck;
    sp<MetaData> mFileMetadata;

    // There is only one track
    sp<MetaData> mTrackMetadata;

    status_t init(const sp<AMessage> &meta);
	IAPEDecompress *m_spAPEDecompress;

    APExtractor(const APExtractor &);
    APExtractor &operator=(const APExtractor &);

};

bool SniffAPE(const sp<DataSource> &source, String8 *mimeType,
        float *confidence, sp<AMessage> *meta);

}  // namespace android

#endif  // FLAC_EXTRACTOR_H_
