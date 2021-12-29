#include <cxxopts.hpp>
#include <spdlog/spdlog.h>

#include "Application.h"
#include "util/FileHelper.h"

#include <portable-file-dialogs.h>

int main(int argc, char** argv) {
    cxxopts::Options options{"GECK::Mapper", "Fallout 2 map editor"};

    spdlog::set_pattern("[%^%l%$] [thread %t] %v");

    std::filesystem::path data_path = std::filesystem::current_path();

    options.add_options()(
        "d,data", "path to the directory where master.dat was extracted", cxxopts::value(data_path))(
        "m,map", "name of the map to load")( //, cxxopts::value<std::string>()->default_value("kladwtwn.map"))(
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
    if (result.count("data")) {
        FileHelper::getInstance().setFallout2DataPath(result["data"].as<std::string>());
    } else {
        auto dir = pfd::select_folder("Select Fallout 2 \"data\" directory which contains maps", data_path).result();
        spdlog::info("User selected data directory: {}", dir);
        FileHelper::getInstance().setFallout2DataPath(dir);
    }
    spdlog::info("Fallout 2 data path: {}", FileHelper::getInstance().fallout2DataPath().string());

    // TODO: get real path to the binary
    std::filesystem::path resources_path = std::filesystem::weakly_canonical(argv[0]).parent_path();
    resources_path /= "resources";

    spdlog::info("Resources path: {}", resources_path.string());

    FileHelper::getInstance().setResourcesPath(resources_path);

    geck::Application app{data_path, !result.count("map") ? std::string() : result["map"].as<std::string>()};

    app.run();

    return 0;
}
