cmake_minimum_required (VERSION 2.8)
project(onion_server)

include(dlib-19.10/dlib/cmake)
#link_directories(/usr/local/lib)


find_package(OpenCV 3.2.0 REQUIRED)
message(STATUS "OpenCV include dir found at ${OpenCV_INCLUDE_DIRS}")
message(STATUS "OpenCV lib dir found at ${OpenCV_LIBS}")

set(FaceReg
  	HX_FaceReg.cpp
  	HX_FaceReg.h 
  	HX_FaceProcess.cpp
  	HX_FaceProcess.h)

add_executable(onion_server onion_server.c ${FaceReg})

target_link_libraries(onion_server onion_static pthread curl ${OpenCV_LIBS} dlib::dlib)
#target_link_libraries(onion_server onion_static pthread gnutls gcrypt curl ${OpenCV_LIBS} dlib::dlib)

#set_target_properties(onion_server PROPERTIES OUTPUT_NAME "FaceRegServer")

