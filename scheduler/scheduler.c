#include "scheduler.h"

#include <stdio.h>
#include <string.h>

static const char *k_module_name = "scheduler";
static const char *k_module_description = "Bolun Phone 1.0 scheduler module";

static int valid_cpu(BolunCpuFamily cpu) {
    return cpu == BOLUN_CPU_ARMV7 || cpu == BOLUN_CPU_ARM64 || cpu == BOLUN_CPU_X86 || cpu == BOLUN_CPU_X86_64;
}

int bolun_scheduler_init(BolunSCHEDULERState *state, BolunCpuFamily cpu) {
    if (state == NULL || !valid_cpu(cpu)) {
        return -1;
    }
    memset(state, 0, sizeof(*state));
    state->cpu = cpu;
    state->initialized = 1;
    state->generation = 1;
    snprintf(state->name, sizeof(state->name), "%s", k_module_name);
    return 0;
}

const char *bolun_scheduler_describe(void) {
    return k_module_description;
}

int bolun_scheduler_health(const BolunSCHEDULERState *state) {
    if (state == NULL || state->initialized == 0 || !valid_cpu(state->cpu)) {
        return -1;
    }
    return (int)state->generation;
}

int bolun_scheduler_route(BolunSCHEDULERState *state, const char *command) {
    if (bolun_scheduler_health(state) < 0 || command == NULL || command[0] == '\0') {
        return -1;
    }
    state->generation += 1;
    return (int)(state->generation + (uint32_t)strlen(command));
}
