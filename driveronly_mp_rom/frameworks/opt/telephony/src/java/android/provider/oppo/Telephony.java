/*
 * Copyright (C) 2006 The Android Open Source Project
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

package android.provider.oppo;

import android.annotation.SdkConstant;
import android.annotation.SdkConstant.SdkConstantType;
import android.content.ContentResolver;
import android.content.ContentValues;
import android.content.Context;
import android.content.Intent;
import android.database.Cursor;
import android.database.sqlite.SqliteWrapper;
import android.net.Uri;
import android.os.Environment;
import android.telephony.SmsMessage;
import android.text.TextUtils;
import android.util.Log;
import android.util.Patterns;


import java.util.HashSet;
import java.util.Set;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

//MTK-START [mtk04070][111121][ALPS00093395]MTK added
import android.content.ContentUris;
import android.database.DatabaseUtils;
//import android.telephony.gemini.GeminiSmsMessage;
//import android.telephony.gemini.GeminiCbSmsMessage;
//import android.telephony.CbSmsMessage;
import android.telephony.OppoTelephonyConstant;
import java.util.ArrayList;
import java.util.List;
//MTK-END [mtk04070][111121][ALPS00093395]MTK added

//#ifdef VENDOR_EDIT
//PangCong@Prd.CommApp.Mms,2012.06.11,Add for
import android.os.RemoteException;
import android.content.OperationApplicationException; 
import android.content.ContentProviderOperation;
import android.content.ContentProviderResult;
//import com.mediatek.common.featureoption.FeatureOption;
//#endif /* VENDOR_EDIT */

//#ifdef VENDOR_EDIT
//ChenYun@Prd.CommApp.Mms, 2012/06/19, Add for sdcard dynamic mounting on 11071&12007&12009.
//import com.oppo.os.OppoEnvironment;
//#endif /* VENDOR_EDIT */

/** @hide */
@android.annotation.OppoHook(level=android.annotation.OppoHook.OppoHookType.NEW_CLASS,
                             property=android.annotation.OppoHook.OppoRomType.ROM,
                             note="provider's class of OPPO")
public final class Telephony {
    private static final String TAG = "Telephony";
    private static final boolean DEBUG = true;
    private static final boolean LOCAL_LOGV = false;
    
    private static final int OPPO_LONG_4600 = 0;//com.mediatek.R.string.oper_long_46000;

    // Constructor
    public Telephony() {
    }

    /**
     * Base columns for tables that contain text based SMSs.
     */
    public interface TextBasedSmsColumns {
        /**
         * The type of the message
         * <P>Type: INTEGER</P>
         */
        public static final String TYPE = "type";

        public static final int MESSAGE_TYPE_ALL    = 0;
        public static final int MESSAGE_TYPE_INBOX  = 1;
        public static final int MESSAGE_TYPE_SENT   = 2;
        public static final int MESSAGE_TYPE_DRAFT  = 3;
        public static final int MESSAGE_TYPE_OUTBOX = 4;
        public static final int MESSAGE_TYPE_FAILED = 5; // for failed outgoing messages
        public static final int MESSAGE_TYPE_QUEUED = 6; // for messages to send later


        /**
         * The thread ID of the message
         * <P>Type: INTEGER</P>
         */
        public static final String THREAD_ID = "thread_id";

        /**
         * The address of the other party
         * <P>Type: TEXT</P>
         */
        public static final String ADDRESS = "address";
		//#ifdef VENDOR_EDIT
		//PangCong@Prd.CommApp.Mms,2012.06.11,Add for
        /**
         * The groupaddress of the other party
         * <P>Type: TEXT</P>
         */
        public static final String GROUPADDRESS = "oppo_groupaddress"; 
		//#endif /* VENDOR_EDIT */
        /**
         * The person ID of the sender
         * <P>Type: INTEGER (long)</P>
         */
        public static final String PERSON_ID = "person";

        /**
         * The date the message was received
         * <P>Type: INTEGER (long)</P>
         */
        public static final String DATE = "date";

        /**
         * The date the message was sent
         * <P>Type: INTEGER (long)</P>
         */
        public static final String DATE_SENT = "date_sent";

        /**
         * Has the message been read
         * <P>Type: INTEGER (boolean)</P>
         */
        public static final String READ = "read";

        /**
         * Indicates whether this message has been seen by the user. The "seen" flag will be
         * used to figure out whether we need to throw up a statusbar notification or not.
         */
        public static final String SEEN = "seen";

        /**
         * The TP-Status value for the message, or -1 if no status has
         * been received
         */
        public static final String STATUS = "status";

        public static final int STATUS_NONE = -1;
        public static final int STATUS_COMPLETE = 0;
        public static final int STATUS_PENDING = 32;
        public static final int STATUS_FAILED = 64;

        /**
         * The subject of the message, if present
         * <P>Type: TEXT</P>
         */
        public static final String SUBJECT = "subject";

        /**
         * The body of the message
         * <P>Type: TEXT</P>
         */
        public static final String BODY = "body";

        /**
         * The id of the sender of the conversation, if present
         * <P>Type: INTEGER (reference to item in content://contacts/people)</P>
         */
        public static final String PERSON = "person";

        /**
         * The protocol identifier code
         * <P>Type: INTEGER</P>
         */
        public static final String PROTOCOL = "protocol";

        /**
         * Whether the <code>TP-Reply-Path</code> bit was set on this message
         * <P>Type: BOOLEAN</P>
         */
        public static final String REPLY_PATH_PRESENT = "reply_path_present";

        /**
         * The service center (SC) through which to send the message, if present
         * <P>Type: TEXT</P>
         */
        public static final String SERVICE_CENTER = "service_center";

        /**
         * Has the message been locked?
         * <P>Type: INTEGER (boolean)</P>
         */
        public static final String LOCKED = "locked";

        /**
         * Error code associated with sending or receiving this message
         * <P>Type: INTEGER</P>
         */
        public static final String ERROR_CODE = "error_code";

        /**
         * Meta data used externally.
         * <P>Type: TEXT</P>
         */
        public static final String META_DATA = "meta_data";

        //MTK-START [mtk04070][111121][ALPS00093395]MTK added
        public static final String SIM_ID = "sim_id";
        //MTK-END [mtk04070][111121][ALPS00093395]MTK added
		
		//#ifdef VENDOR_EDIT  
		//PangCong@Prd.CommApp.Mms,2012.06.11,Add for
        public static final String MSG_ID = "msgid";
        public static final String MASS = "oppo_mass";
		//#endif /* VENDOR_EDIT */
    }

    /**
     * Contains all text based SMS messages.
     */
    public static final class Sms implements BaseColumns, TextBasedSmsColumns {
        public static final Cursor query(ContentResolver cr, String[] projection) {
            return cr.query(CONTENT_URI, projection, null, null, DEFAULT_SORT_ORDER);
        }

        public static final Cursor query(ContentResolver cr, String[] projection,
                String where, String orderBy) {
            return cr.query(CONTENT_URI, projection, where,
                                         null, orderBy == null ? DEFAULT_SORT_ORDER : orderBy);
        }

        /**
         * The content:// style URL for this table
         */
        public static final Uri CONTENT_URI =
            Uri.parse("content://sms");

        /**
         * The default sort order for this table
         */
        public static final String DEFAULT_SORT_ORDER = "date DESC";

        /**
         * Add an SMS to the given URI.
         *
         * @param resolver the content resolver to use
         * @param uri the URI to add the message to
         * @param address the address of the sender
         * @param body the body of the message
         * @param subject the psuedo-subject of the message
         * @param date the timestamp for the message
         * @param read true if the message has been read, false if not
         * @param deliveryReport true if a delivery report was requested, false if not
         * @return the URI for the new message
         */
        public static Uri addMessageToUri(ContentResolver resolver,
                Uri uri, String address, String body, String subject,
                Long date, boolean read, boolean deliveryReport) {
            return addMessageToUri(resolver, uri, address, body, subject,
                    date, read, deliveryReport, -1L);
        }

        //MTK-START [mtk04070][111121][ALPS00093395]MTK added and modified
        /**
         * Add an SMS to the given URI with thread_id specified.
         *
         * @param resolver the content resolver to use
         * @param uri the URI to add the message to
         * @param address the address of the sender
         * @param body the body of the message
         * @param subject the psuedo-subject of the message
         * @param date the timestamp for the message
         * @param read true if the message has been read, false if not
         * @param deliveryReport true if a delivery report was requested, false if not
         * @param threadId the thread_id of the message
         * @return the URI for the new message
         */
        public static Uri addMessageToUri(ContentResolver resolver,
                Uri uri, String address, String body, String subject,
                Long date, boolean read, boolean deliveryReport, long threadId) {

            return addMessageToUri(resolver, uri, address, body, subject,
                    date, read, deliveryReport, threadId, -1);
        }
        //#ifdef VENDOR_EDIT  
        //PangCong@Prd.CommApp.Mms,2012.06.11,Add for
        public static Uri addGroupMessageToUri(ContentResolver resolver,
                Uri uri, String address, String body, String subject,
                Long date, boolean read, boolean deliveryReport, long threadId,String dest) {
            ContentValues values = new ContentValues(8);

            /*at 2011 add by oppo deng xufeilong*/
            values.put(GROUPADDRESS, dest);
            /*end by oppo*/
            
            values.put(ADDRESS, address);
            if (date != null) {
                values.put(DATE, date);
            }
            values.put(READ, read ? Integer.valueOf(1) : Integer.valueOf(0));
            values.put(SUBJECT, subject);
            values.put(BODY, body);
            if (deliveryReport) {
                values.put(STATUS, STATUS_PENDING);
            }
            if (threadId != -1L) {
                values.put(THREAD_ID, threadId);
            }
            
            values.put(MASS, 1);
            return resolver.insert(uri, values);
        }
        
        public static Uri addGroupMessageToUriGemini(ContentResolver resolver,
                Uri uri, String address, String body, String subject,
                Long date, boolean read, boolean deliveryReport, long threadId,String dest, int simId) {
            ContentValues values = new ContentValues(10);

            /*at 2011 add by oppo deng xufeilong*/
            values.put(GROUPADDRESS, dest);
            /*end by oppo*/
            
            values.put(ADDRESS, address);
            if (date != null) {
                values.put(DATE, date);
            }
            values.put(READ, read ? Integer.valueOf(1) : Integer.valueOf(0));
            values.put(SUBJECT, subject);
            values.put(BODY, body);
            if (deliveryReport) {
                values.put(STATUS, STATUS_PENDING);
            }
            if (threadId != -1L) {
                values.put(THREAD_ID, threadId);
            }
            
            values.put(MASS, 1);
            values.put(SIM_ID, simId); // add by oppo xufeilong
            return resolver.insert(uri, values);
        }
        //#endif /* VENDOR_EDIT */

        /**
         * Add an SMS to the given URI with thread_id specified.
         *
         * @param resolver the content resolver to use
         * @param uri the URI to add the message to
         * @param address the address of the sender
         * @param body the body of the message
         * @param subject the psuedo-subject of the message
         * @param date the timestamp for the message
         * @param read true if the message has been read, false if not
         * @param deliveryReport true if a delivery report was requested, false if not
         * @param threadId the thread_id of the message
         * @param simId the sim_id of the message
         * @return the URI for the new message
         */
        public static Uri addMessageToUri(ContentResolver resolver,
                Uri uri, String address, String body, String subject,
                Long date, boolean read, boolean deliveryReport, long threadId, int simId) {
            
            return addMessageToUri(resolver, uri, address, body, subject, null,
                    date, read, deliveryReport, threadId, simId);
        }

        /**
         * Add an SMS to the given URI with thread_id specified.
         *
         * @param resolver the content resolver to use
         * @param uri the URI to add the message to
         * @param address the address of the sender
         * @param body the body of the message
         * @param subject the psuedo-subject of the message
         * @param sc the service center of the message
         * @param date the timestamp for the message
         * @param read true if the message has been read, false if not
         * @param deliveryReport true if a delivery report was requested, false if not
         * @param threadId the thread_id of the message
         * @param simId the sim_id of the message
         * @return the URI for the new message
         */
        public static Uri addMessageToUri(ContentResolver resolver,
                Uri uri, String address, String body, String subject, String sc,
                Long date, boolean read, boolean deliveryReport, long threadId, int simId) {
            ContentValues values = new ContentValues(9);
			//#ifdef VENDOR_EDIT  
			//PangCong@Prd.CommApp.Mms,2012.06.11,Add for
            values.put(GROUPADDRESS, address);
			//#endif /* VENDOR_EDIT */
            values.put(ADDRESS, address);
            if (date != null) {
                values.put(DATE, date);
            }
            if (sc != null) {
                values.put(SERVICE_CENTER, sc);
            }
            values.put(READ, read ? Integer.valueOf(1) : Integer.valueOf(0));
            values.put(SUBJECT, subject);
            values.put(BODY, body);
            values.put(SEEN, read ? Integer.valueOf(1) : Integer.valueOf(0));
            if (deliveryReport) {
                values.put(STATUS, STATUS_PENDING);
            }
            if (threadId != -1L) {
                values.put(THREAD_ID, threadId);
            }

            if (simId != -1) {
                values.put(SIM_ID, simId);
            }
            
            return resolver.insert(uri, values);
        }
        //MTK-END [mtk04070][111121][ALPS00093395]MTK added and modified

        /**
         * Move a message to the given folder.
         *
         * @param context the context to use
         * @param uri the message to move
         * @param folder the folder to move to
         * @return true if the operation succeeded
         */
        public static boolean moveMessageToFolder(Context context,
                Uri uri, int folder, int error) {
            if (uri == null) {
                return false;
            }

            boolean markAsUnread = false;
            boolean markAsRead = false;
            switch(folder) {
            case MESSAGE_TYPE_INBOX:
            case MESSAGE_TYPE_DRAFT:
                break;
            case MESSAGE_TYPE_OUTBOX:
            case MESSAGE_TYPE_SENT:
                markAsRead = true;
                break;
            case MESSAGE_TYPE_FAILED:
            case MESSAGE_TYPE_QUEUED:
                markAsUnread = true;
                break;
            default:
                return false;
            }

            ContentValues values = new ContentValues(3);

            values.put(TYPE, folder);
            if (markAsUnread) {
                values.put(READ, Integer.valueOf(0));
            } else if (markAsRead) {
                values.put(READ, Integer.valueOf(1));
            }
            values.put(ERROR_CODE, error);

            return 1 == SqliteWrapper.update(context, context.getContentResolver(),
                            uri, values, null, null);
        }

        /**
         * Returns true iff the folder (message type) identifies an
         * outgoing message.
         */
        public static boolean isOutgoingFolder(int messageType) {
            return  (messageType == MESSAGE_TYPE_FAILED)
                    || (messageType == MESSAGE_TYPE_OUTBOX)
                    || (messageType == MESSAGE_TYPE_SENT)
                    || (messageType == MESSAGE_TYPE_QUEUED);
        }

        /**
         * Contains all text based SMS messages in the SMS app's inbox.
         */
        public static final class Inbox implements BaseColumns, TextBasedSmsColumns {
            /**
             * The content:// style URL for this table
             */
            public static final Uri CONTENT_URI =
                Uri.parse("content://sms/inbox");

            /**
             * The default sort order for this table
             */
            public static final String DEFAULT_SORT_ORDER = "date DESC";

            /**
             * Add an SMS to the Draft box.
             *
             * @param resolver the content resolver to use
             * @param address the address of the sender
             * @param body the body of the message
             * @param subject the psuedo-subject of the message
             * @param date the timestamp for the message
             * @param read true if the message has been read, false if not
             * @return the URI for the new message
             */
            public static Uri addMessage(ContentResolver resolver,
                    String address, String body, String subject, Long date,
                    boolean read) {
                return addMessageToUri(resolver, CONTENT_URI, address, body,
                        subject, date, read, false);
            }

            //MTK-START [mtk04070][111121][ALPS00093395]MTK added
            public static Uri addMessage(ContentResolver resolver,
                    String address, String body, String subject, String sc, Long date,
                    boolean read) {
                return addMessageToUri(resolver, CONTENT_URI, address, body,
                        subject, sc, date, read, false, -1L, -1);
            }

             /**
             * Add an SMS to the Inbox.
             *
             * @param resolver the content resolver to use
             * @param address the address of the sender
             * @param body the body of the message
             * @param subject the psuedo-subject of the message
             * @param date the timestamp for the message
             * @param read true if the message has been read, false if not
             * @param simId the sim_id of the message
             * @return the URI for the new message
             */
            public static Uri addMessage(ContentResolver resolver,
                    String address, String body, String subject, Long date,
                    boolean read, int simId) {
                return addMessageToUri(resolver, CONTENT_URI, address, body,
                        subject, date, read, false, -1L, simId);
            }

            public static Uri addMessage(ContentResolver resolver,
                    String address, String body, String subject, String sc, Long date,
                    boolean read, int simId) {
                return addMessageToUri(resolver, CONTENT_URI, address, body,
                        subject, sc, date, read, false, -1L, simId);
            }
            //MTK-END [mtk04070][111121][ALPS00093395]MTK added
        }

        /**
         * Contains all sent text based SMS messages in the SMS app's.
         */
        public static final class Sent implements BaseColumns, TextBasedSmsColumns {
            /**
             * The content:// style URL for this table
             */
            public static final Uri CONTENT_URI =
                    Uri.parse("content://sms/sent");

            /**
             * The default sort order for this table
             */
            public static final String DEFAULT_SORT_ORDER = "date DESC";

            /**
             * Add an SMS to the Draft box.
             *
             * @param resolver the content resolver to use
             * @param address the address of the sender
             * @param body the body of the message
             * @param subject the psuedo-subject of the message
             * @param date the timestamp for the message
             * @return the URI for the new message
             */
            public static Uri addMessage(ContentResolver resolver,
                    String address, String body, String subject, Long date) {
                return addMessageToUri(resolver, CONTENT_URI, address, body,
                        subject, date, true, false);
            }

            //MTK-START [mtk04070][111121][ALPS00093395]MTK added
            public static Uri addMessage(ContentResolver resolver,
                    String address, String body, String subject, String sc, Long date) {
                return addMessageToUri(resolver, CONTENT_URI, address, body,
                        subject, sc, date, true, false, -1L, -1);
            }

            /**
             * Add an SMS to the sent box.
             *
             * @param resolver the content resolver to use
             * @param address the address of the sender
             * @param body the body of the message
             * @param subject the psuedo-subject of the message
             * @param date the timestamp for the message
             * @param simId the sim_id of the message             
             * @return the URI for the new message
             */
            public static Uri addMessage(ContentResolver resolver,
                    String address, String body, String subject, Long date, int simId) {
                return addMessageToUri(resolver, CONTENT_URI, address, body,
                        subject, date, true, false, -1L, simId);
            }

            public static Uri addMessage(ContentResolver resolver,
                    String address, String body, String subject, String sc, Long date, int simId) {
                return addMessageToUri(resolver, CONTENT_URI, address, body,
                        subject, sc, date, true, false, -1L, simId);
            }
            //MTK-END [mtk04070][111121][ALPS00093395]MTK added
        }

        /**
         * Contains all sent text based SMS messages in the SMS app's.
         */
        public static final class Draft implements BaseColumns, TextBasedSmsColumns {
            /**
             * The content:// style URL for this table
             */
            public static final Uri CONTENT_URI =
                    Uri.parse("content://sms/draft");

            /**
             * The default sort order for this table
             */
            public static final String DEFAULT_SORT_ORDER = "date DESC";

            /**
             * Add an SMS to the Draft box.
             *
             * @param resolver the content resolver to use
             * @param address the address of the sender
             * @param body the body of the message
             * @param subject the psuedo-subject of the message
             * @param date the timestamp for the message
             * @return the URI for the new message
             */
            public static Uri addMessage(ContentResolver resolver,
                    String address, String body, String subject, Long date) {
                return addMessageToUri(resolver, CONTENT_URI, address, body,
                        subject, date, true, false);
            }

            //MTK-START [mtk04070][111121][ALPS00093395]MTK added
            /**
             * Add an SMS to the Draft box.
             *
             * @param resolver the content resolver to use
             * @param address the address of the sender
             * @param body the body of the message
             * @param subject the psuedo-subject of the message
             * @param date the timestamp for the message
             * @param simId the sim_id of the message         
             * @return the URI for the new message
             */
            public static Uri addMessage(ContentResolver resolver,
                    String address, String body, String subject, Long date, int simId) {
                return addMessageToUri(resolver, CONTENT_URI, address, body,
                        subject, date, true, false, -1L, simId);
            }
            //MTK-END [mtk04070][111121][ALPS00093395]MTK added

            /**
             * Save over an existing draft message.
             *
             * @param resolver the content resolver to use
             * @param uri of existing message
             * @param body the new body for the draft message
             * @return true is successful, false otherwise
             */
            public static boolean saveMessage(ContentResolver resolver,
                    Uri uri, String body) {
                ContentValues values = new ContentValues(2);
                values.put(BODY, body);
                values.put(DATE, System.currentTimeMillis());
                return resolver.update(uri, values, null, null) == 1;
            }
        }
		
        //#ifdef VENDOR_EDIT  
        //PangCong@Prd.CommApp.Mms,2012.06.11,Add for
        /**
         * Contains all sent text based SMS messages in the SMS app's.
         */
        public static final class GroupMsg implements BaseColumns, TextBasedSmsColumns {
            /**
             * The content:// style URL for this table
             */
            public static final Uri CONTENT_URI =
                    Uri.parse("content://sms/groupmsg");

            /**
             * The default sort order for this table
             */
            public static final String DEFAULT_SORT_ORDER = "date DESC";

            /**
             * Add an SMS to the Draft box.
             *
             * @param resolver the content resolver to use
             * @param address the address of the sender
             * @param body the body of the message
             * @param subject the psuedo-subject of the message
             * @param date the timestamp for the message
             * @return the URI for the new message
             */
            public static Uri addMessage(ContentResolver resolver,
                    String[] address, int vMsgId, int vType, long threadId)
            {
                ArrayList<ContentProviderOperation> operationList = new ArrayList<ContentProviderOperation>(); 
                ContentProviderOperation.Builder builder = null;     

                for(int i=0;i<address.length;i++)        
                {
                    builder = ContentProviderOperation.newInsert(CONTENT_URI);
                    builder.withValue(MSG_ID, vMsgId);  
                    builder.withValue(ADDRESS, address[i]); 
                    builder.withValue(TYPE, vType);
                    builder.withValue(THREAD_ID, threadId);
                    operationList.add(builder.build());
                }

                try        
                {
                    ContentProviderResult[] results = resolver.applyBatch(
                        "sms", operationList);        
                }
                catch(RemoteException e)
                {
                    Log.d(TAG,"oppoa=="+e);
                }
                catch(OperationApplicationException e)
                {
                    Log.d(TAG,"oppob=="+e);
                }
            
                return null;
            }
            
            /**
             * Add an SMS to the Draft box.
             *
             * @param resolver the content resolver to use
             * @param address the address of the sender
             * @param body the body of the message
             * @param subject the psuedo-subject of the message
             * @param date the timestamp for the message
             * @param simId the simid for the message
             * @return the URI for the new message
             */
            public static Uri addMessageGemini(ContentResolver resolver,
                    String[] address, int vMsgId, int vType, long threadId, int simId)
            {
                ArrayList<ContentProviderOperation> operationList = new ArrayList<ContentProviderOperation>(); 
                ContentProviderOperation.Builder builder = null;     

                for(int i=0;i<address.length;i++)        
                {
                    builder = ContentProviderOperation.newInsert(CONTENT_URI);
                    builder.withValue(MSG_ID, vMsgId);  
                    builder.withValue(ADDRESS, address[i]); 
                    builder.withValue(TYPE, vType);
                    builder.withValue(THREAD_ID, threadId);
                    builder.withValue(SIM_ID, simId);//add by oppo xufeilong
                    operationList.add(builder.build());
                }

                try        
                {
                    ContentProviderResult[] results = resolver.applyBatch(
                        "sms", operationList);        
                }
                catch(RemoteException e)
                {
                    Log.d(TAG,"oppoa=="+e);
                }
                catch(OperationApplicationException e)
                {
                    Log.d(TAG,"oppob=="+e);
                }
            
                return null;
            }
        }
        //#endif /* VENDOR_EDIT */

        /**
         * Contains all pending outgoing text based SMS messages.
         */
        public static final class Outbox implements BaseColumns, TextBasedSmsColumns {
            /**
             * The content:// style URL for this table
             */
            public static final Uri CONTENT_URI =
                Uri.parse("content://sms/outbox");

            /**
             * The default sort order for this table
             */
            public static final String DEFAULT_SORT_ORDER = "date DESC";

            /**
             * Add an SMS to the Out box.
             *
             * @param resolver the content resolver to use
             * @param address the address of the sender
             * @param body the body of the message
             * @param subject the psuedo-subject of the message
             * @param date the timestamp for the message
             * @param deliveryReport whether a delivery report was requested for the message
             * @return the URI for the new message
             */
            public static Uri addMessage(ContentResolver resolver,
                    String address, String body, String subject, Long date,
                    boolean deliveryReport, long threadId) {
                return addMessageToUri(resolver, CONTENT_URI, address, body,
                        subject, date, true, deliveryReport, threadId);
            }
			//#ifdef VENDOR_EDIT  
			//PangCong@Prd.CommApp.Mms,2012.06.11,Add for
            public static Uri addGroupMessage(ContentResolver resolver,
                    String address, String body, String subject, Long date,
                    boolean deliveryReport, long threadId, String dest) {
                return addGroupMessageToUri(resolver, CONTENT_URI, address, body,
                        subject, date, true, deliveryReport, threadId, dest);
            }
            //#endif /* VENDOR_EDIT */

            //MTK-START [mtk04070][111121][ALPS00093395]MTK added
            /**
             * Add an SMS to the Out box.
             *
             * @param resolver the content resolver to use
             * @param address the address of the sender
             * @param body the body of the message
             * @param subject the psuedo-subject of the message
             * @param date the timestamp for the message
             * @param deliveryReport whether a delivery report was requested for the message
             * @param simId the sim_id of the message         
             * @return the URI for the new message
             */
            public static Uri addMessage(ContentResolver resolver,
                    String address, String body, String subject, Long date,
                    boolean deliveryReport, long threadId, int simId) {
                return addMessageToUri(resolver, CONTENT_URI, address, body,
                        subject, date, true, deliveryReport, threadId, simId);
            }
            //MTK-END [mtk04070][111121][ALPS00093395]MTK added
        }

        /**
         * Contains all sent text-based SMS messages in the SMS app's.
         */
        public static final class Conversations
                implements BaseColumns, TextBasedSmsColumns {
            /**
             * The content:// style URL for this table
             */
            public static final Uri CONTENT_URI =
                Uri.parse("content://sms/conversations");

            /**
             * The default sort order for this table
             */
            public static final String DEFAULT_SORT_ORDER = "date DESC";

            /**
             * The first 45 characters of the body of the message
             * <P>Type: TEXT</P>
             */
            public static final String SNIPPET = "snippet";

            /**
             * The number of messages in the conversation
             * <P>Type: INTEGER</P>
             */
            public static final String MESSAGE_COUNT = "msg_count";
        }

        /**
         * Contains info about SMS related Intents that are broadcast.
         */
        public static final class Intents {
            /**
             * Set by BroadcastReceiver. Indicates the message was handled
             * successfully.
             */
            public static final int RESULT_SMS_HANDLED = 1;

            /**
             * Set by BroadcastReceiver. Indicates a generic error while
             * processing the message.
             */
            public static final int RESULT_SMS_GENERIC_ERROR = 2;

            /**
             * Set by BroadcastReceiver. Indicates insufficient memory to store
             * the message.
             */
            public static final int RESULT_SMS_OUT_OF_MEMORY = 3;

            /**
             * Set by BroadcastReceiver. Indicates the message, while
             * possibly valid, is of a format or encoding that is not
             * supported.
             */
            public static final int RESULT_SMS_UNSUPPORTED = 4;

            /**
             * Broadcast Action: A new text based SMS message has been received
             * by the device. The intent will have the following extra
             * values:</p>
             *
             * <ul>
             *   <li><em>pdus</em> - An Object[] od byte[]s containing the PDUs
             *   that make up the message.</li>
             * </ul>
             *
             * <p>The extra values can be extracted using
             * {@link #getMessagesFromIntent(Intent)}.</p>
             *
             * <p>If a BroadcastReceiver encounters an error while processing
             * this intent it should set the result code appropriately.</p>
             */
            @SdkConstant(SdkConstantType.BROADCAST_INTENT_ACTION)
            public static final String SMS_RECEIVED_ACTION =
                    "android.provider.Telephony.SMS_RECEIVED";

            /**
             * Broadcast Action: A new data based SMS message has been received
             * by the device. The intent will have the following extra
             * values:</p>
             *
             * <ul>
             *   <li><em>pdus</em> - An Object[] of byte[]s containing the PDUs
             *   that make up the message.</li>
             * </ul>
             *
             * <p>The extra values can be extracted using
             * {@link #getMessagesFromIntent(Intent)}.</p>
             *
             * <p>If a BroadcastReceiver encounters an error while processing
             * this intent it should set the result code appropriately.</p>
             */
            @SdkConstant(SdkConstantType.BROADCAST_INTENT_ACTION)
            public static final String DATA_SMS_RECEIVED_ACTION =
                    "android.intent.action.DATA_SMS_RECEIVED";

            /**
             * Broadcast Action: A new WAP PUSH message has been received by the
             * device. The intent will have the following extra
             * values:</p>
             *
             * <ul>
             *   <li><em>transactionId (Integer)</em> - The WAP transaction ID</li>
             *   <li><em>pduType (Integer)</em> - The WAP PDU type</li>
             *   <li><em>header (byte[])</em> - The header of the message</li>
             *   <li><em>data (byte[])</em> - The data payload of the message</li>
             *   <li><em>contentTypeParameters (HashMap&lt;String,String&gt;)</em>
             *   - Any parameters associated with the content type
             *   (decoded from the WSP Content-Type header)</li>
             * </ul>
             *
             * <p>If a BroadcastReceiver encounters an error while processing
             * this intent it should set the result code appropriately.</p>
             *
             * <p>The contentTypeParameters extra value is map of content parameters keyed by
             * their names.</p>
             *
             * <p>If any unassigned well-known parameters are encountered, the key of the map will
             * be 'unassigned/0x...', where '...' is the hex value of the unassigned parameter.  If
             * a parameter has No-Value the value in the map will be null.</p>
             */
            @SdkConstant(SdkConstantType.BROADCAST_INTENT_ACTION)
            public static final String WAP_PUSH_RECEIVED_ACTION =
                    "android.provider.Telephony.WAP_PUSH_RECEIVED";

            /**
             * Broadcast Action: A new Cell Broadcast message has been received
             * by the device. The intent will have the following extra
             * values:</p>
             *
             * <ul>
             *   <li><em>message</em> - An SmsCbMessage object containing the broadcast message
             *   data. This is not an emergency alert, so ETWS and CMAS data will be null.</li>
             * </ul>
             *
             * <p>The extra values can be extracted using
             * {@link #getMessagesFromIntent(Intent)}.</p>
             *
             * <p>If a BroadcastReceiver encounters an error while processing
             * this intent it should set the result code appropriately.</p>
             */
            @SdkConstant(SdkConstantType.BROADCAST_INTENT_ACTION)
            public static final String SMS_CB_RECEIVED_ACTION =
                    "android.provider.Telephony.SMS_CB_RECEIVED";

            /**
             * Broadcast Action: A new Emergency Broadcast message has been received
             * by the device. The intent will have the following extra
             * values:</p>
             *
             * <ul>
             *   <li><em>message</em> - An SmsCbMessage object containing the broadcast message
             *   data, including ETWS or CMAS warning notification info if present.</li>
             * </ul>
             *
             * <p>The extra values can be extracted using
             * {@link #getMessagesFromIntent(Intent)}.</p>
             *
             * <p>If a BroadcastReceiver encounters an error while processing
             * this intent it should set the result code appropriately.</p>
             */
            @SdkConstant(SdkConstantType.BROADCAST_INTENT_ACTION)
            public static final String SMS_EMERGENCY_CB_RECEIVED_ACTION =
                    "android.provider.Telephony.SMS_EMERGENCY_CB_RECEIVED";

            /**
             * Broadcast Action: A new CDMA SMS has been received containing Service Category
             * Program Data (updates the list of enabled broadcast channels). The intent will
             * have the following extra values:</p>
             *
             * <ul>
             *   <li><em>operations</em> - An array of CdmaSmsCbProgramData objects containing
             *   the service category operations (add/delete/clear) to perform.</li>
             * </ul>
             *
             * <p>The extra values can be extracted using
             * {@link #getMessagesFromIntent(Intent)}.</p>
             *
             * <p>If a BroadcastReceiver encounters an error while processing
             * this intent it should set the result code appropriately.</p>
             */
            @SdkConstant(SdkConstantType.BROADCAST_INTENT_ACTION)
            public static final String SMS_SERVICE_CATEGORY_PROGRAM_DATA_RECEIVED_ACTION =
                    "android.provider.Telephony.SMS_SERVICE_CATEGORY_PROGRAM_DATA_RECEIVED";

            /**
             * Broadcast Action: The SIM storage for SMS messages is full.  If
             * space is not freed, messages targeted for the SIM (class 2) may
             * not be saved.
             */
            @SdkConstant(SdkConstantType.BROADCAST_INTENT_ACTION)
            public static final String SIM_FULL_ACTION =
                    "android.provider.Telephony.SIM_FULL";

            /**
             * Broadcast Action: An incoming SMS has been rejected by the
             * telephony framework.  This intent is sent in lieu of any
             * of the RECEIVED_ACTION intents.  The intent will have the
             * following extra value:</p>
             *
             * <ul>
             *   <li><em>result</em> - An int result code, eg,
             *   <code>{@link #RESULT_SMS_OUT_OF_MEMORY}</code>,
             *   indicating the error returned to the network.</li>
             * </ul>

             */
            @SdkConstant(SdkConstantType.BROADCAST_INTENT_ACTION)
            public static final String SMS_REJECTED_ACTION =
                "android.provider.Telephony.SMS_REJECTED";

            //MTK-START [mtk04070][111121][ALPS00093395]MTK added
            /**
             * Broadcast Action: The SMS sub-system in the modem is ready.
             * The intent is sent to inform the APP if the SMS sub-system
             * is ready or not. The intent will have the following extra value:</p>
             *
             * <ul>
             *   <li><em>ready</em> - An boolean result code, true for ready</li>
             * </ul>
             */
            @SdkConstant(SdkConstantType.BROADCAST_INTENT_ACTION)
            public static final String SMS_STATE_CHANGED_ACTION =
                "android.provider.Telephony.SMS_STATE_CHANGED";

            /**
             * Broadcast Action: This intent is similar to DATA_SMS_RECEIVED_ACTION 
             * except that we will use this intent only when the SMS comes from 
             * OA: 10654040 to port number: 16998 for CMCC DM
             *
             * The intent will have the following extra values:</p>
             *
             * <ul>
             *   <li><em>pdus</em> - An Object[] od byte[]s containing the PDUs
             *   that make up the message.</li>
             * </ul>
             *
             * <p>The extra values can be extracted using
             * {@link #getMessagesFromIntent(Intent)}.</p>
             *
             * <p>If a BroadcastReceiver encounters an error while processing
             * this intent it should set the result code appropriately.</p>
             */
            @SdkConstant(SdkConstantType.BROADCAST_INTENT_ACTION)
            public static final String DM_REGISTER_SMS_RECEIVED_ACTION =
                    "android.intent.action.DM_REGISTER_SMS_RECEIVED";
            //MTK-END [mtk04070][111121][ALPS00093395]MTK added

            /**
             * Broadcast Action: A new SMS message has been received
             * by the device, which contains specail message indication,
             * defined in 23.040 9.2.3.24.2
             *
             * @hide
             */
            public static final String MWI_SMS_RECEIVED_ACTION =
                    "android.provider.Telephony.MWI_SMS_RECEIVED";

            /**
             * Read the PDUs out of an {@link #SMS_RECEIVED_ACTION} or a
             * {@link #DATA_SMS_RECEIVED_ACTION} intent.
             *
             * @param intent the intent to read from
             * @return an array of SmsMessages for the PDUs
             */
            public static SmsMessage[] getMessagesFromIntent(
                    Intent intent) {
                Object[] messages = (Object[]) intent.getSerializableExtra("pdus");
                String format = intent.getStringExtra("format");
                byte[][] pduObjs = new byte[messages.length][];

                for (int i = 0; i < messages.length; i++) {
                    pduObjs[i] = (byte[]) messages[i];
                }
                byte[][] pdus = new byte[pduObjs.length][];
                int pduCount = pdus.length;
                SmsMessage[] msgs = new SmsMessage[pduCount];
                for (int i = 0; i < pduCount; i++) {
                    pdus[i] = pduObjs[i];
                    msgs[i] = SmsMessage.createFromPdu(pdus[i], format);
                }
                return msgs;
            }
        }
    }

    //MTK-START [mtk04070][111121][ALPS00093395]MTK added
    /**
     * Base columns for tables that contain text based CbSMSs.
     */
    public interface TextBasedCbSmsColumns {

        /**
         * The SIM ID which indicated which SIM the CbSMS comes from
         * Reference to Telephony.SIMx
         * <P>Type: INTEGER</P>
         */
        public static final String SIM_ID = "sim_id";

        /**
         * The channel ID of the message
         * which is the message identifier defined in the Spec. 3GPP TS 23.041
         * <P>Type: INTEGER</P>
         */
        public static final String CHANNEL_ID = "channel_id";

        /**
         * The date the message was sent
         * <P>Type: INTEGER (long)</P>
         */
        public static final String DATE = "date";

        /**
         * Has the message been read
         * <P>Type: INTEGER (boolean)</P>
         */
        public static final String READ = "read";

        /**
         * The body of the message
         * <P>Type: TEXT</P>
         */
        public static final String BODY = "body";
        
        /**
         * The thread id of the message
         * <P>Type: INTEGER</P>
         */
        public static final String THREAD_ID = "thread_id";

        /**
         * Indicates whether this message has been seen by the user. The "seen" flag will be
         * used to figure out whether we need to throw up a statusbar notification or not.
         */
        public static final String SEEN = "seen";

        /**
         * Has the message been locked?
         * <P>Type: INTEGER (boolean)</P>
         */
        public static final String LOCKED = "locked";
    }

    public static final class CbSms implements BaseColumns, TextBasedCbSmsColumns {
        
        public static final Cursor query(ContentResolver cr, String[] projection) {
            return cr.query(CONTENT_URI, projection, null, null, DEFAULT_SORT_ORDER);
        }

        public static final Cursor query(ContentResolver cr, String[] projection,
                String where, String orderBy) {
            return cr.query(CONTENT_URI, projection, where,
                                         null, orderBy == null ? DEFAULT_SORT_ORDER : orderBy);
        }

        /**
         * The content:// style URL for this table
         */
        public static final Uri CONTENT_URI =
            Uri.parse("content://cb/messages");
        
        /**
         * The content:// style URL for "canonical_addresses" table
         */
        public static final Uri ADDRESS_URI = Uri.parse("content://cb/addresses");

        /**
         * The default sort order for this table
         */
        public static final String DEFAULT_SORT_ORDER = "date DESC";

        /**
         * Add an SMS to the given URI with thread_id specified.
         *
         * @param resolver the content resolver to use
         * @param uri the URI to add the message to
         * @param sim_id the id of the SIM card
         * @param channel_id the message identifier of the CB message         
         * @param date the timestamp for the message
         * @param read true if the message has been read, false if not
         * @param body the body of the message
         * @return the URI for the new message
         */
        public static Uri addMessageToUri(ContentResolver resolver,
                Uri uri, int sim_id, int channel_id, long date, 
                boolean read, String body) {
            ContentValues values = new ContentValues(5);

            values.put(SIM_ID, Integer.valueOf(sim_id));
            values.put(DATE, Long.valueOf(date));
            values.put(READ, read ? Integer.valueOf(1) : Integer.valueOf(0));
            values.put(BODY, body);          
            values.put(CHANNEL_ID, Integer.valueOf(channel_id));
            
            return resolver.insert(uri, values);
        }

        /**
         * Contains all received CbSMS messages in the SMS app's.
         */
        public static final class Conversations
                implements BaseColumns, TextBasedCbSmsColumns {
            /**
             * The content:// style URL for this table
             */
            public static final Uri CONTENT_URI =
                Uri.parse("content://cb/threads");

            /**
             * The default sort order for this table
             */
            public static final String DEFAULT_SORT_ORDER = "date DESC";

            /**
             * The first 45 characters of the body of the message
             * <P>Type: TEXT</P>
             */
            public static final String SNIPPET = "snippet";

            /**
             * The number of messages in the conversation
             * <P>Type: INTEGER</P>
             */
            public static final String MESSAGE_COUNT = "msg_count";
            
            /**
             * The _id of address table in the conversation
             * <P>Type: INTEGER</P>
             */
            public static final String ADDRESS_ID = "address_id";
        }

            /**
             * Columns for the "canonical_addresses" table used by CB-SMS
             */
        public interface CanonicalAddressesColumns extends BaseColumns {
            /**
             * An address used in CB-SMS. Just a channel number
             * <P>Type: TEXT</P>
             */
            public static final String ADDRESS = "address";
        }

            /**
             * Columns for the "canonical_addresses" table used by CB-SMS
             */
        public static final class CbChannel implements BaseColumns {
            /**
             * The content:// style URL for this table
             */
            public static final Uri CONTENT_URI =
                Uri.parse("content://cb/channel");

            public static final String NAME = "name";
      
            public static final String NUMBER = "number";

            public static final String ENABLE = "enable";
            
        }
        // TODO open when using CB Message
        public static final class Intents {
           
            /**
             * Broadcast Action: A new cell broadcast sms message has been received
             * by the device. The intent will have the following extra
             * values:</p>
             *
             * <ul>
             *   <li><em>pdus</em> - An Object[] od byte[]s containing the PDUs
             *   that make up the message.</li>
             * </ul>
             *
             * <p>The extra values can be extracted using
             * {@link #getMessagesFromIntent(Intent)}.</p>
             */
            @SdkConstant(SdkConstantType.BROADCAST_INTENT_ACTION)
            public static final String CB_SMS_RECEIVED_ACTION =
                    "android.provider.Telephony.CB_SMS_RECEIVED";                        

            /** 
             * Read the PDUs out of an {@link #CB_SMS_RECEIVED_ACTION}.
             *
             * @param intent the intent to read from
             * @return an array of CbSmsMessages for the PDUs
			 * @hide
            */ 
/*
            public static final CbSmsMessage[] getMessagesFromIntent(
                    Intent intent) {
                Object[] messages = (Object[]) intent.getSerializableExtra("pdus");
                if (messages == null) {
                    return null;
                }
                
                CbSmsMessage[] msgs = new CbSmsMessage[messages.length];
                int simId = intent.getIntExtra(OppoTelephonyConstant.GEMINI_SIM_ID_KEY, -1);
                
                Log.d(TAG, "Get CbSmeMessage ID: " + simId);

                
                for (int i = 0; i < messages.length; i++) {      
                    msgs[i] = GeminiCbSmsMessage.createFromPdu((byte[]) messages[i], simId);
                }
                return msgs;
            }
*/            
        }
    }
    //MTK-END [mtk04070][111121][ALPS00093395]MTK added


    /**
     * Base columns for tables that contain MMSs.
     */
    public interface BaseMmsColumns extends BaseColumns {

        public static final int MESSAGE_BOX_ALL    = 0;
        public static final int MESSAGE_BOX_INBOX  = 1;
        public static final int MESSAGE_BOX_SENT   = 2;
        public static final int MESSAGE_BOX_DRAFTS = 3;
        public static final int MESSAGE_BOX_OUTBOX = 4;

        /**
         * The date the message was received.
         * <P>Type: INTEGER (long)</P>
         */
        public static final String DATE = "date";

        /**
         * The date the message was sent.
         * <P>Type: INTEGER (long)</P>
         */
        public static final String DATE_SENT = "date_sent";

        /**
         * The box which the message belong to, for example, MESSAGE_BOX_INBOX.
         * <P>Type: INTEGER</P>
         */
        public static final String MESSAGE_BOX = "msg_box";

        /**
         * Has the message been read.
         * <P>Type: INTEGER (boolean)</P>
         */
        public static final String READ = "read";

        /**
         * Indicates whether this message has been seen by the user. The "seen" flag will be
         * used to figure out whether we need to throw up a statusbar notification or not.
         */
        public static final String SEEN = "seen";

        /**
         * The Message-ID of the message.
         * <P>Type: TEXT</P>
         */
        public static final String MESSAGE_ID = "m_id";

        /**
         * The subject of the message, if present.
         * <P>Type: TEXT</P>
         */
        public static final String SUBJECT = "sub";

        /**
         * The character set of the subject, if present.
         * <P>Type: INTEGER</P>
         */
        public static final String SUBJECT_CHARSET = "sub_cs";

        /**
         * The Content-Type of the message.
         * <P>Type: TEXT</P>
         */
        public static final String CONTENT_TYPE = "ct_t";

        /**
         * The Content-Location of the message.
         * <P>Type: TEXT</P>
         */
        public static final String CONTENT_LOCATION = "ct_l";

        /**
         * The address of the sender.
         * <P>Type: TEXT</P>
         */
        public static final String FROM = "from";

        /**
         * The address of the recipients.
         * <P>Type: TEXT</P>
         */
        public static final String TO = "to";

        /**
         * The address of the cc. recipients.
         * <P>Type: TEXT</P>
         */
        public static final String CC = "cc";

        /**
         * The address of the bcc. recipients.
         * <P>Type: TEXT</P>
         */
        public static final String BCC = "bcc";

        /**
         * The expiry time of the message.
         * <P>Type: INTEGER</P>
         */
        public static final String EXPIRY = "exp";

        /**
         * The class of the message.
         * <P>Type: TEXT</P>
         */
        public static final String MESSAGE_CLASS = "m_cls";

        /**
         * The type of the message defined by MMS spec.
         * <P>Type: INTEGER</P>
         */
        public static final String MESSAGE_TYPE = "m_type";

        /**
         * The version of specification that this message conform.
         * <P>Type: INTEGER</P>
         */
        public static final String MMS_VERSION = "v";

        /**
         * The size of the message.
         * <P>Type: INTEGER</P>
         */
        public static final String MESSAGE_SIZE = "m_size";

        /**
         * The priority of the message.
         * <P>Type: TEXT</P>
         */
        public static final String PRIORITY = "pri";

        /**
         * The read-report of the message.
         * <P>Type: TEXT</P>
         */
        public static final String READ_REPORT = "rr";

        /**
         * Whether the report is allowed.
         * <P>Type: TEXT</P>
         */
        public static final String REPORT_ALLOWED = "rpt_a";

        /**
         * The response-status of the message.
         * <P>Type: INTEGER</P>
         */
        public static final String RESPONSE_STATUS = "resp_st";

        /**
         * The status of the message.
         * <P>Type: INTEGER</P>
         */
        public static final String STATUS = "st";

        /**
         * The transaction-id of the message.
         * <P>Type: TEXT</P>
         */
        public static final String TRANSACTION_ID = "tr_id";

        /**
         * The retrieve-status of the message.
         * <P>Type: INTEGER</P>
         */
        public static final String RETRIEVE_STATUS = "retr_st";

        /**
         * The retrieve-text of the message.
         * <P>Type: TEXT</P>
         */
        public static final String RETRIEVE_TEXT = "retr_txt";

        /**
         * The character set of the retrieve-text.
         * <P>Type: TEXT</P>
         */
        public static final String RETRIEVE_TEXT_CHARSET = "retr_txt_cs";

        /**
         * The read-status of the message.
         * <P>Type: INTEGER</P>
         */
        public static final String READ_STATUS = "read_status";

        /**
         * The content-class of the message.
         * <P>Type: INTEGER</P>
         */
        public static final String CONTENT_CLASS = "ct_cls";

        /**
         * The delivery-report of the message.
         * <P>Type: INTEGER</P>
         */
        public static final String DELIVERY_REPORT = "d_rpt";

        /**
         * The delivery-time-token of the message.
         * <P>Type: INTEGER</P>
         */
        public static final String DELIVERY_TIME_TOKEN = "d_tm_tok";

        /**
         * The delivery-time of the message.
         * <P>Type: INTEGER</P>
         */
        public static final String DELIVERY_TIME = "d_tm";

        /**
         * The response-text of the message.
         * <P>Type: TEXT</P>
         */
        public static final String RESPONSE_TEXT = "resp_txt";

        /**
         * The sender-visibility of the message.
         * <P>Type: TEXT</P>
         */
        public static final String SENDER_VISIBILITY = "s_vis";

        /**
         * The reply-charging of the message.
         * <P>Type: INTEGER</P>
         */
        public static final String REPLY_CHARGING = "r_chg";

        /**
         * The reply-charging-deadline-token of the message.
         * <P>Type: INTEGER</P>
         */
        public static final String REPLY_CHARGING_DEADLINE_TOKEN = "r_chg_dl_tok";

        /**
         * The reply-charging-deadline of the message.
         * <P>Type: INTEGER</P>
         */
        public static final String REPLY_CHARGING_DEADLINE = "r_chg_dl";

        /**
         * The reply-charging-id of the message.
         * <P>Type: TEXT</P>
         */
        public static final String REPLY_CHARGING_ID = "r_chg_id";

        /**
         * The reply-charging-size of the message.
         * <P>Type: INTEGER</P>
         */
        public static final String REPLY_CHARGING_SIZE = "r_chg_sz";

        /**
         * The previously-sent-by of the message.
         * <P>Type: TEXT</P>
         */
        public static final String PREVIOUSLY_SENT_BY = "p_s_by";

        /**
         * The previously-sent-date of the message.
         * <P>Type: INTEGER</P>
         */
        public static final String PREVIOUSLY_SENT_DATE = "p_s_d";

        /**
         * The store of the message.
         * <P>Type: TEXT</P>
         */
        public static final String STORE = "store";

        /**
         * The mm-state of the message.
         * <P>Type: INTEGER</P>
         */
        public static final String MM_STATE = "mm_st";

        /**
         * The mm-flags-token of the message.
         * <P>Type: INTEGER</P>
         */
        public static final String MM_FLAGS_TOKEN = "mm_flg_tok";

        /**
         * The mm-flags of the message.
         * <P>Type: TEXT</P>
         */
        public static final String MM_FLAGS = "mm_flg";

        /**
         * The store-status of the message.
         * <P>Type: TEXT</P>
         */
        public static final String STORE_STATUS = "store_st";

        /**
         * The store-status-text of the message.
         * <P>Type: TEXT</P>
         */
        public static final String STORE_STATUS_TEXT = "store_st_txt";

        /**
         * The stored of the message.
         * <P>Type: TEXT</P>
         */
        public static final String STORED = "stored";

        /**
         * The totals of the message.
         * <P>Type: TEXT</P>
         */
        public static final String TOTALS = "totals";

        /**
         * The mbox-totals of the message.
         * <P>Type: TEXT</P>
         */
        public static final String MBOX_TOTALS = "mb_t";

        /**
         * The mbox-totals-token of the message.
         * <P>Type: INTEGER</P>
         */
        public static final String MBOX_TOTALS_TOKEN = "mb_t_tok";

        /**
         * The quotas of the message.
         * <P>Type: TEXT</P>
         */
        public static final String QUOTAS = "qt";

        /**
         * The mbox-quotas of the message.
         * <P>Type: TEXT</P>
         */
        public static final String MBOX_QUOTAS = "mb_qt";

        /**
         * The mbox-quotas-token of the message.
         * <P>Type: INTEGER</P>
         */
        public static final String MBOX_QUOTAS_TOKEN = "mb_qt_tok";

        /**
         * The message-count of the message.
         * <P>Type: INTEGER</P>
         */
        public static final String MESSAGE_COUNT = "m_cnt";

        /**
         * The start of the message.
         * <P>Type: INTEGER</P>
         */
        public static final String START = "start";

        /**
         * The distribution-indicator of the message.
         * <P>Type: TEXT</P>
         */
        public static final String DISTRIBUTION_INDICATOR = "d_ind";

        /**
         * The element-descriptor of the message.
         * <P>Type: TEXT</P>
         */
        public static final String ELEMENT_DESCRIPTOR = "e_des";

        /**
         * The limit of the message.
         * <P>Type: INTEGER</P>
         */
        public static final String LIMIT = "limit";

        /**
         * The recommended-retrieval-mode of the message.
         * <P>Type: INTEGER</P>
         */
        public static final String RECOMMENDED_RETRIEVAL_MODE = "r_r_mod";

        /**
         * The recommended-retrieval-mode-text of the message.
         * <P>Type: TEXT</P>
         */
        public static final String RECOMMENDED_RETRIEVAL_MODE_TEXT = "r_r_mod_txt";

        /**
         * The status-text of the message.
         * <P>Type: TEXT</P>
         */
        public static final String STATUS_TEXT = "st_txt";

        /**
         * The applic-id of the message.
         * <P>Type: TEXT</P>
         */
        public static final String APPLIC_ID = "apl_id";

        /**
         * The reply-applic-id of the message.
         * <P>Type: TEXT</P>
         */
        public static final String REPLY_APPLIC_ID = "r_apl_id";

        /**
         * The aux-applic-id of the message.
         * <P>Type: TEXT</P>
         */
        public static final String AUX_APPLIC_ID = "aux_apl_id";

        /**
         * The drm-content of the message.
         * <P>Type: TEXT</P>
         */
        public static final String DRM_CONTENT = "drm_c";

        /**
         * The adaptation-allowed of the message.
         * <P>Type: TEXT</P>
         */
        public static final String ADAPTATION_ALLOWED = "adp_a";

        /**
         * The replace-id of the message.
         * <P>Type: TEXT</P>
         */
        public static final String REPLACE_ID = "repl_id";

        /**
         * The cancel-id of the message.
         * <P>Type: TEXT</P>
         */
        public static final String CANCEL_ID = "cl_id";

        /**
         * The cancel-status of the message.
         * <P>Type: INTEGER</P>
         */
        public static final String CANCEL_STATUS = "cl_st";

        /**
         * The thread ID of the message
         * <P>Type: INTEGER</P>
         */
        public static final String THREAD_ID = "thread_id";

        /**
         * Has the message been locked?
         * <P>Type: INTEGER (boolean)</P>
         */
        public static final String LOCKED = "locked";

        /**
         * Meta data used externally.
         * <P>Type: TEXT</P>
         */
        public static final String META_DATA = "meta_data";

        //MTK-START [mtk04070][111121][ALPS00093395]MTK added
        public static final String SIM_ID = "sim_id";

        /**
         * The service center (SC) through which to send the message, if present
         * <P>Type: TEXT</P>
         */
        public static final String SERVICE_CENTER = "service_center";
        //MTK-END [mtk04070][111121][ALPS00093395]MTK added
    }

    /**
     * Columns for the "canonical_addresses" table used by MMS and
     * SMS."
     */
    public interface CanonicalAddressesColumns extends BaseColumns {
        /**
         * An address used in MMS or SMS.  Email addresses are
         * converted to lower case and are compared by string
         * equality.  Other addresses are compared using
         * PHONE_NUMBERS_EQUAL.
         * <P>Type: TEXT</P>
         */
        public static final String ADDRESS = "address";
    }

    /**
     * Columns for the "threads" table used by MMS and SMS.
     */
    public interface ThreadsColumns extends BaseColumns {
        /**
         * The date at which the thread was created.
         *
         * <P>Type: INTEGER (long)</P>
         */
        public static final String DATE = "date";

        /**
         * A string encoding of the recipient IDs of the recipients of
         * the message, in numerical order and separated by spaces.
         * <P>Type: TEXT</P>
         */
        public static final String RECIPIENT_IDS = "recipient_ids";

        /**
         * The message count of the thread.
         * <P>Type: INTEGER</P>
         */
        public static final String MESSAGE_COUNT = "message_count";
        
        //MTK-START [mtk04070][111121][ALPS00093395]MTK added
         /**
         * The read message count of the thread.
         * <P>Type: INTEGER</P>
         */
        public static final String READCOUNT = "readcount";
        //MTK-END [mtk04070][111121][ALPS00093395]MTK added
        
        /**
         * Indicates whether all messages of the thread have been read.
         * <P>Type: INTEGER</P>
         */
        public static final String READ = "read";

        /**
         * The snippet of the latest message in the thread.
         * <P>Type: TEXT</P>
         */
        public static final String SNIPPET = "snippet";
        /**
         * The charset of the snippet.
         * <P>Type: INTEGER</P>
         */
        public static final String SNIPPET_CHARSET = "snippet_cs";
        /**
         * Type of the thread, either Threads.COMMON_THREAD or
         * Threads.BROADCAST_THREAD.
         * <P>Type: INTEGER</P>
         */
        public static final String TYPE = "type";
        /**
         * Indicates whether there is a transmission error in the thread.
         * <P>Type: INTEGER</P>
         */
        public static final String ERROR = "error";
        /**
         * Indicates whether this thread contains any attachments.
         * <P>Type: INTEGER</P>
         */
        public static final String HAS_ATTACHMENT = "has_attachment";
    }

    /**
     * Helper functions for the "threads" table used by MMS and SMS.
     */
    public static final class Threads implements ThreadsColumns {
        private static final String[] ID_PROJECTION = { BaseColumns._ID };
        private static final String STANDARD_ENCODING = "UTF-8";
        private static final Uri THREAD_ID_CONTENT_URI = Uri.parse(
                "content://mms-sms/threadID");
        public static final Uri CONTENT_URI = Uri.withAppendedPath(
                MmsSms.CONTENT_URI, "conversations");
        public static final Uri OBSOLETE_THREADS_URI = Uri.withAppendedPath(
                CONTENT_URI, "obsolete");

        public static final int COMMON_THREAD    = 0;
        public static final int BROADCAST_THREAD = 1;

        //MTK-START [mtk04070][111121][ALPS00093395]MTK added
        public static final int WAPPUSH_THREAD = 2;
        public static final int CELL_BROADCAST_THREAD = 3;
        //MTK-END [mtk04070][111121][ALPS00093395]MTK added

        /**
         * Whether a thread is being writen or not
         * 0: normal 1: being writen
         * <P>Type: INTEGER (boolean)</P>
         */
        public static final String STATUS = "status";
        // No one should construct an instance of this class.
        private Threads() {
        }

        /**
         * This is a single-recipient version of
         * getOrCreateThreadId.  It's convenient for use with SMS
         * messages.
         */
        public static long getOrCreateThreadId(Context context, String recipient) {
            Set<String> recipients = new HashSet<String>();
            recipients.add(recipient);
            return getOrCreateThreadId(context, recipients);
        }

	/**
          * This is a single-recipient version of
          * getOrCreateThreadId. It's used for internal
         */
        public static long getOrCreateThreadIdInternal(Context context, String recipient) {
            Set<String> recipients = new HashSet<String>();
            recipients.add(recipient);
            //only create a thread with status 1
            return getOrCreateThreadIdInternal(context, recipients);
        }
        /**
         * Given the recipients list and subject of an unsaved message,
         * return its thread ID. It's used for internal.
         */
        public static long getOrCreateThreadIdInternal(
                Context context, Set<String> recipients) {
            Uri.Builder uriBuilder = THREAD_ID_CONTENT_URI.buildUpon();

            for (String recipient : recipients) {
                if (Mms.isEmailAddress(recipient)) {
                    recipient = Mms.extractAddrSpec(recipient);
                }

                uriBuilder.appendQueryParameter("recipient", recipient);
            }

            Uri uri = uriBuilder.build();
            //if (DEBUG) Log.v(TAG, "getOrCreateThreadId uri: " + uri);

            Cursor cursor = SqliteWrapper.query(context, context.getContentResolver(),
                    uri, ID_PROJECTION, null, null, null);
            if (cursor != null) {
                try {
                    if (cursor.moveToFirst()) {
                        return cursor.getLong(0);
                    } else {
                        Log.e(TAG, "getOrCreateThreadId returned no rows!");
                    }
                } finally {
                    cursor.close();
                }
            }

            Log.e(TAG, "getOrCreateThreadId failed with uri " + uri.toString());
            throw new IllegalArgumentException("Unable to find or allocate a thread ID.");
        }

        /**
         * Given the recipients list and subject of an unsaved message,
         * return its thread ID.  If the message starts a new thread,
         * allocate a new thread ID.  Otherwise, use the appropriate
         * existing thread ID.
         *
         * Find the thread ID of the same set of recipients (in
         * any order, without any additions). If one
         * is found, return it.  Otherwise, return a unique thread ID.
         */
        public static long getOrCreateThreadId(
                Context context, Set<String> recipients) {
            Uri.Builder uriBuilder = THREAD_ID_CONTENT_URI.buildUpon();

            for (String recipient : recipients) {
                if (Mms.isEmailAddress(recipient)) {
                    recipient = Mms.extractAddrSpec(recipient);
                }

                uriBuilder.appendQueryParameter("recipient", recipient);
            }

            Uri uri = uriBuilder.build();
            //if (DEBUG) Log.v(TAG, "getOrCreateThreadId uri: " + uri);

            Cursor cursor = SqliteWrapper.query(context, context.getContentResolver(),
                    uri, ID_PROJECTION, null, null, null);
            if (cursor != null) {
                try {
                    if (cursor.moveToFirst()) {
                        // update the status to 0
                        ContentValues values = new ContentValues(1);
                        values.put(STATUS, 0);
                        Uri statusUri = ContentUris.withAppendedId(Uri.parse("content://mms-sms/conversations/status"), cursor.getLong(0));
                        int row = SqliteWrapper.update(context, context.getContentResolver(), statusUri, values, null, null);
                        Log.d(TAG,"getOrCreateThreadId getOrCreateThreadId row " + row);
                        return cursor.getLong(0);
                    } else {
                        Log.e(TAG, "getOrCreateThreadId returned no rows!");
                    }
                } finally {
                    cursor.close();
                }
            }

            Log.e(TAG, "getOrCreateThreadId failed with uri " + uri.toString());
            throw new IllegalArgumentException("Unable to find or allocate a thread ID.");
        }
		
        //#ifdef VENDOR_EDIT  
        //PangCong@Prd.CommApp.Mms,2012.06.11,Add for mmssms drafts
        public static long getOrCreateThreadId(Context context, String recipient,int drafts)
        {
            Set<String> recipients = new HashSet<String>();

            recipients.add(recipient);

            return getOrCreateThreadId(context, recipients, drafts);
        }

        public static long getOrCreateThreadId(Context context, Set<String> recipients, int drafts)
        {
            Uri.Builder uriBuilder = THREAD_ID_CONTENT_URI.buildUpon();

            for (String recipient : recipients) 
            {
                if (Mms.isEmailAddress(recipient)) 
                {
                    recipient = Mms.extractAddrSpec(recipient);
                }
                uriBuilder.appendQueryParameter("recipient", recipient);
            }

            Uri uri = uriBuilder.build();
            
            Cursor cursor = SqliteWrapper.query(context, context.getContentResolver(),
                    uri, ID_PROJECTION, "oppo_drafts="+drafts, null, null);
            
            if (cursor != null) 
            {
                try 
                {
                    if (cursor.moveToFirst()) 
                    {
						//Should we update the status to 0 here? Julien.
                        return cursor.getLong(0);
                    } 
                    else 
                    {
                    }
                } 
                finally 
                {
                    cursor.close();
                }
            }
            throw new IllegalArgumentException("Unable to find or allocate a thread ID.");
        }
        //#endif /* VENDOR_EDIT */
    }

    /**
     * Contains all MMS messages.
     */
    public static final class Mms implements BaseMmsColumns {
        /**
         * The content:// style URL for this table
         */
        public static final Uri CONTENT_URI = Uri.parse("content://mms");

        public static final Uri REPORT_REQUEST_URI = Uri.withAppendedPath(
                                            CONTENT_URI, "report-request");

        public static final Uri REPORT_STATUS_URI = Uri.withAppendedPath(
                                            CONTENT_URI, "report-status");

        /**
         * The default sort order for this table
         */
        public static final String DEFAULT_SORT_ORDER = "date DESC";

        /**
         * mailbox         =       name-addr
         * name-addr       =       [display-name] angle-addr
         * angle-addr      =       [CFWS] "<" addr-spec ">" [CFWS]
         */
        public static final Pattern NAME_ADDR_EMAIL_PATTERN =
                Pattern.compile("\\s*(\"[^\"]*\"|[^<>\"]+)\\s*<([^<>]+)>\\s*");

        /**
         * quoted-string   =       [CFWS]
         *                         DQUOTE *([FWS] qcontent) [FWS] DQUOTE
         *                         [CFWS]
         */
        public static final Pattern QUOTED_STRING_PATTERN =
                Pattern.compile("\\s*\"([^\"]*)\"\\s*");

        public static final Cursor query(
                ContentResolver cr, String[] projection) {
            return cr.query(CONTENT_URI, projection, null, null, DEFAULT_SORT_ORDER);
        }

        public static final Cursor query(
                ContentResolver cr, String[] projection,
                String where, String orderBy) {
            return cr.query(CONTENT_URI, projection,
                    where, null, orderBy == null ? DEFAULT_SORT_ORDER : orderBy);
        }

        public static final String getMessageBoxName(int msgBox) {
            switch (msgBox) {
                case MESSAGE_BOX_ALL:
                    return "all";
                case MESSAGE_BOX_INBOX:
                    return "inbox";
                case MESSAGE_BOX_SENT:
                    return "sent";
                case MESSAGE_BOX_DRAFTS:
                    return "drafts";
                case MESSAGE_BOX_OUTBOX:
                    return "outbox";
                default:
                    throw new IllegalArgumentException("Invalid message box: " + msgBox);
            }
        }

        public static String extractAddrSpec(String address) {
            Matcher match = NAME_ADDR_EMAIL_PATTERN.matcher(address);

            if (match.matches()) {
                return match.group(2);
            }
            return address;
        }

        /**
         * Returns true if the address is an email address
         *
         * @param address the input address to be tested
         * @return true if address is an email address
         */
        public static boolean isEmailAddress(String address) {
            if (TextUtils.isEmpty(address)) {
                return false;
            }

            String s = extractAddrSpec(address);
            Matcher match = Patterns.EMAIL_ADDRESS.matcher(s);
            return match.matches();
        }

        /**
         * Returns true if the number is a Phone number
         *
         * @param number the input number to be tested
         * @return true if number is a Phone number
         */
        public static boolean isPhoneNumber(String number) {
            if (TextUtils.isEmpty(number)) {
                return false;
            }

            Matcher match = Patterns.PHONE.matcher(number);
            return match.matches();
        }

        /**
         * Contains all MMS messages in the MMS app's inbox.
         */
        public static final class Inbox implements BaseMmsColumns {
            /**
             * The content:// style URL for this table
             */
            public static final Uri
                    CONTENT_URI = Uri.parse("content://mms/inbox");

            /**
             * The default sort order for this table
             */
            public static final String DEFAULT_SORT_ORDER = "date DESC";
        }

        /**
         * Contains all MMS messages in the MMS app's sent box.
         */
        public static final class Sent implements BaseMmsColumns {
            /**
             * The content:// style URL for this table
             */
            public static final Uri
                    CONTENT_URI = Uri.parse("content://mms/sent");

            /**
             * The default sort order for this table
             */
            public static final String DEFAULT_SORT_ORDER = "date DESC";
        }

        /**
         * Contains all MMS messages in the MMS app's drafts box.
         */
        public static final class Draft implements BaseMmsColumns {
            /**
             * The content:// style URL for this table
             */
            public static final Uri
                    CONTENT_URI = Uri.parse("content://mms/drafts");

            /**
             * The default sort order for this table
             */
            public static final String DEFAULT_SORT_ORDER = "date DESC";
        }

        /**
         * Contains all MMS messages in the MMS app's outbox.
         */
        public static final class Outbox implements BaseMmsColumns {
            /**
             * The content:// style URL for this table
             */
            public static final Uri
                    CONTENT_URI = Uri.parse("content://mms/outbox");

            /**
             * The default sort order for this table
             */
            public static final String DEFAULT_SORT_ORDER = "date DESC";
        }

        public static final class Addr implements BaseColumns {
            /**
             * The ID of MM which this address entry belongs to.
             */
            public static final String MSG_ID = "msg_id";

            /**
             * The ID of contact entry in Phone Book.
             */
            public static final String CONTACT_ID = "contact_id";

            /**
             * The address text.
             */
            public static final String ADDRESS = "address";

            /**
             * Type of address, must be one of PduHeaders.BCC,
             * PduHeaders.CC, PduHeaders.FROM, PduHeaders.TO.
             */
            public static final String TYPE = "type";

            /**
             * Character set of this entry.
             */
            public static final String CHARSET = "charset";
        }

        public static final class Part implements BaseColumns {
            /**
             * The identifier of the message which this part belongs to.
             * <P>Type: INTEGER</P>
             */
            public static final String MSG_ID = "mid";

            /**
             * The order of the part.
             * <P>Type: INTEGER</P>
             */
            public static final String SEQ = "seq";

            /**
             * The content type of the part.
             * <P>Type: TEXT</P>
             */
            public static final String CONTENT_TYPE = "ct";

            /**
             * The name of the part.
             * <P>Type: TEXT</P>
             */
            public static final String NAME = "name";

            /**
             * The charset of the part.
             * <P>Type: TEXT</P>
             */
            public static final String CHARSET = "chset";

            /**
             * The file name of the part.
             * <P>Type: TEXT</P>
             */
            public static final String FILENAME = "fn";

            /**
             * The content disposition of the part.
             * <P>Type: TEXT</P>
             */
            public static final String CONTENT_DISPOSITION = "cd";

            /**
             * The content ID of the part.
             * <P>Type: INTEGER</P>
             */
            public static final String CONTENT_ID = "cid";

            /**
             * The content location of the part.
             * <P>Type: INTEGER</P>
             */
            public static final String CONTENT_LOCATION = "cl";

            /**
             * The start of content-type of the message.
             * <P>Type: INTEGER</P>
             */
            public static final String CT_START = "ctt_s";

            /**
             * The type of content-type of the message.
             * <P>Type: TEXT</P>
             */
            public static final String CT_TYPE = "ctt_t";

            /**
             * The location(on filesystem) of the binary data of the part.
             * <P>Type: INTEGER</P>
             */
            public static final String _DATA = "_data";

            public static final String TEXT = "text";

        }

        public static final class Rate {
            public static final Uri CONTENT_URI = Uri.withAppendedPath(
                    Mms.CONTENT_URI, "rate");
            /**
             * When a message was successfully sent.
             * <P>Type: INTEGER</P>
             */
            public static final String SENT_TIME = "sent_time";
        }

        public static final class ScrapSpace {
            /**
             * The content:// style URL for this table
             */
            public static final Uri CONTENT_URI = Uri.parse("content://mms/scrapSpace");

            /**
             * This is the scrap file we use to store the media attachment when the user
             * chooses to capture a photo to be attached . We pass {#link@Uri} to the Camera app,
             * which streams the captured image to the uri. Internally we write the media content
             * to this file. It's named '.temp.jpg' so Gallery won't pick it up.
             */
            public static final String SCRAP_FILE_PATH = "/sdcard/mms/scrapSpace/.temp.jpg";
        }

        public static final class Intents {
            private Intents() {
                // Non-instantiatable.
            }

            /**
             * The extra field to store the contents of the Intent,
             * which should be an array of Uri.
             */
            public static final String EXTRA_CONTENTS = "contents";
            /**
             * The extra field to store the type of the contents,
             * which should be an array of String.
             */
            public static final String EXTRA_TYPES    = "types";
            /**
             * The extra field to store the 'Cc' addresses.
             */
            public static final String EXTRA_CC       = "cc";
            /**
             * The extra field to store the 'Bcc' addresses;
             */
            public static final String EXTRA_BCC      = "bcc";
            /**
             * The extra field to store the 'Subject'.
             */
            public static final String EXTRA_SUBJECT  = "subject";
            /**
             * Indicates that the contents of specified URIs were changed.
             * The application which is showing or caching these contents
             * should be updated.
             */
            public static final String
            CONTENT_CHANGED_ACTION = "android.intent.action.CONTENT_CHANGED";
            /**
             * An extra field which stores the URI of deleted contents.
             */
            public static final String DELETED_CONTENTS = "deleted_contents";
        }
    }

    /**
     * Contains all MMS and SMS messages.
     */
    public static final class MmsSms implements BaseColumns {
        /**
         * The column to distinguish SMS &amp; MMS messages in query results.
         */
        public static final String TYPE_DISCRIMINATOR_COLUMN =
                "transport_type";

        public static final Uri CONTENT_URI = Uri.parse("content://mms-sms/");

        public static final Uri CONTENT_CONVERSATIONS_URI = Uri.parse(
                "content://mms-sms/conversations");

        public static final Uri CONTENT_FILTER_BYPHONE_URI = Uri.parse(
                "content://mms-sms/messages/byphone");

        public static final Uri CONTENT_UNDELIVERED_URI = Uri.parse(
                "content://mms-sms/undelivered");

        public static final Uri CONTENT_DRAFT_URI = Uri.parse(
                "content://mms-sms/draft");

        public static final Uri CONTENT_LOCKED_URI = Uri.parse(
                "content://mms-sms/locked");

        //MTK-START [mtk04070][111121][ALPS00093395]MTK added
        public static final Uri CONTENT_URI_QUICKTEXT =
            Uri.parse("content://mms-sms/quicktext");
        //MTK-END [mtk04070][111121][ALPS00093395]MTK added

        /***
         * Pass in a query parameter called "pattern" which is the text
         * to search for.
         * The sort order is fixed to be thread_id ASC,date DESC.
         */
        public static final Uri SEARCH_URI = Uri.parse(
                "content://mms-sms/search");

        // Constants for message protocol types.
        public static final int SMS_PROTO = 0;
        public static final int MMS_PROTO = 1;

        // Constants for error types of pending messages.
        public static final int NO_ERROR                      = 0;
        public static final int ERR_TYPE_GENERIC              = 1;
        public static final int ERR_TYPE_SMS_PROTO_TRANSIENT  = 2;
        public static final int ERR_TYPE_MMS_PROTO_TRANSIENT  = 3;
        public static final int ERR_TYPE_TRANSPORT_FAILURE    = 4;
        public static final int ERR_TYPE_GENERIC_PERMANENT    = 10;
        public static final int ERR_TYPE_SMS_PROTO_PERMANENT  = 11;
        public static final int ERR_TYPE_MMS_PROTO_PERMANENT  = 12;

        public static final class PendingMessages implements BaseColumns {
            public static final Uri CONTENT_URI = Uri.withAppendedPath(
                    MmsSms.CONTENT_URI, "pending");
            /**
             * The type of transport protocol(MMS or SMS).
             * <P>Type: INTEGER</P>
             */
            public static final String PROTO_TYPE = "proto_type";
            /**
             * The ID of the message to be sent or downloaded.
             * <P>Type: INTEGER</P>
             */
            public static final String MSG_ID = "msg_id";
            /**
             * The type of the message to be sent or downloaded.
             * This field is only valid for MM. For SM, its value is always
             * set to 0.
             */
            public static final String MSG_TYPE = "msg_type";
            /**
             * The type of the error code.
             * <P>Type: INTEGER</P>
             */
            public static final String ERROR_TYPE = "err_type";
            /**
             * The error code of sending/retrieving process.
             * <P>Type:  INTEGER</P>
             */
            public static final String ERROR_CODE = "err_code";
            /**
             * How many times we tried to send or download the message.
             * <P>Type:  INTEGER</P>
             */
            public static final String RETRY_INDEX = "retry_index";
            /**
             * The time to do next retry.
             */
            public static final String DUE_TIME = "due_time";
            /**
             * The time we last tried to send or download the message.
             */
            public static final String LAST_TRY = "last_try";

            //MTK-START [mtk04070][111121][ALPS00093395]MTK added
            public static final String SIM_ID = "pending_sim_id";            
            //MTK-END [mtk04070][111121][ALPS00093395]MTK added
        }

        public static final class WordsTable {
            public static final String ID = "_id";
            public static final String SOURCE_ROW_ID = "source_id";
            public static final String TABLE_ID = "table_to_use";
            public static final String INDEXED_TEXT = "index_text";
        }
    }

    public static final class Carriers implements BaseColumns {
        /**
         * The content:// style URL for this table
         */
        public static final Uri CONTENT_URI =
            Uri.parse("content://telephony/carriers");

        //MTK-START [mtk04070][111121][ALPS00093395]MTK added
        public static final Uri CONTENT_URI_DM =
            Uri.parse("content://telephony/carriers_dm");

        /* Add by mtk01411 for test purpose */
        public static final Uri CONTENT_URI_2 =
            Uri.parse("content://telephony/carriers2");
        //MTK-END [mtk04070][111121][ALPS00093395]MTK added

        /**
         * The default sort order for this table
         */
        public static final String DEFAULT_SORT_ORDER = "name ASC";

        public static final String NAME = "name";

        public static final String APN = "apn";

        public static final String PROXY = "proxy";

        public static final String PORT = "port";

        public static final String MMSPROXY = "mmsproxy";

        public static final String MMSPORT = "mmsport";

        public static final String SERVER = "server";

        public static final String USER = "user";

        public static final String PASSWORD = "password";

        public static final String MMSC = "mmsc";

        public static final String MCC = "mcc";

        public static final String MNC = "mnc";

        public static final String NUMERIC = "numeric";

        public static final String AUTH_TYPE = "authtype";

        public static final String TYPE = "type";

        public static final String INACTIVE_TIMER = "inactivetimer";

        // Only if enabled try Data Connection.
        public static final String ENABLED = "enabled";

        // Rules apply based on class.
        public static final String CLASS = "class";

        //MTK-START [mtk04070][111121][ALPS00093395]MTK added
        public static final String OMACPID = "omacpid";
        public static final String NAPID = "napid";
        public static final String PROXYID = "proxyid";

		//#ifdef VENDOR_EDIT
		//PangCong@Prd.CommApp.Mms,2012.06.11,Add for	
        public static final String IPVERSION = "ipversion";
		//#endif /* VENDOR_EDIT */
        
        public static final String SOURCE_TYPE = "sourcetype";
        public static final String CSD_NUM = "csdnum";      
        //MTK-END [mtk04070][111121][ALPS00093395]MTK added

        /**
         * The protocol to be used to connect to this APN.
         *
         * One of the PDP_type values in TS 27.007 section 10.1.1.
         * For example, "IP", "IPV6", "IPV4V6", or "PPP".
         */
        public static final String PROTOCOL = "protocol";

        /**
          * The protocol to be used to connect to this APN when roaming.
          *
          * The syntax is the same as protocol.
          */
        public static final String ROAMING_PROTOCOL = "roaming_protocol";

        public static final String CURRENT = "current";

        /**
          * Current status of APN
          * true : enabled APN, false : disabled APN.
          */
        public static final String CARRIER_ENABLED = "carrier_enabled";

        /**
          * Radio Access Technology info
          * To check what values can hold, refer to ServiceState.java.
          * This should be spread to other technologies,
          * but currently only used for LTE(14) and EHRPD(13).
          */
        public static final String BEARER = "bearer";
		
		//#ifdef VENDOR_EDIT
		//TongJing.Shi@EXP.DataComm.Phone, 2013.10.05, Modify for thai apn
		public static final String OPPOSPN = "oppoSpn";
		//#endif /* VENDOR_EDIT */

        //MTK-START [mtk04070][111121][ALPS00093395]MTK added
        public static final String SPN = "spn";
        public static final String IMSI = "imsi";
        public static final String PNN = "pnn";

        public static final class GeminiCarriers {
            public static final  Uri CONTENT_URI =
                Uri.parse("content://telephony/carriers_gemini"); 
            public static final  Uri CONTENT_URI_DM =
                Uri.parse("content://telephony/carriers_dm_gemini"); 
        }
        public static final class SIM1Carriers {
            public static final  Uri CONTENT_URI =
                Uri.parse("content://telephony/carriers_sim1"); 
        }
        public static final class SIM2Carriers {
            public static final  Uri CONTENT_URI =
                Uri.parse("content://telephony/carriers_sim2"); 
        }
        //MTK-END [mtk04070][111121][ALPS00093395]MTK added

    }

    //MTK-START [mtk04070][111121][ALPS00093395]MTK added
    public static final class SimInfo implements BaseColumns{
        public static final Uri CONTENT_URI = 
            Uri.parse("content://telephony/siminfo");
        
        public static final String DEFAULT_SORT_ORDER = "name ASC";
        
        /**
         * <P>Type: TEXT</P>
         */
        public static final String ICC_ID = "icc_id";
        /**
         * <P>Type: TEXT</P>
         */
        public static final String DISPLAY_NAME = "display_name";
        public static final int DEFAULT_NAME_MIN_INDEX = 01;
        public static final int DEFAULT_NAME_MAX_INDEX= 99;
        public static final int DEFAULT_NAME_RES = 0;
        public static final String NAME_SOURCE = "name_source";
        public static final int DEFAULT_SOURCE = 0;
        public static final int SIM_SOURCE = 1;
        public static final int USER_INPUT = 2;

        /**
         * <P>Type: TEXT</P>
         */
        public static final String NUMBER = "number";
        
        /**
         * 0:none, 1:the first four digits, 2:the last four digits.
         * <P>Type: INTEGER</P>
         */
        public static final String DISPLAY_NUMBER_FORMAT = "display_number_format";
        public static final int DISPALY_NUMBER_NONE = 0;
        public static final int DISPLAY_NUMBER_FIRST = 1;
        public static final int DISPLAY_NUMBER_LAST = 2;
        public static final int DISLPAY_NUMBER_DEFAULT = DISPLAY_NUMBER_FIRST;
        
        /**
         * Eight kinds of colors. 0-3 will represent the eight colors.
         * Default value: any color that is not in-use.
         * <P>Type: INTEGER</P>
         */
        public static final String COLOR = "color";
        public static final int COLOR_1 = 0;
        public static final int COLOR_2 = 1;
        public static final int COLOR_3 = 2;
        public static final int COLOR_4 = 3;
        public static final int COLOR_DEFAULT = COLOR_1;
        
        /**
         * 0: Don't allow data when roaming, 1:Allow data when roaming
         * <P>Type: INTEGER</P>
         */
        public static final String DATA_ROAMING = "data_roaming";
        public static final int DATA_ROAMING_ENABLE = 1;
        public static final int DATA_ROAMING_DISABLE = 0;
        public static final int DATA_ROAMING_DEFAULT = DATA_ROAMING_DISABLE;
        
        /**
         * <P>Type: INTEGER</P>
         */
        public static final String SLOT = "slot";
        public static final int SLOT_NONE = -1;
        
        public static final int ERROR_GENERAL = -1;
        public static final int ERROR_NAME_EXIST = -2;
        
        /**
         * <p>Type: INTEGER<p>
         */
        public static final String WAP_PUSH = "wap_push";
        public static final int WAP_PUSH_DEFAULT = -1;
        public static final int WAP_PUSH_DISABLE = 0;
        public static final int WAP_PUSH_ENABLE = 1;
        
        
    }
    
    public static final int[] SIMBackgroundRes = new int[] {
        0,//com.mediatek.internal.R.drawable.sim_background_blue,
        0,//com.mediatek.internal.R.drawable.sim_background_orange,
        0,//com.mediatek.internal.R.drawable.sim_background_green,
        0//com.mediatek.internal.R.drawable.sim_background_purple
    };
    
    public static class SIMInfo {
        public long mSimId;
        public String mICCId;
        public String mDisplayName = "";
        public int mNameSource;
        public String mNumber = "";
        public int mDispalyNumberFormat = SimInfo.DISLPAY_NUMBER_DEFAULT;
        public int mColor;
        public int mDataRoaming = SimInfo.DATA_ROAMING_DEFAULT;
        public int mSlot = SimInfo.SLOT_NONE;
        public int mSimBackgroundRes = SIMBackgroundRes[SimInfo.COLOR_DEFAULT];
        public int mWapPush = -1;
        private SIMInfo() {
        }
        
        public static class ErrorCode {
            public static final int ERROR_GENERAL = -1;
            public static final int ERROR_NAME_EXIST = -2;
        }
        //#ifndef VENDOR_EDIT
        //Hong.Liu@Prd.CommApp.Telephony, 2012/04/09, Add for 		
        private static SIMInfo fromCursor(Cursor cursor, Context ctx) {
            SIMInfo info = new SIMInfo();
            info.mSimId = cursor.getLong(cursor.getColumnIndexOrThrow(SimInfo._ID));
            info.mICCId = cursor.getString(cursor.getColumnIndexOrThrow(SimInfo.ICC_ID));
            info.mSlot = cursor.getInt(cursor.getColumnIndexOrThrow(SimInfo.SLOT));

            if(/*FeatureOption.OPPO_ULIKE_STYLE_SUPPORT ==*/ true){
                String name =  cursor.getString(cursor.getColumnIndexOrThrow(SimInfo.DISPLAY_NAME));
				Log.d(TAG,"name:" + name + ",info.mSlot:"+info.mSlot + "info.mSimId:" + info.mSimId);
                if(name != null && name.equals("SIM1") && (info.mSlot == OppoTelephonyConstant.GEMINI_SIM_2)){                    
                    info.mDisplayName = "SIM2";
                    setDisplayNameEx(ctx,"SIM2",info.mSimId,SimInfo.DEFAULT_SOURCE);
                } else if(name != null && name.equals("SIM2") && (info.mSlot == OppoTelephonyConstant.GEMINI_SIM_1)){
                    info.mDisplayName = "SIM1";
                    setDisplayNameEx(ctx,"SIM1",info.mSimId,SimInfo.DEFAULT_SOURCE);  
                } else {
                    info.mDisplayName = name;
                }
            } else {
                String name = cursor.getString(cursor.getColumnIndexOrThrow(SimInfo.DISPLAY_NAME));
                if(name != null && name.equals("46000")){
                    info.mDisplayName = ctx.getText(OPPO_LONG_4600).toString();
                    if(info.mSlot == OppoTelephonyConstant.GEMINI_SIM_1 || info.mSlot == OppoTelephonyConstant.GEMINI_SIM_2){
                        info.mDisplayName = info.mDisplayName + "0" + (info.mSlot+1);
                    }    
                    Log.e("SIMINFO", "info.mDisplayName1 = " + info.mDisplayName);
                }else if(name != null && name.equals("46001")){
                    info.mDisplayName = ctx.getText(OPPO_LONG_4600).toString();
                    if(info.mSlot == OppoTelephonyConstant.GEMINI_SIM_1 || info.mSlot == OppoTelephonyConstant.GEMINI_SIM_2){
                        info.mDisplayName = info.mDisplayName + "0" + (info.mSlot+1);
                    }  
                    Log.e("SIMINFO", "info.mDisplayName2 = " + info.mDisplayName);
                }else{
                    info.mDisplayName = name;
                }  
            }
            info.mNameSource = cursor.getInt(cursor.getColumnIndexOrThrow(SimInfo.NAME_SOURCE));
            info.mNumber = cursor.getString(cursor.getColumnIndexOrThrow(SimInfo.NUMBER));
            info.mDispalyNumberFormat = cursor.getInt(cursor.getColumnIndexOrThrow(SimInfo.DISPLAY_NUMBER_FORMAT));
            info.mColor = cursor.getInt(cursor.getColumnIndexOrThrow(SimInfo.COLOR));
            info.mDataRoaming = cursor.getInt(cursor.getColumnIndexOrThrow(SimInfo.DATA_ROAMING));
            int size = SIMBackgroundRes.length;
            if (info.mColor >= 0 && info.mColor < size) {
                info.mSimBackgroundRes = SIMBackgroundRes[info.mColor];
            }
            info.mWapPush = cursor.getInt(cursor.getColumnIndexOrThrow(SimInfo.WAP_PUSH));
            return info;
        }
		//#endif /* VENDOR_EDIT */
        private static SIMInfo fromCursor(Cursor cursor) {
            SIMInfo info = new SIMInfo();
            info.mSimId = cursor.getLong(cursor.getColumnIndexOrThrow(SimInfo._ID));
            info.mICCId = cursor.getString(cursor.getColumnIndexOrThrow(SimInfo.ICC_ID));
            info.mDisplayName = cursor.getString(cursor.getColumnIndexOrThrow(SimInfo.DISPLAY_NAME));
            info.mNameSource = cursor.getInt(cursor.getColumnIndexOrThrow(SimInfo.NAME_SOURCE));
            info.mNumber = cursor.getString(cursor.getColumnIndexOrThrow(SimInfo.NUMBER));
            info.mDispalyNumberFormat = cursor.getInt(cursor.getColumnIndexOrThrow(SimInfo.DISPLAY_NUMBER_FORMAT));
            info.mColor = cursor.getInt(cursor.getColumnIndexOrThrow(SimInfo.COLOR));
            info.mDataRoaming = cursor.getInt(cursor.getColumnIndexOrThrow(SimInfo.DATA_ROAMING));
            info.mSlot = cursor.getInt(cursor.getColumnIndexOrThrow(SimInfo.SLOT));
            int size = SIMBackgroundRes.length;
            if (info.mColor >= 0 && info.mColor < size) {
                info.mSimBackgroundRes = SIMBackgroundRes[info.mColor];
            }
            info.mWapPush = cursor.getInt(cursor.getColumnIndexOrThrow(SimInfo.WAP_PUSH));
            return info;
        }
        
        /**
         * 
         * @param ctx
         * @return the array list of Current SIM Info
         */
        public static List<SIMInfo> getInsertedSIMList(Context ctx) {
            ArrayList<SIMInfo> simList = new ArrayList<SIMInfo>();
			//#ifndef VENDOR_EDIT 
			//FeiLong.Xu@Prd.CommApp.Telephony, 2012/01/09, Modify for sim selection show by order
			/*
            Cursor cursor = ctx.getContentResolver().query(SimInfo.CONTENT_URI, 
                    null, SimInfo.SLOT + "!=" + SimInfo.SLOT_NONE, null, null);
			*/       
			//#else /* VENDOR_EDIT */
            Cursor cursor = ctx.getContentResolver().query(SimInfo.CONTENT_URI, 
                    null, SimInfo.SLOT + "!=" + SimInfo.SLOT_NONE, null, "slot ASC");
			//#endif /* VENDOR_EDIT */
            try {
                if (cursor != null) {
                    while (cursor.moveToNext()) {
                        //#ifndef VENDOR_EDIT
                        //Hong.Liu@Prd.CommApp.Telephony, 2012/04/09,Modify for
                        /*
                        simList.add(SIMInfo.fromCursor(cursor));
                        */
                        //#else /* VENDOR_EDIT */
                        simList.add(SIMInfo.fromCursor(cursor, ctx));    
                        //#endif /* VENDOR_EDIT */
                    }
                }
            } finally {
                if (cursor != null) {
                    cursor.close();
                }
            }
            return simList;
        }
        
        /**
         * 
         * @param ctx
         * @return array list of all the SIM Info include what were used before
         */
        public static List<SIMInfo> getAllSIMList(Context ctx) {
            ArrayList<SIMInfo> simList = new ArrayList<SIMInfo>();
            Cursor cursor = ctx.getContentResolver().query(SimInfo.CONTENT_URI, 
                    null, null, null, null);
            try {
                if (cursor != null) {
                    while (cursor.moveToNext()) {
                        //#ifndef VENDOR_EDIT
                        //Hong.Liu@Prd.CommApp.Telephony, 2012/04/09,Modify for
                        /*
                        simList.add(SIMInfo.fromCursor(cursor));
                        */
                        //#else /* VENDOR_EDIT */
                        simList.add(SIMInfo.fromCursor(cursor, ctx));    
                        //#endif /* VENDOR_EDIT */
                    }
                }
            } finally {
                if (cursor != null) {
                    cursor.close();
                }
            }
            return simList;
        }
        
        /**
         * 
         * @param ctx
         * @param SIMId the unique SIM id
         * @return SIM-Info, maybe null
         */
        public static SIMInfo getSIMInfoById(Context ctx, long SIMId) {
            if (SIMId <= 0 ) return null;
            Cursor cursor = ctx.getContentResolver().query(ContentUris.withAppendedId(SimInfo.CONTENT_URI, SIMId), 
                    null, null, null, null);
            try {
                if (cursor != null) {
                    if (cursor.moveToFirst()) {
                        //#ifndef VENDOR_EDIT
                        //Hong.Liu@Prd.CommApp.Telephony, 2012/04/09,Modify for
                        /*
                        return SIMInfo.fromCursor(cursor);
                        */
                        //#else /* VENDOR_EDIT */
                        return SIMInfo.fromCursor(cursor, ctx);    
                        //#endif /* VENDOR_EDIT */
                    
                        
                    }
                }
            } finally {
                if (cursor != null) {
                    cursor.close();
                }
            }
            return null;
        }
        
        /**
         * 
         * @param ctx
         * @param SIMName the Name of the SIM Card
         * @return SIM-Info, maybe null
         */
        public static SIMInfo getSIMInfoByName(Context ctx, String SIMName) {
            if (SIMName == null) return null;
            Cursor cursor = ctx.getContentResolver().query(SimInfo.CONTENT_URI, 
                    null, SimInfo.DISPLAY_NAME + "=?", new String[]{SIMName}, null);
            try {
                if (cursor != null) {
                    if (cursor.moveToFirst()) {
                        //#ifndef VENDOR_EDIT
                        //Hong.Liu@Prd.CommApp.Telephony, 2012/04/09,Modify for
                        /*
                        return SIMInfo.fromCursor(cursor);
                        */
                        //#else /* VENDOR_EDIT */
                        return SIMInfo.fromCursor(cursor, ctx);    
                        //#endif /* VENDOR_EDIT */
                    }
                }
            } finally {
                if (cursor != null) {
                    cursor.close();
                }
            }
            return null;
        }
        
        /**
         * @param ctx
         * @param cardSlot
         * @return The SIM-Info, maybe null
         */
        public static SIMInfo getSIMInfoBySlot(Context ctx, int cardSlot) {
            if (cardSlot < 0) return null;
            Cursor cursor = ctx.getContentResolver().query(SimInfo.CONTENT_URI, 
                    null, SimInfo.SLOT + "=?", new String[]{String.valueOf(cardSlot)}, null);
            try {
                if (cursor != null) {
                    if (cursor.moveToFirst()) {
                        //#ifndef VENDOR_EDIT
                        //Hong.Liu@Prd.CommApp.Telephony, 2012/04/09 ,Modify for
                        /*
                        return SIMInfo.fromCursor(cursor);
                        */
                        //#else /* VENDOR_EDIT */
                        return SIMInfo.fromCursor(cursor, ctx);    
                        //#endif /* VENDOR_EDIT */
                    }
                }
            } finally {
                if (cursor != null) {
                    cursor.close();
                }
            }
            return null;
        }
        
        /**
         * @param ctx
         * @param iccid 
         * @return The SIM-Info, maybe null
         */
        public static SIMInfo getSIMInfoByICCId(Context ctx, String iccid) {
            if (iccid == null) return null;
            Cursor cursor = ctx.getContentResolver().query(SimInfo.CONTENT_URI, 
                    null, SimInfo.ICC_ID + "=?", new String[]{iccid}, null);
            try {
                if (cursor != null) {
                    if (cursor.moveToFirst()) {
                        //#ifndef VENDOR_EDIT
                        //Hong.Liu@CommApp.Telephony, 2012/04/09,Modify for
                        /*
                        return SIMInfo.fromCursor(cursor);
                        */
                        //#else /* VENDOR_EDIT */
                        return SIMInfo.fromCursor(cursor, ctx);    
                        //#endif /* VENDOR_EDIT */
                    }
                }
            } finally {
                if (cursor != null) {
                    cursor.close();
                }
            }
            return null;
        }
        
        /**
         * @param ctx
         * @param SIMId
         * @return the slot of the SIM Card, -1 indicate that the SIM card is missing
         */
        public static int getSlotById(Context ctx, long SIMId) {
            if (SIMId <= 0 ) return SimInfo.SLOT_NONE;
            Cursor cursor = ctx.getContentResolver().query(ContentUris.withAppendedId(SimInfo.CONTENT_URI, SIMId), 
                    new String[]{SimInfo.SLOT}, null, null, null);
            try {
                if (cursor != null) {
                    if (cursor.moveToFirst()) {
                        return cursor.getInt(0);
                    }
                }
            } finally {
                if (cursor != null) {
                    cursor.close();
                }
            }
            return  SimInfo.SLOT_NONE;
        }

        /**
         * @param ctx
         * @param SIMId
         * @return the id of the SIM Card, 0 indicate that no SIM card is inserted
         */
        public static long getIdBySlot(Context ctx, int slot) {
            SIMInfo simInfo = getSIMInfoBySlot(ctx, slot);
            if (simInfo != null)
                return simInfo.mSimId;
            return 0;
        }
        
        /**
         * @param ctx
         * @param SIMName
         * @return the slot of the SIM Card, -1 indicate that the SIM card is missing
         */
        public static int getSlotByName(Context ctx, String SIMName) {
            if (SIMName == null) return SimInfo.SLOT_NONE;
            Cursor cursor = ctx.getContentResolver().query(SimInfo.CONTENT_URI, 
                    new String[]{SimInfo.SLOT}, SimInfo.DISPLAY_NAME + "=?", new String[]{SIMName}, null);
            try {
                if (cursor != null) {
                    if (cursor.moveToFirst()) {
                        return cursor.getInt(0);
                    }
                }
            } finally {
                if (cursor != null) {
                    cursor.close();
                }
            }
            return SimInfo.SLOT_NONE;
        }
        
        /**
         * @param ctx
         * @return current SIM Count
         */
        public static int getInsertedSIMCount(Context ctx) {
            Cursor cursor = ctx.getContentResolver().query(SimInfo.CONTENT_URI, 
                    null, SimInfo.SLOT + "!=" + SimInfo.SLOT_NONE, null, null);
            try {
                if (cursor != null) {
                    return cursor.getCount();
                }
            } finally {
                if (cursor != null) {
                    cursor.close();
                }
            }
            return 0;
        }
        
        /**
         * @param ctx
         * @return the count of all the SIM Card include what was used before
         */
        public static int getAllSIMCount(Context ctx) {
            Cursor cursor = ctx.getContentResolver().query(SimInfo.CONTENT_URI, 
                    null, null, null, null);
            try {
                if (cursor != null) {
                    return cursor.getCount();
                }
            } finally {
                if (cursor != null) {
                    cursor.close();
                }
            }
            return 0;
        }
        
        /**
         * set display name by SIM ID
         * @param ctx
         * @param displayName
         * @param SIMId
         * @return -1 means general error, -2 means the name is exist. >0 means success
         */
        public static int setDisplayName(Context ctx, String displayName, long SIMId) {
            if (displayName == null || SIMId <= 0) return ErrorCode.ERROR_GENERAL;
           //#ifndef VENDOR_EDIT
            //Hong.Liu@CommApp.Telephony, 2012/02/13,Remove for
            /*
            Cursor cursor = ctx.getContentResolver().query(SimInfo.CONTENT_URI, 
                    new String[]{SimInfo._ID}, SimInfo.DISPLAY_NAME + "=?", new String[]{displayName}, null);
            try {
                if (cursor != null) {
                    if (cursor.getCount() > 0) {
                        return ErrorCode.ERROR_NAME_EXIST;
                    }
                }
            } finally {
                if (cursor != null) {
                    cursor.close();
                }
            }
            */
            //#endif /* VENDOR_EDIT */
            ContentValues value = new ContentValues(1);
            value.put(SimInfo.DISPLAY_NAME, displayName);
            return ctx.getContentResolver().update(ContentUris.withAppendedId(SimInfo.CONTENT_URI, SIMId), 
                    value, null, null);
        }

        /**
         * set display name by SIM ID with name source
         * @param ctx
         * @param displayName
         * @param SIMId
         * @param Source, ex, SYSTEM_INPUT, USER_INPUT
         * @return -1 means general error, -2 means the name is exist. >0 means success
         */
        public static int setDisplayNameEx(Context ctx, String displayName, long SIMId, long Source) {
            Log.i(TAG, "setDisplayNameEx SIMId " + SIMId + " source = " + Source + "displayName = " + displayName);
            if (displayName == null || SIMId <= 0) return ErrorCode.ERROR_GENERAL;
            //#ifndef VENDOR_EDIT
            //Hong.Liu@CommApp.Telephony, 2012/02/13,Remove for
            /*
            Cursor cursor = ctx.getContentResolver().query(SimInfo.CONTENT_URI, 
                    new String[]{SimInfo._ID}, SimInfo.DISPLAY_NAME + "=?", new String[]{displayName}, null);
            try {
                if (cursor != null) {
                    if (cursor.getCount() > 0) {
                        return ErrorCode.ERROR_NAME_EXIST;
                    }
                }
            } finally {
                if (cursor != null) {
                    cursor.close();
                }
            }
            */
            //#endif /* VENDOR_EDIT */
            
            ContentValues value = new ContentValues(1);
            value.put(SimInfo.DISPLAY_NAME, displayName);
            value.put(SimInfo.NAME_SOURCE, Source);
            return ctx.getContentResolver().update(ContentUris.withAppendedId(SimInfo.CONTENT_URI, SIMId), 
                    value, null, null);
        }
        
        /**
         * @param ctx
         * @param number
         * @param SIMId
         * @return >0 means success
         */
        public static int setNumber(Context ctx, String number, long SIMId) {
            if (number == null || SIMId <= 0) return -1;
            ContentValues value = new ContentValues(1);
            value.put(SimInfo.NUMBER, number);
            return ctx.getContentResolver().update(ContentUris.withAppendedId(SimInfo.CONTENT_URI, SIMId), 
                    value, null, null);
        }
        
        /**
         * 
         * @param ctx
         * @param color
         * @param SIMId
         * @return >0 means success
         */
        public static int setColor(Context ctx, int color, long SIMId) {
            int size = SIMBackgroundRes.length;
            if (color < 0 || SIMId <= 0 || color >= size) return -1;
            ContentValues value = new ContentValues(1);
            value.put(SimInfo.COLOR, color);
            return ctx.getContentResolver().update(ContentUris.withAppendedId(SimInfo.CONTENT_URI, SIMId), 
                    value, null, null);
        }
        
        /**
         * set the format.0: none, 1: the first four digits, 2: the last four digits.
         * @param ctx
         * @param format
         * @param SIMId
         * @return >0 means success
         */
        public static int setDispalyNumberFormat(Context ctx, int format, long SIMId) {
            if (format < 0 || SIMId <= 0) return -1;
            ContentValues value = new ContentValues(1);
            value.put(SimInfo.DISPLAY_NUMBER_FORMAT, format);
            return ctx.getContentResolver().update(ContentUris.withAppendedId(SimInfo.CONTENT_URI, SIMId), 
                    value, null, null);
        }
        
        /**
         * set data roaming.0:Don't allow data when roaming, 1:Allow data when roaming
         * @param ctx
         * @param roaming
         * @param SIMId
         * @return >0 means success
         */
        public static int setDataRoaming(Context ctx, int roaming, long SIMId) {
            if (roaming < 0 || SIMId <= 0) return -1;
            ContentValues value = new ContentValues(1);
            value.put(SimInfo.DATA_ROAMING, roaming);
            return ctx.getContentResolver().update(ContentUris.withAppendedId(SimInfo.CONTENT_URI, SIMId), 
                    value, null, null);
        }
        
        /**
         * set the wap push flag
         * @return >0 means success
         */
        public static int setWAPPush(Context ctx, int enable, long SIMId) {
            if (enable > 1 || enable < -1 || SIMId <= 0) {
                return -1;
            }
            ContentValues value = new ContentValues(1);
            value.put(SimInfo.WAP_PUSH, enable);
            return ctx.getContentResolver().update(ContentUris.withAppendedId(SimInfo.CONTENT_URI, SIMId),
                    value, null, null);
        }
        /**
         * Insert the ICC ID and slot if needed
         * @param ctx
         * @param ICCId
         * @param slot
         * @return
         */
        public static Uri insertICCId(Context ctx, String ICCId, int slot) {
            if (ICCId == null) {
                throw new IllegalArgumentException("ICCId should not null.");
            }
            Uri uri;
            ContentResolver resolver = ctx.getContentResolver();
            String selection = SimInfo.ICC_ID + "=?";
            Cursor cursor = resolver.query(SimInfo.CONTENT_URI, new String[]{SimInfo._ID, SimInfo.SLOT}, selection, new String[]{ICCId}, null);
            try {
                if (cursor == null || !cursor.moveToFirst()) {
                    ContentValues values = new ContentValues();
                    values.put(SimInfo.ICC_ID, ICCId);
                    values.put(SimInfo.COLOR, -1);
                    values.put(SimInfo.SLOT, slot);
                    uri = resolver.insert(SimInfo.CONTENT_URI, values);
                    //setDefaultName(ctx, ContentUris.parseId(uri), null);
                } else {
                    long simId = cursor.getLong(0);
                    int oldSlot = cursor.getInt(1);
                    uri = ContentUris.withAppendedId(SimInfo.CONTENT_URI, simId);
                    if (slot != oldSlot) {
                        ContentValues values = new ContentValues(1);
                        values.put(SimInfo.SLOT, slot);
                        resolver.update(uri, values, null, null);
                    } 
                }
            } finally {
                if (cursor != null) {
                    cursor.close();
                }
            }
            
            return uri;
        }
        
        public static int setDefaultName(Context ctx, long simId, String name) {
            //#ifndef VENDOR_EDIT
            //Hong.Liu@CommApp.Telephony, 2012/02/13,Modify for
            /*
            if (simId <= 0)
                return ErrorCode.ERROR_GENERAL;
            String default_name = ctx.getString(SimInfo.DEFAULT_NAME_RES);
            ContentResolver resolver = ctx.getContentResolver();
            Uri uri = ContentUris.withAppendedId(SimInfo.CONTENT_URI, simId);
            if (name != null) {
                int result = setDisplayName(ctx, name, simId);
                if (result > 0) {
                    return result;
                }
            }
            int index = getAppropriateIndex(ctx, simId, name);
            String suffix = getSuffixFromIndex(index);
            ContentValues value = new ContentValues(1);
            String display_name = (name == null ? default_name + " " + suffix : name + " " + suffix);
            value.put(SimInfo.DISPLAY_NAME, display_name);
            return ctx.getContentResolver().update(uri, value, null, null);
            */
            //#else /* VENDOR_EDIT */
            if (simId <= 0 || name == null)
                return ErrorCode.ERROR_GENERAL;
                
            //String default_name = ctx.getString(SimInfo.DEFAULT_NAME_RES);
            //ContentResolver resolver = ctx.getContentResolver();
            //Uri uri = ContentUris.withAppendedId(SimInfo.CONTENT_URI, simId);

            if (name != null) {
                ContentValues value = new ContentValues(1);
                value.put(SimInfo.DISPLAY_NAME, name);
                return ctx.getContentResolver().update(ContentUris.withAppendedId(SimInfo.CONTENT_URI, simId), 
                        value, null, null);
            }

            return ErrorCode.ERROR_GENERAL;
            //#endif /* VENDOR_EDIT */
        }

        public static int setDefaultNameEx(Context ctx, long simId, String name, long nameSource) {
    //#ifndef VENDOR_EDIT
    //ChengJun.Duan@Prd.CommApp.Phone, 2013/03/06, Modify for 
    /*
            if (simId <= 0)
                return ErrorCode.ERROR_GENERAL;
            Log.i(TAG, "setDefaultNameEx SIMId " + simId + "displayName = " + name + "nameSource = " + nameSource);
            String default_name = ctx.getString(SimInfo.DEFAULT_NAME_RES);
            ContentResolver resolver = ctx.getContentResolver();
            Uri uri = ContentUris.withAppendedId(SimInfo.CONTENT_URI, simId);
            if (name != null) {
                int result = setDisplayNameEx(ctx, name, simId, nameSource);
                if (result > 0) {
                    return result;
                }
            }
            int index = getAppropriateIndex(ctx, simId, name);
            String suffix = getSuffixFromIndex(index);
            ContentValues value = new ContentValues(1);
            String display_name = (name == null ? default_name + " " + suffix : name + " " + suffix);
            value.put(SimInfo.DISPLAY_NAME, display_name);
            value.put(SimInfo.NAME_SOURCE, nameSource);
            return ctx.getContentResolver().update(uri, value, null, null);

    */
    //#else /* VENDOR_EDIT */
            return setDefaultName(ctx,simId,name);
    //#endif /* VENDOR_EDIT */
            
        }
        
        private static String getSuffixFromIndex(int index) {
            if (index < 10) {
                return "0" + index;
            } else {
                return String.valueOf(index);
            }
            
        }
        private static int getAppropriateIndex(Context ctx, long simId, String name) {
            String default_name = ctx.getString(SimInfo.DEFAULT_NAME_RES);
            StringBuilder sb = new StringBuilder(SimInfo.DISPLAY_NAME + " LIKE ");
            if (name == null) {
                DatabaseUtils.appendEscapedSQLString(sb, default_name + '%');
            } else {
                DatabaseUtils.appendEscapedSQLString(sb, name + '%');
            }
            sb.append(" AND (");
            sb.append(SimInfo._ID + "!=" + simId);
            sb.append(")");
            
            Cursor cursor = ctx.getContentResolver().query(SimInfo.CONTENT_URI, new String[]{SimInfo._ID, SimInfo.DISPLAY_NAME},
                    sb.toString(), null, SimInfo.DISPLAY_NAME);
            ArrayList<Long> array = new ArrayList<Long>();
            int index = SimInfo.DEFAULT_NAME_MIN_INDEX;
            if (cursor != null) {
                while (cursor.moveToNext()) {
                    String display_name = cursor.getString(1);
                    
                    if (display_name != null) {
                        int length = display_name.length();
                        if (length >= 2) {
                            String sub = display_name.substring(length -2);
                            if (TextUtils.isDigitsOnly(sub)) {
                                long value = Long.valueOf(sub);
                                array.add(value);
                            }
                        }
                    }
                }
                cursor.close();
            }
            for (int i = SimInfo.DEFAULT_NAME_MIN_INDEX; i <= SimInfo.DEFAULT_NAME_MAX_INDEX; i++) {
                if (array.contains((long)i)) {
                    continue;
                } else {
                    index = i;
                    break;
                }
            }
            return index;
        }
    }
    
    public static final class GprsInfo implements BaseColumns{
        public static final Uri CONTENT_URI = 
            Uri.parse("content://telephony/gprsinfo");
                
        /**
         * <P>Type: INTEGER</P>
         */
        public static final String SIM_ID = "sim_id";
        /**
         * <P>Type: INTEGER</P>
         */
        public static final String GPRS_IN = "gprs_in";
        /**
         * <P>Type: INTEGER</P>
         */
        public static final String GPRS_OUT = "gprs_out";
    }
    
    public static final class GPRSInfo {
        public long mSimId = 0;
        public long mGprsIn = 0;
        public long mGprsOut = 0;
        private GPRSInfo () {
            
        }
        private static GPRSInfo fromCursor (Cursor cursor) {
            GPRSInfo info = new GPRSInfo();
            info.mSimId = cursor.getLong(cursor.getColumnIndexOrThrow(GprsInfo.SIM_ID));
            info.mGprsIn = cursor.getLong(cursor.getColumnIndexOrThrow(GprsInfo.GPRS_IN));
            info.mGprsOut = cursor.getLong(cursor.getColumnIndexOrThrow(GprsInfo.GPRS_OUT));
            return info;
        }
        
        public static long getGprsInBySim(Context context, long simId) {
            if (simId <= 0) return 0;
            Cursor cursor = context.getContentResolver().query(GprsInfo.CONTENT_URI, 
                    new String[]{GprsInfo.GPRS_IN}, GprsInfo.SIM_ID + "=" + simId, null, null);
            try {
                if (cursor != null && cursor.moveToFirst()) {
                    return cursor.getLong(0);
                }
            } finally {
                if (cursor != null) {
                    cursor.close();
                }
            }
            return 0;
        }
        
        public static long getGprsOutBySim(Context context, long simId) {
            if (simId <= 0) return 0;
            Cursor cursor = context.getContentResolver().query(GprsInfo.CONTENT_URI, 
                    new String[]{GprsInfo.GPRS_OUT}, GprsInfo.SIM_ID + "=" + simId, null, null);
            try {
                if (cursor != null && cursor.moveToFirst()) {
                    return cursor.getLong(0);
                }
            } finally {
                if (cursor != null) {
                    cursor.close();
                }
            }
            return 0;
        }
        
        public static GPRSInfo getGprsInfoBySim(Context context, long simId) {
            GPRSInfo info = new GPRSInfo();
            if (simId <= 0) return info;
            Cursor cursor = context.getContentResolver().query(GprsInfo.CONTENT_URI, 
                    null, GprsInfo.SIM_ID + "=" + simId, null, null);
            try {
                if (cursor != null && cursor.moveToFirst()) {
                    return GPRSInfo.fromCursor(cursor);
                }
            } finally {
                if (cursor != null) {
                    cursor.close();
                }
            }
            return info;
        }
        
        public static int resetGprsBySim(Context context, long simId) {
            if (simId <= 0) return 0;
            ContentValues values = new ContentValues(2);
            values.put(GprsInfo.GPRS_IN, 0);
            values.put(GprsInfo.GPRS_OUT, 0);
            return context.getContentResolver().update(GprsInfo.CONTENT_URI, values, GprsInfo.SIM_ID + "=" + simId, null);
        }
    }
    //MTK-END [mtk04070][111121][ALPS00093395]MTK added

    /**
     * Contains received SMS cell broadcast messages.
     */
    public static final class CellBroadcasts implements BaseColumns {

        /** Not instantiable. */
        private CellBroadcasts() {}

        /**
         * The content:// style URL for this table
         */
        public static final Uri CONTENT_URI =
            Uri.parse("content://cellbroadcasts");

        /**
         * Message geographical scope.
         * <P>Type: INTEGER</P>
         */
        public static final String GEOGRAPHICAL_SCOPE = "geo_scope";

        /**
         * Message serial number.
         * <P>Type: INTEGER</P>
         */
        public static final String SERIAL_NUMBER = "serial_number";

        /**
         * PLMN of broadcast sender. (SERIAL_NUMBER + PLMN + LAC + CID) uniquely identifies a
         * broadcast for duplicate detection purposes.
         * <P>Type: TEXT</P>
         */
        public static final String PLMN = "plmn";

        /**
         * Location Area (GSM) or Service Area (UMTS) of broadcast sender. Unused for CDMA.
         * Only included if Geographical Scope of message is not PLMN wide (01).
         * <P>Type: INTEGER</P>
         */
        public static final String LAC = "lac";

        /**
         * Cell ID of message sender (GSM/UMTS). Unused for CDMA. Only included when the
         * Geographical Scope of message is cell wide (00 or 11).
         * <P>Type: INTEGER</P>
         */
        public static final String CID = "cid";

        /**
         * Message code (OBSOLETE: merged into SERIAL_NUMBER).
         * <P>Type: INTEGER</P>
         */
        public static final String V1_MESSAGE_CODE = "message_code";

        /**
         * Message identifier (OBSOLETE: renamed to SERVICE_CATEGORY).
         * <P>Type: INTEGER</P>
         */
        public static final String V1_MESSAGE_IDENTIFIER = "message_id";

        /**
         * Service category (GSM/UMTS message identifier, CDMA service category).
         * <P>Type: INTEGER</P>
         */
        public static final String SERVICE_CATEGORY = "service_category";

        /**
         * Message language code.
         * <P>Type: TEXT</P>
         */
        public static final String LANGUAGE_CODE = "language";

        /**
         * Message body.
         * <P>Type: TEXT</P>
         */
        public static final String MESSAGE_BODY = "body";

        /**
         * Message delivery time.
         * <P>Type: INTEGER (long)</P>
         */
        public static final String DELIVERY_TIME = "date";

        /**
         * Has the message been viewed?
         * <P>Type: INTEGER (boolean)</P>
         */
        public static final String MESSAGE_READ = "read";
        /**
         * add for gemini
         * The sim card id the messge come from.
         * <p>Type: INTEGER</p>
         */
        public static final String SIM_ID = "sim_id";
        /**
         * Message format (3GPP or 3GPP2).
         * <P>Type: INTEGER</P>
         */
        public static final String MESSAGE_FORMAT = "format";

        /**
         * Message priority (including emergency).
         * <P>Type: INTEGER</P>
         */
        public static final String MESSAGE_PRIORITY = "priority";

        /**
         * ETWS warning type (ETWS alerts only).
         * <P>Type: INTEGER</P>
         */
        public static final String ETWS_WARNING_TYPE = "etws_warning_type";

        /**
         * CMAS message class (CMAS alerts only).
         * <P>Type: INTEGER</P>
         */
        public static final String CMAS_MESSAGE_CLASS = "cmas_message_class";

        /**
         * CMAS category (CMAS alerts only).
         * <P>Type: INTEGER</P>
         */
        public static final String CMAS_CATEGORY = "cmas_category";

        /**
         * CMAS response type (CMAS alerts only).
         * <P>Type: INTEGER</P>
         */
        public static final String CMAS_RESPONSE_TYPE = "cmas_response_type";

        /**
         * CMAS severity (CMAS alerts only).
         * <P>Type: INTEGER</P>
         */
        public static final String CMAS_SEVERITY = "cmas_severity";

        /**
         * CMAS urgency (CMAS alerts only).
         * <P>Type: INTEGER</P>
         */
        public static final String CMAS_URGENCY = "cmas_urgency";

        /**
         * CMAS certainty (CMAS alerts only).
         * <P>Type: INTEGER</P>
         */
        public static final String CMAS_CERTAINTY = "cmas_certainty";

        /**
         * The default sort order for this table
         */
        public static final String DEFAULT_SORT_ORDER = DELIVERY_TIME + " DESC";

        /**
         * Query columns for instantiating {@link android.telephony.CellBroadcastMessage} objects.
         */
        public static final String[] QUERY_COLUMNS = {
                _ID,
                GEOGRAPHICAL_SCOPE,
                PLMN,
                LAC,
                CID,
                SERIAL_NUMBER,
                SERVICE_CATEGORY,
                LANGUAGE_CODE,
                MESSAGE_BODY,
                DELIVERY_TIME,
                MESSAGE_READ,
                SIM_ID,
                MESSAGE_FORMAT,
                MESSAGE_PRIORITY,
                ETWS_WARNING_TYPE,
                CMAS_MESSAGE_CLASS,
                CMAS_CATEGORY,
                CMAS_RESPONSE_TYPE,
                CMAS_SEVERITY,
                CMAS_URGENCY,
                CMAS_CERTAINTY
        };
    }

    public static final class Intents {
        private Intents() {
            // Not instantiable
        }

        /**
         * Broadcast Action: A "secret code" has been entered in the dialer. Secret codes are
         * of the form *#*#<code>#*#*. The intent will have the data URI:</p>
         *
         * <p><code>android_secret_code://&lt;code&gt;</code></p>
         */
        public static final String SECRET_CODE_ACTION =
                "android.provider.Telephony.SECRET_CODE";

        /**
         * Broadcast Action: The Service Provider string(s) have been updated.  Activities or
         * services that use these strings should update their display.
         * The intent will have the following extra values:</p>
         * <ul>
         *   <li><em>showPlmn</em> - Boolean that indicates whether the PLMN should be shown.</li>
         *   <li><em>plmn</em> - The operator name of the registered network, as a string.</li>
         *   <li><em>showSpn</em> - Boolean that indicates whether the SPN should be shown.</li>
         *   <li><em>spn</em> - The service provider name, as a string.</li>
         * </ul>
         * Note that <em>showPlmn</em> may indicate that <em>plmn</em> should be displayed, even
         * though the value for <em>plmn</em> is null.  This can happen, for example, if the phone
         * has not registered to a network yet.  In this case the receiver may substitute an
         * appropriate placeholder string (eg, "No service").
         *
         * It is recommended to display <em>plmn</em> before / above <em>spn</em> if
         * both are displayed.
         *
         * <p>Note this is a protected intent that can only be sent
         * by the system.
         */
        public static final String SPN_STRINGS_UPDATED_ACTION =
                "android.provider.Telephony.SPN_STRINGS_UPDATED";

        public static final String EXTRA_SHOW_PLMN  = "showPlmn";
        public static final String EXTRA_PLMN       = "plmn";
        public static final String EXTRA_SHOW_SPN   = "showSpn";
        public static final String EXTRA_SPN        = "spn";

        //MTK-START [mtk04070][111121][ALPS00093395]MTK added
        /**
         * Broadcast Action: Notify AP to pop up dual SIM mode selection menu for user
         *
         * <p>Note this is a protected intent that can only be sent
         * by the system.
         */
        public static final String ACTION_DUAL_SIM_MODE_SELECT =
                "android.provider.Telephony.DUAL_SIM_MODE_SELECT";

        /**
         * Broadcast Action: Notify AP to pop up GPRS selection menu for user
         *
         * <p>Note this is a protected intent that can only be sent
         * by the system.
         */
        public static final String ACTION_GPRS_CONNECTION_TYPE_SELECT =
                "android.provider.Telephony.GPRS_CONNECTION_TYPE_SELECT";
        /**
         * Broadcast Action: Unlock keyguard for user
         *
         * <p>Note this is a protected intent that can only be sent
         * by the system.
         */
        public static final String ACTION_UNLOCK_KEYGUARD =
                "android.provider.Telephony.UNLOCK_KEYGUARD";
        //MTK-END [mtk04070][111121][ALPS00093395]MTK added
        
        /**
        * @hide
        */
        public static final String ACTION_REMOVE_IDLE_TEXT = "android.intent.aciton.stk.REMOVE_IDLE_TEXT";
        
        /**
        * @hide
        */
        public static final String ACTION_REMOVE_IDLE_TEXT_2 = "android.intent.aciton.stk.REMOVE_IDLE_TEXT_2";
    }

    //MTK-START [mtk04070][111121][ALPS00093395]MTK added
    //Wap Push
    // WapPush table columns
    public static final class WapPush implements BaseColumns{
        
        //public static final Uri CONTENT_URI = 
        public static final String DEFAULT_SORT_ORDER = "date ASC";
        public static final Uri CONTENT_URI = Uri.parse("content://wappush");
        public static final Uri CONTENT_URI_SI = Uri.parse("content://wappush/si");
        public static final Uri CONTENT_URI_SL = Uri.parse("content://wappush/sl");
        public static final Uri CONTENT_URI_THREAD = Uri.parse("content://wappush/thread_id");
        
        //Database Columns
        public static final String THREAD_ID = "thread_id";
        public static final String ADDR = "address";
        public static final String SERVICE_ADDR = "service_center";
        public static final String READ = "read";
        public static final String SEEN = "seen";
        public static final String LOCKED = "locked";
        public static final String ERROR = "error";
        public static final String DATE = "date";
        public static final String TYPE = "type";
        public static final String SIID = "siid";
        public static final String URL = "url";
        public static final String CREATE = "created";
        public static final String EXPIRATION = "expiration";
        public static final String ACTION = "action";
        public static final String TEXT = "text";
        public static final String SIM_ID = "sim_id";
        
        //
        public static final int TYPE_SI = 0;
        public static final int TYPE_SL = 1;
        
        public static final int STATUS_SEEN = 1;
        public static final int STATUS_UNSEEN = 0;
        
        public static final int STATUS_READ = 1;
        public static final int STATUS_UNREAD = 0;
        
        public static final int STATUS_LOCKED = 1;
        public static final int STATUS_UNLOCKED = 0;
    }
    //MTK-END [mtk04070][111121][ALPS00093395]MTK added
}
