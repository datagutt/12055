/******************************************************************************
 * Copyright 2008-2010 OPPO Mobile Comm Corp., Ltd, All rights reserved.
 * FileName:EffectDolby.h 
 * ModuleName:libdolby
 * Author:Popeye
 * Creat Data: 2012-12-15
 * Description:The adaptation layer between Android and Dolby lib
 * History
 * version       time         	author     description
    1.0    		2012-12-15   	Popeye     rebuilt the interface 
 ******************************************************************************/

#ifndef ANDROID_TDAS_AUDIO_H
#define ANDROID_TDAS_AUDIO_H

#include <stdint.h>
#include <sys/types.h>
#include <utils/Errors.h>

#define DEFAULT_OUTPUT_SAMPLERATE 44100
#define DEFAULT_INPUT_SAMPLERATE 44100
#define DEFAULT_N_CHANNELS 2
#define DEFAULT_PCM_BYTES 2
#define DOLBY_ACTIVE 8
#define DOLBY_MIN_VOL 256
#define PROCESS_EMPTY_BUF_COUNT 3

#define SET_DOLBY    100
#define CLOSE_DOLBY  101
#define RESET_DOLBY  102

#define SET_PLAY_MODE 103
#define SET_MEDIA_TYPE 104
#define SET_GEQ_TYPE  105



extern "C"
{
    #include "target_client.h"
    #include "control.h"
    #include "tdas_client.h"
    #include "tdas_resampler_helper.h"
    #include "compiler.h"
    #include "bundle_features.h"
}
//play mode
#define MODE_INVALID      -1       
#define MODE_HEADPHONE     0
#define MODE_SPEAKER       1

//media type
#define TYPE_INVALID      -1
#define TYPE_MUSIC         0
#define TYPE_VIDEO         1

#define LEVEL_NORMAL       0
#define LEVEL_MIDDING      1
#define LEVEL_HARD         2 
#define LEVEL_CLOSE        3

 
#define GEQ_INVALID        -1    
//music
#define GEQ_MUSIC_DEFAULT  0
#define GEQ_MUSIC_FLAT     1
#define GEQ_ROCK           2            
#define GEQ_JAZZ           3    
#define GEQ_POP            4    
#define GEQ_CLASSIC        5    
#define GEQ_COUNTRY        6    
#define GEQ_DANCE          7    
#define GEQ_METAL          8        
#define GEQ_RB             9    
#define GEQ_HIPHOP         10    
#define GEQ_BLUES          11    
#define GEQ_ELECTRONIC     12    
#define GEQ_URBAN          13    
#define GEQ_PODCAST        14    
#define GEQ_LATIN          15    
#define GEQ_JUNGLE         16    
//video
//public static final int GEQ_VIDEO_DEFAULT = 17;
//public static final int GEQ_VIDEO_FLAT = 18;   
#define GEQ_MV              17         
#define GEQ_MOVIE           18           
#define GEQ_COMEDY          19    
#define GEQ_ACTION          20
#define GEQ_ANIMATED        21
#define GEQ_NEWS            22
#define GEQ_SPORTS          23
#define GEQ_DRAMA           24
#define GEQ_MUSICAL         25
#define GEQ_VARIIETY        26
#define GEQ_VIDEO_FLAT      27 

#define GEQ_CUSTOM          28 

#define GEQ_N              29

const int NB_LEVEL_SETTING_HEADPHONE[] = {0,3,6};
const int NB_LEVEL_SETTING_SPEAKER[] = {0,2,4};
const int HFE_LEVEL_SETTING[] = {0,2,4};
const int SSH_LEVEL_SETTING[] = {0,3,6};   

//index
#define slc_enable         0 
#define slc_input_level    1
#define slc_depth          2 
//#define geq_enable         3 
#define geq_nband          3 
#define geq_band1          4 
#define geq_band2          5 
#define geq_band3          6 
#define geq_band4          7 
#define geq_band5          8 
#define geq_band6          9 
#define geq_band7          10 
//#define geq_pre_amp        12
//#define geq_max_boost      13
//#define nb_enable          14 
#define nb_cf              11 
//#define nb_boost           16 
#define nb_ol              12 
//#define sr_enable          18 
//#define sr_room            19 
//#define sr_brightness      20
//#define hfe_enable         21 
//#define hfe_depth          22 
//#define profile            23 
//#define matrix             24 
//#define spke_chn1          25 
//#define spke_chn2          26
//#define spke_enable        27
#define param_n            13

	const int PRESET[][GEQ_N][param_n]=
	{
	    //headphone
        //slc_enable    input_level  depth     geq_nband    band1       band2       band3       band4       band5       band6       band7       nb_cf       nb_ol
        {
            //music
            {1,		0,		0,		7,		0,		0,		-4,		-4,		-8,		1,		-1,		3,		0},//music default
            {0,		0,		0,		7,		0,		0,		0,		0,		0,		0,		0,		3,		0},//Flat
            {1,		0,		0,		7,		16,		8,		-4,		-12,	-8,		9, 		16,		3,		0},//rock
            {1,		0,		0,		7,		0,		0,  	-8,	    -12,	-16,	-3,		0,		3,		0},//jazz
            {1,		0,		0,		7,		-8,		-20,	0,		8,		0,		9,		8,		3,		0},//pop
            {1,		0,		0,		7,		4,		0,		-4,		-12,	-12, 	1,		0,		3,		0},//Classical
            {1,		0,		0,		7,		0,		-4,		-8,		-8,		-16,	-3,		-8,		3,		0},//Country        
            {1,		0,		0,		7,		16,		12,		-28,    -32,	-24,	13,		16,		3,		0},//Dance        
            {1,		0,		0,		7,		24,		14,		-24,	2,		4,		17,		8,	    3,		0},//Metal                
            {1,		0,		0,		7,		16,		4,		-8,		-8,		-4,		5,		4,		3,		0},//R&B
            {1,		0,		0,		7,		16,		-4,		-4,		-12,	-8,		-3,		-4,		3,		0},//Hip Hop
            {1,		0,		0,		7,		4,		0,		-8,		-12,	-8,		5,		8,		3,		0},//Blues
            {1,		0,		0,		7,		16,		4,		-12,	-8,		-16,	-3,		0,		3,		0},//Electronic
            {1,		0,		0,		7,		16,		-4,		-8,		-12,	-8,		-3,		-4,		3,		0},//Urban Contemporary
            {1,		0,		0,		7,		-20,	-12,	-12,	-8,		-8,		9,		0,		3,		0},//Podcast
            {1,		0,		0,		7,		4,		-4,		-12,	-12,	-16,	-3,		0,	    3,		0},//Latin    
            {1,		0,		0,		7,		24,		4,		-32,	-6,		-10,	13,		12,		3,		0},//Jungle
            //video
        //slc_enable    input_level depth       geq_nband   band1       band2       band3       band4       band5       band6       band7       nb_cf       nb_ol            
            {0,		0,		0,		7,		6,		-12,	3,		12,		6,		8,		9,		3,		3},//music video        
            {1,		0,		1,		7,		8,		4,		-8,		-16,	-20,	-8,		8,		6,		0},//movie
            {1,		0,		1,		7,		-8,		-4,		4,		12,		16,		14,		-2,		6,		0},//comedy
            {1,		0,		1,		7,		12,		-4,		4,		12,		8,		0,		-8,		6,		0},//action
            {1,		0,		1,		7,		4,		2,		-4,		-8,		-4,		2,		4,		6,		0},//animated
            {1,		0,		1,		7,		-8,		-4,		4,		12,		8,		0,		-8,		6,		0},//news
            {1,		0,		1,		7,		4,		0,		-4,		-8,		-4,		0,		4,		6,		0},//sports
            {0,		0,		0,		7,		4,		-4,		4, 		10,		-8,		0,		-8,		6,		0},//drama
            {1,		0,		1,		7,		4,		0,		-4,		-8,		-4,		0,		4,      6,		0},//musical
            {1,		0,		1,		7,		-8,		-4,		4,		12,		8,		0,		-8,		6,		0},//variiety show  
            {1,		0,		1,		7,		0,		0,		0,		0,		0,		0,		0,		6,		0}//video flat  
        },
        //speaker
        {
            //music
        //slc_enable    input_level depth       geq_nband   band1       band2       band3       band4       band5       band6       band7       nb_cf       nb_ol            
            {0,		0,		0,		7,		0,		0,		0,		0,		0,		0,		0,		15,     3},//music default
            {0,		0,		0,		7,		0,		0,		0,		0,		0,		0,		0,		15,		3},//Flat
            {0,		0,		0,		7,		16,		8,		0,		-8,		0,      8,		16,		15,		3},//rock
            {0,		0,		0,		7,		0,      0,		-4,		-8,     -8, 	-4,		0,		15,		3},//jazz
            {0,		0,		0,		7,		-8,		-20,	4,		12,		8,		8,		8,		15,		3},//pop
            {0,		0,		1,		7,		4,		0,		0,		-8,		-4,		0,		0,		15,		3},//Classical
            {0,		0,		0,		7,		0,		-4,		-4,		-4,		-8,		-4,		-8,		15,		3},//Country        
            {0,		0,		0,		7,		16,		12,		-24,	-28,	-16,	12,		16,		15,		3},//Dance        
            {0,		0,		0,		7,		24,		14,		-20,	6, 		12,		16,		8,		15,		3},//Metal                
            {0,		0,		0,		7,		16,		4,		-4,		-4,		4,		4,		4,		15,		3},//R&B
            {0,		0,		0,		7,		16,		-4,		0,		-8,		0,		-4,		-4,		15,		3},//Hip Hop
            {0,		0,		0,		7,		4,		0,		-4,		-8,		0,		4,		8,		15,		3},//Blues
            {0,		0,		0,		7,		16,		4,		-8,		-4,		-8,		-4,		0,		15,		3},//Electronic
            {0,		0,		0,		7,		16,		-4,		-4,		8,		0,		-4,		-4,		15,		3},//Urban Contemporary
            {0,		0,		0,		7,		-20,	-12,	-8,		-4,		0,		8,		0,		15,		3},//Podcast
            {0,		0,		0,		7,		4,		-4,		-8,		-8,		-8,		-4,		0,		15,		3},//Latin    
            {0,		0,		0,		7,		24,		4,		-28,	-2,		-2,		12,		12,		15,		3},//Jungle 
            //video
        //slc_enable    input_level depth       geq_nband   band1       band2       band3       band4       band5       band6       band7       nb_cf       nb_ol            
            {1,		0,		0,		7,		0,		0,		0,		0,		0,		0,		0,		14,		3},//music video        
            {1,		0,		1,		7,		0,		0,		0,		0,		0,		0,		0,		14,		3},//movie
            {1,		0,		1,		7,		-8,		-4,		4,		12,		16,		14,		-2,		14,		3},//comedy
            {1,		0,		1,		7,		12,		-4,		4,		12,		8,		0,		-8,		14,		3},//action
            {1,		0,		1,		7,		4,		2,	    -4,		-8,		-4,		2,		4,		14,		3},//animated
            {1,		0,		1,		7,		-8,		-4,		4,		12,		8,		0,		-8,		14,		3},//news
            {1,		0,		1,		7,		4,		0,		-4,		-8,		-4,		0,		4,		14,		3},//sports
            {1,		0,		1,		7,		4,		-4,		4,		12,		8,		0,		-8,		14,		3},//drama
            {1,		0,		1,		7,		4,		0,		-4,		-8,		-4,		0,		4,		14,		3},//musical
            {1,		0,		1,		7,		-8,		-4,		4,	    12,		8,	    0,		-8,		14,		3},//variiety show
            {1,		0,		1,		7,		0,		0,	    0,		0,		0,		0,		0,		14,		3}//video flat  
        } 
    };


namespace android {

#define MY_LOOP_LOG
#ifdef MY_LOOP_LOG
#define DUMP_PER_TIMES 30
#define DUMP_PER_X_TIMES_BEGIN\
    {\
	static int count = 0;\
	if(count == 0)\
	{\
		ALOGV("DUMP_PER_%d_TIMES begin!", DUMP_PER_TIMES);	
#define DUMP_PER_X_TIMES_END\
	}\
	count ++;\
	count %= DUMP_PER_TIMES;\
    }
#define MY_LOOP_LOGV(...)\
	DUMP_PER_X_TIMES_BEGIN\
	ALOGV(__VA_ARGS__);\
	DUMP_PER_X_TIMES_END

#define MY_LOOP_LOGW(...)\
	DUMP_PER_X_TIMES_BEGIN\
	ALOGW(__VA_ARGS__);\
	DUMP_PER_X_TIMES_END

#define MY_LOOP_LOGE(...)\
	DUMP_PER_X_TIMES_BEGIN\
	ALOGE(__VA_ARGS__);\
	DUMP_PER_X_TIMES_END
#else
	#define MY_LOOP_LOGV(...)	((void)0)
	#define MY_LOOP_LOGW(...)	((void)0)
	#define MY_LOOP_LOGE(...)	((void)0)
#endif

class DolbyAudioClient
{
public:
	DolbyAudioClient();
	~DolbyAudioClient();
	status_t 	openDolby(int sr);
	status_t    closeDolby();
	status_t    SoundEffectEnable(int se,int enable);
	status_t    SetSoundEffectParameter(int se,int parm,int value);
	status_t    GetSoundEffectParameter(int se,int parm); 
	status_t    process(short *in, short *out, int n_samples); 
	status_t    setDolbyVol(float vol);
	status_t    setDolby(int enable);
	void        setInputSamplerate(uint32_t sr);
	uint32_t    getInputSamplerate();
	status_t 	setSPKEQ();
	status_t    resetDolby();
	status_t    paramProcessSwitch(int event, int* value, int vLength);
	void        logDolbyParam();
	status_t    setPlayMode(int mode);
	status_t    setMediaType(int type);
	status_t    setGEQType(int type);
	status_t    setHFELevel(int level);
	status_t    setNBLevel(int level);
	status_t    setSSHLevel(int level);
	status_t    openMSR(int enabled);
	status_t    updateSetting();
	void        enableMSREnhanceMode(int aEnable);
	bool        mIsByPasson;
private:
    void *mTdasHandle;
    uint32_t mInputSamplerate;
    int mVolume;   
    int mDolbyEnable;
    int mEffectEnable[TDAS_N_FEATURES];

    int mMediaType;
	int mPlayMode;
	int mGEQType;
	int mNBLevel;
	int mHFELevel;
	int mSSHLevel;
	
	int mCurrentPlayMode;
	int mCurrentGEQType;
	int mIsMSREnable;

	
	int mSwitchOnDolbyActionFlag;
};

}; // namespace android

#endif // ANDROID_TDAST_H

