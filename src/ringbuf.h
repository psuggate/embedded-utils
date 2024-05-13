#ifndef __RINGBUF_H__
#define __RINGBUF_H__


#include <stdint.h>


/**
 * Ring-buffer that stores up to 'n-1' items, and is concurrency-safe.
 *
 * Note(s):
 *  - if 'rb_clear(rb);' is called while a 'rb_pop(rb, val)' is in progress (for
 *    example, from an interrupt service routine), then this call may have no
 *    effect;
 *  - therefore, buffer-clearing should be done from the "pop" thread;
 *  - 'wrap' is one less than 'size', and 'size' must be a power of two;
 *  - the data-structure size is 16 bytes (on 32-bit processors);
 *  - total size is 16 bytes (struct) + 'sizeof(x)*size' bytes (buffer);
 */
typedef struct {
    int32_t head;
    int32_t tail;
    int32_t wrap;
    void* data;
} ringbuf_t;


// -- Private macros -- //

#define __RB_PUSH(rb, typ, elem)                \
    if (rb_count(rb) < rb->wrap)                \
    {                                           \
        ((typ*)rb->data)[rb->head++] = elem;    \
        rb->head &= rb->wrap;                   \
        return 1;                               \
    }                                           \
    else                                        \
    {                                           \
        return 0;                               \
    }

// Note: 'tail' is advanced this way so that if 'rb_clear(..)' is called from an
//   ISR, during this routine's execution, the erroneous value of 'tail' won't
//   (immediately) result in a buffer-overflow -- as would be the case if we set
//   to 'tail = (tail = head)++', which indicates "full."
#define __RB_POP(rb, typ, elem)                 \
    int32_t tail = rb->tail;                    \
    if (tail != rb->head)                       \
    {                                           \
        *elem = ((typ*)rb->data)[tail++];       \
        rb->tail = tail & rb->wrap;             \
        return 1;                               \
    }                                           \
    else                                        \
    {                                           \
        return 0;                               \
    }


// -- User macros -- //

#define RB_MAKE_PUSH(typ)                               \
    static inline int rb_push(ringbuf_t* rb, typ x)     \
    {                                                   \
        __RB_PUSH(rb, typ, x);                          \
    }

#define RB_MAKE_POP(typ)                                \
    static inline int rb_pop(ringbuf_t* rb, typ* p)     \
    {                                                   \
        __RB_POP(rb, typ, p);                           \
    }


// -- Inlinable user functions -- //

static inline int32_t rb_count(ringbuf_t* rb)
    {
    return (rb->head - rb->tail) & rb->wrap;
    }

static inline int32_t rb_space(ringbuf_t* rb)
    {
    return rb->wrap - rb_count(rb);
    }


// -- External user functions -- //

#ifdef __cplusplus
extern "C"
    {
#endif

    ringbuf_t* rb_create(void* rb_ptr, int32_t size);
    void rb_clear(ringbuf_t* rb);

    int rb_acquire(ringbuf_t* rb, int32_t* index);
    int rb_current(ringbuf_t* rb, int32_t* index);
    int rb_release(ringbuf_t* rb);

#ifdef __cplusplus
    }
#endif


#endif /* __RINGBUF_H__ */
