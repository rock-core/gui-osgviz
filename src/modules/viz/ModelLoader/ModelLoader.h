/*
* ModelLoader.h
*
*  Created on: 09.01.2015
*      Author: planthaber
*/

#ifndef GUI_OSGVIZ_SRC_PLUGINS_DATA_MODELLOADER_H_
#define GUI_OSGVIZ_SRC_PLUGINS_DATA_MODELLOADER_H_

#include "../../../Object.h"
#include <string>

namespace osgviz {

class ModelLoader {
 public:
    explicit ModelLoader(int argc = 0, char** argv = nullptr);
    virtual ~ModelLoader();

    virtual osg::ref_ptr<Object> loadModel(const std::string &path);

    virtual osg::ref_ptr<osg::Geometry> loadTexturedQuad(const std::string &imagefile, const float &sizex, const float &sizey);
 private:
};

} /* namespace osgviz */

#endif /* GUI_OSGVIZ_SRC_PLUGINS_DATA_MODELLOADER_H_ */
