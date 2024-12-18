# CMakeList.txt : CMake project for Day12, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (Day11 "Day11.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Day11 PROPERTY CXX_STANDARD 20)
  target_link_libraries(Day11 PRIVATE TBB::tbb)
endif()

# TODO: Add tests and install targets if needed.
