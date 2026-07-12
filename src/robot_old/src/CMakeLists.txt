add_library(robot_lib
		Environment.cpp
    	Lidar.cpp
		Canvas.cpp
		Robot.cpp
)

add_library(robot_lib_old
		Environment_old.cpp
		Canvas_old.cpp
		Lidar_old.cpp
		Robot.cpp)

target_include_directories(robot_lib PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(robot_lib PUBLIC ${OpenCV_LIBS})

target_include_directories(robot_lib_old PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(robot_lib_old PUBLIC ${OpenCV_LIBS})