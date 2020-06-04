/*_______________________________________________________________________________
# CE865 Programming Embedded Systems Assignment 2 | Ogulcan Ozer. | 19 Dec 2018
_______________________________________________________________________________*/
/*-------------------------------------------------------------------------------
#	CE865 Sound recorder with playback and playback with echo functionality,
	using flash memory.

	Usage:
	After loading the program, button 1 lights up. In step one, press button 1 to
	start	recording audio from the line input. After the recording is done, press
	button 2 for playback, button 3 for playback with echo, and button 1 for
	going back to the step 1.

-------------------------------------------------------------------------------*/
#include <config/AT91SAM7S256.h>
#include <board/I2C-lib.h>
#include <AudioDAC.h> // Audio DAC library.
#include <Flash.h> // Flash macros.
#include <atmel/aic.h>
#include <embed_assign_libraries/myLED.h>// LED library from assignment 1.
#include <embed_assign_libraries/myBUTTON.h>// Button library from assignment 1.
/*-------------------------------------------------------------------------------
#	Variables and definitions
-------------------------------------------------------------------------------*/
#define AUDIO_DAC_ADDR	0x1A	//I2C address of Audio converter
#define SIZE 1024	// Page Size.
unsigned int *fptr; // Variable for holding flash pointer.
unsigned int *fptr_e; // Second flash pointer for playback with echo.
unsigned int fpg = 1; // Current flash page.
unsigned long buffer[64]; // SRAM buffer.
/*-------------------------------------------------------------------------------
#	Function definitions
-------------------------------------------------------------------------------*/
void WriteAudioDACreg(unsigned char reg, unsigned short value);
void record(void);
void play(void);
void playWithEcho(void);
void SSC_initialise(void);
/*-------------------------------------------------------------------------------
#		Main Program
-------------------------------------------------------------------------------*/
int main()
{

	UNLOCK_ALL_SECURITY_BITS; //Unlock security bits of the sectors - to be programmed
	while (FLASH_NOT_READY); //Wait for Flash.
	ERASE_ALL; // Erase the memory plane.
	while (FLASH_NOT_READY);
	fptr = FLASH_BASE_ADDRESS;
	SetI2CBusSpeed(16000);
	SSC_initialise();
	WriteAudioDACreg(AIC_REG_RESET, 0x00);
	WriteAudioDACreg(AIC_REG_POWER, 1 << 1); //Mic. powered down.
	WriteAudioDACreg(AIC_REG_LINVOL, 0x117);
	WriteAudioDACreg(AIC_REG_RINVOL, 0x117);
	WriteAudioDACreg(AIC_REG_SRATE, AIC_SR1 | AIC_SR0 | AIC_USB); // 8KHz sampling rate.
	WriteAudioDACreg(AIC_REG_AN_PATH, AIC_DAC | AIC_MICM); // Enable DAC and set input to line.
	WriteAudioDACreg(AIC_REG_DIG_PATH, 0 << 3); // Disable soft mute.
	WriteAudioDACreg(AIC_REG_DIG_FORMAT, (1 << 6) | 2); // Master, I2S left aligned.
	WriteAudioDACreg(AIC_REG_DIG_ACT, 1 << 0); // Activate digital interface.

	// Main Loop
	while (1) {
		clearLEDS();
		ledON(1,1);//Turn Led 1 on.
		while (!isReleased(1));//
		while (!isPressed(1));// Wait for button press.

		record(); //Start recording.

		turnAllON(0); //Turn all the leds on (green colored) to indicate the end of recording.
		for (int loop = 0; loop <2000000 ; loop++); //Delay, so the green leds can be seen.
		clearLEDS();

		ledON(1,1);// Led 1 red
		ledON(2,0);// Led 2 green
		ledON(3,0);// Led 3 green

		while (1) {
			while (!isReleased(1));
			if(isPressed(2)){

				play(); //If button 2 is pressed, do playback.

			}
			if(isPressed(3)){

				playWithEcho(); //If button 3 is pressed, do playback with echo.

			}
			if(isPressed(1)){

				ERASE_ALL; //If button 1 is pressed, erase flash,
				while (FLASH_NOT_READY);
				break; // go back to recording.
			}

		}

	}//End of Main Loop.

	return 0;
}
//End of Main Program.

/*-------------------------------------------------------------------------------
#		Functions
-------------------------------------------------------------------------------*/

//Function to communicate with the DAC, taken from lab9 example.
void WriteAudioDACreg(unsigned char reg, unsigned short value)
{
	unsigned char message[2];

	//Create 2 byte message,
	message[0] = (reg << 1) | ((value >> 8) & 0x01);
	message[1] = value & 0xFF;

	//and send it to the DAC.
	WriteI2CDataList(AUDIO_DAC_ADDR, 0, 0, message, 2);
}

//Function to play the recorded audio.(Based on outputUsingPollLoop in lab 9 example)
void play(void)
{
	int i=0;
		fptr=FLASH_BASE_ADDRESS+64; //Set the flash pointer to the beginning of the first page.
		int tmp;
		short first,second;
		while (i<SIZE*64) // Do for each sample(s) in 1024 pages.
		{

			tmp=*fptr;//Get the sample pair.

			//Separate the samples.
			first=(short)((tmp & 0xFFFF0000) >> 16);
			*AT91C_SSC_THR = first; //Send the first sample, using transmit hold register.
			while (!(*AT91C_SSC_SR & AT91C_SSC_TXRDY));//Wait for transmit register.
			second=(short)(tmp & 0xFFFF);
			*AT91C_SSC_THR = second;//Send the second sample, using transmit hold register.
			while (!(*AT91C_SSC_SR & AT91C_SSC_TXRDY));

			fptr++;//Move to the next pair.
			i++;

		}
}



//Function to play the recorded audio with echo.
void playWithEcho(void)
{

	int i=0;
	fptr=FLASH_BASE_ADDRESS+64;//Set the flash pointer to the beginning of the first page.
	fptr_e=(FLASH_BASE_ADDRESS+(64*126));//Set the echo flash pointer to the 125th page (approx. 1 second).
	int tmp,tmp_e;
	short first,second,first_e,second_e;

	while (i<((SIZE*64)+(125*64)))
	{
		tmp=*fptr;//Original pair.
		tmp_e=*fptr_e;//Echo pair.

		//If at the intersection, play both.
		if(i>125*64 && i<SIZE*64){

			first_e=(short)((tmp_e & 0xFFFF0000) >> 16);
			first=(short)((tmp & 0xFFFF0000) >> 16);
			*AT91C_SSC_THR = first+first_e;
			while (!(*AT91C_SSC_SR & AT91C_SSC_TXRDY));//Wait for transmit register.
			second_e=(short)(tmp_e & 0xFFFF);
			second=(short)(tmp & 0xFFFF);
			*AT91C_SSC_THR = second+second_e;
			while (!(*AT91C_SSC_SR & AT91C_SSC_TXRDY));

			fptr++;
			fptr_e++;

		//If at the beginning, play only original.
		}else if(i<125*64){

			first=(short)((tmp & 0xFFFF0000) >> 16);
			*AT91C_SSC_THR = first;
			while (!(*AT91C_SSC_SR & AT91C_SSC_TXRDY));
			second=(short)(tmp & 0xFFFF);
			*AT91C_SSC_THR = second;
			while (!(*AT91C_SSC_SR & AT91C_SSC_TXRDY));

			fptr++;

		//If at the end, play only echo.
		}else{

			first_e=(short)((tmp_e & 0xFFFF0000) >> 16);
			*AT91C_SSC_THR = first_e;
			while (!(*AT91C_SSC_SR & AT91C_SSC_TXRDY));
			second_e=(short)(tmp_e & 0xFFFF);
			*AT91C_SSC_THR = second_e;
			while (!(*AT91C_SSC_SR & AT91C_SSC_TXRDY));

			fptr_e++;
		}


		i++;


	}
}


//Function to record audio. Same approach as play(), but using the receive hold register.
void record(void)
{
	int i=0;
	int j=0;
	int p=0;
	short first,second;

	while (j<SIZE) //Do until all the pages are full.
	{
		while(i<64){// each page holds 4bytes(word)*16 words

			//Get two samples(short), make a pair(long) to hold in 1 flash memory cell.
			if ((*AT91C_SSC_SR & AT91C_SSC_RXRDY)) {
				first=*AT91C_SSC_RHR;
				while (!(*AT91C_SSC_SR & AT91C_SSC_RXRDY));//Wait for receive register.
				second=*AT91C_SSC_RHR;
				buffer[i]=((first & 0x0000FFFF) << 16 ) | (second & 0x0000FFFF);//Add it to the buffer.
			i++;
			}
		}
		i=0;

		fptr=FLASH_BASE_ADDRESS;//Set the flash pointer to the flash buffer.
		for(int j=0;j<64;j++){
			*fptr=buffer[j]; //Fill the buffer.
			fptr++;
		}
		while(FLASH_NOT_READY); //If ready,
		fptr=FLASH_BASE_ADDRESS;
		FLASH_PAGE(fpg); // flash the page indicated by 'fpg'
		fpg++;
		j++;

		//Increment the LED bar according to the last page flashed.
		p=j/(SIZE/8);
		if(j%(SIZE/8)==0)
		{
			ledON(p+1,1);
		}

	}


return;


}

//Function to create I2S protocol using Synchronous Serial Controller.(taken from lab9 example.)
void SSC_initialise(void)
{
	AT91PS_PMC pPMC = AT91C_BASE_PMC;

	/*
	 * Initialise the codec clock to 12MHz
	 *
	 * So we want,
	 *    select source (MAIN CLK = external clock)
	 *    select prescaler (1)
	 */

	pPMC->PMC_PCKR[2] = (AT91C_PMC_PRES_CLK_8 | AT91C_PMC_CSS_PLL_CLK);

	/*
	 * Now enable PCLK2 and wait for it to start
	 */

	*AT91C_PMC_SCER = AT91C_PMC_PCK2;
	while( !(*AT91C_PMC_SR & AT91C_PMC_PCK2RDY) );

	/*
	 * Next make the output available on the PIO
	 */

	*AT91C_PIOA_BSR = AT91C_PA31_PCK2;
	*AT91C_PIOA_PDR = AT91C_PA31_PCK2;

	/*
	 * Finally we setup the I2S interface
	 */

	*AT91C_PMC_PCER = (1 << AT91C_ID_SSC); // Enable Clock for SSC controller
	*AT91C_SSC_CR = AT91C_SSC_SWRST; // reset
	*AT91C_SSC_CMR = 0; // no divider

	/*
	 * Configure Transmit side
	 */
	*AT91C_PIOA_PDR = AT91C_PA16_TK | AT91C_PA15_TF | AT91C_PA17_TD | AT91C_PA18_RD; // enable pins

	*AT91C_SSC_TCMR = AT91C_SSC_CKS_RK |		// external clock on TK pin (macro name is misleading)
	                  AT91C_SSC_START_EDGE_RF |	// any edge
	                  (0 << 16);				// STTDLY = 0!
	*AT91C_SSC_TFMR = (15) |					// 16 bit word length
	                  (0 << 8) |				// DATNB = 0 => 1 words per frame
	                  AT91C_SSC_MSBF;			// MSB first
	*AT91C_SSC_CR = AT91C_SSC_TXEN; // enable TX

	/*
	 * Configure Receive side
	 */
	*AT91C_SSC_RCMR = AT91C_SSC_CKS_TK |		// Use same clock as TX
	                  AT91C_SSC_START_TX |
	                  AT91C_SSC_CKI |			// sample on rising clock edge
	                  (1 << 16);				// STTDLY = 0
	*AT91C_SSC_RFMR = (15) |					// 16 bit word length
	                  (0 << 8) |				// DATNB = 0 => 1 words per frame
	                  AT91C_SSC_MSBF;			// MSB first
	*AT91C_SSC_CR = AT91C_SSC_RXEN;				// enable RX
}
/*-------------------------------------------------------------------------------
#		End of Program.
-------------------------------------------------------------------------------*/
/*_______________________________________________________________________________
# Acknowledgements:

SSC_initialise() Function, taken from module`s Lab 9 example code.
WriteAudioDACreg() Function, taken from module`s Lab 9 example code.

_______________________________________________________________________________*/
