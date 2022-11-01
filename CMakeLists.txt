# CMakeLists.txt has to be located in the project folder and cmake has to be
# executed from 'project/build' with 'cmake ../'.
cmake_minimum_required(VERSION 3.3)
find_package(Rock)
rock_init(osgViz 0.1)
if(MSVC)
    #set_target_properties(osgViz PROPERTIES PLATFORM_TOOLSET "v120_CTP_Nov2012")
    set(CMAKE_GENERATOR_TOOLSET "v120_CTP_Nov2012" CACHE STRING "Platform Toolset" FORCE) 
    set(CMAKE_VS_PLATFORM_TOOLSET "v120_CTP_Nov2012" CACHE STRING "Platform Toolset" FORCE) 
else()
    rock_activate_cxx11()
endif()

rock_standard_layout()
