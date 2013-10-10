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

//#define LOG_NDEBUG 0
#define LOG_TAG "HTTPLiveSource"
#include <utils/Log.h>

#include "HTTPLiveSource.h"

#include "ATSParser.h"
#include "AnotherPacketSource.h"
#include "LiveDataSource.h"
#include "LiveSession.h"

#include <media/stagefright/foundation/ABuffer.h>
#include <media/stagefright/foundation/ADebug.h>
#include <media/stagefright/foundation/AMessage.h>
#include <media/stagefright/MediaErrors.h>
#include <media/stagefright/MetaData.h>

namespace android {

NuPlayer::HTTPLiveSource::HTTPLiveSource(
        const char *url,
        const KeyedVector<String8, String8> *headers,
        bool uidValid, uid_t uid)
    : mURL(url),
      mUIDValid(uidValid),
      mUID(uid),
      mFlags(0),
      mFinalResult(OK),
      mOffset(0) {
    if (headers) {
        mExtraHeaders = *headers;

        ssize_t index =
            mExtraHeaders.indexOfKey(String8("x-hide-urls-from-log"));

        if (index >= 0) {
            mFlags |= kFlagIncognito;

            mExtraHeaders.removeItemsAt(index);
        }
    }
}

NuPlayer::HTTPLiveSource::~HTTPLiveSource() {
    ALOGD("~HTTPLiveSource");
    if (mLiveSession != NULL) {
        mLiveSession->disconnect();
        mLiveLooper->stop();
        mLiveLooper->unregisterHandler(mLiveSession->id());
    }
}

void NuPlayer::HTTPLiveSource::start() {
    mLiveLooper = new ALooper;
    mLiveLooper->setName("http live");
    mLiveLooper->start();

    mLiveSession = new LiveSession(
            (mFlags & kFlagIncognito) ? LiveSession::kFlagIncognito : 0,
            mUIDValid, mUID);

    mLiveLooper->registerHandler(mLiveSession);

    mLiveSession->connect(
            mURL.c_str(), mExtraHeaders.isEmpty() ? NULL : &mExtraHeaders);

    mTSParser = new ATSParser;
}

sp<MetaData> NuPlayer::HTTPLiveSource::getFormatMeta(bool audio) {
    ATSParser::SourceType type =
        audio ? ATSParser::AUDIO : ATSParser::VIDEO;

    sp<AnotherPacketSource> source =
        static_cast<AnotherPacketSource *>(mTSParser->getSource(type).get());

    if (source == NULL) {
        return NULL;
    }

    return source->getFormat();
}

#ifndef ANDROID_DEFAULT_CODE
status_t NuPlayer::HTTPLiveSource::getBufferedDuration(bool audio, int64_t *durationUs) {
    status_t err = OK;
    *durationUs = 0;
    ATSParser::SourceType type =
        audio ? ATSParser::AUDIO : ATSParser::VIDEO;

    sp<AnotherPacketSource> source =
        static_cast<AnotherPacketSource *>(mTSParser->getSource(type).get());
    
    if (source == NULL) {
        *durationUs = 0;
        return OK;
    }
    *durationUs = source->getBufferedDurationUs(&err);
    return err;
}
#endif

status_t NuPlayer::HTTPLiveSource::feedMoreTSData() {
    if (mFinalResult != OK) {
        return mFinalResult;
    }

    sp<LiveDataSource> source =
        static_cast<LiveDataSource *>(mLiveSession->getDataSource().get());

    for (int32_t i = 0; i < 50; ++i) {
        char buffer[188];
        ssize_t n = source->readAtNonBlocking(mOffset, buffer, sizeof(buffer));

        if (n == -EWOULDBLOCK) {
            break;
        } else if (n < 0) {
            if (n != ERROR_END_OF_STREAM) {
                ALOGI("input data EOS reached, error %ld", n);
            } else {
                ALOGI("input data EOS reached.");
            }
            mTSParser->signalEOS(n);
            mFinalResult = n;
            break;
        } else {
            if (buffer[0] == 0x00) {
#ifndef ANDROID_DEFAULT_CODE
                handleSpecialBuffer(buffer, 188);
#else
                // XXX legacy
                sp<AMessage> extra;
                mTSParser->signalDiscontinuity(
                        buffer[1] == 0x00
                            ? ATSParser::DISCONTINUITY_SEEK
                            : ATSParser::DISCONTINUITY_FORMATCHANGE,
                        extra);
#endif
            } else {
                status_t err = mTSParser->feedTSPacket(buffer, sizeof(buffer));

                if (err != OK) {
                    ALOGE("TS Parser returned error %d", err);
                    mTSParser->signalEOS(err);
                    mFinalResult = err;
                    break;
                }
            }

            mOffset += n;
        }
    }

    return OK;
}

status_t NuPlayer::HTTPLiveSource::dequeueAccessUnit(
        bool audio, sp<ABuffer> *accessUnit) {
    ATSParser::SourceType type =
        audio ? ATSParser::AUDIO : ATSParser::VIDEO;

    sp<AnotherPacketSource> source =
        static_cast<AnotherPacketSource *>(mTSParser->getSource(type).get());

    if (source == NULL) {
        return -EWOULDBLOCK;
    }

    status_t finalResult;
    if (!source->hasBufferAvailable(&finalResult)) {
        return finalResult == OK ? -EWOULDBLOCK : finalResult;
    }

    return source->dequeueAccessUnit(accessUnit);
}

status_t NuPlayer::HTTPLiveSource::getDuration(int64_t *durationUs) {
    return mLiveSession->getDuration(durationUs);
}
#ifndef ANDROID_DEFAULT_CODE
status_t NuPlayer::HTTPLiveSource::seekTo(int64_t seekTimeUs) {
    // We need to make sure we're not seeking until we have seen the very first
    // PTS timestamp in the whole stream (from the beginning of the stream).
    while (!mTSParser->PTSTimeDeltaEstablished() && feedMoreTSData() == OK) {
        ALOGD("PTSTimeDelta is not established yet, sleeping");
        usleep(100000);
    }

    if (mFinalResult != OK) {
        ALOGW("Error state %d, Ignore this seek", mFinalResult);
        return mFinalResult;
    }

    int64_t newSeekTime;

    mLiveSession->seekTo(seekTimeUs);
    //TODO: if LiveSession seek in the same file or the same format, don't clear format
    mTSParser->signalDiscontinuity(ATSParser::DISCONTINUITY_FLUSH_SOURCE_AND_CLEAR_FORMAT, NULL);
    return OK;
}
#else
status_t NuPlayer::HTTPLiveSource::seekTo(int64_t seekTimeUs) 
{
    // We need to make sure we're not seeking until we have seen the very first
    // PTS timestamp in the whole stream (from the beginning of the stream).
    while (!mTSParser->PTSTimeDeltaEstablished() && feedMoreTSData() == OK) {
        ALOGD("PTSTimeDelta is not established yet, sleeping");
        usleep(100000);
    }

    mLiveSession->seekTo(seekTimeUs);

    return OK;
}
#endif

bool NuPlayer::HTTPLiveSource::isSeekable() {
    return mLiveSession->isSeekable();
}

#ifndef ANDROID_DEFAULT_CODE
void NuPlayer::HTTPLiveSource::handleSpecialBuffer(char* buffer, size_t size) {

        ATSParser::DiscontinuityType type = (buffer[1] == 0x00) ? ATSParser::DISCONTINUITY_SEEK : ATSParser::DISCONTINUITY_FORMATCHANGE; // android default

    //verify if the buffer is defined by MTK
    // 00->07: 00 00 00 00 00 00 00 00    //this reserved for Android default code
    // 08->15: 'm' 't' 'k' '\0' [what] [content_len_in_byte] [content]
    uint8_t* p = (uint8_t*)buffer;
    ssize_t s = size;
    p += 8; s -= 8;
    ALOGD("special buffer, %c %c %c '%d' x%02x x%02x x%02x x%02x"
          " | x%02x x%02x x%02x x%02x",
            p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7],
            p[8], p[9], p[10], p[11]);

    sp<AMessage> extra = NULL;
    if (!strcmp((char*)p, "mtk")) {        
        p += 4; s -= 4;
        LiveDataSource::BufferType buf_type = (LiveDataSource::BufferType)(*p);
        while ((buf_type != LiveDataSource::BUF_END) && s > 0) {
            if (extra == NULL) {
                extra = new AMessage('extr', mLiveSession->id());
            }

            if (buf_type == LiveDataSource::BUF_DO_NOT_DISCONTINUITY) {
                p ++; s --;
                CHECK(*p == 0);
                ALOGD("info only, don't discontinuity");
                type = ATSParser::DISCONTINUITY_NONE;
            } else if (buf_type == LiveDataSource::BUF_FIRST_TIMESTAMP) {
                p ++; s --;
                ALOGD("@debug: timestamp content size = %d", *p);
                CHECK(*p == sizeof(int64_t));
                p ++; s --; 
                int64_t timeUs;
                memcpy(&timeUs, p, sizeof(int64_t));
                ALOGD("current buffer timeUs = %lld", timeUs);
                p += sizeof(int64_t);
                extra->setInt64("hls-FirstTimeUs", timeUs); 
            } else if (buf_type = LiveDataSource::BUF_END) {
                break;
            } else {
                p ++; s --;
                size_t buf_len = *p;
                ALOGD("unknown buffer type = %d (len = %d) ", buf_type, buf_len);
                p += buf_len; s -+ buf_len;
            }
            buf_type = (LiveDataSource::BufferType)(*p);
        }
    }
    CHECK(s > 0);


    mTSParser->signalDiscontinuity(type, extra);
}

#endif

}  // namespace android

