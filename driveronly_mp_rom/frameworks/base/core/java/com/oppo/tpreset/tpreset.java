package com.oppo.tpreset;
import android.util.Log;

public class tpreset{
		static {
			System.loadLibrary("tpreset");
		}
		public tpreset(){}
		public static native int tp_reset();
		//#ifdef VENDOR_EDIT
		//moqingping@Prd.SysAPP.SecureMgr, 2013/04/08, Add for 
		public static native int oppo_lcd_display_change(int valuescreancolor);
		public static native int oppo_lcd_display_read();    
		//#endif /* VENDOR_EDIT */
		
		//#ifdef VENDOR_EDIT
		//SheTao@Prd.BasicSrv.PowerManager, 2013/05/21, Add for double click screen on
		public static native int tp_double_tap_enable(int enable);    
		//#endif /* VENDOR_EDIT */

        //#ifdef VENDOR_EDIT
        //SheTao@Prd.BasicSrv.PowerManager, 2013/08/13, Add for 13015 open button light for black touch panel.
		public static native int oppo_tp_colour_read();    
        //#endif /* VENDOR_EDIT */
		  
}

