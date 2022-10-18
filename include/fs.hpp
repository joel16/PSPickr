#pragma once

#include <string>

namespace FS {
    bool FileExists(const std::string &path);
    int ReadFile(const std::string &path, void *buf, int size);
    int WriteFile(const std::string &path, void *buf, int size);
}
