#include "FileHelper.h"

/**
 * @brief Path to the Geck mapper resources
 */
std::filesystem::path FileHelper::resourcesPath() const {
    return _path;
}

void FileHelper::setResourcesPath(const std::filesystem::path& path) {
    _path = path;
}

/**
 * @brief Path to where the Fallout 2 installation including the "data" folder is located
 */
std::filesystem::path FileHelper::fallout2DataPath() const {
    return _fallout2DataPath;
}

void FileHelper::setFallout2DataPath(const std::filesystem::path& path) {
    _fallout2DataPath = path;
}
