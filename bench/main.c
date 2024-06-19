#include "ringbuf_tb.h"
#include "strfmt_tb.h"
#include "response_tb.h"
#include "stm32crc_tb.h"
#include "fwupdate_tb.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <assert.h>
#include <string.h>


long gethex(char* line)
{
    return strtol(line, NULL, 16);
}

void gethex_tb(void)
{
    char str1[] = "  0xfab";
    char str2[] = "\t0baf\n";

    printf("\nGetHex Sanity-Checks:\n");

    assert(gethex(str1) == 0xfab);
    assert(gethex(str2) == 0xbaf);

    char done[] = "*d 70112 aa699def\n";
    char* rest = done;
    unsigned long len = strtoul(&done[2], &rest, 10);
    unsigned long crc = strtoul(rest, NULL, 16);

    assert(len == 70112);
    assert(crc == 0xaa699def);

    const char* null_str = NULL;
    char dst[32] = {0};
    char* res;
    // assert(strlen(null_str) == 0);
    // res = stpcpy(dst, null_str);
    res = memcpy(dst, null_str, 0);
    printf("strcpy(dst, NULL) = %p\n", res);
    // assert(gethex(null_str) == 0);

    printf("passed\n");
}

int main(int argc, char* argv[])
{
    fwupdate_tb();
    gethex_tb();
    stm32crc_tb();

    ringbuf_tb();
    bytebuf_tb();
    response_tb();
    strfmt_tb();

    return 0;
}
