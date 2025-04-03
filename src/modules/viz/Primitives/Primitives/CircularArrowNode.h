/*
* CircularArrowNode.h
*
*  Created on: 13.01.2021
*      Author: mmaurus
*/

#ifndef GUI_OSGVIZ_OSGVIZ_SRC_MODULES_VIZ_PRIMITIVES_PRIMITIVES_CIRCULARARROWNODE_H_
#define GUI_OSGVIZ_OSGVIZ_SRC_MODULES_VIZ_PRIMITIVES_PRIMITIVES_CIRCULARARROWNODE_H_

#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <osgManipulator/Projector>

#include "../../../../Object.h"
//#include <osg/Cylinder>

namespace osgviz {

    class CircularArrowNode : public osgviz::Object{
    public:
        CircularArrowNode(float radius = 1.0, float tube = 0.4, unsigned int radialSegments = 12, unsigned int tubularSegments = 32, float arc = 2.0*M_PI, float animationTimeSec = 2.0);
        virtual ~CircularArrowNode();

        // void setRadius(float rad) {
        //     radius = rad;
        // }
        // void setTubeRadius(float rad) {
        //     tube = rad;
        // }
        // void setArc(float rad) {
        //     arc = rad;
        // }
        // void setRadialSegments(unsigned int num) {
        //     radialSegments = num;
        // }
        // void setTubularSegments(unsigned int num) {
        //     tubularSegments = num;
        // }
        // void setAnimationTimeSec(float sec) {
        //     animationTimeSec = sec;
        // }

        virtual void setColor(const osg::Vec4f& color){
            osgManipulator::setMaterialColor(color, *shaft_geode);
            osgManipulator::setMaterialColor(color, *head_geode);
        }

    private:

        osg::ref_ptr<osg::Geometry> createTubeBufferGeometry();
        void createCircularArrowInternal();

        osg::ref_ptr<osgManipulator::CylinderPlaneProjector> projector;

        float radius, tube;
        unsigned int radialSegments, tubularSegments;
        float arc;
        float animationTimeSec;

        osg::ref_ptr<osg::Geode> shaft_geode, head_geode;

    };

} /* namespace robot_manager */

#endif /* GUI_OSGVIZ_OSGVIZ_SRC_MODULES_VIZ_PRIMITIVES_PRIMITIVES_CIRCULARARROWNODE_H_ */
