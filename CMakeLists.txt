cmake_minimum_required(VERSION 3.7)

project(homophily_wsn)

if(DEFINED ENV{K})
	set(CMAKE_CXX_COMPILER FCCpx)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Kfast -Xg")
endif()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

set(SOURCE_FILES
  node.cpp
  node.hpp
  random.cpp
  random.hpp
  homophily_wsn.cpp
  homophily_wsn.hpp
  )

find_package(OpenMP)

add_executable(wsn.out main.cpp ${SOURCE_FILES})
target_link_libraries(wsn.out PUBLIC OpenMP::OpenMP_CXX)

add_executable(wsn_different_q.out main_different_q.cpp ${SOURCE_FILES})
target_link_libraries(wsn_different_q.out PUBLIC OpenMP::OpenMP_CXX)

set(SOURCE_FILES2
        spreading/NetSpreading.cpp
        spreading/NetSpreading.hpp
        spreading/spreading_main.cpp
        )
add_executable(spreading_main.out ${SOURCE_FILES2})
