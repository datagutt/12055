#ifndef OPPO_H_
#define OPPO_H_
//add in ui operation
//
//
typedef int STATUS;
#define RET_OK 0
#define RET_FAIL -1
#define RET_INVALID_ARG -1
#define RET_YES 1
#define RET_NO 0
#define RET_NULL 0

#define ICON_ENABLE   "@enable"
#define ICON_DISABLE  "@disable"
//MAX_MENU_
#define ITEM_COUNT 128
#define MENU_BACK ITEM_COUNT 
#define MENU_QUIT ITEM_COUNT + 1

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
       printf("(pid:%d)[%s]function %s(line %d) " #p " \n",getpid(), __FILE__, __FUNCTION__, __LINE__);return NULL;}
#endif

#ifndef assert_if_fail
#define assert_if_fail(p) \
    if (!(p)) { \
       printf("(pid:%d)[%s]function %s(line %d) " #p " \n",getpid(), __FILE__,  __FUNCTION__, __LINE__);}
#endif


#define MENU_LEN 32 //international, ~ or direct print string;
typedef struct _menuUnit {
    char name[MENU_LEN];
    char title_name[MENU_LEN];
    char icon[MENU_LEN];
    char desc[MENU_LEN];
    int  result;

    void *data;

    struct _menuUnit *child;
    struct _menuUnit *nextSilbing;
    struct _menuUnit *parent;
    //

    //method
    STATUS (*show)(struct _menuUnit *p);

    int (*get_child_count)(struct _menuUnit *p);
    struct _menuUnit * (*get_child_by_index)(struct _menuUnit *p, int index);

}menuUnit, *pmenuUnit;
typedef STATUS (*menuUnitFunction)(struct _menuUnit *p);


//oppo_ui.c
char * oppo_readfromfs(char * name);
void oppo_writetofs(char * name, char * value);
char * oppo_readfromtmp(char * name);
void oppo_writetotmp(char * name, char * value);
char * oppo_readfromzip(char * name);
char * oppo_parseprop(char * filename,char *key);
char * oppo_parsepropzip(char * filename,char *key);
char * oppo_getvar(char * name);
void oppo_setvar(char * name, char * value);
void oppo_appendvar(char * name, char * value);
void oppo_delvar(char * name);
void oppo_prependvar(char * name, char * value);
STATUS oppo_font(char *ttf_type, char *ttf_file, char *ttf_size);
char * oppo_getprop(char *file, char *key);
char * oppo_gettmpprop(char *file, char *key);
char * oppo_resread(char *file);
//* 
//* ini_get
//*
char * oppo_ini_get(char *item);
//* 
//* ini_set
//*
STATUS oppo_ini_set(char *item, char *value);

//* 
//* textbox, agreebox
//*
STATUS oppo_textbox(char *title, char *desc, char *icon, char *message); 

STATUS oppo_langmenu(char *title_name, char *title_icon) ;
//* menubox
//*
STATUS oppo_mainmenu(char *title_name, char **item, char **item_icon, char **item_icon_append, int item_cnt); 
STATUS oppo_menubox(char *title_name, char **item, int item_cnt);
STATUS oppo_sdmenu(char *title_name, char **item, char **item_sub, int item_cnt); 

STATUS wipe_menu(char *title_name, char **item, char **item_icon, char **item_icon_append, int item_cnt); 

//* 
//* confirm
//*
STATUS oppo_confirm(int argc, char *format, ...);
//* 
//* loadlang
//*
STATUS oppo_loadlang(char * name);

STATUS oppo_notice(int argc, char *format, ...);
void oppo_ums_notic(menuUnit *p);

//* 
//* lang
//*
char * oppo_lang(char *name);
int oppo_ui_init();
int oppo_ui_config(const char * file);
STATUS oppo_ui_start();
STATUS oppo_ui_end();

void wipe_thread_start(char *title);
void wipe_thread_close();
void set_wipe_process(int process);

//common_ui.c 
int common_get_child_count(menuUnit *p);
struct _menuUnit * common_get_child_by_index(struct _menuUnit *p, int index);
STATUS common_ui_show(menuUnit *p);
STATUS common_menu_show(menuUnit *p);
STATUS menu_default_init(struct _menuUnit *p);
struct _menuUnit *common_ui_init();
STATUS menuUnit_set_name(struct _menuUnit *p, const char* name);
STATUS menuUnit_set_icon(struct _menuUnit *p, const char* name);
STATUS menuUnit_set_title(struct _menuUnit *p, const char* name);
STATUS menuUnit_set_desc(struct _menuUnit *p, const char* name);
STATUS menuUnit_set_result(struct _menuUnit *p, const int result);
STATUS menuUnit_set_show(struct _menuUnit *p, menuUnitFunction fun);
//lang_ui.c 
struct _menuUnit* lang_ui_init();

//sd_ui.c
struct _menuUnit* sd_ui_init();
//power_ui.c
struct _menuUnit* power_ui_init();
struct _menuUnit* reboot_ui_init();
struct _menuUnit* poweroff_ui_init();
struct _menuUnit * ums_ui_init();

//power_ui.c
struct _menuUnit* power_ui_init();
//wipe_ui.c
struct _menuUnit* wipe_ui_init();
//oppo.c
extern struct _menuUnit* g_main_menu;
extern struct _menuUnit* g_root_menu;
STATUS main_ui_init();
STATUS main_ui_show();
STATUS main_ui_release();
//for re draw screen
STATUS oppo_set_isbgredraw(int value);

/*
 *oppo_install.c
 *@DESCRIPTION:make install pacage, be invoked by installer
 */

typedef int (*oppoInstall_fun)(const char* path, int *wipe_cache, int *wipe_data, const char* install_file);
typedef struct _oppoInstall {
    oppoInstall_fun pfun;
    char *path;
    char *install_file;
    int wipe_cache;
	int wipe_data;
}oppoInstall, *poppoInstall;
//for install init 
STATUS oppoInstall_init(oppoInstall_fun fun, char *path, int wipe_cache, int wipe_data, const char* install_file);

void oppoInstall_show_progress(float portion, int seconds);
void oppoInstall_set_progress(float fraction);
void oppoInstall_set_text(char *str);
void oppoInstall_set_info(char *file_name);
//menuNode operation
struct _menuUnit * menuNode_init(struct _menuUnit *node);
STATUS menuNode_add(struct _menuUnit *parent, struct _menuUnit *child);
STATUS menuNode_delete(struct _menuUnit *parent, struct _menuUnit *child);


#endif // __OPPO_H__
