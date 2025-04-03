#include "LinesNode.h"
#include <osg/PrimitiveSet>
#include <osg/LineWidth>
using namespace osg;
namespace osgviz
{

    LinesNode::LinesNode(const Vec4& rgbaColor) : 
        colors(new Vec4Array), vertices(new Vec3Array), geom(new Geometry),
        geode(new Geode)
    {
        geom->setVertexArray(vertices);
        geom->setColorArray(colors);
        geom->setColorBinding(osg::Geometry::BIND_OVERALL);
        colors->push_back(rgbaColor);
        geode->addDrawable(geom);
        addChild(geode);
    }

    void LinesNode::addLine(const Vec3& start, const Vec3& end, const int &linewidht)
    {
        vertices->push_back(start);
        vertices->push_back(end);
        geom->addPrimitiveSet(new osg::DrawArrays(GL_LINES, vertices->size()-2, 2));
        osg::StateSet* state = geom->getOrCreateStateSet();
        osg::LineWidth* lw = new osg::LineWidth;
        lw->setWidth(linewidht);
        state->setAttribute(lw, osg::StateAttribute::ON);
        state->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    }

}