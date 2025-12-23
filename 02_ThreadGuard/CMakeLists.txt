#编译器启用 UTF-8 编码支持
add_compile_options("/utf-8")

file(GLOB_RECURSE SRC ${PROJECT_SOURCE_DIR}/02_ThreadGuard/*.cpp)

add_executable (02_ThreadGuard ${SRC})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET 02_ThreadGuard PROPERTY CXX_STANDARD 20)
endif()
