/*
* HierarchicalEventHandler.cpp
*
*  Created on: 21.12.2015
*      Author: planthaber
*/

#include "HierarchicalEventHandler.h"

#include <stdio.h>
#include "../../tools/TypeNameDemangling.h"


namespace osgviz {

    HierarchicalEventHandler::HierarchicalEventHandler() {
    }

    HierarchicalEventHandler::~HierarchicalEventHandler() {
    }

    int HierarchicalEventHandler::addEventHandler(osg::ref_ptr<osgGA::GUIEventHandler> handler, int priority) {
        osg::ref_ptr<osgGA::GUIEventHandler> existing_handler = handlers[priority];
        while (existing_handler.get()){
            --priority;
            existing_handler = handlers[priority];
        }
        handlers[priority] = handler;
        return priority;
    }

    void HierarchicalEventHandler::removeEventHandler(osg::ref_ptr<osgGA::GUIEventHandler> handlerToRemove) {

        for (std::map<int, osg::ref_ptr<osgGA::GUIEventHandler> >::iterator handler = handlers.begin(); handler != handlers.end();++handler){
            if (handler->second.get() == handlerToRemove.get()){
                handlers.erase(handler);
                break;
            }
        }
    }


    bool HierarchicalEventHandler::handle(const osgGA::GUIEventAdapter& ea,	osgGA::GUIActionAdapter& aa) {
        for (std::map<int, osg::ref_ptr<osgGA::GUIEventHandler> >::reverse_iterator handler = handlers.rbegin(); handler != handlers.rend();++handler){
            if (handler->second->handle(ea,aa)){
                return true;
            }
        }
        return false;
    }

} /* namespace osgviz */
