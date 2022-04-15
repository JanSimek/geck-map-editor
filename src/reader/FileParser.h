#pragma once

#include <fstream>
#include <memory>
#include <filesystem>
#include <spdlog/spdlog.h>

template <typename T>
class FileParser {
protected:
    std::ifstream stream;
    std::filesystem::path path;

public:

    std::unique_ptr<T> openFile(const std::filesystem::path& path) {
        stream = std::ifstream{path.string(), std::ifstream::in | std::ifstream::binary};
        this->path = path;

        if (!stream.is_open()) {
            spdlog::error("Could not read dat file {}", path.string());
        }

        return read();
    }

    virtual std::unique_ptr<T> read() = 0;

    inline uint32_t read_u8() {
        uint8_t buf[1];
        stream.read(reinterpret_cast<char*>(buf), sizeof(buf));

        uint32_t val = buf[0];

        if (static_cast<size_t>(stream.gcount()) != sizeof(buf)) {
            spdlog::error("ran out of data when trying to read a stream read_u8");
        }

        return val;
    }

    inline uint32_t read_le_u32() {
        uint8_t buf[4];
        stream.read(reinterpret_cast<char*>(buf), sizeof(buf));

        uint32_t val = buf[0];
        val |= static_cast<uint32_t>(buf[1]) << 8;
        val |= static_cast<uint32_t>(buf[2]) << 16;
        val |= static_cast<uint32_t>(buf[3]) << 24;

        if (static_cast<size_t>(stream.gcount()) != sizeof(buf)) {
            spdlog::error("ran out of data when trying to read a stream read_le_u32");
        }

        return val;
    }

    inline uint8_t read_be_u8() {
        uint8_t buf[1];
        stream.read(reinterpret_cast<char*>(buf), sizeof(buf));

        uint8_t val = buf[0];

        if (static_cast<size_t>(stream.gcount()) != sizeof(buf)) {
            spdlog::error("ran out of data when trying to read a stream read_be_u8");
        }

        return val;
    }

    inline uint16_t read_be_u16() {
        uint8_t buf[2]{};
        stream.read(reinterpret_cast<char*>(buf), sizeof(buf));

        uint16_t val = buf[1];
        val |= static_cast<uint16_t>(buf[0]) << 8;

        if (static_cast<size_t>(stream.gcount()) != sizeof(buf)) {
            spdlog::error("ran out of data when trying to read a stream read_be_u16");
        }

        return val;
    }

    inline uint32_t read_be_u32() {
        uint8_t buf[4]{};
        stream.read(reinterpret_cast<char*>(buf), sizeof(buf));

        uint32_t val = buf[3];
        val |= static_cast<uint32_t>(buf[2]) << 8;
        val |= static_cast<uint32_t>(buf[1]) << 16;
        val |= static_cast<uint32_t>(buf[0]) << 24;

        if (static_cast<size_t>(stream.gcount()) != sizeof(buf)) {
            // FIXME: pro file 0..14.pro in kladwntn.map doesn't have the last critter field "damage type"
            spdlog::error("ran out of data when trying to read a stream read_be_u32");
    //        throw std::runtime_error("ran out of data when trying to read a stream");
        }

        return val;
    }

    inline int32_t read_be_i32() {
        return read_be_u32();
    }

    inline std::string read_str(size_t len) {
        std::string ret(len, '\0');
        stream.read(ret.data(), len);
        return ret;
    }

    inline void openFile(uint8_t* buf, size_t n) {
        stream.read(reinterpret_cast<char*>(buf), static_cast<long int>(n));

        if (static_cast<size_t>(stream.gcount()) != n) {
            spdlog::error("ran out of data when trying to read a stream - openFile");
        }
    }

    template <size_t N>
    inline void skip() {
        std::array<uint8_t, N> buf;
        openFile(buf.data(), buf.size());
    }
};
