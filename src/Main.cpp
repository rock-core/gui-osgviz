#include "OsgViz.hpp"

#include <osg/LOD>
#include <osgUtil/Simplifier>
#include <iostream>

#include "modules/viz/Primitives/PrimitivesFactory.h"
#include "modules/viz/ModelLoader/ModelLoader.h"
#include "tools/GraphEditor.h"

#ifndef WIN32
#include <unistd.h>//sleep
#else
#include <Windows.h>

#define sleep(S) Sleep(S*1000)
#endif
#include <stdio.h>

#include <osgViz/windows/SuperView.h>
#include <osgViz/windows/HUD/HUD.h>


class Output : public osgviz::Clickable{
    virtual bool clicked(const int &buttonMask, const osg::Vec2d &cursor,
        const osg::Vec3d &world, const osg::Vec3d &local,
        Clickable* object, const int modifierMask,
        osgviz::WindowInterface* window = NULL){
            osgviz::Object* osgvizObject = dynamic_cast<osgviz::Object*> (object);
            if (osgvizObject){
                printf("clicked %s (%.2f %.2f %.2f)\n",osgvizObject->getName().c_str(),world.x(),world.y(),world.z());
                return true;
            }else{
                printf("clicked (%.2f %.2f %.2f)\n",world.x(),world.y(),world.z());
                return false;
            }
    }
};


int main(int argc, char** argv)
{
    printf("getting instance\n");
    fflush(stdout);
    osg::ref_ptr<osgviz::OsgViz> osgViz = osgviz::OsgViz::getInstance();

    //load lib with some helpful primitives
    printf("load modules\n");	fflush(stdout);
    //osgviz::PrimitivesFactory *primitivesfactory = new osgviz::PrimitivesFactory(NULL);
    std::shared_ptr<osgviz::PrimitivesFactory> primitivesfactory = osgviz::OsgViz::getModuleInstance<osgviz::PrimitivesFactory>("PrimitivesFactory");

    std::shared_ptr<osgviz::PrimitivesFactory> primitivesfactory2 = osgviz::OsgViz::getModuleInstance<osgviz::PrimitivesFactory>("PrimitivesFactory");

    std::shared_ptr<osgviz::PrimitivesFactory> primitivesfactory3 = osgviz::OsgViz::getModuleInstance<osgviz::PrimitivesFactory>("PrimitivesFactory2");

    try {
        std::shared_ptr<osgviz::ModelLoader> primitivesfactory3 = osgviz::OsgViz::getModuleInstance<osgviz::ModelLoader>("PrimitivesFactory2");

    } catch (std::exception& e){
        printf("ERROR: %s\n", e.what());
    }


    osgviz::OsgViz::printModules();

    printf("PF \n\t%p \n\t%p\n\t%p\n",primitivesfactory.get(),primitivesfactory2.get(),primitivesfactory3.get());

    if (!primitivesfactory){
        printf("plugin not found\n");	fflush(stdout);
    }

    osg::ref_ptr<osgviz::Object> grid = primitivesfactory->createGrid();
    osgViz->addChild(grid);

    osg::ref_ptr<osgviz::Object> arrow = primitivesfactory->createArrow();
    arrow->rotate(M_PI/2.0,osg::Vec3d(0,1,0));
    arrow->setName("Arrow");
    osgViz->addChild(arrow);

    osg::ref_ptr<osgviz::Object> axes = primitivesfactory->createAxes();
    osgViz->addChild(axes);

    printf("creating window\n");fflush(stdout);

    int winid = 0;
	if (argc >= 2){
		if (strcmp(argv[1],"fullscreen")==0){
            osgviz::WindowConfig windowConf;
            windowConf.fullScreen = true;


            winid = osgViz->createWindow(windowConf);
		}
	}else{
        winid = osgViz->createWindow();
    }

    osg::ref_ptr<osgviz::Window> window = osgViz->getWindowManager()->getWindowByID(winid);

    osg::ref_ptr<osgviz::HUD> hud = window->addHUD(1920,1080,osg::Camera::ProjectionResizePolicy::FIXED);

    osg::ref_ptr<osgviz::Object> hudarrow = primitivesfactory->createArrow();
    hudarrow->setPosition(100,100,0);
    hudarrow->setScale(200,200,200);
    hud->addHudObject(hudarrow);


    osg::ref_ptr<osg::Geode> geode = new osg::Geode();
    osg::ref_ptr<osgText::Text> text = new  osgText::Text;
    geode->addDrawable( text );
    osg::Vec3 position(150.0f,800.0f,0.0f);
    text->setPosition(position);
    text->setText("Head Up Displays are simple :-)");
    hud->addHudObject(geode);



    osg::ref_ptr<osgviz::Object> axes2 = primitivesfactory->createArrow();
    axes2->setName("Head Up Displays are really simple :-)");
    //axes2->displayName(0.3);
    axes2->setScale(100,100,100);
    axes2->setPosition(100,100,0);
    axes2->rotate(M_PI/2.0,osg::Vec3(1,0,0));
    hud->addHudObject(axes2);


    osg::ref_ptr<osgviz::PrimitivesFactory::Shape> shape = primitivesfactory->createShape(osgviz::PrimitivesFactory::BOX,100,100,0);
    shape->setPosition(200,200,0);
    //shape->setScale(100,100,100);
    shape->setName("BoxBoxBoxBox");
    shape->displayName(10);
    shape->setColor(1,0,0,0.5);

    std::shared_ptr<Output> bouxout(new Output);
    shape->addClickableCallback(bouxout);

    hud->addHudObject(shape);

    hud->makeObjectScaleOnHover(shape.get(),osg::Vec3d(100,100,0),osg::Vec3d(2,2,1),osgviz::HUDHoverScaler::NE);
    hud->changeObjectPositionByResize(shape.get(), osg::Vec3d(10,10,0), osg::Vec2d(100, 100));


    osg::ref_ptr<osgviz::PrimitivesFactory::Shape> shape1 = primitivesfactory->createShape(osgviz::PrimitivesFactory::BOX,0.5,0.5,0.5);
    shape1->setColor(1,0,0,0.5);
    //shape1->setPosition(0.5,0.5,0.5);
    osgViz->addChild(shape1);


    //test 2nd HUD

    osg::ref_ptr<osgviz::HUD> hud2 = osgViz->getWindowManager()->getWindowByID(winid)->getSuperView(0)->addHUD(1920,1080);
    osg::ref_ptr<osgviz::PrimitivesFactory::Shape> cone = primitivesfactory->createShape(osgviz::PrimitivesFactory::CONE,100,100,0);
    cone->setPosition(700,500,0);
    //shape->setScale(100,100,100);
    cone->rotate(M_PI/2.0,osg::Vec3(1,0,0));
    cone->setName("cone");
    cone->displayName(10);
    cone->setColor(1,0,0,0.5);
    hud2->addHudObject(cone);


    // add a lod node
    osg::ref_ptr<osg::LOD> lodnode = new osg::LOD();
    lodnode->setRangeMode(osg::LOD::DISTANCE_FROM_EYE_POINT);
    GraphEditor::insertNode(arrow, lodnode);
    // set range for the moved child
    lodnode->setRange(0, 0, 10);

    osg::ref_ptr<osgviz::Object> lodaxes = primitivesfactory->createAxes();
    lodaxes->setPosition(-1,0,0);
    lodnode->addChild(lodaxes, 10, FLT_MAX);

    // osg::ref_ptr<osgviz::Object> lodimage = primitivesfactory->loadImage("test.png",1,1);
    // lodimage->setPosition(-0.5,-0.5,0);
    // lodimage->rotate(M_PI, osg::Vec3d(0,0,1));
    // lodnode->addChild(lodimage, 10, FLT_MAX);

    osg::ref_ptr<osg::Node> lowres = dynamic_cast<osg::Node*>(arrow->clone(osg::CopyOp::DEEP_COPY_ALL));
    osgUtil::Simplifier simplifer;
    simplifer.setSampleRatio(0.5);
    lowres->accept(simplifer);
    lodnode->addChild(lowres, 12, FLT_MAX);


    //sleep(1);
    //you can create multiple windows and views ones:
    //	int win2 = osgViz->createWindow();
    //	osg::ref_ptr<osgviz::Window> window2 = osgViz->getWindowManager()->getWindowByID(win2);
    //	osgviz::ViewConfig view(0,0,0.3,0.3);
    //	window2->addView(view);
    
    //this starts a update thread for the visualisation, requires locking in the other threads:
    //osgViz->lockThread();
    //do updates
    //osgViz->unlockThread();
    //printf("startThread\n");fflush(stdout);
    //osgViz->startThread();

    //or don't start the thread and update manually using osgViz->update();

    while (!osgViz->done()){
        //osgViz->lockThread();
        //do updates
        //osgViz->unlockThread();
        osgViz->update();
        //printf("sleep\n");fflush(stdout);

        //sleep(5);
        //osgViz->getWindowManager()->getWindowByID(0)->setFullscreen(true);
        //sleep(5);
        //osgViz->getWindowManager()->getWindowByID(0)->setFullscreen(false);
        //sleep(5);
        //osgViz->getWindowManager()->getWindowByID(0)->setWindowGeometry(150,150,1240,768);
        //sleep(5);
    }

    return 0;
}
