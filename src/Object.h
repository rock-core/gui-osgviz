/*
* Object.h
*
*  Created on: 08.01.2015
*      Author: planthaber
*/


#include <osg/Node>
#include <osgText/Font>
#include <osgText/Text>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <osg/AutoTransform>

#include <string>
#include <memory>

#include "interfaces/Clickable.h"

#include <stdio.h>

#ifndef SRC_PLUGINS_OBJECT_H_
#define SRC_PLUGINS_OBJECT_H_


namespace osgviz {


    class Object: public osg::PositionAttitudeTransform, public Clickable{

    public:
        Object();

        virtual ~Object();

        virtual bool clicked(const int &buttonMask, const osg::Vec2d &cursor,
            const osg::Vec3d &world, const osg::Vec3d &local,
            osgviz::Clickable* object, const int modKeyMask,
            WindowInterface* window = NULL);

        virtual bool dragged(const int &buttonMask, const osg::Vec2d &cursor,
            const osg::Vec3d &world, const osg::Vec3d &local,
            osgviz::Clickable* object, const int modKeyMask,
            WindowInterface* window = NULL);

        /**
        * Any Clickable object can be added which are called if the clicked() or dragged () functions are not overloaded
        * They are called on each clicked callback occurence on this object
        * @param cb
        */
        void addClickableCallback(std::shared_ptr<Clickable> cb);

        /**returns true if @p cb is already registered as clickable callback for 
        * this object. False otherwise.
        */
        bool hasClickableCallback(std::shared_ptr<Clickable> cb);

        int getClickableCallbackNumber() {
            return clickablecb.size();
        }

        void removeAllCallbacks() {
            clickablecb.clear();
        }

        inline void setScale(const float &x, const float &y, const float &z){
            PositionAttitudeTransform::setScale(osg::Vec3d(x,y,z));
        }

        inline void setScale(const osg::Vec3d &vec){
            PositionAttitudeTransform::setScale(vec);
        }


        inline void setPosition(const double &x, const double &y, const double &z){
            PositionAttitudeTransform::setPosition(osg::Vec3d(x,y,z));
        }

        inline void setPosition(const osg::Vec3d& vec){
            PositionAttitudeTransform::setPosition(vec);
        }

        inline void setOrientation(const double &x,const double &y,const double &z,const double &w){
            setAttitude( osg::Quat (x,y,z,w));
        }

        inline void setOrientation(const osg::Quat &quat){
            PositionAttitudeTransform::setAttitude( quat );
        }

        inline void setOrientation(const double &angle, const osg::Vec3d &vec){
            PositionAttitudeTransform::setAttitude( osg::Quat(angle, vec) );
        }

        inline void setByPositionAttitudeTransform(const osg::PositionAttitudeTransform &pa){
            this->setPosition(pa.getPosition());
            this->setAttitude(pa.getAttitude());
            this->setScale(pa.getScale());
            this->setPivotPoint(pa.getPivotPoint());
        }

        inline void rotate(const double &angle, const osg::Vec3d &vec){
            setAttitude( getAttitude() * osg::Quat(angle, vec) );
        }

        inline void rotate(const double &x,const double &y,const double &z,const double &w){
            setAttitude( getAttitude() * osg::Quat (x,y,z,w) );
        }

        virtual void setName(const std::string &name);

        void switchCullMask();
        void xorCullMask(unsigned int mask);

        void setLineWidth(const double& width);

        void displayName(const float &font_size = 0.1f, const osg::Vec3 &pos = osg::Vec3(0,0,0), const osg::Vec4 &color = osg::Vec4(0,0,0,1));

        void setTextSize(float font_size);
        void setTextPosition(osg::Vec3 pos);
        void setTextColor(osg::Vec4 color);

        void setDirty();

    protected:
        friend class ModelViewFactory;
        unsigned int cull_mask;
        bool visible;

    private:

        osg::ref_ptr< osg::AutoTransform > texttransform;
        osg::ref_ptr< osg::Geode > textgeode;
        osg::ref_ptr< osgText::Text > text;

        std::vector< std::shared_ptr<Clickable> > clickablecb;

    };

} /* namespace osgviz */

#endif /* SRC_PLUGINS_MODELVIEW_H_ */
