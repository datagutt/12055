/*
 * Copyright (C) 2011 Ahmad Amarullah ( http://amarullz.com/ )
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

/*
 *Author:dennise 
 *Date:2012-10-12
 *Descriptions;
 *    scan file system and show in screen, invoke callback function when touch file
 *  
 *
 *
 */
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include "../oppo_inter.h"
#include "../oppo.h"

//invoke fileFun after touch the file.
//if return value from callback funciton is not equal 0, continue, others , back to up layer
//just support single thread
static char *g_title_name = NULL;
//callback function 
static fileFun g_fun = NULL;
static fileFilterFun g_file_filter_fun= NULL;
static void *g_data = NULL;
pthread_mutex_t g_file_scan_mutex = PTHREAD_MUTEX_INITIALIZER;

extern int selectedCount;
extern int selectedItem[1024];
extern int result_inter_sd, result_external_sd;
extern void feed_back(const char *send_intent);

static void sd_file_dump_array(char **zips, char **zips_desc, int z_size)
{
    oppo_debug("*************%s start******************\n", __FUNCTION__);
    int i = 0;
    for (i = 0; i < z_size; i++)
    {
        oppo_debug("[%d]%s--%s\n", i, zips[i], zips_desc[i]);
    }
    oppo_debug("*************%s end********************\n", __FUNCTION__);
}

static  CANVAS canvas_sdfile;
static int canvas_sdfile_inited = 0;
extern int oppo_is_ac_usb_online();
extern int oppo_get_battery_capacity();

static STATUS _file_scan(char *path, int path_len)
{
    return_val_if_fail(path != NULL, RET_FAIL);
    return_val_if_fail(strlen(path) <= path_len, RET_INVALID_ARG);
    DIR* d = NULL;
    struct dirent* de = NULL;
    int i = 0;
    int result = 0;
    d = opendir(path);
    return_val_if_fail(d != NULL, RET_FAIL);

    int d_size = 0;
    int d_alloc = 10;
    char** dirs = (char **)malloc(d_alloc * sizeof(char*));
    char** dirs_desc = (char **)malloc(d_alloc * sizeof(char*));
    return_val_if_fail(dirs != NULL, RET_FAIL);
    return_val_if_fail(dirs_desc != NULL, RET_FAIL);
    int z_size = 0;
    int z_alloc = 10;
    char** zips = (char **)malloc(z_alloc * sizeof(char*));
    char** zips_desc=(char **)malloc(z_alloc * sizeof(char*));
    return_val_if_fail(zips != NULL, RET_FAIL);
    return_val_if_fail(zips_desc != NULL, RET_FAIL);
//    zips[0] = strdup("../");
//    zips_desc[0]=strdup("../");

    while ((de = readdir(d)) != NULL) {
        int name_len = strlen(de->d_name);
        if (name_len <= 0) continue;
        char de_path[PATH_MAX];
        snprintf(de_path, PATH_MAX, "%s/%s", path, de->d_name);
        struct stat st ;
        assert_if_fail(stat(de_path, &st) == 0);
        if (de->d_type == DT_DIR) {
            //skip "." and ".." entries
            if (name_len == 1 && de->d_name[0] == '.') continue;
            if (name_len == 2 && de->d_name[0] == '.' && 
                    de->d_name[1] == '.') continue;
            if (d_size >= d_alloc) {
                d_alloc *= 2;
                dirs = (char **)realloc(dirs, d_alloc * sizeof(char*));
                assert_if_fail(dirs != NULL);
                dirs_desc = (char **)realloc(dirs_desc, d_alloc * sizeof(char*));
                assert_if_fail(dirs_desc != NULL);
            }
            dirs[d_size] = (char *)malloc(name_len + 2);
            assert_if_fail(dirs[d_size] != NULL);
            dirs_desc[d_size] = (char *)malloc(64);
            assert_if_fail(dirs_desc[d_size] != NULL);
            strncpy(dirs[d_size], de->d_name, name_len);
            dirs[d_size][name_len] = '/';
            dirs[d_size][name_len + 1] = '\0';
            snprintf(dirs_desc[d_size], 63, "%s" ,ctime(&st.st_mtime));
            dirs_desc[d_size][63] = '\0';
            ++d_size;
        } else if (de->d_type == DT_REG) {
            if (g_file_filter_fun == NULL || g_file_filter_fun(de->d_name, name_len) == 0)
            {
                if (z_size >= z_alloc) {
                    z_alloc *= 2;
                    zips = (char **) realloc(zips, z_alloc * sizeof(char*));
                    assert_if_fail(zips != NULL);
                    zips_desc = (char **) realloc(zips_desc, z_alloc * sizeof(char*));
                    assert_if_fail(zips_desc != NULL);
                }
                zips[z_size] = strdup(de->d_name);
                assert_if_fail(zips[z_size] != NULL);
                zips_desc[z_size] = (char*)malloc(64);
                assert_if_fail(zips_desc[z_size] != NULL);
                snprintf(zips_desc[z_size], 63, "%s   %lldbytes" ,ctime(&st.st_mtime), st.st_size);
                zips_desc[z_size][63] = '\0';
                z_size++;
            }
        }
    }
    closedir(d);


    // append dirs to the zips list
    if (d_size + z_size + 1 > z_alloc) {
        z_alloc = d_size + z_size + 1;
        zips = (char **)realloc(zips, z_alloc * sizeof(char*));
        assert_if_fail(zips != NULL);
        zips_desc = (char **)realloc(zips_desc, z_alloc * sizeof(char*));
        assert_if_fail(zips_desc != NULL);
    }
    for (i = 0; i < d_size; i++)
    {
        zips[z_size + i] = dirs[i];
        zips_desc[z_size + i] = dirs_desc[i];
    }
    free(dirs);
    z_size += d_size;
    zips[z_size] = NULL;
    zips_desc[z_size] = NULL;

	int chosen_item = 0;
	do {
      
		if (NULL == g_title_name) 
		{
			oppo_error("g_title_name is NULL \n");
			result = -1;
			goto finish_done;
		}
#if DEBUG
		sd_file_dump_array(zips, zips_desc, z_size);
#endif
		chosen_item = oppo_sdmenu(g_title_name, zips, zips_desc, z_size);

/* OPPO 2013-02-18 jizhengkang azx Add begin for reason */
		selectedItem[selectedCount] = chosen_item;

/* OPPO 2013-02-23 jizhengkang azx Modify begin for reason */
#if 0
		if (chosen_item == -1) {//huanggd for exit explorer when select "return" option
			result = 1;
			if (result_inter_sd || result_external_sd) {
				selectedCount = 1;
				memset(selectedItem + 2, 0, 1022);
			} else {
				selectedCount = 2;
				memset(selectedItem + 3, 0, 1021);
			}
			break;
		}
#endif
/* OPPO 2013-02-23 jizhengkang azx Modify end */

		if (chosen_item == -1) {
			result = -1;
			memset(selectedItem + selectedCount, 0, 1024 - selectedCount - 1);
			selectedCount--;
			break;
		}
		
		return_val_if_fail(chosen_item >= 0, RET_FAIL);
		char * item = zips[chosen_item];
		return_val_if_fail(item != NULL, RET_FAIL);
		int item_len = strlen(item);

//		((chosen_item > 0)&&(item[item_len - 1] == '/'))?selectedCount++:selectedCount--;
//		(chosen_item == 0)?selectedCount--:((item[item_len - 1] == '/')?selectedCount++:NULL);


/* OPPO 2013-02-18 jizhengkang azx Add end */




/* OPPO 2013-02-23 jizhengkang azx Delete begin for reason */
#if 0
		if ( chosen_item == 0) {
           //go up but continue browsing
			result = -1;
			break;
		} else 
#endif
/* OPPO 2013-02-23 jizhengkang azx Delete end */
		if (item[item_len - 1] == '/') {
			selectedCount++;
			char new_path[PATH_MAX];
			strlcpy(new_path, path, PATH_MAX);
			strlcat(new_path, "/", PATH_MAX);
			strlcat(new_path, item, PATH_MAX);
			new_path[strlen(new_path) - 1] = '\0';
			result = _file_scan(new_path, PATH_MAX);
			if (result > 0) break;
		} else {
           // select a zipfile
           // the status to the caller
			char new_path[PATH_MAX];
			strlcpy(new_path, path, PATH_MAX);
			strlcat(new_path, "/", PATH_MAX);
			strlcat(new_path, item, PATH_MAX);
			int wipe_cache = 0;
			//if third parameter is 1, echo sucess dialog
			if (NULL == g_fun) 
			{
				oppo_error("g_fun is NULL in fun\n");
				result = -1;
				goto finish_done;
			}

			if (oppo_get_battery_capacity() < 15) {
				if (!oppo_is_ac_usb_online()) {
					oppo_notice(3, "<~sd.install.notice>", "<~sd.install.caplow.desc>", "@sd");
					continue;			
				}
	    	}

		    if (canvas_sdfile_inited == 0) {
				canvas_sdfile_inited = 1;
				ag_canvas(&canvas_sdfile,agw(),agh());	
		    }
		    ag_draw(&canvas_sdfile,agc(),0,0);  

		    int ret = g_fun(new_path, PATH_MAX, (void *)g_data);
			if (0 == ret)//execute callback fun success
			{
				//back to up layer
				//result = -1;//huanggd for exit explorer when install successfully
				selectedCount = 1;
				memset(selectedItem + 2, 0, 1022);
				result = 1;
				feed_back("2");
			}
			else if(1 == ret){
				continue;//cacel install
			} else {
				oppo_error("g_fun execute fail\n");
				result = 0;
				feed_back("3");

				ag_draw(NULL,&canvas_sdfile,0,0);
				oppo_notice(3, "<~sd.install.failed.name>", "<~sd.install.failed.desc>", "@sd");
				continue;   
			}
			break;
		}
	} while(1);

finish_done:

	for (i = 0; i < z_size; ++i)
	{
		free(zips[i]);
		free(zips_desc[i]);
	}
	free(zips);
	return result;
}

//add data obejcet, optional argument in fileFun
STATUS file_scan(char *path, int path_len, char * title, int title_len, fileFun fun, void* data, fileFilterFun filter_fun)
{
    return_val_if_fail(path != NULL, RET_FAIL);
    return_val_if_fail(strlen(path) <= path_len, RET_INVALID_ARG);
    return_val_if_fail(title != NULL, RET_FAIL);
    return_val_if_fail(strlen(title) <= title_len, RET_INVALID_ARG);
    pthread_mutex_lock(&g_file_scan_mutex);
    g_title_name = title;
    g_fun = fun;
    g_data = data;
    g_file_filter_fun = filter_fun;
    pthread_mutex_unlock(&g_file_scan_mutex);
    return _file_scan(path, path_len);
}
