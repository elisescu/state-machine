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

//test functions
int test_1();
int test_2();

test_sm() {
   test_1();
   test_2();
   LOGV("\n\nDone!\n\n");
}


/************************* TEST 1 ******************************/
/* Have three states. Start with initial state s1 and then transit
 * to state s2 then to s3.
 * In s1 receive two messages with the content "*" and "#" and then
 * in s3 receive another message with content "-".
 */
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
    sm_transit_to(m, s3);
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
    char *expected_result = "1e1*1#1x2e2x3e3-3x";
    state_trace_length = 0;

    StateMachine_t *sm = sm_create("cool-state-machine");
    LOGV("\n\n TEST 1");

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
    sm_destroy(sm);
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


/************************* TEST 2 ******************************/
/*
 * Have 3 states: STOPPED, HANDLE and DONE. Initial state is STOPPED
 * from where we transit to HANDLE when we received message "START".
 * Run in HANDLE 10 times, after which go to DONE and then finish the
 * state machine.
 */
State_t *s_STOPPED, *s_HANDLE, *s_DONE;

/****************** STOPPED state ******************/
void s_stopped_enter(StateMachine_t *m, State_t *s) {
    state_trace[state_trace_length++] = s->state_name[0];
    sm_transit_to(m, s_HANDLE);
}

/****************** HANDLE state ******************/
void s_handle_enter(StateMachine_t *m, State_t *s) {
    static int counter = 1;
    if (counter++ < 10) {
        sm_transit_to(m, s_HANDLE);
    } else {
        sm_transit_to(m, s_DONE);
    }
    state_trace[state_trace_length++] = s->state_name[0];
}

/****************** DONE state ******************/
void s_done_enter(StateMachine_t *m, State_t *s) {
    state_trace[state_trace_length++] = s->state_name[0];
    sm_finish(m, 0);
}

int test_2() {
    State_t stopped_s = {s_stopped_enter, NULL, NULL, "STOPPED"};
    State_t handle_s = {s_handle_enter, NULL, NULL, "HANDLE"};
    State_t done_s = {s_done_enter, NULL, NULL, "DONE"};
    s_STOPPED = &stopped_s;
    s_HANDLE = &handle_s;
    s_DONE = &done_s;
    state_trace_length = 0;

    char *expected_result = "SHHHHHHHHHHD";

    StateMachine_t *sm = sm_create("cool-state-machine");

    // start the machine in STOPPED state
    LOGV("\n\n TEST 2");
    LOGV("Starting the state machine");
    start_machine(sm, s_STOPPED);


    LOGV("Sleeping for a while now.");
    sleep(2);
    LOGV("Slept 2 seconds.");


    sm_finish(sm, 1);
    sm_destroy(sm);
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
