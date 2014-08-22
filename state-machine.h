#ifndef STATEMACHINE_H
#define STATEMACHINE_H

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
    StateEnterFunction_T enter;
    StateRunFunction_T run;
    StateExitFunction_T exit;
    char *state_name;
} State_t;

typedef struct StateMachine_s {
    /**
     * @brief next_state points to the next state of the machine. This should be set in the
     * run function of the current state.
     */

    State_t *next_state;
    /**
     * @brief curr_state is the current state of the machine.
     */
    State_t *curr_state;
    int no_states;
    int run;
    char *machine_name;
} StateMachine_t;


int run_machine(StateMachine_t *machine);

#endif // STATEMACHINE_H
