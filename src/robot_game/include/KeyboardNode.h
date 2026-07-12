#pragma once

#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <X11/Xlib.h>

class KeyboardNode : public rclcpp::Node {
public:
    KeyboardNode();
    ~KeyboardNode();

private:
    void timerCallback();
    bool isKeyPressed(int keycode);

    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr robot1_control_;
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr robot2_control_;
    rclcpp::TimerBase::SharedPtr timer_;

    Display* display_;
};
