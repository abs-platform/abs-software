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

    private native SDBPacket sendSyncNative(SDBPacket array);

    public SDB()
    {
        connectSDBNative();
    }

    public SDBPacket sendSync(SDBPacket packet)
    {
        return sendSyncNative(packet);
    }
}