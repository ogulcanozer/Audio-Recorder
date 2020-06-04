#ifndef _LEDLIB_H_
#define _LEDLIB_H_

//Checks if button "bNum" is pressed or not. 
int isPressed(int bNum);

//Checks if button "bNum" is released or not. 
int isReleased(int bNum);

//Procedure to check if any of the buttons is pressed.
int isAnyPressed(void);
 
#endif