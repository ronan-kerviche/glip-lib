/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : GPLv3                                                                                     */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : IOModules.cpp                                                                             */
/*     Original Date : October 17th 2010                                                                         */
/*                                                                                                               */
/*     Description   : Generic Input and Output Modules                                                          */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    IOModules.cpp
 * \brief   Generic Input and Output Modules
 * \author  R. KERVICHE
 * \version 0.6
 * \date    October 17th 2010
**/

#include "IOModules.hpp"

// Includes
	#include "StreamManager.hpp"
	#include "HdlTexture.hpp"
	#include "HdlFBO.hpp"
	#include "HdlVBO.hpp"

using namespace Glip::CoreGL;
using namespace Glip::CorePipeline;

// Mapping
    Mapping::Mapping(void) { xLow = 0.0; xHigh = 1.0; yLow = 0.0; yHigh = 1.0; }
    Mapping::~Mapping(void) {}

    void Mapping::setQuad(double  xl, double  xh, double  yl, double  yh) { xLow = xl; xHigh = xh; yLow = yl; yHigh = yh;}
    void Mapping::getQuad(double& xl, double& xh, double& yl, double& yh) { xl = xLow; xh = xHigh; yl = yLow; yh = yHigh;}

    double Mapping::xlow(void)  { return xLow; }
    double Mapping::xhigh(void) { return xHigh; }
    double Mapping::ylow(void)  { return yLow; }
    double Mapping::yhigh(void) { return yHigh; }

// Input Module 2nd version
    InputModule::InputModule(int nInput, std::string _owner) : owner(_owner)
    {
        nIn = nInput;

        inputSockets = new SocketID[nIn];
        mappings     = new Mapping[nIn];
        geometry     = NULL;

        for(int i=0; i<nIn; i++)
        {
            std::string tmpName = (owner + "/in:") + static_cast<char>('0'+i);
            inputSockets[i] = StreamManager::querySocket(tmpName);
        }
    }

    InputModule::~InputModule(void)
    {
        for(int i=0; i<nIn; i++)
            StreamManager::releaseSocket(inputSockets[i]);

        delete[] inputSockets;
        delete[] mappings;
    }

    bool InputModule::connected(int i)
    {
        return StreamManager::socketIsLinked(inputSockets[i]);
    }

    bool InputModule::allConnected(void)
    {
        for(int i=0; i<nIn; i++)
            if( !connected(i) )
            {
                std::cout << "Input " << i << " is said to be not connected" << std::endl;
                return false;
            }

        return true;
    }

    int  InputModule::getNumInput(void)
    {
        return nIn;
    }

    SocketID InputModule::in(int i)
    {
        if(i<0 || i>=nIn)
        {
            std::cerr << "Input Socket " << i << " does not exist on " << owner << std::endl;
            return NULL_SOCKET;
        }
        return inputSockets[i];
    }

    HdlTexture& InputModule::texture(int i)
    {
        return StreamManager::getTexture(inputSockets[i]);
    }

    Mapping&    InputModule::mapping(int i)
    {
        return mappings[i];
    }

    void InputModule::bindAll(void)
    {
        if(nIn>1)
        {
            for(int i=0; i<nIn; i++)
                HdlMultiTexturing::bindToUnit(texture(i), HdlMultiTexturing::unitName(i));
        }
        else
        {
            HdlMultiTexturing::setWorkingUnit(GL_TEXTURE0_ARB);

            //bind on unit 0 :
            HdlMultiTexturing::bindToUnit(texture());
        }
    }

    void InputModule::drawGeometry(double aX, double aY, double bX, double bY)
    {
        // Standard Quad : -1.0, -1.0, 1.0, 1.0

        if(geometry==NULL)
        {
            glBegin(GL_QUADS);
                for(int i=0; i<nIn; i++) HdlMultiTexturing::glTexCoord2d( mappings[i].xhigh(), mappings[i].ylow(),  HdlMultiTexturing::unitName(i));
                    glVertex2f(bX,aY);
                for(int i=0; i<nIn; i++) HdlMultiTexturing::glTexCoord2d( mappings[i].xlow(),  mappings[i].ylow(),  HdlMultiTexturing::unitName(i));
                    glVertex2f(aX,aY);
                for(int i=0; i<nIn; i++) HdlMultiTexturing::glTexCoord2d( mappings[i].xlow(),  mappings[i].yhigh(), HdlMultiTexturing::unitName(i));
                    glVertex2f(aX,bY);
                for(int i=0; i<nIn; i++) HdlMultiTexturing::glTexCoord2d( mappings[i].xhigh(), mappings[i].yhigh(), HdlMultiTexturing::unitName(i));
                    glVertex2f(bX,bY);
            glEnd();
        }
        else
            geometry->draw();
    }

    void InputModule::unbindAll(void)
    {
        if(nIn>1)
        {
            for(int i=0; i<nIn; i++)
                HdlMultiTexturing::unbindUnit(HdlMultiTexturing::unitName(i));
        }
        else
            HdlMultiTexturing::unbindUnit();
    }

    void InputModule::debugConnection(void)
    {
        for(int i=0; i<nIn; i++)
            if( !connected(i) )
            {
                std::cout << "Input " << i << " is said to be not connected" << std::endl;
                //Map this connection :
                StreamManager::mapSocketConnection(in(i));
            }
    }

    // TEST : geometry action
    void InputModule::setGeometry(HdlVBO* geo)
    {
        geometry = geo;
    }

    void InputModule::resetGeometry(void)
    {
        geometry = NULL;
    }

// Output Module 2nd version
    OutputModule::OutputModule(int nOutput, std::string _owner) : owner(_owner)
    {
        nOut = nOutput;
        outputSockets = new SocketID[nOut];

        for(int i=0; i<nOut; i++)
        {
            std::string tmpName = (owner + "/out:") + static_cast<char>('0'+i);
            outputSockets[i] = StreamManager::querySocket(tmpName);
        }
    }

    OutputModule::~OutputModule(void)
    {
        for(int i=0; i<nOut; i++)
            StreamManager::releaseSocket(outputSockets[i]);

        delete[] outputSockets;
    }

    SocketID OutputModule::out(int i)
    {
        if(i<0 || i>=nOut)
        {
            std::cerr << "Output Socket " << i << " does not exist on " << owner << std::endl;
            return NULL_SOCKET;
        }

        return outputSockets[i];
    }

    int OutputModule::getNumOutput(void)
    {
        return nOut;
    }


