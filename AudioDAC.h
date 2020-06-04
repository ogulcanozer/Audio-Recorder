/*
 * Header file describing Audio codec register layout and commands
 */

#ifndef _DAC_H_
#define _DAC_H_

#define AIC_REG_LINVOL		0x00
#define AIC_REG_RINVOL		0x01
#define AIC_REG_LOUTVOL		0x02
#define AIC_REG_ROUTVOL		0x03
#define AIC_REG_AN_PATH		0x04
#define AIC_REG_DIG_PATH	0x05
#define AIC_REG_POWER		0x06
#define AIC_REG_DIG_FORMAT	0x07
#define AIC_REG_SRATE		0x08
#define AIC_REG_DIG_ACT		0x09
#define AIC_REG_RESET		0x0F

#define AIC_USB 			(1 << 0)
#define AIC_BOSR 			(1 << 1)
#define AIC_SR0 			(1 << 2)
#define AIC_SR1 			(1 << 3)
#define AIC_SR2 			(1 << 4)
#define AIC_SR3 			(1 << 5)

#define AIC_MICB			(1 << 0)
#define AIC_MICM			(1 << 1)
#define AIC_INSEL			(1 << 2)
#define AIC_BYP				(1 << 3)
#define AIC_DAC				(1 << 4)
#define AIC_STE				(1 << 5)
#define AIC_STA0			(1 << 6)
#define AIC_STA1			(1 << 7)
#define AIC_STA2			(1 << 8)

#endif /* _DAC_H_ */
