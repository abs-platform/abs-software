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

/*The next variables save the changes on the default parameters*/
int frequency;
uint8_t bandsel = BANDSEL;
int bitrate = BITRATE_DEFAULT;
uint8_t modulationValue = 0xFF;//to distinguish it from value 0
uint8_t tmgcorrfrac = TMGCORRFRAC_DEFAULT;
uint8_t cicdec;
uint32_t fskmul;
uint32_t datarate;

Comms::Comms()
{
    /*Constructor not used*/
}

Comms::~Comms()
{
    /*Destructor not used*/
}

/*Setting up the transceiver*/
void Comms::configure()
{
    pinMode(chip_select_pin, OUTPUT);
    SPI.begin();    
    write_register(AGCTARGET, 0x0E);
    write_register(PLLRNG, (read_register(PLLRNG) | 0x01));
    write_register(RXMISC, ((read_register(RXMISC) | 0x01)& 0xFD));//write 01 in the last 2 bits
    configure_PLLLOOP(BANDSEL);
    configure_FREQ(FCARRIER);
    configure_TXPWR();
    configure_IFFREQ();
    configure_FSKDEV(BITRATE_DEFAULT);
    configure_TXRATE(BITRATE_DEFAULT);
    cicdec = configure_CICDEC(BITRATE_DEFAULT);
    configure_MODULATION(MOD_DEFAULT);
    fskmul = compute_FSKMUL(BITRATE_DEFAULT, TMGCORRFRAC_DEFAULT, MOD_DEFAULT, cicdec);
    datarate = configure_DATARATE(BITRATE_DEFAULT, cicdec, fskmul);
    //we need the values of fskmul and datarate for TMGGAIN config
    configure_TMGGAIN(fskmul, datarate, TMGCORRFRAC_DEFAULT);
    configure_AGCATTACK(BITRATE_DEFAULT, MOD_DEFAULT);
    configure_AGCDECAY(BITRATE_DEFAULT, MOD_DEFAULT);
    configure_PHASEGAIN(MOD_DEFAULT);
    configure_FREQGAIN(MOD_DEFAULT);
    configure_FREQGAIN2();
    configure_AMPLGAIN();

    /*LACKS ENCODING PROGRAMMING*/
  
    int fabort = 0; /* '1' to abort packet*/
    byte frmmode = 010; /*HDLC*/
    frmmode = frmmode  << 1;
    byte crcmode = 001; /*CRC-16*/
    crcmode = crcmode << 4;
    byte framing = crcmode |frmmode | fabort;
    write_register(FRAMING, framing);

    write_register(IFMODE, 0x00); /*Frame Mode*/
  
    /*IRQ and DCLK control the switches: '00' OFF, '01' Rx, '10' Tx*/ 
    byte irq_txenz = 0b11011111;
    byte dclkz= 0b10111111;
    byte pincfg1 = read_register(PINCFG1);
    pincfg1 = pincfg1 & dclkz & irq_txenz; 
    write_register(PINCFG1, pincfg1);
  
    byte pincfg2 = 0b01110110;
    write_register(PINCFG2, pincfg2);
}

void Comms::tx(uint8_t *data,int data_size)
{
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

char* Comms::rx()
{  
    write_register(PWRMODE, 0x60);
    delay(3);
    if(frequency == 0){
        configure_FREQ(FCARRIER);
    }else{
        configure_FREQ(frequency);
    }
 
    /*LACKS ENCODING*/

    write_register(PWRMODE, 0x68);
    delay(0.05);
    auto_ranging();
    write_register(PWRMODE, 0x69);
    delay(0.05);
    char *data = hdlc_rx(); 
    write_register(PWRMODE, 0x00);
    return data;
}

/*It has to be specified to the HWmod_comms. Parameters defined in comms.h*/
void Comms::change_x(int parameter, int value)
{
    switch(parameter) {
        case FREQUENCY:
            /*We expect the value to be 1(433) or 0(915)*/
            frequency = ((1-value)*915 + value*433)*10^6;
            configure_FREQ(frequency);
            configure_PLLLOOP(value);
            bandsel = value;
            break;            
        case BITRATE: 
            configure_FSKDEV(value);
            configure_TXRATE(value);
            cicdec = configure_CICDEC(value); 
            if(modulationValue == 0xFF){
                fskmul = compute_FSKMUL(value, tmgcorrfrac, MOD_DEFAULT, cicdec);
            } else {
                fskmul = compute_FSKMUL(value, tmgcorrfrac, modulationValue, cicdec);
            }
            datarate = configure_DATARATE(value, cicdec, fskmul);
            configure_TMGGAIN(fskmul, datarate, tmgcorrfrac);
            configure_AGCATTACK(value); 
            configure_AGCDECAY(value); 
            bitrate = value;
            break;
  
        case MODULATION_TYPE:
            configure_MODULATION(value);
            fskmul = compute_FSKMUL(bitrate, tmgcorrfrac, value, cicdec);
            datarate = configure_DATARATE(bitrate, cicdec, fskmul);
            configure_TMGGAIN(fskmul, datarate, tmgcorrfrac);
            configure_AGCATTACK(bitrate, value);
            configure_AGCDECAY(bitrate, value);
            configure_PHASEGAIN(value);
            configure_FREQGAIN(value);
            modulationValue = value;
            break;
            
        case TMRECOV:
            uint8_t modulation;
            if(modulationValue == 0xFF){
                modulation = MOD_DEFAULT;
            }else{
                modulation = modulationValue;
            } 
            fskmul = compute_FSKMUL(bitrate, value, modulation, cicdec);
            datarate = configure_DATARATE(bitrate, cicdec, fskmul);
            configure_TMGGAIN(fskmul, datarate, value);
            configure_AGCATTACK(bitrate, modulation);
            configure_AGCDECAY(bitrate, modulation);
            configure_PHASEGAIN(modulation);
            configure_FREQGAIN(modulation);
            tmgcorrfrac = value;
            break;
        default:
            /*Parameter not changeable*/
    }
}

/*Read a register*/
unsigned int Comms::read_register(byte this_register)
{
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

/*Function to write a determined value in a determined Register*/
void Comms::write_register(byte this_register, byte this_value)
{
    byte writing = 0b10000000;
    byte data_to_send = writing | this_register;
    digitalWrite(chip_select_pin, LOW);
    SPI.transfer(data_to_send); 
    SPI.transfer(this_value);
    digitalWrite(chip_select_pin, HIGH);
}

/*Auto range needed after initialize or setting chip in SYNTHRX or SYNTHTX*/
void Comms::auto_ranging()
{
    write_register(PLLRANGING, 0x08);
}

/*------------------CONFIGURATION FUNCTIONS---------------------*/
/*This function configures register PLLLOOP*/
void Comms::configure_PLLLOOP(int band)
{
    band = band << 5;
    byte pllloop = band | 0b00001111;
    write_register(PLLLOOP, pllloop);
}

/*The parameter "freq" is supposed to be in Hz.
 *This function configures registers FREQ3, FREQ2,
 *FREQ1, FREQ0*/
void Comms::configure_FREQ(int fcarrier)
{
    uint8_t freq0;
    uint32_t freq = (fcarrier / FXTAL) * pow(2,24) + 1 / 2;
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
void Comms::configure_TXPWR()
{
    write_register(TXPWR, 0x0F);
}

/*This function configures register IFFREQHI, IFFREQLO*/
void Comms::configure_IFFREQ()
{
    uint8_t iffreq0;
    uint32_t iffreq = (FIF / FXTAL) * pow(2,17) + 1 / 2;
    iffreq0 = iffreq >> 8;
    write_register(IFFREQHI, iffreq0);
    iffreq0 = iffreq;
    write_register(IFFREQLO, iffreq0);
}

/*This function configures registers FSKDEV2, FSKDEV1,
*FSKDEV0*/
void Comms::configure_FSKDEV(int bitrate)
{
    int fdev = H / 2 * bitrate;
    uint8_t fsk0;
    uint32_t fsk = (fdev / FXTAL) * pow(2,24) + 1 / 2;
    fsk0 = fsk >> 16;
    write_register(FSKDEV2, fsk0);
    fsk0 = fsk >> 8;
    write_register(FSKDEV1, fsk0);
    fsk0 = fsk;
    write_register(FSKDEV0, fsk0);
}

/*This function configures registers TXRATEHI, TXRATEMID,
*TXRATELO*/
void Comms::configure_TXRATE(int bitrate)
{
    uint8_t txr0;
    uint32_t txr = (bitrate / FXTAL) * pow(2,24) + 1/2;
    txr0 = txr >> 16;
    write_register(TXRATEHI, txr0);
    txr0 = txr >> 8;
    write_register(TXRATEMID, txr0);
    txr0 = txr;
    write_register(TXRATELO, txr0);
}

/*This function configures registers CICDECHI, CICDECLO*/
uint32_t Comms::configure_CICDEC(int bitrate)
{
    uint8_t cicdec0;
    uint32_t cicdec = 1.5 * (FXTAL / (8 * 1.2 *(1 + H) * bitrate));
    cicdec0 = cicdec >> 8;
    write_register(CICDECHI, cicdec0);
    cicdec0 = cicdec;
    write_register(CICDECLO, cicdec0);
    return cicdec;
}

/*This function configures register MODULATION*/
void Comms::configure_MODULATION(int modulation)
{
    uint8_t modValue = getModulationReg(modulation);
    write_register(MODULATION, modValue); 
}

/*This function configures register FSKMUL*/
uint32_t Comms::compute_FSKMUL(int bitrate, uint8_t tmgcorrfrac, uint8_t modValue, uint32_t cicdec)
{
    if(getModulationReg(modValue) < 8){
        fskmul = 1;
    }else{
        unsigned int comprovacion = FXTAL / (4 * bitrate * cicdec);
        if(tmgcorrfrac >= comprovacion){
            fskmul = 1 / (4 * bitrate * (cicdec / FXTAL) + (1 / tmgcorrfrac));
        }
    }
    return fskmul;
}

/*This function configures register DATARATE*/
uint32_t Comms::configure_DATARATE(int bitrate, int cicdec, int fskmul)
{
    uint8_t datarate0;
    datarate = pow(2,10) * (FXTAL / (cicdec * fskmul * bitrate)) + 1 / 2;
    datarate0 = datarate >> 8;
    write_register(DATARATEHI, datarate0);
    datarate0 = datarate;
    write_register(DATARATELO, datarate0);
    return datarate;
}

/*This function configures registers TMGGAINHI, TMGGAINLO*/
void Comms::configure_TMGGAIN(uint32_t fskmul,uint32_t datarate,int tmgcorrfrac)
{
    uint8_t tmggain0;
    uint32_t tmggain = fskmul * (datarate/tmgcorrfrac) + 1/2;
    tmggain0 = tmggain >> 8;
    write_register(TMGGAINHI, tmggain0);
    tmggain0 = tmggain;
    write_register(TMGGAINLO, tmggain0);
}

/*This function configures registers AGCATTACK*/
void Comms::configure_AGCATTACK(int bitrate, uint8_t modValue)
{
    uint8_t agcattack;
    if(modValue == 0){
        /*log2(x) = log(x)/log(2)*/
        agcattack = 27 + log(bitrate / (10 * FXTAL)) / log(2);
    }else{
        agcattack = 27 + log(bitrate / FXTAL) / log(2);
    }
    write_register(AGCATTACK, agcattack);
}

/*This function configures registers AGCDECAY*/
void Comms::configure_AGCDECAY(int bitrate, uint8_t modValue )
{
    uint8_t agcdecay;
    if(modValue == 0){
        agcdecay = 27 + log(bitrate / (100 * FXTAL)) / log(2);
    }else{
        agcdecay = 27 + log(bitrate / (10 * FXTAL)) / log(2);
    }
    write_register(AGCDECAY, agcdecay);
}

/*This function configures registers PHASEGAIN*/
void Comms::configure_PHASEGAIN(uint8_t modValue)
{
    if(modValue == 0){
        write_register(PHASEGAIN, 0x00);
    }else{
        write_register(PHASEGAIN, 0x03);
    }
}

/*This function configures registers FREQGAIN*/
void Comms::configure_FREQGAIN(uint8_t modValue)
{
    if(modValue < 7){
        write_register(FREQGAIN, 0x06);
    }else{
        write_register(FREQGAIN, 0x03);
    }
}

/*This function configures registers FREQGAIN2*/
void Comms::configure_FREQGAIN2()
{
    write_register(FREQGAIN2, 0x06);
}

/*This function configures registers AMPLGAIN*/
void Comms::configure_AMPLGAIN()
{
    write_register(AMPLGAIN, 0x06); 
}

/*--------------------------------------------------------------*/

/*This function translates the String modulation into the value to be assigned
*to the register in hexadecimal*/
uint8_t Comms::getModulationReg(int modulation)
{
    uint8_t modValue;
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