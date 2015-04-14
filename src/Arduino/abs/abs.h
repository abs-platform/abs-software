#ifndef __ABS_H
#define __ABS_H

#include "Arduino.h"

#define IS_PIN_DIGITAL(p)       ((p) >= 0 && (p) <= 40)
#define IS_PIN_ANALOG(p)        ((p) >= 0 && (p) <= 40)
#define IS_PIN_PWM(p)           digitalPinHasPWM(p)

#define MAX_SERVO 2
#define MAX_SERIAL 2
#define MAX_EVENTS 10
#define MAX_PACKET_SIZE 500
const int bitrate[] = {300, 1200, 2400, 4800, 9600, 14400, 19200, 28800, 38400, 57600,  115200};

typedef struct {
     int command;
     int parameters;
     int cmd_arg1;
     int cmd_arg2;
     int data_size;
     char *data;
     uint8_t *pkg;
     int packet_id;
 } USBPacket;

typedef struct {
    int bufferid;
    int interval;
    int execute;
    USBPacket action;
} Event;

typedef enum {
    CONTROL,
    BASIC_IO,
    COMMS,
    EVENT,
    SERVO
} Command;

typedef enum {
    ANALOG_WRITE,
    DIGITAL_WRITE,
    ANALOG_READ,
    DIGITAL_READ 
} ParametersBasicIO;

typedef enum {
    INIT,
    READ,
    WRITE
} ParametersComms;

typedef enum {
    CONF,    
    DUMP
} ParametersEvents;

typedef enum {
    START,
    SET_DC,    
    STOP  
} ParametersServo;

typedef enum {
  
  
} Errors;

#endif
