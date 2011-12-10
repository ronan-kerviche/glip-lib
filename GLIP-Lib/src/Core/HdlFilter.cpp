/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : GPLv3                                                                                     */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : HdlFilter.cpp                                                                             */
/*     Original Date : October 17th 2010                                                                         */
/*                                                                                                               */
/*     Description   : Filters and Pipelines handle                                                              */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    HdlFilter.cpp
 * \brief   Filters and Pipelines handle
 * \author  R. KERVICHE
 * \version 0.6
 * \date    October 17th 2010
*/

/*#include <sstream>
#include "HdlFilter.hpp"
#include "HdlTexture.hpp"
#include "HdlFBO.hpp"
#include "HdlShader.hpp"
#include "StreamManager.hpp"
#include "../include/GLIPLib.hpp"

// No fragment var :
    extern const char* FragmentNone[]  = { "uniform sampler2D texInput; \n",
                                           "void main() \n",
                                           "{ \n",
                                           "    vec4 col = texture(texInput, gl_TexCoord[0].st); \n",
                                           "    gl_FragColor = col; \n",
                                           "} \n",
                                           0 };

using namespace Glip::CoreGL;
using namespace Glip::CorePipeline;*/

// Static vars init :
    /*std::string getVertexSource(void)
    {
        int nUnits = HdlMultiTexturing::getMaxUnit();

        std::stringstream str;

        str << "void main() \n { \n    gl_FrontColor  = gl_Color; \n";

        for(int i=0; i<nUnits; i++)
            str << "    gl_TexCoord[" << i << "] = gl_TextureMatrix[" << i << "] * gl_MultiTexCoord" << i << "; \n";

        str << "    gl_Position = gl_ModelViewMatrix * gl_Vertex; \n } \n";

        return std::string(str.str());
    }*/

// methods :

// BaseFilter
    /*BaseFilter::BaseFilter(int _kind, int _nInput, int _nOutput, const std::string& _name) : InputModule(_nInput, _name), OutputModule(_nOutput, _name)
    {
        kind    = _kind;
        name    = _name;
    }

    int BaseFilter::getKind(void)
    {
        return kind;
    }

    std::string BaseFilter::getName(void)
    {
        return name;
    }

    int  BaseFilter::getSize(void)
    {
        return 0;
    }

    bool BaseFilter::process(void)
    {
        return true;
    }

// HdlFilter
    HdlFilter::HdlFilter(const std::string& _name, ShaderSource& src, const __HdlTextureFormat_OnlyData& fmt, int nInput, int nOutput, int _outMode, ShaderSource* srcVertex) : BaseFilter(KIND_FILTER, nInput, nOutput, _name), autoClear(true), blendingStack(false)
    {
        cleanInit();

        nTexture = nOutput;
        outMode  = _outMode;

        textureList    = new HdlTexture*[nTexture];
        textureSockets = new SocketID[nTexture];

        if(outMode==MODE_HISTORY)
            fboList        = new HdlFBO*[nTexture];
        else
            fboList        = new HdlFBO*[1];

        for(int i=0; i<nTexture; i++)
        {
            textureList[i]    = new HdlTexture(fmt);
            textureList[i]->fill(0);
            if(outMode==MODE_HISTORY)
                fboList[i]        = new HdlFBO(textureList[i]);
            else
            {
                if(i==0)
                    fboList[0]    = new HdlFBO(textureList[0]);
                else
                    fboList[0]->addTarget(textureList[i]);
            }

            textureSockets[i] = StreamManager::queryTextureSocket(textureList[i]);
            StreamManager::connect(out(i), textureSockets[i]);
        }

        // Vertex shader
        if( srcVertex==NULL )
        {
            srcVertex = new ShaderSource;
            std::string vertsource = getVertexSource();
            srcVertex->loadSource(vertsource);
        }

        vertexShader  = new HdlShader(GL_VERTEX_SHADER,   *srcVertex);
        pixelShader   = new HdlShader(GL_FRAGMENT_SHADER, src);
        prgm          = new HdlProgram(*vertexShader, *pixelShader);

        if(outMode==MODE_HISTORY)
            currentTarget = nTexture-1;
        else
            currentTarget = 0;
    }

    HdlFilter::~HdlFilter(void)
    {
        for(int i=0; i<nTexture; i++)
        {
            StreamManager::releaseTextureSocket(textureSockets[i]);
            delete textureList[i];
        }

        delete[] textureList;
        delete[] textureSockets;

        delete prgm;
        delete vertexShader;
        delete pixelShader;
    }

    void HdlFilter::cleanInit(void)
    {
        textureList        = NULL;
        textureSockets     = NULL;
        vertexShader       = NULL;
        pixelShader        = NULL;
        prgm               = NULL;
    }

    void HdlFilter::updateBufferList(void)
    {
        if(nTexture>1)
        {
            SocketID last = StreamManager::getConnection(out(nTexture-1));

            for(int i=nTexture-1; i>0; i--)
                StreamManager::connect( out(i), StreamManager::getConnection(out(i-1)));

            StreamManager::connect( out(0), last);

            currentTarget--;
            if( currentTarget<0 ) currentTarget = nTexture-1;
        }
    }

    void HdlFilter::clearTexture(int i, char data)
    {
        if(i!=-1)
            textureList[i]->fill(data);
        else
        {
            for(int j=0; j<nTexture; j++)
                textureList[j]->fill(data);
        }
    }

    bool HdlFilter::autoClearEnabled(void)
    {
        return autoClear;
    }

    void HdlFilter::setAutoClear(bool val)
    {
        autoClear = val;
    }

    bool HdlFilter::blendingStackEnabled(void)
    {
        return blendingStack;
    }

    void HdlFilter::setBlendingStack(bool val)
    {
        blendingStack = val;
    }

    HdlProgram& HdlFilter::program(void)
    {
        prgm->use();
        return *prgm;
    }

    void HdlFilter::setInputSamplerName(int id, std::string nameInShader)
    {
        prgm->use();
        prgm->modifyVar(nameInShader.c_str(), HdlProgram::SHADER_VAR, id);
        HdlProgram::stopProgram();
    }

    void HdlFilter::setOutputColorName(int id, std::string nameInShader)
    {
        prgm->use();
        prgm->setFragmentLocation(nameInShader.c_str(), id);
        HdlProgram::stopProgram();
    }

    int HdlFilter::getSize(void)
    {
        return nTexture * textureList[0]->getSize();
    }

    bool HdlFilter::process(void)
    {
        if(!allConnected())
        {
            std::cout << "HdlFilter::process - some input lines are not connected" << std::endl;
            debugConnection();
            return false;
        }

        if(blendingStack)
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_ONE, GL_ONE);
        }

        fboList[currentTarget]->beginRendering();

            prgm->use();

            bindAll();

            //OLD : glColor3ub(255,255,255);
            //New : clear components to avoid re-writing!
            if(autoClear)
            {
                glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            }

            glLoadIdentity();
            applyModification();
            //drawQuad(-1.0, -1.0, 1.0, 1.0);
            drawGeometry();

            unbindAll();

            HdlProgram::stopProgram();

        fboList[currentTarget]->endRendering();

        if(blendingStack)
        {
            glDisable(GL_BLEND);
        }

        if(outMode==MODE_HISTORY)
            updateBufferList();

        return true;
    }

// HdlPipeline
    ///
     \fn 	Create an empty pipeline
     \param	Number of links with outter parts
    ///
    HdlPipeline::HdlPipeline(const std::string& _name, int nInput, int nOutput) : BaseFilter(KIND_PIPELINE, nInput, nOutput, _name)
    {
    }

    ///
     \fn		Classic destructor of the Pipeline class
    ///
    HdlPipeline::~HdlPipeline(void)
    {
        std::vector<BaseFilter*>::iterator current = pipeline.begin();

        while(current!=pipeline.end())
        {
            delete *current;
            current++;
        }
    }

    int HdlPipeline::getSize(void)
    {
        int size = 0;
        std::vector<BaseFilter*>::iterator current = pipeline.begin();

        while(current!=pipeline.end())
        {
            size += (*current)->getSize();
            current ++;
        }

        return size;
    }

    ///
    @fn		Process the entire pipeline
    ///
    bool HdlPipeline::process(void)
    {
        std::vector<BaseFilter*>::iterator current = pipeline.begin();

        while(current!=pipeline.end())
        {
            if(!(*current)->process()) return false;

            current++;
        }

        return true;
    }

    int HdlPipeline::addFilter(BaseFilter* filter)
    {
        pipeline.push_back(filter);
        return pipeline.size()-1;
    }

    HdlFilter& HdlPipeline::filter(int i)
    {
        if(i<0 || i>=pipeline.size()) throw std::string("HdlPipeline::filter - invalid index");

        return reinterpret_cast<HdlFilter&>(*pipeline[i]);
    }

    HdlFilter& HdlPipeline::filter(const std::string& _name)
    {
        std::vector<BaseFilter*>::iterator current = pipeline.begin();

        while(current!=pipeline.end())
        {
            if((*current)->getName()==_name) return reinterpret_cast<HdlFilter&>(**current);
            current++;
        }

        throw (std::string("HdlPipeline::filter - can't find ") + _name);
    }

    HdlPipeline& HdlPipeline::subPipeline(int i)
    {
        if(i<0 || i>=pipeline.size()) throw std::string("HdlPipeline::subPipeline - invalid index");

        return reinterpret_cast<HdlPipeline&>(*pipeline[i]);
    }

    HdlPipeline& HdlPipeline::subPipeline(const std::string& _name)
    {
        std::vector<BaseFilter*>::iterator current = pipeline.begin();

        while(current!=pipeline.end())
        {
            if((*current)->getName()==_name) return reinterpret_cast<HdlPipeline&>(**current);
            current++;
        }

        throw (std::string("HdlPipeline::filter - can't find ") + _name);
    }
    */
