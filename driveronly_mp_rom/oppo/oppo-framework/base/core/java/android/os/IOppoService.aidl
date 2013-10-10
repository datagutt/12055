/********************************************************************************
 ** Copyright (C), 2008-2013, OPPO Mobile Comm Corp., Ltd
 ** VENDOR_EDIT, All rights reserved.
 ** 
 ** File: - IOppoService.aidl
 ** Description: 
 **     oppo service
 **     
 ** Version: 1.0
 ** Date: 2013-06-03
 ** Author: clee
 ** 
 ** ------------------------------- Revision History: ----------------------------
 ** <author>                        <data>       <version>   <desc>
 ** ------------------------------------------------------------------------------
 ** clee                            2013-06-03   1.0         Create this moudle
 ********************************************************************************/

package android.os;

/**
 * @hide
 */
interface IOppoService {
    /**
     * This part is for adjusting LCD frequency
     */
    int adjustLCDFrequency(int rate);
    
    /**
     * This part is for get acl state
     */
    int getSaveModeState();
    
    /**
     * This part is for set acl state
     */
    int setSaveModeState(int state);
    
    /**
     * This part is for writing raw partition
     */
    String readRawPartition(int id, int size);
    int writeRawPartition(int id, String content);
}