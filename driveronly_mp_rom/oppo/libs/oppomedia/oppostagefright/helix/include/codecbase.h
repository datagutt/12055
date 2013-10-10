#ifndef _SKY_MM_CODEC_BASE_H
#define _SKY_MM_CODEC_BASE_H

#include "basetypes.h"

/* video frame structure */
typedef struct {
	MMUInt8 * m_pY;
	MMUInt8 * m_pU;
	MMUInt8 * m_pV;
	MMUInt32  m_uWidth;
	MMUInt32  m_uHeight;
	MMUInt32  m_uPitch;
	MMUInt32  m_uPitch_uv;
} SkyVideoOutput;

/* audio frame structure */
typedef struct {
	MMUInt8 * m_pPCMData;
	MMUInt32  m_uPCMLength;
	MMUInt32  m_uSampleRate;
	MMUInt32  m_uSampleBits;
	MMUInt32  m_uChannels;
} SkyAudioOutput;

/* Speedup status */
enum {
	SKY_CODEC_SPEEDUP_ENABLE  = 0,
	SKY_CODEC_SPEEDUP_DISABLE = 1,
};

typedef struct RealDecoderInterface_Tag {
	MMVoid* (*RealDecoderOpen)(MMVoid * pInfo);
	HRESULT (*RealDecoderDecode)(MMVoid * pDecoder, MMUInt8 * pStream, MMUInt32 uLen, MMUInt32 * uLenParsed, MMVoid * pOutput);
	HRESULT (*RealDecoderGetDecodedData)(MMVoid * pDecoder, MMVoid * pOutput);
	HRESULT (*RealDecoderFlush)(MMVoid * pDecoder);
	HRESULT (*RealDecoderSpeedup)(MMVoid * pDecoder, MMUInt32 uLevel);
	HRESULT (*RealDecoderClose)(MMVoid * pDecoder);
} RealDecoderInterface;

#endif	//_SKY_MM_CODEC_BASE_H
