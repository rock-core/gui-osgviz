/*
* CircularArrowNode.cpp
*
*  Created on: 13.01.2021
*      Author: mmaurus
*/

#include "CircularArrowNode.h"
#include <osgUtil/SmoothingVisitor>

#include <iostream>
#include <assert.h>

namespace osgviz {

    CircularArrowNode::CircularArrowNode(float radius, float tube, unsigned int radialSegments, unsigned int tubularSegments, float arc, float animationTimeSec) :
        radius(radius),
        tube(tube),
        radialSegments(radialSegments),
        tubularSegments(tubularSegments),
        arc(arc),
        animationTimeSec(animationTimeSec)
    {
        projector = new osgManipulator::CylinderPlaneProjector();
        createCircularArrowInternal();
        setColor(osg::Vec4f(1,1,0,1));

        assert(arc != 0);
    }

    CircularArrowNode::~CircularArrowNode() {
        // TODO Auto-generated destructor stub
    }

    osg::ref_ptr<osg::Geometry> CircularArrowNode::createTubeBufferGeometry() {
        osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
        osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
        osg::ref_ptr<osg::Vec2Array> texcoords = new osg::Vec2Array;

        // generate vertices, normals and texcoords
        osg::Vec3f center, vertex, normal;
        osg::Vec2f uv;

        for (unsigned int j=0; j<=radialSegments; ++j) {
            for (unsigned int i=0; i<=tubularSegments; ++i) {

                float u = float(i) / tubularSegments * arc;
                float v = float(j) / radialSegments * M_PI * 2.0;

                // vertex
                vertex.x() = (radius + tube * cos(v)) * cos(u);
                vertex.y() = (radius + tube * cos(v)) * sin(u);
                vertex.z() = tube * sin(v);
                vertices->push_back(vertex);

                //std::cout << vertex.x() << ", " << vertex.y() << ", " << vertex.z() << std::endl;

                // normal
                center.x() = radius * cos(u);
                center.y() = radius * sin(u);
                normal = vertex - center;
                normal.normalize();
                normals->push_back(normal);

                // texcoords
                uv.x() = float(i) / tubularSegments;
                uv.y() = float(j) / radialSegments;
                texcoords->push_back(uv);
            }
        }

        // generate indices
        osg::ref_ptr<osg::DrawElementsUInt> indices = new osg::DrawElementsUInt(GL_TRIANGLES);
        for (unsigned int j = 1; j <= radialSegments; ++j) {
            for (unsigned int i = 1; i <= tubularSegments; ++i ) {

                // indices
                unsigned int a = ( tubularSegments + 1 ) * j + i - 1;
                unsigned int b = ( tubularSegments + 1 ) * ( j - 1 ) + i - 1;
                unsigned int c = ( tubularSegments + 1 ) * ( j - 1 ) + i;
                unsigned int d = ( tubularSegments + 1 ) * j + i;

                // faces
                indices->push_back(a);
                indices->push_back(b);
                indices->push_back(d);

                indices->push_back(b);
                indices->push_back(c);
                indices->push_back(d);
            }
        }

        // create geometry
        osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
        geom->setVertexArray(vertices.get());
        geom->setNormalArray(normals.get());
        geom->setTexCoordArray(0, texcoords.get());
        geom->addPrimitiveSet(indices.get());
        osgUtil::SmoothingVisitor::smooth(*geom);
        
        return geom;
    }

    void CircularArrowNode::createCircularArrowInternal() {
        removeChildren(0, getNumChildren());

        osg::ref_ptr<osg::PositionAttitudeTransform> group = new osg::PositionAttitudeTransform;

        // shaft of arrow
        shaft_geode = new osg::Geode();
        osg::ref_ptr<osg::Geometry> shaft_geom = createTubeBufferGeometry();
        shaft_geode->addDrawable(shaft_geom);
        group->addChild(shaft_geode);
        
        // head of the arrow
        float cone_height = 4*tube;
        float cone_radius = 2*tube;
        osg::Vec3f cone_center = osg::Vec3f(0.25*cone_height, -0.12+tube, 0.0);
        cone_center.x() = radius * cos(arc) ;
        cone_center.y() = radius * sin(arc);
        cone_center.z() = 0;

        osg::Quat rot = osg::Quat(-M_PI_2, osg::X_AXIS) * osg::Quat(arc, osg::Z_AXIS);
        if (arc < 0.0) {
            rot *= osg::Quat(M_PI, osg::Z_AXIS);
        }
        osg::Vec3f offset = rot * osg::Vec3f(0, 0, 0.25*cone_height);

        cone_center += offset;

        osg::ref_ptr<osg::Cone> head = new osg::Cone(cone_center, cone_radius, cone_height);
        head->setRotation(rot);
        osg::ref_ptr<osg::ShapeDrawable> shc = new osg::ShapeDrawable(head);
        head_geode = new osg::Geode();
        head_geode->addDrawable(shc);
        group->addChild(head_geode);

        // create animation
        if (animationTimeSec > 0.0) {
            osg::ref_ptr<osg::AnimationPath> path = new osg::AnimationPath;
            path->setLoopMode(osg::AnimationPath::LoopMode::LOOP);
            unsigned int numSamples = 32;
            float delta_rot = 2.0 * osg::PI / ((float)numSamples - 1.0);
            float delta_time = animationTimeSec / (float)numSamples;
            osg::Vec3f pos(0,0,0);

            osg::Vec3f axis = osg::Z_AXIS * std::copysign(1.0, arc);
            for (unsigned int i=0; i<numSamples; ++i) {
                osg::Quat rot(delta_rot * (float) i, axis);
                path->insert(delta_time * (float) i, osg::AnimationPath::ControlPoint(pos, rot));
            }

            osg::ref_ptr<osg::AnimationPathCallback> apcb = new osg::AnimationPathCallback;
            apcb->setAnimationPath(path.get());
            group->setUpdateCallback(apcb.get());
        }

        addChild(group.release());
    }
} /* namespace robot_manager */
