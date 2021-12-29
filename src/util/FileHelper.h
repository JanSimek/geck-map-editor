#pragma once

#include <string>
#include <filesystem>

class FileHelper {
private:
    FileHelper() {}

    std::string _path;
    std::string _fallout2DataPath;

public:
    // Singleton
    FileHelper(FileHelper const&) = delete;
    void operator=(FileHelper const&) = delete;

    static FileHelper& getInstance() {
        static FileHelper instance;  // Guaranteed to be destroyed.
        return instance;
    }

    std::filesystem::path resourcesPath() const;
    void setResourcesPath(const std::filesystem::path& path);


    std::filesystem::path fallout2DataPath() const;
    void setFallout2DataPath(const std::filesystem::path& path);

};
