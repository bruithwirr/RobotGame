#include "BotNode.h"
#include "ConfigParser.h"
#include <cmath>
#include <limits>

BotNode::BotNode() : rclcpp::Node("bot_node"),
    state_received_(false),
    map_height_(1024.0), max_load_(100)
{
    GameConfig cfg;
    try {
        cfg = ConfigParser::load(
            "/home/bruithwirr/uni/ros2_ws/src/robot_game/config/game_config.yaml"
        );
        station_x_  = cfg.station.x;
        station_y_  = cfg.station.y;
        max_load_   = cfg.robot.max_capacity;
        RCLCPP_INFO(this->get_logger(), "BotNode: konfiguracia nacitana");
    } catch (const ConfigException& e) {
        RCLCPP_WARN(this->get_logger(), "BotNode: %s — pouzivam predvolene hodnoty", e.what());
    }


    robot_state_sub_ = this->create_subscription<robot_msgs::msg::RobotState>(
        "/robot1_state", 10,
        std::bind(&BotNode::onRobotState, this, std::placeholders::_1)
    );

    waste_sub_ = this->create_subscription<visualization_msgs::msg::MarkerArray>(
        "/waste_markers", 10,
        std::bind(&BotNode::onWasteMarkers, this, std::placeholders::_1)
    );

    cmd_pub_ = this->create_publisher<geometry_msgs::msg::Twist>(
        "/robot1_control", 10
    );

    timer_ = this->create_wall_timer(
        std::chrono::milliseconds(100),
        std::bind(&BotNode::timerCallback, this)
    );

    RCLCPP_INFO(this->get_logger(),
        "BotNode spusteny. ");
}

void BotNode::onRobotState(const robot_msgs::msg::RobotState::SharedPtr msg)
{
    robot_state_   = *msg;
    state_received_ = true;
}

void BotNode::onWasteMarkers(const visualization_msgs::msg::MarkerArray::SharedPtr msg)
{
    waste_positions_.clear();

    for (const auto& marker : msg->markers) {
        if (marker.action == visualization_msgs::msg::Marker::DELETEALL) continue;
        if (marker.ns != "waste") continue;

        WastePos pos;
        pos.x = marker.pose.position.x / 0.05;
        pos.y = map_height_ + marker.pose.position.y / 0.05;
        waste_positions_.push_back(pos);
    }
}

void BotNode::timerCallback()
{
    if (!state_received_) return;

    double robot_x = robot_state_.x;
    double robot_y = robot_state_.y;
    double theta   = robot_state_.theta;
    int    load    = robot_state_.current_load;


    double target_x = -1.0;
    double target_y = -1.0;

    if (load >= max_load_) {
        target_x = station_x_;
        target_y = station_y_;
    } else if (!waste_positions_.empty()) {
        double min_dist = std::numeric_limits<double>::max();
        for (const auto& w : waste_positions_) {
            double dx   = robot_x - w.x;
            double dy   = robot_y - w.y;
            double dist = std::sqrt(dx*dx + dy*dy);
            if (dist < min_dist) {
                min_dist = dist;
                target_x = w.x;
                target_y = w.y;
            }
        }
    }

    if (target_x < 0) return;

    double dx         = target_x - robot_x;
    double dy         = target_y - robot_y;
    double angle      = std::atan2(dy, dx);
    double angle_diff = angle - theta;

    while (angle_diff >  M_PI) angle_diff -= 2.0 * M_PI;
    while (angle_diff < -M_PI) angle_diff += 2.0 * M_PI;

    auto msg = geometry_msgs::msg::Twist();

    if (std::abs(angle_diff) < 0.3) {
        msg.linear.x  = BOT_SPEED;
        msg.angular.z = angle_diff * 50.0;
    } else {
        msg.linear.x  = 0.0;
        msg.angular.z = (angle_diff > 0) ? BOT_SPEED : -BOT_SPEED;
    }

    cmd_pub_->publish(msg);
}

int main(int argc, char* argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<BotNode>());
    rclcpp::shutdown();
    return 0;
}
