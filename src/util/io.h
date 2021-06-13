#pragma once

#include <array>
#include <istream>
#include <stdexcept>
#include <vector>
#include <spdlog/spdlog.h>

/**
 * @author Adam Kewley @ https://github.com/adamkewley/klamath
 */
namespace geck {

namespace io {

inline uint32_t read_u8(std::istream& file) {
    uint8_t buf[1];
    file.read(reinterpret_cast<char*>(buf), sizeof(buf));

    uint32_t val = buf[0];

    if (static_cast<size_t>(file.gcount()) != sizeof(buf)) {
        throw std::runtime_error("ran out of data when trying to read a stream");
    }

    return val;
}

inline uint32_t read_le_u32(std::istream& file) {
    uint8_t buf[4];
    file.read(reinterpret_cast<char*>(buf), sizeof(buf));

    uint32_t val = buf[0];
    val |= static_cast<uint32_t>(buf[1]) << 8;
    val |= static_cast<uint32_t>(buf[2]) << 16;
    val |= static_cast<uint32_t>(buf[3]) << 24;

    if (static_cast<size_t>(file.gcount()) != sizeof(buf)) {
        throw std::runtime_error("ran out of data when trying to read a stream");
    }

    return val;
}

inline uint8_t read_be_u8(std::istream& file) {
    uint8_t buf[1];
    file.read(reinterpret_cast<char*>(buf), sizeof(buf));

    uint16_t val = buf[0];

    if (static_cast<size_t>(file.gcount()) != sizeof(buf)) {
        throw std::runtime_error("ran out of data when trying to read a stream");
    }

    return val;
}

inline uint16_t read_be_u16(std::istream& file) {
    uint8_t buf[2];
    file.read(reinterpret_cast<char*>(buf), sizeof(buf));

    uint16_t val = buf[1];
    val |= static_cast<uint16_t>(buf[0]) << 8;

    if (static_cast<size_t>(file.gcount()) != sizeof(buf)) {
        throw std::runtime_error("ran out of data when trying to read a stream");
    }

    return val;
}

inline uint32_t read_be_u32(std::istream& file) {
    uint8_t buf[4];
    file.read(reinterpret_cast<char*>(buf), sizeof(buf));

    uint32_t val = buf[3];
    val |= static_cast<uint32_t>(buf[2]) << 8;
    val |= static_cast<uint32_t>(buf[1]) << 16;
    val |= static_cast<uint32_t>(buf[0]) << 24;

    if (static_cast<size_t>(file.gcount()) != sizeof(buf)) {
        // FIXME: pro file 0..14.pro in kladwntn.map doesn't have the last critter field "damage type"
        spdlog::error("ran out of data when trying to read a stream");
//        throw std::runtime_error("ran out of data when trying to read a stream");
    }

    return val;
}

inline int32_t read_be_i32(std::istream& file) {
    return read_be_u32(file);
}

inline std::string read_str(std::istream& file, size_t len) {
    std::string ret(len, '\0');
    file.read(ret.data(), len);
    return ret;
}

inline void read(std::istream& file, uint8_t* buf, size_t n) {
    file.read(reinterpret_cast<char*>(buf), static_cast<long int>(n));

    if (static_cast<size_t>(file.gcount()) != n) {
        throw std::runtime_error("ran out of data when trying to read a stream");
    }
}

template <size_t N>
inline void skip(std::istream& file) {
    std::array<uint8_t, N> buf;
    read(file, buf.data(), buf.size());
}

}  // namespace io

}  // namespace geck
