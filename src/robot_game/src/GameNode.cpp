#include "GameNode.h"
#include "ConfigParser.h"
#include <cmath>

GameNode::GameNode() : rclcpp::Node("game_node"),
    load_robot0_(0), load_robot1_(0), max_load_(5),
    score_robot0_(0), score_robot1_(0),
    duel_bot_mode_(false)
{
    GameConfig cfg;
    try {
        cfg = ConfigParser::load(
            "/home/bruithwirr/uni/ros2_ws/src/robot_game/config/game_config.yaml"
        );
        RCLCPP_INFO(this->get_logger(), "Konfiguracia nacitana. Mod: %s", cfg.game_mode.c_str());
    } catch (const ConfigException& e) {
        RCLCPP_FATAL(this->get_logger(), "Chyba konfigurácie: %s", e.what());
        rclcpp::shutdown();
        return;
    }

    duel_bot_mode_ = (cfg.game_mode == "duel_bot");

    environment::Config env_config;
    env_config.map_filename = cfg.map.filename;
    env_config.resolution   = cfg.map.resolution;
    env_ = std::make_shared<environment::Environment>(env_config);

    map_height_ = env_->getMap().rows;
    max_load_   = cfg.robot.max_capacity;
    station_    = {cfg.station.x, cfg.station.y, cfg.station.radius};

    circle_obstacles_ = cfg.obstacles.circles;
    rect_obstacles_   = cfg.obstacles.rectangles;

    waste::WasteConfig waste_config;
    waste_config.count      = cfg.waste.count;
    waste_config.min_radius = cfg.waste.min_radius;
    waste_config.max_radius = cfg.waste.max_radius;
    generator_ = std::make_unique<waste::WasteGenerator>(waste_config, env_);

    waste_items_ = generator_->generateBatch(waste_config.count);
    RCLCPP_INFO(this->get_logger(), "Vygenerovanych %zu odpadkov", waste_items_.size());

    sub_robot0_ = this->create_subscription<robot_msgs::msg::RobotState>(
        "/robot0_state", 10,
        std::bind(&GameNode::onRobot0State, this, std::placeholders::_1)
    );
    sub_robot1_ = this->create_subscription<robot_msgs::msg::RobotState>(
        "/robot1_state", 10,
        std::bind(&GameNode::onRobot1State, this, std::placeholders::_1)
    );

    waste_pub_ = this->create_publisher<visualization_msgs::msg::MarkerArray>(
        "/waste_markers", 10
    );
    obstacles_pub_ = this->create_publisher<visualization_msgs::msg::MarkerArray>(
        "/obstacle_markers", 10
    );
    station_pub_ = this->create_publisher<visualization_msgs::msg::Marker>(
        "/station_marker", 10
    );

    timer_ = this->create_wall_timer(
        std::chrono::milliseconds(100),
        std::bind(&GameNode::gameLoop, this)
    );

    RCLCPP_INFO(this->get_logger(),
        "GameNode spusteny. Stanica: (%.0f, %.0f). Max kapacita: %d. Bot mod: %s",
        station_.x, station_.y, max_load_, duel_bot_mode_ ? "ANO" : "NIE");
}

void GameNode::onRobot0State(const robot_msgs::msg::RobotState::SharedPtr msg)
{
    robot0_state_ = *msg;
    if (load_robot0_ < max_load_) checkCollection(robot0_state_, 0);
    checkUnload(robot0_state_, 0);
}

void GameNode::onRobot1State(const robot_msgs::msg::RobotState::SharedPtr msg)
{
    robot1_state_ = *msg;

    if (duel_bot_mode_) {
        checkCollection(robot1_state_, 1);
    } else {
        if (load_robot1_ < max_load_) checkCollection(robot1_state_, 1);
        checkUnload(robot1_state_, 1);
    }
}

void GameNode::checkCollection(const robot_msgs::msg::RobotState& state, int robot_id)
{
    int& load = (robot_id == 0) ? load_robot0_ : load_robot1_;
    bool is_bot = (robot_id == 1 && duel_bot_mode_);

    for (auto& item : waste_items_) {
        if (item->isCollected()) continue;
        if (!is_bot && load >= max_load_) break;

        double dx   = state.x - item->getX();
        double dy   = state.y - item->getY();
        double dist = std::sqrt(dx*dx + dy*dy);

        if (dist < COLLECT_RADIUS + item->getRadius()) {
            item->collect();
            if (!is_bot) load++;

            if (is_bot) {
                int& score = score_robot1_;
                score++;
                RCLCPP_INFO(this->get_logger(),
                    "Bot zobral %s! Skore bota: %d",
                    item->getType().c_str(), score);
            } else {
                RCLCPP_INFO(this->get_logger(),
                    "Robot %d zobral %s! Kapacita: %d/%d",
                    robot_id, item->getType().c_str(), load, max_load_);
                if (load >= max_load_) {
                    RCLCPP_WARN(this->get_logger(),
                        "Robot %d je PLNY! Presun sa na stanicu.", robot_id);
                }
            }

            try {
                waste_items_.push_back(generator_->generate());
            } catch (const std::exception& e) {
                RCLCPP_WARN(this->get_logger(), "%s", e.what());
            }
        }
    }

    waste_items_.erase(
        std::remove_if(waste_items_.begin(), waste_items_.end(),
            [](const auto& item) { return item->isCollected(); }),
        waste_items_.end()
    );
}

void GameNode::checkUnload(const robot_msgs::msg::RobotState& state, int robot_id)
{
    int& load  = (robot_id == 0) ? load_robot0_  : load_robot1_;
    int& score = (robot_id == 0) ? score_robot0_ : score_robot1_;

    if (load == 0) return;

    double dx   = state.x - station_.x;
    double dy   = state.y - station_.y;
    double dist = std::sqrt(dx*dx + dy*dy);

    if (dist < UNLOAD_RADIUS + station_.radius) {
        score += load;
        RCLCPP_INFO(this->get_logger(),
            "Robot %d vylozil odpad! +%d bodov. Skore: R0=%d R1=%d",
            robot_id, load, score_robot0_, score_robot1_);
        load = 0;
    }
}

void GameNode::gameLoop()
{
    publishWasteMarkers();
    publishStationMarker();
    publishObstacleMarkers();
}

void GameNode::publishWasteMarkers()
{
    auto marker_array = visualization_msgs::msg::MarkerArray();

    visualization_msgs::msg::Marker delete_marker;
    delete_marker.action          = visualization_msgs::msg::Marker::DELETEALL;
    delete_marker.header.frame_id = "map";
    delete_marker.header.stamp    = this->now();
    marker_array.markers.push_back(delete_marker);

    for (const auto& item : waste_items_) {
        if (item->isCollected()) continue;

        visualization_msgs::msg::Marker marker;
        marker.header.frame_id = "map";
        marker.header.stamp    = this->now();
        marker.ns              = "waste";
        marker.id              = item->getId();
        marker.type            = visualization_msgs::msg::Marker::CYLINDER;
        marker.action          = visualization_msgs::msg::Marker::ADD;

        marker.pose.position.x = item->getX() * 0.05;
        marker.pose.position.y = -(map_height_ - item->getY()) * 0.05;
        marker.pose.position.z = 0.1;
        marker.pose.orientation.w = 1.0;

        double diameter = item->getRadius() * 0.05 * 2.0;
        marker.scale.x = diameter;
        marker.scale.y = diameter;
        marker.scale.z = 0.1;

        auto color = item->getColor();
        marker.color.r = color[0];
        marker.color.g = color[1];
        marker.color.b = color[2];
        marker.color.a = 1.0;

        marker_array.markers.push_back(marker);
    }

    waste_pub_->publish(marker_array);
}

void GameNode::publishStationMarker()
{
    visualization_msgs::msg::Marker marker;
    marker.header.frame_id = "map";
    marker.header.stamp    = this->now();
    marker.ns              = "station";
    marker.id              = 0;
    marker.type            = visualization_msgs::msg::Marker::CYLINDER;
    marker.action          = visualization_msgs::msg::Marker::ADD;

    marker.pose.position.x = station_.x * 0.05;
    marker.pose.position.y = -(map_height_ - station_.y) * 0.05;
    marker.pose.position.z = 0.05;
    marker.pose.orientation.w = 1.0;

    double diameter = station_.radius * 0.05 * 2.0;
    marker.scale.x = diameter;
    marker.scale.y = diameter;
    marker.scale.z = 0.05;

    marker.color.r = 0.0;
    marker.color.g = 1.0;
    marker.color.b = 0.0;
    marker.color.a = 0.7;

    station_pub_->publish(marker);
}

void GameNode::publishObstacleMarkers()
{
    auto marker_array = visualization_msgs::msg::MarkerArray();
    int id = 0;

    for (const auto& obs : circle_obstacles_) {
        visualization_msgs::msg::Marker marker;
        marker.header.frame_id = "map";
        marker.header.stamp    = this->now();
        marker.ns              = "obstacles_circle";
        marker.id              = id++;
        marker.type            = visualization_msgs::msg::Marker::CYLINDER;
        marker.action          = visualization_msgs::msg::Marker::ADD;
        marker.pose.position.x = obs.x * 0.05;
        marker.pose.position.y = -(map_height_ - obs.y) * 0.05;
        marker.pose.position.z = 0.1;
        marker.pose.orientation.w = 1.0;
        double diameter = obs.radius * 0.05 * 2.0;
        marker.scale.x = diameter;
        marker.scale.y = diameter;
        marker.scale.z = 0.2;
        marker.color.r = 1.0; marker.color.g = 0.0;
        marker.color.b = 0.0; marker.color.a = 0.8;
        marker_array.markers.push_back(marker);
    }

    for (const auto& obs : rect_obstacles_) {
        visualization_msgs::msg::Marker marker;
        marker.header.frame_id = "map";
        marker.header.stamp    = this->now();
        marker.ns              = "obstacles_rect";
        marker.id              = id++;
        marker.type            = visualization_msgs::msg::Marker::CUBE;
        marker.action          = visualization_msgs::msg::Marker::ADD;
        marker.pose.position.x = obs.x * 0.05;
        marker.pose.position.y = -(map_height_ - obs.y) * 0.05;
        marker.pose.position.z = 0.1;
        marker.pose.orientation.w = 1.0;
        marker.scale.x = obs.width  * 0.05;
        marker.scale.y = obs.height * 0.05;
        marker.scale.z = 0.2;
        marker.color.r = 1.0; marker.color.g = 0.0;
        marker.color.b = 0.0; marker.color.a = 0.8;
        marker_array.markers.push_back(marker);
    }

    obstacles_pub_->publish(marker_array);
}

int main(int argc, char* argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<GameNode>());
    rclcpp::shutdown();
    return 0;
}
