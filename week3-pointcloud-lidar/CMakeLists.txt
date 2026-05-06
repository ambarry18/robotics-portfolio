cmake_minimum_required(VERSION 3.10)
project(week3_pcl)

find_package(PCL 1.12 REQUIRED)

include_directories(${PCL_INCLUDE_DIRS})
link_directories(${PCL_LIBRARY_DIRS})
add_definitions(${PCL_DEFINITIONS})

add_executable(load_pcd src/load_pcd.cpp)
target_link_libraries(load_pcd ${PCL_LIBRARIES})

add_executable(filter_pcd src/filter_pcd.cpp)
target_link_libraries(filter_pcd ${PCL_LIBRARIES})

add_executable(ransac_plane src/ransac_plane.cpp)
target_link_libraries(ransac_plane ${PCL_LIBRARIES})

add_executable(generate_scene src/generate_scene.cpp)
target_link_libraries(generate_scene ${PCL_LIBRARIES})

add_executable(clustering src/clustering.cpp)
target_link_libraries(clustering ${PCL_LIBRARIES})
