#include <stdio.h>
#include <state-machine.h>
#include <message-queue.h>

#define LOGV(...) printf("\n" __VA_ARGS__)

State_t *stopped, *starting, *started;
/****************** STOPPED state ******************/
int stopped_run(StateMachine_t *m, State_t *s) {
    LOGV(" RUNNING in %s state", s->state_name);
    m->next_state = starting;
    return SM_OK;
}
int stopped_enter(StateMachine_t *m, State_t *s) {
    LOGV(" ENTERED in %s state", s->state_name);
    return SM_OK;
}
int stopped_exit(StateMachine_t *m, State_t *s) {
    LOGV(" EXITED in %s state", s->state_name);
    return SM_OK;
}

/****************** STARTING state ******************/
int starting_run(StateMachine_t *m, State_t *s) {
    LOGV(" RUNNING in %s state", s->state_name);
    m->next_state = started;
    return SM_OK;
}
int starting_enter(StateMachine_t *m, State_t *s) {
    LOGV(" ENTERED in %s state", s->state_name);
    return SM_OK;
}
int starting_exit(StateMachine_t *m, State_t *s) {
    LOGV(" EXITED in %s state", s->state_name);
    return SM_OK;
}

/****************** STARTED state ******************/
int started_run(StateMachine_t *m, State_t *s) {
    LOGV(" RUNNING in %s state", s->state_name);
    m->next_state = NULL;
    return SM_OK;
}
int started_enter(StateMachine_t *m, State_t *s) {
    LOGV(" ENTERED in %s state", s->state_name);
    return SM_OK;
}
int started_exit(StateMachine_t *m, State_t *s) {
    LOGV(" EXITED in %s state", s->state_name);
    return SM_EXIT;
}

int main(void)
{
    State_t stopped_s = {stopped_enter, stopped_run, stopped_exit, "STOPPED"};
    State_t starting_s = {starting_enter, starting_run, starting_exit, "STARTING"};
    State_t started_s = {started_enter, started_run, started_exit, "STARTED"};
    stopped = &stopped_s;
    started = &started_s;
    starting = &starting_s;

    StateMachine_t sm;
    sm.curr_state = NULL;
    sm.next_state = stopped;
    sm.run = 1;

#if 0
    LOGV("Starting the machine..\n");
    run_machine(&sm);
    LOGV("Machine finised..\n");
#endif

    mq_test();
    return 0;
}
