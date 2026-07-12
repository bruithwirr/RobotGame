#pragma once

#include <rclcpp/rclcpp.hpp>
#include "robot_msgs/msg/robot_state.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "visualization_msgs/msg/marker.hpp"
#include "nav_msgs/msg/occupancy_grid.hpp"
#include "environment/Environment.h"
#include "robot/Robot.h"
#include "Config.h"

class RobotNode : public rclcpp::Node {
public:
    explicit RobotNode(int robot_id);

private:
    void publishState();
    void publishMap();
    void checkCollision();
    void rcv_message(const geometry_msgs::msg::Twist::SharedPtr msg);

    rclcpp::Publisher<robot_msgs::msg::RobotState>::SharedPtr publisher_;
    rclcpp::Publisher<visualization_msgs::msg::Marker>::SharedPtr rviz_marker_;
    rclcpp::Publisher<visualization_msgs::msg::Marker>::SharedPtr arrow_marker_;
    rclcpp::Publisher<nav_msgs::msg::OccupancyGrid>::SharedPtr map_pub_;
    rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr subscriber_;
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::TimerBase::SharedPtr map_timer_;

    std::shared_ptr<robot::Robot> robot_;
    std::shared_ptr<environment::Environment> env_;

    int robot_id_;
    double robot_radius_;
    GameConfig cfg_;
};
