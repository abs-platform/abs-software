package com.google.abs.payloadsdk.SBD;

import java.net.Socket;
import android.util.Log;
import java.io.IOException;
import android.os.AsyncTask;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import android.os.SystemClock;

public class SDB extends AsyncTask<Void, Void, Void> {

    private static final int PORT = 1111;
    private static final String INET_ADDR = "127.0.0.1";

    private byte[] result;
    private Socket socket;
    private DataInputStream rxStream;
    private DataOutputStream txStream;
    private boolean connected = false;

    @Override
    protected Void doInBackground(Void... params)
    {
        try {
            socket = new Socket(INET_ADDR, PORT);
            rxStream = new DataInputStream(socket.getInputStream());
            txStream = new DataOutputStream(socket.getOutputStream());
            Log.d("[ABS]", "Connection established");
            connected = true;
        } catch (Exception e) {
            e.printStackTrace();
        }
        waitForSocketToConnect(); /* TODO handle error if socket fails */
        if(socket.isConnected()) {
            result = receive();
            while (connected) {
                result = receive();
            }
        }
        return null; /* error */
    }

    /**
     * Send packets to the SDB and waits for response
     *
     * @param packet    packet sent to the SDB
     * @return          response to the SDBPacket
     */

    public synchronized SDBPacket send(SDBPacket packet)
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
                return new SDBPacket(result);
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        return null; /* error */
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
        if(connected) {
            return true;
        }
        int count = 0;
        while(!connected) {
            try {
                Thread.sleep(500);
                count++;
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            if(count>20) {
                break; /* Exit and return false */
            }
        }
        return connected; //return error code
    }
}