#define LOG_TAG "ThemeManager"
//#define LOG_NDEBUG 0	/* Use to control whether LOGV is shown. */

#include <androidfw/OPPOThemeManager.h>
#include <utils/Log.h>
#include <utils/String8.h>
#include "tinyxml.h"
#include <stdio.h>

using namespace android;

static const char* kThemeMapFile = "/system/etc/theme/thememap.xml";
static const char* kThemeModuleName = "Module";
static const char* kThemePathAttr = "path";
static const char* kThemeItemName = "item";
static const int MAX_THEME_VERSION = 4;

OPPOThemeManager* OPPOThemeManager::mInstance = NULL;
int OPPOThemeManager::mModuleCount = 0;
int OPPOThemeManager::mThemeVersion = MAX_THEME_VERSION;


//xinyang.hu@ComUI.Theme, 2012/04/06, add for theme color
static const char* kColorIdMapFile = "/system/etc/theme/coloridmap.xml";
static const char* kColor = "color";
static const char* kColorName = "name";
static const char* kColorId = "id";

static const char* kColorsFile = "/data/oppo/OPPO_THEME_CHANGE/framework/colors.xml";

static const char DEFAULT_THEME_PATH[] = "/system/framework/framework-res.apk";

int OPPOThemeManager::mColorIdCount = 0;
int OPPOThemeManager::mColorsCount = 0;
//theme color end


int getChildNodeCount(const char* childName, TiXmlNode *parentNode, TiXmlNode *firstNode);

OPPOThemeManager::OPPOThemeManager()
{
	ALOGV("OPPOThemeManager constructor mInstance = %d.", mInstance);
	mPathMap = NULL;
//xinyang.hu@ComUI.Theme, 2012/04/06, add for theme color    
       mColorIdMapList = NULL;
	mColorsList = NULL;
	memset(mLastThemePath, 0, sizeof(char)*120);
	strcpy(mLastThemePath, DEFAULT_THEME_PATH); 
//theme color end
}

OPPOThemeManager::~OPPOThemeManager()
{
	ALOGV("OPPOThemeManager deconstructor mInstance = %d.", mInstance);
	/* Recycle theme file list arrays. */
	for (int i = 0; i < OPPOThemeManager::mModuleCount; i++) {
		if (mPathMap[i].fileList != NULL) {
			delete[] mPathMap[i].fileList;
			mPathMap[i].fileList = NULL;
		}
	}

	/* Recycle theme map array. */
	if (mPathMap != NULL)
	{
		delete[] mPathMap;
		mPathMap = NULL;
	}

//xinyang.hu@ComUI.Theme, 2012/04/06, add for theme color    
	/* Recycle theme map array. */
	if (mColorIdMapList != NULL)
	{
		delete[] mColorIdMapList;
		mColorIdMapList = NULL;
	}

	/* Recycle colors array. */
	if (mColorsList != NULL)
	{
		delete[] mColorsList;
		mColorsList = NULL;
	}
//theme color end

    
}

/**
 * Return a OPPOThemeManager instance, if there is an exists one,
 * use it, otherwise, construct an object, using singleton mode.
 */
OPPOThemeManager* OPPOThemeManager::getInstance()
{
	ALOGV("OPPOThemeManager getInstance start mInstance = %d.", mInstance);
	if (mInstance == NULL)
	{
		mInstance = new OPPOThemeManager();
	}
	ALOGV("OPPOThemeManager getInstance end mInstance = %d.", mInstance);
	return mInstance;
}

void OPPOThemeManager::releaseInstance() {
	ALOGV("OPPOThemeManager releaseInstance mInstance = %d.", mInstance);
	if (mInstance != NULL)
	{
		delete mInstance;
		mInstance = NULL;
	}
}

/**
 * Judge whether the given resource of the apk is in the theme resource map,
 * first find the apkName in the mPathMap array, if find successfully, then find
 * the fileName in the file list of the found apk, return true if the resource
 * of the apk is the in map, else false.
 */
bool OPPOThemeManager::isFileInMap(const char* apkName, const char* fileName)
{
    int i;
    int moduleCnt = OPPOThemeManager::mModuleCount;
    bool ret = false;

	ALOGV("isFileInMap start apkName = %s, fileName = %s, moduleCnt = %d. \n", apkName, fileName, moduleCnt);
	if (moduleCnt == 0)	/* There is no apks in the map.*/
	{
		ALOGI("isFileInMap return false because no apk path in path map list.");
		return false;
	}

	/* Find apk in the mPathMap, record the index if find successfully. */
	for (i = 0; i < moduleCnt; i++)
	{
		// if the theme version is smaller then 4.0, we skip to check the frameworks module
		if (i == 0 && mThemeVersion < 4)
			continue;
//		LOGV("isFileInMap i = %d, mPathMap[i].path = %s.", i, mPathMap[i].path);
		if (strstr(apkName, mPathMap[i].path))
		{
			ALOGV("isMappedFile i = %d, mPathMap[i].path = %s, listLen = %d, file = %s.\n",
					i, mPathMap[i].path, mPathMap[i].fileCount, fileName);
			break;
		}
	}

	if (i == moduleCnt || mPathMap[i].fileList == NULL || mPathMap[i].fileCount == 0)
	{
		ALOGV("There is no such apk path in the theme map, or there is no items in the apk file list.\n");
		return false;
	}

	ret = findFileInList(mPathMap[i].fileList, mPathMap[i].fileCount, fileName);
	ALOGV("isFileInMap end apkName = %s, fileName = %s,ret = %d.\n", apkName, fileName, ret);

	return ret;
}

/**
 * Find the specified fileName in the given fileList, if find successfully, return true, else false.
 */
bool OPPOThemeManager::findFileInList(ThemeFileList *fileList, int listLen, const char* fileName) {
//	LOGV("Run into findFileInList listLen = %d, fileName = %s.\n", listLen, fileName);
	for (int i = 0; i < listLen; i++)
	{
//		LOGV("findFileInList i=%d, fileList[i].fileName = %s,fileName = %s.\n", i, fileList[i].fileName, fileName);
		if (fileList[i].fileName != NULL && strstr(fileName, fileList[i].fileName))
		{
//			LOGV("findFileInList return true,fileName = %s.\n", fileName);
			return true;
		}
	}
	return false;
}

/**
 * set the theme version
 */
void OPPOThemeManager::setThemeVersion(int themeVersion)
{
	//LOGW("setThemeVersion:%d...",themeVersion);
	if (themeVersion <= MAX_THEME_VERSION && themeVersion >= 0 && mThemeVersion != themeVersion)
		mThemeVersion = themeVersion;
}

/**
 * Parse the theme map if the mPathMap is empty, else use the mPathMap parsed before.
 */
void OPPOThemeManager::parseThemeMapIfNeeded()
{
	if (mPathMap != NULL) {
		ALOGV("The path has already parsed.");
		return;
	}

	/* Load theme map xml file. */
	TiXmlDocument* pDoc = new TiXmlDocument(kThemeMapFile);
	if (pDoc == NULL)
	{
		ALOGE("Read theme map xml file failed!");
		return;
	}
	pDoc->LoadFile();

	/* Get the root node(thememap) and the first module child node.*/
	TiXmlElement *pRootElement = pDoc->RootElement();
	TiXmlElement *pFirstModuleElement = pRootElement->FirstChildElement(kThemeModuleName);
	ALOGV("Module element is %s, path = %s.", pFirstModuleElement->Value(), pFirstModuleElement->Attribute(kThemePathAttr));

	/* Get module node count to create the path map array.*/
	int moduleCnt = getChildNodeCount(kThemeModuleName, pRootElement, pFirstModuleElement);
	ALOGV("Total element count is %d.", moduleCnt);

	mPathMap = new ThemePathMap[moduleCnt];
	if (mPathMap == NULL)
	{
		ALOGE("Failed to allocate memory for theme path map.");
		return;
	}
	OPPOThemeManager::mModuleCount = moduleCnt;

	TiXmlNode *pModuleNode = pFirstModuleElement;
	TiXmlNode *pItemNode = NULL;
	TiXmlNode *pFirstItemElement = NULL;
	int itemCnt = 0;
	int moduleIndex = 0;
	int tempIndex = 0;

	/* Parse the whole xml by module. */
	while (pModuleNode != NULL)
	{
		mPathMap[moduleIndex].path = ((TiXmlElement *)pModuleNode)->Attribute(kThemePathAttr);
		ALOGV("parseThemeMap while start moduleIndex = %d, pModuleNode = %d, path = %s.",
				moduleIndex, pModuleNode, mPathMap[moduleIndex].path);

		pFirstItemElement = pModuleNode->FirstChildElement(kThemeItemName);
		itemCnt = getChildNodeCount(kThemeItemName, pModuleNode, pFirstItemElement);
		mPathMap[moduleIndex].fileCount = itemCnt;
		if (itemCnt == 0)
		{
			ALOGD("There is no item in apk %s.", ((TiXmlElement *)pModuleNode)->Attribute(kThemePathAttr));
			mPathMap[moduleIndex].fileList = NULL;
			continue;
		}

		ThemeFileList *itemFileList = new ThemeFileList[itemCnt];
		if (itemFileList == NULL)
		{
			ALOGE("Failed to allocate memory for item file list array.");
			return;
		}

		pItemNode = pFirstItemElement;
		tempIndex = 0;
		/* Parse all items in the current module pModuleNode. */
		while (pItemNode != NULL)
		{
			itemFileList[tempIndex++].fileName = ((TiXmlElement *)pItemNode)->GetText();
			ALOGV("parseThemeMap pItemNode->GetText() = %s, itemFileList[tempIndex].fileName = %s.",
					((TiXmlElement *)pItemNode)->GetText(), itemFileList[tempIndex-1].fileName);
			pItemNode = (TiXmlElement *)pModuleNode->IterateChildren(kThemeItemName, pItemNode);
		}

		mPathMap[moduleIndex].fileList = itemFileList;
		ALOGV("parseThemeMap moduleIndex = %d, itemCnt = %d, mPathMap[moduleIndex].fileList = %d,"
						"itemFileList = %d, filename0 = %s, itemFileList filename0 = %s.",
						moduleIndex, itemCnt,
						mPathMap[moduleIndex].fileList, itemFileList,
						(mPathMap[moduleIndex].fileList)[0].fileName, itemFileList[0].fileName);
		moduleIndex++;

		pModuleNode = (TiXmlElement *)pRootElement->IterateChildren(kThemeModuleName, pModuleNode);
	}
	ALOGV("Theme path map parsed completely.");
}

//xinyang.hu@ComUI.Theme, 2012/04/06, add for theme color
/**
 * if theme be changed, find the special color's value by id.
 */
int OPPOThemeManager::findColorByColorId(int colorId) {
	int result = -1;
	bool isColorExist = false;
	const char *colorName = NULL;
	
	//LOGD("findColorByColorId enter!!!!!!!!!!!!!!!!!colorId = %d", colorId);

	if (mColorIdMapList == NULL || mColorIdCount == 0
		|| mColorsList == NULL || mColorsCount == 0) 
	{
	//	LOGD("NULL!!!!!!!!!!!findColorByColorId  mColorIdCount = %d, mColorsCount = %d",
	//			mColorIdCount, mColorsCount);
		return result;
	}

	
	for (int i = 0; i < mColorIdCount; i++)
	{
		if (colorId > 0 && colorId == mColorIdMapList[i].colorId)
		{
			//LOGD("##########findColorByColorId i = %d, mColorIdMapList[i].colorName = %s",
			//	i, mColorIdMapList[i].colorName);
			colorName = mColorIdMapList[i].colorName;
			isColorExist = true;
			break;
		}
	}

	if (true == isColorExist) 
	{
		for (int i = 0; i < mColorsCount; i++)
		{
			if (mColorsList[i].colorName != NULL && colorName != NULL
				&& strcmp(mColorsList[i].colorName, colorName) == 0)
			{
			//	LOGD("##########findColorByColorId i = %d, colorName = %s, result value = 0x%x",i,
			//	colorName, mColorsList[i].colorValue);
				result = mColorsList[i].colorValue;
				break;
			}
		}
	}
	
//	LOGD("##########findColorByColorId result value = 0x%x", result);
	return result;
}

int OPPOThemeManager::findColorByColorName(const char *colorName) {
	int result = -1;
	//LOGE("----findColorByColorName(colorName=%s)\n",colorName);
	if (colorName == NULL || mColorsList == NULL || mColorsCount == 0) 
	{
		return result;
	}
	
	for (int i = 0; i < mColorsCount; i++)
	{
		if (mColorsList[i].colorName != NULL && strcmp(mColorsList[i].colorName, colorName) == 0)
		{
			result = mColorsList[i].colorValue;
			break;
		}
	}
	//LOGE("----findColorByColorName(result=%d)\n",result);
	return result;
}

/**
 * Parse the colorId map if the mColorIdMapList is empty, else use the mColorIdMapList parsed before.
 */
void OPPOThemeManager::parseColorIdMapIfNeeded()
{
	if (mColorIdMapList != NULL) {
		ALOGE("The mColorIdMapList has already parsed.");
		return;
	}

	/* Load colorid map xml file. */
	TiXmlDocument* pDoc = new TiXmlDocument(kColorIdMapFile);
	if (pDoc == NULL)
	{
		ALOGE("Read color map xml file failed!");
		return;
	}
	pDoc->LoadFile();

	/* Get the root node(colormap) and the first color child node.*/
	TiXmlElement *pRootElement = pDoc->RootElement();
	TiXmlElement *pFirstModuleElement = pRootElement->FirstChildElement(kColor);

	/* Get module node count to create the path map array.*/
	int moduleCnt = getChildNodeCount(kColor, pRootElement, pFirstModuleElement);
	//LOGV("Total element count is %d.", moduleCnt);

	mColorIdMapList = new ColorIdMap[moduleCnt];
	if (mColorIdMapList == NULL)
	{
		ALOGE("Failed to allocate memory for theme path map.");
		return;
	}
	OPPOThemeManager::mColorIdCount = moduleCnt;

	TiXmlNode *pModuleNode = pFirstModuleElement;
	int moduleIndex = 0;
	uint32_t temp = 0;
	const char *tempStr = NULL;

	/* Parse the whole xml by module. */
	while (pModuleNode != NULL)
	{
		mColorIdMapList[moduleIndex].colorName= ((TiXmlElement *)pModuleNode)->Attribute(kColorName);
	//	LOGD("parseColorMap while start moduleIndex = %d, pModuleNode = %d, colorName = %s.",
	//			moduleIndex, pModuleNode, mColorIdMapList[moduleIndex].colorName);
		
		tempStr = ((TiXmlElement *)pModuleNode)->Attribute(kColorId);
	//	LOGD("#################### colorId = %s.",
	//		tempStr);
		sscanf(tempStr, "%x", &temp);
		mColorIdMapList[moduleIndex].colorId = temp;
	//	LOGD("####################parseColorMap while start moduleIndex = %d, pModuleNode = %d, colorId = 0x%x.",
	//			moduleIndex, pModuleNode, mColorIdMapList[moduleIndex].colorId);
		
		moduleIndex++;
		pModuleNode = (TiXmlElement *)pRootElement->IterateChildren(kColor, pModuleNode);
	}

	ALOGV("color map parsed completely.");
}

/**
 * Parse the colors file if the mColorsList is empty, else use the mColorsList parsed before.
 */
void OPPOThemeManager::parseColorsFileIfNeeded()
{
//	LOGD("parseColorsFileIfNeeded enter!!!!!!!!!.");
	
	if (mColorsList != NULL) {
		ALOGE("The mColorsList has already parsed.");
		delete[] mColorsList;
		mColorsList = NULL;
	}

	if (access(kColorsFile, R_OK) == -1) {
		ALOGE("This file does not exist!!!!!!!!!!");
		return;
	}
	

/*
    FILE *file = NULL;  
    if ((file = fopen(kColorsFile, "r")) == NULL)  
    {
    	LOGE("This file does not exist!!!!!!!!!!");
	return;
    } else {
    	fclose(file);
    }
    */

	/* Load theme map xml file. */
	TiXmlDocument* pDoc = new TiXmlDocument(kColorsFile);
	if (pDoc == NULL)
	{
		ALOGE("Read color map xml file failed!");
		return;
	}
	bool bLoadOK = pDoc->LoadFile();
	if(!bLoadOK){
		ALOGD("parseColorsFileIfNeeded bLoadOK == false");
		return;
	}

	/* Get the root node(colormap) and the first color child node.*/
	TiXmlElement *pRootElement = pDoc->RootElement();
	TiXmlElement *pFirstModuleElement = pRootElement->FirstChildElement(kColor);

	/* Get module node count to create the path map array.*/
	int moduleCnt = getChildNodeCount(kColor, pRootElement, pFirstModuleElement);
//	LOGD("Total element count is %d.", moduleCnt);

	mColorsList = new Color[moduleCnt];
	if (mColorsList == NULL)
	{
		ALOGE("Failed to allocate memory for theme path map.");
		return;
	}
	OPPOThemeManager::mColorsCount = moduleCnt;

	TiXmlNode *pModuleNode = pFirstModuleElement;
	int moduleIndex = 0;
	uint32_t temp = 0;
	const char *tempStr = NULL;

	/* Parse the whole xml by module. */
	while (pModuleNode != NULL)
	{
		mColorsList[moduleIndex].colorName= ((TiXmlElement *)pModuleNode)->Attribute(kColorName);
	//	LOGD("parseColorMap while start moduleIndex = %d, pModuleNode = %d, colorName = %s.",
	//			moduleIndex, pModuleNode, mColorsList[moduleIndex].colorName);
		
		tempStr = ((TiXmlElement *)pModuleNode)->GetText();
	//	LOGD("#################### colorValue = %s.",
	//		tempStr);
		sscanf(tempStr, "%x", &temp);
		mColorsList[moduleIndex].colorValue= temp;
	//	LOGD("####################parseColorMap while start moduleIndex = %d, pModuleNode = %d, colorValue = 0x%x.",
	//			moduleIndex, pModuleNode, mColorsList[moduleIndex].colorValue);
		
		moduleIndex++;
		pModuleNode = (TiXmlElement *)pRootElement->IterateChildren(kColor, pModuleNode);
	}

	ALOGE("colors file parsed completely.");
}

    char* OPPOThemeManager::getLastThemePath() {
		return mLastThemePath;
    	}
	
    void OPPOThemeManager::setLastThemePath(const char* themePath){
	memset(mLastThemePath, 0, sizeof(char)*120);
	strcpy(mLastThemePath, themePath); 
    	}
//theme color end


/** This function can not be set as member function because the include file issue. */
int getChildNodeCount(const char* childName, TiXmlNode *parentNode, TiXmlNode *firstNode)
{
	int nodeCount = 0;
	TiXmlNode *pNode = firstNode;
	while (pNode != NULL)
	{
		nodeCount++;
		pNode = parentNode->IterateChildren(childName, pNode);
	}
	return nodeCount;
}

/**
 * Helper class used for debug.
 */
void OPPOThemeManager::dumpMapInfo()
{
	int i, j = 0;
	ALOGV("dumpMapInfo: moduleCount = %d,module1 = %s,module2 = %s.", OPPOThemeManager::mModuleCount,
			mPathMap[0].path, mPathMap[1].path);
	for (i = 0; i < OPPOThemeManager::mModuleCount; i++ )
	{
		ALOGV("dumpMapInfo: i = %d, path = %s, item = %s.", i, mPathMap[i].path,
				(mPathMap[i].fileList[0]).fileName);
	}
}


