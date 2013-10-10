#ifndef AMRWBRINGEDITER_H
#define AMRWBRINGEDITER_H

#include "OppoRingEditerInterface.h"

namespace android {

class AMRWBRingEditer : public OppoRingEditerInterface {

public:
	AMRWBRingEditer();
	virtual ~AMRWBRingEditer();
	void setExtractor(sp<MediaExtractor> extractor);
	int writeHeader(const char* destPath);
	int writeFoot();

private:
	sp<MediaExtractor> mExtractor;
	int mfd;
};

}  // namespace android

#endif
