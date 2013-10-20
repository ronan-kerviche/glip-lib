#ifndef __GLIPSTUDIO_SETTINGSMANAGER__
#define __GLIPSTUDIO_SETTINGSMANAGER__

	#include "GLIPLib.hpp"

	using namespace Glip;
	using namespace Glip::Modules;
	using namespace Glip::Modules::VanillaParserSpace;

	class SettingsManager
	{
		private : 
			static SettingsManager* master;
			static VanillaParser* parser;
			static std::string settingsFilename;
			
			void checkOpenedSettings(void);
			std::vector<Element>::iterator getModuleIterator(const std::string& moduleName, const std::string& propertyName);

		public : 
			SettingsManager(const std::string& filename);
			SettingsManager(void);
			~SettingsManager(void);

			bool		moduleDataExists(const std::string& moduleName, const std::string& propertyName);
			Element 	getModuleData(const std::string& moduleName, const std::string& propertyName);
			void		setModuleData(const std::string& moduleName, const std::string& propertyName, Element& data);
	};

#endif

