cmake_minimum_required(VERSION 3.8)
project(gambar_kopi)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(turtlesim REQUIRED)

add_executable(gambar_kopi_node src/gambar_kopi.cpp)

ament_target_dependencies(
  gambar_kopi_node
  rclcpp
  geometry_msgs
  turtlesim
)

install(
  TARGETS gambar_kopi_node
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()