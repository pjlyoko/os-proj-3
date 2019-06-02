cmake_minimum_required(VERSION 3.14)
project(SO2)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

add_executable(SO2
        Client.cpp
        Client.h
        Cook.cpp
        Cook.h
        main.cpp
        Order.cpp
        Order.h
        Pizza.cpp
        Pizza.h
        Supplier.cpp
        Supplier.h
        Waiter.cpp
        Waiter.h)
