/*****************************************************************************
*
* Filename:
* ---------
*   OppoAACExtractor.h
*
* Project:
* --------
*   OPPO Rom
*
* Description:
* ------------
*   Oppo AAC Parser
*
* Author: luodexiang
* -------
****************************************************************************/

#ifndef OPPO_AAC_EXTRACTOR_H_

#define OPPO_AAC_EXTRACTOR_H_

#include <media/oppostagefright/MediaExtractor.h>
#include <utils/Errors.h>

namespace android {

struct AMessage;
class DataSource;
class String8;

class OppoAACExtractor : public MediaExtractor {
public:
    // Extractor assumes ownership of "source".
    OppoAACExtractor(const sp<DataSource> &source, const sp<AMessage> &meta);

    virtual size_t countTracks();
    virtual sp<MediaSource> getTrack(size_t index);
    virtual sp<MetaData> getTrackMetaData(size_t index, uint32_t flags);

    virtual sp<MetaData> getMetaData();
	
    virtual uint32_t flags() const;
	sp<MetaData> MakeAACCodecSpecificData(
        unsigned profile, unsigned sampling_freq_index,
        unsigned channel_configuration);

private:
    status_t mInitCheck;

	bool mIsADTS;
    sp<DataSource> mDataSource;
    off_t mFirstFramePos;
    sp<MetaData> mMeta;
    uint32_t mFixedHeader;
	//int64_t mFrameDurationUs;

    OppoAACExtractor(const OppoAACExtractor &);
    OppoAACExtractor &operator=(const OppoAACExtractor &);
};

bool SniffOppoAAC(
        const sp<DataSource> &source, String8 *mimeType, float *confidence,
        sp<AMessage> *meta);

}  // namespace android

#endif  // OPPO_AAC_EXTRACTOR_H_