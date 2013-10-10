/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef HELIX_EXTRACTOR_H_

#define HELIX_EXTRACTOR_H_

#include <media/oppostagefright/MediaExtractor.h>
#include <utils/Vector.h>
#include <utils/String8.h>
#include "RealDemuxerInterface.h"

namespace android {

struct AMessage;
class DataSource;
class String8;

struct HelixDemux {
			HelixDemux();
			~HelixDemux();
            void            incStrong();
            void            decStrong(); 
            int32_t         getStrongCount();	

			int 			mRefcount;
			RealDemuxerInterface * pDemuxInterface;
			MMVoid * pDemuxer;
			int             mTrackCount;
			Mutex mLock;
};

class HelixExtractor : public MediaExtractor {
public:
    // HelixExtractor assumes ownership of "source".
    HelixExtractor(const sp<DataSource> &source);

    virtual size_t countTracks();
    virtual sp<MediaSource> getTrack(size_t index);
    virtual sp<MetaData> getTrackMetaData(size_t index, uint32_t flags);

    virtual sp<MetaData> getMetaData();
	virtual void    dropFrame(int level = 0);
protected:
    virtual ~HelixExtractor();

private:
    struct Track {
        sp<MetaData> meta;
        uint32_t timescale;
        bool includes_expensive_metadata;
    };

	HelixDemux *mHelixDemuxer;
    sp<DataSource> mDataSource;
    status_t mInitCheck;
    bool mHasVideo;

    Track *mVideoTrack, *mAudioTrack;
	int mTrackCount;

    sp<MetaData> mFileMetaData;
	int64_t  mThumbnailTimeUs;

    Vector<uint32_t> mPath;
    status_t readMetaData();
    static status_t verifyTrack(Track *track);

    HelixExtractor(const HelixExtractor &);
    HelixExtractor &operator=(const HelixExtractor &);
};

bool SniffHelix(
        const sp<DataSource> &source, String8 *mimeType, float *confidence,
        sp<AMessage> *);

}  // namespace android

#endif  // HELIX_EXTRACTOR_H_