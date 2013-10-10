/***********************************************************
** Copyright (C), 2008-2012, OPPO Mobile Comm Corp., Ltd
** VENDOR_EDIT
** File: PermissionsHelper.java
* Description: help resolving Permissions for NearMe.
** Version: 1.0
** Date : 2012/10/31	
** Author: Wangy@OnLineRD.framework.pms
** 
** ------------------------------- Revision History: -------------------------------
**  	<author>		<data> 	   <version >	       <desc>
**      Wangy        2012/10/31    1.0         build this moudle
****************************************************************/


package com.android.server.pm;

import android.os.Build;
import android.os.Environment;
import android.util.Log;
import android.content.pm.PackageParser;

import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Enumeration;
import java.util.zip.ZipEntry;
import java.util.zip.ZipException;
import java.util.zip.ZipFile;
import java.io.File;
import java.io.IOException;

/*
 * @hide
 */
public class PermissionsHelper {

	private static Map<String, List<String>> sAllowedPackagesList = new HashMap<String, List<String>>();
	private static List<String> sNearMeMarketAllowedPermissionList = new LinkedList<String>();
    
	static {
		sNearMeMarketAllowedPermissionList.add("android.permission.INSTALL_PACKAGES");
		sNearMeMarketAllowedPermissionList.add("android.permission.MOVE_PACKAGE");
		sNearMeMarketAllowedPermissionList.add("android.permission.DELETE_PACKAGES");		
		sAllowedPackagesList.put("oppo.uid.nearme", sNearMeMarketAllowedPermissionList);
	}
	
	public static boolean allowPermission(String permission, PackageParser.Package pkg) {
        String id = pkg.mSharedUserId != null ? pkg.mSharedUserId : pkg.packageName;
		List<String> allowPermissionList = sAllowedPackagesList.get(id);
		if (null != allowPermissionList && allowPermissionList.contains(permission)) {
			return true;
		}
		return false;
	}

	public static boolean needCopyingLibsToDataForSystemApp(String path) {
		return ApkLibsHelper.isLibsNeedCopy(path);
	}

	private static class ApkLibsHelper {		
		private static final boolean DEBUG = false;
		private static final String LOG_TAG = "ApkLibsHelper";

		private static final String sSystemLib = Environment.getRootDirectory()
				.getAbsolutePath() + File.separator + "lib";
		
		private static final String sApkLib = "lib/";
		private static final int sApkLibLen = sApkLib.length();
		
		private static final String sLibPrefix = "/lib";
		private static final int sLibPrefixLen = sLibPrefix.length();
		
		private static final String sLibSuffix = ".so";
		private static final int sLibSuffixLen = sLibSuffix.length();
		
		private static final int sMinEntryLen = sApkLibLen + 2 + sLibPrefixLen + 1 + sLibSuffixLen;
		
		private ApkLibsHelper() {
		}

		public static boolean isLibsNeedCopy(String path) {
	        if (path == null) {
	            return false;
	        }
	        
			File file = new File(path);
			if (!file.exists() || file.isDirectory()) {
				return false;
			}
			
	        ZipFile zipFile = null;
			boolean need = false;
			try {
				zipFile = new ZipFile(file);
				Enumeration<? extends ZipEntry> entries = zipFile.entries();
				while(entries.hasMoreElements()) {
					ZipEntry entry = entries.nextElement();
					if (entry == null)
						continue;
					
					String filePath = entry.getName();
					if (!filePath.startsWith(sApkLib))
						// Make sure we're in the lib directory of the ZIP.
						continue;
					
					if (!filePath.endsWith(sLibSuffix))
						// Make sure this is a .so file
						continue;
					
					if (filePath.length() < sMinEntryLen)
						// Make sure the filename is at least to the minimum library name size.
						continue;
					
					// Check to make sure the CPU ABI of this file is one we support.
					final int lastIndexOfSeperator = filePath.lastIndexOf('/');
					if (lastIndexOfSeperator == -1)
						continue;
					
					final String cpuAbi = Build.CPU_ABI;
					final String cpuAbi2 = Build.CPU_ABI2;
					final int cpuAbiRegionSize = lastIndexOfSeperator - sApkLibLen;
					
					if (cpuAbiRegionSize == cpuAbi.length()
							&& filePath.substring(sApkLibLen, lastIndexOfSeperator).equals(cpuAbi)) {
						if (DEBUG) Log.v(LOG_TAG, "Using ABI " + cpuAbi);
					}
					else if (cpuAbiRegionSize == cpuAbi2.length()
							&& filePath.substring(sApkLibLen, lastIndexOfSeperator).equals(cpuAbi2)) {
						if (DEBUG) Log.v(LOG_TAG, "Using ABI " + cpuAbi2);
					}
					else {
						if (DEBUG) Log.v(LOG_TAG, "abi didn't match anything");
						continue;
					}
					
					// If any of the libraries has already been packed into system lib directory, return false
					final String fileName = filePath.substring(lastIndexOfSeperator + 1);
					File systemLibFile = new File(sSystemLib, fileName);
					if (systemLibFile.exists() && systemLibFile.isFile()) {
						need = false;
						break;
					}
					else {
						need = true;
					}
				}
				
			} catch (ZipException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			} finally {
	            if (zipFile != null) {
	                try {
	                    zipFile.close();
	                } catch (IOException e) {
	                }
	            }
	        }

	        if (DEBUG) if (need) Log.v(LOG_TAG, "need to unpack library for system apk: " + path);
			
			return need;
		}
	}
}
