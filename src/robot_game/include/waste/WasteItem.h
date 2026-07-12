#pragma once
#include <string>

namespace waste {

    class WasteItem {
    public:
        WasteItem(int id, double x, double y, double radius)
            : id_(id), x_(x), y_(y), radius_(radius), collected_(false) {}

        virtual ~WasteItem() = default;

        virtual std::string getType() const = 0;
        virtual std::array<float, 3> getColor() const = 0;

        int    getId()      const { return id_; }
        double getX()       const { return x_; }
        double getY()       const { return y_; }
        double getRadius()  const { return radius_; }
        bool   isCollected() const { return collected_; }

        void collect() { collected_ = true; }

    protected:
        int    id_;
        double x_;
        double y_;
        double radius_;
        bool   collected_;
    };


    class Paper : public WasteItem {
    public:
        using WasteItem::WasteItem;
        std::string getType() const override { return "paper"; }
        std::array<float, 3> getColor() const override { return {1.0f, 1.0f, 0.0f}; } // yellow
    };


    class Plastic : public WasteItem {
    public:
        using WasteItem::WasteItem;
        std::string getType() const override { return "plastic"; }
        std::array<float, 3> getColor() const override { return {0.0f, 1.0f, 1.0f}; } // cyan
    };


    class Glass : public WasteItem {
    public:
        using WasteItem::WasteItem;
        std::string getType() const override { return "glass"; }
        std::array<float, 3> getColor() const override { return {0.0f, 1.0f, 0.0f}; } // green
    };

} // namespace waste