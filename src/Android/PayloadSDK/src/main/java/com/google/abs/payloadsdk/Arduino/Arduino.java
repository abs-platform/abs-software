package com.google.abs.payloadsdk.Arduino;

import android.util.Log;

import com.google.abs.payloadsdk.CmdType;
import com.google.abs.payloadsdk.SBD.SDB;
import com.google.abs.payloadsdk.SBD.SDBPacket;

public class Arduino {

    private SDB sdb;

    /**
     * Arduino constructor
     * @param sdb
     */

    public Arduino(SDB sdb)
    {
        this.sdb = sdb;
    }

    /**
     * Write a 1 or a 0 value to a digital pin.
     *
     * @param pin   the number of the digital pin you want to write (int)
     * @param value 1 or 0
     * @return      error_code
     */

    public int digitalWrite(int pin, int value)
    {
        byte[] measure = sdb.send(new SDBPacket(CmdType.DIGITAL_WRITE,(byte)pin));
        Log.d("[ABS]", String.valueOf(measure[3]));
        return (int) measure[2];
    }

    /**
     * Reads the value from a specified digital pin.
     *
     * @param pin   the number of the digital pin you want to read (int)
     * @return      1 or 0
     */

    public int digitalRead(int pin)
    {
        byte[] measure = sdb.send(new SDBPacket(CmdType.DIGITAL_READ,(byte)pin));
        return (int) measure[2];
    }

    /**
     * Writes an analog value (PWM wave) to a pin.
     *
     * @param pin   the number of the analog pin you want to write (int)
     * @param value the duty cycle: between 0 and 255
     * @return      error_code
     */

    public int analogWrite(int pin, int value)
    {
        byte[] measure = sdb.send(new SDBPacket(CmdType.ANALOG_WRITE,(byte)pin));
        return (int) measure[2];
    }

    /**
     * Reads the value from a specific analog pin.
     *
     * @param pin   the number of the analog pin you want to read (int)
     * @return      (int) 0 to 1023
     */

    public int analoglRead(int pin)
    {
        byte[] measure = sdb.send(new SDBPacket(CmdType.ANALOG_READ,(byte)pin));
        return (int) measure[2];
    }
}