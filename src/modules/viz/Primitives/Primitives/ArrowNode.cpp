/*
* ArrowNode.cpp
*
*  Created on: 27.03.2015
*      Author: planthaber
*/

#include "ArrowNode.h"

namespace osgviz {

    ArrowNode::ArrowNode()	
        : ArrowNode(false) {
    }

    ArrowNode::ArrowNode(bool invert) : inverted(invert)
    {
        float coneheight = 0.3;
        float coneoffset = -0.25*coneheight; // center of cone is center of mass, not center of base! 0.25 is the baseOffsetFactor of an osg::Cone
        float coneposz =  -(coneheight/2.0)+coneoffset;
 
        float arrowheight = 1;
        float cylinderheight = arrowheight - coneheight;
        float cylinderposz = -0.5*cylinderheight;

        if (invert == true)
        {
            coneposz = cylinderheight - coneoffset;

            cylinderposz = 0.5*cylinderheight;
        }

        coneGeode = new osg::Geode();
        cylinderGeode = new osg::Geode();

        cone = new osg::Cone(osg::Vec3(0,0,coneposz),0.1,coneheight);

        cylinder = new osg::Cylinder(osg::Vec3(0,0,cylinderposz),0.05,cylinderheight);

        coneDrawable = new osg::ShapeDrawable(cone);
        cylinderDrawable = new osg::ShapeDrawable(cylinder);

        coneGeode->addDrawable(coneDrawable);
        cylinderGeode->addDrawable(cylinderDrawable);

        coneTransform = new osg::PositionAttitudeTransform;
        coneTransform->addChild(coneGeode);
        head_switch = new osg::Switch;
        head_switch->addChild(coneTransform, true);
        addChild(head_switch);

        cylinderTransform = new osg::PositionAttitudeTransform;
        cylinderTransform->addChild(cylinderGeode);
        addChild(cylinderTransform);
        coneposz_default = coneposz;
    }

    ArrowNode::~ArrowNode() {
        // TODO Auto-generated destructor stub
    }

} /* namespace robot_manager */
