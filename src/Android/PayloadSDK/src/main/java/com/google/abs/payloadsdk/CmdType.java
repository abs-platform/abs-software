package com.google.abs.payloadsdk;

public enum CmdType
{
    DIGITAL_WRITE   (new byte[]{1, 0, 1}),
    DIGITAL_READ    (new byte[]{1, 0, 2}),
    ANALOG_WRITE    (new byte[]{1, 0, 3}),
    ANALOG_READ     (new byte[]{1, 0, 4}),
    WRITE_I2C       (new byte[]{1, 0, 5}),
    READ_I2C        (new byte[]{1, 0, 6}),
    WRITE_UART      (new byte[]{1, 0, 7}),
    READ_UART       (new byte[]{1, 0, 8});

    //TODO

    private final byte[] cmd;

    CmdType(byte[] cmd)
    {
        this.cmd = cmd;
    }

    private byte[] cmd(){return cmd; }

    public byte[] getCmd() {
        return cmd;
    }
}