if(MSVC)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS 1)
	#set(BUILD_SHARED_LIBS OFF)
	
endif(MSVC)	

if(MSVC)
  set(Boost_DIR "$ENV{Boost_DIR}")
  set(Boost_INCLUDE_DIRS "${Boost_DIR}/include")
  include_directories(${Boost_INCLUDE_DIRS})
else(MSVC)
#  set(Boost_NO_BOOST_CMAKE ON)
  find_package(Boost REQUIRED)
  include_directories(${Boost_INCLUDE_DIRS})
  
endif(MSVC)

include(CheckIncludeFileCXX)

set (CMAKE_REQUIRED_INCLUDES ${CMAKE_REQUIRED_INCLUDES} ${OPENSCENEGRAPH_INCLUDE_DIRS})
CHECK_INCLUDE_FILE_CXX("osg/Version" HAVE_OSG_VERSION_H)
if(${HAVE_OSG_VERSION_H})
    ADD_DEFINITIONS(-DHAVE_OSG_VERSION_H)
endif()

CHECK_INCLUDE_FILE_CXX("tr1/functional" USE_TR1)
if(${USE_TR1})
    ADD_DEFINITIONS(-DUSE_TR1)
else(${USE_TR1})
    ADD_DEFINITIONS(-DNO_TR1)
endif()

set(USE_MARS_VBO 0)
if(USE_MARS_VBO)
    ADD_DEFINITIONS(-DUSE_MARS_VBO)
endif()

if (UNIX)
	set(ADDITIONAL_LIBS X11)
endif (UNIX)

rock_library(osgVizInterfaces
	HEADERS interfaces/Clickable.h
			interfaces/WindowInterface.h
			interfaces/Updatable.h
			interfaces/MouseMoveCallback.h
      interfaces/WindowResizeCallback.h
	SOURCES Main.cpp
)


rock_library(osgVizGraphics
    SOURCES graphics/interfaces/data/GraphicData.cpp
            graphics/interfaces/data/LightData.cpp
            graphics/wrapper/OSGLightStruct.cpp
    HEADERS
            graphics/interfaces/OsgVizDefs.h
            graphics/interfaces/data/GraphicData.h
            graphics/interfaces/data/LightData.h
            graphics/wrapper/OSGLightStruct.h
    DEPS_PKGCONFIG openscenegraph)


rock_library(osgViz
    SOURCES OsgViz.cpp
           Object.cpp
           windows/Window.cpp
           windows/WindowManager.cpp
           windows/SuperView.cpp
           windows/HUD/HUD.cpp
           windows/HUD/HUDHoverScaler.cpp
           windows/HUD/HUDPositionChanger.cpp
           windows/EventHandlers/HierarchicalEventHandler.cpp
           windows/EventHandlers/ObjectSelector.cpp
           windows/EventHandlers/MouseMoveEvent.cpp
           windows/EventHandlers/WindowResizeEvent.cpp
           windows/CameraManipulator/TerrainZoomManipulator.cpp
           tools/UpdateThread.cpp    
           
    HEADERS OsgViz.hpp
           Object.h
           Module.h
           windows/Window.h
           windows/WindowManager.h
           windows/SuperView.h
           windows/HUD/HUD.h
           windows/HUD/HUDHoverScaler.h
           windows/HUD/HUDPositionChanger.h
           windows/EventHandlers/HierarchicalEventHandler.h
           windows/EventHandlers/ObjectSelector.h
           windows/EventHandlers/NullClickObject.h
           windows/EventHandlers/MouseMoveEvent.h
           windows/EventHandlers/WindowResizeEvent.h
           windows/config/WindowConfig.h
           windows/CameraManipulator/TerrainZoomManipulator.hpp
           tools/UpdateThread.h
           tools/Timing.h
           tools/TypeNameDemangling.h
           tools/GraphPrinter.h
           tools/SearchNodes.h
           tools/GlobalPos.h
           tools/GraphEditor.h
               
    LIBS ${ADDITIONAL_LIBS}
    DEPS osgVizGraphics 
    DEPS_PKGCONFIG openscenegraph
)

rock_library(PrimitivesFactory
    SOURCES
    modules/viz/Primitives/PrimitivesFactory.cpp
    modules/viz/Primitives/Primitives/AxesNode.cpp
    modules/viz/Primitives/Primitives/GridNode.cpp
    modules/viz/Primitives/Primitives/ArrowNode.cpp
    modules/viz/Primitives/Primitives/CircularArrowNode.cpp
    modules/viz/Primitives/Primitives/WireframeBox.cpp
    modules/viz/Primitives/Primitives/LinesNode.cpp
    modules/viz/Primitives/Primitives/RingNode.cpp
    modules/viz/Primitives/Primitives/SphereNode.cpp
    modules/viz/Primitives/Primitives/TextNode.cpp
    modules/viz/Primitives/Primitives/Graph_2D.cpp

    HEADERS
    modules/viz/Primitives/PrimitivesFactory.h
    modules/viz/Primitives/Primitives/ArrowNode.h
    modules/viz/Primitives/Primitives/CircularArrowNode.h
    modules/viz/Primitives/Primitives/WireframeBox.hpp
    modules/viz/Primitives/Primitives/LinesNode.h
    modules/viz/Primitives/Primitives/RingNode.hpp
    modules/viz/Primitives/Primitives/SphereNode.hpp
    modules/viz/Primitives/Primitives/TextNode.hpp
    modules/viz/Primitives/Primitives/Graph_2D.hpp
    DEPS osgViz)

rock_library(ManipulationClickHandler
    SOURCES
    modules/ManipulationClickHandler/ManipulationClickHandler.cpp
    modules/ManipulationClickHandler/TranslateRotateDragger.cpp
    HEADERS
    modules/ManipulationClickHandler/ManipulationClickHandler.h
    modules/ManipulationClickHandler/TranslateRotateDragger.h
    DEPS osgViz PrimitivesFactory)

    

rock_library(ModelLoader
    SOURCES modules/viz/ModelLoader/ModelLoader.cpp
    HEADERS modules/viz/ModelLoader/ModelLoader.h
    DEPS osgViz)
    
rock_executable(osgViz_bin Main.cpp
    DEPS osgViz PrimitivesFactory ModelLoader
    LIBS ${ADDITIONAL_LIBS})




