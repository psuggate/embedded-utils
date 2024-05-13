#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "response.h"


void response_tb()
{
    int idx;
    char* buf;

    printf("\nResponse-buffer Testbench\n");
    
    buf = sda_start(&idx);
    assert(buf != NULL);
    printf("started\n");

    assert(sda_finish(buf, idx));
    printf("finished\n");

    printf("passed\n");
}
