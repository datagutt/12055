package com.android.server;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Matrix;
import android.graphics.Rect;
import android.graphics.YuvImage;
import android.hardware.Camera;
import android.hardware.Camera.Parameters;
import android.hardware.Camera.Size;
import android.os.Handler;
//import android.os.Message;
import android.util.Log;
import android.intelligent.EyeDetector;
import android.os.SystemClock;
import android.view.OrientationEventListener;
//import android.content.Intent;

public abstract class IntelligentSleepManager {

    private String TAG = "IntelligentSleepManager";

    private Camera mCamera = null;
    private Parameters mParameters = null;
    private Handler mHandler = new Handler();
    private Context mContext = null;
    private final EyeDetector face = new EyeDetector();
    private PreviewCallback mPreviewCallback = new PreviewCallback();
    private MyOrientationEventListener mOrientationListener = null;
    private int mLastOrientation = 0;
	
    private static final int PREVIEW_SIZE_WIDTH = 480;
    private static final int PREVIEW_SIZE_HEIGHT = 320;
	private static final int CAMERA_INTELLIGENT_SLEEP = 1001;
//    private static final String IntentName = "statusbar_show_eye_dectected";
	
    private boolean isProccing = false; 

    private class PreviewCallback implements
            android.hardware.Camera.PreviewCallback {

        public synchronized void onPreviewFrame (byte[] data,
                android.hardware.Camera camera) {
            Log.v(TAG, "onPreviewFrame");
            Log.v(TAG, "data:" + data);

            if (null == camera || null == data) {
                return;
            }

            try {
                if (null != camera) {
                    Camera.Parameters parameters = camera.getParameters();
                    Size size = parameters.getPictureSize();
                    Log.v(TAG, "size:" + size.width + "," + size.height);          
                    eyeDected(data,size.width,size.height);
                }
            } catch (Exception ex) {
                Log.v(TAG, "eye dected failed");
            }
        }
    }

    private void eyeDected (byte[] data,int width,int height) {
    
        Log.v(TAG, "mLastOrientation:" + mLastOrientation);

        int Orientation = 0;

	    if (mLastOrientation == 0) {
            Orientation = 270;   
	    } else if (mLastOrientation == 90) {
            Orientation = 180;
	    } else if (mLastOrientation == 180 ) {
            Orientation = 90;
	    } 
        
        
        int facecount = face.isHavEye(data,width,height,Orientation);               

        stopPreview();
        
        Log.v(TAG, "facecount:" + facecount);

        if (facecount >= 1) {
            handleEyeDectedResult();
        } 
        facecount = 0;
    }

    abstract protected void eyeDectedResultCallBack ();

    private void handleEyeDectedResult () {
        Log.v(TAG, "handleEyeDectedResult()");
        Log.v(TAG, "now = " + SystemClock.uptimeMillis());           
        eyeDectedResultCallBack();

/*        Intent mIntent = new Intent(IntentName);
        mIntent.putExtra("eye_dectect", true);
        mContext.sendBroadcast(mIntent);

        mHandler.postDelayed(new Runnable () {
            public void run() {
	            Log.v(TAG, "Notification cancel");
                Intent mIntent = new Intent(IntentName);
                mIntent.putExtra("eye_dectect", false);
                mContext.sendBroadcast(mIntent);
		    }
        },1000);*/ 
        
    }

    private synchronized void startPreview () {
        Log.v(TAG, "isProccing:" + isProccing);
        int rotation = 0;
        if (isProccing) {
            try {
                mCamera = android.hardware.Camera.open(CAMERA_INTELLIGENT_SLEEP);  
                isProccing = false;
            } catch (Exception e) {
                // TODO Auto-generated catch block
                Log.v(TAG,"open failed");
                isProccing = true;
                if (null != mOrientationListener) {
                    mOrientationListener.disable();
                }
                
            }
        }

        if (isProccing) {
            Log.v(TAG, "isProccing ...");
            return;
        } else {
            isProccing = true;
        }

        if (null != mOrientationListener) {
            mOrientationListener.enable();
        }

        try {
        
            if (null == mCamera) {
                mCamera = android.hardware.Camera.open(CAMERA_INTELLIGENT_SLEEP);
            }   /*adjust where the IntellegentCamera is destroyed by normal exit*/
                
            if (null != mCamera) {
            
                mParameters = mCamera.getParameters();
                mParameters.setPreviewSize(PREVIEW_SIZE_WIDTH, PREVIEW_SIZE_HEIGHT);
                mParameters.setPictureSize(PREVIEW_SIZE_WIDTH, PREVIEW_SIZE_HEIGHT);        
                mCamera.setParameters(mParameters);
        
                mCamera.startPreview();
                isProccing = true;

                mCamera.setOneShotPreviewCallback(mPreviewCallback);
            }
        } catch (Exception ex) {
            mCamera = null;
            mParameters = null;
            isProccing = false;

            if (null != mOrientationListener) {
                mOrientationListener.disable();
            }
        }
    }

    private void stopPreview () {
        Log.v(TAG, "stopPreview()");
        if (null != mCamera) {
            mCamera.setPreviewCallback(null);
            mCamera.stopPreview();
            mCamera.release();
            mCamera = null;
            mParameters = null;
        }

        if (null != mOrientationListener) {
            mOrientationListener.disable();
			mOrientationListener = null;
        }

        isProccing = false;
    }

    public void startEyeDected () {
        Log.v(TAG, "now = " + SystemClock.uptimeMillis());   
        if (null == mOrientationListener) {
        	mOrientationListener = new MyOrientationEventListener(mContext);
        }
        startPreview();
    }

    public void stopEyeDected () {
        stopPreview();
    }

    public IntelligentSleepManager(Context context) {
        mContext = context;
    }

    /* The Orientation listener of camera */
	private class MyOrientationEventListener extends OrientationEventListener {
		
        public MyOrientationEventListener(Context context) {
            super(context);
        }

        @Override
        public void onOrientationChanged(int orientation) {
            // We keep the last known orientation. So if the user
            // first orient the camera then point the camera to
            // floor/sky, we still have the correct orientation.
			orientation = roundOrientation(orientation);
			
		    mLastOrientation = orientation;
		    
        }
    }

    private int roundOrientation(int orientationInput) {
		int orientation = orientationInput;

		if (orientation == OrientationEventListener.ORIENTATION_UNKNOWN) {
			orientation = 0;
		}

		orientation = orientation % 360;
		int retVal;
		
		if (orientation < (0 * 90) + 45) {
			retVal = 0;
		} else if (orientation < (1 * 90) + 45) {
			retVal = 90;
		} else if (orientation < (2 * 90) + 45) {
			retVal = 180;
		} else if (orientation < (3 * 90) + 45) {
			retVal = 270;
		} else {
			retVal = 0;
		}

		return retVal;
	}
}
