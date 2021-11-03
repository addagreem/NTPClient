cmake_minimum_required(VERSION 3.12)

project(NTPClient LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(-DBOOST_ASIO_NO_DEPRECATED)

set(ASIO_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/../3rd-party/asio-1.20.0/include")
include_directories(${ASIO_INCLUDE_DIRS})

set(SRC "blocking_client.hpp"
        "ntp_packet.hpp"
        "network_time_requester.hpp" "network_time_requester.cpp"
)

add_executable(NTPClient ${SRC})
target_link_libraries(NTPClient -lpthread)
