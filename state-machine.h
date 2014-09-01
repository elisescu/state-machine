#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include "message-queue.h"
#include <pthread.h>

// private section
enum {
    MSG_STATE_TRANSIT = 0,
    MSG_STATE_MESSAGE = 1,
    MSG_MACHINE_EXIT =2
};

typedef struct SMachineMessage_s{
    uint32_t message_type;
    void *data;
} SMachineMessage_t;

// public section
enum {
    SM_OK = 0,
    SM_EXIT = 1,
    SM_ERROR = 2
};

typedef struct StateMessage_s {
    uint32_t message_type;
    uint32_t val1;
    uint32_t val2;
    void *obj;
} StateMessage_t;

struct StateMachine_s;
struct State_s;

typedef void (*StateEnterFunction_T)(struct StateMachine_s *, struct State_s *);
typedef void (*StateExitFunction_T)(struct StateMachine_s *, struct State_s *);
typedef void (*StateProcessMessageFunction_T)(struct StateMachine_s *, struct State_s *, StateMessage_t *msg);

typedef struct State_s {
    /**
     * @brief enter is the function to execute when entering to the state. Can be NULL.
     */
    StateEnterFunction_T enter;
    /**
     * @brief processMessage is the function to execute when a message has been sent to the
     * state machine. It will be called on the current state of the machine, when the message
     * has been dequeued from the queue.
     */
    StateProcessMessageFunction_T processMessage;
    /**
     * @brief exit is the function to execute when exiting from the current state. Can be NULL.
     */
    StateExitFunction_T exit;
    char *state_name;
} State_t;

typedef struct StateMachine_s {
    char *machine_name;
    MessageQueue_t *machine_queue;
    pthread_t *machine_thread;
    int machine_finished;
    pthread_cond_t *machine_cond;
    pthread_mutex_t *machine_mutex;
    State_t *init_state;
} StateMachine_t;

/**
 * @brief sm_create creates a new state machine with a given name.
 * @param name the name of the state machine.
 * @return  a pointer to the newly created state machine.
 */
StateMachine_t* sm_create(char *name);

/**
 * @brief starts the state machine which will wait for new states to be queued in order to transit to them.
 * @param machine the machine to start.
 * @param init_state the initial state of the machine.
 */
void start_machine(StateMachine_t *machine, State_t *init_state);

/**
 * @brief sm_transit_to adds a new state to the states queue and notifies the state machine to run
 * the new state.
 * @param sm the state machine.
 * @param new_state the new state to transit to.
 */
void sm_transit_to(StateMachine_t *sm, State_t *new_state);

/**
 * @brief sm_send_state_message send a message to the state machine to be processed by the current
 * state.
 * @param sm the state machine.
 * @param state_message the message to send.
 */
void sm_send_state_message(StateMachine_t *sm, StateMessage_t *state_message);

/**
 * @brief sm_finish finish the state machine execution.
 * @param machine the machine to finish
 * @param wait 1 if we wish to wait until the machine has finished the execution.
 */
void sm_finish(StateMachine_t *machine, int wait);

/**
 * @brief sm_destroy release and free the state machine and its resources.
 * @param machine the state machine.
 */
void sm_destroy(StateMachine_t *machine);

#endif // STATEMACHINE_H
