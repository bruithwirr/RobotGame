#pragma once
#include <memory>
#include <opencv2/opencv.hpp>
#include <types/Geometry.h>
#include "environment/Lidar_old.h"
#include <string>

namespace canvas {
    class Canvas {
    public:
        Canvas(std::string filename, double resolution);
        std::shared_ptr<environment::Environment> createEnvironment();
        void createLidar(double max_range,
                    double beam_count,
                    double first_ray_angle,
                    double last_ray_angle,
                    std::shared_ptr<environment::Environment> env);
        void setEnvPtr(std::shared_ptr<environment::Environment> env_ptr);
        void setLidarPtr(std::shared_ptr<lidar::Lidar> lidar_ptr);
        void addRobot(double x, double y, double theta);
        void showImage();
    private:
        std::string filename_;
        double resolution_;
        std::shared_ptr<lidar::Lidar> lidar_ptr_;
        std::shared_ptr<cv::Mat> image_ptr_;
    };
}
