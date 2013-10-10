/************************************************************************************
** File: frameworks\base\services\java\com\android\server\\OppoUsbManager.java
** VENDOR_EDIT
** Copyright (C), 2008-2012, OPPO Mobile Comm Corp., Ltd
** 
** Abstract: - 
**      It is used for manager USB function
** 
** Version: 1.0.0
** Date created: 15:44:25,13/06/2012
** Author: Bingshu.Zhou@Prd.SysSrv.USB
** 
** --------------------------- Revision History: --------------------------------
** Revision by Tongxi.li@Plf.Framework 
** Date: 12/7/2013
************************************************************************************/
package com.android.server.usb;
import android.app.PendingIntent;
import android.app.Notification;
import android.app.NotificationManager;
import android.content.ActivityNotFoundException;
import android.content.BroadcastReceiver;
import android.content.ContentResolver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.content.res.Resources;
import android.hardware.usb.UsbManager;
import android.net.Uri;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.os.Message;
import android.os.storage.StorageManager;
import android.os.SystemProperties;
import android.util.Slog;
import android.util.Log;
import java.io.File;
import java.util.ArrayList;
import java.util.List;
import android.provider.Settings;
import com.android.internal.telephony.ITelephony;
import java.util.Timer;
import java.util.TimerTask;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;
import android.net.ConnectivityManager;
import android.os.ServiceManager;
import android.os.RemoteException;
import android.app.ActivityManagerNative;
import android.app.IActivityManager;
import com.oppo.content.OppoIntent;
import android.os.UserHandle;
import android.os.Environment.UserEnvironment;
import oppo.R;
/**
 * OppoUsbManager manages USB function in device mode.
 */
@android.annotation.OppoHook(level=android.annotation.OppoHook.OppoHookType.NEW_CLASS,
							 property=android.annotation.OppoHook.OppoRomType.ROM,
            				 note="TongXi.Li@Plf.Framework modify for oppo_usb ")
public class OppoUsbManager {
    private static final String TAG = "OppoUsbManager";    
    private static final boolean DEBUG = true;
    private static final String IPO_POWER_ON  = "android.intent.action.ACTION_BOOT_IPO";
    private static final String IPO_POWER_OFF = "android.intent.action.ACTION_SHUTDOWN_IPO";
    private final Context mContext;
    private final UsbDeviceManager mDeviceManager;
    private NotificationManager mNotificationManager;    
    private boolean mUsbConnected=false;
    private UsbSelectionHandler mUsbSelectionHandler;
    private static final int MESSAGE_USB_SELECTION_TYPE = 1;
    private static final boolean DBG = true;
    private Timer mInterruptTimer = null;
    private List<Timer> mTimerList = new ArrayList<Timer>();
    private boolean mProcessing = false;
    private boolean mUmsProcessing = false;
    private HandlerThread mUsbSelectionThread;
    private Notification mUsbNotification = null;
    private int mUsbSelected = -1;	
    private int usbFlag = 0;
    private boolean bootComplete = false;
    private IActivityManager mAm;
    File externalStorageDir = null;
	
	private StorageManager mStorageManager;
    private boolean LastPlugUsbType=false;
	private String Imei = null;
	private final int MESSAGE_BOOT = 0;
	private final long BOOTCOMPLETE_DELAY_TIME = 3*1000;//delay 3S sent message
	private final long BOOTIPO_DELAY_TIME = 2*1000;//delay 2S sent message
    private boolean IsShutdownIpo=false;	

    public static final int USB_CHARGE = 0;
    public static final int USB_STORAGE = 1;
    public static final int USB_MTP = 2;  

    public static final String USB_SELECT_ACTION = "android.intent.action.ACTION_USB_SELECTION";
    public static final String USB_SELECTED_ACTION = "android.intent.action.ACTION_USB_SELECED";
    public static final String USB_SELECTED_FINISH_ACTION = "android.intent.action.ACTION_USB_SELECED_FINISH";
    public static final String EXTRA_USB_SELECTED_TYPE = "UsbSelectedType";	
	
    public OppoUsbManager(Context context,UsbDeviceManager deviceManager) {
        mContext = context;
        mDeviceManager = deviceManager;
        PackageManager pm = mContext.getPackageManager();
		
		final int userId = UserHandle.myUserId();
        UserEnvironment currentUser = new UserEnvironment(userId);    
		externalStorageDir = currentUser.getExternalStorageDirectory();

        mStorageManager = (StorageManager) mContext.getSystemService(Context.STORAGE_SERVICE);		
		mNotificationManager = (NotificationManager)mContext.getSystemService(Context.NOTIFICATION_SERVICE); 
        mAm = ActivityManagerNative.getDefault();
        
        mContext.registerReceiver(mUsbStateReceiver, new IntentFilter(UsbManager.ACTION_USB_STATE));

        IntentFilter filter = new IntentFilter();
		filter.addAction(USB_SELECTED_FINISH_ACTION);		
		filter.addAction("android.intent.action.ACTION_SHUTDOWN_IPO");
		filter.addAction("android.intent.action.ACTION_BOOT_IPO");
		filter.addAction("android.intent.action.IPO_DONE");
		filter.addAction(Intent.ACTION_BOOT_COMPLETED); 
		mContext.registerReceiver(mReceiver, filter);

		IntentFilter usbFilter = new IntentFilter();
		usbFilter.addAction(USB_SELECTED_ACTION);		
		mContext.registerReceiver(mUsbSelectionReceiver, usbFilter);
		IntentFilter mediaFilter = new IntentFilter();
		mediaFilter.addAction(Intent.ACTION_MEDIA_SHARED);
		mediaFilter.addAction(Intent.ACTION_MEDIA_UNSHARED);
		mediaFilter.addDataScheme("file");
		mContext.registerReceiver(mUsbSelectionReceiver, mediaFilter);
		
        mUsbSelectionThread  = new HandlerThread("OppoUsbManager");
        mUsbSelectionThread.start();     
        mUsbSelectionHandler = new UsbSelectionHandler(mUsbSelectionThread.getLooper());
		
    }

    
    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();            
            if (DBG) Log.d(TAG, "Receive intent.action = " + action);          			
            if(action.equals(Intent.ACTION_BOOT_COMPLETED)){
        	    if (DBG) Log.d(TAG,"BOOT_COMPLETED");			    
        	    mBootHandler.sendEmptyMessageDelayed(MESSAGE_BOOT,BOOTCOMPLETE_DELAY_TIME);
            }else if(action.equals("android.intent.action.ACTION_SHUTDOWN_IPO")){
                if (DBG) Log.d(TAG,"ACTION_SHUTDOWN_IPO");
                IsShutdownIpo=true;
                bootComplete=false;
            }else if(action.equals("android.intent.action.ACTION_BOOT_IPO")){
                if (DBG) Log.d(TAG,"android.intent.action.ACTION_BOOT_IPO");               
                mBootHandler.sendEmptyMessageDelayed(MESSAGE_BOOT,BOOTIPO_DELAY_TIME);
            }
        }
    };

        
    private final Handler mBootHandler = new Handler() {
        @Override
        public void handleMessage(Message msg){
            if (DBG) Slog.d(TAG, "bootcomplete:hand after 3s---bootIPO:hand after 2s");
            ContentResolver cr = mContext.getContentResolver();
            IsShutdownIpo=false; 			
            bootComplete=true;
            usbFlag=1;
            mUsbSelected = -1;			
            if(Settings.System.getInt(cr,Settings.System.USB_NO_ASK_AGAIN, 0)== 0){
                 handleUsbSelectedStart(0);
            }
            startUsbSelectionActivity();                
        }
    };
        

    private BroadcastReceiver mUsbStateReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            mUsbConnected = intent.getBooleanExtra(UsbManager.USB_CONNECTED, false);                
            if (DBG) Log.d(TAG, "1.UsbPlugType = " + mUsbConnected);
            if (mUsbConnected != LastPlugUsbType) { 
                LastPlugUsbType=mUsbConnected;
                if (DBG) Log.d(TAG, "1.UsbPlugType = " + mUsbConnected);
                startUsbSelectionActivity();
            }
        }
    };

    private BlockingQueue<Intent> mStateQueue = new LinkedBlockingQueue<Intent>(100);
    
    private BroadcastReceiver mUsbSelectionReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            if (DBG) Slog.i(TAG, "mUsbSelectionReceiver action: " + action);
            if (action.equals(USB_SELECTED_ACTION)){
                final int type = intent.getIntExtra(EXTRA_USB_SELECTED_TYPE,USB_CHARGE);
                handleUsbSelectedStart(type);
            } else if (action.equals(Intent.ACTION_MEDIA_SHARED)
                        || action.equals(Intent.ACTION_MEDIA_UNSHARED)){
                try {
                    if (mProcessing){
                        mStateQueue.remove(intent);
                        mStateQueue.put(intent);
                    }
                } catch (InterruptedException ex){
                    Slog.e(TAG, "Failed to put response onto queue", ex);
                }
            }
        }
    };
    
	private void startUsbSelectionActivity() {
		Log.i(TAG, "2. startUsbSelectionActivity()");
		ContentResolver cr = mContext.getContentResolver();
		if (cr != null && Settings.System.getInt(cr, Settings.System.USB_NO_ASK_AGAIN,0) == 0) {
			if (mUsbConnected == true && bootComplete) {			
				ITelephony phoneServ = ITelephony.Stub.asInterface(ServiceManager
								.checkService(Context.TELEPHONY_SERVICE));				
				if (phoneServ != null) {
					try {
						if (!(phoneServ.isRinging() || phoneServ.isOffhook())) {
							startActivity();
						}
					} catch (RemoteException e) {
						Log.w(TAG, "Phone sevices remoteException" + e);
					} catch (ActivityNotFoundException e) {
						Log.w(TAG,"Start usb selection activity threw ActivityNotFoundException "+ e);
					}
				} else {
					try {
						startActivity();
					} catch (ActivityNotFoundException e) {
						Log.w(TAG,"Start usb selection activity threw ActivityNotFoundException "+ e);
					}
				}
				if (!mProcessing) {
					updateUsbNotification();
				}
			} else if ((usbFlag == 1 && mUsbConnected == true && bootComplete)) {
				try {
					Log.i(TAG, "the first IPO complete usbselectActivity");
					startActivity();
					updateUsbNotification();
					usbFlag = 0;
				} catch (ActivityNotFoundException e) {
					Log.e(TAG,"Start usb selection activity threw ActivityNotFoundException "+ e);
				}
			} else if (bootComplete && mUsbConnected == false) {
				mNotificationManager.cancel(R.string.usb_selection);
				handleUsbSelectedStart(0);
			}
		} else if (cr != null && Settings.System.getInt(cr, Settings.System.USB_NO_ASK_AGAIN,0) != 0) {
			Log.i(TAG, "second");
			if (mUsbConnected && bootComplete) {
				int selectTmp = Settings.System.getInt(cr,Settings.System.USB_REMEBER_SELECTION, 0);
				Log.i(TAG, "selectTmp= " + selectTmp);
				handleUsbSelectedStart(selectTmp);
				if (selectTmp == 0 && mUsbConnected == true) {
					updateUsbNotification();
				}
			} else if (mUsbConnected == false && bootComplete) {
				mNotificationManager.cancel(R.string.usb_selection);
				handleUsbSelectedStart(0);
			}
		}
	}

	private void startActivity(){
	    Intent usbIntent = new Intent(USB_SELECT_ACTION);
		usbIntent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK
							| Intent.FLAG_ACTIVITY_RESET_TASK_IF_NEEDED
							| Intent.FLAG_ACTIVITY_NO_USER_ACTION);
		mContext.startActivity(usbIntent);
	}

    private void updateUsbNotification() {		
		if (mNotificationManager != null) {
			mNotificationManager.cancel(R.string.usb_selection);
			Resources r = mContext.getResources();
			CharSequence title = r.getText(getTitleId());
			CharSequence message = r.getText(R.string.usb_selection);
			Log.i(TAG, "9. title=" + title + "  message=" + message);
			if (mUsbNotification == null) {
				mUsbNotification = new Notification();
				if (mUsbNotification == null)
					return;
			}
			mUsbNotification.icon = R.drawable.oppo_stat_sys_adb;
			mUsbNotification.when = 0;
			mUsbNotification.flags = Notification.FLAG_ONGOING_EVENT;
			mUsbNotification.tickerText = title;
			mUsbNotification.defaults = 0; 
			mUsbNotification.sound = null;
			mUsbNotification.vibrate = null;
			Intent usbIntent = new Intent(USB_SELECT_ACTION);
			usbIntent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_RESET_TASK_IF_NEEDED);
			PendingIntent pi = PendingIntent.getActivity(mContext, 0,usbIntent, 0);
			mUsbNotification.setLatestEventInfo(mContext, title, message, pi);
			mNotificationManager.notify(R.string.usb_selection,mUsbNotification);
		}
	}

	private void handleUsbSelectedStart(int type) {
		if(mUsbSelected != type) {
			mUsbSelected = type;
			if (DBG)
				Slog.i(TAG, "12. USB_SELECTED_ACTION type: " + type);
			if (USB_STORAGE == type) {
				mContext.sendBroadcast(new Intent(
						OppoIntent.ACTION_PRE_MEDIA_SHARED));
				mUmsProcessing = true;
			}
			Message msg = mUsbSelectionHandler.obtainMessage(MESSAGE_USB_SELECTION_TYPE, type, 0);
			mUsbSelectionHandler.removeMessages(MESSAGE_USB_SELECTION_TYPE);
			startInterruptTimer();
			mUsbSelectionHandler.sendMessage(msg);
		} else {
			Slog.i(TAG, "The same as before");
		}
	}

	private class UsbSelectionHandler extends Handler {
		public UsbSelectionHandler(Looper loop) {
			super(loop);
		}

		@Override
		public void handleMessage(Message msg) {			
			switch (msg.what) {
			case MESSAGE_USB_SELECTION_TYPE:
				handleUsbSelected(msg.arg1);
				break;

			default:
				break;
			}
		}
	}

	private void handleUsbSelected(int type) {
		if (DBG)
			Slog.i(TAG, "15. handleUsbSelected TYPE= " + type +"mProcessing 1 : " +mProcessing);		
		mProcessing = true;		
		usbSelectionInit();
		// We should clear state queue here
		mStateQueue.clear();
		if (mUsbConnected == true) {
			updateUsbNotification();
		}
		switch (type) {
		case USB_STORAGE:
			mDeviceManager.setCurrentFunctions(UsbManager.USB_FUNCTION_MASS_STORAGE, true);
			enableUMS();
			mUmsProcessing = false;
			break;

		case USB_MTP:
			mDeviceManager.setCurrentFunctions(UsbManager.USB_FUNCTION_MTP,	true);
			break;

		case USB_CHARGE:
			mDeviceManager.setCurrentFunctions(UsbManager.USB_FUNCTION_MASS_STORAGE, true);
			break;

		default:			
			mDeviceManager.setCurrentFunctions(UsbManager.USB_FUNCTION_MASS_STORAGE, true);
			break;
		}
		mProcessing = false;
		if (DBG)
			Slog.i(TAG, "handleUsbSelected mProcessing 2 : " + mProcessing);
		stopInterruptTimer();
		updateUsbSelection();
	}

	private void startInterruptTimer() {
		if (DBG)
			Slog.i(TAG, "13. startInterruptTimer");
		mInterruptTimer = new Timer();
		TimerTask task = new TimerTask() {
			public void run() {
				if (null != mUsbSelectionThread) {
					if (DBG)
						Slog.i(TAG, "13.1 interrupt task");
					mProcessing = false;
					mUmsProcessing = false;
					mUsbSelectionThread.interrupt();
				}
			}
		};
		mInterruptTimer.schedule(task, 300 * 1000);
		mTimerList.add(mInterruptTimer);
	}

	private void stopInterruptTimer() {
		if (DBG)
			Slog.i(TAG, "stopInterruptTimer");
		if (mTimerList.size() > 0) {
			mInterruptTimer = mTimerList.get(0);
			if (mInterruptTimer != null) {
				mInterruptTimer.cancel();
				mTimerList.remove(0);
			}
		}
	}

	private void updateUsbSelection() {
		if (DBG)
			Slog.i(TAG, "updateUsbSelection= " + mUsbSelected);
		Intent intent = new Intent(USB_SELECTED_FINISH_ACTION);
		intent.putExtra(EXTRA_USB_SELECTED_TYPE, mUsbSelected);
		mContext.sendBroadcast(intent);
	}

	private void usbSelectionInit() {
		disableUMS();
	}

	private void enableUMS() {
		if (DBG) 
		    Slog.i(TAG, "16. enableUMS");
		for (int i = 0; i < 10; i++) {
			String isFormating = SystemProperties.get("persist.oppo.formating","no");
			Slog.i(TAG, "isFormating = " + isFormating);
			if ("no".equals(isFormating))
				break;
			try {
				Slog.i(TAG, "SDcard is Formating!sleep! i=" + i);
				Thread.sleep(1000);
			} catch (InterruptedException e) {
			}
		}
		if (null != mStorageManager
				&& !mStorageManager.isUsbMassStorageEnabled()) {
			mStorageManager.enableUsbMassStorage();
			if (DBG)
				Slog.i(TAG, "before latency wait Intent.ACTION_MEDIA_SHARED");
			latency(Intent.ACTION_MEDIA_SHARED,
					Uri.parse("file://" + externalStorageDir.toString()).toString());

		}
	}

	private void disableUMS() {
		if (DBG)
			Slog.i(TAG, "5. disableUMS");		
		if (mStorageManager != null
				&& mStorageManager.isUsbMassStorageEnabled()) {
			mStorageManager.disableUsbMassStorage();
			if (DBG)
				Slog.i(TAG, "before latency wait Intent.ACTION_MEDIA_UNSHARED");
			latency(Intent.ACTION_MEDIA_UNSHARED,
					Uri.parse("file://" + externalStorageDir.toString()).toString());
		}
	}

	private boolean latency(String action, String extra) {
		while (true) {
			try {
				Intent i = mStateQueue.take();
				if (DBG) {
					Slog.i(TAG, "Request action : " + action + " extra : "+ extra);
					Slog.i(TAG, "latency intent : ");
					Slog.i(TAG, "  Component: " + i.getComponent());
					Slog.i(TAG, "  Aciton: " + i.getAction());
					Slog.i(TAG, "  Categories: " + i.getCategories());
					Slog.i(TAG, "  Data: " + i.getData());
					Slog.i(TAG, "  DataType: " + i.getType());
					Slog.i(TAG, "  DataSchema: " + i.getScheme());
				}
				if (action.equals(i.getAction())) {
					if ((null == extra) || (null != extra && extra.equals(i.getDataString()))) {
						if (DBG) Slog.i(TAG, "latency finished");
						return true;
					}
				}
			} catch (InterruptedException e) {
				Slog.e(TAG, "Failed to get action : " + action, e);
				handleUsbSelectedStart(0);
				return false;
			}
		}
	}

	public boolean isUmsProcessing() {
		return mUmsProcessing;
	}

	public boolean isUsbSelectionProcessing() {
		return mProcessing;
	}

	public int getUsbSeletcedType() {
		return mUsbSelected;
	}

	public boolean isUsbConnected() {
		return mUsbConnected;
	}

	private int getTitleId() {
		Slog.i(TAG, "8. getTitleId");
		int id = R.string.usb_charge;
		switch (mUsbSelected) {
		case USB_CHARGE: {
			id = R.string.usb_charge;
		}
			break;

		case USB_STORAGE: {
			id = R.string.usb_storage;
		}
			break;

		case USB_MTP: {
			id = R.string.usb_mtp;
		}
			break;

		default: {
			id = R.string.usb_charge;
		}
			break;
		}
		return id;
	}	         
}
