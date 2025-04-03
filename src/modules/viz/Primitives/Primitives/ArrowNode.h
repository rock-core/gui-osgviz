/*
* ArrowNode.h
*
*  Created on: 27.03.2015
*      Author: planthaber
*/

#ifndef GUI_OSGVIZ_OSGVIZ_SRC_MODULES_VIZ_PRIMITIVES_PRIMITIVES_ARROWNODE_H_
#define GUI_OSGVIZ_OSGVIZ_SRC_MODULES_VIZ_PRIMITIVES_PRIMITIVES_ARROWNODE_H_

#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <osg/Switch>

#include "../../../../Object.h"
#include <iostream>
//#include <osg/Cylinder>

namespace osgviz {

    class ArrowNode : public osgviz::Object{
    public:
        ArrowNode();
        ArrowNode(bool invert);
        virtual ~ArrowNode();

        osg::ref_ptr<osg::Cone> Cone(){
            return cone;
        }

        osg::ref_ptr<osg::Cylinder> Cylinder(){
            return cylinder;
        }

        /**
         * @brief Set the Length of the arrow.
         * Works like scaling but with fixed arrow head length
         * 
         * @param length 
         */
        inline void setLength(float length) {

            // TODO: what if length < coneheight?


            //coneTransform->setScale(scale);
            osg::Vec3d offset(0,0,0);

            float coneheight = 0.3;
            if (length <= 2*coneheight) {
                head_switch->setChildValue(coneTransform, false);
                coneheight = 0.0;
            } else {
                head_switch->setChildValue(coneTransform, true);
            }
            float coneoffset = -0.25*coneheight; // center of cone is center of mass, not center of base! 0.25 is the baseOffsetFactor of an osg::Cone
            float coneposz =  -(coneheight/2.0)+coneoffset;
    
            float arrowheight = length;
            float cylinderheight = arrowheight - coneheight;

            if (inverted == true) {
                coneposz = cylinderheight - coneoffset;
            }

            offset.z() = coneposz - coneposz_default;
            coneTransform->setPosition(offset);

            osg::Vec3 S = {1,1,1};
            S.z() = ((length - coneheight) / 0.7);
            cylinderTransform->setScale(S);

        }

        virtual void setColor(const float &r,const float &g,const float &b,const float &a = 1){
            coneDrawable->setColor(osg::Vec4(r,g,b,a));
            cylinderDrawable->setColor(osg::Vec4(r,g,b,a));

            if (a<1) {
                this->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN |
                    osg::StateAttribute::OVERRIDE |
                    osg::StateAttribute::PROTECTED);
                this->getOrCreateStateSet()->setMode( GL_BLEND, osg::StateAttribute::ON );
            }
        }

    private:
        osg::ref_ptr<osg::Geode> coneGeode, cylinderGeode;

        osg::ref_ptr<osg::Cone> cone;
        osg::ref_ptr<osg::ShapeDrawable> coneDrawable;
        osg::ref_ptr<osg::PositionAttitudeTransform> coneTransform;

        osg::ref_ptr<osg::Cylinder> cylinder;
        osg::ref_ptr<osg::ShapeDrawable> cylinderDrawable;
        osg::ref_ptr<osg::PositionAttitudeTransform> cylinderTransform;

        osg::ref_ptr<osg::Switch> head_switch;

        bool inverted;
        float coneposz_default;
    };

} /* namespace robot_manager */

#endif /* GUI_OSGVIZ_OSGVIZ_SRC_MODULES_VIZ_PRIMITIVES_PRIMITIVES_ARROWNODE_H_ */
