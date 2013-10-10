#ifndef OPPORINGEDITER_INTERFACE_H
#define OPPORINGEDITER_INTERFACE_H

#include <utils/Errors.h>  // for status_t
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <media/oppostagefright/MediaErrors.h>
#include <media/oppostagefright/MediaExtractor.h>

namespace android {

class OppoRingEditerInterface: public RefBase {

public:
    OppoRingEditerInterface() {}
    virtual ~OppoRingEditerInterface() {}
    virtual int writeHeader(const char* destPath) = 0;
    virtual int writeFrameHeader(uint32_t frameLength) { return 0; }
    virtual int writeFoot() = 0;
    virtual void setExtractor(sp<MediaExtractor> extractor) = 0;
    virtual void setChannelCount(int32_t value) { Channels = value; }
    virtual void setSamplesPerSec(int32_t value) { SamplesPerSec = value; }
    virtual int32_t getChannelCount() { return Channels; }
    virtual int32_t getSamplesPerSec() { return SamplesPerSec; }

protected:
    int32_t Channels;
    int32_t SamplesPerSec;
};

}; // namespace android

#endif