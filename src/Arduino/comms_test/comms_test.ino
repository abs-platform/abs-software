#include <hdlc.h>
#include <comms.h>
#include <adk.h>
#include <usbhub.h>
#include <TimerOne.h>
#include <SoftwareSerial.h>
#include <SD.h>
#include <SPI.h>
#include <Servo.h>
#include <avr/wdt.h>
#include "comms_test.h"

Comms comms;

Servo myServo[MAX_SERVO];
SoftwareSerial mySerial[MAX_SERIAL] = {SoftwareSerial(10,11), SoftwareSerial(7,8)};

File buffer;
USB Usb;
Event event_list[MAX_EVENTS];
int timer = 0, eventCount = 0, packetCount = 0;
char str[80];
    int config_done, config_time;
    int changex_done, changex_time;
    int transmit_done, transmit_time;

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

    /*if(Usb.Init() == -1) {
        Serial.println("OSCOKIRQ failed to assert");
        while(Usb.Init() == -1); /* retry */
    //}

    Timer1.initialize(TIMER_INTERVAL);
    Timer1.pwm(9, 512);
    Timer1.attachInterrupt(events_routine);

    pinMode(SD_CS, OUTPUT);

    /*if (!SD.begin(SD_CS)) {
        Serial.println("Error initializing SDcard");
    }
*/
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

    packet->packet_id = 1;
    packet->command = COMMS;
    packet->parameters = CONFIGURE;

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

    //response = to_raw(pkt_res, &len);
    free(pkt_res);
    config_done = 1;

    packet->packet_id = 2;
    packet->command = COMMS;
    packet->parameters = CHANGE_X;
    packet->cmd_arg1 = BITRATE;
    packet->cmd_arg2 = 50000;


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

    //response = to_raw(pkt_res, &len);
    free(pkt_res);
    changex_done = 1;
}

void events_routine(void)
{
    if(config_done){
      config_time = timer*TIMER_INTERVAL/1000; //TIMER_INTERVAL in us
      config_done = 0;
      sprintf(str,"The execution time of configuration was %d ms", config_time);
      Serial.println(str);
      timer = 0;
    }else if(changex_done){
      changex_time = timer*TIMER_INTERVAL/1000;
      changex_done = 0;
      sprintf(str,"The execution time of change_X was %d ms", changex_time);
      Serial.println(str);
      timer = 0;
    }else if(transmit_done){
      transmit_time = timer*TIMER_INTERVAL/1000;
      transmit_done = 0;
      sprintf(str,"The execution time of transmit was %d ms", transmit_time);
      Serial.println(str);
      
      timer = 0;
    }
    timer++;
}
