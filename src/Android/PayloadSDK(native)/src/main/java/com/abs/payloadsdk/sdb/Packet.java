package com.abs.payloadsdk.sdb;

import javax.xml.transform.OutputKeys;

public class Packet {

    public enum CMD
    {
        MSC_PAYLOAD_ARDUINO_ANALOG_WRITE    (131072),
        MSC_PAYLOAD_ARDUINO_DIGITAL_WRITE   (131073),
        MSC_PAYLOAD_ARDUINO_ANALOG_READ     (131074),
        MSC_PAYLOAD_ARDUINO_DIGITAL_READ    (131075),
        MSC_PAYLOAD_ARDUINO_DIGITAL_TOOGLE  (131076),
        MSC_PAYLOAD_ARDUINO_CREATE_EVENT    (131077),
        OK                                  (0),
        OK_DATA                             (1),
        ERROR                               (2);

        private int cmd;

        private CMD(int cmd)
        {
            this.cmd = cmd;
        }

        public int getCmd()
        {
            return cmd;
        }
    }

    private CMD cmd;
    private byte[] args;
    private byte[] data;

    public Packet(CMD cmd, byte[] args, byte[] data)
    {
        this.cmd = cmd;
        this.args = args;
        this.data = data;
    }

    public Packet(int cmd)
    {
        this.cmd = CMD.OK_DATA;
        this.args = new byte[]{1, 1};
        this.data = new byte[]{1, 1};
    }

    public CMD getCmd()
    {
        return cmd;
    }

    public byte[] getArgs()
    {
        return args;
    }

    public byte[] getData()
    {
        return data;
    }

}
