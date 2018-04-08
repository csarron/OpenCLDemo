cmake_minimum_required(VERSION 3.4.1)

set(cl_stub_DIR ${PROJECT_BINARY_DIR})
set(CL_stub_URL "https://github.com/csarron/cl_stub/archive/master.zip")

set(cl_stub_FILE "${cl_stub_DIR}/master.zip")
set(cl_stub_SRC "${cl_stub_DIR}/cl_stub-master")

if (NOT EXISTS "${cl_stub_FILE}")
    file(DOWNLOAD "${CL_stub_URL}" "${cl_stub_FILE}")
    execute_process(COMMAND ${CMAKE_COMMAND} -E tar xfz ${cl_stub_FILE}
      WORKING_DIRECTORY ${cl_stub_DIR})
endif()

set(cl_stub_build_DIR ${PROJECT_BINARY_DIR}/cl_stub_outputs)
file(MAKE_DIRECTORY ${cl_stub_build_DIR})
add_subdirectory(${cl_stub_SRC} ${cl_stub_build_DIR})
add_library(libcl_stub SHARED IMPORTED)

set_target_properties(libcl_stub PROPERTIES IMPORTED_LOCATION ${cl_stub_build_DIR}/${ANDROID_ABI}/libcl_stub.so)

add_library(OpenCLDemo SHARED
            OpenCLDemo.cpp
            openCLNR.cpp
            refNR.cpp)

target_include_directories(OpenCLDemo PRIVATE
                           ${cl_stub_SRC}/include)

target_link_libraries(OpenCLDemo
                      android
                      libcl_stub
                      log)