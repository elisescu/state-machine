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
void s1_enter(StateMachine_t *m, State_t *s) {
    state_trace[state_trace_length++] = s->state_name[0];
    state_trace[state_trace_length++] = 'e';
}
void s1_process_message(StateMachine_t *m, State_t *s, StateMessage_t *msg) {
    char *string_msg = msg->obj;
    state_trace[state_trace_length++] = s->state_name[0];
    state_trace[state_trace_length++] = string_msg[0];
}
void s1_exit(StateMachine_t *m, State_t *s) {
    state_trace[state_trace_length++] = s->state_name[0];
    state_trace[state_trace_length++] = 'x';
}

/****************** S2 state ******************/
void s2_enter(StateMachine_t *m, State_t *s) {
    static int counter = 0;
    state_trace[state_trace_length++] = s->state_name[0];
    state_trace[state_trace_length++] = 'e';
    if (counter++ < 3) {
        sm_transit_to(m, s2);
    } else {
        sm_transit_to(m, s3);
    }
}
void s2_process_message(StateMachine_t *m, State_t *s, StateMessage_t *msg) {
    char *string_msg = msg->obj;
    state_trace[state_trace_length++] = s->state_name[0];
    state_trace[state_trace_length++] = string_msg[0];
}
void s2_exit(StateMachine_t *m, State_t *s) {
    state_trace[state_trace_length++] = s->state_name[0];
    state_trace[state_trace_length++] = 'x';
}

/****************** S3 state ******************/
void s3_enter(StateMachine_t *m, State_t *s) {
    state_trace[state_trace_length++] = s->state_name[0];
    state_trace[state_trace_length++] = 'e';
}
void s3_process_message(StateMachine_t *m, State_t *s, StateMessage_t *msg) {
    char *string_msg = msg->obj;
    state_trace[state_trace_length++] = s->state_name[0];
    state_trace[state_trace_length++] = string_msg[0];
}
void s3_exit(StateMachine_t *m, State_t *s) {
    state_trace[state_trace_length++] = s->state_name[0];
    state_trace[state_trace_length++] = 'x';
}

int test_1() {
    State_t s1_s = {s1_enter, s1_process_message, s1_exit, "1"};
    State_t s2_s = {s2_enter, s2_process_message, s2_exit, "2"};
    State_t s3_s = {s3_enter, s3_process_message, s3_exit, "3"};
    s1 = &s1_s;
    s2 = &s2_s;
    s3 = &s3_s;
    char *expected_result = "1e1*1#1x2e2x2e2x2e2x2e2x3e3-3x";

    StateMachine_t *sm = sm_create("cool-state-machine");

    // start the machine in STATE s1
    LOGV("Starting the state machine");
    start_machine(sm, s1);


    // sending 2 messages to the machine
    StateMessage_t msg;
    msg.message_type = 3;
    msg.obj = (void*)"*";
    sm_send_state_message(sm, &msg);
    StateMessage_t msg2;
    msg2.message_type = 3;
    msg2.obj = (void*)"#";
    sm_send_state_message(sm, &msg2);

    // transit to STATE s2
    sm_transit_to(sm, s2);

    LOGV("Sleeping for a while now.");
    sleep(2);
    LOGV("Slept 2 seconds.");

    // here we should already be in STATE s3
    // send another message to the machine
    StateMessage_t msg3;
    msg3.message_type = 3;
    msg3.obj = (void *) "-";
    sm_send_state_message(sm, &msg3);


    sm_finish(sm, 1);
    state_trace[state_trace_length] = '\0';
    if (strcmp(state_trace, expected_result) == 0) {
        LOGV("Test passed! --> OK");
        LOGV("State machine trace: %s ", state_trace);
        return 1;
    } else {
        LOGV("Test failed! --> ERROR");
        LOGV("actual result:   %s", state_trace);
        LOGV("expected result: %s", expected_result);
    }
    return 0;
}

test_sm() {
   test_1();
   LOGV("\n\nDone!\n\n");
}

