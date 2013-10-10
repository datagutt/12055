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

#include "../oppo_inter.h"

/***************************[ BACK ]**************************/
#define TITLE_TXT_OFFSET (agdp()*16)

static const char * image_back = "@back_arrow";

typedef struct{
  char titlep[64];
  int h;
  int w;
  
  int ty;
  int th;


  CANVAS    control;
  CANVAS    control_pushed;
  CANVAS    control_focused;
  byte      touchmsg;
  byte      focused;
  byte      pushed;
} ACBACKD, * ACBACKDP;

static byte actitle_touchoncontrol(void *win, int x, int y) {
    ACONTROLP ctl  = (ACONTROLP) win;
    ACBACKDP  d  = (ACBACKDP) ctl->d;
    int wx = ctl->x;
    int wx2 = ctl->x + d->w;
    int wy = ctl->y;
    int wy2 = ctl->y + d->h;
    //if ((x >= wx) && (x < wx2) && (y >= wy) && (y < wy2))
    if ((y >= wy) && (y < wy2))		
        return 1;
    return 0;
}

dword actitle_oninput(void * x,int action,ATEV * atev){
	ACONTROLP ctl  = (ACONTROLP) x;
	ACBACKDP  d  = (ACBACKDP) ctl->d;
  
#ifdef DEBUG
	oppo_debug("*****%s entry******\n", __FUNCTION__);
#endif
	dword msg = 0;
	if (atev->k != 888 || actitle_touchoncontrol(x, atev->x, atev->y)){
		switch (action){
			case ATEV_MOUSEDN:
			{
				vibrate(30);
				d->pushed=1;
				msg=aw_msg(0,1,0,0);
				ctl->ondraw(ctl);
			}
			break;
			case ATEV_MOUSEUP:
			{
				d->pushed=0;
#ifdef DEBUG
				oppo_debug("touch MOUSEUP in %s\n", __FUNCTION__);
#endif
				msg=aw_msg(d->touchmsg,1,0,0);
				ctl->ondraw(ctl);
			}
			break;
			case ATEV_SELECT:
			{
				if (atev->d){
					vibrate(30);
					d->pushed=1;
					msg=aw_msg(0,1,0,0);
					ctl->ondraw(ctl);
				}
				else{
					d->pushed=0;
					msg=aw_msg(d->touchmsg,1,0,0);
					ctl->ondraw(ctl);
				}
			}
			break;
		}
	} else {
		d->pushed = 0;
		msg = aw_msg(0, 1, 0, 0);
		ctl->ondraw(ctl);
	}
	return msg;
}
void actitle_ondraw(void * x){
  ACONTROLP   ctl= (ACONTROLP) x;
  ACBACKDP  d  = (ACBACKDP) ctl->d;
  CANVAS *  pc = &ctl->win->c;

  if (d->pushed){
    ag_draw(pc,&d->control_pushed,ctl->x,ctl->y);
  }
  else if(d->focused) {
    ag_draw(pc,&d->control_focused,ctl->x,ctl->y);
  }
  else
    ag_draw(pc,&d->control,ctl->x,ctl->y);
}
void actitle_ondestroy(void * x){
  ACONTROLP   ctl= (ACONTROLP) x;
  ACBACKDP  d  = (ACBACKDP) ctl->d;
  ag_ccanvas(&d->control);
  ag_ccanvas(&d->control_pushed);
  ag_ccanvas(&d->control_focused);
  free(ctl->d);
}
byte actitle_onfocus(void * x){
  ACONTROLP   ctl= (ACONTROLP) x;
  ACBACKDP  d  = (ACBACKDP) ctl->d;

  d->focused=1;
  ctl->ondraw(ctl);
  return 1;
}
void actitle_onblur(void * x){
  ACONTROLP   ctl= (ACONTROLP) x;
  ACBACKDP  d  = (ACBACKDP) ctl->d;
  
  d->focused=0;
  ctl->ondraw(ctl);
}
ACONTROLP actitle(
  AWINDOWP win,
  int x,
  int y,
  int w,
  int *ph,
  char * text,
  byte isbig,
  byte touchmsg
){
  //-- Validate Minimum Size
  if (*ph<agdp()*16) *ph=agdp()*16;
  if (w<agdp()*16) w=agdp()*16;
  
  //-- Initializing Text Metrics

  int txth     = ag_fontheight(isbig);
  
  //-- Initializing Button Data
  ACBACKDP d = (ACBACKDP) malloc(sizeof(ACBACKD));
  memset(d,0,sizeof(ACBACKD));

  *ph = HEIGHT_TITLE;
  int h = *ph;  
  
  PNGCANVAS * image = (PNGCANVAS*)malloc(sizeof(PNGCANVAS));

  //normal
  memset(image, 0, sizeof(PNGCANVAS));
  if (!apng_load(image, image_back)){
      oppo_printf("load %s failed\n", image_back);
      goto apng_load_fail;
  } 
  
  //-- Initializing Canvas
  ag_canvas(&d->control,w,h);
  ag_canvas(&d->control_pushed,w,h);
  ag_canvas(&d->control_focused,w,h);

  //add background of titile and black line of bottom 
  //atheme_draw("img.titlebg", &d->control,0,0,w,h);
  //atheme_draw("img.titlebg", &d->control_pushed,0,0,w,h);
  //atheme_draw("img.titlebg", &d->control_focused,0,0,w,h);  
  
  ag_rect(&d->control, 0, 0, w, h, acfg()->winbg);
  ag_rect(&d->control_pushed, 0, 0, w, h, acfg()->selectbg);
  ag_rect(&d->control_focused, 0, 0, w, h, acfg()->selectbg);  
  
  ag_rect(&d->control, LIST_LINE_OFFSET, h-HEIGHT_TITLE_LINE, w-LIST_LINE_OFFSET*2, HEIGHT_TITLE_LINE, acfg()->title_line);
  ag_rect(&d->control_pushed, LIST_LINE_OFFSET, h-HEIGHT_TITLE_LINE, w-LIST_LINE_OFFSET*2, HEIGHT_TITLE_LINE, acfg()->title_line);
  ag_rect(&d->control_focused, LIST_LINE_OFFSET, h-HEIGHT_TITLE_LINE, w-LIST_LINE_OFFSET*2, HEIGHT_TITLE_LINE, acfg()->title_line);

  int imgH = h;
  int imgW=imgH*image->w/image->h;
  int imgX = 0;
  int imgY = round((imgH-imgH)/2);

  apng_stretch(&d->control, image, imgX ,imgY, imgW, imgH, 0, 0, image->w, image->h);
  apng_stretch(&d->control_focused, image, imgX ,imgY, imgW, imgH, 0, 0, image->w, image->h);
  apng_stretch(&d->control_pushed, image, imgX ,imgY, imgW, imgH, 0, 0, image->w, image->h);

  apng_close(image);
apng_load_fail:
  free(image);
  image = NULL;
  //normal
  //
  //-- Save Touch Message & Set Stats
  d->touchmsg  = touchmsg;
  d->focused   = 0;
  d->pushed    = 0;
  //init th , ty id h,w
  d->th = h;
  d->ty = y;
  d->h = *ph;
  d->w = imgW;
  
  int txtx     = TITLE_TXT_OFFSET;
  int txty     = round(h/2) - round(txth/2);
  int txtw     = ag_txtwidth(text,isbig);

  ag_text(&d->control, txtw, txtx, txty, text, acfg()->txt_title, isbig);
  //focused
  ag_text(&d->control_focused, txtw, txtx, txty, text, acfg()->txt_select, isbig);
  //pushed
  ag_text(&d->control_pushed, txtw, txtx, txty, text, acfg()->txt_select, isbig);

  //-- Initializing Control
  ACONTROLP ctl  = malloc(sizeof(ACONTROL));
  ctl->ondestroy= &actitle_ondestroy;
  ctl->oninput  = &actitle_oninput;
  ctl->ondraw   = &actitle_ondraw;
  ctl->onblur   = &actitle_onblur;
  ctl->onfocus  = &actitle_onfocus;
  ctl->win      = win;
  ctl->x        = x;
  ctl->y        = y;
  ctl->w        = w;
  ctl->h        = h;
  ctl->forceNS  = 0;
  ctl->d        = (void *) d;
  aw_add(win,ctl);
  return ctl;
}
