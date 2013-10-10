/**
* Copyright 2008-2010 OPPO Mobile Comm Corp., Ltd, All rights reserved.
* FileName: PermissionSelectionActivity
* Author:zyx    
* Create Date:2010-12-13
* Description:
   
* History:
<version >  <time>  <author>  <desc>
*/

//package com.oppo.internal.permission;
package com.oppo.server;

import com.android.internal.app.AlertActivity;
import com.android.internal.app.AlertController;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.res.Resources;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;
import android.widget.ListView;
import android.widget.TextView;
import android.app.WallpaperManager;
import android.app.WallpaperInfo;
import android.os.Handler;
import android.os.Message;
import android.view.WindowManager;
import android.content.DialogInterface;
import android.app.KeyguardManager;
import android.app.KeyguardManager.KeyguardLock;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener; 
import android.provider.Settings;
import android.os.storage.StorageManager;
import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.os.SystemProperties;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
//import com.oppo.internal.R;

import java.io.IOException;
import java.util.List;
import java.util.ArrayList;
//#ifdef VENDOR_EDIT
//Bingshu.Zhou@Prd.SysSrv.PMS, 2013/04/15, Add for permission control
import android.app.ActivityManager;    
//#endif /* VENDOR_EDIT */


public class PermissionSelectionActivity extends AlertActivity implements
        DialogInterface.OnClickListener{
    private static final String TAG = "PermissionSelectionActivity";
    private Context mContext = null;
    private Resources mResources = null;
    private String mPackageName = null;
    static final String MODIFY_PERMISSION_PACKAGE_NAME = "MODIFY_PERMISSION_PACKAGE_NAME";
    static final String APP_PKG_NAME = "pkg";
    static final String MODIFY_ACTION = "android.intent.action.OPPO_MODIFY_PKG_PERMISSIONS";
    private PackageManager mPm;
    private ApplicationInfo mAppInfo;
    //#ifdef VENDOR_EDIT
    //Bingshu.Zhou@Prd.SysSrv.PMS, 2013/04/15, Add for permission control
    private ActivityManager am; 
    //#endif /* VENDOR_EDIT */
    
    
    private boolean initAppInfo(String packageName) {
        try {
            mAppInfo = mPm.getApplicationInfo(packageName,
                    PackageManager.GET_UNINSTALLED_PACKAGES);
            return true;
        } catch (NameNotFoundException e) {
            Log.e(TAG, "Exception when retrieving package: " + packageName, e);
            return false;
        }
    }
    
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        mContext = this;
        mResources = mContext.getResources();
        mPm = getPackageManager();
        
        mPackageName = getIntent().getStringExtra(MODIFY_PERMISSION_PACKAGE_NAME);
        if (! initAppInfo(mPackageName)) {
            return; // could not find package, finish called
        }

        // Set up the "dialog"
        //#ifdef VENDOR_EDIT
        //Bingshu.Zhou@Prd.SysSrv.USB, 2013/04/15, Add for permission control
        am = (ActivityManager)mContext.getSystemService(Context.ACTIVITY_SERVICE);  
        //#endif /* VENDOR_EDIT */
        
        final AlertController.AlertParams p = mAlertParams;
        p.mIconId = android.R.drawable.ic_dialog_info;
        p.mTitle = mResources.getString(com.android.internal.R.string.modify_pkg_perms);
        p.mMessage = mResources.getString(com.android.internal.R.string.modify_pkg_perms_detail);
        
        p.mPositiveButtonText = getString(com.android.internal.R.string.ok);
        p.mPositiveButtonListener = this;
        
        p.mNegativeButtonText = getString(com.android.internal.R.string.cancel);
        p.mNegativeButtonListener = this;
        
        setupAlert();
    }

    
    public void onClick(DialogInterface dialog, int which) {
        switch (which) {
            case DialogInterface.BUTTON_POSITIVE:
            {
                Log.i(TAG, "BUTTON_POSITIVE");
                SystemProperties.set("oppo.permissions.disable", "false");
                Intent modifyIntent = new Intent(MODIFY_ACTION);
                /*modifyIntent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK |
                        Intent.FLAG_ACTIVITY_RESET_TASK_IF_NEEDED);*/
                modifyIntent.putExtra(APP_PKG_NAME, mPackageName);  
                mContext.startActivity(modifyIntent);
            }
            break;
            case DialogInterface.BUTTON_NEGATIVE:
            {
                //#ifdef VENDOR_EDIT
                //Bingshu.Zhou@Prd.SysSrv.PMS, 2013/04/15, Add for permission control
                if(am != null){
                    Log.i(TAG, "BUTTON_NEGATIVE :forceStopPackage "+mPackageName);
                    am.forceStopPackage(mPackageName);
                }
                //#endif /* VENDOR_EDIT */

            }
            break;
        }
    }
    
    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        return super.onKeyDown(keyCode, event);
    }
}
