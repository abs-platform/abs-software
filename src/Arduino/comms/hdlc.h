
#ifndef _HDLC_H
#define _HDLC_H

#include "Arduino.h"
#include <SPI.h> /* For communication via SPI. */
#include "comms.h"

char *hdlc_rx();
void send_preamble();
void send_packet(char *data, int data_size);
void send_ABORT();
void hdlc_tx(uint8_t *data, int data_size);

#endif