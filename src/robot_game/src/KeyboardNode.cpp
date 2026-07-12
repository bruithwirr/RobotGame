#include "KeyboardNode.h"
#include <X11/keysym.h>

KeyboardNode::KeyboardNode() : rclcpp::Node("KeyboardNode")
{
    robot1_control_ = this->create_publisher<geometry_msgs::msg::Twist>(
        "/robot0_control", 10
    );
    robot2_control_ = this->create_publisher<geometry_msgs::msg::Twist>(
        "/robot1_control", 10
    );

    display_ = XOpenDisplay(nullptr);
    if (!display_) {
        RCLCPP_ERROR(this->get_logger(), "Nepodarilo sa otvorit X11 display!");
        rclcpp::shutdown();
        return;
    }

    timer_ = this->create_wall_timer(
        std::chrono::milliseconds(50),
        std::bind(&KeyboardNode::timerCallback, this)
    );

    RCLCPP_INFO(this->get_logger(), "Player 1: WASD  Player 2: IJKL  Exit: ESC");
}

KeyboardNode::~KeyboardNode()
{
    if (display_) XCloseDisplay(display_);
}

bool KeyboardNode::isKeyPressed(int keycode)
{
    char keys[32];
    XQueryKeymap(display_, keys);
    return (keys[keycode / 8] & (1 << (keycode % 8))) != 0;
}

void KeyboardNode::timerCallback()
{
    auto kc = [this](KeySym sym) {
        return XKeysymToKeycode(display_, sym);
    };

    auto msg1 = geometry_msgs::msg::Twist();
    auto msg2 = geometry_msgs::msg::Twist();

    if (isKeyPressed(kc(XK_w))) msg1.linear.x  =  70.0;
    if (isKeyPressed(kc(XK_s))) msg1.linear.x  = -70.0;
    if (isKeyPressed(kc(XK_a))) msg1.angular.z =  70.0;
    if (isKeyPressed(kc(XK_d))) msg1.angular.z = -70.0;

    if (isKeyPressed(kc(XK_i))) msg2.linear.x  =  70.0;
    if (isKeyPressed(kc(XK_k))) msg2.linear.x  = -70.0;
    if (isKeyPressed(kc(XK_j))) msg2.angular.z =  70.0;
    if (isKeyPressed(kc(XK_l))) msg2.angular.z = -70.0;

    if (isKeyPressed(kc(XK_Escape))) {
        rclcpp::shutdown();
        return;
    }

    robot1_control_->publish(msg1);
    robot2_control_->publish(msg2);
}

int main(int argc, char* argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<KeyboardNode>());
    rclcpp::shutdown();
    return 0;
}
