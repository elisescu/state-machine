#include "state-machine.h"
#include <stdio.h>
#include <stdlib.h>

#if 0
#define LOGV(...) printf("\nstate_machine: " __VA_ARGS__)
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
    machine->init_state = NULL;
    return machine;
}

void sm_destroy(StateMachine_t *machine) {
    pthread_mutex_destroy(machine->machine_mutex);
    pthread_cond_destroy(machine->machine_cond);
    free(machine->machine_mutex);
    free(machine->machine_cond);
    mq_destroy(machine->machine_queue);
    pthread_join(*machine->machine_thread, NULL);
    free(machine->machine_thread);
    free(machine);
}

void* run_machine(void *data) {
    StateMachine_t *machine = (StateMachine_t *) data;
    State_t *curr_state = machine->init_state;
    char *state_name = (curr_state->state_name) ? curr_state->state_name : "NULL";
    machine->machine_finished = 0;

    // ENTER the initial state
    if (curr_state->enter) {
        LOGV(" entering to state %s", state_name);
        curr_state->enter(machine, curr_state);
    }

    // main machine loop
    LOGV(" starting the machine main loop");
    while (1) {
        Message_t *msg = mq_dequeue_message_wait(machine->machine_queue);
        if (msg == NULL || msg->data == NULL) {
            LOGV(" Empty message on the machine queue");
            continue;
        }
        SMachineMessage_t *sm_msg = (SMachineMessage_t*) msg->data;
        LOGV("Dequeued message %d ", sm_msg->message_type);
        switch (sm_msg->message_type) {
        case MSG_STATE_TRANSIT: {
            State_t *next_state = (State_t*) sm_msg->data;
            state_name = (curr_state->state_name) ? curr_state->state_name : "NULL";
            char *next_state_name = (next_state->state_name) ? next_state->state_name : "NULL";

            LOGV(" got MSG_STATE_TRANSIT. Next state %s", next_state_name);

            // EXIT callback. Exit from the previous state
            if (curr_state->exit) {
                LOGV(" exiting state %s", state_name);
                curr_state->exit(machine, curr_state);
            }

            // ENTER callback. Enter the new state
            curr_state = next_state;
            state_name = next_state_name;
            if (curr_state->enter) {
                LOGV(" entering to state %s", state_name);
                curr_state->enter(machine, curr_state);
            }
            break;
        }
        case MSG_STATE_MESSAGE: {
            StateMessage_t *s_msg = (StateMessage_t *)sm_msg->data;
            LOGV(" got MSG_STATE_MESSAGE.");
            if (curr_state->processMessage) {
                LOGV(" delivering message %d to state %s", s_msg->message_type, state_name);
                curr_state->processMessage(machine, curr_state, s_msg);
            } else {
                LOGV(" state %s cannot process any message.", state_name);
            }
            break;
        }
        case MSG_MACHINE_EXIT: {
            state_name = (curr_state->state_name) ? curr_state->state_name : "NULL";
            LOGV(" got MSG_MACHINE_EXIT. Machine current state %s", state_name);

            // EXIT callback. Exit from the previous state
            if (curr_state->exit) {
                LOGV(" exiting state %s", state_name);
                curr_state->exit(machine, curr_state);
            }
            free(msg);
            free(sm_msg);
            goto machine_done;
        }
        }
        free(msg);
        free(sm_msg);
    }

    //machine done
    machine_done:
    pthread_mutex_lock(machine->machine_mutex);
    pthread_cond_signal(machine->machine_cond);
    machine->machine_finished = 1;
    pthread_mutex_unlock(machine->machine_mutex);
    LOGV(" main loop of the machine finished.");
    return NULL;
}

void start_machine(StateMachine_t *machine, State_t *init_state) {
    machine->init_state = init_state;
    if (machine->init_state == NULL)  {
        LOGV(" Machine doesn't have a initial state, cannot start");
    } else {
        pthread_create(machine->machine_thread, NULL, run_machine, (void*)machine);
    }
}

void sm_send_state_message(StateMachine_t *sm, StateMessage_t *state_message) {
    SMachineMessage_t *m_msg = (SMachineMessage_t *) malloc(sizeof(SMachineMessage_t));
    m_msg->message_type = MSG_STATE_MESSAGE;
    m_msg->data = (void*) state_message;
    Message_t *msg = mq_message_create();
    msg->data = (void*) m_msg;
    mq_queue_message(sm->machine_queue, msg);
}

void sm_transit_to(StateMachine_t *sm, State_t *new_state) {
    SMachineMessage_t *m_msg = (SMachineMessage_t *) malloc(sizeof(SMachineMessage_t));
    m_msg->message_type = MSG_STATE_TRANSIT;
    m_msg->data = (void*) new_state;
    Message_t *msg = mq_message_create();
    msg->data = (void*) m_msg;
    mq_queue_message(sm->machine_queue, msg);
}

void sm_finish(StateMachine_t *machine, int wait) {
    LOGV("Finishing the state machine.");
    if (!machine->machine_finished) {
        SMachineMessage_t *m_msg = (SMachineMessage_t *) malloc(sizeof(SMachineMessage_t));
        m_msg->message_type = MSG_MACHINE_EXIT;
        Message_t *msg = mq_message_create();
        msg->data = (void*) m_msg;
        mq_queue_message(machine->machine_queue, msg);

        if (wait) {
            LOGV("Waiting for the machine to finish");
            pthread_mutex_lock(machine->machine_mutex);
            if (!machine->machine_finished) {
                pthread_cond_wait(machine->machine_cond, machine->machine_mutex);
            }
            pthread_mutex_unlock(machine->machine_mutex);
            LOGV("Machine finshed execution!");
        }
    }
}
