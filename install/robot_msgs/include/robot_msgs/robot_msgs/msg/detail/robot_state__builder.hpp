// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from robot_msgs:msg/RobotState.idl
// generated code does not contain a copyright notice

#ifndef ROBOT_MSGS__MSG__DETAIL__ROBOT_STATE__BUILDER_HPP_
#define ROBOT_MSGS__MSG__DETAIL__ROBOT_STATE__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "robot_msgs/msg/detail/robot_state__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace robot_msgs
{

namespace msg
{

namespace builder
{

class Init_RobotState_max_load
{
public:
  explicit Init_RobotState_max_load(::robot_msgs::msg::RobotState & msg)
  : msg_(msg)
  {}
  ::robot_msgs::msg::RobotState max_load(::robot_msgs::msg::RobotState::_max_load_type arg)
  {
    msg_.max_load = std::move(arg);
    return std::move(msg_);
  }

private:
  ::robot_msgs::msg::RobotState msg_;
};

class Init_RobotState_current_load
{
public:
  explicit Init_RobotState_current_load(::robot_msgs::msg::RobotState & msg)
  : msg_(msg)
  {}
  Init_RobotState_max_load current_load(::robot_msgs::msg::RobotState::_current_load_type arg)
  {
    msg_.current_load = std::move(arg);
    return Init_RobotState_max_load(msg_);
  }

private:
  ::robot_msgs::msg::RobotState msg_;
};

class Init_RobotState_angular_velocity
{
public:
  explicit Init_RobotState_angular_velocity(::robot_msgs::msg::RobotState & msg)
  : msg_(msg)
  {}
  Init_RobotState_current_load angular_velocity(::robot_msgs::msg::RobotState::_angular_velocity_type arg)
  {
    msg_.angular_velocity = std::move(arg);
    return Init_RobotState_current_load(msg_);
  }

private:
  ::robot_msgs::msg::RobotState msg_;
};

class Init_RobotState_linear_velocity
{
public:
  explicit Init_RobotState_linear_velocity(::robot_msgs::msg::RobotState & msg)
  : msg_(msg)
  {}
  Init_RobotState_angular_velocity linear_velocity(::robot_msgs::msg::RobotState::_linear_velocity_type arg)
  {
    msg_.linear_velocity = std::move(arg);
    return Init_RobotState_angular_velocity(msg_);
  }

private:
  ::robot_msgs::msg::RobotState msg_;
};

class Init_RobotState_theta
{
public:
  explicit Init_RobotState_theta(::robot_msgs::msg::RobotState & msg)
  : msg_(msg)
  {}
  Init_RobotState_linear_velocity theta(::robot_msgs::msg::RobotState::_theta_type arg)
  {
    msg_.theta = std::move(arg);
    return Init_RobotState_linear_velocity(msg_);
  }

private:
  ::robot_msgs::msg::RobotState msg_;
};

class Init_RobotState_y
{
public:
  explicit Init_RobotState_y(::robot_msgs::msg::RobotState & msg)
  : msg_(msg)
  {}
  Init_RobotState_theta y(::robot_msgs::msg::RobotState::_y_type arg)
  {
    msg_.y = std::move(arg);
    return Init_RobotState_theta(msg_);
  }

private:
  ::robot_msgs::msg::RobotState msg_;
};

class Init_RobotState_x
{
public:
  Init_RobotState_x()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_RobotState_y x(::robot_msgs::msg::RobotState::_x_type arg)
  {
    msg_.x = std::move(arg);
    return Init_RobotState_y(msg_);
  }

private:
  ::robot_msgs::msg::RobotState msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::robot_msgs::msg::RobotState>()
{
  return robot_msgs::msg::builder::Init_RobotState_x();
}

}  // namespace robot_msgs

#endif  // ROBOT_MSGS__MSG__DETAIL__ROBOT_STATE__BUILDER_HPP_
