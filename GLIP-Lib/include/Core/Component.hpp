/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : Component.hpp                                                                             */
/*     Original Date : August 15th 2011                                                                          */
/*                                                                                                               */
/*     Description   : Components of pipelines                                                                   */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    Component.hpp
 * \brief   Components of pipelines.
 * \author  R. KERVICHE
 * \date    August 15th 2011
**/

#ifndef __GLIP_COMPONENT_HPP__
#define __GLIP_COMPONENT_HPP__

	// Includes
	#include <string>
        #include <vector>
	#include "Core/LibTools.hpp"

	namespace Glip
	{
		namespace CoreGL
		{
			class HdlTexture;
		}

		using namespace CoreGL;

		namespace CorePipeline
		{
			// Prototypes
			class ComponentLayout;

			// Objects
			/**
			\class AbstractComponentLayout
			\brief Layout of a component template (Read Only)
			**/
			class GLIP_API AbstractComponentLayout
			{
				private :
					// Data
					std::string typeName;
					std::vector<std::string> inputPorts;
					std::vector<std::string> outputPorts;

					friend class ComponentLayout;

				protected :
					// Tools
					AbstractComponentLayout(const std::string& _typeName);
					AbstractComponentLayout(const std::string& _typeName, const std::vector<std::string>& _inputPorts, const std::vector<std::string>& _outputPorts);

				public :
					// Tools
					AbstractComponentLayout(const AbstractComponentLayout&);
					virtual ~AbstractComponentLayout(void);

					void checkInputPort(int i) const;
					void checkOutputPort(int i) const;
					virtual std::string	getFullName(void) const;
					const std::string&	getTypeName(void) const;
					int                	getNumInputPort(void) const;
					const std::string& 	getInputPortName(int i) const;
					int                	getInputPortID(const std::string& name) const;
					bool			doesInputPortExist(const std::string& name) const;
					int                	getNumOutputPort(void) const;
					const std::string& 	getOutputPortName(int i) const;
					int                	getOutputPortID(const std::string& name) const;
					bool 			doesOutputPortExist(const std::string& name) const;
			};

			/**
			\class ComponentLayout
			\brief Layout of a component template
			**/
			class GLIP_API ComponentLayout : virtual public AbstractComponentLayout
			{
				protected :
					// Tools
					int addOutputPort(const std::string& name);
					int addInputPort(const std::string& name);

				public :
					// Tools
					ComponentLayout(const std::string& _typeName);
					ComponentLayout(const AbstractComponentLayout&);
					virtual ~ComponentLayout(void);

					void setInputPortName(int i, const std::string& newName);
					void setOutputPortName(int i, const std::string& newName);
			};

			/**
			\class Component
			\brief Element of a pipeline
			**/
			class GLIP_API Component : virtual public AbstractComponentLayout
			{
				private :
					std::string instanceName;

				protected :
					// Tools
					Component(const std::string& _typeName, const std::vector<std::string>& _inputPorts, const std::vector<std::string>& _outputPorts, const std::string& _instanceName);
					Component(const AbstractComponentLayout&, const std::string& _instanceName);

				public :
					virtual ~Component(void);

					std::string getFullName(void) const;
					const std::string& getName(void) const;
			};
		}
	}

#endif
