cmake_minimum_required(VERSION 3.0.0)
project(serial-transfer-server)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(MSVC)
add_compile_options("/utf-8")
endif()
SET(CMAKE_BUILD_TYPE "Release")
add_subdirectory(boost_wrapper)
# 使用rapidjson作为json解析工具
include_directories(serial-transfer-server include)

add_executable(serial-transfer-server main.cpp TransferServer.cpp TransferServer.h hex.h hex.cpp)



# 直接依赖动态库
target_link_libraries(serial-transfer-server web_sock Setupapi)
