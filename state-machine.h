#ifndef STATEMACHINE_H
#define STATEMACHINE_H

typedef int (*StateEnterFunction_T)(StateMachine_t *, State_t *, State_t *);
typedef int (*StateExitFunction_T)(StateMachine_t *, State_t *);
typedef int (*StateRunFunction_T)(StateMachine_t *, State_t *);

typedef struct StateMachine_s {
    State_t *next_state;
    State_t *curr_state;
    int no_states;
    int run;
    char *machine_name;
} StateMachine_t;

typedef struct State_s {
    StateEnterFunction_T enter;
    StateRunFunction_T run;
    StateExitFunction_T exit;
    char *state_name;
} State_t;


#endif // STATEMACHINE_H
