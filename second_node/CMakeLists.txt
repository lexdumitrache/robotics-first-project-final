cmake_minimum_required(VERSION 2.8.3)
project(first_project)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  tf
  nav_msgs
  ## std_msgs - trovato su ex. codice lab pub_sub
)

catkin_package()

include_directories(
  ${catkin_INCLUDE_DIRS}
)

add_executable(odom_to_tf src/odom_to_tf.cpp)
target_link_libraries(odom_to_tf ${catkin_LIBRARIES})


