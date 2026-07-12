#pragma once
#include <string>
#include <vector>

struct MapConfig {
    std::string filename;
    double resolution;
};

struct RobotStartPos {
    double x;
    double y;
};

struct RobotConfig {
    double radius;
    int    max_capacity;
    std::vector<RobotStartPos> start_positions;
};

struct WasteConfig {
    int    count;
    double min_radius;
    double max_radius;
    std::vector<std::string> types;
};

struct StationConfig {
    double x;
    double y;
    double radius;
};

struct CircleObstacle {
    double x;
    double y;
    double radius;
};

struct RectObstacle {
    double x;
    double y;
    double width;
    double height;
};

struct ObstaclesConfig {
    std::vector<CircleObstacle> circles;
    std::vector<RectObstacle>   rectangles;
};

struct GameConfig {
    std::string     game_mode;
    MapConfig       map;
    RobotConfig     robot;
    WasteConfig     waste;
    StationConfig   station;
    ObstaclesConfig obstacles;
};
