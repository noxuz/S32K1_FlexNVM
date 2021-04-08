/*
 * EEEPROM_io_control.h
 *
 *  Created on: Jan 22, 2018
 *      Author: nxf24838
 */

#ifndef EEEPROM_IO_CONTROL_H_
#define EEEPROM_IO_CONTROL_H_

#define EEE_SUCCESS	0
#define EEE_ALREADY_ENABLED -1

int EEE_Init();
int EEE_Write_ByteArray(char* source,char* target,unsigned int size);
int EEE_Write_int32(int data, int* target);
int EEE_Write_float(float data, float* target);
int EEE_Write_double(double data, double* target);



#endif /* EEEPROM_IO_CONTROL_H_ */
