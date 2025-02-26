cmake_minimum_required(VERSION 3.16)
project(p2pal-chat LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)

find_package(Qt6 COMPONENTS Widgets REQUIRED)
include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(p2pal-chat
    src/main.cpp
    src/main_window.cpp
    src/chat_session.cpp
    src/message_input.cpp
    src/message.cpp
    include/main_window.h
    include/chat_session.h
    include/message_input.h
    include/message.h
)

target_link_libraries(p2pal-chat PRIVATE Qt6::Widgets)
