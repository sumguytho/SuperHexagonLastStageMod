#ifndef WINAPI_WRAPPERS_HPP
#define WINAPI_WRAPPERS_HPP

extern "C" {
#include <Windows.h>
#include <TlHelp32.h>
}

#include "callresult.hpp"


namespace wrappers {

BOOL CreateProcessA(
    LPCSTR                lpApplicationName,
    LPSTR                 lpCommandLine,
    LPSECURITY_ATTRIBUTES lpProcessAttributes,
    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    BOOL                  bInheritHandles,
    DWORD                 dwCreationFlags,
    LPVOID                lpEnvironment,
    LPCSTR                lpCurrentDirectory,
    LPSTARTUPINFOA        lpStartupInfo,
    LPPROCESS_INFORMATION lpProcessInformation
    )
{
    return callresult::CheckResultNeq( ::CreateProcessA(
        lpApplicationName,
        lpCommandLine,
        lpProcessAttributes,
        lpThreadAttributes,
        bInheritHandles,
        dwCreationFlags,
        lpEnvironment,
        lpCurrentDirectory,
        lpStartupInfo,
        lpProcessInformation), "CreateProcessA" );
}


DWORD WaitForInputIdle(HANDLE hProcess, DWORD dwMilliseconds = INFINITE) {
    return callresult::CheckValuesEqAnyOf(::WaitForInputIdle(hProcess, dwMilliseconds), "WaitForInputIdle", 0, WAIT_TIMEOUT);
}

BOOL CloseHandle(HANDLE hObject) {
    return callresult::CheckResultNeq(::CloseHandle(hObject), "CloseHandle");
}

DWORD SuspendThread(HANDLE hThread) {
    return callresult::CheckResultGeq(::SuspendThread(hThread), "SuspendThread");
}

DWORD ResumeThread(HANDLE hThread) {
    return callresult::CheckResultGeq(::ResumeThread(hThread), "ResumeThread");
}

HANDLE CreateToolhelp32Snapshot(DWORD dwFlags, DWORD th32ProcessID) {
    return callresult::CheckHandleResult(::CreateToolhelp32Snapshot(dwFlags, th32ProcessID), "CreateToolhelp32Snapshot");
}

BOOL WriteProcessMemory(HANDLE hProcess, LPVOID lpBaseAddress, LPCVOID lpBuffer, SIZE_T nSize, SIZE_T *lpNumberOfBytesWritten) {
    return callresult::CheckResultNeq(::WriteProcessMemory(hProcess, lpBaseAddress, lpBuffer, nSize, lpNumberOfBytesWritten), "WriteProcessMemory");
}

BOOL ReadProcessMemory(HANDLE hProcess, LPCVOID lpBaseAddress, LPVOID lpBuffer, SIZE_T nSize, SIZE_T *lpNumberOfBytesWritten) {
    return callresult::CheckResultNeq(::ReadProcessMemory(hProcess, lpBaseAddress, lpBuffer, nSize, lpNumberOfBytesWritten), "ReadProcessMemory");
}

BOOL VirtualProtectEx(HANDLE hProcess, LPVOID lpAddress, SIZE_T dwSize, DWORD flNewProtect, PDWORD lpflOldProtect){
    return callresult::CheckResultNeq(::VirtualProtectEx(hProcess, lpAddress, dwSize, flNewProtect, lpflOldProtect), "VirtualProtectEx");
}

BOOL TerminateProcess(HANDLE hProcess, UINT uExitCode) {
    return callresult::CheckResultNeq(::TerminateProcess(hProcess, uExitCode), "TerminateProcess");
}

}//wrappers

#endif //WINAPI_WRAPPERS_HPP
