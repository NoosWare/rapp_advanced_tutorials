cmake_minimum_required(VERSION 2.6)
project(face_detection)

#find_package(qibuild)
#
#find_package(rapp)
#message(STATUS "libraries: ${RAPP_LIBRARIES}")
#message(STATUS "headers: ${RAPP_INCLUDE_DIRS}")
#
#set(CMAKE_CXX_FLAGS "-std=gnu++1y -static-libstdc++")
#qi_create_bin(face_detection "main.cpp")
#qi_use_lib(face_detection ALCOMMON ALPROXIES RAPP openssl boost_system boost_thread pthread ALVISION OPENCV2_CORE OPENCV2_HIGHGUI)

#set(CMAKE_CXX_FLAGS 
#"-std=gnu++1y -I/home/ubuntu14/naoqi-sdk-2.1.4.13-linux32/include -L/home/ubuntu14/naoqi-sdk-2.1.4.13-linux32/lib -static-libstdc++ -static-libgcc -Wl,-Bstatic -lc -lssl -lcrypto -L /usr/local/lib -lrapp -Wl,-Bdynamic -lboost_system -lpthread -ldl -march=atom -mtune=atom -mfpmath=sse")
#find_package(qibuild)
#qi_create_bin(face_detection "main.cpp")

#set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
##COMMON
#set(LIBDL_NAMES dl libdl ltdl libltdl)
#find_library(LIBDL_LIBRARY NAMES ${LIBDL_NAMES})
#message(STATUS ${LIBDL_LIBRARY})
#find_package(OpenSSL REQUIRED)
#message(STATUS ${OPENSSL_LIBRARIES})
#find_package(Threads REQUIRED)

#set(CMAKE_FIND_LIBRARY_SUFFIXES ".so")
#
##ALDEBARAN
#set(NAOQI ${NAOQI} "$ENV{HOME}/naoqi-sdk-2.1.4.13-linux32/")
#set(LIB_PATH ${LIB_PATH} "${NAOQI}/lib")
#set(INCLUDE_PATH ${INCLUDE_PATH} "${NAOQI}/include")
#
#find_library(ALPROXIES_LIBRARY NAMES alproxies HINTS ${LIB_PATH})
#message(STATUS ${ALPROXIES_LIBRARY})
#
#find_library(ALERROR_LIBRARY NAMES alerror HINTS ${LIB_PATH})
#message(STATUS ${ALERROR_LIBRARY})
#
#find_library(ALCOMMON_LIBRARY NAMES alcommon HINTS ${LIB_PATH})
#message(STATUS ${ALCOMMON_LIBRARY})
#
#find_library(ALVALUE_LIBRARY NAMES alvalue HINTS ${LIB_PATH})
#message(STATUS ${ALVALUE_LIBRARY})
#
#find_library(QI_LIBRARY NAMES qi HINTS ${LIB_PATH})
#message(STATUS ${QI_LIBRARY})
#
#find_library(QITYPE_LIBRARY NAMES qitype HINTS ${LIB_PATH})
#message(STATUS ${QITYPE_LIBRARY})
#
#find_library(ALVISION_LIBRARY NAMES alvision HINTS ${LIB_PATH})
#message(STATUS ${ALVISION_LIBRARY})
#
#find_library(OPENCV_CORE_LIBRARY NAMES opencv_core HINTS ${LIB_PATH})
#message(STATUS ${OPENCV_CORE_LIBRARY})
#
#find_library(OPENCV_HIGHGUI_LIBRARY NAMES opencv_highgui HINTS ${LIB_PATH})
#message(STATUS ${OPENCV_HIGHGUI_LIBRARY})
#
#find_library(Boost_SYSTEM NAMES boost_system)
#find_library(Boost_THREAD NAMES boost_thread)
#
#RAPP
#find_package(rapp REQUIRED)
#message(STATUS "libraries: ${RAPP_LIBRARIES}")
#message(STATUS "headers: ${RAPP_INCLUDE_DIRS}")
#
##SET LIBRARIES
#set(Boost_LIBRARIES ${Boost_SYSTEM}
#                    ${Boost_THREAD})
#message(STATUS ${Boost_LIBRARIES})
#
#include_directories(${INCLUDE_PATH})
#
#set(NAO_LIBRARIES ${ALPROXIES_LIBRARY}
#                  ${ALCOMMON_LIBRARY}
#                  ${ALERROR_LIBRARY}
#                  ${ALVALUE_LIBRARY}
#                  ${OPENCV_CORE_LIBRARY}
#                  ${OPENCV_HIGHGUI_LIBRARY}
#                  ${QI_LIBRARY}
#                  ${QITYPE_LIBRARY})
#
#set(RAPP_LIBRARY ${RAPP_LIBRARIES} 
#				 ${Boost_LIBRARIES}
#                 ${OPENSSL_LIBRARIES} 
#                 ${LIBDL_LIBRARY}
#				 ${CMAKE_THREAD_LIBS_INIT})
#
###BOTH
#target_link_libraries(face_detection ${RAPP_LIBRARY})
#                                     ${NAO_LIBRARIES})
#
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}-static-libstdc++ -static-libgcc")
#set(CMAKE_CXX_FLAGS "-std=gnu++1y -march=atom -mtune=atom -mfpmath=sse")

find_package(qibuild)
qi_create_bin(face_detection "main.cpp")

set_target_properties(face_detection PROPERTIES LINK_SEARCH_START_STATIC 1)

#ALDEBARAN
set(NAOQI ${NAOQI} "$ENV{HOME}/naoqi-sdk-2.1.4.13-linux32/")
set(LIB_PATH ${LIB_PATH} "${NAOQI}/lib")
set(INCLUDE_PATH ${INCLUDE_PATH} "${NAOQI}/include")

set(CMAKE_FIND_LIBRARY_SUFFIXES ".so")
find_library(ALPROXIES_LIBRARY NAMES alproxies HINTS ${LIB_PATH})
message(STATUS ${ALPROXIES_LIBRARY})

find_library(ALERROR_LIBRARY NAMES alerror HINTS ${LIB_PATH})
message(STATUS ${ALERROR_LIBRARY})

find_library(ALCOMMON_LIBRARY NAMES alcommon HINTS ${LIB_PATH})
message(STATUS ${ALCOMMON_LIBRARY})

find_library(ALVALUE_LIBRARY NAMES alvalue HINTS ${LIB_PATH})
message(STATUS ${ALVALUE_LIBRARY})

find_library(QI_LIBRARY NAMES qi HINTS ${LIB_PATH})
message(STATUS ${QI_LIBRARY})

find_library(QITYPE_LIBRARY NAMES qitype HINTS ${LIB_PATH})
message(STATUS ${QITYPE_LIBRARY})

find_library(OPENCV_CORE_LIBRARY NAMES opencv_core HINTS ${LIB_PATH})
message(STATUS ${OPENCV_CORE_LIBRARY})

find_library(OPENCV_HIGHGUI_LIBRARY NAMES opencv_highgui HINTS ${LIB_PATH})
message(STATUS ${OPENCV_HIGHGUI_LIBRARY})

set(LIBDL_NAMES dl libdl ltdl libltdl)
find_library(LIBDL_LIBRARY NAMES ${LIBDL_NAMES})
message(STATUS ${LIBDL_LIBRARY})

#add_executable(face_detection main.cpp)
set(LIBRARY_PATH ${LIBRARY_PATH} /usr/local/lib)

set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")

find_library(RAPP_LIBRARY NAMES rapp REQUIRED)
message(STATUS "${RAPP_LIBRARY}")

find_library(Boost_SYSTEM NAMES boost_system)
find_library(Boost_THREAD NAMES boost_thread)
set(Boost_LIBRARIES ${Boost_SYSTEM} 
                    ${Boost_THREAD})

find_package(OpenSSL REQUIRED)
message(STATUS "${OPENSSL_LIBRARIES}")

find_package(Threads REQUIRED)

include_directories(${Boost_INCLUDE_DIR}
                    "/usr/include"
                    "/usr/local/include"
                    ${INCLUDE_PATH})

message(STATUS "${Boost_LIBRARIES}")

set(NAO_LIBRARIES ${ALPROXIES_LIBRARY}
                  ${ALCOMMON_LIBRARY}
                  ${ALERROR_LIBRARY}
                  ${ALVALUE_LIBRARY}
                  ${QI_LIBRARY}
                  ${QITYPE_LIBRARY}
                  ${OPENCV_CORE_LIBRARY}
                  ${OPENCV_HIGHGUI_LIBRARY})

target_link_libraries(face_detection ${RAPP_LIBRARY} 
                                     ${OPENSSL_LIBRARIES} 
                                     ${Boost_LIBRARIES}
                                     ${CMAKE_THREAD_LIBS_INIT}
                                     ${LIBDL_LIBRARY}
                                     ${NAO_LIBRARIES})

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}-static")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++1y -march=atom -mtune=atom -mfpmath=sse")
