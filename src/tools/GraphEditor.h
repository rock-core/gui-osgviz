

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
    template <class C> static void insertNode(osg::Node* oldnode, osg::ref_ptr<C> newparent) {
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
