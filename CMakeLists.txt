cmake_minimum_required(VERSION 3.15)

project(ellipselio)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_POLICY_DEFAULT_CMP0074 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0144 NEW)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast -fopenmp -fPIC")

find_package(ament_cmake_auto REQUIRED)
ament_auto_find_build_dependencies()

find_package(ament_cmake REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_components REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(std_srvs REQUIRED)
find_package(visualization_msgs REQUIRED)
find_package(pcl_ros REQUIRED)
find_package(pcl_conversions REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(image_transport REQUIRED)

find_package(OpenCV REQUIRED)
find_package(Boost REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(PCL REQUIRED)

rosidl_generate_interfaces(${PROJECT_NAME}
  "msg/EllipseLioAnalytics.msg"
  DEPENDENCIES std_msgs
)
ament_export_dependencies(rosidl_default_runtime)

if(DEFINED ENV{ROS_DISTRO} AND "$ENV{ROS_DISTRO}" STREQUAL "jazzy")
  set(ROS_JAZZY ON)
else()
  set(ROS_JAZZY OFF)
endif()

if(DEFINED ENV{ROS_DISTRO} AND "$ENV{ROS_DISTRO}" STREQUAL "foxy")
  set(ROS_FOXY ON)
else()
  set(ROS_FOXY OFF)
endif()

if(ROS_JAZZY)
  message(STATUS "Auto-detected ROS Jazzy: enabling updated headers and includes")
  add_compile_definitions(ROS_JAZZY)
endif()

if(ROS_FOXY)
  message(STATUS "Auto-detected ROS Foxy: enabling compatibility code")
  add_compile_definitions(ROS_FOXY)
endif()

set(dependencies
  cv_bridge
  rclcpp
  rclcpp_components
  geometry_msgs
  nav_msgs
  sensor_msgs
  std_msgs
  std_srvs
  visualization_msgs
  pcl_ros
  pcl_conversions
  image_transport
)

ament_auto_add_library(ellipselio_mapping SHARED src/map_processing.cpp src/lidar_processing.cpp src/imu_processing.cpp src/cam_processing.cpp)
target_include_directories(ellipselio_mapping PRIVATE ${PYTHON_INCLUDE_DIRS})

target_include_directories(ellipselio_mapping PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
  ${EIGEN3_INCLUDE_DIR}
  ${PCL_INCLUDE_DIRS}
  ${OpenCV_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
)
target_link_libraries(ellipselio_mapping ${PCL_LIBRARIES} ${PYTHON_LIBRARIES} ${EIGEN3_LIBRARIES} ${OpenCV_LIBS} ${Boost_LIBRARIES})

ament_target_dependencies(ellipselio_mapping ${dependencies})

if(COMMAND rosidl_get_typesupport_target)
  rosidl_get_typesupport_target(cpp_typesupport_target ${PROJECT_NAME} "rosidl_typesupport_cpp")
  target_link_libraries(ellipselio_mapping ${cpp_typesupport_target})
else()
  rosidl_target_interfaces(ellipselio_mapping ${PROJECT_NAME} "rosidl_typesupport_cpp")
endif()

rclcpp_components_register_node(ellipselio_mapping
PLUGIN "ellipselio::MappingNode"
EXECUTABLE ellipselio_mapping_node
)

ament_auto_add_executable(ellipselio_mapping_mt_node
  src/standalone_mapping_node.cpp
)
target_link_libraries(ellipselio_mapping_mt_node ellipselio_mapping)
ament_target_dependencies(ellipselio_mapping_mt_node ${dependencies})

ament_auto_package(USE_SCOPED_HEADER_INSTALL_DIR INSTALL_TO_SHARE
launch
rviz
config
)
