#include "environment/Canvas_old.h"

#include <memory>


namespace canvas {
    Canvas::Canvas(std::string filename, double resolution)
    :filename_(filename),resolution_(resolution),image_ptr_(nullptr),lidar_ptr_(nullptr) {}

    void Canvas::setLidarPtr(std::shared_ptr<lidar::Lidar> lidar_ptr) {
        lidar_ptr_ = lidar_ptr;
    }

    std::shared_ptr<environment::Environment> Canvas::createEnvironment() {
        environment::Config config_map;
        const environment::Config& config_map_ref=config_map;
        config_map.map_filename = filename_;
        config_map.resolution = resolution_;

        environment::Environment environment(config_map_ref);
        std::shared_ptr<environment::Environment> environment_ptr=std::make_shared<environment::Environment>(environment);
        image_ptr_ = std::make_shared<cv::Mat>(environment_ptr->getMap());
        cv::cvtColor(*image_ptr_, *image_ptr_, cv::COLOR_GRAY2BGR);
        return environment_ptr;
    }


    void Canvas::createLidar(double max_range,
                    double beam_count,
                    double first_ray_angle,
                    double last_ray_angle,
                    std::shared_ptr<environment::Environment> environment_ptr)
    {
        lidar::Config config_lidar;
        config_lidar.max_range=max_range;
        config_lidar.beam_count=beam_count;
        config_lidar.first_ray_angle=first_ray_angle;
        config_lidar.last_ray_angle=last_ray_angle;
        const lidar::Config& config_lidar_ref=config_lidar;
        lidar::Lidar lidar(config_lidar_ref,environment_ptr);
        std::shared_ptr<lidar::Lidar> lidar_ptr=std::make_shared<lidar::Lidar>(lidar);
        lidar_ptr_=lidar_ptr;
    }

    void Canvas::addRobot(double x,double y, double theta) {
        if (lidar_ptr_ == nullptr) {
            throw std::runtime_error("Lidar nebol inicializovaný — zavolaj createLidar()");
        }
        geometry::RobotState new_robot;
        geometry::RobotState& new_robot_ref = new_robot;
        new_robot.x=x;
        new_robot.y=y;
        new_robot.theta=theta;
        std::vector<geometry::Point2d> hits = lidar_ptr_->scan(new_robot_ref);

        for (const auto hit:hits) {
            cv::circle(*image_ptr_,cv::Point(hit.x,hit.y),3,cv::Scalar(0, 0, 255),-1);
        }
        cv::circle(*image_ptr_,cv::Point(new_robot.x,new_robot.y),10,cv::Scalar(0, 255, 0),-1);
    }

void Canvas::showImage() {
        if (image_ptr_ == nullptr) {
            throw std::runtime_error("Obraz nebol inicializovaný — zavolaj createEnvironment()");
        }
        cv::imshow(" ",*image_ptr_);
        cv::waitKey();
    }
}