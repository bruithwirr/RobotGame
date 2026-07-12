#include "environment/Canvas.h"

#include <memory>


namespace canvas {
    Canvas::Canvas(std::string filename, double resolution)
    :filename_(filename),resolution_(resolution),image_ptr_(nullptr){}


    std::shared_ptr<environment::Environment> Canvas::createEnvironment() {
        environment::Config config_map;
        const environment::Config& config_map_ref=config_map;
        config_map.map_filename = filename_;
        config_map.resolution = resolution_;

        environment::Environment environment(config_map_ref);
        std::shared_ptr<environment::Environment> environment_ptr=std::make_shared<environment::Environment>(environment);

        image_ptr_=std::make_shared<cv::Mat>(environment.getMap());

        base_image_=environment.getMap();
        cv::cvtColor(base_image_, base_image_, cv::COLOR_GRAY2BGR);
        return environment_ptr;
    }






void Canvas::redraw(geometry::RobotState robot_state,std::vector<geometry::Point2d>& lidar_hits) {
        if (image_ptr_ == nullptr) {
            throw std::runtime_error("Obraz nebol inicializovaný — zavolaj createEnvironment()");
        }
        base_image_.copyTo(*image_ptr_);
        for (const auto hit:lidar_hits) {
            cv::circle(*image_ptr_,cv::Point(hit.x,hit.y),3,cv::Scalar(0, 0, 255),-1);
        }
        cv::circle(*image_ptr_,cv::Point(robot_state.x,robot_state.y),10,cv::Scalar(0, 255, 0),-1);
        cv::imshow(" ",*image_ptr_);
    }
}