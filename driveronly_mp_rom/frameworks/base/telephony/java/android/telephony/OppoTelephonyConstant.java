/*
 * Copyright (C) 2008 The Android Open Source Project
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

/**
 * Provides access to information about the telephony services on
 * the device. Applications can use the methods in this class to
 * determine telephony services and states, as well as to access some
 * types of subscriber information. Applications can also register
 * a listener to receive notification of telephony state changes.
 * <p>
 * You do not instantiate this class directly; instead, you retrieve
 * a reference to an instance through
 * {@link android.content.Context#getSystemService
 * Context.getSystemService(Context.TELEPHONY_SERVICE)}.
 * <p>
 * Note that access to some telephony information is
 * permission-protected. Your application cannot access the protected
 * information unless it has the appropriate permissions declared in
 * its manifest file. Where permissions apply, they are noted in the
 * the methods through which you access the protected information.
 */
@android.annotation.OppoHook(level=android.annotation.OppoHook.OppoHookType.NEW_CLASS,
                             property=android.annotation.OppoHook.OppoRomType.ROM,
                             note="constant define from OPPO and MTK")
public class OppoTelephonyConstant
{
    //MTK-START [mtk04070][111117][ALPS00093395]Used in GeminiPhone for SIM1/SIM2 color
    public static final int TOTAL_SIM_COLOR_COUNT = 4;
    //MTK-END [mtk04070][111117][ALPS00093395]Used in GeminiPhone for SIM1/SIM2 color 

    //MTK-START [mtk04070][111117][ALPS00093395]MTK added
    /* Add by mtk01411 */
    public static final int APN_REQUEST_FAILED_DUE_TO_RADIO_OFF = 98;
    public static final int APN_TYPE_NOT_AVAILABLE_DUE_TO_RECORDS_NOT_LOADED = 99;
    public static final int APN_TYPE_DISABLE_ONGOING = 100;

    //MTK-START [mtk04070][111117][ALPS00093395]MTK added
    @android.annotation.OppoHook(level=android.annotation.OppoHook.OppoHookType.NEW_FIELD,
								     property=android.annotation.OppoHook.OppoRomType.MTK,
								     note="add SIM_INDICATOR_XXX") 
     /** UNKNOWN, invalid value */
    public static final int SIM_INDICATOR_UNKNOWN = -1;
    /** ABSENT, no SIM/USIM card inserted for this phone */
    public static final int SIM_INDICATOR_ABSENT = 0;
    /** RADIOOFF,  has SIM/USIM inserted but not in use . */
    public static final int SIM_INDICATOR_RADIOOFF = 1;
    /** LOCKED,  has SIM/USIM inserted and the SIM/USIM has been locked. */
    public static final int SIM_INDICATOR_LOCKED = 2;
    /** INVALID : has SIM/USIM inserted and not be locked but failed to register to the network. */
    public static final int SIM_INDICATOR_INVALID = 3; 
    /** SEARCHING : has SIM/USIM inserted and SIM/USIM state is Ready and is searching for network. */
    public static final int SIM_INDICATOR_SEARCHING = 4; 
    /** NORMAL = has SIM/USIM inserted and in normal service(not roaming and has no data connection). */
    public static final int SIM_INDICATOR_NORMAL = 5; 
    /** ROAMING : has SIM/USIM inserted and in roaming service(has no data connection). */	
    public static final int SIM_INDICATOR_ROAMING = 6; 
    /** CONNECTED : has SIM/USIM inserted and in normal service(not roaming) and data connected. */
    public static final int SIM_INDICATOR_CONNECTED = 7; 
    /** ROAMINGCONNECTED = has SIM/USIM inserted and in roaming service(not roaming) and data connected.*/
    public static final int SIM_INDICATOR_ROAMINGCONNECTED = 8;
    //MTK-END [mtk04070][111117][ALPS00093395]MTK added

    /**
     * SIM ID for GEMINI
     */
    public static final int GEMINI_SIM_1 = 0;
    public static final int GEMINI_SIM_2 = 1;
    public static final int GEMINI_SIM_3 = 2;
    public static final int GEMINI_SIM_4 = 3;
    public static final int GEMINI_SIP_CALL = -1; 
    public static final String GEMINI_SIM_ID_KEY = "simId";
    public static final String MULTI_SIM_ID_KEY = "simid";
    public static final String GEMINI_DEFAULT_SIM_PROP = "persist.radio.default_sim";
    public static final String GEMINI_SIM_NUM_PROP = "persist.gemini.sim_num";
	
    //MTK-START [mtk04070][111117][ALPS00093395]MTK added
    /* Add by vendor: Multiple PDP Context for MobileDataStateTracker usage */ 
    public static final String DISCONNECT_DATA_FLAG ="disconnectPdpFlag";
    /* vt start */
    public static final String IS_VT_CALL = "isVtCall";
    /* vt end */
    //MTK-END [mtk04070][111117][ALPS00093395]MTK added

    //MTK-START [mtk04070][111117][ALPS00093395]MTK added
    public static final String APN_TYPE_DM = "dm";
    public static final String APN_TYPE_WAP = "wap";
    public static final String APN_TYPE_NET = "net";
    public static final String APN_TYPE_CMMAIL = "cmmail";
    public static final String APN_TYPE_TETHERING = "tethering";
    public static final String APN_TYPE_RCSE = "rcse";
    //MTK-END [mtk04070][111117][ALPS00093395]MTK added

    //MTK-START [mtk04070][111117][ALPS00093395]MTK added
    public static final String FEATURE_ENABLE_DM = "enableDM";
    public static final String FEATURE_ENABLE_WAP = "enableWAP";
    public static final String FEATURE_ENABLE_NET = "enableNET";
    public static final String FEATURE_ENABLE_CMMAIL = "enableCMMAIL";
    //MTK-END [mtk04070][111117][ALPS00093395]MTK added

    //MTK-START [mtk04070][111117][ALPS00093395]MTK added
    public static final String REASON_GPRS_ATTACHED_TIMEOUT = "gprsAttachedTimeout";
    /* Add by mtk01411 */
    public static final String REASON_ON_RADIO_AVAILABLE = "onRadioAvailable";
    public static final String REASON_ON_RECORDS_LOADED = "onRecordsLoaded";
    public static final String REASON_POLL_STATE_DONE = "pollStateDone";
    public static final String REASON_NO_SUCH_PDP = "noSuchPdp";
    public static final String REASON_PDP_NOT_ACTIVE = "pdpNotActive";
    //MTK-END [mtk04070][111117][ALPS00093395]MTK added

    //MTK-START [mtk04070][111118][ALPS00093395]MTK added
    public static final String PROPERTY_OPERATOR_ALPHA_2 = "gsm.operator.alpha.2";    
    public static final String PROPERTY_OPERATOR_NUMERIC_2 = "gsm.operator.numeric.2";    
    public static final String PROPERTY_OPERATOR_ISMANUAL_2 = "operator.ismanual.2";    
    public static final String PROPERTY_OPERATOR_ISROAMING_2 = "gsm.operator.isroaming.2";    
    public static final String PROPERTY_OPERATOR_ISO_COUNTRY_2 = "gsm.operator.iso-country.2";    
    public static String PROPERTY_SIM_STATE_2 = "gsm.sim.state.2";
    public static String PROPERTY_ICC_OPERATOR_NUMERIC_2 = "gsm.sim.operator.numeric.2";
    public static String PROPERTY_ICC_OPERATOR_ALPHA_2 = "gsm.sim.operator.alpha.2";
    public static String PROPERTY_ICC_OPERATOR_ISO_COUNTRY_2 = "gsm.sim.operator.iso-country.2";

    /** PROPERTY_ICC_OPERATOR_DEFAULT_NAME is the operator name for plmn which origins the SIM.
     *  Availablity: SIM state must be "READY"
     */
    public static String PROPERTY_ICC_OPERATOR_DEFAULT_NAME = "gsm.sim.operator.default-name";
    public static String PROPERTY_ICC_OPERATOR_DEFAULT_NAME_2 = "gsm.sim.operator.default-name.2";
    public static String PROPERTY_DATA_NETWORK_TYPE_2 = "gsm.network.type.2";    
    
    /**
     * Indicate how many SIM cards are inserted
     */
    public static final String PROPERTY_GSM_SIM_INSERTED = "gsm.sim.inserted";

    /**
    * Indicate CS network type
    */
    public static final String PROPERTY_CS_NETWORK_TYPE = "gsm.cs.network.type";
	
    /**
    * Indicate CS network type
    */
    public static final String PROPERTY_CS_NETWORK_TYPE_2 = "gsm.cs.network.type.2";	

    /**
    * Indicate whether the SIM info has been updated
    */
    public static final String PROPERTY_SIM_INFO_READY = "gsm.siminfo.ready";
	
    /**
    * Indicate if Roaming Indicator needed for SIM/USIM in slot1
    */
    public static final String PROPERTY_ROAMING_INDICATOR_NEEDED = "gsm.roaming.indicator.needed";
	
    /**
    * Indicate if Roaming Indicator needed for SIM/USIM in slot2
    */
    public static final String PROPERTY_ROAMING_INDICATOR_NEEDED_2 = "gsm.roaming.indicator.needed.2";

   
    public static final String ACTION_DOWNLOAD_CALIBRATION_DATA
    	= "android.intent.action.DOWNLOAD_CALIBRATION_DATA";
    	
    public static final String EXTRA_CALIBRATION_DATA = "calibrationData";	
    
    public static final String ACTION_SIM_INSERTED_STATUS
        = "android.intent.action.SIM_INSERTED_STATUS";
        
    public static final String ACTION_DEFAULT_SIM_REMOVED
			= "android.intent.action.DEFAULT_SIM_REMOVED";    
			 
    public static final String ACTION_SIM_STATE_CHANGED
            = "android.intent.action.SIM_STATE_CHANGED";
    
    public static final String ACTION_NEW_SIM_DETECTED
			= "android.intent.action.NEW_SIM_DETECTED";
		
		public static final String ACTION_SIM_INFO_UPDATE
    	= "android.intent.action.SIM_INFO_UPDATE";     
    	
    public static final String ACTION_RADIO_OFF
         = "android.intent.action.RADIO_OFF";	  

    public static final String INTENT_VALUE_ICC_ABSENT_REASON    = "PLUGOUT";
    public static final String INTENT_VALUE_ICC_NOT_READY_REASON = "PLUGIN";

    public static final String ACTION_EMT_CALL_STATE_CHANGED
            = "android.intent.action.ACTION_EMT_CALL_STATE_CHANGED";
    public static final String EXTRA_EMT_CALL_STATE = "state";
    public static final String EXTRA_EMT_CALL_NUMBER = "number";             


    public static final int OPPO_SIMPHONEBOOK_STATE_ERROR = -1;    
    public static final int OPPO_SIMPHONEBOOK_STATE_NOT_READY = 0;    
    public static final int OPPO_SIMPHONEBOOK_STATE_READY = 1;	
    public static final int SIM_STATE_SWICTHCARD = 99;    
    public static final int SIM_STATE_SIMREFRESH = 100;    
    public static final int EMT_CALL_STATE_ERROR = -1;    
    public static final int EMT_CALL_STATE_IDLE = 0;    
    public static final int EMT_CALL_STATE_DIALING = 1;    
    public static final int EMT_CALL_STATE_ACTIVE = 2;    
    public static final int EMT_CALL_STATE_HOLDING = 3;    
    public static final int EMT_CALL_STATE_UNKNOWN = 4;    
    public static final int EMT_CALL_STATE_ALERTING = 5;    
    public static final int mOppoMatchLen = -1;
    public static final int OPPO_MAX_PB_NUMBER_LENGTH = 20;
    public static final int OPPO_MAX_PB_NAME_LENGTH = 14;
    public static final int OPPO_MAX_PB_EMAIL_LENGTH = 30;

    public static final int PORT_WAP_VCARD = 9204;

    public static final int PORT_WAP_VCAL_S  = 9207;
    public static final int OPPO_EVENT_ADN_SIZE_DONE = 990;

    public static final int EF_CC = 0X4F23;

    public static final int OPPO_EVENT_GET_ADN_FIELD_INFO_DONE = 102;

    public static final int OPPO_EVENT_UPDATE_ADN_DONE = 90;
    public static final int OPPO_EVENT_UPDATE_PBC_DONE = 91;
    public static final int OPPO_EVENT_READ_PBC_DONE = 92;
    public static final int OPPO_EVENT_READ_CC_DONE = 93;
    public static final int OPPO_EVENT_GET_PBC_SIZE_DONE = 94;

    public static final boolean CTA_ENABLE = false;	

    public static final int MAXNUM_RETRY_SIMRECORD = 3;

    public static final int EVENT_POLL_IS_PHB_READY = 303;    
    public static final int PHB_POLL_TIMEOUT = 240000;
    public static final int PHB_START_POLL_TIME1 = 120000;
    public static final int PHB_START_POLL_TIME2 = 240000;
    public static final int POLL_MAX_TIME = 10;

    //#ifdef VENDOR_EDIT 
//BaoZhu.Yu@Prd.CommApp.Telephony, 2012/06/11, Add for engineering mode test.
    public static final int EVENT_EM_DIAL_DONE = 301;
    public static final int EVENT_EM_GET_CURRENT_CALLS_DONE = 302;
    public static final int EVENT_EM_HANGUP_DONE = 303;    
    public static final int EVENT_SEND_UPLINK_DATA_DONE = 304;
    public static final int EVENT_EM_CALL_STATUS_CHANGED = 305;

	//BaoZhu.Yu@Prd.CommApp.Telephony, 2012/06/11, Add for if get PCB failed, we can read it loop
	public static final int EVENT_GET_PCB_DONE = 306;
//#endif /* VENDOR_EDIT */

	//OPPO lixuan add begin for black list at 2012-09-06
	/* oppo 2011-11-05 zengzhigang add begin for black list */
 	public static final String ACTION_NEW_INCOMING_CALL = "android.intent.action.NEW_INCOMING_CALL";
 	public static final String EXTRA_INCOMING_NUMBER = "PHONE_NUMBER";
	/* oppo 2011-11-05 zengzhigang add end */

    public static final int RESULT_PASS   = 0;
    public static final int RESULT_BLOCK  = 1;

}
