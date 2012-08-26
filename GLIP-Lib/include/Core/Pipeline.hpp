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
			\brief Pipeline layout (Read Only)
			**/
			class __ReadOnly_PipelineLayout : virtual public __ReadOnly_ComponentLayout
			{
				public :
					// Constants
					enum ComponentKind
					{
						THIS_PIPELINE = -1,
						FILTER        =  0,
						PIPELINE      =  1
					};

					/**
					\struct Connection
					\brief Object describing connetion betwen elements in a pipeline
					**/
					struct Connection // Connection goes from idOut::portOut to idIn::portIn
					{
						/// The ID of the element receiving the conection
						int idIn;
						/// The ID of the port of the element receiving the conection
						int portIn;
						/// The ID of the element emitting the conection
						int idOut;
						/// The ID of the port of the element emitting the conection
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
			\brief Pipeline layout
			**/
			class PipelineLayout : virtual public ComponentLayout, virtual public __ReadOnly_PipelineLayout
			{
				public :
					// Tools
					PipelineLayout(const std::string& type);
					PipelineLayout(__ReadOnly_PipelineLayout& c);
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
			\brief Pipeline object
			**/
			class Pipeline : public __ReadOnly_PipelineLayout, public Component
			{
				public :
					enum ActionType
					{
						Process,
						Reset
					};

				private :
					// Data
					typedef std::vector<HdlTexture*> 	TablePtr;
					typedef std::vector<Filter*>     	TableFilter;
					typedef std::vector<Connection>   	TableConnection;
					typedef std::vector<int>		TableIndex;
					typedef std::vector<HdlFBO*>		TableBuffer;
					TablePtr       				input;
					TableIndex       			outputBuffer;
					TableIndex       			outputBufferPort;
					TableFilter    				filters;
					TableBuffer				buffers;
					TableIndex				useBuffer;
					TableIndex				actionFilter;
					std::vector<TableIndex*>		listOfArgBuffers;
					std::vector<TableIndex*>		listOfArgBuffersOutput;
					bool 					perfsMonitoring;
					GLuint					queryObject;
					std::vector<double>			perfs;
					double					totalPerf;

					// Tools
					void cleanInput(void);
					void build(void);

				protected :
					// Tools
					void process(void);

				public :
					// Tools
					Pipeline(__ReadOnly_PipelineLayout& p, const std::string& name);
					~Pipeline(void);

					int 		getNumActions(void);
					int 		getSize(bool askDriver = false);
					Pipeline& 	operator<<(HdlTexture&);
					Pipeline& 	operator<<(ActionType);
					HdlTexture& 	out(int id = 0);
					HdlTexture& 	out(const std::string&);
					int 		getFilterID(const std::string& path);
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
