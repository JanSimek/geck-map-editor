#pragma once

#include <string>
#include <vector>

namespace geck {

class Gam {
public:
    const std::string& gvarKey(int index) const;
    int gvarValue(const std::string& key);
    int gvarValue(int index);

    const std::string& mvarKey(int index) const;
    int mvarValue(const std::string& key);
    int mvarValue(int index);

    void addMvar(const std::string& key, int value);
    void addGvar(const std::string& key, int value);

private:
    std::vector<std::pair<std::string, int>> _gvars;
    std::vector<std::pair<std::string, int>> _mvars;
};

} // namespace geck
