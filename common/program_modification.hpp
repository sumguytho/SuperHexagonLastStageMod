#ifndef COMMON_HPP
#define COMMON_HPP

#include <cstdio>
#include <cstring>

#include <ostream>
#include <algorithm>
#include <stdexcept>
#include <functional>
#include <string>
#include <vector>

#include "common.hpp"


namespace common {

struct ProgramModification {
    using byte_t = common::byte_t;
    using bytes_t = common::bytes_t;
    using addr_t = common::addr_t;
    using caddr_t = common::caddr_t;
    using rdfn = common::rdfn;
    using wrfn = common::wrfn;

    void relocate(addr_t image_base_old, addr_t image_base_new){
        std::int64_t ptr_diff = static_cast<byte_t*>(image_base_new) - static_cast<byte_t*>(image_base_old);
        for (auto &patch : _patches){
            patch.offset = static_cast<byte_t*>(patch.offset) + ptr_diff;
        }
    }
    void relocate(addr_t image_base_new){
        const addr_t IMAGE_BASE_DEFAULT = common::make_addr(0x400000);
        relocate(IMAGE_BASE_DEFAULT, image_base_new);
    }
    template<typename int_type>
    void add_patch(addr_t offset, int_type int_expected, identity_t<int_type> int_new, bool is_data = false) {
        bytes_t expected_bytes(sizeof(int_expected));
        std::memcpy(expected_bytes.data(), &int_expected, sizeof(int_expected));
        bytes_t new_bytes(sizeof(int_new));
        std::memcpy(new_bytes.data(), &int_new, sizeof(int_new));
        add_patch(offset, std::move(expected_bytes), std::move(new_bytes), is_data);
    }
    void add_patch(addr_t offset, bytes_t expected_bytes, bytes_t new_bytes, bool is_data = false) {
        if (expected_bytes.size() != new_bytes.size()){
            throw std::runtime_error(
                "patch size mismatch: expected_bytes: "
                + std::to_string(expected_bytes.size())
                + "; new_bytes: "
                + std::to_string(new_bytes.size())
            );
        }
        std::size_t num_bytes = expected_bytes.size();
        _patches.push_back( { offset, std::move(expected_bytes), std::move(new_bytes), num_bytes, is_data } );
    }
    void test(const rdfn &readfn){
        test(readfn, readfn);
    }
    void test(const rdfn &readdatafn, const rdfn &readcodefn){
        using std::to_string;
        using common::to_string;
        bytes_t cmp_buf;
        for (auto &patch : _patches){
            if (cmp_buf.size() < patch.num_bytes){
                cmp_buf.resize(patch.num_bytes);
            }
            if (patch.is_data){
                readdatafn(patch.offset, cmp_buf.data(), patch.num_bytes);
            }
            else {
                readcodefn(patch.offset, cmp_buf.data(), patch.num_bytes);
            }
            if (std::memcmp(cmp_buf.data(), patch.expected_bytes.data(), patch.num_bytes)){
                throw std::runtime_error(
                    "at offset "
                    + to_string(patch.offset)
                    + ": expected bytes { "
                    + to_string(patch.expected_bytes)
                    + " }, got bytes { "
                    + to_string(cmp_buf)
                    + " }"
                );
            }
        }
    }
    void apply(const wrfn &writefn){
        apply(writefn, writefn);
    }
    void apply(const wrfn &writedatafn, const wrfn &writecodefn) {
        for (auto &patch : _patches){
            if (patch.is_data){
                writedatafn(patch.offset, patch.new_bytes.data(), patch.num_bytes);
            }
            else{
                writecodefn(patch.offset, patch.new_bytes.data(), patch.num_bytes);
            }
        }
    }
    void print(std::ostream &ostream){
        for (auto &patch : _patches){
            ostream << patch.offset << ": { " << common::to_string(patch.expected_bytes)
                << " } -> { " << common::to_string(patch.new_bytes) << " } ("
                << patch.num_bytes << " byte(s))\n";
        }
    }
private:
    struct PatchInfo {
        addr_t offset;
        bytes_t expected_bytes;
        bytes_t new_bytes;
        std::size_t num_bytes;
        bool is_data;
    };
    std::vector<PatchInfo> _patches;
};

}//common

#endif //COMMON_HPP
