#include <adk.h>
#include <usbhub.h>
#include <TimerOne.h>
#include <SoftwareSerial.h>
#include <SD.h>
#include <SPI.h>
#include <Servo.h>
#include <avr/wdt.h>
#include "abs.h"
#include <comms.h>
#include <hdlc.h>

Comms comms;

Servo myServo[MAX_SERVO];
SoftwareSerial mySerial[MAX_SERIAL] = {SoftwareSerial(10,11), SoftwareSerial(7,8)};

File buffer;
USB Usb;
Event event_list[MAX_EVENTS];
int time = 0, eventCount = 0, packetCount = 0;

ADK adk(&Usb, "UPC, BarcelonaTech",
              "Android Beyond the Stratoshpere",
              "Android Beyond the Stratoshpere",
              "1.0",
              "http://www.upc.edu",
              "000000000000000001");

void reboot(void)
{
    wdt_enable(WDTO_15MS);
    while(1);
}

void setup(void)
{
    Serial.begin(SERIAL_BITRATE);
    Serial.println("\r\nArduino firmware start");

    if(Usb.Init() == -1) {
        Serial.println("OSCOKIRQ failed to assert");
        while(Usb.Init() == -1); /* retry */
    }

    Timer1.initialize(TIMER_INTERVAL);
    Timer1.pwm(9, 512);
    Timer1.attachInterrupt(events_routine);

    pinMode(SD_CS, OUTPUT);

    if (!SD.begin(SD_CS)) {
        Serial.println("Error initializing SDcard");
    }

    Serial.println("Setup Ok\n");
}

void loop(void)
{
    USBPacket *packet;
    USBPacket *pkt_res;
    uint8_t *response;
    uint8_t msg[MAX_PACKET_SIZE];
    int i = 0;
    char str[80];
    uint8_t ret;
    uint16_t len;

    Usb.Task();

    if(!adk.isReady()) {
        Serial.println("ADK not ready\n");
        return;
    }

    len = sizeof(msg);
    adk.RcvData(&len, msg);
    if(len > 0) {
        packet = process_packet(msg);

        sprintf(str, "Received %d %d:%d %d:%d:%d", packet->packet_id,
                    packet->command, packet->parameters, packet->cmd_arg1,
                    packet->cmd_arg2, packet->data_size);
        Serial.println(str);

        pkt_res = execute_packet(packet);
        free(packet);

        sprintf(str, "Result %d %d:%d %d:%d:%d", pkt_res->packet_id,
                    pkt_res->command, pkt_res->parameters, pkt_res->cmd_arg1,
                    pkt_res->cmd_arg2, pkt_res->data_size);
        Serial.println(str);

        response = to_raw(pkt_res, &len);
        free(pkt_res);

        adk.SndData(len, response);
        free(response);
    }

    for(i = 0; i < eventCount; i++) {
        if(event_list[i].execute == 1) {
            pkt_res = execute_packet(event_list[i].action);
            save_event_data(i, pkt_res);
            free(pkt_res);
            event_list[i].execute = 0;
        }
    }
}

void events_routine(void)
{
    int i = 0;
    for(i = 0; i < eventCount; i++) {
      if(time % event_list[i].interval == 0) {
            Serial.println("Event!!!");
            event_list[i].execute = 1;
        }
    }
    time++;
}
