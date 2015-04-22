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

    private native int connectSDBNative();

    private native byte[] sendSyncNative(byte[] array);


    public SDBPacket sendSync(SDBPacket packet)
    {
        byte[] response;
        response = sendSyncNative(packet.toRaw());
        return new SDBPacket(response);
    }

    public SDB()
    {
        connectSDBNative();
        Log.d("ABS","SDB connected");
    }

}