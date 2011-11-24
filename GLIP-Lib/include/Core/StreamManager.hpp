/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : GPLv3                                                                                     */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : StreamManager.hpp                                                                         */
/*     Original Date : October 17th 2010                                                                         */
/*                                                                                                               */
/*     Description   : StreamManager is the manager that interconnects streams                                   */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    StreamManager.hpp
 * \brief   StreamManager is the manager that interconnects streams
 * \author  R. KERVICHE
 * \version 0.6
 * \date    October 17th 2010
**/

#ifndef __STREAM_MANAGER_INCLUDE__
#define __STREAM_MANAGER_INCLUDE__

	// Lib
		#include <string>

	// Defines
		#define STRM_MNG_MAX_SOCKETS  (4096)
		#define NULL_SOCKET              (0)

	// Macro
		#define INDEX_TO_SOCKET(id)  (-((id)+1))
		#define SOCKET_TO_INDEX(id)  (-(id)-1)
		#define TEXTURE_TO_INDEX(id) ((id)-1)
		#define INDEX_TO_TEXTURE(id) ((id)+1)

namespace Glip
{
    namespace CoreGL
    {
        // Prototypes
            class HdlTexture;
    }

    using namespace CoreGL;

    namespace CorePipeline
    {
        // Alias
            typedef signed int SocketID;

        // Main structure
            class StreamManager
            {
                private :
                    static bool         built;
                    static HdlTexture*  texturelist[STRM_MNG_MAX_SOCKETS];
                    static SocketID     socketlist[STRM_MNG_MAX_SOCKETS];
                    static std::string* descriptions[STRM_MNG_MAX_SOCKETS];
                    static int          inst_count;
                public :
                    StreamManager(void);
                    ~StreamManager(void);

                    // Socket declaring
                    static SocketID    queryTextureSocket  (HdlTexture* ptr);
                    static void        releaseTextureSocket(SocketID s);
                    static SocketID    querySocket         (std::string info="<No information given>");
                    static void        releaseSocket       (SocketID s);

                    // Socket checking
                    static bool socketExists(SocketID s);
                    static bool textureExists(SocketID s);
                    static bool socketIsLinked(SocketID s, int depth=-1);
                    static bool isTexture(SocketID s);
                    static bool isSocket(SocketID s);

                    // Texture access
                    static HdlTexture& getTexture(SocketID s);

                    // Socket Connection
                    static void     connect(SocketID src, SocketID dst);
                    static SocketID getConnection(SocketID s);

                    // Socket information
                    static std::string getSocketInfo(SocketID s);

                    // NetWork mapping information
                    static void mapSocketConnection(SocketID s);
                    static void debugInformation(void);
            };

        /** Function
            Creates socket for a texture

            SocketID s;

            s<0 => s is a port
            s>0 => s is a texture
            s=0 => s is free
            s=s => s is not yet initialized (allocated but not linked)
        **/

        // Global Connect operator :
        //void connectStream(SocketID src,  SocketID& dst);
        //void connectStream(SocketID& dst, SocketID src);
    }
}

#endif

