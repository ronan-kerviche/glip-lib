/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : GPLv3                                                                                     */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : StreamManager.cpp                                                                         */
/*     Original Date : October 17th 2010                                                                         */
/*                                                                                                               */
/*     Description   : StreamManager is the manager that interconnects streams                                   */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    StreamManager.cpp
 * \brief   StreamManager is the manager that interconnects streams
 * \author  R. KERVICHE
 * \version 0.6
 * \date    October 17th 2010
**/

/*#include "StreamManager.hpp"
#include <iostream>

using namespace Glip::CoreGL;
using namespace Glip::CorePipeline;

// Static data
	bool         StreamManager::built = false;
	HdlTexture*  StreamManager::texturelist[STRM_MNG_MAX_SOCKETS];
	SocketID     StreamManager::socketlist[STRM_MNG_MAX_SOCKETS];
	std::string* StreamManager::descriptions[STRM_MNG_MAX_SOCKETS];
	int          StreamManager::inst_count=0;

// Shield Macro :
	#define STREAMMANAGER_VERIFICATION if(!built) { std::cout << "StreamManager NOT built yet!" << std::endl; }
	#define THROW_ERROR(MESSAGE)       { std::cout << MESSAGE << std::endl; }

// Functions
	StreamManager::StreamManager(void)
	{
		if(!built)
		{
			for(int i=0; i<STRM_MNG_MAX_SOCKETS; i++)
			{
				texturelist[i]  = NULL;
				socketlist[i]   = 0;
				descriptions[i] = NULL;
			}
			built = true;
		}

		inst_count++;
	}

	StreamManager::~StreamManager(void)
	{
		inst_count--;

		if(inst_count==0) //last one
		{
			for(int i=0; i<STRM_MNG_MAX_SOCKETS; i++)
				if(descriptions[i]!=NULL) delete descriptions[i];
		}
	}

	// Socket declaring
	SocketID    StreamManager::queryTextureSocket(HdlTexture* ptr)
	{
		STREAMMANAGER_VERIFICATION
		for(int i=0; i<STRM_MNG_MAX_SOCKETS; i++)
		{
			if(texturelist[i]==NULL)
			{
				texturelist[i] = ptr;
				return INDEX_TO_TEXTURE(i);
			}
		}

		return NULL_SOCKET;
	}

	void        StreamManager::releaseTextureSocket(SocketID s)
	{
		texturelist[TEXTURE_TO_INDEX(s)] = NULL;
	}

	SocketID    StreamManager::querySocket(std::string info)
	{
		STREAMMANAGER_VERIFICATION
		for(int i=0; i<STRM_MNG_MAX_SOCKETS; i++)
		{
			if(socketlist[i]==NULL_SOCKET)
			{
				socketlist[i] = INDEX_TO_SOCKET(i);
				descriptions[i] = new std::string(info);
				return INDEX_TO_SOCKET(i);
			}
		}

		return NULL_SOCKET;
	}

	void        StreamManager::releaseSocket(SocketID s)
	{
		socketlist[SOCKET_TO_INDEX(s)] = NULL_SOCKET;
	}

	// Socket checking
	bool StreamManager::socketExists(SocketID s)
	{
		STREAMMANAGER_VERIFICATION

		if(SOCKET_TO_INDEX(s)>=0) return socketlist[SOCKET_TO_INDEX(s)]!=NULL_SOCKET;
		else                      return false;
	}

	bool StreamManager::textureExists(SocketID s)
	{
		STREAMMANAGER_VERIFICATION
		if(TEXTURE_TO_INDEX(s)>=0) return texturelist[TEXTURE_TO_INDEX(s)]!=NULL_SOCKET;
		else                       return false;
	}

	bool StreamManager::socketIsLinked(SocketID s, int depth)
	{
		STREAMMANAGER_VERIFICATION
		if(SOCKET_TO_INDEX(s)>=0)
		{
			int dp = 0;
			SocketID cur = socketlist[SOCKET_TO_INDEX(s)];

			while( socketlist[SOCKET_TO_INDEX(cur)]!=cur && socketlist[SOCKET_TO_INDEX(cur)]!=NULL_SOCKET && socketlist[SOCKET_TO_INDEX(cur)]<0)
			{
				dp++;
				cur = socketlist[SOCKET_TO_INDEX(cur)];
			}

			if( isTexture(cur))
				return textureExists(cur) && dp>=depth;
			else
				return (socketlist[SOCKET_TO_INDEX(cur)]!=cur && socketlist[SOCKET_TO_INDEX(cur)]!=NULL_SOCKET && dp>=depth);
		}
		else
			return false;
	}

	bool StreamManager::isTexture(SocketID s) { return TEXTURE_TO_INDEX(s)>=0; }
	bool StreamManager::isSocket(SocketID s)  { return SOCKET_TO_INDEX(s)>=0; }

// Socket information
	std::string StreamManager::getSocketInfo(SocketID s)
	{
		STREAMMANAGER_VERIFICATION

		if( !socketExists(s) )
			return "<Socket doesn't exist!>";
		else
			return *descriptions[SOCKET_TO_INDEX(s)];
	}

// Texture access
	HdlTexture& StreamManager::getTexture(SocketID s)
	{
		STREAMMANAGER_VERIFICATION

		SocketID cur = s;

		while(!isTexture(cur))
		{
			if(cur==socketlist[SOCKET_TO_INDEX(cur)])
				THROW_ERROR("StreamManager : Infinite loop detected!")
			if(socketlist[SOCKET_TO_INDEX(cur)]==NULL_SOCKET)
				THROW_ERROR("StreamManager : Socket not connected")

			cur = socketlist[SOCKET_TO_INDEX(cur)];
		}

		return *texturelist[TEXTURE_TO_INDEX(cur)];
	}

// Socket Connection
	void StreamManager::connect(SocketID src, SocketID dst)
	{
		STREAMMANAGER_VERIFICATION

		if( !socketExists(src) )
			THROW_ERROR("StreamManager : attempting to connect an unknown socket.")

		if( !(socketExists(dst) || textureExists(dst)) )
			THROW_ERROR("StreamManager : attempting to connect socket to unknown socket.")

		if(src==dst)
			THROW_ERROR("StreamManager : attempting to connect socket to itself.")

		socketlist[SOCKET_TO_INDEX(src)] = dst;
	}

	SocketID StreamManager::getConnection(SocketID s)
	{
		STREAMMANAGER_VERIFICATION
		return socketlist[SOCKET_TO_INDEX(s)];
	}

// NetWork mapping information
	void StreamManager::mapSocketConnection(SocketID s)
	{
		STREAMMANAGER_VERIFICATION

		SocketID cur = s;

		std::cout << "Socket Mapping information for socket : " << s << std::endl;
		std::cout << "    List : " << std::endl;

		while(!isTexture(cur))
		{
			if(cur==socketlist[SOCKET_TO_INDEX(cur)])
			{
				std::cout << "      Socket  " << cur << " : " << getSocketInfo(cur) << " [INFINITE LOOP]" << std::endl;
				THROW_ERROR("StreamManager : Infinite loop detected!")
				return ;
			}
			if(socketlist[SOCKET_TO_INDEX(cur)]==NULL_SOCKET)
			{
				THROW_ERROR("StreamManager : Socket not connected")
				return ;
			}

			if(!isTexture(cur))
				std::cout << "      Socket  " << cur << " : " << getSocketInfo(cur) << std::endl;
			else
				std::cout << "      Texture " << cur << std::endl;

			cur = socketlist[SOCKET_TO_INDEX(cur)];
		}

		std::cout << "END Mapping" << std::endl;
	}

	void StreamManager::debugInformation(void)
	{
		// For all sockets, verify that they are connected :
		std::cout << "StreamManager::debugInformation : " << std::endl;

		for(int i=0; i<STRM_MNG_MAX_SOCKETS; i++)
		{
			if( socketlist[i]!=NULL_SOCKET ) // Socket is allocated
			{
				if( socketlist[i]==INDEX_TO_SOCKET(i) ) std::cout << "    Socket " << INDEX_TO_SOCKET(i) << " isn't connected [Info : " << *descriptions[i] << "]" << std::endl;
			}
		}


		std::cout << "End" << std::endl;
	}

*/

