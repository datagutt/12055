/*
**
** Copyright (C) 2008 The Android Open Source Project
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

#ifndef ANDROID_MEDIAMETADATARETRIEVERINTERFACE_H
#define ANDROID_MEDIAMETADATARETRIEVERINTERFACE_H

#include <utils/RefBase.h>
#include <private/media/VideoFrame.h>

namespace android {

// Keep these in synch with the constants defined in MediaMetadataRetriever.java
// class.
enum {
    METADATA_KEY_CD_TRACK_NUMBER = 0,
    METADATA_KEY_ALBUM           = 1,
    METADATA_KEY_ARTIST          = 2,
    METADATA_KEY_AUTHOR          = 3,
    METADATA_KEY_COMPOSER        = 4,
    METADATA_KEY_DATE            = 5,
    METADATA_KEY_GENRE           = 6,
    METADATA_KEY_TITLE           = 7,
    METADATA_KEY_YEAR            = 8,
    METADATA_KEY_DURATION        = 9,
    METADATA_KEY_NUM_TRACKS      = 10,
    METADATA_KEY_WRITER          = 11,
    METADATA_KEY_MIMETYPE        = 12,
    METADATA_KEY_ALBUMARTIST     = 13,
    METADATA_KEY_DISC_NUMBER     = 14,
    METADATA_KEY_COMPILATION     = 15,
    METADATA_KEY_HAS_AUDIO       = 16,
    METADATA_KEY_HAS_VIDEO       = 17,
    METADATA_KEY_VIDEO_WIDTH     = 18,
    METADATA_KEY_VIDEO_HEIGHT    = 19,
    METADATA_KEY_BITRATE         = 20,
    METADATA_KEY_TIMED_TEXT_LANGUAGES      = 21,
    METADATA_KEY_IS_DRM          = 22,
    METADATA_KEY_LOCATION        = 23,

    // Add more here...
};

// Abstract base class
class MediaMetadataRetrieverBase : public RefBase
{
public:
                        MediaMetadataRetrieverBase() {}
    virtual             ~MediaMetadataRetrieverBase() {}

    virtual status_t    setDataSource(
            const char *url,
            const KeyedVector<String8, String8> *headers = NULL) = 0;

    virtual status_t    setDataSource(int fd, int64_t offset, int64_t length) = 0;
    virtual VideoFrame* getFrameAtTime(int64_t timeUs, int option) = 0;
    virtual MediaAlbumArt* extractAlbumArt() = 0;
    virtual const char* extractMetadata(int keyCode) = 0;
};

// MediaMetadataRetrieverInterface
class MediaMetadataRetrieverInterface : public MediaMetadataRetrieverBase
{
public:
    MediaMetadataRetrieverInterface() {}

    virtual             ~MediaMetadataRetrieverInterface() {}
    virtual VideoFrame* getFrameAtTime(int64_t timeUs, int option) { return NULL; }
    virtual MediaAlbumArt* extractAlbumArt() { return NULL; }
    virtual const char* extractMetadata(int keyCode) { return NULL; }
    virtual void setLocale(const char* locale){};
};

}; // namespace android

#endif // ANDROID_MEDIAMETADATARETRIEVERINTERFACE_H
