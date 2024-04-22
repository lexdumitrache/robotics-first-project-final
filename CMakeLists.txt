cmake_minimum_required(VERSION 3.0.0)
project(first_project VERSION 0.1.0 LANGUAGES C CXX)

find_package(catkin REQUIRED_COMPONENTS roscpp std_msgs)

catkin_package()

# include directories(include ${catkin_INCLUDE_DIRS})

# add executable(first_project src/first_project.cpp)
# target_link_libraries(first_project ${catkin_LIBRARIES})
