/*
 * Copyright (C) 2010 The Android Open Source Project
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

#ifndef MP3_ENCODER_H
#define MP3_ENCODER_H

#include <media/oppostagefright/MediaSource.h>
#include <media/oppostagefright/MetaData.h>

struct lame_global_struct;

namespace android {

struct MediaBufferGroup;

class MP3Encoder: public MediaSource {
    public:
        MP3Encoder(const sp<MediaSource> &source, const sp<MetaData> &meta);

        virtual status_t start(MetaData *params);
        virtual status_t stop();
        virtual sp<MetaData> getFormat();
        virtual status_t read(
                MediaBuffer **buffer, const ReadOptions *options);


    protected:
        virtual ~MP3Encoder();

    private:
        sp<MediaSource>   mSource;
        sp<MetaData>      mMeta;
        bool              mStarted;
        MediaBufferGroup *mBufferGroup;
        MediaBuffer      *mInputBuffer;
        status_t          mInitCheck;
        int32_t           mSampleRate;
        int32_t           mChannels;
        int32_t           mBitRate;
        int32_t           mFrameCount;

        int64_t           mAnchorTimeUs;
        int64_t           mNumInputSamples;
		lame_global_struct *mLame;
        int16_t           *mInputFrame;

        void             *mEncoderHandle;
        status_t 		 initCheck();

        MP3Encoder& operator=(const MP3Encoder &rhs);
        MP3Encoder(const MP3Encoder& copy);

};

}

#endif  //#ifndef MP3_ENCODER_H

