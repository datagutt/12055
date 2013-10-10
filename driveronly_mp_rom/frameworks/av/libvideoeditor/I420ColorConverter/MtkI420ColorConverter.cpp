/*
 * Copyright (C) 2011 The Android Open Source Project
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

#include <II420ColorConverter.h>
#include <OMX_IVCommon.h>
#include <string.h>
#include <utils/Log.h>
#include "DpBlitStream.h"
#define LOG_TAG "MTKI420CC"

#define MEM_ALIGN_32 32
#define ROUND_16(X)     ((X + 0xF) & (~0xF))
#define ROUND_32(X)     ((X + 0x1F) & (~0x1F))
#define YUV_SIZE(W,H)   (W * H * 3 >> 1)

static int Mtk_getDecoderOutputFormat() {
    return OMX_COLOR_FormatVendorMTKYUV;
}

static int Mtk_convertDecoderOutputToI420(
    void* srcBits, int srcWidth, int srcHeight, ARect srcRect, void* dstBits) {

    //ALOGD ("@@ convert srcWidth(%d), srcHeight(%d)", srcWidth, srcHeight);
    uint8_t   *srcYUVbuf_va = NULL;
    uint32_t  srcWStride = ROUND_16(srcWidth);
    uint32_t  srcHStride = ROUND_32(srcHeight);
    uint32_t  srcBufferSize = srcWStride * srcHStride * 3 >> 1;

    uint32_t dstWidth = ROUND_16(srcWidth);// srcRect.right - srcRect.left + 1;
    uint32_t dstHeight = ROUND_16(srcHeight);// srcRect.bottom - srcRect.top + 1;
    uint32_t  dstBufferSize = dstWidth * dstHeight * 3 >> 1;

    srcBufferSize = ROUND_32(YUV_SIZE(srcWStride, srcHStride));
    srcYUVbuf_va = (uint8_t *)memalign(MEM_ALIGN_32, srcBufferSize);    // 32 byte alignment for MDP

    uint8_t* srcYUVbufArray[2];
    unsigned int srcYUVbufSizeArray[2];
    srcYUVbufArray[0] = srcYUVbuf_va;      // Y
    srcYUVbufArray[1] = srcYUVbuf_va + srcWStride * srcHStride;  // C
    srcYUVbufSizeArray[0] = srcWStride * srcHStride;
    srcYUVbufSizeArray[1] = srcWStride * srcHStride / 2;
 
    memcpy(srcYUVbuf_va, (uint8_t*)srcBits, YUV_SIZE(srcWStride, srcHStride));

    uint8_t* dstYUVbufArray[3];
    unsigned int dstYUVbufSizeArray[3];
    dstYUVbufArray[0] = (uint8_t*)dstBits;
    dstYUVbufArray[1] = dstYUVbufArray[0] + (dstWidth * dstHeight);
    dstYUVbufArray[2] = dstYUVbufArray[1] + (dstWidth * dstHeight) /4;
    dstYUVbufSizeArray[0] = dstWidth * dstHeight;
    dstYUVbufSizeArray[1] = (dstWidth * dstHeight) /4;
    dstYUVbufSizeArray[2] = (dstWidth * dstHeight) /4;

    DpBlitStream blitStream;
    DpColorFormat srcColorFormat = eNV12_BLK;
    DpColorFormat dstColorFormat = eYUV_420_3P;

    ALOGD("srcRect.left(%d), srcRect.top(%d), srcRect.right(%d), srcRect.bottom(%d), srcBufferSize(%d), srcColorFormat(%d), dstBufferSize(%d), dstColorFormat(%d)",
    	         srcRect.left, srcRect.top, srcRect.right, srcRect.bottom, srcBufferSize, srcColorFormat, dstBufferSize, dstColorFormat);

#if 0 // dump input
            static int count = 0;
            char buf[255];
            ALOGD ("@@ convert srcWidth(%d), srcHeight(%d), srcWStride(%d), srcHStride(%d)", srcWidth, srcHeight, srcWStride, srcHStride);
            // Y
            sprintf (buf, "/sdcard/blkout/out_%d_%d_%d.raw", srcWStride, srcHStride, count);
            FILE *fpY = fopen(buf, "ab");
            if (fpY) {
                fwrite((void *)srcYUVbufArray[0], 1,  srcYUVbufSizeArray[0], fpY);
                fclose(fpY);
            }
            // C
            sprintf (buf, "/sdcard/blkout/out_%d_%d_%d.raw", srcWStride, srcHStride, count);
            FILE *fpC = fopen(buf, "ab");
            if (fpC) {
                fwrite((void *)srcYUVbufArray[1], 1, srcYUVbufSizeArray[1], fpC);
                fclose(fpC);
            }
#endif

    DpRect srcRoi;
    srcRoi.x = srcRect.left;
    srcRoi.y = srcRect.top;
    srcRoi.w = srcRect.right - srcRect.left + 1;
    srcRoi.h = srcRect.bottom - srcRect.top + 1;
    blitStream.setSrcBuffer((void**)srcYUVbufArray, (unsigned int*)srcYUVbufSizeArray, 2);
    blitStream.setSrcConfig(srcWStride, srcHStride, srcColorFormat, eInterlace_None, &srcRoi);

    DpRect dstRoi;
    dstRoi.x = 0;
    dstRoi.y = 0;
    dstRoi.w = srcWidth;
    dstRoi.h = srcHeight;
    blitStream.setDstBuffer((void**)dstYUVbufArray, (unsigned int*)dstYUVbufSizeArray, 3);
    blitStream.setDstConfig(dstWidth, dstHeight, dstColorFormat, eInterlace_None, &dstRoi);
    blitStream.invalidate();

#if 0 // dump output
            char bufOut[255];
            sprintf (bufOut, "/sdcard/out_%d_%d_%d.yuv", dstWidth, dstHeight, count);
            FILE *fp = fopen(bufOut, "ab");
            if (fp) {
                fwrite((void *)dstYUVbufArray[0], 1, dstBufferSize, fp);
                fclose(fp);
            }
            count++;
#endif

    if (srcYUVbuf_va) {
        free (srcYUVbuf_va);
        srcYUVbuf_va = NULL;
    }

    return 0;
}

static int Mtk_getEncoderInputFormat() {
    return OMX_COLOR_FormatYUV420Planar;
}

static int Mtk_convertI420ToEncoderInput(
    void* srcBits, int srcWidth, int srcHeight,
    int dstWidth, int dstHeight, ARect dstRect,
    void* dstBits) {

    return 0;
}

static int Mtk_getEncoderInputBufferInfo(
    int actualWidth, int actualHeight,
    int* encoderWidth, int* encoderHeight,
    ARect* encoderRect, int* encoderBufferSize) {

    *encoderWidth = actualWidth;
    *encoderHeight = actualHeight;
    encoderRect->left = 0;
    encoderRect->top = 0;
    encoderRect->right = actualWidth - 1;
    encoderRect->bottom = actualHeight - 1;
    *encoderBufferSize = (actualWidth * actualHeight * 3 / 2);

    return 0;
}

extern "C" void getI420ColorConverter(II420ColorConverter *converter) {
    converter->getDecoderOutputFormat = Mtk_getDecoderOutputFormat;
    converter->convertDecoderOutputToI420 = Mtk_convertDecoderOutputToI420;
    converter->getEncoderInputFormat = Mtk_getEncoderInputFormat;
    converter->convertI420ToEncoderInput = Mtk_convertI420ToEncoderInput;
    converter->getEncoderInputBufferInfo = Mtk_getEncoderInputBufferInfo;
}
