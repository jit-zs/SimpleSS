cmake_minimum_required(VERSION 3.28)
project(SimpleSS VERSION 0.0.1)
include(FetchContent)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)



FetchContent_Declare(
    magic_enum
    GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
    GIT_TAG v0.9.7
)
FetchContent_MakeAvailable(magic_enum)

add_library(${PROJECT_NAME} STATIC
    src/Lexer.cpp src/Parser.cpp src/StyleSheet.cpp
)
target_include_directories(${PROJECT_NAME} PUBLIC include)

add_executable(${PROJECT_NAME}_dev
    dev/main.cpp
)
target_link_libraries(${PROJECT_NAME}_dev PRIVATE ${PROJECT_NAME} magic_enum::magic_enum)