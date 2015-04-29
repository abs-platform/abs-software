LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := libabs
LOCAL_SRC_FILES := lib/$(LOCAL_MODULE).so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libmcs
LOCAL_SRC_FILES := lib/$(LOCAL_MODULE).so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := NdkModule
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_SRC_FILES := wrapper.c libraries.c
LOCAL_LDLIBS := -L/Users/arnauprat/PayloadApp/PayloadSDK/src/main/jni/lib -llog -lmcs -labs
include $(BUILD_SHARED_LIBRARY)