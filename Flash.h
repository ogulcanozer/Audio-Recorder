/*
 * Flash.h
 *
 *  Created on: 24 Sep 2013
 *      Author: martin
 */

/*
 * This header file provides some basic macros that can be used to program the flash memory.
 */

#ifndef FLASH_H_
#define FLASH_H_

#include <config/AT91SAM7S256.h>

/*
 * Configure the flash memory access speed to allow set/reset operations on the security bits.
 */
#define CONFIGURE_FLASH_SECURITY AT91C_BASE_MC->MC_FMR = (48 << 16)

/*
 * Configure the flash memory access speed to allow normal read/write operations.
 */
#define CONFIGURE_FLASH_ACCESS AT91C_BASE_MC->MC_FMR = ((72 << 16) | 0x100)

/*
 * Erase the entire flash memory. ALL flash pages must be unlocked for this operation to succeed.
 */
#define ERASE_ALL AT91C_BASE_MC->MC_FCR = ((0x5a << 24) | AT91C_MC_FCMD_ERASE_ALL)

/*
 * Program (flash) a single page of flash memory. The parameter x specifies the page number
 */
#define FLASH_PAGE(x) AT91C_BASE_MC->MC_FCR = ((0x5a << 24) | ((x) << 8) | AT91C_MC_FCMD_START_PROG)

/*
 * Return TRUE if the last flash operation has not completed.
 */
#define FLASH_NOT_READY ((AT91C_BASE_MC->MC_FSR & AT91C_MC_FRDY) != AT91C_MC_FRDY)

/*
 * Return TRUE if the last flash operation has completed.
 */
#define FLASH_IS_READY ((AT91C_BASE_MC->MC_FSR & AT91C_MC_FRDY) == AT91C_MC_FRDY)

/*
 * Translate the security lock number to the required bit position.
 */
#define SECURITY_LOCK_NUMBER(x) ((x) << 6)

/*
 * Unlock the specified security (write protect) bit.
 *
 * The flash device must be configured for security bit operations.
 */
#define UNLOCK_SECURITY_BIT(x) AT91C_BASE_MC->MC_FCR = ((0x5a << 24) | (SECURITY_LOCK_NUMBER(x) << 8) | AT91C_MC_FCMD_UNLOCK)


/*
 * Unlock all security (write protect) bits.
 */
#define UNLOCK_ALL_SECURITY_BITS CONFIGURE_FLASH_SECURITY;\
						 UNLOCK_SECURITY_BIT(0); UNLOCK_SECURITY_BIT(1); UNLOCK_SECURITY_BIT(2); UNLOCK_SECURITY_BIT(3);\
						 UNLOCK_SECURITY_BIT(4); UNLOCK_SECURITY_BIT(5); UNLOCK_SECURITY_BIT(6); UNLOCK_SECURITY_BIT(7);\
						 UNLOCK_SECURITY_BIT(8); UNLOCK_SECURITY_BIT(9); UNLOCK_SECURITY_BIT(10); UNLOCK_SECURITY_BIT(11);\
						 UNLOCK_SECURITY_BIT(12); UNLOCK_SECURITY_BIT(13); UNLOCK_SECURITY_BIT(14); UNLOCK_SECURITY_BIT(15);\
						 CONFIGURE_FLASH_ACCESS;

/*
 * Base address of flash memory device - Device is 256k bytes
 */
#define FLASH_BASE_ADDRESS 0x100000

#endif /* FLASH_H_ */
