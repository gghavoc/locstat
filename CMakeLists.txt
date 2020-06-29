cmake_minimum_required(VERSION 3.16)
project(locstat)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-std=c++17 -pthread -Wall")
set(CMAKE_C_FLAGS "-std=c11")
set(CMAKE_C_STANDARD 11)
set(SRC_FILES
        main.cpp
        src/locstat/loc_info.cpp
        src/locstat/file_stats.cpp
        src/locstat/exceptions/invalid_characters.cpp
        src/locstat/loc_counter.cpp
        src/locstat/exceptions/invalid_file.cpp
        src/locstat/util/finally.cpp
        src/locstat/exceptions/invalid_input.cpp)

add_executable(locstat ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} stdc++fs)