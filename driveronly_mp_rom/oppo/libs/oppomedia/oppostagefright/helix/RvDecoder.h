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

#ifndef RV_DECODER_H_

#define RV_DECODER_H_

#include <media/oppostagefright/MediaBuffer.h>
#include <media/oppostagefright/MediaSource.h>
#include "Rv34DecoderInterface.h"


namespace android {

struct RvDecoder : public MediaSource,
                        public MediaBufferObserver {
    RvDecoder(const sp<MediaSource> &source);

    virtual status_t start(MetaData *params);
    virtual status_t stop();

    virtual sp<MetaData> getFormat();

    virtual status_t read(
            MediaBuffer **buffer, const ReadOptions *options);

    virtual void signalBufferReturned(MediaBuffer *buffer);

protected:
    virtual ~RvDecoder();

private:
    sp<MediaSource> mSource;
    bool mStarted;
    int32_t mWidth, mHeight;

    sp<MetaData> mFormat;

    RealDecoderInterface *pVideoInterface;
	MMVoid *pVidDecoder;
	DemuxVideoInfo mVideoInfo;
	MediaBuffer *mFrames[2];

    MediaBuffer *mInputBuffer;

    int64_t mNumSamplesOutput;
    int64_t mTargetTimeUs;

    void allocateFrames(int32_t width, int32_t height);
    void releaseFrames();

    RvDecoder(const RvDecoder &);
    RvDecoder &operator=(const RvDecoder &);
};

}  // namespace android

#endif  // RV_DECODER_H_