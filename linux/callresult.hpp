#ifndef CALLRESULT_HPP
#define CALLRESULT_HPP

#include <stdexcept>
#include <string>
#include <functional>

#include <cstring>
#include <cstdio>
#include <cstdlib>

#include <unistd.h>

namespace callresult {

template <typename int_type>
int_type check_result(int_type result, const char str[]){
    if (result < 0){
        throw std::runtime_error(
            std::string(str)
            + ": "
            + strerror(errno)
        );
    }
    return result;
}

template <typename int_type>
int_type check_errno(const std::function<int_type()> &checkfn, const char str[]){
    errno = 0;
    auto result = checkfn();
    if (errno != 0){
        throw std::runtime_error(
            std::string(str)
            + ": "
            + strerror(errno)
        );
    }
    return result;
}

}//callresult

#endif //CALLRESULT_HPP
