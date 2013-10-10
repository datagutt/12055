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

#ifndef __OPPO_INTER_H__
#define __OPPO_INTER_H__

//
// Common Headers, Always Used
//
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <ctype.h>
#include <minutf8.h>
#include <pthread.h>

//
// Freetype Headers
//
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_TRUETYPE_IDS_H
#include <freetype/ftsynth.h>
#include FT_GLYPH_H

//
// ARM NEON - Testing Only
//
#ifdef __ARM_NEON__
  #include <arm_neon.h>
#endif

#ifndef malloc
#define malloc(x) malloc(x)

#define realloc(x,s) realloc(x,s)
#endif

#ifndef free
#define free(x) if(x != NULL){ \
                   free(x);x=NULL;}
#endif

//#ifdef DEBUG
#define oppo_debug(fmt...) printf("(%lu ticks)(pid:%d)[%s]%s:%d::", alib_tick(), getpid(), __FILE__, __FUNCTION__, __LINE__);printf(fmt)
//#else
//#define oppo_debug(fmt...) do{}while(0)
//#endif
#ifndef oppo_printf
#define oppo_printf(fmt...) printf("(pid:%d)[%s]%s:%d::", getpid(), __FILE__, __FUNCTION__, __LINE__);printf(fmt)
#endif
#ifndef oppo_error
#define oppo_error(fmt...) printf("(pid:%d)[%s]%s:%d::", getpid(), __FILE__, __FUNCTION__, __LINE__);printf(fmt)
#endif
#ifndef return_val_if_fail
#define return_val_if_fail(p, val) \
	if (!(p)) { \
	   printf("(pid:%d)function %s(line %d) cause %s failed  return %d\n", getpid(), __FUNCTION__, __LINE__, #p,  val);return val;}	
#endif
#ifndef return_null_if_fail
#define return_null_if_fail(p) \
	if (!(p)) { \
	   printf("(pid:%d)[%s]function %s(line %d) " #p " \n",getpid(),__FILE__,  __FUNCTION__, __LINE__);return NULL;}	
#endif
#ifndef assert_if_fail
#define assert_if_fail(p) \
	if (!(p)) { \
	   printf("(pid:%d)[%s]function %s(line %d) " #p " \n",getpid(), __FILE__,  __FUNCTION__, __LINE__);}	
#endif
//#######################################################//
//##                                                   ##//
//##               LIST OF DEFINITIONS                 ##//
//##                                                   ##//
//#######################################################//
//
// Common Data Type
//
#define byte              unsigned char
#define dword             unsigned int
#define word              unsigned short
#define color             unsigned short
typedef int u32;

//-- Temporary Dir - Move from /tmp/oppo-data to /tmp/oppo symlink to /tmp/oppo-data for backward compatibility
#define OPPO_SYSTMP      "/tmp"
#define OPPO_TMP         OPPO_SYSTMP "/oppo"
#define OPPO_TMP_S       OPPO_SYSTMP "/oppo-data"

#define OPPO_DIR         "/res"
#define OPPO_FRAMEBUFFER "/dev/graphics/fb0"

#define OPPO_THEME_CNT 8
//Canvas Structure

#define HEIGHT_TITLE (agdp()*22)
#define HEIGHT_TITLE_LINE (agdp()*4/6)
#define LIST_LINE_OFFSET (agdp()*6)

typedef struct{
	int     w;       // Width
	int     h;       // Height
	int     sz;      // Data Size
	color * data;    // Data 
} CANVAS;

//
// Assosiative Array Structure
//
typedef struct{
  char * key;
  char * val;
} AARRAY_ITEM, * AARRAY_ITEMP;

typedef struct{
  int length;
  AARRAY_ITEMP items;
} AARRAY, * AARRAYP;

AARRAYP   aarray_create();
char *    aarray_get(AARRAYP a, char * key);
byte      aarray_set(AARRAYP a, char * key, char * val);
byte      aarray_del(AARRAYP a, char * key);
byte      aarray_free(AARRAYP a);

//
// PNG Canvas Structure
//
typedef struct {
  int     w;       // Width
  int     h;       // Height
  int     s;       // Buffer Size
  byte    c;       // Channels
  byte *  r;       // Red Channel
  byte *  g;       // Green Channel
  byte *  b;       // Blue Channel
  byte *  a;       // Alpha Channel
} PNGCANVAS, * PNGCANVASP;


//
// PNG Font Canvas Structure
//
typedef struct {
  byte    loaded;    // Font is Loaded 
  int     fx[96];    // Font X Positions
  byte    fw[96];    // Font Width
  byte    fh;        // Font Height
  int     w;         // Png Width
  int     h;         // Png Height
  int     s;         // Buffer Size
  byte    c;         // Channels
  byte *  d;         // Fonts Alpha Channel
} PNGFONTS;

//
// ZIP Memory Structure
//
typedef struct{
	int sz;         // Data Size
	byte *data;     // Data 
} AZMEM;

//
// FREETYPE GLYPH CACHE
//
typedef struct {
  FT_Glyph  g;
  byte      w;    // width
  byte      init; // cached
} AFTGLYPH, * AFTGLYPHP;

//
// FREETYPE FONT FACE
//
typedef struct {
  FT_Face     face;
  AFTGLYPHP   cache;
  long        cache_n;
  byte        kern;
  byte *      mem;
} AFTFACE, * AFTFACEP;

//
// FREETYPE FAMILY
//
typedef struct {
  //-- Face Holder
  AFTFACEP  faces;
  int       facen;
  
  //-- General Info
  byte      s;
  byte      p;
  byte      h;
  byte      y;
  byte      init;
} AFTFAMILY, * AFTFAMILYP;

//
// Touch & Event Structure
//
typedef struct{
	int   x;        // Touch X
	int   y;        // Touch Y
	int   d;        // Down State
	int   k;        // Key Code
	dword msg;      // Window Message for postmessage
} ATEV;


//
// Math Macro
//
#define max(a,b) ((a>=b)?a:b)
#define min(a,b) ((a<=b)?a:b)
#define LOWORD(l) ((word)(l))
#define HIWORD(l) ((word)(((dword)(l) >> 16) & 0xFFFF))
#define MAKEDWORD(a, b) ((dword) (((word) (a)) | ((dword) ((word) (b))) << 16))

//
// Graphic Pixel Macro
//
#define ag_r(rgb)	          ((byte) (((((word)(rgb))&0xF800))>>8) ) 
#define ag_g(rgb)	          ((byte) (((((word)(rgb))&0x07E0))>>3) ) 
#define ag_b(rgb)	          ((byte) (((((word)(rgb))&0x001F))<<3) ) 

#define ag_rgb(r,g,b)       ((color) ((r >> 3) << 11)| ((g >> 2) << 5)| ((b >> 3) << 0))
#define ag_rgba32(r,g,b,a)  ((dword)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))
#define ag_rgb32(r,g,b)     ag_rgba32(r,g,b,0xff)
#define ag_b32(rgb)         ((byte) (rgb)) 
#define ag_g32(rgb)         ((byte) (((word) (rgb))>>8))
#define ag_r32(rgb)         ((byte) ((rgb) >> 16)) 

#define ag_a32(rgb)         ((byte) (((dword) (rgb))>>24))
#define ag_close_r(r)       (((byte) r)>>3<<3)
#define ag_close_g(g)       (((byte) g)>>2<<2)
#define ag_close_b(b)       ag_close_r(b)
#define ag_rgbto32(rgb)     (ag_rgba32(ag_r(rgb),ag_g(rgb),ag_b(rgb),0xff))
#define ag_rgbto16(rgb)     (ag_rgb(ag_r32(rgb),ag_g32(rgb),ag_b32(rgb)))


//
// Touch Event Code
//
#define ATEV_DOWN     1
#define ATEV_UP       2
#define ATEV_LEFT     3
#define ATEV_RIGHT    4
#define ATEV_SELECT   5
#define ATEV_BACK     6
#define ATEV_MENU     7
#define ATEV_HOME     8
#define ATEV_MOUSEDN  9
#define ATEV_MOUSEUP  10
#define ATEV_MOUSEMV  11
#define ATEV_SEARCH   12
#define ATEV_MESSAGE  30
#define KEY_CENTER    232


//
// Kinetic Library Structures
//
#define AKINETIC_HISTORY_LENGTH     10
#define AKINETIC_DAMPERING          0.98              // Gravity
typedef struct  {
  byte    isdown;                                     // Is Touch Down
  double  velocity;                                   // Fling Velocity
  int     previousPoints[AKINETIC_HISTORY_LENGTH];    // Touch Y Pos History
  long    previousTimes[AKINETIC_HISTORY_LENGTH];     // Touch Time History
  byte    history_n;                                  // Number of Touch History
} AKINETIC;


//
// Window Message - In DWORD (4bytes)
//
// m = message, d = draw, l = don't lost focus, h = high value
#define aw_msg(m,d,l,h) ((dword)((((h)&0xff)<<24)|(((l)&0xff)<<16)|(((d)&0xff)<<8)|((m)&0xff)))
#define aw_gm(msg)      ((byte) (msg)) 
#define aw_gd(msg)      ((byte) (((word) (msg))>>8))
#define aw_gl(msg)      ((byte) ((msg) >> 16)) 
#define aw_gh(msg)      ((byte) (((dword) (msg))>>24))


//
// Main Configuration Structure
//
typedef struct  {
  // Colors
  color winbg;                // Window Background
  color selectbg;             // Selected Item/Control Background 
  color txt_title;               // Text of title Font Color
  color txt_menu;               // Text of menu items Font Color 
  color txt_select;           // Text of selectd Font Color
  color txt_version;             // Text of version Font Color
  color txt_pic_tit;           // Text of title font Color when updating/wiping/UMS 
  color txt_pic_desc;             // Text of description font Color when updating/wiping/UMS 
  color menuline_top;               //  seperation Line Color of first line between menu items
  color menuline_bot;          // seperation Line Color of Second line between menu items
  color title_line;              //bottom of title line color
  color border;                 //border color of Prompt box
  color statusbg;              //statusbar Background
  color txt_status;           //text of statusbar font color
  
  // Property
  byte  roundsz;              // Control Rounded Size
  byte  btnroundsz;           // Button Control Rounded Size
  byte  winroundsz;           // Window Rounded Size
  
  // Transition
  byte  fadeframes;           // Number of Frame used for Fade Transition
  
  // Common Text
  char  text_ok[64];          // OK
  char  text_next[64];        // Next >
  char  text_back[64];        // < Back
  
  char  text_yes[64];         // Yes
  char  text_no[64];          // No
  char  text_about[64];       // About
  char  text_calibrating[64]; // Calibration Tools
  char  text_quit[64];        // Quit
  char  text_quit_msg[128];   // Quit Message
#if 0  
  // ROM Text
  char rom_name[128];          // ROM Name
  char rom_version[128];       // ROM Version
  char rom_author[128];        // ROM Author
  char rom_device[128];        // ROM Device Name
  char rom_date[128];          // ROM Date
#endif  
  // CUSTOM KEY
  int ckey_up;
  int ckey_down;
  int ckey_select;
  int ckey_back;
  int ckey_menu;
  u32 input_filter;
  
  // THEME
  PNGCANVASP theme[OPPO_THEME_CNT];
  byte       theme_9p[OPPO_THEME_CNT];
  char themename[64];
} AC_CONFIG;



//
// Window Control Callback Typedef
//
typedef dword (*AC_ONINPUT)(void *,int,ATEV *);
typedef void  (*AC_ONBLUR)(void *);
typedef byte  (*AC_ONFOCUS)(void *);
typedef void  (*AC_ONDRAW)(void *);
typedef void  (*AC_ONDESTROY)(void *);


//
// Window Structure
//
typedef struct{
  byte          isActived;    // Active & Showed
	CANVAS *      bg;           // Background Canvas
	CANVAS        c;            // Window drawing canvas
	void**        controls;     // Child Controls
	int           controln;     // Number of Controls
	int           threadnum;    // Number of running thread
	int           focusIndex;   // Child Focus Index
	int           touchIndex;   // Child Touch Index
} AWINDOW, *AWINDOWP;


//
// Control Structure
//
typedef struct{
  AWINDOWP      win;          // Parent Window
  AC_ONDESTROY  ondestroy;    // On Destroy Callback
  AC_ONINPUT    oninput;      // On Input Callback
  AC_ONDRAW     ondraw;       // On Draw Callback
  AC_ONBLUR     onblur;       // On Blur Callback
  AC_ONFOCUS    onfocus;      // On Focus Callback
  int           x;            // Control X
  int           y;            // Control Y
  int           w;            // Control Width
  int           h;            // Control Height
  byte          forceNS;      // Force to Stop Scroll
  void *        d;            // Control Specific Data
} ACONTROL, *ACONTROLP;

//move from another file by jizhengkang azx
typedef struct{
  char title[64];
  PNGCANVAS * img;
  PNGCANVAS * img_append;
  int  id;
  int  h;
  int  y;
  
  /* Title & Desc Size/Pos */
  int  th;
} ACMENUI, * ACMENUIP;

typedef struct{
  /*append up the items*/
  byte      acheck_signature;
  CANVAS    client;
  CANVAS    control;
  CANVAS    control_focused;
  AKINETIC  akin;
  int       scrollY;
  int       maxScrollY;
  int       prevTouchY;
  int       invalidDrawItem;
  
  /* Client Size */
  int clientWidth;
  int clientTextW;
  int clientTextX;
  int nextY;
  
  /* Items */
  ACMENUIP * items;
  int       itemn;
  int       touchedItem;
  int       focusedItem;
  int       draweditemn;
  int       selectedIndex;
  byte      touchmsg;
  /* Focus */
  byte      focused;
} ACMENUD, * ACMENUDP;
//move end

//#######################################################//
//##                                                   ##//
//##                LIST OF FUNCTIONS                  ##//
//##                                                   ##//
//#######################################################//

//az_readmem  in oppo_lib function

byte az_readmem(AZMEM * out,const char * zpath, byte bytesafe);
//
// Root Functions
//
FILE *    apipe();        // Recovery pipe to communicate the command
byte      oppo_start();    // Start recoveary UI
char *    oppo_readfromfs(char * name);
char *    oppo_readfromzip(char * name);
char*     getArgv(int id);
void      a_reboot(byte type);

//
// Languages Functions
//
void alang_release();
byte alang_load(char * z);
char * alang_ams(const char * str);
void acfg_reset_text();
char * alang_get(char * key);



//-- UI Functions
//char * oppo_parsepropstring(char * buffer,char *key);
char * oppo_readfromzip(char * name);
char * oppo_getvar(char * name);


//-- .9.png struct
typedef struct{
  int x;  //-- Strect X
  int y;  //-- Strect Y
  int w;  //-- Strect Width  
  int h;  //-- Strect Height
  
  int t;  //-- Padding Top
  int l;  //-- Padding Left
  int b;  //-- Padding Bottom
  int r;  //-- Padding Right
} APNG9, *APNG9P;

//
// PNG Functions
//
byte      apng_load(PNGCANVAS * pngcanvas,char* imgname);         // Load PNG From Zip Item
void      apng_close(PNGCANVAS * pngcanvas);                            // Release PNG Memory
byte      apng_draw(CANVAS * _b, PNGCANVAS * p, int xpos, int ypos);    // Draw PNG Into Canvas
byte apng_stretch(
  CANVAS * _b,
  PNGCANVAS * p,
  int dx,
  int dy,
  int dw,
  int dh,
  
  int sx,
  int sy,
  int sw,
  int sh  
);
byte apng9_calc(PNGCANVAS * p, APNG9P v,byte with_pad);
byte apng9_draw(
  CANVAS * _b,
  PNGCANVAS * p,
  int dx,
  int dy,
  int dw,
  int dh,
  APNG9P v,
  byte with_pad
);

//
// Freetype Wrapper
//
byte    aft_fontready(byte isbig);
byte    aft_open();
byte    aft_close();
int     aft_kern(int c, int p, byte isbig);
int     aft_fontwidth(int c,byte isbig);
int     aft_spacewidth(byte isbig);
byte    aft_fontheight(byte isbig);
byte    aft_load(const char * source_name, int size, byte isbig,char * relativeto);
byte    aft_drawfont(CANVAS * _b, byte isbig, int fpos, int xpos, int ypos, color cl,byte underline,byte bold);
// byte    aft_loadfont(char * zpath, byte size, byte isbig);

//
// Freetype Arabic & RTL Handler
//
byte AFT_ISARABIC(int c);
byte aft_read_arabic(int * soff, const char * src, const char ** ss, int * string, byte * prop, int maxlength, int * outlength, int * move);
byte aft_isrtl(int c,byte checkleft);


//
// PNG Font Functions
//
byte      apng_loadfont(PNGFONTS * pngfont,const char* imgname);        // Load PNG Font From Zip Item
byte      apng_drawfont(CANVAS * _b, PNGFONTS * p, byte fpos,           // Draw PNG Font Into Canvas
            int xpos, int ypos, color cl, byte underline, byte bold);
byte      apng_draw_ex(CANVAS * _b, PNGCANVAS * p, int xpos,            // Draw PNG Font Into Canvas
            int ypos, int sxpos, int sypos,int sw, int sh);             // With Extra Arguments
void apng_closefont(PNGFONTS *p);

//
// Graphic Function
//
byte      ag_isfreetype(byte isbig);
byte      ag_fontready(byte isbig);
CANVAS *  agc();          // Get Main Graph Canvas
byte      ag_init();      // Init  Graph and Framebuffers
void      ag_close_thread(); // Close Graph Thread
void      ag_close();     // Close Graph and Framebuffers
void      ag_changecolor(char , char, char, char); // Change Color Space

void      ag_sync();                        // Sync Main Canvas with Framebuffer
int       agw();                            // Get Display X Resolution
int       agh();                            // Get Display Y Resolution
int       agdp();                           // Get Device Pixel Size (WVGA = 3, HVGA = 2)
void      set_agdp(int dp);                 // Force Graphic Device Pixel Size
void      ag_sync_fade(int frame);          // Transition Sync - Async
void      ag_sync_fade_wait(int frame);     // Transition Sync - Sync
void      ag_sync_force();                  // Force to Sync
void      ag_setbusy();                     // Set Display to show Please Wait Progress
byte ag_blur(CANVAS *d, CANVAS *s, int radius);

//
// Canvas Functions
//
void ag_canvas(CANVAS * c,int w,int h);   // Create Canvas
void ag_ccanvas(CANVAS * c);              // Release Canvas
void ag_blank(CANVAS * c);                // Set Blank into Canvas memset(0)


//
// Canvas Manipulation Functions
//
color *   agxy(CANVAS *_b, int x, int y);                             // Get Pixel Pointer
byte      ag_setpixel(CANVAS *_b,int x, int y,color cl);              // Set Pixel Color
byte      ag_subpixel(CANVAS *_b,int x, int y, color cl,byte l);      // Set Pixel Color with Opacity


//
// Canvas Drawing Functions
//
byte      ag_rect(CANVAS *_b,int x, int y, int w, int h, color cl);   // Draw Solid Rectangle
byte      ag_rectopa(CANVAS *_b,int x, int y, int w, int h,           // Draw Solid Rectangle with Opacity
            color cl,byte l);
byte      ag_draw(CANVAS * dc,CANVAS * sc,int dx, int dy);            // Draw Canvas to Canvas
byte      ag_draw_ex(CANVAS * dc,CANVAS * sc, int dx, int dy,         // Draw Canvas to Canvas + Extra Arguments
            int sx, int sy, int sw, int sh);
byte      ag_roundgrad(CANVAS *_b,int x, int y, int w, int h,         // Draw Rounded & Gradient Rectangle
            color cl1, color cl2, int roundsz);
byte      ag_roundgrad_ex(CANVAS *_b,int x, int y, int w, int h,      // Draw Rounded & Gradient Rectangle
            color cl1, color cl2, int roundsz, byte tlr,              // With Extra Arguments
            byte trr, byte blr, byte brr);


//
// Color Calculator Functions
//
color     ag_subpixelget(CANVAS *_b,int x, int y, color cl,byte l);   // Calculate Color Opacity with Canvas Pixel
color     ag_calculatealpha(color dcl,color scl,byte l);              // Calculate 2 Colors with Opacity
color     strtocolor(char * c);                                       // Convert String Hex Color #fff,#ffffff to color
dword     ag_calchighlight(color c1,color c2);
dword     ag_calcpushlight(color c1,color c2);
color     ag_calpushad(color c_g);
color     ag_calculatecontrast(color c,float intensity);

//
// PNG Font Functions
//
int   ag_fontheight(byte isbig);                      // Get Font Height
byte  ag_loadsmallfont(char * fontname, byte is_freetype, char * relativeto); // Load Small Font From Zip
byte  ag_loadbigfont(char * fontname, byte is_freetype, char * relativeto); // Load Big Font From Zip
void  ag_closefonts();                                // Release Big & Small Fonts
byte  ag_drawchar(CANVAS *_b,int x, int y, int c,    // Draw Character into Canvas
        color cl, byte isbig);
byte ag_drawchar_ex(CANVAS *_b,int x, int y, int c, color cl, byte isbig, byte underline, byte bold);
byte  ag_text(CANVAS *_b,int maxwidth,int x,int y,    // Draw String into Canvas
        const char *s, color cl,byte isbig);
byte  ag_textf(CANVAS *_b,int maxwidth,int x,int y,    // Draw String into Canvas
        const char *s, color cl,byte isbig);          // Force Default Color

byte ag_text_ex(CANVAS *_b,int maxwidth,int x,int y,  // Draw String into Canvas
        const char *s, color cl_def,byte isbig,       // With Extra Arguments
        byte forcecolor);
int   ag_txtheight(int maxwidth,                      // Calculate String Height to be drawn
        const char *s, byte isbig);
int   ag_txtwidth(const char *s, byte isbig);         // Calculate String Width to be drawn
int  ag_tabwidth(int x, byte isbig);
byte ag_fontwidth(int c,byte isbig);                // Calculate font width for 1 character
byte ag_texts(CANVAS *_b,int maxwidth,int x,int y, const char *s, color cl_def,byte isbig);
byte ag_textfs(CANVAS *_b,int maxwidth,int x,int y, const char *s, color cl_def,byte isbig);
byte ag_text_exl(CANVAS *_b,int maxwidth,int x,int y, const char *s, color cl_def,byte isbig,byte forcecolor,byte multiline);
//
// EVENTS & Input Functions
//   NOTE: Contains Others Works
//         Modified from "minui/events.c"
//         Copyright (C) 2007 The Android Open Source Project
//         Licensed under the Apache License
//
byte    atouch_gethack();
void    atouch_sethack(byte t);
struct input_event;
int     atouch_wait(ATEV *atev);
int     atouch_wait_ex(ATEV *atev, byte calibratingtouch);
byte    atouch_send_message(dword msg);
int     vibrate(int timeout_ms);
void    ui_init();
int     ev_init(void);
void    ev_exit(void);
int     ev_get(struct input_event *ev, unsigned dont_wait);
//int     ui_wait_key();
int     ui_key_pressed(int key);
void    ui_clear_key_queue();
int     touchX();
int     touchY();
int     ontouch();
void    set_key_pressed(int key,char val);

//
// System Library Functions
//
int * ai_rtrimw(int * chr,int len);
char * ai_rtrim(char * chr);
char * ai_trim(char * chr);
byte  ismounted(char * path);
byte alib_disksize(const char * path, unsigned long * ret, int division);
int   alib_diskusage(const char * path);
byte alib_diskfree(const char * path, unsigned long * ret, int division);
void  alib_exec(char * cmd, char * arg);
void  create_directory(const char *path);
int   remove_directory(const char *path);
long  alib_tick();

//
// Kinetic Calculator Functions
//
void  akinetic_downhandler(AKINETIC * p, int mouseY);
int   akinetic_movehandler(AKINETIC * p, int mouseY);
byte  akinetic_uphandler(AKINETIC * p, int mouseY);
int   akinetic_fling(AKINETIC * p);
int   akinetic_fling_dampered(AKINETIC * p, float dampersz);

//
// Customization Functions
//
AC_CONFIG * acfg();           // Get Config Structure
void        acfg_init();      // Set Default Config
void acfg_init_ex(byte themeonly);

//
// Start Main Installer
//
int oppo_start_install(
  CANVAS * bg,
  int cx, int cy, int cw, int ch,
  int px, int py, int pw, int ph,
  CANVAS * cvf, int imgY, int chkFY, int chkFH,
  int echo
);

int oppo_get_progress(); 
int oppo_install_package();
	
//
// THEME MANAGER
//
void        atheme_releaseall();
void        atheme_release(char * key);
PNGCANVASP  atheme_create(char * key, char * path);
PNGCANVASP  atheme(char * key);
int         atheme_id(char * key);
char *      atheme_key(int id);
byte        atheme_id_draw(int id, CANVAS * _b, int x, int y, int w, int h);
byte        atheme_draw(char * key, CANVAS * _b, int x, int y, int w, int h);

//
// Window Management System Functions
//
AWINDOWP  aw(CANVAS * bg);                                  // Create New Window
void      aw_destroy(AWINDOWP win);                         // Destroy Window
void      aw_show(AWINDOWP win);                            // Show Window
void      aw_draw(AWINDOWP win);                            // Redraw Window
void      aw_add(AWINDOWP win,ACONTROLP ctl);               // Add Control into Window
void      aw_post(dword msg);                               // Post Message
dword     aw_dispatch(AWINDOWP win);                        // Dispatch Event, Message & Input
byte      aw_touchoncontrol(ACONTROLP ctl, int x, int y);   // Calculate Touch Position
byte      aw_setfocus(AWINDOWP win,ACONTROLP ctl);          // Set Focus into Control
void      aw_set_on_dialog(byte d);
// Window Dialog Controls
//
void aw_alert(AWINDOWP parent,char * titlev,char * textv,char * img,char * ok_text);
byte aw_confirm(AWINDOWP parent, char * titlev,char * textv,char * img,char * yes_text,char * no_text,int num_button);
void aw_about_dialog(AWINDOWP parent);
byte aw_showmenu(AWINDOWP parent);

byte aw_notice(AWINDOWP parent, char * titlev,char * textv,char * img,char * yes_text,char * no_text);

//
// Window Threading Functions
//
void ac_regbounce(
  ACONTROLP       ctl,
  int *           scrollY,
  int             maxScrollY
);
void ac_regfling(
  ACONTROLP       ctl,
  AKINETIC *      akin,
  int *           scrollY,
  int             maxScrollY
);
void ac_regpushwait(
  ACONTROLP     ctl,
  int *         moveY,
  int *         flagpointer,
  int           flagvalue
);
void ac_regscrollto(
  ACONTROLP       ctl,
  int *           scrollY,
  int             maxScrollY,
  int             requestY,
  int *           requestHandler,
  int             requestValue
);

//
// Controls Functions
//
void actext_rebuild(ACONTROLP ctl,int x,int y,int w,int h,char * text,byte isbig,byte toBottom);
void actext_appendtxt(ACONTROLP ctl,char * txt);

ACONTROLP acbutton(
  AWINDOWP win,
  int x,
  int y,
  int w,
  int h,
  char * text,
  byte isbig,
  byte touchmsg,
  int style
);
ACONTROLP actitle(
  AWINDOWP win,
  int x,
  int y,
  int w,
  int *ph,
  char *text,
  byte isbig,
  byte touchmsg
);

byte accheck_add(ACONTROLP ctl,char * title, char * desc, byte checked);
byte accheck_addgroup(ACONTROLP ctl,char * title, char * desc);
int accheck_itemcount(ACONTROLP ctl);
byte accheck_ischecked(ACONTROLP ctl, int index);
byte accheck_isgroup(ACONTROLP ctl, int index);
int accheck_getgroup(ACONTROLP ctl, int index);
int accheck_getgroupid(ACONTROLP ctl, int index);
ACONTROLP acopt(
  AWINDOWP win,
  int x,
  int y,
  int w,
  int h
);
byte acopt_addgroup(ACONTROLP ctl,char * title, char * desc);
byte acopt_add(ACONTROLP ctl,char * title, char * desc, byte selected);
int acopt_getselectedindex(ACONTROLP ctl,int group);
int acopt_getgroupid(ACONTROLP ctl, int index);
byte accb_ischecked(ACONTROLP ctl);


ACONTROLP acsdmenu(
  AWINDOWP win,
  int x,
  int y,
  int w,
  int h,
  byte touchmsg
);
byte acsdmenu_add(ACONTROLP ctl,char * title, char *title_sub, char *image);
int acsdmenu_getselectedindex(ACONTROLP ctl);

ACONTROLP acmenu(
  AWINDOWP win,
  int x,
  int y,
  int w,
  int h,
  byte touchmsg
);
byte acmenu_add(ACONTROLP ctl,char * title, char *img, char *img_append);
int acmenu_getselectedindex(ACONTROLP ctl);


typedef int (*fileFun)(char * file_name, int len, void *data);
typedef int (*fileFilterFun)(char *file, int file_len);
int file_scan(char *path, int path_len, char * title, int title_len, fileFun fun, void* data, fileFilterFun filter_fun);

//**********[ LOGGING ]**********//
#define _OPPO_DEBUG_TAG "oppo"
#if 0
#define LOGS(...) fprintf(stdout, _OPPO_DEBUG_TAG "/s: " __VA_ARGS__)
#define LOGE(...) fprintf(stdout, _OPPO_DEBUG_TAG "/e: " __VA_ARGS__)
#define LOGW(...) fprintf(stdout, _OPPO_DEBUG_TAG "/w: " __VA_ARGS__)
#define LOGI(...) fprintf(stdout, _OPPO_DEBUG_TAG "/i: " __VA_ARGS__)
#define LOGV(...) fprintf(stdout, _OPPO_DEBUG_TAG "/v: " __VA_ARGS__)
#define LOGD(...) fprintf(stdout, _OPPO_DEBUG_TAG "/d: " __VA_ARGS__)
#else
#define LOGS(...) fprintf(stdout, _OPPO_DEBUG_TAG "/s: " __VA_ARGS__)
#define LOGE(...) fprintf(stdout, _OPPO_DEBUG_TAG "/e: " __VA_ARGS__)
#define LOGW(...) /**/
#define LOGI(...) /**/
#define LOGV(...) /**/
#define LOGD(...) /**/
#endif
#define STRINGIFY(x) #x
#define EXPAND(x) STRINGIFY(x)

#endif // __OPPO_H__
