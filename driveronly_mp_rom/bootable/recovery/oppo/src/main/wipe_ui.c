#include "../oppo_inter.h"
#include "../oppo.h"
#include "../../../oppo_intent.h"
/* OPPO 2013-01-17 jizhengkang Add begin for reason */
#include "../../../common.h"
#include "../../../bootloader.h"
extern int selectedCount;
extern int selectedItem[1024];
/* OPPO 2013-01-17 jizhengkang Add end */

#define WIPE_FACTORY     1
#define WIPE_DATA        2
#define WIPE_CACHE       3
#define FORMAT_SYSTEM    4
#define FORMAT_DATA      5
#define FORMAT_CACHE     6
#define FORMAT_BOOT      7
#define FORMAT_SDCARD    8
#define FORMAT_ALL       9

 CANVAS canvas_wipemenu;
enum {
	WIPE_ALL_OK,
	WIPE_ALL_FAILED,
	WIPE_CACHE_OK,
	WIPE_CACHE_FAILED,
	WIPE_DATA_OK,
	WIPE_DATA_FAILED,
	WIPE_IGNORE,
};
extern CANVAS canvas_wipemenu;
extern CANVAS canvas_mainmenu;
STATUS wipe_result_confirm(menuUnit *p, int wipe_result)
{
    char name[MENU_LEN], desc[MENU_LEN];

    oppo_debug("start .....\n");
//    strncpy(name, "<~wipe.result.title>", MENU_LEN);
    switch(wipe_result) {
        case WIPE_ALL_OK:
            strncpy(desc, "<~wipe.result.factory.ok>", MENU_LEN);
	     break;					
	 case WIPE_ALL_FAILED:
            strncpy(desc, "<~wipe.result.factory.failed>", MENU_LEN);
	     break;					
	 case WIPE_CACHE_OK:
            strncpy(desc, "<~wipe.result.cache.ok>", MENU_LEN);	
	     break;					
	 case WIPE_CACHE_FAILED:
            strncpy(desc, "<~wipe.result.cache.failed>", MENU_LEN);
	     break;					
	 case WIPE_DATA_OK:
            strncpy(desc, "<~wipe.result.data.ok>", MENU_LEN);		
	     break;					
	 case WIPE_DATA_FAILED:
            strncpy(desc, "<~wipe.result.data.failed>", MENU_LEN);	
	     break;		
        default:
            assert_if_fail(0);
            break;		 
    }
//    ag_draw(NULL,&canvas_wipemenu,0,0);
	ag_draw(NULL,&canvas_mainmenu,0,0);
    oppo_notice(3, p->title_name, desc, p->icon);
    oppo_debug("end .....\n");
    return MENU_BACK;
}

static int oppo_wipe_items(int item)
{
    intentResult*result1;
	int ret1;
	int ret2;
    int wipe_result = WIPE_IGNORE;
	
	switch(item) {
        case WIPE_FACTORY:
			oppo_debug("WIPE_FACTORY\n");	

			wipe_thread_start("<~wipe.factory.name>");
			set_sdcard_update_bootloader_message("--wipe_data", NULL);
			LOGW("Erase data and cache write bcb...\n");
			result1 = oppoIntent_send(INTENT_WIPE, 1, "/data");
			ret1 = result1->ret;
			set_wipe_process(50);
			result1 = oppoIntent_send(INTENT_WIPE, 1, "/cache");
			ret2 = result1->ret;
			oppo_debug("result1=%d, result2=%d\n", ret1, ret2);

			if ((ret1 == 0) && (ret2 == 0)) {
				wipe_result = WIPE_ALL_OK;
			} else {
				wipe_result = WIPE_ALL_FAILED;
			}
			break;
		case WIPE_DATA:
			oppo_debug("WIPE_DATA\n");
			wipe_thread_start("<~wipe.data.name>");
			set_sdcard_update_bootloader_message("--wipe_data_only", NULL);
			LOGW("Erase data write bcb...\n");
			result1 = oppoIntent_send(INTENT_WIPE, 1, "/data");
			if (result1->ret == 0) {
				wipe_result = WIPE_DATA_OK;
			} else {
				wipe_result = WIPE_DATA_FAILED;
			}
			break;
		case WIPE_CACHE:
			oppo_debug("WIPE_CACHE\n");
			wipe_thread_start("<~wipe.cache.name>");
			set_sdcard_update_bootloader_message("--wipe_cache", NULL);
			LOGW("Erase cache write bcb...\n");
			result1 = oppoIntent_send(INTENT_WIPE, 1, "/cache");
			if (result1->ret == 0) {
				wipe_result = WIPE_CACHE_OK;
			} else {
				wipe_result = WIPE_CACHE_FAILED;
			}		
			break;
		default:
			assert_if_fail(0);
			break;
	}
	set_wipe_process(100);
	wipe_thread_close();
	return wipe_result;   
}

int oppo_wipe_factory()
{
	int ret = 0;
    if (oppo_wipe_items(WIPE_FACTORY) == WIPE_ALL_FAILED)	
        ret = -1;
    oppo_debug("end... ret=%d\n", ret);	
    return ret;	
}

int oppo_wipe_data()
{
    int ret = 0;
    if (oppo_wipe_items(WIPE_DATA) == WIPE_DATA_FAILED)	
        ret = -1;
    oppo_debug("end... ret=%d\n", ret);	
    return ret;	
}

int oppo_wipe_cache()
{
    int ret = 0;
    if (oppo_wipe_items(WIPE_CACHE) == WIPE_CACHE_FAILED)	
        ret = -1;
    oppo_debug("end... ret=%d\n", ret);		
    return ret;	
}
int wipe_item_show_end = 0;

STATUS wipe_item_show(menuUnit *p)
{
/* OPPO 2013-01-17 jizhengkang azx Add begin for reason */
	struct bootloader_message boot;
/* OPPO 2013-01-17 jizhengkang Add end */
    int wipe_result;
    
    oppo_debug("start .....\n");
    if (RET_YES == oppo_confirm(3, p->name, p->desc, p->icon)) {
		wipe_result = oppo_wipe_items(p->result);
/* OPPO 2013-01-17 jizhengkang azx Add begin for reason */
		if (!(wipe_result%2)) {
			memset(&boot, 0, sizeof(boot));
			set_bootloader_message(&boot);
			strncpy(p->title_name, "<~wipe.result.success.title>", MENU_LEN);
		} else {
			strncpy(p->title_name, "<~wipe.result.failed.title>", MENU_LEN);
		}
/* OPPO 2013-01-17 jizhengkang Add end */
		wipe_result_confirm(p, wipe_result);
		wipe_item_show_end = 1;
		selectedCount--;
    }
/* OPPO 2013-02-18 jizhengkang azx Add begin for reason */
	selectedCount--;
/* OPPO 2013-02-18 jizhengkang azx Add end */
    oppo_debug("end...\n");
    return MENU_BACK;

}

STATUS wipe_menu_show(menuUnit *p)
{
    oppo_debug("start...\n");
	if (wipe_item_show_end) {
		wipe_item_show_end = 0;
		return MENU_BACK;
	}
	
    return_val_if_fail(p != NULL, RET_FAIL);
    int n = p->get_child_count(p);
    int selindex = 0;
    return_val_if_fail(n >= 1, RET_FAIL);
    return_val_if_fail(n < ITEM_COUNT, RET_FAIL);
    struct _menuUnit *temp = p->child;
    return_val_if_fail(temp != NULL, RET_FAIL);
    char **menu_item = malloc(n * sizeof(char *));
    assert_if_fail(menu_item != NULL);
    char **icon_item=malloc(n * sizeof(char *));
    assert_if_fail(icon_item != NULL);	
    int i = 0;
    for (i = 0; i < n; i++)
    {
        menu_item[i] = temp->name;
        icon_item[i] = temp->icon;		
        temp = temp->nextSilbing;
    }
    selindex = wipe_menu(p->name, menu_item, icon_item, NULL, n);

/* OPPO 2013-02-18 jizhengkang azx Add begin for reason */
	selectedItem[selectedCount] = selindex - 1;
	selindex?selectedCount++:selectedCount--;
/* OPPO 2013-02-18 jizhengkang azx Add end */

    p->result = selindex;
    if (menu_item != NULL) free(menu_item);
    oppo_debug("end...p->result=%d\n", p->result);
    return p->result;
}
struct _menuUnit* wipe_ui_init()
{
    ag_canvas(&canvas_wipemenu,agw(),agh());
	
    struct _menuUnit* p = common_ui_init();
    return_null_if_fail(p != NULL);
    return_null_if_fail(menuUnit_set_name(p, "<~wipe.name>") == RET_OK);
    return_null_if_fail(menuUnit_set_title(p, "<~wipe.title>") == RET_OK);
    return_null_if_fail(menuUnit_set_icon(p, "@wipe") == RET_OK);
    return_null_if_fail(RET_OK == menuUnit_set_show(p, &wipe_menu_show));
    return_null_if_fail(menuNode_init(p) != NULL);
    //wipe_data/factory reset
    struct _menuUnit* temp = common_ui_init();
    assert_if_fail(menuNode_add(p, temp) == RET_OK);
    return_null_if_fail(menuUnit_set_name(temp, "<~wipe.factory.name>") == RET_OK);
    return_null_if_fail(menuUnit_set_result(temp, WIPE_FACTORY) == RET_OK);
    return_null_if_fail(menuUnit_set_desc(temp, "<~wipe.factory.desc>") == RET_OK);
    return_null_if_fail(menuUnit_set_icon(temp, "@wipe.all") == RET_OK);	
    return_null_if_fail(RET_OK == menuUnit_set_show(temp, &wipe_item_show));
    //wipe_data
    temp = common_ui_init();
    assert_if_fail(menuNode_add(p, temp) == RET_OK);
    return_null_if_fail(menuUnit_set_name(temp, "<~wipe.data.name>") == RET_OK);
    return_null_if_fail(menuUnit_set_result(temp, WIPE_DATA) == RET_OK);
    return_null_if_fail(menuUnit_set_desc(temp, "<~wipe.data.desc>") == RET_OK);
    return_null_if_fail(menuUnit_set_icon(temp, "@wipe.data") == RET_OK);		
    return_null_if_fail(RET_OK == menuUnit_set_show(temp, &wipe_item_show));
    //wipe_cache
    temp = common_ui_init();
    assert_if_fail(menuNode_add(p, temp) == RET_OK);
    return_null_if_fail(menuUnit_set_name(temp, "<~wipe.cache.name>") == RET_OK);
    return_null_if_fail(menuUnit_set_result(temp, WIPE_CACHE) == RET_OK);
    return_null_if_fail(menuUnit_set_desc(temp, "<~wipe.cache.desc>") == RET_OK);
    return_null_if_fail(menuUnit_set_icon(temp, "@wipe.cache") == RET_OK);		
    return_null_if_fail(RET_OK == menuUnit_set_show(temp, &wipe_item_show));
    return p;
}
