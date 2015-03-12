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


public class PayloadAPP extends Service {

    private SDB sdb;

    public PayloadAPP()
    {
        sdb = new SDB();
        sdb.execute();
    }

    public Arduino getArduino()
    {
        return new Arduino(sdb);
    }

    public Attitude getAttitude()
    {
        return new Attitude();
    }

    public Energy getEnergy()
    {
        return new Energy();
    }

    public Power getPower()
    {
        return new Power();
    }

    public OrbitalState orbitalState()
    {
        return new OrbitalState();
    }

    @Override
    public IBinder onBind(Intent intent)
    {
        return null;
    }
}
