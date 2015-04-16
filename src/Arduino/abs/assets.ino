USBPacket usb_ok_packet()
{
    USBPacket packet;
    packet.command = 0;
    packet.parameters = 0;
    return packet;
}

USBPacket usb_ok_data_packet(int arg, char *result, int size)
{
    USBPacket packet;
    packet.command = 0; 
    packet.parameters = 1;
    packet.cmd_arg1 = (arg >> 1) & 0x7F;
    packet.data_size = size;
    packet.data = (char *)((arg >> 1) & 0x7F);
    return packet;
}

USBPacket usb_error_packet(int error)
{
    USBPacket packet;
    packet.command = 0;
    packet.parameters = 2;
    packet.cmd_arg1 = error;
    return packet;
}

int save_event_data(int buffer_id, char *data)
{
    Serial.println("Data saved");
    String temp = "buffer";
    temp.concat(buffer_id);
    temp.concat(".txt");
    char filename[temp.length() + 1];
    temp.toCharArray(filename, sizeof(filename));
    if(buffer.available()) {
      buffer = SD.open(filename, FILE_WRITE);
      buffer.println(data[0]);
      buffer.close();
    }
    return 1;
}

void dumpData(int buffer_id)
{
    noInterrupts();
    String temp = "buffer";
    temp.concat(buffer_id);
    temp.concat(".txt");
    char filename[temp.length() + 1];
    temp.toCharArray(filename, sizeof(filename));
    File dataFile = SD.open(filename);
    if (dataFile) {
        adk.SndData(1, (uint8_t *)dataFile.read());
    }
    dataFile.close();
}

uint8_t *to_raw(USBPacket packet, uint16_t *length)
{
    int i = 0;
    uint8_t msg[MAX_PACKET_SIZE]; 
    *length = 4 + packet.data_size;
    
    msg[0] = ((packet.command << 5) & 0xE0) + ((packet.parameters << 1) & 0x1E) + 1;
    msg[1] = (packet.cmd_arg1 << 1) + 1;
    Serial.println(msg[1],HEX);
    msg[2] = (packet.cmd_arg2 << 1) + 1;
    msg[3] = (packet.data_size >> 7) + 1;
    msg[4] = (packet.data_size << 1) + 1; 
    msg[5] = 100;
    msg[5 + packet.data_size] = packet.packet_id << 7;
    return &msg[0];
}

USBPacket process_packet(uint8_t *msg)
{ 
    USBPacket packet;  
    packet.command = (msg[0] >> 5) & 0x07;
    packet.parameters = (msg[0] >> 1) & 0x0F;
    packet.cmd_arg1 = (msg[1] >> 1);
    packet.cmd_arg2 = (msg[2] >> 1);
    packet.data_size = msg[4] >> 1 + ((msg[3] & 0xFE) << 8);
    if(packet.data_size > 0) {  
        packet.pkg = &msg[5];
    }
    packet.packet_id = (packetCount++) % 128; 
    return packet;   
}

USBPacket execute_packet(USBPacket *packet)
{
    USBPacket response;
    char *data;
    int pin, num, value, result, j;

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
                        response = usb_ok_packet();
                    } else {
                        response = usb_error_packet(1);
                    }
                    break;
                case DIGITAL_WRITE:
                    if(IS_PIN_DIGITAL(pin)) { 
                        pinMode(pin, OUTPUT);
                        value = packet->cmd_arg2;
                        digitalWrite(pin, value);
                        response = usb_ok_packet();
                    } else {
                        response = usb_error_packet(1);
                    }
                    break;
                case ANALOG_READ:
                    if(IS_PIN_ANALOG(pin)) {
                        result = analogRead(pin);
                        Serial.println(result);
                        response = usb_ok_data_packet(result, NULL, 1);
                    } else {
                        response = usb_error_packet(1);
                    }
                    break;
                case DIGITAL_READ:
                    if(IS_PIN_DIGITAL(pin)) {
                        pinMode(pin, INPUT);
                        result = digitalRead(pin);
                        response = usb_ok_data_packet(result, NULL, 1);
                    } else {
                        response = usb_error_packet(1);
                    }
                    break;
                default:
                    response = usb_error_packet(1);
                    break;
            }
            break;
        case COMMS:
            /* Command type: Serial Communications */
            num = packet->cmd_arg1; 
            if(num <= MAX_SERIAL) {
                switch(packet->parameters) {
                    case INIT:
                        mySerial[num].begin(packet->cmd_arg2); 
                        break;
                    case READ:
                        data = (char *) mySerial[num].read();
                        response = usb_ok_data_packet(NULL, data, 1);
                        break;
                    case WRITE:
                        Serial.println("Sending data");
                        data = packet->data;
                        for(j = 0; j < packet->data_size; j++) {
                            mySerial[num].print(data[j]);
                        }
                        mySerial[num].print("\n");
                        
                        response = usb_ok_packet();
                        break;
                }
            } else {
                response = usb_error_packet(1);
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
                        event_list[eventCount].action = process_packet(packet->pkg);
                        eventCount++;
                    } else {
                        response = usb_error_packet(1); 
                    }
                    break;
                case DUMP:
                    Serial.println("Dump event");
                    dumpData(packet->cmd_arg1);
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
                        break;
                    case SET_DC:
                        myServo[num].write(value);
                        break;
                    case STOP:
                        myServo[num].detach();
                        break;
                }
                break;
            }
        default:
            /* Command type: Unknown */
            response = usb_error_packet(1);
            break;
    }
    return response;
}
