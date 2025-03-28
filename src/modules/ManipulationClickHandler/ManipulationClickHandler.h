#ifndef GUI_OSGVIZ_OSGVIZ_PLUGINS_MANIPULATION_CLICK_HANDLER_MANIPULATION_CLICK_HANDLER_H
#define GUI_OSGVIZ_OSGVIZ_PLUGINS_MANIPULATION_CLICK_HANDLER_MANIPULATION_CLICK_HANDLER_H

#include <boost/signals2.hpp>
#include "../../interfaces/Clickable.h"
#include "../../Object.h"
#include <osgManipulator/Dragger>
#include "../../windows/EventHandlers/NullClickObject.h"
#include <osg/Referenced>

namespace osgviz {


    /** A click handler that aggregates TranslateInPlaneCommands and Rotate3DCommands
    *  and forwards them once the user has finished dragging.

    *  
    * FIXME small usage example */
    class ManipulationClickHandler : public osgviz::Clickable
    {
        //TODO verschiedene Dragger unterstüzen, user auswählen lassen
    public:

        ManipulationClickHandler();

        /**Called when the user clicks on the object */
        virtual bool clicked(const int& buttonMask, const osg::Vec2d& cursor,
            const osg::Vec3d& world, const osg::Vec3d& local,
            Clickable* object, const int modKeyMask,
            osgviz::WindowInterface* window = 0);

        /** is called whenever the user moves the dragger */
        virtual bool receive(const osgManipulator::MotionCommand& command);

        /** Select the specified object.  */
        void selectObject(osgviz::Object* obj);

        /** Deselect whatever was selected before */
        void deselectCurrentObject();

        /** Enabled/disable the handler */
        void setEnabled(const bool value);

        void resetClickedObject();

        /**This signal is emitted once the movement is complete.
        * I.e. the user has clicked the object, dragged it and released it.
        * @param motion the final motion of the object relative to the original object
        *               position.*/
        boost::signals2::signal<void (const osgviz::Object* obj, const osg::Matrix& motion)>
            objectMoved;

        /**This signal is emiitted while the user is moving an object. */
        boost::signals2::signal<void (const osgviz::Object* obj, const osg::Matrix& motion)>
            objectMoving;

        /**This signal is emitted whenever the user selectes a new object*/
        boost::signals2::signal<void (const osgviz::Object* obj)> objectSelected;




    private:

        /** A simple handler for drag events. It just forwards the events to the ManipulationClickHandler.
        * 
        *  @Note The ManipulationClickHandler does not inherit from DraggerCallback directly 
        *        to avoid a bug in memory managemengt. The Dragger uses an osg::ref_ptr to manage 
        *        pointers to the callbacks. Thus wenn calling addDraggerCallback(this), it creates a 
        *        ref_ptr to this. When the callback is removed for whatever reason, the ref_ptr would
        *        free this. If the ManipulationClickHandler is freed, this behavior leads to double free
        *        and memory corruption. */
        struct ManipulationDraggerCallback : public osgManipulator::DraggerCallback 
        {
            ManipulationDraggerCallback(ManipulationClickHandler& handler);
            //is called whenever the user moves the dragger
            virtual bool receive(const osgManipulator::MotionCommand& command);
            ManipulationClickHandler& handler;
        };



        osgviz::Object* clickedObject;
        osg::ref_ptr<osgManipulator::Dragger> translationDragger;
        osg::ref_ptr<osg::Group> draggerGroup; //contains both draggers
        /**NullClickObject is used a parent for the draggers to avoid propagating
        * click events from the dragger to the dragged node*/
        osg::ref_ptr<NullClickObject> translationDraggerParent;
        osg::ref_ptr<ManipulationDraggerCallback> draggerCallback;

        osg::Matrix initialMotionMatrix;
        osg::Matrix worldToObject; //transformation from world to selected object
        osg::Matrix objectToWorld;
        osg::Matrix currentMotionMatrix;
        bool moved; //inidicates whether the user actually moved an object or just clicked & released
        bool enabled;
    };

}

#endif
