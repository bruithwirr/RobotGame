#pragma once

#include <rclcpp/rclcpp.hpp>
#include <visualization_msgs/msg/marker_array.hpp>
#include "robot_msgs/msg/robot_state.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "Config.h"

class BotNode : public rclcpp::Node {
public:
    BotNode();

private:
    void onRobotState(const robot_msgs::msg::RobotState::SharedPtr msg);
    void onWasteMarkers(const visualization_msgs::msg::MarkerArray::SharedPtr msg);
    void timerCallback();

    rclcpp::Subscription<robot_msgs::msg::RobotState>::SharedPtr robot_state_sub_;
    rclcpp::Subscription<visualization_msgs::msg::MarkerArray>::SharedPtr waste_sub_;
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr cmd_pub_;
    rclcpp::TimerBase::SharedPtr timer_;

    robot_msgs::msg::RobotState robot_state_;
    bool state_received_;


    struct WastePos { double x; double y; };
    std::vector<WastePos> waste_positions_;

    double map_height_;
    int    max_load_;
    double station_x_;
    double station_y_;


    static constexpr double BOT_SPEED = 70.0;
};
