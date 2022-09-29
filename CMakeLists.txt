cmake_minimum_required(VERSION 3.23)
project(naive_bayes_classifier)

set(CMAKE_CXX_STANDARD 14)

add_executable(nbc_v0.0.2
        train.h
        train.cpp
        classify.h
        classify.cpp
        main.cpp)