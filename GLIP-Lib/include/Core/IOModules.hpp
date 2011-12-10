/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : GPLv3                                                                                     */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : IOModules.hpp                                                                             */
/*     Original Date : October 17th 2010                                                                         */
/*                                                                                                               */
/*     Description   : Generic Input and Output Modules                                                          */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    IOModules.hpp
 * \brief   Generic Input and Output Modules
 * \author  R. KERVICHE
 * \version 0.6
 * \date    October 17th 2010
**/

/*#ifndef __GLIP_IOMODULES__
#define __GLIP_IOMODULES__

// Includes
	#include <string>
	#include "OglInclude.hpp"

namespace Glip
{
    namespace CoreGL
    {
        // Prototypes
            class HdlTexture;
            class __HdlTextureFormat_OnlyData;
            class HdlFBO;
            class HdlVBO;
    }

    using namespace CoreGL;

    namespace CorePipeline
    {
        // Prototypes
            class StreamManager;

            typedef signed int SocketID;

        // Mapping
            class Mapping
            {
                private :
                    // Data
                        double xLow, xHigh, yLow, yHigh;
                public :
                    // Functions
                        Mapping(void);
                        ~Mapping(void);

                        void setQuad(double  xl, double  xh, double  yl, double  yh);
                        void getQuad(double& xl, double& xh, double& yl, double& yh);

                        double xlow(void);
                        double xhigh(void);
                        double ylow(void);
                        double yhigh(void);
            };

        // Input
            class InputModule
            {
                private :
                    // Data
                        int nIn;
                        std::string owner;
                        SocketID *inputSockets;
                        Mapping  *mappings;
                        HdlVBO   *geometry;
                public :
                    // Functions
                        InputModule(int nInput, std::string _owner);
                        ~InputModule(void);

                        // Attributes :
                        bool connected(int i=0);
                        bool allConnected(void);
                        int  getNumInput(void);

                        // Data access :
                        SocketID    in(int i=0);
                        HdlTexture& texture(int i=0);
                        Mapping&    mapping(int i=0);

                        // Drawing and geometry specifics :
                                        void setGeometry(HdlVBO* geo);
                        void resetGeometry(void);
                        void bindAll(void);
                        void drawGeometry(double aX=-1.0, double aY=-1.0, double bX=1.0, double bY=1.0);
                        void unbindAll(void);

                        // Debug specifics :
                        void debugConnection(void);
            };

        // Output
            class OutputModule
            {
                private :
                    // Data
                        int nOut;
                        std::string owner;
                        SocketID *outputSockets;
                public :
                    // Functions
                        OutputModule(int nOutput, std::string _owner);
                        ~OutputModule(void);

                        SocketID  out(int i);
                        int       getNumOutput(void);
            };
    }
}

#endif*/
