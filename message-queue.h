#ifndef MESSGEQUEUE_H
#define MESSGEQUEUE_H

#include <stdint.h>

typedef struct Message_s {
    uint32_t message_type;
    uint32_t data_length;
    void *data;
} Message_t;

typedef struct MessageQueue_s {
   Message_t *messages;
}MessageQueue_t;

#endif // MESSGEQUEUE_H
