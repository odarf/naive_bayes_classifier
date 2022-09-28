cmake_minimum_required(VERSION 3.23)
project(naive_bayes_classifier)

set(CMAKE_CXX_STANDARD 14)

add_executable(train
        train.h
        train.cpp)
add_executable(class
        classify.h
        classify.cpp)