cmake_minimum_required(VERSION 3.5)

project(ros_sec_test)

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(class_loader REQUIRED)
find_package(geometry_msgs)
find_package(lifecycle_msgs REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_components REQUIRED)
find_package(rclcpp_lifecycle REQUIRED)
find_package(rcutils REQUIRED)

include_directories(
    include
    ${geometry_msgs_INCLUDE_DIRS}
    ${lifecycle_msgs_INCLUDE_DIRS}
    ${rclcpp_INCLUDE_DIRS}
    ${rclcpp_lifecycle_INCLUDE_DIRS}
    )


function(custom_component target file_path component)
    add_library(${target} SHARED ${file_path})
    target_compile_definitions(${target}
        PRIVATE "COMPOSITION_BUILDING_DLL")
    ament_target_dependencies(${target}
        "class_loader"
        "${rclcpp_LIBRARIES}"
        "${rclcpp_lifecycle_LIBRARIES}"
        "geometry_msgs")
    rclcpp_components_register_nodes(${target} ${component})
endfunction()

custom_component(
    noop_component src/attacks/noop/component.cpp
    ros_sec_test::attacks::noop::Component)
custom_component(
    teleop_component src/attacks/coms/teleop/component.cpp
    ros_sec_test::attacks::coms::teleop::Component)
custom_component(
    disk_component src/attacks/resources/disk/component.cpp
    ros_sec_test::attacks::resources::disk::Component)

# since the package installs libraries without exporting them
# it needs to make sure that the library path is being exported
if(NOT WIN32)
  ament_environment_hooks(
    "${ament_cmake_package_templates_ENVIRONMENT_HOOK_LIBRARY_PATH}")
endif()

add_executable(
    runner src/runner/main.cpp src/runner/runner.cpp src/runner/lifecycle_service_client.cpp)
target_link_libraries(runner 
    noop_component
    disk_component
    teleop_component)
ament_target_dependencies(runner "rclcpp" "rclcpp_lifecycle")

install(TARGETS
  noop_component
  teleop_component
  disk_component
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)

install(TARGETS
    runner
    DESTINATION lib/${PROJECT_NAME})

ament_package()