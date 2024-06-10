#include <string.h>
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
 * Copy as many entries as required or possible, and returns the number of bytes
 * written.
 */
int32_t rb_copy(ringbuf_t* rb, const uint8_t* src, int32_t len)
    {
    int32_t count = rb_space(rb);
    int32_t head = rb->head;
    uint8_t* dst;

    if (count < len)
        {
        // Partial fill
        len = count;
        }
    count = 0;

    if (head + len > rb->wrap)
        {
        // If 'len' requires a "wrap," then first fill to the end of the buffer
        count = rb->wrap - head + 1;
	dst = (uint8_t*)rb->data + head;
        memcpy((void*)dst, (const void*)src, count);
        len -= count;
	src += count;
        head = 0;
        }

    // Write (remaining) bytes into buffer, at the current "head"
    dst = (uint8_t*)rb->data + head;
    memcpy((void*)dst, (const void*)src, len);
    head += len;
    count += len;

    // Update the 'ringbuf_t'
    // Todo: atomically ??
    rb->head = head & rb->wrap;

    return count;
    }

int32_t rb_many(ringbuf_t* rb, uint8_t val, int32_t len)
{
    int32_t count = rb_space(rb);
    int32_t head = rb->head;
    uint8_t* dst;

    if (count > len) {
	count = len;
    } else {
	len = count;
    }

    if (head + len > rb->wrap) {
        // If 'len' requires a "wrap," then first fill to the end of the buffer
        count = rb->wrap - head + 1;
	dst = (uint8_t*)rb->data + head;
        memset((void*)dst, val, count);
	count = len - count;
        head = 0;
    }

    // Write (remaining) bytes into buffer, at the current "head"
    dst = (uint8_t*)rb->data + head;
    memset((void*)dst, val, count);
    head += count;

    // Update the 'ringbuf_t'
    rb->head = head & rb->wrap;
    return len;
}

/**
 * Take as many entries as required or possible, and returns the number of bytes
 * read.
 */
int32_t rb_take(ringbuf_t* rb, uint8_t* dst, int32_t len)
    {
    int32_t count = rb_count(rb);
    int32_t tail = rb->tail;
    uint8_t* src;

    if (count < len)
        {
        // Partial take
        len = count;
        }
    count = 0;

    if (tail + count > rb->wrap)
        {
        // If 'len' requires a "wrap," then first take to the end of the buffer
        count = rb->wrap - tail + 1;
	src = (uint8_t*)rb->data + tail;
        memcpy((void*)dst, (const void*)src, len);
        len -= count;
	dst += count;
        tail = 0;
        }

    // Read (remaining) bytes into 'dst' buffer, from the current "tail"
    src = (uint8_t*)rb->data + tail;
    memcpy(dst, (const void*)src, len);
    tail += len;
    count += len;

    // Update the 'ringbuf_t'
    // Todo: atomically ??
    rb->tail = tail & rb->wrap;

    return count;
    }

/**
 * Drop the inidicated number of entries, if possible, and returns the number of
 * bytes dropped.
 */
int32_t rb_drop(ringbuf_t* rb, int32_t len)
    {
    int32_t count = rb_count(rb);

    if (count < len)
        {
        // Partial drop
        len = count;
        }
    rb->tail = (rb->tail + len) & rb->wrap;

    return len;
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
