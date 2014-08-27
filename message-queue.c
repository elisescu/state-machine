#include <stdlib.h>
#include <stdio.h>

#include "message-queue.h"

#if 1
#define LOG(...) printf("\n" __VA_ARGS__)
#else
#define LOG(...)
#endif

MessageQueue_t* mq_create() {
    MessageQueue_t *queue = (MessageQueue_t*) malloc(sizeof(MessageQueue_t));
    queue->dequeue_head = queue->queue_head = NULL;
    queue->no_messages = 0;
    queue->mutex = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(queue->mutex, NULL);
    return queue;
}

void mq_destroy(MessageQueue_t *queue) {
    pthread_mutex_destroy(queue->mutex);
    // free all messages in the queue if any
    free(queue->mutex);
    free(queue);
}

void mq_queue_message(MessageQueue_t *queue, Message_t *m) {
    pthread_mutex_lock(queue->mutex);
    queue->no_messages++;
    m->next = NULL;
    m->prev = queue->queue_head;
    if (queue->no_messages == 1) {
        queue->dequeue_head = m;
    } else {
        queue->queue_head->next = m;
    }

    queue->queue_head = m;
    pthread_mutex_unlock(queue->mutex);
}

Message_t* mq_dequeue_message(MessageQueue_t *queue) {
    Message_t *m = NULL;
    pthread_mutex_lock(queue->mutex);
    queue->no_messages--;
    m = queue->dequeue_head;
    if (queue->no_messages > 0) {
        queue->dequeue_head = queue->dequeue_head->next;
        queue->dequeue_head->prev = NULL;
    }
    pthread_mutex_unlock(queue->mutex);
    m->next = NULL;
    m->prev = NULL;
    return m;
}

int mq_empty(MessageQueue_t *queue) {
    return queue->dequeue_head == queue->queue_head;
}

int mq_test() {
#define NO_MSG_TEST (100)
    MessageQueue_t *queue = mq_create();
    //Message_t msgs_[NO_MSG_TEST];
    uint16_t i;
    LOG("-----------Message Queue Basic Test------------");

    LOG("Adding %d messages to the queue", NO_MSG_TEST);
    for (i = 0; i < NO_MSG_TEST; i++) {
        LOG(" - queue message %d", i);
        //Message_t *msg = &msgs_[i];
        Message_t *msg = (Message_t*) malloc(sizeof(Message_t));
        msg->message_type = i;
        mq_queue_message(queue, msg);
    }

    LOG("Having %d messages in the queue:", queue->no_messages);
    for (;queue->no_messages > 0;) {
        Message_t *msg = mq_dequeue_message(queue);
        LOG(" - dequeue message %d ", msg->message_type);
    }

    LOG("Now we have %d messages, (deque_head==queue_head) = %d\n", queue->no_messages, queue->dequeue_head == queue->queue_head);
    return 1;
}
