#include "ringbuf_tb.h"
#include "strfmt_tb.h"
#include "response_tb.h"


int main(int argc, char* argv[])
{
    ringbuf_tb();
    response_tb();
    strfmt_tb();

    return 0;
}
