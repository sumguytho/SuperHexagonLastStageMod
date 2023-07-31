#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <functional>
#include <utility>

#include <cstring>

#include <sched.h>
#include <sys/ptrace.h>

#include "common.hpp"
#include "callresult.hpp"

// somewhat misleading name since it writes using ptrace and not /proc/<pid>/mem
namespace memory {

using word_t = decltype( ptrace(PTRACE_TRACEME, 0, 0, 0) );

// i don't know why i use static in header files
static inline word_t peek_word(pid_t pid, __ptrace_request peek_request, const void *src){
    // a hack since check_errno deduces int_type from function type
    std::function<word_t()> peekfn = [&](){
        return ptrace(peek_request, pid, src, 0);
    };
    return callresult::check_errno(peekfn, "ptrace");
}

static inline void poke_word(pid_t pid, __ptrace_request poke_request, void *dst, word_t value){
    callresult::check_result( ptrace(poke_request, pid, dst, value), "ptrace" );
}

static inline void read(pid_t pid, __ptrace_request peek_request, const void *address, void *buffer, std::size_t count){
    auto addr_cur = static_cast<const common::byte_t*>(address);
    auto buffer_cur = static_cast<common::byte_t*>(buffer);
    for (std::size_t bread{}; bread < count;){
        const auto data = peek_word(pid, peek_request, addr_cur + bread);
        const std::size_t data_used = std::min(count - bread, sizeof(data));
        // i don't think i need to account for BE
        std::memcpy(buffer_cur + bread, &data, data_used);
        bread += data_used;
    }
}

static inline void write(pid_t pid, __ptrace_request poke_request, __ptrace_request peek_request,
    void *address, const void *buffer, std::size_t count)
{
    auto addr_cur = static_cast<common::byte_t*>(address);
    for (std::size_t bwritten{}; bwritten < count;){
        const auto data = peek_word(pid, peek_request, addr_cur);
        const std::size_t data_used = std::min(count - bwritten, sizeof(data));
        word_t new_data{};
        std::memcpy(&new_data, buffer, data_used);
        if (data_used < sizeof(data)){
            new_data = common::merge_words(new_data, data_used, data);
        }
        poke_word(pid, poke_request, static_cast<void *>(addr_cur), new_data);
        bwritten += data_used;
        addr_cur += data_used;
    }
}

inline void read_code(pid_t pid, const void *address, void *buffer, std::size_t count){
    read(pid, PTRACE_PEEKTEXT, address, buffer, count);
}

inline void write_code(pid_t pid, void *address, const void *buffer, std::size_t count){
    write(pid, PTRACE_POKETEXT, PTRACE_PEEKTEXT, address, buffer, count);
}

inline void read_data(pid_t pid, const void *address, void *buffer, std::size_t count){
    read(pid, PTRACE_PEEKDATA, address, buffer, count);
}

inline void write_data(pid_t pid, void *address, const void *buffer, std::size_t count){
    write(pid, PTRACE_POKEDATA, PTRACE_PEEKDATA, address, buffer, count);
}

}//memory

#endif //MEMORY_HPP
