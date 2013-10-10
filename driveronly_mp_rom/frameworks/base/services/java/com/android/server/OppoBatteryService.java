/************************************************************************************
** Copyright (C), 2000-2013, OPPO Mobile Comm Corp., Ltd
** All rights reserved.
** 
** VENDOR_EDIT
** 
** Description: - 
**      OPPO Battery Service
** 
** 
** --------------------------- Revision History: --------------------------------
** <author>		                      <data> 	<version >  <desc>
** ------------------------------------------------------------------------------
** yaolang@Plf.Framework  2013/07/30   1.0	    create file
** ------------------------------------------------------------------------------
** 
************************************************************************************/

package com.android.server;


import android.app.ActivityManagerNative;
import android.content.Context;
import android.content.Intent;
import android.os.Handler;
import android.os.OppoBatteryManager;
import android.os.SystemProperties;
import android.os.UserHandle;
import android.util.Slog;
import java.io.File;
import java.io.FileDescriptor;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.PrintWriter;

@android.annotation.OppoHook(level=android.annotation.OppoHook.OppoHookType.NEW_CLASS,
                            property=android.annotation.OppoHook.OppoRomType.ROM,
                            note="yaolang@Plf.Framework, add for oppo battery service")
public final class OppoBatteryService {
    private static final String TAG = OppoBatteryService.class.getSimpleName();

    private static final boolean DEBUG = true;//false;

    private int mChargerVoltage;
    private int mBatteryCurrent;
    private int mChargeVolStatus;
    private int mBatteryTempStatus;
    private int mBatteryVolStatus;
    private int mBatteryVoltage;
    private int mBatteryTemperature;
    private int mAccessoryStatus;
    private int mAccessoryCapacity;
    private boolean mAccessoryPresent;
    private boolean mChargeTimeout;
    private boolean mChargeSocFall;

    private int mLastChargerVoltage;
    private int mLastBatteryCurrent;
    private int mLastChargeVolStatus;
    private int mLastBatteryTempStatus;
    private int mLastBatteryVolStatus;
    private int mLastBatteryVoltage;
    private int mLastBatteryTemperature;
    private int mLastAccessoryStatus;
    private int mLastAccessoryCapacity;
    private boolean mLastAccessoryPresent;
    private boolean mLastChargeTimeout;
    private boolean mLastChargeSocFall;

    private final int mLowBatteryVoltageWarningLevel;
    private final int mHighBatteryVoltageWarningLevel;
    private final int mLowChargerVoltageWarningLevel;
    private final int mHighChargerVoltageWarningLevel;
    private final int mTomorrowLowBatteryWarningLevel;
    private int mPlugType;
    
    // This should probably be exposed in the API, though it's not critical
    private static final int BATTERY_PLUGGED_NONE = 0;

    private final Context mContext;
    private final Handler mHandler;

    private boolean mUpdatesStopped = false;

    public OppoBatteryService(Context context) {
        mContext = context;
        mHandler = new Handler(true /*async*/);

        mLowBatteryVoltageWarningLevel = mContext.getResources().getInteger(
                com.oppo.internal.R.integer.oppo_config_lowBatteryVoltageWarningLevel);
        mHighBatteryVoltageWarningLevel = mContext.getResources().getInteger(
                com.oppo.internal.R.integer.oppo_config_highBatteryVoltageWarningLevel);
        mLowChargerVoltageWarningLevel = mContext.getResources().getInteger(
                com.oppo.internal.R.integer.oppo_config_lowChargerVoltageWarningLevel);
        mHighChargerVoltageWarningLevel = mContext.getResources().getInteger(
                com.oppo.internal.R.integer.oppo_config_highChargerVoltageWarningLevel);
        mTomorrowLowBatteryWarningLevel = mContext.getResources().getInteger(
                com.oppo.internal.R.integer.oppo_config_tomorrowLowBatteryWarningLevel);
    }

    public void processAdditionalValuesLocked(final int batteryVoltage, final int batteryTemperature, final int plugType) {
        if (!mUpdatesStopped) {
            mBatteryVoltage = batteryVoltage;
            mBatteryTemperature = batteryTemperature;
            mPlugType = plugType;
        }

        if (DEBUG) {
            Slog.d(TAG, "Processing additional values: "
                    + "mChargerVoltage=" + mChargerVoltage
                    + ", mLastChargerVoltage=" + mLastChargerVoltage
                    + ", mBatteryCurrent=" + mBatteryCurrent
                    + ", mLastBatteryCurrent=" + mLastBatteryCurrent
                    + ", mChargeVolStatus=" + mChargeVolStatus
                    + ", mBatteryTemperature=" + mBatteryTemperature
                    + ", mBatteryTempStatus=" + mBatteryTempStatus
                    + ", mAccessoryPresent=" + mAccessoryPresent
                    + ", mAccessoryStatus=" + mAccessoryStatus
                    + ", mAccessoryCapacity=" + mAccessoryCapacity
                    + ", mChargeTimeout=" + mChargeTimeout
                    + ", mChargeSocFall=" + mChargeSocFall);
        }
        
        if (mChargerVoltage != mLastChargerVoltage ||
               mBatteryCurrent != mLastBatteryCurrent ||
               mBatteryVoltage != mLastBatteryVoltage ||
               mChargeVolStatus != mLastChargeVolStatus ||
               mBatteryTemperature != mLastBatteryTemperature ||
               mBatteryTempStatus != mLastBatteryTempStatus ||
               mBatteryVolStatus != mLastBatteryVolStatus ||
               mAccessoryPresent != mLastAccessoryPresent ||
               mAccessoryStatus != mLastAccessoryStatus ||
               mAccessoryCapacity != mLastAccessoryCapacity ||
               mChargeTimeout != mLastChargeTimeout ||
               mChargeSocFall != mLastChargeSocFall) {
               
            if (mBatteryTemperature < OppoBatteryManager.TEMPERATURE_THRESHOLD_LOW) {
                mBatteryTempStatus = OppoBatteryManager.TEMPERATURE_STATUS_LOW;
            } else if (mBatteryTemperature > OppoBatteryManager.TEMPERATURE_THRESHOLD_HIGH) {
                mBatteryTempStatus = OppoBatteryManager.TEMPERATURE_STATUS_HIGH;
            } else {
                mBatteryTempStatus = OppoBatteryManager.TEMPERATURE_STATUS_NORMAL;
            }

            if (mBatteryVoltage >= mHighBatteryVoltageWarningLevel) {
                mBatteryVolStatus = OppoBatteryManager.BATTERY_VOL_HIGH;
            } else if (mBatteryVoltage <= mLowBatteryVoltageWarningLevel) {
                mBatteryVolStatus = OppoBatteryManager.BATTERY_VOL_LOW;
            } else {
                mBatteryVolStatus = OppoBatteryManager.BATTERY_VOL_NORMAL;
            }

            if (mPlugType != BATTERY_PLUGGED_NONE &&
                    mChargerVoltage >= mHighChargerVoltageWarningLevel) {
                mChargeVolStatus = OppoBatteryManager.CHARGER_VOL_HIGH;
            } else if (mPlugType != BATTERY_PLUGGED_NONE &&
                       mChargerVoltage <= mLowChargerVoltageWarningLevel) {
                mChargeVolStatus = OppoBatteryManager.CHARGER_VOL_LOW;
            } else {
                mChargeVolStatus = OppoBatteryManager.CHARGER_VOL_NORMAL;
            }

            mLastChargerVoltage = mChargerVoltage;
            mLastBatteryCurrent = mBatteryCurrent;
            mLastBatteryVoltage = mBatteryVoltage;
            mLastChargeVolStatus = mChargeVolStatus;
            mLastBatteryTempStatus = mBatteryTempStatus;
            mLastBatteryVolStatus = mBatteryVolStatus;
            mLastAccessoryPresent = mAccessoryPresent;
            mLastAccessoryStatus = mAccessoryStatus;
            mLastAccessoryCapacity = mAccessoryCapacity;
            mLastChargeTimeout = mChargeTimeout;
            mLastChargeSocFall = mChargeSocFall;

            sendAdditionalIntentLocked();
        }
    }

    private void sendAdditionalIntentLocked() {
        //  Pack up the values and broadcast them to everyone
        final Intent intent = new Intent(OppoBatteryManager.ACTION_ADDITIONAL_BATTERY_CHANGED);
        intent.addFlags(Intent.FLAG_RECEIVER_REGISTERED_ONLY
                | Intent.FLAG_RECEIVER_REPLACE_PENDING);

	 	intent.putExtra(OppoBatteryManager.EXTRA_CHARGER_VOLTAGE, mChargerVoltage);
        intent.putExtra(OppoBatteryManager.EXTRA_BATTERY_CURRENT, mBatteryCurrent);
        intent.putExtra(OppoBatteryManager.EXTRA_CHARGER_VOL_STATUS, mChargeVolStatus);
        
        intent.putExtra(OppoBatteryManager.EXTRA_BATTERY_TEMP_STATUS, mBatteryTempStatus);
        intent.putExtra(OppoBatteryManager.EXTRA_BATTERY_VOL_STATUS, mBatteryVolStatus);

        // For wireless charger set
        intent.putExtra(OppoBatteryManager.EXTRA_ACCESSORY_PRESENT, mAccessoryPresent);
        intent.putExtra(OppoBatteryManager.EXTRA_ACCESSORY_STATUS, mAccessoryStatus);
        intent.putExtra(OppoBatteryManager.EXTRA_ACCESSORY_CAPACITY, mAccessoryCapacity);

        intent.putExtra(OppoBatteryManager.EXTRA_CHARGE_TIMEOUT, mChargeTimeout);

        intent.putExtra(OppoBatteryManager.EXTRA_CHARGE_SOC_FALL, mChargeSocFall);

        if (DEBUG || SystemProperties.getBoolean("debug.batteryservice", false)) {
            Slog.d(TAG, "Sending ACTION_ADDITIONAL_BATTERY_CHANGED.  chargervoltage: " + mChargerVoltage +
                    " ,batterycurrent: " + mBatteryCurrent +
                    " ,chargervolstatus: " + mChargeVolStatus +
                    " ,batterytempstatus: " + mBatteryTempStatus +
                    " ,batteryvolstatus: " + mBatteryVolStatus +
                    " ,accessorypresent: " + mAccessoryPresent +
                    " ,accessorystatus: " + mAccessoryStatus + 
                    " ,accessorycapacity: " + mAccessoryCapacity +
                    " ,chargetimeout: " + mChargeTimeout +
                    " ,chargesocfall: " + mChargeSocFall +
					" ,mPlugType: " + mPlugType);
        }

        mHandler.post(new Runnable() {
            @Override
            public void run() {
                ActivityManagerNative.broadcastStickyIntent(intent, null, UserHandle.USER_ALL);
            }
         });
    }

    public boolean getChargeSocFall() {
        return mChargeSocFall;
    }

    public int getChargerVoltage() {
        return mChargerVoltage;
    }

    public boolean dumpAddition(FileDescriptor fd, PrintWriter pw, String[] args) {
        boolean update = true;
        if (args == null || args.length == 0 || "-a".equals(args[0])) {
            pw.println("Current OPPO Battery Service state:");
            pw.println("  Charger voltage : " + mChargerVoltage);
            pw.println("  Battery current : " + mBatteryCurrent);
            pw.println("  Accessory present:" + mAccessoryPresent);
            pw.println("  Accessory status: " + mAccessoryStatus);
            pw.println("  Accessory capacity: " + mAccessoryCapacity);
            pw.println("  Chargetimeout: " + mChargeTimeout);
            pw.println("  Chargesocfall: " + mChargeSocFall);
            pw.println("  PlugType: " + mPlugType);
            pw.println("  UpdatesStopped: " + mUpdatesStopped);
            pw.println("  LowBatteryVoltageWarningLevel: " + mLowBatteryVoltageWarningLevel);
            pw.println("  HighBatteryVoltageWarningLevel: " + mHighBatteryVoltageWarningLevel);
            pw.println("  LowChargerVoltageWarningLevel: " + mLowChargerVoltageWarningLevel);
            pw.println("  HighChargerVoltageWarningLevel: " + mTomorrowLowBatteryWarningLevel);
            pw.println("  TomorrowLowBatteryWarningLevel: " + mTomorrowLowBatteryWarningLevel);
            pw.println("");
        } else if (args.length == 3 && "set".equals(args[0])) {
            String key = args[1];
            String value = args[2];
            try {
                if ("voltage".equals(key)){
                    mBatteryVoltage = Integer.parseInt(value);
                } else if ("temperature".equals(key)){
                    mBatteryTemperature = Integer.parseInt(value);
                } else if ("current".equals(key)){
                    mBatteryCurrent = Integer.parseInt(value);
                } else if ("chargervol".equals(key)){
                    mChargerVoltage = Integer.parseInt(value);
                } else if ("chargetimeout".equals(key)) {
                    mChargeTimeout = Integer.parseInt(value) != 0;
                } else if ("chargesocfall".equals(key)) {
                    mChargeSocFall = Integer.parseInt(value) != 0;
                } else {
                    pw.println("Unknown set option: " + key);
                    update = false;
                }
                if (update) {
                    mUpdatesStopped = true;
                }
            } catch (NumberFormatException ex) {
                pw.println("Bad value: " + value);
            }
        } else if (args.length == 1 && "reset".equals(args[0])) {
                mUpdatesStopped = false;
        } else {
            pw.println("Dump current OPPO battery state, or:");
            pw.println("  set voltage|temperature|current|chargervol|chargetimeout|chargesocfall <value>");
            pw.println("  reset");
            pw.println("");
        }
        return update;
    }

    public native void native_update();
}
