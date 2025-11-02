cmake_minimum_required(VERSION 3.31)
project(FakeBook)

set(CMAKE_CXX_STANDARD 20)

add_executable(FakeBook
        src/main.cpp
        include/Fakebook.h
        include/User.h
        include/Post.h
        include/Authenticator.h
        include/DummyDataGenerator.h
        src/DummyDataGenerator.cpp
        src/FakeBook.cpp)

target_include_directories(FakeBook PRIVATE include)