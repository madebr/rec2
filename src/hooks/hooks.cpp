#include "hooks.h"

#include <windows.h>
#include <detours.h>

#include <cstdio>
#include <functional>
#include <vector>

extern "C" {

static std::vector<std::function<void(void)>> hook_startup_functions;
static std::vector<std::function<void(void)>> hook_shutdown_functions;

void hook_function_register(void **victim, void *detour) {
    hook_startup_functions.push_back([=]() {
        DetourAttach(victim, detour);
    });
}

void hook_function_deregister(void **victim, void *detour) {
    hook_shutdown_functions.push_back([&]() {
        DetourDetach(victim, detour);
    });
}

void hook_apply_all(void) {
    for (const auto &func : hook_startup_functions) {
        func();
    }
}

void hook_unapply_all(void) {
    for (const auto &func : hook_shutdown_functions) {
        func();
    }
}

void hook_print_stats(void) {
    printf("===================================================");
    printf("Plugin name: %s", PLUGIN_NAME);
    printf("Startup count: %d", (int)hook_startup_functions.size());
    printf("Shutdown count: %d", (int)hook_shutdown_functions.size());
    printf("===================================================");
}

}
