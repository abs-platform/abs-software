package com.abs.payloadsdk.measures;

import com.abs.payloadsdk.sdb.SDB;

public class Energy extends SystemData {

    private float batteryLevel;
    private int batteryStatus;
    private int batteryTimeRemaining;

    public Energy(SDB sdb)
    {
        this.batteryLevel = 0;
        this.batteryStatus = 0;
        this.batteryTimeRemaining = 0;
    }

    public float getBatteryLevel()
    {
        return batteryLevel;
    }

    public int getBatteryStatus()
    {
        return batteryStatus;
    }

    public int getBatteryTimeRemaining()
    {
        return batteryTimeRemaining;
    }
}
