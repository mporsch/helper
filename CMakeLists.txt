cmake_minimum_required (VERSION 3.0.0)

project (helper CXX)
add_executable (helper_test test.cpp helper.h work_queue.h)
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  target_link_libraries (helper_test pthread)
endif()
