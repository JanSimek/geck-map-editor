#pragma once

#include <fstream>
#include <vector>

namespace geck {

class Lst {
private:
    std::vector<std::string> _list;

public:
    std::vector<std::string> list() const;
    void setList(const std::vector<std::string>& list);

    const std::string& at(int line) const;
};

} // namespace geck
