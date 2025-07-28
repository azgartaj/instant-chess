cmake_minimum_required(VERSION 3.11)
project(InstantChessAPI)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare(
  crow
  GIT_REPOSITORY https://github.com/CrowCpp/Crow.git
  GIT_TAG master
)

FetchContent_MakeAvailable(crow)

add_executable(api_server src/main.cpp)

target_include_directories(api_server PRIVATE ${crow_SOURCE_DIR}/include)