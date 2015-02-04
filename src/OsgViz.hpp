#ifndef _OSGVIZ_HPP_
#define _OSGVIZ_HPP_

#include <iostream>
#include <lib_manager/LibManager.hpp>
#include <osg/Group>
#include <boost/thread/mutex.hpp>


#include "plugins/OsgVizPlugin.h"
#include "graphics/GraphicsManager.h"

#include <stdio.h>

namespace osgviz
{

class OsgViz;

class FrameUpdateThread : public OpenThreads::Thread
	{
	    public:

	FrameUpdateThread(mars::graphics::GraphicsManager* osgviz):
	        	osgviz(osgviz){
	        	running = false;
	        }

	        ~FrameUpdateThread()
	        {
	            if (isRunning())
	            {
	                cancel();
	                join();
	            }
	        }

	        int cancel()
	        {
	        	mutex.lock();
	        	running = false;
	        	mutex.unlock();
	            return 0;
	        }

	        void run()
	        {
	        	mutex.lock();
	        	running = true;
	        	mutex.unlock();

	        	while (running){
	        		mutex.unlock();
	        		usleep(10000);
					mutex.lock();
					//int result = _viewerBase->run();
					osgviz->draw();
					mutex.unlock();
					//give others a chance to lock
					usleep(10000);
					mutex.lock();
	        	}
	        }

	        void lock(){
	        	mutex.lock();
	        }
	        void unlock(){
	        	mutex.unlock();
	        }

	    private:
	        bool running;
	        mars::graphics::GraphicsManager* osgviz;
	        OpenThreads::Mutex mutex;

	};


	class OsgViz: public mars::graphics::GraphicsManager
	{

		public: 

	    CREATE_MODULE_INFO();

		static OsgViz* getInstance(int argc = 0,char** argv = NULL);

		OsgViz(lib_manager::LibManager * manager);

		OsgViz(int argc, char** argv);

		~OsgViz();


		virtual const std::string getLibName() const;

	    virtual int getLibVersion() const;



	    void init(int argc,char** argv);

	    /**
	     * starts a thread calling updateContent
	     */
	    void startThread();
	    void stopThread();

	    void lockThread();
	    void unlockThread();


		void updateContent();

		int createWindow(bool threaded = true);
		void destroyWindow(int id);

		template <class VIZPLUGIN> VIZPLUGIN* getVisualizerPlugin(std::string classname){
			VIZPLUGIN* viz = (VIZPLUGIN*)getVizPlugin(classname,classname);
			viz->setRootNode(root);
			return viz;
		}

		template <class VIZPLUGIN> VIZPLUGIN* getDataPlugin(std::string classname){
			VIZPLUGIN* data = (VIZPLUGIN*)getVizPlugin(classname,classname);
			data->init(m_argc,m_argv);
			return data;
		}




		private:

		OsgVizPlugin* getVizPlugin(std::string path, std::string name);

		bool createdOwnManager;
		lib_manager::LibManager *libmanager;

		osg::ref_ptr<osg::Group> root;

		osgViewer::Viewer viewer;

		private:
		bool initialized;
		std::vector< OsgVizPlugin* >loadedPlugins;

		int m_argc;
		char** m_argv;


		FrameUpdateThread* thread;

		//std::vector<osgViewer::Viewer *> viewers;

	};

} // end namespace osgviz

#endif // _DUMMYPROJECT_DUMMY_HPP_
