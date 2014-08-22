#include "state-machine.h"

#if 1
#define LOGV(...) printf(__VA_ARGS__)
#endif

int run_machine(StateMachine_t *machine) {
    while (machine->run) {
        if (machine->curr_state && machine->curr_state->enter) {
                    if (machine->curr_state->enter(machine, machine->curr_state, machine->next_state) == 0) break;
                    machine->curr_state = machine->next_state;
        }

        if (machine->curr_state && machine->curr_state->run) {
                    if (machine->curr_state->run(machine, machine->curr_state) == 0) break;
        }

        if (machine->curr_state && machine->curr_state->exit) {
                    if (machine->curr_state->exit(machine, machine->curr_state) == 0) break;
        }
    }
}
