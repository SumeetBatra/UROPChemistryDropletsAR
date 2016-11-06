LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

ifdef OPENCV_ANDROID_SDK
  ifneq ("","$(wildcard C:/Android/OpenCV-android-sdk/sdk/native/jni/OpenCV.mk)")
    include C:/Android/OpenCV-android-sdk/sdk/native/jni/OpenCV.mk
  else
    include C:/Android/OpenCV-android-sdk/sdk/native/jni/OpenCV.mk
  endif
else
  include C:/Android/OpenCV-android-sdk/sdk/native/jni/OpenCV.mk
endif

LOCAL_MODULE := opencvnative
LOCAL_SRC_FILES := opencvnative.cpp Droplet.cpp DropletDetector.cpp
LOCAL_LDLIBS += -L$(SYSROOT)/usr/lib -llog -ldl
LOCAL_CFLAGS += -std=c++11

include $(BUILD_SHARED_LIBRARY)