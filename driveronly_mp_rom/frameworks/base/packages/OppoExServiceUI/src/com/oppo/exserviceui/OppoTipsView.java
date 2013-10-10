/************************************************************************************
 ** File: - OppoTipsView.java
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
import android.view.InputDevice;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.VelocityTracker;
import android.view.View;
import android.view.ViewConfiguration;
import android.view.MotionEvent.PointerCoords;

import android.graphics.Point;
import android.content.res.Configuration;
import android.util.DisplayMetrics;
import android.app.ActivityManager;
import android.content.ComponentName;
import android.view.IWindowManager;
import android.os.RemoteException;

import android.view.WindowManager;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.widget.LinearLayout;
import android.widget.ImageView;
import android.widget.TextView;
import android.os.SystemProperties;
import android.content.BroadcastReceiver;
import android.content.Intent;
import android.content.IntentFilter;
import com.oppo.exserviceui.R;

@android.annotation.OppoHook(level=android.annotation.OppoHook.OppoHookType.NEW_CLASS,
		property=android.annotation.OppoHook.OppoRomType.ROM,
		note="Jun.Zhang@Plf.Framework add for screen shot")
public class OppoTipsView extends LinearLayout {
    private static final String TAG = "OppoTipsView";
	
	private static final int TEXT_COLOR = 0xcc259932;
	
	//record pointers
	private Context mContext;
    private LinearLayout mShotUpView;
    private LinearLayout mShotDownView;
    private TextView mUpTextView;
    private TextView mDownTextView;
	private int mScreenHeight = -1;
	private int mScreenWidth = -1;

    public OppoTipsView(Context c) {
        super(c);
		mContext = c;

		
		DisplayMetrics displayMetrics = new DisplayMetrics();
		WindowManager wm = (WindowManager)mContext.getSystemService(Context.WINDOW_SERVICE);          
		wm.getDefaultDisplay().getMetrics(displayMetrics);	
		mScreenHeight = Math.max(displayMetrics.widthPixels, displayMetrics.heightPixels);
		mScreenWidth = Math.min(displayMetrics.widthPixels, displayMetrics.heightPixels);	

        createShotTips();
    }


    private void createShotTips() {
        final float scale = mContext.getResources().getDisplayMetrics().density;
        final int textColor = TEXT_COLOR;
        final int textSize = (int)(17 * scale + 0.5f);
        final int textPadding = (int)(8 * scale + 0.5f);
        //Log.d(TAG, "scale=[ " + scale + " ] " + "textSize=[ " + textSize + " ]");
           
        // up
        mShotUpView = new LinearLayout(mContext);
        mShotUpView.setGravity(Gravity.CENTER);
        mShotUpView.setOrientation(VERTICAL);
        //mShotUpView.setLayoutParams(lp);
        
        ImageView upImageView = new ImageView(mContext);
        upImageView.setImageDrawable(mContext.getResources().getDrawable(R.drawable.oppo_screen_shot_up_pic));
        
        mUpTextView = new TextView(mContext);
        mUpTextView.setGravity(Gravity.CENTER);
        mUpTextView.setText(R.string.oppo_screen_shot_up_text);
        mUpTextView.setTextColor(textColor);
        mUpTextView.setPadding(0, 0, 0, textPadding);
        
        mShotUpView.addView(mUpTextView, 0);
        mShotUpView.addView(upImageView, 1);            
        
        // down
        mShotDownView = new LinearLayout(mContext);
        mShotDownView.setGravity(Gravity.CENTER);
        mShotDownView.setOrientation(VERTICAL);
        //mShotDownView.setLayoutParams(lp);           
        
        ImageView downImageView = new ImageView(mContext);
        downImageView.setImageDrawable(mContext.getResources().getDrawable(R.drawable.oppo_screen_shot_down_pic));
        
        mDownTextView = new TextView(mContext);
        mDownTextView.setGravity(Gravity.CENTER);
        mDownTextView.setText(R.string.oppo_screen_shot_down_text);
        mDownTextView.setTextColor(textColor);
        mDownTextView.setPadding(0, 0, 0, textPadding);
        
        mShotDownView.addView(downImageView, 0);
        mShotDownView.addView(mDownTextView, 1);
        
        // this
        setGravity(Gravity.CENTER);
        setOrientation(VERTICAL);
        addView(mShotUpView, 0);
        addView(mShotDownView, 1);
        
		mShotUpView.setVisibility(View.VISIBLE);
        mShotDownView.setVisibility(View.VISIBLE);  
        //hide all
        hideShotTips();
    }
    
    public void showShotTips(boolean isUpShot) {
		Log.i(TAG, "showShotTips------------- isUpShot=" + isUpShot);
        if (isUpShot) {
            mUpTextView.setText(R.string.oppo_screen_shot_up_text);
            mShotUpView.setVisibility(View.VISIBLE);            
        } else {
            mDownTextView.setText(R.string.oppo_screen_shot_down_text);
            mShotDownView.setVisibility(View.VISIBLE);
        }
        setVisibility(View.VISIBLE);
    }
    
    public void hideShotTips() {
        mShotUpView.setVisibility(View.INVISIBLE);
        mShotDownView.setVisibility(View.INVISIBLE);
        setVisibility(View.INVISIBLE);
    }
}
