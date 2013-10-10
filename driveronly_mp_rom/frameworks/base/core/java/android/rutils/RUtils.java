/******************************************************************************
******
** File: - RUtils.java
** VENDOR_EDIT
** Copyright (C), 2008-2012, OPPO Mobile Comm Corp., Ltd
** 
** Description: 
**      
** 
** Version: 0.1
** Date created: 20:34:23,11/02/2012
** Author: ZhenHai.Long@Prd.SysSRV
** 
** --------------------------- Revision History: ----------------------------
----
** 	<author>	                    <data>			<desc>
** ZhenHai.Long@Prd.SysSRV          11/02/2012      Init
  *******************************************************************************
*****/

 package android.rutils;

 import android.oppoutils.OppoUtils;
 public class RUtils extends OppoUtils{
   
    public static int RUtilsChmod(String path, int mod){
        return OppoUtils.Chmod(path, mod);
    }
    
    public static int RUtilsChmod(String path, int mod, String packageName){
        return OppoUtils.Chmod(path, mod, packageName);
    }
    
    public static int RUtilsGetPidByName(String processName){
        return OppoUtils.GetPidByName(processName);
    }
    public static int RUtilsSetSystemPropertiesStringInt(String key, int value
){
        return OppoUtils.SetSystemPropertiesStringInt(key, value);
    }
    public static int RUtilsSetSystemPropertiesString(String key, String value
){
        
        return OppoUtils.SetSystemPropertiesString(key, value);

    }

//#ifdef VENDOR_EDIT//Jinquan.Lin@BasicDrv.BL, 2012/05/17, Add for 
//customizing OPPO_CUSTOM partition!
    public static int RUtilsCustPartConfigItemUsbOps(boolean read, int value, 
int voter){
        return OppoUtils.CustPartConfigItemUsbOps(read, value, voter);
    }

    public static int RUtilsCustPartConfigItemClrFlagOps(boolean read, int 
source){
        return OppoUtils.CustPartConfigItemClrFlagOps(read, source);
    }

    public static String RUtilsCustPartCustPartGetMcpId(){
        return OppoUtils.CustPartCustPartGetMcpId();
    }
	public static int[] RUtilsCustPartCustPartCalibrateGsensorData(){

		return OppoUtils.CustPartCustPartCalibrateGsensorData();
	}

	public static int RUtilsCustPartCustPartRecoverGsensorData(){

		return OppoUtils.CustPartCustPartRecoverGsensorData();
	}

	public static int RUtilsCustPartCustPartGetRebootNumber(){

		return OppoUtils.CustPartCustPartGetRebootNumber();
	}

	public static int RUtilsCustPartCustPartClearRebootNumber(){

		return OppoUtils.CustPartCustPartClearRebootNumber();
	}

	public static int RUtilsCustPartCustPartGetRebootReason(){

		return OppoUtils.CustPartCustPartGetRebootReason();
	}
    
 }
