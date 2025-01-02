cmake_minimum_required(VERSION 2.8)

project(nfcsim)
set(CMAKE_BUILD_TYPE Release)
# set(CMAKE_BUILD_TYPE Debug)
include_directories(./include)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wconversion")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wconversion -std=gnu++11")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wconversion -std=gnu++17")

file(GLOB SOURCES "src/*.c")

# include(CTest)
# enable_testing()

# Create a static library from the source files
add_library(project_lib STATIC ${SOURCES})

# Link each executable to the static library
add_executable(nfcsim ./prog/nfcsim.c)
target_link_libraries(nfcsim project_lib m)

add_executable(avgSpectre ./prog/avgSpectre.c)
target_link_libraries(avgSpectre project_lib m)

# add_test(register_test ./test_register)