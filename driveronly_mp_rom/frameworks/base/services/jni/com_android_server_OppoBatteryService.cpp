/************************************************************************************
** Copyright (C), 2000-2013, OPPO Mobile Comm Corp., Ltd
** All rights reserved.
** 
** VENDOR_EDIT
** 
** Description: - 
**      Oppo Battery Service
** 
** 
** --------------------------- Revision History: --------------------------------
** <author>		                      <data> 	<version >  <desc>
** ------------------------------------------------------------------------------
** yaolang@Plf.DevSrv.Storage  2013/08/01   1.0	    create file
** ------------------------------------------------------------------------------
** 
************************************************************************************/

#define LOG_TAG "OppoBatteryService"

//#define LOG_NDEBUG 0
//#define LOG_NDDEBUG 0
//#define LOG_NIDEBUG 0

#include "JNIHelp.h"
#include "jni.h"
#include <utils/Log.h>
#include <utils/misc.h>

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#include <linux/ioctl.h>

namespace android {

#define POWER_SUPPLY_PATH "/sys/class/power_supply"

struct FieldIds {
    jfieldID mChargerVoltage;
    jfieldID mBatteryCurrent;
    jfieldID mChargeTimeout;
    jfieldID mChargeSocFall;
};
static FieldIds gFieldIds;

struct PowerSupplyPaths {
    char* batteryCharge_NowPath;
    char* batteryCurrent_NowPath;
	char* batteryChargeTimeoutPath;
    char* batteryChargeSocFallPath;
};
static PowerSupplyPaths gPaths;

static int gVoltageDivisor = 1;
static int gElectricityDivisor = 1;

static int readFromFile(const char* path, char* buf, size_t size)
{
    if (!path)
        return -1;
    int fd = open(path, O_RDONLY, 0);
    if (fd == -1) {
        ALOGE("Could not open '%s'", path);
        return -1;
    }
    
    ssize_t count = read(fd, buf, size);
    if (count > 0) {
        while (count > 0 && buf[count-1] == '\n')
            count--;
        buf[count] = '\0';
    } else {
        buf[0] = '\0';
    } 

    close(fd);
    return count;
}

static void setBooleanField(JNIEnv* env, jobject obj, const char* path, jfieldID fieldID)
{
    const int SIZE = 16;
    char buf[SIZE];
    
    jboolean value = false;
    if (readFromFile(path, buf, SIZE) > 0) {
        if (buf[0] != '0') {
            value = true;
        }
    }
    env->SetBooleanField(obj, fieldID, value);
}

static void setIntField(JNIEnv* env, jobject obj, const char* path, jfieldID fieldID)
{
    const int SIZE = 128;
    char buf[SIZE];
    
    jint value = 0;
    if (readFromFile(path, buf, SIZE) > 0) {
        value = atoi(buf);
    }
    env->SetIntField(obj, fieldID, value);
}

static void setVoltageField(JNIEnv* env, jobject obj, const char* path, jfieldID fieldID)
{
    const int SIZE = 128;
    char buf[SIZE];

    jint value = 0;
    if (readFromFile(path, buf, SIZE) > 0) {
        value = atoi(buf);
        value /= gVoltageDivisor;
    }
    env->SetIntField(obj, fieldID, value);
}

static void setElectricityField(JNIEnv* env, jobject obj, const char* path, jfieldID fieldID)
{
    const int SIZE = 128;
    char buf[SIZE];

    jint value = 0;
    if (readFromFile(path, buf, SIZE) > 0) {
        value = atoi(buf);
        value /= gElectricityDivisor;
    }
    env->SetIntField(obj, fieldID, value);
}

static void android_server_OppoBatteryService_update(JNIEnv* env, jobject obj)
{	
    setElectricityField(env, obj, gPaths.batteryCharge_NowPath, gFieldIds.mChargerVoltage);
    setElectricityField(env, obj, gPaths.batteryCurrent_NowPath, gFieldIds.mBatteryCurrent);
    setBooleanField(env, obj, gPaths.batteryChargeTimeoutPath, gFieldIds.mChargeTimeout);
    setBooleanField(env, obj, gPaths.batteryChargeSocFallPath, gFieldIds.mChargeSocFall);
}

static JNINativeMethod sMethods[] = {
     /* name, signature, funcPtr */
	{"native_update", "()V", (void*)android_server_OppoBatteryService_update},
};

int register_android_server_OppoBatteryService(JNIEnv* env)
{
    char    path[PATH_MAX];
    struct dirent* entry;

    DIR* dir = opendir(POWER_SUPPLY_PATH);
    if (dir == NULL) {
        ALOGE("Could not open %s\n", POWER_SUPPLY_PATH);
    } else {
        while ((entry = readdir(dir))) {
            const char* name = entry->d_name;

            // ignore "." and ".."
            if (name[0] == '.' && (name[1] == 0 || (name[1] == '.' && name[2] == 0))) {
                continue;
            }

            char buf[20];
            // Look for "type" file in each subdirectory
            snprintf(path, sizeof(path), "%s/%s/type", POWER_SUPPLY_PATH, name);
            int length = readFromFile(path, buf, sizeof(buf));
            if (length > 0) {
                if (buf[length - 1] == '\n')
                    buf[length - 1] = 0;

                if (strcmp(buf, "Battery") == 0) {
	                // 10087
	                snprintf(path, sizeof(path), "%s/%s/ChargerVoltage", POWER_SUPPLY_PATH, name);
	                if (access(path, R_OK) == 0) {
	                    gPaths.batteryCharge_NowPath = strdup(path);
	                } else {
		                // 10089
		                snprintf(path, sizeof(path), "%s/%s/charge_now", POWER_SUPPLY_PATH, name);
		                if (access(path, R_OK) == 0) {
		                    gPaths.batteryCharge_NowPath = strdup(path);
    					}
					}
					// 10087
					snprintf(path, sizeof(path), "%s/%s/BatteryAverageCurrent", POWER_SUPPLY_PATH, name);
					if (access(path, R_OK) == 0) {
						gPaths.batteryCurrent_NowPath = strdup(path);
					} else {
					// 10089
						snprintf(path, sizeof(path), "%s/%s/current_now", POWER_SUPPLY_PATH, name);
						if (access(path, R_OK) == 0) {
							gPaths.batteryCurrent_NowPath = strdup(path);
						}
					}

                    snprintf(path, sizeof(path), "%s/%s/charge_timeout", POWER_SUPPLY_PATH, name);
                    if (access(path, R_OK) == 0) {
                        gPaths.batteryChargeTimeoutPath = strdup(path);
                    }
                    
                    snprintf(path, sizeof(path), "%s/%s/charge_soc_fall", POWER_SUPPLY_PATH, name);
                    if (access(path, R_OK) == 0) {
                        gPaths.batteryChargeSocFallPath = strdup(path);
                    }  
				}
			}
        }
        closedir(dir);
    }

    if (!gPaths.batteryCharge_NowPath)
        ALOGE("batteryCharge_NowPath not found");
    if (!gPaths.batteryCurrent_NowPath)
        ALOGE("batteryCurrent_NowPath not found");
	if (!gPaths.batteryChargeTimeoutPath)
        ALOGE("batteryChargeTimeoutPath not found");
    if (!gPaths.batteryChargeSocFallPath)
        ALOGE("batteryChargeSocFallPath not found");

    jclass clazz = env->FindClass("com/android/server/OppoBatteryService");

    if (clazz == NULL) {
        ALOGE("Can't find com/android/server/OppoBatteryService");
        return -1;
    }
    gFieldIds.mChargerVoltage = env->GetFieldID(clazz, "mChargerVoltage", "I");
    gFieldIds.mBatteryCurrent = env->GetFieldID(clazz, "mBatteryCurrent", "I");
    gFieldIds.mChargeTimeout= env->GetFieldID(clazz, "mChargeTimeout", "Z");
    gFieldIds.mChargeSocFall= env->GetFieldID(clazz, "mChargeSocFall", "Z");

    LOG_FATAL_IF(gFieldIds.mChargerVoltage == NULL, "Unable to find BatteryService.CHARGER_VOLTAGE_PATH");
    LOG_FATAL_IF(gFieldIds.mBatteryCurrent == NULL, "Unable to find BatteryService.BATTERY_CURRENT_PATH");
	LOG_FATAL_IF(gFieldIds.mChargeTimeout== NULL, "Unable to find BatteryService.BATTERY_CHARGETIMEOUT_PATH");
    LOG_FATAL_IF(gFieldIds.mChargeSocFall== NULL, "Unable to find BatteryService.BATTERY_CHARGESOCFALL_PATH");

    return jniRegisterNativeMethods(env, "com/android/server/OppoBatteryService", sMethods, NELEM(sMethods));
}

} /* namespace android */
