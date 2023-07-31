#ifndef CALLRESULT_HPP
#define CALLRESULT_HPP

#include <stdexcept>
#include <string>
#include <functional>

#include <cstring>
#include <cstdio>
#include <cstdlib>

extern "C" {
#include <Windows.h>
#include <tchar.h>
}

#include "common.hpp"

// can't partially specialize a function
// comparison in function name is success condition
#define YIELD_CHECKRESULT(funcName, cmpFnClass, cmpAnchor) \
template <typename ResultT>\
ResultT funcName(ResultT result, const char *str){\
    return CheckResult(result, cmpFnClass<ResultT>(), cmpAnchor, str);\
}

namespace callresult {

// yoinked somewhere from MSDN for WinAPI
std::string StringError()
{
    constexpr std::size_t FormatMessageSizeMax = 1024;
    DWORD eNum;
    char sysMsg[FormatMessageSizeMax];
    char* p;

    eNum = GetLastError( );
    FormatMessageA( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, eNum,
        MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
        sysMsg, FormatMessageSizeMax, NULL );

    // Trim the end of the line and terminate it with a null
    p = sysMsg;
    while( ( *p > 31 ) || ( *p == 9 ) ) ++p;
    do { *p-- = 0; } while( ( p >= sysMsg ) && ( ( *p == '.' ) || ( *p < 33 ) ) );

    return sysMsg;
}

template <typename ResultT, typename Callable>
static ResultT CheckResult(ResultT result, Callable &&cmpFn,
    common::identity_t<ResultT> cmpAnchor, const char *str)
{
    if (!cmpFn(result, cmpAnchor)){
        throw std::runtime_error(
            std::string(str)
            + ": "
            + StringError()
        );
    }
    return result;
}

YIELD_CHECKRESULT(CheckResultEq, std::equal_to, 0);
YIELD_CHECKRESULT(CheckResultNeq, std::not_equal_to, 0);
YIELD_CHECKRESULT(CheckResultGeq, std::greater_equal, 0);
YIELD_CHECKRESULT(CheckHandleResult, std::not_equal_to, INVALID_HANDLE_VALUE);


template <typename ResultT, typename FirstArg, typename ...Args>
ResultT CheckValuesNeq(ResultT result, const char *str, FirstArg typeEnforceArg, Args ...args){
    FirstArg arg_array[] = { typeEnforceArg, args... };
    for (auto &arg : arg_array){
        if (result == arg){
            throw std::runtime_error(
                std::string(str)
                + ": "
                + StringError()
            );
        }
    }
    return result;
}

template <typename ResultT, typename FirstArg, typename ...Args>
ResultT CheckValuesEqAnyOf(ResultT result, const char *str, FirstArg typeEnforceArg, Args ...args){
    FirstArg arg_array[] = { typeEnforceArg, args... };
    bool has_match = false;
    for (auto &arg : arg_array){ has_match = has_match || result == static_cast<ResultT>(arg); }
    if (!has_match){
        throw std::runtime_error(
            std::string(str)
            + ": "
            + StringError()
        );
    }
    return result;
}

}//callresult

#undef YIELD_CHECKRESULT

#endif //CALLRESULT_HPP
