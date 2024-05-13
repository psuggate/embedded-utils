#include "ringbuf.h"

/**
 * Ring-buffer that stores up to 'n-1' items, and is concurrency-safe.
 *
 * Note(s):
 *  - 'wrap' is one less than 'size', and 'size' must be a power of two;
 *  - the data-structure size is 16 bytes (on 32-bit processors);
 *  - total size is 16 bytes (struct) + 'sizeof(x)*n' bytes (buffer);
 */
ringbuf_t* rb_create(void* rb_ptr, int32_t size)
    {
    ringbuf_t *rb = (ringbuf_t*) rb_ptr;

    rb->wrap = size - 1;
    rb->head = 0;
    rb->tail = 0;
    rb->data = rb_ptr + sizeof(ringbuf_t);

    return rb;
    }

/**
 * Clears the contents of a 'ringbuf_t'.
 *
 * Note(s):
 *  - if 'rb_clear(rb);' is called while a 'rb_pop(rb, elem)' is in progress,
 *    then this call may have no effect;
 *  - therefore, buffer-clearing should be done from the "pop" thread;
 */
void rb_clear(ringbuf_t* rb)
    {
    rb->tail = rb->head;
    }


/**
 * Acquire an index into a buffer of buffers/structs.
 */
int rb_acquire(ringbuf_t* rb, int32_t* index)
{
    if (rb_count(rb) < rb->wrap)
    {
        *index = rb->head++;
        rb->head &= rb->wrap;
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
 * Get the currently-active index, that will be dropped on the next release.
 */
int rb_current(ringbuf_t* rb, int32_t* index)
{
    if (rb_count(rb) > 0)
    {
        *index = rb->tail;
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
 * Release the currently-active index.
 */
int rb_release(ringbuf_t* rb)
{
    int32_t tail = rb->tail;
    if (tail++ != rb->head)
    {
        rb->tail = tail & rb->wrap;
        return 1;
    }
    else
    {
        return 0;
    }
}
