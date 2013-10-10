/************************************************************************************
** File: - CheckPermission.cpp
** VENDOR_EDIT
** Copyright (C), 2008-2012, OPPO Mobile Comm Corp., Ltd
** 
** Description: permission check, add for permission intercept.
** 
** Version: 1.0
** Date created: 04/25/2013
** Author: Wanglan@Plf.Framework
** Add for: permission intercept
** --------------------------- Revision History: --------------------------------
** 	<author>	                    <data>					<desc>
** 	Wanglan@Plf.Framework  	04/25/2013	     			 Init
************************************************************************************/
#define LOG_TAG "CheckPermission"
#define MAX_BUF_SIZE 256
 
#include <cutils/log.h>
#include <binder/IServiceManager.h>
#include <binder/Parcel.h>
#include <utils/String8.h>
#include <utils/String16.h>
#include <binder/CheckPermission.h>

namespace android {

// ---------------------------------------------------------------------------

static String16 _call("android.app.IActivityManager");
static String16 _sendsms("com.android.internal.telephony.ISms");
static String16 _data_network("android.net.IConnectivityManager");
static String16 _wifi("android.net.wifi.IWifiManager");
static String16 _bluetooth("android.bluetooth.IBluetooth");
static String16 _contentprovider("android.content.IContentProvider");
static String16 _location("android.location.ILocationManager");
static String16 _telephony("com.android.internal.telephony.ITelephony");
static String16 _camera("android.hardware.ICameraService");
static String16 _audio_record("android.media.IMediaRecorder");
static String16 _nfc("android.nfc.INfcAdapter");

static String16 URI_CONTACT("com.android.contacts");
static String16 URI_SMS("sms");
static String16 URI_MMS("mms");
static String16 URI_MMS_SMS("mms-sms");
static String16 URI_CALLLOG("call_log");

static String16 ACTION_CALL("android.intent.action.CALL");

bool parseAuthorityCallLog(const char16_t* mString, ssize_t length, ssize_t index) {
	if (length > index + 2
			&& *(mString+index+1) == '/'
			&& *(mString+index+2) == '/') {
	    const char16_t* p = mString + index + 3;
		String16 authority = String16(p, 8);
	    return (URI_CALLLOG == authority);
	}
	return false;
}

bool parseAuthoritySMS(const char16_t* mString, ssize_t length, ssize_t index) {
	if (length > index + 2
			&& *(mString+index+1) == '/'
			&& *(mString+index+2) == '/') {
	    const char16_t* p = mString + index + 3;
		String16 authority = String16(p, 3);
	    if ((URI_SMS == authority) || (URI_MMS == authority)) {
			return true;
		}
	}
	return false;
}

bool parseAuthorityContact(const char16_t* mString, ssize_t length, ssize_t index) {
	if (length > index + 2
			&& *(mString+index+1) == '/'
			&& *(mString+index+2) == '/') {
	    const char16_t* p = mString + index + 3;
		String16 authority = String16(p, 20);
	    return (URI_CONTACT == authority);
	}
	return false;
}


int CheckPermission(uint32_t code, const Parcel& data, int pid, int uid) {	

	//getInterfaceDescriptor
	data.setDataPosition(0);
	data.readInt32();
	const String16 InterfaceDescriptor(data.readString16());
	
	if (InterfaceDescriptor.size() != 0) {
		if (InterfaceDescriptor == _call) {
			if (code == 3) {
				size_t len;
				data.readStrongBinder();
				const char16_t* intent = data.readString16Inplace(&len);
				const String16 action(String16(intent, len));
				if (action) {
					if (action == ACTION_CALL) {
						if (!checkPermission(String16("android.permission.CALL_PHONE"), pid, uid)) {
							ALOGE("Permission Denial: "
							"can't call out pid=%d, uid=%d", pid, uid);
							return PERMISSION_DENIED;
						}
					}
				}
			}
		}

		if (InterfaceDescriptor == _telephony) {
			//ITelephony call("10086"), endCall()
			if ((code == 2) || (code == 5)) {
				if (!checkPermission(String16("android.permission.CALL_PHONE"), pid, uid)) {
					ALOGE("Permission Denial: "
					"can't call out pid=%d, uid=%d", pid, uid);
					return PERMISSION_DENIED;
				}
			}
		}	

		if (InterfaceDescriptor == _sendsms) {
			//ISms sendText, sendData, sendMultipartText
			if ((code == 4) || (code == 5) || (code == 6)) {
				if (!checkPermission(String16("android.permission.SEND_SMS"), pid, uid)) {
					ALOGE("Permission Denial: "
					"can't send sms pid=%d, uid=%d", pid, uid);
					return PERMISSION_DENIED;
				}
			}
		}
		
		if (InterfaceDescriptor == _wifi) {
			if (code == 13) {
				if (!checkPermission(String16("android.permission.CHANGE_WIFI_STATE"), pid, uid)) {
						ALOGE("Permission Denial: "
						"can't change wifi state pid=%d, uid=%d", pid, uid);
						return PERMISSION_DENIED;
				}
			}
		}

		if (InterfaceDescriptor == _bluetooth) {
			ALOGE("_bluetooth code = %d", code);
			if (code == 5) {
				if (!checkPermission(String16("android.permission.BLUETOOTH_ADMIN"), pid, uid)) {
					ALOGE("Permission Denial: "
					"can't change bluetooth state pid=%d, uid=%d", pid, uid);
					return PERMISSION_DENIED;
				}
			}
		}
		
		if (InterfaceDescriptor == _contentprovider) {
			//query, insert, delete, update
			if (code == 1) {
				size_t len;
				int type = data.readInt32();

				//scheme
				const char16_t* string = data.readString16Inplace(&len);
				const String16 scheme(String16(string, len));
				if (scheme) {
					if (type == 1)
					{
						ssize_t index = -1;
						index = scheme.findFirst(':');
						if (index != -1) {
							if (parseAuthorityCallLog(string, scheme.size(), index)) {
								if (!checkPermission(String16("android.permission.READ_CALL_LOG"), pid, uid)) {
									ALOGE("Permission Denial: "
									"can't read call log pid=%d, uid=%d", pid, uid);
									return PERMISSION_DENIED;
								}
							}
							if (parseAuthoritySMS(string, scheme.size(), index)) {
								if (!checkPermission(String16("android.permission.READ_SMS"), pid, uid)) {
									ALOGE("Permission Denial: "
									"can't read sms pid=%d, uid=%d", pid, uid);
									return PERMISSION_DENIED;
								}
							}
							if (parseAuthorityContact(string, scheme.size(), index)) {
								if (!checkPermission(String16("android.permission.READ_CONTACTS"), pid, uid)) {
									ALOGE("Permission Denial: "
									"can't read contacts pid=%d, uid=%d", pid, uid);
									return PERMISSION_DENIED;
								}
							}
						}							
					}
					if (type == 3) {
						//authority
						int representation = data.readInt32();
						string = data.readString16Inplace(&len);
						const String16 encoded(String16(string, len));
						string = data.readString16Inplace(&len);
						const String16 decoded(String16(string, len));
						if (decoded) {
							if (decoded == URI_CONTACT) {
								if (!checkPermission(String16("android.permission.READ_CONTACTS"), pid, uid)) {
									ALOGE("Permission Denial: "
									"can't read contacts pid=%d, uid=%d", pid, uid);
									return PERMISSION_DENIED;
								}
							}
							if ((decoded == URI_SMS) || (decoded == URI_MMS) || (decoded == URI_MMS_SMS)) {
								if (!checkPermission(String16("android.permission.READ_SMS"), pid, uid)) {
									ALOGE("Permission Denial: "
									"can't read sms pid=%d, uid=%d", pid, uid);
									return PERMISSION_DENIED;
								}
							}
							if (decoded == URI_CALLLOG) {
								if (!checkPermission(String16("android.permission.READ_CALL_LOG"), pid, uid)) {
									ALOGE("Permission Denial: "
									"can't read contacts pid=%d, uid=%d", pid, uid);
									return PERMISSION_DENIED;
								}
							}							
						}
					}
				}
			}
		}
	
		if (InterfaceDescriptor == _location) {
			if (code == 1) {
				if (!checkPermission(String16("android.permission.ACCESS_FINE_LOCATION"), pid, uid)) {
					ALOGE("Permission Denial: "
					"can't access location pid=%d, uid=%d", pid, uid);
					return PERMISSION_DENIED;
				}
			}
		}

		if (InterfaceDescriptor == _data_network) {
			//setMobileDataEnabled
			if (code == 20) {
				if (!checkPermission(String16("android.permission.CHANGE_NETWORK_STATE"), pid, uid)) {
					ALOGE("Permission Denial: "
					"can't access location pid=%d, uid=%d", pid, uid);
					return PERMISSION_DENIED;
				}
			}
		}


		if (InterfaceDescriptor == _data_network) {
			//send mms, startUsingNetworkFeature
			if (code == 15) {
				ALOGE("_data_network code = %d", code);
				if (!checkPermission(String16("android.permission.SEND_SMS"), pid, uid)) {
					ALOGE("Permission Denial: "
					"can't access location pid=%d, uid=%d", pid, uid);
					return PERMISSION_DENIED;
				}
			}
		}		

		if (InterfaceDescriptor == _camera) {
			if (code == 3) {
				if (!checkPermission(String16("android.permission.CAMERA"), pid, uid)) {
					ALOGE("Permission Denial: "
					"can't use camera pid=%d, uid=%d", pid, uid);
					return PERMISSION_DENIED;
				}
			}
		}
	
		if (InterfaceDescriptor == _audio_record) {
			if (code == 2) {
				if (!checkPermission(String16("android.permission.RECORD_AUDIO"), pid, uid)) {
					ALOGE("Permission Denial: "
						"can't record pid=%d, uid=%d", pid, uid);
					return PERMISSION_DENIED;
				}
			}
		}
		
		if (InterfaceDescriptor == _nfc) {
			if ((code == 4)) {
				if (!checkPermission(String16("android.permission.NFC"), pid, uid)) {
					ALOGE("Permission Denial: "
						"can't record pid=%d, uid=%d", pid, uid);
					return PERMISSION_DENIED;
				}
			}
		}		

	}
	return OK;
}

bool get_taskname(int pid) {
	char *base_pname = NULL;
	FILE *fp;
	char path[MAX_BUF_SIZE];
	char cmdline[MAX_BUF_SIZE];

	sprintf(path, "/proc/%d/cmdline", pid);
	fp = fopen(path, "r");
	if(fp == NULL) {
		return false;
	}
	
	memset(cmdline, 0, sizeof(cmdline));
	if(fread(cmdline, MAX_BUF_SIZE - 1, 1, fp) < 0){
	   fclose(fp);
	   return false;
	}
	fclose(fp);
	
	base_pname = basename(cmdline);
	if (strstr(base_pname, "com.android.cts.")) {
		ALOGE("CTS test is running , %s! ", base_pname);
		return true;
	}
    return false;
}

char *basename(const char *path) {
	register const char *s;
	register const char *p;
	
	p = s = path;
	while (*s) {
		if (*s++ == '/') {
			p = s;
		}
	}
	return (char *) p;
}


}

