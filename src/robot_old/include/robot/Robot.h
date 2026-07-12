#pragma once

#include <functional>
#include <thread>

#include "types/Geometry.h"
#include "environment/Environment.h"
#include "environment/Lidar.h"

namespace robot {

struct Config {
    geometry::Twist accelerations;
    geometry::Twist emergency_decelerations;
    double command_duration;
    int simulation_period_ms;
};


class Robot {
public:
    using CollisionCb = std::function<bool(geometry::RobotState)>;

    Robot(const Config& config,
        geometry::RobotState initial_state = geometry::RobotState{200.0, 200.0, 0.0},
        const CollisionCb& collision_cb = nullptr
        );

    ~Robot();
    void stop();
    void move(double dx, double dy);
    void setVelocity(const geometry::Twist& velocity);
    geometry::RobotState getState() const;
    bool isInCollision() const;
    void robotThreadFcn();
    void setDesiredVelocity(geometry::Twist velocity);
protected:
    void update(const geometry::Twist& velocity, double dt);
private:
    Config config_;
    geometry::RobotState robot_state_;
    geometry::Twist robot_velocity_;
    bool interrupted_;
    CollisionCb collision_cb_;
    std::thread thread_;
    geometry::Twist desired_velocity_;
    mutable std::mutex mutex_;
};
} // namespace robot

