#ifndef __STRFMT_H__
#define __STRFMT_H__

/**
 * Number-to-string conversions as a standalone set of modules, so that they can
 * be unit-tested independently from any STM32 platform.
 */

#include <stdint.h>
#include <string.h>


// -- External user functions -- //

#ifdef __cplusplus
extern "C"
    {
#endif


// -- Exported functions -- //

char* printu16(char* buf, uint16_t n);
char* printi16(char* buf, int16_t n);
int sprintu16(char* buf, uint16_t n);
int sprinti16(char* buf, int16_t n);

char* printu32(char* buf, uint32_t n);
char* printi32(char* buf, int32_t n);
int sprinti32(char* buf, int32_t n);
int sprintu32(char* buf, uint32_t n);

char* printu64(char* buf, uint64_t n);
char* printi64(char* buf, int64_t n);
int sprinti64(char* buf, int64_t n);
int sprintu64(char* buf, uint64_t n);

int float_to_str(char* buf, float n);
int fp32_to_str(char* buf, float x);
int double_to_str(char* buf, double n);


// -- Helper-functions -- //

int strapp(char* dst, char* src);

int frac_to_str(char* buf, uint64_t frac, int limit);
uint64_t bankers64(uint64_t frac, int* limit);


#ifdef __cplusplus
    }
#endif


#endif /* __STRFMT_H__ */
