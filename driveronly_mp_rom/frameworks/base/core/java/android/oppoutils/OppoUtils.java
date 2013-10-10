/************************************************************************************
** File: - RUtils.java
** VENDOR_EDIT
** Copyright (C), 2008-2012, OPPO Mobile Comm Corp., Ltd
** 
** Description: 
**      Add this file for affording special opts to the application layer !
** 
** Version: 0.1
** Date created: 20:34:23,11/02/2012
** Author: ZhenHai.Long@Prd.SysSRV
** 
** --------------------------- Revision History: --------------------------------
** 	<author>	                    <data>			<desc>
** ZhenHai.Long@Prd.SysSRV          11/02/2012      Init
************************************************************************************/
 
 package android.oppoutils;
 import android.util.Log;
 import android.os.Process;
 import java.util.ArrayList;
 import java.util.Arrays;
 import android.content.Context;
 import android.content.BroadcastReceiver;
 import android.content.Intent;
 import android.content.IntentFilter;
 import android.os.Bundle;
 import android.content.pm.PackageManager;
 import android.content.Intent;
 import android.os.SystemProperties;
 //Pantengjiao@GraphicTech.cpu_boot, 2013/09/18, Add for Open multiple CPU
import android.os.SystemClock;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
 
 public class OppoUtils{
     
    private static String TAG = "RUTILS";
    private static ArrayList<Integer> mHiddenPidList = new ArrayList();
    private final static String[] mHiddenSysProcessArray = {
        "com.oppo.c2u",
        "ppo.findmyphone",
        "com.oppo.market",
        "com.oppo.usercenter",
        "com.oppo.tribune",
        "po.speechassist",
        "com.nearme.gamecenter",
        "com.nearme.note",
        "com.nearme.sync",
        ".SpeechMediaButtonService",

    };
    private final static String[] mHiddenSysAppArray = {
//oppo-common
        "com.nearme.gamecenter",
        "com.nearme.note",
        "com.nearme.sync", 
        "com.oppo.usercenter",
        "com.oppo.market",
        "com.oppo.c2u",
        "com.oppo.findmyphone",
        "com.oppo.service.account",
        "com.oppo.reader",
        "com.oppo.community",
//oppo-special      
        "com.oppo.ubeauty",
        "com.oppo.speechassist",
        "com.oppo.weather",
        "com.oppo.tribune",
        "com.oppo.weather.locationservice",
//third-part   
        "com.iflytek.speechservice",
        ".SpeechMediaButtonService",
        "com.tencent.mobileqq",
        "com.taobao.taobao",
        "com.tenpay.oppo",
        "com.qiyi.video",
        "com.netease.newsreader.activity",
        "com.mt.mtxx.mtxx",
        "com.youdao.dict",
        "com.baidu.BaiduMap",
        "com.baidu.searchbox_oppo",
        "com.tmall.wireless",
        "com.dianping.v1",
        "cn.emoney.level2",
        "com.oppo.widget.smallweather",
    };
    
    private final static String[] mSpecialAppArr = {
        "com.android.mms",
    };
    private final static ArrayList<String> mHiddenSysAppList = new ArrayList(Arrays.asList(mHiddenSysAppArray));
    private final static ArrayList<String> mSpecialAppList = new ArrayList(Arrays.asList(mSpecialAppArr));
    private final static ArrayList<String> mHiddenSysProcessList = new ArrayList(Arrays.asList(mHiddenSysProcessArray));
    //add for cmcc 
    BroadcastReceiver mOppoUtilsTmpReceiver = new BroadcastReceiver(){
         public void onReceive(Context context, Intent intent){
            PackageManager pm = context.getPackageManager();
            if(pm == null){
                Log.d(TAG, pm + " is not ready or has been crashed !");
                return;
            }
            if(intent.getAction().equals("oppo.specialaction.CMCC_ON")){
                Log.d(TAG, "oppo.specialaction.CMCC_ON msg received");
                SetSystemPropertiesStringInt("persist.oppo.cmccversion", 1);
                
                for(String packageName : mHiddenSysAppList){
			//modify for linzy
                    //pm.setLoginMode(packageName, PackageManager.LOGINMODE_PRIVACE);

                }
                Intent in  = new Intent("oppo.intent.action.ADDAPP_TO_VISTORMODE_OR_PRIVACYZONE");
		        in.putExtra("addflag", PackageManager.LOGINMODE_PRIVACE);  		 
		        in.putStringArrayListExtra("pkgnames",mHiddenSysAppList);    		 
		        context.sendBroadcast(in);
		        
		        hideProcessList(mHiddenSysProcessList);
            }else if(intent.getAction().equals("oppo.specialaction.CMCC_OFF")){
                Log.d(TAG, "oppo.specialaction.CMCC_OFF msg received");
                SetSystemPropertiesStringInt("persist.oppo.cmccversion", 0);
                
                for(String packageName : mHiddenSysAppList){
			//modify for linzy
                    //pm.setLoginMode(packageName, PackageManager.LOGINMODE_NORMAL);
                    Intent in  = new Intent("oppo.intent.action.ADDAPP_TO_VISTORMODE_OR_PRIVACYZONE");
    		        in.putExtra("addflag", PackageManager.LOGINMODE_PRIVACE);  		 
    		        in.putExtra("packagename", packageName);    		 
    		        context.sendBroadcast(in);
                }
                unhideProcessList(mHiddenPidList);
                
            }else if(intent.getAction().equals("android.intent.action.BOOT_COMPLETED")){
                if(SystemProperties.getInt("persist.oppo.cmccversion", 1) == 1){ 
                    Intent in  = new Intent("oppo.intent.action.ADDAPP_TO_VISTORMODE_OR_PRIVACYZONE");
    		        in.putExtra("addflag", PackageManager.LOGINMODE_PRIVACE);  		 
    		        in.putStringArrayListExtra("pkgnames",mHiddenSysAppList);    		 
    		        context.sendStickyBroadcast(in);
    		        //hideProcessList(mHiddenSysProcessList);
		        }
            }
         }
         
         
    }; 
    BroadcastReceiver mOppoUtilsReceiver = new BroadcastReceiver(){
        public void onReceive(Context context, Intent intent){

            if(intent.getAction().equals("RUTILS_RECEIVER_CHMOD")){
               Log.d(TAG, "RUTILS_RECEIVER_CHMOD msg received");
               Bundle bundle = intent.getExtras();
               
               String strFilePath = bundle.getString("file_path");
               if(strFilePath == null){
                Log.d(TAG, strFilePath + " is not existed !");
                return;
               }
               NativeChmod(strFilePath, 0777); 
            }else if(intent.getAction().equals("android.action.AMS_SYSTEM_READY")){
                Log.d(TAG, "android.action.AMS_SYSTEM_READY msg received");
                PackageManager pm = context.getPackageManager();
                if(pm == null){
                    Log.d(TAG, pm + " is not ready or has been crashed !");
                    return;
                }
                if(SystemProperties.getInt("persist.oppo.cmccversion", 1) == 1){
                    for(String packageName : mHiddenSysAppList){     
				//modify for linzy
                        //pm.setLoginMode(packageName, PackageManager.LOGINMODE_PRIVACE);
                        Log.d(TAG, packageName + " 's loginmode is setted ");
                    }
                    
                }
                
            }
                
          
        }

    };

    public OppoUtils(){}
    public OppoUtils(Context context){
        Log.d(TAG, "constructor with args is called");
        IntentFilter rutils_filter = new IntentFilter();
        rutils_filter.addAction("RUTILS_RECEIVER_CHMOD");
        //rutils_filter.addAction("android.action.AMS_SYSTEM_READY");
        Intent rutils_intent = context.registerReceiver(mOppoUtilsReceiver, rutils_filter);

        //add for cmcc
        IntentFilter cmcc_filter = new IntentFilter();
        cmcc_filter.addAction("oppo.specialaction.CMCC_OFF");
        cmcc_filter.addAction("oppo.specialaction.CMCC_ON");
        cmcc_filter.addAction("android.intent.action.BOOT_COMPLETED");
        //cmcc_filter.addAction("android.intent.action.SKIN_CHANGED");
        Intent cmcc_intent = context.registerReceiver(mOppoUtilsTmpReceiver, cmcc_filter);

        // always hidden processes
        int pid = GetPidByName("rutilsdaemon");
        if (pid == -1) {
            Log.d(TAG, "process not exist.");
        } else if(HideProcessByPid(pid) != 0) {
            Log.d(TAG, "process " + pid + " hide failed ");
        }
    }

    private static int hideProcessList(ArrayList<String> processNameList){

        for(String processName : processNameList){
            int pid = GetPidByName(processName);
            if(pid == -1){
                Log.d(TAG, "process not exist.");
                continue;
            }
            if(HideProcessByPid(pid) == 0){                   
                mHiddenPidList.add(pid);
            }else{
                Log.d(TAG, "process " + processName + " hide failed ");
            }
            
        }
        return 0;
    }
    private static int unhideProcessList(ArrayList<Integer> processIdList){
        for(int pid: processIdList){
            if(UnHideProcessByPid(pid) != 0){                   
                Log.d(TAG, "process " + pid + " unhide failed ");
            }
        }
        return 0;
    }
//cmcc support funcs
    public static boolean isCmccVersion(){
        return (SystemProperties.getInt("persist.oppo.cmccversion", 1) == 1);
    }
    public static boolean isCmccOperator(){

        return true;

    }

//special opts    
    public static int Chmod(String path, int mod){
        if(Process.myUid() != Process.SYSTEM_UID && Process.myUid() != Process.PHONE_UID){
            Log.d(TAG,"uid = " + Process.myUid() + " Permission Denied ~");
            return -1;
        }else{
            return NativeChmod(path, mod);
        }
    }
    
    public static int Chmod(String path, int mod, String packageName){
        if(!mSpecialAppList.contains(packageName)){
            Log.d(TAG,"pid = " + Process.myUid() + "Permission Denied ~");
            return -1;
        }else{
            return NativeChmod(path, mod);
        }
    }
    public static int GetPidByName(String processName){
        return NativeGetPidByName(processName);
    }

	//#ifdef VENDOR_EDIT
	//Pantengjiao@GraphicTech.cpu_boost, 2013/09/18, Add for Open multiple CPU
    // Max time (microseconds) to allow a CPU boost for
    private static final int MAX_CPU_BOOST_TIME = 5000000;
    private static long endTime = 0;

    /**
     * Boost the CPU
     * @param duration Duration to boost the CPU for, in milliseconds.
     * @hide
     */
    public static void cpuBoost(int duration) {
        if (mCpuBoostHandler == null) {
            return;
        }
        if (duration <= 0 || duration > MAX_CPU_BOOST_TIME) {
            Log.e(TAG, "CPU_BOOST Invalid boost duration: " + duration);
            return;
        }

        final long now = SystemClock.uptimeMillis();
        final long newEndtime = now + duration/1000;
        if (newEndtime > endTime) {
            endTime = newEndtime;
            //startCpuBoost();
            mCpuBoostHandler.removeMessages(START_CPU_BOOST);
            mCpuBoostHandler.sendMessage(mCpuBoostHandler.obtainMessage(START_CPU_BOOST));
        }
    }

    static final int START_CPU_BOOST = 1;
    static final int STOP_CPU_BOOST = 2;
    private static Handler mCpuBoostHandler = null;
    static {
        if (Looper.myLooper() != null) {
            mCpuBoostHandler = new Handler() {
                public void handleMessage(Message msg) {
                    switch (msg.what) {
                        case START_CPU_BOOST:
                            Log.d(TAG, "CPU_BOOST start cpu boost");
                            NativeCpuBoost(true);
                            mCpuBoostHandler.removeMessages(STOP_CPU_BOOST);
                            mCpuBoostHandler.sendMessageAtTime(mCpuBoostHandler.obtainMessage(STOP_CPU_BOOST), endTime);
                            break;
                        case STOP_CPU_BOOST:
                            Log.d(TAG, "CPU_BOOST stop cpu boost");
                            NativeCpuBoost(false);
                            break;
                        default:
                            break;
                    }
                }
            };
        }
    }
//#endif /* VENDOR_EDIT */

    public static int SetSystemPropertiesStringInt(String key, int value){
        if(key == null){
            return -1;
        }
        return NativeSetSystemProperties(key, String.valueOf(value));
    }
    public static int SetSystemPropertiesString(String key, String value){
        if(key == null || value == null){
            return -1;
        }
        return NativeSetSystemProperties(key, value);

    }
    public static int HideProcessByPid(int pid){

        return NativeHideProcessByPid(pid);
    }
    public static int UnHideProcessByPid(int pid){

        return NativeUnHideProcessByPid(pid);
    }

//#ifdef VENDOR_EDIT//Jinquan.Lin@BasicDrv.BL, 2012/05/17, Add for customizing OPPO_CUSTOM partition!
    public static int CustPartConfigItemUsbOps(boolean read, int value, int voter){
        return NativeCustPartConfigItemUsbOps(read, value, voter);
    }

    public static int CustPartConfigItemClrFlagOps(boolean read, int source){
        return NativeCustPartConfigItemClrFlagOps(read, source);
    }

    public static String CustPartCustPartGetMcpId(){
        return NativeCustPartCustPartGetMcpId();
    }
	public static int[] CustPartCustPartCalibrateGsensorData(){

		return NativeCustPartCustPartCalibrateGsensorData();
	}

	public static int CustPartCustPartRecoverGsensorData(){

		return NativeCustPartCustPartRecoverGsensorData();
	}

	public static int CustPartCustPartGetRebootNumber(){

		return NativeCustPartCustPartGetRebootNumber();
	}

	public static int CustPartCustPartClearRebootNumber(){

		return NativeCustPartCustPartClearRebootNumber();
	}

	public static int CustPartCustPartGetRebootReason(){

		return NativeCustPartCustPartGetRebootReason();
	}
//mingqiang.guo@Prd.BasicDrv.Sensor add for ps recover send pulse count
    	public static int[] CustPartCustPartAlsPsCalibratePsSendPulse(){

          return NativeCustPartCustPartAlsPsCalibratePsSendPulse();
    }

    public static int CustPartCustPartAlsPsRecoverPsSendPulse(){

          return NativeCustPartCustPartAlsPsRecoverPsSendPulse();
    }
//mingqiang.guo@Prd.BasicDrv.Sensor, add end

//LiuPing@Prd.BasicDrv.Sensor, add 2012/12/05  for ps switch 
    public static int CustPartCustPartAlsPsSetPsSwitch(){

          return NativeCustPartCustPartAlsPsSetPsSwitch();
    }

    public static int CustPartCustPartAlsPsRecoverPsSwitch(){

          return NativeCustPartCustPartAlsPsRecoverPsSwitch();
    }
    public static int CustPartCustPartAlsPsGetPsSwitchStatus(){

          return NativeCustPartCustPartAlsPsGetPsSwitchStatus();
    }
//LiuPing@Prd.BasicDrv.Sensor, add end
    public static native int NativeCustPartConfigItemUsbOps(boolean read, int value, int voter);
    public static native int NativeCustPartConfigItemClrFlagOps(boolean read, int source);
    public static native String NativeCustPartCustPartGetMcpId();
	public static native int[] NativeCustPartCustPartCalibrateGsensorData();
	public static native int NativeCustPartCustPartRecoverGsensorData();
	public static native int NativeCustPartCustPartGetRebootNumber();
	public static native int NativeCustPartCustPartClearRebootNumber();
	public static native int NativeCustPartCustPartGetRebootReason();
//#endif /* VENDOR_EDIT */    
    
    public static native int NativeChmod(String path, int mod);   
    public static native int NativeSetSystemProperties(String key, String value);
	public static native int NativeGetPidByName(String processName);

	public static native int NativeHideProcessByPid(int pid);
	public static native int NativeUnHideProcessByPid(int pid);
//mingqiang.guo@Prd.BasicDrv.Sensor add for ps recover send pulse count
   	public static native int[] NativeCustPartCustPartAlsPsCalibratePsSendPulse();
    public static native int NativeCustPartCustPartAlsPsRecoverPsSendPulse();
//mingqiang.guo@Prd.BasicDrv.Sensor, add end
//LiuPing@Prd.BasicDrv.Sensor, add 2012/12/05  for ps switch 
    public static native int NativeCustPartCustPartAlsPsSetPsSwitch();
    public static native int NativeCustPartCustPartAlsPsRecoverPsSwitch();
    public static native int NativeCustPartCustPartAlsPsGetPsSwitchStatus();
//LiuPing@Prd.BasicDrv.Sensor, add end
	//#ifdef VENDOR_EDIT
	//Pantengjiao@GraphicTech.cpu_boost, 2013/09/18, Add for Open multiple CPU
	public static native void NativeCpuBoost(boolean bStart);
	//#endif /* VENDOR_EDIT */

 }
