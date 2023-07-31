#ifndef PROCESS_HPP
#define PROCESS_HPP

#include <stdexcept>
#include <string>

#include <sched.h>
#include <sys/ptrace.h>
#include <sys/wait.h>

#include "callresult.hpp"

// efn = expose function name
#define ptraceefn(arg, ...) ptrace(arg, ##__VA_ARGS__), "ptrace"
#define forkefn(arg, ...) fork(arg, ##__VA_ARGS__), "fork"
#define execvefn(arg, ...) execv(arg, ##__VA_ARGS__), "execv"
#define waitpidefn(arg, ...) waitpid(arg, ##__VA_ARGS__), "waitpid"

namespace process {

inline pid_t start_suspended(char **argv){
    auto fork_result = callresult::check_result( forkefn() );
    if (fork_result > 0) {
        return fork_result;
    }
    callresult::check_result( ptraceefn(PTRACE_TRACEME, 0, 0, 0) );
    callresult::check_result( execvefn(argv[0], argv) );
    return fork_result;
}

inline void wait_for_ptrace_suspend(pid_t pid){
    int	status;
    callresult::check_result( waitpidefn(pid, &status, __WALL) );
    if (WIFSTOPPED(status) && WSTOPSIG(status) == SIGTRAP) {
        return;
    }
    throw std::runtime_error(
        std::string("process with pid ")
        + std::to_string(pid)
        + " wasn't suspended by ptrace"
    );
}

inline void resume(pid_t pid){
    callresult::check_result( ptraceefn(PTRACE_CONT, pid, 0, 0) );
}

inline void terminate(pid_t pid){
    kill(pid, SIGTERM);
}

}//process

#undef ptraceefn
#undef forkfn
#undef execvfn
#undef waitpidefn

#endif // PROCESS_HPP
