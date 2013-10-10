/************************************************************
  Copyright (C), 2008-2010, OPPO Mobile Comm Corp., Ltd
  FileName: ffmpeg.h
  Author: zouf
  Create Date: 2010-11-1
  Description: wrap for ffmpeg lib
  History: 
      <author>  <time>   <version >   <desc>
      zouf       2010-11-1     1.0     create this moudle  
***********************************************************/

#ifndef __FFMEPGPARSER_H__
#define __FFMEPGPARSER_H__

#include <pthread.h>
#include <stdint.h>

#ifndef UINT64_C
#define UINT64_C(c)  __UINT64_C(c)
#endif

extern "C"
{
#include "libavformat/avformat.h"
#include "libavformat/metadata.h"
}

#define INT64_C(c)       __INT64_C(c)

#define 	MAX_TRACK_NUM 		32
#define 	MAX_FILENAME_LEN 	1024
#define    	MAX_QUEUE_NUM    	500

enum TrackParamType
{
    TRACK_DURATION,
    TRACK_CODEC_ID,
    TRACK_CODEC_TAG,
    TRACK_MIME,
    TRACK_TIMESCALE,
    TRACK_EXTRADATA_SIZE,
    TRACK_EXTRADATA,
    TRACK_WIDTH,
    TRACK_HEIGHT,
    TRACK_CHANNELS,
    TRACK_SAMPLE_RATE,
    TRACK_BIT_RATE,
    TRACK_BLOCK_ALIGN,
    TRACK_FRAME_SIZE,
    TRACK_ROTATION_DEGREES,
    TRACK_THUMBNAIL_TIME,
    TRACK_SAMPLEFMT,
    TRACK_BIT_SAMPLE,
    TRACK_LANGUAGE,
};


typedef struct MetaInfo
{
    char *title;
    char *author;
    char *copyright;
    char *comment;
    char *album;
    char *year;  /**< ID3 year, 0 if none */
    char *track; /**< track number, 0 if none */
    char *genre; /**< ID3 genre */
    char *artist;
    char *album_artist;
    char *albumArt;
    int albumSize;
    char *albumArtMime;
}MetaInfo;

typedef struct PacketQueue 
{
    AVPacketList *first_pkt, *last_pkt;
    int nb_packets;
    int size;
    int abort_request;
     pthread_mutex_t mutex;
} PacketQueue;

class FfmpegParser
{
public:
	static FfmpegParser* create();
	static const char* probe_format(char* buf,  int len);

	int open_file(char* filename);
	int close_file();

	int get_track_num();
	int getTrackParam(int index, enum TrackParamType Type, void* param);
	uint32_t get_frame_nums(int index);
	int get_cbr(int index);
	int get_file_param(char* mime, MetaInfo* meta);
	int seek(int index, int64_t seek_time, int mode = 0);
	int read_video_frame(char* buf, size_t buf_len, size_t* size, int64_t* dts, int* flag);
	int read_audio_frame(char* buf, size_t buf_len, size_t* size, int64_t* dts, int* flag);
	void dropBFrame(int level);
	void select_AudioTrack(int index);
	void incRef();
	void decRef();
	virtual ~FfmpegParser();
  
private:
	FfmpegParser();
	
	int  read_common_frame(int frame_type, char* buf, size_t buf_len, size_t* size, int64_t* dts, int* flag);
	void packet_queue_init(PacketQueue *q);
	void packet_queue_flush(PacketQueue *q);
	void packet_queue_end(PacketQueue *q);
	int  packet_queue_put(PacketQueue *q, AVPacket *pkt);
	void packet_queue_abort(PacketQueue *q);
	int  packet_queue_get(PacketQueue *q, AVPacket *pkt);
	int  get_index_type(int index);
	
    const char* get_track_mime(int index);
    int64_t get_best_thumbnail_time(int index);
	
private:
	static bool  	mFfmpeglib_init;
	int    		  	mRefCount;
	AVFormatContext *mAVContext;
	AVInputFormat 	*mAVIformat;
	char 			mFilename[MAX_FILENAME_LEN];
	PacketQueue 	mAudioq;
	PacketQueue 	mVideoq;
    bool 			bAudioEof;
	int   			mTrackNum;
	int    			mAudioCount;
	int   		 	mVideoCount;
	int 			mAudioIndex[MAX_TRACK_NUM];
	int 			mSelectAudioIndex;
	int 			mVideoIndex[MAX_TRACK_NUM];
	pthread_mutex_t read_frame_mutex; // video & audio use it
	int 			mDropFrameLevel;
};

#endif

