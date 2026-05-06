cmake_minimum_required(VERSION 3.8)
project(lidar_pipeline)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(laser_geometry REQUIRED)
find_package(pcl_conversions REQUIRED)
find_package(pcl_ros REQUIRED)
find_package(PCL REQUIRED)

include_directories(${PCL_INCLUDE_DIRS})

add_executable(lidar_pipeline_node src/lidar_pipeline_node.cpp)

ament_target_dependencies(lidar_pipeline_node
    rclcpp
    sensor_msgs
    laser_geometry
    pcl_conversions
    pcl_ros
)

target_link_libraries(lidar_pipeline_node ${PCL_LIBRARIES})

install(TARGETS lidar_pipeline_node
    DESTINATION lib/${PROJECT_NAME})

ament_package()
