#include "libraryInterface.hpp"
#include <algorithm>

	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;

// LibraryInterface :
	LibraryInterface::LibraryInterface(ControlModule& _masterModule, QWidget *parent)
	 : Module(_masterModule, parent), layout(this), tabs(this), resourceTab(_masterModule, this), compilationTab(_masterModule, this), ioTab(_masterModule, resourceTab.getResourcesManagerLink(), this), uniformsTab(_masterModule, this)
	{
		// Layout : 
		tabs.addTab(&resourceTab, 	"   Resources   ");
		tabs.addTab(&compilationTab,	"   Compilation   ");
		tabs.addTab(&ioTab,		"   I/O   ");
		tabs.addTab(&uniformsTab, 	"   Uniforms   ");
		layout.addWidget(&tabs);
	}

	LibraryInterface::~LibraryInterface(void)
	{ }

	void LibraryInterface::pipelineCompilationFailed(const Exception& e)
	{
		tabs.setCurrentIndex(tabs.indexOf(&compilationTab));
	}

