#pragma once

#include <map>
#include <string>
#include <filesystem>

class Msg
{
public:
    struct Message {
        int id;
        std::string audio;
        std::string text;
    };

    Msg(std::filesystem::path path);

    const Message& message(int id);

private:
    std::map<int, Message> _messages;
};

