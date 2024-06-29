/*
 * UVCCamera
 * library and sample to access to UVC web camera on non-rooted Android device
 *
 * Copyright (c) 2014-2017 saki t_saki@serenegiant.com
 *
 * File name: UVCPreview.h
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 * All files in the folder are under this Apache License, Version 2.0.
 * Files in the jni/libjpeg, jni/libusb, jin/libuvc, jni/rapidjson folder may have a different license, see the respective files.
*/

#ifndef UVCPREVIEW_H_
#define UVCPREVIEW_H_

#include "libUVCCamera.h"
#include <pthread.h>
#include <android/native_window.h>
#include "objectarray.h"
#include "RotateImage.h"
#include <media/NdkMediaCodec.h>
#include <media/NdkMediaFormat.h>

#pragma interface

#define FRAME_FORMAT_YUYV 0
#define FRAME_FORMAT_MJPEG 1
#define FRAME_FORMAT_H264 2

#define H264_CODEC_MIME "video/avc"

#define DEFAULT_PREVIEW_WIDTH 640
#define DEFAULT_PREVIEW_HEIGHT 480
#define DEFAULT_PREVIEW_FPS_MIN 1
#define DEFAULT_PREVIEW_FPS_MAX 30
#define DEFAULT_FRAME_ROTATION_ANGLE 0

typedef uvc_error_t (*convFunc_t)(uvc_frame_t *in, uvc_frame_t *out);

#define PIXEL_FORMAT_RAW 0		// same as PIXEL_FORMAT_YUV
#define PIXEL_FORMAT_YUV 1
#define PIXEL_FORMAT_RGB565 2
#define PIXEL_FORMAT_RGBX 3
#define PIXEL_FORMAT_YUV20SP 4
#define PIXEL_FORMAT_NV21 5		// YVU420SemiPlanar

// for callback to Java object
typedef struct {
	jmethodID onFrame;
} Fields_iframecallback;

class UVCPreview {
private:
	uvc_device_handle_t *mDeviceHandle;
	// preview view
	ANativeWindow *mPreviewWindow;
	volatile bool mIsRunning;
	int requestWidth, requestHeight, frameFormat;
	int requestMinFps, requestMaxFps;
	int frameWidth, frameHeight;
	// The angle at which the image frame needs to be rotated
	int frameRotationAngle;
	// Whether to mirror horizontally
	int frameHorizontalMirror;
	// Whether to mirror vertically
	int frameVerticalMirror;
	RotateImage *rotateImage;
	pthread_t preview_thread;
	pthread_mutex_t preview_mutex;
	pthread_cond_t preview_sync;
	// Preview frame array, the obtained camera data frames are placed here
	ObjectArray<uvc_frame_t *> previewFrames;
	// Preview format
	int32_t previewFormat;
	// fps
	struct timespec tStart, tEnd;
	int framesCounter, currentFps, defaultCameraFps;
//
	volatile bool mIsCapturing;
	ANativeWindow *mCaptureWindow;
	pthread_t capture_thread;
	pthread_mutex_t capture_mutex;
	pthread_cond_t capture_sync;
	// Capture frame
	uvc_frame_t *captureQueu;			// keep latest frame
	// Frame callback Java object
	jobject mFrameCallbackObj;
	// Pixel format conversion method
	convFunc_t mFrameCallbackFunc;
	convFunc_t mPreviewConvertFunc;
	Fields_iframecallback iframecallback_fields;
	// Frame callback pixel format
	int mPixelFormat;
	size_t callbackPixelBytes;
// improve performance by reducing memory allocation
	pthread_mutex_t pool_mutex;
	// frame pool
	ObjectArray<uvc_frame_t *> mFramePool;
	uvc_frame_t *get_frame(size_t data_bytes);
	void recycle_frame(uvc_frame_t *frame);
	void init_pool(size_t data_bytes);
	void clear_pool();
//
	void clearDisplay();
	static convFunc_t getConvertFunc(uvc_frame_t *frame, int32_t outputPixelFormat);
	static void uvc_preview_frame_callback(uvc_frame_t *frame, void *vptr_args);
	void addPreviewFrame(uvc_frame_t *frame);
	uvc_frame_t *waitPreviewFrame();
	void clearPreviewFrame();
	static void *preview_thread_func(void *vptr_args);
	int prepare_preview(uvc_stream_ctrl_t *ctrl);
	void do_preview(uvc_stream_ctrl_t *ctrl);
	uvc_frame_t *draw_preview_one(uvc_frame_t *frame, ANativeWindow **window, convFunc_t func, int pixelBytes);
//
	void addCaptureFrame(uvc_frame_t *frame);
	uvc_frame_t *waitCaptureFrame();
	void clearCaptureFrame();
	static void *capture_thread_func(void *vptr_args);
	void do_capture(JNIEnv *env);
	void do_capture_surface(JNIEnv *env);
	void do_capture_idle_loop(JNIEnv *env);
	void do_capture_callback(JNIEnv *env, uvc_frame_t *frame);
	void callbackPixelFormatChanged();
	uvc_frame_format getUvcFrameFormat(int mode);
	// decoder for H264 format
	AMediaCodec *decoder = nullptr;
	AMediaFormat *decoderFormat = nullptr;
	int decoderColorFormat = 0;
	pthread_t decoderOutputThread;
	pthread_t decoderConvertThread;
	ObjectArray<uvc_frame_t *> decoderOutputFrames;
	pthread_mutex_t decoder_mutex;
	pthread_cond_t decoder_sync;
	static void *decoder_output_func(void *vptr_args);
	static void *decoder_convert_func(void *vptr_args);
	void processDecoderOutput();
	void processDecoderConvert();
	int startDecoder();
	void stopDecoder();
	void addDecoderFrame(uvc_frame_t *frame);
	uvc_frame_t *waitDecoderFrame();
	void clearDecoderFrame();
public:
	UVCPreview(uvc_device_handle_t *devh);
	~UVCPreview();

	inline const bool isRunning() const {return mIsRunning; };
	int setPreviewSize(int width, int height, int cameraAngle, int min_fps, int max_fps, int mode);
	int setPreviewDisplay(ANativeWindow *preview_window);
	int setFrameCallback(JNIEnv *env, jobject frame_callback_obj, int pixel_format);
	int startPreview();
	int stopPreview();
	inline const bool isCapturing() const;
	int setCaptureDisplay(ANativeWindow *capture_window);

	void setHorizontalMirror(int horizontalMirror);
	void setVerticalMirror(int verticalMirror);
	void setCameraAngle(int cameraAngle);

	int getCurrentFps();
	int getDefaultCameraFps();
	int getFrameWidth();
	int getFrameHeight();
};

#endif /* UVCPREVIEW_H_ */
