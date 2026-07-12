#include "environment/Lidar_old.h"

#include <vector>
#include <memory>

#include "types/Geometry.h"
#include "environment/Environment_old.h"

namespace lidar {
    Lidar::Lidar(const Config& config, std::shared_ptr<environment::Environment> env)
        :config_(config), env_(env){}

    std::vector<geometry::Point2d> Lidar::scan(const geometry::RobotState& state) const{
        if (config_.beam_count <= 0) {
            throw std::runtime_error("beam_count nesmie byť <= 0");
        }
        double first_ray_angle_absolute= state.theta+config_.first_ray_angle-M_PI/2;
        double last_ray_angle_absolute= state.theta+config_.last_ray_angle-M_PI/2;
        double angle_step=(config_.last_ray_angle-config_.first_ray_angle)/config_.beam_count;
        double step=0.05;
        geometry::Point2d ray_state;
        std::vector<geometry::Point2d> hits;
        for (double ray=first_ray_angle_absolute; ray<=last_ray_angle_absolute; ray+=angle_step) {
            ray_state.x=0;
            ray_state.y=0;
            for (double current_ray_length=0;current_ray_length<=config_.max_range;current_ray_length+=step) {
                ray_state.x=state.x+current_ray_length*std::cos(ray);
                ray_state.y=state.y+current_ray_length*std::sin(ray);

                if (env_->isOccupied(ray_state.x,ray_state.y)) {
                    hits.push_back(ray_state);
                    break;
                }
            }
        }
        return hits;

    }
}