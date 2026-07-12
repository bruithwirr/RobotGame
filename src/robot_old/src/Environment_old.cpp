#include "environment/Environment_old.h"

#include <string>
#include <opencv2/opencv.hpp>

namespace environment {
    Environment::Environment(const Config& config)
        :resolution_(config.resolution), map_(cv::imread(config.map_filename,cv::IMREAD_GRAYSCALE)) {

            if (map_.empty()) {
                throw std::runtime_error("Nepodarilo sa načítať mapu: " + config.map_filename);
            }
    }

    bool Environment::isOccupied(double x, double y) const{
        double width  = map_.cols;
        double height = map_.rows;
        if (x < 0 || y < 0 || x > width || y > height) {
            return true;
        }
        double px = x;
        double py = y;

        return (map_.at<uint8_t>(py,px)==0);
    }

    double Environment::getWidth() const{
        return (map_.cols*resolution_);
    }

    double Environment::getHeight() const{
        return(map_.rows*resolution_);
    }
}