#ifndef WAVRINGEDITER_H
#define WAVRINGEDITER_H

#include "OppoRingEditerInterface.h"

namespace android {

class WAVRingEditer : public OppoRingEditerInterface {

public:
	WAVRingEditer();
	virtual ~WAVRingEditer();
	void setExtractor(sp<MediaExtractor> extractor);
	int writeHeader(const char* destPath);
	int writeFoot();

private:
	void writeWAVHead(uint32_t dataLength);
	void writeU16(uint8_t *p, uint16_t val);
	void writeU32(uint8_t *p, uint32_t val);

	sp<MediaExtractor> mExtractor;
	int mfd;
};

}  // namespace android

#endif
