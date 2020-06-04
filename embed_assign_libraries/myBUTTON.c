#include <config/AT91SAM7S256.h>
#include "myBUTTON.h"

#define SWITCH_SELECT 0x02000000// Select line (Output)
#define BUTTON4_8 0x04000000	// Button 4 or 8 (Input)
#define BUTTON3_7 0x08000000	// Button 3 or 7 (Input)
#define BUTTON2_6 0x10000000	// Button 2 or 6 (Input)
#define BUTTON1_5 0x20000000	// Button 1 or 5 (Input)

//Variable to hold the button to be compared.
unsigned long ButtonValues;

//Checks if button "bNum" is pressed or not. 
int isPressed(int bNum){
		bNum=bNum-1;
		
		ButtonValues = 0x20000000;
		
	if ((bNum >= 0) && (bNum <=7))
	{
		if (bNum <= 4)
		{
		AT91C_BASE_PIOA->PIO_SODR = SWITCH_SELECT;//If button number is smaller than 4, set s.select to 1.
		
		ButtonValues = ButtonValues >> bNum;
		if((AT91C_BASE_PIOA->PIO_PDSR & ButtonValues) == 0){//"And" the desired button with status register.

					return 1;

				}else{
					return 0;
				}
		}
		else
		{
		AT91C_BASE_PIOA->PIO_CODR = SWITCH_SELECT;//If button number is bigger than 4, set s.select to 0.
		ButtonValues = ButtonValues >> (bNum - 4);
		if((AT91C_BASE_PIOA->PIO_PDSR & ButtonValues) == 0){//"And" the desired button with status register.

					return 1;

				}else{
					return 0;
				}
		}


	}
	
	return 0;
	
}
int isReleased(int bNum){
		bNum=bNum-1;

		ButtonValues = 0x20000000;
		
	if ((bNum >= 0) && (bNum <=7))
	{
		if (bNum <= 4)
		{
		AT91C_BASE_PIOA->PIO_SODR = SWITCH_SELECT;//If button number is smaller than 4, set s.select to 1.
		
		ButtonValues = ButtonValues >> bNum;
		if((AT91C_BASE_PIOA->PIO_PDSR & ButtonValues) == ButtonValues){//"And" the desired button with status register.

					return 1;

				}else{

					return 0;
				}
		}
		else
		{
		AT91C_BASE_PIOA->PIO_CODR = SWITCH_SELECT;//If button number is bigger than 4, set s.select to 0.
		ButtonValues = ButtonValues >> (bNum - 4);
		if((AT91C_BASE_PIOA->PIO_PDSR & ButtonValues) == ButtonValues){//"And" the desired button with status register.

					return 1;

				}else{

					return 0;
				}
		}


	}
	
	return 0;
	
}

//Procedure to check if any of the buttons is pressed.
int isAnyPressed(void){
	
	AT91C_BASE_PIOA->PIO_SODR = SWITCH_SELECT;
	if((AT91C_BASE_PIOA->PIO_PDSR & 0x3C000000)== 0x3C000000){//Check if any button in the left bank is pressed.
		
		AT91C_BASE_PIOA->PIO_CODR = SWITCH_SELECT;
		
		if((AT91C_BASE_PIOA->PIO_PDSR & 0x3C000000) == 0x3C000000){//Check if any button in the right bank is pressed.
		
			return 0;
		
		}
		
	}
	
	return 1;
	
	
}
