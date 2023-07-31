#include <cstdint>
#include <cstring>
#include <exception>
#include <functional>
#include <iostream>
#include <stdexcept>

#include "program_modification.hpp"

template<typename CallableT, typename ...Args>
void wrap_ex(CallableT &&c, Args&& ...args){
    try {
        std::invoke(std::forward<CallableT>(c), std::forward<Args>(args)...);
    }
    catch(std::exception &ex){
        std::cerr << "caught exception: " << ex.what() << '\n';
        return;
    }
    throw std::runtime_error("provided callable didn't generate an exception");
}

int main(){
    namespace cm = common;
    using cm::ProgramModification;
    using cm::make_addr;
    using cm::make_lpint;

    ProgramModification mod;
    wrap_ex([&](){
        mod.add_patch(make_addr(0x0), {2,3}, {2});
    });
    cm::bytes_t ref{2,3,4,5};
    mod.add_patch(make_addr(0x1), {3}, {6});
    mod.add_patch(make_addr(0x2), {4}, {7});
    mod.add_patch(make_addr(0x3), {5}, {8});

    cm::rdfn peekfn = [&](cm::caddr_t from, cm::addr_t to, std::size_t sz){
        auto idx = make_lpint(from);
        std::memcpy(to, &ref[idx], sz);
    };
    cm::wrfn modifyfn = [&](cm::addr_t to, cm::caddr_t from, std::size_t sz){
        auto idx = make_lpint(to);
        std::memcpy(&ref[idx], from, sz);
    };
    std::cout << "before modificaition: " << cm::to_string(ref) << '\n';
    mod.test(peekfn);
    mod.apply(modifyfn);
    std::cout << "after modificaition: " << cm::to_string(ref) << '\n';

    ProgramModification mod2;
    mod2.add_patch(make_addr(0x1), {7}, {7});
    wrap_ex([&](){
        mod2.test(peekfn);
    });

    ref.assign({2,3,4,5});
    ProgramModification mod3;
    // alignment doesn't matter since it's all gonna be treated as bytes internally
    mod3.add_patch(make_addr(0x0), uint32_t{0x05040302}, uint32_t{0x02030405});
    std::cout << "before modification: " << cm::to_string(ref) << '\n';
    mod3.test(peekfn);
    mod3.apply(modifyfn);
    std::cout << "after modification: " << cm::to_string(ref) << '\n';

    std::cout << "modification with base 0x0:\n";
    mod.print(std::cout);
    mod.relocate(make_addr(0x0), make_addr(0x10));
    std::cout << "modification with base 0x10:\n";
    mod.print(std::cout);
    mod.relocate(make_addr(0x10), make_addr(0x8));
    std::cout << "modification with base 0x8:\n";
    mod.print(std::cout);
}
