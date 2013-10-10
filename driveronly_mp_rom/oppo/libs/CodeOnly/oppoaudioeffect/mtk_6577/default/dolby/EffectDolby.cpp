/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
 /******************************************************************************
 * Copyright 2008-2011 OPPO Mobile Comm Corp., Ltd, All rights reserved.
 * FileName:EffectDolby.cpp
 * ModuleName:libdolby
 * Author:Popeye
 * Creat Data: 2011-12-15
 * Description:dolby engneer interface
 * History
 * version       time         		author     	description
 	1.0    	    2012-12-15     	    Popeye      rebuilt the interface  
 ******************************************************************************/
//order:create-command-process

#define LOG_TAG "EffectDolby"
//#define LOG_NDEBUG 0
#include <cutils/log.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <new>
//#include <media/EffectApi.h>
#include <hardware/audio_effect.h>
#include "EffectDolby.h"

namespace android {

#include <sys/time.h> 
static int64_t getCurSystemTimeUs()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (int64_t)tv.tv_sec * 1000000 + tv.tv_usec;
}

#if 0 //for debug
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#define MAX_FILE_SIZE 100 * 1024 * 1024 //50M

static int writeDestFileLog(short* buf, int len, char* logtag, const int line) 
{
	MY_LOOP_LOGW("__________GJ DUMP PCM_________")
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
		MY_LOOP_LOGW("file size is out of rang: %dM, donn't write into file!!!", MAX_FILE_SIZE / 1024 /1024);
	}
	close(fd);	
	
	return 0;
}
#define GJ_DUMP(x, y) writeDestFileLog(x, y, LOG_TAG, __LINE__) 
#else
#define GJ_DUMP(x, y)
#endif


// effect_interface_t interface implementation for DolbyAudio effect
extern "C" const struct effect_interface_s gDolbyAudioInterface;

// Dolby AudioEffect UUID
const effect_descriptor_t gDolbyAudioDescriptor = {
        {0x4721c740, 0xfe36, 0x11e0, 0xa358, {0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b}}, // type
        {0xefaeb540, 0x038a, 0x11e1, 0xb36f, {0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b}}, // uuid Eq NXP
        EFFECT_CONTROL_API_VERSION,
        (EFFECT_FLAG_TYPE_INSERT | EFFECT_FLAG_INSERT_LAST | EFFECT_FLAG_DEVICE_IND
        ),
        220,
        25,
        "DolbyAudio",
        "oppo Software Ltd.",
};

//FOR SET SPKEQ
int sample_rates[TDAS_N_SAMPLERATES]= {SAMPLERATE_48000, SAMPLERATE_44100,SAMPLERATE_32000, SAMPLERATE_24000};
char speaker_eq_coef_files[TDAS_N_SAMPLERATES][64] = {
#if 0	
{"sdcard/coefs_48000.bin"},
{"sdcard/coefs_44100.bin"},
{"sdcard/coefs_32000.bin"},
{"sdcard/coefs_24000.bin"}};
#else
{"system/etc/dolby_coef/coefs_48000.bin"},
{"system/etc/dolby_coef/coefs_48000.bin"},
{"system/etc/dolby_coef/coefs_48000.bin"},
{"system/etc/dolby_coef/coefs_48000.bin"}};
#endif

enum tdasaudio_state_e {
    TDASAUDIO_STATE_UNINITIALIZED,
    TDASAUDIO_STATE_INITIALIZED,
    TDASAUDIO_STATE_ACTIVE,
};
 
struct DolbyAudioContext {
    const struct effect_interface_s *mItfe;
    effect_config_t     mConfig;
    uint32_t            mState;
    DolbyAudioClient*   mpTdas;
    short*              mWorkBuffer;
    int                 mFrameCount;
};


#define CHECK_ARG2(cond) {                    \
    if (!(cond)) {                            \
        ALOGV("Invalid argument: "#cond);      \
        return -EINVAL;                       \
    }                                         \
}

int AudioTdas_configure(DolbyAudioContext *pContext, effect_config_t *pConfig, bool reconfig){
    ALOGV("AudioTdas_configure start");

    if (pConfig->inputCfg.samplingRate != pConfig->outputCfg.samplingRate) return -EINVAL;
    if (pConfig->inputCfg.channels != pConfig->outputCfg.channels) return -EINVAL;
    if (pConfig->inputCfg.format != pConfig->outputCfg.format) return -EINVAL;
    if (pConfig->inputCfg.channels != AUDIO_CHANNEL_OUT_STEREO) return -EINVAL;
    if (pConfig->outputCfg.accessMode != EFFECT_BUFFER_ACCESS_WRITE &&
            pConfig->outputCfg.accessMode != EFFECT_BUFFER_ACCESS_ACCUMULATE) return -EINVAL;
    if (pConfig->inputCfg.format != AUDIO_FORMAT_PCM_16_BIT) return -EINVAL;

    memcpy(&pContext->mConfig, pConfig, sizeof(effect_config_t));

    if(reconfig) {
    
    #ifndef VENDOR_EDIT
    //TuGuang@OnLineRD.MultiMediaService.AF, 2012/11/29, Remove for AudioEffec.setEnabled to
    //keep the last parametes.
    /*
    pContext->mpTdas->closeDolby();
    pContext->mpTdas->openDolby(pConfig->inputCfg.samplingRate);
    */
    #endif /* VENDOR_EDIT */
          
    }

    if(pConfig->inputCfg.samplingRate != pContext->mpTdas->getInputSamplerate()){
        pContext->mpTdas->setInputSamplerate(pConfig->inputCfg.samplingRate);
    }
    ALOGV("AudioTdas_configure end");

    return 0;
}

int AudioTdas_init(DolbyAudioContext *pContext){
    ALOGD("AudioTdas_init start");
    CHECK_ARG2(pContext != NULL);

    if (pContext->mpTdas != NULL) {
        ALOGE("pContext->mpTdas != NULL");
        delete pContext->mpTdas;
        pContext->mpTdas = NULL;
    }
    
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

    DolbyAudioClient* pTdas = new DolbyAudioClient;
    if(pTdas->openDolby(44100) != NO_ERROR){
        ALOGE("AudioTdas_init openDolby failure!!!");
        delete pTdas;
        return UNKNOWN_ERROR;
    }
    pContext->mpTdas = pTdas;

    AudioTdas_configure(pContext, &pContext->mConfig, false);

    return 0;
}

//
//--- Effect Library Interface Implementation
//

extern "C" {

int EffectQueryNumberEffects(uint32_t *pNumEffects) {
    ALOGV("Enter TdasAudio: EffectQueryNumberEffects");
    *pNumEffects = 1;
    return 0;
}

int EffectQueryEffect(uint32_t index, effect_descriptor_t *pDescriptor) {
	ALOGV("Enter TdasAudio: EffectQueryEffect");
    if (pDescriptor == NULL) {
        return -EINVAL;
    }
    if (index > 0) {
        return -EINVAL;
    }
    memcpy(pDescriptor, &gDolbyAudioDescriptor, sizeof(effect_descriptor_t));
    return 0;
}

int EffectCreate(const effect_uuid_t *uuid,
        int32_t sessionId,
        int32_t ioId,
        effect_handle_t *pInterface) {
    int ret;
    int i;
    ALOGV("EffectCreate");
    if (pInterface == NULL || uuid == NULL) {
        return -EINVAL;
    }

    if (memcmp(uuid, &gDolbyAudioDescriptor.uuid, sizeof(effect_uuid_t)) != 0) {
        return -EINVAL;
    }

    DolbyAudioContext *pContext = new DolbyAudioContext;
    pContext->mItfe = &gDolbyAudioInterface;
    pContext->mpTdas = NULL;
    pContext->mState = TDASAUDIO_STATE_UNINITIALIZED;
    pContext->mWorkBuffer = NULL;
    pContext->mFrameCount = -1;

    ret = AudioTdas_init(pContext);
    if (ret < 0) {
        ALOGW("EffectCreate() init failed");
        delete pContext;
        return ret;
    }

    *pInterface = (effect_handle_t)pContext;
    pContext->mState = TDASAUDIO_STATE_INITIALIZED;

    ALOGV("EffectCreate %p", pContext);
    return 0;
}

int EffectRelease(effect_handle_t interfacee) {
    DolbyAudioContext * pContext = (DolbyAudioContext *)interfacee;
    ALOGV("EffectRelease %p", interfacee);
    if (pContext == NULL) {
        return -EINVAL;
    }
    pContext->mState = TDASAUDIO_STATE_UNINITIALIZED;
    pContext->mpTdas->closeDolby();
    delete pContext->mpTdas;
    if(pContext->mWorkBuffer)
        free(pContext->mWorkBuffer);
    delete pContext;

    return 0;
}

int EffectGetDescriptor(const effect_uuid_t       *uuid,	effect_descriptor_t *pDescriptor) 
{
	ALOGV("dolby________________________EffectGetDescriptor____");
    if (pDescriptor == NULL || uuid == NULL){
        ALOGW("EffectGetDescriptor() called with NULL pointer");
        return -EINVAL;
    }

    if (memcmp(uuid, &gDolbyAudioDescriptor.uuid, sizeof(effect_uuid_t)) == 0) {
        memcpy(pDescriptor, &gDolbyAudioDescriptor, sizeof(effect_descriptor_t));
        return 0;
    }

    return  -EINVAL;
} /* end EffectGetDescriptor */

//
//--- Effect Control Interface Implementation
//
static inline int16_t clamp16b(int32_t sample)
{
    if ((sample>>15) ^ (sample>>31))
        sample = 0x7FFF ^ (sample>>31);
    return sample;
}

int AudioTdas_process(
        effect_handle_t self,audio_buffer_t *inBuffer, audio_buffer_t *outBuffer)
{
    android::DolbyAudioContext * pContext = (android::DolbyAudioContext *)self;
    short* pOutTmp;
    int fc;
    
    if (pContext == NULL) {
        return -EINVAL;
    }

    if (inBuffer == NULL || inBuffer->raw == NULL ||
        outBuffer == NULL || outBuffer->raw == NULL ||
        inBuffer->frameCount != outBuffer->frameCount ||
        inBuffer->frameCount == 0) {
        return -EINVAL;
    }

    if (pContext->mState != TDASAUDIO_STATE_ACTIVE) {
        ALOGE("AudioTdas_process not in active state!");
        return -ENODATA;
    }

    fc = inBuffer->frameCount;

    if (pContext->mConfig.outputCfg.accessMode == EFFECT_BUFFER_ACCESS_WRITE){
        pOutTmp = outBuffer->s16;
    }else if (pContext->mConfig.outputCfg.accessMode == EFFECT_BUFFER_ACCESS_ACCUMULATE){
        if (pContext->mFrameCount != fc) {
            if (pContext->mWorkBuffer != NULL) {
                free(pContext->mWorkBuffer);
            }
            pContext->mWorkBuffer = (short *)malloc(fc * 2 * 2);
            pContext->mFrameCount = fc;
        }
        pOutTmp = pContext->mWorkBuffer;
    }else{
        ALOGE("AudioTdas_process : invalid access mode");
        return -EINVAL;
    }

    pContext->mpTdas->process(inBuffer->s16, pOutTmp, inBuffer->frameCount);

	//the problem may be happened here!!!
    if (pContext->mConfig.outputCfg.accessMode == EFFECT_BUFFER_ACCESS_ACCUMULATE){
        //short* pOut = outBuffer->s16; //modify by maojg 2011-09-28
        for (int i=0; i<fc*2; i++){
            //pOut[i] = clamp16b((int32_t)pOut[i] + (int32_t)pOutTmp[i]);
            //modify by maojg 2011-09-28
			outBuffer->s16[i] = clamp16b((int32_t)outBuffer->s16[i] + (int32_t)pOutTmp[i]);
        }
    }

    return 0;
}   // end AudioTdas_process

int AudioTdas_command(effect_handle_t self, uint32_t cmdCode, uint32_t cmdSize,
        void *pCmdData, uint32_t *replySize, void *pReplyData) {

    android::DolbyAudioContext * pContext = (android::DolbyAudioContext *)self;
    int retsize;

    if (pContext == NULL || pContext->mState == TDASAUDIO_STATE_UNINITIALIZED) {
        return -EINVAL;
    }

    
    ALOGV("FUNC:%s(),line:%d,command:%d,cmdSize:%d",__FUNCTION__,__LINE__,cmdCode, cmdSize);
    
    
    MY_LOOP_LOGV("AudioTdas_command command %d cmdSize %d",cmdCode, cmdSize);

    switch (cmdCode) {
    case EFFECT_CMD_INIT:
        if (pReplyData == NULL || *replySize != sizeof(int)) {
            return -EINVAL;
        }
        //*(int *) pReplyData = AudioTdas_init(pContext);
        *(int *) pReplyData = 0;
        break;
    case EFFECT_CMD_SET_CONFIG:
        if (pCmdData == NULL || cmdSize != sizeof(effect_config_t)
                || pReplyData == NULL || *replySize != sizeof(int)) {
            return -EINVAL;
        }
        *(int *) pReplyData = AudioTdas_configure(pContext, (effect_config_t *) pCmdData, false);
        break;
    case EFFECT_CMD_RESET:
        AudioTdas_configure(pContext, &pContext->mConfig, true);
        break;
    case EFFECT_CMD_ENABLE:
		//pContext->mpTdas->SetSoundEffectParameter(TDAS_BUNDLE, TDAS_BYPASS, 0);
		//pContext->mpTdas->mIsByPasson = false;
		//ALOGV("disable the tdas_bypass");
		
        if (pReplyData == NULL || *replySize != sizeof(int)) {
            return -EINVAL;
        }
        if (pContext->mState != TDASAUDIO_STATE_INITIALIZED) {
            return -ENOSYS;
        }
        pContext->mState = TDASAUDIO_STATE_ACTIVE;
        ALOGV("EFFECT_CMD_ENABLE() OK");
        *(int *)pReplyData = 0;
        break;
    case EFFECT_CMD_DISABLE:		
		//pContext->mpTdas->SetSoundEffectParameter(TDAS_BUNDLE, TDAS_BYPASS, 1);
		//pContext->mpTdas->mIsByPasson = true;
		//ALOGV("enable the tdas_bypass");
		
        if (pReplyData == NULL || *replySize != sizeof(int)) {
            return -EINVAL;
        }
        if (pContext->mState != TDASAUDIO_STATE_ACTIVE) {
            return -ENOSYS;
        }
        pContext->mState = TDASAUDIO_STATE_INITIALIZED;
        ALOGV("EFFECT_CMD_DISABLE() OK");
        *(int *)pReplyData = 0;
        break;
    case EFFECT_CMD_GET_PARAM: {
        if (pCmdData == NULL || cmdSize < (int)(sizeof(effect_param_t) + sizeof(int32_t) * 2) ||
            pReplyData == NULL || *replySize < (int)(sizeof(effect_param_t) + sizeof(int32_t) * 2)) {
            return -EINVAL;
        }
        
        effect_param_t *p = (effect_param_t *)pCmdData;//ÖžÏòÊäÈëÊýŸÝ
        if (p->psize != 8) {
            ALOGE("EFFECT_CMD_GET_PARAM only support two params");
            p->status = -EINVAL;
            break;
        }
        memcpy(pReplyData, pCmdData, sizeof(effect_param_t) + p->psize);
        p = (effect_param_t *)pReplyData;//ÖžÏòÊä³öÊýŸÝ
        p->status = 0;
        int voffset = ((p->psize - 1) / sizeof(int32_t) + 1) * sizeof(int32_t);
        int ret;
       
        ret = pContext->mpTdas->GetSoundEffectParameter(*(int*)p->data, *(int*)(p->data + 4));
        ALOGV("GET_PARAM para0:%d,param1:%d,ret:%d",*(int*)p->data,*(int*)(p->data + 4),ret);
        *((int32_t *)(p->data + voffset)) = ret;
        p->vsize = sizeof(int32_t);
        *replySize = sizeof(effect_param_t) + voffset + p->vsize;
        } break;
    case EFFECT_CMD_SET_PARAM: {
        if (pCmdData == NULL ||
            cmdSize < (int)(sizeof(effect_param_t) + sizeof(uint32_t) + sizeof(uint32_t)) ||
            pReplyData == NULL || *replySize != sizeof(int32_t)) {
            return -EINVAL;
        }
        effect_param_t *p = (effect_param_t *)pCmdData;
        if ((p->vsize != 4 && p->vsize != 8) || p->psize != 4) {
            ALOGE("EFFECT_CMD_GET_PARAM only support two or three params");
            *(int32_t *)pReplyData = -EINVAL;
            break;
        }
        int ret = 0;
        
        ALOGV("SET_PARAM p->psize:%d,p->vsize:%d",p->psize,p->vsize);

        ret = pContext->mpTdas->paramProcessSwitch(*(int*)p->data, 
            (int*)(p->data +4), p->vsize / 4);
        
        *(int32_t *)pReplyData = ret;
        } break;
    case EFFECT_CMD_SET_VOLUME:{
        if (pCmdData == NULL || cmdSize != 2 * sizeof(uint32_t)) {
            ALOGE("EFFECT_CMD_SET_VOLUME: ERROR");
            return -EINVAL;
        }
        // if pReplyData is NULL, VOL_CTRL is delegated to another effect
       if(pReplyData == NULL){
           pContext->mpTdas->SoundEffectEnable(TDAS_VOL,0);
           break;
       }
       pContext->mpTdas->SoundEffectEnable(TDAS_VOL,1);
       *(uint32_t *)pReplyData = (1 << 24);
       *((uint32_t *)pReplyData + 1) = (1 << 24);
        //×¢ÒâµœÔÚAudioTrackµÄcreateTrackº¯ÊýÖÐ,mCblk->volume[0/1]=0x1000=4096=2xy12
       float left = (float)(*(uint32_t *)pCmdData) / (1 << 24);//ÕâÀïµœµ×ÊÇÓŠžÃ³ýÒÔ(1<<24)»¹ÊÇ(1<<12)ÄØ?
       float right = (float)(*((uint32_t *)pCmdData + 1)) / (1 << 24);

       ALOGV("EFFECT_CMD_SET_VOLUME left:%f,right:%f", left, right);
       pContext->mpTdas->setDolbyVol((left + right) / 2);
       }break;
    case EFFECT_CMD_SET_DEVICE:
    case EFFECT_CMD_SET_AUDIO_MODE:
        ALOGW("audiotdas_command unsupport command %d",cmdCode);
        break;

    default:
        ALOGW("audiotdas_command invalid command %d",cmdCode);
        return -EINVAL;
    }

    return 0;
}

int AudioTdas_getDescriptor(effect_handle_t   self,	effect_descriptor_t *pDescriptor)
{
    DolbyAudioContext * pContext = (DolbyAudioContext *)self;

    if (pContext == NULL || pDescriptor == NULL) {
        ALOGW("AudioTdas_getDescriptor() invalid param");
        return -EINVAL;
    }

    memcpy(pDescriptor, &gDolbyAudioDescriptor, sizeof(effect_descriptor_t));

    return 0;
}

// effect_interface_t interface implementation for dolby effect
const struct effect_interface_s gDolbyAudioInterface = {
        AudioTdas_process,
        AudioTdas_command,
        AudioTdas_getDescriptor,
        NULL
};

audio_effect_library_t AUDIO_EFFECT_LIBRARY_INFO_SYM = {
    tag : AUDIO_EFFECT_LIBRARY_TAG,
    version : EFFECT_LIBRARY_API_VERSION,
    name : "dolby Library",
    implementor : "OPPO Software Ltd.",
    query_num_effects : EffectQueryNumberEffects,
    query_effect : EffectQueryEffect,
    create_effect : EffectCreate,
    release_effect : EffectRelease,
    get_descriptor : EffectGetDescriptor,
};

}

#define DM3_MIN_PROCESS_SAMPLES 32
const uint32_t NUM_EQ_MODE = 16;

#ifndef INTEGER
typedef short INTEGER;
#endif

DolbyAudioClient::DolbyAudioClient():
    mIsByPasson(false),
    mTdasHandle(NULL),
    mInputSamplerate(DEFAULT_INPUT_SAMPLERATE), 
    mVolume(-1),
    mDolbyEnable(1),

    mSwitchOnDolbyActionFlag(0)
     
{
    mMediaType = TYPE_MUSIC;
    mPlayMode = MODE_SPEAKER;
    mGEQType = GEQ_INVALID;
    mNBLevel = LEVEL_NORMAL;
    mHFELevel = LEVEL_NORMAL;
    mSSHLevel = LEVEL_NORMAL;
    
    mCurrentPlayMode = MODE_INVALID;
    mCurrentGEQType= GEQ_INVALID;
    
    mIsMSREnable = 0;
    
    memset(&mEffectEnable,0,sizeof(int)*TDAS_N_FEATURES);
}
    
DolbyAudioClient::~DolbyAudioClient(){
    ALOGV("DolbyAudioClient destructor");
    closeDolby();
}

status_t DolbyAudioClient::process(short *in, short *out, int n_samples)
{
    //n_samples * channels * sizeof(short)(16Î»²ÉÑù)
    int buf_size = n_samples * 2 * 2;
			
    //logDolbyParam();	
	
    if (mTdasHandle == 0){   
        MY_LOOP_LOGW("tdasProcess:: tdas is not opened!");
        return UNKNOWN_ERROR;
    }

    if(n_samples%DM3_MIN_PROCESS_SAMPLES){
        ALOGW("The nummber of sample is not the multiple of  DM3_MIN_PROCESS_SAMPLES or channel is not stereo");
        memcpy(out, in, buf_size); 
    }else{

        #ifdef VENDOR_EDIT
        //TuGuang@OnLineRD.MultiMediaService.AF, 2013/01/05, Add for to optimize switch on dolby cacophony issue
        if(mSwitchOnDolbyActionFlag == 1)
	  	    {
	  		    int count = 0;
	  		    short* pcm = new short [buf_size / sizeof(short)];
			    memset(pcm, 0, buf_size);
			    int64_t cur_time = getCurSystemTimeUs();
	  		    ALOGW("dolby switch on, we let dolby process %d empty frame buffers!!!", PROCESS_EMPTY_BUF_COUNT);
	  		    while(count < PROCESS_EMPTY_BUF_COUNT)
			    {
				    tdas_process(mTdasHandle, in, pcm, n_samples);
				    count ++;
			    }		
			    mSwitchOnDolbyActionFlag = 0;
			    delete [] pcm;
			    ALOGV("process those empty buffer delayed %lld (us) !!!", getCurSystemTimeUs() - cur_time);
	  	    }
        #endif /* VENDOR_EDIT */
       
        MY_LOOP_LOGV("process in:%p,out:%p,n_samples:%d", in, out, n_samples); 
	    tdas_process(mTdasHandle, in, out, n_samples);
	              
    }
				
	return NO_ERROR;
}

status_t DolbyAudioClient::paramProcessSwitch(int event, int* value, int vLength)
{
    int value1 = -100;
    int value2 = -100;
    status_t result = NO_ERROR;

    if (1 == vLength)
    {
        value1 = *value;
        ALOGV("paramProcessSwitch  value1:%d, vLength:%d", value1, vLength);
    }
    else if (2 == vLength)
    {
        value1 = *value;
        value2 = *(value + 1);
        ALOGV("paramProcessSwitch  value1:%d, value2:%d, vLength:%d", value1, value2, vLength);
    }
    else
    {
        ALOGE("Unsupported params length!");
        return -EINVAL;
    }
    
    switch(event)
    {
        case SET_DOLBY:
        {
            ALOGV("case SET_DOLBY");
            result=setDolby(value1);
        }break;

        case RESET_DOLBY:
        {
            ALOGV("case RESET_DOLBY");
            result=resetDolby(); 
        }break;
        
        case CLOSE_DOLBY:
        {
            ALOGV("case CLOSE_DOLBY");
            result=closeDolby();
        }break;

        case SET_PLAY_MODE:
        {
            ALOGV("case SET_PLAY_MODE:%d", value1);
            result=setPlayMode(value1);
        }break;

        case SET_MEDIA_TYPE:
        {
            ALOGV("case SET_MEDIA_TYPE:%d", value1);
            result=setMediaType(value1);
        }break;
        
        case SET_GEQ_TYPE:
        {
            ALOGV("case SET_GEQ_TYPE:%d", value1);
            result=setGEQType(value1); 
        }break;
        
        case TDAS_HFE:
        {
            ALOGV("case TDAS_HFE:%d", value1);
            result=setHFELevel(value1);		
        }break;

        case TDAS_NB:
        { 
            ALOGV("case TDAS_NB:%d", value1);
            result=setNBLevel(value1);
        }break;

        case TDAS_SSH:
        { 
            ALOGV("case TDAS_SSH:%d", value1);
            result=setSSHLevel(value1);
        }break;
        
        case TDAS_MSR:
        {
            ALOGV("case TDAS_MSR:%d", value1);
            result=openMSR(value1);
        }break;
        
        case TDAS_GEQ:
        {
            ALOGV("case TDAS_GEQ value1:%d, value2:%d, vLenght:%d", value1, value2, vLength);
            if (1 == vLength)
            { 
                ALOGV("Enable geq effect");
                SoundEffectEnable(TDAS_GEQ, value1);
            }
            else if (2 == vLength)
            {
                ALOGV("set customed geq params");
                SetSoundEffectParameter(TDAS_GEQ, value1, value2);
            }    
        }break;
        
        default:
            ALOGW("Unsupported param!");
            break;         
    }
    return result;
}
	

status_t DolbyAudioClient::SoundEffectEnable(int aSe, int aEnable){
    ALOGV("SoundEffectEnable(%d,%d)",aSe,aEnable);
    if (NULL == mTdasHandle){
        ALOGE("Dolby not open");
        return UNKNOWN_ERROR;
    }
    if (mDolbyEnable){
        tdas_set(mTdasHandle, aSe, 0, aEnable);
    }    
    mEffectEnable[aSe] = aEnable;
    return NO_ERROR;
}

status_t DolbyAudioClient::SetSoundEffectParameter(int se, int parm, int value){
    ALOGV("SetSoundEffectParameter(se:%d,parm:%d,value:%d)",se,parm,value);
    if (NULL == mTdasHandle){
        ALOGE("Dolby not open");
        return UNKNOWN_ERROR;
    }

    tdas_set(mTdasHandle,se,parm,value);

    return NO_ERROR;
}

status_t DolbyAudioClient::GetSoundEffectParameter(int se, int parm){
    ALOGV("GetSoundEffectParameter");
    if (NULL == mTdasHandle){
        ALOGE("Dolby not open");
        return UNKNOWN_ERROR;
    }

    return tdas_get(mTdasHandle,se,parm);
}
status_t DolbyAudioClient::closeDolby(){
    ALOGV("closeDolby");
    if(mTdasHandle != NULL){
        tdas_close(mTdasHandle);
        mTdasHandle = NULL;
    }
    mInputSamplerate = DEFAULT_INPUT_SAMPLERATE;
    mVolume = -1;
    mDolbyEnable = 1;  
    memset(&mEffectEnable,0,sizeof(int)*TDAS_N_FEATURES);
    return NO_ERROR;
}

status_t DolbyAudioClient::openDolby(int sr){
    ALOGV("openDolby");
    int sr_index;
#if 0
    tFnPtr_mem_alloc    oppo_alloc = malloc;
    tFnPtr_mem_free     oppo_free = free;
    tFnPtr_mem_cpy      oppo_memcpy = memcpy;
    tFnPtr_mem_set      oppo_memset = memset;
    ret = tdas_mem_utils_init(oppo_alloc, oppo_free, oppo_memcpy, oppo_memset);
#endif
    /* Initialise TDAS */
    mTdasHandle = tdas_open();
    if (mTdasHandle == 0){
        ALOGE("tdas open failed");
        return UNKNOWN_ERROR;
    }
	
    tdas_set(mTdasHandle, TDAS_BUNDLE, TDAS_RATE_MULTIPLIER, SAMPLERATE_MULTIPLIER_1);	    		
    setInputSamplerate(sr);
		
    short tmpData[DM3_MIN_PROCESS_SAMPLES*2*2];
    tdas_process(mTdasHandle,tmpData,tmpData,DM3_MIN_PROCESS_SAMPLES); 
		
    setSPKEQ();//set Speakeq
		
    ALOGV("init: success over. mTdasHandle:%p", mTdasHandle);
    return NO_ERROR;
}
status_t   DolbyAudioClient::setDolbyVol(float vol)
{
    int dolbyVol = (int)(DOLBY_MIN_VOL*(1.0-vol));
    if (mVolume != dolbyVol){         
        ALOGV("setDolbyVol:%6f,dolby vol :%d",vol,dolbyVol);                   
        if (dolbyVol == DOLBY_MIN_VOL){
            tdas_set(mTdasHandle,TDAS_VOL,VOL_MUTE,1);
        }else if(tdas_get(mTdasHandle,TDAS_VOL,VOL_MUTE)){
            tdas_set(mTdasHandle,TDAS_VOL,VOL_MUTE,0);
            tdas_set(mTdasHandle,TDAS_VOL,VOL_GAIN_INTERNAL,dolbyVol);
        }else{
            tdas_set(mTdasHandle,TDAS_VOL,VOL_GAIN_INTERNAL,dolbyVol);        
        }
        mVolume = dolbyVol;
    }
    return NO_ERROR;
}

status_t DolbyAudioClient::setDolby(int enable){
    ALOGV("setDolby(%d)",enable);
    if (NULL == mTdasHandle)
    {
        ALOGE("NULL == mTdasHandle setDolby");
        return -EINVAL;
    }
    for (int i = 0;i < TDAS_N_FEATURES;i++)
    {
        if (mEffectEnable[i])
        {
            tdas_set(mTdasHandle, i, 0, enable);
        }
    }	

    #ifdef VENDOR_EDIT
    //TuGuang@OnLineRD.MultiMediaService.AF, 2013/01/05, Add for to optimize switch on dolby cacophony issue
    if(mDolbyEnable == 0 && enable == 1) 
  	{
		mSwitchOnDolbyActionFlag = 1;
  	}
	else 
	{
		mSwitchOnDolbyActionFlag = 0;
	}
    #endif /* VENDOR_EDIT */
   
    mDolbyEnable = enable;
    return NO_ERROR;
}

status_t DolbyAudioClient:: setPlayMode(int mode){
    ALOGV("setPlayMode: %d",mode);
    if ((mode < MODE_HEADPHONE) ||(mode > MODE_SPEAKER) )
    {
        ALOGE("unsupported Play Mode:%d",mode);
        return -EINVAL;
    }     
    mPlayMode = mode;
    updateSetting();
    return NO_ERROR;
}
    
status_t DolbyAudioClient:: setMediaType(int type){
    ALOGV("setMediaType: %d",type);
    if ((type < TYPE_MUSIC )||(type >TYPE_VIDEO))
    {
        ALOGE("unsupported Media Type:%d",type);
        return -EINVAL;
    }          
    mMediaType = type;
    return NO_ERROR;
}
    
status_t DolbyAudioClient:: setGEQType(int type){
    ALOGV("setGEQType: %d",type);    
    if ((type <0)||(type >= GEQ_N))
    {
        ALOGE("unsupported GEQ Type:%d",type);
        return -EINVAL;
    }  
	mGEQType = type;
    updateSetting();    
    return NO_ERROR;
}
status_t DolbyAudioClient::updateSetting()
{
    ALOGV("updateSetting: mode:%d,GEQ type:%d ",mPlayMode,mGEQType);
    if (mPlayMode == MODE_INVALID || mGEQType == GEQ_INVALID)
    {
        ALOGE("INVALID MODE OR GEQ");
        return -EINVAL;
    }
	        

    if ((mCurrentGEQType != mGEQType) ||(mPlayMode != mCurrentPlayMode))
	{
	    if(mGEQType == GEQ_CUSTOM)
		{
	        if(mPlayMode == MODE_HEADPHONE)
		    {
		        SoundEffectEnable(TDAS_SEQ, 0);
		    }
		    SoundEffectEnable(TDAS_GEQ, 1);
		    SetSoundEffectParameter(TDAS_GEQ, GEQ_NBANDS, 7);
		    mCurrentGEQType = mGEQType;
		    mCurrentPlayMode = mPlayMode;
		    return NO_ERROR;
		}
		
		//we no more use music flat as video flat 
		if( mMediaType == TYPE_VIDEO && mGEQType == GEQ_MUSIC_FLAT)
		{
            mGEQType = GEQ_VIDEO_FLAT;  
		}

		//slc_enable   input_level    depth  geq_nband band1 band2 band3 band4 band5 band6 band7 nb_cf nb_ol
		SoundEffectEnable(TDAS_SLC, PRESET[mPlayMode][mGEQType][0]);
		SetSoundEffectParameter(TDAS_SLC, SLC_LEVEL,PRESET[mPlayMode][mGEQType][1]);
		SetSoundEffectParameter(TDAS_SLC, SLC_DEPTH,PRESET[mPlayMode][mGEQType][2]);
		SetSoundEffectParameter(TDAS_GEQ, GEQ_NBANDS,PRESET[mPlayMode][mGEQType][3]);
		SetSoundEffectParameter(TDAS_GEQ, GEQ_BAND1,PRESET[mPlayMode][mGEQType][4]);
		SetSoundEffectParameter(TDAS_GEQ, GEQ_BAND2,PRESET[mPlayMode][mGEQType][5]);
		SetSoundEffectParameter(TDAS_GEQ, GEQ_BAND3,PRESET[mPlayMode][mGEQType][6]);
		SetSoundEffectParameter(TDAS_GEQ, GEQ_BAND4,PRESET[mPlayMode][mGEQType][7]);
		SetSoundEffectParameter(TDAS_GEQ, GEQ_BAND5,PRESET[mPlayMode][mGEQType][8]);
		SetSoundEffectParameter(TDAS_GEQ, GEQ_BAND6,PRESET[mPlayMode][mGEQType][9]);
		SetSoundEffectParameter(TDAS_GEQ, GEQ_BAND7,PRESET[mPlayMode][mGEQType][10]);    
		SetSoundEffectParameter(TDAS_NB, NB_CUTOFF,PRESET[mPlayMode][mGEQType][11]);    
		SetSoundEffectParameter(TDAS_NB, NB_LEVEL,PRESET[mPlayMode][mGEQType][12]);  

		if (mPlayMode == MODE_HEADPHONE)
		{
			SoundEffectEnable(TDAS_SEQ, 0);
			SoundEffectEnable(TDAS_MSR,1);
			if (mMediaType == TYPE_MUSIC)
			{
				SoundEffectEnable(TDAS_MSR,mIsMSREnable);
				SoundEffectEnable(TDAS_HFE, 1);
				SetSoundEffectParameter(TDAS_HFE, HFE_DEPTH, 0);
				SetSoundEffectParameter(TDAS_MSR, MSR_ROOM, 0);	
				SetSoundEffectParameter(TDAS_MSR, MSR_BRIGHT, 2);
				enableMSREnhanceMode(false);
				SoundEffectEnable(TDAS_GEQ, 1);
				SetSoundEffectParameter(TDAS_GEQ, GEQ_MAXBOOST, 48); 
				SetSoundEffectParameter(TDAS_GEQ, GEQ_PREAMP, -4);
				SoundEffectEnable(TDAS_NB, 1);
				SetSoundEffectParameter(TDAS_NB, NB_BOOST, 3); 
				if(mGEQType == GEQ_MUSIC_DEFAULT)
				{
					//SetSoundEffectParameter(SoundEffect.GEQ, SoundEffectParm.GEQ_PREAMP, 0);//only for music default ?
					//_SoundEffectEnable(TDAS_MSR, 0);
					SetSoundEffectParameter(TDAS_NB, NB_BOOST, 3);
				}
				else if(mGEQType == GEQ_MUSIC_FLAT)//we disable EQ in headphone mode for music
				{
					SoundEffectEnable(TDAS_GEQ, 0);
				}
			}
			else if (mMediaType == TYPE_VIDEO)
			{
				SoundEffectEnable(TDAS_HFE,0);
				SetSoundEffectParameter(TDAS_MSR, MSR_ROOM, 1);	
				SetSoundEffectParameter(TDAS_MSR, MSR_BRIGHT, 1);
				enableMSREnhanceMode(true);
				SoundEffectEnable(TDAS_GEQ, 1);
				SetSoundEffectParameter(TDAS_GEQ, GEQ_MAXBOOST, 48); 
				SetSoundEffectParameter(TDAS_GEQ, GEQ_PREAMP, 0);
				SoundEffectEnable(TDAS_NB, 1);
				SetSoundEffectParameter(TDAS_NB, NB_BOOST, 3);   
				if(mGEQType == GEQ_MV)
				{
					SoundEffectEnable(TDAS_HFE, 1);
					SetSoundEffectParameter(TDAS_HFE, HFE_DEPTH, 1);
					SetSoundEffectParameter(TDAS_MSR, MSR_ROOM, 0);	
					SetSoundEffectParameter(TDAS_MSR, MSR_BRIGHT, 2);
					enableMSREnhanceMode(false);
				}
			}
		} 

		if (mPlayMode == MODE_SPEAKER)
		{
			SoundEffectEnable(TDAS_SEQ, 1);
			SoundEffectEnable(TDAS_MSR, 0);
			enableMSREnhanceMode(false);
			if (mMediaType == TYPE_MUSIC)
			{
				SetSoundEffectParameter(TDAS_SEQ, SPKEQ_CHGAIN_DB1, 16); 
				SetSoundEffectParameter(TDAS_SEQ, SPKEQ_CHGAIN_DB2, 16); 

				SoundEffectEnable(TDAS_HFE, 1);
				SetSoundEffectParameter(TDAS_HFE,HFE_DEPTH, 0);
				SoundEffectEnable(TDAS_GEQ, 1);
				SetSoundEffectParameter(TDAS_GEQ,GEQ_MAXBOOST, 48); 
				SetSoundEffectParameter(TDAS_GEQ, GEQ_PREAMP, 0);
                SoundEffectEnable(TDAS_NB, 1);
				SetSoundEffectParameter(TDAS_NB, NB_BOOST, 4); 
			    if(mGEQType == GEQ_MUSIC_DEFAULT)
			    {
				    SoundEffectEnable(TDAS_HFE, 0);
			        //SetSoundEffectParameter(TDAS_NB, NB_BOOST, 5);
			    }
				if(mGEQType == GEQ_MUSIC_FLAT)//we disable EQ in headphone mode for music
				{
					SoundEffectEnable(TDAS_GEQ, 0);
				}
			}
			else if (mMediaType == TYPE_VIDEO)
			{
				SetSoundEffectParameter(TDAS_SEQ, SPKEQ_CHGAIN_DB1, 24); 
				SetSoundEffectParameter(TDAS_SEQ, SPKEQ_CHGAIN_DB2, 24);
				SoundEffectEnable(TDAS_HFE, 1);
				SetSoundEffectParameter(TDAS_HFE, HFE_DEPTH, 0);
				SoundEffectEnable(TDAS_GEQ, 1);
				SetSoundEffectParameter(TDAS_GEQ, GEQ_MAXBOOST, 48); 
				SetSoundEffectParameter(TDAS_GEQ, GEQ_PREAMP, 0);
                SoundEffectEnable(TDAS_NB, 1);
				SetSoundEffectParameter(TDAS_NB, NB_BOOST, 3);

				if(mGEQType == GEQ_MV)
				{
					SetSoundEffectParameter(TDAS_SEQ, SPKEQ_CHGAIN_DB1, 16); 
					SetSoundEffectParameter(TDAS_SEQ, SPKEQ_CHGAIN_DB2, 16);
				}
			}
		}
    }
    mCurrentGEQType = mGEQType;
    mCurrentPlayMode = mPlayMode;    
    return NO_ERROR;	
}


status_t DolbyAudioClient::setNBLevel(int level)
{
    ALOGV("setNBLevel: %d",level);
    if (level < LEVEL_NORMAL || level > LEVEL_CLOSE)
    {
        ALOGE("unsupported hfe level:%d", level);
        return -EINVAL;
    }
            
    if (LEVEL_CLOSE == level)
    {
        SoundEffectEnable(TDAS_NB, 0);
    }
    else
    {
        if (0 == GetSoundEffectParameter(TDAS_NB, NB_ENABLE))
        {
            SoundEffectEnable(TDAS_NB, 1);
        }
        if (MODE_SPEAKER == mPlayMode)
        {
            SetSoundEffectParameter(TDAS_NB, NB_BOOST, NB_LEVEL_SETTING_SPEAKER[level]);
        }
        else if (MODE_HEADPHONE == mPlayMode)
        {
            SetSoundEffectParameter(TDAS_NB, NB_BOOST, NB_LEVEL_SETTING_HEADPHONE[level]);
        }
    }
    mNBLevel = level;
    return NO_ERROR;
}

status_t DolbyAudioClient::setSSHLevel(int level)
{
    ALOGV("setSSHLevel: %d",level);
    if (level < LEVEL_NORMAL || level > LEVEL_CLOSE)
    {
        ALOGE("unsupported hfe level:%d", level);
        return -EINVAL;
    }
            
    if (LEVEL_CLOSE == level)
    {
        SoundEffectEnable(TDAS_SSH, 0);
    }
    else
    {
        if (0 == GetSoundEffectParameter(TDAS_SSH, HFE_ENABLE))
        {
            SoundEffectEnable(TDAS_SSH, 1);
        }
        SetSoundEffectParameter(TDAS_SSH, SSH_MODE, NB_LEVEL_SETTING_HEADPHONE[level]);
    }
    mSSHLevel = level;
    return NO_ERROR;
}

status_t DolbyAudioClient::openMSR(int enabled){
    ALOGV("openMSR mMode:%d, enabled:%d", mPlayMode, enabled);
    if (MODE_HEADPHONE != mPlayMode)
    {
        ALOGV("We can set msr effect under headset mode");
        return -EINVAL;
    }
    SoundEffectEnable(TDAS_MSR, enabled);
    mIsMSREnable = enabled;

    return NO_ERROR;   
}

status_t DolbyAudioClient::setHFELevel(int level){
    ALOGV("setHFELevel: %d",level); 
    if (level < LEVEL_NORMAL || level > LEVEL_CLOSE)
    {
        ALOGE("unsupported hfe level:%d",level);
        return -EINVAL;
    }
            
    if (LEVEL_CLOSE == level)
    {
        SoundEffectEnable(TDAS_HFE, 0);
    }
    else
    {
        if (0 == GetSoundEffectParameter(TDAS_HFE, HFE_ENABLE))
        {
            SoundEffectEnable(TDAS_HFE, 1);
        }
        SetSoundEffectParameter(TDAS_HFE, HFE_DEPTH, HFE_LEVEL_SETTING[level]);
    }
    mHFELevel=level;
    return NO_ERROR;   
}

status_t DolbyAudioClient::setSPKEQ()
{
	FILE *p_file = NULL;
	unsigned long coef_size= 0;
	char *coefficient_buffer= NULL;
	int i = 0;
	
	ALOGV("setSPKEQ");
	for (i=0; i < TDAS_N_SAMPLERATES; i++){
		
		p_file = fopen(speaker_eq_coef_files[i], "rb");
		if(NULL == p_file)
		{
			ALOGW("setSPKEQ, can not open %s", speaker_eq_coef_files[i]);
			continue;
		}

		ALOGV("setSPKEQ, open %s succced!!!", speaker_eq_coef_files[i]);
		//Determine the size of the file 
		fseek(p_file, 0, SEEK_END);
		coef_size = ftell(p_file);
		rewind(p_file);
		
		//Allocate memory for the file 
		coefficient_buffer = (char*) malloc(sizeof(char)*coef_size);
		if(NULL == coefficient_buffer)
		{
			ALOGE("setSPKEQ, out of memory");
			continue;
		}
		
		//Read in coefficients from file and put into temporary buffer.
		fread(coefficient_buffer, sizeof(char), coef_size, p_file);
		fclose(p_file);

		//Set up Speaker EQ coefficients 
		tdas_set_coef(mTdasHandle, TDAS_SEQ, sample_rates[i], coef_size, coefficient_buffer);

		//Clean up 
		coef_size=0;
		free(coefficient_buffer);
		coefficient_buffer = NULL;
	}

	return NO_ERROR;
}

status_t DolbyAudioClient::resetDolby()
{
	ALOGV("resetDolby");
	if (NULL == mTdasHandle)
	{
        ALOGE("Dolby not open");
        return UNKNOWN_ERROR;
    }
	tdas_reset(mTdasHandle);
	return NO_ERROR;
}

// Switch dobly MSR enhance mode on/off.
void DolbyAudioClient::enableMSREnhanceMode(int aEnable){
	if(aEnable)
	{
		SetSoundEffectParameter(TDAS_BUNDLE, TDAS_MSR_MAX_PROFILE, 3);
		SetSoundEffectParameter(TDAS_BUNDLE, TDAS_IN_MATRIX, 4);
	}
	else
	{
		SetSoundEffectParameter(TDAS_BUNDLE, TDAS_MSR_MAX_PROFILE, 0);
		SetSoundEffectParameter(TDAS_BUNDLE, TDAS_IN_MATRIX, 0);
	}	    
}

void DolbyAudioClient::setInputSamplerate(uint32_t sr){
	
    int sr_index;
    switch (sr) {
    case 48000:
        sr_index = SAMPLERATE_48000;
        break;
    case 44100:
        sr_index = SAMPLERATE_44100;
        break;
    case 32000:
        sr_index = SAMPLERATE_32000;
        break;
    case 24000:
        sr_index = SAMPLERATE_24000;
        break;
    case 22050:
        sr_index = SAMPLERATE_22050;
        break;
    case 16000:
        sr_index = SAMPLERATE_16000;
        break;
    case 12000:
        sr_index = SAMPLERATE_12000;
        break;
    case 11025:
        sr_index = SAMPLERATE_11025;
        break;
    case 8000:
        sr_index = SAMPLERATE_8000;
        break;
    default:
        ALOGE("illegal samplerate: %d", sr);
        return;
    }
	mInputSamplerate = sr;

    tdas_set(mTdasHandle, TDAS_BUNDLE, TDAS_SAMPLERATE, sr_index);
       
}

uint32_t DolbyAudioClient::getInputSamplerate(){
    return mInputSamplerate;
}
void DolbyAudioClient::logDolbyParam()
{
	MY_LOOP_LOGW("=========================================TDAS PARAM============================================\n"
	"\t - [MODE]: the following param only set for <HEADSET>\n"
	"\n"
	"\t - TDAS_BYPASS:\t\t  %d \n"
	"\t - TDAS_IN_MATRIX:\t  %d \n"
	"\t - TDAS_MSR_MAX_PROFILE:  %d \n"
	"\t - M2S_ENABLE:\t\t  %d \n"
	"\t - SSE_ENABLE:\t\t  %d \n"
	"\t - MSR_ENABLE:\t\t  %d\tMSR_ROOM: %d\t    MSR_BRIGHT: %d \n"
	"\t - SLC_ENABLE:\t\t  %d\tSLC_LEVEL: %d\t    SLC_DEPTH: %d \n"
	"\t - VOL_ENABLE:\t\t  %d \n"
	"\t - NB_ENABLE:\t\t  %d \tNB_CUTOFF: %d\t    NB_BOOST: %d \tNB_LEVEL: %d \n"
	"\t - HFE_ENABLE:\t\t  %d \tHFE_DEPTH: %d\n"
	"\t - FADE_ENABLE:\t\t  %d \n"
	"\t - GEQ_ENABLE:\t\t  %d\tGEQ_MAXBOOST: %d    GEQ_PREAMP: %d\tGEQ_NBANDS: %d \n"
	"\t - GEQ_BAND1~GEQ_BAND7:\t  %d  %d  %d  %d  %d  %d  %d \n"
	"\t - SPKEQ_ENABLE:\t  %d\n"
	"=========================================TDAS PARAM============================================\n",
	tdas_get(mTdasHandle, TDAS_BUNDLE, TDAS_BYPASS),
	tdas_get(mTdasHandle, TDAS_BUNDLE, TDAS_IN_MATRIX),
	tdas_get(mTdasHandle, TDAS_BUNDLE, TDAS_MSR_MAX_PROFILE),
	tdas_get(mTdasHandle, TDAS_M2S, M2S_ENABLE),
	tdas_get(mTdasHandle, TDAS_SSE, SSE_ENABLE),
	tdas_get(mTdasHandle, TDAS_MSR, MSR_ENABLE),tdas_get(mTdasHandle, TDAS_MSR, MSR_ROOM),tdas_get(mTdasHandle, TDAS_MSR, MSR_BRIGHT),
	tdas_get(mTdasHandle, TDAS_SLC, SLC_ENABLE),tdas_get(mTdasHandle, TDAS_SLC, SLC_LEVEL),tdas_get(mTdasHandle, TDAS_SLC, SLC_DEPTH),
	tdas_get(mTdasHandle, TDAS_VOL, VOL_ENABLE),
	tdas_get(mTdasHandle, TDAS_NB, NB_ENABLE),tdas_get(mTdasHandle, TDAS_NB, NB_CUTOFF),tdas_get(mTdasHandle, TDAS_NB, NB_BOOST),tdas_get(mTdasHandle, TDAS_NB, NB_LEVEL),
	tdas_get(mTdasHandle, TDAS_HFE, HFE_ENABLE),tdas_get(mTdasHandle, TDAS_HFE, HFE_DEPTH),
	tdas_get(mTdasHandle, TDAS_FADE, FADE_ENABLE),
	tdas_get(mTdasHandle, TDAS_GEQ, GEQ_ENABLE),
	tdas_get(mTdasHandle, TDAS_GEQ, GEQ_MAXBOOST),
	tdas_get(mTdasHandle, TDAS_GEQ, GEQ_PREAMP),
	tdas_get(mTdasHandle, TDAS_GEQ, GEQ_NBANDS),
	tdas_get(mTdasHandle, TDAS_GEQ, GEQ_BAND1),
	tdas_get(mTdasHandle, TDAS_GEQ, GEQ_BAND2),
	tdas_get(mTdasHandle, TDAS_GEQ, GEQ_BAND3),
	tdas_get(mTdasHandle, TDAS_GEQ, GEQ_BAND4),
	tdas_get(mTdasHandle, TDAS_GEQ, GEQ_BAND5),
	tdas_get(mTdasHandle, TDAS_GEQ, GEQ_BAND6),
	tdas_get(mTdasHandle, TDAS_GEQ, GEQ_BAND7),
	tdas_get(mTdasHandle, TDAS_SEQ, SPKEQ_ENABLE));	
}

} // namespace

