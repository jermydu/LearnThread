#编译器启用 UTF-8 编码支持
add_compile_options("/utf-8")

file(GLOB_RECURSE SRC ${PROJECT_SOURCE_DIR}/07_ThreadSharedMutex/*.cpp)

add_executable (07_ThreadSharedMutex ${SRC})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET 07_ThreadSharedMutex PROPERTY CXX_STANDARD 20)
endif()
