/************************************************************************************
 ** File: - OppoThreePointersShotPolicy.java
 ** VENDOR_EDIT
 ** Copyright (C), 2008-2013, OPPO Mobile Comm Corp., Ltd
 ** 
 ** Description: 
 **      Add this file for screen capture!
 ** 
 ** Version: 0.1
 ** Date created: 2013-08-03
 ** Author: Jun.Zhang@Plf.Framework
 ** 
 ** --------------------------- Revision History: --------------------------------
 ** 	<author>	                    		<data>					<desc>
 ** 	Jun.Zhang@Plf.Framework 				2013-08-03      		Init
 ************************************************************************************/
 
package com.oppo.exserviceui;

import android.content.Context;
import android.util.Log;
import android.view.ViewConfiguration;
import android.view.MotionEvent;
import android.view.MotionEvent.PointerCoords;
import java.util.ArrayList;

import android.content.Intent;
import android.view.OppoScreenShotUtil;
import java.util.Collections;
import java.util.Comparator;
import android.graphics.Point;
import android.content.res.Configuration;
import android.util.DisplayMetrics;
import android.app.StatusBarManager;
import android.app.ActivityManager;
import android.content.ComponentName;
import android.view.OrientationEventListener;
import android.os.ServiceManager;
import android.view.IWindowManager;
import android.os.RemoteException;

import android.view.WindowManager;
import android.view.LayoutInflater;
import android.os.SystemProperties;
import android.content.BroadcastReceiver;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Handler;
import android.os.Message;
import com.oppo.content.OppoIntent;
import com.oppo.exserviceui.R;

@android.annotation.OppoHook(level=android.annotation.OppoHook.OppoHookType.NEW_CLASS,
		property=android.annotation.OppoHook.OppoRomType.ROM,
		note="Jun.Zhang@Plf.Framework add for screen shot")
public class OppoThreePointersShotPolicy{
    private static final String TAG = "OppoThreePointersShotPolicy";
	
	private static final int MSG_OPPO_SHOW_TIP = 10002;
    private static final int MSG_OPPO_HIDE_TIP = 10003;
	
	// screen shot
	private static final String ACTION_SCREEN_SHOT = OppoIntent.ACTION_SCREEN_SHOT;

    private boolean mCurDown;
    private int mCurNumPointers;
    private int mMaxNumPointers;
    private int mActivePointerId;

	//record pointers
	private Context mContext;
	private Handler mHandler;
	private ArrayList<Point> mPointers0 = new ArrayList<Point>();
	private ArrayList<Point> mPointers1 = new ArrayList<Point>();
	private ArrayList<Point> mPointers2 = new ArrayList<Point>();
	private static final int MAX_POINT_NUM = 100;
	
	private ShotOrientationEventListener mShotOrientationListener;
	private boolean mIsEnableSensor = false;
	private boolean mIsSensorOrientationPort = true;
	
	private boolean mIsShotScreenEnable = false;
	private static final int SCREEN_SHOT_UP = -1;
    private static final int SCREEN_SHOT_DOWN = 1;
    private static final int SCREEN_SHOT_PRECISE = 5;
    private int mFirstShotY = -1;
    private Point mLast1ShotY;
    private Point mLast2ShotY;
       
    private static final String SCREEN_SHOT_TIMES_PROPERTY = "persist.sys.screenshot.times";
    private static final int SCREEN_SHOT_TIMES = 3;
    private boolean mNeedShowTips = true;
    private boolean mTipShowing = false;

	private int mScreenHeight = -1;
	private int mScreenWidth = -1;

    public OppoThreePointersShotPolicy(Context c, Handler handler) {
		mContext = c;
		mHandler = handler;
		
		DisplayMetrics displayMetrics = new DisplayMetrics();
		WindowManager wm = (WindowManager)mContext.getSystemService(Context.WINDOW_SERVICE);          
		wm.getDefaultDisplay().getMetrics(displayMetrics);	
		mScreenLength_1_3 = Math.max(displayMetrics.widthPixels, displayMetrics.heightPixels) / 3;
		mScreenWidth_1_3 = Math.min(displayMetrics.widthPixels, displayMetrics.heightPixels) / 3;

		mScreenHeight = Math.max(displayMetrics.widthPixels, displayMetrics.heightPixels);
		mScreenWidth = Math.min(displayMetrics.widthPixels, displayMetrics.heightPixels);	

        createShotTips();
        mContext.registerReceiver(mScreenOffBroadcastReceiver, new IntentFilter(Intent.ACTION_SCREEN_OFF));
		
    }
	
	private int getScreenWidth_1_3() {
		int orient = mContext.getResources().getConfiguration().orientation;  
		if (Configuration.ORIENTATION_LANDSCAPE == orient) {
			return mScreenLength_1_3;
		} else /*if (Configuration.ORIENTATION_PORTRAIT == orient)*/ {
			return mScreenWidth_1_3;
		}
	}

	private int getScreenHeight_1_3() {
		int orient = mContext.getResources().getConfiguration().orientation;  
		if (Configuration.ORIENTATION_LANDSCAPE == orient) {
			return mScreenWidth_1_3;
		} else /*if (Configuration.ORIENTATION_PORTRAIT == orient)*/ {
			return mScreenLength_1_3;
		}
	}

	int mScreenLength_1_3 = -1;
	int mScreenWidth_1_3 = -1;

    public void addPointerEvent(MotionEvent event) {
		if (!mIsShotScreenEnable){
			return;
		}
		
        final int action = event.getAction();

        if (action == MotionEvent.ACTION_DOWN
                || (action & MotionEvent.ACTION_MASK) == MotionEvent.ACTION_POINTER_DOWN) {
            final int index = (action & MotionEvent.ACTION_POINTER_INDEX_MASK)
                    >> MotionEvent.ACTION_POINTER_INDEX_SHIFT; // will be 0 for down
            if (action == MotionEvent.ACTION_DOWN) {
                mCurDown = true;
                mCurNumPointers = 0;
                mMaxNumPointers = 0;
				
				//
				mPointers0.clear();
				mPointers1.clear();
				mPointers2.clear();
				
				//
				resumeDeliverPointer();
            }

            mCurNumPointers += 1;
			
			// add for three touch 
			if (mCurNumPointers == 3){
                if(mFirstShotY == -1) mFirstShotY = (int)event.getY(0);                
				//pause deliver pointer event to view                
				pauseDeliverPointer();
                showShotTips();
			} else {
				//resume deliver pointer event to view
				resumeDeliverPointer();
                hideShotTips();
			}
			
			if (mMaxNumPointers < mCurNumPointers) {
                mMaxNumPointers = mCurNumPointers;
            }
		}
        final int NI = event.getPointerCount();
		
		for (int i = 0; i < NI; i++){
			recordPointers(event, i, mMaxNumPointers);
		}

        if (action == MotionEvent.ACTION_UP
                || action == MotionEvent.ACTION_CANCEL
                || (action & MotionEvent.ACTION_MASK) == MotionEvent.ACTION_POINTER_UP) {
            final int index = (action & MotionEvent.ACTION_POINTER_INDEX_MASK)
                    >> MotionEvent.ACTION_POINTER_INDEX_SHIFT; // will be 0 for UP

            if (action == MotionEvent.ACTION_UP
                    || action == MotionEvent.ACTION_CANCEL) {
                mCurDown = false;
                mCurNumPointers = 0;
            } else {
				// three pointers shot screen
				if(mCurNumPointers == 3){
					//screen shot 
					if (shouldShot()){                        
                        final int direction = mLast1ShotY.y > mLast2ShotY.y ? SCREEN_SHOT_UP : SCREEN_SHOT_DOWN;
                        Log.d(TAG, "direction = " + direction + "--" + mLast1ShotY.y + "--" + mLast2ShotY.y);					

						sendScreenShot(direction);
					}
					resumeDeliverPointer();
                    hideShotTips();
				}
				
                mCurNumPointers -= 1;
            }
        }
    }

	private void sendScreenShot(int direction){
		Log.i(TAG, "sendScreenShot send ACTION_SCREEN_SHOT");
		Intent intent = new Intent(ACTION_SCREEN_SHOT);
        intent.putExtra("direction", direction);
		mContext.sendBroadcast(intent);
	}
	
	public void pauseDeliverPointer(){
		Log.i(TAG, "pauseDeliverPointer");	
		OppoScreenShotUtil.pauseDeliverPointerEvent();
	}
	
	public void resumeDeliverPointer(){
		//Log.i(TAG, "resumeDeliverPointer");
		OppoScreenShotUtil.resumeDeliverPointerEvent();
	}
	
	private void recordPointers(MotionEvent event, int pointIndex, int maxNumPointers){
		if (maxNumPointers == 3){
			Point pointTemp = new Point();
			final int pointId = event.getPointerId(pointIndex);
			if (pointId == 0){
				pointTemp.x = (int)event.getX(pointIndex);
				pointTemp.y = (int)event.getY(pointIndex);
				if (mPointers0.size() > MAX_POINT_NUM){
					mPointers0.remove(0);
				}
				mPointers0.add(pointTemp);

			} else if (pointId == 1){
				pointTemp.x = (int)event.getX(pointIndex);
				pointTemp.y = (int)event.getY(pointIndex);
				if (mPointers1.size() > MAX_POINT_NUM){
					mPointers1.remove(0);
				}
				mPointers1.add(pointTemp);
			} else if (pointId == 2){
				pointTemp.x = (int)event.getX(pointIndex);
				pointTemp.y = (int)event.getY(pointIndex);
				if (mPointers2.size() > MAX_POINT_NUM){
					mPointers2.remove(0);
				}
				mPointers2.add(pointTemp);
			}
		}
	}
	
	private boolean shouldShot(){
        // get last 2 points
        if (mPointers0.size() > 0) {
            int last2 = mPointers0.size() -1;
            int last1 = last2 - SCREEN_SHOT_PRECISE;
            last1 = last1 > 0 ? last1 : 0;
            last2 = last2 > 0 ? last2 : 0;
            mLast1ShotY = mPointers0.get(last1);
            mLast2ShotY = mPointers0.get(last2);
        }
		return shouldShotByPoints(mPointers0) && shouldShotByPoints(mPointers1) && shouldShotByPoints(mPointers2);
	}
	
	private boolean shouldShotByPoints(ArrayList<Point> pointers){
		if (pointers.size() < 0 || pointers.size() == 0){
			return false;
		}

		// get Y max and min point
		YComparator yComparator = new YComparator();
		Collections.sort(pointers, yComparator);
		Point maxPCY = Collections.max(pointers, yComparator);
		Point minPCY = Collections.min(pointers, yComparator);
		Log.d(TAG, " maxPCY.y" + maxPCY.y + " minPCY.y" + minPCY.y + "pointers" + pointers.size());
		
		//// get X max and min point
		XComparator xComparator = new XComparator();
		Collections.sort(pointers, xComparator);
		Point maxPCX = Collections.max(pointers, xComparator);
		Point minPCX = Collections.min(pointers, xComparator);
		Log.d(TAG, " maxPCX.x" + maxPCX.x + " minPCX.x" + minPCX.x + "pointers" + pointers.size());
		
		//Log.d(TAG, " --------- getWidth()" + getWidth() + "  getHeight()" + getHeight());
		boolean bResult = false;
		if (mIsEnableSensor){
			// camera screen
			boolean mIsOrientationPort = (mContext.getResources().getConfiguration().orientation == Configuration.ORIENTATION_PORTRAIT);
			if (mIsSensorOrientationPort == mIsOrientationPort){
				bResult = (Math.abs(maxPCY.y - minPCY.y) > getScreenHeight_1_3() && Math.abs(maxPCX.x - minPCX.x) < 2*getScreenWidth_1_3());
			} else {
				bResult = (Math.abs(maxPCY.y - minPCY.y) < 2*getScreenHeight_1_3() && Math.abs(maxPCX.x - minPCX.x) > getScreenWidth_1_3());
			}	
		} else {
			bResult = (Math.abs(maxPCY.y - minPCY.y) > getScreenHeight_1_3() && Math.abs(maxPCX.x - minPCX.x) < 2*getScreenWidth_1_3());
		}
		return bResult;
	}
	
	class XComparator implements Comparator<Point> {
		public int compare(Point o1, Point o2) {
			return (o1.x < o2.x ? -1 : (o1.x == o2.x ? 0 : 1));
		}
	}
	
	class YComparator implements Comparator<Point> {
		public int compare(Point o1, Point o2) {
			return (o1.y < o2.y ? -1 : (o1.y == o2.y ? 0 : 1));
		}
	}

	// sensor listen get phone pos
	private class ShotOrientationEventListener extends OrientationEventListener{
	
        public ShotOrientationEventListener(Context context) {
            super(context);
        }
		
		public void onOrientationChanged(int orientation){
			//Log.d(TAG, " orientation = " + orientation);
			if ((orientation > 45 && orientation < 135) || (orientation > 225 && orientation < 315)){
				mIsSensorOrientationPort = false;
			} else {
				mIsSensorOrientationPort = true;
			}
		}
	}
	
	//set sensor on or off
	public void setSensorMode(boolean enable){
		Log.d(TAG, "setSensorMode enable = " + enable + " mIsEnableSensor = " + mIsEnableSensor);
		if (mIsEnableSensor == enable){
			return;
		}
		
		if (mShotOrientationListener == null){
			mShotOrientationListener = new ShotOrientationEventListener(mContext);
		}
		if (enable){ 
			mShotOrientationListener.enable();
		} else {
			mShotOrientationListener.disable();
		}
		
		mIsEnableSensor = enable;
	}

	//set screen shot on or off
	public void setScreenShotEnable(boolean enable){
		mIsShotScreenEnable = enable;
	}

    private void createShotTips() {
        int times = SystemProperties.getInt(SCREEN_SHOT_TIMES_PROPERTY, 0);
        if(times >= SCREEN_SHOT_TIMES) {
            Log.i(TAG, "Dont show anim. - " + times);
            mNeedShowTips = false;
            return;
        }
        mTipShowing = true;       
        //hide all
        hideShotTips();
    }
    
    private void showShotTips() {
        if (!mNeedShowTips) {
            return;
        }
          
        int times = SystemProperties.getInt(SCREEN_SHOT_TIMES_PROPERTY, 0);
        if(times >= SCREEN_SHOT_TIMES) {
            Log.i(TAG, "Dont show anim. " + times);
            mNeedShowTips = false;
            return;
        } else {
            SystemProperties.set(SCREEN_SHOT_TIMES_PROPERTY, times + 1 + "");
        }

        Message msg = new Message();
		msg.what = MSG_OPPO_SHOW_TIP;
		msg.arg1 = ((mFirstShotY > mScreenHeight / 2 )? 1 : 0);
		mHandler.sendMessage(msg);
        mTipShowing = true;
    }
    
    private void hideShotTips() {
        if (!mNeedShowTips || !mTipShowing) {
            return;
        }
        mFirstShotY = -1;
        mTipShowing = false;

		Message msg = new Message();
		msg.what = MSG_OPPO_HIDE_TIP;
		mHandler.sendMessage(msg);
    }
    
    private BroadcastReceiver mScreenOffBroadcastReceiver = new BroadcastReceiver() {
        public void onReceive(Context context, Intent intent) {
            Log.d(TAG, "mScreenOffBroadcastReceiver");
            hideShotTips();
			
			mHandler.post(mScreenOffRunnable);
        }
    };
	
	private final Runnable mScreenOffRunnable = new Runnable() {
        public void run() {
			// resume pointer deliver
			resumeDeliverPointer();
			
			// disable sensor
			setSensorMode(false);
        }
    };
}
