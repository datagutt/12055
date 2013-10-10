#ifndef _AUDIOPLAYEREVENT_H
#define _AUDIOPLAYEREVENT_H

namespace android {

enum audio_event_type {
	AUDIOTRACK_CONSTRUCT = 0,
	AUDIOTRACK_START     = 1,	
	AUDIOTRACK_PAUSE     = 2, 
	AUDIOTRACK_WRITE     = 3,
	AUDIOTRACK_FLUSH     = 4,
	AUDIOTRACK_STOP      = 5,
	AUDIOTRACK_RELEASE   = 6,
	AUDIOTRACK_SETVOLUME = 7,
	AUDIOTRACK_GETMINBUFFERSIZE = 8,
	AUDIOTRACK_SETAUXEFFECTSENdLEVEL = 9,
	AUDIOTRACK_ATTACHAUXEFFECT = 10,
	AUDIOTRACK_GETPOSITION = 11,
};

enum mode_type {
	MODE_STATIC = 0,
	MODE_STREAM = 1,
};

/* For AudioTrack.java */
enum audio_format_type {
	ENCODING_PCM_16BIT = 2,
	ENCODING_PCM_8BIT  = 3,
};

struct AudioParameters {
	int samperate;
	int channel;
	int channelmask;
	int format;
	int buffersize;
	int mode;
	int sessionid;
	int streamtype;
	int auxeffectid;
	uint32_t position;
	float leftvolume;
	float rightvolume;
	float sendlevel;
};

struct DataPacket {
	DataPacket() {
		data = NULL;
	}
	
    uint8_t *data;
	size_t datasize;
    size_t offset;
	size_t length;
};	

}
#endif
