#pragma once
#include "WasteItem.h"
#include "WasteFactory.h"
#include "environment/Environment.h"
#include <vector>
#include <memory>
#include <random>

namespace waste {

    struct WasteConfig {
        int    count;
        double min_radius;
        double max_radius;
    };

    class WasteGenerator {
    public:
        WasteGenerator(
            const WasteConfig& config,
            std::shared_ptr<environment::Environment> env)
            : config_(config), env_(env), next_id_(0),
              rng_(std::random_device{}()) {}


        std::shared_ptr<WasteItem> generate()
        {
            double x, y;
            int attempts = 0;
            do {
                x = randomDouble(10.0, env_->getWidth()  / 0.05 - 10.0);
                y = randomDouble(10.0, env_->getHeight() / 0.05 - 10.0);
                attempts++;
                if (attempts > 1000) {
                    throw std::runtime_error("Nepodarilo sa najst volne miesto pre odpadok");
                }
            } while (env_->isOccupied(x, y));

            double radius = randomDouble(config_.min_radius, config_.max_radius);
            auto type     = WasteFactory::randomType();

            return WasteFactory::create(type, next_id_++, x, y, radius);
        }


        std::vector<std::shared_ptr<WasteItem>> generateBatch(int count)
        {
            std::vector<std::shared_ptr<WasteItem>> items;
            for (int i = 0; i < count; i++) {
                items.push_back(generate());
            }
            return items;
        }

    private:
        double randomDouble(double min, double max) {
            std::uniform_real_distribution<double> dist(min, max);
            return dist(rng_);
        }

        WasteConfig config_;
        std::shared_ptr<environment::Environment> env_;
        int next_id_;
        std::mt19937 rng_;
    };

} // namespace waste