# COMMS DRIVER

## Definition

In the comms driver it has been implemented all the functionalities for the AX5042 transceiver. The required algorithms for the transmission and reception routines are implemented in files hdlc.c and comms.cpp. These contain Main functions (configure, change_x, transmit and receive) . The final SPI connection between the arduino board and the 
transceiver is also defined here through the functions write_register and read_register.

The main code in the comms driver is structured as a class using c++. If using the Arduino IDE, the driver must be imported following the procedure for its IDE.

## Modifications from the previous version
1. Fix errors from the previous version.
2. Addition of one configuration function for each register.
3. Restructuring of the configuration code using functions mentioned in the previous statement.
4. Analysis of the dependencies between registers in order to implement change_x function. Implementation of change_x function.
5. Change the reception algorithm to make an error control and send back to the HWmod all the packet.
6. Integration of the new comms driver version in the Arduino firmware.
7. Creation of new _structs_ and _enums_ in the Arduino firmware.
8. Develop comms_test to test the execution time of configuration and change_x routines.

    
## Future modifications (expected in a few weeks)
1. Implementation of a reception event in order to update the reception info every some time and avoid stopping Arduino from its main routine.
    - Option B. Using interruptions from the transceiver (FIFO full, FIFO empty...).
2. Adaptation of communication protocol with HWmod.