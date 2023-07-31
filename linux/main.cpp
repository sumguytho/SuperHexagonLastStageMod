#include <exception>
#include <iostream>
#include <optional>

#include "common.hpp"
#include "memory.hpp"
#include "process.hpp"
#include "superhexmod.hpp"


int main(){
    namespace cm = common;

    char tracee_path[] = "./SuperHexagon";
    char *child_argv[] = { tracee_path, 0 };

    std::optional<pid_t> tracee_pid;
    try {
        tracee_pid = process::start_suspended(child_argv);
        std::cout << "started child process with pid " << *tracee_pid << '\n';

        process::wait_for_ptrace_suspend(*tracee_pid);

        auto mod = superhexmod::steam_linux64_8838351();

        mod.test([&](cm::caddr_t dst, cm::addr_t src, std::size_t sz){
            memory::read_data(*tracee_pid, dst, src, sz);
        });
        mod.apply([&](cm::addr_t dst, cm::caddr_t src, std::size_t sz){
            memory::write_data(*tracee_pid, dst, src, sz);
        });

        std::cout << "mod has been applied successfully, resuming the process " << *tracee_pid << '\n';
        process::resume( *tracee_pid );
    }
    catch(std::exception &ex){
        std::cerr << "caught exception while applying the mod: " << ex.what() << '\n';
        if (tracee_pid){
            process::terminate(*tracee_pid);
        }
        return EXIT_FAILURE;
    }
}
