/************************************************************************************
 ** File: - OppoControlVolumePolicy.java
 ** VENDOR_EDIT
 ** Copyright (C), 2008-2013, OPPO Mobile Comm Corp., Ltd
 ** 
 ** Description: 
 **      Add this file for screen capture!
 ** 
 ** Version: 0.1
 ** Date created: 2013-09-13
 ** Author: Jun.Zhang@Plf.Framework
 ** 
 ** --------------------------- Revision History: --------------------------------
 ** 	<author>	                    		<data>					<desc>
 ** 	Jun.Zhang@Plf.Framework 				2013-09-13     		Init
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
import android.os.PowerManager;
import android.media.AudioManager;
import android.media.IAudioService;
import android.view.GestureDetector;
import com.android.internal.telephony.ITelephony;
import android.app.KeyguardManager;
import android.os.Bundle;

import com.oppo.exserviceui.R;

@android.annotation.OppoHook(level=android.annotation.OppoHook.OppoHookType.NEW_CLASS,
		property=android.annotation.OppoHook.OppoRomType.ROM,
		note="Jun.Zhang@Plf.Framework add for screen shot")
public class OppoControlVolumePolicy{
    private static final String TAG = "OppoControlVolumePolicy";
	
	private static final int MSG_OPPO_SHOW_VOL_CONTROL = 10004;
    private static final int MSG_OPPO_HIDE_VOL_CONTROL = 10005;
	
	private static final int POS_SHOW_LEFT = 0;
	private static final int POS_SHOW_RIGHT = 1;
	
	private static final int SHOW_STREAMTYPE_SYSTEM = 0;
	private static final int SHOW_STREAMTYPE_MEDIA = 1;
	private static final int SHOW_STREAMTYPE_RINGTONE = 2;
	
	private Context mContext;
	private Handler mHandler;
	
	private int mCurNumPointers;
    private int mMaxNumPointers;
	
	private static final int MAX_POINT_NUM = 100;
	
	PowerManager mPowerManager;
	PowerManager.WakeLock mBroadcastWakeLock;
	
	private int mScreenHeight = -1;
	private int mScreenWidth = -1;
	
	private GestureDetector mGestureDetector;
	
	private AudioManager mAudiomanager;
	private int maxVolume = 16;
	
	private float allDifference = 0;
	private float oldDifference = 0;
	private float oldDy = 0;
	private int mPercent = 0;
	
	private int mPosStartX = 0;
	private int mPosStartY = 0;
	private int mPosCurrentX = 0;
	private int mPosCurrentY = 0;
	private int mPosMaxX = 0;
	private int mPosMinX = 0;
	
	private int mPosStartX_1 = 0;
	private int mPosStartY_1 = 0;
	private int mPosCurrentX_1 = 0;
	private int mPosCurrentY_1 = 0;
	private int mPosMaxX_1 = 0;
	private int mPosMinX_1 = 0;
	
	private boolean mShouldVolumeCtrl = false;
	
    public OppoControlVolumePolicy(Context c, Handler handler) {
		mContext = c;
		mHandler = handler;
		
		mPowerManager = (PowerManager)mContext.getSystemService(Context.POWER_SERVICE);
        mBroadcastWakeLock = mPowerManager.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK,
                "OppoControlVolumePolicy.mBroadcastWakeLock");
				
		DisplayMetrics displayMetrics = new DisplayMetrics();
		WindowManager wm = (WindowManager)mContext.getSystemService(Context.WINDOW_SERVICE);          
		wm.getDefaultDisplay().getMetrics(displayMetrics);	
		mScreenHeight = Math.max(displayMetrics.widthPixels, displayMetrics.heightPixels);
		mScreenWidth = Math.min(displayMetrics.widthPixels, displayMetrics.heightPixels);

		mGestureDetector = new GestureDetector(mContext, new MyGestureListener());
		mGestureDetector.setIsLongpressEnabled(false);

		mAudiomanager = (AudioManager) mContext.getSystemService(Context.AUDIO_SERVICE);
		//maxVolume = mAudiomanager.getStreamMaxVolume(AudioManager.STREAM_MUSIC);
		
		mContext.registerReceiver(mScreenOffBroadcastReceiver, new IntentFilter(Intent.ACTION_SCREEN_OFF));
    }
	
    public void addPointerEvent(MotionEvent event) {
		if (isKeyguardLocked()){
			return;
		}
	
		mGestureDetector.onTouchEvent(event);
	
		final int action = event.getAction();
        if (action == MotionEvent.ACTION_DOWN
                || (action & MotionEvent.ACTION_MASK) == MotionEvent.ACTION_POINTER_DOWN) {
			if (action == MotionEvent.ACTION_DOWN) {
				mCurNumPointers = 0;
				mMaxNumPointers = 0;
			}
			
			if (mCurNumPointers == 2){
				hideVolumeView();
				mShouldVolumeCtrl = false;
			}
			
			mCurNumPointers += 1;
		}
		if ((action & MotionEvent.ACTION_MASK) == MotionEvent.ACTION_POINTER_DOWN 
				&& mCurNumPointers == 2){
			mPosStartX = (int)event.getX(0);
			mPosStartY = (int)event.getY(0);
			
			mPosStartX_1 = (int)event.getX(1);
			mPosStartY_1 = (int)event.getY(1);
			
			mPosMaxX = mPosStartX;
			mPosMinX = mPosStartX;
			mPosMaxX_1 = mPosStartX_1;
			mPosMinX_1 = mPosStartX_1;
		}
		
		if (mMaxNumPointers < mCurNumPointers) {
			mMaxNumPointers = mCurNumPointers;
		}
		
		if (action == MotionEvent.ACTION_UP
                || action == MotionEvent.ACTION_CANCEL
                || (action & MotionEvent.ACTION_MASK) == MotionEvent.ACTION_POINTER_UP) {
            final int index = (action & MotionEvent.ACTION_POINTER_INDEX_MASK)
                    >> MotionEvent.ACTION_POINTER_INDEX_SHIFT; // will be 0 for UP

            if (action == MotionEvent.ACTION_UP
                    || action == MotionEvent.ACTION_CANCEL) {
                mCurNumPointers = 0;
            } else {
				// two pointers
				if(mCurNumPointers == 2){
					hideVolumeView();
					resumeDeliverPointer();
					mShouldVolumeCtrl = false;
				}
                mCurNumPointers -= 1;
            }
        }
		
		if (action == MotionEvent.ACTION_MOVE){
			if(mCurNumPointers == 2){
				mPosCurrentX = (int)event.getX(0);
				mPosCurrentY = (int)event.getY(0);
				
				mPosCurrentX_1 = (int)event.getX(1);
				mPosCurrentY_1 = (int)event.getY(1);
				
				if (mPosMaxX < mPosCurrentX){
					mPosMaxX = mPosCurrentX;
				}
				if (mPosMaxX_1 < mPosCurrentX_1){
					mPosMaxX_1 = mPosCurrentX_1;
				}
				if (mPosMinX > mPosCurrentX){
					mPosMinX = mPosCurrentX;
				}
				if (mPosMinX_1 > mPosCurrentX_1){
					mPosMinX_1 = mPosCurrentX_1;
				}
				
				// two pointer must not far away
				if (Math.abs(mPosCurrentX_1 - mPosCurrentX) >  mScreenWidth/2){
					mPosStartX = (int)event.getX(0);
					mPosStartY = (int)event.getY(0);
			
					mPosStartX_1 = (int)event.getX(1);
					mPosStartY_1 = (int)event.getY(1);
				}
				
				// should move
				if (shouldControlVolume()){
					pauseDeliverPointer();
					mShouldVolumeCtrl = true;
				}
			}
		}
    }

	boolean shouldControlVolume(){
		return Math.abs(mPosCurrentY - mPosStartY) > mScreenHeight/10 
						&& Math.abs(mPosCurrentX - mPosStartX) < mScreenWidth/20
						&& Math.abs(mPosCurrentY_1 - mPosStartY_1) > mScreenHeight/10 
						&& Math.abs(mPosCurrentX_1 - mPosStartX_1) < mScreenWidth/20
						&& ((mPosCurrentY_1 - mPosStartY_1)^(mPosCurrentY - mPosStartY)) > 0
						&& mPosMaxX - mPosMinX < mScreenWidth/20
						&& mPosMaxX_1 - mPosMinX_1 < mScreenWidth/20;
	}
	
	public void pauseDeliverPointer(){
		Log.i(TAG, "pauseDeliverPointer");	
		OppoScreenShotUtil.pauseDeliverPointerEvent();
	}
	
	public void resumeDeliverPointer(){
		//Log.i(TAG, "resumeDeliverPointer");
		OppoScreenShotUtil.resumeDeliverPointerEvent();
	}
	
	//------------------------------------------------------------------------------------
	private class MyGestureListener extends GestureDetector.SimpleOnGestureListener {
		public boolean onDown(MotionEvent e) {
			Log.d(TAG, "movie onDown");
			return false;
		}

		public boolean onScroll(MotionEvent arg0, MotionEvent arg1, float dx, float dy) {
			if (!mShouldVolumeCtrl){
				return false;
			}

			float difference = 0;
			difference = dy / mScreenHeight;
					if (oldDy != arg0.getY()) {
						allDifference = 0;
						oldDifference = 0;
						oldDy = arg0.getY();
					}
			mPercent = setVolumeLocal(difference*3);

			showVolumeView(mPercent, ((mPosCurrentX + mPosStartX_1)/2 > mScreenWidth/2)? POS_SHOW_LEFT : POS_SHOW_RIGHT);
			return false;
		}
	}

	private int getStreamType(){
	     ITelephony telephonyService = getTelephonyService();
         if (telephonyService != null) {
			try {
				if (telephonyService.isOffhook()) {
					return AudioManager.STREAM_VOICE_CALL;
				}
			} catch (RemoteException ex) {
				Log.w(TAG, "ITelephony threw RemoteException", ex);
			}
		}

		if (isMusicActive()) {
			return AudioManager.STREAM_MUSIC;
		} else if (isFMActive()) {
			return AudioManager.STREAM_FM;
		}
		return AudioManager.STREAM_RING;
	}
	
	static ITelephony getTelephonyService() {
        return ITelephony.Stub.asInterface(
                ServiceManager.checkService(Context.TELEPHONY_SERVICE));
    }
	
	 /**
     * @return Whether music is being played right now.
     */
    boolean isMusicActive() {
        final AudioManager am = (AudioManager)mContext.getSystemService(Context.AUDIO_SERVICE);
        if (am == null) {
            Log.w(TAG, "isMusicActive: couldn't get AudioManager reference");
            return false;
        }
        return am.isMusicActive();
    }

    /**
    * @return Whether FM is being played right now.
    */
    boolean isFMActive() {
/*        final AudioManager am = (AudioManager)mContext.getSystemService(Context.AUDIO_SERVICE);
        if (am == null) {
            Log.w(TAG, "isFMActive: couldn't get AudioManager reference");
            return false;
        }
        return am.isFMActive();*/
		return false;
    }
	
	/**
	 * set volume for media
	 * 
	 * @param difference
	 *            the difference from former volume
	 * @return the percent of volume
	 */
	private int setVolumeLocal(float difference) {
		final int nStreamType = getStreamType();
		maxVolume = mAudiomanager.getStreamMaxVolume(nStreamType);
		
		int oldVolume = mAudiomanager.getStreamVolume(nStreamType);
		/*
		 * when reduce volume, we should first add 1, because when changed to
		 * int, it will lose the decimal
		 */
		float currentVol = oldVolume + difference * maxVolume + (difference < 0 ? 1 : 0);

		/*
		 * if volume do not change, if sign of difference and oldDifference is
		 * the same, we should add the difference into allDifference, otherwise
		 * set allDifference to 0
		 */
		if ((int) currentVol == oldVolume) {
			if (oldDifference * difference > 0) {
				allDifference += difference;
				currentVol = oldVolume + allDifference * maxVolume + (allDifference < 0 ? 1 : 0);
				if ((int) currentVol != oldVolume) {
					allDifference = 0;
				}
			} else {
				allDifference = difference;
			}
		} else {
			allDifference = 0;
		}

		oldDifference = difference;

		if (currentVol < 0) {
			currentVol = 0;
		} else if (currentVol > maxVolume) {
			currentVol = maxVolume;
		}
		mAudiomanager.setStreamVolume(nStreamType, (int) currentVol, 0);

		/*
		 * when reduce volume, we should first minus 1, namely if currentVol
		 * less than 1, it will be set 0
		 */
		if (difference < 0 && currentVol < 1) {
			currentVol = 0;
		}

		return (int) (currentVol * 100 / maxVolume);
	}

	int getShowStreamType(){
		final int streamType = getStreamType();
		int showStreamType = SHOW_STREAMTYPE_SYSTEM;
		switch(streamType){
			case AudioManager.STREAM_RING:
				showStreamType = SHOW_STREAMTYPE_SYSTEM;
				break;
			case AudioManager.STREAM_VOICE_CALL:
				showStreamType = SHOW_STREAMTYPE_RINGTONE;
				break;
			case AudioManager.STREAM_MUSIC:
			case AudioManager.STREAM_FM:
				showStreamType = SHOW_STREAMTYPE_MEDIA;
			break;
			default:
				break;
		}
		return showStreamType;
	}
	
	void showVolumeView(int percent, int pos){
		Message msg = new Message();
		msg.what = MSG_OPPO_SHOW_VOL_CONTROL;
		Bundle mMessageBundle = new Bundle();
		mMessageBundle.putInt("percent", percent);
		mMessageBundle.putInt("pos", pos);
		mMessageBundle.putInt("streamType", getShowStreamType());
		msg.setData(mMessageBundle);
		mHandler.sendMessage(msg);
	}
	
	void hideVolumeView(){
		Message msg = new Message();
		msg.what = MSG_OPPO_HIDE_VOL_CONTROL;
		mHandler.sendMessage(msg);
	}
	
	private boolean isKeyguardLocked(){
		KeyguardManager mKeyguardManager = (KeyguardManager) mContext.getSystemService(mContext.KEYGUARD_SERVICE); 
		return mKeyguardManager.isKeyguardLocked();
	}
	
	private BroadcastReceiver mScreenOffBroadcastReceiver = new BroadcastReceiver() {
        public void onReceive(Context context, Intent intent) {
			hideVolumeView();
			resumeDeliverPointer();
			mShouldVolumeCtrl = false;
        }
    };
}
