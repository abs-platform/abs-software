package com.abs.payloadsdk;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;

import com.abs.payloadsdk.arduino.Arduino;
import com.abs.payloadsdk.measures.Attitude;
import com.abs.payloadsdk.measures.Energy;
import com.abs.payloadsdk.measures.OrbitalState;
import com.abs.payloadsdk.measures.Power;
import com.abs.payloadsdk.sdb.SDB;


public class PayloadApp extends Service {

    private SDB sdb;
    private Arduino arduino;

    public PayloadApp(String appName)
    {
        sdb = new SDB();
        //sdb.send_sync(new SDBPacket(SDBPacket.CMD.HANDSHAKE, appName.getBytes()));
        arduino = new Arduino(sdb);
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
