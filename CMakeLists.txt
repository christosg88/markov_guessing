cmake_minimum_required(VERSION 3.9)
project(markov_guessing LANGUAGES CXX C)

# compiler flags
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

# WjCryptLib library
add_library(WjCryptLib
        inc/WjCryptLib/lib/WjCryptLib_Md5.c
        inc/WjCryptLib/lib/WjCryptLib_Md5.h)
target_include_directories(WjCryptLib PUBLIC
        inc/WjCryptLib/lib)

# gen_markov_matrices executable
add_executable(gen_markov_matrices
        src/gen_markov_matrices.cpp)

# guess_pass executable
add_executable(guess_pass
        src/guess_pass.cpp
        inc/guess_pass.hpp
        inc/ascii_progress_bar.hpp)
target_link_libraries(guess_pass WjCryptLib)
target_include_directories(guess_pass PRIVATE inc)

# install binaries in bin/
install(TARGETS gen_markov_matrices guess_pass DESTINATION
        ${PROJECT_SOURCE_DIR}/bin)
