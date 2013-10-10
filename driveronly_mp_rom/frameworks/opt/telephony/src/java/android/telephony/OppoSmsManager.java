/*
 * Copyright (C) 2008 The Android Open Source Project
 * Copyright (c) 2012, The Linux Foundation. All rights reserved.
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

import android.app.PendingIntent;
import android.app.PendingIntent.CanceledException;
import android.content.Context;
import android.os.Bundle;
import android.os.RemoteException;
import android.os.ServiceManager;
import android.os.SystemProperties;
import android.text.TextUtils;

import com.android.internal.telephony.ISms;
import com.android.internal.telephony.IccConstants;
import com.android.internal.telephony.SmsRawData;
import com.android.internal.telephony.ITelephony;
import com.android.internal.telephony.EncodeException;
import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneConstants;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import android.util.Log;
import android.telephony.gemini.GeminiSmsManager;
import android.telephony.gemini.GeminiSmsMessage;
import android.telephony.SimSmsInsertStatus;
import android.telephony.SmsParameters;
import com.mediatek.common.telephony.IccSmsStorageStatus;
/// M: added for MTK_ONLY_OWNER_SIM_SUPPORT @{
import android.os.UserHandle;
import com.mediatek.common.featureoption.FeatureOption;
/// @}
/*
 * TODO(code review): Curious question... Why are a lot of these
 * methods not declared as static, since they do not seem to require
 * any local object state?  Presumably this cannot be changed without
 * interfering with the API...
 */

/**
 * Manages SMS operations such as sending data, text, and pdu SMS messages.
 * Get this object by calling the static method SmsManager.getDefault().
 */
public final class OppoSmsManager {
    /** Singleton object constructed during class initialization. */
    private static final OppoSmsManager sInstance = new OppoSmsManager();

    /** Generic failure cause */
    static public final int RESULT_ERROR_GENERIC_FAILURE    = 1;

    /**
     * Get the default instance of the SmsManager
     *
     * @return the default instance of the SmsManager
     */
    public static OppoSmsManager getDefault() {
        return sInstance;
    }

    private OppoSmsManager() {
        //nothing
    }


//#ifdef VENDOR_EDIT
    public static SmsMemoryStatus oppoGetSmsSimMemoryStatusGemini(int simId) {
		//IccSmsStorageStatus mIccSmsStorageStatus = GeminiSmsManager.getSmsSimMemoryStatusGemini(simId);
		//SmsMemoryStatus mSmsMemoryStatus = new SmsMemoryStatus(mIccSmsStorageStatus.mUsed,mIccSmsStorageStatus.mTotal);
		//return mSmsMemoryStatus;
        return GeminiSmsManager.getSmsSimMemoryStatusGemini(simId);
      //  return null;
	}
//#endif /* VENDOR_EDIT */
}
