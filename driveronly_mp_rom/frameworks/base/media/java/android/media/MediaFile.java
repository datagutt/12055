/*
 * Copyright (C) 2007 The Android Open Source Project
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

package android.media;

import android.content.ContentValues;
import android.provider.MediaStore.Audio;
import android.provider.MediaStore.Images;
import android.provider.MediaStore.Video;
import android.media.DecoderCapabilities;
import android.media.DecoderCapabilities.VideoDecoder;
import android.media.DecoderCapabilities.AudioDecoder;
import android.mtp.MtpConstants;

import java.util.HashMap;
import java.util.Iterator;
import java.util.List;

import com.mediatek.common.featureoption.FeatureOption;

//#ifdef VENDOR_EDIT
//WangHu@Prd.Audio.MediaProvider, 2013/08/31, Add for new file types
import android.annotation.OppoHook;
import android.annotation.OppoHook.*;

import android.provider.Settings;
import android.net.Uri;
import android.content.Context;
import android.util.Log;
//#endif /* VENDOR_EDIT */

/**
 * MediaScanner helper class.
 *
 * {@hide}
 */
public class MediaFile {

    // Audio file types
    public static final int FILE_TYPE_MP3     = 1;
    public static final int FILE_TYPE_M4A     = 2;
    public static final int FILE_TYPE_WAV     = 3;
    public static final int FILE_TYPE_AMR     = 4;
    public static final int FILE_TYPE_AWB     = 5;
    public static final int FILE_TYPE_WMA     = 6;
    public static final int FILE_TYPE_OGG     = 7;
    public static final int FILE_TYPE_AAC     = 8;
    public static final int FILE_TYPE_MKA     = 9;
    public static final int FILE_TYPE_FLAC    = 10;
    private static final int FIRST_AUDIO_FILE_TYPE = FILE_TYPE_MP3;
    private static final int LAST_AUDIO_FILE_TYPE = FILE_TYPE_FLAC;
    
    // FFMPEG support extra audio & video type
    public static final int FILE_TYPE_APE     = 1001;
    public static final int FILE_TYPE_MP2     = 1002;
    public static final int FILE_TYPE_AC3     = 1003;
    public static final int FILE_TYPE_RA      = 1004;
    
    
    private static final int FIRST_FFMPEG_AUDIO_FILE_TYPE = FILE_TYPE_APE;
    private static final int LAST_FFMPEG_AUDIO_FILE_TYPE = FILE_TYPE_RA;
    
    public static final int FILE_TYPE_FLV     = 1101;
    public static final int FILE_TYPE_RV      = 1102;
    public static final int FILE_TYPE_MOV     = 1103;
    public static final int FILE_TYPE_M2TS    = 1104;
    private static final int FIRST_FFMPEG_VIDEO_FILE_TYPE = FILE_TYPE_FLV;
    private static final int LAST_FFMPEG_VIDEO_FILE_TYPE = FILE_TYPE_M2TS;
    // MIDI file types
    public static final int FILE_TYPE_MID     = 11;
    public static final int FILE_TYPE_SMF     = 12;
    public static final int FILE_TYPE_IMY     = 13;
    private static final int FIRST_MIDI_FILE_TYPE = FILE_TYPE_MID;
    private static final int LAST_MIDI_FILE_TYPE = FILE_TYPE_IMY;
   
    // Video file types
    public static final int FILE_TYPE_MP4     = 21;
    public static final int FILE_TYPE_M4V     = 22;
    public static final int FILE_TYPE_3GPP    = 23;
    public static final int FILE_TYPE_3GPP2   = 24;
    public static final int FILE_TYPE_WMV     = 25;
    public static final int FILE_TYPE_ASF     = 26;
    public static final int FILE_TYPE_MKV     = 27;
    public static final int FILE_TYPE_MP2TS   = 28;
    public static final int FILE_TYPE_AVI     = 29;
    public static final int FILE_TYPE_WEBM    = 30;
    private static final int FIRST_VIDEO_FILE_TYPE = FILE_TYPE_MP4;
    private static final int LAST_VIDEO_FILE_TYPE = FILE_TYPE_WEBM;
    
    // More video file types
    public static final int FILE_TYPE_MP2PS   = 200;
    private static final int FIRST_VIDEO_FILE_TYPE2 = FILE_TYPE_MP2PS;
    private static final int LAST_VIDEO_FILE_TYPE2 = FILE_TYPE_MP2PS;

    // Image file types
    public static final int FILE_TYPE_JPEG    = 31;
    public static final int FILE_TYPE_GIF     = 32;
    public static final int FILE_TYPE_PNG     = 33;
    public static final int FILE_TYPE_BMP     = 34;
    public static final int FILE_TYPE_WBMP    = 35;
    public static final int FILE_TYPE_WEBP    = 36;
    private static final int FIRST_IMAGE_FILE_TYPE = FILE_TYPE_JPEG;
    private static final int LAST_IMAGE_FILE_TYPE = FILE_TYPE_WEBP;

    /// M: More image types
    public static final int FILE_TYPE_JPS     = 498;
    public static final int FILE_TYPE_MPO     = 499;
    private static final int FIRST_MORE_IMAGE_FILE_TYPE = FILE_TYPE_JPS;
    private static final int LAST_MORE_IMAGE_FILE_TYPE = FILE_TYPE_MPO;
    
    // Playlist file types
    public static final int FILE_TYPE_M3U      = 41;
    public static final int FILE_TYPE_PLS      = 42;
    public static final int FILE_TYPE_WPL      = 43;
    public static final int FILE_TYPE_HTTPLIVE = 44;

    private static final int FIRST_PLAYLIST_FILE_TYPE = FILE_TYPE_M3U;
    private static final int LAST_PLAYLIST_FILE_TYPE = FILE_TYPE_HTTPLIVE;

    // Drm file types
    public static final int FILE_TYPE_FL      = 51;
    private static final int FIRST_DRM_FILE_TYPE = FILE_TYPE_FL;
    private static final int LAST_DRM_FILE_TYPE = FILE_TYPE_FL;

    // Other popular file types
    public static final int FILE_TYPE_TEXT          = 100;
    public static final int FILE_TYPE_HTML          = 101;
    public static final int FILE_TYPE_PDF           = 102;
    public static final int FILE_TYPE_XML           = 103;
    public static final int FILE_TYPE_MS_WORD       = 104;
    public static final int FILE_TYPE_MS_EXCEL      = 105;
    public static final int FILE_TYPE_MS_POWERPOINT = 106;
    public static final int FILE_TYPE_ZIP           = 107;
    
    public static class MediaFileType {
        public final int fileType;
        public final String mimeType;
        
        MediaFileType(int fileType, String mimeType) {
            this.fileType = fileType;
            this.mimeType = mimeType;
        }
    }
    
    private static final HashMap<String, MediaFileType> sFileTypeMap
            = new HashMap<String, MediaFileType>();
    private static final HashMap<String, Integer> sMimeTypeMap
            = new HashMap<String, Integer>();
    // maps file extension to MTP format code
    private static final HashMap<String, Integer> sFileTypeToFormatMap
            = new HashMap<String, Integer>();
    // maps mime type to MTP format code
    private static final HashMap<String, Integer> sMimeTypeToFormatMap
            = new HashMap<String, Integer>();
    // maps MTP format code to mime type
    private static final HashMap<Integer, String> sFormatToMimeTypeMap
            = new HashMap<Integer, String>();

    static void addFileType(String extension, int fileType, String mimeType) {
        sFileTypeMap.put(extension, new MediaFileType(fileType, mimeType));
        sMimeTypeMap.put(mimeType, Integer.valueOf(fileType));
    }

    static void addFileType(String extension, int fileType, String mimeType, int mtpFormatCode) {
        addFileType(extension, fileType, mimeType);
        sFileTypeToFormatMap.put(extension, Integer.valueOf(mtpFormatCode));
        sMimeTypeToFormatMap.put(mimeType, Integer.valueOf(mtpFormatCode));
        sFormatToMimeTypeMap.put(mtpFormatCode, mimeType);
    }

    private static boolean isWMAEnabled() {
        List<AudioDecoder> decoders = DecoderCapabilities.getAudioDecoders();
        int count = decoders.size();
        for (int i = 0; i < count; i++) {
            AudioDecoder decoder = decoders.get(i);
            if (decoder == AudioDecoder.AUDIO_DECODER_WMA) {
                return true;
            }
        }
        return false;
    }

    private static boolean isWMVEnabled() {
        List<VideoDecoder> decoders = DecoderCapabilities.getVideoDecoders();
        int count = decoders.size();
        for (int i = 0; i < count; i++) {
            VideoDecoder decoder = decoders.get(i);
            if (decoder == VideoDecoder.VIDEO_DECODER_WMV) {
                return true;
            }
        }
        return false;
    }

    static {
        addFileType("MP3", FILE_TYPE_MP3, "audio/mpeg", MtpConstants.FORMAT_MP3);
        addFileType("MPGA", FILE_TYPE_MP3, "audio/mpeg", MtpConstants.FORMAT_MP3);
        addFileType("M4A", FILE_TYPE_M4A, "audio/mp4", MtpConstants.FORMAT_MPEG);
        addFileType("WAV", FILE_TYPE_WAV, "audio/x-wav", MtpConstants.FORMAT_WAV);
        addFileType("AMR", FILE_TYPE_AMR, "audio/amr");
        addFileType("AWB", FILE_TYPE_AWB, "audio/amr-wb");

        /// M: for media file whose MIME type is 'audio/3gpp'
        //addFileType("3GP", FILE_TYPE_3GPP3, "audio/3gpp");
        /// M: ALPS00689526 @{
        addFileType("MP2", FILE_TYPE_MP2PS, "video/mp2p");
        /// @}
        
        //addFileType("MOV", FILE_TYPE_QUICKTIME_AUDIO, "audio/quicktime");
       // addFileType("QT", FILE_TYPE_QUICKTIME_AUDIO, "audio/quicktime");
        
        if (FeatureOption.MTK_DRM_APP) {
            addFileType("DCF", FILE_TYPE_MP3, "audio/mpeg");
        }

        //#ifndef VENDOR_EDIT
        //WangHu@Prd.Audio.MediaProvider, 2013/08/31, Modify for: Always identify WMA
        /*
        if (isWMAEnabled() && FeatureOption.MTK_ASF_PLAYBACK_SUPPORT) {
            addFileType("WMA", FILE_TYPE_WMA, "audio/x-ms-wma", MtpConstants.FORMAT_WMA);
        }
        */
        //#else /* VENDOR_EDIT */
        addFileType("WMA", FILE_TYPE_WMA, "audio/x-ms-wma", MtpConstants.FORMAT_WMA);
        //#endif /* VENDOR_EDIT */

        /// M: Adds mimetype audio/vorbis.
        addFileType("OGG", FILE_TYPE_OGG, "audio/vorbis", MtpConstants.FORMAT_OGG);
        addFileType("OGG", FILE_TYPE_OGG, "audio/ogg", MtpConstants.FORMAT_OGG);
        addFileType("OGG", FILE_TYPE_OGG, "application/ogg", MtpConstants.FORMAT_OGG);
        addFileType("OGA", FILE_TYPE_OGG, "application/ogg", MtpConstants.FORMAT_OGG);

        //if (FeatureOption.MTK_OGM_PLAYBACK_SUPPORT) { 
        //    addFileType("OGV", FILE_TYPE_OGG, "video/ogm", MtpConstants.FORMAT_OGG);
        //    addFileType("OGM", FILE_TYPE_OGG, "video/ogm", MtpConstants.FORMAT_OGG);
        //}
        
        addFileType("AAC", FILE_TYPE_AAC, "audio/aac", MtpConstants.FORMAT_AAC);
        addFileType("AAC", FILE_TYPE_AAC, "audio/aac-adts", MtpConstants.FORMAT_AAC);
        addFileType("MKA", FILE_TYPE_MKA, "audio/x-matroska");
 
        addFileType("MID", FILE_TYPE_MID, "audio/midi");
        addFileType("MIDI", FILE_TYPE_MID, "audio/midi");
        addFileType("XMF", FILE_TYPE_MID, "audio/midi");
        addFileType("RTTTL", FILE_TYPE_MID, "audio/midi");
        addFileType("SMF", FILE_TYPE_SMF, "audio/sp-midi");
        addFileType("IMY", FILE_TYPE_IMY, "audio/imelody");
        addFileType("RTX", FILE_TYPE_MID, "audio/midi");
        addFileType("OTA", FILE_TYPE_MID, "audio/midi");
        addFileType("MXMF", FILE_TYPE_MID, "audio/midi");
        
        addFileType("MPEG", FILE_TYPE_MP4, "video/mpeg", MtpConstants.FORMAT_MPEG);
        addFileType("MPG", FILE_TYPE_MP4, "video/mpeg", MtpConstants.FORMAT_MPEG);
        addFileType("MP4", FILE_TYPE_MP4, "video/mp4", MtpConstants.FORMAT_MPEG);
        addFileType("M4V", FILE_TYPE_M4V, "video/mp4", MtpConstants.FORMAT_MPEG);
        addFileType("3GP", FILE_TYPE_3GPP, "video/3gpp",  MtpConstants.FORMAT_3GP_CONTAINER);
        addFileType("3GPP", FILE_TYPE_3GPP, "video/3gpp", MtpConstants.FORMAT_3GP_CONTAINER);
        addFileType("3G2", FILE_TYPE_3GPP2, "video/3gpp2", MtpConstants.FORMAT_3GP_CONTAINER);
        addFileType("3GPP2", FILE_TYPE_3GPP2, "video/3gpp2", MtpConstants.FORMAT_3GP_CONTAINER);
        addFileType("MKV", FILE_TYPE_MKV, "video/x-matroska");
        addFileType("WEBM", FILE_TYPE_WEBM, "video/webm");
        addFileType("TS", FILE_TYPE_MP2TS, "video/mp2ts");
        /// M: support mts file extension @{
        addFileType("MTS", FILE_TYPE_MP2TS, "video/mp2ts");
        /// @}
        addFileType("M2TS", FILE_TYPE_MP2TS, "video/mp2ts");
        addFileType("AVI", FILE_TYPE_AVI, "video/avi");
        //addFileType("MOV", FILE_TYPE_QUICKTIME_VIDEO, "video/quicktime");
        //addFileType("QT", FILE_TYPE_QUICKTIME_VIDEO, "video/quicktime");
         
        //#ifndef VENDOR_EDIT
        //ChenTieQun@Plf.MediaApp.MediaProvider, 2012/09/29, Modify for: Always identify those file types
        /*
        if (FeatureOption.MTK_TB_DEBUG_SUPPORT) {
            addFileType("RA", FILE_TYPE_RA, "audio/x-pn-realaudio");
            addFileType("RM", FILE_TYPE_RM, "video/x-pn-realvideo");
            addFileType("RV", FILE_TYPE_RM, "video/x-pn-realvideo");
            addFileType("RMVB", FILE_TYPE_RMVB, "video/x-pn-realvideo");
        }

        if (FeatureOption.MTK_FLV_PLAYBACK_SUPPORT) {
            addFileType("FLV", FILE_TYPE_FLV, "video/x-flv");
            addFileType("FLA", FILE_TYPE_FLA, "audio/x-flv");
        }
        /// @}

        if (isWMVEnabled() && FeatureOption.MTK_ASF_PLAYBACK_SUPPORT) {
            addFileType("WMV", FILE_TYPE_WMV, "video/x-ms-wmv", MtpConstants.FORMAT_WMV);
            addFileType("ASF", FILE_TYPE_ASF, "video/x-ms-asf");
        }
		*/
        //#else /* VENDOR_EDIT */
        addFileType("RA", FILE_TYPE_RA, "audio/x-pn-realaudio");
        //addFileType("RM", FILE_TYPE_RM, "video/x-pn-realvideo");
        //addFileType("RV", FILE_TYPE_RM, "video/x-pn-realvideo");
        //addFileType("RMVB", FILE_TYPE_RMVB, "video/x-pn-realvideo");
        addFileType("FLV", FILE_TYPE_FLV, "video/x-flv");
        //addFileType("FLA", FILE_TYPE_FLA, "audio/x-flv");
        addFileType("WMV", FILE_TYPE_WMV, "video/x-ms-wmv", MtpConstants.FORMAT_WMV);
        addFileType("ASF", FILE_TYPE_ASF, "video/x-ms-asf");
        //#endif /* VENDOR_EDIT */

        addFileType("JPG", FILE_TYPE_JPEG, "image/jpeg", MtpConstants.FORMAT_EXIF_JPEG);
        addFileType("JPEG", FILE_TYPE_JPEG, "image/jpeg", MtpConstants.FORMAT_EXIF_JPEG);
        addFileType("GIF", FILE_TYPE_GIF, "image/gif", MtpConstants.FORMAT_GIF);
        addFileType("PNG", FILE_TYPE_PNG, "image/png", MtpConstants.FORMAT_PNG);
        addFileType("BMP", FILE_TYPE_BMP, "image/x-ms-bmp", MtpConstants.FORMAT_BMP);
        addFileType("WBMP", FILE_TYPE_WBMP, "image/vnd.wap.wbmp");
        addFileType("WEBP", FILE_TYPE_WEBP, "image/webp");
        /// M: Mpo files should not be scanned as images in BSP. ALPS00332560 @{
        if (!FeatureOption.MTK_BSP_PACKAGE) {
            addFileType("MPO", FILE_TYPE_MPO, "image/mpo");
        }
        /// @}

        /// M: Jps files should not be scanned as images when S3D is disabled. @{
        if (FeatureOption.MTK_S3D_SUPPORT) {
            addFileType("JPS", FILE_TYPE_JPS, "image/x-jps");
        }
        /// @}
 
        addFileType("M3U", FILE_TYPE_M3U, "audio/x-mpegurl", MtpConstants.FORMAT_M3U_PLAYLIST);
        addFileType("M3U", FILE_TYPE_M3U, "application/x-mpegurl", MtpConstants.FORMAT_M3U_PLAYLIST);
        addFileType("PLS", FILE_TYPE_PLS, "audio/x-scpls", MtpConstants.FORMAT_PLS_PLAYLIST);
        addFileType("WPL", FILE_TYPE_WPL, "application/vnd.ms-wpl", MtpConstants.FORMAT_WPL_PLAYLIST);
        addFileType("M3U8", FILE_TYPE_HTTPLIVE, "application/vnd.apple.mpegurl");
        addFileType("M3U8", FILE_TYPE_HTTPLIVE, "audio/mpegurl");
        addFileType("M3U8", FILE_TYPE_HTTPLIVE, "audio/x-mpegurl");

        addFileType("FL", FILE_TYPE_FL, "application/x-android-drm-fl");

        addFileType("TXT", FILE_TYPE_TEXT, "text/plain", MtpConstants.FORMAT_TEXT);
        addFileType("HTM", FILE_TYPE_HTML, "text/html", MtpConstants.FORMAT_HTML);
        addFileType("HTML", FILE_TYPE_HTML, "text/html", MtpConstants.FORMAT_HTML);
        addFileType("PDF", FILE_TYPE_PDF, "application/pdf");
        addFileType("DOC", FILE_TYPE_MS_WORD, "application/msword", MtpConstants.FORMAT_MS_WORD_DOCUMENT);
        addFileType("XLS", FILE_TYPE_MS_EXCEL, "application/vnd.ms-excel", MtpConstants.FORMAT_MS_EXCEL_SPREADSHEET);
        addFileType("PPT", FILE_TYPE_MS_POWERPOINT, "application/mspowerpoint", MtpConstants.FORMAT_MS_POWERPOINT_PRESENTATION);
        addFileType("FLAC", FILE_TYPE_FLAC, "audio/flac", MtpConstants.FORMAT_FLAC);
        addFileType("ZIP", FILE_TYPE_ZIP, "application/zip");
        addFileType("MPG", FILE_TYPE_MP2PS, "video/mp2p");
        addFileType("MPEG", FILE_TYPE_MP2PS, "video/mp2p");
        
        addFileType("APE", FILE_TYPE_APE, "audio/ape");
        addFileType("MP2",	FILE_TYPE_MP2,  "audio/mpeg");
        addFileType("AC3",	FILE_TYPE_AC3,  "audio/ac3");
        addFileType("RA",	FILE_TYPE_RA,  "audio/x-pn-realaudio");
        addFileType("FLV", FILE_TYPE_FLV, "video/x-flv");
        addFileType("F4V", FILE_TYPE_FLV, "video/x-flv");
        addFileType("RMVB", FILE_TYPE_RV, "video/x-pn-realvideo");
        addFileType("RM", FILE_TYPE_RV, "video/x-pn-realvideo");
        addFileType("RV", FILE_TYPE_RV, "video/x-pn-realvideo");
        addFileType("MOV", FILE_TYPE_MOV, "video/x-quicktime");
        addFileType("M2TS", FILE_TYPE_M2TS, "video/m2ts");
        addFileType("DOCX", FILE_TYPE_MS_WORD, "application/msword", MtpConstants.FORMAT_MS_WORD_DOCUMENT);
        addFileType("XLSX", FILE_TYPE_MS_EXCEL, "application/vnd.ms-excel", MtpConstants.FORMAT_MS_EXCEL_SPREADSHEET);
        addFileType("PPTX", FILE_TYPE_MS_POWERPOINT, "application/mspowerpoint", MtpConstants.FORMAT_MS_POWERPOINT_PRESENTATION);
    }

    @OppoHook(level=OppoHookType.CHANGE_CODE,
		     property=OppoRomType.OPPO, note="additional audio type")	
    public static boolean isAudioFileType(int fileType) {
        return ((fileType >= FIRST_AUDIO_FILE_TYPE &&
                fileType <= LAST_AUDIO_FILE_TYPE) ||
                (fileType >= FIRST_MIDI_FILE_TYPE &&
                fileType <= LAST_MIDI_FILE_TYPE) ||
                (fileType >= FIRST_FFMPEG_AUDIO_FILE_TYPE &&
                        fileType <= LAST_FFMPEG_AUDIO_FILE_TYPE));
    }

    @OppoHook(level=OppoHookType.CHANGE_CODE,
		     property=OppoRomType.OPPO, note="additional video type")
    public static boolean isVideoFileType(int fileType) {
        return (fileType >= FIRST_VIDEO_FILE_TYPE &&
                fileType <= LAST_VIDEO_FILE_TYPE)
            || (fileType >= FIRST_VIDEO_FILE_TYPE2 &&
                fileType <= LAST_VIDEO_FILE_TYPE2)
            || (fileType >= FIRST_FFMPEG_VIDEO_FILE_TYPE &&
                fileType <= LAST_FFMPEG_VIDEO_FILE_TYPE);
    }
    
    public static boolean isImageFileType(int fileType) {
        return (fileType >= FIRST_IMAGE_FILE_TYPE &&
                fileType <= LAST_IMAGE_FILE_TYPE)
            || (fileType >= FIRST_MORE_IMAGE_FILE_TYPE &&
                fileType <= LAST_MORE_IMAGE_FILE_TYPE);
    }
    
    public static boolean isPlayListFileType(int fileType) {
        return (fileType >= FIRST_PLAYLIST_FILE_TYPE &&
                fileType <= LAST_PLAYLIST_FILE_TYPE);
    }

    public static boolean isDrmFileType(int fileType) {
        return (fileType >= FIRST_DRM_FILE_TYPE &&
                fileType <= LAST_DRM_FILE_TYPE);
    }

    public static MediaFileType getFileType(String path) {
        int lastDot = path.lastIndexOf(".");
        if (lastDot < 0)
            return null;
        return sFileTypeMap.get(path.substring(lastDot + 1).toUpperCase());
    }

    public static boolean isMimeTypeMedia(String mimeType) {
        int fileType = getFileTypeForMimeType(mimeType);
        return isAudioFileType(fileType) || isVideoFileType(fileType)
                || isImageFileType(fileType) || isPlayListFileType(fileType);
    }

    // generates a title based on file name
    public static String getFileTitle(String path) {
        // extract file name after last slash
        int lastSlash = path.lastIndexOf('/');
        if (lastSlash >= 0) {
            lastSlash++;
            if (lastSlash < path.length()) {
                path = path.substring(lastSlash);
            }
        }
        // truncate the file extension (if any)
        int lastDot = path.lastIndexOf('.');
        if (lastDot > 0) {
            path = path.substring(0, lastDot);
        }
        return path;
    }

    public static int getFileTypeForMimeType(String mimeType) {
        Integer value = sMimeTypeMap.get(mimeType);
        return (value == null ? 0 : value.intValue());
    }

    public static String getMimeTypeForFile(String path) {
        MediaFileType mediaFileType = getFileType(path);
        return (mediaFileType == null ? null : mediaFileType.mimeType);
    }

    public static int getFormatCode(String fileName, String mimeType) {
        if (mimeType != null) {
            Integer value = sMimeTypeToFormatMap.get(mimeType);
            if (value != null) {
                return value.intValue();
            }
        }
        int lastDot = fileName.lastIndexOf('.');
        if (lastDot > 0) {
            String extension = fileName.substring(lastDot + 1).toUpperCase();
            Integer value = sFileTypeToFormatMap.get(extension);
            if (value != null) {
                return value.intValue();
            }
        }
        return MtpConstants.FORMAT_UNDEFINED;
    }

    public static String getMimeTypeForFormatCode(int formatCode) {
        return sFormatToMimeTypeMap.get(formatCode);
    }

    /**
     * {@hide}
     */
    public static int getFileTypeBySuffix(String filename){
        MediaFileType mdeiaFileType = getFileType(filename);
        if(null == mdeiaFileType){
            return -1;
        }
        return mdeiaFileType.fileType;
    }

    /**
     * {@hide}
     */
    public static String getMimeTypeBySuffix(String filename){
        MediaFileType mdeiaFileType = getFileType(filename);
        if(null == mdeiaFileType){
            return null;
        }
        return mdeiaFileType.mimeType;
    }

	//#ifdef VENDOR_EDIT
    //Chentiequn@Plf.MediaApp.MediaProvider, 2012/12/18, Add for 
	@OppoHook(level=OppoHookType.NEW_FIELD,
		     property=OppoRomType.OPPO, note="oppo default uri name")
    public final static String OPPO_DEFAULT_RINGTONE = "oppo_default_ringtone";
	
	@OppoHook(level=OppoHookType.NEW_FIELD,
		     property=OppoRomType.OPPO, note="oppo default uri name")
    public final static String OPPO_DEFAULT_ALARM = "oppo_default_alarm";
	
	@OppoHook(level=OppoHookType.NEW_FIELD,
		     property=OppoRomType.OPPO, note="oppo default uri name")	
    public final static String OPPO_DEFAULT_NOTIFICATION = "oppo_default_notification";
	// add for the secondary SIM card default ringtone & notification.
	@OppoHook(level=OppoHookType.NEW_FIELD,
		     property=OppoRomType.OPPO, note="oppo default uri name")	
	public final static String OPPO_DEFAULT_RINGTONE_SIM2 = "oppo_default_ringtone_sim2";
	
	@OppoHook(level=OppoHookType.NEW_FIELD,
		     property=OppoRomType.OPPO, note="oppo default uri name")
	public final static String OPPO_DEFAULT_NOTIFICATION_SIM2 = "oppo_default_notification_sim2";
	
	@OppoHook(level=OppoHook.OppoHookType.NEW_METHOD,
		     property=OppoRomType.OPPO, note="get oppo default ringtone uri")
    public static Uri getDefaultRingtoneUri(Context context) {
        return getUriFor(context,  OPPO_DEFAULT_RINGTONE);
    }
 	@OppoHook(level=OppoHook.OppoHookType.NEW_METHOD,
		     property=OppoRomType.OPPO, note="get oppo default alarm uri")
    public static Uri getDefaultAlarmUri(Context context) {
        return getUriFor(context,  OPPO_DEFAULT_ALARM);
    }
	@android.annotation.OppoHook(level=OppoHook.OppoHookType.NEW_METHOD,
		     property=OppoRomType.OPPO, note="get oppo default notification uri")
	public static Uri getDefaultNotificationUri(Context context) {
        return getUriFor(context,  OPPO_DEFAULT_NOTIFICATION);
    }
	
	@OppoHook(level=OppoHook.OppoHookType.NEW_METHOD,
		     property=OppoRomType.OPPO, note="get oppo default ringtone uri for sim2")
	public static Uri getDefaultRingtoneUriSIM2(Context context) {
        return getUriFor(context,  OPPO_DEFAULT_RINGTONE_SIM2);
    }
	@OppoHook(level=OppoHook.OppoHookType.NEW_METHOD,
		     property=OppoRomType.OPPO, note="get oppo default notificaiton uri for sim2")
	public static Uri getDefaultNotificationUriSIM2(Context context) {
        return getUriFor(context,  OPPO_DEFAULT_NOTIFICATION_SIM2);
    }

	@OppoHook(level=OppoHook.OppoHookType.NEW_METHOD,
		     property=OppoRomType.OPPO, note="get oppo default  uris ")
    private static Uri getUriFor(Context context, String name) {

		String value = Settings.System.getString(context.getContentResolver() , name);
        if(null != value) {
            return Uri.parse(value);
        } else {
            Log.e("MediaFile", name + " not set?!!!");
        }
        return null;
    }
	
	@OppoHook(level=OppoHookType.NEW_FIELD,
		     property=OppoRomType.OPPO, note="flag for scanning all files on storage")
    public final static int SCAN_ALL_FILE = 0;
	
	@OppoHook(level=OppoHookType.NEW_FIELD,
		     property=OppoRomType.OPPO, note="flag for scanning audio files on storage")
    public final static int SCAN_AUDIO_FILE = 1;
	
	@OppoHook(level=OppoHookType.NEW_FIELD,
		     property=OppoRomType.OPPO, note="flag for scanning image files on storage")
    public final static int SCAN_IMAGE_FILE = 2;
	
	@OppoHook(level=OppoHookType.NEW_FIELD,
		     property=OppoRomType.OPPO, note="flag for scanning video files on storage")
    public final static int SCAN_VIDEO_FILE = 4;
	
	@OppoHook(level=OppoHookType.NEW_FIELD,
		     property=OppoRomType.OPPO, note="flag for scanning other files on storage" +
		     		" like *.apk, *.csv, *.vcf, *.ics, *.vcs")
    public final static int SCAN_OTHER_FILE = 8;
	//#endif /* VENDOR_EDIT */
}
