package com.abs.payloadsdk.measures;


import com.abs.payloadsdk.sdb.SDB;

public class Attitude extends SystemData {

    private float acc;
    private float gyro;

    public Attitude(SDB sdb)
    {
        acc = 0;
        gyro = 0;
    }

    public float getAcc()
    {
        return acc;
    }

    public float getGyro()
    {
        return gyro;
    }

}
