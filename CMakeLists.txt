cmake_minimum_required(VERSION 4.0)
project(ChessEngine)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 3.0.2
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM)
FetchContent_MakeAvailable(SFML)

add_executable(
        ChessEngine src/main.cpp
        src/GameBoard.cpp
        src/MoveSearcher.cpp
        src/BoardRenderer.cpp
        include/BoardRenderer.h)

target_compile_features(ChessEngine PRIVATE cxx_std_20)
target_link_libraries(ChessEngine PRIVATE SFML::Graphics)