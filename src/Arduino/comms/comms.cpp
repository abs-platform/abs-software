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
 * All the programming has be done following the AX5042 Programming Manual
*/

#include "comms.h"

const int chip_select_pin = 49;

/*The next variables save the changes on the default ones*/
int frequency;
int bitrate = BITRATE;
uint8_t modulationValue = 0xFF;//to distinguish it from value 0

/*Setting up the transceiver*/
void configure() {
    pinMode(chip_select_pin, OUTPUT);
    SPI.begin();    
    write_register(AGCTARGET, 0x0E);
    write_register(PLLRNG, (read_register(PLLRNG) | 0x01));
    write_register(RXMISC, ((read_register(RXMISC) | 0x01)& 0xFD)//write 01 in the last 2 bits
    configure_PLLLOOP(BANDSEL);
    configure_FREQ(FCARRIER);
    configure_TXPWR();
    configure_IFFREQ(FXTAL);
    configure_FSKDEV(BITRATE);
    configure_TXRATE(BITRATE);
    configure_CICDEC(BITRATE);
    configure_MODULATION(MOD_DEFAULT);
    uint32_t fskmul = configure_FSKMUL(BITRATE,TMGCORRFRAC_DEFAULT);
    uint32_t datarate = configure_DATARATE(BITRATE);
    //we need the values of fskmul and datarate for TMGGAIN config
    configure_TMGGAIN(fskmul,datarate,TMGCORRFRAC_DEFAULT);
    configure_AGCATTACK(BITRATE);
    configure_AGCDECAY(BITRATE);
    configure_PHASEGAIN();
    configure_FREQGAIN();
    configure_FREQGAIN2();
    configure_AMPLGAIN();
    configure_AMPLGAIN();

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
    if(frequency == NULL){
        configure_FREQ(FCARRIER);
    }else{
        configure_FREQ(frequency);
    }

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
    if(frequency == NULL){
        configure_FREQ(FCARRIER);
    }else{
        configure_FREQ(frequency);
	}
 
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

/*It has to be specified the value of each case when the protocol
  would be written*/
void change_x(int parameter, int value) {
    switch(parameter) {
        case FREQUENCY:
			/*We expect the value to be 1(433) or 0(915)*/
			frequency = ((1-value)*915 + value*433)*10^6;
            configure_FREQ(frequency);
            configure_PLLLOOP(value);
            break;
			
        case BITRATE
            uint32_t tmgcorrfrac = read_register(TMGCORRFRAC);    
            configure_FSKDEV(value);
            configure_TXRATE(value);
            configure_CICDEC(value); 
            if(modulationValue == 0xFF){
				uint32_t fskmul = configure_FSKMUL(value,tmgcorrfrac, MOD_DEFAULT);
			} else {
				uint32_t fskmul = configure_FSKMUL(value,tmgcorrfrac, modulationValue);
			}
            uint32_t datarate = configure_DATARATE(value);
            configure_TMGGAIN(fskmul, datarate, tmgcorrfrac)
            configure_AGCATTACK(value); 
            configure_AGCDECAY(value); 
            bitrate=value;
            break;
  
        case MODULATION:
            uint32_t tmgcorrfrac = read_register(TMGCORRFRAC);
            configure_MODULATION(value);
            uint32_t fskmul = configure_FSKMUL(bitrate,tmgcorrfrac, value);
            uint32_t datarate = configure_DATARATE(bitrate);
            configure_TMGGAIN(fskmul, datarate, tmgcorrfrac);
            break;
			
        case TMRECOV:
            if(modulationValue==0xFF){
                modulation = MOD_DEFAULT;
            }else{
                modulation=modulationValue;
            }
			uint32_t tmgcorrfrac = read_register(TMGCORRFRAC); 
            uint32_t fskmul = configure_FSKMUL(bitrate,value, modulationValue);
            uint32_t datarate = configure_DATARATE(value,bitrate);
            configure_TMGGAIN(fskmul, datarate, tmgcorrfrac);
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

/*------------------CONFIGURATION FUNCTIONS---------------------*/
/*This function configures register PLLLOOP*/
void configure_PLLLOOP(int band){
    bandsel = band << 5;
    byte pllloop = bandsel | 0b00001111;
    write_register(PLLLOOP, pllloop);
}

/*The parameter "freq" is supposed to be in Hz.
 *This function configures registers FREQ3, FREQ2,
 *FREQ1, FREQ0*/
void configure_FREQ(int freq){
    uint8_t freq0;
    uint32_t freq = freq/FXTAL*2^24 + 1/2;
    freq0 = freq >> 24;
    write_register(FREQ3, freq0);
    freq0 = freq >> 16;
    write_register(FREQ2, freq0);
    freq0 = freq >> 8;
    write_register(FREQ1, freq0);
    freq0 = freq;
    write_register(FREQ0, freq0);  
}

/*This function configures register TXPWR*/
void configure_TXPWR(){
    write_register(TXPWR, 0x0F);
}

/*This function configures register IFFREQHI, IFFREQLO*/
void configure_IFFREQ(){
    int fif=1;
    uint8_t iffreq0;
    uint32_t iffreq = fif/FXTAL*2^17 + 1/2;
    iffreq0 = iffreq >> 8;
    write_register(IFFREQHI, iffreq0);
    iffreq0 = iffreq;
    write_register(IFFREQLO, iffreq0)
}

/*This function configures registers FSKDEV2, FSKDEV1,
*FSKDEV0*/
void configure_FSKDEV(int bitrate){
    int fdev=H/2*bitrate;
    uint8_t fsk0;
    uint32_t fsk = fdev/FXTAL*2^24 + 1/2;
    fsk0 = fsk >> 16;
    write_register(FSKDEV2, fsk0);
    fsk0 = fsk >> 8;
    write_register(FSKDEV1, fsk0);
    fsk0 = fsk;
    write_register(FSKDEV0, fsk0);
}

/*This function configures registers TXRATEHI, TXRATEMID,
*TXRATELO*/
void configure_TXRATE(int bitrate){
    uint8_t txr0;
    uint32_t txr = bitrate/FXTAL*2^24 + 1/2;
    txr0 = txr >> 16;
    write_register(TXRATEHI, txr0);
    txr0 = txr >> 8;
    write_register(TXRATEMID, txr0);
    txr0 = txr;
    write_register(TXRATELO, txr0);
}

/*This function configures registers CICDECHI, CICDECLO*/
void configure_CICDEC(int bitrate){
    uint8_t cicdec0;
    uint32_t cicdec = 1.5*FXTAL/(8*1.2*(1+H)*bitrate);
    cicdec0 = cicdec >> 8;
    write_register(CICDECHI, cicdec0);
    cicdec0 = cicdec;
    write_register(CICDECLO, cicdec0);
    }

/*This function configures register MODULATION*/
void configure_MODULATION(int modulation){
    uint8_t modValue = getModulationReg(modulation);
    write_register(MODULATION, modValue); 
}

/*This function configures register FSKMUL*/
uint32_t configure_FSKMUL(int bitrate, int tmgcorrfrac, int modValue){
    unsigned int = fskmul;
    if(getModulationReg(modValue) < 8){
        fskmul = 1;
    }else{
        unsigned int comprovacion = FXTAL/(4*bitrate*cicdec);
        if(tmgcorrfrac >= comprovacion){
            fskmul = 1/(4*bitrate*cicdec/FXTAL+1/tmgcorrfrac);
        }
	}
    write_register(FSKMUL,fskmul);
    return fskmul;
}

/*This function configures register DATARATE*/
uint32_t configure_DATARATE(int bitrate){
    uint8_t datarate0;
    uint32_t datarate = 2^10*FXTAL/(2*169*bitrate)+1/2;
    datarate0 = datarate >> 8;
    write_register(DATARATEHI, datarate0);
    datarate0 = datarate;
    write_register(DATARATELO, datarate0);
    return datarate;
}

/*This function configures registers TMGGAINHI, TMGGAINLO*/
void configure_TMGGAIN(uint32_t fskmul,uint32_t datarate,int tmgcorrfrac){
    uint8_t tmggain0;
    uint32_t tmggain = fskmul*datarate/tmgcorrfrac + 1/2;
    tmggain0 = tmggain >> 8;
    write_register(TMGGAINHI, tmggain0);
    tmggain0 = tmggain;
    write_register(TMGGAINLO, tmggain0);
}

/*This function configures registers AGCATTACK*/
void configure_AGCATTACK(int bitrate){
    uint32_t modValue = read_register(MODULATION);
    uint32_t agcattack;
    if(modValue == 0){
        agcattack = 27 + log(bitrate/(10*FXTAL))/log(2);
    }else{
        agcattack = 27 + log(bitrate/FXTAL)/log(2);
    }
    write_register(AGCATTACK, agcattack);
}

/*This function configures registers AGCDECAY*/
void configure_AGCDECAY(int bitrate){
    uint32_t modValue = read_register(MODULATION);
    uint32_t agcdecay;
    if(modValue == 0){
        agcdecay = 27 + log(bitrate/(100*FXTAL))/log(2);
    }else{
        agcdecay = 27 + log(bitrate/(10*FXTAL))/log(2);
    }
    write_register(AGCDECAY, agcdecay);
}

/*This function configures registers PHASEGAIN*/
void configure_PHASEGAIN(){
    uint32_t modValue = read_register(MODULATION);
    if(modValue == 0){
        write_register(PHASEGAIN, 0x00);
    }else{
        write_register(PHASEGAIN, 0x03);
    }
}

/*This function configures registers FREQGAIN*/
void configure_FREQGAIN(){
    modValue = read_register(MODULATION);
    if(modValue < 7){
        write_register(FREQGAIN, 0x06);
    }else{
        write_register(FREQGAINEQ, 0x03);
    }
}

/*This function configures registers FREQGAIN2*/
void configure_FREQGAIN2(){
    write_register(FREQGAIN2, 0x06);
}

/*This function configures registers AMPLGAIN*/
void configure_AMPLGAIN(){
    write_register(AMPLGAIN, 0x06); 
}

/*--------------------------------------------------------------*/

/*This function translates the String modulation into the value to be assigned
*to the register in hexadecimal*/
uint8_t getModulationReg(int modulation){
    switch (modulation){
        case ASK:
            modValue = 0x00;
            break;
        case PSK:
            modValue = 0x04;
            break;
        case OQPSK:
            modValue = 0x06;
            break;
        case MSK:
            modValue = 0x07;
            break;
        case FSK:
            modValue = 0x0B;
            break;
        case GFSK:
		    modValue = 0x0F;
            break;
    }
    return modValue;
}