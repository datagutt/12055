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

package android.net;

@android.annotation.OppoHook(level=android.annotation.OppoHook.OppoHookType.NEW_CLASS,
                             property=android.annotation.OppoHook.OppoRomType.ROM,
                             note="sub class of ConnectivityManager")
public class OppoConnectivityManager extends ConnectivityManager {
    /** @hide */
    public OppoConnectivityManager(IConnectivityManager service) {
        super(service);
    }

    public String[] oppoGetTetherableUsbRegexs() {
        return getTetherableUsbRegexs();
    }

    public String[] oppoGetTetherableWifiRegexs() {
        return getTetherableWifiRegexs();
    }
	
    public String[] oppoGetTetherableBluetoothRegexs() {        return getTetherableBluetoothRegexs();
    }

    public boolean oppoGetMobileDataEnabled() {
        return getMobileDataEnabled();
    }

    public boolean oppoIsNetworkSupported(int networkType) {
        return isNetworkSupported(networkType);
    }
    
    @android.annotation.OppoHook(level=android.annotation.OppoHook.OppoHookType.NEW_METHOD,
                                 property=android.annotation.OppoHook.OppoRomType.OPPO,
                                 note="get mobile data enabled state, only for MTK platform")
    public boolean oppoGetMobileDataEnabledGemini(int simId) {
        return getMobileDataEnabledGemini(simId);
    }

    @android.annotation.OppoHook(level=android.annotation.OppoHook.OppoHookType.NEW_METHOD,
                                 property=android.annotation.OppoHook.OppoRomType.OPPO,
                                 note="set mobile data enabled, only for MTK platform")
    public boolean oppoSetMobileDataEnabledGemini(int slotId) {
        return setMobileDataEnabledGemini(slotId);
    }

    @android.annotation.OppoHook(level=android.annotation.OppoHook.OppoHookType.NEW_METHOD,
                                 property=android.annotation.OppoHook.OppoRomType.OPPO,
                                 note="start using network feature, only for MTK platform")
    public int oppoStartUsingNetworkFeatureGemini(int networkType, String feature, int radioNum) {
        return startUsingNetworkFeatureGemini(networkType, feature, radioNum);
    }

    @android.annotation.OppoHook(level=android.annotation.OppoHook.OppoHookType.NEW_METHOD,
                                 property=android.annotation.OppoHook.OppoRomType.OPPO,
                                 note="stop using network feature, only for MTK platform")
    public int oppoStopUsingNetworkFeatureGemini(int networkType, String feature, int radioNum) {
        return stopUsingNetworkFeatureGemini(networkType, feature, radioNum);
    }
}