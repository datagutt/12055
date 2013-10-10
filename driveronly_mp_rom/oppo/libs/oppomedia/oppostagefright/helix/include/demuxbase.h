#ifndef _SKY_MM_DEMUX_BASE_H
#define _SKY_MM_DEMUX_BASE_H
#include "basetypes.h"
#ifndef S_OK
#define S_OK                                   ((HRESULT)0x00000000L)
#endif

#ifndef S_FALSE
#define S_FALSE                                ((HRESULT)0x00000001L)
#endif

typedef	int				HRESULT;

typedef enum __tagVideoCodecType
{
	VIDEO_UNKNOWN = 0,
	VIDEO_MJPEG,
	VIDEO_MPEG4,
	VIDEO_H263,
	VIDEO_FLV1,
	VIDEO_H264,
	VIDEO_RV20,
	VIDEO_RV30,
	VIDEO_RV40,
}VideoCodecType;

typedef enum __tagAudioCodecType
{
	AUDIO_UNKNOWN = 0,
	AUDIO_PCM,
	AUDIO_AMR,
	AUDIO_AMR_IF2,
	AUDIO_G723,
	AUDIO_G726_64,
	AUDIO_G711_A,
	AUDIO_G711_U,
	AUDIO_IMA,
	AUDIO_DIVX,
	AUDIO_MP3,
	AUDIO_AC3,
	AUDIO_AAC,
	AUDIO_DTS,
	AUDIO_VORBIS,
	AUDIO_COOK,
	AUDIO_ATRC,
	AUDIO_SIPR,
}AudioCodecType;

typedef struct tagDemuxAudioInfo{
	MMUInt32	dwSamplesPerSec;	/// audio sampling freq.
	MMUInt32	dwChannels;			/// audio # channels.
	MMUInt32	dwBitsPerSample;	/// audio quant. length.
	MMUInt32	dwAudioType;		/// MP4DEMUX_XXX.
	MMUInt32	dwAudioStreamSize;	/// raw stream size in bytes.
	MMUInt32	dwAudioTimeScale;	/// audio timescale.
	MMUInt32	dwAudioDuration;	/// playback duration time in dwAudioTimeScale.
	MMUInt8*	pcbAudioDsiBuffer;		/// audio DSI buffer
	MMUInt32	dwAudioDsiBufferSize;
	MMUInt32	dwMaxAudioBitRate;		/// BPS,will be used to determine internal buffering size.
	MMInt32     nStreamNum;
}DemuxAudioInfo;

typedef struct tagDemuxVideoInfo{
	MMUInt32	dFrameRate;			/// video rendering frame rate.
	MMUInt32	dwWidth;			/// frame width.
	MMUInt32	dwHeight;			/// frame height.
	MMUInt32	dwTotalFrames;		/// # video frames.
	MMUInt32	dwVideoType;		/// MP4DEMUX_XXX.
	MMUInt32	dwVideoStreamSize;	/// raw stream size in bytes.
	MMUInt32	dwVideoTimeScale;	/// video timescale.
	MMUInt32	dwVideoDuration;	/// playback duration time in dwVideoTimeScale.
	MMUInt8*	pcbVideoDsiBuffer;		/// video DSI buffer
	MMUInt32	dwVideoDsiBufferSize;
	MMUInt32	dwMaxVideoBitRate;		/// BPS,will be used to determine internal buffering size.
	MMInt32     nStreamNum;
	MMUInt32    dwThumbnailTime;
}DemuxVideoInfo;


#endif	//_SKY_MM_DEMUX_BASE_H