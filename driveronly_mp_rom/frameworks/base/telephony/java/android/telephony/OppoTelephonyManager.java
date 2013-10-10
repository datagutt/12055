package android.telephony;

import android.annotation.SdkConstant;
import android.annotation.SdkConstant.SdkConstantType;
import android.content.Context;
import android.os.Bundle;
import android.os.RemoteException;
import android.os.ServiceManager;
import android.os.SystemProperties;
import com.android.internal.telephony.IPhoneSubInfo;
import com.android.internal.telephony.ITelephony;
import com.android.internal.telephony.ITelephonyRegistry;
import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneFactory;
import com.android.internal.telephony.TelephonyProperties;
import java.util.List;
import com.android.internal.telephony.IIccPhoneBook;
import android.telephony.OppoTelephonyConstant;
import com.android.internal.telephony.IIccPhoneBook;
import android.util.Log;
import android.content.pm.PackageManager;
import android.os.Message;


@android.annotation.OppoHook(level=android.annotation.OppoHook.OppoHookType.NEW_CLASS,
                             property=android.annotation.OppoHook.OppoRomType.ROM,
                             note="sub class of TelephonyManager")
public class OppoTelephonyManager extends TelephonyManager {
    static final String TAG = "OppoTelephonyManager"; 
    private static Context sContext;	
		
    private static  boolean isMtkSupport;          // MTK or qcom
    private static  boolean isMtkGeminiSupport;   //MTK gemini or not
    private static  boolean isOppoSupport;         //oppo platform
    private static  boolean isRomSupport;          //Rom platform 

    private static final boolean LOCAL_DEBUG = true;    

    /** @hide */
    public OppoTelephonyManager(Context context) {
    	 super(context);
         if (sContext == null) {
            Context appContext = context.getApplicationContext();
            if (appContext != null) {
                sContext = appContext;
            } else {
                sContext = context;
            }
        }	

    	isOppoSupport = sContext.getPackageManager().hasSystemFeature("oppo.sw.solution.device");
    	isRomSupport = sContext.getPackageManager().hasSystemFeature("oppo.sw.solution.rom");
        isMtkGeminiSupport  = sContext.getPackageManager().hasSystemFeature("mtk.gemini.support");          // MTK or qcom
    	isMtkSupport = sContext.getPackageManager().hasSystemFeature("oppo.hw.manufacturer.mtk");   //MTK gemini or not
    	Log.d(TAG, "OppoTelephonyManager isMtkSupport =" + isMtkSupport  + " isMtkGeminiSupport =" + isMtkGeminiSupport);
    }
    
    /** @hide */
    /*  Construction function for TelephonyManager */
    protected OppoTelephonyManager() {
    }

    /*  Create static instance for TelephonyManager */
    private static OppoTelephonyManager sInstance = new OppoTelephonyManager();

    /* @deprecated - use getSystemService as described above */
    public static OppoTelephonyManager getDefault() {
        return sInstance;
    }
	
    private ITelephony getITelephony() {
        return ITelephony.Stub.asInterface(ServiceManager.getService(Context.TELEPHONY_SERVICE));
    } 

    private IIccPhoneBook getIccPhoneBookInfo() {
        if(isMtkSupport && isMtkGeminiSupport){
            return getIccPhoneBookInfoGemini(getDefaultSim());
        } else {
            return IIccPhoneBook.Stub.asInterface(ServiceManager.getService("simphonebook"));
        }
    }
    
    public int oppoSimPhonebookIsReady() {
        if(isMtkSupport && isMtkGeminiSupport){
            return oppoSimPhonebookIsReadyGemini(getDefaultSim());
        } else{
            try {
                return getITelephony().oppoSimPhonebookIsReady();
            } catch (RemoteException ex) {
                return OppoTelephonyConstant.OPPO_SIMPHONEBOOK_STATE_ERROR;
            }
        }
    }
   
    public int oppoGetPreferredNetworkType(){
        try {
             ITelephony telephony = getITelephony();
             
             if (telephony != null) {
                 return telephony.getPreferredNetworkType();
             } else {
                 return -1;
             }          
        } catch (RemoteException ex) {
            return -1;
        } catch (NullPointerException ex) {
            return -1;
        }
    }

    public int oppoSetPreferredNetworkType(int networkType){
        try {
             ITelephony telephony = getITelephony();
             if (telephony != null) {
                 return telephony.setPreferredNetworkType(networkType);
             } else {
                 return -1;
             }          
        } catch (RemoteException ex) {
            return -1;
        } catch (NullPointerException ex) {
            return -1;
        }
    }

    public int oppoGetSimPhonebookAllSpace() {
        if(isMtkSupport && isMtkGeminiSupport){
            return oppoGetSimPhonebookAllSpaceGemini(getDefaultSim());
        } else{
            try {
                return getIccPhoneBookInfo().oppoGetSimPhonebookAllSpace();
            } catch (RemoteException ex) {
                return -1;
            }
       }
    }	    

    public int oppoGetSimPhonebookUsedSpace() {
        if(isMtkSupport && isMtkGeminiSupport){
            return oppoGetSimPhonebookUsedSpaceGemini(getDefaultSim());
        } else{
            try {
                return getIccPhoneBookInfo().oppoGetSimPhonebookUsedSpace();
            } catch (RemoteException ex) {
                return -1;
            }
        }
    }

    public int oppoGetSimPhonebookNameLength() {
        if(isMtkSupport && isMtkGeminiSupport){
            return oppoGetSimPhonebookNameLengthGemini(getDefaultSim());
        } else{
            try {
                return getIccPhoneBookInfo().oppoGetSimPhonebookNameLength();
            } catch (RemoteException ex) {
                return -1;
            } catch (NullPointerException ex) {
                return -1;
            }
        }
    }

    public int oppoCheckPhbNameLength(String name) {
        //BaoZhu.Yu@CommApp., 2012/02/25, Add for mtkGeminiSupport
        if(isMtkSupport && isMtkGeminiSupport){
            return oppoCheckPhbNameLengthGenimi(name,getDefaultSim());
        } else{
            int leng = oppoGetSimPhonebookNameLength();
    		int[] encodeInfo = SmsMessage.calculateLength(name,false);
    		if(encodeInfo[3] == SmsMessage.ENCODING_16BIT){
    			leng = (leng-1)/2;
    		}
            return leng;
        }
    }

    public int oppoGetSimPhonebookNumberLength() {
        //return OppoTelephonyConstant.OPPO_MAX_PB_NUMBER_LENGTH;
        if(isMtkSupport && isMtkGeminiSupport){
            return oppoGetSimPhonebookNumberLengthGemini(getDefaultSim());
        } else{
            try {
                return getIccPhoneBookInfo().oppoGetSimPhonebookNumberLength();
            } catch (RemoteException ex) {
                  Log.e(TAG, "oppoGetSimPhonebookNumberLength: remote exception.");
                return -1;
            } catch (NullPointerException ex) {
                // This could happen before phone restarts due to crashing
                  Log.e(TAG, "oppoGetSimPhonebookNumberLength: null pointer exception.");
                return -1;
            }
        }
    }

    public int oppoGetSimPhonebookEmailLength() {
        //BaoZhu.Yu@CommApp., 2012/02/25, Add for mtkGeminiSupport
        if(isMtkSupport && isMtkGeminiSupport){
            return oppoGetSimPhonebookEmailLengthGemini(getDefaultSim());
        } else{
            try {
                return getIccPhoneBookInfo().oppoGetSimPhonebookEmailLength();
            } catch (RemoteException ex) {
                 if(LOCAL_DEBUG) Log.e("TelephonyManager", "oppoGetSimPhonebookEmailLength: remote exception.");
                return -1;
            } catch (NullPointerException ex) {
                // This could happen before phone restarts due to crashing
                 if(LOCAL_DEBUG) Log.e("TelephonyManager", "oppoGetSimPhonebookEmailLength: null pointer exception.");
                return -1;
            }
        }
    }
 
    public boolean oppoEnableDataConnectivity() {
        if(isMtkSupport && isMtkGeminiSupport){
            return (oppoEnableDataConnectivityGemini(getDefaultSim()) > 0 ? true : false);
        } else{
            try {
                return getITelephony().enableDataConnectivity();
            } catch (RemoteException ex) {
                return false;
            }
        }
    }

    public boolean oppoDisableDataConnectivity() {
        if(isMtkSupport && isMtkGeminiSupport){
            return (oppoDisableDataConnectivityGemini(getDefaultSim()) > 0 ? true : false);
        } else{
            try {
                return getITelephony().disableDataConnectivity();
            } catch (RemoteException ex) {
                return false;
            }
        }
    }

    public Bundle oppoGetServiceState(){
       if(isMtkSupport && isMtkGeminiSupport){
            return oppoGetServiceStateGemini(getDefaultSim());
       } else {
           try {
                ITelephony telephony = getITelephony();
                if (telephony != null) {
                    return telephony.getServiceState();
                } else {
                    return null;
                }          
           } catch (RemoteException ex) {
               return null;
           } catch (NullPointerException ex) {
               return null;
           }
       }
    }

    public int oppoGetSimIndicatorState(){   
       if(isMtkSupport && isMtkGeminiSupport){
            return oppoGetSimIndicatorStateGemini(getDefaultSim());
       } else{
           try {
                ITelephony telephony = getITelephony();
                if (telephony != null) {
                    return telephony.getSimIndicatorState();
                } else {
                    return -1;
                }          
           } catch (RemoteException ex) {
               return -1;
           } catch (NullPointerException ex) {
               return -1;
           }
    	}
    }
	
    /**
     * Get service center address
     * @param slotId SIM ID
     * @return Current service center address
     */
    public String oppoGetScAddress(int slotId) {
        try {
            return getITelephony().getScAddressGemini(slotId);
        } catch(RemoteException e1) {
            return null;
        } catch(NullPointerException e2) {
            return null;
        }
    }

    /**
    * Set service center address
    * @param address Address to be set
    * @param slotId SIM ID
    * @return True for success, false for failure 
    */
    public boolean oppoSetScAddress(String address, int slotId) {        
          try {
             getITelephony().setScAddressGemini(address, slotId);
             return true;
          } catch(RemoteException e1) {
             return false;
          } catch(NullPointerException e2) {
             return false;
          }
    }

    private IIccPhoneBook getIccPhoneBookInfoGemini(int simId) {
        if(simId == OppoTelephonyConstant.GEMINI_SIM_1){
            return IIccPhoneBook.Stub.asInterface(ServiceManager.getService("simphonebook"));
        }
        else if(simId == OppoTelephonyConstant.GEMINI_SIM_2){
            return IIccPhoneBook.Stub.asInterface(ServiceManager.getService("simphonebook2"));
        }else{
            return IIccPhoneBook.Stub.asInterface(ServiceManager.getService("simphonebook"));
        }
    }


    public int oppoSimPhonebookIsReadyGemini(int simId) {
        try {
            return getITelephony().oppoSimPhonebookIsReadyGemini(simId);
        } catch (RemoteException ex) {
            return -1;
        }
    }

    public int oppoGetSimPhonebookAllSpaceGemini(int simId) {
        try {
            return getIccPhoneBookInfoGemini(simId).oppoGetSimPhonebookAllSpace();
        } catch (RemoteException ex) {
            return -1;
        }
    }

    public int oppoGetSimPhonebookUsedSpaceGemini(int simId) {
        try {
            return getIccPhoneBookInfoGemini(simId).oppoGetSimPhonebookUsedSpace();
        } catch (RemoteException ex) {
            return -1;
        }
    }

    public int oppoCheckPhbNameLengthGenimi(String name,int simId) {
        int leng = oppoGetSimPhonebookNameLengthGemini(simId);
		//#ifndef VENDOR_EDIT 
		//BaoZhu.Yu@Prd.CommApp.Telephony, 2012/06/11, Add for
		/*
        try {
            GsmAlphabet.countGsmSeptets(name, true);
        } catch (EncodeException e) {
            leng = (leng-1)/2;
        }
		*/
		//#else /* VENDOR_EDIT */
		int[] encodeInfo = SmsMessage.calculateLength(name,false);
		if(encodeInfo[3] == SmsMessage.ENCODING_16BIT){
			leng = (leng-1)/2;
		}
		//#endif /* VENDOR_EDIT */
        return leng;

    }


			
    public int oppoGetSimPhonebookNameLengthGemini(int simId) {
        try {
            return getIccPhoneBookInfoGemini(simId).oppoGetSimPhonebookNameLength();
        } catch (RemoteException ex) {
            return -1;
        } catch (NullPointerException ex) {
            // This could happen before phone restarts due to crashing
            return -1;
        }
    }	
	

								 
    public int oppoGetSimPhonebookNumberLengthGemini(int simId) {
        try {
            return getIccPhoneBookInfoGemini(simId).oppoGetSimPhonebookNumberLength();
        } catch (RemoteException ex) {
             if(LOCAL_DEBUG) Log.e("TelephonyManager", "oppoGetSimPhonebookNumberLengthGemini: remote exception.");
            return -1;
        } catch (NullPointerException ex) {
            // This could happen before phone restarts due to crashing
             if(LOCAL_DEBUG) Log.e("TelephonyManager", "oppoGetSimPhonebookNumberLengthGemini: null pointer exception.");
            return -1;
        }
    }


    public int oppoGetSimPhonebookEmailLengthGemini(int simId) {
        try {
            return getIccPhoneBookInfoGemini(simId).oppoGetSimPhonebookEmailLength();
        } catch (RemoteException ex) {
             if(LOCAL_DEBUG) Log.e("TelephonyManager", "oppoGetSimPhonebookEmailLengthGemini: remote exception.");
            return -1;
        } catch (NullPointerException ex) {
            // This could happen before phone restarts due to crashing
             if(LOCAL_DEBUG) Log.e("TelephonyManager", "oppoGetSimPhonebookEmailLengthGemini: null pointer exception.");
            return -1;
        }
    }

    public int oppoEnableDataConnectivityGemini(int simId) {
        try {
            return getITelephony().enableDataConnectivityGemini(simId);
        } catch (RemoteException ex) {
            return -1;
        }
    }


    public int oppoDisableDataConnectivityGemini(int simId) {
        try {
            return getITelephony().disableDataConnectivityGemini(simId);
        } catch (RemoteException ex) {
            return -1;
        }
    }

    public Bundle oppoGetServiceStateGemini(int simId){
        try {
             ITelephony telephony = getITelephony();
             if (telephony != null) {
                 return telephony.getServiceStateGemini(simId);
             } else {
                 // This can happen when the ITelephony interface is not up yet.
                 return null;
             }          
        } catch (RemoteException ex) {
            // the phone process is restarting.
            return null;
        } catch (NullPointerException ex) {
            return null;
        }  
    }

    public int oppoGetSimIndicatorStateGemini(int simId){
       try {
           ITelephony telephony = getITelephony();
           if (telephony != null) {
               return telephony.getSimIndicatorStateGemini(simId);
           } else {
               return -1;
           }          
       } catch (RemoteException ex) {
           return -1;
       } catch (NullPointerException ex) {
           return -1;
       }
    }

    public boolean oppoSetLine1Number(String phoneNumber) {
        //BaoZhu.Yu@CommApp., 2012/02/25, Add for mtkGeminiSupport
        if(isMtkSupport && isMtkGeminiSupport){
            return oppoSetLine1NumberGemini(phoneNumber,getDefaultSim());
        } else{
            try {
                return getITelephony().setLine1Number(phoneNumber);
            } catch (RemoteException ex) {
                Log.e("TelephonyManager", "setLine1Number: remote exception.");
                // the phone process is restarting.
                return false;
            } catch (NullPointerException ex) {
                Log.e("TelephonyManager", "setLine1Number: null pointer exception.");
                // the phone process is restarting.
                return false;
            }
        }
    } 
	
	public boolean oppoSetLine1NumberGemini(String phoneNumber,int simId) {
        try {
            return getITelephony().setLine1NumberGemini(phoneNumber,simId);
        } catch (RemoteException ex) {
            Log.e("TelephonyManager", "setLine1NumberGemini: remote exception.");
            // the phone process is restarting.
            return false;
        } catch (NullPointerException ex) {
            Log.e("TelephonyManager", "setLine1NumberGemini: null pointer exception.");
            // the phone process is restarting.
            return false;
        }
    }

    public boolean oppoGetIccLockEnabled() {
        try {
            return getITelephony().oppoGetIccLockEnabled();
        } catch (RemoteException ex) {
            Log.e("TelephonyManager", "oppoGetIccLockEnabled: remote exception.");
            return false;
        } catch (NullPointerException ex) {
            Log.e("TelephonyManager", "oppoGetIccLockEnabled: null pointer exception.");
            return false;
        }
    }

    public void oppoSetIccLockEnabled(boolean enabled, String password, Message onComplete) {
        try {
            getITelephony().oppoSetIccLockEnabled(enabled,password,onComplete);
        } catch (RemoteException ex) {
            Log.e("TelephonyManager", "oppoSetIccLockEnabled: remote exception.");
        } catch (NullPointerException ex) {
            Log.e("TelephonyManager", "oppoSetIccLockEnabled: null pointer exception.");
        }
    }

    public void oppoChangeIccLockPassword(String oldPassword, String newPassword,Message onComplete) {
        try {
            getITelephony().oppoChangeIccLockPassword(oldPassword,newPassword,onComplete);
        } catch (RemoteException ex) {
            Log.e("TelephonyManager", "oppoChangeIccLockPassword: remote exception.");
        } catch (NullPointerException ex) {
            Log.e("TelephonyManager", "oppoChangeIccLockPassword: null pointer exception.");
        }
    }

    public boolean oppoIsDialing() {
        try {
            return getITelephony().oppoIsDialing();
        } catch (RemoteException ex) {
            Log.e("TelephonyManager", "oppoIsDialing: remote exception.");
            return false;
        } catch (NullPointerException ex) {
            Log.e("TelephonyManager", "oppoIsDialing: null pointer exception.");
            return false;
        }
    }
	
    public static final int OPPO_EMT_CALL_STATE_ERROR = -1;
    public static final int OPPO_EMT_CALL_STATE_IDLE = 0;
    public static final int OPPO_EMT_CALL_STATE_DIALING = 1;
    public static final int OPPO_EMT_CALL_STATE_ACTIVE = 2;
    public static final int OPPO_EMT_CALL_STATE_HOLDING = 3;
    public static final int OPPO_EMT_CALL_STATE_UNKNOWN = 4;
    public static final int OPPO_EMT_CALL_STATE_ALERTING = 5;
	
    public void oppoemtDial(String telNumber) {
        //BaoZhu.Yu@CommApp., 2012/02/25, Add for mtkGeminiSupport
        if(isMtkSupport && isMtkGeminiSupport){
            oppoemtDialGemini(telNumber,getDefaultSim());
        } else{
            try {
                getITelephony().emtDial(telNumber);
            } catch (RemoteException ex) {
                // the phone process is restarting.
                //return CALL_STATE_IDLE;
            }
        }
    }
    public void oppoemtDialGemini(String telNumber,int slotid) {
        try {
            getITelephony().emtDialGemini(telNumber,slotid);
        } catch (RemoteException ex) {
            // the phone process is restarting.
            //return CALL_STATE_IDLE;
        }
    }

    public void oppoemtHungup(String telNumber) {
        //BaoZhu.Yu@CommApp., 2012/02/25, Add for mtkGeminiSupport
        if(isMtkSupport && isMtkGeminiSupport){
            oppoemtHungupGemini(telNumber,getDefaultSim());
        } else{
            try {
                getITelephony().emtHungup(telNumber);
            } catch (RemoteException ex) {
                // the phone process is restarting.
                //return CALL_STATE_IDLE;
            }
        }
    }
	
    public void oppoemtHungupGemini(String telNumber,int slotid) {
        try {
            getITelephony().emtHungupGemini(telNumber,slotid);
        } catch (RemoteException ex) {
            // the phone process is restarting.
            //return CALL_STATE_IDLE;
        }
    }

    public void oppoenableEngineerTest(boolean on) {
        try {
            getITelephony().enableEngineerTest(on);
        } catch (RemoteException ex) {
            // the phone process is restarting.
            //return CALL_STATE_IDLE;
        }
    }

    public boolean oppoisSimInsert(int simId) {
		try {
            return getITelephony().isSimInsert(simId);
        } catch (RemoteException ex) {
            return false;
        }
	}

    public boolean oppoGetAudioRecordState() {
    	return false;	
    }
   
    public String oppoGetIccCardType()
    {
        return getIccCardType();
    }

    public boolean oppoIs3GSwitchVisible() {
        try {
            return getITelephony().is3GSwitchVisible();
        } catch (RemoteException ex) {
            return false;
        }
    }

    public boolean oppoSwitch3GDefaultSim(int simid) {
        try {
            return getITelephony().switch3GDefaultSim(simid);
        } catch (RemoteException ex) {
            return false;
        }
    }

}
//#endif /* VENDOR_EDIT */
