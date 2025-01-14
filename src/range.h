/*
 * range.h
 *
 *  Created on: May 6, 2021
 *      Author: haye9466
 */

#ifndef RANGE_H_
#define RANGE_H_

// sonar functions
void wait_ticks(int ticks) {
	volatile int count;
	for (count=0; count<ticks; count++) {
		//do nothing
	}
}
_Bool getPinState(int pin) {
	return (FIO2PIN >> pin) & 1;
}
int sonarTrigger() {
	printf("trigger\n");
	FIO2PIN |= (1 << 0);  // pin 0.0, set high
	wait_ticks(400);
    FIO2PIN &= ~(1 << 0);

    int j = 0;
    int i = 1000;
    while (i > 0) {
    	if (getPinState(1)) {
    		j++;
    	}
    	i--;
    }
    return j/10.875;
	/*
	 * 10 microsecond trigger
	 * wait for rising edge on echo
	 * count pulse width
	 * convert to distance
	 */
}
void sonarInit() {
	FIO2DIR |= (1 << 0);  // pin 0.10, define write
	FIO2DIR &= ~(1 << 1); // pin 0.11, define read
	PINMODE4 |= (1<<2) | (1<<3);  //p 0.11 pull-down

	wait_ticks(100);
}
// end sonar functions


#endif /* RANGE_H_ */
