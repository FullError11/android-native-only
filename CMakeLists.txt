cmake_minimum_required(VERSION 3.10)
project(android_native_only)

if(NOT ANDROID)
  message(FATAL_ERROR "需要配置安卓NDK 并 设置ANDROID变量")
endif()

add_library(ano INTERFACE)
target_include_directories(ano INTERFACE include)
target_compile_features(ano INTERFACE cxx_std_23)
target_link_libraries(ano INTERFACE log android GLESv3 EGL)
