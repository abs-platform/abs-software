package com.abs.payloadapp;

import android.app.NotificationManager;
import android.content.Context;
import android.content.Intent;
import android.os.IBinder;
import android.support.v4.app.NotificationCompat;
import android.util.Log;

import com.abs.payloadsdk.PayloadApp;
import com.abs.payloadsdk.arduino.Arduino;
import com.abs.payloadsdk.sdb.SDB;

public class MyPayloadApp extends PayloadApp {

    public MyPayloadApp()
    {
        super("a");
    }

    Arduino arduino;

    public void sendNotification(String title, String text)
    {
        Context context = MainActivity.getLastSetContext();

        //Building the Notification
        NotificationCompat.Builder builder = new NotificationCompat.Builder(context);
        builder.setSmallIcon(R.drawable.ic_launcher);
        builder.setContentTitle(title);
        builder.setContentText(text);

        NotificationManager notificationManager = (NotificationManager) getSystemService(
                NOTIFICATION_SERVICE);
        notificationManager.notify(1, builder.build());
    }

    @Override
    public void onCreate()
    {
       arduino = this.getArduino();
       thread.start();

    }

    Thread thread = new Thread() {
        @Override
        public void run() {
            try {
                //byte[] action = {(byte)35, (byte)11, (byte)3, (byte)0, (byte)0};
                //arduino.createEvent(1, action);
                while (true) {
                        arduino.digitalToogle(4);
                        sleep(200);
                }
            } catch (Exception e) {

            }
        }
    };


    @Override
    public void onDestroy(){
        super.onDestroy();
    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId)
    {
        return super.onStartCommand(intent, flags,startId);
    }

}



