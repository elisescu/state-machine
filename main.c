#include <stdio.h>
#include <state-machine.h>
#include <message-queue.h>

#define LOGV(...) {printf("\n" __VA_ARGS__); fflush(stdout);}

State_t *stopped, *starting, *started;
/****************** STOPPED state ******************/
int stopped_run(StateMachine_t *m, State_t *s) {
    LOGV(" RUNNING in %s state", s->state_name);
    sm_transit_to(m, starting);
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
    sm_transit_to(m, NULL);
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

    StateMachine_t *sm = sm_create("cool-state-machine");
    sm_transit_to(sm, starting);
    sm_transit_to(sm, stopped);

    LOGV("Starting the state machine");
    start_machine(sm);

    LOGV("Sleeping for a while now");
    sleep(2);
    LOGV("Slept 2 seconds. Transiting to started state");
    sm_transit_to(sm, starting);
    sm_transit_to(sm, started);

    sm_finish(sm, 1);
    //mq_test();
    return 0;
}
