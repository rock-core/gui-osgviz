/*
* Object.cpp
*
*  Created on: 08.01.2015
*      Author: planthaber
*/

#include "Object.h"

#include <stdio.h>
//#include <cxxabi.h>

//#include <unistd.h>

#include <osg/ValueObject>
#include <osg/LineWidth>
#include <algorithm>

namespace osgviz {

    Object::Object():cull_mask(0xffffffff),visible(true){

    }

    Object::~Object() {

    }

    void Object::displayName(const float &font_size, const osg::Vec3 &pos, const osg::Vec4 &color){

        if (!texttransform.valid()){
            texttransform = new osg::AutoTransform();
            textgeode = new osg::Geode();

            osg::StateSet* stateset = textgeode->getOrCreateStateSet();
            stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
            stateset->setMode(GL_BLEND,osg::StateAttribute::OFF);
            //stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
            stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);

            text = new osgText::Text();

            text->setCharacterSize(font_size);
            text->setAxisAlignment(osgText::Text::XY_PLANE);
            text->setAlignment(osgText::Text::LEFT_TOP);
            text->setPosition(pos);
            text->setColor(color);


            texttransform->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN);

            textgeode->addDrawable(text);
            texttransform->addChild(textgeode);
            this->addChild(texttransform);
        }

        text->setText(this->getName());



    }

    void Object::setTextSize(float font_size) {
        if (text != NULL)
            text->setCharacterSize(font_size);
    }

    void Object::setTextPosition(osg::Vec3 pos) {
        if (text != NULL)
            text->setPosition(pos);
    }

    void Object::setTextColor(osg::Vec4 color) {
        if (text != NULL)
            text->setColor(color);
    }

    void Object::xorCullMask(unsigned int mask) {
        cull_mask = cull_mask^mask;
        this->setNodeMask(cull_mask);
    }

    void Object::switchCullMask() {
        if(visible) {
            visible = false;
            this->setNodeMask(0);
        }
        else {
            visible = true;
            this->setNodeMask(cull_mask);
        }
    }

    void Object::setLineWidth(const double& width) {
        osg::ref_ptr<osg::LineWidth> linewidth = new osg::LineWidth();
        linewidth->setWidth(width);
        this->getOrCreateStateSet()->setAttributeAndModes(linewidth, osg::StateAttribute::ON);
    }

    bool Object::clicked(const int &buttonMask, const osg::Vec2d &cursor, const osg::Vec3d &world,
        const osg::Vec3d &local, Clickable *object, const int modKeyMask,
        WindowInterface* window){
            // printf("%s click: %.2f,%.2f,%.2f local: %.2f,%.2f,%.2f \n",this->getName().c_str(),world.x(),world.y(),world.z(),local.x(),local.y(),local.z());
            bool finish = false;
            if (!clickablecb.empty()){
                for(std::shared_ptr<Clickable>& clickable : clickablecb) {
                    if (clickable->clicked(buttonMask,cursor,world,local, this, modKeyMask, window)){
                        // printf("%s forwarded click\n",this->getName().c_str());
                        finish = true;
                    }
                }

            }
            return finish;
    }

    bool Object::dragged(const int &buttonMask, const osg::Vec2d &cursor,
        const osg::Vec3d &world, const osg::Vec3d &local,
        Clickable *object, const int modKeyMask, WindowInterface* window){
            bool finish = false;
            if (!clickablecb.empty()){
                for(std::shared_ptr<Clickable>& clickable : clickablecb) {
                    if (clickable->dragged(buttonMask,cursor,world,local, this, modKeyMask, window)){
                        finish = true;
                    }
                }

            }
            return finish;
    }

    void Object::addClickableCallback(std::shared_ptr<Clickable> cb) {
        this->clickablecb.push_back(cb);
    }

    void Object::setDirty(){
        this->setUserValue("dirty",true);
    }

    void Object::setName(const std::string &name){
        osg::PositionAttitudeTransform::setName(name);
        if (text.valid()){
            text->setText(name);
        }
    }

    bool Object::hasClickableCallback(std::shared_ptr<Clickable> cb)
    {
        return std::find(clickablecb.begin(), clickablecb.end(), cb) != clickablecb.end();
    }



} /* namespace osgviz */


