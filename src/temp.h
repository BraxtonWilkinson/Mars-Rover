/*
 * temp.h
 *
 *  Created on: May 6, 2021
 *      Author: haye9466
 */

#ifndef TEMP_H_
#define TEMP_H_

/*
 * temp_sens.h
 *
 *  Created on: May 5, 2021
 *      Author: haye9466
 */


/*
===============================================================================
 Name        : Proj4.c
 Author      :  Braxton, Sage
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#include <stdio.h>
#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>

// TODO: insert other include files here

// TODO: insert other definitions and declarations here
#define PINSEL1 (*(volatile unsigned int *)0x04002c004) // pin select
#define PCLKSEL0 (*(volatile unsigned int *)0x400Fc1A8) // peripheral clock i2c0
#define I2CONSET (*(volatile unsigned int *)0x4001c000) // set bit
#define I2CONCLR (*(volatile unsigned int *)0x4001c018) // clear bit
#define I2STAT (*(volatile unsigned int *)0x4001c004)
#define I2DAT (*(volatile unsigned int *)0x4001c008) // data
#define I2SCLH (*(volatile unsigned int *)0x4001c010) // duty cycle high
#define I2SCLL (*(volatile unsigned int *)0x4001c014) // duty cycle low
#define FIO0DIR (*(volatile unsigned int *)0x2009c000) // manage address

void wait_si() { // wait for SI bit to flip
    I2CONCLR = (1 << 3);
    while (!(I2CONSET & (1 << 3))) {
        // wait for bit flip
        //printf("waiting \n");
    }
}

void start() { // I2C start condition
    I2CONSET = (1 << 3);
    I2CONSET = (1 << 5);

    wait_si();
    I2CONCLR = (1 << 5);
}
void write(int w) { // I2C read condition
    I2DAT = w;

    wait_si();
    //I2CONCLR = (1<<3);
}
int read(_Bool ack) { // I2C write condition
    if (ack == 1) {
        I2CONCLR = (1 << 2);
    } else {
        I2CONSET = (1 << 2);
    }

    wait_si();
    return I2DAT;
}
void stop() { // I2C stop condition
    I2CONSET = (1 << 4);

    I2CONCLR = (1 << 3);
    while (I2CONSET & (1 << 4)) {
        // wait for bit flip
    }
}
int getTemp() { // get the temperature reading from the sensor
    start();
    write(0x91);
    int msb = read(0); // retrieve most significant byte
    int lsb = read(1); // retrieve least significant byte
    stop();
    // calculate temperature.
    int temperature;
    msb = msb << 3; // shift left by 3
    lsb = lsb >> 5; // shift right by 5
    temperature = msb + lsb; // create 11 bit reading
    temperature = temperature / 8; // divide by 8 to convert from 0.125 resolution
    return temperature;
}
void tempInit() {
    PINSEL1 |= (1 << 22); // set SDA0 0.27
    PINSEL1 |= (1 << 24); // set SCL0 0.28
    FIO0DIR |= (1 << 22);
    FIO0DIR |= (1 << 24);
    I2SCLL = (5 << 0); // set duty cycle
    I2SCLH = I2SCLL + 1;

    I2CONCLR = (1 << 6); // clear
    I2CONSET = (1 << 6); // enable interface
}




#endif /* TEMP_H_ */
