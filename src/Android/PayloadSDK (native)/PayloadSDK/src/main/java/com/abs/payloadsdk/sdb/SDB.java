package com.abs.payloadsdk.sdb;

import android.os.AsyncTask;
import android.os.SystemClock;
import android.util.Log;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.Socket;


public class SDB {

    static {

        System.loadLibrary("NdkModule");
    } //load abs native library

    private native int startNative();

    // public native SDBPacket
    // sendSyncNative (int cmd, int n_args, byte[] args, int data_size, byte[] data);

        /*

    Wrapper:

    jbyteArray
    Java_com_abs_payloadsdk_sdb_SDB_sentSyncNative(JNIEnv *env, jobject obj, jobject SDBPacket)
    {
        int cmd;
        unsigned short nArgs;
        unsigned char *args;
        unsigned short data_size;
        unsigned char *data;

        jclass packet = (*env)->GetObjectClass(env, SDBPacket);

        methodID getCmd = (*env)->GetMethodID(env, packet, "getCmd", "()I");
        cmd = (*env)->CallIntMethod(env, packet, getCmd);
        methodID getNArgs = (*env)->GetMethodID(env, packet, "getNArgs", "()I");
        nArgs =  (*env)->CallIntMethod(env, packet, getNArgs);
        methodID getArgs = (*env)->GetMethodID(env, packet, "getArgs", "()[C");
        args =  (*env)->CallIntMethod(env, packet, getArgs);
        methodID getDataSize = (*env)->GetMethodID(env, packet, "getDataSize", "()I");
        dataSize =  (*env)->CallIntMethod(env, packet, getDataSize);
        methodID getData = (*env)->GetMethodID(env, packet, "getData", "()[C");
        data = (*env)->CallIntMethod(env, packet, getData);

        // MCSPacket *pkg = mcs_create_packet(cmd, nArgs, args, data_size, data);
        //
        // MCSPacket *res = sendPkg(pkg);

        jmethodID constructor = (*env)->GetMethodID(env, packet, "<init>", "(II[CI[C)V");
        jobject response = (*env)->NewObject(env, packet, constructor,
        cmd, nArgs, data_size, data);
    }

     */

    private native byte[] sendSyncNative(byte[] array);

    public SDBPacket sendSync(SDBPacket packet)
    {
        byte[] response;
        response = sendSyncNative(packet.toRaw());
        return new SDBPacket(response);
    }

    public SDB()
    {
        int res = startNative();
        Log.d("ABS",String.valueOf(res));
    }

}