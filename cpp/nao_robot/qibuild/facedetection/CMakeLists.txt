cmake_minimum_required(VERSION 2.6)
project(face_detection)

find_package(qibuild)

find_package(rapp)
message(STATUS "libraries: ${RAPP_LIBRARIES}")
message(STATUS "headers: ${RAPP_INCLUDE_DIRS}")

set(CMAKE_CXX_FLAGS "-std=gnu++1y -static-libstdc++")
qi_create_bin(face_detection "main.cpp")
qi_use_lib(face_detection ALCOMMON ALPROXIES RAPP openssl boost_system boost_thread pthread ALVISION OPENCV2_CORE OPENCV2_HIGHGUI)
