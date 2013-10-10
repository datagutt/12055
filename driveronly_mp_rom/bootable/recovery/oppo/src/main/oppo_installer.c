/*
 * Copyright (C) 2011 Ahmad Amarullah ( http://www.micode.net )
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
 * Descriptions:
 * -------------
 * Installer Proccess
 *
 */

#include <sys/stat.h>
#include <time.h>
#include <errno.h>

#include "../oppo_inter.h"
#include "../oppo.h"
#include "../../../oppo_intent.h"

static float     ai_progress_pos     = 0;
static float     gProgressScopeStart = 0;
static float     gProgressScopeSize = 0;

static pthread_mutex_t ai_progress_mutex = PTHREAD_MUTEX_INITIALIZER;

static struct _oppoInstall oppo_install_struct ={
    .pfun = NULL,
    .path = NULL,
    .install_file = NULL,
    .wipe_cache = 0
};
static struct _oppoInstall* poppo_install = &oppo_install_struct;

extern void erase_modem_nv();
int oppo_install_package(){
    int ret = 0;
	
    oppo_debug("start .....\n");
    if (poppo_install->pfun != NULL)
    {
        oppo_debug("start install package\n");
        //run install process
        ret = poppo_install->pfun(poppo_install->path, &poppo_install->wipe_cache, &poppo_install->wipe_data, poppo_install->install_file);
        if (poppo_install->wipe_cache) {
            oppo_debug("wipe cache\n");
            oppoIntent_send(INTENT_WIPE, 1 , "/cache");
        }
/* OPPO 2013-01-25 jizhengkang azx Add begin for reason */
#ifdef ERASE_MODEM_NV_12025
		if (poppo_install->wipe_data) {
			oppo_debug("wipe data\n");
			oppoIntent_send(INTENT_WIPE, 1 , "/data");
			erase_modem_nv();
		}
#endif		
/* OPPO 2013-01-25 jizhengkang azx Add end */
	
        oppo_debug("end ret=%d\n", ret);
        return ret;
    }
    oppo_error("poppo_install->pfun is NULL, force return");
    return -1;
}

int oppo_get_progress() {
    int progress;
	
    pthread_mutex_lock(&ai_progress_mutex);
    if (ai_progress_pos>1.0) ai_progress_pos=1.0;
    if (ai_progress_pos<0.0) ai_progress_pos=0.0;
	
    progress = 	ai_progress_pos * 100;
    pthread_mutex_unlock(&ai_progress_mutex); 	
    return progress;	
}

STATUS oppoInstall_init(oppoInstall_fun fun, char* path, int wipe_cache, int wipe_data, const char* install_file)
{
    poppo_install->path = path;
    poppo_install->wipe_cache = wipe_cache;
    poppo_install->wipe_data = wipe_data;
    poppo_install->install_file = install_file;
    poppo_install->pfun = fun;
    return RET_OK;
}
void oppoInstall_show_progress(float portion, int seconds)
{
    oppo_debug("portion=%f, seconds=%d\n",portion, seconds);	
    pthread_mutex_lock(&ai_progress_mutex); 
    gProgressScopeStart += gProgressScopeSize;
    gProgressScopeSize = portion;
    pthread_mutex_unlock(&ai_progress_mutex); 
}

void oppoInstall_set_progress(float fraction)
{
    oppo_debug("fraction=%f\n",fraction);
    pthread_mutex_lock(&ai_progress_mutex);
    if (fraction < 0.0) fraction = 0.0;
    if (fraction > 1.0) fraction = 1.0;	
    ai_progress_pos     = gProgressScopeStart + fraction*gProgressScopeSize;
    pthread_mutex_unlock(&ai_progress_mutex);
}
void oppoInstall_reset_progress()
{
    pthread_mutex_lock(&ai_progress_mutex); 
    gProgressScopeStart = gProgressScopeSize = 0;	
    ai_progress_pos     = 0;
    pthread_mutex_unlock(&ai_progress_mutex);
}

void oppoInstall_set_text(char *str)
{
    oppo_debug("text(%s)\n",str);
}
//echo text with progress item
void oppoInstall_set_info(char* file_name)
{
}
