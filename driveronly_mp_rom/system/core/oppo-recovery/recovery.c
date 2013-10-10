/*
 * Copyright (C) 2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  * See the License for the specific language governing permissions and * limitations under the License.
 */

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <limits.h>
#include <linux/input.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <dirent.h>

#ifdef ERASE_MODEM_NV_12025
#include <ext_nv_bk_parti_lib.h>
#endif

#include "bootloader.h"
#include "common.h"
#include "cutils/properties.h"
#include "cutils/android_reboot.h"
#include "install.h"
#include "minzip/DirUtil.h"
#include "roots.h"
#include "recovery_ui.h"
#include "oppo/src/oppo.h"
#include "oppo_intent.h"
#include "mtdutils/mounts.h"


static const struct option OPTIONS[] = {
  { "send_intent", required_argument, NULL, 's' },
  { "update_package", required_argument, NULL, 'u' },
/* OPPO 2013-01-16 jizhengkang azx Add begin for reason */
  { "wipe_data_only", no_argument, NULL, 'd' },
/* OPPO 2013-01-16 jizhengkang Add end */
  { "wipe_data", no_argument, NULL, 'w' },
  { "wipe_cache", no_argument, NULL, 'c' },
  { "show_text", no_argument, NULL, 't' },
  { "locale", required_argument, NULL, 'l' },
  { NULL, 0, NULL, 0 },
};

static const char *COMMAND_FILE = "/cache/recovery/command";
static const char *INTENT_FILE = "/cache/recovery/intent";
static const char *LOG_FILE = "/cache/recovery/log";
static const char *LAST_LOG_FILE = "/cache/recovery/last_log";
static const char *LAST_INSTALL_FILE = "/cache/recovery/last_install";
static const char *CACHE_ROOT = "/cache";
static const char *SDCARD_ROOT = "/sdcard";
static const char *TEMPORARY_LOG_FILE = "/tmp/recovery.log";
static const char *TEMPORARY_INSTALL_FILE = "/tmp/last_install";
static const char *SIDELOAD_TEMP_DIR = "/tmp/sideload";

/* OPPO 2013-01-17 jizhengkang azx Add begin for reason */
int default_language = 0;
int record_language = 0;
/* OPPO 2013-01-17 jizhengkang azx Add end */

/*
 * The recovery tool communicates with the main system through /cache files.
 *   /cache/recovery/command - INPUT - command line for tool, one arg per line
 *   /cache/recovery/log - OUTPUT - combined log file from recovery run(s)
 *   /cache/recovery/intent - OUTPUT - intent that was passed in
 *
 * The arguments which may be supplied in the recovery.command file:
 *   --send_intent=anystring - write the text out to recovery.intent
 *   --update_package=path - verify install an OTA package file
 *   --wipe_data - erase user data (and cache), then reboot
 *   --wipe_cache - wipe cache (but not user data), then reboot
 *   --set_encrypted_filesystem=on|off - enables / diasables encrypted fs
 *
 * After completing, we remove /cache/recovery/command and reboot.
 * Arguments may also be supplied in the bootloader control block (BCB).
 * These important scenarios must be safely restartable at any point:
 *
 * FACTORY RESET
 * 1. user selects "factory reset"
 * 2. main system writes "--wipe_data" to /cache/recovery/command
 * 3. main system reboots into recovery
 * 4. get_args() writes BCB with "boot-recovery" and "--wipe_data"
 *    -- after this, rebooting will restart the erase --
 * 5. erase_volume() reformats /data
 * 6. erase_volume() reformats /cache
 * 7. finish_recovery() erases BCB
 *    -- after this, rebooting will restart the main system --
 * 8. main() calls reboot() to boot main system
 *
 * OTA INSTALL
 * 1. main system downloads OTA package to /cache/some-filename.zip
 * 2. main system writes "--update_package=/cache/some-filename.zip"
 * 3. main system reboots into recovery
 * 4. get_args() writes BCB with "boot-recovery" and "--update_package=..."
 *    -- after this, rebooting will attempt to reinstall the update --
 * 5. install_package() attempts to install the update
 *    NOTE: the package install must itself be restartable from any point
 * 6. finish_recovery() erases BCB
 *    -- after this, rebooting will (try to) restart the main system --
 * 7. ** if install failed **
 *    7a. prompt_and_wait() shows an error icon and waits for the user
 *    7b; the user reboots (pulling the battery, etc) into the main system
 * 8. main() calls maybe_install_firmware_update()
 *    ** if the update contained radio/hboot firmware **:
 *    8a. m_i_f_u() writes BCB with "boot-recovery" and "--wipe_cache"
 *        -- after this, rebooting will reformat cache & restart main system --
 *    8b. m_i_f_u() writes firmware image into raw cache partition
 *    8c. m_i_f_u() writes BCB with "update-radio/hboot" and "--wipe_cache"
 *        -- after this, rebooting will attempt to reinstall firmware --
 *    8d. bootloader tries to flash firmware
 *    8e. bootloader writes BCB with "boot-recovery" (keeping "--wipe_cache")
 *        -- after this, rebooting will reformat cache & restart main system --
 *    8f. erase_volume() reformats /cache
 *    8g. finish_recovery() erases BCB
 *        -- after this, rebooting will (try to) restart the main system --
 * 9. main() calls reboot() to boot main system
 */

struct selabel_handle *sehandle;

static const int MAX_ARG_LENGTH = 4096;
static const int MAX_ARGS = 100;

// open a given path, mounting partitions as necessary
FILE*
fopen_path(const char *path, const char *mode) {
    if (ensure_path_mounted(path) != 0) {
        LOGE("Can't mount %s\n", path);
        return NULL;
    }

    // When writing, try to create the containing directory, if necessary.
    // Use generous permissions, the system (init.rc) will reset them.
    if (strchr("wa", mode[0])) dirCreateHierarchy(path, 0777, NULL, 1,sehandle);

    FILE *fp = fopen(path, mode);
    return fp;
}

// close a file, log an error if the error indicator is set
static void
check_and_fclose(FILE *fp, const char *name) {
    fflush(fp);
    if (ferror(fp)) LOGE("Error in %s\n(%s)\n", name, strerror(errno));
    fclose(fp);
}

// command line args come from, in decreasing precedence:
//   - the actual command line
//   - the bootloader control block (one per line, after "recovery")
//   - the contents of COMMAND_FILE (one per line)
static void
get_args(int *argc, char ***argv) {
    struct bootloader_message boot;
    memset(&boot, 0, sizeof(boot));
    get_bootloader_message(&boot);  // this may fail, leaving a zeroed structure
/* OPPO 2013-01-17 jizhengkang azx Add begin for reason */
	if (boot.language[0] != 0 && boot.language[0] != 255) {
		LOGI("Boot language: %.*s\n", sizeof(boot.language), boot.language);
		default_language = boot.language[0] - '0';
		record_language = default_language;
	}
/* OPPO 2013-01-17 jizhengkang azx Add end */

    if (boot.command[0] != 0 && boot.command[0] != 255) {
        LOGI("Boot command: %.*s\n", sizeof(boot.command), boot.command);
    }

    if (boot.status[0] != 0 && boot.status[0] != 255) {
        LOGI("Boot status: %.*s\n", sizeof(boot.status), boot.status);
    }

    // --- if arguments weren't supplied, look in the bootloader control block
    if (*argc <= 1) {
        boot.recovery[sizeof(boot.recovery) - 1] = '\0';  // Ensure termination
        const char *arg = strtok(boot.recovery, "\n");
        if (arg != NULL && !strcmp(arg, "recovery")) {
            *argv = (char **) malloc(sizeof(char *) * MAX_ARGS);
            (*argv)[0] = strdup(arg);
            for (*argc = 1; *argc < MAX_ARGS; ++*argc) {
                if ((arg = strtok(NULL, "\n")) == NULL) break;
                (*argv)[*argc] = strdup(arg);
            }
            LOGI("Got arguments from boot message\n");
        } else if (boot.recovery[0] != 0 && boot.recovery[0] != 255) {
            LOGE("Bad boot message\n\"%.20s\"\n", boot.recovery);
        }
    }
    // --- if that doesn't work, try the command file
    if (*argc <= 1) {
        FILE *fp = fopen_path(COMMAND_FILE, "r");
        if (fp != NULL) {
            char *argv0 = (*argv)[0];
            *argv = (char **) malloc(sizeof(char *) * MAX_ARGS);
            (*argv)[0] = argv0;  // use the same program name

            char buf[MAX_ARG_LENGTH];
            for (*argc = 1; *argc < MAX_ARGS; ++*argc) {
                if (!fgets(buf, sizeof(buf), fp)) break;
                (*argv)[*argc] = strdup(strtok(buf, "\r\n"));  // Strip newline.
            }

            check_and_fclose(fp, COMMAND_FILE);
            LOGI("Got arguments from %s\n", COMMAND_FILE);
        }
    }
    // --> write the arguments we have back into the bootloader control block
    // always boot into recovery after this (until finish_recovery() is called)
    strlcpy(boot.command, "boot-recovery", sizeof(boot.command));
    strlcpy(boot.recovery, "recovery\n", sizeof(boot.recovery));
    int i;
    for (i = 1; i < *argc; ++i) {
        strlcat(boot.recovery, (*argv)[i], sizeof(boot.recovery));
        strlcat(boot.recovery, "\n", sizeof(boot.recovery));
    }
    set_bootloader_message(&boot);
}

void
set_sdcard_update_bootloader_message(char *recovery_command, char *parameter) {
    struct bootloader_message boot;
    memset(&boot, 0, sizeof(boot));
    strlcpy(boot.command, "boot-recovery", sizeof(boot.command));
    strlcpy(boot.recovery, "recovery\n", sizeof(boot.recovery));
	strlcat(boot.recovery, recovery_command, sizeof(boot.recovery));
	if(parameter != NULL)
		strlcat(boot.recovery, parameter, sizeof(boot.recovery));
	strlcat(boot.recovery, "\n", sizeof(boot.recovery));
//	strlcpy(boot.language, '0' + record_language, sizeof(boot.language));
	boot.language[0] = '0' + record_language;
	LOGE("this is new recovery changed by azx\n");
    set_bootloader_message(&boot);
}

// How much of the temp log we have copied to the copy in cache.
static long tmplog_offset = 0;

static void
copy_log_file(const char* source, const char* destination, int append) {
    FILE *log = fopen_path(destination, append ? "a" : "w");
    if (log == NULL) {
        LOGE("Can't open %s\n", destination);
    } else {
        FILE *tmplog = fopen(source, "r");
        if (tmplog != NULL) {
            if (append) {
                fseek(tmplog, tmplog_offset, SEEK_SET);  // Since last write
            }
            char buf[4096];
            while (fgets(buf, sizeof(buf), tmplog)) fputs(buf, log);
            if (append) {
                tmplog_offset = ftell(tmplog);
            }
            check_and_fclose(tmplog, source);
        }
        check_and_fclose(log, destination);
    }
}


// clear the recovery command and prepare to boot a (hopefully working) system,
// copy our log file to cache as well (for the system to read), and
// record any intent we were asked to communicate back to the system.
// this function is idempotent: call it as many times as you like.
static void
finish_recovery(const char *send_intent) {
    // By this point, we're ready to return to the main system...
    if (send_intent != NULL) {
        FILE *fp = fopen_path(INTENT_FILE, "w");
        if (fp == NULL) {
            LOGE("Can't open %s\n", INTENT_FILE);
        } else {
            fputs(send_intent, fp);
            check_and_fclose(fp, INTENT_FILE);
        }
    }

    // Copy logs to cache so the system can find out what happened.
    copy_log_file(TEMPORARY_LOG_FILE, LOG_FILE, true);
    copy_log_file(TEMPORARY_LOG_FILE, LAST_LOG_FILE, false);
    copy_log_file(TEMPORARY_INSTALL_FILE, LAST_INSTALL_FILE, false);
    chmod(LOG_FILE, 0600);
    chown(LOG_FILE, 1000, 1000);   // system user
    chmod(LAST_LOG_FILE, 0640);
    chmod(LAST_INSTALL_FILE, 0644);

	chmod(INTENT_FILE, 0777);

    // Reset to mormal system boot so recovery won't cycle indefinitely.
    struct bootloader_message boot;
    memset(&boot, 0, sizeof(boot));
    set_bootloader_message(&boot);

    // Remove the command file, so recovery won't repeat indefinitely.
    if (ensure_path_mounted(COMMAND_FILE) != 0 ||
        (unlink(COMMAND_FILE) && errno != ENOENT)) {
        LOGW("Can't unlink %s\n", COMMAND_FILE);
    }

    ensure_path_unmounted(CACHE_ROOT);
    sync();  // For good measure.
}

static int
erase_volume(const char *volume) {
    ui_set_background(BACKGROUND_ICON_INSTALLING);
    ui_show_indeterminate_progress();
    ui_print("Formatting %s...\n", volume);

    ensure_path_unmounted(volume);

    if (strcmp(volume, "/cache") == 0) {
        // Any part of the log we'd copied to cache is now gone.
        // Reset the pointer so we copy from the beginning of the temp
        // log.
        tmplog_offset = 0;
    }

    return format_volume(volume);
}

static char*
copy_sideloaded_package(const char* original_path) {
  if (ensure_path_mounted(original_path) != 0) {
    LOGE("Can't mount %s\n", original_path);
    return NULL;
  }

  if (ensure_path_mounted(SIDELOAD_TEMP_DIR) != 0) {
    LOGE("Can't mount %s\n", SIDELOAD_TEMP_DIR);
    return NULL;
  }

  if (mkdir(SIDELOAD_TEMP_DIR, 0700) != 0) {
    if (errno != EEXIST) {
      LOGE("Can't mkdir %s (%s)\n", SIDELOAD_TEMP_DIR, strerror(errno));
      return NULL;
    }
  }

  // verify that SIDELOAD_TEMP_DIR is exactly what we expect: a
  // directory, owned by root, readable and writable only by root.
  struct stat st;
  if (stat(SIDELOAD_TEMP_DIR, &st) != 0) {
    LOGE("failed to stat %s (%s)\n", SIDELOAD_TEMP_DIR, strerror(errno));
    return NULL;
  }
  if (!S_ISDIR(st.st_mode)) {
    LOGE("%s isn't a directory\n", SIDELOAD_TEMP_DIR);
    return NULL;
  }
  if ((st.st_mode & 0777) != 0700) {
    LOGE("%s has perms %o\n", SIDELOAD_TEMP_DIR, st.st_mode);
    return NULL;
  }
  if (st.st_uid != 0) {
    LOGE("%s owned by %lu; not root\n", SIDELOAD_TEMP_DIR, st.st_uid);
    return NULL;
  }

  char copy_path[PATH_MAX];
  strcpy(copy_path, SIDELOAD_TEMP_DIR);
  strcat(copy_path, "/package.zip");

  char* buffer = malloc(BUFSIZ);
  if (buffer == NULL) {
    LOGE("Failed to allocate buffer\n");
    return NULL;
  }

  size_t read;
  FILE* fin = fopen(original_path, "rb");
  if (fin == NULL) {
    LOGE("Failed to open %s (%s)\n", original_path, strerror(errno));
    return NULL;
  }
  FILE* fout = fopen(copy_path, "wb");
  if (fout == NULL) {
    LOGE("Failed to open %s (%s)\n", copy_path, strerror(errno));
    return NULL;
  }

  while ((read = fread(buffer, 1, BUFSIZ, fin)) > 0) {
    if (fwrite(buffer, 1, read, fout) != read) {
      LOGE("Short write of %s (%s)\n", copy_path, strerror(errno));
      return NULL;
    }
  }

  free(buffer);

  if (fclose(fout) != 0) {
    LOGE("Failed to close %s (%s)\n", copy_path, strerror(errno));
    return NULL;
  }

  if (fclose(fin) != 0) {
    LOGE("Failed to close %s (%s)\n", original_path, strerror(errno));
    return NULL;
  }

  // "adb push" is happy to overwrite read-only files when it's
  // running as root, but we'll try anyway.
  if (chmod(copy_path, 0400) != 0) {
    LOGE("Failed to chmod %s (%s)\n", copy_path, strerror(errno));
    return NULL;
  }

  return strdup(copy_path);
}
/*
 * for register intent for ui send intent to some operation
 */
#define return_intent_result_if_fail(p) if (!(p)) \
    {printf("function %s(line %d) " #p "\n", __FUNCTION__, __LINE__); \
    intent_result.ret = -1; return &intent_result;}
#define return_intent_ok(val, str) intent_result.ret = val; \
    if (str != NULL) \
        strncpy(intent_result.result, str, INTENT_RESULT_LEN); \
     else intent_result.result[0] = '\0'; \
     return &intent_result
//INTENT_MOUNT, mount recovery.fstab
static intentResult* intent_mount(int argc, char* argv[])
{
    printf("intent_system: start ..... cmd(%s)\n", argv[0]);
    return_intent_result_if_fail(argc == 1);
    return_intent_result_if_fail(argv != NULL);
    int result = ensure_path_mounted(argv[0]);
    printf("intent_system: end result(%d)\n", result);
    if (result == 0)
        return oppoIntent_result_set(result, "mounted");
    return oppoIntent_result_set(result, "fail");
}

//INTENT_ISMOUNT, mount recovery.fstab
static intentResult* intent_ismount(int argc, char* argv[])
{
    return_intent_result_if_fail(argc == 1);
    return_intent_result_if_fail(argv != NULL);
    int result = is_path_mounted(argv[0]);
    return oppoIntent_result_set(result, NULL);
}
//INTENT_MOUNT, mount recovery.fstab
static intentResult* intent_unmount(int argc, char* argv[])
{
    return_intent_result_if_fail(argc == 1);
    return_intent_result_if_fail(argv != NULL);
    int result = ensure_path_unmounted(argv[0]);
    if (result == 0)
        return oppoIntent_result_set(result, "ok");
    return oppoIntent_result_set(result, "fail");
}
//INTENT_WIPE ,wipe "/data" | "cache"
static intentResult* intent_wipe(int argc, char *argv[])
{
    return_intent_result_if_fail(argc == 1);
    return_intent_result_if_fail(argv != NULL);
    int result = erase_volume(argv[0]);

    printf("[intent_wipe] wipe (%s), result=%d\n", argv[0], result);
    if (result == 0)
        return oppoIntent_result_set(result, "ok");
    return oppoIntent_result_set(result, "fail");	
}
//INTENT_REBOOT, reboot, 0, NULL | reboot, 1, "recovery" | bootloader |
static intentResult * intent_reboot(int argc, char *argv[])
{
    return_intent_result_if_fail(argc == 1);
    finish_recovery(NULL);
    if(strstr(argv[0], "reboot") != NULL)
        android_reboot(ANDROID_RB_RESTART, 0, 0);
    else if(strstr(argv[0], "poweroff") != NULL)
        android_reboot(ANDROID_RB_POWEROFF, 0, 0);
    else android_reboot(ANDROID_RB_RESTART2, 0, argv[0]);
    return oppoIntent_result_set(0, NULL);
}
//INTENT_INSTALL install path, wipe_cache, install_file
extern STATUS oppo_install(void) ;
extern int sd_path_verify(char* sdpath);
static intentResult* intent_install(int argc, char *argv[])
{
    int ret;
/* OPPO 2013-01-05 jizhengkang Add begin for reason */
    return_intent_result_if_fail(argc == 4);
    return_intent_result_if_fail(argv != NULL);
    int wipe_cache = atoi(argv[1]);
	int wipe_data = atoi(argv[2]);
    int echo = atoi(argv[3]);
    set_sdcard_update_bootloader_message("--update_package=", argv[0]);
/* OPPO 2013-01-05 jizhengkang Add end */

    //ensure /cache mounted
    if (ensure_path_mounted("/cache") != 0) {
        LOGE("Can't mount /cache\n");
    }
	if (ensure_path_unmounted("/system") != 0)
	{
		LOGE("Can't unmount /system\n");
	}
		 
    char *newpath;
    newpath = malloc(100);
    strcpy(newpath, argv[0]);
    sd_path_verify(newpath);
/* OPPO 2013-01-25 jizhengkang azx Modify begin for reason */
	oppoInstall_init(&install_package, newpath, wipe_cache, wipe_data, TEMPORARY_INSTALL_FILE);
/* OPPO 2013-01-25 jizhengkang azx Modify end */

    ret = oppo_install();

    free(newpath);
	
    if (ret == 0) {
         return oppoIntent_result_set(RET_OK, NULL);
    } else {
        return oppoIntent_result_set(RET_FAIL, NULL);
    }    
}

extern int __system(const char *command);
static intentResult* intent_system(int argc, char* argv[])
{
    printf("intent_system: start ..... cmd(%s)\n", argv[0]);
    return_intent_result_if_fail(argc == 1);
    return_intent_result_if_fail(argv != NULL);
    int result = __system(argv[0]);
    assert_if_fail(result == 0);
	printf("intent_system: end .....\n");
    return oppoIntent_result_set(result, NULL);
}

static void
print_property(const char *key, const char *name, void *cookie) {
    printf("%s=%s\n", key, name);
}

/* OPPO 2013-01-03 jizhengkang Add begin for reason */
void feed_back(const char *send_intent)
{
    if (send_intent != NULL) {
        FILE *fp = fopen_path(INTENT_FILE, "w");
        if (fp == NULL) {
			LOGE("Can't open %s\n", INTENT_FILE);
        } else {
            fputs(send_intent, fp);
            check_and_fclose(fp, INTENT_FILE);
        }
    }
}

static void note_running_command(const char *running_command)
{
	struct bootloader_message boot;
	memset(&boot, 0, sizeof(boot));

    get_bootloader_message(&boot);

	strlcpy(boot.running_command, running_command, sizeof(boot.running_command));
	set_bootloader_message(&boot);
}

#ifdef ERASE_MODEM_NV_12025
#define  MODEM_ST1_BLOCK  "/dev/block/platform/msm_sdcc.1/by-name/modemst1"
#define  MODEM_ST2_BLOCK  "/dev/block/platform/msm_sdcc.1/by-name/modemst2"

#define  OPPO_DYC_NVBK_BLOCK "/dev/block/platform/msm_sdcc.1/by-name/oppodycnvbk"
#define  OPPO_STA_NVBK_BLOCK "/dev/block/platform/msm_sdcc.1/by-name/oppostanvbk"

#define  TEST_WRITE_SIZE  1024
#define  NV_BLOCK_SIZE    3*1024*1024

/*===========================================================================
 *  METHOD:
 *  lib_validate_nv_backup_file
 *
 *  DESCRIPTION:
 *  read all data out to the buf, and validate the content.
 *
 *  PARAMETERS
 *  path        - file path
 *  type        - image type static or dynamic
 *
 *  RETURN VALUE:
 *  OPPO_NV_PARTITION_TYPE  0:SUCCESS /other: FAILED
 *  ===========================================================================*/ 
static int lib_validate_nv_backup_file(const char* path, OPPO_NV_PARTITION_TYPE type)
{
	int iFd;
    uint8_t* fbuf = NULL;
    ssize_t num_bytes;
    long long file_size_in_bytes = NV_BLOCK_SIZE;

	iFd  = open(path,O_RDWR);
	if(iFd < 0 ) {
		LOGE("Open partition %s failed!\n", path);
		goto fail;
	}

	 fbuf = (uint8_t*)malloc(file_size_in_bytes);
	 if (NULL==fbuf) {
		 LOGE("Malloc memory failed!\n");
		 goto fail;
	 }

	 num_bytes = read(iFd, fbuf, file_size_in_bytes);
	 if ((num_bytes < 0) || ((size_t)num_bytes != file_size_in_bytes)) {
	 	LOGE("Read failed\n");
		goto fail;
	 }

	close(iFd);
    return oppo_nvbk_parti_validate(fbuf, file_size_in_bytes, type);
    
fail:
	close(iFd);
    return -1;
}

static int eraseRawBlock(const char* path, int file_size_in_bytes)
{
	int iFd;
	char acWriteBuf[TEST_WRITE_SIZE] = {0};
	int iResult = -1;
	int lWriteLen = 0;
	
	iFd  = open(path,O_RDWR);
	if(iFd < 0 ) {
		LOGE("Open %s failed\n", path);
		return -1;
	}
	
	while(1) {
		iResult = write(iFd,acWriteBuf,TEST_WRITE_SIZE);

		if(iResult < 0) {
			LOGE("Write 0 to %s error!\n", path);
			close(iFd);
			return -1;
		}
		
		lWriteLen += iResult;
		if(lWriteLen >= file_size_in_bytes) {
			break;
		}
		
	}	
	close(iFd);
    return 0;
}

void erase_modem_nv()
{
	int iResult = -1;	

	LOGE("Verify and backup nv\n");
	
	//static nv backup check
	iResult = lib_validate_nv_backup_file(OPPO_STA_NVBK_BLOCK, OPPO_NV_PARTITION_STATIC_PARTI);
	if(iResult != OPPO_NV_ERROR_REC_NONE_ERROR) {
		LOGE("Verify static backup nv partition(/oppostanvbk) failed!\n");
		return;
	}
	
	//Dynamic nv backup check
	iResult = lib_validate_nv_backup_file(OPPO_DYC_NVBK_BLOCK, OPPO_NV_PARTITION_DYNAMIC_PARTI);
	if(iResult != OPPO_NV_ERROR_REC_NONE_ERROR) {
		LOGE("Verify dynamic backup nv partition(/oppodycnvbk) failed!\n");
		return;
	}

	LOGE("Formating nv partition...\n");

	//erase MODEM_ST1_BLOCK
	iResult = eraseRawBlock(MODEM_ST1_BLOCK, NV_BLOCK_SIZE);
	if(iResult != 0) {
		LOGE("Format partition(/modemst1) failed!\n");
		return;
	}

	//erase MODEM_ST2_BLOCK
	iResult = eraseRawBlock(MODEM_ST2_BLOCK, NV_BLOCK_SIZE);
	if(iResult != 0) {
		LOGE("Format partition(/modemst2) failed!\n");
		return;
	}

}
#endif
/* OPPO 2013-01-03 jizhengkang Add end */

extern int oppo_wipe_data();
extern int oppo_wipe_cache();
extern int oppo_wipe_factory();

int auto_install = 0;
int
main(int argc, char **argv) {
    time_t start = time(NULL);

    // If these fail, there's not really anywhere to complain...
//#ifndef DEBUG
    //unlink(TEMPORARY_LOG_FILE);
//#endif

    freopen(TEMPORARY_LOG_FILE, "a", stdout); setbuf(stdout, NULL);
    freopen(TEMPORARY_LOG_FILE, "a", stderr); setbuf(stderr, NULL);
    printf("Starting recovery on %s", ctime(&start));
    //oppoIntent init
    oppoIntent_init(10);
    oppoIntent_register(INTENT_MOUNT, &intent_mount);
    oppoIntent_register(INTENT_ISMOUNT, &intent_ismount);
    oppoIntent_register(INTENT_UNMOUNT, &intent_unmount);
    oppoIntent_register(INTENT_REBOOT, &intent_reboot);
    oppoIntent_register(INTENT_INSTALL, &intent_install);
    oppoIntent_register(INTENT_WIPE, &intent_wipe);
    oppoIntent_register(INTENT_SYSTEM, &intent_system);
    load_volume_table();
    get_args(&argc, &argv);
    device_ui_init();

	ensure_path_unmounted("/data");

    struct bootloader_message boot;
    memset(&boot, 0, sizeof(boot));
    set_bootloader_message(&boot);

    int previous_runs = 0;
    const char *send_intent = NULL;
    const char *update_package = NULL;
    int wipe_data = 0, wipe_cache = 0;

    int arg;

    device_recovery_start();

#ifdef HAVE_SELINUX
    struct selinux_opt seopts[] = {
      { SELABEL_OPT_PATH, "/file_contexts" }
    };

    sehandle = selabel_open(SELABEL_CTX_FILE, seopts, 1);

    if (!sehandle) {
        fprintf(stderr, "Warning: No file_contexts\n");
        ui->Print("Warning:  No file_contexts\n");
    }
#endif

#if 0//huanggd for test install update.zip    
    argv[1] = strdup("--wipe_cache");
    argv[2] = strdup("--wipe_data");
    argv[3] = strdup("--update_package=/sdcard/update.zip");
    argv[4] = strdup("--update_package=/sdcard/update.zip");
    argv[5] = strdup("--update_package=/sdcard/update.zip");
    argc=6;
#endif

    printf("Command:");
    for (arg = 0; arg < argc; arg++) {
        printf(" \"%s\"", argv[arg]);
    }
    printf("\n");

    if (update_package) {
        // For backwards compatibility on the cache partition only, if
        // we're given an old 'root' path "CACHE:foo", change it to
        // "/cache/foo".
        if (strncmp(update_package, "CACHE:", 6) == 0) {
            int len = strlen(update_package) + 10;
            char* modified_path = malloc(len);
            strlcpy(modified_path, "/cache/", len);
            strlcat(modified_path, update_package+6, len);
            printf("(replacing path \"%s\" with \"%s\")\n",
                   update_package, modified_path);
            update_package = modified_path;
        }
    }
    printf("\n");

    property_list(print_property, NULL);
    printf("\n");

    int status = INSTALL_SUCCESS;
/* OPPO 2013-01-03 jizhengkang Add begin for reason */
	status = INSTALL_ERROR;
	char former_command[256];
	int persist_update = 0;
	if (ensure_path_mounted("/cache") != 0) {
		LOGE("Can't mount /cache\n");
    }
	while ((arg = getopt_long(argc, argv, "", OPTIONS, NULL)) != -1) {
        switch (arg) {
        case 'p': previous_runs = atoi(optarg); break;
        case 's': send_intent = optarg; break;
        case 'u': {
			status = INSTALL_SUCCESS;
			strcpy(former_command, "--update_package=");
			/*0:update success
			  1:update failed
			*/
			send_intent = "0";
			memset(&boot, 0, sizeof(boot));

    		get_bootloader_message(&boot);
			strcat(former_command, optarg);
			if(!(*(boot.running_command)) || !strncmp(boot.running_command, former_command, strlen(former_command)))
				persist_update = 1;
			if (strncmp(boot.running_command, former_command, strlen(former_command)) && !persist_update)	{
				ui_print("%s has been installed\n", former_command);
				persist_update = 0;
				break;
			}
			note_running_command(former_command);
			auto_install = 1;
			oppoIntent_send(INTENT_INSTALL, 4, optarg, "0", "0", "0");
			auto_install = 0;
			status = oppoIntent_result_get_int();
			sync();
			usleep(2000000);
        	if (status != INSTALL_SUCCESS) {
				send_intent = "1";
				feed_back(send_intent);
				ui_print("Installation aborted.\n");
        	}
			break;
        }
        case 'w': {
			status = INSTALL_SUCCESS;
			if (device_wipe_data()) status = INSTALL_ERROR;
			if (oppo_wipe_factory()) status = INSTALL_ERROR;//display the same effect
			
			if (status != INSTALL_SUCCESS) ui_print("Data wipe failed.\n");
			break;
        }

		case 'd': {
			status = INSTALL_SUCCESS;
			if (oppo_wipe_data()) status = INSTALL_ERROR;
			if (status != INSTALL_SUCCESS) ui_print("Data_only wipe failed.\n");
			break;
        }

        case 'c': {
			status = INSTALL_SUCCESS;
			if (oppo_wipe_cache()) status = INSTALL_ERROR;
			if (status != INSTALL_SUCCESS) ui_print("Cache wipe failed.\n");
			break;
        }
        case '?':
			LOGE("Invalid command argument\n");
			status = INSTALL_ERROR;
            continue;
        }
		if (status != INSTALL_SUCCESS)	break;
    }
/* OPPO 2013-01-03 jizhengkang Add end */
    if (status != INSTALL_SUCCESS) device_main_ui_show();//show menu
    ui_print("main_ui_release...\n");
    device_main_ui_release();
    // Otherwise, get ready to boot the main system...
    ui_print("finish_recovery...\n");
    finish_recovery(send_intent);
    ui_print("Rebooting...\n");
    android_reboot(ANDROID_RB_RESTART, 0, 0);
    //android_reboot(ANDROID_RB_RESTART2, 0, "recovery");	//huanggd for test install update.zip
    return EXIT_SUCCESS;
}
