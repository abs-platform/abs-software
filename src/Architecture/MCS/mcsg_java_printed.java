public class SDBPacket {

    public enum CMD {

        ARDUINO_GET_PIN (131074),
        TEMPERATURE_ARDUINO1 (65537),
        PROCMAN_1 (0);

        private int cmd;

        private CMD (int cmd)
        {
            this.cmd = cmd;
        }

        public int getCmd ()
        {
            return cmd;
        }
    }

    private CMD cmd;
    private byte [] args;
    private byte [] data;

    public SDBPacket (CMD cmd, byte [] args, byte [] data)
    {
        //...
    }

}