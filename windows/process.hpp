#ifndef PROCESS_HPP
#define PROCESS_HPP


#include <iostream>

#include "winapi_wrappers.hpp"

extern "C" {
#include <Windows.h>
}


namespace wrappers {

struct Process {
    Process(
        LPCSTR                lpApplicationName,
        LPSTR                 lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes,
        BOOL                  bInheritHandles,
        DWORD                 dwCreationFlags,
        LPVOID                lpEnvironment,
        LPCSTR                lpCurrentDirectory,
        LPSTARTUPINFOA        lpStartupInfo)
    {
        wrappers::CreateProcessA(
            lpApplicationName,
            lpCommandLine,
            lpProcessAttributes,
            lpThreadAttributes,
            bInheritHandles,
            dwCreationFlags,
            lpEnvironment,
            lpCurrentDirectory,
            lpStartupInfo,
            &_pi
        );
    }
    Process(const Process&) = delete;
    Process &operator=(const Process&) = delete;
    // the warning is false positive but i will oblige
    Process(Process &&other) : _pi(std::exchange( other._pi, { INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, 0, 0 } )) { }
    Process() { }

    HANDLE GetProcessHandle() const {
        return _pi.hProcess;
    }
    DWORD GetProcessId() const {
        return _pi.dwProcessId;
    }
    HANDLE GetMainThreadHandle() const {
        return _pi.hThread;
    }
    DWORD GetMainThreadId() const {
        return _pi.dwThreadId;
    }
    DWORD WaitForInputIdle(DWORD dwMilliseconds = INFINITE) {
        return wrappers::WaitForInputIdle(GetProcessHandle(), dwMilliseconds);
    }
    DWORD SuspendMainThread(){
        return wrappers::SuspendThread(GetMainThreadHandle());
    }
    DWORD ResumeMainThread(){
        return wrappers::ResumeThread(GetMainThreadHandle());
    }
    BOOL WriteProcessMemory(LPVOID lpBaseAddress, LPCVOID lpBuffer, SIZE_T nSize, SIZE_T *lpNumberOfBytesWritten){
        return wrappers::WriteProcessMemory(GetProcessHandle(), lpBaseAddress, lpBuffer, nSize, lpNumberOfBytesWritten);
    }
    BOOL ReadProcessMemory(LPCVOID lpBaseAddress, LPVOID lpBuffer, SIZE_T nSize, SIZE_T *lpNumberOfBytesWritten){
        return wrappers::ReadProcessMemory(GetProcessHandle(), lpBaseAddress, lpBuffer, nSize, lpNumberOfBytesWritten);
    }
    BOOL VirtualProtectEx(LPVOID lpAddress, SIZE_T dwSize, DWORD flNewProtect, PDWORD lpflOldProtect){
        return wrappers::VirtualProtectEx(GetProcessHandle(), lpAddress, dwSize, flNewProtect, lpflOldProtect);
    }
    void WriteProcessROMemory(LPVOID lpBaseAddress, LPCVOID lpBuffer, SIZE_T nSize, SIZE_T *lpNumberOfBytesWritten){ 
        DWORD protPrev = 0;
        DWORD protNew = PAGE_READWRITE;
        VirtualProtectEx(lpBaseAddress, nSize, protNew, &protPrev);
        WriteProcessMemory(lpBaseAddress, lpBuffer, nSize, lpNumberOfBytesWritten);
        VirtualProtectEx(lpBaseAddress, nSize, protPrev, &protNew);
    }
    BOOL TerminateProcess(UINT uExitCode = EXIT_SUCCESS) {
        return wrappers::TerminateProcess(GetProcessHandle(), uExitCode);
    }
    ~Process(){
        if (_pi.hThread != INVALID_HANDLE_VALUE){
            wrappers::CloseHandle(_pi.hThread);
        }
        if (_pi.hProcess != INVALID_HANDLE_VALUE){
            wrappers::CloseHandle(_pi.hProcess);
        }
    }
private:
    PROCESS_INFORMATION _pi { INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, 0, 0 };
};

}//wrappers

#endif //PROCESS_HPP
