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

/***************************[ BUTTON ]**************************/
typedef struct{
  CANVAS    control;
  CANVAS    control_pushed;
  CANVAS    control_focused;
  byte      touchmsg;
  byte      focused;
  byte      pushed;
} ACBUTTOND, * ACBUTTONDP;
dword acbutton_oninput(void * x,int action,ATEV * atev){
  ACONTROLP ctl  = (ACONTROLP) x;
  ACBUTTONDP  d  = (ACBUTTONDP) ctl->d;
  
  dword msg = 0;
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
        if (aw_touchoncontrol(ctl,atev->x,atev->y))
          msg=aw_msg(d->touchmsg,1,0,0);
        else
          msg=aw_msg(0,1,0,0);
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
  return msg;
}
void acbutton_ondraw(void * x){
  ACONTROLP   ctl= (ACONTROLP) x;
  ACBUTTONDP  d  = (ACBUTTONDP) ctl->d;
  CANVAS *  pc = &ctl->win->c;
  
  if (d->pushed)
    ag_draw(pc,&d->control_pushed,ctl->x,ctl->y);
  else if(d->focused)
    ag_draw(pc,&d->control_focused,ctl->x,ctl->y);
  else
    ag_draw(pc,&d->control,ctl->x,ctl->y);
}
void acbutton_ondestroy(void * x){
  ACONTROLP   ctl= (ACONTROLP) x;
  ACBUTTONDP  d  = (ACBUTTONDP) ctl->d;
  ag_ccanvas(&d->control);
  ag_ccanvas(&d->control_pushed);
  ag_ccanvas(&d->control_focused);
  free(ctl->d);
}
byte acbutton_onfocus(void * x){
  ACONTROLP   ctl= (ACONTROLP) x;
  ACBUTTONDP  d  = (ACBUTTONDP) ctl->d;
  d->focused=1;
  ctl->ondraw(ctl);
  return 1;
}
void acbutton_onblur(void * x){
  ACONTROLP   ctl= (ACONTROLP) x;
  ACBUTTONDP  d  = (ACBUTTONDP) ctl->d;
  
  d->focused=0;
  ctl->ondraw(ctl);
}
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
){
  //-- Validate Minimum Size
  if (h<agdp()*16) h=agdp()*16;
  if (w<agdp()*16) w=agdp()*16;
  
  //-- Initializing Text Metrics
  int txtw     = ag_txtwidth(text,isbig);
  int txth     = ag_fontheight(isbig);
  int txtx     = round(w/2) - round(txtw/2);
  int txty     = round(h/2) - round(txth/2);
  
  //-- Initializing Button Data
  ACBUTTONDP d = (ACBUTTONDP) malloc(sizeof(ACBUTTOND));
  memset(d,0,sizeof(ACBUTTOND));
  
  //-- Save Touch Message & Set Stats
  d->touchmsg  = touchmsg;
  d->focused   = 0;
  d->pushed    = 0;
  
  //-- Initializing Canvas
  ag_canvas(&d->control,w,h);
  ag_canvas(&d->control_pushed,w,h);
  ag_canvas(&d->control_focused,w,h);

  if (style == 1) {
     //-- Draw Rest Control
    ag_draw_ex(&d->control,&win->c,0,0,x,y,w,h);	
    atheme_draw("img.button", &d->control,0,0,w,h);
    ag_text(&d->control,txtw,txtx,txty,text,acfg()->txt_menu,isbig);
  
    //-- Draw Pushed Control
    ag_draw_ex(&d->control_pushed,&win->c,0,0,x,y,w,h);
    atheme_draw("img.button.focus", &d->control_pushed,0,0,w,h);
    ag_text(&d->control_pushed,txtw,txtx,txty,text,acfg()->txt_select,isbig);
  
    //-- Draw Focused Control
    ag_draw_ex(&d->control_focused,&win->c,0,0,x,y,w,h);
    atheme_draw("img.button.focus", &d->control_focused,0,0,w,h);
    ag_text(&d->control_focused,txtw,txtx,txty,text,acfg()->txt_select,isbig);
  } else {
     //-- Draw Rest Control
    ag_draw_ex(&d->control,&win->c,0,0,x,y,w,h);	
    //ag_rect(&d->control, 0, 0, w, h, acfg()->winbg);	 
    ag_roundgrad(&d->control,0,0,w,h,acfg()->winbg,acfg()->winbg, agdp());	
    ag_rect(&d->control, 0, 1, w, 1, acfg()->menuline_bot);
    ag_rect(&d->control, 0, 0, w, 1, acfg()->menuline_top);	
    ag_text(&d->control,txtw,txtx,txty,text,acfg()->txt_menu,isbig);
  
    //-- Draw Pushed Control
    ag_draw_ex(&d->control_pushed,&win->c,0,0,x,y,w,h);
    //ag_rect(&d->control_pushed, 0, 0, w, h, acfg()->selectbg);	
    ag_roundgrad(&d->control_pushed,0,0,w,h,acfg()->selectbg,acfg()->selectbg, agdp());		
    ag_rect(&d->control_pushed, 0, 1, w, 1, acfg()->menuline_bot);
    ag_rect(&d->control_pushed, 0, 0, w, 1, acfg()->menuline_top);	
    ag_text(&d->control_pushed,txtw,txtx,txty,text,acfg()->txt_select,isbig);
  
    //-- Draw Focused Control
    ag_draw_ex(&d->control_focused,&win->c,0,0,x,y,w,h);
    //ag_rect(&d->control_focused, 0, 0, w, h, acfg()->selectbg);		
    ag_roundgrad(&d->control_focused,0,0,w,h,acfg()->selectbg,acfg()->selectbg, agdp());		
    ag_rect(&d->control_focused, 0, 1, w, 1, acfg()->menuline_bot);
    ag_rect(&d->control_focused, 0, 0, w, 1, acfg()->menuline_top);	
    ag_text(&d->control_focused,txtw,txtx,txty,text,acfg()->txt_select,isbig);
  }
 
  
  //-- Initializing Control
  ACONTROLP ctl  = malloc(sizeof(ACONTROL));
  ctl->ondestroy= &acbutton_ondestroy;
  ctl->oninput  = &acbutton_oninput;
  ctl->ondraw   = &acbutton_ondraw;
  ctl->onblur   = &acbutton_onblur;
  ctl->onfocus  = &acbutton_onfocus;
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
