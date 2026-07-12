#pragma once
#include <memory>
#include <opencv2/opencv.hpp>
#include <types/Geometry.h>
#include "environment/Lidar.h"
#include <string>

namespace canvas {
    class Canvas {
    public:
        Canvas(std::string filename, double resolution);
        std::shared_ptr<environment::Environment> createEnvironment();

        void addRobot(double x, double y, double theta);
        void redraw(geometry::RobotState robot_state,std::vector<geometry::Point2d>& lidar_hits);
    private:
        std::string filename_;
        double resolution_;
        cv::Mat base_image_;
        std::shared_ptr<cv::Mat> image_ptr_;
    };
}
