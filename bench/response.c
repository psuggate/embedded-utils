#include "response.h"
#include "ringbuf.h"
#include "strfmt.h"


static uint8_t sdamsg_readies[SDACMD_OUTPUT_BUFFERS_NUM];
static uint8_t sample_readies[SAMPLE_OUTPUT_BUFFERS_NUM];

static char sdamsg_buffers[SDACMD_OUTPUT_BUFFERS_NUM][USB_SEND_BUFFER_SIZE];
static char sample_buffers[SAMPLE_OUTPUT_BUFFERS_NUM][USB_SEND_BUFFER_SIZE];

static ringbuf_t sdam_rb = {0, 0, SDACMD_OUTPUT_BUFFERS_NUM-1, (void*)sdamsg_readies};
static ringbuf_t samp_rb = {0, 0, SAMPLE_OUTPUT_BUFFERS_NUM-1, (void*)sample_readies};


char* sda_start(int32_t* idx)
{
    if (rb_acquire(&sdam_rb, idx)) {
        sdamsg_readies[*idx] = 0;
        return sdamsg_buffers[*idx];
    }

    return NULL;
}


// Todo: how to indicate the the buffer is ready !?
int sda_finish(char* end, int32_t idx)
{
    *end++ = '\n';
    *end = '\0';
    sdamsg_readies[idx] = 1;

    return 1;
}
