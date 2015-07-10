#include "wrapper.h"
#include "libraries.h"
#include <abs.h>

int fd = 0;

jobject
Java_com_abs_payloadsdk_sdb_SDB_sendSyncNative(JNIEnv *env, jobject obj, jobject SDBPacket)
{
        jclass pkg;
        jmethodID getCmd, getArgs, getData, constructor;
        jbyteArray args, data;
        jsize argsLength, dataLength;
        jbyte *argsBuffer, *dataBuffer;
        jbyteArray ret;
        jobject result;

        MCSPacket *packet, *response;

        /* Get the SDBPacket object */

        pkg = (*env)->GetObjectClass(env, SDBPacket);

        /* Get the elements of SDBPacket */

        getCmd = (*env)->GetMethodID(env, pkg, "getCmd", "()I");
        int cmd =  (*env)->CallIntMethod(env, SDBPacket, getCmd);

        getArgs = (*env)->GetMethodID(env, pkg, "getArgs", "()[B");
        args = (*env)->CallObjectMethod(env, SDBPacket, getArgs);
        argsLength = (*env)->GetArrayLength(env, args);
        argsBuffer = (*env)->GetByteArrayElements(env, args, NULL);

        getData = (*env)->GetMethodID(env, pkg, "getData", "()[B");
        data = (*env)->CallObjectMethod(env, SDBPacket, getData);
        dataLength = (*env)->GetArrayLength(env, data);
        dataBuffer = (*env)->GetByteArrayElements(env, data, NULL);

        /* perform the call to the SDB */

        packet = mcs_create_packet(cmd, argsLength, argsBuffer, dataLength, dataBuffer);
        response = sendSyncSDB(fd, packet);

        /* Return the response */

        (*env)->ReleaseByteArrayElements(env, args, argsBuffer, 0);
        (*env)->ReleaseByteArrayElements(env, data, dataBuffer, 0);

        constructor = (*env)->GetMethodID(env, pkg, "<init>", "(I[B)V");

        if(response != NULL){

            ret = (*env)->NewByteArray(env, response->data_size);
            (*env)->SetByteArrayRegion(env, ret, 0, response->data_size, response->data);
            result = (*env)->NewObject(env, pkg, constructor, response->data_size, ret);

        } else {
            result = (*env)->NewObject(env, pkg, constructor, 0, NULL );
        }
        return result;
}

jint
Java_com_abs_payloadsdk_sdb_SDB_connectSDBNative(JNIEnv* env, jobject object)
{
    fd = connectSDB(0);
    return fd;
}

// Required for the default JNI implementation
jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    return JNI_VERSION_1_6;
}