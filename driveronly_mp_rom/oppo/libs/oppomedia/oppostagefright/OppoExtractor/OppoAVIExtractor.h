/******************************************************************************
** Copyright (C), 2012-2014, OPPO Mobile Comm Corp., Ltd
** VENDOR_EDIT 
** FileName: OppoAVIExtractor.h
** Author: luodexiang
** Create Date: 2013-2-18
** Description: AVI Extractor interface.
** Version: 1.0
** ------------------Revision History: ------------------------
** <author>       <time>    <version >    <desc>
** luodexiang 2013-2-18  Revision 2.1   create this moudle  
******************************************************************************/

#ifndef MTK_AVI_EXTRACTOR_H_

#define MTK_AVI_EXTRACTOR_H_

#include <media/oppostagefright/MediaExtractor.h>
#include <utils/Vector.h>
#include <../include/SampleTable.h>
#include <media/oppostagefright/MetaData.h>

typedef off64_t OppoAVIOffT;
//typedef off_t MtkAVIOffT;

namespace android {

struct AMessage;
class DataSource;
class String8;
class OppoAVISource;
struct OppoAVISample;
struct OppoAVIIndexChunk;

class OppoAVIExtractor : public MediaExtractor {
public:
    // Extractor assumes ownership of "source".
    OppoAVIExtractor(const sp<DataSource> &source);

    virtual size_t countTracks();
    virtual sp<MediaSource> getTrack(size_t index);
    virtual sp<MetaData> getTrackMetaData(size_t index, uint32_t flags);

    virtual sp<MetaData> getMetaData();
    virtual uint32_t flags() const;
    virtual status_t stopParsing();
    virtual status_t finishParsing();

protected:
    virtual ~OppoAVIExtractor();

private:
    struct Track {
        sp<MetaData> meta;
        int scale;
        int rate;
        int sampleSize;
        uint32_t sampleCount;
        int64_t durationUs;
        bool mIsVideo;
        uint32_t mStartTimeOffset;
        int priority;
    };

    uint32_t mIndexOffset;
    uint32_t mMOVIOffset;
    uint32_t mMOVISize;
    uint32_t mWidth;
    uint32_t mHeight;
    uint32_t mEmptyChunks;
    int32_t mNumTracks;
    sp<DataSource> mDataSource;
    bool mHasMetadata;
    bool mHasVideo;
	bool mHasAudio;
    bool mHasIndex;
    bool mStopped;
	bool mQcomPlatform;
    ssize_t mFileSize;
    status_t mInitCheck;
    Vector<sp<OppoAVISource> > mTracks;

    sp<MetaData> mFileMetaData;

    status_t readMetaData();

    status_t readHeader(OppoAVIOffT *pos, char *h, ssize_t size);
    status_t parseFirstRIFF();
    status_t parseMoreRIFF(OppoAVIOffT *off);

    status_t addSample(size_t id, struct OppoAVISample* s);
    status_t parseHDRL(OppoAVIOffT off, OppoAVIOffT end);
    status_t parseMOVI(OppoAVIOffT off, OppoAVIOffT end);
    status_t parseDataChunk(int32_t pos, int ID, int size, int sync);
    status_t parseMOVIMore(bool full = true);
    status_t parseIDX1(OppoAVIOffT off, OppoAVIOffT end);
    status_t parseINFO(OppoAVIOffT off, OppoAVIOffT end);

    status_t parseAVIH(OppoAVIOffT off, OppoAVIOffT end);
    status_t parseSTRL(OppoAVIOffT off, OppoAVIOffT end, int index);

    status_t parseSTRH(OppoAVIOffT off, OppoAVIOffT end, sp<OppoAVISource> source);
    status_t parseSTRF(OppoAVIOffT off, OppoAVIOffT end, sp<OppoAVISource> source);
    status_t parseSTRD(OppoAVIOffT off, OppoAVIOffT end, sp<OppoAVISource> source);
    status_t parseSTRN(OppoAVIOffT off, OppoAVIOffT end, sp<OppoAVISource> source);
    status_t parseVPRP(OppoAVIOffT off, OppoAVIOffT end, sp<OppoAVISource> source);
    status_t parseINDX(OppoAVIOffT pos, OppoAVIOffT end, sp<OppoAVISource> source);
    status_t parseChunkIndex(OppoAVIOffT pos, OppoAVIOffT end, sp<OppoAVISource> source,
            struct OppoAVIIndexChunk* pHeader);

    status_t checkCapability();

    status_t parseMetaData(OppoAVIOffT offset, size_t size);

    static status_t verifyTrack(Track *track);

    status_t parseTrackHeader(OppoAVIOffT data_offset, OppoAVIOffT data_size);

    OppoAVIExtractor(const OppoAVIExtractor &);
    OppoAVIExtractor &operator=(const OppoAVIExtractor &);
};

bool OppoSniffAVI(
        const sp<DataSource> &source, String8 *mimeType, float *confidence,
        sp<AMessage> *);

}  // namespace android

#endif  // MTK_AVI_EXTRACTOR_H_
