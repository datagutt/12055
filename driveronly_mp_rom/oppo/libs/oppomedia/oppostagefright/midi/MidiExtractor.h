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

#ifndef MIDI_EXTRACTOR_H_

#define MIDI_EXTRACTOR_H_

#include <media/oppostagefright/MediaExtractor.h>
#include <utils/Vector.h>
#include <utils/String8.h>
#include <libopposonivox/arm-wt-22k/host_src/eas.h>
#include <libopposonivox/arm-wt-22k/host_src/eas_reverb.h>

namespace android {

struct AMessage;
class DataSource;
class String8;

struct MidiDemux {
			MidiDemux();
			~MidiDemux();
            void            incStrong();
            void            decStrong(); 
            int32_t         getStrongCount();	
			 void           updateState();

			int 			mRefcount;
			int             mTrackCount;
			Mutex 			mLock;
			const S_EAS_LIB_CONFIG* pLibConfig;
			EAS_DATA_HANDLE     mEasData;
    		EAS_HANDLE          mEasHandle;
			EAS_STATE           mState;
    		EAS_FILE            mFileLocator;
};

class MidiExtractor : public MediaExtractor {
public:
    // MidiExtractor assumes ownership of "source".
    MidiExtractor(const sp<DataSource> &source);

    virtual size_t countTracks();
    virtual sp<MediaSource> getTrack(size_t index);
    virtual sp<MetaData> getTrackMetaData(size_t index, uint32_t flags);

    virtual sp<MetaData> getMetaData();

protected:
    virtual ~MidiExtractor();

private:
    struct Track {
        sp<MetaData> meta;
        uint32_t timescale;
        bool includes_expensive_metadata;
    };
	MidiDemux *mMidiDemuxer;
    sp<DataSource> mDataSource;
    status_t mInitCheck;
    bool mHasVideo;

    Track  *mAudioTrack;
	int mTrackCount;

    sp<MetaData> mFileMetaData;
	

    Vector<uint32_t> mPath;
    status_t readMetaData();
    static status_t verifyTrack(Track *track);

    MidiExtractor(const MidiExtractor &);
    MidiExtractor &operator=(const MidiExtractor &);
};

bool SniffMidi(
        const sp<DataSource> &source, String8 *mimeType, float *confidence,
        sp<AMessage> *);

}  // namespace android

#endif  // MIDI_EXTRACTOR_H_