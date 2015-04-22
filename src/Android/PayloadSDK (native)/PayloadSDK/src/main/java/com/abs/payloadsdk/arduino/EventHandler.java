package com.abs.payloadsdk.arduino;

import com.abs.payloadsdk.sdb.SDB;
import com.abs.payloadsdk.sdb.SDBPacket;

public class EventHandler {

    SDB sdb;
    int eventID;


    public EventHandler(int eventID, SDB sdb)
    {
        this.sdb = sdb;
        this.eventID = eventID;
    }

    public byte dumpBuffer()
    {
        byte[] cmd_args = {(byte)eventID};

        SDBPacket response = sdb.sendSync(new SDBPacket(
                SDBPacket.CMD.DUMP_BUFFER, cmd_args, null));

        byte res = response.getParameter(0);

        return res;
    }
}