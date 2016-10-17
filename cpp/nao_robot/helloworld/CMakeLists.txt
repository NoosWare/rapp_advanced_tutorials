cmake_minimum_required(VERSION 2.6)

project(helloworld)

add_executable(helloworld source/helloworld.cpp)

set(LIBRARY_PATH ${LIBRARY_PATH} /usr/local/lib)

find_package(OpenSSL REQUIRED)
find_package(Boost COMPONENTS system REQUIRED)
find_package(Threads REQUIRED)

set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
find_library(RAPP_LIBRARY NAMES rapp REQUIRED)
message(STATUS "${RAPP_LIBRARY}")

set(RAPP_LIBRARIES ${RAPP_LIBRARY} 
                   ${OPENSSL_LIBRARIES} 
				   ${Boost_LIBRARIES}
				   ${CMAKE_THREAD_LIBS_INIT})

target_link_libraries(helloworld ${RAPP_LIBRARIES})

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}-static-libstdc++ -static-libgcc")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y -Wall -fPIC -Os -s -O2 -march=atom -mtune=atom -mfpmath=sse")
