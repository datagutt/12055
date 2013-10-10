/*
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
 **
 ** limitations under the License.
 */

#ifndef OPPO_ANDROID_MEDIARECORDER_H
#define OPPO_ANDROID_MEDIARECORDER_H

#include <utils/Log.h>
#include <utils/threads.h>
#include <utils/List.h>
#include <utils/Errors.h>

#include "OppoStagefrightRecorder.h"

namespace android {

class Surface;
//class IMediaRecorder;
class ICamera;
class ICameraRecordingProxy;
class ISurfaceTexture;
class SurfaceTextureClient;
class StagefrightRecorder;

typedef void (*media_completion_f)(status_t status, void *cookie);


// ----------------------------------------------------------------------------
//class MediaRecorder : public BnMediaRecorderClient,
//                      public virtual IMediaDeathNotifier
};  // namespace android

#endif // ANDROID_MEDIARECORDER_H
