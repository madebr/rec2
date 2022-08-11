#include "hooks.h"

#include <windows.h>
#include <detours.h>

#include <cstdio>
#include <cstdlib>
#include <functional>
#include <unordered_map>
#include <vector>

extern "C" {

typedef struct hook_function_information {
    void** victim;
    void* original_victim;
    void* detour;
    std::function<void(void)> callback;
} hook_function_information;

static std::vector<std::function<void(void)>> hook_startups;
static std::vector<hook_function_information> hook_function_startups;
static std::vector<hook_function_information> hook_function_shutdowns;

void hook_register(void (*function)(void)) {
    hook_startups.push_back(function);
}

void hook_function_register(void **victim, void *detour) {
    hook_function_startups.push_back({
        victim,
        *victim,
        detour,
        [=]() { DetourAttach(victim, detour); },
    });
}

void hook_function_deregister(void **victim, void *detour) {
    hook_function_shutdowns.push_back({
        victim,
        *victim,
        detour,
        [=]() { DetourDetach(victim, detour); },
    });
}

void hook_run_functions(void) {
    for (const auto &hook_startup : hook_startups) {
        hook_startup();
    }
}

void hook_apply_all(void) {
    for (const auto &info : hook_function_startups) {
        info.callback();
    }
}

void hook_check(void) {
    bool problem = false;
    std::unordered_map<uintptr_t, const hook_function_information*> detected_original_victims;
    for (const auto &info : hook_function_startups) {
        auto already_in = detected_original_victims.find((uintptr_t)info.original_victim);
        if (already_in != detected_original_victims.end()) {
            fprintf(stderr, "Already found a function at 0x%p.\n", info.original_victim);
            problem = true;
        }
        detected_original_victims[(uintptr_t)info.original_victim] = &info;
    }
    if (problem) {
        abort();
    }
}

void hook_unapply_all(void) {
    for (const auto &info : hook_function_shutdowns) {
        info.callback();
    }
}

void hook_print_stats(void) {
    printf("===================================================\n");
    printf("Plugin name: %s\n", PLUGIN_NAME);
    printf("Hook count: %d\n", (int)hook_startups.size());
    printf("Function hook startup count: %d\n", (int)hook_function_startups.size());
    printf("Function hook startup count: %d\n", (int)hook_function_shutdowns.size());
    printf("===================================================\n");
}

}
