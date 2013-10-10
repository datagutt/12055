#include "../oppo_inter.h"
#include "../oppo.h"

extern int record_language;
int extra_move_enable = 0;

static STATUS lang_menu_show(menuUnit *p)
{
    oppo_debug("start...\n");
    return_val_if_fail(p != NULL, RET_FAIL);
	extra_move_enable = 1;

    /*
    int ret = oppo_langmenu(p->title_name,p->icon, 
            "简体中文", "欢迎到recovery", "@lang",
            "English", "Welcome to recovery", "@lang", 2);
    */
    int ret = oppo_langmenu(p->title_name, p->icon);

    if (1 == ret)
    {
        oppo_loadlang("langs/cn.lang");
        p->result = 1;
    }
    else if (2 == ret)
    {
        oppo_loadlang("langs/cn_trad.lang");
        p->result = 1;
    }
    else if (3 == ret)
    {
        oppo_loadlang("langs/en.lang");
        p->result = 1;
    }
    else {
        oppo_error("should not be here");
        p->result = 0;
    }
	record_language = ret;
	extra_move_enable = 0;
    oppo_debug("end...\n");
    return p->result;
}

struct _menuUnit * lang_ui_init()
{
    struct _menuUnit *p = common_ui_init();
    return_null_if_fail(p != NULL);
    strncpy(p->name, "<~lang.name>", MENU_LEN);
    strncpy(p->title_name, "<~lang.title_name>", MENU_LEN);
    menuUnit_set_icon(p, "@lang");
    p->result = 0;
    p->show = &lang_menu_show;
    return p;
}

