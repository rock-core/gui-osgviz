#ifndef _OSGVIZ_HPP_
#define _OSGVIZ_HPP_

#include <iostream>
#include <osg/Group>
#include <osgViewer/Viewer>
#include <osgDB/WriteFile>

#include "Module.h"
#include "windows/WindowManager.h"

#include "windows/config/WindowConfig.h"

#include <stdio.h>
//#include "tools/Timing.h"
#include "tools/UpdateThread.h"
#include "tools/TypeNameDemangling.h"


namespace osgDB{
    class ReaderWriter;
}


namespace osgviz
{


    /**
    * @class OsgViz
    * This is the main class to 3d Visualisaztions, it uses the lib_manager::LibManager to load plugins
    * this allows to load plugins as shared so all plugins can access each other on the same instances
    * In case this is not wanted, the plugin should implement a "Factory"
    */
    class OsgViz: public Updatable, public osg::Referenced
    {

    public: 

        /**
        * This instance Methods should be called on normal main programs
        * osgviz does not have any args, but they are forwarded to the plugins loaded by getPlugin()
        * @param argc
        * @param argv
        * @return the OsgViz instance
        */
        static osg::ref_ptr<OsgViz> getInstance(int argc = 0,char** argv = NULL);

        /**
        * This method is meant for plugins to obtain the instance which is already loaded.
        * If no instance exists, a new instance with default parameters is created
        * @return the OsgViz main
        */
        static osg::ref_ptr<OsgViz> getExistingInstance();

    private:
        /**
        * private constructor used by getInstance()
        * @param argc
        * @param argv
        */
        OsgViz(int argc, char** argv);

        /**
        * init function with shared code for both constructors
        * @param argc
        * @param argv
        */
        void init(int argc,char** argv);


    public:
        ~OsgViz();

        /**
        * starts a thread calling the update() function.
        * @note This will also initialize the xlib threading (XInitThreads()). This will break
        *       when used together with qt or any other xlib based gui framework.
        * @param microseconds number of microseconds to wait between the calls on update()
        */
        void startThread(int microseconds = 10000);
        void stopThread();

        /**
        * tries to lock osgviz, no matter if threaded or not
        * calls trylock on a OpenThreads::Mutex
        * @return 0 if normal, -1 if errno set, errno code otherwise.
        */
        int tryLockThread();

        /**
        * lock osgviz, no matter if threaded or not
        */
        void lockThread();

        /**
        * unlock osgviz, no matter if threaded or not
        */
        void unlockThread();


        /**
        * render a new frame() in each window created by this instance
        * automatically called if startThread() was called
        */
        virtual void update();

        virtual bool done() const;

        /**
        * creates a new window to display a scene
        *
        * the globel scene of osgviz is always displayed addChild()
        * you can also add scenes to a single window by using the returned id and
        * getWindowManager()->getWindowByID(int id)
        *
        * @param windowConfig window configuration, if a graphicsContext is given only ViewConfig settings are applied
        * @param graphicsContext set an external osg::GraphicsContext for example a osgQt::GraphicsWindowQt
        * @return window id, which can be used to obtain a osgviz::Window pointer getWindowManager()->getWindowByID(id);
        */
        unsigned int createWindow(WindowConfig windowConfig = WindowConfig(), osg::ref_ptr<osg::GraphicsContext> graphicsContext = NULL);

        /**
        * removed a window
        * @param id the id of the window to remove
        */
        void destroyWindow(unsigned int id);


        /**
        * adds a callback which is called on each update() before the scened are rendered
        * @param callback pointer to the  Updatable to be called.
        * @param priority higher numbers are called first, if the priority is taken, the new one is placed after the existing
        * @return the actual priority number the Updatable aquired
        */
        virtual int addUpdateCallback(Updatable* callback, int priority = 0);


        /**
        * loads an osgviz::Module class into the Inctance management of osgviz given a name
        * Applications using osgviz or another module can use the same instance of a Module identified by its name
        * (same name/same instance)
        * @ param moduleName The name of the instance to obtain (either existing or not)
        * @ param argc The argument count passed to the Module (only for non-exixting instances)
        * @ param argv The arguments passed to the Module (only for non-exixting instances)
        */
        template <class MOD> static std::shared_ptr<MOD> getModuleInstance(std::string moduleName, int argc = 0, char **argv = NULL){

            static_assert(!std::is_const<MOD>::value, "using const types is not supported");
            static_assert(!std::is_volatile<MOD>::value, "using volatile types is not supported");

            auto it =  modules.find(moduleName);
            if (it == modules.end() || it->second.get() == nullptr ){
                modules[moduleName].reset(new Module<MOD>(std::shared_ptr<MOD>(new MOD(argc,argv))));
                it = modules.find(moduleName);
            }

            std::unique_ptr< struct ModuleBase>& base = it->second;
            try {
                Module<MOD>& mod = dynamic_cast< Module<MOD>& > (*base);
                return mod.module;
            } catch (std::bad_cast const& e) {
                std::string message = " there is no module '" +
                    moduleName + "' of type '" +
#if(WIN32)
                    std::string(typeid(Module<MOD>).name()) + 
#else
                    std::string(abi::__cxa_demangle(typeid(Module<MOD>).name(),0,0,0)) + 
#endif
                    "' the module type is: " +
                    demangledTypeName(*base) + "'";

                throw std::runtime_error(message);
            }
        }



        /**
        * print available module names and classes
        */
        static void printModules();

        /**
        * return the osgviz globally shared scene
        * This node is added by default in each window created
        * If you want different contents, call osgviz::Window::getRootNode()
        * @return the global root node
        */
        inline osg::ref_ptr<osg::Group> getRootNode(){
            return root;
        }

        /**
        * add a child to the globally shared scene
        * these nodes are shared across all windows created
        * If you want different contents, call osgviz::Window::getRootNode()
        * @param node ht enode to add to the shared scene
        */
        inline void addChild(osg::Node * node){
            root->addChild(node);
        }

        /**
        * replace the shred scene
        * @param node the node to replaced all previous objects
        */
        inline void setScene(osg::Node * node){
            root->removeChildren(0,root->getNumChildren());
            root->addChild(node);
        }

        /**
        * get a child of the root node
        * @param index
        * @return
        */
        inline osg::Node* getChild(int index = 0){
            return root->getChild(index);
        }

        /**
        * save the (globally shared) scene displayed to a file
        * @param name
        */
        inline void write(const char* name){
            osgDB::writeNodeFile(*root, name);
        }

        /**
        * get a pointer to the windowManager which gived access to single windows and their scenes
        * @return the osgviz::WindowManager
        */
        inline WindowManager* getWindowManager(){
            return windowManager.get();
        }


    private:

        osg::ref_ptr<osg::Group> root;
        bool initialized;

        static std::map< std::string, std::unique_ptr< struct ModuleBase> > modules;

        int m_argc;
        char** m_argv;


        UpdateThread* thread;

        OpenThreads::Mutex* mutex;

        //graphics::GraphicsManager* graphicsManager;
        //osgViewer::Viewer viewer;
        //std::vector<osgViewer::Viewer *> viewers;

        osg::ref_ptr<WindowManager> windowManager;


        std::map<int, Updatable*> updateCallbacks;

    };




} // end namespace osgviz

#endif // _DUMMYPROJECT_DUMMY_HPP_
