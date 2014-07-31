cmake_minimum_required (VERSION 2.6)

project (sa-ptv)

set (PROJECT_VERSION_MAJOR 1)
set (PROJECT_VERSION_MINOR 0)

# find required packages
find_package (OpenCV REQUIRED)

# prototyping executables
add_executable (mtiff ${PROJECT_SOURCE_DIR}/mtiff.cpp)

set (OTHER_LIBS tiff)

target_link_libraries (mtiff ${OpenCV_LIBS} ${OTHER_LIBS})