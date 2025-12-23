cmake_minimum_required (VERSION 3.8)

# 如果支持，请为 MSVC 编译器启用热重载。
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# 设置可执行文件目录 这是一个字符串，表示新的输出目录路径。这里指定了在二进制树的顶级目录下创建一个名为bin的子目录，所有的可执行文件将会被放置在这个目录中。
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

project (LearnThread)

# 包含子项目。
add_subdirectory ("01_CreateThread")
add_subdirectory ("02_ThreadGuard")
add_subdirectory ("03_ThreadLocaL")
add_subdirectory ("04_ThreadAtomic")
add_subdirectory ("05_ThreadMutex")
add_subdirectory ("06_ThreadCallOnce")
add_subdirectory ("07_ThreadSharedMutex")
