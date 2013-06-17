/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : Pipeline.hpp                                                                              */
/*     Original Date : August 15th 2011                                                                          */
/*                                                                                                               */
/*     Description   : Pipeline object                                                                           */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    Pipeline.hpp
 * \brief   Pipeline object
 * \author  R. KERVICHE
 * \date    August 15th 2011
**/

#ifndef __GLIPLIB_PIPELINE__
#define __GLIPLIB_PIPELINE__

	// Includes
	#include <ctime>
	#include <algorithm>
	#include <map>
	#include <limits>
        #include "Component.hpp"
        #include "Filter.hpp"

	namespace Glip
	{
		namespace CoreGL
		{
			// Prototypes
			class HdlTexture;
			class __HdlTextureFormat_OnlyData;
			class HdlFBO;
			class ShaderSource;
			class HdlShader;
			class HdlProgram;
		}

		namespace CorePipeline
		{
			// Prototypes
			class PipelineLayout;
			class Pipeline;

			// Constants
			#define ELEMENT_NOT_ASSOCIATED (-2)

			// Objects
/**
\class __ReadOnly_PipelineLayout
\brief Pipeline layout (Read Only).
**/
			class __ReadOnly_PipelineLayout : virtual public __ReadOnly_ComponentLayout
			{
				public :
					///Flags describing the component kind.
					enum ComponentKind
					{
						///For this pipeline (internal use).
						THIS_PIPELINE = -1,
						///For filters.
						FILTER        =  0,
						///For pipelines.
						PIPELINE      =  1
					};

					/**
					\struct Connection
					\brief Object describing connetion betwen elements in a pipeline
					**/
					struct Connection // Connection goes from idOut::portOut to idIn::portIn
					{
						/// The ID of the element receiving the connection
						int idIn;
						/// The ID of the port of the element receiving the connection
						int portIn;
						/// The ID of the element emitting the connection
						int idOut;
						/// The ID of the port of the element emitting the connection
						int portOut;
					};

				private :
					// Data
					typedef std::vector<__ReadOnly_ComponentLayout*> ComponentList;
					std::vector<Connection>    connections;
					ComponentList              elementsLayout;
					std::vector<ComponentKind> elementsKind;
					std::vector<int>           elementsID;

					// Friends
					friend class PipelineLayout;

				protected :
					// Tools
					__ReadOnly_PipelineLayout(const std::string& type);
					int                    	getElementID(int i);
					void                   	setElementID(int i, int ID);
					std::vector<Connection> getConnectionDestinations(int id, int p);
					Connection              getConnectionSource(int id, int p);

					// Friends
					friend class Pipeline;

				public :
					// Tools
					__ReadOnly_PipelineLayout(const __ReadOnly_PipelineLayout& c);

					void 				checkElement(int i) const;
					int  				getNumElements(void) const;
					int  				getNumConnections(void) const;
					void				getInfoElements(int& numFilters, int& numPipelines);
					int  				getElementIndex(const std::string& name) const;
					bool				doesElementExist(const std::string& name) const;
					ComponentKind 			getElementKind(int i) const;
					static ObjectName&      	componentName(int i, const void* obj);
					__ReadOnly_ComponentLayout& 	componentLayout(int i) const;
					__ReadOnly_ComponentLayout& 	componentLayout(const std::string& name) const;

					__ReadOnly_FilterLayout&   	filterLayout(int i) const;
					__ReadOnly_FilterLayout&   	filterLayout(const std::string& name) const;
					__ReadOnly_PipelineLayout& 	pipelineLayout(int i) const;
					__ReadOnly_PipelineLayout& 	pipelineLayout(const std::string& name) const;

					Connection              	getConnection(int i) const;
					std::string 			getConnectionDestinationsName(int filterSource, int port);
					std::string 			getConnectionDestinationsName(const std::string& filterSource, const std::string& port);
					std::string 			getConnectionSourceName(int filterDestination, int port);
					std::string 			getConnectionSourceName(const std::string& filterDestination, const std::string& port);
					bool 				check(bool exception = true);
			};

			typedef __ReadOnly_PipelineLayout::ComponentKind ComponentKind;

/**
\class PipelineLayout
\brief Pipeline layout.

How to create a pipeline layout :
\code
	// Create the layout with a name (the "type" of the layout) :
	PipelineLayout sobelLayout("SobelFilter");

	// Add one input and one output with their name :
	sobelLayout.addInput("intputImage");
	sobelLayout.addInput("imageDerivatives");

	// Add an instance of a filter previously created (see FilterLayout for more information) :
	sobelLayout.add(sobelFilter, "instSobelFilter");
	// It also work with another PipelineLayout object, adding a sub pipeline.

	// If the port names of the FilterLayout don't correspond to those of the PipelineLayout, you have to explicitly declare the connections :

	// Connection going from input "inputImage" of this pipeline to the input port "inputTexture" of the filter "sobelFilter" :
	sobelLayout.connectToInput("inputImage", "sobelFilter", "inputTexture");

	// In the case of multiple elements (works for filter to sub-pipeline, filter to filter, sub-pipeline to filter
	// or sub-pipeline to sub-pipeline connections).
	// you will use the following syntax for a connection from output Element1::outputE1 to input Element2::inputE2 :
	// someLayout.connect("Element1","outputE1","Element2","inputE2");

	// Connection going from input "inputImage" of this pipeline to the input port "inputTexture" of the filter "sobelFilter" :
	sobelLayout.connectToOutput("sobelFilter", "outputTexture", "imageDerivatives");

	// In the case where you are using explicit names (same names indicate a connection), then you can use the automatic method :
	// sobelLayout.autoConnect();
	// Which is equivalent to :
	// sobelLayout.connectToInput("inputImage", "sobelFilter", "inputImage");
	// sobelLayout.connectToOutput("sobelFilter", "imageDerivatives", "imageDerivatives");

	// If you save the id of the elements, you can use the ID connection method but it usually is harder to read in code afterward.
\endcode
**/
			class PipelineLayout : virtual public ComponentLayout, virtual public __ReadOnly_PipelineLayout
			{
				public :
					// Tools
					PipelineLayout(const std::string& type);
					PipelineLayout(const __ReadOnly_PipelineLayout& c);
					int	add(const __ReadOnly_FilterLayout& filterLayout,     const std::string& name);
					int  	add(const __ReadOnly_PipelineLayout& pipelineLayout, const std::string& name);
					int  	addInput(const std::string& name);
					int  	addOutput(const std::string& name);

					void 	connect(int filterOut, int portOut, int filterIn, int portIn);
					void 	connect(const std::string& filterOut, const std::string& portOut, const std::string& filterIn, const std::string& portIn);
					void 	connectToInput(int port, int filterIn, int portIn);
					void	connectToInput(const std::string& port, const std::string& filterIn, const std::string& portIn);
					void 	connectToOutput(int filterOut, int portOut, int port);
					void 	connectToOutput(const std::string& filterOut, const std::string& portOut, const std::string& port);
					void 	autoConnect(void);
			};

/**
\class Pipeline
\brief Pipeline object.

How to process the inputs :
\code
	HdlPipeline myPipeline(myPipelineLayout, "InstanceName");
	HdlTexture input1(...), input2(...), ...;

	myPipeline << input1 << input2 << ... << Pipeline::Process;

	outputDevice << myPipeline.out(0);
\endcode
**/
			class Pipeline : public __ReadOnly_PipelineLayout, public Component
			{
				public :
					///Actions enumeration.
					enum ActionType
					{
						///To start process.
						Process,
						///To reset argument chain.
						Reset
					};

				private :
					struct ActionHub
					{
						std::vector<int> 	inputBufferIdx;		// The index of the buffer providing the argument k.
						std::vector<int> 	inputArgumentIdx;	// The index of the output providing the argument k.
						int			bufferIdx;		// The index of the buffer to use for output.
						int			filterIdx;		// The index of the filter.
					};

					struct OutputHub
					{
						int 			bufferIdx;		// The index of the targeted buffer holding the input.
						int			outputIdx;		// The index of the output for this buffer which has to be used.
					};

					// Data
					std::vector<HdlTexture*> 		inputsList;
					std::vector<HdlFBO*>			buffersList;				
					std::vector<Filter*>			filtersList;					
					std::vector<ActionHub>			actionsList;
					std::vector<OutputHub>			outputsList;
					std::map<int, int>			filtersGlobalIDsList;

					bool 					perfsMonitoring;
					GLuint					queryObject;
					std::vector<double>			perfs;
					double					totalPerf;

					// Tools
					Pipeline(const __ReadOnly_PipelineLayout& p);
					void cleanInput(void);
					void build(int& currentIdx, std::vector<Filter*>& filters, std::map<int, int>& filtersGlobalID, std::vector<Connection>& connections, __ReadOnly_PipelineLayout& originalLayout);
					void allocateBuffers(std::vector<Connection>& connections);
					//void build(void);

				protected :
					// Tools
					void process(void);

				public :
					// Tools
					Pipeline(const __ReadOnly_PipelineLayout& p, const std::string& name);
					~Pipeline(void);

					int 		getNumActions(void);
					int 		getSize(bool askDriver = false);
					Pipeline& 	operator<<(HdlTexture& texture);
					Pipeline& 	operator<<(Pipeline& pipeline);
					Pipeline& 	operator<<(ActionType a);
					HdlTexture& 	out(int id = 0);
					HdlTexture& 	out(const std::string& portName);
					int 		getFilterID(const std::string& path);
					Filter& 	operator[](int filterID);
					Filter& 	operator[](const std::string& path);
					void 		enablePerfsMonitoring(void);
					void 		disablePerfsMonitoring(void);
					double		getTiming(const std::string& path);
					double 		getTiming(int action, std::string& filterName);
					double 		getTotalTiming(void);
			};
		}
	}


#endif
