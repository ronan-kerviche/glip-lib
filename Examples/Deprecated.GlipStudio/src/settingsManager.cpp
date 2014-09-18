#include "settingsManager.hpp"
#include <fstream>

// SettingsManager
	SettingsManager* SettingsManager::master 	= NULL;
	VanillaParser* SettingsManager::parser		= NULL;
	std::string SettingsManager::settingsFilename;
	bool SettingsManager::firstTimeRun		= false;

	SettingsManager::SettingsManager(const std::string& filename, bool reset)	
	{
		if(master!=NULL)
			throw Exception("SettingsManager::SettingsManager - The settings file was already opened (\"" + settingsFilename + "\").", __FILE__, __LINE__);
		
		// Safe to set data now :
		parser 		= NULL;
		firstTimeRun	= false;

		if(!reset)
		{
			try
			{
				std::ifstream file;
	
				file.open(filename.c_str());

				std::string code;

				if(file.is_open() && file.good() && !file.fail())
				{
					// Read : 
					std::string line;

					file.seekg(0, std::ios::beg);

					while(std::getline(file,line))
					{
						code += line;
						code += "\n";
					}

					file.close();
				}
				else
					firstTimeRun = true;

				// Parse : 
				parser = new VanillaParser(code);
			}
			catch(Exception& e)
			{
				delete parser;
				Exception m("SettingsManager::SettingsManager - Error caught while reading the settings file (\"" + filename + "\") : ", __FILE__, __LINE__);
				throw m+e;
			}
		}
		else
		{
			parser 		= new VanillaParser("");
			firstTimeRun 	= true;
		}	

		master 			= this;
		settingsFilename 	= filename;
	}

	SettingsManager::SettingsManager(void)
	{
		if(master==NULL)
			throw Exception("SettingsManager::SettingsManager - The settings file is not currently opened.", __FILE__, __LINE__);
	}

	SettingsManager::~SettingsManager(void)
	{
		if(master==this)
		{
			// Write to file : 
			try
			{
				std::fstream file;
	
				file.open(settingsFilename.c_str(), std::fstream::out | std::fstream::trunc);

				if(!file.is_open() || !file.good() || file.fail())
					throw Exception("Unable to open file : \"" + settingsFilename + "\".", __FILE__, __LINE__); 

				// Write all elements : 
				for(int k=0; k<parser->elements.size(); k++)
				{
					// Clean first : 
					parser->elements[k].body = parser->elements[k].getCleanBody();

					// Then, get code : 
					file << parser->elements[k].getCode() << std::endl << std::endl;
				}

				file.close();
			}
			catch(Exception& e)
			{
				Exception m("SettingsManager::SettingsManager - Error caught while writing the settings file (\"" + settingsFilename + "\") : ", __FILE__, __LINE__);
				throw m+e;
			}

			// clean : 
			master = NULL;
			delete parser;
			settingsFilename.clear();
		}
	}

	bool SettingsManager::isFirstTimeRun(void) const
	{
		return firstTimeRun;
	}

	void SettingsManager::checkOpenedSettings(void)
	{
		if(master==NULL || parser==NULL)
			throw Exception("SettingsManager - The settings file is not currently opened.", __FILE__, __LINE__);
	}

	std::vector<Element>::iterator SettingsManager::getModuleIterator(const std::string& moduleName, const std::string& propertyName)
	{
		std::vector<Element>::iterator it;

		for(it=parser->elements.begin(); it!=parser->elements.end(); it++)
		{
			if( (*it).strKeyword==moduleName && (*it).name==propertyName )
				break;
		}

		return it;
	}

	bool SettingsManager::moduleDataExists(const std::string& moduleName, const std::string& propertyName)
	{
		checkOpenedSettings();

		return getModuleIterator(moduleName, propertyName)!=parser->elements.end();
	}

	Element SettingsManager::getModuleData(const std::string& moduleName, const std::string& propertyName)
	{
		checkOpenedSettings();

		std::vector<Element>::iterator it = getModuleIterator(moduleName, propertyName);

		if(it!=parser->elements.end())
			return *it;
		else
		{
			Element e;
			e.strKeyword 	= moduleName;
			e.name 		= propertyName;
			return e;
		}
	}

	void SettingsManager::setModuleData(const std::string& moduleName, const std::string& propertyName, Element& data)
	{
		checkOpenedSettings();

		std::vector<Element>::iterator it = getModuleIterator(moduleName, propertyName);

		if(it!=parser->elements.end())
			(*it) = data;
		else
			parser->elements.push_back(data);
	}

	void SettingsManager::removeModuleData(const std::string& moduleName, const std::string& propertyName)
	{
		checkOpenedSettings();

		std::vector<Element>::iterator it = getModuleIterator(moduleName, propertyName);

		if(it!=parser->elements.end())
			parser->elements.erase(it);
	}

