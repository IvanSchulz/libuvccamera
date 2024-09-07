/*
 * UVCCamera
 * library and sample to access to UVC web camera on non-rooted Android device
 *
 * Copyright (c) 2014-2017 saki t_saki@serenegiant.com
 *
 * File name: serenegiant_usb_UVCCamera.cpp
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

#if 1	// When debugging information is not output
	#ifndef LOG_NDEBUG
		#define	LOG_NDEBUG		// LOGV/LOGD/MARK - No output
		#endif
	#undef USE_LOGALL			// Only output the specified LOGx
#else
	#define USE_LOGALL
	#undef LOG_NDEBUG
	#undef NDEBUG
#endif

#include <jni.h>
#include <android/native_window_jni.h>

#include "libUVCCamera.h"
#include "UVCCamera.h"
#include "libuvc/libuvc_internal.h"

/**
 * set the value into the long field
 * @param env: this param should not be null
 * @param bullet_obj: this param should not be null
 * @param field_name
 * @params val
 */
static jlong setField_long(JNIEnv *env, jobject java_obj, const char *field_name, jlong val) {
#if LOCAL_DEBUG
	LOGV("setField_long:");
#endif

	jclass clazz = env->GetObjectClass(java_obj);
	jfieldID field = env->GetFieldID(clazz, field_name, "J");
	if (LIKELY(field))
		env->SetLongField(java_obj, field, val);
	else {
		LOGE("__setField_long:field '%s' not found", field_name);
	}
#ifdef ANDROID_NDK
	env->DeleteLocalRef(clazz);
#endif
	return val;
}

/**
 * @param env: this param should not be null
 * @param bullet_obj: this param should not be null
 */
static jlong __setField_long(JNIEnv *env, jobject java_obj, jclass clazz, const char *field_name, jlong val) {
#if LOCAL_DEBUG
	LOGV("__setField_long:");
#endif

	jfieldID field = env->GetFieldID(clazz, field_name, "J");
	if (LIKELY(field))
		env->SetLongField(java_obj, field, val);
	else {
		LOGE("__setField_long:field '%s' not found", field_name);
	}
	return val;
}

/**
 * @param env: this param should not be null
 * @param bullet_obj: this param should not be null
 */
jint __setField_int(JNIEnv *env, jobject java_obj, jclass clazz, const char *field_name, jint val) {
	LOGV("__setField_int:");

	jfieldID id = env->GetFieldID(clazz, field_name, "I");
	if (LIKELY(id))
		env->SetIntField(java_obj, id, val);
	else {
		LOGE("__setField_int:field '%s' not found", field_name);
		env->ExceptionClear();	// clear java.lang.NoSuchFieldError exception
	}
	return val;
}

/**
 * set the value into int field
 * @param env: this param should not be null
 * @param java_obj: this param should not be null
 * @param field_name
 * @params val
 */
jint setField_int(JNIEnv *env, jobject java_obj, const char *field_name, jint val) {
	LOGV("setField_int:");

	jclass clazz = env->GetObjectClass(java_obj);
	__setField_int(env, java_obj, clazz, field_name, val);
#ifdef ANDROID_NDK
	env->DeleteLocalRef(clazz);
#endif
	return val;
}

extern "C"
JNIEXPORT jlong JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeCreate(JNIEnv *env, jobject thiz) {
    ENTER();
    UVCCamera *camera = new UVCCamera();
    setField_long(env, thiz, "mNativePtr", reinterpret_cast<ID_TYPE>(camera));
    RETURN(reinterpret_cast<ID_TYPE>(camera), ID_TYPE);
}

extern "C"
JNIEXPORT void JNICALL
// Destroy the native camera object
Java_com_serenegiant_libuvccamera_UVCCamera_nativeDestroy(JNIEnv *env, jobject thiz,
                                                          jlong id_camera) {
    ENTER();
    setField_long(env, thiz, "mNativePtr", 0);
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        SAFE_DELETE(camera);
    }
    EXIT();
}

extern "C"
JNIEXPORT jint JNICALL
// Connect camera
Java_com_serenegiant_libuvccamera_UVCCamera_nativeConnect(JNIEnv *env, jobject thiz,
                                                          jlong id_camera, jint vender_id,
                                                          jint product_id, jint file_descriptor,
                                                          jstring usbfs) {
    ENTER();
    int result = JNI_ERR;
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    const char *c_usbfs = env->GetStringUTFChars(usbfs, JNI_FALSE);
    if (LIKELY(camera && (file_descriptor > 0))) {
//		libusb_set_debug(NULL, LIBUSB_LOG_LEVEL_DEBUG);
        result = camera->connect(vender_id, product_id, file_descriptor, c_usbfs);
    }
    env->ReleaseStringUTFChars(usbfs, c_usbfs);
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
// Disconnect from camera
Java_com_serenegiant_libuvccamera_UVCCamera_nativeRelease(JNIEnv *env, jclass clazz,
                                                          jlong id_camera) {
    ENTER();
    int result = JNI_ERR;
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->release();
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeSetStatusCallback(JNIEnv *env, jclass clazz,
                                                                    jlong id_camera,
                                                                    jobject callback) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        jobject status_callback_obj = env->NewGlobalRef(callback);
        result = camera->setStatusCallback(env, status_callback_obj);
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeSetButtonCallback(JNIEnv *env, jclass clazz,
                                                                    jlong id_camera,
                                                                    jobject callback) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        jobject button_callback_obj = env->NewGlobalRef(callback);
        result = camera->setButtonCallback(env, button_callback_obj);
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
// Set preview screen size
Java_com_serenegiant_libuvccamera_UVCCamera_nativeSetPreviewSize(JNIEnv *env, jclass clazz,
                                                                 jlong id_camera, jint width,
                                                                 jint height, jint camera_angle,
                                                                 jint min_fps, jint max_fps,
                                                                 jint mode) {
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        return camera->setPreviewSize(width, height, camera_angle, min_fps, max_fps, mode);
    }
    RETURN(JNI_ERR, jint);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeGetSupportedSize(JNIEnv *env, jclass clazz,
                                                                   jlong id_camera) {
    ENTER();
    jstring result = NULL;
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        char *c_str = camera->getSupportedSize();
        if (LIKELY(c_str)) {
            result = env->NewStringUTF(c_str);
            free(c_str);
        }
    }
    RETURN(result, jstring);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeStartPreview(JNIEnv *env, jclass clazz,
                                                               jlong id_camera) {
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        return camera->startPreview();
    }
    RETURN(JNI_ERR, jint);
}

extern "C"
JNIEXPORT jint JNICALL
// Stop preview
Java_com_serenegiant_libuvccamera_UVCCamera_nativeStopPreview(JNIEnv *env, jclass clazz,
                                                              jlong id_camera) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->stopPreview();
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeSetPreviewDisplay(JNIEnv *env, jclass clazz,
                                                                    jlong id_camera,
                                                                    jobject surface) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        ANativeWindow *preview_window = surface ? ANativeWindow_fromSurface(env, surface) : NULL;
        result = camera->setPreviewDisplay(preview_window);
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeSetFrameCallback(JNIEnv *env, jclass clazz,
                                                                   jlong id_camera,
                                                                   jobject callback,
                                                                   jint pixel_format) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        jobject frame_callback_obj = env->NewGlobalRef(callback);
        result = camera->setFrameCallback(env, frame_callback_obj, pixel_format);
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeSetCaptureDisplay(JNIEnv *env, jclass clazz,
                                                                    jlong id_camera,
                                                                    jobject surface) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        ANativeWindow *capture_window = surface ? ANativeWindow_fromSurface(env, surface) : NULL;
        result = camera->setCaptureDisplay(capture_window);
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jlong JNICALL
// Get camera control supported functions
Java_com_serenegiant_libuvccamera_UVCCamera_nativeGetCtrlSupports(JNIEnv *env, jclass clazz,
                                                                  jlong id_camera) {
    jlong result = 0;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        uint64_t supports;
        int r = camera->getCtrlSupports(&supports);
        if (!r)
            result = supports;
    }
    RETURN(result, jlong);
}

extern "C"
JNIEXPORT jlong JNICALL
// Get the functions supported by the processing unit
Java_com_serenegiant_libuvccamera_UVCCamera_nativeGetProcSupports(JNIEnv *env, jclass clazz,
                                                                  jlong id_camera) {
    jlong result = 0;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        uint64_t supports;
        int r = camera->getProcSupports(&supports);
        if (!r)
            result = supports;
    }
    RETURN(result, jlong);
}

extern "C"
JNIEXPORT jint JNICALL
// Java method correspond to this function should not be a static method
Java_com_serenegiant_libuvccamera_UVCCamera_nativeUpdateScanningModeLimit(JNIEnv *env, jobject thiz,
                                                                          jlong id_camera) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateScanningModeLimit(min, max, def);
        if (!result) {
            // Write to Java side
            setField_int(env, thiz, "mScanningModeMin", min);
            setField_int(env, thiz, "mScanningModeMax", max);
            setField_int(env, thiz, "mScanningModeDef", def);
        }
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeSetScanningMode(JNIEnv *env, jclass clazz,
                                                                  jlong id_camera,
                                                                  jint scanning_mode) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setScanningMode(scanning_mode);
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeGetScanningMode(JNIEnv *env, jclass clazz,
                                                                  jlong id_camera) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getScanningMode();
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
// Java method correspond to this function should not be a static method
Java_com_serenegiant_libuvccamera_UVCCamera_nativeUpdateExposureModeLimit(JNIEnv *env, jobject thiz,
                                                                          jlong id_camera) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateExposureModeLimit(min, max, def);
        if (!result) {
            // Write to Java side
            setField_int(env, thiz, "mExposureModeMin", min);
            setField_int(env, thiz, "mExposureModeMax", max);
            setField_int(env, thiz, "mExposureModeDef", def);
        }
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeSetExposureMode(JNIEnv *env, jclass clazz,
                                                                  jlong id_camera,
                                                                  jint exposure_mode) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setExposureMode(exposure_mode);
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeGetExposureMode(JNIEnv *env, jclass clazz,
                                                                  jlong id_camera) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getExposureMode();
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
// Java method correspond to this function should not be a static method
Java_com_serenegiant_libuvccamera_UVCCamera_nativeUpdateExposurePriorityLimit(JNIEnv *env,
                                                                              jobject thiz,
                                                                              jlong id_camera) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateExposurePriorityLimit(min, max, def);
        if (!result) {
            // Write to Java side
            setField_int(env, thiz, "mExposurePriorityMin", min);
            setField_int(env, thiz, "mExposurePriorityMax", max);
            setField_int(env, thiz, "mExposurePriorityDef", def);
        }
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeSetExposurePriority(JNIEnv *env, jclass clazz,
                                                                      jlong id_camera,
                                                                      jint priority) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setExposurePriority(priority);
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeGetExposurePriority(JNIEnv *env, jclass clazz,
                                                                      jlong id_camera) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getExposurePriority();
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
// Java method correspond to this function should not be a static method
Java_com_serenegiant_libuvccamera_UVCCamera_nativeUpdateExposureLimit(JNIEnv *env, jobject thiz,
                                                                      jlong id_camera) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateExposureLimit(min, max, def);
        if (!result) {
            // Write to Java side
            setField_int(env, thiz, "mExposureMin", min);
            setField_int(env, thiz, "mExposureMax", max);
            setField_int(env, thiz, "mExposureDef", def);
        }
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeSetExposure(JNIEnv *env, jclass clazz,
                                                              jlong id_camera, jint exposure) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setExposure(exposure);
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeGetExposure(JNIEnv *env, jclass clazz,
                                                              jlong id_camera) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getExposure();
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
// Java method correspond to this function should not be a static method
Java_com_serenegiant_libuvccamera_UVCCamera_nativeUpdateExposureRelLimit(JNIEnv *env, jobject thiz,
                                                                         jlong id_camera) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateExposureRelLimit(min, max, def);
        if (!result) {
            // Write to Java side
            setField_int(env, thiz, "mExposureRelMin", min);
            setField_int(env, thiz, "mExposureRelMax", max);
            setField_int(env, thiz, "mExposureRelDef", def);
        }
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeSetExposureRel(JNIEnv *env, jclass clazz,
                                                                 jlong id_camera,
                                                                 jint exposure_rel) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setExposureRel(exposure_rel);
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeGetExposureRel(JNIEnv *env, jclass clazz,
                                                                 jlong id_camera) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getExposureRel();
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
// Java method correspond to this function should not be a static method
Java_com_serenegiant_libuvccamera_UVCCamera_nativeUpdateAutoFocusLimit(JNIEnv *env, jobject thiz,
                                                                       jlong id_camera) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateAutoFocusLimit(min, max, def);
        if (!result) {
            // Write to Java side
            setField_int(env, thiz, "mAutoFocusMin", min);
            setField_int(env, thiz, "mAutoFocusMax", max);
            setField_int(env, thiz, "mAutoFocusDef", def);
        }
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeSetAutoFocus(JNIEnv *env, jclass clazz,
                                                               jlong id_camera,
                                                               jboolean autofocus) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setAutoFocus(autofocus);
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeGetAutoFocus(JNIEnv *env, jclass clazz,
                                                               jlong id_camera) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getAutoFocus();
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
// Java method correspond to this function should not be a static method
Java_com_serenegiant_libuvccamera_UVCCamera_nativeUpdateFocusLimit(JNIEnv *env, jobject thiz,
                                                                   jlong id_camera) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateFocusLimit(min, max, def);
        if (!result) {
            // Write to Java side
            setField_int(env, thiz, "mFocusMin", min);
            setField_int(env, thiz, "mFocusMax", max);
            setField_int(env, thiz, "mFocusDef", def);
        }
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeSetFocus(JNIEnv *env, jclass clazz,
                                                           jlong id_camera, jint focus) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setFocus(focus);
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeGetFocus(JNIEnv *env, jclass clazz,
                                                           jlong id_camera) {
    jint result = 0;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getFocus();
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
// Java method correspond to this function should not be a static method
Java_com_serenegiant_libuvccamera_UVCCamera_nativeUpdateFocusRelLimit(JNIEnv *env, jobject thiz,
                                                                      jlong id_camera) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateFocusRelLimit(min, max, def);
        if (!result) {
            // Write to Java side
            setField_int(env, thiz, "mFocusRelMin", min);
            setField_int(env, thiz, "mFocusRelMax", max);
            setField_int(env, thiz, "mFocusRelDef", def);
        }
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeSetFocusRel(JNIEnv *env, jclass clazz,
                                                              jlong id_camera, jint focus_rel) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setFocusRel(focus_rel);
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeGetFocusRel(JNIEnv *env, jclass clazz,
                                                              jlong id_camera) {
    jint result = 0;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getFocusRel();
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
// Java method correspond to this function should not be a static method
Java_com_serenegiant_libuvccamera_UVCCamera_nativeUpdateIrisLimit(JNIEnv *env, jobject thiz,
                                                                  jlong id_camera) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateIrisLimit(min, max, def);
        if (!result) {
            // Write to Java side
            setField_int(env, thiz, "mIrisMin", min);
            setField_int(env, thiz, "mIrisMax", max);
            setField_int(env, thiz, "mIrisDef", def);
        }
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeSetIris(JNIEnv *env, jclass clazz,
                                                          jlong id_camera, jint iris) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setIris(iris);
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeGetIris(JNIEnv *env, jclass clazz,
                                                          jlong id_camera) {
    jint result = 0;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getIris();
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
// Java method correspond to this function should not be a static method
Java_com_serenegiant_libuvccamera_UVCCamera_nativeUpdateIrisRelLimit(JNIEnv *env, jobject thiz,
                                                                     jlong id_camera) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateIrisRelLimit(min, max, def);
        if (!result) {
            // Write to Java side
            setField_int(env, thiz, "mIrisRelMin", min);
            setField_int(env, thiz, "mIrisRelMax", max);
            setField_int(env, thiz, "mIrisRelDef", def);
        }
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeSetIrisRel(JNIEnv *env, jclass clazz,
                                                             jlong id_camera, jint iris_rel) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setIrisRel(iris_rel);
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeGetIrisRel(JNIEnv *env, jclass clazz,
                                                             jlong id_camera) {
    jint result = 0;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getIrisRel();
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
// Java method correspond to this function should not be a static method
Java_com_serenegiant_libuvccamera_UVCCamera_nativeUpdatePanLimit(JNIEnv *env, jobject thiz,
                                                                 jlong id_camera) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updatePanLimit(min, max, def);
        if (!result) {
            // Write to Java side
            setField_int(env, thiz, "mPanMin", min);
            setField_int(env, thiz, "mPanMax", max);
            setField_int(env, thiz, "mPanDef", def);
        }
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeSetPan(JNIEnv *env, jclass clazz, jlong id_camera,
                                                         jint pan) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setPan(pan);
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeGetPan(JNIEnv *env, jclass clazz,
                                                         jlong id_camera) {
    jint result = 0;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getPan();
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
// Java method correspond to this function should not be a static method
Java_com_serenegiant_libuvccamera_UVCCamera_nativeUpdatePanRelLimit(JNIEnv *env, jobject thiz,
                                                                    jlong id_camera) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updatePanRelLimit(min, max, def);
        if (!result) {
            // Write to Java side
            setField_int(env, thiz, "mPanRelMin", min);
            setField_int(env, thiz, "mPanRelMax", max);
            setField_int(env, thiz, "mPanRelDef", def);
        }
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeSetPanRel(JNIEnv *env, jclass clazz,
                                                            jlong id_camera, jint pan_rel) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setPanRel(pan_rel);
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeGetPanRel(JNIEnv *env, jclass clazz,
                                                            jlong id_camera) {
    jint result = 0;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getPanRel();
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
// Java method correspond to this function should not be a static method
Java_com_serenegiant_libuvccamera_UVCCamera_nativeUpdateTiltLimit(JNIEnv *env, jobject thiz,
                                                                  jlong id_camera) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateTiltLimit(min, max, def);
        if (!result) {
            // Write to Java side
            setField_int(env, thiz, "mTiltMin", min);
            setField_int(env, thiz, "mTiltMax", max);
            setField_int(env, thiz, "mTiltDef", def);
        }
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeSetTilt(JNIEnv *env, jclass clazz,
                                                          jlong id_camera, jint tilt) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setTilt(tilt);
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeGetTilt(JNIEnv *env, jclass clazz,
                                                          jlong id_camera) {
    jint result = 0;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getTilt();
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
// Java method correspond to this function should not be a static method
Java_com_serenegiant_libuvccamera_UVCCamera_nativeUpdateTiltRelLimit(JNIEnv *env, jobject thiz,
                                                                     jlong id_camera) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateTiltRelLimit(min, max, def);
        if (!result) {
            // Write to Java side
            setField_int(env, thiz, "mTiltRelMin", min);
            setField_int(env, thiz, "mTiltRelMax", max);
            setField_int(env, thiz, "mTiltRelDef", def);
        }
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeSetTiltRel(JNIEnv *env, jclass clazz,
                                                             jlong id_camera, jint tilt_rel) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setTiltRel(tilt_rel);
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeGetTiltRel(JNIEnv *env, jclass clazz,
                                                             jlong id_camera) {
    jint result = 0;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getTiltRel();
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
// Java method correspond to this function should not be a static method
Java_com_serenegiant_libuvccamera_UVCCamera_nativeUpdateRollLimit(JNIEnv *env, jobject thiz,
                                                                  jlong id_camera) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateRollLimit(min, max, def);
        if (!result) {
            // Write to Java side
            setField_int(env, thiz, "mRollMin", min);
            setField_int(env, thiz, "mRollMax", max);
            setField_int(env, thiz, "mRollDef", def);
        }
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeSetRoll(JNIEnv *env, jclass clazz,
                                                          jlong id_camera, jint roll) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setRoll(roll);
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeGetRoll(JNIEnv *env, jclass clazz,
                                                          jlong id_camera) {
    jint result = 0;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getRoll();
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
// Java method correspond to this function should not be a static method
Java_com_serenegiant_libuvccamera_UVCCamera_nativeUpdateRollRelLimit(JNIEnv *env, jobject thiz,
                                                                     jlong id_camera) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateRollRelLimit(min, max, def);
        if (!result) {
            // Write to Java side
            setField_int(env, thiz, "mRollRelMin", min);
            setField_int(env, thiz, "mRollRelMax", max);
            setField_int(env, thiz, "mRollRelDef", def);
        }
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeSetRollRel(JNIEnv *env, jclass clazz,
                                                             jlong id_camera, jint roll_rel) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setRollRel(roll_rel);
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeGetRollRel(JNIEnv *env, jclass clazz,
                                                             jlong id_camera) {
    jint result = 0;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getRollRel();
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
// Java method correspond to this function should not be a static method
Java_com_serenegiant_libuvccamera_UVCCamera_nativeUpdateAutoWhiteBlanceLimit(JNIEnv *env,
                                                                             jobject thiz,
                                                                             jlong id_camera) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateAutoWhiteBlanceLimit(min, max, def);
        if (!result) {
            // Write to Java side
            setField_int(env, thiz, "mAutoWhiteBlanceMin", min);
            setField_int(env, thiz, "mAutoWhiteBlanceMax", max);
            setField_int(env, thiz, "mAutoWhiteBlanceDef", def);
        }
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeSetAutoWhiteBlance(JNIEnv *env, jclass clazz,
                                                                     jlong id_camera,
                                                                     jboolean auto_white_blance) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setAutoWhiteBlance(auto_white_blance);
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeGetAutoWhiteBlance(JNIEnv *env, jclass clazz,
                                                                     jlong id_camera) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getAutoWhiteBlance();
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
// Java method correspond to this function should not be a static method
Java_com_serenegiant_libuvccamera_UVCCamera_nativeUpdateAutoWhiteBlanceCompoLimit(JNIEnv *env,
                                                                                  jobject thiz,
                                                                                  jlong id_camera) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateAutoWhiteBlanceCompoLimit(min, max, def);
        if (!result) {
            // Write to Java side
            setField_int(env, thiz, "mAutoWhiteBlanceCompoMin", min);
            setField_int(env, thiz, "mAutoWhiteBlanceCompoMax", max);
            setField_int(env, thiz, "mAutoWhiteBlanceCompoDef", def);
        }
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeSetAutoWhiteBlanceCompo(JNIEnv *env, jclass clazz,
                                                                          jlong id_camera,
                                                                          jboolean auto_white_blance_compo) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setAutoWhiteBlanceCompo(auto_white_blance_compo);
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeGetAutoWhiteBlanceCompo(JNIEnv *env, jclass clazz,
                                                                          jlong id_camera) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getAutoWhiteBlanceCompo();
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
// Java method correspond to this function should not be a static method
Java_com_serenegiant_libuvccamera_UVCCamera_nativeUpdateWhiteBlanceLimit(JNIEnv *env, jobject thiz,
                                                                         jlong id_camera) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateWhiteBlanceLimit(min, max, def);
        if (!result) {
            // Write to Java side
            setField_int(env, thiz, "mWhiteBlanceMin", min);
            setField_int(env, thiz, "mWhiteBlanceMax", max);
            setField_int(env, thiz, "mWhiteBlanceDef", def);
        }
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeSetWhiteBlance(JNIEnv *env, jclass clazz,
                                                                 jlong id_camera,
                                                                 jint white_blance) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setWhiteBlance(white_blance);
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeGetWhiteBlance(JNIEnv *env, jclass clazz,
                                                                 jlong id_camera) {
    jint result = 0;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getWhiteBlance();
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
// Java method correspond to this function should not be a static method
Java_com_serenegiant_libuvccamera_UVCCamera_nativeUpdateWhiteBlanceCompoLimit(JNIEnv *env,
                                                                              jobject thiz,
                                                                              jlong id_camera) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateWhiteBlanceCompoLimit(min, max, def);
        if (!result) {
            // Write to Java side
            setField_int(env, thiz, "mWhiteBlanceCompoMin", min);
            setField_int(env, thiz, "mWhiteBlanceCompoMax", max);
            setField_int(env, thiz, "mWhiteBlanceCompoDef", def);
        }
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeSetWhiteBlanceCompo(JNIEnv *env, jclass clazz,
                                                                      jlong id_camera,
                                                                      jint white_blance_compo) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setWhiteBlanceCompo(white_blance_compo);
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeGetWhiteBlanceCompo(JNIEnv *env, jclass clazz,
                                                                      jlong id_camera) {
    jint result = 0;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getWhiteBlanceCompo();
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
// Java method correspond to this function should not be a static method
Java_com_serenegiant_libuvccamera_UVCCamera_nativeUpdateBacklightCompLimit(JNIEnv *env,
                                                                           jobject thiz,
                                                                           jlong id_camera) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateBacklightCompLimit(min, max, def);
        if (!result) {
            // Write to Java side
            setField_int(env, thiz, "mBacklightCompMin", min);
            setField_int(env, thiz, "mBacklightCompMax", max);
            setField_int(env, thiz, "mBacklightCompDef", def);
        }
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeSetBacklightComp(JNIEnv *env, jclass clazz,
                                                                   jlong id_camera,
                                                                   jint backlight_comp) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setBacklightComp(backlight_comp);
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeGetBacklightComp(JNIEnv *env, jclass clazz,
                                                                   jlong id_camera) {
    jint result = 0;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getBacklightComp();
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
// Java method correspond to this function should not be a static method
Java_com_serenegiant_libuvccamera_UVCCamera_nativeUpdateBrightnessLimit(JNIEnv *env, jobject thiz,
                                                                        jlong id_camera) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateBrightnessLimit(min, max, def);
        if (!result) {
            // Write to Java side
            setField_int(env, thiz, "mBrightnessMin", min);
            setField_int(env, thiz, "mBrightnessMax", max);
            setField_int(env, thiz, "mBrightnessDef", def);
        }
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeSetBrightness(JNIEnv *env, jclass clazz,
                                                                jlong id_camera, jint brightness) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setBrightness(brightness);
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeGetBrightness(JNIEnv *env, jclass clazz,
                                                                jlong id_camera) {
    jint result = 0;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getBrightness();
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
// Java method correspond to this function should not be a static method
Java_com_serenegiant_libuvccamera_UVCCamera_nativeUpdateContrastLimit(JNIEnv *env, jobject thiz,
                                                                      jlong id_camera) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateContrastLimit(min, max, def);
        if (!result) {
            // Write to Java side
            setField_int(env, thiz, "mContrastMin", min);
            setField_int(env, thiz, "mContrastMax", max);
            setField_int(env, thiz, "mContrastDef", def);
        }
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeSetContrast(JNIEnv *env, jclass clazz,
                                                              jlong id_camera, jint contrast) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setContrast(contrast);
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeGetContrast(JNIEnv *env, jclass clazz,
                                                              jlong id_camera) {
    jint result = 0;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getContrast();
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
// Java method correspond to this function should not be a static method
Java_com_serenegiant_libuvccamera_UVCCamera_nativeUpdateAutoContrastLimit(JNIEnv *env, jobject thiz,
                                                                          jlong id_camera) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateAutoContrastLimit(min, max, def);
        if (!result) {
            // Write to Java side
            setField_int(env, thiz, "mAutoContrastMin", min);
            setField_int(env, thiz, "mAutoContrastMax", max);
            setField_int(env, thiz, "mAutoContrastDef", def);
        }
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeSetAutoContrast(JNIEnv *env, jclass clazz,
                                                                  jlong id_camera,
                                                                  jboolean autocontrast) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setAutoContrast(autocontrast);
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeGetAutoContrast(JNIEnv *env, jclass clazz,
                                                                  jlong id_camera) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getAutoContrast();
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
// Java method correspond to this function should not be a static method
Java_com_serenegiant_libuvccamera_UVCCamera_nativeUpdateSharpnessLimit(JNIEnv *env, jobject thiz,
                                                                       jlong id_camera) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateSharpnessLimit(min, max, def);
        if (!result) {
            // Write to Java side
            setField_int(env, thiz, "mSharpnessMin", min);
            setField_int(env, thiz, "mSharpnessMax", max);
            setField_int(env, thiz, "mSharpnessDef", def);
        }
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeSetSharpness(JNIEnv *env, jclass clazz,
                                                               jlong id_camera, jint sharpness) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setSharpness(sharpness);
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeGetSharpness(JNIEnv *env, jclass clazz,
                                                               jlong id_camera) {
    jint result = 0;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getSharpness();
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
// Java method correspond to this function should not be a static method
Java_com_serenegiant_libuvccamera_UVCCamera_nativeUpdateGainLimit(JNIEnv *env, jobject thiz,
                                                                  jlong id_camera) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateGainLimit(min, max, def);
        if (!result) {
            // Write to Java side
            setField_int(env, thiz, "mGainMin", min);
            setField_int(env, thiz, "mGainMax", max);
            setField_int(env, thiz, "mGainDef", def);
        }
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeSetGain(JNIEnv *env, jclass clazz,
                                                          jlong id_camera, jint gain) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setGain(gain);
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeGetGain(JNIEnv *env, jclass clazz,
                                                          jlong id_camera) {
    jint result = 0;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getGain();
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
// Java method correspond to this function should not be a static method
Java_com_serenegiant_libuvccamera_UVCCamera_nativeUpdateGammaLimit(JNIEnv *env, jobject thiz,
                                                                   jlong id_camera) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateGammaLimit(min, max, def);
        if (!result) {
            // Write to Java side
            setField_int(env, thiz, "mGammaMin", min);
            setField_int(env, thiz, "mGammaMax", max);
            setField_int(env, thiz, "mGammaDef", def);
        }
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeSetGamma(JNIEnv *env, jclass clazz,
                                                           jlong id_camera, jint gamma) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setGamma(gamma);
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeGetGamma(JNIEnv *env, jclass clazz,
                                                           jlong id_camera) {
    jint result = 0;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getGamma();
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
// Java method correspond to this function should not be a static method
Java_com_serenegiant_libuvccamera_UVCCamera_nativeUpdateSaturationLimit(JNIEnv *env, jobject thiz,
                                                                        jlong id_camera) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateSaturationLimit(min, max, def);
        if (!result) {
            // Write to Java side
            setField_int(env, thiz, "mSaturationMin", min);
            setField_int(env, thiz, "mSaturationMax", max);
            setField_int(env, thiz, "mSaturationDef", def);
        }
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeSetSaturation(JNIEnv *env, jclass clazz,
                                                                jlong id_camera, jint saturation) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setSaturation(saturation);
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeGetSaturation(JNIEnv *env, jclass clazz,
                                                                jlong id_camera) {
    jint result = 0;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getSaturation();
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
// Java method correspond to this function should not be a static method
Java_com_serenegiant_libuvccamera_UVCCamera_nativeUpdateHueLimit(JNIEnv *env, jobject thiz,
                                                                 jlong id_camera) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateHueLimit(min, max, def);
        if (!result) {
            // Write to Java side
            setField_int(env, thiz, "mHueMin", min);
            setField_int(env, thiz, "mHueMax", max);
            setField_int(env, thiz, "mHueDef", def);
        }
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeSetHue(JNIEnv *env, jclass clazz, jlong id_camera,
                                                         jint hue) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setHue(hue);
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeGetHue(JNIEnv *env, jclass clazz,
                                                         jlong id_camera) {
    jint result = 0;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getHue();
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
// Java method correspond to this function should not be a static method
Java_com_serenegiant_libuvccamera_UVCCamera_nativeUpdateAutoHueLimit(JNIEnv *env, jobject thiz,
                                                                     jlong id_camera) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateAutoHueLimit(min, max, def);
        if (!result) {
            // Write to Java side
            setField_int(env, thiz, "mAutoHueMin", min);
            setField_int(env, thiz, "mAutoHueMax", max);
            setField_int(env, thiz, "mAutoHueDef", def);
        }
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeSetAutoHue(JNIEnv *env, jclass clazz,
                                                             jlong id_camera, jboolean autohue) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setAutoHue(autohue);
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeGetAutoHue(JNIEnv *env, jclass clazz,
                                                             jlong id_camera) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getAutoHue();
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
// Java method correspond to this function should not be a static method
Java_com_serenegiant_libuvccamera_UVCCamera_nativeUpdatePowerlineFrequencyLimit(JNIEnv *env,
                                                                                jobject thiz,
                                                                                jlong id_camera) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updatePowerlineFrequencyLimit(min, max, def);
        if (!result) {
            // Write to Java side
            setField_int(env, thiz, "mPowerlineFrequencyMin", min);
            setField_int(env, thiz, "mPowerlineFrequencyMax", max);
            setField_int(env, thiz, "mPowerlineFrequencyDef", def);
        }
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeSetPowerlineFrequency(JNIEnv *env, jclass clazz,
                                                                        jlong id_camera,
                                                                        jint frequency) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setPowerlineFrequency(frequency);
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeGetPowerlineFrequency(JNIEnv *env, jclass clazz,
                                                                        jlong id_camera) {
    jint result = 0;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getPowerlineFrequency();
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
// Java method correspond to this function should not be a static method
Java_com_serenegiant_libuvccamera_UVCCamera_nativeUpdateZoomLimit(JNIEnv *env, jobject thiz,
                                                                  jlong id_camera) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateZoomLimit(min, max, def);
        if (!result) {
            // Write to Java side
            setField_int(env, thiz, "mZoomMin", min);
            setField_int(env, thiz, "mZoomMax", max);
            setField_int(env, thiz, "mZoomDef", def);
        }
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeSetZoom(JNIEnv *env, jclass clazz,
                                                          jlong id_camera, jint zoom) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setZoom(zoom);
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeGetZoom(JNIEnv *env, jclass clazz,
                                                          jlong id_camera) {
    jint result = 0;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getZoom();
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
// Java method correspond to this function should not be a static method
Java_com_serenegiant_libuvccamera_UVCCamera_nativeUpdateZoomRelLimit(JNIEnv *env, jobject thiz,
                                                                     jlong id_camera) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateZoomRelLimit(min, max, def);
        if (!result) {
            // Write to Java side
            setField_int(env, thiz, "mZoomRelMin", min);
            setField_int(env, thiz, "mZoomRelMax", max);
            setField_int(env, thiz, "mZoomRelDef", def);
        }
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeSetZoomRel(JNIEnv *env, jclass clazz,
                                                             jlong id_camera, jint zoom_rel) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setZoomRel(zoom_rel);
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeGetZoomRel(JNIEnv *env, jclass clazz,
                                                             jlong id_camera) {
    jint result = 0;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getZoomRel();
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
// Java method correspond to this function should not be a static method
Java_com_serenegiant_libuvccamera_UVCCamera_nativeUpdateDigitalMultiplierLimit(JNIEnv *env,
                                                                               jobject thiz,
                                                                               jlong id_camera) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateDigitalMultiplierLimit(min, max, def);
        if (!result) {
            // Write to Java side
            setField_int(env, thiz, "mDigitalMultiplierMin", min);
            setField_int(env, thiz, "mDigitalMultiplierMax", max);
            setField_int(env, thiz, "mDigitalMultiplierDef", def);
        }
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeSetDigitalMultiplier(JNIEnv *env, jclass clazz,
                                                                       jlong id_camera,
                                                                       jint multiplier) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setDigitalMultiplier(multiplier);
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeGetDigitalMultiplier(JNIEnv *env, jclass clazz,
                                                                       jlong id_camera) {
    jint result = 0;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getDigitalMultiplier();
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
// Java method correspond to this function should not be a static method
Java_com_serenegiant_libuvccamera_UVCCamera_nativeUpdateDigitalMultiplierLimitLimit(JNIEnv *env,
                                                                                    jobject thiz,
                                                                                    jlong id_camera) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateDigitalMultiplierLimitLimit(min, max, def);
        if (!result) {
            // Write to Java side
            setField_int(env, thiz, "mDigitalMultiplierLimitMin", min);
            setField_int(env, thiz, "mDigitalMultiplierLimitMax", max);
            setField_int(env, thiz, "mDigitalMultiplierLimitDef", def);
        }
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeSetDigitalMultiplierLimit(JNIEnv *env,
                                                                            jclass clazz,
                                                                            jlong id_camera,
                                                                            jint multiplier_limit) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setDigitalMultiplierLimit(multiplier_limit);
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeGetDigitalMultiplierLimit(JNIEnv *env,
                                                                            jclass clazz,
                                                                            jlong id_camera) {
    jint result = 0;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getDigitalMultiplierLimit();
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
// Java method correspond to this function should not be a static method
Java_com_serenegiant_libuvccamera_UVCCamera_nativeUpdateAnalogVideoStandardLimit(JNIEnv *env,
                                                                                 jobject thiz,
                                                                                 jlong id_camera) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateAnalogVideoStandardLimit(min, max, def);
        if (!result) {
            // Write to Java side
            setField_int(env, thiz, "mAnalogVideoStandardMin", min);
            setField_int(env, thiz, "mAnalogVideoStandardMax", max);
            setField_int(env, thiz, "mAnalogVideoStandardDef", def);
        }
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeSetAnalogVideoStandard(JNIEnv *env, jclass clazz,
                                                                         jlong id_camera,
                                                                         jint standard) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setAnalogVideoStandard(standard);
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeGetAnalogVideoStandard(JNIEnv *env, jclass clazz,
                                                                         jlong id_camera) {
    jint result = 0;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getAnalogVideoStandard();
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
// Java method correspond to this function should not be a static method
Java_com_serenegiant_libuvccamera_UVCCamera_nativeUpdateAnalogVideoLockStateLimit(JNIEnv *env,
                                                                                  jobject thiz,
                                                                                  jlong id_camera) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateAnalogVideoLockStateLimit(min, max, def);
        if (!result) {
            // Write to Java side
            setField_int(env, thiz, "mAnalogVideoLockStateMin", min);
            setField_int(env, thiz, "mAnalogVideoLockStateMax", max);
            setField_int(env, thiz, "mAnalogVideoLockStateDef", def);
        }
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeSetAnalogVideoLockState(JNIEnv *env,
                                                                           jclass clazz,
                                                                           jlong id_camera,
                                                                           jint state) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setAnalogVideoLockState(state);
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeGetAnalogVideoLockState(JNIEnv *env, jclass clazz,
                                                                          jlong id_camera) {
    jint result = 0;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getAnalogVideoLockState();
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
// Java method correspond to this function should not be a static method
Java_com_serenegiant_libuvccamera_UVCCamera_nativeUpdatePrivacyLimit(JNIEnv *env, jobject thiz,
                                                                     jlong id_camera) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updatePrivacyLimit(min, max, def);
        if (!result) {
            // Write to Java side
            setField_int(env, thiz, "mPrivacyMin", min);
            setField_int(env, thiz, "mPrivacyMax", max);
            setField_int(env, thiz, "mPrivacyDef", def);
        }
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeSetPrivacy(JNIEnv *env, jclass clazz,
                                                             jlong id_camera, jboolean privacy) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setPrivacy(privacy ? 1: 0);
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeGetPrivacy(JNIEnv *env, jclass clazz,
                                                             jlong id_camera) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getPrivacy();
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT void JNICALL
// Set whether horizontal mirroring is required
Java_com_serenegiant_libuvccamera_UVCCamera_nativeHorizontalMirror(JNIEnv *env, jclass clazz,
                                                                   jlong id_camera,
                                                                   jint horizontal_mirror) {
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        return camera->setHorizontalMirror(horizontal_mirror);
    }
}

extern "C"
JNIEXPORT void JNICALL
// Set whether vertical mirroring is required
Java_com_serenegiant_libuvccamera_UVCCamera_nativeVerticalMirror(JNIEnv *env, jclass clazz,
                                                                 jlong id_camera,
                                                                 jint vertical_mirror) {
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        return camera->setVerticalMirror(vertical_mirror);
    }
}

extern "C"
JNIEXPORT void JNICALL
// Set the camera's own angle
Java_com_serenegiant_libuvccamera_UVCCamera_nativeCameraAngle(JNIEnv *env, jclass clazz,
                                                              jlong id_camera, jint camera_angle) {
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        return camera->setCameraAngle(camera_angle);
    }
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeGetCurrentFps(JNIEnv *env, jclass clazz,
                                                                jlong id_camera) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getCurrentFps();
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeGetDefaultCameraFps(JNIEnv *env, jclass clazz,
                                                                      jlong id_camera) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getDefaultCameraFps();
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeGetFrameWidth(JNIEnv *env, jclass clazz,
                                                                jlong id_camera) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getFrameWidth();
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeGetFrameHeight(JNIEnv *env, jclass clazz,
                                                                 jlong id_camera) {
    jint result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getFrameHeight();
    }
    RETURN(result, jint);
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_serenegiant_libuvccamera_UVCCamera_nativeIsRunning(JNIEnv *env, jclass clazz,
                                                            jlong id_camera) {
    jboolean result = JNI_FALSE;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->isRunning();
    }
    RETURN(result, jboolean);
}