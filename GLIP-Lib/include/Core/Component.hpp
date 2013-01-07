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
        #include "NamingLayout.hpp"

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
			\class __ReadOnly_ComponentLayout
			\brief Layout of a component template (Read Only).
			**/
			class __ReadOnly_ComponentLayout : public ObjectName
			{
				private :
					// Data
					std::vector<ObjectName> inputPortDescription;
					std::vector<ObjectName> outputPortDescription;

					friend class ComponentLayout;
				protected :
					// Tools
					__ReadOnly_ComponentLayout(const std::string& type);

				public :
					// Tools
					__ReadOnly_ComponentLayout(const __ReadOnly_ComponentLayout&);
					virtual ~__ReadOnly_ComponentLayout(void);

					void checkInputPort(int i) const;
					void checkOutputPort(int i) const;
					int                	getNumInputPort(void) const;
					const std::string& 	getInputPortName(int i) const;
					std::string        	getInputPortNameExtended(int i) const;
					int                	getInputPortID(const std::string& name) const;
					bool			doesInputPortExist(const std::string& name) const;
					int                	getNumOutputPort(void) const;
					const std::string& 	getOutputPortName(int i) const;
					std::string        	getOutputPortNameExtended(int i) const;
					int                	getOutputPortID(const std::string& name) const;
					bool 			doesOutputPortExist(const std::string& name) const;
			};

			/**
			\class ComponentLayout
			\brief Layout of a component template.
			**/
			class ComponentLayout : virtual public __ReadOnly_ComponentLayout
			{
				protected :
					// Tools
					int addOutputPort(const std::string& name); // =""?
					int addInputPort(const std::string& name);  // =""?
				public :
					// Tools
					ComponentLayout(const std::string& type);
					ComponentLayout(const __ReadOnly_ComponentLayout&);
					virtual ~ComponentLayout(void);

					void setInputPortName(int id, const std::string& name);
					void setOutputPortName(int id, const std::string& name);
			};

			/**
			\class Component
			\brief Element of a pipeline.
			**/
			class Component : virtual public __ReadOnly_ComponentLayout
			{
				protected :
					// Tools
					Component(const __ReadOnly_ComponentLayout&, const std::string& name);
					virtual ~Component(void);
			};
		}
	}

#endif
