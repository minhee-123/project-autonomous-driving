cmake_minimum_required(VERSION 3.8)
project(speed_module_pkg)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(can_msgs REQUIRED)

# read_tof_node
add_executable(read_tof_node src/read_tof.cpp)
ament_target_dependencies(read_tof_node
  rclcpp
  std_msgs
  can_msgs
)

# send_pwm_node
add_executable(send_pwm_node src/send_pwm.cpp)
ament_target_dependencies(send_pwm_node
  rclcpp
  std_msgs
  can_msgs
)

# acc_node
add_executable(acc_node src/acc.cpp)
ament_target_dependencies(acc_node
  rclcpp
  std_msgs
)

# creep_node
add_executable(creep_node src/creep.cpp)
ament_target_dependencies(creep_node
  rclcpp
  std_msgs
)

install(TARGETS
  read_tof_node
  send_pwm_node
  acc_node
  creep_node
  DESTINATION lib/${PROJECT_NAME}
)

# launch file copy to DIRECTORY
install(DIRECTORY
  launch
  DESTINATION share/${PROJECT_NAME}
)

ament_package()