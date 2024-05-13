#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "strfmt.h"


#define MAX_FRAC_DIGITS  6


// The "reference" for the testbench.
static int f32_to_str(char* buf, float x)
{
    int n = sprintf(buf, "%.6f", x);

    for (int i = 1; i < 8; i++) {
        if (buf[n-i] == '0' || buf[n-i] == '.') {
            buf[n-i] = '\0';
        } else {
            return n - i + 1;
        }
    }

    return n - 7;
}

void dump_float_bits(float x)
{
    uint32_t* ptr = (uint32_t*)(&x);
    uint32_t word = *ptr;
    char sstr[3] = "+1";
    if ((word & (1 << 31)) != 0) {
        sstr[0] = '-';
    }
    int16_t expo = (int16_t)((word >> 23) & 0xff) - 127;
    uint32_t mant = (word & 0x7fffff) | 0x800000;
    printf("x: %.9f, s: %s, e: %-3d, m: %8u (0x%06x)\n", x, sstr, (int)expo, mant, mant);
}


void strfmt_floating_bench()
{
    clock_t start, end;
    double ticks;
    uint64_t bytes = 0;
    float rands[1000];
    char buf[256];
    
    for (int i=1000; i--;) {
        int l = rand();
        int u = rand() + 1;
        float x = (float)((double)l / (double)u);
        if (rand() >= (RAND_MAX >> 1)) {
            x = -x;
        }
        rands[i] = x;
    }

    printf("\nMicrobenchmarks for FLOATING formatters:\n\n");

    bytes = 0; ticks = 0.0;
    for (int k=10; k--;) {
        start = clock();
        for (int i=1000; i--;) {
            for (int j=1000; j--;) {
                int n = fp32_to_str(buf, rands[j]);
                bytes += n;
            }
        }
        end = clock();
        ticks += ((double)(end - start)) / CLOCKS_PER_SEC;
    }
    printf("\tfp32_to_str():   \t%.3f (bytes: %lu, 10M)\n", ticks, bytes);

    bytes = 0; ticks = 0.0;
    for (int k=10; k--;) {
        start = clock();
        for (int i=1000; i--;) {
            for (int j=1000; j--;) {
                int n = float_to_str(buf, rands[j]);
                bytes += n;
            }
        }
        end = clock();
        ticks += ((double)(end - start)) / CLOCKS_PER_SEC;
    }
    printf("\tfloat_to_str():   \t%.3f (bytes: %lu, 10M)\n", ticks, bytes);

    bytes = 0; ticks = 0.0;
    for (int k=10; k--;) {
        start = clock();
        for (int i=1000; i--;) {
            for (int j=1000; j--;) {
                int n = f32_to_str(buf, rands[j]);
                bytes += n;
            }
        }
        end = clock();
        ticks += ((double)(end - start)) / CLOCKS_PER_SEC;
    }
    printf("\tsprintf(float):   \t%.3f (bytes: %lu, 10M)\n", ticks, bytes);

    bytes = 0; ticks = 0.0;
    for (int k=10; k--;) {
        start = clock();
        for (int i=1000; i--;) {
            for (int j=1000; j--;) {
                int n = double_to_str(buf, (double)rands[j]);
                bytes += n;
            }
        }
        end = clock();
        ticks += ((double)(end - start)) / CLOCKS_PER_SEC;
    }
    printf("\tdouble_to_str():   \t%.3f (bytes: %lu, 10M)\n", ticks, bytes);

    printf("\ndone\n\n");
}

/*
static void run_microbench(const char* name, uint64_t rands[1000], int (*fun)(char*, uint64_t))
{
    clock_t start, end;
    char buf[256];
    uint64_t bytes = 0;
    double ticks = 0.0;

    for (int k=10; k--;) {
        start = clock();
        for (int i=1000; i--;) {
            for (int j=1000; j--;) {
                bytes += fun(buf, rands[j]);
            }
        }
        end = clock();
        ticks += ((double)(end - start)) / CLOCKS_PER_SEC;
    }
    printf("\t%s():\t\t%.3f (bytes: %lu, 10M)\n", name, ticks, bytes);
}

static int run_printi64(char* buf, uint64_t val)
{
    return 21 + buf - printi64(buf, (int64_t)val);
}
*/

void strfmt_integral_bench()
{
    volatile clock_t start, end;
    volatile double ticks;
    uint64_t bytes = 0;
    uint64_t rands[1000];
    char buf[256];
    
    for (int i=1000; i--;) {
        uint64_t x = (uint64_t)rand() << 62 | (uint64_t)rand() << 31 | (uint64_t)rand();
        rands[i] = x;
    }

    printf("\nMicrobenchmarks for INTEGRAL formatters:\n\n");

    bytes = 0; ticks = 0.0;
    for (int k=10; k--;) {
        start = clock();
        for (int i=1000; i--;) {
            for (int j=1000; j--;) {
                char* p = printi64(buf, (int64_t)rands[j]);
                bytes += 21 - (int)(p - buf);
            }
        }
        end = clock();
        ticks += ((double)(end - start)) / CLOCKS_PER_SEC;
    }
    printf("\tprinti64():\t\t%.3f (bytes: %lu, 10M)\n", ticks, bytes);

    bytes = 0; ticks = 0.0;
    for (int k=10; k--;) {
        start = clock();
        for (int i=1000; i--;) {
            for (int j=1000; j--;) {
                int n = sprintu64(buf, rands[j]);
                bytes += n;
            }
        }
        end = clock();
        ticks += ((double)(end - start)) / CLOCKS_PER_SEC;
    }
    printf("\tsprintu64():\t\t%.3f (bytes: %lu, 10M)\n", ticks, bytes);

    bytes = 0; ticks = 0.0;
    for (int k=10; k--;) {
        start = clock();
        for (int i=1000; i--;) {
            for (int j=1000; j--;) {
                int n = sprintf(buf, "%lu", rands[j]);
                bytes += n;
            }
        }
        end = clock();
        ticks += ((double)(end - start)) / CLOCKS_PER_SEC;
    }
    printf("\tsprintf(uint64_t): \t%.3f (bytes: %lu, 10M)\n", ticks, bytes);
    printf("\n");

    bytes = 0; ticks = 0.0;
    for (int k=10; k--;) {
        start = clock();
        for (int i=1000; i--;) {
            for (int j=1000; j--;) {
                char* p = printu32(buf, (uint32_t)rands[j]);
                bytes += (int)(p - buf);
            }
        }
        end = clock();
        ticks += ((double)(end - start)) / CLOCKS_PER_SEC;
    }
    printf("\tprintu32():\t\t%.3f (bytes: %lu, 10M)\n", ticks, bytes);

    bytes = 0; ticks = 0.0;
    for (int k=10; k--;) {
        start = clock();
        for (int i=1000; i--;) {
            for (int j=1000; j--;) {
                char* p = printi32(buf, (int32_t)rands[j]);
                bytes += (int)(p - buf);
            }
        }
        end = clock();
        ticks += ((double)(end - start)) / CLOCKS_PER_SEC;
    }
    printf("\tprinti32():\t\t%.3f (bytes: %lu, 10M)\n", ticks, bytes);

    bytes = 0; ticks = 0.0;
    for (int k=10; k--;) {
        start = clock();
        for (int i=1000; i--;) {
            for (int j=1000; j--;) {
                int n = sprintu32(buf, (uint32_t)rands[j]);
                bytes += n;
            }
        }
        end = clock();
        ticks += ((double)(end - start)) / CLOCKS_PER_SEC;
    }
    printf("\tsprintu32():\t\t%.3f (bytes: %lu, 10M)\n", ticks, bytes);

    bytes = 0; ticks = 0.0;
    for (int k=10; k--;) {
        start = clock();
        for (int i=1000; i--;) {
            for (int j=1000; j--;) {
                int n = sprinti32(buf, (int32_t)rands[j]);
                bytes += n;
            }
        }
        end = clock();
        ticks += ((double)(end - start)) / CLOCKS_PER_SEC;
    }
    printf("\tsprinti32():\t\t%.3f (bytes: %lu, 10M)\n", ticks, bytes);

    bytes = 0; ticks = 0.0;
    for (int k=10; k--;) {
        start = clock();
        for (int i=1000; i--;) {
            for (int j=1000; j--;) {
                int n = sprintf(buf, "%d", (int32_t)rands[j]);
                bytes += n;
            }
        }
        end = clock();
        ticks += ((double)(end - start)) / CLOCKS_PER_SEC;
    }
    printf("\tsprintf(int32_t): \t%.3f (bytes: %lu, 10M)\n", ticks, bytes);
    printf("\n");

    bytes = 0; ticks = 0.0;
    for (int k=10; k--;) {
        start = clock();
        for (int i=10000; i--;) {
            for (int j=1000; j--;) {
                char* p = printu16(buf, (uint16_t)rands[j]);
                bytes += (int)(p - buf);
            }
        }
        end = clock();
        ticks += ((double)(end - start)) / CLOCKS_PER_SEC;
    }
    printf("\tprintu16():\t\t%.3f (bytes: %lu, 100M)\n", ticks, bytes);

    bytes = 0; ticks = 0.0;
    for (int k=10; k--;) {
        start = clock();
        for (int i=10000; i--;) {
            for (int j=1000; j--;) {
                char* p = printi16(buf, (int16_t)rands[j]);
                bytes += (int)(p - buf);
            }
        }
        end = clock();
        ticks += ((double)(end - start)) / CLOCKS_PER_SEC;
    }
    printf("\tprinti16():\t\t%.3f (bytes: %lu, 100M)\n", ticks, bytes);

    bytes = 0; ticks = 0.0;
    for (int k=10; k--;) {
        start = clock();
        for (int i=10000; i--;) {
            for (int j=1000; j--;) {
                int n = sprintu16(buf, (uint16_t)rands[j]);
                bytes += n;
            }
        }
        end = clock();
        ticks += ((double)(end - start)) / CLOCKS_PER_SEC;
    }
    printf("\tsprintu16():\t\t%.3f (bytes: %lu, 100M)\n", ticks, bytes);

    bytes = 0; ticks = 0.0;
    for (int k=10; k--;) {
        start = clock();
        for (int i=10000; i--;) {
            for (int j=1000; j--;) {
                int n = sprinti16(buf, (int16_t)rands[j]);
                bytes += n;
            }
        }
        end = clock();
        ticks += ((double)(end - start)) / CLOCKS_PER_SEC;
    }
    printf("\tsprinti16():\t\t%.3f (bytes: %lu, 100M)\n", ticks, bytes);

    bytes = 0; ticks = 0.0;
    for (int k=10; k--;) {
        start = clock();
        for (int i=1000; i--;) {
            for (int j=1000; j--;) {
                int n = sprintf(buf, "%d", (int16_t)rands[j]);
                bytes += n;
            }
        }
        end = clock();
        ticks += ((double)(end - start)) / CLOCKS_PER_SEC;
    }
    printf("\tsprintf(int16_t): \t%.3f (bytes: %lu, 10M)\n", ticks, bytes);

    printf("\ndone\n\n");
}


void strfmt_integral_correct(const int reps)
{
    char p[32];
    char q[32];
    uint64_t x;
    int n, m;

    printf("\nTesting INTEGRAL formatters for correctness (N = %d):\n", reps);

    char fmti64[] = "i: %07d  =>  x = %ld, p: '%s' (n = %d), q: '%s' (m = %d)\n";
    char fmtu64[] = "i: %07d  =>  x = %lu, p: '%s' (n = %d), q: '%s' (m = %d)\n";
    char fmti32[] = "i: %07d  =>  x = %d, p: '%s' (n = %d), q: '%s' (m = %d)\n";
    char fmtu32[] = "i: %07d  =>  x = %u, p: '%s' (n = %d), q: '%s' (m = %d)\n";
    for (int i=reps; i--;) {
        x = (uint64_t)rand() << 62 | (uint64_t)rand() << 31 | (uint64_t)rand();

        // -- 64-bit -- //

        n = sprintf(p, "%ld", (int64_t)x);
        m = sprinti64(q, x);
        if (n != m) {
            printf(fmti64, i, (int64_t)x, p, n, q, m);
            assert(n == m);
        }
        if (strcmp(p, q) != 0) {
            printf(fmti64, i, (int64_t)x, p, n, q, m);
            assert(strcmp(p, q) == 0);
        }

        n = sprintf(p, "%lu", (uint64_t)x);
        m = sprintu64(q, x);
        if (n != m) {
            printf(fmtu64, i, (uint64_t)x, p, n, q, m);
            assert(n == m);
        }
        if (strcmp(p, q) != 0) {
            printf(fmtu64, i, (uint64_t)x, p, n, q, m);
            assert(strcmp(p, q) == 0);
        }

        // -- 32-bit -- //

        n = sprintf(p, "%d", (int32_t)x);
        m = sprinti32(q, x);
        if (n != m) {
            printf(fmti32, i, (int32_t)x, p, n, q, m);
            assert(n == m);
        }
        if (strcmp(p, q) != 0) {
            printf(fmti32, i, (int32_t)x, p, n, q, m);
            assert(strcmp(p, q) == 0);
        }

        n = sprintf(p, "%u", (uint32_t)x);
        m = sprintu32(q, x);
        if (n != m) {
            printf(fmtu32, i, (uint32_t)x, p, n, q, m);
            assert(n == m);
        }
        if (strcmp(p, q) != 0) {
            printf(fmtu32, i, (uint32_t)x, p, n, q, m);
            assert(strcmp(p, q) == 0);
        }
    }

    printf("passed\n\n");
}

static float rand_float()
{
    float x;
    do {
	int l = rand();
	int u = rand() + 1;
	x = (float)((double)l / (double)u);
	if (rand() & 0x01) {
	    x = -x;
	}
    } while (x > 1e-9 && x < 1e12);
    return x;
}

void strfmt_floating_correct(int reps)
{
    char p[256];
    char q[256];
    char r[256];
    char s[256];

    printf("\nTesting FLOATING formatters for correctness (N = %d):\n", reps);

    for (int i=reps; i--;) {
        float x = rand_float();

        int n = f32_to_str(p, x);
        int m = float_to_str(q, x);

        if (n != m) {
            printf("i: %6d  =>  x = %.9f, p: '%s' (n = %d), q: '%s' (m = %d)\n", i, x, p, n, q, m);
            dump_float_bits(x);
            assert(n == m);
        }

        if (strcmp(p, q) != 0) {
            printf("i: %6d  =>  x = %.9f, p: '%s' (n = %d), q: '%s' (m = %d)\n", i, x, p, n, q, m);
            dump_float_bits(x);
            assert(strcmp(p, q) == 0);
        }

        int a = fp32_to_str(r, x);

        if (n != a) {
            printf("i: %6d  =>  x = %.9f, p: '%s' (n = %d), r: '%s' (a = %d)\n", i, x, p, n, r, a);
            dump_float_bits(x);
            assert(n == a);
        }

        if (strcmp(p, r) != 0) {
            printf("i: %6d  =>  x = %.9f, p: '%s' (n = %d), r: '%s' (a = %d)\n", i, x, p, n, r, a);
            dump_float_bits(x);
            assert(strcmp(p, r) == 0);
        }

        int b = double_to_str(s, (double)x);

        if (n != b) {
            printf("i: %6d  =>  x = %.9f, p: '%s' (n = %d), s: '%s' (b = %d)\n", i, x, p, n, s, b);
            dump_float_bits(x);
            assert(n == b);
        }

        if (strcmp(p, s) != 0) {
            printf("i: %6d  =>  x = %.9f, p: '%s' (n = %d), s: '%s' (b = %d)\n", i, x, p, n, s, b);
            dump_float_bits(x);
            assert(strcmp(p, s) == 0);
        }
    }

    printf("passed\n\n");
}

void strfmt_correct(const int reps)
{
    strfmt_integral_correct(reps);
    strfmt_floating_correct(reps);
}


void strapp_correct()
{
    char dst[256] = {0};
    char src[256] = "Salad, potato, gelatine, and kiwi.";

    printf("\nTesting string-utilities for correctness:\n");

    int n = strlen(src);
    int m = strapp(dst, src);
    int o = strlen(dst);

    assert(n == m);
    assert(n == o);

    printf("passed\n\n");
}


void strfmt_tb()
{
    time_t t;
    srand((unsigned) time(&t));

    strfmt_integral_bench();
    strfmt_floating_bench();

    strapp_correct();
    strfmt_correct(10000000);
}
