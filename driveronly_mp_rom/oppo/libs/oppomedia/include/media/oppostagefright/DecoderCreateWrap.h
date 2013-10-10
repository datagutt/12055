/******************************************************************************
** Copyright (C), 2011-2013, OPPO Mobile Comm Corp., Ltd
** VENDOR_EDIT 
** FileName: oppo_adpcm.cpp
** Author: luodexiang
** Create Date: 2012-12-17
** Description: Adpcm decoder interface.
** Version: 1.0
** ------------------Revision History: ------------------------
** <author>       <time>    <version >    <desc>
** luodexiang 2012-6-19  Revision 2.1   create this moudle  
******************************************************************************/

#ifndef DECORDERCREATEWRAP_H
#define DECORDERCREATEWRAP_H

#include <media/oppostagefright/MediaSource.h>
#include <android/native_window.h>
#include <media/IOMX.h>

namespace android {

class DecoderCreateWrap : public RefBase{
public:
	DecoderCreateWrap();
	~DecoderCreateWrap();
	
    sp<MediaSource> CreateDecoder(
            const sp<IOMX> &omx,
            const sp<MetaData> &meta, bool createEncoder,
            const sp<MediaSource> &source,
            uint32_t useDecoderFlags = 0,
            const char *matchComponentName = NULL,
            uint32_t flags = 0,
            const sp<ANativeWindow> &nativeWindow = NULL);
	
private:
	DecoderCreateWrap(const DecoderCreateWrap &);
    DecoderCreateWrap &operator=(const DecoderCreateWrap &);
	bool mInitOmxOk;

	typedef sp<MediaSource> (*CreateFuc)(
            const sp<IOMX>&,
            const sp<MetaData>&, bool ,
            const sp<MediaSource> &,
            const char* ,
            uint32_t,
            const sp<ANativeWindow>&);
	CreateFuc mCreateFun;

	void* mCodecLibHandle;
	void addOMXCodec();
	void clearOMXCodec();	
	
public:
	enum DecoderFlags {
		kPreferUseOmx      = 0,
		kPreferUseReal     = 1,
		kPreferUseFfmpeg   = 2,
    };

	/* the following parameters that changed with google android version is for google android 4.1*/
    enum CreationFlags {
        kPreferSoftwareCodecs    = 1,
        kIgnoreCodecSpecificData = 2,

        // The client wants to access the output buffer's video
        // data for example for thumbnail extraction.
        kClientNeedsFramebuffer  = 4,

        // Request for software or hardware codecs. If request
        // can not be fullfilled, Create() returns NULL.
        kSoftwareCodecsOnly      = 8,
        kHardwareCodecsOnly      = 16,

        // Store meta data in video buffers
        kStoreMetaDataInVideoBuffers = 32,

        // Only submit one input buffer at one time.
        kOnlySubmitOneInputBufferAtOneTime = 64,

        // Enable GRALLOC_USAGE_PROTECTED for output buffers from native window
        kEnableGrallocUsageProtected = 128,

        // Secure decoding mode
        kUseSecureInputBuffers = 256,
    };	
};

}
#endif

