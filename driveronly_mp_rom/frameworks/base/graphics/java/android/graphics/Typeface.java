/*
 * Copyright (C) 2006 The Android Open Source Project
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

package android.graphics;

import android.content.res.AssetManager;
import android.util.SparseArray;

import java.io.File;
//#ifdef VENDOR_EDIT
//HaiPing.Zhong@Prd.MidWare.Fonts, 2013/01/24, Add for Android4.2 Fonts
// FlipFont {
import android.util.Log;
import android.os.SystemProperties;
import java.io.BufferedInputStream;
import java.io.DataInputStream;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
// FlipFont }
//#endif /* VENDOR_EDIT */

/**
 * The Typeface class specifies the typeface and intrinsic style of a font.
 * This is used in the paint, along with optionally Paint settings like
 * textSize, textSkewX, textScaleX to specify
 * how text appears when drawn (and measured).
 */
public class Typeface {
//#ifdef VENDOR_EDIT
//HaiPing.Zhong@Prd.MidWare.Fonts, 2013/01/24, Add for Android4.2 Fonts
    // FlipFont {
    private static final String TAG = "Monotype";
    // FlipFont }
//#endif /* VENDOR_EDIT */

    /** The default NORMAL typeface object */
    public static final Typeface DEFAULT;
    /**
     * The default BOLD typeface object. Note: this may be not actually be
     * bold, depending on what fonts are installed. Call getStyle() to know
     * for sure.
     */
    public static final Typeface DEFAULT_BOLD;
    /** The NORMAL style of the default sans serif typeface. */
    public static final Typeface SANS_SERIF;
    /** The NORMAL style of the default serif typeface. */
    public static final Typeface SERIF;
    /** The NORMAL style of the default monospace typeface. */
    public static final Typeface MONOSPACE;

    static Typeface[] sDefaults;
    private static final SparseArray<SparseArray<Typeface>> sTypefaceCache =
            new SparseArray<SparseArray<Typeface>>(3);

    int native_instance;

    // Style
    public static final int NORMAL = 0;
    public static final int BOLD = 1;
    public static final int ITALIC = 2;
    public static final int BOLD_ITALIC = 3;

    private int mStyle = 0;

//#ifdef VENDOR_EDIT
//HaiPing.Zhong@Prd.MidWare.Fonts, 2013/01/24, Add for Android4.2 Fonts
    // FlipFont {
   
    private static final String FONTS_FOLDER = "/system/fonts/";
    private static final String UI_FONT_PATH= FONTS_FOLDER + "UIFont.ttf";
    private static final String UI_FONT_BOLD_PATH= FONTS_FOLDER + "UIBoldFont.ttf";
    private static final String SANS_LOC_PATH = "/data/data/com.android.settings/app_fonts/sans.loc";

    private static String FlipFontPath = "";
    
    private static final int SANS_INDEX = 1;
    private static final int SERIF_INDEX = 2;
    private static final int MONOSPACE_INDEX = 3;
    private static final boolean FLIP_ALL_APPS = true;//false;//HaiPing.Zhong@Prd.MidWare.Fonts, 2013/01/24, Add for Android4.2 Fonts
    private static final boolean DEBUG = false;

    private static final String DROIDSANS = "DroidSans.ttf";
    private static final String DROIDSANS_BOLD = "DroidSans-Bold.ttf"; 

    // hold onto the last app name and in-list boolean
    private static String  lastAppNameString = null;
    private static boolean lastAppInList = false;

    // hold onto the UIFont/Bold paths (if the files exist)
    private static String UIFontPath = null;
    private static String UIFontBoldPath = null;
    
    // static flag - is any flipfont or default in current use - true=flipfont, false=default
    public static boolean isFlipFontUsed = false; 
    // dynamic flag - is created typeface similar to the standard default (like "sans-serif")
    public boolean isLikeDefault = false; 
    private static final Typeface[] sStaticDefaults;

    private static final String[] FlipFontAppList = { 
    "android",
    "com.android.*",
    "com.oppo.*",
    "com.nearme.*",
    "com.google.*",
    "com.mediatek.*",
    "com.baidu.input",
    "com.sohu.inputmethod.sogou",
    "com.iflytek.speechservice",
    "oppo.multimedia.soundrecorder",
    "com.sina.weibo",
    "com.svox.pico",
    "com.cooliris.media",
    "jp.co.omronsoft.openwnn",
    "com.monotypeimaging.*"
    };

//HaiPing.Zhong@Prd.MidWare.Fonts, 2013/01/24, Add for Android4.2 Fonts
    private static final String[] FlipFontAppFiltertList = { 
    "com.android.browser",
    "com.android.htmlviewer",
    "com.oppo.tribune",
    "com.oppo.socialhub"
    };

    private static final String[] FontsLikeDefault = { 
    "sans-serif-light"
    };

    // FlipFont }
//#endif /* VENDOR_EDIT */
    /** Returns the typeface's intrinsic style attributes */
    public int getStyle() {
        return mStyle;
    }

    /** Returns true if getStyle() has the BOLD bit set. */
    public final boolean isBold() {
        return (mStyle & BOLD) != 0;
    }

    /** Returns true if getStyle() has the ITALIC bit set. */
    public final boolean isItalic() {
        return (mStyle & ITALIC) != 0;
    }

//#ifdef VENDOR_EDIT
//HaiPing.Zhong@Prd.MidWare.Fonts, 2013/01/24, Add for Android4.2 Fonts
    // FlipFont {
    /** private version of create() for internal use */
    private static Typeface _create(String familyName, int style) {
        return new Typeface(nativeCreate(familyName, style));
    }
    // FlipFont }
//#endif /* VENDOR_EDIT */
    /**
     * Create a typeface object given a family name, and option style information.
     * If null is passed for the name, then the "default" font will be chosen.
     * The resulting typeface object can be queried (getStyle()) to discover what
     * its "real" style characteristics are.
     *
     * @param familyName May be null. The name of the font family.
     * @param style  The style (normal, bold, italic) of the typeface.
     *               e.g. NORMAL, BOLD, ITALIC, BOLD_ITALIC
     * @return The best matching typeface.
     */
    public static Typeface create(String familyName, int style) {
//#ifndef VENDOR_EDIT
//HaiPing.Zhong@Prd.MidWare.Fonts, 2013/01/24, Modify for Android4.2 Fonts
//        return new Typeface(nativeCreate(familyName, style));
//#else /* VENDOR_EDIT */
        // FlipFont {
        if(DEBUG)
            Log.v(TAG, "create1:" + familyName + " style=" + style);

        // protect from invalid styles
        if (style < NORMAL || style > BOLD_ITALIC)
            style = NORMAL;
            
        // If null family name - use default/flipfont
        if (familyName == null) {
            return sDefaults[style];
        }
        else
        {
            // otherwise call the private local copy
            Typeface tf = _create(familyName, style);

            if (DEBUG)
            {
                Log.v(TAG, "create1 name:" + familyName + " style:" + style + " tf:" + tf);
                Log.v(TAG, "    create1: DEF:" + DEFAULT + " DEF[]:" + sDefaults[style] + " STATIC[]:" + sStaticDefaults[style]);
            }
            
            // if the created typeface is the same as the static/default - use default/flipfont
            if (tf != null && tf.equals(sStaticDefaults[style]))
            {
                return sDefaults[style];
            }

            // loop looking for family names that are similar to default
            for (int ix = 0; ix < FontsLikeDefault.length; ix++)
            {
                if (familyName.equals(FontsLikeDefault[ix]))
                {
                    tf.isLikeDefault = true;
                    break;
                }
            }

            return tf;
        }
        // FlipFont }
//#endif /* VENDOR_EDIT */
    }

    /**
     * Create a typeface object that best matches the specified existing
     * typeface and the specified Style. Use this call if you want to pick a new
     * style from the same family of an existing typeface object. If family is
     * null, this selects from the default font's family.
     *
     * @param family May be null. The name of the existing type face.
     * @param style  The style (normal, bold, italic) of the typeface.
     *               e.g. NORMAL, BOLD, ITALIC, BOLD_ITALIC
     * @return The best matching typeface.
     */
    public static Typeface create(Typeface family, int style) {
        int ni = 0;        
//#ifdef VENDOR_EDIT
//HaiPing.Zhong@Prd.MidWare.Fonts, 2013/01/24, Add for Android4.2 Fonts
        // FlipFont {
        if(DEBUG)
            Log.v(TAG, "create2:" + family + " style=" + style);
        // protect from invalid styles
        if (style < NORMAL || style > BOLD_ITALIC)
            style = NORMAL;
        // FlipFont }
//#endif /* VENDOR_EDIT */

//#ifndef VENDOR_EDIT
//HaiPing.Zhong@Prd.MidWare.Fonts, 2013/01/24, Modify for Android4.2 Fonts
/*
        if (family != null) {
            // Return early if we're asked for the same face/style
            if (family.mStyle == style) {
                return family;
            }

            ni = family.native_instance;
        }
*/
//#else /* VENDOR_EDIT */
        if (family != null) {
            // Return early if we're asked for the same face/style
            if (family.mStyle == style) {
                return family;
            }
            // FlipFont {
            // if the family is the current default/flipfont return the current default[style]
            if (family.equals(sDefaults[family.mStyle])) {
                return sDefaults[style];
            }
			// FlipFont }
            ni = family.native_instance;
        }
        // FlipFont {
        else {
            return sDefaults[style];
        }
        // FlipFont }
//#endif /* VENDOR_EDIT */


        Typeface typeface;
        SparseArray<Typeface> styles = sTypefaceCache.get(ni);

        if (styles != null) {
            typeface = styles.get(style);
            if (typeface != null) {
                return typeface;
            }
        }

        typeface = new Typeface(nativeCreateFromTypeface(ni, style));
//#ifdef VENDOR_EDIT
//HaiPing.Zhong@Prd.MidWare.Fonts, 2013/01/24, Add for Android4.2 Fonts
        // FlipFont {
        // typeface inherits "like default" attribute from [in] typeface 
        if (typeface != null && family != null)
        {
            typeface.isLikeDefault = family.isLikeDefault;
        }
        // FlipFont }
//#endif /* VENDOR_EDIT */
        if (styles == null) {
            styles = new SparseArray<Typeface>(4);
            sTypefaceCache.put(ni, styles);
        }
        styles.put(style, typeface);

        return typeface;
    }

    /**
     * Returns one of the default typeface objects, based on the specified style
     *
     * @return the default typeface that corresponds to the style
     */
    public static Typeface defaultFromStyle(int style) {
//#ifdef VENDOR_EDIT
//HaiPing.Zhong@Prd.MidWare.Fonts, 2013/01/24, Add for Android4.2 Fonts
        // FlipFont {
        // protect from invalid styles
        if (style < NORMAL || style > BOLD_ITALIC)
            style = NORMAL;
        // FlipFont }
//#endif /* VENDOR_EDIT */
        return sDefaults[style];
    }
    
    /**
     * Create a new typeface from the specified font data.
     * @param mgr The application's asset manager
     * @param path  The file name of the font data in the assets directory
     * @return The new typeface.
     */
    public static Typeface createFromAsset(AssetManager mgr, String path) {
        return new Typeface(nativeCreateFromAsset(mgr, path));
    }

    /**
     * Create a new typeface from the specified font file.
     *
     * @param path The path to the font data. 
     * @return The new typeface.
     */
    public static Typeface createFromFile(File path) {
        return new Typeface(nativeCreateFromFile(path.getAbsolutePath()));
    }

    /**
     * Create a new typeface from the specified font file.
     *
     * @param path The full path to the font data. 
     * @return The new typeface.
     */
    public static Typeface createFromFile(String path) {
        return new Typeface(nativeCreateFromFile(path));
    }

    // don't allow clients to call this directly
    private Typeface(int ni) {
        if (ni == 0) {
            throw new RuntimeException("native typeface cannot be made");
        }

        native_instance = ni;
        mStyle = nativeGetStyle(ni);
    }

//#ifndef VENDOR_EDIT
//HaiPing.Zhong@Prd.MidWare.Fonts, 2013/01/24, Modify for Android4.2 Fonts
/*
    static {
        DEFAULT         = create((String) null, 0);
        DEFAULT_BOLD    = create((String) null, Typeface.BOLD);
        SANS_SERIF      = create("sans-serif", 0);
        SERIF           = create("serif", 0);
        MONOSPACE       = create("monospace", 0);
        
        sDefaults = new Typeface[] {
            DEFAULT,
            DEFAULT_BOLD,
            create((String) null, Typeface.ITALIC),
            create((String) null, Typeface.BOLD_ITALIC),
        };
    }
*/
//#else /* VENDOR_EDIT */
    static {
        // FlipFont { // call the private local "_create()" instead of public "create()"
        DEFAULT         = _create((String)null, 0);
        DEFAULT_BOLD    = _create((String)null, Typeface.BOLD);
        SANS_SERIF      = _create("sans-serif", 0);
        SERIF           = _create("serif", 0);
        MONOSPACE       = _create("monospace", 0);
        
        sDefaults = new Typeface[] {
            DEFAULT,
            DEFAULT_BOLD,
            _create((String)null, Typeface.ITALIC),
            _create((String)null, Typeface.BOLD_ITALIC),
        };

        sStaticDefaults = new Typeface[] {
            _create((String)null, Typeface.NORMAL),
            _create((String)null, Typeface.BOLD),
            _create((String)null, Typeface.ITALIC),
            _create((String)null, Typeface.BOLD_ITALIC)
        };
        // FlipFont }
    }
//#endif /* VENDOR_EDIT */
    

    protected void finalize() throws Throwable {
        try {
            nativeUnref(native_instance);
        } finally {
            super.finalize();
        }
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;

        Typeface typeface = (Typeface) o;

        return mStyle == typeface.mStyle && native_instance == typeface.native_instance;
    }

    @Override
    public int hashCode() {
        int result = native_instance;
        result = 31 * result + mStyle;
        return result;
    }

    private static native int  nativeCreate(String familyName, int style);
    private static native int  nativeCreateFromTypeface(int native_instance, int style); 
    private static native void nativeUnref(int native_instance);
    private static native int  nativeGetStyle(int native_instance);
    private static native int  nativeCreateFromAsset(AssetManager mgr, String path);
    private static native int nativeCreateFromFile(String path);

    /**
     * Set the global gamma coefficients for black and white text. This call is
     * usually a no-op in shipping products, and only exists for testing during
     * development.
     *
     * @param blackGamma gamma coefficient for black text
     * @param whiteGamma gamma coefficient for white text
     *
     * @hide - this is just for calibrating devices, not for normal apps
     */
    public static native void setGammaForText(float blackGamma, float whiteGamma);

//#ifdef VENDOR_EDIT
//HaiPing.Zhong@Prd.MidWare.Fonts, 2013/01/24, Add for Android4.2 Fonts
// FlipFont {
/*=== FLIPFONT ==========================================================================================*/    

    private static String[] loadAppList()
    {
        // we would load the dynamic list here - file, system properties, etc.
        if (false) {
        }
        return FlipFontAppList;
    }

    private static boolean isAppInFlipList(String appNameString)
    {
//HaiPing.Zhong@Prd.MidWare.Fonts, 2013/01/24, Add for Android4.2 Fonts
        //situations where we always return false
        if (appNameString == null || appNameString.contains(".cts"))
            return false;

        // situations where we always return true
        if (FLIP_ALL_APPS)
            return true;

//HaiPing.Zhong@Prd.MidWare.Fonts, 2013/01/24, Remove for Android4.2 Fonts
        //situations where we always return false
//        if (appNameString == null || appNameString.contains(".cts"))
//            return false;

//HaiPing.Zhong@Prd.MidWare.Fonts, 2013/01/24, Add for Android4.2 Fonts FlipFontAppFiltertList
        String AppFiltertList[] = FlipFontAppFiltertList;
        for (int index = 0; index < AppFiltertList.length; index++)
        {
            if ( (AppFiltertList[index].endsWith("*") && appNameString.startsWith(AppFiltertList[index].substring(0, AppFiltertList[index].length()-1))) ||
                 appNameString.equals(AppFiltertList[index]) )
            {
                return false;
            }
        }


        // if the same as last time asked - return previous true/false
        if (appNameString.equals(lastAppNameString)) {
            if(DEBUG) 
                Log.v(TAG, "Typeface same as last time - str = " + appNameString);
            return lastAppInList;
        }

        if(DEBUG)
            Log.v("appname", "Appname=" + appNameString + " Last=" + lastAppNameString);

        // load the list
        String AppList[] = loadAppList();
        
        // default to not found and look through the list
        boolean bInList = false;
        for (int i = 0; i < AppList.length; i++)
        {
            if ( (AppList[i].endsWith("*") && appNameString.startsWith(AppList[i].substring(0, AppList[i].length()-1))) ||
                 appNameString.equals(AppList[i]) )
            {
                bInList = true;
                if(DEBUG) 
                    Log.v(TAG, "		App in flipfont list - app name is " + appNameString );
                break;
            }
        }

        // hold onto the last app info
        lastAppNameString = appNameString;
        lastAppInList = bInList;

        return bInList;
    }

    public static String getFontNameFlipFont(int typefaceIndex) {
        // get the full data
        String sx = getFullFlipFont(typefaceIndex);
        
        // split it - file name is before the '#', font name is after it (if available)
        String[] parts = sx.split("#"); 

        // if no name available (no '#' or nothing after it) 
        if (parts.length < 2)
        {
            if (parts[0].endsWith("default")) // if the file name is "default"
                return "default";
            else
                return null; // a flipfont is used but the name is not available
        }
        return parts[1];
    }
    
    public static String getFontPathFlipFont(int typefaceIndex) {
        // get the full data
        String sx = getFullFlipFont(typefaceIndex);

        // split it - file name is before the '#' (or the full data if no '#" found)
        String[] parts = sx.split("#"); 
        return parts[0];
    }
    
    private static String getFullFlipFont(int typefaceIndex) {
        if(DEBUG) Log.v(TAG, "		Typeface getFontPathFlipFont - typefaceIndex = " + typefaceIndex);
        
        String systemFont = "default";
        switch (typefaceIndex) {
            case SANS_INDEX:
                // Use system properties to get current flipfont
                systemFont = SystemProperties.get("persist.sys.flipfontpath", "empty");
                // if path is empty get the font path from the sans.loc file and
                // set the property with font path
                if(systemFont.equals("empty")) {
                    // Open the specified .loc file
                    File file;
                    file = new File(SANS_LOC_PATH);
                    FileInputStream fis = null;
                    BufferedInputStream bis = null;
                    DataInputStream dis = null;
                    String string = null; 
                    
                    try {
                        fis = new FileInputStream(file);
                        // Here BufferedInputStream is added for fast reading.
                        bis = new BufferedInputStream(fis);
                        dis = new DataInputStream(bis);
                        string = dis.readLine();
                        
                        // dispose all the resources after using them.
                        fis.close();
                        bis.close();
                        dis.close();
                    } catch (FileNotFoundException e) {
                        string = "default";
                    } catch (IOException e) {
                        string = "default";
                        e.printStackTrace();
                    } 
                    systemFont = string;
                    SystemProperties.set("persist.sys.flipfontpath", systemFont);
                }
                break;
            case SERIF_INDEX:
                break;
            case MONOSPACE_INDEX:
                break;
            default:
                break;
        }
        if(DEBUG) Log.v(TAG, "		Typeface getFontPathFlipFont - fontPath = " + systemFont);

        return systemFont;
    }

    // set the native typefaces to FlipFont - return if none set up (default will be used)
    private static void SetFlipFonts()
    {
        String strFontPathBold = "";

        // see if there is an actual FlipFont set up
        String strFontPath = getFontPathFlipFont(SANS_INDEX);

        // see if there is a FlipFont or "default". If none check for a UIFont
        if (strFontPath.endsWith("default")) {
            if(DEBUG) Log.v(TAG, "Typeface->setTypefaceFlipFont Font Path = none.  path = " + strFontPath);

            // we are using the default font (not flipfont)
            isFlipFontUsed = false;

            // assume if UIFont file exists the first time it will always exist
            if (UIFontPath == null)
            {
                File f;

                UIFontPath = "";
                UIFontBoldPath = "";

                f = new File(UI_FONT_PATH);
                if (f.exists()) 
                    UIFontPath = UI_FONT_PATH;

                f = new File(UI_FONT_BOLD_PATH);
                if (f.exists()) 
                    UIFontBoldPath = UI_FONT_BOLD_PATH;
            }
            strFontPath = UIFontPath;
            strFontPathBold = UIFontBoldPath;
        }
        else {
            // we are using flipfont (not the default font)
            isFlipFontUsed = true;

            strFontPathBold = strFontPath + "/" + DROIDSANS_BOLD;
            strFontPath = strFontPath + "/" + DROIDSANS;
        }
        
        if(DEBUG) Log.v(TAG, "Typeface->setTypefaceFlipFont path = " + strFontPath);
        
        // if FlipFont is the same as last time - don't create again but return true;
        if (strFontPath.equals(FlipFontPath))
        {
            if(DEBUG) Log.v(TAG, "Typeface->setTypefaceFlipFont path has not changed.  path = " + strFontPath);
            return;
        }
        FlipFontPath = strFontPath;

        // we're flipping - set the native typefaces
        int iNative;

        if(DEBUG)
            Log.v(TAG, "Typeface->SetFlip() before:" + DEFAULT.native_instance + "|" +
                       DEFAULT_BOLD.native_instance + "::" + sDefaults[0].native_instance +
                       "|" + sDefaults[1].native_instance + "|" + sDefaults[2].native_instance +
                       "|" + sDefaults[3].native_instance);

        // default/normal
        if(DEBUG) Log.v(TAG, "Typeface->setTypefaceFlipFont flipping normal, path = " + strFontPath);
        iNative = DEFAULT.native_instance;
        if (strFontPath.isEmpty())
            DEFAULT.native_instance = nativeCreate(null, 0);
        else
        {
            DEFAULT.native_instance = nativeCreateFromFile(strFontPath);
            if (DEFAULT.native_instance == 0) 
                DEFAULT.native_instance = nativeCreate(null, 0);
        }
        DEFAULT.mStyle = nativeGetStyle(DEFAULT.native_instance);
        nativeUnref(iNative);

        // bold
        if(DEBUG) Log.v(TAG, "Typeface->setTypefaceFlipFont flipping bold.  path = " + strFontPathBold);
        iNative = DEFAULT_BOLD.native_instance;
        if (strFontPathBold.isEmpty())
            DEFAULT_BOLD.native_instance = nativeCreate(null, Typeface.BOLD);
        else
        {
            DEFAULT_BOLD.native_instance = nativeCreateFromFile(strFontPathBold);
            if (DEFAULT_BOLD.native_instance == 0) 
                DEFAULT_BOLD.native_instance = nativeCreate(null, Typeface.BOLD);
        }
        DEFAULT_BOLD.mStyle = nativeGetStyle(DEFAULT_BOLD.native_instance);
        nativeUnref(iNative);

        // sDefaults ----
        // normal
        iNative = sDefaults[0].native_instance;
        sDefaults[0].native_instance = nativeCreateFromTypeface(DEFAULT.native_instance, Typeface.NORMAL);
        sDefaults[0].mStyle = nativeGetStyle(sDefaults[0].native_instance);
        nativeUnref(iNative);

        // bold
        iNative = sDefaults[1].native_instance;
        sDefaults[1].native_instance = nativeCreateFromTypeface(DEFAULT_BOLD.native_instance, Typeface.BOLD);
        sDefaults[1].mStyle = nativeGetStyle(sDefaults[1].native_instance);
        nativeUnref(iNative);

        // italic
        iNative = sDefaults[2].native_instance;
        sDefaults[2].native_instance = nativeCreateFromTypeface(DEFAULT.native_instance, Typeface.ITALIC);
        sDefaults[2].mStyle = nativeGetStyle(sDefaults[2].native_instance);
        nativeUnref(iNative);

        // bold-italic
        iNative = sDefaults[3].native_instance;
        sDefaults[3].native_instance = nativeCreateFromTypeface(DEFAULT_BOLD.native_instance, Typeface.BOLD_ITALIC);
        sDefaults[3].mStyle = nativeGetStyle(sDefaults[3].native_instance);
        nativeUnref(iNative);

//HaiPing.Zhong@Prd.MidWare.Fonts, 2013/01/24, Add for Android4.2 Fonts, password TextView MONOSPACE Typeface
        // MONOSPACE
        iNative = MONOSPACE.native_instance;
        if (strFontPath.isEmpty())
            MONOSPACE.native_instance = nativeCreate("monospace", 0);
        else
        {
            MONOSPACE.native_instance = nativeCreateFromTypeface(DEFAULT.native_instance, Typeface.NORMAL);
            if (MONOSPACE.native_instance == 0) 
                MONOSPACE.native_instance = nativeCreate("monospace", 0);
        }
        MONOSPACE.mStyle = nativeGetStyle(MONOSPACE.native_instance);
        nativeUnref(iNative);


        if(DEBUG)
            Log.v(TAG, "Typeface->SetFlip() after:" + DEFAULT.native_instance + "|" +
                       DEFAULT_BOLD.native_instance + "::" + sDefaults[0].native_instance + "|" +
                       sDefaults[1].native_instance + "|" + sDefaults[2].native_instance + "|" + sDefaults[3].native_instance);
   }

    public static void SetAppTypeFace(String sAppName)
    {
        if(DEBUG) 
            Log.v(TAG, "Typeface->setAppTypeface app=" + sAppName + " def=" + DEFAULT.native_instance);

        // if app should be flipped
        if (isAppInFlipList(sAppName))
        {
            if(DEBUG) 
               Log.v(TAG, "SetAppTypeFace- try to flip, app = " + sAppName);

            // flip
            SetFlipFonts();
        }
        if(DEBUG) 
            Log.v(TAG, "END Typeface->setAppTypeface app=" + sAppName + " def=" + DEFAULT.native_instance);
    }
    
 /*=== FLIPFONT ==========================================================================================*/    
// FlipFont }
//#endif /* VENDOR_EDIT */
}
