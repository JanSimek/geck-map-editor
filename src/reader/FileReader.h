#pragma once

#include <fstream>
#include <memory>

template <typename T>
class FileReader {
public:
    std::unique_ptr<T> read(const std::string& filename) {
        std::ifstream stream{filename, std::ifstream::in};

        if (!stream.is_open()) {
            throw std::runtime_error{"Could not read dat file " + filename};
        }

        return read(stream);
    }

    virtual std::unique_ptr<T> read(std::istream& stream) = 0;
};
