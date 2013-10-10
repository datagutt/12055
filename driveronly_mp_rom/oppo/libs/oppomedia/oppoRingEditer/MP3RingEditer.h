#ifndef MP3RINGEDITER_H
#define MP3RINGEDITER_H

#include "OppoRingEditerInterface.h"

namespace android {

class MP3RingEditer : public OppoRingEditerInterface {

public:
	MP3RingEditer();
	virtual ~MP3RingEditer();
	void setExtractor(sp<MediaExtractor> extractor);
	int writeHeader(const char* destPath);
	int writeFoot();

private:
	sp<MediaExtractor> mExtractor;
	int mfd;
};

}  // namespace android

#endif
