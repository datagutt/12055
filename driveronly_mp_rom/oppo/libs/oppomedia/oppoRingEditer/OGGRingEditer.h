#ifndef OGGRINGEDITER_H
#define OGGRINGEDITER_H

#include "OppoRingEditerInterface.h"

namespace android {

class OGGRingEditer : public OppoRingEditerInterface {

public:
	OGGRingEditer();
	virtual ~OGGRingEditer();
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
