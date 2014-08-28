#include <stdio.h>
#include <state-machine.h>
#include <message-queue.h>

#define LOGV(...) {printf("\n" __VA_ARGS__); fflush(stdout);}

int main(void)
{
    test_sm();
    //mq_test();
    return 0;
}
