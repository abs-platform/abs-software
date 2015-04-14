package com.google.abs.payloadsdk;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;

import com.google.abs.payloadsdk.Arduino.Arduino;
import com.google.abs.payloadsdk.Measures.Attitude;
import com.google.abs.payloadsdk.Measures.Energy;
import com.google.abs.payloadsdk.Measures.OrbitalState;
import com.google.abs.payloadsdk.Measures.Power;
import com.google.abs.payloadsdk.SBD.SDB;



public class PayloadApp extends Service {

    private SDB sdb;
    private Arduino arduino;

    public PayloadApp(String appName)
    {
        sdb = new SDB();
        sdb.execute();
        arduino = new Arduino(sdb);
        /* sdb.send(new SDBPacket(CmdType.HAND_SHAKE,appName.getBytes())); */
    }

    public Arduino getArduino()
    {
        return arduino;
    }

    public Attitude getAttitude()
    {
        return null;
    }

    public Energy getEnergy()
    {
        return null;
    }

    public Power getPower()
    {
        return null;
    }

    public OrbitalState orbitalState()
    {
        return null;
    }

    @Override
    public IBinder onBind(Intent intent)
    {
        return null;
    }
}
