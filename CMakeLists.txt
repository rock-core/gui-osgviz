# CMakeLists.txt has to be located in the project folder and cmake has to be
# executed from 'project/build' with 'cmake ../'.
cmake_minimum_required(VERSION 3.3)
find_package(Rock)
rock_init(osgViz 0.1)

if (MSYS)
    ADD_DEFINITIONS(-DWIN32)
endif(MSYS)


rock_standard_layout()
