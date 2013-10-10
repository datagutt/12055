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
#include "../oppo.h"


struct _menuUnit *g_main_menu;//main menu
struct _menuUnit *g_root_menu;//language ui

/* OPPO 2013-01-17 jizhengkang azx Add begin for reason */
extern int default_language;
/* OPPO 2013-01-17 jizhengkang azx Add end */

static STATUS main_ui_clear(struct _menuUnit *p)
{
    //release tree, post order release
    if (p == NULL)
        return RET_OK;
    main_ui_clear(p->child);
    main_ui_clear(p->nextSilbing);
    free(p);
    return RET_OK;
}

static STATUS main_ui_clear_root()
{
    return main_ui_clear(g_root_menu);
}
static struct _menuUnit *tree_init()
{
    oppo_debug("tree_init entery\n");
    g_root_menu = lang_ui_init();
    return_null_if_fail(g_root_menu != NULL);
    //main menu
    g_main_menu = common_ui_init();
    return_null_if_fail(g_main_menu != NULL);
    strncpy(g_main_menu->name, "<~mainmenu.name>", MENU_LEN);
    menuUnit_set_show(g_main_menu, &common_ui_show);
    g_root_menu->child = g_main_menu;
    g_main_menu->parent = g_root_menu;
    //add back operation
    g_main_menu = menuNode_init(g_main_menu);
    //inital mainmenu 

    //add sd operation 
    assert_if_fail(menuNode_add(g_main_menu, sd_ui_init()) == RET_OK);
    //add wipe
    assert_if_fail(menuNode_add(g_main_menu, wipe_ui_init()) == RET_OK);
    //add usb mass storage
#ifndef  XIAOMI_M2    
    assert_if_fail(menuNode_add(g_main_menu, ums_ui_init()) == RET_OK);	
#endif
    //add reboot
    assert_if_fail(menuNode_add(g_main_menu, reboot_ui_init()) == RET_OK);
    //add power off
    assert_if_fail(menuNode_add(g_main_menu, poweroff_ui_init()) == RET_OK);		
    //add power
    //assert_if_fail(menuNode_add(g_main_menu, power_ui_init()) == RET_OK);	

    return g_root_menu;
}
STATUS main_ui_init()
{
    oppo_printf("Initializing...\n");
    remove_directory(OPPO_TMP);
    unlink(OPPO_TMP_S);
    create_directory(OPPO_TMP);
    symlink(OPPO_TMP, OPPO_TMP_S);

    //oppo  config init
    oppo_ui_init();
    //read config file and execute it
    oppo_ui_config("/res/init.conf");
    //input thread start
    ui_init();
    //graphic thread start, print background
    ag_init();

    //oppo_ui start
    oppo_ui_start();
	//device config after oppo_ui start
    oppo_ui_config("/res/device.conf");
    tree_init();

    oppo_font( "0", "ttf/DroidSansFallback.ttf", "10" );
    oppo_font( "1", "ttf/DroidSansFallback.ttf", "15" );

    oppo_debug("ag_fontheight(1)=%d, ag_fontheight(0)=%d\n", ag_fontheight(1), ag_fontheight(0));	
/* OPPO 2013-01-17 jizhengkang azx Modify begin for reason */
#if 0
    oppo_loadlang("langs/cn.lang");	
#else
	if (default_language == 3)
		oppo_loadlang("langs/en.lang");
	else if (default_language == 2)
		oppo_loadlang("langs/cn_trad.lang");
	else
		oppo_loadlang("langs/cn.lang");
#endif
/* OPPO 2013-01-17 jizhengkang azx Modify end */
    return RET_OK;
}
STATUS main_ui_show()
{
    struct _menuUnit *node_show = g_root_menu;
    int index = 0;
    //show mainmenu
    while (index != MENU_QUIT)
    {
        return_val_if_fail(node_show != NULL, RET_FAIL);
        return_val_if_fail(node_show->show != NULL, RET_FAIL);
        oppo_set_isbgredraw(1);
        index = node_show->show(node_show);
		oppo_debug("index=%d\n", index);
        if (index > 0 && index < MENU_BACK) {
            node_show = node_show->get_child_by_index(node_show, index);
        }
        else if (index == MENU_BACK || index == 0 )
        {
            if (node_show->parent != NULL)
                node_show = node_show->parent;
        }
        else {
            //TODO add MENU QUIT or some operation?
            oppo_error("invalid index %d in %s\n", index, __FUNCTION__);
        }
    }
    return RET_FAIL;
}

STATUS main_ui_release()
{

  oppo_ui_end();
  ag_close_thread();
  //clear ui tree
  main_ui_clear_root(); 
  //-- Release All
  ag_closefonts();  //-- Release Fonts
  oppo_debug("Font Released\n");
  ev_exit();        //-- Release Input Engine
  oppo_debug("Input Released\n");
  ag_close();       //-- Release Graph Engine
  oppo_debug("Graph Released\n");

  oppo_debug("Cleanup Temporary\n");
  usleep(500000);
  unlink(OPPO_TMP_S);
  remove_directory(OPPO_TMP);
  oppo_debug("end...\n");
  return RET_OK;
}

