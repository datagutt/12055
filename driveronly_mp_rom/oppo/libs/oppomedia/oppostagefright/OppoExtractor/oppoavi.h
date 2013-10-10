/******************************************************************************
** Copyright (C), 2012-2014, OPPO Mobile Comm Corp., Ltd
** VENDOR_EDIT 
** FileName: oppoavi.h
** Author: luodexiang
** Create Date: 2013-2-18
** Description: AVI header file.
** Version: 1.0
** ------------------Revision History: ------------------------
** <author>       <time>    <version >    <desc>
** luodexiang 2013-2-18  Revision 2.1   create this moudle  
******************************************************************************/

#ifndef OPPO_AVI_H
#define OPPO_AVI_H

#include <utils/String8.h>

namespace android {

#define FORMATFOURCC "0x%08x:%c%c%c%c"
#define PRINTFOURCC(x) x,((uint8_t*)&x)[0],((uint8_t*)&x)[1],((uint8_t*)&x)[2],((uint8_t*)&x)[3]

// big endian fourcc
#define BFOURCC(c1, c2, c3, c4) \
    (c4 << 24 | c3 << 16 | c2 << 8 | c1)
// size of chunk should be WORD align
#define EVEN(i) (uint32_t)((i) + ((i) & 1))

struct riffList {
    int32_t ID;
    int32_t size;
    int32_t type;
};

struct OppoAVISample {
    uint32_t offset;
    uint32_t size;
    uint8_t isSyncSample;
};

struct aviMainHeader {
    uint32_t microSecPerFrame;
    uint32_t maxBytesPerSec;
    uint32_t paddingGranularity;
    uint32_t flags;
    uint32_t totalFrames;
    uint32_t initialFrames;
    uint32_t streams;
    uint32_t suggestedBufferSize;
    uint32_t width;
    uint32_t height;
    uint32_t reserved[4];
};

struct aviStreamHeader {
    uint32_t fccType;
    uint32_t fccHandler;
    uint32_t flags;
    uint16_t priority;
    uint16_t language;
    uint32_t initialFrames;
    uint32_t scale;
    uint32_t rate;
    uint32_t start;
    uint32_t length;
    uint32_t suggestedBufferSize;
    uint32_t quality;
    uint32_t sampleSize;
    struct {
        short int left;
        short int top;
        short int right;
        short int bottom;
    } rcFrame;
};
// rcFrame is not provided by some files

struct bitmapInfo {
    uint32_t size; 
    uint32_t width; 
    uint32_t height; 
    uint16_t planes; 
    uint16_t bitCount;
    uint32_t compression; 
    uint32_t sizeImage; 
    uint32_t xPelsPerMeter; 
    uint32_t yPelsPerMeter; 
    uint32_t clrUsed; 
    uint32_t clrImportant; 
    // may be more datas
};

struct waveFormatEx { 
    uint16_t formatTag; 
    uint16_t nChannels; 
    uint32_t nSamplesPerSec; 
    uint32_t nAvgBytesPerSec; 
    uint16_t nBlockAlign; 
    uint16_t bitsPerSample; 
    uint16_t size; // for PCM, no this member
}; 

struct aviOldIndexEntry {
    uint32_t ID;
    uint32_t flags;
    uint32_t offset;
    uint32_t size;
};

struct OppoAVIIndexChunk {
    uint16_t longsPerEntry;
    uint8_t indexSubType;
    uint8_t indexType;
    uint32_t entriesInUse;
    uint32_t ID;
    // split offset to two int32_t to avoid 8 bytes padding
    uint32_t baseOffset;
    uint32_t baseOffsetHigh;
    uint32_t reserved3;
};

struct aviStdIndexEntry {
    uint32_t offset;
    uint32_t size;
};

struct aviSuperIndexEntry {
    uint64_t offset;
    uint32_t size;
    uint32_t duration;
};

struct ac3HeaderInfo {
    uint16_t sync_word;
    uint16_t crc1;
	uint8_t sr_code;
	uint8_t bitstream_id;
	uint16_t frame_size;
};

const uint16_t ac3_frame_size_tab[38][3] = {
    { 64,   69,   96   },
    { 64,   70,   96   },
    { 80,   87,   120  },
    { 80,   88,   120  },
    { 96,   104,  144  },
    { 96,   105,  144  },
    { 112,  121,  168  },
    { 112,  122,  168  },
    { 128,  139,  192  },
    { 128,  140,  192  },
    { 160,  174,  240  },
    { 160,  175,  240  },
    { 192,  208,  288  },
    { 192,  209,  288  },
    { 224,  243,  336  },
    { 224,  244,  336  },
    { 256,  278,  384  },
    { 256,  279,  384  },
    { 320,  348,  480  },
    { 320,  349,  480  },
    { 384,  417,  576  },
    { 384,  418,  576  },
    { 448,  487,  672  },
    { 448,  488,  672  },
    { 512,  557,  768  },
    { 512,  558,  768  },
    { 640,  696,  960  },
    { 640,  697,  960  },
    { 768,  835,  1152 },
    { 768,  836,  1152 },
    { 896,  975,  1344 },
    { 896,  976,  1344 },
    { 1024, 1114, 1536 },
    { 1024, 1115, 1536 },
    { 1152, 1253, 1728 },
    { 1152, 1254, 1728 },
    { 1280, 1393, 1920 },
    { 1280, 1394, 1920 },
};

#define RIFF_WAVE_FORMAT_PCM            (0x0001)
#define RIFF_WAVE_FORMAT_ALAW           (0x0006)
#define RIFF_WAVE_FORMAT_MULAW          (0x0007)
#define RIFF_WAVE_FORMAT_MPEGL12        (0x0050)
#define RIFF_WAVE_FORMAT_MPEGL3         (0x0055)
#define RIFF_WAVE_FORMAT_AMR_NB         (0x0057)
#define RIFF_WAVE_FORMAT_AMR_WB         (0x0058)
#define RIFF_WAVE_FORMAT_AAC            (0x00ff)
#define RIFF_IBM_FORMAT_MULAW           (0x0101)
#define RIFF_IBM_FORMAT_ALAW            (0x0102)
#define RIFF_WAVE_FORMAT_WMAV1          (0x0160)
#define RIFF_WAVE_FORMAT_WMAV2          (0x0161)
#define RIFF_WAVE_FORMAT_WMAV3          (0x0162)
#define RIFF_WAVE_FORMAT_WMAV3_L        (0x0163)
#define RIFF_WAVE_FORMAT_AAC_AC         (0x4143)
#define RIFF_WAVE_FORMAT_VORBIS         (0x566f)
#define RIFF_WAVE_FORMAT_VORBIS1        (0x674f)
#define RIFF_WAVE_FORMAT_VORBIS2        (0x6750)
#define RIFF_WAVE_FORMAT_VORBIS3        (0x6751)
#define RIFF_WAVE_FORMAT_VORBIS1PLUS    (0x676f)
#define RIFF_WAVE_FORMAT_VORBIS2PLUS    (0x6770)
#define RIFF_WAVE_FORMAT_VORBIS3PLUS    (0x6771)
#define RIFF_WAVE_FORMAT_AAC_pm         (0x706d)
#define RIFF_WAVE_FORMAT_GSM_AMR_CBR    (0x7A21)
#define RIFF_WAVE_FORMAT_GSM_AMR_VBR    (0x7A22)

#define AC3_HENDER_SYNCWORD              (0x0B77)
#define MAX_AC3_FRAMESIZE                (1920*2)       
#define MAX_MP3_FRAMESIZE  1441 //bytes

inline size_t chID2streamID(uint32_t id) {
    return ((id & 0xff) - '0') * 10 + ((id >> 8) & 0xff) - '0';
}

inline size_t ixID2streamID(uint32_t id) {
    return chID2streamID(id >> 16);
}

struct MPEG4Info {
    int progressive;
    int cpcf;
};

int findVOLHeader(const uint8_t *start, int length);
int decodeVOLHeader(const uint8_t *vol, size_t size, struct MPEG4Info *s);
int decodeShortHeader(const uint8_t *vol, size_t size, struct MPEG4Info* s);
bool checkVideoResolution(int32_t width, int32_t height, const char* streamtype);
bool checkProfileLevel(int32_t width, int32_t height, int32_t scale, int32_t rate,uint32_t level);
bool get_mp3_frame_size(
        uint32_t header, size_t *frame_size,
        int *out_sampling_rate = NULL, int *out_channels = NULL,
        int *out_bitrate = NULL,int *out_sampleperframe=NULL);

const char *wave2MIME(uint32_t id,uint16_t bps);

const char *BFourCC2MIME(uint32_t fourcc);

int ac3_find_syncinfo(const uint8_t *srcdata,size_t size,struct ac3HeaderInfo *hdr);
bool ac3_parse_header(const uint8_t *srcdata, size_t size,struct ac3HeaderInfo *hdr);


}
#endif
