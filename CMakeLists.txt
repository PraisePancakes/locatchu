cmake_minimum_required(VERSION 3.28)
set(LOCATCHU_VERSION_REGEX "#define LOCATCHU_VERSION_.*[ \t]+(.+)")
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/src/locatchu/build_config/version.hpp" LOCATCHU_VERSION REGEX ${LOCATCHU_VERSION_REGEX})
list(TRANSFORM LOCATCHU_VERSION REPLACE ${LOCATCHU_VERSION_REGEX} "\\1")
string(JOIN "." LOCATCHU_VERSION ${LOCATCHU_VERSION})
set(PROJECT_NAME LOCATCHU)

project(
    ${PROJECT_NAME} 
    VERSION ${LOCATCHU_VERSION} 
    DESCRIPTION "locatchu source"
    HOMEPAGE_URL "https://github.com/praispancakes/locatchu"
    LANGUAGES C CXX
)
message("Your C++ compiler supports these C++ features:")
foreach(i ${CMAKE_CXX_COMPILE_FEATURES})
    message("${i}")
endforeach()
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()
include(FetchContent)
set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_X11 ON CACHE BOOL "" FORCE)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG master
)

FetchContent_MakeAvailable(raylib)

FetchContent_Declare(
    forge
    GIT_REPOSITORY https://github.com/praisepancakes/forge.git
    GIT_TAG main
)
FetchContent_MakeAvailable(forge)

add_executable(${PROJECT_NAME} 
    main.cpp 
)
target_link_libraries(${PROJECT_NAME}  PRIVATE forge raylib)
message("INFO :")
message("  C: '${CMAKE_C_COMPILER}'")
message("  C++: '${CMAKE_CXX_COMPILER}'")
message("  Source: ${PROJECT_SOURCE_DIR}")
message("  Binary: ${PROJECT_BINARY_DIR}")
message("  Version: ${PROJECT_VERSION}")
message("  Version (alt): ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_23)

message(STATUS  "*")
message(STATUS  "* ${PROJECT_NAME} v${PROJECT_VERSION} (${CMAKE_BUILD_TYPE})")
message(STATUS  "*")


