#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "ringbuf.h"
#include "strfmt.h"


RB_MAKE_PUSH(int32_t)
RB_MAKE_POP(int32_t)


//
// Todo:
//  - check wrapping;
//  - check "concurrency;"
//
void ringbuf_tb() {
    void* rb_mem = malloc(16 + 32);
    ringbuf_t* rb = rb_create(rb_mem, 8);
    int32_t val;

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

    free(rb_mem);
}
