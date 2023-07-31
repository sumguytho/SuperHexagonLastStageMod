#ifndef TOOLHELP_SNAPSHOT_HPP
#define TOOLHELP_SNAPSHOT_HPP

#include <functional>

extern "C" {
#include <Windows.h>
#include <TlHelp32.h>
}

#include "winapi_wrappers.hpp"
#include "process.hpp"
#include "common.hpp"

namespace wrappers{

struct ToolhelpSnapshot {
    ToolhelpSnapshot(DWORD dwFlags, DWORD th32ProcessID){
        _hSnap = wrappers::CreateToolhelp32Snapshot(dwFlags, th32ProcessID);
    }
    ToolhelpSnapshot(const ToolhelpSnapshot&) = delete;
    ToolhelpSnapshot &operator=(const ToolhelpSnapshot&) = delete;

    ToolhelpSnapshot(DWORD dwFlags, const wrappers::Process &process)
        : ToolhelpSnapshot(dwFlags, process.GetProcessId()) {}

    void VisitModules(const std::function<bool(MODULEENTRY32&)> &visitFn) {
        Visit(visitFn, Module32First, Module32Next);
    }
    ~ToolhelpSnapshot(){
        if (_hSnap != INVALID_HANDLE_VALUE){
            wrappers::CloseHandle(_hSnap);
        }
    }
private:
    template<typename EntryType, typename Callable>
    void Visit(
        const std::function<bool(EntryType&)> &visitFn, 
        Callable &&firstFn,
        common::identity_t<Callable> nextFn)
    {
        EntryType entry;
        entry.dwSize = sizeof(EntryType);
        if (!firstFn(_hSnap, &entry)) { return; }
        while( visitFn(entry) && nextFn( _hSnap, &entry ) ); 
    }
    HANDLE _hSnap = INVALID_HANDLE_VALUE;
};

}//wrappers

#endif //TOOLHELP_SNAPSHOT_HPP
