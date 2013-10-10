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

/***************************[ MENU BOX ]**************************/
typedef struct{
  char title[64];
  char desc[128];
  PNGCANVAS * img;
  int  id;
  int  h;
  int  y;
  
  /* Title & Desc Size/Pos */
  int  th;
} ACMAINMENUI, * ACMAINMENUIP;
typedef struct{
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
  ACMAINMENUIP * items;
  int       itemn;
  int       touchedItem;
  int       focusedItem;
  int       draweditemn;
  int       selectedIndex;
  byte      touchmsg;
  /* Focus */
  byte      focused;
} ACMAINMENUD, * ACMAINMENUDP;
static void control_item_dump(ACMAINMENUD * d)
{
    oppo_debug("**************ACMAINMENUD***************\n");
    oppo_debug("itemn:%d,touchedItem:%d,focusedItem:%d,\n", d->itemn, d->touchedItem, d->focusedItem);
    oppo_debug("drawedItemn:%d,selectIndex:%d,touchmsg:%x\n,", d->draweditemn, d->selectedIndex, d->touchmsg);
    oppo_debug("******************END*******************\n");
}
void acsdmenu_ondestroy(void * x){
  ACONTROLP ctl= (ACONTROLP) x;
  ACMAINMENUDP d  = (ACMAINMENUDP) ctl->d;
  ag_ccanvas(&d->control);
  ag_ccanvas(&d->control_focused);
  if (d->itemn>0){
    int i;
    for (i=0;i<d->itemn;i++){
      if (d->items[i]->img!=NULL){
        apng_close(d->items[i]->img);
        free(d->items[i]->img);
        d->items[i]->img=NULL;
      }
      free(d->items[i]);
    }
    free(d->items);
    ag_ccanvas(&d->client);
  }
  free(ctl->d);
}

void acsdmenu_redrawitem(ACONTROLP ctl, int index){
  ACMAINMENUDP d = (ACMAINMENUDP) ctl->d;
  if (d->acheck_signature != 144) return;   //-- Not Valid Signature
  if ((index>=d->itemn)||(index<0)) return; //-- Not Valid Index
  
  ACMAINMENUIP p = d->items[index];
  CANVAS *  c = &d->client;
  
  //-- Cleanup Background
  ag_rect(c,0,p->y,d->clientWidth,p->h,acfg()->textbg);
  color txtcolor = acfg()->textfg;
  color graycolor= acfg()->textfg_gray;
  byte isselectcolor=0;

  /* HuangGuoDong@Drv.recovery, 2013/1/7, modify for draw the background of selected item */
   if ((index==d->touchedItem) ||
  	((index==d->focusedItem)&&(d->focused))) {
	//ag_rect(c,0,p->y+agdp(),d->clientWidth,p->h-1-(agdp()*2),acfg()->selectbg);
	ag_rect(c,0,p->y,d->clientWidth,p->h,acfg()->selectbg);
  }
	
  /* HuangGuoDong@Drv.recovery, 2013/1/7, modify for draw the last black line of items */
  if (index<d->itemn){
    ag_rect(c,0,p->y+p->h-1,d->clientWidth,1,acfg()->textfg);
  }

  //-- Now Draw The Checkbox
  int imgS =  p->h;
  if (p->img!=NULL){
    int imgW = p->img->w;
    int imgH = p->img->h;   

/* HuangGuoDong@Drv.recovery, 2013/1/21, add for scaling image*/
    if (imgH < ag_fontheight(1))  imgH = ag_fontheight(1);
    if (imgH>imgS) imgH=imgS;	
    imgW=imgH*p->img->w/p->img->h;
    int imgX = round((imgS-imgW)/2);
    int imgY = round((imgS-imgH)/2);
    apng_stretch(c, p->img, imgX ,p->y+imgY, imgW, imgH, 0, 0, p->img->w, p->img->h);
  }
   
  int txtH    = p->th;
  int txtAddY = round((p->h-txtH)/2);

  //-- Now Draw The Text
  ag_text(c,d->clientTextW,d->clientTextX,(p->y+txtAddY),p->title,txtcolor,1);

}

void acsdmenu_redraw(ACONTROLP ctl){
  ACMAINMENUDP d = (ACMAINMENUDP) ctl->d;
  if (d->acheck_signature != 144) return; //-- Not Valid Signature
  if ((d->itemn>0)&&(d->draweditemn<d->itemn)) {
    ag_ccanvas(&d->client);
    ag_canvas(&d->client,d->clientWidth,d->nextY);
    ag_rect(&d->client,0,0,d->clientWidth,agdp()*max(acfg()->roundsz,4),acfg()->textbg);
    
    //-- Set Values
    d->scrollY     = 0;
    d->maxScrollY  = d->nextY-(ctl->h-(agdp()*max(acfg()->roundsz,4)));
    if (d->maxScrollY<0) d->maxScrollY=0;
    
    //-- Draw Items
    int i;
    for (i=0;i<d->itemn;i++){
      acsdmenu_redrawitem(ctl,i);
    }
    d->draweditemn=d->itemn;
  }
  
}
int acsdmenu_getselectedindex(ACONTROLP ctl){
  ACMAINMENUDP d = (ACMAINMENUDP) ctl->d;
  if (d->acheck_signature != 144) return -1; //-- Not Valid Signature
  return d->selectedIndex < 0 ?0:d->selectedIndex;
}
//-- Add Item Into Control
byte acsdmenu_add(ACONTROLP ctl,char * title, char * desc, char * img){
  ACMAINMENUDP d = (ACMAINMENUDP) ctl->d;
  if (d->acheck_signature != 144) return 0; //-- Not Valid Signature
  
  //-- Allocating Memory For Item Data
  ACMAINMENUIP newip = (ACMAINMENUIP) malloc(sizeof(ACMAINMENUI));
  snprintf(newip->title,64,"%s",title);
  snprintf(newip->desc,128,"%s",desc);
  
  //-- Load Image
  newip->img      = (PNGCANVAS *) malloc(sizeof(PNGCANVAS));
  memset(newip->img,0,sizeof(PNGCANVAS));
  if (!apng_load(newip->img,img)){
    free(newip->img);
    newip->img=NULL;
  }

  newip->th       = ag_txtheight(d->clientTextW - d->clientTextX,newip->title,1);//calcu text's y address more accurately
  newip->h        = (agdp()*4)  + newip->th;
  if (newip->h<(agdp()*26)) newip->h = (agdp()*26);
  newip->id       = d->itemn;
  newip->y        = d->nextY;
  d->nextY       += newip->h;
  
  if (d->itemn>0){
    int i;
    ACMAINMENUIP * tmpitms   = d->items;
    d->items              = malloc( sizeof(ACMAINMENUIP)*(d->itemn+1) );
    for (i=0;i<d->itemn;i++)
      d->items[i]=tmpitms[i];
    d->items[d->itemn] = newip;
    free(tmpitms);
  }
  else{
    d->items    = malloc(sizeof(ACMAINMENUIP));
    d->items[0] = newip;
  }
  d->itemn++;
  return 1;
}
void acsdmenu_ondraw(void * x){
  ACONTROLP   ctl= (ACONTROLP) x;
  ACMAINMENUDP   d  = (ACMAINMENUDP) ctl->d;
  CANVAS *    pc = &ctl->win->c;
  
  acsdmenu_redraw(ctl);

  if (d->invalidDrawItem!=-1){
    d->touchedItem = d->invalidDrawItem;
    acsdmenu_redrawitem(ctl,d->invalidDrawItem);
    d->invalidDrawItem=-1;
  }

  //-- Init Device Pixel Size
  int minpadding = max(acfg()->roundsz,4);
  int agdp3 = (agdp()*minpadding);
  int agdp6 = (agdp()*(minpadding*2));
  int agdpX = agdp6;

  if (d->focused){
    ag_draw(pc,&d->control_focused,ctl->x,ctl->y);
/* HuangGuoDong@Drv.recovery, 2013/1/7, modify for cleanup the blank on the left and right edge of screen */
    ag_draw_ex(pc,&d->client,0,ctl->y,0,d->scrollY,ctl->w,ctl->h);
  }
  else{
    ag_draw(pc,&d->control,ctl->x,ctl->y);
/* HuangGuoDong@Drv.recovery, 2013/1/7, modify for cleanup the blank on the left and right edge of screen */
    ag_draw_ex(pc,&d->client,0,ctl->y,0,d->scrollY,ctl->w,ctl->h);
  }
  
  if (d->maxScrollY>0){
    //-- Scrollbar
    int newh = ctl->h - agdp6;
    float scrdif    = ((float) newh) / ((float) d->client.h);
    int  scrollbarH = round(scrdif * newh);
    int  scrollbarY = round(scrdif * d->scrollY) + agdp3;
    if (d->scrollY<0){
      scrollbarY = agdp3;
      int alp = (1.0 - (((float) abs(d->scrollY)) / (((float) ctl->h)/4))) * 255;
      if (alp<0) alp = 0;
      ag_rectopa(pc,(ctl->w-agdp()-2)+ctl->x,scrollbarY+ctl->y,agdp(),scrollbarH,acfg()->scrollbar, alp);
    }
    else if (d->scrollY>d->maxScrollY){
      scrollbarY = round(scrdif * d->maxScrollY) + agdp3;
      int alp = (1.0 - (((float) abs(d->scrollY-d->maxScrollY)) / (((float) ctl->h)/4))) * 255;
      if (alp<0) alp = 0;
      ag_rectopa(pc,(ctl->w-agdp()-2)+ctl->x,scrollbarY+ctl->y,agdp(),scrollbarH,acfg()->scrollbar, alp);
    }
    else{
      ag_rect(pc,(ctl->w-agdp()-2)+ctl->x,scrollbarY+ctl->y,agdp(),scrollbarH,acfg()->scrollbar);
    }
  }

}
dword acsdmenu_oninput(void * x,int action,ATEV * atev){
  ACONTROLP ctl= (ACONTROLP) x;
  ACMAINMENUDP d  = (ACMAINMENUDP) ctl->d;

/* HuangGuoDong@Drv.recovery, 2013/1/7, add for cycle selection*/  
  AWINDOWP win = ctl->win;

  //oppo_debug("action=%d, d->prevTouchY=%d, atev->y=%d, agdp()*5=%d\n", action, d->prevTouchY, atev->y, agdp()*5);	
  dword msg = 0;
  switch (action){
    case ATEV_MOUSEDN:
      {
        d->prevTouchY  = atev->y;
        akinetic_downhandler(&d->akin,atev->y);
        
        int touchpos = atev->y - ctl->y + d->scrollY;
        int i;
        for (i=0;i<d->itemn;i++){
          if ((touchpos>=d->items[i]->y)&&(touchpos<d->items[i]->y+d->items[i]->h)){
            ac_regpushwait(
              ctl,&d->prevTouchY,&d->invalidDrawItem,i
            );
            break;
          }
        }
      }
      break;
    case ATEV_MOUSEUP:
      {
        byte retmsgx = 0;
        if ((d->prevTouchY!=-50)&&(abs(d->prevTouchY-atev->y)<agdp()*5)){
          d->prevTouchY=-50;
          int touchpos = atev->y - ctl->y + d->scrollY;
          int i;
#ifdef DEBUG
          control_item_dump(d);
#endif
          for (i=0;i<d->itemn;i++){
            if ((touchpos>=d->items[i]->y)&&(touchpos<d->items[i]->y+d->items[i]->h)){
              if ((d->touchedItem != -1)&&(d->touchedItem!=i)){
                int tmptouch=d->touchedItem;
                d->touchedItem = -1;
                acsdmenu_redrawitem(ctl,tmptouch);
              }
              if ((d->selectedIndex != -1)&&(d->selectedIndex!=i)){
                int tmpsidx=d->selectedIndex;
                d->selectedIndex = -1;
                acsdmenu_redrawitem(ctl,tmpsidx);
              }
              
              int prevfocus = d->focusedItem;
              d->focusedItem= i;
              d->touchedItem   = i;
              d->selectedIndex = i;
              if ((prevfocus!=-1)&&(prevfocus!=i)){
                acsdmenu_redrawitem(ctl,prevfocus);
              }
              
              acsdmenu_redrawitem(ctl,i);
              ctl->ondraw(ctl);
              aw_draw(ctl->win);
              vibrate(30);
              retmsgx = d->touchmsg;
              msg=aw_msg(retmsgx,1,0,0);
              break;
            }
          }
          if ((d->scrollY<0)||(d->scrollY>d->maxScrollY)){
            ac_regbounce(ctl,&d->scrollY,d->maxScrollY);
          }
        }
        else{
          if (akinetic_uphandler(&d->akin,atev->y)){
            ac_regfling(ctl,&d->akin,&d->scrollY,d->maxScrollY);
          }
          else if ((d->scrollY<0)||(d->scrollY>d->maxScrollY)){
            ac_regbounce(ctl,&d->scrollY,d->maxScrollY);
          }
        }
#if 1
        if (d->touchedItem != -1){
          usleep(30);
          int tmptouch=d->touchedItem;
          d->touchedItem = -1;
          acsdmenu_redrawitem(ctl,tmptouch);
          ctl->ondraw(ctl);
          msg=aw_msg(retmsgx,1,0,0);
        }
#endif
      }
      break;
    case ATEV_MOUSEMV:
      {
        byte allowscroll=1;
        if (atev->y!=0){
          if (d->prevTouchY!=-50){
            if (abs(d->prevTouchY-atev->y)>=agdp()*5){
              d->prevTouchY=-50;
              if (d->touchedItem != -1){
                int tmptouch=d->touchedItem;
                d->touchedItem = -1;
                acsdmenu_redrawitem(ctl,tmptouch);
                ctl->ondraw(ctl);
                aw_draw(ctl->win);
              }
            }
            else
              allowscroll=0;
          }
          if (allowscroll){
            int mv = akinetic_movehandler(&d->akin,atev->y);
            if (mv!=0){
              if ((d->scrollY<0)&&(mv<0)){
                float dumpsz = 0.6-(0.6*(((float) abs(d->scrollY))/(ctl->h/4)));
                d->scrollY+=floor(mv*dumpsz);
              }
              else if ((d->scrollY>d->maxScrollY)&&(mv>0)){
                float dumpsz = 0.6-(0.6*(((float) abs(d->scrollY-d->maxScrollY))/(ctl->h/4)));
                d->scrollY+=floor(mv*dumpsz);
              }
              else
                d->scrollY+=mv;
  
              if (d->scrollY<0-(ctl->h/4)) d->scrollY=0-(ctl->h/4);
              if (d->scrollY>d->maxScrollY+(ctl->h/4)) d->scrollY=d->maxScrollY+(ctl->h/4);
              msg=aw_msg(0,1,0,0);
              ctl->ondraw(ctl);
            }
          }
        }
      }
      break;
      case ATEV_SELECT:
      {
        if ((d->focusedItem>-1)&&(d->draweditemn>0)){
          if (atev->d){
            if ((d->touchedItem != -1)&&(d->touchedItem!=d->focusedItem)){
              int tmptouch=d->touchedItem;
              d->touchedItem = -1;
              acsdmenu_redrawitem(ctl,tmptouch);
            }
            vibrate(30);
            d->touchedItem=d->focusedItem;
            acsdmenu_redrawitem(ctl,d->focusedItem);
            ctl->ondraw(ctl);
            msg=aw_msg(0,1,0,0);
          }
          else{
            if ((d->touchedItem != -1)&&(d->touchedItem!=d->focusedItem)){
              int tmptouch=d->touchedItem;
              d->touchedItem = -1;
              acsdmenu_redrawitem(ctl,tmptouch);
            }
            if ((d->selectedIndex != -1)&&(d->selectedIndex!=d->focusedItem)){
              int tmpsidx=d->selectedIndex;
              d->selectedIndex = -1;
              acsdmenu_redrawitem(ctl,tmpsidx);
            }
            d->selectedIndex = d->focusedItem;
            d->touchedItem=-1;
            acsdmenu_redrawitem(ctl,d->focusedItem);
            ctl->ondraw(ctl);
            // msg=aw_msg(0,1,0,0);
            msg=aw_msg(d->touchmsg,1,0,0);
          }
        }
      }
      break;
      case ATEV_DOWN:
        {
/* HuangGuoDong@Drv.recovery, 2013/1/7, modifyfor cycle selection*/	
          if (win->controln>1) {
            if (d->focusedItem>=d->itemn-1) {
              break; 
	     }
	   }
		  
	   if (d->draweditemn>0) {
	     int prevfocus = d->focusedItem;
            if (d->focusedItem<d->itemn-1)
		d->focusedItem++;
	     else
		 d->focusedItem = 0;	

	     acsdmenu_redrawitem(ctl,prevfocus);
            acsdmenu_redrawitem(ctl,d->focusedItem);
            ctl->ondraw(ctl);
            msg=aw_msg(0,1,1,0);
            
            int reqY = d->items[d->focusedItem]->y - round((ctl->h/2) - (d->items[d->focusedItem]->h/2));
            ac_regscrollto(
              ctl,
              &d->scrollY,
              d->maxScrollY,
              reqY,
              &d->focusedItem,
              d->focusedItem
            );	 
	   }		
        }
      break;
      case ATEV_UP:
        {
/* HuangGuoDong@Drv.recovery, 2013/1/7, add for cycle selection*/
          if (win->controln>1) {
            if (d->focusedItem<=0)
	       break;
	   }
		  
	   if (d->draweditemn>0) {
            int prevfocus = d->focusedItem;
            if (d->focusedItem>0)
		d->focusedItem--;
	     else
		 d->focusedItem = d->draweditemn-1;	

	     acsdmenu_redrawitem(ctl,prevfocus);
            acsdmenu_redrawitem(ctl,d->focusedItem);
            ctl->ondraw(ctl);
            msg=aw_msg(0,1,1,0);
            
            int reqY = d->items[d->focusedItem]->y - round((ctl->h/2) - (d->items[d->focusedItem]->h/2));
            ac_regscrollto(
              ctl,
              &d->scrollY,
              d->maxScrollY,
              reqY,
              &d->focusedItem,
              d->focusedItem
            );	 
	   }
        }
      break;
  }
  return msg;
}

/* HuangGuoDong@Drv.recovery, 2013/1/7, add for cycle selection*/
extern int action_global;
extern int selectedCount;
extern int selectedItem[1024];

byte acsdmenu_onfocus(void * x){
  ACONTROLP   ctl= (ACONTROLP) x;
  ACMAINMENUDP   d  = (ACMAINMENUDP) ctl->d;
  
  d->focused=1;
  
  if ((d->focusedItem==-1)&&(d->itemn>0)){
    d->focusedItem=(selectedItem[selectedCount] > 0)?selectedItem[selectedCount]:0;
  }

  if(action_global == 1) {//ATEV_DOWN
    d->focusedItem=0;
  } else if (action_global == 2) {//ATEV_UP
    d->focusedItem=d->itemn-1;
  }

  if ((d->focusedItem!=-1)&&(d->draweditemn>0)){
    acsdmenu_redrawitem(ctl,d->focusedItem);
  }

  ctl->ondraw(ctl);
  return 1;
}
void acsdmenu_onblur(void * x){
  ACONTROLP   ctl= (ACONTROLP) x;
  ACMAINMENUDP   d  = (ACMAINMENUDP) ctl->d;
  d->focused=0;
  if ((d->focusedItem!=-1)&&(d->draweditemn>0)){
    acsdmenu_redrawitem(ctl,d->focusedItem);
  }
  ctl->ondraw(ctl);
}
ACONTROLP acsdmenu(
  AWINDOWP win,
  int x,
  int y,
  int w,
  int h,
  byte touchmsg
){

  //-- Validate Minimum Size
  if (h<agdp()*16) h=agdp()*16;
  if (w<agdp()*20) w=agdp()*20;

  //-- Initializing Text Data
  ACMAINMENUDP d        = (ACMAINMENUDP) malloc(sizeof(ACMAINMENUD));
  memset(d,0,sizeof(ACMAINMENUD));
  
  //-- Set Signature
  d->acheck_signature = 144;
  
  //-- Initializing Canvas
  ag_canvas(&d->control,w,h);
  ag_canvas(&d->control_focused,w,h);
  
  int minpadding = max(acfg()->roundsz,4);
  
  //-- Initializing Client Size
  d->clientWidth  = w;
  d->clientTextW  = d->clientWidth - ((agdp()*34) + (agdp()*acfg()->btnroundsz*2));
  d->clientTextX  = (agdp()*31) + (agdp()*acfg()->btnroundsz*2);
  d->client.data=NULL;
  
  //-- Draw Control
  ag_draw_ex(&d->control,&win->c,0,0,x,y,w,h);
  ag_rect(&d->control,0,0,w,h,acfg()->textbg);
  
  //-- Draw Focused Control
  ag_draw_ex(&d->control_focused,&win->c,0,0,x,y,w,h);
  ag_rect(&d->control_focused,0,0,w,h,acfg()->textbg);
  
  //-- Set Scroll Value
  d->scrollY     = 0;
  d->maxScrollY  = 0;
  d->prevTouchY  =-50;
  d->invalidDrawItem = -1;
  //-- Set Data Values
  d->items       = NULL;
  d->itemn       = 0;
  d->touchedItem = -1;
  d->focusedItem = -1;
  d->nextY       = 0;
  d->draweditemn = 0;
  d->selectedIndex = -1;
  d->touchmsg    = touchmsg;
  
  ACONTROLP ctl  = malloc(sizeof(ACONTROL));
  ctl->ondestroy= &acsdmenu_ondestroy;
  ctl->oninput  = &acsdmenu_oninput;
  ctl->ondraw   = &acsdmenu_ondraw;
  ctl->onblur   = &acsdmenu_onblur;
  ctl->onfocus  = &acsdmenu_onfocus;
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
