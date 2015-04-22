#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include "main.h"
#include <mcs.h>
#include <sdb.h>
#include <abs_test.h>

#define TAG "[ABS native]"

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)

/*
int sockfd = 0, n = 0;
char recvBuff[1024];
struct sockaddr_in serv_addr;


jObject
Java_com_abs_payloadsdk_sdb_SDB_sentSyncNative(JNIEnv *env, jobject obj, jobject SDBPacket)
{
    int cmd;
    unsigned short n_args;
    unsigned char *args;
    unsigned short data_size;
    unsigned char *data;

    jclass packet = (*env)->GetObjectClass(env, SDBPacket);

    jmethodID getCmd = (*env)->GetMethodID(env, packet, "getCmd", "()I");
    cmd = (*env)->CallIntMethod(env, packet, getCmd);

    jclass newClass = (*env)->FindClass(env, "com")

    return 0;
}
*/


jbyteArray
Java_com_abs_payloadsdk_sdb_SDB_sendSyncNative(JNIEnv *env, jobject obj, jbyteArray data)
{
    jsize length = (*env)->GetArrayLength(env, data);
    jbyte *array = (*env)->GetByteArrayElements(env, data, 0);

    jchar buffer[50];
    write(sockfd, array, length);

    jbyteArray datas[50];



    while(read(sockfd, datas, 50)<=0);

    jbyteArray ret = (*env)->NewByteArray(env, 50);
    (*env)->SetByteArrayRegion (env, ret, 0, 50, datas);

    (*env)->ReleaseByteArrayElements(env, data, buffer, 0);

    return datas;
}

jint
Java_com_abs_payloadsdk_sdb_SDB_startNative(JNIEnv *env, jobject obj) {


              int fd;
              int id;
              unsigned int orig_id;
              char name[5];
              char buffer[100];
              char welcome[] = "appX:app";
              struct sockaddr_in addr;
              MCSPacket *pkt;

              id = 0;
              welcome[3] = id + '0';

              sprintf(name, "app%d", id);

              addr.sin_family = AF_INET;
              addr.sin_port = htons(SDB_SOCK_PORT);
              addr.sin_addr.s_addr = INADDR_ANY;

              /* SDB connection */
              sprintf(buffer, "%s: SDB Connection\n", name);
              printf("Setting connection for %s\n", name);

              fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
              if (fd < 0) {
                  printf("Could not create the socket\n");
                  return 0;
              }

              if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
                  printf("Could not connect to the socket\n");
                  return 0;
              }

              /* Handshaking */
              sprintf(buffer, "%s: Handshake\n", name);
              /* Remember NULL character! */
              pkt = mcs_create_packet(MCS_MESSAGE_SDB_HANDSHAKE, 0, NULL,
                                      strlen(welcome) + 1, (unsigned char *)welcome);
              if(pkt == NULL) {
                  printf("Packet generated is NULL\n");
                  return 0;
              }
              orig_id = pkt->id;

              if(mcs_write_command(pkt, fd) != 0) {
                  printf("Could not send handshake packet\n");
                  return 0;
              }

              mcs_free(pkt);

              /* Aswer to the handshake */
              pkt = mcs_read_command(fd, fd);
              if(pkt == NULL) {
                  printf("Could not read response packet\n");
                  return 0;
              } else if(pkt->id != orig_id) {
                  printf("IDs from question and answer are different\n");
              } else if(pkt->type != MCS_TYPE_OK) {
                  if(pkt->type == MCS_TYPE_ERR) {
                      printf("Wrong response packet. Error %d\n",
                                                      mcs_err_code_from_command(pkt));
                  } else {
                      printf("Wrong reponse packet. Type: %hhd\n", pkt->type);
                  }
                  return 0;
              }
      return 1;
}
// Required for the default JNI implementation
jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    return JNI_VERSION_1_6;

}