package com.google.abs.payloadsdk.SBD;

import java.util.Map;
import java.util.ArrayList;
import android.util.Log;
import java.util.EnumMap;

public class SDBPacket {

    public enum CMD {
        DIGITAL_WRITE,
        DIGITAL_READ,
        ANALOG_WRITE,
        ANALOG_READ,
        WRITE_SERIAL,
        READ_SERIAL,
        CREATE_EVENT,
        OK,
        OK_DATA,
        ERROR
    }

    public static final Map<CMD, byte[]> enumMap =
                new EnumMap<CMD, byte[]>(CMD.class);

    static {
        /* Basic I/O Arduino */
        enumMap.put(CMD.DIGITAL_WRITE,  new byte[]{1, 0, 0});
        enumMap.put(CMD.DIGITAL_READ,   new byte[]{1, 0, 1});
        enumMap.put(CMD.ANALOG_WRITE,   new byte[]{1, 0, 2});
        enumMap.put(CMD.ANALOG_READ,    new byte[]{1, 0, 3});
        /* Serial comm Arduino */
        enumMap.put(CMD.WRITE_SERIAL,   new byte[]{1, 0, 4});
        enumMap.put(CMD.READ_SERIAL,    new byte[]{1, 0, 5});
        /* Response packets */
        enumMap.put(CMD.OK,             new byte[]{(byte)254, 0, 3});
        enumMap.put(CMD.OK_DATA,        new byte[]{(byte)253, 0, 3});
        enumMap.put(CMD.ERROR,          new byte[]{(byte)255, 0, 3});
    }

    private CMD cmd;
    private ArrayList<Byte> parameters;

    public SDBPacket(CMD cmd)
    {
        this.cmd = cmd;
        Log.d("[ABS]", "SDBPacket created");
    }

    public SDBPacket(CMD cmd, Byte... param)
    {
        this.cmd = cmd;
        parameters = new ArrayList<Byte>();
        for (byte b : param) {
            parameters.add(b);
        }
        Log.d("[ABS]", "SDBPacket created");
    }

    public SDBPacket(byte[] packet)
    {
        if(packet[0] == (byte)253) {
            this.cmd = CMD.OK;
        } else if(packet[0] == (byte)254) {
            this.cmd = CMD.OK_DATA;
            for (int i = 4; i < (int)packet[3]; i++) {
                this.addParameter(packet[i]);
            }
        } else if(packet[0] == (byte)255) {
            this.cmd = CMD.ERROR;
        } else {
            this.cmd = null; //invalid response
        }
    }

    public byte[] toRaw()
    {
        byte[] byteArray = new byte[parameters.size()];
        for(int i = 0; i < parameters.size(); i++) {
            byteArray[i] = parameters.get(i).byteValue();
        }
        return concatenate(enumMap.get(cmd), byteArray);
    }

    private byte[] concatenate(byte[]...arrays)
    {
        int totalLength = 0;
        for (byte[] array : arrays) {
            totalLength += array.length;
        }
        byte[] result = new byte[totalLength];
        int currentIndex = 0;
        for (byte[] array : arrays) {
            System.arraycopy(array, 0, result, currentIndex, array.length);
            currentIndex += array.length;
        }
        return result;
    }

    public void addParameter(byte param)
    {
        parameters.add(param);
        Log.d("[ABS]", "SDB parameter added");
    }

    public CMD getCMD()
    {
        return cmd;
    }

    public byte getParameter(int pos)
    {
        return parameters.get(pos);
    }
}