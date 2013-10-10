/*
 * Copyright (C) 2011 OPPO
 *The wave encoder interface.
 *author: maojg
 *date:2011-08-17
 */

#ifndef WAV_WRITER_H_

#define WAV_WRITER_H_

#include <stdio.h>

#include <media/oppostagefright/MediaWriter.h>
#include <utils/threads.h>

namespace android {

struct MediaSource;
struct MetaData;

struct WAVWriter : public MediaWriter {
    WAVWriter(const char *filename);
    WAVWriter(int fd);

    status_t initCheck() const;

    virtual status_t addSource(const sp<MediaSource> &source);
    virtual bool reachedEOS();
    virtual status_t start(MetaData *params = NULL);
    virtual status_t stop();
    virtual status_t pause();
    void writeInt16(int8_t *p,int16_t val);
    void writeInt32(int8_t *p,int32_t val);
    status_t writeCString(char* val, size_t length);
    void writeWavHead(int64_t dataLength);
protected:
    virtual ~WAVWriter();

private:
    FILE *mFile;
    status_t mInitCheck;
    sp<MediaSource> mSource;
    bool mStarted;
    volatile bool mPaused;
    volatile bool mResumed;
    volatile bool mDone;
    volatile bool mReachedEOS;
    pthread_t mThread;
    int64_t mEstimatedSizeBytes;
    int64_t mEstimatedDurationUs;
    int32_t mAudioChannels;
    int32_t mAudioSampleRate;
    int32_t mAudioBitsPerSample;

    // always hold mLock when reading or writing
    thread_id_t     mThread1;
    mutable Mutex   mLock1;
    Condition       mThreadExitedCondition;
    
    static void *ThreadWrapper(void *);
    status_t threadFunc();
    bool exceedsFileSizeLimit();
    bool exceedsFileDurationLimit();

    WAVWriter(const WAVWriter &);
    WAVWriter &operator=(const WAVWriter &);
};

}  // namespace android

#endif  // WAV_WRITER_H_

