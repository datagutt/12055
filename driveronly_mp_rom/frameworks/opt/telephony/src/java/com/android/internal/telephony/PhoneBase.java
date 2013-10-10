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
 * Copyright (C) 2007 The Android Open Source Project
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

import android.app.ActivityManagerNative;
import android.app.IActivityManager;
import android.content.Context;
import android.content.res.Configuration;
import android.content.SharedPreferences;
import android.net.LinkCapabilities;
import android.net.LinkProperties;
import android.net.wifi.WifiManager;
import android.os.AsyncResult;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.os.RegistrantList;
import android.os.SystemProperties;
import android.preference.PreferenceManager;
import android.provider.Settings;
import android.telephony.CellInfo;
import android.telephony.ServiceState;
import android.telephony.SignalStrength;
import android.text.TextUtils;
import android.util.Log;

import com.android.internal.R;
import com.android.internal.telephony.IccCardApplicationStatus.AppState;
import com.android.internal.telephony.IccCardApplicationStatus.AppType;
import com.android.internal.telephony.gsm.UsimServiceTable;
import com.android.internal.telephony.ims.IsimRecords;
import com.android.internal.telephony.test.SimulatedRadioControl;
import com.android.internal.telephony.uicc.UiccController;
import com.android.internal.telephony.gsm.SIMRecords;

import java.io.FileDescriptor;
import java.io.PrintWriter;
import java.util.List;
import java.util.Locale;
import java.util.concurrent.atomic.AtomicReference;

//MTK-START [mtk04070][111117][ALPS00093395]MTK added
import com.android.internal.telephony.gsm.SmsBroadcastConfigInfo;
import com.android.internal.telephony.gsm.NetworkInfoWithAcT;
import java.util.List;
//MTK-END [mtk04070][111117][ALPS00093395]MTK added



/**
 * (<em>Not for SDK use</em>)
 * A base implementation for the com.android.internal.telephony.Phone interface.
 *
 * Note that implementations of Phone.java are expected to be used
 * from a single application thread. This should be the same thread that
 * originally called PhoneFactory to obtain the interface.
 *
 *  {@hide}
 *
 */

public abstract class PhoneBase extends Handler implements Phone {
    private static final String LOG_TAG = "PHONE";
    private static final boolean LOCAL_DEBUG = true;

    // Key used to read and write the saved network selection numeric value
    public static final String NETWORK_SELECTION_KEY = "network_selection_key";
    // Key used to read and write the saved network selection operator name
    public static final String NETWORK_SELECTION_NAME_KEY = "network_selection_name_key";

    //MTK-START [mtk04070][111117][ALPS00093395]MTK added
    public static final String NETWORK_SELECTION_KEY_2 = "network_selection_key_2";
    public static final String NETWORK_SELECTION_NAME_KEY_2 = "network_selection_name_key_2";
    //MTK-END [mtk04070][111117][ALPS00093395]MTK added


    // Key used to read/write "disable data connection on boot" pref (used for testing)
    public static final String DATA_DISABLED_ON_BOOT_KEY = "disabled_on_boot_key";

    /* Event Constants */
    protected static final int EVENT_RADIO_AVAILABLE             = 1;
    /** Supplementary Service Notification received. */
    protected static final int EVENT_SSN                         = 2;
    protected static final int EVENT_SIM_RECORDS_LOADED          = 3;
    protected static final int EVENT_MMI_DONE                    = 4;
    protected static final int EVENT_RADIO_ON                    = 5;
    protected static final int EVENT_GET_BASEBAND_VERSION_DONE   = 6;
    protected static final int EVENT_USSD                        = 7;
    protected static final int EVENT_RADIO_OFF_OR_NOT_AVAILABLE  = 8;
    protected static final int EVENT_GET_IMEI_DONE               = 9;
    protected static final int EVENT_GET_IMEISV_DONE             = 10;
    protected static final int EVENT_GET_SIM_STATUS_DONE         = 11;
    protected static final int EVENT_SET_CALL_FORWARD_DONE       = 12;
    protected static final int EVENT_GET_CALL_FORWARD_DONE       = 13;
    protected static final int EVENT_CALL_RING                   = 14;
    protected static final int EVENT_CALL_RING_CONTINUE          = 15;

    // Used to intercept the carrier selection calls so that
    // we can save the values.
    protected static final int EVENT_SET_NETWORK_MANUAL_COMPLETE    = 16;
    protected static final int EVENT_SET_NETWORK_AUTOMATIC_COMPLETE = 17;
    protected static final int EVENT_SET_CLIR_COMPLETE              = 18;
    protected static final int EVENT_REGISTERED_TO_NETWORK          = 19;
    protected static final int EVENT_SET_VM_NUMBER_DONE             = 20;
    // Events for CDMA support
    protected static final int EVENT_GET_DEVICE_IDENTITY_DONE       = 21;
    protected static final int EVENT_RUIM_RECORDS_LOADED            = 22;
    protected static final int EVENT_NV_READY                       = 23;
    protected static final int EVENT_SET_ENHANCED_VP                = 24;
    protected static final int EVENT_EMERGENCY_CALLBACK_MODE_ENTER  = 25;
    protected static final int EVENT_EXIT_EMERGENCY_CALLBACK_RESPONSE = 26;
    protected static final int EVENT_CDMA_SUBSCRIPTION_SOURCE_CHANGED = 27;
    // other
    protected static final int EVENT_SET_NETWORK_AUTOMATIC          = 28;
    protected static final int EVENT_NEW_ICC_SMS                    = 29;
    protected static final int EVENT_ICC_RECORD_EVENTS              = 30;
    protected static final int EVENT_ICC_CHANGED                    = 31;

    //MTK-START [mtk04070][111117][ALPS00093395]MTK added
    protected static final int EVENT_SIP_CALL_RING               = 32;
    // Events for Call Forwarding Info flag
    protected static final int EVENT_CFU_IND = 33;
    // Events for Call Related SuppSvc Notificaiton
    protected static final int EVENT_CRSS_IND = 34;
    //Add by mtk80372 for Barcode Number
    protected static final int EVENT_GET_BARCODE_NUMBER = 35;
    //3G switch
    protected static final int EVENT_GET_3G_CAPABILITY = 36;
    protected static final int EVENT_SET_3G_CAPABILITY = 37;
    //MTK-END [mtk04070][111117][ALPS00093395]MTK added

    // ALPS00294581 Replace "RIL_UNSOL_SIM_MISSING in RIL.java" with "acively query SIM missing status"
    protected static final int EVENT_QUERY_SIM_MISSING_STATUS = 38;

    //MTK-START [mtk80950][120410][ALPS00266631]check whether download calibration data or not
    protected static final int EVENT_GET_CALIBRATION_DATA_DONE = 39;
    //MTK-END [mtk80950][120410][ALPS00266631]check whether download calibration data or not



    //via support start
    protected static final int EVENT_QUERY_UIM_INSERTED_STATUS_DONE = 100;
    protected static final int EVENT_GET_UIM_INSERT_STATUS_RETRY = 101;
    // via support end 

    protected static final int EVENT_RADIO_NOT_AVAILABLE = 200;

    // Key used to read/write current CLIR setting
    public static final String CLIR_KEY = "clir_key";
    /// M: Add key for SIM2 CLIR setting.
    public static final String CLIR_KEY_2 = "clir_key_2";

    // Key used to read/write "disable DNS server check" pref (used for testing)
    public static final String DNS_SERVER_CHECK_DISABLED_KEY = "dns_server_check_disabled_key";

    /* Instance Variables */
    public CommandsInterface mCM;
    boolean mDnsCheckDisabled;
    public DataConnectionTracker mDataConnectionTracker;
    boolean mDoesRilSendMultipleCallRing;
    int mCallRingContinueToken;
    int mCallRingDelay;
    public boolean mIsTheCurrentActivePhone = true;
    boolean mIsVoiceCapable = true;
    protected UiccController mUiccController = null;
    public AtomicReference<IccRecords> mIccRecords = new AtomicReference<IccRecords>();
    protected AtomicReference<IccCard> mIccCard = new AtomicReference<IccCard>();
    public SmsStorageMonitor mSmsStorageMonitor;
    public SmsUsageMonitor mSmsUsageMonitor;
    protected AtomicReference<UiccCardApplication> mUiccApplication =
            new AtomicReference<UiccCardApplication>();
    public SMSDispatcher mSMS;

    //MTK-START [mtk04070][111117][ALPS00093395]Used when received EVENT_SIP_CALL_RING
    int mSipCallRingContinueToken = 0;
    //MTK-END [mtk04070][111117][ALPS00093395]Used when received EVENT_SIP_CALL_RING


    /**
     * Set a system property, unless we're in unit test mode
     */
    public void
    setSystemProperty(String property, String value) {
        if(getUnitTestMode()) {
            return;
        }
        SystemProperties.set(property, value);
    }


    protected final RegistrantList mPreciseCallStateRegistrants
            = new RegistrantList();

    protected final RegistrantList mNewRingingConnectionRegistrants
            = new RegistrantList();

    protected final RegistrantList mIncomingRingRegistrants
            = new RegistrantList();

    protected final RegistrantList mDisconnectRegistrants
            = new RegistrantList();

    protected final RegistrantList mServiceStateRegistrants
            = new RegistrantList();

    protected final RegistrantList mMmiCompleteRegistrants
            = new RegistrantList();

    protected final RegistrantList mMmiRegistrants
            = new RegistrantList();

    protected final RegistrantList mUnknownConnectionRegistrants
            = new RegistrantList();

    protected final RegistrantList mSuppServiceFailedRegistrants
            = new RegistrantList();

    //MTK-START [mtk04070][111117][ALPS00093395]Used for VT
    /* vt start */
    protected final RegistrantList mVtReplaceDisconnectRegistrants
            = new RegistrantList();
    /* vt end */
    //MTK-END [mtk04070][111117][ALPS00093395]Used for VT

    protected Looper mLooper; /* to insure registrants are in correct thread*/

    protected final Context mContext;

    /**
     * PhoneNotifier is an abstraction for all system-wide
     * state change notification. DefaultPhoneNotifier is
     * used here unless running we're inside a unit test.
     */
    protected PhoneNotifier mNotifier;

    protected SimulatedRadioControl mSimulatedRadioControl;

    boolean mUnitTestMode;

    // sim slot id of phone
    protected int mySimId;

    /**
     * Constructs a PhoneBase in normal (non-unit test) mode.
     *
     * @param notifier An instance of DefaultPhoneNotifier,
     * @param context Context object from hosting application
     * @param notifier An instance of DefaultPhoneNotifier,
     * unless unit testing.
     * @param ci the CommandsInterface
     */
    protected PhoneBase(PhoneNotifier notifier, Context context, CommandsInterface ci) {
        this(notifier, context, ci, false);
    }

    /**
     * Constructs a PhoneBase in normal (non-unit test) mode.
     *
     * @param notifier An instance of DefaultPhoneNotifier,
     * @param context Context object from hosting application
     * @param notifier An instance of DefaultPhoneNotifier,
     * unless unit testing.
     * @param ci the CommandsInterface
     * @param simId the slot id
     */
    protected PhoneBase(PhoneNotifier notifier, Context context, CommandsInterface ci, int simId) {
        this(notifier, context, ci, false, simId);
    }

    /**
     * Constructs a PhoneBase in normal (non-unit test) mode.
     *
     * @param notifier An instance of DefaultPhoneNotifier,
     * @param context Context object from hosting application
     * @param notifier An instance of DefaultPhoneNotifier,
     * unless unit testing.
     * @param ci is CommandsInterface
     * @param unitTestMode when true, prevents notifications
     * of state change events
     */
    protected PhoneBase(PhoneNotifier notifier, Context context, CommandsInterface ci,
            boolean unitTestMode) {
        this.mNotifier = notifier;
        this.mContext = context;
        mLooper = Looper.myLooper();
        mCM = ci;

        setPropertiesByCarrier();

        setUnitTestMode(unitTestMode);

        SharedPreferences sp = PreferenceManager.getDefaultSharedPreferences(context);
        mDnsCheckDisabled = sp.getBoolean(DNS_SERVER_CHECK_DISABLED_KEY, false);
        mCM.setOnCallRing(this, EVENT_CALL_RING, null);

        /* "Voice capable" means that this device supports circuit-switched
        * (i.e. voice) phone calls over the telephony network, and is allowed
        * to display the in-call UI while a cellular voice call is active.
        * This will be false on "data only" devices which can't make voice
        * calls and don't support any in-call UI.
        */
        mIsVoiceCapable = mContext.getResources().getBoolean(
                com.android.internal.R.bool.config_voice_capable);

        /**
         *  Some RIL's don't always send RIL_UNSOL_CALL_RING so it needs
         *  to be generated locally. Ideally all ring tones should be loops
         * and this wouldn't be necessary. But to minimize changes to upper
         * layers it is requested that it be generated by lower layers.
         *
         * By default old phones won't have the property set but do generate
         * the RIL_UNSOL_CALL_RING so the default if there is no property is
         * true.
         */
        mDoesRilSendMultipleCallRing = SystemProperties.getBoolean(
                TelephonyProperties.PROPERTY_RIL_SENDS_MULTIPLE_CALL_RING, true);
        Log.d(LOG_TAG, "mDoesRilSendMultipleCallRing=" + mDoesRilSendMultipleCallRing);

        mCallRingDelay = SystemProperties.getInt(
                TelephonyProperties.PROPERTY_CALL_RING_DELAY, 3000);
        Log.d(LOG_TAG, "mCallRingDelay=" + mCallRingDelay);

        // Initialize device storage and outgoing SMS usage monitors for SMSDispatchers.
        mSmsStorageMonitor = new SmsStorageMonitor(this);
        mSmsUsageMonitor = new SmsUsageMonitor(context);
        mUiccController = UiccController.getInstance();
        mUiccController.registerForIccChanged(this, EVENT_ICC_CHANGED, null);
    }

    /**
     * Constructs a PhoneBase in normal (non-unit test) mode.
     *
     * @param notifier An instance of DefaultPhoneNotifier,
     * @param context Context object from hosting application
     * @param notifier An instance of DefaultPhoneNotifier,
     * unless unit testing.
     * @param ci is CommandsInterface
     * @param unitTestMode when true, prevents notifications
     * of state change events
     * @param simId the slot id
     */
    protected PhoneBase(PhoneNotifier notifier, Context context, CommandsInterface ci,
            boolean unitTestMode, int simId) {
        this.mNotifier = notifier;
        this.mContext = context;
        mLooper = Looper.myLooper();
        mCM = ci;
        mySimId = simId;
        Log.d(LOG_TAG, "[PhoneBase] mysimId:" + mySimId);
        
        setPropertiesByCarrier();

        setUnitTestMode(unitTestMode);

        SharedPreferences sp = PreferenceManager.getDefaultSharedPreferences(context);
        mDnsCheckDisabled = sp.getBoolean(DNS_SERVER_CHECK_DISABLED_KEY, false);
        mCM.setOnCallRing(this, EVENT_CALL_RING, null);

        /* "Voice capable" means that this device supports circuit-switched
        * (i.e. voice) phone calls over the telephony network, and is allowed
        * to display the in-call UI while a cellular voice call is active.
        * This will be false on "data only" devices which can't make voice
        * calls and don't support any in-call UI.
        */
        mIsVoiceCapable = mContext.getResources().getBoolean(
                com.android.internal.R.bool.config_voice_capable);

        /**
         *  Some RIL's don't always send RIL_UNSOL_CALL_RING so it needs
         *  to be generated locally. Ideally all ring tones should be loops
         * and this wouldn't be necessary. But to minimize changes to upper
         * layers it is requested that it be generated by lower layers.
         *
         * By default old phones won't have the property set but do generate
         * the RIL_UNSOL_CALL_RING so the default if there is no property is
         * true.
         */
        mDoesRilSendMultipleCallRing = SystemProperties.getBoolean(
                TelephonyProperties.PROPERTY_RIL_SENDS_MULTIPLE_CALL_RING, true);
        Log.d(LOG_TAG, "mDoesRilSendMultipleCallRing=" + mDoesRilSendMultipleCallRing);

        mCallRingDelay = SystemProperties.getInt(
                TelephonyProperties.PROPERTY_CALL_RING_DELAY, 3000);
        Log.d(LOG_TAG, "mCallRingDelay=" + mCallRingDelay);

        // Initialize device storage and outgoing SMS usage monitors for SMSDispatchers.
        mSmsStorageMonitor = new SmsStorageMonitor(this);
        mSmsUsageMonitor = new SmsUsageMonitor(context);
        mUiccController = UiccController.getInstance(mySimId);
        mUiccController.registerForIccChanged(this, EVENT_ICC_CHANGED, null);
    }

    public void dispose() {
        synchronized(PhoneProxy.lockForRadioTechnologyChange) {
            Log.d(LOG_TAG, "[PhoneBase] dispose");
            mCM.unSetOnCallRing(this);
            // Must cleanup all connectionS and needs to use sendMessage!
            mDataConnectionTracker.cleanUpAllConnections(null);
            mIsTheCurrentActivePhone = false;
            // Dispose the SMS usage and storage monitors
            mSmsStorageMonitor.dispose();
            mSmsUsageMonitor.dispose();
            mUiccController.unregisterForIccChanged(this);
        }
    }

    public void removeReferences() {
        mSmsStorageMonitor = null;
        mSmsUsageMonitor = null;
        mSMS = null;
        mIccRecords.set(null);
        mUiccApplication.set(null);
        mDataConnectionTracker = null;
        mUiccController = null;
    }

    /**
     * When overridden the derived class needs to call
     * super.handleMessage(msg) so this method has a
     * a chance to process the message.
     *
     * @param msg
     */
    @Override
    public void handleMessage(Message msg) {
        AsyncResult ar;

        switch(msg.what) {
            case EVENT_CALL_RING:
                Log.d(LOG_TAG, "Event EVENT_CALL_RING Received state=" + getState());
                ar = (AsyncResult)msg.obj;
                if (ar.exception == null) {
                    PhoneConstants.State state = getState();
                    if ((!mDoesRilSendMultipleCallRing)
                            && ((state == PhoneConstants.State.RINGING) || (state == PhoneConstants.State.IDLE))) {
                        mCallRingContinueToken += 1;
                        sendIncomingCallRingNotification(mCallRingContinueToken);
                    } else {
                        notifyIncomingRing();
                    }
                }
                break;

            case EVENT_CALL_RING_CONTINUE:
                Log.d(LOG_TAG, "Event EVENT_CALL_RING_CONTINUE Received stat=" + getState());
                if (getState() == PhoneConstants.State.RINGING) {
                    sendIncomingCallRingNotification(msg.arg1);
                }
                break;
            case EVENT_ICC_CHANGED:
                Log.d(LOG_TAG, "Event EVENT_ICC_CHANGED Received stat=" + getState());
                onUpdateIccAvailability();
                break;
            //MTK-START [mtk04070][111117][ALPS00093395]SIP call ring event
            case EVENT_SIP_CALL_RING:
                int token = msg.arg1;
                Log.d(LOG_TAG, "Event EVENT_SIP_CALL_RING Received state=" + getState() + " token=" + token);
                if (getState() == PhoneConstants.State.RINGING && token == mSipCallRingContinueToken) {
                    notifyIncomingRing();
                    mSipCallRingContinueToken++;
                    sendMessageDelayed(
                    obtainMessage(EVENT_SIP_CALL_RING, mSipCallRingContinueToken, 0), mCallRingDelay);
                }
                break;
            //MTK-END [mtk04070][111117][ALPS00093395]SIP call ring event
                
            default:
                throw new RuntimeException("unexpected event not handled");
        }
    }

    // Inherited documentation suffices.
    public Context getContext() {
        return mContext;
    }

    // Will be called when icc changed
    protected abstract void onUpdateIccAvailability();

    /**
     * Disables the DNS check (i.e., allows "0.0.0.0").
     * Useful for lab testing environment.
     * @param b true disables the check, false enables.
     */
    public void disableDnsCheck(boolean b) {
        mDnsCheckDisabled = b;
        SharedPreferences sp = PreferenceManager.getDefaultSharedPreferences(getContext());
        SharedPreferences.Editor editor = sp.edit();
        editor.putBoolean(DNS_SERVER_CHECK_DISABLED_KEY, b);
        editor.apply();
    }

    /**
     * Returns true if the DNS check is currently disabled.
     */
    public boolean isDnsCheckDisabled() {
        return mDnsCheckDisabled;
    }

    // Inherited documentation suffices.
    public void registerForPreciseCallStateChanged(Handler h, int what, Object obj) {
        checkCorrectThread(h);

        mPreciseCallStateRegistrants.addUnique(h, what, obj);
    }

    // Inherited documentation suffices.
    public void unregisterForPreciseCallStateChanged(Handler h) {
        mPreciseCallStateRegistrants.remove(h);
    }

    /**
     * Subclasses of Phone probably want to replace this with a
     * version scoped to their packages
     */
    protected void notifyPreciseCallStateChangedP() {
        AsyncResult ar = new AsyncResult(null, this, null);
        mPreciseCallStateRegistrants.notifyRegistrants(ar);
    }

    // Inherited documentation suffices.
    public void registerForUnknownConnection(Handler h, int what, Object obj) {
        checkCorrectThread(h);

        mUnknownConnectionRegistrants.addUnique(h, what, obj);
    }

    // Inherited documentation suffices.
    public void unregisterForUnknownConnection(Handler h) {
        mUnknownConnectionRegistrants.remove(h);
    }

    // Inherited documentation suffices.
    public void registerForNewRingingConnection(
            Handler h, int what, Object obj) {
        checkCorrectThread(h);

        mNewRingingConnectionRegistrants.addUnique(h, what, obj);
    }

    // Inherited documentation suffices.
    public void unregisterForNewRingingConnection(Handler h) {
        mNewRingingConnectionRegistrants.remove(h);
    }

    // Inherited documentation suffices.
    public void registerForInCallVoicePrivacyOn(Handler h, int what, Object obj){
        mCM.registerForInCallVoicePrivacyOn(h,what,obj);
    }

    // Inherited documentation suffices.
    public void unregisterForInCallVoicePrivacyOn(Handler h){
        mCM.unregisterForInCallVoicePrivacyOn(h);
    }

    // Inherited documentation suffices.
    public void registerForInCallVoicePrivacyOff(Handler h, int what, Object obj){
        mCM.registerForInCallVoicePrivacyOff(h,what,obj);
    }

    // Inherited documentation suffices.
    public void unregisterForInCallVoicePrivacyOff(Handler h){
        mCM.unregisterForInCallVoicePrivacyOff(h);
    }

    // Inherited documentation suffices.
    public void registerForIncomingRing(
            Handler h, int what, Object obj) {
        checkCorrectThread(h);

        mIncomingRingRegistrants.addUnique(h, what, obj);
    }

    // Inherited documentation suffices.
    public void unregisterForIncomingRing(Handler h) {
        mIncomingRingRegistrants.remove(h);
    }

    // Inherited documentation suffices.
    public void registerForDisconnect(Handler h, int what, Object obj) {
        checkCorrectThread(h);

        mDisconnectRegistrants.addUnique(h, what, obj);
    }

    // Inherited documentation suffices.
    public void unregisterForDisconnect(Handler h) {
        mDisconnectRegistrants.remove(h);
    }

    // Inherited documentation suffices.
    public void registerForSuppServiceFailed(Handler h, int what, Object obj) {
        checkCorrectThread(h);

        mSuppServiceFailedRegistrants.addUnique(h, what, obj);
    }

    // Inherited documentation suffices.
    public void unregisterForSuppServiceFailed(Handler h) {
        mSuppServiceFailedRegistrants.remove(h);
    }

    // Inherited documentation suffices.
    public void registerForMmiInitiate(Handler h, int what, Object obj) {
        checkCorrectThread(h);

        mMmiRegistrants.addUnique(h, what, obj);
    }

    // Inherited documentation suffices.
    public void unregisterForMmiInitiate(Handler h) {
        mMmiRegistrants.remove(h);
    }

    // Inherited documentation suffices.
    public void registerForMmiComplete(Handler h, int what, Object obj) {
        checkCorrectThread(h);

        mMmiCompleteRegistrants.addUnique(h, what, obj);
    }

    // Inherited documentation suffices.
    public void unregisterForMmiComplete(Handler h) {
        checkCorrectThread(h);

        mMmiCompleteRegistrants.remove(h);
    }

    /**
     * Method to retrieve the saved operator id from the Shared Preferences
     */
    private String getSavedNetworkSelection() {
        // open the shared preferences and search with our key.
        SharedPreferences sp = PreferenceManager.getDefaultSharedPreferences(getContext());
        return sp.getString(NETWORK_SELECTION_KEY, "");
    }

    /**
     * Method to restore the previously saved operator id, or reset to
     * automatic selection, all depending upon the value in the shared
     * preferences.
     */
    public void restoreSavedNetworkSelection(Message response) {
        // retrieve the operator id
        String networkSelection = getSavedNetworkSelection();

        // set to auto if the id is empty, otherwise select the network.
        if (TextUtils.isEmpty(networkSelection)) {
            mCM.setNetworkSelectionModeAutomatic(response);
        } else {
            mCM.setNetworkSelectionModeManual(networkSelection, response);
        }
    }

    // Inherited documentation suffices.
    public void setUnitTestMode(boolean f) {
        mUnitTestMode = f;
    }

    // Inherited documentation suffices.
    public boolean getUnitTestMode() {
        return mUnitTestMode;
    }

    /**
     * To be invoked when a voice call Connection disconnects.
     *
     * Subclasses of Phone probably want to replace this with a
     * version scoped to their packages
     */
    protected void notifyDisconnectP(Connection cn) {
        AsyncResult ar = new AsyncResult(null, cn, null);
        mDisconnectRegistrants.notifyRegistrants(ar);
    }

    // Inherited documentation suffices.
    public void registerForServiceStateChanged(
            Handler h, int what, Object obj) {
        checkCorrectThread(h);

        mServiceStateRegistrants.add(h, what, obj);
    }

    // Inherited documentation suffices.
    public void unregisterForServiceStateChanged(Handler h) {
        mServiceStateRegistrants.remove(h);
    }

    // Inherited documentation suffices.
    public void registerForRingbackTone(Handler h, int what, Object obj) {
        mCM.registerForRingbackTone(h,what,obj);
    }

    // Inherited documentation suffices.
    public void unregisterForRingbackTone(Handler h) {
        mCM.unregisterForRingbackTone(h);
    }

    // Inherited documentation suffices.
    public void registerForResendIncallMute(Handler h, int what, Object obj) {
        mCM.registerForResendIncallMute(h,what,obj);
    }

    // Inherited documentation suffices.
    public void unregisterForResendIncallMute(Handler h) {
        mCM.unregisterForResendIncallMute(h);
    }

    public void setEchoSuppressionEnabled(boolean enabled) {
        // no need for regular phone
    }

    /**
     * Subclasses of Phone probably want to replace this with a
     * version scoped to their packages
     */
    protected void notifyServiceStateChangedP(ServiceState ss) {
        AsyncResult ar = new AsyncResult(null, ss, null);
        mServiceStateRegistrants.notifyRegistrants(ar);

        mNotifier.notifyServiceState(this);
    }

    // Inherited documentation suffices.
    public SimulatedRadioControl getSimulatedRadioControl() {
        return mSimulatedRadioControl;
    }

    /**
     * Verifies the current thread is the same as the thread originally
     * used in the initialization of this instance. Throws RuntimeException
     * if not.
     *
     * @exception RuntimeException if the current thread is not
     * the thread that originally obtained this PhoneBase instance.
     */
    private void checkCorrectThread(Handler h) {
        if (h.getLooper() != mLooper) {
            throw new RuntimeException(
                    "com.android.internal.telephony.Phone must be used from within one thread");
        }
    }

    /**
     * Set the properties by matching the carrier string in
     * a string-array resource
     */
    private void setPropertiesByCarrier() {
        String carrier = SystemProperties.get("ro.carrier");

        if (null == carrier || 0 == carrier.length() || "unknown".equals(carrier)) {
            return;
        }

        CharSequence[] carrierLocales = mContext.
                getResources().getTextArray(R.array.carrier_properties);

        for (int i = 0; i < carrierLocales.length; i+=3) {
            String c = carrierLocales[i].toString();
            if (carrier.equals(c)) {
                String l = carrierLocales[i+1].toString();

                String language = l.substring(0, 2);
                String country = "";
                if (l.length() >=5) {
                    country = l.substring(3, 5);
                }
                MccTable.setSystemLocale(mContext, language, country);

                if (!country.isEmpty()) {
                    try {
                        Settings.Global.getInt(mContext.getContentResolver(),
                                Settings.Global.WIFI_COUNTRY_CODE);
                    } catch (Settings.SettingNotFoundException e) {
                        // note this is not persisting
                        WifiManager wM = (WifiManager)
                                mContext.getSystemService(Context.WIFI_SERVICE);
                        wM.setCountryCode(country, false);
                    }
                }
                return;
            }
        }
    }

    /**
     * Get state
     */
    public abstract PhoneConstants.State getState();

    /**
     * Retrieves the IccFileHandler of the Phone instance
     */
    public IccFileHandler getIccFileHandler(){
        UiccCardApplication uiccApplication = mUiccApplication.get();
        if (uiccApplication == null) return null;
        return uiccApplication.getIccFileHandler();
    }

    /*
     * Retrieves the Handler of the Phone instance
     */
    public Handler getHandler() {
        return this;
    }

    /**
    * Retrieves the ServiceStateTracker of the phone instance.
    */
    public ServiceStateTracker getServiceStateTracker() {
        return null;
    }

    /**
    * Get call tracker
    */
    public CallTracker getCallTracker() {
        return null;
    }

    public AppType getCurrentUiccAppType() {
        UiccCardApplication currentApp = mUiccApplication.get();
        if (currentApp != null) {
            return currentApp.getType();
        }
        return AppType.APPTYPE_UNKNOWN;
    }

    @Override
    public IccCard getIccCard() {
        return null;
        //throw new Exception("getIccCard Shouldn't be called from PhoneBase");
    }

    @Override
    public String getIccSerialNumber() {
        IccRecords r = mIccRecords.get();
        return (r != null) ? r.iccid : "";
    }

    @Override
    public boolean getIccRecordsLoaded() {
        IccRecords r = mIccRecords.get();
        return (r != null) ? r.getRecordsLoaded() : false;
    }

    /**
     * @return all available cell information or null if none.
     */
    @Override
    public List<CellInfo> getAllCellInfo() {
        return getServiceStateTracker().getAllCellInfo();
    }

    @Override
    public boolean getMessageWaitingIndicator() {
        IccRecords r = mIccRecords.get();
        return (r != null) ? r.getVoiceMessageWaiting() : false;
    }

    @Override
    public boolean getCallForwardingIndicator() {
        IccRecords r = mIccRecords.get();
        return (r != null) ? r.getVoiceCallForwardingFlag() : false;
    }

    /**
     *  Query the status of the CDMA roaming preference
     */
    public void queryCdmaRoamingPreference(Message response) {
        mCM.queryCdmaRoamingPreference(response);
    }

    /**
     * Get the signal strength
     */
    @Override
    public SignalStrength getSignalStrength() {
        ServiceStateTracker sst = getServiceStateTracker();
        if (sst == null) {
            return new SignalStrength();
        } else {
            return sst.getSignalStrength();
        }
    }
    

    /**
     *  Set the status of the CDMA roaming preference
     */
    public void setCdmaRoamingPreference(int cdmaRoamingType, Message response) {
        mCM.setCdmaRoamingPreference(cdmaRoamingType, response);
    }

    /**
     *  Set the status of the CDMA subscription mode
     */
    public void setCdmaSubscription(int cdmaSubscriptionType, Message response) {
        mCM.setCdmaSubscriptionSource(cdmaSubscriptionType, response);
    }

    /**
     *  Set the preferred Network Type: Global, CDMA only or GSM/UMTS only
     */
    public void setPreferredNetworkType(int networkType, Message response) {
        mCM.setPreferredNetworkType(networkType, response);
    }

    public void getPreferredNetworkType(Message response) {
        mCM.getPreferredNetworkType(response);
    }

    public void getSmscAddress(Message result) {
        mCM.getSmscAddress(result);
    }

    public void setSmscAddress(String address, Message result) {
        mCM.setSmscAddress(address, result);
    }

    public void setTTYMode(int ttyMode, Message onComplete) {
        mCM.setTTYMode(ttyMode, onComplete);
    }

    public void queryTTYMode(Message onComplete) {
        mCM.queryTTYMode(onComplete);
    }

    public void enableEnhancedVoicePrivacy(boolean enable, Message onComplete) {
        // This function should be overridden by the class CDMAPhone. Not implemented in GSMPhone.
        logUnexpectedCdmaMethodCall("enableEnhancedVoicePrivacy");
    }

    public void getEnhancedVoicePrivacy(Message onComplete) {
        // This function should be overridden by the class CDMAPhone. Not implemented in GSMPhone.
        logUnexpectedCdmaMethodCall("getEnhancedVoicePrivacy");
    }

    public void setBandMode(int bandMode, Message response) {
        mCM.setBandMode(bandMode, response);
    }

    public void queryAvailableBandMode(Message response) {
        mCM.queryAvailableBandMode(response);
    }

    public void invokeOemRilRequestRaw(byte[] data, Message response) {
        mCM.invokeOemRilRequestRaw(data, response);
    }

    public void invokeOemRilRequestStrings(String[] strings, Message response) {
        mCM.invokeOemRilRequestStrings(strings, response);
    }

    public void notifyDataActivity() {
        mNotifier.notifyDataActivity(this);
    }

    public void notifyMessageWaitingIndicator() {
        // Do not notify voice mail waiting if device doesn't support voice
        if (!mIsVoiceCapable)
            return;

        // This function is added to send the notification to DefaultPhoneNotifier.
        mNotifier.notifyMessageWaitingChanged(this);
    }

    public void notifyDataConnection(String reason, String apnType,
            PhoneConstants.DataState state) {
        mNotifier.notifyDataConnection(this, reason, apnType, state);
    }

    public void notifyDataConnection(String reason, String apnType) {
        mNotifier.notifyDataConnection(this, reason, apnType, getDataConnectionState(apnType));
    }

    public void notifyDataConnection(String reason) {
        String types[] = getActiveApnTypes();
        for (String apnType : types) {
            mNotifier.notifyDataConnection(this, reason, apnType, getDataConnectionState(apnType));
        }
    }

    public void notifyOtaspChanged(int otaspMode) {
        mNotifier.notifyOtaspChanged(this, otaspMode);
    }

    public void notifySignalStrength() {
        mNotifier.notifySignalStrength(this);
    }

    public void notifyCellInfo(List<CellInfo> cellInfo) {
        mNotifier.notifyCellInfo(this, cellInfo);
    }

    /**
     * @return true if a mobile originating emergency call is active
     */
    public boolean isInEmergencyCall() {
        return false;
    }

    /**
     * @return true if we are in the emergency call back mode. This is a period where
     * the phone should be using as little power as possible and be ready to receive an
     * incoming call from the emergency operator.
     */
    public boolean isInEcm() {
        return false;
    }

    public abstract String getPhoneName();

    public abstract int getPhoneType();

    /** @hide */
    public int getVoiceMessageCount(){
        return 0;
    }

    /**
     * Returns the CDMA ERI icon index to display
     */
    public int getCdmaEriIconIndex() {
        logUnexpectedCdmaMethodCall("getCdmaEriIconIndex");
        return -1;
    }

    /**
     * Returns the CDMA ERI icon mode,
     * 0 - ON
     * 1 - FLASHING
     */
    public int getCdmaEriIconMode() {
        logUnexpectedCdmaMethodCall("getCdmaEriIconMode");
        return -1;
    }

    /**
     * Returns the CDMA ERI text,
     */
    public String getCdmaEriText() {
        logUnexpectedCdmaMethodCall("getCdmaEriText");
        return "GSM nw, no ERI";
    }

    public String getCdmaMin() {
        // This function should be overridden by the class CDMAPhone. Not implemented in GSMPhone.
        logUnexpectedCdmaMethodCall("getCdmaMin");
        return null;
    }

    public boolean isMinInfoReady() {
        // This function should be overridden by the class CDMAPhone. Not implemented in GSMPhone.
        logUnexpectedCdmaMethodCall("isMinInfoReady");
        return false;
    }

    public String getCdmaPrlVersion(){
        //  This function should be overridden by the class CDMAPhone. Not implemented in GSMPhone.
        logUnexpectedCdmaMethodCall("getCdmaPrlVersion");
        return null;
    }

    public void sendBurstDtmf(String dtmfString, int on, int off, Message onComplete) {
        // This function should be overridden by the class CDMAPhone. Not implemented in GSMPhone.
        logUnexpectedCdmaMethodCall("sendBurstDtmf");
    }

    public void exitEmergencyCallbackMode() {
        // This function should be overridden by the class CDMAPhone. Not implemented in GSMPhone.
        logUnexpectedCdmaMethodCall("exitEmergencyCallbackMode");
    }

    public void registerForCdmaOtaStatusChange(Handler h, int what, Object obj) {
        // This function should be overridden by the class CDMAPhone. Not implemented in GSMPhone.
        logUnexpectedCdmaMethodCall("registerForCdmaOtaStatusChange");
    }

    public void unregisterForCdmaOtaStatusChange(Handler h) {
        // This function should be overridden by the class CDMAPhone. Not implemented in GSMPhone.
        logUnexpectedCdmaMethodCall("unregisterForCdmaOtaStatusChange");
    }

    public void registerForSubscriptionInfoReady(Handler h, int what, Object obj) {
        // This function should be overridden by the class CDMAPhone. Not implemented in GSMPhone.
        logUnexpectedCdmaMethodCall("registerForSubscriptionInfoReady");
    }

    public void unregisterForSubscriptionInfoReady(Handler h) {
        // This function should be overridden by the class CDMAPhone. Not implemented in GSMPhone.
        logUnexpectedCdmaMethodCall("unregisterForSubscriptionInfoReady");
    }

    /**
     * Returns true if OTA Service Provisioning needs to be performed.
     * If not overridden return false.
     */
    public boolean needsOtaServiceProvisioning() {
        return false;
    }

    /**
     * Return true if number is an OTASP number.
     * If not overridden return false.
     */
    public  boolean isOtaSpNumber(String dialStr) {
        return false;
    }

    public void registerForCallWaiting(Handler h, int what, Object obj){
        // This function should be overridden by the class CDMAPhone. Not implemented in GSMPhone.
        logUnexpectedCdmaMethodCall("registerForCallWaiting");
    }

    public void unregisterForCallWaiting(Handler h){
        // This function should be overridden by the class CDMAPhone. Not implemented in GSMPhone.
        logUnexpectedCdmaMethodCall("unregisterForCallWaiting");
    }

    public void registerForEcmTimerReset(Handler h, int what, Object obj) {
        // This function should be overridden by the class CDMAPhone. Not implemented in GSMPhone.
        logUnexpectedCdmaMethodCall("registerForEcmTimerReset");
    }

    public void unregisterForEcmTimerReset(Handler h) {
        // This function should be overridden by the class CDMAPhone. Not implemented in GSMPhone.
        logUnexpectedCdmaMethodCall("unregisterForEcmTimerReset");
    }

    public void registerForSignalInfo(Handler h, int what, Object obj) {
        mCM.registerForSignalInfo(h, what, obj);
    }

    public void unregisterForSignalInfo(Handler h) {
        mCM.unregisterForSignalInfo(h);
    }

    public void registerForDisplayInfo(Handler h, int what, Object obj) {
        mCM.registerForDisplayInfo(h, what, obj);
    }

     public void unregisterForDisplayInfo(Handler h) {
         mCM.unregisterForDisplayInfo(h);
     }

    public void registerForNumberInfo(Handler h, int what, Object obj) {
        mCM.registerForNumberInfo(h, what, obj);
    }

    public void unregisterForNumberInfo(Handler h) {
        mCM.unregisterForNumberInfo(h);
    }

    public void registerForRedirectedNumberInfo(Handler h, int what, Object obj) {
        mCM.registerForRedirectedNumberInfo(h, what, obj);
    }

    public void unregisterForRedirectedNumberInfo(Handler h) {
        mCM.unregisterForRedirectedNumberInfo(h);
    }

    public void registerForLineControlInfo(Handler h, int what, Object obj) {
        mCM.registerForLineControlInfo( h, what, obj);
    }

    public void unregisterForLineControlInfo(Handler h) {
        mCM.unregisterForLineControlInfo(h);
    }

    public void registerFoT53ClirlInfo(Handler h, int what, Object obj) {
        mCM.registerFoT53ClirlInfo(h, what, obj);
    }

    public void unregisterForT53ClirInfo(Handler h) {
        mCM.unregisterForT53ClirInfo(h);
    }

    public void registerForT53AudioControlInfo(Handler h, int what, Object obj) {
        mCM.registerForT53AudioControlInfo( h, what, obj);
    }

    public void unregisterForT53AudioControlInfo(Handler h) {
        mCM.unregisterForT53AudioControlInfo(h);
    }

     public void setOnEcbModeExitResponse(Handler h, int what, Object obj){
         // This function should be overridden by the class CDMAPhone. Not implemented in GSMPhone.
         logUnexpectedCdmaMethodCall("setOnEcbModeExitResponse");
     }

     public void unsetOnEcbModeExitResponse(Handler h){
        // This function should be overridden by the class CDMAPhone. Not implemented in GSMPhone.
         logUnexpectedCdmaMethodCall("unsetOnEcbModeExitResponse");
     }

    public String[] getActiveApnTypes() {
        return mDataConnectionTracker.getActiveApnTypes();
    }

    public String getActiveApnHost(String apnType) {
        return mDataConnectionTracker.getActiveApnString(apnType);
    }

    public LinkProperties getLinkProperties(String apnType) {
        return mDataConnectionTracker.getLinkProperties(apnType);
    }

    public LinkCapabilities getLinkCapabilities(String apnType) {
        return mDataConnectionTracker.getLinkCapabilities(apnType);
    }

    public int enableApnType(String type) {
        return mDataConnectionTracker.enableApnType(type);
    }

    public int disableApnType(String type) {
        return mDataConnectionTracker.disableApnType(type);
    }

    public boolean isDataConnectivityPossible() {
        return isDataConnectivityPossible(PhoneConstants.APN_TYPE_DEFAULT);
    }

    public boolean isDataConnectivityPossible(String apnType) {
        return ((mDataConnectionTracker != null) &&
                (mDataConnectionTracker.isDataPossible(apnType)));
    }

    /**
     * Notify registrants of a new ringing Connection.
     * Subclasses of Phone probably want to replace this with a
     * version scoped to their packages
     */
    protected void notifyNewRingingConnectionP(Connection cn) {
        if (!mIsVoiceCapable)
            return;
        AsyncResult ar = new AsyncResult(null, cn, null);
        mNewRingingConnectionRegistrants.notifyRegistrants(ar);
    }

    /**
     * Notify registrants of a RING event.
     */
    private void notifyIncomingRing() {
        if (!mIsVoiceCapable)
            return;
        AsyncResult ar = new AsyncResult(null, this, null);
        mIncomingRingRegistrants.notifyRegistrants(ar);
    }

    /**
     * Send the incoming call Ring notification if conditions are right.
     */
    private void sendIncomingCallRingNotification(int token) {
        if (mIsVoiceCapable && !mDoesRilSendMultipleCallRing &&
                (token == mCallRingContinueToken)) {
            Log.d(LOG_TAG, "Sending notifyIncomingRing");
            notifyIncomingRing();
            sendMessageDelayed(
                    obtainMessage(EVENT_CALL_RING_CONTINUE, token, 0), mCallRingDelay);
        } else {
            Log.d(LOG_TAG, "Ignoring ring notification request,"
                    + " mDoesRilSendMultipleCallRing=" + mDoesRilSendMultipleCallRing
                    + " token=" + token
                    + " mCallRingContinueToken=" + mCallRingContinueToken
                    + " mIsVoiceCapable=" + mIsVoiceCapable);
        }
    }

    public boolean isCspPlmnEnabled() {
        // This function should be overridden by the class GSMPhone.
        // Not implemented in CDMAPhone.
        logUnexpectedGsmMethodCall("isCspPlmnEnabled");
        return false;
    }

     // ALPS00302698 ENS
    public boolean isCspPlmnEnabled(int simId) {
        logUnexpectedGsmMethodCall("isCspPlmnEnabled");
        return false;
    }

    public IsimRecords getIsimRecords() {
        Log.e(LOG_TAG, "getIsimRecords() is only supported on LTE devices");
        return null;
    }

    public void requestIsimAuthentication(String nonce, Message result) {
        Log.e(LOG_TAG, "requestIsimAuthentication() is only supported on LTE devices");
    }

    public String getMsisdn() {
        logUnexpectedGsmMethodCall("getMsisdn");
        return null;
    }

    /**
     * Common error logger method for unexpected calls to CDMA-only methods.
     */
    private static void logUnexpectedCdmaMethodCall(String name)
    {
        Log.e(LOG_TAG, "Error! " + name + "() in PhoneBase should not be " +
                "called, CDMAPhone inactive.");
    }

    public PhoneConstants.DataState getDataConnectionState() {
        return getDataConnectionState(PhoneConstants.APN_TYPE_DEFAULT);
    }

    /**
     * Common error logger method for unexpected calls to GSM/WCDMA-only methods.
     */
    private static void logUnexpectedGsmMethodCall(String name) {
        Log.e(LOG_TAG, "Error! " + name + "() in PhoneBase should not be " +
                "called, GSMPhone inactive.");
    }

    // Called by SimRecords which is constructed with a PhoneBase instead of a GSMPhone.
    public void notifyCallForwardingIndicator() {
        // This function should be overridden by the class GSMPhone. Not implemented in CDMAPhone.
        Log.e(LOG_TAG, "Error! This function should never be executed, inactive CDMAPhone.");
    }

    public void notifyDataConnectionFailed(String reason, String apnType) {
        mNotifier.notifyDataConnectionFailed(this, reason, apnType);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public int getLteOnCdmaMode() {
        return mCM.getLteOnCdmaMode();
    }

    /**
     * Sets the SIM voice message waiting indicator records.
     * @param line GSM Subscriber Profile Number, one-based. Only '1' is supported
     * @param countWaiting The number of messages waiting, if known. Use
     *                     -1 to indicate that an unknown number of
     *                      messages are waiting
     */
    @Override
    public void setVoiceMessageWaiting(int line, int countWaiting) {
        IccRecords r = mIccRecords.get();
        if (r != null) {
            r.setVoiceMessageWaiting(line, countWaiting);
        }
    }

    public void dump(FileDescriptor fd, PrintWriter pw, String[] args) {
        pw.println("PhoneBase:");
        pw.println(" mCM=" + mCM);
        pw.println(" mDnsCheckDisabled=" + mDnsCheckDisabled);
        pw.println(" mDataConnectionTracker=" + mDataConnectionTracker);
        pw.println(" mDoesRilSendMultipleCallRing=" + mDoesRilSendMultipleCallRing);
        pw.println(" mCallRingContinueToken=" + mCallRingContinueToken);
        pw.println(" mCallRingDelay=" + mCallRingDelay);
        pw.println(" mIsTheCurrentActivePhone=" + mIsTheCurrentActivePhone);
        pw.println(" mIsVoiceCapable=" + mIsVoiceCapable);
        pw.println(" mIccRecords=" + mIccRecords.get());
        pw.println(" mUiccApplication=" + mUiccApplication.get());
        pw.println(" mSmsStorageMonitor=" + mSmsStorageMonitor);
        pw.println(" mSmsUsageMonitor=" + mSmsUsageMonitor);
        pw.println(" mSMS=" + mSMS);
        pw.flush();
        pw.println(" mLooper=" + mLooper);
        pw.println(" mContext=" + mContext);
        pw.println(" mNotifier=" + mNotifier);
        pw.println(" mSimulatedRadioControl=" + mSimulatedRadioControl);
        pw.println(" mUnitTestMode=" + mUnitTestMode);
        pw.println(" isDnsCheckDisabled()=" + isDnsCheckDisabled());
        pw.println(" getUnitTestMode()=" + getUnitTestMode());
        pw.println(" getState()=" + getState());
        pw.println(" getIccSerialNumber()=" + getIccSerialNumber());
        pw.println(" getIccRecordsLoaded()=" + getIccRecordsLoaded());
        pw.println(" getMessageWaitingIndicator()=" + getMessageWaitingIndicator());
        pw.println(" getCallForwardingIndicator()=" + getCallForwardingIndicator());
        pw.println(" isInEmergencyCall()=" + isInEmergencyCall());
        pw.flush();
        pw.println(" isInEcm()=" + isInEcm());
        pw.println(" getPhoneName()=" + getPhoneName());
        pw.println(" getPhoneType()=" + getPhoneType());
        pw.println(" getVoiceMessageCount()=" + getVoiceMessageCount());
        pw.println(" getActiveApnTypes()=" + getActiveApnTypes());
        pw.println(" isDataConnectivityPossible()=" + isDataConnectivityPossible());
        pw.println(" needsOtaServiceProvisioning=" + needsOtaServiceProvisioning());
    }

    //MTK-START [mtk04070][111223][ALPS00106134]Merge to ICS 4.0.3
    /**
     * Gets the USIM service table from the UICC, if present and available.
     * @return an interface to the UsimServiceTable record, or null if not available
     */
    @Override
    public UsimServiceTable getUsimServiceTable() {
        IccRecords r = mIccRecords.get();
        return (r != null) ? r.getUsimServiceTable() : null;
    }
    //MTK-END [mtk04070][111223][ALPS00106134]Merge to ICS 4.0.3


    //MTK-START [mtk04070][111117][ALPS00093395]MTK proprietary methods
    /**
     * set GPRS transfer type: data prefer/call prefer
     */
    public void setGprsTransferType(int type, Message response) {
        mCM.setGprsTransferType(type, response);
    }

    //Add by mtk80372 for Barcode Number
    /**
     * Request to get Barcode number.
     */
    public void getMobileRevisionAndIMEI(int type,Message result){
        mCM.getMobileRevisionAndIMEI(type,result);
    }

    /* Add by vendor for Multiple PDP Context */
    public void notifyDataConnection(String reason, String apnType, boolean disconnectPdpFlag){
        mNotifier.notifyDataConnection(this, reason, apnType, disconnectPdpFlag);
    }
    
    /* Add by vendor for Multiple PDP Context */
    public String getApnForType(String type){
        //Temporary mark
        return "";
        //return mDataConnectionTracker.getApnForType(type);
        //Temporary mark
    }
    
    /**
      * Notify SIP call ring event
      */
    protected void notifySipCallRing(){
       Log.d(LOG_TAG, "notify Sip Call Ring");
       mSipCallRingContinueToken = 0;
       sendMessage(obtainMessage(EVENT_SIP_CALL_RING, mSipCallRingContinueToken, 0));
    }

    // Inherited documentation suffices.
      public void registerForNeighboringInfo(Handler h, int what, Object obj) {
        mCM.registerForNeighboringInfo(h, what, obj);
      }

    // Inherited documentation suffices.
      public void unregisterForNeighboringInfo(Handler h) {
        mCM.unregisterForNeighboringInfo(h);
      }
      
    /**
     * Configure cell broadcast SMS.
     * @param chIdList
     *            Channel ID list, fill in the fromServiceId, toServiceId, and selected
     *            in the SmsBroadcastConfigInfo only
     * @param langList
     *            Channel ID list, fill in the fromCodeScheme, toCodeScheme, and selected
     *            in the SmsBroadcastConfigInfo only     
     * @param response     
     *            Callback message is empty on completion
     */
    public void setCellBroadcastSmsConfig(SmsBroadcastConfigInfo[] chIdList, 
            SmsBroadcastConfigInfo[] langList, Message response)
    {
        Log.e(LOG_TAG, "Error! This function should never be executed, inactive CDMAPhone.");
    }

    /**
     * Query if the Cell broadcast is adtivated or not
     * @param response     
     *            Callback message is empty on completion
     */
    public void queryCellBroadcastSmsActivation(Message response)
    {
        Log.e(LOG_TAG, "Error! This function should never be executed, inactive CDMAPhone.");
    }

    // Inherited documentation suffices.
    public void registerForNetworkInfo(Handler h, int what, Object obj) {
        mCM.registerForNetworkInfo(h, what, obj);
    }

    // Inherited documentation suffices.
    public void unregisterForNetworkInfo(Handler h) {
        mCM.unregisterForNetworkInfo(h);
    }

    /**
     * Refresh Spn Display due to configuration change
     */
    public void refreshSpnDisplay() {
    }

    public int getMySimId() {
        return -1;
    }

    public void registerForSpeechInfo(Handler h, int what, Object obj) {
        mCM.registerForSpeechInfo(h, what, obj);
    }

    public void unregisterForSpeechInfo(Handler h) {
        mCM.unregisterForSpeechInfo(h);
    }

    /* vt start */
    public void registerForVtStatusInfo(Handler h, int what, Object obj) {
        mCM.registerForVtStatusInfo(h, what, obj);
    }

    public void unregisterForVtStatusInfo(Handler h) {
        mCM.unregisterForVtStatusInfo(h);
    }

    public void registerForVtRingInfo(Handler h, int what, Object obj) {
        mCM.registerForVtRingInfo(h, what, obj);
    }

    public void unregisterForVtRingInfo(Handler h) {
        mCM.unregisterForVtRingInfo(h);
    }

    public void registerForVtReplaceDisconnect(Handler h, int what, Object obj) {
        mVtReplaceDisconnectRegistrants.addUnique(h, what, obj);
    }

    public void unregisterForVtReplaceDisconnect(Handler h) {
        mVtReplaceDisconnectRegistrants.remove(h);
    }
    /* vt end*/

    /**
    *send BT SAP profile
    */
    public void  sendBTSIMProfile(int nAction, int nType, String strData, Message response) {
        mCM.sendBTSIMProfile(nAction, nType, strData, response);
    }

    public void updateSimIndicateState(){
    }

    public int getSimIndicateState(){
        return PhoneConstants.SIM_INDICATOR_UNKNOWN;
    }

    public boolean isSimInsert() {
        return false;
    }

    public void doSimAuthentication (String strRand, Message result) {
    }

    public void doUSimAuthentication (String strRand, String strAutn, Message result) {
    }
//MTK-START [mtk80601][111212][ALPS00093395]IPO feature
    public void setRadioPowerOn(){
    }
//MTK-END [mtk80601][111212][ALPS00093395]IPO feature

    public void updateMobileData() {
        //Temporary mark
         //mDataConnectionTracker.updateMobileData();
        //Temporary mark
    }

    /* 3G switch start */
    public int get3GCapabilitySIM() {
        return PhoneConstants.GEMINI_SIM_1;
    }

    public boolean set3GCapabilitySIM(int simId) {
        return false;
    }  
    /* 3G switch end */

    public void getPOLCapability(Message onComplete) {
    }

    public void getPreferedOperatorList(Message onComplete) {
    }
    
    public void setPOLEntry(NetworkInfoWithAcT networkWithAct, Message onComplete) {
    }

    public String getInterfaceName(String apnType) {
        //Temporary mark
        return "";
        //return mDataConnectionTracker.getInterfaceName(apnType);
        //Temporary mark
    }

    public String getIpAddress(String apnType) {
        //Temporary mark
        return "";
        //return mDataConnectionTracker.getIpAddress(apnType);
        //Temporary mark
    }

    public String getGateway(String apnType) {
        //Temporary mark
        return "";
        //return mDataConnectionTracker.getGateway(apnType);
        //Temporary mark
    }

    public String[] getDnsServers(String apnType) {
        //Temporary mark
        return null;
        //return mDataConnectionTracker.getDnsServers(apnType);
        //Temporary mark
    }
    //MTK-END [mtk04070][111117][ALPS00093395]MTK proprietary methods

    public void hangupAllEx() throws CallStateException {
        //do nothing
    }

    //ALPS00279048
    public void setCRO(int mode, Message onComplete) {
    }

    // ALPS00294581
    public void notifySimMissingStatus(boolean isSimInsert) {
    }

     // ALPS00302702 RAT balancing
    public int getEfRatBalancing() {
        return IccRecords.EF_RAT_UNDEFINED;
    }

    public int getEfRatBalancing(int simId) {
        return IccRecords.EF_RAT_UNDEFINED;
    }

    // MVNO-API START
    public String getSpNameInEfSpn() {
        // override by child-class
        return null;
    }

    public String getSpNameInEfSpn(int simId) {
        // do nothing
        return null;
    }

    public String isOperatorMvnoForImsi() {
        // override by child-class
        return null;
    }

    public String isOperatorMvnoForImsi(int simId) {
        // do nothing
        return null;
    }

    public String getFirstFullNameInEfPnn() {
        // override by child-class
        Log.d(LOG_TAG, "getFirstFullNameInEfPnn return null");
        return null;
    }

    public String getFirstFullNameInEfPnn(int simId) {
        // do nothing
        Log.d(LOG_TAG, "getFirstFullNameInEfPnn return null");
        return null;
    }

    public String isOperatorMvnoForEfPnn() {
        // override by child-class
        Log.d(LOG_TAG, "isOperatorMvnoForEfPnn return null");
        return null;
    }

    public String isOperatorMvnoForEfPnn(int simId) {
        // do nothing
        Log.d(LOG_TAG, "isOperatorMvnoForEfPnn return null");
        return null;
    }

    public boolean isIccCardProviderAsMvno() {
        // override by child-class
        return false;
    }

    public boolean isIccCardProviderAsMvno(int simId) {
        // do nothing
        return false;
    }
    // MVNO-API END

    //[New R8 modem FD]
    public int setFDTimerValue(String newTimerValue[], Message onComplete) {
        return 0;    
    }
    
    //[New R8 modem FD] 
    public String[] getFDTimerValue() {
        return null;
    }
//#ifdef VENDOR_EDIT 
//BaoZhu.Yu@Prd.CommApp.Telephony, 2012/06/11, Add for
    /**
     * Oppo shenxiashu add for engineer mode test audio loop back.
     */
    @android.annotation.OppoHook(level=android.annotation.OppoHook.OppoHookType.NEW_METHOD,
                                 property=android.annotation.OppoHook.OppoRomType.OPPO,
                                 note="add getPhone method, only for MTK platform")
    public void enableEngineerTest(boolean on) {
    }
    
    /**
     *  oppo shenxianshu add for engineer mode test.
     */
    @android.annotation.OppoHook(level=android.annotation.OppoHook.OppoHookType.NEW_METHOD,
                                 property=android.annotation.OppoHook.OppoRomType.OPPO,
                                 note="add getPhone method, only for MTK platform")
    public void emDial(String telNumber) {
    }
    
    /**
     *  oppo shenxianshu add for engineer mode test.
     */
    @android.annotation.OppoHook(level=android.annotation.OppoHook.OppoHookType.NEW_METHOD,
                                 property=android.annotation.OppoHook.OppoRomType.OPPO,
                                 note="add getPhone method, only for MTK platform")
    public void emHungup(String telNumber) {
    }
//#endif /* VENDOR_EDIT */
}