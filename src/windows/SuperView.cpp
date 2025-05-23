#include "SuperView.h"

#include <stdio.h>

#include <osg/LightModel>

#include <osgViewer/ViewerEventHandlers>

#ifdef _MSC_VER
#define __PRETTY_FUNCTION__  __FUNCTION__ __FUNCSIG__
#endif

namespace osgviz {

    SuperView::SuperView() : osgViewer::View() {}

    SuperView::SuperView(ViewConfig viewConfig, osg::GraphicsContext* graphicsContext, osg::Group* viewScene) 
        : osgViewer::View(), 
        viewConfig(viewConfig),
        graphicsContext(graphicsContext),
        root(new osg::Group),
        globalStateset(new osg::StateSet),
        lightGroup(new osg::Group) {

            if (graphicsContext == NULL)
                throw std::runtime_error("SuperView: Graphic Context is null pointer.");
            else
                getCamera()->setGraphicsContext(graphicsContext);

            // set background color
            getCamera()->setClearColor(osg::Vec4(viewConfig.clearColorRed,
                viewConfig.clearColorGreen,
                viewConfig.clearColorBlue,
                viewConfig.clearColorAlpha));

            // calculate the size and position of view
            const osg::GraphicsContext::Traits* traits = getCamera()->getGraphicsContext()->getTraits();
            int posX = traits->width * viewConfig.posX;
            int posY = traits->height * viewConfig.posY;
            int width = traits->width * viewConfig.width;
            int height = traits->height * viewConfig.height;

            // set projection matrix
            double fovy, aspectRatio, zNear, zFar;
            getCamera()->getProjectionMatrixAsPerspective(fovy, aspectRatio, zNear, zFar);

            double newAspectRatio = double(width) / double(height);
            double aspectRatioChange = newAspectRatio / aspectRatio;
            if (aspectRatioChange != 1.0) {
                getCamera()->getProjectionMatrix() *= osg::Matrix::scale(1.0 / aspectRatioChange, 1.0, 1.0);
            }	    

            getCamera()->setViewport(new osg::Viewport(posX, posY, width, height));  

            // set buffer
            GLenum buffer = traits->doubleBuffer ? GL_BACK : GL_FRONT;
            getCamera()->setDrawBuffer(buffer);
            getCamera()->setReadBuffer(buffer);	 

            // set camera manipulator
            keyswitchManipulator = new osgGA::KeySwitchMatrixManipulator;  
            keyswitchManipulator->addMatrixManipulator( '0', "NONE", NULL );
            keyswitchManipulator->addMatrixManipulator( '1', "Terrain", new osgGA::TerrainManipulator() );
            keyswitchManipulator->addMatrixManipulator( '2', "Flight", new osgGA::FlightManipulator() );
            keyswitchManipulator->addMatrixManipulator( '3', "TerrainZoom", new TerrainZoomManipulator() );
            keyswitchManipulator->selectMatrixManipulator('1');

            setCameraManipulator(keyswitchManipulator);

            eventsEnabled = false;
            if (viewConfig.handlesEvents == true) {
                enableEventHandling();
            }    

            root->setName("SuperView root");
            setSceneData(root);
            addChild(viewScene);

            globalStateset->setGlobalDefaults();
            globalStateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
            globalStateset->setMode(GL_LIGHTING, osg::StateAttribute::ON);
            globalStateset->setMode(GL_LIGHT0, osg::StateAttribute::ON);
            globalStateset->setMode(GL_LIGHT1, osg::StateAttribute::OFF);
            globalStateset->setMode(GL_LIGHT2, osg::StateAttribute::OFF);
            globalStateset->setMode(GL_LIGHT3, osg::StateAttribute::OFF);
            globalStateset->setMode(GL_LIGHT4, osg::StateAttribute::OFF);
            globalStateset->setMode(GL_LIGHT5, osg::StateAttribute::OFF);
            globalStateset->setMode(GL_LIGHT6, osg::StateAttribute::OFF);
            globalStateset->setMode(GL_LIGHT7, osg::StateAttribute::OFF);
            globalStateset->setMode(GL_BLEND,osg::StateAttribute::OFF);
            root->setStateSet(globalStateset.get()); 

            // some fixed function pipeline stuff...
            // i guess the default is smooth shading, that means
            // light influence is calculated per vertex and interpolated for fragments.
            osg::ref_ptr<osg::LightModel> myLightModel = new osg::LightModel;
            myLightModel->setTwoSided(false);
            globalStateset->setAttributeAndModes(myLightModel.get(), osg::StateAttribute::ON);

            root->addChild(lightGroup.get());

            // init light (osg can have only 8 lights enabled at a time)
            for (unsigned int i =0; i<8;i++) {
                lightmanager ltemp;
                ltemp.free=true;
                myLights.push_back(ltemp);
            }

            initDefaultLight();    

            // background color for the scene
            graphicData.clearColor = osgviz::Color(0.55, 0.67, 0.88, 1.0);

            // setup FOG
            // graphicData.fogColor = osgviz::Color(0.2, 0.2, 0.2, 1.0);
            // graphicData.fogEnabled = true;
            // graphicData.fogDensity = 0.35;
            // graphicData.fogStart = 10.0;
            // graphicData.fogEnd = 300.0;

            // setFogSettings(graphicData);    

            // Event Handler Factory
            statsHandler = new osgViewer::StatsHandler();
            addEventHandler(statsHandler,-100);

            objectSelectorEvent = new ObjectSelector(this);
            addEventHandler(objectSelectorEvent,10);

            mouseMoveEvent = new MouseMoveEvent();
            addEventHandler(mouseMoveEvent,-50);

            windowResizeEvent = new WindowResizeEvent();
            addEventHandler(windowResizeEvent, 0);	
    }

    SuperView::~SuperView() {
    }

    osg::ref_ptr<osgviz::HUD> SuperView::addHUD(int width,int height, osg::Camera::ProjectionResizePolicy policy){

        osg::ref_ptr<osgviz::HUD> hud = new HUD(graphicsContext,width,height,policy);
        hud->setViewport(getCamera()->getViewport());

        // set hud to the full viewport
        // TODO: use additional attribute to specify the size of hud: full viewport size or user-defined size 
        hud->resize();

        this->addChild(hud);

        // set events
        hud->setMouseMoveEvent(mouseMoveEvent.get());
        hud->setWindowResizeEvent(windowResizeEvent.get());

        huds.push_back(hud);

        return hud;
    }

    void SuperView::setFogSettings(const osgviz::interfaces::GraphicData &graphicOptions) {
        osg::ref_ptr<osg::Fog> myFog = (osg::Fog*)globalStateset->getAttribute(osg::StateAttribute::FOG);
        if (myFog.valid() == false) {
            myFog = new osg::Fog;
        }

        myFog->setMode(osg::Fog::LINEAR);

        graphicData = graphicOptions;

        // in original GraphicsManager code		
        //      for(unsigned int i=0; i<graphicsWindows.size(); i++)
        //        graphicsWindows[i]->setClearColor(graphicOptions.clearColor);	

        // Q: in original code
        // myFog->setColor(osg::Vec4(graphicOptions.fogColor.r,
        //								graphicOptions.fogColor.g,
        //								graphicOptions.fogColor.b, 1.0));

        myFog->setColor(osg::Vec4(graphicData.fogColor.x(),
            graphicData.fogColor.y(),
            graphicData.fogColor.z(), 1.0));
        myFog->setStart(graphicData.fogStart);
        myFog->setEnd(graphicData.fogEnd);
        myFog->setDensity(graphicData.fogDensity);
        if (graphicData.fogEnabled == true) {
            globalStateset->setAttributeAndModes(myFog.get(), osg::StateAttribute::ON);
            // in original GraphicsManager code		
            //        useFog = true;
        } else {
            globalStateset->setMode(GL_FOG, osg::StateAttribute::OFF);
            // in original GraphicsManager code			
            //        useFog = false;		
        }		

        // in original GraphicsManager code			
        //      map<unsigned long, osg::ref_ptr<OSGNodeStruct> >::iterator iter;
        //
        //      for(iter=drawObjects_.begin(); iter!=drawObjects_.end(); ++iter)
        //        iter->second->object()->setUseFog(useFog);	
    }

    int SuperView::addEventHandler(osg::ref_ptr<osgGA::GUIEventHandler> handler,int priority) {
        if (!eventsEnabled){
            enableEventHandling();
        }
        return hierarchicalEventHandler->addEventHandler(handler,priority);
    }

    void SuperView::removeEventHandler(osg::ref_ptr<osgGA::GUIEventHandler> handler) {
        if (hierarchicalEventHandler.valid() == true){
            hierarchicalEventHandler->removeEventHandler(handler);
        }
    }

    void SuperView::enableEventHandling() {
        eventsEnabled = true;
        if (hierarchicalEventHandler.valid() != true){
            hierarchicalEventHandler = new HierarchicalEventHandler();
        }
        osgViewer::View::addEventHandler(hierarchicalEventHandler);
    }

    void SuperView::disableEventHandling() {
        if (hierarchicalEventHandler.valid() == true) {
            osgViewer::View::removeEventHandler(hierarchicalEventHandler.get());
        } else {
            OSG_WARN << "SuperView::deactivateObjectSelector: No object selector was active at the moment." << std::endl;
        }
    }

    void SuperView::enableCameraControl() {
        keyswitchManipulator->selectMatrixManipulator('1');
    }

    void SuperView::disableCameraControl() {
        keyswitchManipulator->selectMatrixManipulator('0');
    }

    void SuperView::setCursorShape(int cursor){
        printf("not implemented : %s\n", __PRETTY_FUNCTION__);
        //graphicsWindow->setCursor((osgViewer::GraphicsWindow::MouseCursor)cursor);
    }
    void SuperView::setCursorPos(int x, int y){
        printf("not implemented : %s\n", __PRETTY_FUNCTION__);
        //graphicsWindow->requestWarpPointer(x,y);
        //	keyswitchManipulator->selectMatrixManipulator('0');
    }


    void SuperView::initDefaultLight() {
        defaultLight.lStruct.pos = osg::Vec3f(0.0, 0.0, 10.0);
        defaultLight.lStruct.ambient = osgviz::Color(0.0, 0.0, 0.0, 1.0);
        defaultLight.lStruct.diffuse = osgviz::Color(1.0, 1.0, 1.0, 1.0);
        defaultLight.lStruct.specular = osgviz::Color(1.0, 1.0, 1.0, 1.0);
        defaultLight.lStruct.constantAttenuation = 0.0;
        defaultLight.lStruct.linearAttenuation = 0.0;
        defaultLight.lStruct.quadraticAttenuation = 0.00001;
        defaultLight.lStruct.directional = false;
        defaultLight.lStruct.type = osgviz::OMNILIGHT;
        defaultLight.lStruct.index = 0;
        defaultLight.lStruct.angle = 0;
        defaultLight.lStruct.exponent = 0;

        osg::ref_ptr<osg::LightSource> myLightSource = new osgviz::graphics::OSGLightStruct(defaultLight.lStruct);

        //add to lightmanager for later editing possibility
        defaultLight.light = myLightSource->getLight();
        defaultLight.lightSource = myLightSource;
        defaultLight.free = false;

        lightGroup->addChild( myLightSource.get() );
        globalStateset->setMode(GL_LIGHT0, osg::StateAttribute::ON);
        myLightSource->setStateSetModes(*globalStateset, osg::StateAttribute::ON);
    }





}

