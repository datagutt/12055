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
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <errno.h>
#include <stdlib.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#if 1 //wschen 2012-07-10
#include <sys/ioctl.h>
#include <fcntl.h>
#include "../../mediatek/kernel/include/sd_misc.h"
#endif
#include "mtdutils/mtdutils.h"
#include "mtdutils/mounts.h"
#include "roots.h"
#include "common.h"
#ifdef __cplusplus
extern "C" {
#endif
#include "make_ext4fs.h"
#ifdef __cplusplus
}
#endif

static int num_volumes = 0;
static Volume* device_volumes = NULL;

extern struct selabel_handle *sehandle;

static int parse_options(char* options, Volume* volume) {
    char* option;
    while ((option = strtok(options, ","))) {
        options = NULL;

        if (strncmp(option, "length=", 7) == 0) {
            volume->length = strtoll(option+7, NULL, 10);
        } else {
            LOGE("bad option \"%s\"\n", option);
            return -1;
        }
    }
    return 0;
}

void load_volume_table() {
    int alloc = 2;
    device_volumes = (Volume*)malloc(alloc * sizeof(Volume));

    // Insert an entry for /tmp, which is the ramdisk and is always mounted.
    device_volumes[0].mount_point = "/tmp";
    device_volumes[0].fs_type = "ramdisk";
    device_volumes[0].device = NULL;
    device_volumes[0].device2 = NULL;
    device_volumes[0].length = 0;
    num_volumes = 1;

#if 1 //wschen 2011-12-23 auto workaround if recovery.fstab is wrong, phone is eMMC but recovery.fstab is NAND
#define NAND_TYPE    0
#define EMMC_TYPE    1
#define UNKNOWN_TYPE 2

#define CACHE_INDEX  0
#define DATA_INDEX   1
#define SYSTEM_INDEX 2
#define FAT_INDEX    3

    int setting_ok = 0;
    int phone_type = 0, fstab_type = UNKNOWN_TYPE;
    int has_fat = 0;
    FILE *fp_fstab, *fp_info;
    char buf[512];
    char p_name[32], p_size[32], p_addr[32], p_actname[64];
    char dev[4][64];
    unsigned int p_type;
    int j;

    fp_info = fopen("/proc/dumchar_info", "r");
    if (fp_info) {
        //ignore the header line
        if (fgets(buf, sizeof(buf), fp_info) != NULL) {
            printf("Partition Information:\n");
            while (fgets(buf, sizeof(buf), fp_info)) {
                printf("%s", buf);
                if (sscanf(buf, "%s %s %s %d %s", p_name, p_size, p_addr, &p_type, p_actname) == 5) {
                    if (!strcmp(p_name, "bmtpool")) {
                        break;
                    }
                    if (!strcmp(p_name, "preloader")) {
                        if (p_type == 2) {
                            phone_type = EMMC_TYPE;
                        } else {
                            phone_type = NAND_TYPE;
                        }
                    } else if (!strcmp(p_name, "fat")) {
                        has_fat = 1;
                        snprintf(dev[FAT_INDEX], sizeof(dev[FAT_INDEX]), "%s", p_actname);
                    } else if (!strcmp(p_name, "cache")) {
                        snprintf(dev[CACHE_INDEX], sizeof(dev[CACHE_INDEX]), "%s", p_actname);
                    } else if (!strcmp(p_name, "usrdata")) {
                        snprintf(dev[DATA_INDEX], sizeof(dev[DATA_INDEX]), "%s", p_actname);
                    } else if (!strcmp(p_name, "android")) {
                        snprintf(dev[SYSTEM_INDEX], sizeof(dev[SYSTEM_INDEX]), "%s", p_actname);
                    }
                }
            }
        }
        fclose(fp_info);
        printf("\n");

        fp_fstab = fopen("/etc/recovery.fstab", "r");
        if (fp_fstab) {
            while (fgets(buf, sizeof(buf), fp_fstab)) {
                for (j = 0; buf[j] && isspace(buf[j]); ++j) {
                }

                if (buf[j] == '\0' || buf[j] == '#') {
                    continue;
                }
                if (strstr(&buf[j], "/boot") == (&buf[j])) {
                    j += strlen("/boot");
                    for (; buf[j] && isspace(buf[j]); ++j) {
                    }
                    if (strstr(&buf[j], "emmc") == (&buf[j])) {
                        fstab_type = EMMC_TYPE;
                    } else if (strstr(&buf[j], "mtd") == (&buf[j])) {
                        fstab_type = NAND_TYPE;
                    }
                    break;
                }
            }

            fclose(fp_fstab);

            if (fstab_type != UNKNOWN_TYPE) {
                if (phone_type != fstab_type) {
                    printf("WARNING : fstab is wrong, phone=%s fstab=%s\n", phone_type == EMMC_TYPE ? "eMMC" : "NAND", fstab_type == EMMC_TYPE ? "eMMC" : "NAND");

#if defined(SUPPORT_SDCARD2) && !defined(MTK_SHARED_SDCARD) //wschen 2012-11-15
                    device_volumes = (Volume*)realloc(device_volumes, 9 * sizeof(Volume));
                    num_volumes = 9;
#else
                    device_volumes = (Volume*)realloc(device_volumes, 8 * sizeof(Volume));
                    num_volumes = 8;
#endif //SUPPORT_SDCARD2

                    if (phone_type == EMMC_TYPE) {
                        //boot
                        device_volumes[1].mount_point = strdup("/boot");
                        device_volumes[1].fs_type = strdup("emmc");
                        device_volumes[1].device = strdup("boot");
                        device_volumes[1].device2 = NULL;
                        device_volumes[1].length = 0;

                        //cache
                        device_volumes[2].mount_point = strdup("/cache");
                        device_volumes[2].fs_type = strdup("ext4");
                        device_volumes[2].device = strdup(dev[CACHE_INDEX]);
                        device_volumes[2].device2 = NULL;
                        device_volumes[2].length = 0;

                        //data
                        device_volumes[3].mount_point = strdup("/data");
                        device_volumes[3].fs_type = strdup("ext4");
                        device_volumes[3].device = strdup(dev[DATA_INDEX]);
                        device_volumes[3].device2 = NULL;
                        device_volumes[3].length = 0;

                        //misc
                        device_volumes[4].mount_point = strdup("/misc");
                        device_volumes[4].fs_type = strdup("emmc");
                        device_volumes[4].device = strdup("misc");
                        device_volumes[4].device2 = NULL;
                        device_volumes[4].length = 0;

                        //recovery
                        device_volumes[5].mount_point = strdup("/recovery");
                        device_volumes[5].fs_type = strdup("emmc");
                        device_volumes[5].device = strdup("recovery");
                        device_volumes[5].device2 = NULL;
                        device_volumes[5].length = 0;

                        //sdcard
                        if (has_fat) {
                            device_volumes[6].mount_point = strdup("/storage/sdcard0");
                            device_volumes[6].fs_type = strdup("vfat");
#if defined(MTK_SHARED_SDCARD) || defined(MTK_2SDCARD_SWAP)
                            device_volumes[6].device = strdup("/dev/block/mmcblk1p1");
                            device_volumes[6].device2 = strdup(dev[FAT_INDEX]);
#else
                            device_volumes[6].device = strdup(dev[FAT_INDEX]);
                            device_volumes[6].device2 = strdup("/dev/block/mmcblk1p1");
#endif
                            device_volumes[6].length = 0;
                        } else {
                            device_volumes[6].mount_point = strdup("/storage/sdcard0");
                            device_volumes[6].fs_type = strdup("vfat");
                            device_volumes[6].device = strdup("/dev/block/mmcblk1p1");
                            device_volumes[6].device2 = strdup("/dev/block/mmcblk1");
                            device_volumes[6].length = 0;
                        }

                        //system
                        device_volumes[7].mount_point = strdup("/system");
                        device_volumes[7].fs_type = strdup("ext4");
                        device_volumes[7].device = strdup(dev[SYSTEM_INDEX]);
                        device_volumes[7].device2 = NULL;
                        device_volumes[7].length = 0;


#if defined(SUPPORT_SDCARD2) && !defined(MTK_SHARED_SDCARD) //wschen 2012-11-15
                        //sdcard2
                        if (has_fat) {
                            device_volumes[8].mount_point = strdup("/sdcard2");
                            device_volumes[8].fs_type = strdup("vfat");
#if defined(MTK_2SDCARD_SWAP)
                            device_volumes[8].device = strdup(dev[FAT_INDEX]);
                            device_volumes[8].device2 = NULL;
#else
                            device_volumes[8].device = strdup("/dev/block/mmcblk1p1");
                            device_volumes[8].device2 = strdup("/dev/block/mmcblk1");
#endif

                            device_volumes[8].length = 0;
                        } else {
                            //no 2nd SD
                            num_volumes--;
                        } // has_fat
#endif //SUPPORT_SDCARD2

                    } else {
                        //boot
                        device_volumes[1].mount_point = strdup("/boot");
                        device_volumes[1].fs_type = strdup("mtd");
                        device_volumes[1].device = strdup("boot");
                        device_volumes[1].device2 = NULL;
                        device_volumes[1].length = 0;

                        //cache
                        device_volumes[2].mount_point = strdup("/cache");
                        device_volumes[2].fs_type = strdup("yaffs2");
                        device_volumes[2].device = strdup("cache");
                        device_volumes[2].device2 = NULL;
                        device_volumes[2].length = 0;

                        //data
                        device_volumes[3].mount_point = strdup("/data");
                        device_volumes[3].fs_type = strdup("yaffs2");
                        device_volumes[3].device = strdup("userdata");
                        device_volumes[3].device2 = NULL;
                        device_volumes[3].length = 0;

                        //misc
                        device_volumes[4].mount_point = strdup("/misc");
                        device_volumes[4].fs_type = strdup("mtd");
                        device_volumes[4].device = strdup("misc");
                        device_volumes[4].device2 = NULL;
                        device_volumes[4].length = 0;

                        //recovery
                        device_volumes[5].mount_point = strdup("/recovery");
                        device_volumes[5].fs_type = strdup("mtd");
                        device_volumes[5].device = strdup("recovery");
                        device_volumes[5].device2 = NULL;
                        device_volumes[5].length = 0;

                        //sdcard
                        device_volumes[6].mount_point = strdup("/storage/sdcard0");
                        device_volumes[6].fs_type = strdup("vfat");
                        device_volumes[6].device = strdup("/dev/block/mmcblk0p1");
                        device_volumes[6].device2 = strdup("/dev/block/mmcblk0");
                        device_volumes[6].length = 0;

                        //system
                        device_volumes[7].mount_point = strdup("/system");
                        device_volumes[7].fs_type = strdup("yaffs2");
                        device_volumes[7].device = strdup("system");
                        device_volumes[7].device2 = NULL;
                        device_volumes[7].length = 0;
#if defined(SUPPORT_SDCARD2) && !defined(MTK_SHARED_SDCARD) //wschen 2012-11-15
                        //NAND no 2nd SD
                        num_volumes--;
#endif //SUPPORT_SDCARD2

                    }

                    printf("recovery filesystem table\n");
                    printf("=========================\n");
                    for (j = 0; j < num_volumes; ++j) {
                        Volume* v = &device_volumes[j];
                        printf("  %d %s %s %s %s %lld\n", j, v->mount_point, v->fs_type, v->device, v->device2, v->length);
                    }
                    printf("\n");
                    return;
                }
            } else {
                printf("fstab type setting is wrong\n");
            }
        }

    } else {
        printf("Fail to open /proc/dumchar_info\n");
    }
#endif

    FILE* fstab = fopen("/etc/recovery.fstab", "r");
    if (fstab == NULL) {
        LOGE("failed to open /etc/recovery.fstab (%s)\n", strerror(errno));
        return;
    }

    char buffer[1024];
    int i;
    while (fgets(buffer, sizeof(buffer)-1, fstab)) {
        for (i = 0; buffer[i] && isspace(buffer[i]); ++i);
        if (buffer[i] == '\0' || buffer[i] == '#') continue;

        char* original = strdup(buffer);

        char* mount_point = strtok(buffer+i, " \t\n");
        char* fs_type = strtok(NULL, " \t\n");
        char* device = strtok(NULL, " \t\n");
        // lines may optionally have a second device, to use if
        // mounting the first one fails.
        char* options = NULL;
        char* device2 = strtok(NULL, " \t\n");
        if (device2) {
            if (device2[0] == '/') {
                options = strtok(NULL, " \t\n");
            } else {
                options = device2;
                device2 = NULL;
            }
        }

        if (mount_point && fs_type && device) {
            while (num_volumes >= alloc) {
                alloc *= 2;
                device_volumes = (Volume*)realloc(device_volumes, alloc*sizeof(Volume));
            }
            device_volumes[num_volumes].mount_point = strdup(mount_point);
            device_volumes[num_volumes].fs_type = strdup(fs_type);

#if 0 //wschen 2012-05-15
            device_volumes[num_volumes].device = strdup(device);
            device_volumes[num_volumes].device2 =
                device2 ? strdup(device2) : NULL;
#else

            if (phone_type == EMMC_TYPE) {

                if (!strcmp(mount_point, "/cache") && strcmp(device, dev[CACHE_INDEX])) {
                    device_volumes[num_volumes].device = strdup(dev[CACHE_INDEX]);
                    device_volumes[num_volumes].device2 = device2 ? strdup(device2) : NULL;
                } else if (!strcmp(mount_point, "/data") && strcmp(device, dev[DATA_INDEX])) {
                    device_volumes[num_volumes].device = strdup(dev[DATA_INDEX]);
                    device_volumes[num_volumes].device2 = device2 ? strdup(device2) : NULL;
                } else if (!strcmp(mount_point, "/system") && strcmp(device, dev[SYSTEM_INDEX])) {
                    device_volumes[num_volumes].device = strdup(dev[SYSTEM_INDEX]);
                    device_volumes[num_volumes].device2 = device2 ? strdup(device2) : NULL;
#if defined(MTK_SHARED_SDCARD) || defined(MTK_2SDCARD_SWAP)

	#ifndef VENDOR_EDIT
	//GuoDong.Huang@Drv.OTA, 2012/10/11, Modify for /sdcard mount correctly when external TF card insert 
                } else if (!strcmp(mount_point, "/storage/sdcard0")) {
                    device_volumes[num_volumes].device = strdup("/dev/block/mmcblk1p1");
                    if (has_fat) {
                        device_volumes[num_volumes].device2 = strdup(dev[FAT_INDEX]);
                    } else {
                        device_volumes[num_volumes].device2 = strdup("/dev/block/mmcblk1");
                    }
	#else /* VENDOR_EDIT */
                } else if (!strcmp(mount_point, "/storage/sdcard0") && has_fat && strcmp(device, dev[FAT_INDEX])) {
                    device_volumes[num_volumes].device = strdup(dev[FAT_INDEX]);
                    device_volumes[num_volumes].device2 = device2 ? strdup(device2) : NULL;	
	#endif /* VENDOR_EDIT */

#else
                } else if (!strcmp(mount_point, "/storage/sdcard0") && has_fat && strcmp(device, dev[FAT_INDEX])) {
                    device_volumes[num_volumes].device = strdup(dev[FAT_INDEX]);
                    device_volumes[num_volumes].device2 = device2 ? strdup(device2) : NULL;
#endif
                } else {
                    device_volumes[num_volumes].device = strdup(device);
                    device_volumes[num_volumes].device2 = device2 ? strdup(device2) : NULL;
                }

            } else {
                device_volumes[num_volumes].device = strdup(device);
                device_volumes[num_volumes].device2 = device2 ? strdup(device2) : NULL;
            }
#endif

            device_volumes[num_volumes].length = 0;
            if (parse_options(options, device_volumes + num_volumes) != 0) {
                LOGE("skipping malformed recovery.fstab line: %s\n", original);
            } else {
                ++num_volumes;
            }
        } else {
            LOGE("skipping malformed recovery.fstab line: %s\n", original);
        }
        free(original);
    }

    fclose(fstab);

#if defined(SUPPORT_SDCARD2) && !defined(MTK_SHARED_SDCARD) //wschen 2012-11-15

    if ((phone_type == EMMC_TYPE) && has_fat) {
        int match = 0;
        for (i = 0; i < num_volumes; i++) {
            Volume* v = &device_volumes[i];
            if (strcmp(v->mount_point, "/sdcard2") == 0) {
                match = 1;
                break;
            }
        }

        if (match == 0) {
            while (num_volumes >= alloc) {
                alloc *= 2;
                device_volumes = (Volume*)realloc(device_volumes, alloc*sizeof(Volume));
            }

            device_volumes[num_volumes].mount_point = strdup("/sdcard2");
            device_volumes[num_volumes].fs_type = strdup("vfat");
#ifdef MTK_2SDCARD_SWAP
            device_volumes[num_volumes].device = strdup(dev[FAT_INDEX]);
            device_volumes[num_volumes].device2 = NULL;
#else
            device_volumes[num_volumes].device = strdup("/dev/block/mmcblk1p1");
            device_volumes[num_volumes].device2 = strdup("/dev/block/mmcblk1");
#endif

            device_volumes[num_volumes].length = 0;
            num_volumes++;
        }
    }

#endif //SUPPORT_SDCARD2

    printf("recovery filesystem table\n");
    printf("=========================\n");
    for (i = 0; i < num_volumes; ++i) {
        Volume* v = &device_volumes[i];
        printf("  %d %s %s %s %s %lld\n", i, v->mount_point, v->fs_type,
               v->device, v->device2, v->length);
    }
    printf("\n");
}

Volume* volume_for_path(const char* path) {
    int i;
    for (i = 0; i < num_volumes; ++i) {
        Volume* v = device_volumes+i;
        int len = strlen(v->mount_point);
        if (strncmp(path, v->mount_point, len) == 0 &&
            (path[len] == '\0' || path[len] == '/')) {
            return v;
        }
    }
    return NULL;
}

void load_volume_table2() {
}

int ensure_path_mounted(const char* path) {
    Volume* v = volume_for_path(path);
    if (v == NULL) {
        LOGE("unknown volume for path [%s]\n", path);
        return -1;
    }
    if (strcmp(v->fs_type, "ramdisk") == 0) {
        // the ramdisk is always mounted.
        return 0;
    }

    int result;
    result = scan_mounted_volumes();
    if (result < 0) {
        LOGE("failed to scan mounted volumes\n");
        return -1;
    }

    const MountedVolume* mv =
        find_mounted_volume_by_mount_point(v->mount_point);
    if (mv) {
        // volume is already mounted
        return 0;
    }

    mkdir(v->mount_point, 0755);  // in case it doesn't already exist

    if (strcmp(v->fs_type, "yaffs2") == 0) {
        // mount an MTD partition as a YAFFS2 filesystem.
        mtd_scan_partitions();
        const MtdPartition* partition;
        partition = mtd_find_partition_by_name(v->device);
        if (partition == NULL) {
            LOGE("failed to find \"%s\" partition to mount at \"%s\"\n",
                 v->device, v->mount_point);
            return -1;
        }
        return mtd_mount_partition(partition, v->mount_point, v->fs_type, 0);
    } else if (strcmp(v->fs_type, "ext4") == 0 ||
               strcmp(v->fs_type, "vfat") == 0) {
        result = mount(v->device, v->mount_point, v->fs_type,
                       MS_NOATIME | MS_NODEV | MS_NODIRATIME, "");
        if (result == 0) return 0;

        if (v->device2) {
#if 1 //wschen 2012-09-04
            //try mmcblk1 if mmcblk1p1 failed, then try internal FAT
            if (!strcmp(v->mount_point, "/storage/sdcard0") && !strcmp(v->device, "/dev/block/mmcblk1p1") && !strstr(v->device2, "/dev/block/mmcblk1")) {
                result = mount("/dev/block/mmcblk1", v->mount_point, v->fs_type, MS_NOATIME | MS_NODEV | MS_NODIRATIME, "");
                if (result == 0) {
                    return 0;
                }
            }
#endif
            LOGW("failed to mount %s (%s); trying %s\n",
                 v->device, strerror(errno), v->device2);
            result = mount(v->device2, v->mount_point, v->fs_type,
                           MS_NOATIME | MS_NODEV | MS_NODIRATIME, "");
            if (result == 0) return 0;
        }

        LOGE("failed to mount %s (%s)\n", v->mount_point, strerror(errno));
        return -1;
    }

    LOGE("unknown fs_type \"%s\" for %s\n", v->fs_type, v->mount_point);
    return -1;
}

int ensure_path_unmounted(const char* path) {
    Volume* v = volume_for_path(path);
    if (v == NULL) {
        LOGE("unknown volume for path [%s]\n", path);
        return -1;
    }
    if (strcmp(v->fs_type, "ramdisk") == 0) {
        // the ramdisk is always mounted; you can't unmount it.
        return -1;
    }

    int result;
    result = scan_mounted_volumes();
    if (result < 0) {
        LOGE("failed to scan mounted volumes\n");
        return -1;
    }

    const MountedVolume* mv =
        find_mounted_volume_by_mount_point(v->mount_point);
    if (mv == NULL) {
        // volume is already unmounted
        return 0;
    }

    return unmount_mounted_volume(mv);
}

int format_volume(const char* volume) {
    Volume* v = volume_for_path(volume);
    if (v == NULL) {
        LOGE("unknown volume \"%s\"\n", volume);
        return -1;
    }
    if (strcmp(v->fs_type, "ramdisk") == 0) {
        // you can't format the ramdisk.
        LOGE("can't format_volume \"%s\"", volume);
        return -1;
    }
    if (strcmp(v->mount_point, volume) != 0) {
        LOGE("can't give path \"%s\" to format_volume\n", volume);
        return -1;
    }

    if (ensure_path_unmounted(volume) != 0) {
        LOGE("format_volume failed to unmount \"%s\"\n", v->mount_point);
        return -1;
    }

    if (strcmp(v->fs_type, "yaffs2") == 0 || strcmp(v->fs_type, "mtd") == 0) {
        mtd_scan_partitions();
        const MtdPartition* partition = mtd_find_partition_by_name(v->device);
        if (partition == NULL) {
            LOGE("format_volume: no MTD partition \"%s\"\n", v->device);
            return -1;
        }

        MtdWriteContext *write = mtd_write_partition(partition);
        if (write == NULL) {
            LOGW("format_volume: can't open MTD \"%s\"\n", v->device);
            return -1;
        } else if (mtd_erase_blocks(write, -1) == (off_t) -1) {
            LOGW("format_volume: can't erase MTD \"%s\"\n", v->device);
            mtd_write_close(write);
            return -1;
        } else if (mtd_write_close(write)) {
            LOGW("format_volume: can't close MTD \"%s\"\n", v->device);
            return -1;
        }
        return 0;
    }

    if (strcmp(v->fs_type, "ext4") == 0) {
#if 1 //wschen 2012-02-14
        int fd;
        struct msdc_ioctl msdc_io;

        fd = open("/dev/misc-sd", O_RDONLY);
        if (fd < 0) {
            LOGE("open: /dev/misc-sd failed\n");
            return -1;
        }

        msdc_io.opcode = MSDC_ERASE_PARTITION;
        if (!strcmp(volume, "/cache")) {
            msdc_io.buffer = (unsigned int*) "cache";
            msdc_io.total_size = 6;
        } else if (!strcmp(volume, "/data")) {
            msdc_io.buffer = (unsigned int*) "usrdata";
            msdc_io.total_size = 8;
        }

        ioctl(fd, 0, &msdc_io);
        close(fd);
#endif

        int result = make_ext4fs(v->device, v->length, volume, sehandle);
        if (result != 0) {
            LOGE("format_volume: make_extf4fs failed on %s\n", v->device);
            return -1;
        }

        return 0;
    }

    LOGE("format_volume: fs_type \"%s\" unsupported\n", v->fs_type);
    return -1;
}
