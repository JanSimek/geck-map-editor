#pragma once

#include <filesystem>
#include <string>

class IFile {
public:
    IFile(const std::filesystem::path& path)
        : _filename(path.filename().string())
        , _path(path) { }
    virtual ~IFile() = default;

    const std::string& filename() { return _filename; }
    const std::filesystem::path& path() { return _path; }

protected:
    std::string _filename;
    std::filesystem::path _path;
};
