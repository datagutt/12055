/************************************************************
  Copyright (C), 2008-2010, OPPO Mobile Comm Corp., Ltd
  FileName: FfmpegExtractor.h
  Author: zouf
  Create Date: 2010-11-1
  Description: parse fileformat for stagefright (use ffmpeg avlib)
  History: 
      <author>  <time>   <version >   <desc>
      zouf       2010-11-1     1.0     create this moudle  
***********************************************************/


#ifndef __FFMEPG_EXTRACTOR_H__

#define __FFMEPG_EXTRACTOR_H__


#include <utils/Vector.h>
#include <utils/RefBase.h>
#include <media/oppostagefright/MediaExtractor.h>

#include "FfmpegParser.h"

#define MAX_META_NUM 32
/*MultimediaGroup zys(80056176),20110819*/
/** Available Profiles */
//@{
enum Profile {
    PROFILE_SIMPLE,
    PROFILE_MAIN,
    PROFILE_COMPLEX, ///< TODO: WMV9 specific
    PROFILE_ADVANCED
};

#define FF_I_TYPE  1 ///< Intra
#define FF_P_TYPE  2 ///< Predicted
#define FF_B_TYPE  3 ///< Bi-dir predicted
#define FF_S_TYPE  4 ///< S(GMC)-VOP MPEG4
#define FF_SI_TYPE 5 ///< Switching Intra
#define FF_SP_TYPE 6 ///< Switching Predicted
#define FF_BI_TYPE 7

typedef struct VC1SeqHeader{
    /** Simple/Main Profile sequence header */
    //@{
    int rangered;         ///< RANGEREDFRM (range reduction) syntax element present
    //@}

    /** Advanced Profile */
    //@{
    int interlace;        ///< Progressive/interlaced (RPTFTM syntax element)
    //@}

    /** Sequence header data for all Profiles
     * TODO: choose between ints, uint8_ts and monobit flags
     */
    //@{
    int profile;          ///< 2bits, Profile
    int finterpflag;      ///< INTERPFRM present
    //@}
    int max_b_frames; ///< max number of b-frames for encoding
} VC1SeqHeader;
/*MultimediaGroup zys(80056176),20110819,end*/

namespace android {

class DataSource;
class String8;


class FfmpegExtractor : public MediaExtractor {
public:
    // Extractor assumes ownership of "source".
    FfmpegExtractor(const sp<DataSource> &source);

    virtual size_t countTracks();
    virtual sp<MediaSource> getTrack(size_t index);
	
    virtual sp<MetaData> getTrackMetaData(size_t index, uint32_t flags);
    virtual sp<MetaData> getMetaData();
	virtual void    	 dropFrame(int level = 0); 
    status_t verifyTrack(int index);//for test
protected:
    virtual ~FfmpegExtractor();

private:
int descrLength(int len);
int fillESDS(const char* extra, int size, char** pesds, int* esds_len);


private:
    sp<DataSource> mDataSource;
    bool mHaveMetadata;
    bool mHasVideo;
	bool mQcomPlatform;
    FfmpegParser   *mParser;
    bool isOpen;
    int    mTrackNum;
    sp<MetaData> mFileMetaData;
    sp<MetaData> meta[MAX_META_NUM];

/*MultimediaGroup ggt,20110609, add for VC1 HW DECODE  begin*/
    int getVC1SequenceLayerDataAndSet(int index);
    int addDIVXCodecSpecificData(int index);
    int addAVCCodecSpecificData(int index);
	int foundH264StartCodeFromNaluData(const char* data,int size);
/*MultimediaGroup ggt add for use DIVX HW decode,20120417 end*/
    FfmpegExtractor(const FfmpegExtractor &);
    FfmpegExtractor &operator=(const FfmpegExtractor &);
};

bool SniffFfmpeg(
        const sp<DataSource> &source, String8 *mimeType, float *confidence,  sp<AMessage> *);

}  // namespace android

#endif  // FFMEPG_EXTRACTOR_H_


