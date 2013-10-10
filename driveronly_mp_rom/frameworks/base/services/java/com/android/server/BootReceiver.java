/*
 * Copyright (C) 2009 The Android Open Source Project
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

package com.android.server;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Build;
import android.os.DropBoxManager;
import android.os.FileObserver;
import android.os.FileUtils;
import android.os.RecoverySystem;
import android.os.SystemProperties;
import android.provider.Downloads;
import android.util.Slog;

import java.io.File;
import java.io.IOException;
//#ifdef VENDOR_EDIT
//Lizhong.Liang@PlatformSrv.Poweronoff, 2012/08/08, Add for reboot
import java.io.InputStreamReader;
import org.apache.http.util.EncodingUtils;
import java.io.FileInputStream;
//import android.rutils.RUtils;
import android.oppoutils.OppoUtils;
//#endif /* VENDOR_EDIT */

//#ifdef VENDOR_EDIT
//#@OppoHook
//ZhiYong.Lin@Plf.Framework.OTA, Add for read ota update result 2012/12/15
import com.oppo.ota.OppoOtaUtils;
//#endif /* VENDOR_EDIT */

//#ifdef VENDOR_EDIT
//#@OppoHook
//TongXi.Li@Plf.Framework, 2013/4/1, add for feed_back 
import java.io.InputStreamReader;
import org.apache.http.util.EncodingUtils;
import java.io.FileInputStream;
import android.os.OppoManager;
import java.text.SimpleDateFormat;
import java.util.Date;
//#endif /* VENDOR_EDIT */
/**
 * Performs a number of miscellaneous, non-system-critical actions
 * after the system has finished booting.
 */
public class BootReceiver extends BroadcastReceiver {
    private static final String TAG = "BootReceiver";

    // Maximum size of a logged event (files get truncated if they're longer).
    // Give userdebug builds a larger max to capture extra debug, esp. for last_kmsg.
    private static final int LOG_SIZE =
        SystemProperties.getInt("ro.debuggable", 0) == 1 ? 98304 : 65536;

    private static final File TOMBSTONE_DIR = new File("/data/tombstones");

    // The pre-froyo package and class of the system updater, which
    // ran in the system process.  We need to remove its packages here
    // in order to clean up after a pre-froyo-to-froyo update.
    private static final String OLD_UPDATER_PACKAGE =
        "com.google.android.systemupdater";
    private static final String OLD_UPDATER_CLASS =
        "com.google.android.systemupdater.SystemUpdateReceiver";

    // Keep a reference to the observer so the finalizer doesn't disable it.
    private static FileObserver sTombstoneObserver = null;

    @Override
    @android.annotation.OppoHook(level=android.annotation.OppoHook.OppoHookType.CHANGE_CODE,
    		property=android.annotation.OppoHook.OppoRomType.ROM,
            note="ZhiYong.Lin@Plf.Framework modify for ota update result; TongXi.Li@Plf.Framework modify for feed_back")
    public void onReceive(final Context context, Intent intent) {
        // Log boot events in the background to avoid blocking the main thread with I/O
        new Thread() {
            @Override
            public void run() {
            	//#ifdef VENDOR_EDIT
            	//#@OppoHook
				//ZhiYong.Lin@Plf.Framework.OTA, Add for read ota update result 2012/12/15
            	OppoOtaUtils.notifyOTAUpdateResult(context);
                //#endif /* VENDOR_EDIT */
            	
                try {
                    //#ifndef VENDOR_EDIT
					//#@OppoHook
                    //TongXi.Li@Plf.Framework, 2013/4/1, modify for feed_back
					/*
                    logBootEvents(context);
                    */
					//#else /* VENDOR_EDIT */
                    logOppoBootEvents(context);
                    //#endif /* VENDOR_EDIT */
                } catch (Exception e) {
                    Slog.e(TAG, "Can't log boot events", e);
                }
                try {
                    removeOldUpdatePackages(context);
                } catch (Exception e) {
                    Slog.e(TAG, "Can't remove old update packages", e);
                }

            }
        }.start();
    }

    private void removeOldUpdatePackages(Context context) {
        Downloads.removeAllDownloadsByPackage(context, OLD_UPDATER_PACKAGE, OLD_UPDATER_CLASS);
    }

    private void logBootEvents(Context ctx) throws IOException {
        final DropBoxManager db = (DropBoxManager) ctx.getSystemService(Context.DROPBOX_SERVICE);
        final SharedPreferences prefs = ctx.getSharedPreferences("log_files", Context.MODE_PRIVATE);
        final String headers = new StringBuilder(512)
            .append("Build: ").append(Build.FINGERPRINT).append("\n")
            .append("Hardware: ").append(Build.BOARD).append("\n")
            .append("Revision: ")
            .append(SystemProperties.get("ro.revision", "")).append("\n")
            .append("Bootloader: ").append(Build.BOOTLOADER).append("\n")
            .append("Radio: ").append(Build.RADIO).append("\n")
            .append("Kernel: ")
            .append(FileUtils.readTextFile(new File("/proc/version"), 1024, "...\n"))
            .append("\n").toString();

        String recovery = RecoverySystem.handleAftermath();
        if (recovery != null && db != null) {
            db.addText("SYSTEM_RECOVERY_LOG", headers + recovery);
        }
				
//#ifdef VENDOR_EDIT
//Lizhong.Liang@PlatformSrv.Poweronoff, 2012/08/10, Add for store the times of reboot 
/*
		String bootReason = readBootReason();
		Slog.d(TAG, "Aha! Boot reason is " + bootReason + "!!!");
		if (bootReason.equals("kernel") || bootReason.equals("modem") || bootReason.equals("android")) {
			 int panicTime = SystemProperties.getInt("persist.sys.panictime", 0);	
			 SystemProperties.set("persist.sys.panictime", Integer.toString(panicTime+1));			
		 }	
*/
//#endif /* VENDOR_EDIT */

        if (SystemProperties.getLong("ro.runtime.firstboot", 0) == 0) {
            String now = Long.toString(System.currentTimeMillis());
            SystemProperties.set("ro.runtime.firstboot", now);
            if (db != null) db.addText("SYSTEM_BOOT", headers);

            // Negative sizes mean to take the *tail* of the file (see FileUtils.readTextFile())
            addFileToDropBox(db, prefs, headers, "/proc/last_kmsg",
                    -LOG_SIZE, "SYSTEM_LAST_KMSG");
            addFileToDropBox(db, prefs, headers, "/cache/recovery/log",
                    -LOG_SIZE, "SYSTEM_RECOVERY_LOG");
            addFileToDropBox(db, prefs, headers, "/data/dontpanic/apanic_console",
                    -LOG_SIZE, "APANIC_CONSOLE");
            addFileToDropBox(db, prefs, headers, "/data/dontpanic/apanic_threads",
                    -LOG_SIZE, "APANIC_THREADS");
        } else {
            if (db != null) db.addText("SYSTEM_RESTART", headers);
        }

        // Scan existing tombstones (in case any new ones appeared)
        File[] tombstoneFiles = TOMBSTONE_DIR.listFiles();
        for (int i = 0; tombstoneFiles != null && i < tombstoneFiles.length; i++) {
            addFileToDropBox(db, prefs, headers, tombstoneFiles[i].getPath(),
                    LOG_SIZE, "SYSTEM_TOMBSTONE");
        }

        // Start watching for new tombstone files; will record them as they occur.
        // This gets registered with the singleton file observer thread.
        sTombstoneObserver = new FileObserver(TOMBSTONE_DIR.getPath(), FileObserver.CLOSE_WRITE) {
            @Override
            public void onEvent(int event, String path) {
                try {
                    String filename = new File(TOMBSTONE_DIR, path).getPath();
                    addFileToDropBox(db, prefs, headers, filename, LOG_SIZE, "SYSTEM_TOMBSTONE");
                } catch (IOException e) {
                    Slog.e(TAG, "Can't log tombstone", e);
                }
            }
        };

        sTombstoneObserver.startWatching();
    }

  	@android.annotation.OppoHook(level=android.annotation.OppoHook.OppoHookType.NEW_METHOD,
								property=android.annotation.OppoHook.OppoRomType.ROM,
            					note="TongXi.Li@Plf.Framework add for feed_back ")
    private void logOppoBootEvents(Context ctx) throws IOException {
        final DropBoxManager db = (DropBoxManager) ctx.getSystemService(Context.DROPBOX_SERVICE);
        final SharedPreferences prefs = ctx.getSharedPreferences("log_files", Context.MODE_PRIVATE);
        final StringBuilder headers = new StringBuilder(1024)
            .append("Build: ").append(Build.FINGERPRINT).append("\n")
            .append("Hardware: ").append(Build.BOARD).append("\n")
            .append("Bootloader: ").append(Build.BOOTLOADER).append("\n")
            .append("Radio: ").append(Build.RADIO).append("\n")
            .append("Kernel: ")
            .append(FileUtils.readTextFile(new File("/proc/version"), 1024, "...\n"))
			//oppo Peirs add for our kernel version is not the same as in /proc/version, so append the real one here:
            //if 12001 proj:
            //.append("Kernel real svn:" + SystemProperties.get("ro.buiid.soft.snapshot")).append("\n")
            //else if 12025 proj:
            .append("Kernel real svn:" + SystemProperties.get("ro.build.kernel.id")).append("\n")
			//oppo Peirs add end.
            .append("\n");

        String recovery = RecoverySystem.handleAftermath();
        if (recovery != null && db != null) {
            db.addText("SYSTEM_RECOVERY_LOG", headers.toString() + recovery);
        }
        
        String bootReason = readBootReason();
        Slog.d(TAG, "Aha! Boot reason is " + bootReason + "!!!");
		
        // here add because of died number
        if (bootReason.equals("kernel") || bootReason.equals("modem") || bootReason.equals("android")) {
            {
                SimpleDateFormat format = new SimpleDateFormat("yyyy-MM-dd hh:mm:SSS: ");
                String content = format.format(new Date()) + bootReason;
                int res = OppoManager.writeRawPartition(OppoManager.RAW_ID_BREAK_DOWN, content);
                if (res <= 0) {
                    db.addText("BOOT_REASON_RECORD_FAILED", "Boot reason record:\n" + content);
                    Slog.w(TAG, "Can't record boot reason!");
                }
            }            
            int panicTime = SystemProperties.getInt("persist.sys.panictime", 0) + 1;
            SystemProperties.set("persist.sys.panictime", Integer.toString(panicTime));            
        }
		
        if (SystemProperties.getLong("ro.runtime.firstboot", 0) == 0) {
            String now = Long.toString(System.currentTimeMillis());
            SystemProperties.set("ro.runtime.firstboot", now);
            
            if (bootReason.equals("kernel") || bootReason.equals("modem")) {
                addFileToDropBox(db, prefs, headers.toString(), "/proc/last_kmsg",
                        -LOG_SIZE, "SYSTEM_LAST_KMSG");
            } else if (bootReason.equals("android")) {
                String systemcrashFile = SystemProperties.get("persist.sys.panic.file", "");
				File sysFile = new File(systemcrashFile);
                if (sysFile != null && sysFile.exists()) {
				    SystemProperties.set("persist.sys.send.file", systemcrashFile);
					if (systemcrashFile.endsWith(".gz")) {
						if (db != null) {
							db.addText("SYSTEM_SERVER_GZ", "LOG FOR GZ");
						}	
					} else {
						addFileToDropBox(db, prefs, headers.toString(), systemcrashFile,
							LOG_SIZE, "SYSTEM_SERVER");		
					}
				} else {
					if (db != null) {
					    db.addText("SYSTEM_SERVER", headers.toString() 
									+ "system_Server is killed by other proc such as vold!!!\n\n"
									+ SystemProperties.get("persist.sys.panic.fd", ""));
					}
				}
            }
        }
		
		// init persist.sys.panic.file value
		SystemProperties.set("persist.sys.panic.file", "");
		SystemProperties.set("persist.sys.panic.fd", "");
    }
    
    
    @android.annotation.OppoHook(level=android.annotation.OppoHook.OppoHookType.NEW_FIELD,
								property=android.annotation.OppoHook.OppoRomType.ROM,
            					note="TongXi.Li@Plf.Framework add for feed_back ")
    private static final String BOOT_REASON_FILE = "/sys/power/app_boot";
    
	@android.annotation.OppoHook(level=android.annotation.OppoHook.OppoHookType.NEW_METHOD,
								property=android.annotation.OppoHook.OppoRomType.ROM,
            					note="TongXi.Li@Plf.Framework add for feed_back ")
    private String readBootReason() {
        String res = "";
        try {
            FileInputStream fin = new FileInputStream(BOOT_REASON_FILE);
            int length = fin.available();
            byte[] buffer = new byte[length];
            fin.read(buffer);
            res = EncodingUtils.getString(buffer, "UTF-8").trim();
            fin.close();
        } catch (Exception e) {
            e.printStackTrace();
        }	
        return res;
    }
    
	@android.annotation.OppoHook(level=android.annotation.OppoHook.OppoHookType.CHANGE_CODE,
								property=android.annotation.OppoHook.OppoRomType.ROM,
            					note="TongXi.Li@Plf.Framework modify for feed_back ")
    private static void addFileToDropBox(
            DropBoxManager db, SharedPreferences prefs,
            String headers, String filename, int maxSize, String tag) throws IOException {
        if (db == null || !db.isTagEnabled(tag)) return;  // Logging disabled

        File file = new File(filename);
        long fileTime = file.lastModified();
        if (fileTime <= 0) return;  // File does not exist

        if (prefs != null) {
            long lastTime = prefs.getLong(filename, 0);
            //#ifndef VENDOR_EDIT
			//#@OppoHook
            //TongXi.Li@Plf.Framework, 2013/4/1, modify for feed_back
            /*
            if (lastTime == fileTime) return;  // Already logged this particular file
            */
			//#else /* VENDOR_EDIT */
            if (!tag.equals("SYSTEM_LAST_KMSG") ) {          	    
                if (lastTime == fileTime) {
                    return;
                }
            }
            //#endif /* VENDOR_EDIT */
            // TODO: move all these SharedPreferences Editor commits
            // outside this function to the end of logBootEvents
            prefs.edit().putLong(filename, fileTime).apply();
        }

        Slog.i(TAG, "Copying " + filename + " to DropBox (" + tag + ")");
        //#ifndef VENDOR_EDIT
		//#@OppoHook
        //TongXi.Li@Plf.Framework, 2013/4/1, modify for feed_back
        /*
        db.addText(tag, headers + FileUtils.readTextFile(file, maxSize, "[[TRUNCATED]]\n"));
        */
		//#else /* VENDOR_EDIT */
        if(filename.indexOf("tombstoneNoCrash") == -1) {
            db.addText(tag, headers + FileUtils.readTextFile(file, maxSize, "[[TRUNCATED]]\n"));
        }
        //#endif /* VENDOR_EDIT */
    }
//#ifdef VENDOR_EDIT
}
