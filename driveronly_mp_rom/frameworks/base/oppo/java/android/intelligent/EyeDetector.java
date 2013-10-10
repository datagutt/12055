package android.intelligent;

import android.graphics.Bitmap;
import android.graphics.PointF;
import android.util.Log;

import java.lang.IllegalArgumentException;

public class EyeDetector{

	public EyeDetector(){
		 
	}
	 
	@Override
    protected void finalize() throws Throwable {
    	eye_detect_destroy();
    }
	
	public int isHavEye(Bitmap bitmap)
	{
		return eye_detect(bitmap);
	}


	public int isHavEye(String path)
	{
		return eye_detect(path);
	}

	public int isHavEye(byte[] data,int width,int height,int angle)
	{
		return eye_detect(data,width,height,angle);
	}
	
	
	private static boolean sInitialized;
    

    static {
        sInitialized = false;
        try {
			Log.d("libintel_jni","opencv libintel_jni begin");
            System.loadLibrary("intel_jni");
			 Log.d("libintel_jni","opencv libintel_jni end");
            nativeClassInit();
            sInitialized = true;
        } catch (UnsatisfiedLinkError e) {
            Log.d("libintel_jni", "eye detection library not found!");
        }
    }
	
	native private static void nativeClassInit();
	native private void eye_detect_destroy(); 
	native private int eye_detect(Bitmap bitmap);
	native private int eye_detect(String path);
	native private int eye_detect(byte[] data,int width,int height,int angle);

}