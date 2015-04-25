#include "wrapper.h"
#include "libraries.h"

int fd = 0;

jbyteArray
Java_com_abs_payloadsdk_sdb_SDB_sendSyncNative(JNIEnv *env, jobject obj, jbyteArray packet)
{
    jsize length = (*env)->GetArrayLength(env, packet);
    jbyte *buffer = (*env)->GetByteArrayElements(env, packet, NULL);

    int cmd = (buffer[0] << 16) + (buffer[1] << 8) + buffer[2];
    int n_args = buffer[3];
    int data_size = buffer[4 + n_args];
    MCSPacket *mcs_packet;
    mcs_packet = mcs_create_packet(cmd, n_args, &buffer[4], data_size, &buffer[5+n_args]);

    sendSyncSDB(fd, mcs_packet);


    (*env)->ReleaseByteArrayElements(env, packet, buffer, 0);

        jbyte a[] = {1,2,3,4,5,6};
                jbyteArray ret = (*env)->NewByteArray(env,6);
                (*env)->SetByteArrayRegion(env, ret, 0, 6, a);
                return ret;
}

jint
Java_com_abs_payloadsdk_sdb_SDB_connectSDBNative(JNIEnv* env, jobject object)
{
    int res = connectSDB(2);
    fd = res;
    return res;
}

// Required for the default JNI implementation
jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    return JNI_VERSION_1_6;
}