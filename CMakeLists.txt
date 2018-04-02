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
  homophily_wsn.cpp
  homophily_wsn.hpp
  )

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
add_executable(wsn.out main.cpp ${SOURCE_FILES})

add_executable(wsn_different_q.out main_different_q.cpp ${SOURCE_FILES})

set(SOURCE_FILES2
        spreading/NetSpreading.cpp
        spreading/NetSpreading.hpp
        spreading/spreading_main.cpp
        )
add_executable(spreading_main.out ${SOURCE_FILES2})
