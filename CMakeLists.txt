cmake_minimum_required(VERSION 3.13)
project(SO2)

set(CMAKE_CXX_STANDARD 17)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

include_directories(.)

add_executable(SO2
        Client.cpp
        Client.h
        Pizzaiolo.cpp
        Pizzaiolo.h
        main.cpp
        Order.cpp
        Order.h
        Pizza.cpp
        Pizza.h
        Supplier.cpp
        Supplier.h
        Waiter.cpp
        Waiter.h Chair.cpp Chair.h)

target_link_libraries(SO2 ${CURSES_LIBRARIES})
target_link_libraries(SO2 Threads::Threads)