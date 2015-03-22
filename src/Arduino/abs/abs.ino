#include <adk.h>
#include <usbhub.h>
#include "TimerOne.h"
#include <SoftwareSerial.h>
#define MAX_SERIAL 2

typedef enum {
    BASIC_IO = 1,
    COMMS = 2,
    EVENTS = 3
} Format;

struct Event {
  int action;
  int buffer;
  int time;
};

uint8_t rc;
int pin = 0, num=0, res = 0, i = 0; 
uint8_t msg[100] = { 0x00 };
Event event_list[50];

const int bitrate[] = {300, 1200, 2400, 4800, 9600, 14400, 19200, 28800, 38400, 57600,  115200};

SoftwareSerial mySerial[MAX_SERIAL] = {SoftwareSerial(6,7), SoftwareSerial(7,8)};

bool state = false;

USB Usb;
USBHub hub0(&Usb);
USBHub hub1(&Usb);

int eventCount = 0;

ADK adk(&Usb,"UPC, BarcelonaTech",
            "Android Beyond the Stratoshpere",
            "Android Beyond the Stratoshpere",
            "1.0",
            "http://www.upc.edu",
            "000000000000000001");
            

void setup()
{
	Serial.begin(115200);
	//while (!Serial); 
	Serial.println("\r\nADK demo start");
        
        if (Usb.Init() == -1) {
          Serial.println("OSCOKIRQ failed to assert");
          while(1); //halt
        }
        pinMode(11, OUTPUT);
        Timer1.initialize(500000);        
        Timer1.pwm(9, 512);                
        Timer1.attachInterrupt(events);
}

void loop()
{  
   
   //critical time-sensitive code
   Usb.Task(); 
   if( adk.isReady() == false ) {
     return;
   }
   uint16_t length = sizeof(msg);
   rc = adk.RcvData(&length, msg);
 
   if(length > 0) {
      noInterrupts();
      byte cmd = (msg[0]>>5)&0x07;
      switch(cmd) {      
      case BASIC_IO:
        /* Command type: BASIC_IO */
        cmd = (msg[0]>>3)&0x03;
        pin = (msg[1]>>1)&0xFF;
        
        Serial.println(String(cmd));
        
        switch(cmd){
          
          case 0:
            /* Analog Write */
            pinMode(pin, OUTPUT);
            analogWrite(pin, (msg[0]<<5)&0xC0);
            break;
            
          case 1:
            /* Digital Write */
            pinMode(pin, OUTPUT); 
            digitalWrite(pin, (msg[0]>>2)&0x01);
            break;
            
          case 2:
            /* Analog Read */
            res = analogRead(pin);
            break;
            
          case 3:
            /* Digital Read */
            pinMode(pin, INPUT);
            res = digitalRead(pin);
            break;
        }  
        break;   
        
        case COMMS:
          /* Command type: Serial Comms */
          Serial.println("Comms"); 
          switch(cmd){
            num = (msg[1]<<1)&0xFF;
            case 0:
              /* Serial setup */
              if(num <= MAX_SERIAL){
                mySerial[num].begin(bitrate[(msg[0]<<1)&0x07]);
              }
            break;
            
            case 1:
              /* Serial read */
              if(num <= MAX_SERIAL){
                mySerial[num].read();
              }
            break;
            
            case 2:
              /* Serial write */
              if(num <= MAX_SERIAL){
                mySerial[num].println(msg[2]);
              }
            break;
         }
         break;
         
         case EVENTS:
          /* Command type: Events */
          Serial.println("Events"); 
          //TODO
          event_list[eventCount].action = 1; 
          event_list[eventCount].buffer = 1;
          event_list[eventCount].time = 5; t
          eventCount++;
          //TODO
          break;
          
         default:
           Serial.println("Unknown");
           break;
         }
         /* Send response packet */
         interrupts();
       }     
   delay(100);       
}

void events()
{  
   int i = 0;
   //TODO
   while(i<eventCount) {
     Serial.println("Event nº "+String(i));
     i++;
   }
   state = !state;
   digitalWrite(11, state); 
}
