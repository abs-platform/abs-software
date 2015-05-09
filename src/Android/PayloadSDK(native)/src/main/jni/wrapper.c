#include "wrapper.h"
#include "libraries.h"
#include <abs.h>

int fd = 0;

jobject
Java_com_abs_payloadsdk_sdb_SDB_sendSyncNative(JNIEnv *env, jobject obj, jobject SDBPacket)
{
        jclass pkg;
        jmethodID getCmd, getArgs, getData, constructor;

        MCSPacket *packet, *response;

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

        packet = mcs_create_packet(cmd, argsLength, argsBuffer, dataLength, dataBuffer);
        response = sendSyncSDB(fd, packet);

        (*env)->ReleaseByteArrayElements(env, args, argsBuffer, 0);
        (*env)->ReleaseByteArrayElements(env, data, dataBuffer, 0);

        constructor = (*env)->GetMethodID(env, pkg, "<init>", "(I[B)V");

        jbyteArray ret;

        char * a;
        int data_size;
        jobject res;

        if(response != NULL){

        data_size = response->data_size;
        a = response->data;

        ret = (*env)->NewByteArray(env,data_size);
        (*env)->SetByteArrayRegion(env, ret, 0, data_size, (jbyte *)a);

            res = (*env)->NewObject(env, pkg, constructor, 254, ret );

        } else {
            res = (*env)->NewObject(env, pkg, constructor, 0, NULL );
        }

        return res;

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