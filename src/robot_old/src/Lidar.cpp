#include "environment/Lidar.h"

#include <vector>
#include <memory>

#include "types/Geometry.h"
#include "environment/Environment.h"

namespace lidar {
    Lidar::Lidar(const Config& config, std::shared_ptr<environment::Environment> env)
        :config_(config), env_(env){}

    std::vector<geometry::Point2d> Lidar::scan(const geometry::RobotState& state) const{
        if (config_.beam_count <= 0) {
            throw std::runtime_error("beam_count nesmie byť <= 0");
        }
        double first_ray_angle_absolute= state.theta+config_.first_ray_angle;
        double last_ray_angle_absolute= state.theta+config_.last_ray_angle;
        double step=0.05;
        geometry::Point2d ray_state;
        std::vector<geometry::Point2d> hits;
        double angle_step = 0.0;
        if (config_.beam_count > 1) {
            angle_step = (config_.last_ray_angle - config_.first_ray_angle)
                       / (config_.beam_count - 1);
        }

        for (int i = 0; i < config_.beam_count; i++) {
            double ray = state.theta + config_.first_ray_angle + i * angle_step;

            geometry::Point2d ray_state;
            for (double dist = 0.0; dist <= config_.max_range; dist += step) {
                ray_state.x = state.x + dist * std::cos(ray);
                ray_state.y = state.y + dist * std::sin(ray);

                if (env_->isOccupied(ray_state.x, ray_state.y)) {
                    hits.push_back(ray_state);
                    break;
                }
            }
        }

        return hits;
    }
}