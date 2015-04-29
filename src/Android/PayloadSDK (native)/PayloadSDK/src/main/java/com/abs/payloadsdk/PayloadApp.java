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

    private Attitude attitude;
    private Power power;
    private Energy energy;
    private OrbitalState orbitalState;


    public PayloadApp(String appName)
    {
        sdb = new SDB();
        arduino = new Arduino(sdb);
        attitude = new Attitude(sdb);
        energy = new Energy(sdb);
        orbitalState = new OrbitalState(sdb);
        power = new Power(sdb);
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

    public String sndMsgApp(int destination, String data)
    {
        return "hello";
    }

    @Override
    public IBinder onBind(Intent intent)
    {
        return null;
    }
}
