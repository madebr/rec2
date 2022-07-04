#include <windows.h>
#include <detours.h>
#include <algorithm>
#include <filesystem>
#include <functional>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

int CDECL main(int argc, char *argv[])
{
    std::vector<std::string> pluginPaths;
    std::vector<std::string> victimArguments;
    std::string victimPath;
    bool victimPathInitialized = false;
    bool seenDoubleDash = false;
    const char *workingPath = NULL;
    for (int i = 1; i < argc;) {
        int consumed = -1;
        bool maybeOptional = !seenDoubleDash;
        if (maybeOptional && argv[i][0] == '-') {
            if (strcmp(argv[i], "--") == 0) {
                seenDoubleDash = true;
                consumed = 1;
            } else if (strcmp(argv[i], "--inject") == 0) {
                if (i + 1 < argc) {
                    pluginPaths.push_back(argv[i + 1]);
                    consumed = 2;
                } else {
                    std::cerr << "--inject is missing an argument\n";
                    return 1;
                }
            } else if (strcmp(argv[i], "--cwd") == 0) {
                if (workingPath != NULL) {
                    consumed = -1;
                    std::cerr << "--cwd can only be used once\n";
                } else if (i + 1 < argc) {
                    workingPath = argv[i + 1];
                    consumed = 2;
                } else {
                    std::cerr << "--working-path is missing an argument\n";
                    return 1;
                }
            }
        } else {
            if (!victimPathInitialized) {
                victimPath = argv[i];
                victimPathInitialized = true;
                consumed = 1;
            } else {
                victimArguments.push_back(argv[i]);
                consumed = 1;
            }
        }
        if (consumed <= 0) {
            std::cerr << "Invalid argument '" << argv[i] << "'\n";
            return 1;
        }
        i += consumed;
    }
    char exePath[2048];
    DWORD nLen = GetModuleFileNameA(nullptr, exePath, sizeof(exePath));
    if (nLen == sizeof(exePath)) {
        return 1;
    }
    exePath[nLen] = '\0';
    auto workPathDir = fs::path(exePath).parent_path();
    const auto pluginsDirPath = workPathDir / "plugins";

    if (!fs::is_regular_file(victimPath)) {
        printf("Victim '%s' does not exist.\n", victimPath.c_str());
        return 1;
    }

    //////////////////////////////////////////////////////////////////////////
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    std::string command;

    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));
    si.cb = sizeof(si);

    command = victimPath;
    for (size_t i = 0; i < victimArguments.size(); i++) {
        command += " ";
        command += victimArguments[i];
    }
    char* commandBuffer = new char[command.size() + 1];
    command.copy(commandBuffer, command.size());
    commandBuffer[command.size()] = '\0';

    std::vector<const char*> pluginPathsCStrs;
    pluginPathsCStrs.resize(pluginPaths.size());
    std::transform(pluginPaths.begin(), pluginPaths.end(), pluginPathsCStrs.begin(), [](const auto &str) { return str.data(); });

    printf("%s: Starting: `%s'\n", argv[0], command.c_str());
    for (const auto& pluginPath : pluginPaths) {
        printf("%s:   with `%s'\n", argv[0], pluginPath.c_str());
    }
    fflush(stdout);

    DWORD dwFlags = CREATE_DEFAULT_ERROR_MODE | CREATE_SUSPENDED;

    SetLastError(0);
    if (!DetourCreateProcessWithDllsA(
            victimPath.c_str(),
            commandBuffer,
            nullptr,
            nullptr,
            TRUE,
            dwFlags,
            nullptr,
            workingPath,
            &si,
            &pi,
            pluginPathsCStrs.size(),
            pluginPathsCStrs.data(),
            nullptr)) {
        DWORD dwError = GetLastError();
        printf("%s : DetourCreateProcessWithDllEx failed: %ld\n", argv[0], dwError);
        printf("ERROR");
        ExitProcess(9009);
    }

    ResumeThread(pi.hThread);

    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD dwResult = 0;
    if (!GetExitCodeProcess(pi.hProcess, &dwResult)) {
        printf("%s : GetExitCodeProcess failed: %ld\n", argv[0], GetLastError());
    }

    return dwResult;
}
