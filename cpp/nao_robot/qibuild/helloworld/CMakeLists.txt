cmake_minimum_required(VERSION 2.8)
project(helloworld)

find_package(qibuild)

set(CMAKE_CXX_FLAGS 
"-std=c++1y -static-libstdc++ -Os -s -O2 -march=atom -mtune=atom -mfpmath=sse -fschedule-insns -fsched-pressure" CACHE INTERNAL "" FORCE)

find_package(rapp)
message(STATUS "libraries: ${RAPP_LIBRARIES}")
message(STATUS "headers: ${RAPP_INCLUDE_DIRS}")

qi_create_bin(helloworld "main.cpp")
qi_use_lib(helloworld openssl RAPP boost_system boost_thread pthread)
