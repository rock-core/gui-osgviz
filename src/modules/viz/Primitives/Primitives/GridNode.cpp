// have this on top so it is used for all included headers
#define _USE_MATH_DEFINES

#include "GridNode.hpp"

#include <sstream>

#include <osg/PositionAttitudeTransform>
#include <osg/Point>
#include <osgText/Text>

#include <math.h>

namespace osgviz
{
    ::osg::Node* GridNode::create(int rows,int cols,float dx, float dy, bool show_coordinates, const ::osg::Vec4 &color)
    {
        float size_x = cols*dx;
        float size_y = rows*dy;
        float size = std::min(size_x,size_y);
        float interval = std::min(dx,dy);

        ::osg::PositionAttitudeTransform *transform = new ::osg::PositionAttitudeTransform();
        ::osg::Geode *geode = new ::osg::Geode;
        transform->addChild(geode);

        ::osg::Geometry* geom = new ::osg::Geometry;
        ::osg::Vec3Array* v = new ::osg::Vec3Array;
        geom->setVertexArray(v);

        // draw grid lines
        for(float x = - size_x*0.5f; x <= size_x*0.5f; x += dx)
        {
            v->push_back( ::osg::Vec3(x, -size_y*0.5f, FLT_MIN ));
            v->push_back( ::osg::Vec3(x, size_y*0.5f, FLT_MIN ));
        }
        for(float y = - size_y*0.5f; y <= size_y*0.5f; y += dy)
        {
            v->push_back( ::osg::Vec3(-size_x*0.5f, y, FLT_MIN ));
            v->push_back( ::osg::Vec3(size_x*0.5f, y, FLT_MIN ));
        }

        // draw concentric circles
        for(float r=0; r<size*0.5f; r+=interval)
        {
            float xp = (2.0*M_PI)/(r*100);
            for(float x=0; x<2.0*M_PI; x+=2*xp)
            {
                v->push_back( ::osg::Vec3(cos(x)*r, sin(x)*r, FLT_MIN ) );
                v->push_back( ::osg::Vec3(cos(x+xp)*r, sin(x+xp)*r, FLT_MIN ) );
            }
        }

        // draw coordinates
        if(show_coordinates)
        {
            for(float x = - size_x*0.5f; x <= size_x*0.5f; x += dx)
            {
                for(float y = - size_y*0.5f; y <= size_y*0.5f; y += dy)
                {
                    osg::Geode *text_geode = new osg::Geode;
                    osgText::Text *text= new osgText::Text;
                    std::stringstream label;
                    label << "(" << x << "," << y << ")";
                    text->setText(label.str());
                    text->setCharacterSize(interval * 0.1);
                    text->setPosition(osg::Vec3d(x+0.02, y+0.05, FLT_MIN ));
                    text_geode->addDrawable(text);
                    transform->addChild(text_geode);
                }
            }
        }

        // set colors
        ::osg::Vec4Array* c = new ::osg::Vec4Array;
        geom->setColorArray(c);
        geom->setColorBinding( ::osg::Geometry::BIND_OVERALL );
        c->push_back(color);

        // Draw a four-vertex quad from the stored data.
        geom->addPrimitiveSet(new ::osg::DrawArrays(::osg::PrimitiveSet::LINES,0,v->size()));

        ::osg::StateSet* stategeode = geode->getOrCreateStateSet();
        stategeode->setMode( GL_LIGHTING, ::osg::StateAttribute::OFF );
        geode->addDrawable(geom);
        return (::osg::Node*)transform;
    }
}
