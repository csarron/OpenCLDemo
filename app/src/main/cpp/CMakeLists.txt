# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

cmake_minimum_required(VERSION 3.4.1)

# configure import libs
set(lib_DIR ${CMAKE_SOURCE_DIR})

add_library(libOpenCL SHARED libopencl.c)
target_include_directories(libOpenCL PRIVATE
                           ${lib_DIR}/include)

add_library(OpenCLDemo SHARED
            OpenCLDemo.cpp
            openCLNR.cpp
            refNR.cpp
            )

target_include_directories(OpenCLDemo PRIVATE
                           ${lib_DIR}/include)

target_link_libraries(OpenCLDemo
                      android
                      libOpenCL
                      log)