#include "RobotNode.h"
#include "ConfigParser.h"
#include "types/Geometry.h"
#include <cmath>

RobotNode::RobotNode(int robot_id)
    : rclcpp::Node("robot_node_" + std::to_string(robot_id)),
      robot_id_(robot_id), robot_radius_(10.0)
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
    cfg_ = cfg;
    robot_radius_ = cfg.robot.radius;

    environment::Config env_config;
    env_config.map_filename = cfg.map.filename;
    env_config.resolution   = cfg.map.resolution;
    env_ = std::make_shared<environment::Environment>(env_config);

    robot::Config robot_config;
    robot_config.simulation_period_ms = 25;
    if (robot_id_ == 1 && cfg.game_mode == "duel_bot") {
        robot_config.accelerations           = {1000.0, 1000.0};
        robot_config.emergency_decelerations = {1000.0, 1000.0};
    } else {
        robot_config.accelerations           = {100.0, 100.0};
        robot_config.emergency_decelerations = {200.0, 200.0};
    }



    double start_x = cfg.robot.start_positions[robot_id_].x;
    double start_y = cfg.robot.start_positions[robot_id_].y;

    robot_ = std::make_shared<robot::Robot>(
        robot_config,
        geometry::RobotState{start_x, start_y, 0.0, {0.0, 0.0}},
        nullptr
    );

    std::string cmd_topic = "/robot" + std::to_string(robot_id_) + "_control";
    subscriber_ = this->create_subscription<geometry_msgs::msg::Twist>(
        cmd_topic, 10,
        std::bind(&RobotNode::rcv_message, this, std::placeholders::_1)
    );

    publisher_ = this->create_publisher<robot_msgs::msg::RobotState>(
        "/robot" + std::to_string(robot_id_) + "_state", 10
    );

    rviz_marker_ = this->create_publisher<visualization_msgs::msg::Marker>(
        "/robot" + std::to_string(robot_id_) + "/marker", 10
    );
    arrow_marker_ = this->create_publisher<visualization_msgs::msg::Marker>(
        "/robot" + std::to_string(robot_id_) + "/arrow", 10
    );

    if (robot_id_ == 0) {
        auto qos = rclcpp::QoS(1).transient_local();
        map_pub_ = this->create_publisher<nav_msgs::msg::OccupancyGrid>("/map", qos);

        map_timer_ = this->create_wall_timer(
            std::chrono::seconds(1),
            [this]() {
                publishMap();
                map_timer_->cancel();
            }
        );
    }

    timer_ = this->create_wall_timer(
        std::chrono::milliseconds(50),
        std::bind(&RobotNode::publishState, this)
    );

    RCLCPP_INFO(this->get_logger(),
        "RobotNode %d spusteny na (%.0f, %.0f) polomer=%.0f",
        robot_id_, start_x, start_y, robot_radius_);
}

void RobotNode::checkCollision()
{
    geometry::RobotState state = robot_->getState();
    double r = robot_radius_;

    if (env_->isOccupied(state.x + r, state.y)) { robot_->stop(); robot_->move(-1.0, 0.0); }
    if (env_->isOccupied(state.x - r, state.y)) { robot_->stop(); robot_->move( 1.0, 0.0); }
    if (env_->isOccupied(state.x, state.y - r)) { robot_->stop(); robot_->move(0.0,  1.0); }
    if (env_->isOccupied(state.x, state.y + r)) { robot_->stop(); robot_->move(0.0, -1.0); }
}

void RobotNode::publishState()
{
    if (!(robot_id_ == 1 && cfg_.game_mode == "duel_bot")) {
        checkCollision();
    }

    geometry::RobotState state = robot_->getState();

    auto msg = robot_msgs::msg::RobotState();
    msg.x                = state.x;
    msg.y                = state.y;
    msg.theta            = state.theta;
    msg.linear_velocity  = state.velocity.linear;
    msg.angular_velocity = state.velocity.angular;
    publisher_->publish(msg);

    double map_height = env_->getMap().rows;

    auto marker = visualization_msgs::msg::Marker();
    marker.header.frame_id = "map";
    marker.header.stamp    = this->now();
    marker.ns              = "robots";
    marker.id              = robot_id_;
    marker.type            = visualization_msgs::msg::Marker::SPHERE;
    marker.action          = visualization_msgs::msg::Marker::ADD;
    marker.pose.position.x = state.x * 0.05;
    marker.pose.position.y = -(map_height - state.y) * 0.05;
    marker.pose.position.z = 0.25;
    marker.pose.orientation.w = 1.0;
    marker.scale.x = 0.5;
    marker.scale.y = 0.5;
    marker.scale.z = 0.5;
    if (robot_id_ == 0) { marker.color.r=0.0; marker.color.g=0.0; marker.color.b=1.0; }
    else                 { marker.color.r=1.0; marker.color.g=0.0; marker.color.b=0.0; }
    marker.color.a = 1.0;
    rviz_marker_->publish(marker);

    auto arrow = visualization_msgs::msg::Marker();
    arrow.header.frame_id = "map";
    arrow.header.stamp    = this->now();
    arrow.ns              = "robot_direction";
    arrow.id              = robot_id_;
    arrow.type            = visualization_msgs::msg::Marker::ARROW;
    arrow.action          = visualization_msgs::msg::Marker::ADD;
    arrow.pose.position.x = marker.pose.position.x;
    arrow.pose.position.y = marker.pose.position.y;
    arrow.pose.position.z = 0.5;
    double half_theta = state.theta / 2.0;
    arrow.pose.orientation.x = 0.0;
    arrow.pose.orientation.y = 0.0;
    arrow.pose.orientation.z = std::sin(half_theta);
    arrow.pose.orientation.w = std::cos(half_theta);
    arrow.scale.x = 1.0;
    arrow.scale.y = 0.25;
    arrow.scale.z = 0.25;
    if (robot_id_ == 0) { arrow.color.r=0.0; arrow.color.g=0.5; arrow.color.b=1.0; }
    else                 { arrow.color.r=1.0; arrow.color.g=0.5; arrow.color.b=0.0; }
    arrow.color.a = 1.0;
    arrow_marker_->publish(arrow);
}

void RobotNode::publishMap()
{
    cv::Mat map = env_->getMap();
    if (map.empty()) { RCLCPP_ERROR(this->get_logger(), "Mapa je prazdna!"); return; }

    auto msg = nav_msgs::msg::OccupancyGrid();
    msg.header.frame_id = "map";
    msg.header.stamp    = this->now();
    msg.info.width      = map.cols;
    msg.info.height     = map.rows;
    msg.info.resolution = 0.05;
    msg.info.origin.position.x  = 0.0;
    msg.info.origin.position.y  = -map.rows * 0.05;
    msg.info.origin.position.z  = 0.0;
    msg.info.origin.orientation.w = 1.0;

    cv::Mat gray;
    if (map.channels() == 3) cv::cvtColor(map, gray, cv::COLOR_BGR2GRAY);
    else gray = map;

    msg.data.resize(gray.rows * gray.cols);
    for (int y = 0; y < gray.rows; y++)
        for (int x = 0; x < gray.cols; x++)
            msg.data[y * gray.cols + x] = (gray.at<uchar>(y,x) < 128) ? 100 : 0;

    map_pub_->publish(msg);
}

void RobotNode::rcv_message(const geometry_msgs::msg::Twist::SharedPtr msg)
{
    geometry::Twist velocity;
    velocity.linear  = msg->linear.x;
    velocity.angular = msg->angular.z;
    robot_->setDesiredVelocity(velocity);
}

int main(int argc, char* argv[])
{
    rclcpp::init(argc, argv);
    int robot_id = 0;
    if (argc > 1) robot_id = std::stoi(argv[1]);
    rclcpp::spin(std::make_shared<RobotNode>(robot_id));
    rclcpp::shutdown();
    return 0;
}
