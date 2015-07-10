package com.google.abs.payloadsdk.SBD;

import java.util.Map;
import java.util.ArrayList;
import android.util.Log;
import java.util.EnumMap;

public class SDBPacket {

    public enum CMD {
        HANDSHAKE,
        DIGITAL_WRITE,
        DIGITAL_READ,
        ANALOG_WRITE,
        ANALOG_READ,
        WRITE_SERIAL,
        READ_SERIAL,
        CREATE_EVENT,
        DUMP_BUFFER,
        OK,
        OK_DATA,
        ERROR
    }

    public static final Map<CMD, byte[]> enumMap =
                new EnumMap<CMD, byte[]>(CMD.class);

    static {
        enumMap.put(CMD.HANDSHAKE,      new byte[]{0, 0, 0});
        /* Basic I/O Arduino */
        enumMap.put(CMD.ANALOG_WRITE,   new byte[]{2, 0, 0});
        enumMap.put(CMD.DIGITAL_WRITE,  new byte[]{2, 0, 1});
        enumMap.put(CMD.ANALOG_READ,    new byte[]{2, 0, 2});
        enumMap.put(CMD.DIGITAL_READ,   new byte[]{2, 0, 3});
        /* Serial comm Arduino */
        enumMap.put(CMD.WRITE_SERIAL,   new byte[]{2, 0, 6});
        enumMap.put(CMD.READ_SERIAL,    new byte[]{2, 0, 7});
        /* Events Arduino */
        enumMap.put(CMD.CREATE_EVENT,   new byte[]{2, 0, 4});
        enumMap.put(CMD.DUMP_BUFFER,    new byte[]{2, 0, 5});
        /* Response packets */
        enumMap.put(CMD.OK,             new byte[]{(byte)0, 0, 0});
        enumMap.put(CMD.OK_DATA,        new byte[]{(byte)0, 0, 1});
        enumMap.put(CMD.ERROR,          new byte[]{(byte)0, 0, 2});
    }

    private CMD cmd;
    private int n_args;
    private ArrayList<Byte> args;
    private int data_size;
    private ArrayList<Byte> data;

    private ArrayList<Byte> parameters;

    public SDBPacket(CMD cmd, byte[] args,  byte[] data)
    {
       this.cmd = cmd;
       this.n_args = args.length;
       this.args = new ArrayList<Byte>();
       for(byte b: args) {
            this.args.add(b);
       }
       this.data = new ArrayList<Byte>();
       if(data != null) {
           this.data_size = data.length;
           for (byte b : data) {
               this.data.add(b);
           }
       } else {
           data_size = 0;
       }
    }

    public SDBPacket(byte[] packet)
    {
        if(packet[0] == (byte)1) {
            this.cmd = CMD.OK;
        } else if(packet[0] == (byte)3) {
            this.cmd = CMD.OK_DATA;
                parameters = new ArrayList<Byte>();
                this.addParameter(packet[1]);
        } else if(packet[0] == (byte)5) {
            this.cmd = CMD.ERROR;
        } else {
            this.cmd = null; //invalid response
        }
    }

    public byte[] toRaw()
    {
        byte[] argsArray = new byte[args.size()];
        byte[] dataArray = new byte[data.size()];
        byte[] n_args = {(byte) args.size()};
        for(int i = 0; i < args.size(); i++) {
            argsArray[i] = args.get(i).byteValue();
        }
        for(int i = 0; i < data.size(); i++) {
            dataArray[i] = data.get(i).byteValue();
        }
        byte[] data_size = {(byte) data.size()};
        return concatenate(enumMap.get(cmd), n_args, argsArray, data_size, dataArray);
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