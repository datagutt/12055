/*
**
** Copyright 2009, The Android Open Source Project
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

#ifndef ANDROID_OPPOMEDIAPLAYER_H
#define ANDROID_OPPOMEDIAPLAYER_H
#include <media/AudioTrack.h>
#include <system/window.h>

#include <media/oppostagefright/MediaPlayerInterface.h>

namespace android {

struct AwesomePlayer;
class OppoMediaPlayer;

class AudioOutput : public AudioSink
{

 public:
                            AudioOutput(int sessionId);
    virtual                 ~AudioOutput();

    virtual bool            ready() const { return mInitOk == true; }
    virtual bool            realtime() const { return true; }
    virtual ssize_t         bufferSize() const;
    virtual ssize_t         frameCount() const;
    virtual ssize_t         channelCount() const;
    virtual ssize_t         frameSize() const;
    virtual uint32_t        latency();
    virtual status_t        getPosition(uint32_t *position) const;
    virtual status_t        getFramesWritten(uint32_t *frameswritten) const;
    virtual int             getSessionId() const;
	virtual status_t 		open(uint32_t sampleRate, int channelCount, 
							audio_channel_mask_t channelMask,
							audio_format_t format, int bufferCount,int32_t *buffersize);
    virtual void            start();
	virtual ssize_t     	write(DataPacket &packet);

    virtual void            stop();
    virtual void            flush();
    virtual void            pause();
    virtual void            close();
            void            setAudioStreamType(audio_stream_type_t streamType) { mStreamType = streamType; }
            void            setVolume(float left, float right);
            status_t        setAuxEffectSendLevel(float level);
            status_t        attachAuxEffect(int effectId);
    virtual status_t        dump(int fd, const Vector<String16>& args) const;

    static bool             isOnEmulator();
    static int              getMinBufferCount();
			void            setAudioNotify(OppoMediaPlayer *player) {mNotify = player;}

private:
    static void             setMinBufferCount();
	
	OppoMediaPlayer         *mNotify;
	uint32_t                 mFrameCount;
	ssize_t                 mChannelCount;
	
    uint64_t                mBytesWritten;
    audio_stream_type_t     mStreamType;
	audio_format_t          mFormat;
    float                   mLeftVolume;
    float                   mRightVolume;
    int32_t                 mPlaybackRatePermille;
    uint32_t                mSampleRateHz; // sample rate of the content, as set in open()
    uint32_t                mLatency;
	bool                    mInitOk;
    int                     mSessionId;
    float                   mSendLevel;
    int                     mAuxEffectId;
    static bool             mIsOnEmulator;
    static int              mMinBufferCount;  // 12 for emulator; otherwise 4
    
    audio_output_flags_t    mFlags; // no used for AudioTrack.java,but used for AudioSystem::getOutput parameter
}; // AudioOutput

class OppoMediaPlayer : public RefBase
{
public:
    OppoMediaPlayer();
    ~OppoMediaPlayer();

    status_t        setDataSource(
            const char *url,
            const KeyedVector<String8, String8> *headers);

    status_t        setDataSource(int fd, int64_t offset, int64_t length);
	status_t 		invoke(const Parcel& request, Parcel *reply);

    status_t        setVideoSurfaceTexture(
                            const sp<ISurfaceTexture>& surfaceTexture);
	status_t 		setSurface(const sp<Surface> &surface);
    status_t        setListener(const sp<MediaPlayerListener>& listener);
    status_t        prepare();
    status_t        prepareAsync();
    status_t        start();
    status_t        stop();
    status_t        pause();
    bool            isPlaying();
    status_t        getVideoWidth(int *w);
    status_t        getVideoHeight(int *h);
    status_t        seekTo(int msec);
    status_t        getCurrentPosition(int *msec);
    status_t        getDuration(int *msec);
    status_t        reset();
    status_t        setAudioStreamType(audio_stream_type_t type);
    status_t        setLooping(int loop);
    bool            isLooping();
    status_t        setVolume(float leftVolume, float rightVolume);
    static void     notify(void* cookie, int msg, int ext1, int ext2);
	int             audioNotify(audio_event_type msg,DataPacket *obj,AudioParameters *parameters);
	void 			handleNotify(int msg, int ext1, int ext2);
    status_t        setAudioSessionId(int sessionId);
    int             getAudioSessionId();
    status_t        setAuxEffectSendLevel(float level);
    status_t        attachAuxEffect(int effectId);
    status_t    	setParameter(int key, const char* param);
    char*    		getParameter(int key);
//    status_t        setRetransmitEndpoint(const char* addrString, uint16_t port);
//    status_t        setNextMediaPlayer(const sp<OppoMediaPlayer>& player);

private:
            void            clear_l();
            status_t        seekTo_l(int msec);
            status_t        prepareAsync_l();
            status_t        getDuration_l(int *msec);
            status_t        reset_l();
			sp<MediaPlayerBase> 		setDataSource_pre(player_type playerType);
			status_t 		setDataSource_post(const sp<MediaPlayerBase>& p, status_t status);
			player_type     getPlayerType(int fd, int64_t offset, int64_t length);
        	player_type     getPlayerType(const char* url);
			sp<MediaPlayerBase>     createPlayer(player_type playerType);
			void 					disconnectNativeWindow();

    sp<MediaPlayerBase>         mPlayer;
	sp<AudioOutput> 			mAudioOutput;
    thread_id_t                 mLockThreadId;
    Mutex                       mLock;
    Mutex                       mNotifyLock;
    Condition                   mSignal;
    sp<MediaPlayerListener>     mListener;
    void*                       mCookie;
    media_player_states         mCurrentState;
    int                         mDuration;
    int                         mCurrentPosition;
    int                         mSeekPosition;
    bool                        mPrepareSync;
    status_t                    mPrepareStatus;
    audio_stream_type_t         mStreamType;
    bool                        mLoop;
    float                       mLeftVolume;
    float                       mRightVolume;
    int                         mVideoWidth;
    int                         mVideoHeight;
    int                         mAudioSessionId;
    float                       mSendLevel;
//    struct sockaddr_in          mRetransmitEndpoint;
//    bool                        mRetransmitEndpointValid;
	sp<ANativeWindow>           mConnectedWindow;

    OppoMediaPlayer(const OppoMediaPlayer &);
    OppoMediaPlayer &operator=(const OppoMediaPlayer &);
};

}  // namespace android

#endif  // ANDROID_OPPOPLAYER_H
