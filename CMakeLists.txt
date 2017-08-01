cmake_minimum_required(VERSION 3.7)

project(homophily_wsn)

if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    set(CMAKE_CXX_COMPILER /usr/local/opt/llvm/bin/clang++)
    include_directories(SYSTEM /usr/local/opt/llvm/include)
    link_directories(/usr/local/opt/llvm/lib)
endif()
if(DEFINED ENV{K})
	set(CMAKE_CXX_COMPILER FCCpx)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Kfast -Xg")
endif()

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
  node.cpp
  node.hpp
  random.cpp
  random.hpp
  wsn_nd_ld_aging.cpp
  wsn_nd_ld_aging.hpp
  main.cpp
  )

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
add_executable(wsn.out ${SOURCE_FILES})

