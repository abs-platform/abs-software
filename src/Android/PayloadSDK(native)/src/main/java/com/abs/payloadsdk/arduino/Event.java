package com.abs.payloadsdk.arduino;

public class Event {

    public enum ACTIONS
    {
        DIGITAL_READ    (17),
        ANALOG_READ     (18),
        DIGITAL_TOOGLE  (20),
        ANALOG_SWEEP    (17),
        READ_I2C        (17),
        WRITE_I2C       (17),
        READ_SPI        (17),
        WRITE_SPI       (17);


        private int action;

        private ACTIONS(int action)
        {
            this.action = action;
        }

        public int getAction()
        {
            return action;
        }
    }

    ACTIONS action;
    byte[] args;
    byte[] data;

    public Event(ACTIONS action, byte[] args, byte[] data)
    {
        this.action = action;
        this.args = args;
        this.data = data;
    }

    public byte[] toRaw()
    {
        byte[] result = new byte[5];

        result[0] = (byte)((this.action.getAction() << 1) + 1);
        result[1] = (byte)((this.args[0] << 1) + 1);
        result[2] = (byte)((this.args[1] << 1) + 1);
        result[3] = 0;
        result[4] = 0;

        return result;
    }
}
