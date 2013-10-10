/************************************************************************************
 ** File: - OppoVolumeView.java
 ** VENDOR_EDIT
 ** Copyright (C), 2008-2013, OPPO Mobile Comm Corp., Ltd
 ** 
 ** Description: 
 **      Add this file for volume control!
 ** 
 ** Version: 0.1
 ** Date created: 2013-09-13
 ** Author: Jun.Zhang@Plf.Framework
 ** 
 ** --------------------------- Revision History: --------------------------------
 ** 	<author>	                    		<data>					<desc>
 ** 	Jun.Zhang@Plf.Framework 				2013-09-13      		Init
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
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import com.oppo.exserviceui.R;

@android.annotation.OppoHook(level=android.annotation.OppoHook.OppoHookType.NEW_CLASS,
		property=android.annotation.OppoHook.OppoRomType.ROM,
		note="Jun.Zhang@Plf.Framework add for screen shot")
public class OppoVolumeView extends LinearLayout {
    private static final String TAG = "OppoVolumeView";
	
	private static final int TEXT_COLOR = 0xccda7305;
	
	private static final int SHOW_STREAMTYPE_SYSTEM = 0;
	private static final int SHOW_STREAMTYPE_MEDIA = 1;
	private static final int SHOW_STREAMTYPE_RINGTONE = 2;
	
	//record pointers
	private Context mContext;

	View mVolumeAdjustView;
	private ImageView mVolumeBrightViewRight;
	private TextView mVolBrightPercent;
	private TextView mVolBrightText;
	//zhangyd add for seekbar
	private OppoVerticalSeekBar mOppoVerticalSeekBarRight; 

    private ImageView mVolumeBrightViewLeft;
	//zhangyd add for seekbar
	private OppoVerticalSeekBar mOppoVerticalSeekBarLeft; 
	private Drawable mDrawableVolumeOn = null;
	private Drawable mDrawableVolumeOff = null;
	private Drawable mDrawableVolumeOffSystem = null;
	
	private int mScreenHeight = -1;
	private int mScreenWidth = -1;

    public OppoVolumeView(Context c) {
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
        LayoutInflater inflater =  LayoutInflater.from(mContext);  
        mVolumeAdjustView = inflater.inflate(R.layout.volume_view, this);  

        mVolumeBrightViewRight= (ImageView)mVolumeAdjustView.findViewById(R.id.volume_bright_right);
		mDrawableVolumeOn = mContext.getResources().getDrawable(R.drawable.volume);
		mDrawableVolumeOff = mContext.getResources().getDrawable(R.drawable.volume_off);
		mDrawableVolumeOffSystem = mContext.getResources().getDrawable(R.drawable.volume_off_system);
        mVolumeBrightViewRight.setImageDrawable(mDrawableVolumeOn);
        
        mVolBrightPercent = (TextView)mVolumeAdjustView.findViewById(R.id.percent);
        mVolBrightPercent.setText(R.string.oppo_screen_shot_up_text);
        mVolBrightPercent.setPadding(0, 0, 0, textPadding);

        mVolBrightText = (TextView)mVolumeAdjustView.findViewById(R.id.text);
        mVolBrightText.setGravity(Gravity.RIGHT);

        mOppoVerticalSeekBarRight = (OppoVerticalSeekBar)mVolumeAdjustView.findViewById(R.id.seekbar_right);

        //left View
        mVolumeBrightViewLeft = (ImageView)mVolumeAdjustView.findViewById(R.id.volume_bright_left);
        mVolumeBrightViewLeft.setImageDrawable(mDrawableVolumeOn);

        mOppoVerticalSeekBarLeft = (OppoVerticalSeekBar)mVolumeAdjustView.findViewById(R.id.seekbar_left);  

        //hide all
        hideVolumeTips();
    }
    
    public void showVolumeTips(Bundle bundle) {
		int percent = bundle.getInt("percent");
		int pos = bundle.getInt("pos");
		int streamType = bundle.getInt("streamType");

		boolean isLeft = (pos == 0)? true: false;
		if(0 == percent) {
			if (SHOW_STREAMTYPE_SYSTEM == streamType){
				mVolumeBrightViewLeft.setImageDrawable(mDrawableVolumeOffSystem);
				mVolumeBrightViewRight.setImageDrawable(mDrawableVolumeOffSystem);
			} else {
				mVolumeBrightViewLeft.setImageDrawable(mDrawableVolumeOff);
				mVolumeBrightViewRight.setImageDrawable(mDrawableVolumeOff);
			}
		} else {
            mVolumeBrightViewLeft.setImageDrawable(mDrawableVolumeOn);
            mVolumeBrightViewRight.setImageDrawable(mDrawableVolumeOn);
		}
        if (isLeft) {
			mOppoVerticalSeekBarLeft.setProgress(percent);
			mOppoVerticalSeekBarRight.setVisibility(View.INVISIBLE);
			mVolumeBrightViewRight.setVisibility(View.INVISIBLE);
			
			mOppoVerticalSeekBarLeft.setVisibility(View.VISIBLE);
			mVolumeBrightViewLeft.setVisibility(View.VISIBLE);
			
			mVolBrightPercent.setGravity(Gravity.LEFT);
			mVolBrightText.setGravity(Gravity.LEFT);			
        } else {
			mOppoVerticalSeekBarRight.setProgress(percent);
			mOppoVerticalSeekBarLeft.setVisibility(View.INVISIBLE);
			mVolumeBrightViewLeft.setVisibility(View.INVISIBLE);
			
			mOppoVerticalSeekBarRight.setVisibility(View.VISIBLE);
			mVolumeBrightViewRight.setVisibility(View.VISIBLE);
			
			mVolBrightPercent.setGravity(Gravity.RIGHT);
			mVolBrightText.setGravity(Gravity.RIGHT);
        }

        mVolBrightPercent.setText(percent+"%");
		updateText(streamType);
        mVolumeAdjustView.setVisibility(View.VISIBLE);
    }
    
    public void hideVolumeTips() {
        setVisibility(View.INVISIBLE);
    }
	
	public void updateText(int streamType){
		int textId = R.string.oppo_volume_system;
		switch(streamType){
			case SHOW_STREAMTYPE_SYSTEM:
				textId = R.string.oppo_volume_system;
				break;
			case SHOW_STREAMTYPE_MEDIA:
				textId = R.string.oppo_volume_media;
				break;
			case SHOW_STREAMTYPE_RINGTONE:
				textId = R.string.oppo_volume_ringtone;
				break;
			default:
				break;
		}
		mVolBrightText.setText(textId);
	}
}
