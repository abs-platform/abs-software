package com.google.abs.payloadsdk.SBD;

import android.util.Log;
import java.net.Socket;
import java.io.IOException;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import android.os.SystemClock;

public class SDB extends android.os.AsyncTask<Void, Void, Void> {

    private byte[] result;
    private Socket socket;
    private DataInputStream rxStream;
    private DataOutputStream txStream;
    private boolean connected = false;

    @Override
    protected Void doInBackground(Void... params)
    {
        try {
            socket = new Socket("127.0.0.1", 1111);
            rxStream = new DataInputStream(socket.getInputStream());
            txStream = new DataOutputStream(socket.getOutputStream());
            Log.d("[ABS]", "Connection established");
            connected = true;
        } catch (Exception e) {
            e.printStackTrace();
        }
        result = receive();
        while(connected) {
            result = receive();
        }
        return null;
    }

    /**
     * Send packets to the SDB and waits for response
     *
     * @param packet    packet sent to the SDB
     * @return          response to the SDBPacket
     */

    public byte[] send(SDBPacket packet)
    {
        waitForSocketToConnect();
        if(socket.isConnected()) {
            try {
                result = null;
                txStream.write(packet.toRaw());
                txStream.flush();
                Log.d("[ABS]", "Packet sent");
                while (result==null) {
                    SystemClock.sleep(500);
                }
                Log.d("[ABS]", "Packet received");
                return result;
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        return null;
    }

    private byte[] receive()
    {
        byte[] data = new byte[2024];
        try {
            rxStream.read(data);
            return data;
        } catch(IOException e) {
            e.printStackTrace();
        }
        return null;
    }

    private boolean waitForSocketToConnect()
    {
        if (connected)
            return true;
        while (!connected) {
            try {
                Thread.sleep(500);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
        return connected;
    }
}