#include "LidarNode.h"
#include "ConfigParser.h"
#include "types/Geometry.h"
#include <cmath>

LidarNode::LidarNode(int robot_id)
    : rclcpp::Node("lidar_node_" + std::to_string(robot_id)),
      state_received_(false), robot_id_(robot_id)
{
    GameConfig cfg;
    try {
        cfg = ConfigParser::load(
            "/home/bruithwirr/uni/ros2_ws/src/robot_game/config/game_config.yaml"
        );
    } catch (const ConfigException& e) {
        RCLCPP_FATAL(this->get_logger(), "Chyba konfigurácie: %s", e.what());
        rclcpp::shutdown();
        return;
    }

    environment::Config env_config;
    env_config.map_filename = cfg.map.filename;
    env_config.resolution   = cfg.map.resolution;
    env_ = std::make_shared<environment::Environment>(env_config);

    map_height_ = env_->getMap().rows;


    lidar::Config lidar_config;
    lidar_config.max_range    = MAX_DIST_PX;
    lidar_config.beam_count        = NUM_RAYS;
    lidar_config.first_ray_angle = ANGLE_MIN;
    lidar_config.last_ray_angle  = ANGLE_MAX;
    lidar_ = std::make_unique<lidar::Lidar>(lidar_config, env_);


    state_sub_ = this->create_subscription<robot_msgs::msg::RobotState>(
        "/robot" + std::to_string(robot_id_) + "_state", 10,
        std::bind(&LidarNode::onRobotState, this, std::placeholders::_1)
    );


    scan_pub_ = this->create_publisher<sensor_msgs::msg::LaserScan>(
        "/robot" + std::to_string(robot_id_) + "/scan", 10
    );

    timer_ = this->create_wall_timer(
        std::chrono::milliseconds(100),
        std::bind(&LidarNode::publishScan, this)
    );

    RCLCPP_INFO(this->get_logger(), "LidarNode %d spusteny", robot_id_);
}

void LidarNode::onRobotState(const robot_msgs::msg::RobotState::SharedPtr msg)
{
    robot_state_    = *msg;
    state_received_ = true;
}

void LidarNode::publishScan()
{
    if (!state_received_) return;


    geometry::RobotState state;
    state.x     = robot_state_.x;
    state.y     = robot_state_.y;
    state.theta = robot_state_.theta;


    auto hits = lidar_->scan(state);

    auto scan_msg = sensor_msgs::msg::LaserScan();
    scan_msg.header.frame_id = "robot" + std::to_string(robot_id_);
    scan_msg.header.stamp    = this->now();

    scan_msg.angle_min       = ANGLE_MIN;
    scan_msg.angle_max       = ANGLE_MAX;
    scan_msg.angle_increment = (ANGLE_MAX - ANGLE_MIN) / (NUM_RAYS - 1);
    scan_msg.range_min       = 0.0;
    scan_msg.range_max       = MAX_DIST_PX * 0.05;

    scan_msg.ranges.resize(NUM_RAYS, scan_msg.range_max);


    for (const auto& hit : hits) {
        double dx   = hit.x - state.x;
        double dy   = hit.y - state.y;
        double dist = std::sqrt(dx*dx + dy*dy) * 0.05;

        double angle = std::atan2(dy, dx) - state.theta;
        while (angle >  M_PI) angle -= 2.0 * M_PI;
        while (angle < -M_PI) angle += 2.0 * M_PI;

        int idx = static_cast<int>(
            (angle - ANGLE_MIN) / scan_msg.angle_increment
        );

        if (idx >= 0 && idx < NUM_RAYS) {
            scan_msg.ranges[idx] = static_cast<float>(dist);
        }
    }

    scan_pub_->publish(scan_msg);
}

int main(int argc, char* argv[])
{
    rclcpp::init(argc, argv);
    int robot_id = 0;
    if (argc > 1) robot_id = std::stoi(argv[1]);
    rclcpp::spin(std::make_shared<LidarNode>(robot_id));
    rclcpp::shutdown();
    return 0;
}
