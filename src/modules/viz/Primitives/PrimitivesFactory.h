/*
* Primitives.h
*
*  Created on: 09.01.2015
*      Author: planthaber
*/

#ifndef GUI_OSGVIZ_SRC_PLUGINS_VIZ_PRIMITIVES_PRIMITIVESFACTORY_H_
#define GUI_OSGVIZ_SRC_PLUGINS_VIZ_PRIMITIVES_PRIMITIVESFACTORY_H_

#include <osg/Shape>
#include <osg/ShapeDrawable>
#include "../../../Object.h"
#include <vector>

namespace osgviz {

    class PrimitivesFactory {
    public:
        PrimitivesFactory(int argc = 0, char** argv = nullptr);
        virtual ~PrimitivesFactory();

        virtual int getLibVersion() const {return 0;};

        virtual const std::string getLibName() const { return "PrimitivesFactory"; };


        virtual osg::ref_ptr<Object> createAxes(float scale=1.0,bool blabels=true);
        virtual osg::ref_ptr<Object> createTextNode(const std::string& text, double fontSize, osg::Vec4 color = osg::Vec4(1, 1, 1, 1));
        virtual osg::ref_ptr<Object> createGrid(int rows = 20,int cols = 20,float dx =1, float dy=1, bool show_coordinates=false, const ::osg::Vec4 &color = ::osg::Vec4(120,120,120,255));
        virtual osg::ref_ptr<Object> createArrow(osg::Vec4 color = osg::Vec4(1, 1, 1, 1), bool invert = false);
        virtual osg::ref_ptr<Object> createCircularArrow(float radius = 1.0, float tube = 0.4, unsigned int radialSegments = 12, unsigned int tubularSegments = 32, float arc = 2.0*M_PI, float animationTimeSec = 2.0, const osg::Vec4& color = {1.0, 1.0, 0.0, 1.0});
        virtual osg::ref_ptr<Object> createWireframeBox(const double xSize, const double ySize, const double zSize, osg::Vec4 color = osg::Vec4(1, 1, 1, 1)) const;
        virtual osg::ref_ptr<Object> createWireframeBox(const double xSize, const double ySize, const double zSize,
            const double centerX, const double centerY, const double centerZ,
            osg::Vec4 color = osg::Vec4(1, 1, 1, 1)) const;
        virtual osg::ref_ptr<Object> createLinesNode(osg::Vec4 color = osg::Vec4(1, 1, 1, 1));
        virtual osg::ref_ptr<Object> createLinesNode(osg::Vec4 color, const std::vector<osg::Vec3>& points, const int &linewidht = 1);
        virtual osg::ref_ptr<Object> createRingNode(const float radius, const float height, const float thickness, osg::Vec4 color = osg::Vec4(1, 1, 1, 1));
        virtual osg::ref_ptr<Object> createSphereNode(double x, double y, double z, double radius, osg::Vec4 color = osg::Vec4(1, 1, 1, 1));
        virtual osg::ref_ptr<Object> createGraph2D(double x, double y, double z,osg::Quat orient,double width, double height);

        enum Shapes{BOX,CAPSULE,CONE,CYLINDER,SPHERE, WIREFRAME_BOX};

        struct Shape : public osgviz::Object{
            osg::ref_ptr<osg::Geode> geode;
            osg::ref_ptr<osg::Shape> shape;
            osg::ref_ptr<osg::ShapeDrawable> drawable;

            virtual void setColor(const float &r,const float &g,const float &b,const float &a = 1){
                drawable->setColor(osg::Vec4(r,g,b,a));

                if (a<1){
                    drawable->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN |
                        osg::StateAttribute::OVERRIDE |
                        osg::StateAttribute::PROTECTED);
                    this->getOrCreateStateSet()->setMode( GL_BLEND, osg::StateAttribute::ON );
                }

            }
        };

        /**
        * These Shapes cannot receive click events
        * @param shape
        * @param sizex aka radius for non-boxes
        * @param sizey only for boxes
        * @param sizez aka height for all except box and sphere
        * @return
        */
        virtual osg::ref_ptr<Shape> createShape(Shapes shape,const float &sizex,const float &sizey,const float &sizez);


        struct BoundingBox : public osgviz::Object{
            osg::ref_ptr<osg::Geometry> selectionBox;
            osg::ref_ptr<osg::Geode> selectionBoxGeode;
            osg::ref_ptr<osg::Vec3Array> points;
            osg::ref_ptr<osg::Vec4Array> colors;
            osg::ref_ptr<osg::Vec3Array> normals;
        };
        virtual osg::ref_ptr<BoundingBox> createBoundingBox(osg::Group* object);


        virtual osg::ref_ptr<Object> loadImage(std::string path, int sizex = 0, int sizey = 0);

    };

} /* namespace osgviz */

#endif /* GUI_OSGVIZ_SRC_PLUGINS_VIZ_PRIMITIVES_PRIMITIVESFACTORY_H_ */
