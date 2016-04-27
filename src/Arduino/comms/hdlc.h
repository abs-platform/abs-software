
#ifndef _AX5042_H
#define _AX5042_H

#include "Arduino.h"
#include <SPI.h> /* For communication via SPI. */
#include "comms.h"


char *hdlc_rx();

void send_preamble();

void send_packet(unsigned char * data, int data_size);

void send_ABORT();

void hdlc_tx(char *data, int data_size);

#endif