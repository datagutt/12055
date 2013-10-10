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
 * Descriptions:
 * -------------
 * Source code for parsing and processing edify script (oppo-config)
 *
 */

#include <sys/stat.h>       //-- Filesystem Stats
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include "../edify/expr.h"

#include "../oppo_inter.h"
#include "../oppo.h"

#define APARSE_MAXHISTORY 256

//* 
//* GLOBAL UI VARIABLES
//* 
static  byte        oppo_isbgredraw   = 0;
static  int         statusbar_height                = 0;
static  CANVAS  oppo_bg;                 //-- Saved CANVAS for background
static  CANVAS  oppo_win_bg;             //-- Current drawed CANVAS for windows background

static CANVAS cOppoIntall;
static CANVAS cOppoWipe;

int selectedCount = 0;
int selectedItem[1024]={0};

//* 
//* MACROS
//* 
#define MAX_FILE_GETPROP_SIZE    65536

#define _INITARGS() \
    int args_i =0; \
    va_list arg_ptr; \
    char **args = (char**)malloc(argc * sizeof(char *)); \
    va_start(arg_ptr, format); \
    args[0] = format; \
    for (args_i = 1; args_i < argc; args_i++) \
        args[args_i] = va_arg(arg_ptr, char*);

#define _FREEARGS() \
    va_end(arg_ptr); \
    free(args);

static pthread_mutex_t title_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t redraw_mutex = PTHREAD_MUTEX_INITIALIZER;
STATUS oppo_set_isbgredraw(int value)
{
    pthread_mutex_lock(&redraw_mutex);
    oppo_isbgredraw = value;
    pthread_mutex_unlock(&redraw_mutex);
    return RET_OK;
}

#define BATTERY_CAPACITY_PATH "/sys/class/power_supply/battery/capacity"
#define BATTERY_CAPACITY_PATH_1 "/sys/class/power_supply/Battery/capacity"
#define USB_ONLINE_PATH "/sys/class/power_supply/usb/online"
#define AC_ONLINE_PATH "/sys/class/power_supply/ac/online"
#define AC_ONLINE_PATH_QCOM8960 "/sys/class/power_supply/pm8921-dc/online"

static int read_from_file(const char* path, char* buf, size_t size) {
    int fd = open(path, O_RDONLY, 0);
    if (fd == -1) {
        LOGE("Could not open '%s'", path);
        return -1;
    }

    size_t count = read(fd, buf, size);
    if (count > 0) {
        count = (count < size) ? count : size - 1;
        while (count > 0 && buf[count-1] == '\n') count--;
        buf[count] = '\0';
    } else {
        buf[0] = '\0';
    }

    close(fd);
    return count;
}

static int read_int(const char* path) {
    const int SIZE = 128;
    char buf[SIZE];
    int value = 0;

    if (read_from_file(path, buf, SIZE) > 0) {
        value = atoi(buf);
    }
    return value;
}

int oppo_get_battery_capacity()
{
    struct stat st;
    int capacity = 50;
	
    if (stat(BATTERY_CAPACITY_PATH, &st) >= 0)
       capacity = read_int(BATTERY_CAPACITY_PATH);
    else if (stat(BATTERY_CAPACITY_PATH_1, &st) >= 0)
        capacity = read_int(BATTERY_CAPACITY_PATH_1);
    else {
        oppo_error("BATTERY_CAPACITY_PATH error\n"); 
    }
    //oppo_debug("capacity=%d\n", capacity);
    return capacity;
}

int oppo_is_ac_usb_online()
{
    struct stat st;
    int online = 0;
	
    if (stat(USB_ONLINE_PATH, &st) >= 0)
	online |= read_int(USB_ONLINE_PATH);
	
    if (stat(AC_ONLINE_PATH, &st) >= 0)
	online |= read_int(AC_ONLINE_PATH);	
    else if (stat(AC_ONLINE_PATH_QCOM8960, &st) >= 0)
	online |= read_int(AC_ONLINE_PATH_QCOM8960);		
    //oppo_debug("online=%d\n", online);
    return online;	
}

int cnt_capacity_chg = 0;
#define HEIGHT_STATUSBAR       (agdp()*16)
#define HEIGHT_IMG_BATTERY   (agdp()*8)

void oppo_set_title(CANVAS *cdst) {
  static char bg_title[64];
  static time_t timep;
  static struct tm *p;
  time(&timep);
  p = gmtime(&timep);

  if (statusbar_height == 0)
    statusbar_height  = HEIGHT_STATUSBAR;

  //draw background
  //ag_roundgrad_ex(cdst,0,0,agw(),statusbar_height,acfg()->titlebg,acfg()->titlebg_g,(acfg()->winroundsz*agdp())-2,1,1,0,0);
  ag_rect(cdst,0,0,agw(),statusbar_height,acfg()->statusbg);
  //draw black line on the bottom of titlebar
  ag_rect(cdst,0,statusbar_height-1,agw(),1,acfg()->border);
  
  //draw battery
  char batter_image[50];
  int capacity_disp;
  int capacity = oppo_get_battery_capacity();
  int online = oppo_is_ac_usb_online();

  if (online) {
    if (cnt_capacity_chg < 0) 
      cnt_capacity_chg = 0;
	
    capacity_disp = capacity + 4*cnt_capacity_chg;
    if (capacity_disp>100) {
       capacity_disp = 100;
	cnt_capacity_chg = -1;
    }	
    snprintf(batter_image,49,"@stat_sys_battery_charge_anim%02d", capacity_disp/4);
  } else {
    cnt_capacity_chg = 0;
    snprintf(batter_image,49,"@stat_sys_battery_%d", capacity/4);
  }
  //oppo_debug("batter_image(%s)\n", batter_image); 

  int imgX = agw();
  PNGCANVAS image;
  if (!apng_load(&image, batter_image)) {
      oppo_error("load %s failed\n", batter_image);
  } else {
      int imgH = HEIGHT_IMG_BATTERY;
      int imgW=imgH*image.w/image.h;
      int imgY = round((statusbar_height-imgH)/2);
      imgX = agw() - imgW - 3*agdp();
      apng_stretch(cdst, &image, imgX, imgY, imgW, imgH, 0, 0, image.w, image.h);
      apng_close(&image);
  }
    
  //draw time
  snprintf(bg_title, 64, "%02d:%02d  %d%%", (p->tm_hour + 8) % 24, p->tm_min, capacity);
  int titW = ag_txtwidth(bg_title, 1);
  int timeX = imgX - titW - 3*agdp();
  int timeH = ag_fontheight(1);
  int timeY = round((statusbar_height-timeH)/2);
  ag_text(cdst,titW,timeX,timeY,bg_title,acfg()->txt_status,1);

}

static int _oppo_setbg_title(CANVAS *win, CANVAS *bg) {
   
  if (statusbar_height == 0)
    statusbar_height = HEIGHT_STATUSBAR;
  return statusbar_height;
}

int oppo_setbg_title() {
    return _oppo_setbg_title(&oppo_win_bg, &oppo_bg);
}
int oppo_setbg_title_win(AWINDOWP win){
    return _oppo_setbg_title(&win->c, win->bg);
}

//* 
//* Read Strings From filesystem
//* 
char * oppo_readfromfs(char * name){
  char* buffer = NULL;
  struct stat st;
  if (stat(name,&st) < 0) return NULL;
  if (st.st_size>MAX_FILE_GETPROP_SIZE) return NULL;
  buffer = malloc(st.st_size+1);
  if (buffer == NULL) goto done;
  FILE* f = fopen(name, "rb");
  if (f == NULL) goto done;
  if (fread(buffer, 1, st.st_size, f) != st.st_size){
      fclose(f);
      goto done;
  }
  buffer[st.st_size] = '\0';
  fclose(f);
  return buffer;
done:
  free(buffer);
  return NULL;
}

//* 
//* Write Strings into file
//* 
void oppo_writetofs(char * name, char * value){
  FILE * fp = fopen(name,"wb");
  if (fp!=NULL){
    fwrite(value,1,strlen(value),fp);
    fclose(fp);
  }
}

//* 
//* Read Strings From Temporary File
//*
char * oppo_readfromtmp(char * name){
  char path[256];
  snprintf(path,256,"%s/%s",OPPO_TMP,name);
  return oppo_readfromfs(path);
  
}

//* 
//* Write Strings From Temporary File
//*
void oppo_writetotmp(char * name, char * value){
  char path[256];
  snprintf(path,256,"%s/%s",OPPO_TMP,name);
  oppo_writetofs(path,value);
}

//* 
//* Read Strings From ZIP
//* 
char * oppo_readfromzip(char * name){
  AZMEM filedata;
  if (!az_readmem(&filedata,name,0)) return NULL;
  return (char *)filedata.data;
}

//* 
//* Parse PROP String
//* 
static char * oppo_parsepropstring(char * bf,char *key){
  char* result = NULL;  
  if (bf==NULL) return result;
  char* buffer=strdup(bf);
  char* line = strtok(buffer, "\n");
  do {
      while (*line && isspace(*line)) ++line;
      if (*line == '\0' || *line == '#') continue;
      char* equal = strchr(line, '=');
      if (equal == NULL) goto done;

      char* key_end = equal-1;
      while (key_end > line && isspace(*key_end)) --key_end;
      key_end[1] = '\0';

      if (strcmp(key, line) != 0) continue;

      char* val_start = equal+1;
      while(*val_start && isspace(*val_start)) ++val_start;

      char* val_end = val_start + strlen(val_start)-1;
      while (val_end > val_start && isspace(*val_end)) --val_end;
      val_end[1] = '\0';

      result = strdup(val_start);
      break;
  } while ((line = strtok(NULL, "\n")));
  free(buffer);
done:
  
  return result;
}

//* 
//* Parse PROP Files
//* 
char * oppo_parseprop(char * filename,char *key){
  char * buffer = oppo_readfromfs(filename);
  char * result = oppo_parsepropstring(buffer,key);
  free(buffer);
  return result;
}

//* 
//* Parse PROP from ZIP
//* 
char * oppo_parsepropzip(char * filename,char *key){
  char * buffer = oppo_readfromzip(filename);
  char * result = oppo_parsepropstring(buffer,key);
  free(buffer);
  return result;
}

//* 
//* Read Variable
//* 
char * oppo_getvar(char * name){
  char path[256];
  snprintf(path,256,"%s/.__%s.var",OPPO_TMP,name);
  return oppo_readfromfs(path);
}

//* 
//* Set Variable
//* 
void oppo_setvar(char * name, char * value){
  char path[256];
  snprintf(path,256,"%s/.__%s.var",OPPO_TMP,name);
  oppo_writetofs(path,value);
}

//* 
//* Append Variable
//* 
void oppo_appendvar(char * name, char * value){
  char path[256];
  snprintf(path,256,"%s/.__%s.var",OPPO_TMP,name);
  FILE * fp = fopen(path,"ab");
  if (fp!=NULL){
    fwrite(value,1,strlen(value),fp);
    fclose(fp);
  }
}

//* 
//* Delete Variable
//* 
void oppo_delvar(char * name){
  char path[256];
  snprintf(path,256,"%s/.__%s.var",OPPO_TMP,name);
  unlink(path);
}

//* 
//* Prepend Variable
//* 
void oppo_prependvar(char * name, char * value){
  char path[256];
  snprintf(path,256,"%s/.__%s.var",OPPO_TMP,name);
  char * buf = oppo_getvar(name);
  FILE * fp = fopen(path,"wb");
  if (fp!=NULL){
    fwrite(value,1,strlen(value),fp);
    if (buf!=NULL){
      fwrite(buf,1,strlen(buf),fp);
    }
    fclose(fp);
  }
  if (buf!=NULL){
    free(buf);
  }
}

//* 
//* Set Colorset From Prop String
//* 
void oppo_setthemecolor(char * prop, char * key, color * cl){
  char * val = oppo_parsepropstring(prop,key);
  if (val!=NULL){
    cl[0] = strtocolor(val);
    oppo_debug("key(%s), cl[0]=0x%x, val(%s)\n", key, cl[0], val);
    free(val);
  }
}
//* 
//* Set Drawing Config From Prop String
//* 
void oppo_setthemeconfig(char * prop, char * key, byte * b){
  char * val = oppo_parsepropstring(prop,key);
  if (val!=NULL){
    b[0] = (byte) min(atoi(val),255);
    free(val);
  }
}

//* 
//* loadtruefont
//*
extern  pthread_mutex_t refresh_mutex;
STATUS oppo_font(char *ttf_type, char *ttf_file, char *ttf_size){
  
  //-- This is Busy Function
  return_val_if_fail(ttf_type != NULL, RET_FAIL);
  return_val_if_fail(ttf_file != NULL, RET_FAIL);
  return_val_if_fail(ttf_size != NULL, RET_FAIL);

/* HuangGuoDong@Drv.recovery, 2013/1/25, add for mutex oppo_font() and oppo_set_title(), otherwise status bar may fliker*/    
  pthread_mutex_lock(&refresh_mutex);

  ag_setbusy();
  
  //-- Get Arguments
  
  char zpath[256];
  snprintf(zpath,256,"%s/",OPPO_DIR);
  
  int size = atoi(ttf_size);
  if (ttf_type[0]=='0'){
    if (!ag_loadsmallfont(ttf_file, size, zpath))
      ag_loadsmallfont("fonts/small",0,NULL);
  }
  else{
    if (!ag_loadbigfont(ttf_file, size, zpath))
      ag_loadbigfont("fonts/big",0,NULL);
  }

/* HuangGuoDong@Drv.recovery, 2013/1/25, add for mutex oppo_font() and oppo_set_title(), otherwise status bar may fliker*/     
  pthread_mutex_unlock(&refresh_mutex);
 
  //-- Return
  return RET_OK;
  
}
//* 
//* set_theme
//* 
STATUS  oppo_theme(char *theme) {
  return_val_if_fail(theme != NULL, RET_FAIL);
  
  //-- This is Busy Function
  ag_setbusy();
  
  if ((strcmp(theme,"")==0)||(strcmp(theme,"generic")==0)){
    //-- Background Should Be Redrawed
    oppo_isbgredraw = 1;
    
    
    //-- Return
    return RET_OK;
  }

  //-- Parse The Prop
  char themename[256];
  snprintf(themename,256,"%s/themes/%s/theme.prop",OPPO_DIR,theme);
  snprintf(acfg()->themename,64,"%s",theme);
  char * propstr = oppo_readfromzip(themename);
  if (propstr){
    int i=0;
    for (i=0;i<OPPO_THEME_CNT;i++){
      char * key = atheme_key(i);
      char * val = oppo_parsepropstring(propstr,key);
      if (val!=NULL){
        if (strcmp(val,"")!=0){
          snprintf(themename,256,"themes/%s/%s",theme,val);
          atheme_create(key,themename);
        }
        free(val);
      }
    }
    //printf("PASS THEME\n");
    oppo_setthemecolor(propstr,  "color.winbg",              &acfg()->winbg);
    oppo_setthemecolor(propstr,  "color.selectbg",            &acfg()->selectbg);
    oppo_setthemecolor(propstr,  "color.txt_title",              &acfg()->txt_title);
    oppo_setthemecolor(propstr,  "color.txt_menu",         &acfg()->txt_menu);
    oppo_setthemecolor(propstr,  "color.txt_select",           &acfg()->txt_select);
    oppo_setthemecolor(propstr,  "color.txt_version",         &acfg()->txt_version);
    oppo_setthemecolor(propstr,  "color.txt_pic_tit",           &acfg()->txt_pic_tit);
    oppo_setthemecolor(propstr,  "color.txt_pic_desc",             &acfg()->txt_pic_desc);
    oppo_setthemecolor(propstr,  "color.menuline_top",             &acfg()->menuline_top);
    oppo_setthemecolor(propstr,  "color.menuline_bot",        &acfg()->menuline_bot);
    oppo_setthemecolor(propstr,  "color.title_line",          &acfg()->title_line);
    oppo_setthemecolor(propstr,  "color.border",          &acfg()->border);
    oppo_setthemecolor(propstr,  "color.statusbg",          &acfg()->statusbg);
    oppo_setthemecolor(propstr,  "color.txt_status",          &acfg()->txt_status);
		
    oppo_setthemeconfig(propstr, "config.roundsize",         &acfg()->roundsz);
    oppo_setthemeconfig(propstr, "config.button_roundsize",  &acfg()->btnroundsz);
    oppo_setthemeconfig(propstr, "config.window_roundsize",  &acfg()->winroundsz);
    oppo_setthemeconfig(propstr, "config.transition_frame",  &acfg()->fadeframes);
    
    //printf("PASS THEME V\n");
    
    
    free(propstr);
  }
  else{
    memset(acfg()->themename, 0x00, 64);
  }

  //-- Background Should Be Redrawed
  oppo_isbgredraw = 1;
  
  
  //-- Return
  return RET_OK;
}

char * oppo_getprop(char *file, char *key) {
  return_null_if_fail(file != NULL); 
  return_null_if_fail(key != NULL); 
  //-- This is Busy Function
  ag_setbusy();

  //-- Parse The Prop
  char* result;
  char path[256];
  snprintf(path,256,"%s/%s",OPPO_DIR,file);
  result = oppo_parseprop(path,key);
  return result;
}
char * oppo_gettmpprop(char *file, char *key) {
  return_null_if_fail(file != NULL); 
  return_null_if_fail(key != NULL); 
  //-- This is Busy Function
  ag_setbusy();

  //-- Parse The Prop
  char* result;
  char path[256];
  snprintf(path,256,"%s/%s",OPPO_TMP,file);
  result = oppo_parseprop(path,key);
  return result;
}

//* 
//* resread, readfile_oppo
//* 
char * oppo_resread(char *file) {
  
  return_null_if_fail(file != NULL);
  //-- This is Busy Function
  ag_setbusy();
  
    
  //-- Create Path Into Resource Dir
  char path[256];
  snprintf(path,256,"%s/%s",OPPO_DIR,file);
  
  //-- Read From Zip
  char * buf = oppo_readfromzip(path);
  
  
  //-- Return
  return buf;
}

static pthread_t                       wipe_pthread; 
static int wipe_process;
#define OFSSET_Y_TXT_WIPE  (agdp()*113)

static int wipe_process_disp(char *wipe_title) {

  int ret = 0;
  int chkH        = agh();
  int chkY        = oppo_setbg_title();
  int chkW          = agw();
  char *title = "<~wipe.progress.namd>";
  char *desc ="<~wipe.progress.desc>";
  char percent[10];
  
  chkH -= chkY;
  //draw background
  ag_rect(&cOppoWipe, 0, chkY, chkW, chkH, acfg()->winbg);

  //draw title after status bar
  int txth       = ag_txtheight(agw(),wipe_title,1);
  int titleH        = HEIGHT_TITLE;
  int txtw     = ag_txtwidth(wipe_title,1);
  int txtx     = (chkW-txtw)/2;
  int txty     = chkY+ (round(titleH/2) - round(txth/2));
    
  ag_rect(&cOppoWipe, 0, chkY, chkW, titleH, acfg()->winbg);
  ag_rect(&cOppoWipe, LIST_LINE_OFFSET, chkY+titleH-HEIGHT_TITLE_LINE, chkW-LIST_LINE_OFFSET*2, HEIGHT_TITLE_LINE, acfg()->title_line);
  ag_text(&cOppoWipe, txtw, txtx, txty, wipe_title, acfg()->txt_title, 1);
  chkY = chkY + titleH;
  chkH = agh() - chkY;
  
  int titW = ag_txtwidth(title, 1);
  int titH = ag_fontheight(1);
  int titX = (agw()/2) - (titW/2);
  int titY = OFSSET_Y_TXT_WIPE;
  ag_textf(&cOppoWipe, titW, titX, titY, title, acfg()->txt_pic_tit, 1);

  int descW = ag_txtwidth(desc, 0);
  int descH = ag_fontheight(0);
  int descX = (agw()/2) - (descW/2);
  int descY = titY + titH + (agdp() *5);
  ag_textf(&cOppoWipe, descW,descX, descY, desc, acfg()->txt_pic_desc, 0);

  if (wipe_process == 100)
  	ret = 1;
#if 0  
  snprintf(percent, 10, "%3d%%", wipe_process);
  int perW = ag_txtwidth(percent, 1);
  int perH = ag_fontheight(1);
  int perX = (agw()/2) - (perW/2);
  int perY = descY + descH + (agdp() *10);
  ag_textf(&cOppoWipe, perW,perX, perY, percent, acfg()->txt_pic_desc, 1);
#endif
  int pad = agdp() *20;
  int barW =  agw() - pad*2;
  int barX = pad;
  int barY = descY + descH +(agdp() *8);
  int barH = agdp() *8;
  atheme_draw("img.progress", &cOppoWipe, barX, barY, barW, barH);
  atheme_draw("img.progress.fill", &cOppoWipe, barX, barY, barW*wipe_process/100, barH);
   
  ag_draw(NULL, &cOppoWipe, 0, 0);
  ag_sync();
  
  return ret;
}

void set_wipe_process(int process){
  if (process > wipe_process)
    wipe_process = process;
}
static void *wipe_thread(void *cookie){
  char *title = cookie;	
  printf("start .....title(%s)\n", title);
  while(1) {
  	if (wipe_process_disp(title)) {
              usleep(1000*1000);
		break;
	}
	usleep(200*1000);
	if(wipe_process < 97)
		wipe_process += 3;
	  
  }
  printf("end .....\n");
  return NULL;
}

void wipe_thread_start(char *title){
  wipe_process = 0;	
  pthread_create(&wipe_pthread, NULL, wipe_thread, title);

}

void wipe_thread_close(){
  pthread_join(wipe_pthread,NULL);
  //pthread_detach(wipe_pthread);
}

//* 
//* ini_get
//*
char * oppo_ini_get(char *item) {
  
  return_null_if_fail(item != NULL);
  //-- This is Busy Function
  ag_setbusy();
  
  
  //-- Convert Arguments
  char retval[128];
  memset(retval,0,128);
  
  //-- Set Property
  if      (strcmp(item,"roundsize") == 0)          snprintf(retval,128,"%i",acfg()->roundsz);
  else if (strcmp(item,"button_roundsize") == 0)   snprintf(retval,128,"%i",acfg()->btnroundsz);
  else if (strcmp(item,"window_roundsize") == 0)   snprintf(retval,128,"%i",acfg()->winroundsz);
  else if (strcmp(item,"transition_frame") == 0)   snprintf(retval,128,"%i",acfg()->fadeframes);

  else if (strcmp(item,"text_ok") == 0)            snprintf(retval,128,"%s",acfg()->text_ok);
  else if (strcmp(item,"text_next") == 0)          snprintf(retval,128,"%s",acfg()->text_next);
  else if (strcmp(item,"text_back") == 0)          snprintf(retval,128,"%s",acfg()->text_back);

  else if (strcmp(item,"text_yes") == 0)           snprintf(retval,128,"%s",acfg()->text_yes);
  else if (strcmp(item,"text_no") == 0)            snprintf(retval,128,"%s",acfg()->text_no);
  else if (strcmp(item,"text_about") == 0)         snprintf(retval,128,"%s",acfg()->text_about);
  else if (strcmp(item,"text_calibrating") == 0)   snprintf(retval,128,"%s",acfg()->text_calibrating);
  else if (strcmp(item,"text_quit") == 0)          snprintf(retval,128,"%s",acfg()->text_quit);
  else if (strcmp(item,"text_quit_msg") == 0)      snprintf(retval,128,"%s",acfg()->text_quit_msg);
#if 0    
  else if (strcmp(item,"rom_name") == 0)           snprintf(retval,128,"%s",acfg()->rom_name);
  else if (strcmp(item,"rom_version") == 0)        snprintf(retval,128,"%s",acfg()->rom_version);
  else if (strcmp(item,"rom_author") == 0)         snprintf(retval,128,"%s",acfg()->rom_author);
  else if (strcmp(item,"rom_device") == 0)         snprintf(retval,128,"%s",acfg()->rom_device);
  else if (strcmp(item,"rom_date") == 0)           snprintf(retval,128,"%s",acfg()->rom_date);
#endif  
  else if (strcmp(item,"customkeycode_up")==0)     snprintf(retval,128,"%i",acfg()->ckey_up);
  else if (strcmp(item,"customkeycode_down")==0)   snprintf(retval,128,"%i",acfg()->ckey_down);
  else if (strcmp(item,"customkeycode_select")==0) snprintf(retval,128,"%i",acfg()->ckey_select);
  else if (strcmp(item,"customkeycode_back") == 0) snprintf(retval,128,"%i",acfg()->ckey_back);
  else if (strcmp(item,"customkeycode_menu") == 0) snprintf(retval,128,"%i",acfg()->ckey_menu);
  else if (strcmp(item,"dp") == 0) snprintf(retval,128,"%i",agdp());
  

  //-- Return
  return strdup(retval);
} 

//* 
//* lang
//*
STATUS oppo_langmenu(char *title_name, char *title_icon) {

  ag_setbusy();
  oppo_isbgredraw=1;
  //-- Get Arguments
  
  //-- Variable Def
  int i;
  
  //-- Init Strings
  
  //-- Init Strings
  char text[256];
  snprintf(text,256,"%s",title_name);

  int pad         = agdp() * 4;
  int chkH        = agh();
  int chkW          = agw();
  
  //-- Draw Navigation Bar
  int chkY= oppo_setbg_title();
  chkH -= chkY; 

  //draw title
  int txth       = ag_txtheight(agw(),text,1);
  int titleH = HEIGHT_TITLE;
  int txtw     = ag_txtwidth(text,1);
  int txtx     = (chkW-txtw)/2;
  int txty     = chkY+ (round(titleH/2) - round(txth/2));

  ag_rect(&oppo_win_bg, 0, chkY, chkW, titleH, acfg()->winbg);
  ag_rect(&oppo_win_bg, LIST_LINE_OFFSET, chkY+titleH-HEIGHT_TITLE_LINE, chkW -LIST_LINE_OFFSET*2, HEIGHT_TITLE_LINE, acfg()->title_line);
  ag_text(&oppo_win_bg, txtw, txtx, txty, text, acfg()->txt_title, 1);
  chkY = chkY + titleH;
  chkH = agh() - chkY;
  
  //-- Create Window
  AWINDOWP hWin   = aw(&oppo_win_bg);

  ACONTROLP menu1  = acmenu(hWin,0,chkY,chkW,chkH,6);
  acmenu_add(menu1, "简体中文","@lang.cn" , NULL);
  acmenu_add(menu1, "繁體中文","@lang.cn_trad" , NULL);
  acmenu_add(menu1, "English", "@lang.en", NULL);

  //-- Dispatch Message
  aw_show(hWin);
  byte ondispatch = 1;
  byte onback = 0;
  while(ondispatch){
    dword msg=aw_dispatch(hWin);
    switch (aw_gm(msg)){
      case 6:{
            ondispatch = 0;
            onback = 0;
      }
      break;
      case 5:{
        //-- BACK
          onback = 1;
          ondispatch      = 0;
      }
      break;
      case 4:{
        //-- EXIT
        onback = 1;
        ondispatch      = 0;
      }
      break;
    }
  }
  
  int selindex;
  if (onback == 0)
      selindex = acmenu_getselectedindex(menu1) + 1;
   else selindex = 0;

/* OPPO 2013-02-19 jizhengkang azx Add begin for reason */
	selectedItem[selectedCount] = selindex - 1;
   	selectedCount++;
/* OPPO 2013-02-19 jizhengkang azx Add end */
  
  //-- Destroy Window
  aw_destroy(hWin);

  //-- Finish
  return selindex;
}
//* 
//* menubox
//*
int show_mainmenu = 0;//display VERSION info in the last line of mainmenu, and save mainmanu
int show_wipemenu = 0;//save wipemenu

STATUS oppo_mainmenu(char *title_name, char **item, char **item_icon, char **item_icon_append, int item_cnt) {
  oppo_debug("start...\n");
  //-- Set Busy before everythings ready
  return_val_if_fail(title_name != NULL, RET_FAIL);
  return_val_if_fail(item_cnt >= 0, RET_FAIL);
  ag_setbusy();
  oppo_isbgredraw=1;
  //-- Get Arguments
  
  //-- Variable Def
  int i;
  
  //-- Init Background
  
  //-- Init Strings
  
  //-- Drawing Data
  int pad         = agdp() * 4;
  int chkH        = agh();
  int chkW          = agw();
  
  //-- Draw Navigation Bar
  int chkY= oppo_setbg_title();
  chkH -= chkY; 

  //-- Create Window
  AWINDOWP hWin   = aw(&oppo_win_bg);

  //-- Check Box
  ACONTROLP backmenu = actitle(hWin, 0, chkY, chkW, &chkH, title_name, 1, 5);
  chkY = chkY + chkH;
  chkH = agh() - chkY;

  ACONTROLP menu1  = acmenu(hWin,0,chkY,chkW,chkH,6);

  //-- Populate Checkbox Items
  for (i=0;i<item_cnt;i++) {
    if (item[i] != NULL && strcmp(item[i],"")!=0){
      if (item_icon != NULL && item_icon_append != NULL)
           acmenu_add(menu1, item[i], item_icon[i], item_icon_append[i]);
      else {
          if (item_icon != NULL)
              acmenu_add(menu1, item[i], item_icon[i], NULL);
          else {
              if (item_icon_append != NULL)
                  acmenu_add(menu1, item[i], NULL, item_icon_append[i]);
              else 
                  acmenu_add(menu1, item[i], NULL, NULL);
          }
      }
    }
  }

  show_mainmenu = 1; 
  
   //huanggd for set focus
  if (menu1->onfocus!=NULL){
        if (menu1->onfocus(menu1)){
          hWin->focusIndex = 1;
        }
  }

  //-- Dispatch Message
  aw_show(hWin);
    
  byte ondispatch = 1;
  byte onback = 0;
  while(ondispatch){
    dword msg=aw_dispatch(hWin);
    switch (aw_gm(msg)){
      case 6:{
            ondispatch = 0;
      }
      break;
      case 5:{
        //-- BACK
          onback = 1;
          ondispatch      = 0;
      }
      break;
      case 4:{
        //-- EXIT
        onback = 1;
        ondispatch      = 0;
      }
      break;
    }
  }
  
  int selindex;
  if (onback == 0)
      selindex = acmenu_getselectedindex(menu1)+1;
   else selindex = 0;

/* OPPO 2013-02-19 jizhengkang azx Add begin for reason */
   selectedItem[selectedCount] = selindex - 1;
   selindex?selectedCount++:selectedCount--;
/* OPPO 2013-02-19 jizhengkang azx Add end */

  //-- Destroy Window
  aw_destroy(hWin);
  show_mainmenu = 0; 
  oppo_debug("end...\n");
  //-- Finish
  return selindex;
}

STATUS wipe_menu(char *title_name, char **item, char **item_icon, char **item_icon_append, int item_cnt) {
oppo_debug("start...\n");
  //-- Set Busy before everythings ready
  return_val_if_fail(title_name != NULL, RET_FAIL);
  return_val_if_fail(item_cnt >= 0, RET_FAIL);
  ag_setbusy();
  oppo_isbgredraw=1;
  //-- Get Arguments
  
  //-- Variable Def
  int i;
  
  //-- Init Background
  
  //-- Init Strings
  
  //-- Drawing Data
  int pad         = agdp() * 4;
  int chkH        = agh();
  int chkW          = agw();
  
  //-- Draw Navigation Bar
  int chkY= oppo_setbg_title();
  chkH -= chkY; 
  
  
  
  //-- Create Window
  AWINDOWP hWin   = aw(&oppo_win_bg);
  
  //-- Check Box
  ACONTROLP backmenu = actitle(hWin, 0, chkY, chkW, &chkH, title_name, 1, 5);
  chkY = chkY + chkH;
  chkH = agh() - chkY;
  ACONTROLP menu1  = acmenu(hWin,0,chkY,chkW,chkH,6);

  //-- Populate Checkbox Items
  for (i=0;i<item_cnt;i++) {
    if (item[i] != NULL && strcmp(item[i],"")!=0){
      if (item_icon != NULL && item_icon_append != NULL)
           acmenu_add(menu1, item[i], item_icon[i], item_icon_append[i]);
      else {
          if (item_icon != NULL)
              acmenu_add(menu1, item[i], item_icon[i], NULL);
          else {
              if (item_icon_append != NULL)
                  acmenu_add(menu1, item[i], NULL, item_icon_append[i]);
              else 
                  acmenu_add(menu1, item[i], NULL, NULL);
          }
      }
    }
  }

  show_wipemenu = 1;//save wipemenu
  
  //huanggd for set focus
  if (menu1->onfocus!=NULL){
        if (menu1->onfocus(menu1)){
          hWin->focusIndex = 1;
        }
  }
  
  //-- Dispatch Message
  aw_show(hWin);
    
  byte ondispatch = 1;
  byte onback = 0;
  while(ondispatch){
    dword msg=aw_dispatch(hWin);
    switch (aw_gm(msg)){
      case 6:{
            ondispatch = 0;
      }
      break;
      case 5:{
        //-- BACK
          onback = 1;
          ondispatch      = 0;
      }
      break;
      case 4:{
        //-- EXIT
        onback = 1;
        ondispatch      = 0;
      }
      break;
    }
  }

  int selindex;
  if (onback == 0)
      selindex = acmenu_getselectedindex(menu1)+1;
   else selindex = 0;
  
  //-- Destroy Window
  aw_destroy(hWin);
  
  show_wipemenu = 0;
  //-- Finish
  return selindex;
}
STATUS oppo_menubox(char *title_name, char **item,  int item_cnt) {
  oppo_debug("start...\n");
  return_val_if_fail(item_cnt >= 1, RET_FAIL); 
  return_val_if_fail(title_name != NULL, RET_FAIL);
  return_val_if_fail(item != NULL, RET_FAIL);
  //-- Set Busy before everythings ready
  ag_setbusy();
  oppo_isbgredraw = 1;
  
  //-- Variable Def
  int i;
  
  //-- Drawing Data
  int pad         = agdp() * 4;
  int chkH        = agh();
  int chkY        = oppo_setbg_title();
  int chkW          = agw();
  
  //-- Draw Navigation Bar
  chkH -= chkY; 
  //-- Create Window
  AWINDOWP hWin   = aw(&oppo_win_bg);
  
  ACONTROLP backmenu = actitle(hWin, 0, chkY, chkW, &chkH, title_name, 1, 5);
  chkY = chkY + chkH;
  chkH = agh() - chkY;
  //-- Check Box
  ACONTROLP menu1  = acmenu(hWin,0,chkY,chkW,chkH,6);

  //-- Populate Checkbox Items
  for (i = 0; i < item_cnt; i++) {
    if (item[i] != NULL && strcmp(item[i],"")!=0)
      acmenu_add(menu1,item[i], NULL, NULL);
  }
  //huanggd add for set focus
  if (menu1->onfocus!=NULL){
        if (menu1->onfocus(menu1)){
          hWin->focusIndex = 1;
        }
  }
  //-- Dispatch Message
  aw_show(hWin);
  byte ondispatch = 1;
  byte onback = 0;
  while(ondispatch){
    dword msg=aw_dispatch(hWin);
    switch (aw_gm(msg)){
      case 6:{
           ondispatch = 0;

      }
      break;
      case 5:{
        //-- BACK
          onback = 1; 
          ondispatch      = 0;
        
      }
      break;
      case 4:{
        //-- EXIT
        onback = 1;
        ondispatch      = 0;
      }
      break;
    }
  }
  
  int selindex;
  if (onback == 0)
      selindex = acmenu_getselectedindex(menu1)+1;
  else selindex = 0;
  
  
  //-- Destroy Window
  aw_destroy(hWin);
  oppo_debug("end...\n");
  //-- Finish
  return selindex;
}

#define DIR_ICON  "@dir"
#define FILE_ICON "@file"
#define BACK_ICON "@back"
/*
 *print sd file system,
 *menu titlname, use set_bg print title 
 *menu name, 
 *
 *
 */

int sd_menu_use = 0;

STATUS oppo_sdmenu(char *title_name, char **item, char **item_sub, int item_cnt) {
	oppo_debug("start...\n");
	//-- Set Busy before everythings ready
	ag_setbusy();
	oppo_isbgredraw = 1;
	sd_menu_use = 1;

	//-- Get Arguments

	//-- Variable Def
	int i;

	//-- Init Background

	//-- Init Strings
	int pad = agdp() * 4;
	int chkH = agh();
	int chkY = oppo_setbg_title();
	int chkW = agw();
	//-- Draw Navigation Bar
	chkH -= chkY; 
	//-- Create Window
	AWINDOWP hWin = aw(&oppo_win_bg);
	int selindex;

	ACONTROLP backmenu = actitle(hWin, 0, chkY, chkW, &chkH, title_name, 1, 5);

	chkY = chkY + chkH;
	chkH = agh() - chkY;

	
	if (item_cnt) {
		//-- Check Box
		ACONTROLP menu1 = acmenu(hWin,0,chkY,chkW,chkH,6);

	
		//-- Populate Checkbox Items
		for (i=0;i<item_cnt;i++) {
			int item_len = strlen(item[i]);
			if (strcmp(item[i],"") != 0)
			{
				acmenu_add(menu1, item[i],NULL, NULL); 
			}
		}

		//huanggd add for set focus
		if (menu1->onfocus!=NULL){
			if (menu1->onfocus(menu1)){
				hWin->focusIndex = 1;
			}
		}

		//-- Dispatch Message
		aw_show(hWin);
		byte ondispatch = 1;  
		byte onback = 0;//huanggd for exit explorer when select "return" option
		while(ondispatch){
			dword msg = aw_dispatch(hWin);
			switch (aw_gm(msg)){
				case 6:{
					ondispatch = 0;
				}
				break;
				case 5:{
					//-- BACK
					onback = 1;//huanggd for exit explorer when select "return" option
					ondispatch = 0;

				}
				break;
				case 4:{
					//-- EXIT
					ondispatch = 0;
				}
				break;
			}
		}
		
		//int selindex = acsdmenu_getselectedindex(menu1) ;
		selindex = acmenu_getselectedindex(menu1);
		if (onback)
			selindex = -1;//huanggd for exit explorer when select "return" option
	}
	else {
		ag_rect(&hWin->c, 0, chkY, chkW, chkH, acfg()->winbg);

//		hWin->focusIndex = 0;
		
		aw_show(hWin);
		byte ondispatch = 1;  
		byte onback = 0;//huanggd for exit explorer when select "return" option
		while(ondispatch){
			dword msg=aw_dispatch(hWin);
			switch (aw_gm(msg)){
				case 6:{
					ondispatch = 0;
				}
				break;
				case 5:{
					//-- BACK
					onback = 1;//huanggd for exit explorer when select "return" option
					ondispatch = 0;

				}
				break;
				case 4:{
					//-- EXIT
					ondispatch = 0;
				}
				break;
			}
		}
		selindex = -1;
		if (onback)
			selindex = -1;
	}
	sd_menu_use = 0;
	//-- Destroy Window
	aw_destroy(hWin);
	oppo_debug("end...\n");
	//-- Finish
	return selindex;
}

//* 
//* confirm
//*
STATUS oppo_confirm(int argc, char *format, ...) {
  if ((argc<2)||(argc>5)) {
    return oppo_error("%s() expects 2-4 args (title, text, [icon, yes text, no text]), got %d", __FUNCTION__, argc);
  }
  oppo_debug("start .....\n");
  //-- Set Busy before everythings ready
  ag_setbusy();
  
  //-- Get Arguments
  _INITARGS();
  
  //-- Show Confirm
  byte res = aw_confirm(
    NULL,
    args[0],
    args[1],
    (argc>2)?args[2]:"",
    (argc>3)?args[3]:NULL,
    (argc>4)?args[4]:NULL,
    2
  );
  
  //-- Release Arguments
  _FREEARGS();
  
  //-- Return
  if (res) return RET_YES;
  return RET_NO;
}

STATUS oppo_notice(int argc, char *format, ...) {
  if ((argc<2)||(argc>5)) {
    return oppo_error("%s() expects 2-4 args (title, text, [icon, yes text, no text]), got %d", __FUNCTION__, argc);
  }
  oppo_debug("start .....\n");
  //-- Set Busy before everythings ready
  ag_setbusy();
  
  //-- Get Arguments
  _INITARGS();
  
  //-- Show Confirm
  byte res = aw_confirm(
    NULL,
    args[0],
    args[1],
    (argc>2)?args[2]:"",
    (argc>3)?args[3]:NULL,
    (argc>4)?args[4]:NULL,
    1
  );
  
  //-- Release Arguments
  _FREEARGS();

  oppo_debug("end...\n");
  //-- Return
  if (res) return RET_YES;
  return RET_NO;
}

#define HEIGHT_IMG_UMS (agdp()*39)
#define OFFSET_Y_IMG_UMS  (agdp()*74)

void oppo_ums_notic(menuUnit *p) {

  oppo_debug("start .....\n");
//-- Initializing Canvas
  CANVAS alertbg;

  ag_setbusy();
  oppo_isbgredraw = 1;
  
  ag_canvas(&alertbg,agw(),agh());
  ag_draw(&alertbg,agc(),0,0);
  
  int chkH        = agh();
  int chkY        = oppo_setbg_title();
  int chkW          = agw();
  char *title = "<~ums.notice.name>";
  char *desc = "<~ums.notice.desc>";
  char percent[10];
  
  chkH -= chkY;
  //draw background
  ag_rect(&alertbg, 0, chkY, chkW, chkH, acfg()->winbg);

#if 1
  int imgH = HEIGHT_IMG_UMS;
  int imgY = OFFSET_Y_IMG_UMS;
  
  PNGCANVAS image;
  if (apng_load(&image, "@ums")) {
      int imgW=imgH*image.w/image.h;	
      int imgX = round((agw()-imgW)/2);
      apng_stretch(&alertbg, &image, imgX, imgY, imgW, imgH, 0, 0, image.w, image.h);
      apng_close(&image);
  }

  int titW = ag_txtwidth(title, 1);
  int titH = ag_fontheight(1);
  int titX = (agw()/2) - (titW/2);
  int titY = imgH + imgY + agdp()*9;
  int color_tit = acfg()->txt_pic_tit;
  ag_textf(&alertbg, titW, titX, titY, title, color_tit, 1);

  int descW = ag_txtwidth(desc, 0);
  int descH = ag_fontheight(0);
  int descX = (agw()/2) - (descW/2);
  int descY = titY + titH + (agdp() *5);
  int color_desc = acfg()->txt_pic_desc;
  ag_textf(&alertbg, descW,descX, descY, desc, color_desc, 0);  
  #else
  int titW = ag_txtwidth(title, 1);
  int titH = ag_fontheight(1);
  int titX = (agw()/2) - (titW/2);
  int titY = chkH/3;
  ag_textf(&alertbg, titW, titX, titY, title, acfg()->txt_pic_tit, 1);

  int descW = ag_txtwidth(desc, 0);
  int descH = ag_fontheight(0);
  int descX = (agw()/2) - (descW/2);
  int descY = titY + titH + (agdp() *2);
  ag_textf(&alertbg, descW,descX, descY, desc, acfg()->txt_pic_desc, 0);
#endif
  AWINDOWP hWin   = aw(&alertbg);
  
  int btnW    = agw()-LIST_LINE_OFFSET*2;
  int btnH = (agdp()*26 > ag_fontheight(1))?agdp()*26:ag_fontheight(1);
  int btnY    = agh()-btnH-LIST_LINE_OFFSET;
  int btnX = LIST_LINE_OFFSET;
  acbutton(hWin,btnX,btnY,btnW,btnH,"<~ums.btn.name>",1,6,1);
  actitle(hWin, 0, chkY, chkW, &chkH, p->name, 1, 5);
  
  aw_show(hWin);
  byte ondispatch = 1;
  byte res = 0;
  while(ondispatch){
    dword msg=aw_dispatch(hWin);
    switch (aw_gm(msg)){
      case 6: res=1; ondispatch = 0; break;
      case 5: ondispatch = 0; break;
    }
  }
  aw_destroy(hWin);
  ag_ccanvas(&alertbg);
  
}

//* 
//* loadlang
//*
static void oppo_langloadsave(char * dest, int max, char * key){
  char * val = alang_get(key);
  if (val!=NULL) snprintf(dest,max,"%s",val);
}

STATUS oppo_loadlang(char * name)
{
  ag_setbusy();
  
  
  //-- Load Language Data
  char path[256];
  snprintf(path,256,"%s/%s",OPPO_DIR,name);
  byte res = alang_load(path);
  
  //-- Replace Text
  if (res){
    acfg_reset_text();
    oppo_langloadsave(acfg()->text_ok, 64, "text_ok");
    oppo_langloadsave(acfg()->text_next, 64, "text_next");
    oppo_langloadsave(acfg()->text_back, 64, "text_back");
    oppo_langloadsave(acfg()->text_yes, 64, "text_yes");
    oppo_langloadsave(acfg()->text_no, 64, "text_no");
    oppo_langloadsave(acfg()->text_about, 64, "text_about");
    oppo_langloadsave(acfg()->text_calibrating, 64, "text_calibrating");
    oppo_langloadsave(acfg()->text_quit, 64, "text_quit");
    oppo_langloadsave(acfg()->text_quit_msg, 128, "text_quit_msg");
  }
  
  return res; 
}

//* 
//* lang
//*
  
char * oppo_lang(char *name){
  //-- Set Busy before everythings ready
  ag_setbusy();

  char * out = alang_get(name);

  return out;
}

static pthread_t  install_disp_pthread; 
static volatile int install_disp_end = 0;//0:installing   1:install success    -1:install failed

#define HEIGHT_IMG_UPDATE  (agdp()*39)
#define OFFSET_Y_IMG_UPDATE  (agdp()*74)

static void install_process_disp(int prog_percent){
  //oppo_debug("start .....\n");

  int chkH        = agh();
  int chkY        = oppo_setbg_title();
  int chkW          = agw();
  char *title = "<~install.package.name>";
  char *desc = "<~install.package.desc>";;
  char percent[10];
  
  chkH -= chkY;
  //draw background
  ag_rect(&cOppoIntall, 0, chkY, chkW, chkH, acfg()->winbg);
#if 0   
  int titW = ag_txtwidth(title, 1);
  int titH = ag_fontheight(1);
  int titX = (agw()/2) - (titW/2);
  int titY = chkH/3;
  ag_textf(&cOppoIntall, titW, titX, titY, title, acfg()->winfg, 1);

  int descW = ag_txtwidth(desc, 0);
  int descH = ag_fontheight(0);
  int descX = (agw()/2) - (descW/2);
  int descY = titY + titH + (agdp() *2);
  ag_textf(&cOppoIntall, descW,descX, descY, desc, acfg()->winfg, 0);
	
  snprintf(percent, 9, "%3d%%", prog_percent);
  int perW = ag_txtwidth(percent, 1);
  int perH = ag_fontheight(1);
  int perX = (agw()/2) - (perW/2);
  int perY = descY + descH + (agdp() *10);
  ag_textf(&cOppoIntall, perW,perX, perY, percent, acfg()->winfg, 1);
  
  int pad = agdp() *10;
  int barW =  agw() - pad*2;
  int barX = pad;
  int barY = perY + perH +(agdp() *10);
  int barH = agdp() *10;
  ag_rect(&cOppoIntall,barX,barY,barW,barH,acfg()->winfg);
  ag_rect(&cOppoIntall,barX+1,barY+1,barW-2,barH-2,acfg()->winbg);
  ag_rect(&cOppoIntall,barX+1,barY+1,(barW-2)*prog_percent/100,barH-2,acfg()->titlefg);
#else

  int imgH = HEIGHT_IMG_UPDATE;
  int imgY = OFFSET_Y_IMG_UPDATE;
  
  
  PNGCANVAS image;
  if (apng_load(&image, "@update")) {
      int imgW=imgH*image.w/image.h;	
      int imgX = round((agw()-imgW)/2);
      apng_stretch(&cOppoIntall, &image, imgX, imgY, imgW, imgH, 0, 0, image.w, image.h);
      apng_close(&image);
  }

  int titW = ag_txtwidth(title, 1);
  int titH = ag_fontheight(1);
  int titX = (agw()/2) - (titW/2);
  int titY = imgH + imgY + agdp()*9;
  int color_tit = acfg()->txt_pic_tit;
  ag_textf(&cOppoIntall, titW, titX, titY, title, color_tit, 1);

  int descW = ag_txtwidth(desc, 0);
  int descH = ag_fontheight(0);
  int descX = (agw()/2) - (descW/2);
  int descY = titY + titH + (agdp() *5);
  int color_desc = acfg()->txt_pic_desc;
  ag_textf(&cOppoIntall, descW,descX, descY, desc, color_desc, 0);  

  
  int pad = agdp() *20;
  int barW =  agw() - pad*2;
  int barX = pad;
  int barY = descY + descH +(agdp() *8);
  int barH = agdp() *8;
  atheme_draw("img.progress", &cOppoIntall, barX, barY, barW, barH);
  atheme_draw("img.progress.fill", &cOppoIntall, barX, barY, barW*prog_percent/100, barH);
#endif
#if 0// for test
  PNGCANVAS image;
  if (!apng_load(&image, "@alert")) {

  } else {
      int imgW = image.w;
      int imgH = image.h;

      int imgY = agh()-imgH+10*agdp();
      int imgX = agw() - imgW - 5*agdp();
      apng_stretch(&cOppoIntall, &image, imgX, imgY, imgW, imgH, 0, 0, image.w, image.h);
      apng_close(&image);
  }
#endif

  //ag_draw(NULL, &cOppoIntall, 0, 0);
  memcpy(agc()->data, cOppoIntall.data, agc()->sz);
  ag_sync();
  //oppo_debug("end .....\n");
}

static void *install_progressthread(void *cookie){
  static int prog_percent = 0, cnt = 0;
  int tmp;
  
  oppo_printf("start .....\n");	
  prog_percent = 0;
  while(1) {
    tmp = oppo_get_progress();
    if (tmp > prog_percent)
	prog_percent = tmp;

    if (install_disp_end == 1)	
        prog_percent = 100;
        
    install_process_disp(prog_percent);	
    if ((prog_percent == 100) || install_disp_end == -1) {
	usleep(1000000);
	break;
    }	
 
    if (prog_percent < 50) {
        prog_percent++;
    }	
    else if (prog_percent < 99) {
	 if (cnt++ == 7) {
            prog_percent++;
	     cnt = 0;		
	 }	
    }
	
    usleep(300*1000);	
  }
   
  oppo_printf("end .....\n");
  return NULL;
}

extern void oppoInstall_reset_progress();
extern void ui_set_progress(float fraction);

static void install_progress_disp_start() {
    install_disp_end = 0;	
    oppoInstall_reset_progress();	
    pthread_create(&install_disp_pthread, NULL, install_progressthread, NULL);
}

STATUS oppo_install(void) {
    int ret = 0;

    oppo_debug("start....\n");
/* OPPO 2013-01-05 jizhengkang Modify begin for reset progress when the second or the third updatepackage starts */
#if 0
    install_progress_disp_start();
    ui_set_progress(0.0);	
#else
    ui_set_progress(0.0);
    install_progress_disp_start();
#endif
/* OPPO 2013-01-05 jizhengkang Modify end */

    ret = oppo_install_package();
    if (ret == 0) {	
        install_disp_end = 1;
    } else {
        install_disp_end = -1;
    }			
    pthread_join(install_disp_pthread,NULL);
	
    oppo_debug("end....  ret(%d)\n", ret);	
    return ret;	
}

#define OPPO_INITARGS() \
          char** args = ReadVarArgs(state, argc, argv); \
          if (args==NULL) return NULL;

#define OPPO_FREEARGS() \
          int freearg_i; \
          for (freearg_i=0;freearg_i<argc;++freearg_i) free(args[freearg_i]); \
          free(args);
extern int ag_changcolor(char ch1, char ch2, char ch3, char ch4);

Value* OPPO_INI_SET(const char* name, State* state, int argc, Expr* argv[]) {
  if (argc != 2) {
    oppo_error("%s() expects 2 args(config name, config value in string), got %d", name, argc);
    return StringValue(strdup(""));
  }
  
  //-- This is Busy Function
  ag_setbusy();
  
  //-- Get Arguments
  OPPO_INITARGS();
  //-- Convert Arguments
  byte valint = (byte) min(atoi(args[1]),255);
  int  valkey = (int) atoi(args[1]);
  
  //-- Set Property
  if      (strcmp(args[0],"roundsize") == 0)          acfg()->roundsz=valint;
  else if (strcmp(args[0],"button_roundsize") == 0)   acfg()->btnroundsz=valint;
  else if (strcmp(args[0],"window_roundsize") == 0)   acfg()->winroundsz=valint;
  else if (strcmp(args[0],"transition_frame") == 0)   acfg()->fadeframes=valint;

  else if (strcmp(args[0],"text_ok") == 0)            snprintf(acfg()->text_ok,64,"%s", args[1]);
  else if (strcmp(args[0],"text_next") == 0)          snprintf(acfg()->text_next,64,"%s", args[1]);
  else if (strcmp(args[0],"text_back") == 0)          snprintf(acfg()->text_back,64,"%s", args[1]);

  else if (strcmp(args[0],"text_yes") == 0)           snprintf(acfg()->text_yes,64,"%s", args[1]);
  else if (strcmp(args[0],"text_no") == 0)            snprintf(acfg()->text_no,64,"%s", args[1]);
  else if (strcmp(args[0],"text_about") == 0)         snprintf(acfg()->text_about,64, "%s", args[1]);
  else if (strcmp(args[0],"text_calibrating") == 0)   snprintf(acfg()->text_calibrating,64,"%s", args[1]);
  else if (strcmp(args[0],"text_quit") == 0)          snprintf(acfg()->text_quit,64,"%s", args[1]);
  else if (strcmp(args[0],"text_quit_msg") == 0)      snprintf(acfg()->text_quit_msg,128,"%s", args[1]);
#if 0    
  else if (strcmp(args[0],"rom_name") == 0)           snprintf(acfg()->rom_name,128,"%s", args[1]);
  else if (strcmp(args[0],"rom_version") == 0)        snprintf(acfg()->rom_version,128,"%s", args[1]);
  else if (strcmp(args[0],"rom_author") == 0)         snprintf(acfg()->rom_author,128,"%s", args[1]);
  else if (strcmp(args[0],"rom_device") == 0)         snprintf(acfg()->rom_device,128,"%s", args[1]);
  else if (strcmp(args[0],"rom_date") == 0)           snprintf(acfg()->rom_date,128,"%s", args[1]);
#endif  
  
  else if (strcmp(args[0],"customkeycode_up")==0)     acfg()->ckey_up=valkey;
  else if (strcmp(args[0],"customkeycode_down")==0)   acfg()->ckey_down=valkey;
  else if (strcmp(args[0],"customkeycode_select")==0) acfg()->ckey_select=valkey;
  else if (strcmp(args[0],"customkeycode_back") == 0) acfg()->ckey_back=valkey;
  else if (strcmp(args[0],"customkeycode_menu") == 0) acfg()->ckey_menu=valkey;
  //add for input event filter
  else if (strcmp(args[0], "input_filter")  == 0) {
     acfg()->input_filter = valkey;
	 oppo_debug("input is 0x%x\n", acfg()->input_filter);
  }
    
  //-- Force Color Space  
  else if (strcmp(args[0],"force_colorspace") == 0){
         ag_changcolor(args[1][0], args[1][1], args[1][2], args[1][3]);
  }
  else if (strcmp(args[0],"dp") == 0){
    set_agdp(valint);
  }
  
  oppo_isbgredraw = 1;
  //-- Release Arguments
  OPPO_FREEARGS();

  //-- Return
  return StringValue(strdup(""));
}

Value* OPPO_CALIBRATE(const char* name, State* state, int argc, Expr* argv[]) {
  //-- Return
  return StringValue(strdup(""));
}

static void oppo_ui_register()
{
//todo
    //--CONFIG FUNCTIONS
    //
    RegisterFunction("ini_set",       OPPO_INI_SET);       //-- SET INI CONFIGURATION
    RegisterFunction("calibrate", OPPO_CALIBRATE);
    //RegisterFunction("calibtool", OPPO_CALIB);
}
int oppo_ui_init()
{
    acfg_init();
    //register function
    RegisterBuiltins();
    oppo_ui_register();
    FinishRegistration();
	return 0;
}

extern int yyparse(Expr** root, int* error_count);

//read config file if exist and then execute it
int oppo_ui_config(const char *file)
{
    //if file exist 
    return_val_if_fail(file != NULL, RET_FAIL);
    struct stat file_stat;
    if (stat(file, &file_stat) < 0)
    {
        oppo_printf("stat file error, file is not exist\n");
        return -1;
    }
    char *script_data = oppo_readfromfs(file);
    return_val_if_fail(script_data != NULL, RET_FAIL);

   //--PARSE CONFIG SCRIPT
   Expr* root;
   int error_count = 0;
   yy_scan_string(script_data); 
   int error = yyparse(&root, &error_count);
   if (error != 0 || error_count > 0) {
       oppo_printf("read file %s failed!\n", file);
       goto config_fail;
   }
   //--- EVALUATE CONFIG SCRIPT 
   State state;
   state.cookie = NULL;
   state.script = script_data;
   state.errmsg = NULL;
   char* result = Evaluate(&state, root);
   if (result == NULL) {
       if (state.errmsg == NULL) {
           oppo_printf("script abortedl\n");
       } else {
           free(state.errmsg);
       }
   } else {
       oppo_printf("scripte aborted!\n");
       free(result);
   }
config_fail:
   if (script_data != NULL) free(script_data);
   return -1;
}

STATUS oppo_ui_start()
{
    int i = 0;
    for (i = 0; i < OPPO_THEME_CNT; i++)
    {
        acfg()->theme[i] = NULL;
        acfg()->theme_9p[i] = 0;
    }
    ag_canvas(&oppo_win_bg, agw(), agh());
    ag_canvas(&oppo_bg, agw(), agh());

    ag_canvas(&cOppoIntall, agw(), agh());
    ag_canvas(&cOppoWipe, agw(), agh());	
    oppo_theme("oppo");
    ag_loadsmallfont("fonts/small", 0, NULL);
    ag_loadbigfont("fonts/big", 0, NULL);
    alang_release();
    return RET_OK;
}
STATUS oppo_ui_end()
{
    ag_ccanvas(&oppo_win_bg);
    ag_ccanvas(&oppo_bg);

    ag_ccanvas(&cOppoIntall);
    ag_ccanvas(&cOppoWipe);
    alang_release();
    atheme_releaseall();
    return RET_OK;
}
