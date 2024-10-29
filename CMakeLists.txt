cmake_minimum_required(VERSION 3.4...3.18)
project(LkhPy)

set(LKH_DIR "LKH-3.0.12")

include_directories(${LKH_DIR}/SRC)
include_directories(${LKH_DIR}/SRC/INCLUDE)

# Find all .c files in the ${LKH_DIR}/SRC directory
file(GLOB LKH_SRC_FILES ${LKH_DIR}/SRC/*.c ${LKH_DIR}/SRC/INCLUDE/*.c)

# Remove unused files from the list
list(REMOVE_ITEM LKH_SRC_FILES  ${CMAKE_SOURCE_DIR}/${LKH_DIR}/SRC/KSwapKick.hunt.c
                                ${CMAKE_SOURCE_DIR}/${LKH_DIR}/SRC/LKHmain.c)

add_subdirectory(pybind11)
pybind11_add_module(LkhPy src/main.cpp ${LKH_SRC_FILES})

target_compile_definitions(LkhPy
                           PRIVATE VERSION_INFO=${VERSION_INFO})
