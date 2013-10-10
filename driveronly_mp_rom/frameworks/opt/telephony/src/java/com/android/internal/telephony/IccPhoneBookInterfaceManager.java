/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

/*
 * Copyright (C) 2006 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.internal.telephony;

import android.content.pm.PackageManager;
import android.os.AsyncResult;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.os.ServiceManager;

import com.android.internal.telephony.IccCardApplicationStatus.AppType;

import com.android.internal.telephony.gsm.UsimPBMemInfo;

import java.util.List;
import java.util.concurrent.atomic.AtomicBoolean;
import com.android.internal.telephony.gsm.SIMRecords;

//#ifdef VENDOR_EDIT
//opporom@Prd.CommApp.Telephony
import android.telephony.OppoTelephonyConstant;
//#endif

/**
 * SimPhoneBookInterfaceManager to provide an inter-process communication to
 * access ADN-like SIM records.
 */
public abstract class IccPhoneBookInterfaceManager extends IIccPhoneBook.Stub {
    protected static final boolean DBG = true;

    protected PhoneBase mPhone;
    protected AdnRecordCache mAdnCache;
    protected final Object mLock = new Object();
    protected int mRecordSize[];
    protected boolean mSuccess;
    protected List<AdnRecord> mRecords;
    protected int mErrorCause;

    protected static final boolean ALLOW_SIM_OP_IN_UI_THREAD = false;

    protected static final int EVENT_GET_SIZE_DONE = 1;
    protected static final int EVENT_LOAD_DONE = 2;
    protected static final int EVENT_UPDATE_DONE = 3;

    //#ifdef VENDOR_EDIT 
    //BaoZhu.Yu@Prd.CommApp.Telephony, 2012/06/11, Add for sim phonebook space and free space. */
    @android.annotation.OppoHook(level=android.annotation.OppoHook.OppoHookType.NEW_FIELD,
                                 property=android.annotation.OppoHook.OppoRomType.OPPO,
                                 note="null, only for Mtk") 
    private int simNameLeng;   

    @android.annotation.OppoHook(level=android.annotation.OppoHook.OppoHookType.NEW_FIELD,
                                 property=android.annotation.OppoHook.OppoRomType.OPPO,
                                 note="null, only for Mtk") 
    private int simNumberLeng;  

    @android.annotation.OppoHook(level=android.annotation.OppoHook.OppoHookType.NEW_FIELD,
                                 property=android.annotation.OppoHook.OppoRomType.OPPO,
                                 note="null, only for Mtk") 
    private boolean phonebookReady;
    //#endif 
    
    private static int sTimes = 1;

    protected Handler mBaseHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            AsyncResult ar;

            switch (msg.what) {
                case EVENT_GET_SIZE_DONE:
                                ar = (AsyncResult) msg.obj;
                    synchronized (mLock) {
                        if (ar.exception == null) {
                            mRecordSize = (int[]) ar.result;
                            // mRecordSize[0] is the record length
                            // mRecordSize[1] is the total length of the EF file
                            // mRecordSize[2] is the number of records in the EF
                            // file
                            logd("GET_RECORD_SIZE Size " + mRecordSize[0] +
                                    " total " + mRecordSize[1] +
                                    " #record " + mRecordSize[2]);
                        }
                        notifyPending(ar);
                    }
                    break;
                case EVENT_UPDATE_DONE:
                                ar = (AsyncResult) msg.obj;
                    synchronized (mLock) {
                        mSuccess = (ar.exception == null);
                        // MTK-START [mtk80601][111215][ALPS00093395]
                        if (!mSuccess) {
                            mErrorCause = getErrorCauseFromException(
                                    (CommandException) ar.exception);
                        } else {
                            mErrorCause = IccProvider.ERROR_ICC_PROVIDER_NO_ERROR;
                        }
                        logd("update done result: " + mErrorCause);
                        // MTK-END [mtk80601][111215][ALPS00093395]
                        notifyPending(ar);
                    }
                    break;
                case EVENT_LOAD_DONE:
                                ar = (AsyncResult) msg.obj;
                    synchronized (mLock) {
                        if (ar.exception == null) {
                            mRecords = (List<AdnRecord>) ar.result;
                            //#ifdef VENDOR_EDIT 
                            //BaoZhu.Yu@Prd.CommApp.Telephony, 2012/06/11, Add for                            
                            phonebookReady = true;
                            //#endif /* VENDOR_EDIT */                            
                        } else {
                            if (DBG) {
                                logd("Cannot load ADN records");
                            }
                            mRecords = null;
                        }
                        notifyPending(ar);
                    }
                    break;
                    default:
                        break;
            }
        }

        private void notifyPending(AsyncResult ar) {
            if (ar.userObj == null) {
                return;
            }
            try {
                AtomicBoolean status = (AtomicBoolean) ar.userObj;
                status.set(true);
                mLock.notifyAll();
            } catch (ClassCastException e) {
                //this may be caused by Duplicated notify,just ignore
                loge("notifyPending " + e.getMessage());
            }
        }
    };

    public IccPhoneBookInterfaceManager(PhoneBase p) {
        this.mPhone = p;
        //#ifdef VENDOR_EDIT 
        //BaoZhu.Yu@Prd.CommApp.Telephony, 2012/06/11, Add for
        phonebookReady = false;		
        simNameLeng = -1;
        simNumberLeng = -1;
        //#endif /* VENDOR_EDIT */			
        IccRecords r = mPhone.mIccRecords.get();
        if (r != null) {
            mAdnCache = r.getAdnCache();
        }
    }

    public void dispose() {
        //#ifdef VENDOR_EDIT 
        //BaoZhu.Yu@Prd.CommApp.Telephony, 2012/06/11, Add for	
        logd("IccPhoneBookInterfaceManager: reset...");
        phonebookReady = false;
        simNameLeng = -1;
        simNumberLeng = -1;
        //#endif /* VENDOR_EDIT */	
    }

    public void updateIccRecords(IccRecords iccRecords) {
        if (iccRecords != null) {
            mAdnCache = iccRecords.getAdnCache();
            logd("[updateIccRecords] Set mAdnCache value");
        } else {
            mAdnCache = null;
            logd("[updateIccRecords] Set mAdnCache value to null");
        }
    }

    protected void publish() {
        // NOTE service "simphonebook" added by IccSmsInterfaceManagerProxy
        ServiceManager.addService("simphonebook", this);
    }

    protected abstract void logd(String msg);

    protected abstract void loge(String msg);

    /**
     * Replace oldAdn with newAdn in ADN-like record in EF getAdnRecordsInEf
     * must be called at least once before this function, otherwise an error
     * will be returned. Currently the email field if set in the ADN record is
     * ignored. throws SecurityException if no WRITE_CONTACTS permission
     * 
     * @param efid must be one among EF_ADN, EF_FDN, and EF_SDN
     * @param oldTag adn tag to be replaced
     * @param oldPhoneNumber adn number to be replaced Set both oldTag and
     *            oldPhoneNubmer to "" means to replace an empty record, aka,
     *            insert new record
     * @param newTag adn tag to be stored
     * @param newPhoneNumber adn number ot be stored Set both newTag and
     *            newPhoneNubmer to "" means to replace the old record with
     *            empty one, aka, delete old record
     * @param pin2 required to update EF_FDN, otherwise must be null
     * @return true for success
     */
    public boolean updateAdnRecordsInEfBySearch(int efid,
            String oldTag, String oldPhoneNumber,
            String newTag, String newPhoneNumber, String pin2) {
        // MTK-START [mtk80601][111215][ALPS00093395]
        int result;

        result = updateAdnRecordsInEfBySearchWithError(
                efid, oldTag, oldPhoneNumber,
                newTag, newPhoneNumber, pin2);

        return result == IccProvider.ERROR_ICC_PROVIDER_NO_ERROR;
    }

    /**
     * Replace oldAdn with newAdn in ADN-like record in EF getAdnRecordsInEf
     * must be called at least once before this function, otherwise an error
     * will be returned. Currently the email field if set in the ADN record is
     * ignored. throws SecurityException if no WRITE_CONTACTS permission This
     * method will return why the error occurs.
     * 
     * @param efid must be one among EF_ADN, EF_FDN, and EF_SDN
     * @param oldTag adn tag to be replaced
     * @param oldPhoneNumber adn number to be replaced Set both oldTag and
     *            oldPhoneNubmer to "" means to replace an empty record, aka,
     *            insert new record
     * @param newTag adn tag to be stored
     * @param newPhoneNumber adn number ot be stored Set both newTag and
     *            newPhoneNubmer to "" means to replace the old record with
     *            empty one, aka, delete old record
     * @param pin2 required to update EF_FDN, otherwise must be null
     * @return ERROR_ICC_PROVIDER_* defined in the IccProvider
     */
    public synchronized int updateAdnRecordsInEfBySearchWithError(int efid,
            String oldTag, String oldPhoneNumber,
            String newTag, String newPhoneNumber, String pin2) {

        int index = -1;
        if (mPhone.getContext().checkCallingOrSelfPermission(
                android.Manifest.permission.WRITE_CONTACTS) != PackageManager.PERMISSION_GRANTED) {
            throw new SecurityException(
                    "Requires android.permission.WRITE_CONTACTS permission");
        }

        if (mAdnCache == null)
        {
            logd("updateAdnRecordsInEfBySearchWithError mAdnCache is null");
            return IccProvider.ERROR_ICC_PROVIDER_UNKNOWN;            
        }
        //#ifdef VENDOR_EDIT 
        //BaoZhu.Yu@Prd.CommApp.Telephony, 2012/06/11, Add for resolve handle sim phone book in fly mode  
        if(!phonebookReady) {
            logd("IccPhoneBookInterfaceManager: updateAdnRecordsInEfBySearchWithError: phonebook not ready.");
            return IccProvider.ERROR_ICC_PROVIDER_UNKNOWN;
        }  
        //#endif /* VENDOR_EDIT */

        if (DBG) {
            logd("updateAdnRecordsInEfBySearch: efid=" + efid +
                    " (" + oldTag + "," + oldPhoneNumber + ")" + "==>" +
                    " (" + newTag + " (" + newTag.length() + ")," + newPhoneNumber + ")" + " pin2="
                    + pin2);
        }
        efid = updateEfForIccType(efid);

        synchronized (mLock) {
            //#ifdef VENDOR_EDIT 
            //Hong.Liu@Prd.CommApp.Telephony, 2012/06/11, Add for avoid re-entrant in the same thread
            if(mAdnCache.hasCmdInProgress(efid)){
                logd("IccPhoneBookInterfaceManager: updateAdnRecordsInEfBySearchWithError: hasCmdInProgress");
                return IccProvider.ERROR_ICC_PROVIDER_GENERIC_FAILURE;
            }
            //#endif /* VENDOR_EDIT */
            checkThread();
            mSuccess = false;
            AtomicBoolean status = new AtomicBoolean(false);
            Message response = mBaseHandler.obtainMessage(EVENT_UPDATE_DONE, status);
            sTimes = (sTimes + 2) % 20000;
            response.arg1 = sTimes;
            AdnRecord oldAdn = new AdnRecord(oldTag, oldPhoneNumber);
            if (null == newPhoneNumber) {
                newPhoneNumber = "";
            }
            AdnRecord newAdn = new AdnRecord(newTag, newPhoneNumber);
            index = mAdnCache.updateAdnBySearch(efid, oldAdn, newAdn, pin2, response);
            waitForResult(status);
        }
        if (mErrorCause == IccProvider.ERROR_ICC_PROVIDER_NO_ERROR) {
            logd("updateAdnRecordsInEfBySearchWithError success index is " + index);
            return index;
        }
        return mErrorCause;
    }

    public synchronized int updateUsimPBRecordsInEfBySearchWithError(int efid,
            String oldTag, String oldPhoneNumber, String oldAnr, String oldGrpIds,
            String[] oldEmails,
            String newTag, String newPhoneNumber, String newAnr, String newGrpIds,
            String[] newEmails) {

        int index = -1;
        AtomicBoolean status = new AtomicBoolean(false);
        if (mPhone.getContext().checkCallingOrSelfPermission(
                android.Manifest.permission.WRITE_CONTACTS) != PackageManager.PERMISSION_GRANTED) {
            throw new SecurityException(
                    "Requires android.permission.WRITE_CONTACTS permission");
        }

        if (mAdnCache == null)
        {
            logd("updateUsimPBRecordsInEfBySearchWithError mAdnCache is null");
            return IccProvider.ERROR_ICC_PROVIDER_UNKNOWN;            
        }

        if (DBG) {
            logd("updateUsimPBRecordsInEfBySearchWithError: efid=" + efid +
                    " (" + oldTag + "," + oldPhoneNumber + "oldAnr" + oldAnr + " oldGrpIds "
                    + oldGrpIds + ")" + "==>" +
                    "(" + newTag + "," + newPhoneNumber + ")" + " newAnr= " + newAnr
                    + " newGrpIds = " + newGrpIds + " newEmails = " + newEmails);
        }
        synchronized (mLock) {
            //#ifdef VENDOR_EDIT 
            //Hong.Liu@Prd.CommApp.Telephony, 2012/06/11, Add for avoid re-entrant in the same thread
            if(mAdnCache.hasCmdInProgress(efid)){
                return IccProvider.ERROR_ICC_PROVIDER_GENERIC_FAILURE;
            }
            //#endif /* VENDOR_EDIT */
            checkThread();
            mSuccess = false;
            Message response = mBaseHandler.obtainMessage(EVENT_UPDATE_DONE, status);
            sTimes = (sTimes + 2) % 20000;
            response.arg1 = sTimes;
            AdnRecord oldAdn = new AdnRecord(oldTag, oldPhoneNumber);
            if (null == newPhoneNumber) {
                newPhoneNumber = "";
            }
            AdnRecord newAdn = new AdnRecord(0, 0, newTag, newPhoneNumber, newAnr, newEmails,
                    newGrpIds);
            index = mAdnCache.updateAdnBySearch(efid, oldAdn, newAdn, null, response);
            waitForResult(status);
        }
        if (mErrorCause == IccProvider.ERROR_ICC_PROVIDER_NO_ERROR) {
            logd("updateUsimPBRecordsInEfBySearchWithError success index is " + index);
            return index;
        }
        return mErrorCause;
    }

    public synchronized int updateUsimPBRecordsBySearchWithError(int efid, AdnRecord oldAdn,
            AdnRecord newAdn) {
        int index = -1;
        AtomicBoolean status = new AtomicBoolean(false);
        if (mPhone.getContext().checkCallingOrSelfPermission(
                android.Manifest.permission.WRITE_CONTACTS) != PackageManager.PERMISSION_GRANTED) {
            throw new SecurityException(
                    "Requires android.permission.WRITE_CONTACTS permission");
        }

        if (mAdnCache == null)
        {
            logd("updateUsimPBRecordsBySearchWithError mAdnCache is null");
            return IccProvider.ERROR_ICC_PROVIDER_UNKNOWN;            
        }

        if (DBG) {
            logd("updateUsimPBRecordsBySearchWithError: efid=" + efid +
                    " (" + oldAdn + ")" + "==>" + "(" + newAdn + ")");
        }
        synchronized (mLock) {
            checkThread();
            mSuccess = false;
            Message response = mBaseHandler.obtainMessage(EVENT_UPDATE_DONE, status);
            sTimes = (sTimes + 2) % 20000;
            response.arg1 = sTimes;
            if (newAdn.number == null) {
                newAdn.number = "";
            }
            index = mAdnCache.updateAdnBySearch(efid, oldAdn, newAdn, null, response);
            waitForResult(status);
        }
        if (mErrorCause == IccProvider.ERROR_ICC_PROVIDER_NO_ERROR) {
            logd("updateUsimPBRecordsBySearchWithError success index is " + index);
            return index;
        }
        return mErrorCause;
    }

    // MTK-END [mtk80601][111215][ALPS00093395]

    /**
     * Update an ADN-like EF record by record index This is useful for iteration
     * the whole ADN file, such as write the whole phone book or erase/format
     * the whole phonebook. Currently the email field if set in the ADN record
     * is ignored. throws SecurityException if no WRITE_CONTACTS permission
     * 
     * @param efid must be one among EF_ADN, EF_FDN, and EF_SDN
     * @param newTag adn tag to be stored
     * @param newPhoneNumber adn number to be stored Set both newTag and
     *            newPhoneNubmer to "" means to replace the old record with
     *            empty one, aka, delete old record
     * @param index is 1-based adn record index to be updated
     * @param pin2 required to update EF_FDN, otherwise must be null
     * @return true for success
     */
    public boolean updateAdnRecordsInEfByIndex(int efid, String newTag,
            String newPhoneNumber, int index, String pin2) {

        int result;

        result = updateAdnRecordsInEfByIndexWithError(
                efid, newTag,
                newPhoneNumber, index, pin2);

        return result == IccProvider.ERROR_ICC_PROVIDER_NO_ERROR;
    }

    /**
     * Update an ADN-like EF record by record index This is useful for iteration
     * the whole ADN file, such as write the whole phone book or erase/format
     * the whole phonebook. Currently the email field if set in the ADN record
     * is ignored. throws SecurityException if no WRITE_CONTACTS permission This
     * method will return why the error occurs
     * 
     * @param efid must be one among EF_ADN, EF_FDN, and EF_SDN
     * @param newTag adn tag to be stored
     * @param newPhoneNumber adn number to be stored Set both newTag and
     *            newPhoneNubmer to "" means to replace the old record with
     *            empty one, aka, delete old record
     * @param index is 1-based adn record index to be updated
     * @param pin2 required to update EF_FDN, otherwise must be null
     * @return ERROR_ICC_PROVIDER_* defined in the IccProvider
     */
    // MTK-START [mtk80601][111215][ALPS00093395]
    public synchronized int updateAdnRecordsInEfByIndexWithError(int efid, String newTag,
            String newPhoneNumber, int index, String pin2) {

        if (mPhone.getContext().checkCallingOrSelfPermission(
                android.Manifest.permission.WRITE_CONTACTS) != PackageManager.PERMISSION_GRANTED) {
            throw new SecurityException(
                    "Requires android.permission.WRITE_CONTACTS permission");
        }

        if (mAdnCache == null)
        {
            logd("updateAdnRecordsInEfByIndex mAdnCache is null");
            return IccProvider.ERROR_ICC_PROVIDER_UNKNOWN;            
        }
        //#ifdef VENDOR_EDIT 
        //BaoZhu.Yu@Prd.CommApp.Telephony, 2012/06/11, Add for resolve handle sim phone book in fly mode   
        if(!phonebookReady) {
            logd("IccPhoneBookInterfaceManager: updateAdnRecordsInEfByIndex: phonebook not ready.");
            return IccProvider.ERROR_ICC_PROVIDER_UNKNOWN;
        }      
        //#endif /* VENDOR_EDIT */

        if (DBG) {
            logd("updateAdnRecordsInEfByIndex: efid=" + efid +
                    " Index=" + index + " ==> " +
                    "(" + newTag + "," + newPhoneNumber + ")" + " pin2=" + pin2);
        }
        synchronized (mLock) {
            //#ifdef VENDOR_EDIT 
            //Hong.Liu@Prd.CommApp.Telephony, 2012/06/11, Add for avoid re-entrant in the same thread
            if(mAdnCache.hasCmdInProgress(efid)){
                return IccProvider.ERROR_ICC_PROVIDER_GENERIC_FAILURE;
            }
            //#endif /* VENDOR_EDIT */
            checkThread();
            mSuccess = false;
            AtomicBoolean status = new AtomicBoolean(false);
            Message response = mBaseHandler.obtainMessage(EVENT_UPDATE_DONE, status);
            if (null == newPhoneNumber) {
                newPhoneNumber = "";
            }
            AdnRecord newAdn = new AdnRecord(newTag, newPhoneNumber);
            mAdnCache.updateAdnByIndex(efid, newAdn, index, pin2, response);
            waitForResult(status);
        }
        return mErrorCause;
    }

    public synchronized int updateUsimPBRecordsInEfByIndexWithError(int efid, String newTag,
            String newPhoneNumber, String newAnr, String newGrpIds, String[] newEmails, int index) {

        if (mPhone.getContext().checkCallingOrSelfPermission(
                android.Manifest.permission.WRITE_CONTACTS) != PackageManager.PERMISSION_GRANTED) {
            throw new SecurityException(
                    "Requires android.permission.WRITE_CONTACTS permission");
        }

        if (mAdnCache == null)
        {
            logd("updateUsimPBRecordsInEfByIndexWithError mAdnCache is null");
            return IccProvider.ERROR_ICC_PROVIDER_UNKNOWN;            
        }

        if (DBG) {
            logd("updateUsimPBRecordsInEfByIndexWithError: efid=" + efid +
                    " Index=" + index + " ==> " +
                    "(" + newTag + "," + newPhoneNumber + ")" + " newAnr= " + newAnr
                    + " newGrpIds = " + newGrpIds + " newEmails = " + newEmails);
        }
        synchronized (mLock) {
            //#ifdef VENDOR_EDIT
            //Hong.Liu@Prd.CommApp.Telephony, 2012/02/01, Add for avoid re-entrant in the same thread
            if(mAdnCache.hasCmdInProgress(efid)){
                return IccProvider.ERROR_ICC_PROVIDER_GENERIC_FAILURE;
            }
            //#endif /* VENDOR_EDIT */
            checkThread();
            mSuccess = false;
            AtomicBoolean status = new AtomicBoolean(false);
            Message response = mBaseHandler.obtainMessage(EVENT_UPDATE_DONE, status);
            if (null == newPhoneNumber) {
                newPhoneNumber = "";
            }
            AdnRecord newAdn = new AdnRecord(efid, index, newTag, newPhoneNumber, newAnr,
                    newEmails, newGrpIds);
            mAdnCache.updateAdnByIndex(efid, newAdn, index, null, response);
            waitForResult(status);
        }
        return mErrorCause;

    }

    public synchronized int updateUsimPBRecordsByIndexWithError(int efid, AdnRecord record,
            int index) {

        if (mPhone.getContext().checkCallingOrSelfPermission(
                android.Manifest.permission.WRITE_CONTACTS) != PackageManager.PERMISSION_GRANTED) {
            throw new SecurityException(
                    "Requires android.permission.WRITE_CONTACTS permission");
        }

        if (mAdnCache == null)
        {
            logd("updateUsimPBRecordsByIndexWithError mAdnCache is null");
            return IccProvider.ERROR_ICC_PROVIDER_UNKNOWN;            
        }

        if (DBG) {
            logd("updateUsimPBRecordsByIndexWithError: efid=" + efid +
                    " Index=" + index + " ==> " + record);
        }
        synchronized (mLock) {
            checkThread();
            mSuccess = false;
            AtomicBoolean status = new AtomicBoolean(false);
            Message response = mBaseHandler.obtainMessage(EVENT_UPDATE_DONE, status);
            mAdnCache.updateAdnByIndex(efid, record, index, null, response);
            waitForResult(status);
        }
        return mErrorCause;

    }

    // MTK-END [mtk80601][111215][ALPS00093395]

    /**
     * Get the capacity of records in efid
     * 
     * @param efid the EF id of a ADN-like ICC
     * @return int[3] array recordSizes[0] is the single record length
     *         recordSizes[1] is the total length of the EF file recordSizes[2]
     *         is the number of records in the EF file
     */
    public abstract int[] getAdnRecordsSize(int efid);

    /**
     * Loads the AdnRecords in efid and returns them as a List of AdnRecords
     * throws SecurityException if no READ_CONTACTS permission
     * 
     * @param efid the EF id of a ADN-like ICC
     * @return List of AdnRecord
     */
    public synchronized List<AdnRecord> getAdnRecordsInEf(int efid) {

        if (mPhone.getContext().checkCallingOrSelfPermission(
                android.Manifest.permission.READ_CONTACTS) != PackageManager.PERMISSION_GRANTED) {
            throw new SecurityException(
                    "Requires android.permission.READ_CONTACTS permission");
        }

        efid = updateEfForIccType(efid);

        if (mAdnCache == null)
        {
            logd("getAdnRecordsInEF mAdnCache is null");
            return null;            
        }

        if (DBG) {
            logd("getAdnRecordsInEF: efid=" + efid);
        }
        synchronized (mLock) {
            //#ifdef VENDOR_EDIT 
            //Hong.Liu@Prd.CommApp.Telephony, 2012/02/01, Add for avoid re-entrant in the same thread
            if(mAdnCache.hasCmdInProgress(efid)){
				return null;
            }
            //#endif /* VENDOR_EDIT */		
            checkThread();
            AtomicBoolean status = new AtomicBoolean(false);
            Message response = mBaseHandler.obtainMessage(EVENT_LOAD_DONE, status);
            mAdnCache.requestLoadAllAdnLike(efid, mAdnCache.extensionEfForEf(efid), response);
            waitForResult(status);
        }
        return mRecords;
    }

    protected void checkThread() {
        if (!ALLOW_SIM_OP_IN_UI_THREAD) {
            // Make sure this isn't the UI thread, since it will block
            if (mBaseHandler.getLooper().equals(Looper.myLooper())) {
                loge("query() called on the main UI thread!");
                throw new IllegalStateException(
                        "You cannot call query on this provder from the main UI thread.");
            }
        }
    }

    protected void waitForResult(AtomicBoolean status) {
        while (!status.get()) {
            try {
                mLock.wait();
            } catch (InterruptedException e) {
                logd("interrupted while trying to update by search");
            }
        }
    }

    private int updateEfForIccType(int efid) {
        // Check if we are trying to read ADN records
        if (efid == IccConstants.EF_ADN) {
            if (mPhone.getCurrentUiccAppType() == AppType.APPTYPE_USIM) {
                return IccConstants.EF_PBR;
            }
        }
        return efid;
    }

    // MTK-START [mtk80601][111215][ALPS00093395]

    private int getErrorCauseFromException(CommandException e) {

        int ret;

        if (e == null) {
            return IccProvider.ERROR_ICC_PROVIDER_NO_ERROR;
        }

        switch (e.getCommandError()) {
            case GENERIC_FAILURE: /* occurs when Extension file is full(?) */
                ret = IccProvider.ERROR_ICC_PROVIDER_GENERIC_FAILURE;
                break;
            case DIAL_STRING_TOO_LONG:
                ret = IccProvider.ERROR_ICC_PROVIDER_NUMBER_TOO_LONG;
                break;
            case SIM_PUK2:
            case PASSWORD_INCORRECT:
                ret = IccProvider.ERROR_ICC_PROVIDER_PASSWORD_ERROR;
                break;
            case TEXT_STRING_TOO_LONG:
                ret = IccProvider.ERROR_ICC_PROVIDER_TEXT_TOO_LONG;
                break;
            case SIM_MEM_FULL:
                ret = IccProvider.ERROR_ICC_PROVIDER_STORAGE_FULL;
                break;
            case NOT_READY:
                ret = IccProvider.ERROR_ICC_PROVIDER_NOT_READY;
                break;
            case ADDITIONAL_NUMBER_STRING_TOO_LONG:
                ret = IccProvider.ERROR_ICC_PROVIDER_ANR_TOO_LONG;
                break;
            case ADN_LIST_NOT_EXIST:
                ret = IccProvider.ERROR_ICC_PROVIDER_ADN_LIST_NOT_EXIST;
                break;
            case EMAIL_SIZE_LIMIT:
                ret = IccProvider.ERROR_ICC_PROVIDER_EMAIL_FULL;
                break;
            case EMAIL_NAME_TOOLONG:
                ret = IccProvider.ERROR_ICC_PROVIDER_EMAIL_TOOLONG;
                break;
            default:
                ret = IccProvider.ERROR_ICC_PROVIDER_UNKNOWN;
                break;
        }

        return ret;
    }

    public void onPhbReady() {
        if (mAdnCache != null)
        {
            mAdnCache.requestLoadAllAdnLike(IccConstants.EF_ADN,
                    mAdnCache.extensionEfForEf(IccConstants.EF_ADN), null);
            //#ifdef VENDOR_EDIT 
            //BaoZhu.Yu@Prd.CommApp.Telephony, 2012/06/11, Add for LoadAllAdnLike then to change phonebookReady = true;
            phonebookReady = isPhbReady();
            //#endif /* VENDOR_EDIT */					
        }
    }

    public boolean isPhbReady() {
        //#ifndef VENDOR_EDIT 
        //BaoZhu.Yu@Prd.CommApp.Telephony,
        //return mPhone.getIccCard().isPhbReady();
        //#else /* VENDOR_EDIT */    
        SIMRecords simRecords = null;
        IccRecords r = mPhone.mIccRecords.get();
        logd("isPhbReady() r="+r);
        if (r != null) {
            simRecords = (SIMRecords)r;
            logd("isPhbReady()"+simRecords.isPhbReady());
            return simRecords.isPhbReady();
        } else {
            return false;
        }
        //#endif /* VENDOR_EDIT */
    }

    public List<UsimGroup> getUsimGroups() {
        return ((mAdnCache == null) ? null : mAdnCache.getUsimGroups());
    }

    public String getUSIMGroupById(int nGasId) {
        return ((mAdnCache == null) ? null : mAdnCache.getUSIMGroupById(nGasId));
    }

    public boolean removeUSIMGroupById(int nGasId) {
        return ((mAdnCache == null) ? false : mAdnCache.removeUSIMGroupById(nGasId));
    }

    public int insertUSIMGroup(String grpName) {
        return ((mAdnCache == null) ? -1 : mAdnCache.insertUSIMGroup(grpName));
    }

    public int updateUSIMGroup(int nGasId, String grpName) {
        return ((mAdnCache == null) ? -1 : mAdnCache.updateUSIMGroup(nGasId, grpName));
    }

    public boolean addContactToGroup(int adnIndex, int grpIndex) {
        return ((mAdnCache == null) ? false : mAdnCache.addContactToGroup(adnIndex, grpIndex));
    }

    public boolean removeContactFromGroup(int adnIndex, int grpIndex) {
        return ((mAdnCache == null) ? false : mAdnCache.removeContactFromGroup(adnIndex, grpIndex));
    }

    public int hasExistGroup(String grpName) {
        return ((mAdnCache == null) ? -1 : mAdnCache.hasExistGroup(grpName));
    }

    public int getUSIMGrpMaxNameLen() {
        return ((mAdnCache == null) ? -1 : mAdnCache.getUSIMGrpMaxNameLen());
    }

    public int getUSIMGrpMaxCount() {
        return ((mAdnCache == null) ? -1 : mAdnCache.getUSIMGrpMaxCount());
    }

    public List<AlphaTag> getUSIMAASList() {
        return ((mAdnCache == null) ? null : mAdnCache.getUSIMAASList());
    }

    public String getUSIMAASById(int index) {
        return ((mAdnCache == null) ? null : mAdnCache.getUSIMAASById(index));
    }

    public boolean removeUSIMAASById(int index, int pbrIndex) {
        return ((mAdnCache == null) ? false : mAdnCache.removeUSIMAASById(index, pbrIndex));
    }

    public int insertUSIMAAS(String aasName) {
        return ((mAdnCache == null) ? -1 : mAdnCache.insertUSIMAAS(aasName));
    }

    public boolean updateUSIMAAS(int index, int pbrIndex, String aasName) {
        return ((mAdnCache == null) ? false : mAdnCache.updateUSIMAAS(index, pbrIndex, aasName));
    }

    /**
     * @param adnIndex: ADN index
     * @param aasIndex: change AAS to the value refered by aasIndex, -1 means
     *            remove
     * @return
     */
    public boolean updateADNAAS(int adnIndex, int aasIndex) {
        return ((mAdnCache == null) ? false : mAdnCache.updateADNAAS(adnIndex, aasIndex));
    }

    public int getAnrCount() {
        return ((mAdnCache == null) ? 0 : mAdnCache.getAnrCount());
    }

    public int getUSIMAASMaxCount() {
        return ((mAdnCache == null) ? -1 : mAdnCache.getUSIMAASMaxCount());
    }

    public int getUSIMAASMaxNameLen() {
        return ((mAdnCache == null) ? -1 : mAdnCache.getUSIMAASMaxNameLen());
    }

    /**
     * M: indicate if SNE exist
     * 
     * @return true if USIM has SNE record
     */
    public boolean hasSne() {
        return ((mAdnCache == null) ? false :mAdnCache.hasSne());
    }

    /**
     * M:
     * 
     * @return the length of SNE record
     */
    public int getSneRecordLen() {
        return ((mAdnCache == null) ? -1 : mAdnCache.getSneRecordLen());
    }

    // M for LGE
    public UsimPBMemInfo[] getPhonebookMemStorageExt() {
        return ((mAdnCache == null) ? null : mAdnCache.getPhonebookMemStorageExt());
    }
    // MTK-END [mtk80601][111215][ALPS00093395]

    //#ifdef VENDOR_EDIT 
    //BaoZhu.Yu@Prd.CommApp.Telephony, 2012/06/11, Add for
    @android.annotation.OppoHook(level=android.annotation.OppoHook.OppoHookType.NEW_METHOD,
                                 property=android.annotation.OppoHook.OppoRomType.OPPO,
                                 note="null, only for MTK platform")
    public int oppoGetSimPhonebookAllSpace() {
        if(!phonebookReady) {
            logd("IccPhoneBookInterfaceManager: oppoGetSimPhonebookAllSpace: phonebook not ready.");
            return -1;
        }

		return mAdnCache.oppoGetTotalAdn();        
    }

    @android.annotation.OppoHook(level=android.annotation.OppoHook.OppoHookType.NEW_METHOD,
                                 property=android.annotation.OppoHook.OppoRomType.OPPO,
                                 note="null, only for MTK platform")
    public int oppoGetSimPhonebookUsedSpace() {
        if(!phonebookReady) {
            logd("IccPhoneBookInterfaceManager: oppoGetSimPhonebookUsedSpace: phonebook not ready.");
            return -1;
        }
        
        return mAdnCache.oppoGetUesedAdn();
    }

    @android.annotation.OppoHook(level=android.annotation.OppoHook.OppoHookType.NEW_METHOD,
                                 property=android.annotation.OppoHook.OppoRomType.OPPO,
                                 note="null, only for MTK platform")
    public int oppoGetSimPhonebookNameLength() {
        if(!phonebookReady) {
            logd("IccPhoneBookInterfaceManager: oppoGetSimPhonebookNameLength: phonebook not ready.");
            return -1;
        }

        synchronized(mLock) {
            if(simNameLeng <= 0) {                                
                checkThread();
                AtomicBoolean status = new AtomicBoolean(false);
                Message response = mOppoHandler.obtainMessage(OppoTelephonyConstant.OPPO_EVENT_GET_ADN_FIELD_INFO_DONE,status);                
                mAdnCache.oppoGetADNFieldsInfo(IccConstants.EF_ADN, response);
                waitForResult(status);
            }
        }
        
        return  simNameLeng;
    }

    @android.annotation.OppoHook(level=android.annotation.OppoHook.OppoHookType.NEW_METHOD,
                                 property=android.annotation.OppoHook.OppoRomType.OPPO,
                                 note="null, only for MTK platform")
    public int oppoGetSimPhonebookNumberLength() {
        if(!phonebookReady) {
            logd("IccPhoneBookInterfaceManager: oppoGetSimPhonebookNumberLength: phonebook not ready.");
            return -1;
        }

        synchronized(mLock) {
            if(simNumberLeng <= 0 ) {
                checkThread();
                AtomicBoolean status = new AtomicBoolean(false);
                Message response = mBaseHandler.obtainMessage(OppoTelephonyConstant.OPPO_EVENT_GET_ADN_FIELD_INFO_DONE,status);
                mAdnCache.oppoGetADNFieldsInfo(IccConstants.EF_ADN, response);
                waitForResult(status);
            }
        }
        
        logd("IccPhoneBookInterfaceManager: oppoGetSimPhonebookNumberLength: simNumberLeng = " + simNumberLeng);
        return simNumberLeng;
    }

    @android.annotation.OppoHook(level=android.annotation.OppoHook.OppoHookType.NEW_METHOD,
                                 property=android.annotation.OppoHook.OppoRomType.OPPO,
                                 note="get sim phonebook's email length, only for MTK platform")
    public int oppoGetSimPhonebookEmailLength() {
        if(!phonebookReady) {
            logd("IccPhoneBookInterfaceManager: oppoGetSimPhonebookNumberLength: phonebook not ready.");
            return -1;
        }        
        return mAdnCache.getUsimEmailLength();
    }    
   
    Handler mOppoHandler = new Handler() 
    {
        @Override
        public void handleMessage(Message msg) 
        {
            AsyncResult ar;
            switch (msg.what) 
            {
                case OppoTelephonyConstant.OPPO_EVENT_GET_ADN_FIELD_INFO_DONE:
                {
                    ar = (AsyncResult)msg.obj;
                    synchronized (mLock) {
                        if( ar.exception == null ) {
                            
                            int[] fieldInfo = (int[])ar.result;
                            if(fieldInfo.length == 2) {
                                simNumberLeng = fieldInfo[0];
                                simNameLeng = fieldInfo[1];
                            } else {
                            }
                        }
                        notifyPending(ar);
                    }
                    break;
                }
            }
        }

        private void notifyPending(AsyncResult ar) {
            if (ar.userObj == null) {
                return;
            }
            AtomicBoolean status = (AtomicBoolean) ar.userObj;
            status.set(true);
            mLock.notifyAll();
        }
    };
    //#endif /* VENDOR_EDIT */	
}
