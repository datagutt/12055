/************************************************************************************
 ** File: - OppoExServiceClient.java
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

import java.io.FileDescriptor;
import java.io.PrintWriter;

import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.res.Configuration;
import android.os.IBinder;
import android.os.RemoteException;
import android.os.ServiceManager;
import android.util.Slog;
import android.util.Log;
import android.os.SystemProperties;
import android.os.OppoExManager;
import android.os.IOppoExService;
import android.os.IOppoExInputCallBack;
import android.view.InputEvent;
import android.view.MotionEvent;
import android.view.InputDevice;
import android.graphics.PixelFormat;
import android.view.IWindowManager;
import android.view.WindowManager;
import android.view.View;
import android.provider.Settings;
import android.os.Handler;
import android.os.Message;
import android.database.ContentObserver;
import android.content.ContentResolver;
import com.oppo.content.OppoIntent;
import android.content.IntentFilter;

public class OppoExServiceClient extends Service {
    private static final String TAG = "OppoExServiceClient";
	private static final String OPPO_EX_DEBUGGABLE = "persist.sys.ex.service";
	private boolean isDebugable = "1".equals(SystemProperties.get(OPPO_EX_DEBUGGABLE));
	
	// for three pointers move shot screen
	int mOppoPointerScreenCaptureMode = 0; // guarded by mLock
	private static final int MSG_OPPO_ENABLE_SCREEN_CAPTURE = 10000;
    private static final int MSG_OPPO_DISABLE_SCREEN_CAPTURE = 10001;
	private static final int MSG_OPPO_SHOW_TIP = 10002;
    private static final int MSG_OPPO_HIDE_TIP = 10003;
	
	
    // for pointers launch camera.
    int mOppoPointerLaunchCameraMode = 0;
	private static final int MSG_OPPO_ENABLE_LAUNCH_CAMERA = 10006;
    private static final int MSG_OPPO_DISABLE_LAUNCH_CAMERA = 10007;
    
	private static final int MSG_OPPO_SHOW_VOL_CONTROL = 10004;
    private static final int MSG_OPPO_HIDE_VOL_CONTROL = 10005;
	
	private static final String ACTION_CAMERA_MODE_CHANGE = OppoIntent.ACTION_CAMERA_MODE_CHANGE;
	private static final String EXTRA_DATA = "iscameramode";
	

	private static final String OPPO_SMART_APPERCEIVE_DOUBLE_VOLUME = Settings.System.OPPO_DOUBLE_FINGER_CONTROL_VOLUME_ENABLED;
	private static final String OPPO_SMART_BLANK_SCREEN_MULTI_TOUCH_CAMERA = Settings.System.OPPO_BLANK_SCREEN_MULTI_TOUCH_CAMERA_ENABLED;

	private IOppoExService mExManager = null;
	private OppoThreePointersShotPolicy mThreePointersPolicy = null;
	private OppoStartCameraPolicy mStartCameraPolicy = null;
	private OppoControlVolumePolicy mControlVolumePolicy = null;
	private Handler mHandler;
	final Object mLock = new Object();
	
	private OppoTipsView mTipsView = null;
	
	private OppoVolumeView mVolumeView = null;
	private boolean mVolumeControlEnabled = false;
	
    @Override
    public void onCreate() {
		super.onCreate();
		
		mHandler = new ExHandler();
		
	    mExManager = IOppoExService.Stub.asInterface(
                ServiceManager.getService(OppoExManager.SERVICE_NAME));
		
		try {
			mExManager.registerInputEvent(mInputReceiver);
        } catch (RemoteException e) {
            Slog.w(TAG, "Failing registerInputEvent", e);
        }
		
		mThreePointersPolicy = new OppoThreePointersShotPolicy(this, mHandler);
		mStartCameraPolicy = new OppoStartCameraPolicy(this, mHandler);
		mControlVolumePolicy = new OppoControlVolumePolicy(this, mHandler);
		
		if (mTipsView == null) {
			mTipsView = new OppoTipsView(this);
            
            WindowManager.LayoutParams lp = new WindowManager.LayoutParams(
                    WindowManager.LayoutParams.MATCH_PARENT,
                    WindowManager.LayoutParams.MATCH_PARENT);
            lp.type = WindowManager.LayoutParams.TYPE_SECURE_SYSTEM_OVERLAY;
            lp.flags = WindowManager.LayoutParams.FLAG_FULLSCREEN
                    | WindowManager.LayoutParams.FLAG_NOT_TOUCHABLE
                    | WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE
                    | WindowManager.LayoutParams.FLAG_LAYOUT_IN_SCREEN;
            lp.format = PixelFormat.TRANSLUCENT;
            lp.setTitle("OppoScreenShotHelper");
			WindowManager wm = (WindowManager)
                    getSystemService(Context.WINDOW_SERVICE);
			
			if (mTipsView != null){
				wm.addView(mTipsView, lp);
			}
		}
		
		if (mVolumeView == null) {
			mVolumeView = new OppoVolumeView(this);
            
            WindowManager.LayoutParams lp = new WindowManager.LayoutParams(
                    WindowManager.LayoutParams.MATCH_PARENT,
                    WindowManager.LayoutParams.MATCH_PARENT);
            lp.type = WindowManager.LayoutParams.TYPE_SECURE_SYSTEM_OVERLAY;
            lp.flags = WindowManager.LayoutParams.FLAG_FULLSCREEN
                    | WindowManager.LayoutParams.FLAG_NOT_TOUCHABLE
                    | WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE
                    | WindowManager.LayoutParams.FLAG_LAYOUT_IN_SCREEN;
            lp.format = PixelFormat.TRANSLUCENT;
            lp.setTitle("OppoVolumeControl");
			WindowManager wm = (WindowManager)
                    getSystemService(Context.WINDOW_SERVICE);
			
			if (mVolumeView != null){
				wm.addView(mVolumeView, lp);
			}
		}
		
		OppoSettingsObserver settingsObserver = new OppoSettingsObserver(mHandler);
        settingsObserver.observe();
		
		IntentFilter filter = new IntentFilter();
		filter.addAction(ACTION_CAMERA_MODE_CHANGE);
		registerReceiver(mOppoBaseReceiver, filter);		
    }
	
    public void onDestroy(){		
		if (mExManager != null){
			try {
				mExManager.unregisterInputEvent(mInputReceiver);
			} catch (RemoteException e) {
				Slog.w(TAG, "Failing unregisterInputEvent", e);
			}
		}
		
		if (mTipsView != null) {
            WindowManager wm = (WindowManager)
                    getSystemService(Context.WINDOW_SERVICE);
				wm.removeView(mTipsView);
            mTipsView = null;
        }
		
		if (mVolumeView != null) {
            WindowManager wm = (WindowManager)
                    getSystemService(Context.WINDOW_SERVICE);
				wm.removeView(mVolumeView);
            mVolumeView = null;
        }
		
		super.onDestroy();
	}

    @Override
    public void onConfigurationChanged(Configuration newConfig) {
    }

    /**
     * Nobody binds to us.
     */
    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    protected void dump(FileDescriptor fd, PrintWriter pw, String[] args) {
        if (args == null || args.length == 0) {

        } else {
            String svc = args[0];
        }
    }
	
	private IOppoExInputCallBack.Stub mInputReceiver = new IOppoExInputCallBack.Stub() {

		@Override
		public void onInputEvent(InputEvent event) {
			if (isDebugable) {
				Log.d(TAG, "OppoExService  enableInputReceiver");
			}
			if (event instanceof MotionEvent
                        && (event.getSource() & InputDevice.SOURCE_CLASS_POINTER) != 0) {
                final MotionEvent motionEvent = (MotionEvent)event;
				
				mThreePointersPolicy.addPointerEvent(motionEvent);
				//
				mStartCameraPolicy.addPointerEvent(motionEvent);
				
				//control volume
				if (mVolumeControlEnabled){
					mControlVolumePolicy.addPointerEvent(motionEvent);
				}	
			} 
		}
	};
	
	BroadcastReceiver mOppoBaseReceiver = new BroadcastReceiver() {
        public void onReceive(Context context, Intent intent) {
			String action = intent.getAction();
			if (ACTION_CAMERA_MODE_CHANGE.equals(action)){
				Log.i(TAG,"ACTION_CAMERA_MODE_CHANGE");				
				mThreePointersPolicy.setSensorMode(intent.getIntExtra(EXTRA_DATA, 0) == 0? false:true);
			}
		}
	};
	
	private class ExHandler extends Handler {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case MSG_OPPO_ENABLE_SCREEN_CAPTURE:
					mThreePointersPolicy.setScreenShotEnable(true);
                    break;
                case MSG_OPPO_DISABLE_SCREEN_CAPTURE:
					mThreePointersPolicy.setScreenShotEnable(false);
                    break;
                case MSG_OPPO_ENABLE_LAUNCH_CAMERA:
                	if (mStartCameraPolicy != null) {
                		mStartCameraPolicy.setLaunchCameraEnable(true);
                	}
                	break;
                case MSG_OPPO_DISABLE_LAUNCH_CAMERA:
                	if (mStartCameraPolicy != null) {
                		mStartCameraPolicy.setLaunchCameraEnable(false);
                	}                   
                	break;
				case MSG_OPPO_SHOW_TIP:
					mTipsView.showShotTips(msg.arg1 == 1? true:false);
                    break;
                case MSG_OPPO_HIDE_TIP:
					mTipsView.hideShotTips();
                    break;
				case MSG_OPPO_SHOW_VOL_CONTROL:{
						mVolumeView.showVolumeTips(msg.getData());
					}
                    break;
                case MSG_OPPO_HIDE_VOL_CONTROL:
					mVolumeView.hideVolumeTips();
                    break;
				default:
					break;
            }
        }
    }
	
	class OppoSettingsObserver extends ContentObserver {
        OppoSettingsObserver(Handler handler) {
            super(handler);
        }

        void observe() {
            ContentResolver resolver = getContentResolver();

            resolver.registerContentObserver(Settings.System.getUriFor(
                    Settings.System.OPPO_SMART_APPERCEIVE_SCREEN_CAPTURE), false, this);
            resolver.registerContentObserver(Settings.System.getUriFor(OPPO_SMART_APPERCEIVE_DOUBLE_VOLUME), false, this);
            resolver.registerContentObserver(Settings.System.getUriFor(
            		OPPO_SMART_BLANK_SCREEN_MULTI_TOUCH_CAMERA), false, this);
            updateOppoSettings();
        }

        @Override public void onChange(boolean selfChange) {
            updateOppoSettings();
        }
    }
	
	public void updateOppoSettings() {
		synchronized (mLock) {
			ContentResolver resolver = getContentResolver();
			int pointerScreenCapture = Settings.System.getInt(resolver,
			Settings.System.OPPO_SMART_APPERCEIVE_SCREEN_CAPTURE, 0);
			Log.d(TAG, "+++++++++++ updateOppoSettings pointerScreenCapture=" + pointerScreenCapture 
							+ "  mOppoPointerScreenCaptureMode " + mOppoPointerScreenCaptureMode);
			if (mOppoPointerScreenCaptureMode != pointerScreenCapture) {
						mOppoPointerScreenCaptureMode = pointerScreenCapture;
						mHandler.sendEmptyMessage(pointerScreenCapture != 0 ?
								MSG_OPPO_ENABLE_SCREEN_CAPTURE : MSG_OPPO_DISABLE_SCREEN_CAPTURE);
			}
			int pointerLaunchCamera = Settings.System.getInt(resolver,
					OPPO_SMART_BLANK_SCREEN_MULTI_TOUCH_CAMERA, 0);
			Log.d(TAG, "+++++++++++ updateOppoSettings pointerLaunchCamera=" + pointerLaunchCamera 
					+ "  mOppoPointerLaunchCameraMode " + mOppoPointerLaunchCameraMode);
			if (mOppoPointerLaunchCameraMode != pointerLaunchCamera) {
				mOppoPointerLaunchCameraMode = pointerLaunchCamera;
				mHandler.sendEmptyMessage(pointerLaunchCamera != 0 ?
						MSG_OPPO_ENABLE_LAUNCH_CAMERA : MSG_OPPO_DISABLE_LAUNCH_CAMERA);
			}
			
			//
			mVolumeControlEnabled = Settings.System.getInt(resolver,
				OPPO_SMART_APPERCEIVE_DOUBLE_VOLUME, 0) == 1? true : false;
		}
	}
}

