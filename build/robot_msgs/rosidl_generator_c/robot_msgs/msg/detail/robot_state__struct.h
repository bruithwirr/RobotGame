// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from robot_msgs:msg/RobotState.idl
// generated code does not contain a copyright notice

#ifndef ROBOT_MSGS__MSG__DETAIL__ROBOT_STATE__STRUCT_H_
#define ROBOT_MSGS__MSG__DETAIL__ROBOT_STATE__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

/// Struct defined in msg/RobotState in the package robot_msgs.
typedef struct robot_msgs__msg__RobotState
{
  double x;
  double y;
  double theta;
  double linear_velocity;
  double angular_velocity;
  int8_t current_load;
  int8_t max_load;
} robot_msgs__msg__RobotState;

// Struct for a sequence of robot_msgs__msg__RobotState.
typedef struct robot_msgs__msg__RobotState__Sequence
{
  robot_msgs__msg__RobotState * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} robot_msgs__msg__RobotState__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // ROBOT_MSGS__MSG__DETAIL__ROBOT_STATE__STRUCT_H_
