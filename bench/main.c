#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "ringbuf.h"
#include "strfmt.h"
#include "strfmt_tb.h"

#include "response.h"
#include "response_tb.h"


RB_MAKE_PUSH(int32_t)
RB_MAKE_POP(int32_t)


void test_ringbuf() {
    void* rb_mem = malloc(16 + 32);
    ringbuf_t* rb = rb_create(rb_mem, 8);
    int ret = 0;
    int32_t val;

    rb_push(rb, 666);
    ret++;
    if (!rb_pop(rb, &val)) {
        printf("Boo: %d\n", ret);
        exit(ret);
    }

    ret++;
    if (rb_pop(rb, &val)) {
        printf("Boo: %d\n", ret);
        exit(ret);
    } else {
        printf("Push-pop yay: %d\n", val);
    }

    for (int i=0; i<8; i++) {
        rb_push(rb, 42069);
    }

    ret++;
    if (rb_push(rb, 90210)) {
        printf("Boo: %d\n", ret);
        exit(ret);
    }

    ret++;
    if (!rb_pop(rb, &val)) {
        printf("Boo: %d\n", ret);
        exit(ret);
    }

    ret++;
    if (!rb_push(rb, 90210)) {
        printf("Boo: %d\n", ret);
        exit(ret);
    }

    rb_clear(rb);
    ret++;
    if (rb_pop(rb, &val)) {
        printf("Boo: %d\n", ret);
        exit(ret);
    }

    ret++;
    if (!rb_push(rb, 90210)) {
        printf("Boo: %d\n", ret);
        exit(ret);
    }

    free(rb_mem);
}


void test_signextend() {
    uint8_t spi_bytes[16] =
        { 0x12, 0xc0, 0, 0
          , 0x03, 0xab, 0xcd
          , 0xff, 0xff, 0x80
          , 0, 0, 0, 0, 0, 0
        };

    // This is equivalent to the ARMv7 version:
    uint8_t moar[4];
    moar[0] = spi_bytes[7]; // REV
    moar[1] = spi_bytes[6];
    moar[2] = spi_bytes[5];
    moar[3] = spi_bytes[4];
    int32_t sint1 = *((int32_t*)moar) >> 8;
    moar[0] = spi_bytes[11]; // REV
    moar[1] = spi_bytes[10];
    moar[2] = spi_bytes[9];
    moar[3] = spi_bytes[8];
    int32_t sint2 = ((*((int32_t*)(spi_bytes+4)) << 24) >> 8) | (*((int32_t*)moar) >> 16);

    int8_t upper1 = (int8_t)spi_bytes[4] >> 7;
    int32_t lower1 = ((int32_t)spi_bytes[4] << 16) |
        ((int32_t)spi_bytes[5] << 8) | ((int32_t)spi_bytes[6]);
    int8_t upper2 = (int8_t)spi_bytes[7] >> 7;
    int32_t lower2 = ((int32_t)spi_bytes[7] << 16) |
        ((int32_t)spi_bytes[8] << 8) | ((int32_t)spi_bytes[9]);

    int32_t value1 = ((int32_t)upper1 << 24) | lower1;
    int32_t value2 = ((int32_t)upper2 << 24) | lower2;

    printf("Raw[1] = 0x%08x\n", value1);
    printf("Raw[2] = 0x%08x\n", value2);

    assert(sint1 == value1);
    assert(sint2 == value2);

    int32_t x = ((spi_bytes[4] << 24) + (spi_bytes[5] << 16) + (spi_bytes[6] << 8)) / 256;
    int32_t y = ((spi_bytes[7] << 24) + (spi_bytes[8] << 16) + (spi_bytes[9] << 8)) / 256;

    assert(x == value1);
    assert(y == value2);

    char buf[16] = {0};
    printf("Sample[1] = 0x%08x (%7d == %s)\n", x, x, printi32(buf, (int32_t)x));
    // printf("Sample[2] = 0x%08x (%7d == %s)\n", y, y, printi16((int16_t)y, buf));
    size_t n = sprinti16(buf, (int16_t)y);
    assert(n == strlen(buf));
    printf("Sample[2] = 0x%08x (%7d == %s, n = %lu)\n", y, y, buf, n);
}

void test_randomax()
{
    char stuffs[256] = {'a', '\0', 48};
    char pudges[256] = {'x', 'y', '\0'};
    printf("Stuffs: %s\n", strcat(stuffs, pudges));

    int fred = 2;
    if (fred -= 3 < 0) {
        printf("Pre\n");
    } else {
        printf("Post\n");
    }

    char buf[20] = {0};
    assert(sprinti32(buf, 3) == 1);

    float x = -1.7f;
    float y = -17.f;
    float z = 3.141593f;
    int n;

    n = float_to_str(buf, x);
    assert(n == strlen(buf));
    printf("Casted: %d (from: %g == %s)\n", (int)x, x, buf);

    n = float_to_str(buf, y);
    assert(n == strlen(buf));
    printf("Casted: %d (from: %g == %s)\n", (int)y, y, buf);

    n = float_to_str(buf, z);
    assert(n == strlen(buf));
    printf("Casted: %d (from: %g == %s)\n", (int)z, z, buf);

    uint64_t pi_minus_3 = (uint64_t)(0.141592654 * (double)(1ul << 60));
    n = frac_to_str(stuffs, pi_minus_3, 7);
    printf("Pi fractional digits: %s (n = %d)\n", stuffs, n);

    int limit = 7;
    uint64_t pi_fracerd = bankers64(pi_minus_3, &limit);
    n = sprintu64(stuffs, pi_fracerd);
    printf("Pi fractional scaled: %s (x = %lu, n = %d)\n", stuffs, pi_fracerd, n);
}


int main(int argc, char* argv[])
{
    test_randomax();
    test_ringbuf();
    test_signextend();

    response_tb();
    strfmt_tb();

    return 0;
}
