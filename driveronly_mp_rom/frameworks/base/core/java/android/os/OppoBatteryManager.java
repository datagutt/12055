/************************************************************************************
** Copyright (C), 2000-2013, OPPO Mobile Comm Corp., Ltd
** All rights reserved.
** 
** VENDOR_EDIT
** 
** Description: - 
**      OPPO Battery Manager
** 
** 
** --------------------------- Revision History: --------------------------------
** <author>		                      <data> 	<version >  <desc>
** ------------------------------------------------------------------------------
** yaolang@Plf.Framework  2013/07/30   1.0	    create file
** ------------------------------------------------------------------------------
** 
************************************************************************************/

package android.os;

/**
 * The OppoBatteryManager class contains strings and constants used for values
 * in the {#ACTION_ADDITIONAL_BATTERY_CHANGED} Intent.
 */
@android.annotation.OppoHook(level=android.annotation.OppoHook.OppoHookType.NEW_CLASS,
                            property=android.annotation.OppoHook.OppoRomType.ROM,
                            note="yaolang@Plf.Framework, add for oppo battery service")
public class OppoBatteryManager {
    /**
     * Extra for {@link #ACTION_ADDITIONAL_BATTERY_CHANGED}:
     * String describing the voltage of the charger.
     */
    public static final String EXTRA_CHARGER_VOLTAGE = "chargervoltage";

    /**
     * Extra for {@link #ACTION_ADDITIONAL_BATTERY_CHANGED}:
     * String describing the current of the battery.
     */
    public static final String EXTRA_BATTERY_CURRENT = "batterycurrent";

    /**
     * Extra for {@link #ACTION_ADDITIONAL_BATTERY_CHANGED}:
     * String describing the volume's status of the charger.
     */
    public static final String EXTRA_CHARGER_VOL_STATUS = "chargervolstatus";

    /**
     * Extra for {@link #ACTION_ADDITIONAL_BATTERY_CHANGED}:
     * String describing the temperature's status of the current battery.
     */
    public static final String EXTRA_BATTERY_TEMP_STATUS = "batterytempstatus";
    
    /**
     * Extra for {@link #ACTION_ADDITIONAL_BATTERY_CHANGED}:
     * String describing the volume's status of the current battery.
     */
    public static final String EXTRA_BATTERY_VOL_STATUS = "batteryvolstatus";

    /**
     * Extra for {@link #ACTION_ADDITIONAL_BATTERY_CHANGED}:
     * String describing if there's a accessory(i.e. extra battery) with phone.
     */
    public static final String EXTRA_ACCESSORY_PRESENT = "accessorypresent";

    /**
     * Extra for {@link #ACTION_ADDITIONAL_BATTERY_CHANGED}:
     * String describing the status of the current accessory.
     */
    public static final String EXTRA_ACCESSORY_STATUS = "accessorystatus";

    /**
     * Extra for {@link #ACTION_ADDITIONAL_BATTERY_CHANGED}:
     * String describing the capacity of the current accessory.
     */
    public static final String EXTRA_ACCESSORY_CAPACITY = "accessorycapacity";

    /**
     * Extra for {@link #ACTION_ADDITIONAL_BATTERY_CHANGED}:
     * String describing it's time out of charge.
     */
    public static final String EXTRA_CHARGE_TIMEOUT = "chargetimeout";

    /**
     * Extra for {@link #ACTION_ADDITIONAL_BATTERY_CHANGED}:
     * String describing the technology of the current battery.
     */
    public static final String EXTRA_CHARGE_SOC_FALL = "chargesocfall";

    /**
     * Broadcast Action:  This is a <em>sticky broadcast</em> containing the
     * current, temperature, volume status, and other information about the battery.
     * See {@link #OppoBatteryManager} for documentation on the
     * contents of the Intent.
     *
     * <p class="note">
     * You can <em>not</em> receive this through components declared
     * in manifests, only by explicitly registering for it with
     * {@link android.content.Context#registerReceiver(BroadcastReceiver, IntentFilter)
     * Context.registerReceiver()}.
     *
     * <p class="note">This is a protected intent that can only be sent
     * by the system.
     */
    public static final String ACTION_ADDITIONAL_BATTERY_CHANGED = "android.intent.action.ADDITIONAL_BATTERY_CHANGED";

    // values for "chargervolstatus" field in the ACTION_BATTERY_CHANGED Intent
    public static final int CHARGER_VOL_UNKNOW = 1;
    public static final int CHARGER_VOL_LOW = 2;
    public static final int CHARGER_VOL_NORMAL = 3;
    public static final int CHARGER_VOL_HIGH = 4;

    // values for "batteryvolstatus" field in the ACTION_BATTERY_CHANGED Intent
    public static final int BATTERY_VOL_UNKNOW = 1;
    public static final int BATTERY_VOL_LOW = 2;
    public static final int BATTERY_VOL_NORMAL = 3;
    public static final int BATTERY_VOL_HIGH = 4;

    // values for "BatteryTemperature" field in the ACTION_BATTERY_CHANGED Intent
    public static final int TEMPERATURE_STATUS_LOW = 1;
    public static final int TEMPERATURE_STATUS_NORMAL = 2;
    public static final int TEMPERATURE_STATUS_HIGH = 3;
    
    public static final int TEMPERATURE_THRESHOLD_LOW = -100;
    public static final int TEMPERATURE_THRESHOLD_HIGH = 550;

}
