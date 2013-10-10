package android.widget;

import java.lang.reflect.Field;
import java.util.ArrayList;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import dalvik.system.DexClassLoader;
import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.content.res.Resources;
import android.content.res.Resources.NotFoundException;
import android.graphics.drawable.Drawable;
import android.text.Spannable;
import android.text.method.TransformationMethod;
import android.text.style.ImageSpan;
import android.util.Log;
import android.widget.TextView;

public class OppoEmojiPlugin {
	private final static String TAG = "OppoEmojiPlugin";
	private final static boolean DEBUG = false;

	private final static String PLUGIN_PACKAGE_NAME = "com.oppo.exp.emoji";
	private final static String DRAWABLE_ClASS_FULL_PATH = PLUGIN_PACKAGE_NAME
			+ ".R$drawable";
	private static Resources mResources;
	static final Object mInstanceSync = new Object();

	@SuppressWarnings("rawtypes")
	private static Class mDrawableClass;

	private static OppoEmojiPlugin mInstance;

	public static OppoEmojiPlugin getInstance(Context context) {
		synchronized (mInstanceSync) {
			if (mInstance == null) {
				mInstance = new OppoEmojiPlugin(context);
			}
		}
		return mInstance;
	}

	private OppoEmojiPlugin(Context context){
		//initEmojiChars();
		PackageManager packageManager = context.getPackageManager();
		ApplicationInfo appInfo = null;
		try {
			appInfo = packageManager.getApplicationInfo(PLUGIN_PACKAGE_NAME, 0);
			if (null == appInfo) {
				return;
			}
			mResources = packageManager.getResourcesForApplication(appInfo);
		} catch (NameNotFoundException e) {
			Log.e(TAG, "couldn't get resources");
		}
		if (null == appInfo || null == mResources) {
			return;
		}
		String dexPath = appInfo.sourceDir;
		String dexOutputDir = context.getApplicationInfo().dataDir;
		String libPath = appInfo.nativeLibraryDir;
		DexClassLoader loader = new DexClassLoader(dexPath, dexOutputDir,
				libPath, getClass().getClassLoader());
		try {
			mDrawableClass = loader.loadClass(DRAWABLE_ClASS_FULL_PATH);
		} catch (ClassNotFoundException e) {
			e.printStackTrace();
		}
	}

	private int getDrawableId(String resName) {
		if(DEBUG)Log.v(TAG, "getDrawableId(String resName) resName="+resName);
		if (mDrawableClass == null) {
			return 0;
		}

		Field field = null;
		int resId = -1;

		try {
			field = mDrawableClass.getField(resName);
			resId = (Integer) field.get(null);
		} catch (SecurityException e) {
			e.printStackTrace();
		} catch (NoSuchFieldException e) {
			e.printStackTrace();
		} catch (IllegalArgumentException e) {
			e.printStackTrace();
		} catch (IllegalAccessException e) {
			e.printStackTrace();
		}

		return resId;
	}

	private Drawable getEmojiDrawable(String resName) {
		if(DEBUG)Log.v(TAG, "getEmojiDrawable(String resName) resName="+resName);
		if (null == mDrawableClass || null == mResources) {
			Log.w(TAG, "OppoEmojiPlugin, NO drawable found ?");
			return null;
		}

		Drawable drawable = null;
		try {
			drawable = mResources.getDrawableForDensity(getDrawableId(resName),
					mResources.getDisplayMetrics().densityDpi);
		} catch (NotFoundException e) {
			e.printStackTrace();
		}

		return drawable;
	}

	// //////For emoji matching
	private static Pattern mMatchPattern;
	private static final int MAX_EMOJI_UNICODE = 0xe537;
	private static final int MIN_EMOJI_UNICODE = 0xe001;

	private static int getNextEmojiUnicode(int emojiUnicode) {
		int next = emojiUnicode;
		if (next == 0xe05a) {// row 1
			next = 0xe101;
		} else if (next == 0xe15a) {// row 2
			next = 0xe201;
		} else if (next == 0xe253) {// row 3
			next = 0xe301;
		} else if (next == 0xe34d) {
			next = 0xe401;
		} else if (next == 0xe44c) {// row 4
			next = 0xe501;
		} else {
			next++;
		}

		return next;
	}

	private static void initEmojiChars() {
		StringBuilder emojiReglex = new StringBuilder("(");

		for (int i = MIN_EMOJI_UNICODE; i <= MAX_EMOJI_UNICODE; i = getNextEmojiUnicode(i)) {
			String emojiChar = String.valueOf((char) i);
			if (i < MAX_EMOJI_UNICODE) {
				emojiReglex.append(emojiChar + "|");
			} else {
				emojiReglex.append(emojiChar + ")");
			}
		}

		mMatchPattern = Pattern.compile(emojiReglex.toString());
	}

    //init while class loaded
	static{
        initEmojiChars();
	}

	// /////////the related data of an emoji in String
	private ArrayList<EmojiItem> mEmojis = new ArrayList<EmojiItem>();

	private static class EmojiItem {
		public int mStart = 0;
		public int mEnd = 0;
		public String mUnicodeStr = "";

		public String toString() {
			return mUnicodeStr + " from:" + mStart + " to:" + mEnd;
		}
	}

	public CharSequence refreshEmojiOfText(CharSequence buffer, TransformationMethod transformation, CharSequence transformed, TextView view, Spannable.Factory spannableFactory) {
		if (null == buffer){
			return transformed;
		}
		if(DEBUG)Log.v(TAG, "refreshEmojiOfText()...mEmojis.size()="+mEmojis.size());
		final int prevSize = mEmojis.size();
		mEmojis.clear();

		// collecting EmojiItems
		String string = new String(buffer.toString());
		Matcher m = mMatchPattern.matcher(string);

		while (m.find()) {
			EmojiItem emoData = new EmojiItem();
			emoData.mStart = m.start();
			emoData.mEnd = m.end();
			int i = m.group().charAt(0);
			emoData.mUnicodeStr = Integer.toHexString(i);
			mEmojis.add(emoData);
			if(DEBUG)Log.v(TAG, "mEmojis.add(emoData) emoData="+emoData.toString());
		}

		// refresh emojis in text
		Spannable emoBuffer = null;
		if (mEmojis.size() > 0 || prevSize != 0) {// the amount of emoji changed!	
			for (EmojiItem emo : mEmojis) {
				Drawable d = getEmojiDrawable(emo.mUnicodeStr);

				if (d == null){
					Log.v(TAG, "Drawable d = getEmojiDrawable(emo.mUnicodeStr)....d == null");
					continue;
				}

				if (emoBuffer == null) {
					emoBuffer = spannableFactory.newSpannable(buffer);
				}

				d.setBounds(0, 0, d.getIntrinsicWidth(), d.getIntrinsicHeight());
				ImageSpan span = new ImageSpan(d, ImageSpan.ALIGN_BOTTOM);
				emoBuffer.setSpan(span, emo.mStart, emo.mEnd,
						Spannable.SPAN_EXCLUSIVE_INCLUSIVE);
			}
		}

		CharSequence text = (emoBuffer != null ? emoBuffer : buffer);
		if (transformation == null) {
			transformed = text;
		} else {
			transformed = transformation.getTransformation(text, view);
		}

		return transformed;
	}

    public static boolean containEmoji(CharSequence cs){        
        if(cs == null || cs.length() <= 0) return false;
        
        Matcher m = mMatchPattern.matcher(cs);
        
		if(m != null && m.find()) return true;		
        else return false;
    }
}
