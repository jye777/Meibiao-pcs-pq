#ifndef CPU_H
#define CPU_H

#include <stdint.h>

typedef  uint32_t  CPU_SR;

#define  CPU_CRITICAL_ENTER()           { cpu_sr = CPU_SR_Save(); }
#define  CPU_CRITICAL_EXIT()            { CPU_SR_Restore(cpu_sr); }

CPU_SR      CPU_SR_Save      (void);
void        CPU_SR_Restore   (CPU_SR      cpu_sr);

#endif

