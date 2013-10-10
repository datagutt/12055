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

package com.android.internal.policy.impl;

import android.content.Context;
import android.util.Log;
import android.view.FallbackEventHandler;
import android.view.LayoutInflater;
import android.view.Window;
import android.view.WindowManagerPolicy;

import com.android.internal.policy.IPolicy;
import com.android.internal.policy.impl.PhoneLayoutInflater;
import com.android.internal.policy.impl.PhoneWindow;
import com.android.internal.policy.impl.PhoneWindowManager;

/**
 * {@hide}
 */

// Simple implementation of the policy interface that spawns the right
// set of objects
public class Policy implements IPolicy {
    private static final String TAG = "PhonePolicy";

    @android.annotation.OppoHook(
            level = android.annotation.OppoHook.OppoHookType.CHANGE_CODE,
            property = android.annotation.OppoHook.OppoRomType.ROM,
            note = "Jianhui.Yu@Plf.Framework.SDK : Modify for oppo PhoneWindow policy")
    private static final String[] preload_classes = {
        "com.android.internal.policy.impl.PhoneLayoutInflater",
        "com.android.internal.policy.impl.PhoneWindow",
        "com.android.internal.policy.impl.PhoneWindow$1",
        "com.android.internal.policy.impl.PhoneWindow$DialogMenuCallback",
        "com.android.internal.policy.impl.PhoneWindow$DecorView",
        "com.android.internal.policy.impl.PhoneWindow$PanelFeatureState",
        "com.android.internal.policy.impl.PhoneWindow$PanelFeatureState$SavedState",
        //#ifdef VENDOR_EDIT
        //#@OppoHook
        //Jianhui.Yu@Plf.Framework.SDK, 2013-09-05 : Add for oppo PhoneWindow policy
        "com.android.internal.policy.impl.OppoPhoneWindow",
        //#endif /* VENDOR_EDIT */
    };

    static {
        // For performance reasons, preload some policy specific classes when
        // the policy gets loaded.
        for (String s : preload_classes) {
            try {
                Class.forName(s);
            } catch (ClassNotFoundException ex) {
                Log.e(TAG, "Could not preload class for phone policy: " + s);
            }
        }
    }

    @android.annotation.OppoHook(
            level = android.annotation.OppoHook.OppoHookType.CHANGE_CODE,
            property = android.annotation.OppoHook.OppoRomType.ROM,
            note = "Jianhui.Yu@Plf.Framework.SDK : Modify for oppo PhoneWindow policy")
    public Window makeNewWindow(Context context) {
        //#ifndef VENDOR_EDIT
        //#@OppoHook
        //Jianhui.Yu@Plf.Framework.SDK, 2013-09-05 : Modify for oppo PhoneWindow policy
        /*
        return new PhoneWindow(context);
        */
        //#else /* VENDOR_EDIT */
        return OppoPhoneWindow.newInstance(context);
        //#endif /* VENDOR_EDIT */
    }

    public LayoutInflater makeNewLayoutInflater(Context context) {
        return new PhoneLayoutInflater(context);
    }

    @android.annotation.OppoHook(
            level = android.annotation.OppoHook.OppoHookType.CHANGE_CODE,
            property = android.annotation.OppoHook.OppoRomType.ROM,
            note = "ZhangJun@Plf.Framework.Input : Modify for OPPO input policy")
    public WindowManagerPolicy makeNewWindowManager() {
        //#ifndef VENDOR_EDIT
        //#@OppoHook
        //ZhangJun@Plf.Framework.Input, 2012-12-14 : Modify for use oppo key policy
        /*
        return new PhoneWindowManager();
        */
        //#else  /* VENDOR_EDIT */
        return OppoClassFactory.createPhoneWindowManager();
        //#endif /* VENDOR_EDIT */
    }

    public FallbackEventHandler makeNewFallbackEventHandler(Context context) {
        return new PhoneFallbackEventHandler(context);
    }
}
