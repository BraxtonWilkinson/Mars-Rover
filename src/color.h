/*
 * color.h
 *
 *  Created on: May 7, 2021
 *      Author: haye9466
 */

#ifndef COLOR_H_
#define COLOR_H_


struct Timer0 {
        volatile unsigned int IR;
        volatile unsigned int TCR;
        volatile unsigned int TC;
        volatile unsigned int PR;
        volatile unsigned int PC;
        volatile unsigned int MCR;
        volatile unsigned int MR0;
        volatile unsigned int MR1;
        volatile unsigned int MR2;
        volatile unsigned int MR3;
        volatile unsigned int CCR;
        volatile unsigned int CR0;
        volatile unsigned int CR1;
        volatile unsigned int filler2[2];
        volatile unsigned int EMR;
        volatile unsigned int filler[12];
        volatile unsigned int CTCR;
};
struct Timer2 {
        volatile unsigned int IR;
        volatile unsigned int TCR;
        volatile unsigned int TC;
        volatile unsigned int PR;
        volatile unsigned int PC;
        volatile unsigned int MCR;
        volatile unsigned int MR0;
        volatile unsigned int MR1;
        volatile unsigned int MR2;
        volatile unsigned int MR3;
        volatile unsigned int CCR;
        volatile unsigned int CR0;
        volatile unsigned int CR1;
        volatile unsigned int filler2[2];
        volatile unsigned int EMR;
        volatile unsigned int filler[12];
        volatile unsigned int CTCR;
};


#define timer0   ((volatile struct Timer0 *)0x40004000)
#define timer2   ((volatile struct Timer2 *)0x40090000)
#define PINSEL0  (*(volatile unsigned int*)0x4002C000)
#define PINSEL4  (*(volatile unsigned int*)0x4002C010)

#define FIO0DIR  (*(volatile unsigned int*)0x2009C000)
#define FIO2DIR  (*(volatile unsigned int*)0x2009C040)

#define PINMODE0 (*(volatile unsigned int*)0x4002C040)
#define PINMODE4 (*(volatile unsigned int*)0x4002C050)

#define PCLKSEL0 (*(volatile unsigned int*)0x400FC1A8)
#define PCLKSEL1 (*(volatile unsigned int*)0x400FC1AC)
#define FIO0PIN  (*(volatile unsigned int*)0x2009C014)
#define FIO2PIN  (*(volatile unsigned int*)0x2009C054)

#define PCONP    (*(volatile unsigned int*)0x400Fc0c4)



void RESET_TC0(){
        //reset timer counter
        timer0->TCR = 0x02;
        //wait for TC to be 0
        while((timer0->TC) != 0){};
        timer0->TCR = 0x00;
        return;

}
void RESET_TC2(){
        //reset timer counter
        timer2->TCR = 0x02;
        //wait for TC to be 0
        while((timer2->TC) != 0){};
        timer2->TCR = 0x00;
        return;

}

void ENABLE_TC0(){
        //enable counter
        timer0->TCR = 0x01;
        return;
}

void DISABLE_TC0(){
        timer0->TCR = 0x00;
        return;
}

void ENABLE_TC2(){
        //enable counter
        timer2->TCR = 0x01;
        return;
}

void DISABLE_TC2(){
        timer2->TCR = 0x00;
        return;
}


void INIT_TIMER0(){
        //set power of Timer/Counter 0 on (bit 1)
        PCONP |= (1<<1);
        //select the clock frequency to be 1MHz (TIMER2)
        PCLKSEL0 &= ~(1<<3) & ~(1<<2);
        //set physical pin 38 to CAP2.0
        //PINSEL0 |= (1<<9) | (1<<8);

        //set Capture Control Register to detect rising edge
        timer0->CCR = 0x00;;
        //set the CTCR Count Control Register as Timer Mode on Rising Edges
        timer0->CTCR = 0x00;
        // set the prescale register value to 0
        timer0->PR = 0;


        return;
}

void INIT_TIMER2(){
        //set power of Timer/Counter 2 on (bit 22)
        PCONP |= (1<<22);
        //select the clock frequency to be 1MHz (TIMER2)
        PCLKSEL1 &= ~(1<<13) & ~(1<<12);
        //set physical pin 38 to CAP2.0
        PINSEL0 |= (1<<9) | (1<<8);

        //set Capture Control Register to detect rising edge
        timer2->CCR = 0x00;
        //set the CTCR Count Control Register as Counter Mode on Rising Edges
        timer2->CTCR = 0x01;
        // set the prescale register value to 0
        timer2->PR = 0;
        //reset counter
        RESET_TC2();


        return;
}
void wait_color_sensor(int wait_time){
        RESET_TC0();
        ENABLE_TC0();
        while(timer0->TC < wait_time){};
        DISABLE_TC0();
        return;
}
void COLOR_SELECT(int S2S3){

        //set color sensor selection pins as outputs
        FIO2DIR |= (1<<11) | (1<<12);
        //set color selection outputs low
        PINMODE4 |= (1<<10) | (1<<11);

        // s2 = p0.11
        // s3 = p0.12

        if(S2S3 == 0){
                //Setting pins S2 and S3 on the color sensor low looks for red
                FIO2PIN &= ~(1<<11);
                FIO2PIN &= ~(1<<12);

        } else if(S2S3 == 1){
                //Setting pins S2 and S3 on the color sensor low looks for blue
                FIO2PIN &= ~(1<<11); //s2
                FIO2PIN |= (1<<12); //s3

        } else if(S2S3 == 2){
                //Setting pins S2 and S3 on the color sensor low looks for clear
                FIO2PIN |= (1<<11); //s2
                FIO2PIN &= ~(1<<12);  //s3

        } else if(S2S3 == 3){
                //Setting pins S2 and S3 on the color sensor low looks for green
                FIO2PIN |= (1<<11); //s2
                FIO2PIN |= (1<<12); //s3

        }


        return;


}


unsigned char tens_msb = 0x30; //assing these two variables as GLOBAL variables to be used in the LCD display code
unsigned char tens_lsb; //assing these two variables as GLOBAL variables to be used in the LCD display code

unsigned char ones_msb = 0x30;
unsigned char ones_lsb;



void conversion_table(unsigned char tens_lsb, unsigned char ones_lsb, int tens, int ones) {


        switch(tens) {

                case 0 :
                        tens_lsb = 0x00;
                break;

                case 1 :
                        tens_lsb = 0x10;
                break;

                case 2 :
                        tens_lsb = 0x20;

                break;

                case 3 :
                        tens_lsb = 0x30;

                break;

                case 4 :
                        tens_lsb = 0x40;

                break;

                case 5 :
                        tens_lsb = 0x50;

                break;

                case 6 :
                        tens_lsb = 0x60;

                break;

                case 7 :
                        tens_lsb = 0x70;

                break;

                case 8 :
                        tens_lsb = 0x80;

                break;

                case 9 :
                        tens_lsb = 0x90;

                break;


        }

        switch(ones) {

                        case 0 :
                                ones_lsb = 0x00;
                        break;

                        case 1 :
                                ones_lsb = 0x10;
                        break;

                        case 2 :
                                ones_lsb = 0x20;

                        break;

                        case 3 :
                                ones_lsb = 0x30;

                        break;

                        case 4 :
                                ones_lsb = 0x40;

                        break;

                        case 5 :
                                ones_lsb = 0x50;

                        break;

                        case 6 :
                                ones_lsb = 0x60;

                        break;

                        case 7 :
                                ones_lsb = 0x70;

                        break;

                        case 8 :
                                ones_lsb = 0x80;

                        break;

                        case 9 :
                                ones_lsb = 0x90;

                        break;


                }

printf("tens_hex = %c ", tens_msb);
printf("ones_hex = %c\n", ones_msb);

        lcd_write_data_letters(ones_msb, ones_lsb); //display ones digit
        lcd_write_data_letters(tens_msb, tens_lsb); //display tens digit
        //lcd_write_data_letters(0x20, 0x00); //space
        lcd_write_instruction(0x10,0x00); //shift the cursor left
        lcd_write_instruction(0x10,0x00); //shift the cursor left


}

void tens_ones(float proportion){
        int tens;
        int ones;


        tens = proportion / 10.0; //gets the tens place of the color proportion
        ones = proportion - tens*10; //gets the ones place of the color proportion

        printf("tens = %d ", tens);
        printf("ones = %d\n", ones);

        conversion_table(tens_lsb, ones_lsb, ones, tens ); //send to the conversion table to convert to display bit pattern. passed by reference values are global located outside of every function


}


int color(void){

        float red_value;
        float blue_value;
        float clear_value;
        float green_value;

        float red_proportion;
        float green_proportion;
        float blue_proportion;
        float clear_proportion;


                for(int i = 0; i < 4; i++){
                        COLOR_SELECT(i);
                        RESET_TC2();
                        ENABLE_TC2();
                        wait_color_sensor(100000);

                        if(i == 0)
                                red_value = timer2->TC;
                        else if (i == 1)
                                blue_value = timer2->TC;
                        else if (i == 2)
                                clear_value = timer2->TC;
                        else if (i == 3)
                                green_value = timer2->TC;
                }


                red_proportion =  (red_value )  /  (red_value + green_value + blue_value);
                red_proportion *= 100;



                green_proportion =  (green_value )  /  (red_value + green_value + blue_value);
                green_proportion *= 100;

                blue_proportion =  (blue_value )  /  (red_value + green_value + blue_value);
                blue_proportion *= 100;



                clear_proportion =  (clear_value )  /  (red_value + green_value + blue_value + clear_value );
                clear_proportion *= 100;

//              tens_ones(red_proportion);




                printf("R = %f, G = %f, B = %f, C = %f\n", red_proportion, green_proportion, blue_proportion, clear_proportion);
                return red_proportion;
                //value_for_display(red_proportion);

}

#define SEC (*(volatile unsigned int*)0x40024020)
#define MIN (*(volatile unsigned int*)0x40024024)
#define CCR1 (*(volatile unsigned int*)0x40024008)




  //      CONFIGURE_MICRO();
 //       RESET_SYSTEM();
//        setup_lcd();





//      RED();
        //temp_preamble();




#endif /* COLOR_H_ */
