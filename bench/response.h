#ifndef __RESPONSE_H__
#define __RESPONSE_H__

#include <stdint.h>
#include <string.h>


#define USB_SEND_BUFFER_SIZE 256
#define USB_RECV_BUFFER_SIZE 256

#define SDACMD_OUTPUT_BUFFERS_NUM  4
#define SAMPLE_OUTPUT_BUFFERS_NUM 16


// -- External user functions -- //

#ifdef __cplusplus
extern "C"
    {
#endif


// -- Exported functions -- //

char* sda_start(int32_t* idx);
int sda_finish(char* end, int32_t idx);


#ifdef __cplusplus
    }
#endif


#endif /* __RESPONSE_H__ */
