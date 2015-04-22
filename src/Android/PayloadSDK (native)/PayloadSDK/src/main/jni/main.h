#ifndef __MAIN_H__
#define __MAIN_H__

#include <jni.h>
#include <android/log.h>

// underscores are reserved characters in JNI referring to package boundaries.
// stick with camelCase moduleNames, classNames and methodNames


jint
Java_com_abs_payloadsdk_sdb_SDB_startNative(JNIEnv* env, jobject thiz);

jbyteArray
Java_com_abs_payloadsdk_sdb_SDB_sendSyncNative(JNIEnv *env, jclass cls, jbyteArray data);

#endif // __MAIN_H__