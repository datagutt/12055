#ifndef _RM_DEMUX_INTERFACE_H
#define _RM_DEMUX_INTERFACE_H

#include "demuxbase.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
	SPEEDUP_DEFAULT         = (0<<0),
	SPEEDUP_SKIPONE_BFRAME  = (1<<0),
	SPEEDUP_ONLYLUM_DBLOCK  = (1<<1),
	SPEEDUP_SKIPALL_BFRAME  = (1<<2),
	SPEEDUP_DISABLE_DBLOCK  = (1<<3),
};

typedef struct RealDemuxInterface_Tag {
	MMVoid* (*RMDemuxCreate)(MMVoid *strFileName, MMUInt32 nFileFormat);
	MMVoid* (*RMDemuxCreateByHandle)(MMUInt32 nFileHandle, MMUInt32 nFileFormat);
	HRESULT (*RMDemuxSeek)(MMVoid *pDemux,MMUInt32 nTime);
	HRESULT (*RMDemuxAudioSeek)(MMVoid *pDemux,MMUInt32 nTime);
	HRESULT (*RMDemuxGetAudioInfo)(MMVoid *pDemux,DemuxAudioInfo *pAudioInfo);
	HRESULT (*RMDemuxGetVideoInfo)(MMVoid *pDemux,DemuxVideoInfo *pVideoInfo);
	HRESULT (*RMDemuxGetAudioData)(MMVoid *pDemux, MMVoid *pData, MMUInt32 *pLen, MMUInt32 *pCts);
	HRESULT (*RMDemuxGetVideoData)(MMVoid *pDemux, MMVoid *pData, MMUInt32 *pLen, MMUInt32 *pCts, MMInt32 * pType);
	HRESULT (*RMDemuxGetNextVideoKeyTime)(MMVoid *pDemux,MMUInt32 *pCts);
	HRESULT (*RMDemuxDropToNextVideoKeyFrame)(MMVoid *pDemux);
	MMVoid  (*RMDemuxDestroy)(MMVoid *pDemux);
	MMVoid  (*RMDemuxSetSpeedup)(MMVoid *pDemux, MMUInt32 uSpeedup);
} RealDemuxerInterface;

RealDemuxerInterface * GetRealDemuxer();

#ifdef __cplusplus
}
#endif

#endif	//_RM_DEMUX_INTERFACE_H
