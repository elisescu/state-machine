#include "state-machine.h"
#include <stdio.h>
#include <stdlib.h>

#if 1
#define LOGV(...) printf("\n" __VA_ARGS__)
#else
#define LOGV(...)
#endif

StateMachine_t* sm_create(char *name) {
    StateMachine_t *machine = (StateMachine_t*) malloc(sizeof(StateMachine_t));
    machine->machine_name = name;
    machine->machine_thread = (pthread_t *) malloc(sizeof(pthread_t));
    machine->machine_queue = mq_create();
    machine->machine_mutex = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(machine->machine_mutex, NULL);
    machine->machine_cond = (pthread_cond_t*) malloc(sizeof(pthread_cond_t));
    pthread_cond_init(machine->machine_cond, NULL);
    machine->machine_running = 0;
    return machine;
}

void* run_machine(void *data) {
    StateMachine_t *machine = (StateMachine_t *) data;
    while (1) {
        pthread_mutex_lock(machine->machine_mutex);
        int running = machine->machine_running;
        pthread_mutex_unlock(machine->machine_mutex);
        if (!running) {
            LOGV(" machine running flag set to 0");
            break;
        }
        Message_t *msg = mq_dequeue_message_wait(machine->machine_queue);
        if (msg == NULL || msg->data == NULL) {
            LOGV(" no state left to run for the machine");
            break;
        }
        State_t *next_state = (State_t*) msg->data;
        free(msg);
        char *state_name = (next_state->state_name) ? next_state->state_name : "NULL";
        if (next_state->enter) {
            LOGV(" entering to state %s", state_name);
            if (next_state->enter(machine, next_state) == SM_EXIT) {
                LOGV(" machine exit normally after entering to the %s state", state_name);
                break;
            }
        }

        if (next_state->run) {
            LOGV(" running state %s", state_name);
            if (next_state->run(machine, next_state) == SM_EXIT) {
                LOGV(" machine exit normally after running the %s state", state_name);
                break;
            }
        } else {
            LOGV(" exiting state machine %s from state %s", machine->machine_name, state_name);
            break;
        }

        if (next_state->exit) {
            LOGV(" exiting state %s", state_name);
            if (next_state->exit(machine, next_state) == SM_EXIT) {
                LOGV(" machine exit normally after exiting from the %s state", state_name);
                break;
            }
        }
    }
    pthread_mutex_lock(machine->machine_mutex);
    pthread_cond_signal(machine->machine_cond);
    machine->machine_finished = 1;
    pthread_mutex_unlock(machine->machine_mutex);
    LOGV(" main loop of the machine finished.");
    return NULL;
}

void start_machine(StateMachine_t *machine) {
    machine->machine_running = 1;
    pthread_create(machine->machine_thread, NULL, run_machine, (void*)machine);
}

void sm_transit_to(StateMachine_t *sm, State_t *new_state) {
    Message_t *msg = mq_message_create();
    msg->data = (void*) new_state;
    mq_queue_message(sm->machine_queue, msg);
}

void sm_finish(StateMachine_t *machine, int wait) {
    LOGV("Finishing the state machine.");
    pthread_mutex_lock(machine->machine_mutex);
    machine->machine_running = 0;
    if (wait) {
        LOGV("Waiting for the machine to finish");
        mq_signal(machine->machine_queue);
        if (!machine->machine_finished) {
            pthread_cond_wait(machine->machine_cond, machine->machine_mutex);
        }
        LOGV("Machine finshed execution!");
    }
    pthread_mutex_unlock(machine->machine_mutex);
}
