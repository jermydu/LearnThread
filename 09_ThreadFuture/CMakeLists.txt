#编译器启用 UTF-8 编码支持
add_compile_options("/utf-8")

file(GLOB_RECURSE SRC ${PROJECT_SOURCE_DIR}/09_ThreadFuture/*.cpp)

add_executable (09_ThreadFuture ${SRC})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET 09_ThreadFuture PROPERTY CXX_STANDARD 20)
endif()
