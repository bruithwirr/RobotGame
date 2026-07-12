#include "robot/Robot.h"
#include <thread>
#include <opencv2/highgui.hpp>

namespace robot {

    Robot::Robot(const Config& config,
        geometry::RobotState initial_state,
        const CollisionCb& collision_cb)
        :config_(config),
        robot_state_(initial_state),
        robot_velocity_{0.0, 0.0},
        interrupted_(false),
        collision_cb_(collision_cb),
        desired_velocity_({0.0,0.0}) {
        thread_ = std::thread(&Robot::robotThreadFcn, this);
    }

    Robot::~Robot() {
        interrupted_=true;
        thread_.join();
    }

    geometry::RobotState Robot::getState() const{
        std::lock_guard<std::mutex> lock(mutex_);
        return robot_state_;
    }

    void Robot::setVelocity(const geometry::Twist& velocity){
        robot_velocity_=velocity;
    }

    bool Robot::isInCollision() const {
        if (collision_cb_ == nullptr) return false;
        return collision_cb_(robot_state_);
    }

    void Robot::setDesiredVelocity(geometry::Twist velocity) {
        std::lock_guard<std::mutex> lock(mutex_);
        desired_velocity_=velocity;
    }

    void Robot::update(const geometry::Twist& velocity, double dt) {
        if (robot_velocity_.linear<velocity.linear) {
            robot_velocity_.linear += config_.accelerations.linear*dt;
        }
        else if(robot_velocity_.linear>velocity.linear) {
            robot_velocity_.linear -= config_.emergency_decelerations.linear*dt;
        }

        if (velocity.angular-robot_velocity_.angular>0.01) {
            robot_velocity_.angular += config_.accelerations.angular*dt;
        }
        else if(velocity.angular-robot_velocity_.angular<-0.01) {
            robot_velocity_.angular -= config_.emergency_decelerations.angular*dt;
        }
        robot_state_.theta += (robot_velocity_.angular*dt)*M_PI/180;
        robot_state_.x += robot_velocity_.linear*dt*cos(robot_state_.theta);
        robot_state_.y += robot_velocity_.linear*dt*sin(robot_state_.theta);
    }

    // Pôvodný robotThreadFcn — bez kolíznej logiky
    void Robot::robotThreadFcn() {
        while (!interrupted_) {
            double dt = config_.simulation_period_ms /1000.0;
            {
                std::lock_guard<std::mutex> lock(mutex_);
                update(desired_velocity_,dt);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(config_.simulation_period_ms));
        }
    }

    void Robot::stop() {
        robot_velocity_.linear=0.0;
    }

    void Robot::move(double dx, double dy) {
        robot_state_.x+=dx;
        robot_state_.y+=dy;
    }
}
