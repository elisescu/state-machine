#ifndef MESSGEQUEUE_H
#define MESSGEQUEUE_H

#include <stdint.h>
#include <pthread.h>

typedef struct Message_s {
    struct Message_s *next;
    struct Message_s *prev;
    uint32_t message_type;
    uint32_t data_length;
    void *data;
} Message_t;

typedef struct MessageQueue_s {
   Message_t *queue_head;
   Message_t *dequeue_head;
   uint32_t no_messages;
   pthread_mutex_t *mutex;
} MessageQueue_t;


MessageQueue_t* mq_create();
void mq_destroy(MessageQueue_t *queue);
void mq_queue_message(MessageQueue_t *queue, Message_t *m);
Message_t* mq_dequeue_message(MessageQueue_t *queue);
int mq_test();




#endif // MESSGEQUEUE_H
