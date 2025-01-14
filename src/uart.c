/*
===============================================================================
 Name        : uartstuff.c
 Author      : $Sage
 Version     :
 Copyright   :
 Description : main definition
===============================================================================
ROVER
===============================================================================
*/

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>

#include <stdio.h>
#include "temp.h"
#include "color.h"
#include "range.h"

// sonar definitions

//UART2 PINS: Tx-40 (P0.10) bit21:20; Rx-41 (P0.11) bit23:22; val 01
#define PCONP    (*(volatile unsigned int *)0x400FC0C4) //power settings
#define PCLKSEL1 (*(volatile unsigned int *)0x400FC1AC) //pclk cclk settings
#define PINSEL0  (*(volatile unsigned int *)0x4002C000)
#define PINMODE0 (*(volatile unsigned int *)0x4002C040)

//UART2 definitions
#define U2LCR    (*(volatile unsigned int *)0x4009800C) //latch control register, enables baudrate changes
#define U2FCR    (*(volatile unsigned int *)0x40098008) //"first-in first-out" register, needed for transfer protocol
#define U2DLL    (*(volatile unsigned int *)0x40098000) //baud divisor
#define U2LSR    (*(volatile unsigned int *)0x40098014) //status codes, ref: pg. 317
#define U2IIR    (*(volatile unsigned int *)0x40098008) //interrupt identifier (stores received interrupts)
#define U2IER    (*(volatile unsigned int *)0x40098004) //rx interrupt enabler
#define U2RBR    (*(volatile unsigned int *)0x40098000) //rx data register
#define U2THR    (*(volatile unsigned int *)0x40098000) //tx data register

//------------------interrupts & priority-----------------------------
//[[[[[[[[[[[[[[READ THIS]]]]]]]]]]]]]]]]]
//--ISER0 contains all needed interrupt enable/disable (table 60; pg 87)
//bit 7 -- UART2; bit 18:21 --P0:P3 ext. interrupts, with 18 references P0, 19 referencing P1, etc.
//For keypad:
//one key column will need to be used to trigger the first interrupt through the ext. interrupts
//this will have to be enabled through the ISER0 register

//---IPER registers control interrupt priority
//highest priority is 0x00, writing will lower priority
//This likely will not needed to be messed with, if works as intended
#define ISER0    (*(volatile unsigned int *)0xE000E100) //interrupt enabler
#define IABR0	 (*(volatile unsigned int *)0xE000E300) //Interrupt active bit
#define IPR1    (*(volatile unsigned int *)0xE000E404)  //Priority1 register --contains UART priority
#define IPR4    (*(volatile unsigned int *)0xE000E410)  //Priority4 register --contains P0 & P1 priorities
#define IPR5    (*(volatile unsigned int *)0xE000E414)  //Priority5 register --contains P2 & P3 priorities

//==================wait==================
void wait(int ticks){
	int i=0;
	volatile double dumb=0;
	for(i=0; i<ticks;i++){
		dumb++;
	}
}

//================baudrate================
//baud can be adjusted through the U2DLL
//don't through random numbers in there though
//must be an integer result or all hell breaks loose
//the manual explains this best in pg. 325
//2-precalculated bauds are available. just switch out the comments

//TO DO: Test on scope to verify calculations
void Baud(){
	PCLKSEL1 |= (01<<16); //set UART2 PCLK to 4MHz
	U2LCR    |= (1<<7);   //enable access to latches DLL & DLM- disables most UART functions
	U2DLL	 |= 250;	  //low baud for testing 1k baud
//	U2DLL	 |= 25;	      //divide by 16x2500 (=40k) to get 10k baudrate
	U2LCR    &= ~(1<<7);  //re-enables interrupts U2IER, table 276 and other normal functions
} //end

//=========================================
//===============Initializer===============
//=========================================
//sets presets for basic UART function
//must be called at least once to enable UART functions
void uartInit(){
PCONP 	 |= (1<<24);    //send power to UART 2 periphery
Baud();					//establish 10k baudrate
ISER0	 = (1<<7);		//enable UART2 interrupts
IPR1	 |=(1<<27);		//lower UART2 Interrupt priority by 1
U2LCR	 |= 3;			//set transfer size to 1 byte
//U2LCR	 |= (1<<2);		//2 stop bits
U2IER	 |= (1);		//enable UART2 Interrupts
U2IER	 |= (1<<2);		//enable status line interrupts
U2IER	 |= 2;			//enable transmission interrupts
//transmission interrupts allow for the device to prioritize
//transmitting when requested to overpower the read function

U2FCR	 |= (1);		//enable fifo
//U2FCR		|=(1<<6);  //uncomment for 4bit rx interrupt, default 1bit
PINSEL0  |= (01<<20);   //set tx pin
PINSEL0  |= (01<<22);   //set rx pin

} //end

//===========================================
//================Keypad=====================
//===========================================
//---intended functionality
//pressing the activation key will trigger the interrupt overriding the UART rx interrupt
//from there 4 bits can be loaded for transmission to the Rover board
//once all 4 bits have been selected via keypad, the nibble will be transmitted to the rover
//rover will run whatever function the nibble maps to before then transmitting the data back

//---implementation
//-Dedicate a Key to activate keypad use
//-map 8 keys to bits
//columns indicate bit significance (ie column 1 indicates the MSB & column 4 indicates the LSB)
//2 rows indicate T/F value
//1st row are binary 1's, 2nd row are binary 0's
//for example: pressing the column 3, row 2 key will indicate a
//Replace EINT0_IRQHandler with relevant number for port of the GPIO pin used activate keypad
void EINT0_IRQHandler(void){
/*	if ((IABR0<<18/19/20/21) & 1){ //<<pick which port is being used bit18=P0 bit21=P3
		char bit0=0;
		char bit1=0;
		char bit2=0;
		char bit3=0;
		char buttonpressed=0;
		while(buttonpressed<4){
			if
		}
	*/
}
//===============transmit=====================
void transmit(int sendbyte){
	printf("transmitting\n");
	U2THR = sendbyte;
	//TRUNCATION TENDS TO OCCUR
	//if what you're getting on the other's rx line doesn't match the tx data sent
	//check the binary for truncation
	//this happens due to start bits (found in initialize())
	//with 1 start bit, I found that the LSB gets sacrificed meaning losing the distinction between odd and even ints
	//this can be worked-around'ed by only transmitting even integers (basically keeping 2^0 = 0)
	//it cuts down on the amount of usable data, but not sure what else to do
}

//==============receive======================
unsigned char receive(void){
	unsigned char In = 0;
	In = U2RBR;
return In;
}

//==============================================
//============RX Interrupt Handler==============
//==============================================
//checks for interrupts on the Rx pin
//Triggers on Low (well it should, but it's just always triggered, kind of like my ex)
//After being triggered the function checks what triggered it through the IIR (the Interrupt Identifier library)
//All received data (RD or rx data for short) comes in through this function
//CHECK THE ALL SACRED CASE 3
//Also provides Rx error codes

void UART2_IRQHandler(void){
	if((IABR0>>7) & 1){  //is this condition right? Try switching with IIR?
//		printf("Im here \n");
		int			  coloread=0;
		unsigned char intcode=0;
		unsigned char received=0;
		unsigned char RLS=0;
		unsigned char timedoutdata=0;
		unsigned char interrupt=0;
		unsigned char temp=0;
		unsigned char range=0;
		interrupt=(U2IIR>>1 & 0x07);
		switch(interrupt) {
//			case 1:    //THRE interrupt (THRE interrupts are unused)
//				printf("oh hey this hasn't been implemented yet \n");
//				transmit(0x32);
//				break;
//			case 2 never worked due to how interrupt clearing works
// 	 	 	rx data gets read in the all sacred case 3 instead
			case 2:    //Received Data Ready
				received=receive();
				//printf("data recieved %u \n", received);
				switch(received){ //RD now triggers related function
															//case values will likely need
															//to be changed because
															//of weird stuff with
															//the start condition
															//I'll handle that part
															case 182:
																coloread=color();
																printf("color: %d \n", coloread);
																transmit(40);
																wait(100);
																transmit(coloread);
															break;

															case 170:
																temp=getTemp();
																printf("temp is %u C \n", temp);
																transmit(32);
																wait(100);
																transmit(temp);
																//place function here
														    break;

															case 154:
																range=sonarTrigger();
																printf("there is an object %u inches in front of the rover \n", range);
																transmit(24);
																wait(100);
																transmit(range);
																//place function here
														    break;

															default:
															//defaults
															break;
												}

				break;
			case 3:    //Line Status Message
	//			printf("RLS ");
				RLS=U2LSR;
	//			printf("%u \n", RLS);

				  if(U2LSR & 1){
					received=receive();
					/*printf("LSR RECIEVE %u \n", received);
						switch(received){ //RD now triggers related function
							//case values will likely need
							//to be changed because
							//of weird stuff with
							//the start condition
							//I'll handle that part
							case 1:
								//place function here
							break;

							case 170:
								temp=getTemp();
								printf("temp is %u C \n", temp);
								//place function here
							break;

							default:
								//if all else fails
								printf("rx: %u \n", received);
							break;
						} //case end*/
					}

					//any additional thing that you want to happen after RDA events
				break; //ends THE ALL SACRED CASE 3

//cases 4 and 5 undefined and will never be defined

			case 6:    //Character Time Out
				//if baud is still at a low rate for testing purposes
				//expect this to happen
				//baud can be adjusted in baud()
				timedoutdata=receive();
				//printf("CTI %u \n", timedoutdata);
				break;

			default:     //Interrupt doesn't match any used codes
				//this shouldn't happen, but if it does, then you'll at least have some clues
				//reference U2IIR (pg. 313) the answer probably lies there
				//printf("uhh something else came through: %u \n", interrupt);
				break;
			}
/*		switch(received){ //RD now triggers related function
									//case values will likely need
									//to be changed because
									//of weird stuff with
									//the start condition
									//I'll handle that part
									case 1:
									//place function here
									break;

									case 170:
									temp=getTemp();
									printf("temp is %u C \n", temp);
									transmit(temp);
									//place function here
								    break;
									default:
									//defaults
									break;
						}*/


		//====================time out===========================
		//*
		switch(timedoutdata){ //RD now triggers related function
											//case values will likely need
											//to be changed because
											//of weird stuff with
											//the start condition
											//I'll handle that part
											case 182:
												coloread=color();
												printf("color: %d \n", coloread);
												transmit(40);
												wait(100);
												transmit(coloread);
											break;

											case 170:
												temp=getTemp();
												printf("temp is %u C \n", temp);
												transmit(32);
												wait(100);
												transmit(temp);
												//place function here
										    break;

											case 154:
												range=sonarTrigger();
												printf("there is an object %u inches in front of the rover \n", range);
												transmit(24);
												wait(100);
												transmit(range);
												//place function here
										    break;

											default:
											//defaults
											break;
								}

	}
}

//=================MAIN======================
int main(void) {
	INIT_TIMER0(); //needed for color.h
	INIT_TIMER2(); //needed for color.h
	int mode = 1;

	tempInit(); //temp initializer
	uartInit(); //uart initializer
	sonarInit(); // sonar initializer
	//nothing seems to be allowed to happen here after initializing
	//if the rest of main starts to do something let me know
	//if you just need to test transmission, commenting out the UART2 interrupt enable in initialize() should do the trixk
	while(1) {
		//	transmit(0xAA);
		//	printf("Im not crazy\n");
		//printf("%d\n", receive());
		//wait(5000);

		while (mode & 1) { // color sensor mode
			//(int)collor=color();
			//transmit(color());
			color();
			//if (receive() == 154) {
			//	mode = 2;
			//}
			wait(10000);
		}
		while (mode & 2) { // sonar range mode
			printf("%d inches\n", sonarTrigger());
			//if (receive() == 182) {
			//	mode = 1;
			//}
			wait(10000);
		}
		while (mode & 3) { // temperature sensor mode
			printf("%d degrees celsius\n", getTemp());
			wait(10000);
		}
		wait(50000);

	}
    return 0 ;

}


//-------------------personal notes-------------------------------

//yeah so the start condition is screwing up the LSB. So fuck the LSB
//we don't need the LSB
//We never needed the LSB
//We got so little data
//LSB's can go fuck right off for all I care
//fuck LSB
//new plan: only even bytes, all bit0=0 cause fuck fixing this nonsense

//changing the number of start bits just screwed it up more
//some success with double transmitting with 4 start bits
//fixed the truncating, but took too long causes overflow errors
//overflow errors worse than truncation
//bad bad not good
//stick to original plan, just fuck the LSB

//praise case 3

//there's probably a better way of handling the keypad
//maybe Braxton will have some ideas
//will come back to later

//code is using super low baud atm, character time outs are occuring because of this, but hey it still works
//will adjust baud higher later

//for whatever reason, beyond all my comprehension.
//including the temperature header NOT ONLY made case 2 work like god intended
//BUT ALSO allowed the program to exit the interrupt hell and execute the rest of the code in main()
//it defies all explanation, but it works now, so just accept it
//I'll need to move the cases to case 2 now
//this marks the defeat of the all sacred case 3, and the rise of a golden era.
//also of note for some reason we're getting thre interrupts now after entering main
//no idea what's causing that, but I believe, if needed, they can be turned off
