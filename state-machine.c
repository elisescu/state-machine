#include "state-machine.h"
#include <stdio.h>

#if 0
#define LOGV(...) printf("\n" __VA_ARGS__)
#else
#define LOGV(...)
#endif

int run_machine(StateMachine_t *machine) {
    while (1) {
        State_t *next_state = machine->next_state;
        if (machine->next_state && machine->next_state->enter) {
            LOGV(" entering to state %s", (machine->next_state->state_name) ? machine->next_state->state_name : "NULL");
            if (machine->next_state->enter(machine, machine->next_state) == SM_EXIT) break;
        }

        machine->curr_state = machine->next_state;
        if (machine->curr_state && machine->curr_state->run) {
            LOGV(" running state %s", (machine->curr_state && machine->curr_state->state_name) ? machine->curr_state->state_name : "NULL");
            if (machine->curr_state->run(machine, machine->curr_state) == SM_EXIT) break;
        } else {
            /* no state left to run
             */
            return SM_EXIT;
        }

        if (next_state == machine->next_state) {
            LOGV("Looping in the same state (%s) ?", machine->next_state->state_name);
            break;
        }

        if (machine->curr_state && machine->curr_state->exit) {
            LOGV(" exiting state %s", (machine->curr_state && machine->curr_state->state_name) ? machine->curr_state->state_name : "NULL");
            if (machine->curr_state->exit(machine, machine->curr_state) == SM_EXIT) break;
        }
    }

    return SM_OK;
}
