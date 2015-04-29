package com.abs.payloadsdk.arduino;

import com.abs.payloadsdk.sdb.SDB;
import com.abs.payloadsdk.sdb.SDBPacket;

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
     * Writes an analog value (PWM wave) to a pin.
     *
     * @param pin   the number of the analog pin you want to write (int)
     * @param value the duty cycle: between 0 and 255
     * @return      (integer) error_code
     */

    public int analogWrite(int pin, int value)
    {
        byte[] cmd_args = {(byte)pin, (byte)value};

        SDBPacket response = sdb.sendSync(new SDBPacket(
                SDBPacket.CMD.ANALOG_WRITE, cmd_args, null));

        if(response.getCMD() == SDBPacket.CMD.OK) {
            return 1;
        } else {
            return -1; /* something has gone wrong */
        }
    }

    /**
     * Write a 1 or a 0 value to a digital pin.
     *
     * @param pin   the number of the digital pin you want to write (int)
     * @param value 1 or 0
     * @return      (integer) error_code
     */

    public int digitalWrite(int pin, int value)
    {
        byte[] cmd_args = {(byte)pin, (byte)value};
        SDBPacket response = sdb.sendSync(new SDBPacket(
                SDBPacket.CMD.DIGITAL_WRITE, cmd_args, null));

        if(response.getCMD() == SDBPacket.CMD.OK) {
            return 1;
        } else {
            return -1; /* something has gone wrong */
        }
    }

    /**
     * Reads the value from a specific analog pin.
     *
     * @param pin   the number of the analog pin you want to read (int)
     * @return      (integer) 0 to 1023
     */

    public int analogRead(int pin)
    {
        byte[] array = {(byte)pin};

        SDBPacket response = sdb.sendSync(new SDBPacket(
                SDBPacket.CMD.ANALOG_READ, array, null));

        if(response.getCMD() == SDBPacket.CMD.OK_DATA) {
            return response.getParameter(0) & 0xFF;
        } else {
            return -1; /* something has gone wrong */
        }
    }

    /**
     * Reads the value from a specified digital pin.
     *
     * @param pin   the number of the digital pin you want to read (int)
     * @return      (boolean) 1 or 0
     */

    public int digitalRead(int pin)
    {
        byte[] cmd_args = {(byte)pin};
        SDBPacket response = sdb.sendSync(new SDBPacket(
                SDBPacket.CMD.DIGITAL_READ, cmd_args, null));

        if(response.getCMD() == SDBPacket.CMD.OK_DATA) {
            return response.getParameter(0);
        } else {
            return -1; /* something has gone wrong */
        }
    }

    public int digitalToogle(int pin)
    {
        byte[] array = {(byte)pin};

        SDBPacket response = sdb.sendSync(new SDBPacket(
                SDBPacket.CMD.DIGITAL_TOOGLE, array, null));

        if(response.getCMD() == SDBPacket.CMD.OK_DATA) {
            return response.getParameter(0) & 0xFF;
        } else {
            return -1; /* something has gone wrong */
        }
    }

    public int startSerial(int num)
    {
        byte[] array = {(byte)num, (byte)0};

        SDBPacket response = sdb.sendSync(new SDBPacket(
                SDBPacket.CMD.INIT_SERIAL, array, null));
        return 1;
    }

    public byte writeSerial(int num, byte[] data)
    {
        byte[] array = {(byte)num, (byte)0};

        SDBPacket response = sdb.sendSync(new SDBPacket(
                SDBPacket.CMD.WRITE_SERIAL, array, data));

        return response.getParameter(0);
    }

    public byte readSerial(int num)
    {
        byte[] array = {(byte)num, (byte)0};

        SDBPacket response = sdb.sendSync(new SDBPacket(
                SDBPacket.CMD.READ_SERIAL, array, null));
        return response.getParameter(0);
    }

    /**
     * Creates a new Event and returns an EventHandler
     *
     * @return EventHandler
     */

    public EventHandler createEvent(int interval, byte[] data)
    {
        byte[] cmd_args = {(byte)interval, (byte)0};

        SDBPacket response = sdb.sendSync(new SDBPacket(
                SDBPacket.CMD.CREATE_EVENT, cmd_args, data));

        if(response.getCMD() == SDBPacket.CMD.OK_DATA) {
            return new EventHandler(1, sdb);
        } else {
            return null; /* something has gone wrong */
        }
    }
}

