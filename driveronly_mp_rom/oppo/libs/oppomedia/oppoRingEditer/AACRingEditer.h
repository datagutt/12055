#ifndef AACRINGEDITER_H
#define AACRINGEDITER_H

#include "OppoRingEditerInterface.h"

namespace android {

class AACRingEditer : public OppoRingEditerInterface {

public:
	AACRingEditer();
	virtual ~AACRingEditer();
	void setExtractor(sp<MediaExtractor> extractor);
	int writeHeader(const char* destPath);
	int writeFrameHeader(uint32_t frameLength);
	int writeFoot();

private:
	bool getSampleRateTableIndex(int sampleRate, uint8_t* tableIndex);
	
	sp<MediaExtractor> mExtractor;
	int mfd;
	int32_t mChannelCount;
	int32_t mSampleRate;
	int32_t mAACProfile;
	uint8_t kSampleFreqIndex;

	enum {
        kAdtsHeaderLength = 7,     // # of bytes for the adts header
        kSamplesPerFrame  = 1024,  // # of samples in a frame
    };
};

}  // namespace android

#endif
