#include <stdio.h>
#include <stdlib.h>
#include <config/AT91SAM7S256.h>
#include "myLED.h"
#include "myBUTTON.h"


#define LED_DATA 0x02000000
#define LED_CLOCK 0x01000000

//Array to hold the current values of the LEDs on the board
int LEDS[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};


//Clear the LEDS array and refresh all the leds.
void clearLEDS(){
	
	AT91C_BASE_PIOA->PIO_CODR = LED_CLOCK;
	
	for (int i = 0; i < 16; i++)
	{
		LEDS[i]=0;
	}
	refresh();
	
}

//Turn a specific led on with a specific color. ledNum is the desired led, mode is the desired color.
//ledNum = int(1-8)
//mode = (0-2) , 0 = green, 1 = red, 2 = both green and red.
void ledON(int ledNum, int mode){
	
	ledNum = 16-(ledNum*2);
	
	if(mode==0){//Turn green on.
		
		LEDS[ledNum]=1;
		
	}else if(mode==1){//Turn red on.
		
		LEDS[ledNum+1]=1;
		
	}else{//Turn both green and red on.
		
		LEDS[ledNum]=1;
		LEDS[ledNum+1]=1;
		
	}
	refresh();

}

//Turn a specific led off with a specific color. ledNum is the desired led, mode is the desired color.
//ledNum = int(1-8)
//mode = (0-2) , 0 = green, 1 = red, 2 = both green and red.
void ledOFF(int ledNum, int mode){
	
	ledNum = 16-(ledNum*2);
	
	if(mode==0){//Turn green off.
		
		LEDS[ledNum]=0;
		
	}else if(mode==1){//Turn red off.
		
		LEDS[ledNum+1]=0;
		
	}else{//Turn both green and red off.
		
		LEDS[ledNum]=0;
		LEDS[ledNum+1]=0;
		
	}
	refresh();

}

//Refresh the status of the LEDs on the board, according to the values in the LEDS array.
void refresh(void){

	AT91C_BASE_PIOA->PIO_CODR = LED_CLOCK;
	for (int i = 0; i < 16; i++)
	{

		if(LEDS[i]==1)
		{

			AT91C_BASE_PIOA->PIO_CODR = LED_DATA;

			
		}else if(LEDS[i]==0)
		{

			AT91C_BASE_PIOA->PIO_SODR = LED_DATA;

		}

		AT91C_BASE_PIOA->PIO_SODR = LED_CLOCK;
		AT91C_BASE_PIOA->PIO_CODR = LED_CLOCK;
	}
	return;
}

//Turn all the LEDs on , with desired color.
//mode = (0-2) , 0 = green, 1 = red, 2 = both green and red.
void turnAllON(int mode){
	
	if(mode==0){//Turn green leds on.
	
		for (int i = 0; i < 16; i++){
			
		if(i%2==0){	
		
		LEDS[i]=1;
		
		}
		else{
		
		LEDS[i]=0;
			
		}
		}
		
	}else if(mode==1){//Turn red leds on.
		
		for (int i = 0; i < 16; i++){
			
		if(i%2==1){	
		
		LEDS[i]=1;
		
		}
		else{
		
		LEDS[i]=0;
			
		}
		}
		
	}else{//Turn both green and red leds on.
		
		for (int i = 0; i < 16; i++){
		
		LEDS[i]=1;
	
		}
		
	}
	refresh();
	

}

//Turn all the leds on (red colored), and turn them off one by one with 1000ms delay.
void countBack(){
	
	turnAllON(1);
	
	for (int i = 8; i > 0; i--){
	delay_ms(1000);
	do{}while(isAnyPressed());
	ledOFF(i,2);
	
	}
	
}

//Make delay with the desired delay value in ms.
void delay_ms(short ms){
	volatile short loop;

	while (ms-- > 0)
		for (loop = 0; loop < 2100; loop++);
}

//Turn a random LED on(green colored), and return the number.
int randomON(){
	
	int ledNum = (rand()%8)+1 ;
	ledON(ledNum,0);
	
	return ledNum;
}

//Flash led 1 one time.
void ledFlash(){

	ledON(1,1);
	delay_ms(1000);
	ledOFF(1,1);
	delay_ms(1000);
	ledON(1,1);

}
