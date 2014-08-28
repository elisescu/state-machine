#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include "message-queue.h"
#include <pthread.h>

enum {
    SM_OK = 0,
    SM_EXIT = 1,
    SM_ERROR = 2
};

struct StateMachine_s;
struct State_s;

typedef int (*StateEnterFunction_T)(struct StateMachine_s *, struct State_s *);
typedef int (*StateExitFunction_T)(struct StateMachine_s *, struct State_s *);
typedef int (*StateRunFunction_T)(struct StateMachine_s *, struct State_s *);

typedef struct State_s {
    /**
     * @brief enter is the function to execute when entering to the state. Can be NULL.
     * Return SM_EXIT to stop the state machine.
     */
    StateEnterFunction_T enter;
    /**
     * @brief run is the state function to be executed. Cannot be NULL. If NULL the state machine will stop.
     * Return SM_EXIT to stop the state machine.
     */
    StateRunFunction_T run;
    /**
     * @brief exit is the function to execute when exiting from the current state. Can be NULL.
     * Return SM_EXIT to stop the state machine.
     */
    StateExitFunction_T exit;
    char *state_name;
} State_t;

typedef struct StateMachine_s {
    char *machine_name;
    MessageQueue_t *machine_queue;
    pthread_t *machine_thread;
    int machine_running;
    pthread_cond_t *machine_cond;
    pthread_mutex_t *machine_mutex;
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
 */
void start_machine(StateMachine_t *machine);

/**
 * @brief sm_transit_to adds a new state to the states queue and notifies the state machine to run
 * the new state.
 * @param sm the state machine.
 * @param new_state the new state to transit to.
 */
void sm_transit_to(StateMachine_t *sm, State_t *new_state);

/**
 * @brief sm_finish finish the state machine execution.
 * @param machine the machine to finish
 * @param wait 1 if we wish to wait until the machine has finished the execution.
 */
void sm_finish(StateMachine_t *machine, int wait);

#endif // STATEMACHINE_H
