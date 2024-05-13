#include "strfmt.h"


// -- Size-limits for various conversions -- //

#define MAX_UINT16_BYTES 6
#define MAX_SINT16_BYTES 7

#define MAX_UINT32_BYTES 11
#define MAX_SINT32_BYTES 12

#define MAX_UINT64_BYTES 21
#define MAX_SINT64_BYTES 22

#define MAX_FRAC_DIGITS  6
#define MAX_FLOAT_BYTES  19
#define MAX_INTEG_VALUE  2147483647
#define MIN_INTEG_VALUE  (-2147483648)

#define __use_bankers_rounding


/**
 * Append the 'src' string into the 'dst' string, returning the number of chars
 * that were appended.
 */
int strapp(char* dst, char* src)
{
    char c, *p = src;
    do {
        c = *p++;
        *dst++ = c;
    } while (c != '\0');
    return p - src - 1;
}


// -- 16-bit printing -- //

static inline char* __printu16(char* buf, uint16_t n)
{
#if 0
    char m[5];
    char* p = m;

    do {
        *p++ = '0' + n % 10;
        n /= 10;
    }
    while (n != 0);

    do {
        *buf++ = *(--p);
    }
    while (p > m);

#else

    char m[4];
    char* p = m;

    *p++ = '0' + n % 10;
    n /= 10;
    if (n == 0) goto end3;

    *p++ = '0' + n % 10;
    n /= 10;
    if (n == 0) goto end2;

    *p++ = '0' + n % 10;
    n /= 10;
    if (n == 0) goto end1;

    *p++ = '0' + n % 10;
    n /= 10;
    if (n == 0) goto end0;

    *buf++ = '0' + n;
end0:
    *buf++ = *(--p);
end1:
    *buf++ = *(--p);
end2:
    *buf++ = *(--p);
end3:
    *buf++ = *(--p);

#endif
    *buf = '\0';

    return buf;
}

/**
 * String-print into the given buffer, returning the updated pointer to the end
 * of the non-'\0' chars written.
 */
char* printu16(char* buf, uint16_t n)
{
    return __printu16(buf, n);
}

/**
 * String-print into the given buffer, returning the updated pointer to the end
 * of the non-'\0' chars written.
 */
char* printi16(char* buf, int16_t n)
{
    if (n < 0) {
        n = -n;
        *buf++ = '-';
    }
    return __printu16(buf, (uint16_t)n);
}

/**
 * String-print into the given buffer, returning the number of non-'\0' chars
 * written.
 */
int sprintu16(char* buf, uint16_t n)
{
    return __printu16(buf, n) - buf;
}

/**
 * String-print into the given buffer, returning the number of non-'\0' chars
 * written.
 */
int sprinti16(char* buf, int16_t n)
{
    char* p = buf;
    if (n < 0) {
        n = -n;
        *buf++ = '-';
    }
    return __printu16(buf, (uint16_t)n) - p;
}


// -- 32-bit printing -- //

static inline char* __printu32(char* buf, uint32_t n)
{
#if 0
    char m[10];
    char* p = m;

    do {
        *p++ = '0' + n % 10;
        n /= 10;
    }
    while (n != 0);

    do {
        *buf++ = *(--p);
    }
    while (p > m);

#else

    char m[5];
    char* p = m;

    *p++ = '0' + n % 10;
    n /= 10;
    if (n == 0) goto end4;

    *p++ = '0' + n % 10;
    n /= 10;
    if (n == 0) goto end3;

    *p++ = '0' + n % 10;
    n /= 10;
    if (n == 0) goto end2;

    *p++ = '0' + n % 10;
    n /= 10;
    if (n == 0) goto end1;

    *p++ = '0' + n % 10;
    n /= 10;
    if (n == 0) goto end0;
    buf = __printu16(buf, n);

end0:
    *buf++ = *(--p);
end1:
    *buf++ = *(--p);
end2:
    *buf++ = *(--p);
end3:
    *buf++ = *(--p);
end4:
    *buf++ = *(--p);

#endif
    *buf = '\0';

    return buf;
}

char* printu32(char* buf, uint32_t n)
{
    return __printu32(buf, n);
}

char* printi32(char* buf, int32_t n)
{
    if (n < 0) {
        n = -n;
        *buf++ = '-';
    }
    return __printu32(buf, n);
}

/**
 * String-print into the given buffer, returning the number of non-'\0' chars
 * written.
 */
int sprinti32(char* buf, int32_t n)
{
    char* p = buf;
    if (n < 0) {
        n = -n;
        *buf++ = '-';
    }
    return __printu32(buf, (uint32_t)n) - p;
}

int sprintu32(char* buf, uint32_t n)
{
    return __printu32(buf, (uint32_t)n) - buf;
}


// -- 64-bit printing -- //

char* printu64(char* buf, uint64_t x)
{
#if 1
    char m[MAX_UINT64_BYTES - 1];
    char* p = &m[MAX_UINT64_BYTES - 2];

    do {
        *p-- = '0' + x % 10;
        x /= 10;
    }
    while (x != 0);

    size_t n = &m[MAX_UINT64_BYTES - 2] - p;
    memcpy(buf, p+1, n);
#else

    char* const end = &buf[MAX_UINT64_BYTES - 1];
    char* p = end;

    do {
        *(--p) = '0' + x % 10;
        x /= 10;
    }
    while (x != 0);

    if (p == buf) {
	*end = '\0';
	return end;
    }

    size_t n = end - p;
    memmove(buf, p, n);
#endif

    buf += n;
    *buf = '\0';

    return buf;
}

static inline char* __printi64(char* buf, int64_t n)
{
    if (n < 0) {
	n = -n;
        *(buf++) = '-';
    }
    return printu64(buf, n);
}

char* printi64(char* buf, int64_t n)
{
    return __printi64(buf, n);
}

/**
 * String-print into the given buffer, returning the number of non-'\0' chars
 * written.
 */
int sprinti64(char* buf, int64_t n)
{
    char* ptr = __printi64(buf, n);
    return (int)(ptr - buf);
}

int sprintu64(char* buf, uint64_t n)
{
    char* ptr = printu64(buf, n);
    return (int)(ptr - buf);
}


// -- Float printing -- //

/**
 * Fixed-point Bankers' rounding.
 *
 * Note(s):
 *  - Assumes the number is in 4.60 fixed-point format.
 *  - Performs quite a few 64-bit multiplications.
 */
uint64_t bankers64(uint64_t frac, int* limit)
{
    int n = *limit;

    const uint64_t bsel = 1ul << 60;
    const uint64_t mask = bsel - 1ul;

    uint64_t x = 0;
    frac &= mask;

    while (frac != 0 && n-- > 0) {
        frac *= 10;
        if (!n) {
            // Bankers' rounding for the final digit
            int oddlsb = (frac & bsel) != 0;
            int roundy = (frac & (bsel >> 1)) != 0;
            int sticky = (frac & (mask >> 1)) != 0;
            if (roundy && (oddlsb || sticky)) {
                frac += bsel;
            }
        }
        x = x*10 + (frac >> 60);
        frac &= mask;
    }

    if (n > 0) {
        *limit = *limit - n;
    }
    return x;
}

// 4 bits integral, 60 bits fractional, and only the "floor."
int frac_to_str(char* buf, uint64_t frac, int limit)
{
    int len = 0;
    while (frac != 0 && len < limit) {
        frac &= ((uint64_t)1 << 60) - (uint64_t)1;
        frac *= (uint64_t)10;
        buf[len++] = '0' + (frac >> 60);
    }
    buf[len] = '\0';
    return len;
}


// Todo:
//  - support for denormals;
int float_to_str(char* buf, float x)
{
    if (x > MAX_INTEG_VALUE) {
        strcpy(buf, "+Inf");
    } else if (x < MIN_INTEG_VALUE) {
        strcpy(buf, "-Inf");
    }

    int len = 0;

#ifdef __use_bankers_rounding
    int32_t intn = (int32_t)x;

    // Extract the mantissa and exponent bit-fields, from the 'float', and then
    // convert to integral values.
    uint32_t* fptr = (uint32_t*)(&x);
    uint32_t word = *fptr;
    int16_t expo = (int16_t)((word >> 23) & 0xff) - 127;
    uint32_t mant = (word & 0x7fffff) | 0x800000;

    if (x < 0.0) {
        buf[0] = '-';
        len++;
    }
    if (intn < 0) {
        intn = -intn;
    }

    int shift;
    uint64_t mbits, scale;

    // Extract, shift, and scale the fractional part of the mantissa.
    if (expo > 16) {
        shift = 23 - expo;
        scale = 1000000;
        mbits = (uint64_t)1 << shift;
    } else {
        shift = 17 - expo;
        scale = 15625;
        mbits = (uint64_t)64 << shift;
    }
    uint64_t lmant = ((uint64_t)mant & (mbits - (uint64_t)1)) * scale;

    // Compute bit -selects/-masks for (Bankers') rounding
    uint64_t obits = (uint64_t)1 << shift; // Odd/even bit-select
    uint64_t rbits = obits >> 1;   // Rounding bit-select
    uint64_t smask = rbits - (uint64_t)1;  // Sticky-bits mask
    int oddlsb = (lmant & obits) != (uint64_t)0;
    int roundy = (lmant & rbits) != (uint64_t)0;
    int sticky = (lmant & smask) != (uint64_t)0;

    uint32_t frac = (lmant >> shift) + (roundy && (oddlsb || sticky) ? 1 : 0);

    // Emit the integral-component of the floating-point number
    if (frac >= 1000000) {
        // Rounding caused the fractional-component to overflow
        frac -= 1000000;
        intn++;
    }
    len += sprinti32(&buf[len], intn);
#else  /* !__use_bankers_rounding */
    // No rounding
    #warning "float_to_str(): No rounding being used, which is not numerically-stable"
    int32_t intn = (int32_t)n;
    int32_t frac = (int32_t)((n - (float)intn)*1e6);
    if (frac < 0) {
        frac = -frac;
    }
    len = sprinti32(buf, intn);
#endif /* !__use_bankers_rounding */
    if (!frac) {
        return len;
    }

    // Emit the fractional-component of the floating-point number
    int prec = MAX_FRAC_DIGITS;
    while (frac % 10 == 0 && prec--) {
        frac /= 10;
    }

    if (prec) {
        buf += len;
        len += prec + 1;

        *(buf++) = '.';
        buf[prec] = '\0';
        while (prec--) {
            buf[prec] = '0' + (frac % 10);
            frac /= 10;
        }
    }

    return len;
}

// Slightly slower that 'float_to_str(..)', presumably due to 'bankers64(..)'
// resulting in many more 64-bit multiplications?
int fp32_to_str(char* buf, float x)
{
    if (x > MAX_INTEG_VALUE) {
        strcpy(buf, "+Inf");
    } else if (x < MIN_INTEG_VALUE) {
        strcpy(buf, "-Inf");
    }

    uint32_t* fptr = (uint32_t*)(&x);
    uint32_t dword = *fptr;
    int16_t expo = (int16_t)((dword >> 23) & 0xff) - 127;
    uint32_t mbits = 1u << 23;
    uint32_t mant = (dword & (mbits - 1u)) | mbits;
    uint64_t lmant = mant;

    int len = 0;
    if (dword >> 31 != 0) {
        buf[len++] = '-';
    }

    int shift = 37 + expo;
    int trunc = 23 - expo;

    uint64_t frac = shift < 0 ? lmant >> (-shift) : lmant << shift;
    uint32_t intn = trunc > 31 ? 0u :
        trunc < 0 ? mant << (-trunc) : mant >> trunc;

    int prec = MAX_FRAC_DIGITS;
    frac = bankers64(frac, &prec);
    if (frac >= 1000000ul) {
        frac = 0ul;
        prec = 0;
        intn++;
    }
    len += sprintu32(&buf[len], intn);

    if (prec) {
        while (frac % 10 == 0 && prec--) {
            frac /= 10;
        }
    }
    if (prec > 0) {
        buf[len++] = '.';
        len += prec;
        buf += len;
        *(buf--) = '\0';
        while (prec--) {
            *(buf--) = '0' + (frac % 10);
            frac /= 10;
        }
    }
    return len;
}

//
// Todo:
//  - support for denormals;
//
int double_to_str(char* buf, double x)
{
    if (x > MAX_INTEG_VALUE) {
        strcpy(buf, "+Inf");
    } else if (x < MIN_INTEG_VALUE) {
        strcpy(buf, "-Inf");
    }

#ifdef __use_bankers_rounding

    uint64_t* dptr = (uint64_t*)(&x);
    uint64_t qword = *dptr;
    int16_t expo = (int16_t)((qword >> 52) & 0x7ff) - 1023;
    uint64_t mbits = 1ul << 52;
    uint64_t mant = (qword & (mbits - 1ul)) | mbits;

    int len = 0;
    if (qword >> 63 != 0) {
        buf[len++] = '-';
    }

    int shift = 8 + expo;
    int trunc = 52 - expo;

    uint64_t frac = shift < 0 ? mant >> (-shift) : mant << shift;
    uint64_t intn = trunc > 63 ? 0ul : mant >> trunc;

    int prec = MAX_FRAC_DIGITS;
    frac = bankers64(frac, &prec);
    if (frac >= 1000000ul) {
        frac = 0ul;
        prec = 0;
        intn++;
    }

    len += sprintu64(&buf[len], intn);
    if (prec) {
        while (frac % 10 == 0 && prec--) {
            frac /= 10;
        }
    }
    if (prec > 0) {
        buf[len++] = '.';
        len += prec;
        buf += len;
        *(buf--) = '\0';
        while (prec--) {
            *(buf--) = '0' + (frac % 10);
            frac /= 10;
        }
    }

#else  /* !__use_bankers_rounding */

    // No rounding
    #warning "double_to_str(): No rounding being used, which is not numerically-stable"

    const int32_t intn = (int32_t)n;
    int32_t frac = (int32_t)((n - (double)intn)*1e6);

    if (!frac) {
        return sprinti32(buf, intn);
    } else if (frac < 0) {
        frac = -frac;
    }

    int prec = MAX_FRAC_DIGITS;
    while (frac % 10 == 0 && prec--) {
        frac /= 10;
    }

    int len = sprinti32(buf, intn);
    if (prec) {
        buf += len;
        len += prec + 1;

        *(buf++) = '.';
        buf[prec] = '\0';
        while (prec--) {
            buf[prec] = '0' + (frac % 10);
            frac /= 10;
        }
    }

#endif /* !__use_bankers_rounding */

    return len;
}
