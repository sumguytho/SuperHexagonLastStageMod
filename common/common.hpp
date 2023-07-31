#ifndef PROGRAM_MODIFICATION_HPP
#define PROGRAM_MODIFICATION_HPP

#include <cstdio>
#include <cstring>
#include <cstdlib>

#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>
#include <functional>


namespace common {

using byte_t = unsigned char;
using bytes_t = std::vector<byte_t>;
using addr_t = void*;
using caddr_t = const void*;
constexpr bool is_x86(){ return sizeof(addr_t) == 4; }
constexpr bool is_x86_64(){ return sizeof(addr_t) == 8; }
using lpint = std::conditional_t<is_x86_64(), uint64_t, uint32_t>;
using rdfn = std::function<void(caddr_t, addr_t, std::size_t)>;
using wrfn = std::function<void(addr_t, caddr_t, std::size_t)>;


template<typename to_t, typename from_t>
constexpr to_t bit_cast(from_t src){
    static_assert(sizeof(to_t) == sizeof(from_t) &&
        std::is_trivially_copyable_v<from_t> &&
        std::is_trivially_copyable_v<to_t>);
    to_t dst;
    std::memcpy(&dst, &src, sizeof(to_t));
    return dst;
}

template<typename T>
struct identity {
    using type = T;
};
template<typename T>
using identity_t = typename identity<T>::type;

template<typename result_t>
result_t mem_cast(byte_t *src){
    result_t result;
    std::memcpy(&result, src, sizeof(result_t));
    return result;
}


constexpr lpint make_lpint(caddr_t addr){ return bit_cast<lpint>(addr); }
constexpr addr_t make_addr(lpint value){ return bit_cast<addr_t>(value); }


template<typename int_type>
int_type merge_words(int_type word1, std::size_t count1, identity_t<int_type> word2){
    const std::size_t count1_bits = count1 * 8;
    const uint64_t merge_mask = 0xffffffffffffffff >> count1_bits << count1_bits;
    word1 = (word1 & ~merge_mask) | (word2 & merge_mask);
    return word1;
}

constexpr char half_byte_char(uint8_t hb) {
    if (hb <= 9) hb += '0';
    else hb = hb - 0xa + 'a';
    return hb;
}

template<typename InputIterator>
std::string bytes_as_str(InputIterator begin, InputIterator end){
    std::string result( (end - begin)*2, 0 );
    auto result_data = result.data();
    for (; begin != end; ++begin, result_data += 2) {
        result_data[0] = half_byte_char(begin[0] >> 4);
        result_data[1] = half_byte_char(begin[0] & 0xf);
    }
    return result;
}

inline std::string to_string(addr_t ptr){
    std::stringstream ss;
    ss << ptr;
    return ss.str();
}

template<typename byte_container>
std::string to_string(byte_container bytes){
    return bytes_as_str(std::begin(bytes), std::end(bytes));
}

}//common

#endif //PROGRAM_MODIFICATION_HPP
