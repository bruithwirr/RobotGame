cmake_minimum_required(VERSION 3.8)
project(robot_game)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(robot_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(visualization_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(OpenCV REQUIRED)
find_package(X11 REQUIRED)
find_package(yaml-cpp REQUIRED)

set(ROBOT_OLD_INCLUDE
  ${CMAKE_CURRENT_SOURCE_DIR}/../robot_old/include
)


add_executable(keyboard_node
  src/KeyboardNode.cpp
)
target_include_directories(keyboard_node PUBLIC
  include
  ${X11_INCLUDE_DIR}
)
ament_target_dependencies(keyboard_node
  rclcpp
  geometry_msgs
)
target_link_libraries(keyboard_node
  ${X11_LIBRARIES}
)


add_executable(robot_node
  src/RobotNode.cpp
  src/ConfigParser.cpp
  ../robot_old/src/Robot.cpp
  ../robot_old/src/Environment.cpp
  ../robot_old/src/Lidar.cpp
  ../robot_old/src/Canvas.cpp
)
target_include_directories(robot_node PUBLIC
  include
  ${ROBOT_OLD_INCLUDE}
  ${OpenCV_INCLUDE_DIRS}
)
ament_target_dependencies(robot_node
  rclcpp
  robot_msgs
  geometry_msgs
  visualization_msgs
  nav_msgs
)
target_link_libraries(robot_node
  ${OpenCV_LIBS}
  yaml-cpp
)


add_executable(game_node
  src/GameNode.cpp
  src/ConfigParser.cpp
  ../robot_old/src/Environment.cpp
)
target_include_directories(game_node PUBLIC
  include
  ${ROBOT_OLD_INCLUDE}
  ${OpenCV_INCLUDE_DIRS}
)
ament_target_dependencies(game_node
  rclcpp
  robot_msgs
  geometry_msgs
  visualization_msgs
)
target_link_libraries(game_node
  ${OpenCV_LIBS}
  yaml-cpp
)


add_executable(bot_node
  src/BotNode.cpp
  src/ConfigParser.cpp
)
target_include_directories(bot_node PUBLIC
  include
  ${ROBOT_OLD_INCLUDE}
  ${OpenCV_INCLUDE_DIRS}
)
ament_target_dependencies(bot_node
  rclcpp
  robot_msgs
  geometry_msgs
  visualization_msgs
)
target_link_libraries(bot_node
  yaml-cpp
)


add_executable(lidar_node
  src/LidarNode.cpp
  src/ConfigParser.cpp
  ../robot_old/src/Environment.cpp
  ../robot_old/src/Lidar.cpp
)
target_include_directories(lidar_node PUBLIC
  include
  ${ROBOT_OLD_INCLUDE}
  ${OpenCV_INCLUDE_DIRS}
)
ament_target_dependencies(lidar_node
  rclcpp
  robot_msgs
  sensor_msgs
)
target_link_libraries(lidar_node
  ${OpenCV_LIBS}
  yaml-cpp
)

install(TARGETS
  keyboard_node
  robot_node
  game_node
  bot_node
  lidar_node
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY launch rviz config
  DESTINATION share/${PROJECT_NAME}
)

ament_package()
