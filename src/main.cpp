#include <cxxopts.hpp>
#include <spdlog/spdlog.h>

#include "Application.h"

int main(int argc, char** argv) {
    cxxopts::Options options{"GECK::Mapper", "Fallout 2 map editor"};

    options.add_options()(
        "d,data", "path to the directory where master.dat was extracted", cxxopts::value<std::string>()->default_value("resources"))(
        "m,map", "name of the map to load", cxxopts::value<std::string>()->default_value("kladwtwn.map"))(
        "debug", "show debug messages")(
        "h,help", "Print usage");

    auto result = options.parse(argc, argv);

    if (result.count("help")) {
        std::cout << options.help() << std::endl;
        exit(0);
    }
    if (result.count("debug")) {
        spdlog::set_level(spdlog::level::debug);
    }

    geck::Application app{result["data"].as<std::string>() + "/", result["map"].as<std::string>()};

    app.run();

    return 0;
}
