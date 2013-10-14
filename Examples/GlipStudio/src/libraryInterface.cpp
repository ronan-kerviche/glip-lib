#include "libraryInterface.hpp"
#include <algorithm>

	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;

// LibraryInterface :
	LibraryInterface::LibraryInterface(ControlModule& _masterModule, QWidget *parent)
	 : Module(_masterModule, parent), layout(this), tabs(this), resourceTab(_masterModule, this), compilationTab(_masterModule, this), uniformsTab(_masterModule, this)
	{
		// Layout : 
		tabs.addTab(&resourceTab, 	"   Resources   ");
		tabs.addTab(&compilationTab,	"   Compilation   ");
		tabs.addTab(&uniformsTab, 	"   Uniforms   ");
		layout.addWidget(&tabs);
	}

	LibraryInterface::~LibraryInterface(void)
	{ }

	void LibraryInterface::pipelineCompilationFailed(Exception& e)
	{
		tabs.setCurrentIndex(tabs.indexOf(&compilationTab));
	}

	/*void LibraryInterface::compute(void)
	{
		if(mainPipeline!=NULL)
		{
			updateComputeStatus(false);

			// Check that all of the input have a connection : 
			for(int k=0; k<mainPipeline->getNumInputPort(); k++)
			{
				if(!resourceTab.isInputConnected(k))
				{
					emit requireRedraw();
					return ;
				}
			}

			// Apply : 
			for(int k=0; k<mainPipeline->getNumInputPort(); k++)
				(*mainPipeline) << resourceTab.input(k);

			(*mainPipeline) << Pipeline::Process;

			updateComputeStatus(true);

			// Need redraw : 
			emit requireRedraw();
		}
	}*/

