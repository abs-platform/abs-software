#ifndef __COMMS_TEST_H
#define __COMMS_TEST_H

#include "Arduino.h"

#define IS_PIN_DIGITAL(p)       ((p) >= 0 && (p) <= 40)
#define IS_PIN_ANALOG(p)        ((p) >= 0 && (p) <= 15)
#define IS_PIN_PWM(p)           digitalPinHasPWM(p)

#define MAX_SERVO 2
#define MAX_SERIAL 2
#define MAX_EVENTS 10
#define MAX_PACKET_SIZE 2 * 1024
#define SD_CS 52
#define COMMS_CS 49
#define TIMER_INTERVAL 50 //50us
#define SERIAL_BITRATE 115200
const long bitrate[] = {300, 1200, 2400, 4800, 9600, 14400,
                        19200, 28800, 38400, 57600,  115200};

typedef struct {
     int command;
     int parameters;
     int cmd_arg1;
     int cmd_arg2;
     int data_size;
     uint8_t *data;
     int packet_id;
 } USBPacket;

typedef struct {
    int bufferid;
    int interval;
    int execute;
    USBPacket *action;
} Event;

typedef enum {
    CONTROL,
    BASIC_IO,
    COMMS,
    EVENT,
    SERVO
} Command;

typedef enum {
    OK,
    OK_DATA,
    ERRORS,
    ABORT
} ParametersControl;

typedef enum {
    ANALOG_WRITE,
    DIGITAL_WRITE,
    ANALOG_READ,
    DIGITAL_READ,
    TOOGLE_PIN
} ParametersBasicIO;

typedef enum {
    INIT_UART,
    READ_UART,
    WRITE_UART,
    INIT_SPI,
    READ_SPI,
    WRITE_SPI,
    INIT_CAN,
    READ_CAN,
    WRITE_CAN,
    CONFIGURE,
    TRANSMIT,
    RECEIVE,
    CHANGE_X
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
    FCS_ERROR  
} Errors;

#endif
