#pragma once
#include <osg/Geode>
#include <osg/Geometry>
#include <osgViz/Object.h>

namespace osgviz 
{
    /** Displays a group of lines*/
    class LinesNode : public osgviz::Object
    {
    public:
        LinesNode(const osg::Vec4& rgbaColor);
        void addLine(const osg::Vec3& start, const osg::Vec3& end, const int &linewidht = 1);
    protected:
        osg::ref_ptr<osg::Vec4Array> colors;
        osg::ref_ptr<osg::Vec3Array> vertices;
        osg::ref_ptr<osg::Geometry> geom;
        osg::ref_ptr<osg::Geode> geode;
    };

}
