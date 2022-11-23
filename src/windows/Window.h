/*
* Window.h
*
*  Created on: 17.02.2015
*      Author: planthaber
*/

#ifndef OSGVIZ_OSGVIZ_SRC_WINDOWS_WINDOW_H_
#define OSGVIZ_OSGVIZ_SRC_WINDOWS_WINDOW_H_

#include <iostream>
#include <vector>

#include <osgViewer/CompositeViewer>
#include <osgParticle/PrecipitationEffect>

#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/TerrainManipulator>
#include <osgGA/FlightManipulator>

#include <osg/GraphicsContext>
#include <osg/Group>
#include <osg/StateSet>

#include "../graphics/interfaces/data/GraphicData.h"
#include "config/WindowConfig.h"

#include "SuperView.h"

namespace osgviz {

    class Window : public osgViewer::CompositeViewer {

    friend class WindowManager;

    public:

        Window(WindowConfig windowConfig, osg::ref_ptr<osg::Node> windowScene, osg::ref_ptr<osg::GraphicsContext> graphicsContext = NULL);
        virtual ~Window();

        //	void setScene(osg::Group *scene);

        //	osg::Group* getScene();

        //void setName(const std::string& name);

        osgViewer::View* addView(ViewConfig viewConfig, osg::Group* viewScene = NULL);

        osg::ref_ptr<osgviz::HUD> addHUD(int width,int height, osg::Camera::ProjectionResizePolicy policy = osg::Camera::ProjectionResizePolicy::FIXED, int viewID = 0);


        osg::ref_ptr<osgViewer::GraphicsWindow> getGraphicsWindow();

        void addChild(osg::ref_ptr<osg::Node> scene) {
            root->addChild(scene);
        }    

        void setWindowGeometry(int posX, int posY, int width, int height, int window=0);

        void setFullscreen(bool state = true, int window=0, int screen = 0);

        void showRain(const bool &val = true);
        void showSnow(const bool &val = true);
        //void showFog(const bool &val = true);    

        /*	void frame();

        void enableCameraControl();
        void disableCameraControl();

        inline osg::ref_ptr<osgViewer::CompositeViewer> getViewer(){
        return viewer;
        }*/

    	inline osg::ref_ptr<osgViewer::View> getView(unsigned int index = 0){
			if (index < views.size()){
				return (osgViewer::View*)views.at(index);
			}
            return osg::ref_ptr<osgViewer::View>();
		}

  		inline osg::ref_ptr<osgviz::SuperView> getSuperView(unsigned int index = 0){
			if (index < views.size()){
				return views.at(index);
			}
            return osg::ref_ptr<osgviz::SuperView>();
		}

        inline osg::ref_ptr<osg::Group> getRootNode(){
            return root;
        }

        inline void clear(){
            root->removeChild(0,root->getNumChildren());
        }

        inline osg::ref_ptr<osg::GraphicsContext> getGraphicsContext(){
            return graphicsContext;
        }

    protected:
        osg::ref_ptr<osg::Group> root;

    private:
        WindowConfig windowConfig;

        osg::ref_ptr<osg::GraphicsContext> graphicsContext;

        osg::ref_ptr<osgViewer::GraphicsWindow> gw;

        osg::ref_ptr<osgParticle::PrecipitationEffect> snow, rain;

        std::vector < osg::ref_ptr<osgviz::SuperView> > views;


    };

} /* namespace osgviz */

#endif /* OSGVIZ_OSGVIZ_SRC_WINDOWS_WINDOW_H_ */
