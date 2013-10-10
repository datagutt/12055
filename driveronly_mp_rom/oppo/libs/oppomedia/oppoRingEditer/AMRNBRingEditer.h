#ifndef AMRNBRINGEDITER_H
#define AMRNBRINGEDITER_H

#include "OppoRingEditerInterface.h"

namespace android {

class AMRNBRingEditer : public OppoRingEditerInterface {

public:
	AMRNBRingEditer();
	virtual ~AMRNBRingEditer();
	void setExtractor(sp<MediaExtractor> extractor);
	int writeHeader(const char* destPath);
	int writeFoot();

private:
	sp<MediaExtractor> mExtractor;
	int mfd;
};

}  // namespace android

#endif
