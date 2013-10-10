/*
 * Copyright (C) 2007 The Android Open Source Project
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

#ifndef ANDROID_MEDIAPLAYERINTERFACE_H
#define ANDROID_MEDIAPLAYERINTERFACE_H

#ifdef __cplusplus

#include <sys/types.h>
#include <utils/Errors.h>
#include <utils/KeyedVector.h>
#include <utils/String8.h>
#include <utils/RefBase.h>

#include <media/AudioSystem.h>
#include <media/oppostagefright/AudioPlayerEvent.h>

//#include <media/Metadata.h>

// Fwd decl to make sure everyone agrees that the scope of struct sockaddr_in is
// global, and not in android::
struct sockaddr_in;

namespace android {

class Surface;
class ISurfaceTexture;

template<typename T> class SortedVector;


enum player_type {
    PV_PLAYER = 1,
    SONIVOX_PLAYER = 2,
    STAGEFRIGHT_PLAYER = 3,
    NU_PLAYER = 4,
    // Test players are available only in the 'test' and 'eng' builds.
    // The shared library with the test player is passed passed as an
    // argument to the 'test:' url in the setDataSource call.
    TEST_PLAYER = 5,

    AAH_RX_PLAYER = 100,
    AAH_TX_PLAYER = 101,
    OPPO_STAGEFRIGHT_PLAYER = 102,
};

#define DEFAULT_AUDIOSINK_BUFFERCOUNT 4
#define DEFAULT_AUDIOSINK_BUFFERSIZE 1200
#define DEFAULT_AUDIOSINK_SAMPLERATE 44100

// when the channel mask isn't known, use the channel count to derive a mask in AudioSink::open()
#define CHANNEL_MASK_USE_CHANNEL_ORDER 0

// duration below which we do not allow deep audio buffering
#define AUDIO_SINK_MIN_DEEP_BUFFER_DURATION_US 5000000


enum media_event_type {
    MEDIA_NOP               = 0, // interface test message
    MEDIA_PREPARED          = 1,
    MEDIA_PLAYBACK_COMPLETE = 2,
    MEDIA_BUFFERING_UPDATE  = 3,
    MEDIA_SEEK_COMPLETE     = 4,
    MEDIA_SET_VIDEO_SIZE    = 5,
    MEDIA_TIMED_TEXT        = 99,
    MEDIA_ERROR             = 100,
    MEDIA_INFO              = 200,
};

// Generic error codes for the media player framework.  Errors are fatal, the
// playback must abort.
//
// Errors are communicated back to the client using the
// MediaPlayerListener::notify method defined below.
// In this situation, 'notify' is invoked with the following:
//   'msg' is set to MEDIA_ERROR.
//   'ext1' should be a value from the enum media_error_type.
//   'ext2' contains an implementation dependant error code to provide
//          more details. Should default to 0 when not used.
//
// The codes are distributed as follow:
//   0xx: Reserved
//   1xx: Android Player errors. Something went wrong inside the MediaPlayer.
//   2xx: Media errors (e.g Codec not supported). There is a problem with the
//        media itself.
//   3xx: Runtime errors. Some extraordinary condition arose making the playback
//        impossible.
//
enum media_error_type {
    // 0xx
    MEDIA_ERROR_UNKNOWN = 1,
    // 1xx
    MEDIA_ERROR_SERVER_DIED = 100,
    // 2xx
    MEDIA_ERROR_NOT_VALID_FOR_PROGRESSIVE_PLAYBACK = 200,
    // 3xx
};


// Info and warning codes for the media player framework.  These are non fatal,
// the playback is going on but there might be some user visible issues.
//
// Info and warning messages are communicated back to the client using the
// MediaPlayerListener::notify method defined below.  In this situation,
// 'notify' is invoked with the following:
//   'msg' is set to MEDIA_INFO.
//   'ext1' should be a value from the enum media_info_type.
//   'ext2' contains an implementation dependant info code to provide
//          more details. Should default to 0 when not used.
//
// The codes are distributed as follow:
//   0xx: Reserved
//   7xx: Android Player info/warning (e.g player lagging behind.)
//   8xx: Media info/warning (e.g media badly interleaved.)
//
enum media_info_type {
    // 0xx
    MEDIA_INFO_UNKNOWN = 1,
    // The player was started because it was used as the next player for another
    // player, which just completed playback
    MEDIA_INFO_STARTED_AS_NEXT = 2,
    // 7xx
    // The video is too complex for the decoder: it can't decode frames fast
    // enough. Possibly only the audio plays fine at this stage.
    MEDIA_INFO_VIDEO_TRACK_LAGGING = 700,
    // MediaPlayer is temporarily pausing playback internally in order to
    // buffer more data.
    MEDIA_INFO_BUFFERING_START = 701,
    // MediaPlayer is resuming playback after filling buffers.
    MEDIA_INFO_BUFFERING_END = 702,
    // Bandwidth in recent past
    MEDIA_INFO_NETWORK_BANDWIDTH = 703,

    // 8xx
    // Bad interleaving means that a media has been improperly interleaved or not
    // interleaved at all, e.g has all the video samples first then all the audio
    // ones. Video is playing but a lot of disk seek may be happening.
    MEDIA_INFO_BAD_INTERLEAVING = 800,
    // The media is not seekable (e.g live stream).
    MEDIA_INFO_NOT_SEEKABLE = 801,
    // New media metadata is available.
    MEDIA_INFO_METADATA_UPDATE = 802,

    //9xx
    MEDIA_INFO_TIMED_TEXT_ERROR = 900,
};



enum media_player_states {
    MEDIA_PLAYER_STATE_ERROR        = 0,
    MEDIA_PLAYER_IDLE               = 1 << 0,
    MEDIA_PLAYER_INITIALIZED        = 1 << 1,
    MEDIA_PLAYER_PREPARING          = 1 << 2,
    MEDIA_PLAYER_PREPARED           = 1 << 3,
    MEDIA_PLAYER_STARTED            = 1 << 4,
    MEDIA_PLAYER_PAUSED             = 1 << 5,
    MEDIA_PLAYER_STOPPED            = 1 << 6,
    MEDIA_PLAYER_PLAYBACK_COMPLETE  = 1 << 7
};

// Keep KEY_PARAMETER_* in sync with MediaPlayer.java.
// The same enum space is used for both set and get, in case there are future keys that
// can be both set and get.  But as of now, all parameters are either set only or get only.
enum media_parameter_keys {
    // Streaming/buffering parameters
    KEY_PARAMETER_CACHE_STAT_COLLECT_FREQ_MS = 1100,            // set only

    // Return a Parcel containing a single int, which is the channel count of the
    // audio track, or zero for error (e.g. no audio track) or unknown.
    KEY_PARAMETER_AUDIO_CHANNEL_COUNT = 1200,                   // get only

    // Playback rate expressed in permille (1000 is normal speed), saved as int32_t, with negative
    // values used for rewinding or reverse playback.
    KEY_PARAMETER_PLAYBACK_RATE_PERMILLE = 1300,                // set only
};

// Keep INVOKE_ID_* in sync with MediaPlayer.java.
enum media_player_invoke_ids {
    INVOKE_ID_GET_TRACK_INFO = 1,
    INVOKE_ID_ADD_EXTERNAL_SOURCE = 2,
    INVOKE_ID_ADD_EXTERNAL_SOURCE_FD = 3,
    INVOKE_ID_SELECT_TRACK = 4,
    INVOKE_ID_UNSELECT_TRACK = 5,
    INVOKE_ID_SET_VIDEO_SCALING_MODE = 6,
};

// Keep MEDIA_TRACK_TYPE_* in sync with MediaPlayer.java.
enum media_track_type {
    MEDIA_TRACK_TYPE_UNKNOWN = 0,
    MEDIA_TRACK_TYPE_VIDEO = 1,
    MEDIA_TRACK_TYPE_AUDIO = 2,
    MEDIA_TRACK_TYPE_TIMEDTEXT = 3,
};

// ----------------------------------------------------------------------------

// callback mechanism for passing messages to MediaPlayer object
typedef void (*notify_callback_f)(void* cookie,
        int msg, int ext1, int ext2);


// ref-counted object for callbacks
class MediaPlayerListener : virtual public RefBase
{
public:
    virtual void notify(int msg, int ext1, int ext2) = 0;
	virtual int audionotify(int msg,DataPacket *dataobj,AudioParameters *parameters) = 0;
};


// AudioSink: abstraction layer for audio output
class AudioSink : public RefBase {
public:
    virtual             ~AudioSink() {}
    virtual bool        ready() const = 0; // audio output is open and ready
    virtual bool        realtime() const = 0; // audio output is real-time output
    virtual ssize_t     bufferSize() const = 0;
    virtual ssize_t     frameCount() const = 0;
    virtual ssize_t     channelCount() const = 0;
    virtual ssize_t     frameSize() const = 0;
    virtual uint32_t    latency() = 0;
    virtual status_t    getPosition(uint32_t *position) const = 0;
    virtual status_t    getFramesWritten(uint32_t *frameswritten) const = 0;
    virtual int         getSessionId() const = 0;

	virtual status_t open(uint32_t sampleRate, int channelCount, 
							audio_channel_mask_t channelMask,
							audio_format_t format, int bufferCount,int32_t *buffersize) = 0;

    virtual void        start() = 0;
	 virtual ssize_t     write(DataPacket &packet) = 0;

    virtual void        stop() = 0;
    virtual void        flush() = 0;
    virtual void        pause() = 0;
    virtual void        close() = 0;

    virtual status_t    setPlaybackRatePermille(int32_t rate) { return INVALID_OPERATION; }
    virtual bool        needsTrailingPadding() { return true; }
};

class MediaPlayerBase : public RefBase
{
public:

                        MediaPlayerBase() : mCookie(0), mNotify(0) {}
    virtual             ~MediaPlayerBase() {}
    virtual status_t    initCheck() = 0;
    virtual bool        hardwareOutput() = 0;

    virtual status_t    setUID(uid_t uid) {
        return INVALID_OPERATION;
    }

    virtual status_t    setDataSource(
            const char *url,
            const KeyedVector<String8, String8> *headers = NULL) = 0;

    virtual status_t    setDataSource(int fd, int64_t offset, int64_t length) = 0;

	virtual status_t 	invoke(const Parcel& request, Parcel *reply) = 0;
    // pass the buffered ISurfaceTexture to the media player service
    virtual status_t    setVideoSurfaceTexture(
                                const sp<ISurfaceTexture>& surfaceTexture) = 0;
	virtual status_t 	setSurface(const sp<Surface> &surface) = 0;

    virtual status_t    prepare() = 0;
    virtual status_t    prepareAsync() = 0;
    virtual status_t    start() = 0;
    virtual status_t    stop() = 0;
    virtual status_t    pause() = 0;
    virtual bool        isPlaying() = 0;
    virtual status_t    seekTo(int msec) = 0;
    virtual status_t    getCurrentPosition(int *msec) = 0;
    virtual status_t    getDuration(int *msec) = 0;
    virtual status_t    reset() = 0;
    virtual status_t    setLooping(int loop) = 0;
    virtual player_type playerType() = 0;
    virtual status_t    setParameter(int key, const char* param) = 0;
    virtual char*    	getParameter(int key) = 0;

    // Right now, only the AAX TX player supports this functionality.  For now,
    // provide a default implementation which indicates a lack of support for
    // this functionality to make life easier for all of the other media player
    // maintainers out there.
    virtual status_t setRetransmitEndpoint(const struct sockaddr_in* endpoint) {
        return INVALID_OPERATION;
    }

   

    void        setNotifyCallback(
            void* cookie, notify_callback_f notifyFunc) {
        Mutex::Autolock autoLock(mNotifyLock);
        mCookie = cookie; mNotify = notifyFunc;
    }

    void        sendEvent(int msg, int ext1=0, int ext2=0) {
        Mutex::Autolock autoLock(mNotifyLock);
        if (mNotify) mNotify(mCookie, msg, ext1, ext2);
    }

    virtual status_t dump(int fd, const Vector<String16> &args) const {
        return INVALID_OPERATION;
    }

private:

    Mutex               mNotifyLock;
    void*               mCookie;
    notify_callback_f   mNotify;
};

// Implement this class for media players that use the AudioFlinger software mixer
class MediaPlayerInterface : public MediaPlayerBase
{
public:
    virtual             ~MediaPlayerInterface() { }
    virtual bool        hardwareOutput() { return false; }
    virtual void        setAudioSink(const sp<AudioSink>& audioSink) { mAudioSink = audioSink; }
protected:
    sp<AudioSink>       mAudioSink;
};


}; // namespace android

#endif // __cplusplus


#endif // ANDROID_MEDIAPLAYERINTERFACE_H
