#pragma once

#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/laser_scan.hpp>
#include "robot_msgs/msg/robot_state.hpp"
#include "environment/Lidar.h"
#include "environment/Environment.h"
#include "Config.h"

class LidarNode : public rclcpp::Node {
public:
    explicit LidarNode(int robot_id);

private:
    void onRobotState(const robot_msgs::msg::RobotState::SharedPtr msg);
    void publishScan();

    rclcpp::Subscription<robot_msgs::msg::RobotState>::SharedPtr state_sub_;
    rclcpp::Publisher<sensor_msgs::msg::LaserScan>::SharedPtr scan_pub_;
    rclcpp::TimerBase::SharedPtr timer_;

    std::shared_ptr<environment::Environment> env_;
    std::unique_ptr<lidar::Lidar> lidar_;

    robot_msgs::msg::RobotState robot_state_;
    bool state_received_;

    int robot_id_;
    double map_height_;

    static constexpr int   NUM_RAYS    = 180;
    static constexpr float ANGLE_MIN   = -M_PI / 2.0;
    static constexpr float ANGLE_MAX   =  M_PI / 2.0;
    static constexpr float MAX_DIST_PX = 200.0;
};
