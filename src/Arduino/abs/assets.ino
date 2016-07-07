USBPacket *usb_ok_packet(int id)
{
    USBPacket *packet = (USBPacket *)malloc(sizeof(*packet));
    packet->command = CONTROL;
    packet->parameters = OK;
    packet->cmd_arg1 = 0;
    packet->cmd_arg2 = 0;
    packet->data_size = 0;
    packet->packet_id = id;
    return packet;
}

USBPacket *usb_abort_packet(int id)
{
    USBPacket *packet = (USBPacket *)malloc(sizeof(*packet));
    packet->command = CONTROL;
    packet->parameters = ABORT;
    packet->cmd_arg1 = 0;
    packet->cmd_arg2 = 0;
    packet->data_size = 0;
    packet->packet_id = id;
    return packet;
}

USBPacket *usb_ok_data_packet(int id, char *result, int dsize)
{
    USBPacket *packet = (USBPacket *)malloc(sizeof(*packet));
    packet->command = CONTROL;
    packet->parameters = OK_DATA;
    packet->data_size = dsize;
    packet->data = (uint8_t *)result;
    packet->packet_id = id;
    return packet;
}

USBPacket *usb_error_packet(int id, int error)
{
    USBPacket *packet = (USBPacket *)malloc(sizeof(*packet));
    packet->command = CONTROL;
    packet->parameters = ERRORS;
    packet->cmd_arg1 = error;
    packet->data_size = 0;
    packet->packet_id = id;
    return packet;
}

int save_event_data(int buffer_id, USBPacket *packet)
{
    Serial.println("Data saved");
    String temp = "buffer";
    temp.concat(buffer_id);
    temp.concat(".txt");
    char filename[temp.length() + 1];
    temp.toCharArray(filename, sizeof(filename));

    buffer = SD.open(filename, FILE_WRITE);
    if(buffer) {
        buffer.write(packet->data, packet->data_size);
        buffer.close();
        return 1;
    } else {
        return 0;
    }
}

char dumpData(int buffer_id)
{
    String temp = "buffer";
    temp.concat(buffer_id);
    temp.concat(".txt");
    char filename[temp.length() + 1];
    temp.toCharArray(filename, sizeof(filename));
    File dataFile = SD.open(filename);
    char data;
    if (dataFile) {
        noInterrupts();
        if(dataFile.available()) {
            /* TODO: it is reading only one byte */
            data = dataFile.read();
        }
        dataFile.close();
        interrupts();
        return data;
    } else {
      dataFile.close();
      return data;
    }
}

uint8_t *to_raw(USBPacket *packet, uint16_t *len)
{
    int i = 0;
    uint8_t *msg;
    *len = 6 + packet->data_size;

    msg = (uint8_t *)malloc(*len);

    msg[0] = ((packet->command << 5) & 0xE0) +
            ((packet->parameters << 1) & 0x1E) + 1;
    msg[1] = (packet->cmd_arg1 << 1) + 1;
    msg[2] = (packet->cmd_arg2 << 1) + 1;
    msg[3] = (packet->data_size >> 7) + 1;
    msg[4] = (packet->data_size << 1) + 1;
    if (packet->data_size > 0) {
        memcpy(&msg[5], packet->data, packet->data_size);
    }
    msg[5 + packet->data_size] = packet->packet_id << 1;
    return msg;
}

USBPacket *process_packet(uint8_t *msg)
{
    USBPacket *packet = (USBPacket *)malloc(sizeof(*packet));
    packet->command = (msg[0] >> 5) & 0x07;
    packet->parameters = (msg[0] >> 1) & 0x0F;
    packet->cmd_arg1 = (msg[1] >> 1);
    packet->cmd_arg2 = (msg[2] >> 1);
    packet->data_size = (msg[4] >> 1) + ((msg[3] & 0xFE) << 6);
    if(packet->data_size > 0) {
        packet->data = &msg[5];
    }
    packet->packet_id = msg[5 + packet->data_size] >> 1;
    return packet;
}

USBPacket *execute_packet(USBPacket *packet)
{
    USBPacket *response;
    char *received;
    char data;
    char *data_arr;
    int pin, num, value, j;
    int *int_res = (int *)malloc(sizeof(*int_res));

    switch(packet->command) {
        case BASIC_IO:
            /* Command type: Basic I/O */
            pin = packet->cmd_arg1;
            switch(packet->parameters) {
                case ANALOG_WRITE:
                    if(IS_PIN_ANALOG(pin)) {
                        pinMode(pin, OUTPUT);
                        value = packet->cmd_arg2;
                        analogWrite(pin, value);
                        response = usb_ok_packet(packet->packet_id);
                    } else {
                        response = usb_error_packet(packet->packet_id, 1);
                    }
                    break;
                case DIGITAL_WRITE:
                    if(IS_PIN_DIGITAL(pin)) {
                        pinMode(pin, OUTPUT);
                        value = packet->cmd_arg2;
                        digitalWrite(pin, value);
                        response = usb_ok_packet(packet->packet_id);
                    } else {
                        response = usb_error_packet(packet->packet_id, 1);
                    }
                    break;
                case ANALOG_READ:
                    if(IS_PIN_ANALOG(pin)) {
                        *int_res = analogRead(pin);
                        response = usb_ok_data_packet(packet->packet_id,
                                        (char *)int_res, sizeof(*int_res));
                    } else {
                        response = usb_error_packet(packet->packet_id, 1);
                    }
                    break;
                case DIGITAL_READ:
                    if(IS_PIN_DIGITAL(pin)) {
                        pinMode(pin, INPUT);
                        *int_res = digitalRead(pin);
                        response = usb_ok_data_packet(packet->packet_id,
                                    (char *)int_res, sizeof(*int_res));
                    } else {
                        response = usb_error_packet(packet->packet_id, 1);
                    }
                    break;
                case TOOGLE_PIN:
                    pinMode(pin, OUTPUT);
                    digitalWrite(pin, !digitalRead(pin));
                    response = usb_ok_packet(packet->packet_id);
                    break;
                default:
                    response = usb_error_packet(packet->packet_id, 1);
                    break;
            }
            break;
        case COMMS:
            /* Command type: Serial Communications */
            /* TODO: only reading one byte? */
            num = packet->cmd_arg1;
            if(num <= MAX_SERIAL) {
                switch(packet->parameters) {
                    case INIT_UART:
                        mySerial[num].begin(bitrate[packet->cmd_arg2]);
                        response = usb_ok_packet(packet->packet_id);
                        break;
                    case READ_UART:
                        data = mySerial[num].read();
                        response = usb_ok_data_packet(packet->packet_id,
                                                        &data, 1);
                        break;
                    case WRITE_UART:
                        Serial.println("Sending data");
                        for(j = 0; j < packet->data_size; j++) {
                            Serial.write(packet->data[j]);
                            mySerial[num].write(packet->data[j]);
                        }
                        response = usb_ok_packet(packet->packet_id);
                        break;
                     case INIT_SPI:
                         SPI.begin();
                         pinMode(10, OUTPUT);
                         break;
                     case READ_SPI:
                         digitalWrite(10, LOW);
                         data = SPI.transfer(0x00);
                         response = usb_ok_data_packet(packet->packet_id,
                                                        &data, 1);
                         digitalWrite(10, HIGH);
                         break;
                     case WRITE_SPI:
                         for(j = 0; j < packet->data_size; j++) {
                             digitalWrite(10, LOW);
                             SPI.transfer(packet->data[j]);
                             digitalWrite(10, HIGH);
                        }
                        response = usb_ok_packet(packet->packet_id);
                        break;
                    case CONFIGURE:
                        comms.configure();
                        response = usb_ok_packet(packet->packet_id);
                        break;
                    case TRANSMIT:
                        comms.tx(packet->data, packet->data_size);
                        response = usb_ok_packet(packet->packet_id);
                        break;
                    case RECEIVE:
                        received = comms.rx();
                        if(*received == 1) {
                            response = usb_ok_data_packet(packet->packet_id,
                                                        received + 2, *(received + 1));
                        } else if(*received == 2) {
                            response = usb_abort_packet(packet->packet_id);
                        } else {
                            response = usb_error_packet(packet->packet_id, FCS_ERROR);
                        }
                        break;
                    case CHANGE_X:
                        comms.change_x(packet->cmd_arg1, packet->cmd_arg2);
                        response = usb_ok_packet(packet->packet_id);
                        break;
                }
            } else {
                response = usb_error_packet(packet->packet_id, 1);
            }
            break;
        case EVENT:
            /* Command type: Events */
            switch(packet->parameters) {
                case CONF:
                    if(packet->data_size > 0) {
                        event_list[eventCount].bufferid = eventCount;
                        event_list[eventCount].interval = packet->cmd_arg1;
                        event_list[eventCount].execute = 0;
                        event_list[eventCount].action = process_packet(packet->data);
                        eventCount++;
                        response = usb_ok_packet(packet->packet_id);
                    } else {
                        response = usb_error_packet(packet->packet_id, 1);
                    }
                    break;
                case DUMP:
                    Serial.println("Dump event");
                    //response = usb_ok_data_packet(dumpData(0), 1);
                    data = dumpData(packet->cmd_arg1);
                    response = usb_ok_data_packet(packet->packet_id, &data, 1);
                    break;
            }
            break;
        case SERVO:
            /* Command type: PWM */
            num = packet->cmd_arg1;
            value = packet->cmd_arg2;
            if(num <= MAX_SERIAL) {
                switch(packet->parameters) {
                    case START:
                        myServo[num].attach(value);
                        response = usb_ok_packet(packet->packet_id);
                        break;
                    case SET_DC:
                        myServo[num].write(value);
                        response = usb_ok_packet(packet->packet_id);
                        break;
                    case STOP:
                        myServo[num].detach();
                        response = usb_ok_packet(packet->packet_id);
                        break;
                }
                break;
            } else {
                response = usb_error_packet(packet->packet_id, 1);
            }
            break;
        default:
            /* Command type: Unknown */
            response = usb_error_packet(packet->packet_id, 1);
            break;
    }
    return response;
}
