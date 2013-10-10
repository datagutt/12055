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

#ifndef AUDIO_PLAYER_H_

#define AUDIO_PLAYER_H_

#include <media/oppostagefright/MediaPlayerInterface.h>
#include <media/oppostagefright/MediaBuffer.h>
#include <media/oppostagefright/TimeSource.h>
#include <utils/threads.h>
#include "AudioPostproc.h"

namespace android {

class MediaSource;
class AudioTrack;
class AwesomePlayer;

class AudioPlayer : public TimeSource {
public:
    enum {
        REACHED_EOS,
        SEEK_COMPLETE
    };

    AudioPlayer(const sp<AudioSink> &audioSink,
                AwesomePlayer *audioObserver = NULL);

    virtual ~AudioPlayer();

    // Caller retains ownership of "source".
    void setSource(const sp<MediaSource> &source);

    // Return time in us.
    virtual int64_t getRealTimeUs();

    status_t start(bool sourceAlreadyStarted = false);

    void pause(bool playPendingSamples = false);
    void resume();

    // Returns the timestamp of the last buffer played (in us).
    int64_t getMediaTimeUs();

    // Returns true iff a mapping is established, i.e. the AudioPlayer
    // has played at least one frame of audio.
    bool getMediaTimeMapping(int64_t *realtime_us, int64_t *mediatime_us);

    status_t seekTo(int64_t time_us);

    bool isSeeking();
    bool reachedEOS(status_t *finalStatus);

private:
	enum state_type {
		IDLE = 0,
		INIT = 1,
		START = 2,
		RUNNING = 3,
		PAUSE = 4,
		SEEKING = 5,
		STOP = 6,
	};


    sp<MediaSource> mSource;

    MediaBuffer *mInputBuffer;

    int mSampleRate;
    int64_t mLatencyUs;
    size_t mFrameSize;

    Mutex mLock;
    int64_t mNumFramesPlayed;
    int64_t mNumFramesPlayedSysTimeUs;

    int64_t mPositionTimeMediaUs;
    int64_t mPositionTimeRealUs;

    bool mSeeking;
    bool mReachedEOS;
    status_t mFinalStatus;
    int64_t mSeekTimeUs;

    bool mStarted;

    bool mIsFirstBuffer;
    status_t mFirstBufferResult;
    MediaBuffer *mFirstBuffer;

    sp<AudioSink> mAudioSink;

    AwesomePlayer *mObserver;
    int64_t mPinnedTimeUs;

    int64_t getRealTimeUsLocked() const;

    void reset();

    uint32_t getNumFramesPendingPlayout() const;
	

    AudioPlayer(const AudioPlayer &);
    AudioPlayer &operator=(const AudioPlayer &);
	
	void init();
	void uninit();
    static int audioTrackWrapper(void *me);
    void threadentry();

//	List<DataPacket> mQueue;
//	DataPacket *pBufferArray[BUFFER_COUNT];	
	thread_id_t     mAudioTrack_Thread;
	int mState;
	bool mIsExit;
	int32_t mBufferSize;
	Condition mPlayerStart;
	Condition mPlayerEnd;
	AudioResample *mAudioResample;
};

}  // namespace android

#endif  // AUDIO_PLAYER_H_
