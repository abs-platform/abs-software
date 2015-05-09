package com.abs.payloadsdk.measures;

import com.abs.payloadsdk.sdb.SDB;
import com.abs.payloadsdk.sdb.SDBPacket;

public class OrbitalState extends SystemData {

    private String TLE;
    private float latitude;
    private float longitude;
    private float[] ECI;
    private float[] IGRF;

    public OrbitalState(SDB sdb)
    {

        this.TLE = null;
        this.latitude = 0;
        this.longitude = 0;
        this.ECI = null;
        this.IGRF = null;
    }

    public float getLatitude()
    {
        return latitude;
    }

    public float getLongitude()
    {
        return longitude;
    }

    public float[] ECI()
    {
        return ECI;
    }

    public float[] IGRF()
    {
        return IGRF;
    }
}