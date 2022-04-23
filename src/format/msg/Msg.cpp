#include "Msg.h"

#include <regex>
#include <fstream>
#include <streambuf>

#include <spdlog/spdlog.h>

#include <iostream>

Msg::Msg(std::filesystem::path path) {
    std::ifstream stream(path.string());

    if (!stream.is_open()) {
        spdlog::error("Could not open MSG file {}", path.string());
        return;
    }

    // load entire file content into string
    std::string s((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());

    //    std::string s(R"REGEX(  #
    //                            # target hit in eyes
    //                            #
    //                            {1380}{ahoj}{Hey, I'm blind}
    //                            {1381}{}{I can't see. That'll make this fair}
    //                            {1382{}{My eyes, bitch!}
    //                            {1381}{}{This is a
    // multiline text}
    //                            )REGEX");

    // because of bug in CMBATAI2.MSG in messages #1382 and #32020 we need to handle missing '}'
    std::regex re{ R"(\{(\d+)[\{\}|\}]?\{(.*?)\}\{([\s\S]*?)\})" };

    std::smatch matches;

    while (std::regex_search(s, matches, re)) {
        if (matches.size() != 4) {
            spdlog::error("Wrong message format in {}: {}", path.filename().string(), matches[0].str());
        } else {
            int id = stoi(matches[1].str());
            std::string audio = matches[2].str();
            std::string message = matches[3].str();

            // remove new lines
            message.erase(std::remove(message.begin(), message.end(), '\n'), message.end()); // TODO: \r\n

            spdlog::debug("{} message {}: {}", path.filename().string(), id, message);

            _messages[id] = { id, audio, message };
        }
        s = matches.suffix(); // next
    }
}

const Msg::Message& Msg::message(int id) {
    return _messages[id];
}
