// VENDOR_EDIT
// OPPO theme management class.
//
#ifndef __LIBS_OPPOTHEMEMANAGER_H
#define __LIBS_OPPOTHEMEMANAGER_H

#include <utils/String8.h>
#include <utils/threads.h>

namespace android {

class OPPOThemeManager {

public:
    static OPPOThemeManager* getInstance();
    static void releaseInstance();
    bool isFileInMap(const char* apkName, const char* fileName);
    void parseThemeMapIfNeeded();
    void setThemeVersion(int themeVersion);

//xinyang.hu@ComUI.Theme,2012/04/06, add for theme color
    void parseColorIdMapIfNeeded();
    void parseColorsFileIfNeeded();
    int findColorByColorId(int colorId);
    int findColorByColorName(const char *colorName);
    char* getLastThemePath();
    void setLastThemePath(const char* themePath);
//theme color end

private:
    OPPOThemeManager();
    virtual ~OPPOThemeManager();

    struct ThemeFileList
    {
    	const char *fileName;
    };

    struct ThemePathMap
    {
    	const char *path;
        ThemeFileList *fileList;
        int fileCount;

        ThemePathMap() {
        	path = NULL;
        	fileList = NULL;
        	fileCount = 0;
        }
    };

//xinyang.hu@ComUI.Theme,2012/04/06, add for theme color
    struct ColorIdMap
    {
    	 const char *colorName;
        uint32_t colorId;

        ColorIdMap() {
        	colorName = NULL;
        	colorId = 0;
        }
    };

	struct Color
    {
    	 const char *colorName;
        uint32_t colorValue;

        Color() {
        	colorName = NULL;
        	colorValue = 0;
        }
    };
    static int mColorIdCount;
    ColorIdMap *mColorIdMapList;
    static int mColorsCount;
    Color *mColorsList;
    // save the last theme path.
    char mLastThemePath[120];
//theme color end


    static int mModuleCount;
    ThemePathMap *mPathMap;
    static OPPOThemeManager *mInstance;
	static int mThemeVersion;

private:
    static char* THEME_MAP_FILE;

private:
    bool findFileInList(ThemeFileList *fileList, int listLen, const char* fileName);
    void dumpMapInfo();
};  // end of OPPOThemeManager

}; // namespace android

#endif // __LIBS_ASSETMANAGER_H
