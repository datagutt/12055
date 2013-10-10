/************************************************************************************
** Copyright (C), 2000-2012, OPPO Mobile Comm Corp., Ltd
** All rights reserved.
** 
** VENDOR_EDIT
** 
** Description: - 
**      Implementation of Dirac HD effect
** 
** 
** --------------------------- Revision History: --------------------------------
** <author>		                      <data> 	<version >  <desc>
** ------------------------------------------------------------------------------
** guole@oppo.com	                     2011-10-26	  1.0       create file
** guole@oppo.com	                     2011-10-26	  1.1       port it to 11071
** Hongye.Jin@Pdt.Audio.AudioEffect&HAL  2012-06-19   1.2	    port it to 12021
** ------------------------------------------------------------------------------
**
************************************************************************************/


#define LOG_TAG "Diraccpp"
//#define LOG_NDEBUG 0
#include <cutils/log.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <new>
#include <utils/Errors.h>
#include <utils/threads.h>

#include "dirac.h"
#include "effect_Dirac.h"

//TuGuang@OnLineRD.MultiMediaService.AudioEffect, 2012/10/26, Add for [debug], {
#if 0 
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#define MAX_FILE_SIZE 100 * 1024 * 1024 //50M

static int writeDestFileLog(short* buf, int len, char* logtag, const int line) {
	ALOGW("__________DUMP PCM_________");
	char filename[1024];
	int ret = 0;
	char tagarray[128] = {0};
	char *tag = tagarray;
	int flag = 0;
	while(*logtag != '\0')
	{
		if(*logtag != ':')
		{
			*tag = *logtag;
		}
		else if(flag%2 == 0)
		{
			*tag = '_';
			flag ++;
		}
		else if(flag%2 != 0)
		{
			tag--;
		}
		logtag++;
		tag++;
	};
	snprintf(filename, sizeof(filename), "/data/local/%s_db_%d.pcm", tagarray, line);
	int fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0666);
	if (fd < 0)
	{
		perror("open");
		return (-1);
	}
	int size = lseek(fd, 0, SEEK_END);

	if(size < 0)
	{
		ALOGE("failed to seek to the end!!!");
		close(fd);
		return (-1);
	}
	else if(size < MAX_FILE_SIZE)
	{
		write(fd, buf, len);
	}
	else
	{
		ALOGE("file size is out of rang: %dM, donn't write into file!!!", MAX_FILE_SIZE / 1024 /1024);
	}
	close(fd);	
	
	return 0;
}
#define GJ_DUMP(x, y) writeDestFileLog(x, y, LOG_TAG, __LINE__) 
#else
#define GJ_DUMP(x, y)
#endif  
//} add end for [debug].



#define CHECK_ARG(cond) {                     \
    if (!(cond)) {                            \
        ALOGI("Invalid argument: "#cond);      \
        return -EINVAL;                       \
    }                                         \
}

#ifdef VENDOR_EDIT
//Guole@MultimediaSrv.AudioEffect, 2012/02/22, Add for that CPU Limit exceeded
#define DIRAC_CUP_LOAD_ARM9E   0    // Expressed in 0.1 MIPS
#endif /* VENDOR_EDIT */

#define ENABLE_DIRAC 1
#define ID 1

// effect_interface_t interface implementation for Dirac Live effect
extern "C" const struct effect_interface_s gDiracInterface;

enum dirac_state_e {
	DIRAC_STATE_INITIALIZED,
	DIRAC_STATE_UNINITIALIZED,
	DIRAC_STATE_ACTIVE,
};

#ifdef VENDOR_EDIT
//Guole@MultimediaSrv.AudioEffect, 2012/03/30, Add for changing dirac mode according to parent-thread's channel count
enum dirac_mode {
    DIRAC_MODE_MONO,
    DIRAC_MODE_STEREO_MIX,
    DIRAC_MODE_STEREO_NO_MIX,
    DIRAC_MODE_INVALID,
};
#endif /* VENDOR_EDIT */

//about volume
static const uint32_t peak  = 16777216; 
static const uint32_t peak1 = 10584064; 
static const uint32_t peak2 = 6676480;

#ifndef VENDOR_EDIT
//TuGuang@OnLineRD.MultiMediaService.AF, 2012/12/07, Modify for adapt 12025 frames to remove noise.
/*
static const int step = 2;
*/
#else /* VENDOR_EDIT */
static const int step = 3;
#endif /* VENDOR_EDIT */

static const int boost = 0x1800; // 3.5dB
static const int noBoost = 0x1000;
static const uint32_t shift = 12;

static inline int16_t clamp16(int32_t sample);
static inline void accumulateToOutput(int16_t *in, int16_t *out, size_t frameCnt);
static inline int32_t mulRL(int left, uint32_t inRL, uint32_t vRL);

/* local functions */
static inline int32_t mulRL(int left, uint32_t inRL, uint32_t vRL) {
#if defined(__arm__) && !defined(__thumb__)
    int32_t out;
    if (left) {
        asm( "smulbb %[out], %[inRL], %[vRL] \n"
             : [out]"=r"(out)
             : [inRL]"%r"(inRL), [vRL]"r"(vRL)
             : );
    } else {
        asm( "smultt %[out], %[inRL], %[vRL] \n"
             : [out]"=r"(out)
             : [inRL]"%r"(inRL), [vRL]"r"(vRL)
             : );
    }
    return out;
#else
    if (left) {
        return int16_t(inRL&0xFFFF) * int16_t(vRL&0xFFFF);
    } else {
        return int16_t(inRL>>16) * int16_t(vRL>>16);
    }
#endif
}

static inline void  accumulateToOutput(int16_t *in, int16_t *out, size_t frameCnt) {	
	for (size_t i = 0; i < frameCnt; i++) {
		out[i] = clamp16((int32_t)out[i] + (int32_t)in[i]);
	}
}

static inline int16_t clamp16(int32_t sample) {
    if ((sample >> 15) ^ (sample >> 31))
        sample = 0x7FFF ^ (sample >> 31);
    return sample;
}


namespace android {
//namespace {

//Dirac UUID: 4c6383e0-ff7d-11e0-b6d8-0002a5d5c51b
const effect_descriptor_t gDiracDescriptor = {
        {0x4c6383e0, 0xff7d, 0x11e0, 0xb6d8, {0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b}}, // type
        {0x4c6383e0, 0xff7d, 0x11e0, 0xb6d8, {0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b}}, // uuid
        EFFECT_CONTROL_API_VERSION,
#ifdef VENDOR_EDIT
//Guole@MultimediaSrv.AudioEffect, 2012/02/21, Add for fixing audio distortion
        ( EFFECT_FLAG_TYPE_INSERT | EFFECT_FLAG_INSERT_LAST | EFFECT_FLAG_VOLUME_IND),
#endif /* VENDOR_EDIT */
        DIRAC_CUP_LOAD_ARM9E,
        0,
        "Dirac HD",
        "OPPO",
};

struct DiracContext {
    const struct effect_interface_s *mItfe;
    effect_config_t mConfig;
    uint32_t mState;
    int mMode; // 0: mono; 1: stereo and mix; 2: stereo and no mix
    int mSessionId;
    volatile uint32_t mPreVolume;
    volatile uint32_t mVolume;
    volatile int mPreFactor;
    volatile int mFactor;
    volatile int mVolumeInc;
    volatile bool mEnabled;
};


//--- local function prototypes

//prototypes of local functions
static int DiracContext_init(DiracContext *pContext);
static int Dirac_configure(DiracContext *pContext, effect_config_t *pConfig);
static int Dirac_setParameter(DiracContext *pContext, int32_t *param, int32_t *value);
static int enableDirac(DiracContext *pContext, int enabled);




static int Dirac_setParameter(DiracContext *pContext, int32_t *param, int32_t *value) {
    if (*param == ENABLE_DIRAC) {
        return enableDirac(pContext, *value);
    }
    
    ALOGW("param not defined, param = %d ", *param);
    
    return -EINVAL;
}

static int enableDirac(DiracContext *pContext, int enabled) {
    if (enabled) {
        Dirac_Enable(ID);
        pContext->mEnabled = true;
        ALOGV("enable dirac..........");   
    } else {
        Dirac_Disable(4, ID);
        pContext->mEnabled = false;
        ALOGV("disable dirac..........");
    }

    return 0;
}
Mutex mDiracInitLock;
// initialize DiracContext
static int DiracContext_init(DiracContext *pContext) {
    ALOGV("DiracContext_init begin");
    #ifdef VENDOR_EDIT
    //Hongye.Jin@Prd.Audio.AudioEffect&HAL, 2012-06-19, use the new style
	pContext->mConfig.inputCfg.accessMode = EFFECT_BUFFER_ACCESS_READ;
	pContext->mConfig.inputCfg.channels = AUDIO_CHANNEL_OUT_STEREO;
	pContext->mConfig.inputCfg.format = AUDIO_FORMAT_PCM_16_BIT;
	pContext->mConfig.inputCfg.samplingRate = 44100;
	pContext->mConfig.inputCfg.bufferProvider.getBuffer = NULL;
	pContext->mConfig.inputCfg.bufferProvider.releaseBuffer = NULL;
	pContext->mConfig.inputCfg.bufferProvider.cookie = NULL;
	pContext->mConfig.inputCfg.mask = EFFECT_CONFIG_ALL;
	pContext->mConfig.outputCfg.accessMode = EFFECT_BUFFER_ACCESS_ACCUMULATE;
	pContext->mConfig.outputCfg.channels = AUDIO_CHANNEL_OUT_STEREO;
	pContext->mConfig.outputCfg.format = AUDIO_FORMAT_PCM_16_BIT;
	pContext->mConfig.outputCfg.samplingRate = 44100;
	pContext->mConfig.outputCfg.bufferProvider.getBuffer = NULL;
	pContext->mConfig.outputCfg.bufferProvider.releaseBuffer = NULL;
	pContext->mConfig.outputCfg.bufferProvider.cookie = NULL;
	pContext->mConfig.outputCfg.mask = EFFECT_CONFIG_ALL;
    //pContext->mState = DIRAC_STATE_UNINITIALIZED;//can't set in this
    pContext->mMode = DIRAC_MODE_INVALID;
    //pContext->mSessionId 
    pContext->mPreVolume = 0;
    pContext->mVolume = 0;
    pContext->mPreFactor = noBoost;
    pContext->mFactor = noBoost;
    pContext->mVolumeInc = 0;
	pContext->mEnabled = false;
    #endif /* VENDOR_EDIT */

    ALOGV("DiracContext_init want to init Dirac");
    mDiracInitLock.lock();
    ESelectedFilter initFilter = R1150268_48k;
    if(Dirac_queryInitializedFilter(ID) != initFilter){    
        ALOGV("Dirac_Initialize begin");
        int ret = Dirac_Initialize(initFilter, ID);
        ALOGV("Dirac_Initialize end ret:%d",ret);
        if (ret == 1){
    		ALOGE("init dirac Dirac_Initialize() failed,unknow error");
			mDiracInitLock.unlock();
    		return UNKNOWN_ERROR; 
        } 
        ALOGV("DiracContext_init end");
    }else{
        ALOGV("Dirac_Initialize is initalized");
    }
    ALOGV("DiracContext_init init end");
    mDiracInitLock.unlock();
	return Dirac_configure(pContext, &pContext->mConfig);
}

static int Dirac_configure(DiracContext *pContext, effect_config_t *pConfig) {
	ALOGV("Dirac_configure start");

    CHECK_ARG(pContext != NULL);
    CHECK_ARG(pConfig != NULL);

    CHECK_ARG(pConfig->inputCfg.samplingRate == pConfig->outputCfg.samplingRate);
    CHECK_ARG(pConfig->inputCfg.channels == pConfig->outputCfg.channels);
    CHECK_ARG(pConfig->inputCfg.format == pConfig->outputCfg.format);
    CHECK_ARG((pConfig->inputCfg.channels == AUDIO_CHANNEL_OUT_MONO)
    			|| (pConfig->inputCfg.channels == AUDIO_CHANNEL_OUT_STEREO));
    CHECK_ARG(pConfig->outputCfg.accessMode == EFFECT_BUFFER_ACCESS_WRITE
				|| pConfig->outputCfg.accessMode == EFFECT_BUFFER_ACCESS_ACCUMULATE);
    CHECK_ARG(pConfig->inputCfg.format == AUDIO_FORMAT_PCM_8_24_BIT
				|| pConfig->inputCfg.format == AUDIO_FORMAT_PCM_16_BIT);

    if (pConfig->inputCfg.channels == AUDIO_CHANNEL_OUT_MONO) {
        pContext->mMode= DIRAC_MODE_MONO;
    } else {
        pContext->mMode= DIRAC_MODE_STEREO_NO_MIX;
    }
	ALOGI("dirac mode=%d", pContext->mMode);

    memcpy(&pContext->mConfig, pConfig, sizeof(effect_config_t));
    //enableDirac(pContext, true);
	//pContext->mState = DIRAC_STATE_INITIALIZED;//can't set in this
	
    ALOGV("Dirac_configure end");
    return 0;
}


//
//--- Effect Library Interface Implementation
//

extern "C" int EffectQueryNumberEffects(uint32_t *pNumEffects) {
	if (pNumEffects == NULL){
		return -EINVAL;
	}
	ALOGD("dirac number 1");
    *pNumEffects = 1;
    return 0;
} /* end EffectQueryNumberEffects */

extern "C" int EffectQueryEffect(uint32_t index,
                                 effect_descriptor_t *pDescriptor) {
    if (pDescriptor == NULL) {
        return -EINVAL;
    }
    if (index > 0) {
        return -EINVAL;
    }
    memcpy(pDescriptor, &gDiracDescriptor, sizeof(effect_descriptor_t));
    return 0;
} /* end EffectQueryNext */

extern "C" int EffectCreate(const effect_uuid_t *uuid,
                            int32_t sessionId,
                            int32_t ioId,
                            effect_handle_t *pHandle) {
    int ret;
    int i;

    ALOGV("EffectLibCreateEffect start");

    if (pHandle == NULL || uuid == NULL) {
        return -EINVAL;
    }

    if (memcmp(uuid, &gDiracDescriptor.uuid, sizeof(effect_uuid_t)) != 0) {
        return -EINVAL;
    }

    DiracContext *pContext = new DiracContext;
    if(pContext == NULL) {
        ALOGE("EffectCreate failed: no more memory, sessionId %d", sessionId);
        return -EINVAL;
    }

    pContext->mItfe = &gDiracInterface;
    pContext->mSessionId = sessionId;
    pContext->mState = DIRAC_STATE_UNINITIALIZED;
	
    
    ret = DiracContext_init(pContext);
    
	if (ret != 0) {
		ALOGE("Dirac no filter initialized, sessionId %d", sessionId);
		delete pContext;
		pContext = NULL;
		return -EINVAL;
	}

    *pHandle = (effect_handle_t)pContext;
    pContext->mState = DIRAC_STATE_INITIALIZED;

    ret=enableDirac(pContext, true);

    ALOGI("Dirac EffectCreate %p, sessionId %d", pContext, pContext->mSessionId);

    return 0;

} /* end EffectCreate */

extern "C" int EffectRelease(effect_handle_t handle) {
    DiracContext *pContext = (DiracContext *)handle;

    if (pContext == NULL) {
        return -EINVAL;
    }
    ALOGD("Dirac EffectRelease %p, sessionId %d", handle, pContext->mSessionId);
    pContext->mState = DIRAC_STATE_UNINITIALIZED;
    //enableDirac(pContext, false);
    delete pContext;
    pContext == NULL;

    return 0;
} /* end EffectRelease */

extern "C" int EffectGetDescriptor(const effect_uuid_t       *uuid,
                                   effect_descriptor_t *pDescriptor) {

    if (pDescriptor == NULL || uuid == NULL){
        ALOGV("EffectGetDescriptor() called with NULL pointer");
        return -EINVAL;
    }

    if (memcmp(uuid, &gDiracDescriptor.uuid, sizeof(effect_uuid_t)) == 0) {
        memcpy(pDescriptor, &gDiracDescriptor, sizeof(effect_descriptor_t));
        return 0;
    }

    return  -EINVAL;
} /* end EffectGetDescriptor */

//} // namespace
} // namespace


//
//--- Effect Control Interface Implementation
//

extern "C" int Dirac_process(effect_handle_t self,
							audio_buffer_t *inBuffer,
							audio_buffer_t *outBuffer) {
    android::DiracContext *pContext = (android::DiracContext *)self;

    if (pContext == NULL) {
        ALOGE("error pContext == NULL");
        return -EINVAL;
    }
    if (inBuffer == NULL || inBuffer->s16 == NULL ||
        outBuffer == NULL || outBuffer->s16 == NULL ||
        inBuffer->frameCount != outBuffer->frameCount) {
        ALOGE("error inBuffer or outBuffer");
        return -EINVAL;
    }
    if (pContext->mState == DIRAC_STATE_UNINITIALIZED) {
        ALOGE("error pContext->mState == DIRAC_STATE_UNINITIALIZED");
        return -EINVAL;
    }
    
    #ifdef VENDOR_EDIT
    //TuGuang@OnLineRD.MultiMediaService.AF, 2012/11/07, Add for can't turn off when disable
    if (pContext->mState != DIRAC_STATE_ACTIVE) {
        ALOGE("pContext->mState != DIRAC_STATE_ACTIVE");
        return -ENODATA;    
	}
    #endif /* VENDOR_EDIT */
    
    
    #ifdef VENDOR_EDIT
    //Guole@MultimediaSrv.AudioEffect, 2012/05/07, Add for checking current mode
    if (pContext->mMode == DIRAC_MODE_INVALID) {
        ALOGE("pContext->mState != DIRAC_STATE_ACTIVE");
        return -EINVAL;
    }
    #endif /* VENDOR_EDIT */

    //GJ_DUMP(inBuffer->s16, inBuffer->frameCount * 2*2);
    Dirac_Convert(inBuffer->s16, inBuffer->s16, inBuffer->frameCount * 2, pContext->mMode, 0, ID);
    //ALOGW("Dirac_process in:%p,out:%p,samples:%d", inBuffer->s16, inBuffer->s16, inBuffer->frameCount);
    //GJ_DUMP(inBuffer->s16, inBuffer->frameCount * 2*2);

    // Dirac will attenuate the volume, so here boosts the volume after dirac processing
    /*
    if (pContext->mEnabled) {
        const short *in = inBuffer->s16;
        int *out = (int *)inBuffer->s32;
        int frames = inBuffer->frameCount;
        int factor = pContext->mFactor;
        int preFactor = pContext->mPreFactor;
        uint32_t vrl = (factor << 16) | (factor & 0xFFFF);
        do {
            // ramp volume if necessary
            if (pContext->mVolumeInc != 0) {
                if ((pContext->mVolumeInc > 0 && preFactor < factor - step)
                    || (pContext->mVolumeInc < 0 && preFactor > factor - step)) {
                    preFactor += pContext->mVolumeInc;
                    vrl = (preFactor << 16) | (preFactor & 0xFFFF);
                } else {
                    pContext->mVolumeInc = 0;
                    vrl = (factor << 16) | (factor & 0xFFFF);;
                    ALOGW("volumeInc %d, volume %d", pContext->mVolumeInc, vrl);
                }
            }

            uint32_t rl = *reinterpret_cast<uint32_t const *>(in);
            in += 2;
            int32_t l = mulRL(1, rl, vrl) >> shift;
            int32_t r = mulRL(0, rl, vrl) >> shift;

            // It is very possible that the sample value overflows, so clamp it.
            l = clamp16(l);
            r = clamp16(r);
            *out++ = (r << 16) | (l & 0xFFFF);
        } while (--frames);
    }
   */

    // Do accumulation only when inBuffer->s16 != outBuffer->s16
    if (inBuffer->s16 != outBuffer->s16) {
	    accumulateToOutput(inBuffer->s16,outBuffer->s16,inBuffer->frameCount * 2);
    }
    
    return 0;
}   // end Dirac_process

extern "C" int Dirac_command(effect_handle_t self,
							uint32_t cmdCode,
							uint32_t cmdSize,
        					void *pCmdData,
        					uint32_t *replySize,
        					void *pReplyData) {
    android::DiracContext *pContext = (android::DiracContext *)self;
    int retsize;

    if (pContext == NULL || pContext->mState == DIRAC_STATE_UNINITIALIZED) {
        ALOGE("Dirac_command,pContext->mState == DIRAC_STATE_UNINITIALIZED");
        return -EINVAL;
    }

    ALOGI("Dirac_command command %d cmdSize %d",cmdCode, cmdSize);

    switch (cmdCode) {
    case EFFECT_CMD_INIT:
        if (pReplyData == NULL || *replySize != sizeof(int)) {
            return -EINVAL;
        }
        *(int *)pReplyData = android::DiracContext_init(pContext);
        ALOGI("Dirac EFFECT_CMD_INIT() OK");
        break;
    case EFFECT_CMD_SET_CONFIG:
        if (pCmdData == NULL || cmdSize != sizeof(effect_config_t)
                || pReplyData == NULL || *replySize != sizeof(int)) {
            return -EINVAL;
        }
        *(int *) pReplyData = android::Dirac_configure(pContext, (effect_config_t *) pCmdData);
        break;
    case EFFECT_CMD_RESET:
        android::Dirac_configure(pContext, &pContext->mConfig);
        break;
    case EFFECT_CMD_ENABLE:
        if (pReplyData == NULL || *replySize != sizeof(int)) {
        	ALOGE("Dirac enable arg1 error");
            return -EINVAL;
        }
        if (pContext->mState != DIRAC_STATE_INITIALIZED) {
        	ALOGE("Dirac enable arg2 error");
            return -ENOSYS;
        }
        if (Dirac_queryInitializedFilter(ID) == -1) {
        	ALOGE("dirac enable error: NO FILTER ENABLED!!!");
        	return -ENOSYS;
        }

        //enableDirac(pContext, true);
        pContext->mState = DIRAC_STATE_ACTIVE;
        ALOGI("Dirac EFFECT_CMD_ENABLE() OK, volume = %u", pContext->mVolume);
        *(int *)pReplyData = 0;
        break;
    case EFFECT_CMD_DISABLE:
        if (pReplyData == NULL || *replySize != sizeof(int)) {
        	ALOGE("disable arg1 error");
            return -EINVAL;
        }
        if (pContext->mState != DIRAC_STATE_ACTIVE) {
        	ALOGE("Dirac disable arg2 error");
            return -ENOSYS;
        }
        if (Dirac_queryInitializedFilter(ID) == -1) {
        	ALOGE("dirac disable error");
        	return -ENOSYS;
        }
        
        //enableDirac(pContext, false);
        pContext->mState = DIRAC_STATE_INITIALIZED;
        ALOGI("Dirac EFFECT_CMD_DISABLE() OK");
        *(int *)pReplyData = 0;
        break;
    case EFFECT_CMD_SET_PARAM: {
        ALOGV("Dirac_command EFFECT_CMD_SET_PARAM cmdSize %d pCmdData %p, *replySize %d, pReplyData %p", cmdSize, pCmdData, *replySize, pReplyData);
        if (pCmdData == NULL || cmdSize < (int)(sizeof(effect_param_t) + sizeof(int32_t) * 2) ||
            pReplyData == NULL || *replySize != sizeof(int32_t)) {
            return -EINVAL;
        }
        effect_param_t *p = (effect_param_t *) pCmdData;
        *(int *)pReplyData = android::Dirac_setParameter(pContext, (int32_t *)p->data,
                (int32_t *)(p->data + p->psize));
        } 
        break;
	case EFFECT_CMD_SET_PARAM_DEFERRED:
	case EFFECT_CMD_SET_PARAM_COMMIT:
	case EFFECT_CMD_GET_PARAM:
		break;
	case EFFECT_CMD_SET_DEVICE:
		// TODO: alter dirac mode depending on output device
		ALOGI("audio device has changed");
		break;
	case EFFECT_CMD_SET_VOLUME:
	    pContext->mPreVolume = pContext->mVolume;
	    pContext->mVolume = (*(uint32_t *)pCmdData);
	    if (pContext->mPreVolume < peak1 && pContext->mVolume >= peak1) {
	        pContext->mVolumeInc = -step;
	        pContext->mFactor = noBoost;
	        pContext->mPreFactor = boost;
	    } else if (pContext->mPreVolume >= peak1 && pContext->mVolume < peak1) {
            pContext->mVolumeInc = step;
            pContext->mFactor = boost;
            pContext->mPreFactor = noBoost;
	    } else if (pContext->mPreVolume < peak1 && pContext->mVolume < peak1) {
	        pContext->mVolumeInc = 0;
	        pContext->mFactor = boost;
	    } else if (pContext->mPreVolume >= peak1 && pContext->mVolume >= peak1) {
	        pContext->mVolumeInc = 0;
	        pContext->mFactor = noBoost;
	    } else{
            pContext->mVolumeInc = 0;
            ALOGE("never be here!!!");
	    }
	    ALOGD("ssss preVolume = %d, volume = %u, volumeInc = %d", pContext->mPreVolume, pContext->mVolume, pContext->mVolumeInc);
		break;
	case EFFECT_CMD_SET_AUDIO_MODE:
		ALOGI("audio mode has changed");
		break;
	//case EFFECT_CMD_CLEARBUF:
	//	break;
	case EFFECT_CMD_FIRST_PROPRIETARY:
		// TODO:
        break;
    default:
        ALOGW("Dirac_command invalid command %d",cmdCode);
        return -EINVAL;
    }

    return 0;
}

extern "C" int Dirac_getDescriptor(effect_handle_t   self,
                                    effect_descriptor_t *pDescriptor)
{
    android::DiracContext * pContext = (android::DiracContext *) self;

    if (pContext == NULL || pDescriptor == NULL) {
        ALOGV("Dirac_getDescriptor() invalid param");
        return -EINVAL;
    }

    memcpy(pDescriptor, &android::gDiracDescriptor, sizeof(effect_descriptor_t));

    return 0;
}

// effect_handle_t interface implementation for dirac effect
extern "C" const struct effect_interface_s gDiracInterface = {
        Dirac_process,
        Dirac_command,
        Dirac_getDescriptor,
        NULL
};


audio_effect_library_t AUDIO_EFFECT_LIBRARY_INFO_SYM = {
    tag : AUDIO_EFFECT_LIBRARY_TAG,
    version : EFFECT_LIBRARY_API_VERSION,
    name : "OPPO Dirac Library",
    implementor : "oppo Software Ltd.",
    query_num_effects : android::EffectQueryNumberEffects,
    query_effect : android::EffectQueryEffect,
    create_effect : android::EffectCreate,
    release_effect : android::EffectRelease,
    get_descriptor : android::EffectGetDescriptor,
};
