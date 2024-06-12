#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "stm32crc.h"


void stm32crc_tb(void)
{
    unsigned char p[1024];
    int32_t* q = (int32_t*)p;

    printf("\nTesting CRC32 correctness (vs STM32G4xx default)\n");

    for (int i=256; i--;) {
	q[i] = rand();
    }
    assert(0 != stm32crc_calc(p, 1024));

    char str[] = "A test-string for CRC32 checking\n";
    size_t len = sizeof(str) - 1; // drop the '\0'
    uint32_t crc = stm32crc_calc((uint8_t*)str, len);
    assert(crc == 0xf1c14ad9L);

    printf("passed\n");
}
