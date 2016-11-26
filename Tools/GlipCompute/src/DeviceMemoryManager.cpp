/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-COMPUTE                                                                                              */
/*     Command-Line Utility for the OpenGL Image Processing LIBrary                                              */
/*                                                                                                               */
/*     Author        : R. Kerviche                                                                               */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : glip-lib.net                                                                              */
/*                                                                                                               */
/*     File          : DeviceMemoryManager.cpp                                                                   */
/*     Original Date : August 18th 2014                                                                          */
/*                                                                                                               */
/*     Description   : Device Memory Manager / Storage Manager for fast resource access.                         */
/*                                                                                                               */
/* ************************************************************************************************************* */

#include "DeviceMemoryManager.hpp"
#include "FreeImagePlusInterface.hpp"
#include "LibRawInterface.hpp"

// Tool :
	std::string getExtension(const std::string& filename)
	{
		const size_t dotPos = filename.find_last_of('.');
		std::string ext = (dotPos==std::string::npos) ? "" : filename.substr(dotPos + 1);
		std::transform(ext.begin(), ext.end(), ext.begin(), ::toupper);
		return ext;
	}

// DeviceMemoryManager :
	DeviceMemoryManager::DeviceMemoryManager(const size_t& _maxMemory)
	 : 	maxMemory(_maxMemory),
		currentMemory(0)
	{ }

	DeviceMemoryManager::~DeviceMemoryManager(void)
	{
		for(std::map<std::string, Glip::CoreGL::HdlTexture*>::iterator it=resources.begin(); it!=resources.end(); it++)
			delete it->second;

		resources.clear();
		history.clear();
	}

	void DeviceMemoryManager::remember(Glip::CoreGL::HdlTexture* ptr)
	{
		std::list<Glip::CoreGL::HdlTexture*>::iterator it = std::find(history.begin(), history.end(), ptr);

		if(it!=history.end())
			history.erase(it);

		history.push_back(ptr);
	}

	void DeviceMemoryManager::forget(size_t oblivionSize)
	{
		size_t stack = 0;

		// Look at old the old copy first : 
		for(std::list<Glip::CoreGL::HdlTexture*>::iterator it=history.begin(); it!=history.end(); )
		{
			stack += (*it)->getSize();

			// Forget it in the map : 
			for(std::map<std::string, Glip::CoreGL::HdlTexture*>::iterator mIt=resources.begin(); mIt!=resources.end(); mIt++)
			{
				if(mIt->second==*it)
					resources.erase(mIt);
			}

			// Delete it : 
			delete (*it);

			it = history.erase(it);

			// Is this sufficient?
			if(stack>oblivionSize)
				break;
		}
	}

	Glip::CoreGL::HdlTexture* DeviceMemoryManager::get(const std::string& filename)
	{
		// Find if the texture was already loaded : 
		std::map<std::string, Glip::CoreGL::HdlTexture*>::iterator it = resources.find(filename);

		if(it==resources.end())
		{
			// Copy not found, need to load it. First get the extension : 
			const std::string ext = getExtension(filename);
	
			Glip::CoreGL::HdlTexture* texture = NULL;
			Glip::Modules::ImageBuffer* imageBuffer = NULL;
			if(ext=="GRW")
				imageBuffer = Glip::Modules::ImageBuffer::load(filename);
			else if(ext=="CRW" || ext=="CR2" || ext=="NEF")
				texture = LibRawInterface::loadTexture(filename);
			else // Default, FreeImagePlus loading : 
				texture = FreeImagePlusInterface::loadTexture(filename);
			if(imageBuffer!=NULL)
			{
				texture = new Glip::CoreGL::HdlTexture(*imageBuffer);
				(*imageBuffer) >> (*texture);
				delete imageBuffer;
			}
			if((currentMemory + texture->getSize())>maxMemory && currentMemory>0)
				forget( maxMemory - (currentMemory + texture->getSize()) );
			resources[filename] = texture;
			remember(texture);
			return texture;
		}
		else
		{
			// Get the existing pre-loaded texture : 
			Glip::CoreGL::HdlTexture* texture = resources[filename];
			remember(texture);
			return texture;
		}
	}

	void DeviceMemoryManager::write(Glip::CoreGL::HdlTexture& texture, const std::string& filename)
	{
		const std::string ext = getExtension(filename);
		if(ext=="GRW")
		{
			Glip::Modules::ImageBuffer imageBuffer(texture);
			imageBuffer.write(filename);
		}	
		else
			FreeImagePlusInterface::saveTexture(texture, filename);
		
		// Update the cache if necessary :
		std::map<std::string, Glip::CoreGL::HdlTexture*>::iterator it = resources.find(filename);
		if(it!=resources.end())
		{
			// Check the memory :
			if((currentMemory - it->second->getSize() + texture.getSize())>maxMemory && currentMemory>0)
				forget(maxMemory - (currentMemory - it->second->getSize() + texture.getSize()));
			Glip::CoreGL::HdlTexture* textureCopy = new Glip::CoreGL::HdlTexture(texture.format());
			Glip::Modules::ImageBuffer imageBuffer(texture);
			imageBuffer >> (*textureCopy);
			resources[filename] = textureCopy;
			remember(textureCopy);
		}
	}

