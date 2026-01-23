cmake_minimum_required(VERSION 3.16)
project(FreeKassaPlugin
  VERSION 1.0.0
  LANGUAGES CXX
)
add_compile_definitions(FreeKassaPlugin_VERSION="${PROJECT_VERSION}" )
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE SRC_FILES
  CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
)

add_library(FreeKassaPlugin SHARED ${SRC_FILES})

target_include_directories(FreeKassaPlugin
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Put resulting .so into workspace/Plugins
set_target_properties(FreeKassaPlugin PROPERTIES
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../../../Plugins"
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../../../Plugins"
  OUTPUT_NAME "FreeKassaPlugin"
)
find_package(PluginCore REQUIRED)
target_link_libraries(FreeKassaPlugin PRIVATE d3156::PluginCore)

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../PaymentsModel/CMakeLists.txt")
    message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}/../PaymentsModel not found, downloading...")
    
    include(FetchContent)
    FetchContent_Declare(
        ${CMAKE_CURRENT_SOURCE_DIR}/../PaymentsModel
        GIT_REPOSITORY https://github.com/d3156/${CMAKE_CURRENT_SOURCE_DIR}/../PaymentsModel.git
        GIT_TAG master
    )
    FetchContent_MakeAvailable(${CMAKE_CURRENT_SOURCE_DIR}/../PaymentsModel)
else()
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../PaymentsModel" 
                     "${CMAKE_BINARY_DIR}/_deps/PaymentsModel")
endif()

target_link_libraries(FreeKassaPlugin PRIVATE PaymentsModel)

