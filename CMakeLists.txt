cmake_minimum_required(VERSION 3.10)

project(seqft) # Fill in the project name.

# set(CMAKE_C_COMPILER clang) # Use GCC by default.
# set(CMAKE_CXX_COMPILER clang++) # Use GCC by default.
set(CMAKE_BUILD_TYPE Debug) # Debug by default.
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O0")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3")
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_C_STANDARD 11)

add_executable(${PROJECT_NAME}
  src/main.c
  src/stack.c
  src/stack.h
  src/tokenizer.c
  src/tokenizer.h
  src/common.h
  src/common.c
  src/evaluator.c
  src/evaluator.h
)

# Libraries to be statically linked with executable.  
target_link_libraries(${PROJECT_NAME}
  m # Math library.
)

## Additional library search directories.
# target_link_directories(${PROJECT_NAME}
# )

## Include directories.
# target_include_directories(${PROJECT_NAME}


