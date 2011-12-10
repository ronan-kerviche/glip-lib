/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : GPLv3                                                                                     */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : HdlFilter.hpp                                                                             */
/*     Original Date : October 17th 2010                                                                         */
/*                                                                                                               */
/*     Description   : Filters and Pipelines handle                                                              */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    HdlFilter.hpp
 * \brief   Filters and Pipelines handle
 * \author  R. KERVICHE
 * \version 0.6
 * \date    October 17th 2010
*/

/*#ifndef __HDLFILTER_INCLUDE__
#define __HDLFILTER_INCLUDE__

// Include
	#include <vector>
	#include <string>
    #include "OglInclude.hpp"
	#include "IOModules.hpp"
	#include "HdlModificator.hpp"

namespace Glip
{
    namespace CoreGL
    {
        // Prototype
            class HdlTexture;
            class __HdlTextureFormat_OnlyData;
            class HdlFBO;
            class ShaderSource;
            class HdlShader;
            class HdlProgram;
    }

    using namespace CoreGL;

    namespace CorePipeline
    {

        // Kind descriptors
            enum
            {
                KIND_FILTER,
                KIND_PIPELINE
            };

        // BaseFilter
            class BaseFilter : public InputModule, public OutputModule
            {
                private :
                    // Data
                        int kind;
                        std::string name;
                protected :
                    // Methods
                        BaseFilter(BaseFilter&);
                        BaseFilter(int _kind, int _nInput, int _nOutput, const std::string& _name);
                public :
                    // Functions
                        int          getKind(void);
                        std::string  getName(void);
                        virtual int  getSize(void);
                        virtual bool process(void);
            };

        // Filter handle :
            class HdlFilter : public BaseFilter, public HdlModificator
            {
                private :
                    // Static data
                        //OLD : static const char* vertexsrc[];
                    // Data
                        int nTexture;
                        int currentTarget;
                        int outMode;
                        bool autoClear, blendingStack;
                        HdlTexture **textureList;
                        HdlFBO     **fboList;
                        SocketID   *textureSockets;
                        HdlShader  *vertexShader, *pixelShader;
                        HdlProgram *prgm;

                    // Private tools
                        void cleanInit(void);
                        void updateBufferList(void);

                public :
                    // Modes
                        enum
                        {
                            MODE_HISTORY,
                            MODE_MULTITARGET
                        };

                    // methods :
                        HdlFilter(const std::string& _name, ShaderSource& src, const __HdlTextureFormat_OnlyData& fmt, int nInput=1, int nOuput=1, int _outMode = MODE_HISTORY, ShaderSource* srcVertex = NULL);
                        ~HdlFilter(void);

                        HdlProgram& program(void);
                        void setInputSamplerName(int i, std::string nameInShader);
                        void setOutputColorName(int i, std::string nameInShader);
                        void clearTexture(int i, char data);
                        bool autoClearEnabled(void);
                        void setAutoClear(bool val);
                        bool blendingStackEnabled(void);
                        void setBlendingStack(bool val);

                        int  getSize(void);
                        bool process(void);
            };

        // Pipeline Handle
            class HdlPipeline : public BaseFilter
            {
                private :
                    // Data
                        std::vector<BaseFilter*> pipeline;
                public :
                    // Functions
                        HdlPipeline(const std::string& _name, int nInput, int nOutput);
                        ~HdlPipeline(void);

                        int  getSize(void);
                        bool process(void);
                        int  addFilter(BaseFilter* filter);
                        HdlFilter& filter(int i);
                        HdlFilter& filter(const std::string& _name);
                        HdlPipeline& subPipeline(int i);
                        HdlPipeline& subPipeline(const std::string& _name);
            };
    }
}

// No fragment var :
    extern const char* FragmentNone[];

#endif*/

