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

#include <sched.h>
#include "../oppo_inter.h"

/***************************[ GLOBAL VARIABLES ]**************************/
static AC_CONFIG acfg_var;

/***************************[ CONFIG FUNCTIONS ]**************************/
AC_CONFIG * acfg(){ return &acfg_var; }
void acfg_reset_text(){
  snprintf(acfg_var.text_ok,64,"OK");
  snprintf(acfg_var.text_next,64,"Next >");
  snprintf(acfg_var.text_back,64,"< Back");
  snprintf(acfg_var.text_yes,64,"Yes");
  snprintf(acfg_var.text_no,64,"No");
  snprintf(acfg_var.text_about,64,"About");
  snprintf(acfg_var.text_calibrating,64,"Calibrating Tools");
  snprintf(acfg_var.text_quit,64,"Quit Installation");
  snprintf(acfg_var.text_quit_msg,128,"Are you sure to quit the installer?");
}
void acfg_init_ex(byte themeonly){
  acfg_var.winbg        = ag_rgb(0xf2,0xf2,0xf2);
  acfg_var.selectbg      = ag_rgb(0xee,0xee,0xee);
  acfg_var.txt_title     = ag_rgb(0xff,0xff,0xff);
  acfg_var.txt_menu   = ag_rgb(0xff,0xff,0xff);
  acfg_var.txt_select  = ag_rgb(0x88,0x88,0x88);
  acfg_var.txt_version   = ag_rgb(0xff,0xff,0xff);
  acfg_var.txt_pic_tit    = ag_rgb(0xcc,0xcc,0xcc);
  acfg_var.txt_pic_desc  = ag_rgb(0xaa,0xaa,0xaa);
  acfg_var.menuline_top    = ag_rgb(0x44,0x44,0x44);
  acfg_var.menuline_bot     = ag_rgb(158,228,32);
  acfg_var.title_line     = ag_rgb(0xff,0xff,0xff);
  acfg_var.border= ag_rgb(0x00,0x00,0x00);
  acfg_var.statusbg = ag_rgb(0x00,0x00,0x00);
  acfg_var.txt_status = ag_rgb(0xff,0xff,0xff);
  
  acfg_var.winroundsz   = 4;
  acfg_var.roundsz      = 3;
  acfg_var.btnroundsz   = 2;
  acfg_var.fadeframes   = 5;

  memset(acfg_var.themename, 0x00, 64);

  acfg_var.input_filter = 0;

  acfg_reset_text();

  acfg_var.ckey_up      = 0;
  acfg_var.ckey_down    = 0;
  acfg_var.ckey_select  = 0;
  acfg_var.ckey_back    = 0;
  acfg_var.ckey_menu    = 0;

  atheme_releaseall();
}
void acfg_init(){
  acfg_init_ex(0);
}

/***************************[ THEME ]**************************/
static char theme_name[OPPO_THEME_CNT][27]={
  "img.titlebg",
  "img.progress",
  "img.progress.fill",
  "img.list_bg",
  "img.list_bg.select",
  "img.list_line",
  "img.button.focus",
  "img.button",
};

void atheme_releaseall(){
  int i=0;
  for (i=0;i<OPPO_THEME_CNT;i++){
    if (acfg_var.theme[i]!=NULL){
      apng_close(acfg_var.theme[i]);
      free(acfg_var.theme[i]);
    }
    acfg_var.theme[i]   =NULL;
    acfg_var.theme_9p[i]=0;
  }
}
void atheme_release(char * key){
  int i=0;
  for (i=0;i<OPPO_THEME_CNT;i++){
    if (strcmp(theme_name[i],key)==0){
      if (acfg_var.theme[i]!=NULL){
        apng_close(acfg_var.theme[i]);
        free(acfg_var.theme[i]);
        acfg_var.theme[i]=NULL;
        acfg_var.theme_9p[i]=0;
      }
      return;
    }
  }
  return;
}
PNGCANVASP atheme_create(char * key, char * path){
  int id = atheme_id(key);
  oppo_debug("id=%d, path(%s)\n", id, path);
  if (id!=-1){
    PNGCANVAS * ap = malloc(sizeof(PNGCANVAS));
    if (apng_load(ap,path)){
      if (acfg_var.theme[id]!=NULL){
        apng_close(acfg_var.theme[id]);
        free(acfg_var.theme[id]);
        acfg_var.theme[id]=NULL;
        acfg_var.theme_9p[id]=0;
      }
      acfg_var.theme[id]  = ap;
      int ln = strlen(path)-1;
      acfg_var.theme_9p[id]=0;
      if (ln>2){
        if ((path[ln]=='9')&&(path[ln-1]=='.')){
          acfg_var.theme_9p[id]=1;
        }
      }
      return ap;
    }
    free(ap);
  }
  return NULL;
}
byte atheme_draw(char * key, CANVAS * _b, int x, int y, int w, int h){
  //oppo_debug("key(%s), x(%d), y(%d), w(%d),h(%d)\n\n", key, x, y, w, h);
  return atheme_id_draw(atheme_id(key),_b,x,y,w,h);
}
byte atheme_id_draw(int id, CANVAS * _b, int x, int y, int w, int h){
  if (id<0) return 0;
  if (id>=OPPO_THEME_CNT) return 0;

  if (acfg_var.theme[id]!=NULL){
    if (acfg_var.theme_9p[id]){
      return apng9_draw(_b,acfg_var.theme[id],x,y,w,h,NULL,1);
    }
    else{
      return apng_stretch(
        _b,
        acfg_var.theme[id],
        x,y,w,h,
        0,0,acfg_var.theme[id]->w,acfg_var.theme[id]->h);
    }
  }
  return 0;
}
PNGCANVASP atheme(char * key){
  int i=0;
  for (i=0;i<OPPO_THEME_CNT;i++){
    if (strcmp(theme_name[i],key)==0)
      return acfg_var.theme[i];
  }
  return NULL;
}
int atheme_id(char * key){
  int i=0;
  for (i=0;i<OPPO_THEME_CNT;i++){
    if (strcmp(theme_name[i],key)==0)
      return i;
  }
  return -1;
}
char * atheme_key(int id){
  if (id<0) return NULL;
  if (id>=OPPO_THEME_CNT) return NULL;
  return theme_name[id];
}


/***************************[ WINDOW FUNCTIONS ]**************************/
//-- CREATE WINDOW
AWINDOWP aw(CANVAS * bg){
  ag_setbusy();
  //sleep(4);
  //-- Create Window
  AWINDOWP win = (AWINDOWP) malloc(sizeof(AWINDOW));
  if (win==NULL) return NULL;
  
  //-- Create Canvas & Draw BG
  ag_canvas(&win->c,agw(),agh());
  ag_draw(&win->c,bg,0,0);
  
  //-- Initializing Variables
  win->bg           = bg;
  win->controls     = NULL;
  win->controln     = 0;
  win->threadnum    = 0;
  win->focusIndex   = -1;
  win->touchIndex   = -1;
  win->isActived    = 0;
  
  //-- RETURN
  return win;
}

//-- DESTROY WINDOW
void aw_destroy(AWINDOWP win){
  ag_setbusy();
  
  //-- Set To Unactive
  win->isActived = 0;
  
  //-- Wait Thread To Closed
  int threadwait_n=0;
  while (win->threadnum>0){
    usleep(500);
    if (threadwait_n++>1000) break;
  }
  
  //-- Cleanup Controls
  if (win->controln>0){
    int i;
    ACONTROLP * controls = (ACONTROLP *) win->controls;
    for (i=win->controln-1;i>=0;i--){
      controls[i]->ondestroy((void*) controls[i]);
      free(controls[i]);
    }
    free(win->controls);
  }
  
  //-- Cleanup Window
  ag_ccanvas(&win->c);
  free(win);
}

//-- Add Control Into Window
void aw_add(AWINDOWP win,ACONTROLP ctl){
  if (win->controln>0){
    int i;
    void ** tmpctls   = win->controls;
    win->controls     = malloc( sizeof(ACONTROLP)*(win->controln+1) );
    for (i=0;i<win->controln;i++)
      win->controls[i]=tmpctls[i];
    win->controls[win->controln] = (void*) ctl;
    free(tmpctls);
  }
  else{
    win->controls    = malloc(sizeof(ACONTROLP));
    win->controls[0] = (void*) ctl;
  }
  win->controln++;
}

//-- Draw Window
void aw_draw(AWINDOWP win){
  if (!win->isActived) return;
  ag_draw(NULL,&win->c,0,0);
  ag_sync();
}

//-- Redraw Window & Controls
void aw_redraw(AWINDOWP win){
  if (!win->isActived) return;
  if (win->controln>0){
    int i;
    for (i=0;i<win->controln;i++){
      ACONTROLP ctl = (ACONTROLP) win->controls[i];
      if (ctl->ondraw!=NULL)
        ctl->ondraw(ctl);
    }
  }
  ag_draw(NULL,&win->c,0,0);
}

extern int sd_menu_use;
//-- Show Window
void aw_show(AWINDOWP win){
  win->threadnum    = 0;
  win->isActived    = 1;
  //-- Find First Focus

/* HuangGuoDong@Drv.recovery, 2013/1/7, modify for set fouces manually in wipemenu and sdmenu*/
  if ((win->controln>0)	&& (win->focusIndex == -1)) {
    int i;
    for (i=0;i<win->controln;i++){
      ACONTROLP ctl = (ACONTROLP) win->controls[i];
      if (ctl->onfocus!=NULL){
        if (ctl->onfocus(ctl)){
          win->focusIndex = i;
          break;
        }
      }
    }
  }  

 	if (win->focusIndex && sd_menu_use) {
		int reqY;
		ACMENUDP d;
		ACONTROLP ctl = (ACONTROLP) win->controls[win->focusIndex];
		d = (ACMENUDP) ctl->d;
		reqY = d->items[d->focusedItem]->y - round((ctl->h/2) - (d->items[d->focusedItem]->h/2));
		ac_regscrollto(ctl, &d->scrollY, d->maxScrollY, reqY, &d->focusedItem, d->focusedItem);

 	}
	
  aw_redraw(win);
  ag_sync_fade(acfg_var.fadeframes);
}

//-- Post Message
void aw_post(dword msg){
  atouch_send_message(msg);
}

//-- Check Mouse Event
byte aw_touchoncontrol(ACONTROLP ctl, int x, int y){
  int wx  = ctl->x;
  int wx2 = wx+ctl->w;
  int wy  = ctl->y;
  int wy2 = wy+ctl->h;
  
  if ((x>=wx)&&(x<wx2)&&(y>=wy)&&(y<wy2))
    return 1;
  return 0;
}

//-- Set Focus
byte aw_setfocus(AWINDOWP win,ACONTROLP ctl){
  if (!win->isActived) return 0;
  int i;
  for (i=0;i<win->controln;i++){
    ACONTROLP fctl = (ACONTROLP) win->controls[i];
    if (fctl==ctl){
      if (fctl->onfocus!=NULL){
        if (fctl->onfocus(fctl)){
          int pf = win->focusIndex;
          win->focusIndex = i;
          if ((pf!=-1)&&(pf!=i)){
            ACONTROLP pctl = (ACONTROLP) win->controls[pf];
            pctl->onblur(pctl);
          }
          aw_draw(win);
          return 1;
        }
      }
    }
  }
  return 0;
}

/* HuangGuoDong@Drv.recovery, 2013/1/7, add for cycle selection*/
int action_global = 0;
extern int extra_move_enable;
extern int sd_menu_use;

//-- Dispatch Messages
dword aw_dispatch(AWINDOWP win){
	dword msg;
	int i;

	int reqY;
	ACMENUDP d;
  
	ui_clear_key_queue();
	while(1){
		int delayUs = 0;	
		//-- Wait For Event
		ATEV        atev;
		int action  =atouch_wait(&atev);
		//oppo_debug("action=%d,atev.d=%d\n", action, atev.d);
     
/* HuangGuoDong@Drv.recovery, 2013/1/7, add for cycle selection*/	
		action_global = action;

		//-- Reset Message Value
		msg = aw_msg(0,0,0,0);
    
		//-- Check an Action Value
		switch (action){
			case ATEV_MESSAGE:{
				msg = atev.msg;
			}
			break;
			case ATEV_BACK:{
				if (!atev.d){
					vibrate(30);
					msg = aw_msg(5,0,0,0);
				}
			}
			break;
			case ATEV_DOWN: case ATEV_RIGHT:
				if (!atev.d && (win->controln > 1 || extra_move_enable)){
					if (win->focusIndex!=-1){
						ACONTROLP ctl = (ACONTROLP) win->controls[win->focusIndex];
						if (ctl->oninput!=NULL){
							msg = ctl->oninput((void*)ctl,action,&atev);
						}
						/* HuangGuoDong@Drv.recovery, 2013/1/7, modify for cycle selection*/
						if (aw_gl(msg)==0){
							if (win->focusIndex < win->controln-1) {
								i = win->focusIndex + 1; 
							} else {
								i = 0;
							}		

							for (;i<win->controln;i++){
								ACONTROLP fctl = (ACONTROLP) win->controls[i];
								if (fctl->onfocus!=NULL){
									if (fctl->onfocus(fctl)){
										win->focusIndex = i;
										ctl->onblur(ctl);
										aw_draw(win);
										if (i && sd_menu_use) {
											d = (ACMENUDP) fctl->d;
											reqY = d->items[d->focusedItem]->y - round((fctl->h/2) - (d->items[d->focusedItem]->h/2));
											ac_regscrollto(fctl, &d->scrollY, d->maxScrollY, reqY, &d->focusedItem, d->focusedItem);
										}
										break;
									}
								}
							}
						}
						action_global = 0;		
					}
				}
				break;
			case ATEV_UP: case ATEV_LEFT:
				if (!atev.d && (win->controln > 1 || extra_move_enable)){
					if (win->focusIndex!=-1){
						ACONTROLP ctl = (ACONTROLP) win->controls[win->focusIndex];
						if (ctl->oninput!=NULL){
								msg = ctl->oninput((void*)ctl,action,&atev);
							}
						/* HuangGuoDong@Drv.recovery, 2013/1/7, modify for cycle selection*/
						if (aw_gl(msg)==0){
							if (win->focusIndex >= 1) {
								i = win->focusIndex -1; 
							} else {
								i = win->controln - 1;
							}	

							for (;i>=0;i--){
								ACONTROLP fctl = (ACONTROLP) win->controls[i];
								if (fctl->onfocus!=NULL){
									if (fctl->onfocus(fctl)){
										win->focusIndex = i;
										ctl->onblur(ctl);
										aw_draw(win);
										if (i && sd_menu_use) {
											d = (ACMENUDP) fctl->d;
											reqY = d->items[d->focusedItem]->y - round((ctl->h/2) - (d->items[d->focusedItem]->h/2));
											ac_regscrollto(fctl, &d->scrollY, d->maxScrollY, reqY, &d->focusedItem, d->focusedItem);
										}
										break;
									}
								}
							}
						}
						action_global = 0;		
					}
				}
				break;
			case ATEV_MENU:
			case ATEV_SEARCH:
			case ATEV_HOME:
			case ATEV_SELECT:{
				if (win->focusIndex!=-1){
					ACONTROLP ctl = (ACONTROLP) win->controls[win->focusIndex];
					if (ctl->oninput!=NULL){
						msg = ctl->oninput((void*)ctl,action,&atev);
					}
				}
			}
			break;
			case ATEV_MOUSEDN: {
				if (win->controln>0){
					int i;
					for (i=win->controln-1;i>=0;i--){
						ACONTROLP ctl = (ACONTROLP) win->controls[i];
						if (aw_touchoncontrol(ctl,atev.x,atev.y)){
							if (ctl->oninput!=NULL){
								msg = ctl->oninput((void*)ctl,action,&atev);
								win->touchIndex = i;
								delayUs = 200*1000;
								break;
							}
						}
					}
				}
			}
			break;
			case ATEV_MOUSEUP:{
				if (win->touchIndex!=-1){
					ACONTROLP ctl = (ACONTROLP) win->controls[win->touchIndex];
					if (ctl->oninput!=NULL)
					msg = ctl->oninput((void*)ctl,action,&atev);
					win->touchIndex = -1;
				}
			}
			break;
			case ATEV_MOUSEMV:{
				if (win->touchIndex!=-1){
					ACONTROLP ctl = (ACONTROLP) win->controls[win->touchIndex];
					if (ctl->oninput!=NULL)
					msg = ctl->oninput((void*)ctl,action,&atev);
				}
			}
			break;
		}
		if (aw_gd(msg)==1) {
			aw_draw(win);
			if (delayUs)
			usleep(delayUs);	  	
		}
		if (aw_gm(msg)!=0) return msg;
	}
	return msg;
}
CANVAS * aw_muteparent(AWINDOWP win){
  if (win==NULL){
    //-- Set Temporary
    CANVAS * tmpbg = (CANVAS *) malloc(sizeof(CANVAS));
    ag_canvas(tmpbg,agw(),agh());
    ag_draw(tmpbg,agc(),0,0);
    return tmpbg;
  }
  else{
    win->isActived = 0;
    return NULL;
  }
}
void aw_unmuteparent(AWINDOWP win,CANVAS * p){
  if (win==NULL){
    if (p!=NULL){
      ag_draw(NULL,p,0,0);
      ag_sync_fade(acfg_var.fadeframes);
      ag_ccanvas(p);
      free(p);
    }
  }
  else{
    win->isActived = 1;
    ag_draw(NULL,&win->c,0,0);
    ag_sync_fade(acfg_var.fadeframes);
  }
}

byte aw_confirm(AWINDOWP parent, char * titlev,char * textv,char * img,char * yes_text,char * no_text, int num_button){
  CANVAS * tmpc = aw_muteparent(parent);
/* HuangGuoDong@Drv.recovery, 2013/1/7, modify for change transparency of background*/  
  ag_rectopa(agc(),0,0,agw(),agh(),0x0000,80);
  
  char title[64];
  char text[512];
  snprintf(title,64,"%s",titlev);
  snprintf(text,512,"%s",textv);

  int pad   = LIST_LINE_OFFSET;
  int gap = pad*2; 
  
  int winW  = agw()-(pad*2);
  int lineW = winW-(pad*2);  
  int txtW  = lineW-(pad*2);  

  int titH  = ag_fontheight(1) + (pad*2);
  int txtH    = ag_txtheight(txtW,text,1);
  int infH    = txtH + gap*2;
  int btnH  =  ag_fontheight(1)+gap;
  
  int winH    = titH + infH + btnH;

  //-- Initializing Canvas
  CANVAS alertbg;
  ag_canvas(&alertbg,agw(),agh());
  ag_draw(&alertbg,agc(),0,0);

  //-- Draw Window
  int winX    = pad;
  int winY    = (agh()/2) - (winH/2);
  //ag_rect(&alertbg,winX,winY,winW,winH,acfg_var.border);
  //ag_rect(&alertbg,winX+1,winY+1,winW-2,winH-2,acfg_var.winbg);
  ag_roundgrad(&alertbg,winX,winY,winW,winH,acfg_var.winbg,acfg_var.winbg, agdp());
  
   //-- Draw Title
  int titW  = ag_txtwidth(title,1);    
  int titX    = (agw()/2) - (titW/2);
  int titY    = winY + pad;
  ag_text(&alertbg,titW,titX,titY,title,acfg_var.txt_select,1);

  //--Draw title line
  int lineX = winX +pad;
  int lineY = winY + titH - HEIGHT_TITLE_LINE;

  ag_rect(&alertbg,lineX,lineY,lineW,HEIGHT_TITLE_LINE,acfg_var.title_line);

 //-- Draw Text
  int txtY    = lineY + ((infH - txtH) / 2);
  int txtX  = lineX+pad;  

  ag_text(&alertbg,txtW,txtX,txtY,text,acfg_var.txt_menu,1);

  AWINDOWP hWin   = aw(&alertbg);

  if (num_button==2)	{
  int btnW1    = (winW)/2;
  int separationW = 3;
  int btnW2    = winW-btnW1-separationW;
  int btnY    = winY+winH-btnH;
  int btnX1    = winX;
  int separationX = btnX1+btnW1;
  int btnX2   = separationX+separationW;

  acbutton(hWin,btnX1,btnY,btnW1,btnH,(yes_text==NULL?acfg_var.text_yes:yes_text),1,6,2);
  acbutton(hWin,btnX2,btnY,btnW2,btnH,(no_text==NULL?acfg_var.text_no:no_text),1,5,2);
  
  //atheme_draw("img.list_bg", &hWin->c,separationX,btnY,separationW,btnH);
  ag_rect(&hWin->c, separationX, btnY, 1, btnH, acfg()->menuline_bot);	
  ag_rect(&hWin->c, separationX+1, btnY, 1, btnH, acfg()->menuline_top);
  ag_rect(&hWin->c, separationX+2, btnY, 1, btnH, acfg()->menuline_bot);	
  	}
  else {
  //-- Calculate Button Size & Position
  int btnW    = winW;
  int btnY    = winY+winH-btnH;
  int btnX    = winX;
  acbutton(hWin,btnX,btnY,btnW,btnH,(yes_text==NULL?acfg_var.text_yes:yes_text),1,6,2);
  } 
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
  aw_unmuteparent(parent,tmpc);
  return res;
}

byte aw_notice(AWINDOWP parent, char * titlev,char * textv,char * img,char * yes_text,char * no_text){
  CANVAS * tmpc = aw_muteparent(parent);

  ag_rectopa(agc(),0,0,agw(),agh(),0x0000,80);
  
  char title[64];
  char text[512];
  snprintf(title,64,"%s",titlev);
  snprintf(text,512,"%s",textv);

  int pad   = agdp()*4;
  int gap = agdp()*8; 
  
  int winW  = agw()-(pad*2);
  int txtW  = winW-(gap*2);  

  int titH  = ag_fontheight(1) + (pad*2);
  int txtH    = ag_txtheight(txtW,text,1);
  int infH    = txtH + gap*4;
  int btnH  =  ag_fontheight(1)+gap*2;
  
  int winH    = titH + infH + btnH;

  //-- Initializing Canvas
  CANVAS alertbg;
  ag_canvas(&alertbg,agw(),agh());
  ag_draw(&alertbg,agc(),0,0);

  //-- Draw Window
  int winX    = pad;
  int winY    = (agh()/2) - (winH/2);
  ag_rect(&alertbg,winX,winY,winW,winH,acfg_var.border);
  ag_rect(&alertbg,winX+1,winY+1,winW-2,winH-2,acfg_var.winbg);

  
  //-- Calculate Title Size & Position
  int titW  = ag_txtwidth(title,1);    
  int titX    = (agw()/2) - (titW/2);
  int titY    = winY + pad;
  //-- Draw Title
  ag_text(&alertbg,titW,titX,titY,title,acfg_var.txt_title,1);

  //--Draw line
  int lineW = txtW;  
  int lineX = winX +gap;
  int lineY = winY + titH;

  ag_rect(&alertbg,lineX,lineY,lineW,1,acfg_var.border);

 //-- Draw Text
  int txtY    = lineY + ((infH - txtH) / 2);
  int txtX  = lineX;  

  ag_text(&alertbg,txtW,txtX,txtY,text,acfg_var.txt_menu,1);

  AWINDOWP hWin   = aw(&alertbg);

  //-- Calculate Button Size & Position
  int btnW    = winW;
  int btnY    = winY+winH-btnH;
  int btnX    = winX;
  acbutton(hWin,btnX,btnY,btnW,btnH,(yes_text==NULL?acfg_var.text_yes:yes_text),1,6,2);
      
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
  aw_unmuteparent(parent,tmpc);
  return res;
}
