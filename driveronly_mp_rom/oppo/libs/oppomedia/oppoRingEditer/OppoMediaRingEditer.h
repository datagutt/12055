#ifndef OPPOMEDIARINGEDITER_H
#define OPPOMEDIARINGEDITER_H

#include "OppoRingEditerInterface.h"
#include <utils/Mutex.h>

namespace android {

class OppoMediaRingEditer: public RefBase {
public:
	OppoMediaRingEditer();
	~OppoMediaRingEditer();
	status_t setDataSource(const char* sourcePath);
	bool saveRingPart(const char* destPath, long startms, long endms);
	
private:
	Mutex mLock;
	sp<OppoRingEditerInterface> mEditer;
	sp<MediaExtractor> extractor;
	int64_t DurationUs;
	char* mSniffMIME;
	char* mimetype;
};

}; // namespace android

#endif
