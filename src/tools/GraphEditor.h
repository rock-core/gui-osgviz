

#ifndef GRAPH_EDITOR_H_
#define GRAPH_EDITOR_H_

#include <osg/Node>


class GraphEditor {
 public:
    /**
     * @brief needs to be tmeplate
     * 
     * @tparam C 
     * @param oldnode 
     * @param newparent 
     */
    static void insertNode(osg::ref_ptr<osg::Node> oldnode, osg::ref_ptr<osg::Group> newparent) {
        // remove oldnode add newparent to all parents
        for (osg::Group *node : oldnode->getParents()) {
            node->removeChild(oldnode);
            node->addChild(newparent);
        }
        // add this after removal, removes itself otherwise
        newparent->addChild(oldnode);
    }
};

#endif
