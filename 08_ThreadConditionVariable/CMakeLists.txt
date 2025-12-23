#编译器启用 UTF-8 编码支持
add_compile_options("/utf-8")

file(GLOB_RECURSE SRC ${PROJECT_SOURCE_DIR}/08_ThreadConditionVariable/*.cpp)

add_executable (08_ThreadConditionVariable ${SRC})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET 08_ThreadConditionVariable PROPERTY CXX_STANDARD 20)
endif()
