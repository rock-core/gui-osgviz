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

ArrowNode::ArrowNode(bool invert) {
		
	float coneheight = 0.3;
	float coneoffset = -0.07; //whyever the center po is not really the center
	float coneposz =  -(coneheight/2.0)+coneoffset;

	float arrowheight = 1;
	float cylinderheight = arrowheight - coneheight;
	float cylinderposz = -arrowheight/2.0 - coneheight/2.0;
	
	if (invert == true)
	{
		coneposz = arrowheight - coneheight;

		cylinderposz = arrowheight/2.0 - coneheight/2.0;
	}

	geode = new osg::Geode();

	cone = new osg::Cone(osg::Vec3(0,0,coneposz),0.1,coneheight);

	cylinder = new osg::Cylinder(osg::Vec3(0,0,cylinderposz),0.05,cylinderheight);

	coneDrawable = new osg::ShapeDrawable(cone);
	cylinderDrawable = new osg::ShapeDrawable(cylinder);

	geode->addDrawable(coneDrawable);
	geode->addDrawable(cylinderDrawable);
	addChild(geode);		
}

ArrowNode::~ArrowNode() {
	// TODO Auto-generated destructor stub
}

} /* namespace robot_manager */
