/*
 * Copyright (C) 2010 The Android Open Source Project
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

package android.drm;

import android.content.ContentResolver;
import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.media.MediaScannerConnection;
import android.media.MediaScannerConnection.OnScanCompletedListener;
import android.net.Uri;
import android.provider.MediaStore;
import android.util.Log;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;

/**
 * A utility class that provides operations for parsing extended metadata embedded in
 * DRM constraint information. If a DRM scheme has specific constraints beyond the standard
 * constraints, the constraints will show up in the
 * {@link DrmStore.ConstraintsColumns#EXTENDED_METADATA} key. You can use
 * {@link DrmUtils.ExtendedMetadataParser} to iterate over those values.
 */
public class DrmUtils {
    /* Should be used when we need to read from local file */
    /* package */ static byte[] readBytes(String path) throws IOException {
        File file = new File(path);
        return readBytes(file);
    }

    /* Should be used when we need to read from local file */
    /* package */ static byte[] readBytes(File file) throws IOException {
        FileInputStream inputStream = new FileInputStream(file);
        BufferedInputStream bufferedStream = new BufferedInputStream(inputStream);
        byte[] data = null;

        try {
            int length = bufferedStream.available();
            if (length > 0) {
                data = new byte[length];
                // read the entire data
                bufferedStream.read(data);
             }
        } finally {
            quietlyDispose(bufferedStream);
            quietlyDispose(inputStream);
        }
        return data;
    }

    /* package */ static void writeToFile(final String path, byte[] data) throws IOException {
        /* check for invalid inputs */
        FileOutputStream outputStream = null;

        if (null != path && null != data) {
            try {
                outputStream = new FileOutputStream(path);
                outputStream.write(data);
            } finally {
                quietlyDispose(outputStream);
            }
        }
    }

    /* package */ static void removeFile(String path) throws IOException {
        File file = new File(path);
        file.delete();
    }

    private static void quietlyDispose(InputStream stream) {
        try {
            if (null != stream) {
                stream.close();
            }
        } catch (IOException e) {
            // no need to care, at least as of now
        }
    }

    private static void quietlyDispose(OutputStream stream) {
        try {
            if (null != stream) {
                stream.close();
            }
        } catch (IOException e) {
            // no need to care
        }
    }

    /**
     * Gets an instance of {@link DrmUtils.ExtendedMetadataParser}, which can be used to parse
     * extended metadata embedded in DRM constraint information.
     *
     * @param extendedMetadata Object in which key-value pairs of extended metadata are embedded.
     *
     */
    public static ExtendedMetadataParser getExtendedMetadataParser(byte[] extendedMetadata) {
        return new ExtendedMetadataParser(extendedMetadata);
    }

    /**
     * Utility that parses extended metadata embedded in DRM constraint information.
     *<p>
     * Usage example:
     *<p>
     * byte[] extendedMetadata<br>
     * &nbsp;&nbsp;&nbsp;&nbsp; =
     *         constraints.getAsByteArray(DrmStore.ConstraintsColumns.EXTENDED_METADATA);<br>
     * ExtendedMetadataParser parser = getExtendedMetadataParser(extendedMetadata);<br>
     * Iterator keyIterator = parser.keyIterator();<br>
     * while (keyIterator.hasNext()) {<br>
     *     &nbsp;&nbsp;&nbsp;&nbsp;String extendedMetadataKey = keyIterator.next();<br>
     *     &nbsp;&nbsp;&nbsp;&nbsp;String extendedMetadataValue =
     *             parser.get(extendedMetadataKey);<br>
     * }
     */
    public static class ExtendedMetadataParser {
        HashMap<String, String> mMap = new HashMap<String, String>();

        private int readByte(byte[] constraintData, int arrayIndex) {
            //Convert byte[] into int.
            return (int)constraintData[arrayIndex];
        }

        private String readMultipleBytes(
                byte[] constraintData, int numberOfBytes, int arrayIndex) {
            byte[] returnBytes = new byte[numberOfBytes];
            for (int j = arrayIndex, i = 0; j < arrayIndex + numberOfBytes; j++,i++) {
                returnBytes[i] = constraintData[j];
            }
            return new String(returnBytes);
        }

        /*
         * This will parse the following format
         * KeyLengthValueLengthKeyValueKeyLength1ValueLength1Key1Value1..\0
         */
        private ExtendedMetadataParser(byte[] constraintData) {
            //Extract KeyValue Pair Info, till terminator occurs.
            int index = 0;

            while (index < constraintData.length) {
                //Parse Key Length
                int keyLength = readByte(constraintData, index);
                index++;

                //Parse Value Length
                int valueLength = readByte(constraintData, index);
                index++;

                //Fetch key
                String strKey = readMultipleBytes(constraintData, keyLength, index);
                index += keyLength;

                //Fetch Value
                String strValue = readMultipleBytes(constraintData, valueLength, index);
                if (strValue.equals(" ")) {
                    strValue = "";
                }
                index += valueLength;
                mMap.put(strKey, strValue);
            }
        }

        /**
         * This method returns an iterator object that can be used to iterate over
         * all values of the metadata.
         *
         * @return The iterator object.
         */
        public Iterator<String> iterator() {
            return mMap.values().iterator();
        }

        /**
         * This method returns an iterator object that can be used to iterate over
         * all keys of the metadata.
         *
         * @return The iterator object.
         */
        public Iterator<String> keyIterator() {
            return mMap.keySet().iterator();
        }

        /**
         * This method retrieves the metadata value associated with a given key.
         *
         * @param key The key whose value is being retrieved.
         *
         * @return The metadata value associated with the given key. Returns null
         * if the key is not found.
         */
        public String get(String key) {
            return mMap.get(key);
        }
    }

    // M:
    // the following are for OMA DRM v1.0 implementation.
    /**
     * Get action according to the mime type of drm media content.
     * The mime type may start with "audio/", "video/", "image/"
     *
     * @param mime Mime type.
     * @return int Action.
     * @hide
     */
    public static int getAction(String mime) {
        if (mime.startsWith(DrmStore.MimePrefix.IMAGE)) {
            return DrmStore.Action.DISPLAY;
        } else if (mime.startsWith(DrmStore.MimePrefix.AUDIO)
                   || mime.startsWith(DrmStore.MimePrefix.VIDEO)) {
            return DrmStore.Action.PLAY;
        }

        return DrmStore.Action.PLAY; // otherwise PLAY is returned.
    }

    private static final String TAG = "DrmUtils";

    private static final Uri[] CID_URIS = new Uri[] {
        MediaStore.Audio.Media.INTERNAL_CONTENT_URI,
        MediaStore.Images.Media.INTERNAL_CONTENT_URI,
        MediaStore.Video.Media.INTERNAL_CONTENT_URI,
        MediaStore.Audio.Media.EXTERNAL_CONTENT_URI,
        MediaStore.Images.Media.EXTERNAL_CONTENT_URI,
        MediaStore.Video.Media.EXTERNAL_CONTENT_URI
    };

    /**
     * Rescan the DRM files with the same DRM content Uri.
     *
     * @param context
     * @param drmContentUri
     * @param callback
     * @return the count of files will be scanned
     * @hide
     */
    public static int rescanDrmMediaFiles(Context context, String drmContentUri, OnDrmScanCompletedListener callback) {
        Log.v(TAG, "rescanDrmMediaFiles() : drmContentUri=" + drmContentUri + " callback=" + callback);

        ContentResolver cr = context.getContentResolver();
        ArrayList<String> pathArray = new ArrayList<String>();
        ContentValues values = new ContentValues();
        values.put(MediaStore.MediaColumns.DATE_MODIFIED, 0);
        String where = "drm_content_uri=?";
        String[] whereArgs = new String[] {drmContentUri};
        int length = CID_URIS.length;
        for (int i = 0; i < length; i++) {
            Uri uri = CID_URIS[i];
            Cursor cursor = cr.query(uri, new String[] {"_data"}, where, whereArgs, null);
            if (cursor != null) {
                while (cursor.moveToNext()) {
                    pathArray.add(cursor.getString(0));
                }
                cursor.close();
            }
        }
        int total = pathArray.size();
        if (total < 1) {
            if (callback != null) {
                callback.onScanCompletedAll(0);
            }
        } else {
            String[] paths = new String[total];
            pathArray.toArray(paths);
            if (callback != null) {
                DrmScanCompletedProxy clientProxy = new DrmScanCompletedProxy(callback, total);
                MediaScannerConnection.scanFile(context, paths, null, clientProxy);
            } else {
                MediaScannerConnection.scanFile(context, paths, null, null);
            }

            int size = pathArray.size();
            for (int i = 0; i < size; i++) {
                Log.v(TAG, "rescanDrmMediaFiles() : path " + i + "=" + pathArray.get(i));
            }
        }
        Log.v(TAG, "rescanDrmMediaFiles() : total need scan: " + total);
        return total;
    }

    /**
     * Interface for notifying clients of the result of scanning a requested media file width DRM info.
     * @hide
     */
    public interface OnDrmScanCompletedListener {
        /**
         * Called to notify the client when the media scanner has finished scanning a file.
         *
         * @param path the path to the file that has been scanned.
         * @param uri the Uri for the file if the scanning operation succeeded
         * and the file was added to the media database, or null if scanning failed.
         */
        void onScanCompletedOne(String path, Uri uri);
        /**
         * When all files are scanned, this function will be called.
         *
         * @param scannedCount files have been scanned.
         */
        void onScanCompletedAll(int scannedCount) ;
    }

    private static class DrmScanCompletedProxy implements OnScanCompletedListener {
        private int mScannedCount;
        private int mScanCount;
        private OnDrmScanCompletedListener mClient;

        public DrmScanCompletedProxy(OnDrmScanCompletedListener callback, int scanCount) {
            mScannedCount = 0;
            mScanCount = scanCount;
            mClient = callback;
        }

        public void onScanCompleted(String path, Uri uri) {
            mScannedCount++;
            mClient.onScanCompletedOne(path, uri);
            if (mScannedCount >= mScanCount) {
                mClient.onScanCompletedAll(mScannedCount);
            }
            Log.v(TAG, "onScanCompleted() : path=" + path + ", uri=" + uri);
            Log.v(TAG, "onScanCompleted() : mScannedCount=" + mScannedCount + ", mScanCount=" + mScanCount);
        }
    }

    /**
     * Check the file described by Uri is drm or not.
     * <br/>Only support file:/// and content:// style,
     * and judge from its file path.
     * <br/>Note: context must not be null
     *
     * @param context
     * @param uri
     * @param client
     * @return null if context or uri or client is null
     * @hide
     */
    public static DrmProfile getDrmProfile(Context context, Uri uri, DrmManagerClient client) {
        Log.v(TAG, "getDrmProfile() : uri: " + uri);
        if (context == null || uri == null || client == null) {
            return null;
        }

        DrmProfile profile = new DrmProfile();
        String scheme = uri.getScheme();
        if (ContentResolver.SCHEME_CONTENT.equals(scheme)
            && MediaStore.AUTHORITY.equals(uri.getHost())) { // the uri is content case
            Cursor c = null;
            String[] projection = new String[] {
                MediaStore.Audio.Media.IS_DRM,
                MediaStore.Audio.Media.DRM_METHOD
            };
            c = context.getContentResolver().query(uri, projection, null, null, null);
            if (c != null && c.moveToNext()) {
                if ("1".equals(c.getString(0))) {
                    profile.isDrm = true;
                    profile.method = c.getInt(1);
                } else {
                    profile.isDrm = false;
                }
            }

            if (c != null) {
                c.close();
            }

        } else if (ContentResolver.SCHEME_FILE.equals(scheme)) { // the uri is file case
            if (isDrmSufix(uri.getLastPathSegment())) {
                profile.isDrm = true;
                profile.method = client.getMethod(uri);
            } else {
                profile.isDrm = false;
            }
        }
        return profile;
    }

    /**
     * Representing the brief DRM info.
     * @hide
     */
    public static class DrmProfile {
        /**
         * Is DRM file or not
         */
        public boolean isDrm;
        /**
         * DRM method
         */
        public int method;
    }
    
    private static boolean isDrmSufix(String filename) {
        Log.v(TAG, "isDrmSufix() : filename: " + filename);
        if (filename == null) {
            return false;
        }
        return filename.toLowerCase().endsWith(".dcf");
    }
}

