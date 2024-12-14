# CMakeList.txt : CMake project for Day12, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (Day12 "Day12.cpp" "Day12.h")

# Set stack size using linker options
if (MSVC)
    # For Visual Studio (MSVC), use the `/STACK` option
    target_link_options(Day12 PRIVATE /STACK:1073741824)  # 8 MB stack size
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    # For GCC/Clang, use `-Wl,-stack` or equivalent
    target_link_options(Day12 PRIVATE -Wl,--stack,8388608)  # 8 MB stack size
endif()

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Day12 PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
