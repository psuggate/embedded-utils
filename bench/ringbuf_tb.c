#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "ringbuf.h"
#include "strfmt.h"


#define RINGBUF_ITEMS 8
#define RINGBUF_BYTES (RINGBUF_ITEMS*sizeof(int32_t))

RB_MAKE_PUSH(int32_t)
RB_MAKE_POP(int32_t)


//
// Todo:
//  - check wrapping;
//  - check "concurrency;"
//
void ringbuf_tb() {
    void* rb_mem = malloc(16 + RINGBUF_BYTES);
    ringbuf_t* rb = rb_create(rb_mem, RINGBUF_ITEMS);
    int32_t val;

    printf("\nRingbuf Sanity-Checks:\n");

    // Push an item, and make sure that just it can be popped.
    rb_push(rb, 666);
    assert(rb_pop(rb, &val));
    assert(!rb_pop(rb, &val));

    // Fill up the ring-buffer, and make sure that it no longer accepts values,
    // and that its values can be popped.
    for (int i=0; i<8; i++) {
        rb_push(rb, 42069);
    }
    assert(!rb_push(rb, 90210));
    assert(rb_pop(rb, &val));
    assert(rb_push(rb, 90210));

    // Check that it clears properly.
    rb_clear(rb);
    assert(!rb_pop(rb, &val));
    assert(rb_push(rb, 90210));

    printf("passed\n");
    printf("\nRingbuf random pushes and pops (10M):\n");

    rb_clear(rb);
    int count = 0;
    for (int i=10000000; i--;) {
	if (rand() & 0x1) {
	    count += rb_push(rb, rand());
	} else {
	    count -= rb_pop(rb, &val);
	}
	assert(count >= 0 && count < RINGBUF_ITEMS);
    }

    printf("passed\n");

    free(rb_mem);
}
