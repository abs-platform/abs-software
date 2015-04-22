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
LOCAL_MODULE := libabs_test
LOCAL_SRC_FILES := lib/$(LOCAL_MODULE).so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := NdkModule
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_SRC_FILES := main.c
LOCAL_LDLIBS := -L/Users/arnauprat/PayloadApp/PayloadSDK/src/main/jni/lib -llog -lmcs -labs_test
include $(BUILD_SHARED_LIBRARY)