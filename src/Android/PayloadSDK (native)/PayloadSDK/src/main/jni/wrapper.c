#include "wrapper.h"
#include "libraries.h"
#include <abs.h>

int fd = 0;

jobject
Java_com_abs_payloadsdk_sdb_SDB_sendSyncNative(JNIEnv *env, jobject obj, jobject SDBPacket)
{
        jclass pkg;
        jmethodID getCmd, getArgs, getData, constructor;

        pkg = (*env)->GetObjectClass(env, SDBPacket);

        getCmd = (*env)->GetMethodID(env, pkg, "getCmd", "()I");
        int cmd =  (*env)->CallIntMethod(env, SDBPacket, getCmd);

        getArgs = (*env)->GetMethodID(env, pkg, "getArgs", "()[B");
        jbyteArray args = (*env)->CallObjectMethod(env, SDBPacket, getArgs);
        jsize argsLength = (*env)->GetArrayLength(env, args);
        jbyte *argsBuffer = (*env)->GetByteArrayElements(env, args, NULL);

        getData = (*env)->GetMethodID(env, pkg, "getData", "()[B");
        jbyteArray data = (*env)->CallObjectMethod(env, SDBPacket, getData);
        jsize dataLength = (*env)->GetArrayLength(env, data);
        jbyte *dataBuffer = (*env)->GetByteArrayElements(env, data, NULL);

        MCSPacket *mcs_packet;

        mcs_packet = mcs_create_packet(cmd, argsLength, argsBuffer, dataLength, dataBuffer);

        MCSPacket *response = sendSyncSDB(fd, mcs_packet);

        (*env)->ReleaseByteArrayElements(env, args, argsBuffer, 0);
        (*env)->ReleaseByteArrayElements(env, data, dataBuffer, 0);


        char *a = "a";

        jbyteArray ret = (*env)->NewByteArray(env,1);
        (*env)->SetByteArrayRegion(env, ret, 0, 1, (jbyte *)a);

/*
        jbyte a[] = {1,2,3,4,5,6};
                jbyteArray ret = (*env)->NewByteArray(env,6);

*/

/*
        unsigned char *a = "a";
        jbyteArray ret = (*env)->NewByteArray(env,1);
        (*env)->SetByteArrayRegion(env, ret, 0, 1, (jbyte *)a);

    */
    constructor = (*env)->GetMethodID(env, pkg, "<init>", "(I[B)V");
    return (*env)->NewObject(env, pkg, constructor, 10, ret );

}


jint
Java_com_abs_payloadsdk_sdb_SDB_connectSDBNative(JNIEnv* env, jobject object)
{
    fd = connectSDB(2);

    return fd;
}

// Required for the default JNI implementation
jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    return JNI_VERSION_1_6;
}