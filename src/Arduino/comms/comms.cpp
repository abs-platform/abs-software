/*RF implementation: setting up the transceiver AX5042
 * It uses the SPI library
 * 
 * The circuit:
 * - Pins 2, 7, 20, 26 of AX5042 connected to 2,5V Power Supply
 * - Pins
 * - Pin 16 (MISO)of AX5042 connected to MISO (digital pin 50)
 * - Pin 17 (MOSI)of AX5042 connected to MOSI (digital pin 51)
 * - Pin 15 (CLK) of AX5042 connected to SCK (digital pin 52)
 * - Pin 14 (SEL) of AX5042 connected to SS (digital pin 49)
 * 
 * All the programation has be done following the AX5042 Programming Manual
*/

#include "comms.h"

const int chip_select_pin = 49;

/*Setting up the transceiver*/
void configure() {
    pinMode(chip_select_pin, OUTPUT);
    SPI.begin(); 
    
    write_register(AGCTARGET, 0x0E);
    write_register(PLLRNG, 0x01);
    write_register(PLLRNG, (read_register(PLLRNG) | 0x01));

    unsigned int bandsel=1; /* '1' for 433MHz, '0' for 915MHz*/ 
    bandsel = bandsel << 8;
    byte pllloop = 0b00001111;
    pllloop = bandsel | pllloop;
    write_register(PLLLOOP, pllloop);
    
    uint8_t freq0;
    uint32_t freq = FCARRIER/FXTAL*2^24 + 1/2;
    freq0 = freq >> 24;
    write_register(FREQ3, freq0);
    freq0 = freq >> 16;
    write_register(FREQ2, freq0);
    freq0 = freq >> 8;
    write_register(FREQ1, freq0);
    freq0 = freq;
    write_register(FREQ0, freq0);
    write_register(TXPWR, 0x0F);
    int fif=1;
    uint8_t iffreq0;
    uint32_t iffreq = fif/FXTAL*2^17 + 1/2;
    iffreq0 = iffreq >> 8;
    write_register(IFFREQHI, iffreq0);
    iffreq0 = iffreq;
    write_register(IFFREQLO, iffreq0);

    int fdev=H/2*BITRATE;
    uint8_t fsk0;
    uint32_t fsk = fdev/FXTAL*2^24 + 1/2;
    fsk0 = fsk >> 16;
    write_register(FSKDEV2, fsk0);
    fsk0 = fsk >> 8;
    write_register(FSKDEV1, fsk0);
    fsk0 = fsk;
    write_register(FSKDEV0, fsk0);

    uint8_t txr0;
    uint32_t txr = BITRATE/FXTAL*2^24 + 1/2;
    txr0 = txr >> 16;
    write_register(TXRATEHI, txr0);
    txr0 = txr >> 8;
    write_register(TXRATEMID, txr0);
    txr0 = txr;
    write_register(TXRATELO, txr0);

    uint8_t cicdec0;
    uint32_t cicdec = 1.5*FXTAL/(8*1.2*(1+H)*BITRATE);
    cicdec0 = cicdec >> 8;
    write_register(CICDECHI, cicdec0);
    cicdec0 = cicdec;
    write_register(CICDECLO, cicdec0);  

    write_register(MODULATION, 0x09);  

    uint8_t datarate0;
    uint32_t datarate = 2^10*FXTAL/(2*169*BITRATE)+1/2;
    datarate0 = datarate >> 8;
    write_register(DATARATEHI, datarate0);
    datarate0 = datarate;
    write_register(DATARATELO, datarate0);  
  
    uint8_t tmggain0;
    uint32_t tmggain = 2*datarate/32 + 1/2;
    tmggain0 = tmggain >> 8;
    write_register(TMGGAINHI, tmggain0);
    tmggain0 = tmggain;
    write_register(TMGGAINLO, tmggain0);  

    write_register(PHASEGAIN, 0x03);  
    write_register(FREQGAIN, 0x03);  
    write_register(FREQGAIN2, 0x06);  
    write_register(AMPLGAIN, 0x06);  

    uint32_t agcattack = 27 + log(BITRATE/FXTAL)/log(2);
    write_register(AGCATTACK, agcattack);
    uint32_t agcdecay = 27 + log(BITRATE/10*FXTAL)/log(2);
    write_register(AGCDECAY, agcdecay);

    /*LACKS ENCODING PROGRAMMING*/
  
    int fabort = 0; /* '1' to abort packet*/
    byte frmmode = 010; /*HDLC*/
    frmmode = frmmode  << 1;
    byte crcmode = 001; /*CRC-16*/
    crcmode = crcmode <<4;
    byte framing = crcmode |frmmode | fabort;
    write_register(FRAMING, framing);

    write_register(IFMODE, 0x00); /*Frame Mode*/
  
    /*IRQ and DCLK control the switches: '00' OFF, '01' Rx, '10' Tx*/ 
    byte irq_txenz = 11011111;
    byte dclkz= 10111111;
    byte pincfg1 = read_register(PINCFG1);
    pincfg1 = pincfg1 & dclkz & irq_txenz; 
    write_register(PINCFG1, pincfg1);
  
    byte pincfg2 = 01110110;
    write_register(PINCFG2, pincfg2);
}

void tx(char *data,int data_size){
    write_register(PWRMODE, 0x60);
    delay (3);
    uint8_t freq0;
    uint32_t freq = FCARRIER/FXTAL*2^24 + 1/2;
    freq0 = freq >> 24;
    write_register(FREQ3, freq0);
    freq0 = freq >> 16;
    write_register(FREQ2, freq0);
    freq0 = freq >> 8;
    write_register(FREQ1, freq0);
    freq0 = freq;
    write_register(FREQ0, freq0);

    /*LACKS ENCODING*/

    write_register(PWRMODE, 0x6C);
    delay (0.05);
    auto_ranging();
    write_register(PWRMODE, 0x6D);
    delay (0.05);
    hdlc_tx(data, data_size);
    write_register(PWRMODE, 0x00);

}

char *rx(){  
    write_register(PWRMODE, 0x60);
    delay (3);
    int fcarrier=433;
    int fxtal=16.3;
    uint8_t freq0;
    uint32_t freq = fcarrier/fxtal*2^24 + 1/2;
    freq0 = freq >> 24;
    write_register(FREQ3, freq0);
    freq0 = freq >> 16;
    write_register(FREQ2, freq0);
    freq0 = freq >> 8;
    write_register(FREQ1, freq0);
    freq0 = freq;
    write_register(FREQ0, freq0);
 
    /*LACKS ENCODING*/

    write_register(PWRMODE, 0x68);
    delay (0.05);
    auto_ranging();
    write_register(PWRMODE, 0x69);
    delay (0.05);
    char *data = hdlc_rx();
    write_register(PWRMODE, 0x00);
    return data;
  }

/*It has to be especified the value of each case when the protocol
  would be written*/
void change_x(int parameter, int value) {
    switch(parameter) {
          case FREQUENCY:
              uint8_t freq0;
              uint32_t freq = value/FXTAL*2^24 + 1/2;
              freq0 = freq >> 24;
              write_register(FREQ3, freq0);
              freq0 = freq >> 16;
              write_register(FREQ2, freq0);
              freq0 = freq >> 8;
              write_register(FREQ1, freq0);
              freq0 = freq;
              write_register(FREQ0, freq0);
              break;
          case BITRATE:
              int fdev=H/2*value;
              uint8_t fsk0;
              uint32_t fsk = fdev/FXTAL*2^24 + 1/2;
              fsk0 = fsk >> 16;
              write_register(FSKDEV2, fsk0);
              fsk0 = fsk >> 8;
              write_register(FSKDEV1, fsk0);
              fsk0 = fsk;
             write_register(FSKDEV0, fsk0);
             break;
      }
  }

/*Read a register*/
unsigned int read_register(byte this_register) {
    unsigned int result = 0;   // result to return
    byte reading = 0b01111111;
    byte data_to_send = reading & this_register;
    Serial.println(this_register, BIN);
    digitalWrite(chip_select_pin, LOW);
    SPI.transfer(data_to_send);
    result = SPI.transfer(0x00);
    digitalWrite(chip_select_pin, HIGH);
    return (result);
}

/*Function to write a determinated value in a determinated Register*/
void write_register(byte this_register, byte this_value) {
    byte writing = 0b10000000;
    byte data_to_send = writing | this_register;
    digitalWrite(chip_select_pin, LOW);
    SPI.transfer(data_to_send); 
    SPI.transfer(this_value);
    digitalWrite(chip_select_pin, HIGH);
}



 /*Auto range needed after initialize or setting chip in SYNTHRX or SYNTHTX*/
  void auto_ranging(){
    write_register(PLLRANGING,0x08);
}

