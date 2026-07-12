#include "ConfigParser.h"
#include <yaml-cpp/yaml.h>
#include <fstream>

GameConfig ConfigParser::load(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.good()) {
        throw ConfigException("Konfiguracny subor nenajdeny: " + filename);
    }

    YAML::Node config;
    try {
        config = YAML::LoadFile(filename);
    } catch (const YAML::Exception& e) {
        throw ConfigException("Chyba pri parsovani YAML: " + std::string(e.what()));
    }

    GameConfig game_config;


    game_config.game_mode = config["game_mode"]
        ? config["game_mode"].as<std::string>()
        : "duel";


    if (!config["map"]) throw ConfigException("Chyba: chyba sekcia 'map'");
    game_config.map.filename   = config["map"]["filename"].as<std::string>();
    game_config.map.resolution = config["map"]["resolution"].as<double>();


    if (!config["robot"]) throw ConfigException("Chyba: chyba sekcia 'robot'");
    game_config.robot.radius       = config["robot"]["radius"].as<double>();
    game_config.robot.max_capacity = config["robot"]["max_capacity"].as<int>();

    if (config["robot"]["start_positions"]) {
        for (const auto& pos : config["robot"]["start_positions"]) {
            RobotStartPos p;
            p.x = pos["x"].as<double>();
            p.y = pos["y"].as<double>();
            game_config.robot.start_positions.push_back(p);
        }
    }


    if (!config["waste"]) throw ConfigException("Chyba: chyba sekcia 'waste'");
    game_config.waste.count      = config["waste"]["count"].as<int>();
    game_config.waste.min_radius = config["waste"]["min_radius"].as<double>();
    game_config.waste.max_radius = config["waste"]["max_radius"].as<double>();
    for (const auto& type : config["waste"]["types"]) {
        game_config.waste.types.push_back(type.as<std::string>());
    }


    if (!config["station"]) throw ConfigException("Chyba: chyba sekcia 'station'");
    game_config.station.x      = config["station"]["x"].as<double>();
    game_config.station.y      = config["station"]["y"].as<double>();
    game_config.station.radius = config["station"]["radius"].as<double>();


    if (config["obstacles"]) {
        if (config["obstacles"]["circles"]) {
            for (const auto& c : config["obstacles"]["circles"]) {
                CircleObstacle obs;
                obs.x      = c["x"].as<double>();
                obs.y      = c["y"].as<double>();
                obs.radius = c["radius"].as<double>();
                game_config.obstacles.circles.push_back(obs);
            }
        }
        if (config["obstacles"]["rectangles"]) {
            for (const auto& r : config["obstacles"]["rectangles"]) {
                RectObstacle obs;
                obs.x      = r["x"].as<double>();
                obs.y      = r["y"].as<double>();
                obs.width  = r["width"].as<double>();
                obs.height = r["height"].as<double>();
                game_config.obstacles.rectangles.push_back(obs);
            }
        }
    }

    return game_config;
}
