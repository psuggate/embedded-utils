#include <stdio.h>
#include <stdlib.h>
#include "stm32crc.h"


void stm32crc_tb(void)
{
    unsigned char p[1024];
    int32_t* q = (int32_t*)p;

    printf("WIP\n");

    for (int i=256; i--;) {
	q[i] = rand();
    }

    printf("WOP: %08x\n", stm32crc_calc(p, 1024));

    char r[] = "salad burger potassiumate\n";
    size_t len = sizeof(r) - 1;
    printf("WUP: %08x (len = %lu)\n", stm32crc_calc((uint8_t*)r, len), len);
}
