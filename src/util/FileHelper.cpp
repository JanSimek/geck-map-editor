#include "FileHelper.h"

std::string FileHelper::path() const {
    return _path;
}

void FileHelper::setPath(const std::string& path) {
    _path = path;
}
