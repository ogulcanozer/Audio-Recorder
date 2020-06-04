#ifndef _LEDLIB_H_
#define _LEDLIB_H_


//Flash led 1 one time.
void ledFlash();

//Clear the LEDS array and refresh all the leds.
void clearLEDS();

//Turn a specific led on with a specific color. ledNum is the desired led, mode is the desired color.
//ledNum = int(1-8)
//mode = (0-2) , 0 = green, 1 = red, 2 = both green and red.
void ledON(int ledNum, int mode);

//Turn a specific led off with a specific color. ledNum is the desired led, mode is the desired color.
//ledNum = int(1-8)
//mode = (0-2) , 0 = green, 1 = red, 2 = both green and red.
void ledOFF(int ledNum, int mode);

//Refresh the status of the LEDs on the board, according to the values in the LEDS array.
void refresh(void);

//Turn all the LEDs on , with desired color.
//mode = (0-2) , 0 = green, 1 = red, 2 = both green and red.
void turnAllON(int mode);

//Turn all the leds on (red colored), and turn them off one by one with 1000ms delay.
void countBack();

//Make delay with the desired delay value in ms.
void delay_ms(short ms);

//Turn a random LED on(green colored), and return the number.
int randomON();

#endif
