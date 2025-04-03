#include "ManipulationClickHandler.h"
#include "TranslateRotateDragger.h"
#include <iostream>
#include <osg/io_utils>
#include <osgManipulator/RotateSphereDragger>
#include <osgManipulator/Dragger>
#include "../../Object.h"

using namespace osgManipulator;
using namespace osg;
namespace osgviz {

    ManipulationClickHandler::ManipulationClickHandler() : clickedObject(NULL),
        translationDragger(new TranslateRotateDragger()), 
        draggerGroup(new osg::Group()), translationDraggerParent(new NullClickObject()),
        draggerCallback(new ManipulationDraggerCallback(*this)), enabled(true)
    {
        translationDragger->setupDefaultGeometry();
        translationDragger->setHandleEvents(true);//allow the dragger to move itself when dragged
        translationDragger->addDraggerCallback(draggerCallback);
        translationDraggerParent->addChild(translationDragger);

        /**The group was added because some code in vizkit3d assumes that all custom
        * elements in a node are inside a group and breaks if this is not the case.
        * Also it is easier to extend the draggers later on if all the dragger components
        * are grouped :-)*/
        draggerGroup->setName("Translation & Rotation dragger group");
        draggerGroup->addChild(translationDraggerParent.get());
    }

    bool ManipulationClickHandler::clicked(const int& buttonMask, const Vec2d& cursor,
        const Vec3d& world, const Vec3d& local,
        Clickable* object, const int modKeyMask,
        WindowInterface* window)
    {
        if(!enabled)
            return false;

        osgviz::Object* obj = dynamic_cast<osgviz::Object*>(object);
        if(obj != NULL && obj != clickedObject)
        {
            selectObject(obj);
            return true; 
        }
        else
        {
            //FIXME replace with real logging
            std::cerr << "osgziv: WARNING ManipulationClickHandler can only be attached " 
                << "to osgviz::Object" << std::endl;
            return false;
        }
    }

    void ManipulationClickHandler::deselectCurrentObject()
    {
        if(clickedObject != NULL)
        {
            //dont know which dragger was used, just remove both of them and 
            //ignore the return value :)
            clickedObject->removeChild(draggerGroup.get());
            clickedObject = NULL;
        }
    }

    void ManipulationClickHandler::selectObject(osgviz::Object* obj)
    {
        //FIXME kollidiert das mit dem TransformerGraph weil der davon ausgeht,
        //      dass alle user knoten in der Group sind?
        deselectCurrentObject();
        clickedObject = obj;
        obj->addChild(draggerGroup);
        objectSelected(clickedObject);
    }

    bool ManipulationClickHandler::receive(const MotionCommand& command)
    {
        if(!enabled)
            return false;

        if(command.getStage() == MotionCommand::START)
        {
            moved = false;
        }
        else if(command.getStage() == MotionCommand::MOVE)
        {
            moved = true;
            osg::NodePath pathFromObjToRoot;
            osgManipulator::computeNodePathToRoot(*clickedObject, pathFromObjToRoot);
            objectToWorld = osg::computeLocalToWorld(pathFromObjToRoot);
            worldToObject = osg::Matrix::inverse(objectToWorld);
            //copy paste from osgManipulator/Dragger.cpp
            // Transform the command's motion matrix into local motion matrix.
            currentMotionMatrix = objectToWorld * command.getWorldToLocal()
                * command.getMotionMatrix() * command.getLocalToWorld() 
                * worldToObject;

            objectMoving(clickedObject, currentMotionMatrix);
        }
        else if(command.getStage() == MotionCommand::FINISH)
        {
            //only update the position when the user has actually moved the object
            //otherwise the object will move by a very small amout due to math inaccuracy
            if(moved)
                objectMoved(clickedObject, currentMotionMatrix);
            //reset the position of the dragger relative to the object after signaling
            //that the object should be moved,
            translationDragger->setMatrix(Matrixd());
        }
        else
        {
            return false;
        }
        return true;
    }

    void ManipulationClickHandler::setEnabled(const bool value)
    {
        enabled = value;
        if(!enabled)
            deselectCurrentObject();
    }


    ManipulationClickHandler::ManipulationDraggerCallback::ManipulationDraggerCallback(ManipulationClickHandler& handler) :
        handler(handler){}

    bool ManipulationClickHandler::ManipulationDraggerCallback::receive(const osgManipulator::MotionCommand& command)
    {
        return handler.receive(command);
    }

    void ManipulationClickHandler::resetClickedObject() {
        clickedObject = NULL;
    }

}