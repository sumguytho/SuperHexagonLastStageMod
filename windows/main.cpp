#include <iostream>
#include <chrono>
#include <thread>
#include <optional>

extern "C" {
#include <Windows.h>
#include <TlHelp32.h>
#include <tchar.h>
}

#include "superhexmod.hpp"
#include "callresult.hpp"
#include "program_modification.hpp"
#include "toolhelp_snapshot.hpp"
#include "process.hpp"


int main(int, char**){
    STARTUPINFO si{};
    char appName[] = "SuperHexagon.exe";

    std::optional<wrappers::Process> process;
    try {
        // cppreference says assignment to optional is construct + move construct
        // but for me it's just a compilation error
        process.emplace(
            nullptr,
            appName,
            nullptr,
            nullptr,
            FALSE,
            DETACHED_PROCESS | CREATE_NEW_PROCESS_GROUP,
            nullptr,
            nullptr,
            &si);

        // creating it in suspended state is no use, it won't be loaded yet
        // which will just timeout the wait call
        // the code i'm modifying isn't executed in main menu so there is
        // nothing to worry about anyway
        process->WaitForInputIdle(5000);
        process->SuspendMainThread();

        wrappers::ToolhelpSnapshot snapshot(TH32CS_SNAPMODULE, *process);
        DWORD imageBase = 0;
        snapshot.VisitModules([&](MODULEENTRY32 &module){
            if (!stricmp(appName, module.szModule)){
                imageBase = common::bit_cast<DWORD>(module.modBaseAddr);
                return false;
            }
            return true;
        });

        auto mod = superhexmod::steam_windows32_8838351();
        common::wrfn wrdatafn = [&](common::addr_t address, common::caddr_t buffer, std::size_t count){
            process->WriteProcessROMemory(address, buffer, count, nullptr);
        };
        common::wrfn wrcodefn = [&](common::addr_t address, common::caddr_t buffer, std::size_t count){
            process->WriteProcessMemory(address, buffer, count, nullptr);
        };
        mod.relocate(common::make_addr(imageBase));
        mod.test([&](common::caddr_t address, common::addr_t buffer, std::size_t count){
            process->ReadProcessMemory(address, buffer, count, nullptr);
        });
        mod.apply(wrdatafn, wrcodefn);

        process->ResumeMainThread();
        std::cout << "the mod has been applied successfully\n";
    }
    catch (std::exception &ex){
        std::cerr << "caught exception while applying the mod: " << ex.what() << '\n';
        if (process){
            process->TerminateProcess(EXIT_FAILURE);
        }
        return EXIT_FAILURE;
    }
}
