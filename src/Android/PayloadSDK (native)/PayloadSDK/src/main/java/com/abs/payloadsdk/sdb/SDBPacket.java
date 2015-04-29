package com.abs.payloadsdk.sdb;

import android.util.Log;

import java.util.ArrayList;
import java.util.EnumMap;
import java.util.Map;

public class SDBPacket {

    public enum CMD
    {
        ANALOG_WRITE    (131072),
        DIGITAL_WRITE   (131073),
        ANALOG_READ     (131074),
        DIGITAL_READ    (131075),
        DIGITAL_TOOGLE  (131076),
        CREATE_EVENT    (131077),
        INIT_SERIAL     (131077),
        WRITE_SERIAL    (131077),
        READ_SERIAL     (131077),
        DUMP_BUFFER     (131077),
        SET_INTERVAL    (131077),
        OK                   (0),
        OK_DATA              (1),
        ERROR                (2);

        private int cmd;

        private CMD(int cmd) {
            this.cmd = cmd;
        }

        public int getCmd() {
            return cmd;
        }
    }

    private CMD cmd;
    private ArrayList<Byte> args;
    private ArrayList<Byte> data;

    public SDBPacket(CMD cmd, byte[] args,  byte[] data)
    {
        this.cmd = cmd;
        this.args = new ArrayList<Byte>();
        for(byte b: args) {
            this.args.add(b);
        }
        this.data = new ArrayList<Byte>();
        if(data != null) {
            for (byte b : data) {
                this.data.add(b);
            }
        }
    }

    public SDBPacket(int cmd, byte[] data)
    {
        switch (cmd) {
            case 253:
                this.cmd = CMD.OK;
                break;
            case 254:
                this.cmd = CMD.OK_DATA;
                break;
            case 255:
                this.cmd = CMD.ERROR;
                break;
            default:
                this.cmd = CMD.ERROR;
                break;
        }
        Log.d("[ABS]", String.valueOf(data[0]));
    }


    public int getCmd()
    {
        return cmd.getCmd();
    }

    public byte[] getArgs()
    {
        byte[] argsArray = new byte[args.size()];
        for(int i = 0; i < args.size(); i++) {
            argsArray[i] = args.get(i).byteValue();
        }
        return argsArray;
    }

    public byte[] getData()
    {
        byte[] dataArray = new byte[data.size()];
        for(int i = 0; i < data.size(); i++) {
            dataArray[i] = data.get(i).byteValue();
        }
        return dataArray;
    }

    public CMD getCMD()
    {
        return cmd;
    }

    public byte getParameter(int pos)
    {
        return data.get(pos);
    }
}
