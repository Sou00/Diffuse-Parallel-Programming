cmake_minimum_required(VERSION 3.20)
project(zadanie_mpi)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(zadanie_mpi
        Consts.cpp
        Consts.h
        Enigma.cpp
        Enigma.h
        EnigmaBreaker.cpp
        EnigmaBreaker.h
        Machinery.cpp
        Machinery.h
        Main.cpp
        MessageComparator.cpp
        MessageComparator.h
        MPIEnigmaBreaker.cpp
        MPIEnigmaBreaker.h
        SerialEnigmaBreaker.cpp
        SerialEnigmaBreaker.h
        SimpleMachinery.cpp
        SimpleMachinery.h
        SimpleMessageComparator.cpp
        SimpleMessageComparator.h)
