cmake_minimum_required(VERSION 2.6)

project(helloworld_static)

add_executable(helloworld_static source/helloworld_static.cpp)

set(LIBRARY_PATH ${LIBRARY_PATH} /usr/local/lib)

set_target_properties(helloworld_static PROPERTIES LINK_SEARCH_START_STATIC 1)
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")

find_library(LIBDL_LIBRARY NAMES dl)
message(STATUS ${LIBDL_LIBRARY})

find_library(RAPP_LIBRARY NAMES rapp REQUIRED)
message(STATUS "${RAPP_LIBRARY}")

find_package(OpenSSL REQUIRED)
message(STATUS "${OPENSSL_LIBRARIES}")

find_package(Boost COMPONENTS system REQUIRED)
set(Boost_USE_STATIC_LIBS ON)
set(BUILD_SHARED_LIBS OFF)
include_directories(${Boost_INCLUDE_DIR})
message(STATUS "${Boost_SYSTEM_LIBRARY}")

find_package(Threads REQUIRED)

target_link_libraries(helloworld_static ${RAPP_LIBRARY} 
                                 ${OPENSSL_LIBRARIES} 
								 ${Boost_LIBRARIES}
								 ${CMAKE_THREAD_LIBS_INIT}
								 ${LIBDL_LIBRARY})

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}-static")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y -Wall -fPIC -Os -s -O2 -march=atom -mtune=atom -mfpmath=sse")
