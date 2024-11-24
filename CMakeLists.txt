cmake_minimum_required(VERSION 3.4...3.18)
project(LKHpy)

set(LKH_DIR "LKH-3.0.12")

set(CMAKE_BUILD_TYPE Debug)
set(TREE_TYPE "TWO_LEVEL_TREE") # ONE_LEVEL_TREE, TWO_LEVEL_TREE, THREE_LEVEL_TREE

include_directories(${LKH_DIR}/SRC)
include_directories(${LKH_DIR}/SRC/INCLUDE)

# Find all .c files in the ${LKH_DIR}/SRC directory
file(GLOB LKH_SRC_FILES ${LKH_DIR}/SRC/*.c ${LKH_DIR}/SRC/INCLUDE/*.c)

# Remove unused files from the list
list(REMOVE_ITEM LKH_SRC_FILES  ${CMAKE_SOURCE_DIR}/${LKH_DIR}/SRC/KSwapKick.hunt.c
                                ${CMAKE_SOURCE_DIR}/${LKH_DIR}/SRC/LKHmain.c
    )

# Find all .cpp files in the src directory
file(GLOB SRC_FILES src/*.cpp src/helpers/*.cpp)

add_subdirectory(pybind11)
pybind11_add_module(LKHpy ${SRC_FILES} ${LKH_SRC_FILES})

# Add the compiler flags
target_compile_options(LKHpy PRIVATE -O3 -Wall -D${TREE_TYPE} -g -flto -fcommon -DCAVA_CUSTOM)
#target_compile_options(LKHpy PRIVATE -O3 -Wall -D${TREE_TYPE} -g -flto -fcommon -DCAVA_CACHE)
#target_compile_options(LKHpy PRIVATE -O3 -Wall -D${TREE_TYPE} -g -flto -fcommon -DCAVA_PENALTY)
#target_compile_options(LKHpy PRIVATE -O3 -Wall -D${TREE_TYPE} -g -flto -fcommon)

# Add the linker flags
target_link_libraries(LKHpy PRIVATE m)

target_compile_definitions(LKHpy PRIVATE VERSION_INFO=${VERSION_INFO})