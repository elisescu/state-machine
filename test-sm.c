#include <stdio.h>
#include <string.h>

#include <state-machine.h>
#include <message-queue.h>

#if 1
#define LOGV(...) {printf("\n" __VA_ARGS__); fflush(stdout);}
#else
#define LOGV(...)
#endif

char state_trace[1000];
int state_trace_length = 0;
State_t *s1, *s2, *s3;

/****************** S1 state ******************/
int s1_enter(StateMachine_t *m, State_t *s) {
    state_trace[state_trace_length++]='e';
    return SM_OK;
}
int s1_run(StateMachine_t *m, State_t *s) {
    state_trace[state_trace_length++]=s->state_name[0];
    return SM_OK;
}
int s1_exit(StateMachine_t *m, State_t *s) {
    state_trace[state_trace_length++]='x';
    return SM_OK;
}

/****************** S2 state ******************/
int s2_enter(StateMachine_t *m, State_t *s) {
    state_trace[state_trace_length++]='e';
    return SM_OK;
}
int s2_run(StateMachine_t *m, State_t *s) {
    state_trace[state_trace_length++]=s->state_name[0];
    return SM_OK;
}
int s2_exit(StateMachine_t *m, State_t *s) {
    state_trace[state_trace_length++]='x';
    sm_transit_to(m, s3);
    return SM_OK;
}

/****************** S3 state ******************/
int s3_enter(StateMachine_t *m, State_t *s) {
    state_trace[state_trace_length++]='e';
    return SM_OK;
}
int s3_run(StateMachine_t *m, State_t *s) {
    state_trace[state_trace_length++]=s->state_name[0];
    return SM_OK;
}
int s3_exit(StateMachine_t *m, State_t *s) {
    state_trace[state_trace_length++]='x';
    return SM_EXIT;
}

int test_1() {
    State_t s1_s = {s1_enter, s1_run, s1_exit, "1"};
    State_t s2_s = {s2_enter, s2_run, s2_exit, "2"};
    State_t s3_s = {s3_enter, s3_run, s3_exit, "3"};
    s1 = &s1_s;
    s2 = &s2_s;
    s3 = &s3_s;
    char *expected_result = "e1xe2xe3x";

    StateMachine_t *sm = sm_create("cool-state-machine");

    LOGV("Starting the state machine");
    start_machine(sm);
    sm_transit_to(sm, &s1_s);
    sm_transit_to(sm, &s2_s);

    LOGV("Sleeping for a while now.");
    sleep(2);
    LOGV("Slept 2 seconds.");

    sm_finish(sm, 1);
    state_trace[state_trace_length] = '\0';
    if (strcmp(state_trace, expected_result) == 0) {
        LOGV("Test passed! --> OK");
        return 1;
    } else {
        LOGV("Test failed! --> ERROR");
        LOGV("result: %s", state_trace);
        LOGV("result: %s", expected_result);
    }
    return 0;
}

test_sm() {
   test_1();
   LOGV("\n\nDone!\n\n");
}

