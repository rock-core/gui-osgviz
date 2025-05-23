/*
* Window.h
*
*  Created on: 17.02.2015
*      Author: planthaber
*/

#ifndef OSGVIZ_OSGVIZ_SRC_WINDOWS_VIEW_H_
#define OSGVIZ_OSGVIZ_SRC_WINDOWS_VIEW_H_

#include <iostream>
#include <stdexcept>

#include <osgViewer/View>
#include <osgParticle/PrecipitationEffect>

#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/TerrainManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/OrbitManipulator>

#include <osgDB/WriteFile>

#include "config/WindowConfig.h"
#include "EventHandlers/HierarchicalEventHandler.h"

#include "CameraManipulator/TerrainZoomManipulator.hpp"

#include "../interfaces/WindowInterface.h"
#include "../graphics/interfaces/data/GraphicData.h"
#include "../graphics/interfaces/data/LightData.h"

#include "../graphics/wrapper/OSGLightStruct.h"

#include "EventHandlers/MouseMoveEvent.h"
#include "EventHandlers/ObjectSelector.h"
#include "EventHandlers/WindowResizeEvent.h"

#include "HUD/HUD.h"

namespace osgviz {

    /**
    * internal struct to manage lights
    */
    struct lightmanager {
        osg::ref_ptr<osg::LightSource> lightSource;
        osg::ref_ptr<osg::Light> light;
        interfaces::LightData lStruct;
        bool free;
    };	

    class SuperView : public osgViewer::View, public WindowInterface {

    public:
        SuperView();

        SuperView(ViewConfig viewConfig, osg::GraphicsContext* graphicsContext, osg::Group* viewScene = NULL);

        ~SuperView();

        virtual void enableEventHandling();
        virtual void disableEventHandling();

        /**
        *
        * @param priority lower values are called after high values, it the value is taken, the handler is added below the current
        * the 3d Click events have a priority of 0
        * @param handler
        * @return the actual priority set (not equal precedence, if it was taken)
        */
        virtual int addEventHandler(osg::ref_ptr<osgGA::GUIEventHandler> handler, int priority = 0);

        virtual void removeEventHandler(osg::ref_ptr<osgGA::GUIEventHandler> handler);


        virtual void enableCameraControl();
        virtual void disableCameraControl();

        virtual void setCursorShape(int cursor);
        virtual void setCursorPos(int x, int y);

        void addChild(osg::ref_ptr<osg::Group> scene) {
            root->addChild(scene);
        }

        void setRoot(osg::ref_ptr<osg::Group> scene){
            root = scene;
        }

        void setFogSettings(const osgviz::interfaces::GraphicData &graphicOptions);

        inline osg::ref_ptr<osgGA::KeySwitchMatrixManipulator> getKeyswitchMatrixManipulator(){
            return keyswitchManipulator;
        }

        void disableCameraManipulator(){
            setCameraManipulator(NULL);
        }

        void enableCameraManipulator(){
            setCameraManipulator(keyswitchManipulator);
        }

        inline std::vector<lightmanager> getLights(){
            return myLights;
        }

        osg::ref_ptr<osgviz::HUD> addHUD(int width,int height, osg::Camera::ProjectionResizePolicy policy = osg::Camera::ProjectionResizePolicy::FIXED);

        osg::ref_ptr<osgviz::HUD> getHUD(const int &id){
            return huds[id];
        }

        inline void addMouseMoveCallback(MouseMoveCallback* cb){
            mouseMoveEvent->addCallback(cb);
        }	 	

        /**
        * in case you want to use a non-default camera for intersection calculation it can be set using this function
        * e.g. use the richt eye cam of the osgoculuswiewer instaed of the scene camera
        * @param cam
        */
        inline void setObjectSelectorCamera(const osg::ref_ptr<osg::Camera>& cam){
            objectSelectorEvent->setCamera(cam);
        }

        osg::ref_ptr<osgViewer::StatsHandler> getStatsHandler() {
            return statsHandler;
        }

    private:
        ViewConfig viewConfig;

        osg::ref_ptr<HierarchicalEventHandler> hierarchicalEventHandler;
        bool eventsEnabled;

        osg::ref_ptr<osg::GraphicsContext> graphicsContext;

        osg::ref_ptr<osgParticle::PrecipitationEffect> snow;
        osg::ref_ptr<osgParticle::PrecipitationEffect> rain;
        osg::ref_ptr<osg::Fog> fog;

        osg::ref_ptr<osgGA::KeySwitchMatrixManipulator> keyswitchManipulator;

        osg::ref_ptr<osg::Group> root;

        osg::ref_ptr<osg::StateSet> globalStateset;	

        // includes osg::lights, osg::lightsource, lightstruct and flag to check if full
        std::vector<lightmanager> myLights;		

        osg::ref_ptr<osg::Group> lightGroup;

        lightmanager defaultLight;	

        osgviz::interfaces::GraphicData graphicData;

        std::vector < osg::ref_ptr<osgviz::HUD> > huds;

        osg::ref_ptr<osgViewer::StatsHandler> statsHandler;
        osg::ref_ptr<MouseMoveEvent> mouseMoveEvent;
        osg::ref_ptr<ObjectSelector> objectSelectorEvent;
        osg::ref_ptr<WindowResizeEvent> windowResizeEvent;

        void initDefaultLight();
    };

} /* namespace osgviz */

#endif /* OSGVIZ_OSGVIZ_SRC_WINDOWS_VIEW_H_ */
