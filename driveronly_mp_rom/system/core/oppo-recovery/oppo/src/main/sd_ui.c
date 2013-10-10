#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "../oppo_inter.h"
#include "../oppo.h"
#include "../../../oppo_intent.h"
#include "../../../common.h"

/* OPPO 2013-01-17 jizhengkang azx Add begin for reason */
#include "../../../bootloader.h"
extern int selectedCount;
extern int selectedItem[1024];
/* OPPO 2013-01-17 jizhengkang Add end */
/*
 *_sd_show_dir show file system on screen
 *return MENU_BACK when pree backmenu
 */
#define SD_MAX_PATH 256
#define INTERNAL_SDCARD "/sdcard"
#define EXTERNAL_SDCARD "/external_sd"

int result_inter_sd, result_external_sd;
static int install_success = 0;
CANVAS canvas_mainmenu;

//callback function , success return 0, non-zero fail
int file_install(char *file_name, int file_len, void *data)
{
    int ret;
/* OPPO 2013-01-17 jizhengkang azx Add begin for reason */
	struct bootloader_message boot;
/* OPPO 2013-01-17 jizhengkang Add end */
	
    return_val_if_fail(file_name != NULL, RET_FAIL);
    return_val_if_fail(strlen(file_name) <= file_len, RET_INVALID_ARG);
    return_val_if_fail(data != NULL, RET_FAIL);
    struct _menuUnit *p = (pmenuUnit)data;
    oppo_debug("start .....\n");
    if (RET_YES == oppo_confirm(3, "<~sd.install.confirm.name>", "<~sd.install.confirm.desc>", p->icon)) {		
        oppoIntent_send(INTENT_INSTALL, 4, file_name, "0", "0", "1");
	 ret = oppoIntent_result_get_int();
	 if (ret == 0) {
            install_success = 1;
/* OPPO 2013-01-17 jizhengkang azx Add begin for reason */
    		memset(&boot, 0, sizeof(boot));
    		set_bootloader_message(&boot);
/* OPPO 2013-01-17 jizhengkang Add end */
	 }
	 oppo_debug("end..... ret=%d\n", ret);
        return ret;
    }
    else return 1;
}
//callback funtion file filter, if access ,return 0; others return -1
int file_filter(char *file, int file_len)
{
    return_val_if_fail(file != NULL, RET_FAIL);
    int len = strlen(file);
    return_val_if_fail(len <= file_len, RET_INVALID_ARG);
    if (len >= 4 && strncasecmp(file + len -4, ".zip", 4) == 0)
        return 0;
    return -1;
     
}

extern  CANVAS canvas_mainmenu;;
static STATUS sd_menu_show(menuUnit *p)
{
    int result;
	
    oppo_debug("start...\n");
    //如果安装成功，在主菜单背景下显示提示框	
    if (install_success) {
		//把保存的主菜单界面复制到ag_c	
		ag_draw(NULL,&canvas_mainmenu,0,0);
		//oppo_notice在当前ag_c的背景下显示提示框
//		oppo_notice(3, "<~sd.install.success.name>", "<~sd.install.success.desc>", p->icon);
		result = oppo_confirm(5, "<~sd.install.success.name>", "<~sd.install.success.desc>", p->icon, "<~sd.install.success.leftbutton>", "<~text_yes>");
		if (result == RET_YES) {
			oppoIntent_send(INTENT_REBOOT, 1, "reboot");
		}
	 
		oppo_debug("return MENU_BACK...\n");	
		install_success = 0;
	 //返回主菜单界面
        return MENU_BACK;
    }	
    result = common_menu_show(p);
    oppo_debug("end... result=%d\n",result);	
    return result;	
}

static STATUS external_sd_menu_show(menuUnit *p)
{
    int ret ;

    oppo_debug("start...\n");		
    ret = file_scan(EXTERNAL_SDCARD, sizeof(EXTERNAL_SDCARD), p->name, strlen(p->name), &file_install, (void *)p, &file_filter);
    oppo_debug("ret=%d\n", ret);	

    if (install_success) {
		if ((result_inter_sd != 0) || (result_external_sd != 0)) {
            //把保存的主菜单界面复制到ag_c	
			ag_draw(NULL,&canvas_mainmenu,0,0);
	     //oppo_notice在当前ag_c的背景下显示提示框
			ret = oppo_confirm(5, "<~sd.install.success.name>", "<~sd.install.success.desc>", p->icon, "<~sd.install.success.leftbutton>", "<~text_yes>");
			if (ret == RET_YES) {
				oppoIntent_send(INTENT_REBOOT, 1, "reboot");
			}
			install_success = 0;	 
		}
    }

    oppo_debug("end...\n");       
    return MENU_BACK;
}

static STATUS inter_sd_menu_show(menuUnit *p)
{
    int ret ;

    oppo_debug("start...\n");	
    ret = file_scan(INTERNAL_SDCARD, sizeof(INTERNAL_SDCARD), p->name, strlen(p->name), &file_install, (void *)p, &file_filter);
    oppo_debug("ret=%d\n", ret);
   
	if (install_success) {
		if ((result_inter_sd != 0) || (result_external_sd != 0)) {
            //把保存的主菜单界面复制到ag_c	
			ag_draw(NULL,&canvas_mainmenu,0,0);
			//oppo_notice在当前ag_c的背景下显示提示框
			ret = oppo_confirm(5, "<~sd.install.success.name>", "<~sd.install.success.desc>", p->icon, "<~sd.install.success.leftbutton>", "<~text_yes>");
			if (ret == RET_YES) {
				oppoIntent_send(INTENT_REBOOT, 1, "reboot");
			}
			install_success = 0;	 
		}
    }
    oppo_debug("end...\n");   
    return MENU_BACK;
}

struct _menuUnit * sd_ui_init()
{
    struct _menuUnit *p;
    struct _menuUnit  *temp;

    ag_canvas(&canvas_mainmenu,agw(),agh());
		
    oppoIntent_send(INTENT_MOUNT, 1, INTERNAL_SDCARD);
    result_inter_sd = oppoIntent_result_get_int();
    oppoIntent_send(INTENT_MOUNT, 1, EXTERNAL_SDCARD);
    result_external_sd = oppoIntent_result_get_int();
    oppo_debug("result_inter_sd=%d, result_external_sd=%d\n", result_inter_sd, result_external_sd);

    p = common_ui_init();
    return_null_if_fail(p != NULL);
    strncpy(p->name, "<~sd.name>", MENU_LEN);
    strncpy(p->title_name, "<~sd.title_name>", MENU_LEN);
    strncpy(p->icon, "@sd",  MENU_LEN);
    p->show = &sd_menu_show;
    p->result = 0;
    return_null_if_fail(menuNode_init(p) != NULL);    
	
    if ((result_inter_sd == 0) && (result_external_sd == 0)) {
        	
        temp = common_ui_init();
        return_null_if_fail(temp != NULL);
        menuUnit_set_icon(temp, "@sd.choose");
        strncpy(temp->name, "<~sd.internal.name>", MENU_LEN);
        temp->show = &inter_sd_menu_show;
        assert_if_fail(menuNode_add(p, temp) == RET_OK);	

		temp = common_ui_init();
        return_null_if_fail(temp != NULL);
        menuUnit_set_icon(temp, "@sd.choose");
        strncpy(temp->name, "<~sd.external.name>", MENU_LEN);
        temp->show = &external_sd_menu_show;
        assert_if_fail(menuNode_add(p, temp) == RET_OK);			
    } else if (result_inter_sd == 0) {
		p->show = &inter_sd_menu_show;
    } else if (result_external_sd == 0) {
        p->show = &external_sd_menu_show;
    }
    	
    return p;
}

#define INTER_SDPATH_ANDROID  "/storage/sdcard0"
#define EXTER_SDPATH_ANDROID  "/storage/sdcard0/external_sd"
extern int auto_install;
int sd_path_verify(char* sdpath)
{
    char newpath[100];
    int path_modify = 0;	

    oppo_debug("sdpath(%s) auto_install=%d\n", sdpath, auto_install);
#if 0
    //if there are both internal and external sd, it's dynamic mount	
    if ((result_inter_sd == 0) && (result_external_sd == 0)) {
        if (!strncmp(sdpath, EXTER_SDPATH_ANDROID, strlen(EXTER_SDPATH_ANDROID))) {
            strlcpy(newpath, INTERNAL_SDCARD, strlen(INTERNAL_SDCARD)+1);			     		
	     strlcat(newpath, &sdpath[strlen(EXTER_SDPATH_ANDROID)], sizeof(newpath));	 
            path_modify = 1;		 
        } else if(!strncmp(sdpath, INTER_SDPATH_ANDROID, strlen(INTER_SDPATH_ANDROID))) {
            strlcpy(newpath, EXTERNAL_SDCARD, strlen(EXTERNAL_SDCARD)+1);		
	     strlcat(newpath, &sdpath[strlen(INTER_SDPATH_ANDROID)], sizeof(newpath));
            path_modify = 1;		 
        }
    } else {
        if(!strncmp(sdpath, INTER_SDPATH_ANDROID, strlen(INTER_SDPATH_ANDROID))) {
            strlcpy(newpath, INTERNAL_SDCARD, strlen(INTERNAL_SDCARD)+1);		
	     strlcat(newpath, &sdpath[strlen(INTER_SDPATH_ANDROID)], sizeof(newpath));
            path_modify = 1;		
        }
    }
#else
        //for ota update
        if(!strncmp(sdpath, EXTER_SDPATH_ANDROID, strlen(EXTER_SDPATH_ANDROID))) {
            strlcpy(newpath, EXTERNAL_SDCARD, strlen(EXTERNAL_SDCARD)+1);		
	     strlcat(newpath, &sdpath[strlen(EXTER_SDPATH_ANDROID)], sizeof(newpath));
            path_modify = 1;		
        } else  if(!strncmp(sdpath, INTER_SDPATH_ANDROID, strlen(INTER_SDPATH_ANDROID))) {
            strlcpy(newpath, INTERNAL_SDCARD, strlen(INTERNAL_SDCARD)+1);				
	     strlcat(newpath, &sdpath[strlen(INTER_SDPATH_ANDROID)], sizeof(newpath));
            path_modify = 1;		
        }
		
        //for one-key flash tool
        if (auto_install && !strncmp(sdpath, INTERNAL_SDCARD, strlen(INTERNAL_SDCARD))) {
		if ((result_inter_sd == 0) && (result_external_sd == 0)) {
		    strlcpy(newpath, EXTERNAL_SDCARD, strlen(EXTERNAL_SDCARD)+1);		
	           strlcat(newpath, &sdpath[strlen(INTERNAL_SDCARD)], sizeof(newpath));	
		    path_modify = 1;
             }	
	 }
#endif
    if (path_modify)		
        strlcpy(sdpath, newpath, strlen(newpath)+1);

    oppo_debug("end...  sdpath(%s)\n", sdpath);

    return 0;	
}

#define MASS_STORAGE_EXTERNAL_FILE_PATH  "/sys/class/android_usb/android0/f_mass_storage/lun1/file"
#define MASS_STORAGE_FILE_PATH  "/sys/class/android_usb/android0/f_mass_storage/lun/file"

int write_char(char *file, char *value)
{
	int fd;
	if ((fd = open(file, O_WRONLY)) < 0) {
		oppo_printf("open %s failed\n", file);
		return -1;
	}

    if (write(fd, value, strlen(value)) < 0) {
		oppo_printf("write %s failed(%d)\n", file, -errno);
        close(fd);
        return -1;
    }
	close(fd);
	return 0;
}

extern Volume* volume_for_path(const char* path);
extern int ensure_path_unmounted(const char* path) ;
static void ums_enable()
{
	Volume* v;

	write_char("/sys/class/android_usb/android0/enable", "0");
	write_char("/sys/class/android_usb/android0/idVendor", "22d9");
	write_char("/sys/class/android_usb/android0/idProduct", "2768");
	write_char("/sys/class/android_usb/android0/functions", "mass_storage");
	
	if (result_inter_sd == 0)  {
		v = volume_for_path(INTERNAL_SDCARD); 
		oppo_debug("inter device(%s), device2(%s)\n", v->device, v->device2);
		if ((v->device) && (access(v->device, F_OK) == 0)) {
			ensure_path_unmounted(INTERNAL_SDCARD);
			write_char(MASS_STORAGE_FILE_PATH, v->device);
		} else if ((v->device2) && (access(v->device2, F_OK) == 0)) {
			ensure_path_unmounted(INTERNAL_SDCARD);
			write_char(MASS_STORAGE_FILE_PATH, v->device2);
		}
		
	}

	if ( result_external_sd == 0) {
		v = volume_for_path(EXTERNAL_SDCARD); 
		oppo_debug("external device(%s), device2(%s)\n", v->device, v->device2);
		if ((v->device) && (access(v->device, F_OK) == 0)) {
			ensure_path_unmounted(EXTERNAL_SDCARD);
			write_char(MASS_STORAGE_EXTERNAL_FILE_PATH, v->device);
		} else if ((v->device2) && (access(v->device2, F_OK) == 0)) {
			ensure_path_unmounted(EXTERNAL_SDCARD);
			write_char(MASS_STORAGE_EXTERNAL_FILE_PATH, v->device2);
		}
		
	}
	
	write_char("/sys/class/android_usb/android0/enable", "1");

}

static void adb_enable()
{
    	write_char("/sys/class/android_usb/android0/enable", "0");
	write_char("/sys/class/android_usb/android0/idVendor", "22d9");
	write_char("/sys/class/android_usb/android0/idProduct", "2769");
	write_char("/sys/class/android_usb/android0/functions", "adb");
	write_char("/sys/class/android_usb/android0/enable", "1");
	
	if (result_inter_sd == 0)  {
		oppoIntent_send(INTENT_MOUNT, 1, INTERNAL_SDCARD);
	}

	if ( result_external_sd == 0) {
		oppoIntent_send(INTENT_MOUNT, 1, EXTERNAL_SDCARD);
	}
}

static STATUS ums_menu_show(menuUnit *p)
{   
    oppo_debug("start...\n");
    ums_enable();	
    
    oppo_ums_notic(p);
	
    adb_enable();	
    oppo_debug("end...\n");
/* OPPO 2013-02-18 jizhengkang azx Add begin for reason */
	selectedCount--;
/* OPPO 2013-02-18 jizhengkang azx Add end */
    return MENU_BACK;
}

struct _menuUnit * ums_ui_init()
{
    struct _menuUnit *p;
  
    p = common_ui_init();
    return_null_if_fail(p != NULL);
    strncpy(p->name, "<~ums.name>", MENU_LEN);
    strncpy(p->title_name, "<~ums.title_name>", MENU_LEN);
    strncpy(p->icon, "@ums",  MENU_LEN);
    p->show = &ums_menu_show;
    p->result = 0;
    return_null_if_fail(menuNode_init(p) != NULL);   
    return p;	
}

