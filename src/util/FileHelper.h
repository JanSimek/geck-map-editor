#pragma once

#include <string>

class FileHelper {
private:
    FileHelper() {}

    std::string _path;

public:
    // Singleton
    FileHelper(FileHelper const&) = delete;
    void operator=(FileHelper const&) = delete;

    static FileHelper& getInstance() {
        static FileHelper instance;  // Guaranteed to be destroyed.
        return instance;
    }

    std::string path() const;
    void setPath(const std::string& path);
};
