#pragma once
#include "WasteItem.h"
#include <memory>
#include <stdexcept>

namespace waste {

    class WasteFactory {
    public:
        enum class Type { PAPER, PLASTIC, GLASS };

        static std::shared_ptr<WasteItem> create(
            Type type, int id, double x, double y, double radius)
        {
            switch (type) {
                case Type::PAPER:   return std::make_shared<Paper>(id, x, y, radius);
                case Type::PLASTIC: return std::make_shared<Plastic>(id, x, y, radius);
                case Type::GLASS:   return std::make_shared<Glass>(id, x, y, radius);
                default:
                    throw std::invalid_argument("Neznamy typ odpadku");
            }
        }

        static Type randomType() {
            return static_cast<Type>(rand() % 3);
        }
    };

} // namespace waste