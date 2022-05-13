#pragma once

#include "format/pro/Pro.h"

namespace geck {

class Lst;
class Msg;

class ProHelper {
public:
    static Msg* msgFile(Pro::OBJECT_TYPE type);

    static Lst* lstFile(uint32_t PID);

    static const std::filesystem::path basePath(uint32_t PID);
};

}
