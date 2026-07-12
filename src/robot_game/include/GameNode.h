#pragma once

#include <rclcpp/rclcpp.hpp>
#include <visualization_msgs/msg/marker_array.hpp>
#include <visualization_msgs/msg/marker.hpp>
#include "robot_msgs/msg/robot_state.hpp"
#include "waste/WasteGenerator.h"
#include "environment/Environment.h"
#include "Config.h"

struct Station {
    double x;
    double y;
    double radius;
};

class GameNode : public rclcpp::Node {
public:
    GameNode();

private:
    void onRobot0State(const robot_msgs::msg::RobotState::SharedPtr msg);
    void onRobot1State(const robot_msgs::msg::RobotState::SharedPtr msg);
    void checkCollection(const robot_msgs::msg::RobotState& state, int robot_id);
    void checkUnload(const robot_msgs::msg::RobotState& state, int robot_id);
    void publishWasteMarkers();
    void publishStationMarker();
    void publishObstacleMarkers();
    void gameLoop();

    rclcpp::Subscription<robot_msgs::msg::RobotState>::SharedPtr sub_robot0_;
    rclcpp::Subscription<robot_msgs::msg::RobotState>::SharedPtr sub_robot1_;

    rclcpp::Publisher<visualization_msgs::msg::MarkerArray>::SharedPtr waste_pub_;
    rclcpp::Publisher<visualization_msgs::msg::MarkerArray>::SharedPtr obstacles_pub_;
    rclcpp::Publisher<visualization_msgs::msg::Marker>::SharedPtr station_pub_;

    rclcpp::TimerBase::SharedPtr timer_;

    std::shared_ptr<environment::Environment> env_;
    std::unique_ptr<waste::WasteGenerator> generator_;
    std::vector<std::shared_ptr<waste::WasteItem>> waste_items_;

    robot_msgs::msg::RobotState robot0_state_;
    robot_msgs::msg::RobotState robot1_state_;

    int load_robot0_;
    int load_robot1_;
    int max_load_;

    int score_robot0_;
    int score_robot1_;

    Station station_;
    double map_height_;

    bool duel_bot_mode_;

    std::vector<CircleObstacle> circle_obstacles_;
    std::vector<RectObstacle>   rect_obstacles_;

    static constexpr double COLLECT_RADIUS = 20.0;
    static constexpr double UNLOAD_RADIUS  = 30.0;
};
