/*
* ModelLoader.cpp
*
*  Created on: 09.01.2015
*      Author: planthaber
*/

#include "ModelLoader.h"
#include <osgDB/ReadFile>
#include <osg/Node>
#include <osg/Image>




namespace osgviz {


    ModelLoader::ModelLoader(int argc, char** argv) {}

    ModelLoader::~ModelLoader() {
        // TODO Auto-generated destructor stub
    }

    osg::ref_ptr<Object> ModelLoader::loadModel(const std::string &path) {
        printf("load %s\n", path.c_str());
        osg::ref_ptr<osg::Node> object = osgDB::readNodeFile(path);
        osg::ref_ptr<Object> model = new Object();
        model->setName(path);
        model->addChild(object);
        return model;
    }

    osg::ref_ptr<osg::Geometry> ModelLoader::loadTexturedQuad(const std::string &imagefile, const float &sizex, const float &sizey) {
        osg::ref_ptr<Object> imageobject(new Object);
        osg::ref_ptr<osg::Texture2D> tex = new osg::Texture2D;
        osg::ref_ptr<osg::Image> image = osgDB::readImageFile(imagefile);
        tex->setImage(image);

        osg::ref_ptr<osg::Geometry> quad = osg::createTexturedQuadGeometry(osg::Vec3(0.0f, 0.0f, 0.0f),
                                                                           osg::Vec3(sizex, 0.0f, 0.0f),
                                                                           osg::Vec3(0.0f, 0.0f, sizey),
                                                                           0.0f, 0.0f, 1.0f, 1.0f);
        quad->getOrCreateStateSet()->setTextureAttributeAndModes(0, tex.get());
        quad->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);

        //imageobject->addChild();
        return quad;
    }

} /* namespace osgviz */
