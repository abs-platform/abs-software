#ifndef __WRAPPER_H__
#define __WRAPPER_H__

#include <jni.h>
#include <android/log.h>

#define TAG "[ABS]"

#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)

jint
Java_com_abs_payloadsdk_sdb_SDB_connectSDBNative(JNIEnv* env, jobject thiz);

jbyteArray
Java_com_abs_payloadsdk_sdb_SDB_sendSyncNative(JNIEnv *env, jclass cls, jbyteArray data);

#endif // __WRAPPER_H__