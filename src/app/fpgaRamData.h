#ifndef __FPGARAMDATA_H__
#define __FPGARAMDATA_H__
#include "fpgaramlist.h"

#if 0
enum DATA_HANDLE_METHOD
{
    METHOD_0 = 0,
    METHOD_1,
    METHOD_2,
    METHOD_3,
    METHOD_4,
    METHOD_5,
    METHOD_6,
    METHOD_7,
	METHOD_QTY,
};


struct fpgaRamData
{
    //int16_t  frd_value;
    uint32_t frd_fpgaRamIndex;
    uint32_t frd_methodIndex;
    uint32_t frd_coefId;
};

#define MODULUS					(10)

#endif



#if 0
int16_t FpgaRamData_HardwareValue(uint16_t index, int16_t value);
int16_t FpgaRamData_SoftwareValue(uint16_t index);
int16_t FpgaRamData_Store(uint16_t index, int16_t value);
#endif
int FpgaRamData_Write(uint32_t addr, uint16_t data);

uint16_t FpgaRamData_Read(uint32_t a);


#endif
