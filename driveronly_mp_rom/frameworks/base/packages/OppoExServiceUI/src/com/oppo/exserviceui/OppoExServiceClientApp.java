/************************************************************************************
 ** File: - OppoExServiceClient.java
 ** VENDOR_EDIT
 ** Copyright (C), 2008-2013, OPPO Mobile Comm Corp., Ltd
 ** 
 ** Description: 
 **      Add this file for screen capture!
 ** 
 ** Version: 0.1
 ** Date created: 2013-08-03
 ** Author: Jun.Zhang@Plf.Framework
 ** 
 ** --------------------------- Revision History: --------------------------------
 ** 	<author>	                    		<data>					<desc>
 ** 	Jun.Zhang@Plf.Framework 				2013-08-03      		Init
 ************************************************************************************/

package com.oppo.exserviceui;
import android.app.Application;
import android.content.Intent;
import android.util.Slog;

public class OppoExServiceClientApp extends Application {
    private static final String TAG = "OppoExServiceClientApp";
    private OppoExServiceClient mServiceClient = null;
	
	@Override
    public void onCreate() {
		try {
			Intent exServiceIntent = new Intent(this, com.oppo.exserviceui.OppoExServiceClient.class);
			startService(exServiceIntent);
		} catch (Exception e) {
            Slog.e(TAG, "Can't start oppo expand service", e);
        }
	}
}

