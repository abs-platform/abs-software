#include <adk.h>
#include <usbhub.h>
#include <TimerOne.h>
#include <SoftwareSerial.h>
#include <SD.h>
#include <Servo.h> 
#include "abs.h"

Servo myServo[MAX_SERVO];
SoftwareSerial mySerial[MAX_SERIAL] = {SoftwareSerial(10,11), SoftwareSerial(7,8)};

File buffer;
uint8_t msg[MAX_PACKET_SIZE];
uint16_t length;
USBPacket packet;
USB Usb;
USBHub hub0(&Usb);
USBHub hub1(&Usb);
Event event_list[MAX_EVENTS];
int time=0, eventCount = 0, packetCount = 0; 
USBPacket res;
uint8_t *response;

ADK adk(&Usb, "UPC, BarcelonaTech",
              "Android Beyond the Stratoshpere",
              "Android Beyond the Stratoshpere",
              "1.0",
              "http://www.upc.edu",
              "000000000000000001");

void setup(void)
{
    Serial.begin(115200);
    //while(!Serial); 
    Serial.println("\r\nArduino firmware start");
        
    if(Usb.Init() == -1) {
        Serial.println("OSCOKIRQ failed to assert");
        while(Usb.Init() == -1); /* retry */
    }
    Timer1.initialize(500000);
    Timer1.pwm(9, 512);
    Timer1.attachInterrupt(events_routine);
    
    pinMode(3, OUTPUT);
   
    if (!SD.begin(3)) {
        Serial.println("Error initializing SDcard");
    }
}

void loop(void)
{  
    int i = 0;
    Usb.Task(); 
    if(!adk.isReady()) { 
        return; /* restart Arduino firmware */
    }
    
    length = sizeof(msg);
    adk.RcvData(&length, msg);
    if(length > 0) {
        packet = process_packet(msg);
        res = execute_packet(&packet);
        response = to_raw(res, &length);
        char str[80];
        sprintf(str,"%x:%x:%x:%x length %d",response[0],response[1],response[2],response[3],length);
        Serial.println(str);
        adk.SndData(length, response);   
        adk.SndData(length, response);  
        free(response);
       
    }    
   
    for(i = 0; i < eventCount; i++) {
        if(event_list[i].execute == 1) {
            packet = event_list[i].action;
            res = execute_packet(&packet);
            save_event_data(i, (char *) res.data);
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
