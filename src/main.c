/*
 * Copyright (c) 2021, NXP. All rights reserved.
 * Distributed under The MIT License.
 * Author: Abraham Rodriguez <abraham.rodriguez@nxp.com>
 *
 * Description:
 * Minimal implementation for the usage of the FlexNVM module
 * with Emulated EEPROM functionality.
 *
 * boot_counter is incremented in each power-up or reset and conserved
 * in non-volatile memory, the value of the counter can be viewed in
 * a debug session.
 *
 * When using PEMicro debugger, a configuration is required:
 * Debug configurations -> debugger tab  and check
 * "emergency kinetis device recovery by full chip erase "
 *
 * Also, in Advanced options inside the debugger tab, check
 * "Preserve partitioning for the device" and in
 * "Non-volatile memory preservation" fill with the range of
 *  10000000 - 1000ffff
 *
 *  An screen capture of the configuration window is provided with the project.
 *
 */

#include "S32K142.h"
#include "s32_core_cm4.h"

/* Power-up counter stored in the emulated EEPROM, section declared in linker script */
__attribute__ ((section(".eeeprom"))) uint32_t boot_counter;

START_FUNCTION_DECLARATION_RAMSECTION
void flash_command(void)
END_FUNCTION_DECLARATION_RAMSECTION

void flash_command(void)
{
    DISABLE_INTERRUPTS()

    /* Clear CCIF flag (W1C) to launch the command */
    FTFC->FSTAT |= FTFC_FSTAT_CCIF_MASK;

    /* Wait command to complete */
    while(!(FTFC->FSTAT & FTFC_FSTAT_CCIF_MASK));

    ENABLE_INTERRUPTS()
}

/* Write to FlexNVM  */
void EEE_write(uint32_t value)
{
    /* Write must be performed atomically */
    /* Writes are performed in RUN mode, read can be done in other clock modes */
    DISABLE_INTERRUPTS()

    /* Verify that EEPROM is available for writing */
    while(!(FTFC->FSTAT & FTFC_FSTAT_CCIF_MASK));

    boot_counter = value;

    /* Wait for write to complete */
    while(!(FTFC->FSTAT & FTFC_FSTAT_CCIF_MASK));

    ENABLE_INTERRUPTS()
}

/* Read from FlexNVM */
uint32_t EEE_read(void)
{
    DISABLE_INTERRUPTS()

    return boot_counter;

    ENABLE_INTERRUPTS()
}


int main(void)
{

/* Perform partition only if one hasn't be done yet */
if(((SIM->FCFG1 & SIM_FCFG1_DEPART_MASK) >> SIM_FCFG1_DEPART_SHIFT) == 0xF)
{

    /* Perform partition, 4kb flexram and 64k eeprom backup */
    FTFC->FCCOB[3] = 0x80; /* PGMRT command */
    FTFC->FCCOB[2] = 0x00; /* No CSEc operation */
    FTFC->FCCOB[1] = 0x00;
    FTFC->FCCOB[7] = 0x02; /* EEPROM data set size code 4Kb */
    FTFC->FCCOB[6] = 0x04; /* FlexNVM, EEPROM 64kb */

    flash_command();

    /* boot count initialization */
    /* Configure FlexRAM for emulated EEPROM usage */
    FTFC->FCCOB[3] = 0x81; /* SetRAM command */
    FTFC->FCCOB[2] = 0x00; /* Emulated EEPROM mode */

    flash_command();

    /* Init boot counter to 0 */
    EEE_write(0);

}

else
{
    /* Configure FlexRAM for emulated EEPROM usage */
    FTFC->FCCOB[3] = 0x81; /* SetRAM command */
    FTFC->FCCOB[2] = 0x00; /* Emulated EEPROM mode */

    flash_command();

    /* Wait for EEPROM to be ready */
    while(!(FTFC->FCNFG & FTFC_FCNFG_EEERDY_MASK));
}
    /* Read the counter from EEEPROM, and save back incremented by 1 */
    uint32_t current_count = EEE_read();

    current_count++;

    EEE_write(current_count);


    /* Infinite for loop */
    for(;;)
    {

    }

    return 0;

}
