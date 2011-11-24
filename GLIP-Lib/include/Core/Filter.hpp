#ifndef __GLIPLIB_FILTER__
#define __GLIPLIB_FILTER__

    // Includes
        #include "./Component.hpp"
        #include "HdlTexture.hpp"

namespace Glip
{
    namespace CoreGL
    {
        // Prototypes
            class ShaderSource;
            class HdlShader;
            class HdlProgram;
            class HdlVBO;
            class HdlFBO;
    }

    namespace CorePipeline
    {
        // Prototypes
            class Pipeline;
            class FilterLayout;

        // Objects
            class __ReadOnly_FilterLayout : virtual public __ReadOnly_ComponentLayout, virtual public __ReadOnly_HdlTextureFormat
            {
                private :
                    // Data
                        ShaderSource *vertexSource,
                                     *fragmentSource;
                        bool blending, clearing;
                    // Friends
                        friend class FilterLayout;
                protected :
                    // Tools
                        __ReadOnly_FilterLayout(const std::string& type, const __ReadOnly_HdlTextureFormat& f);
                public :
                    // Tools
                        __ReadOnly_FilterLayout(const __ReadOnly_FilterLayout&);
                        ~__ReadOnly_FilterLayout(void);
                        ShaderSource& getVertexSource(void) const;
                        ShaderSource& getFragmentSource(void) const;

                        bool isBlendingEnable(void);
                        void enableBlending(void);
                        void disableBlending(void);
                        bool isClearingEnable(void);
                        void enableClearing(void);
                        void disableClearing(void);
            };

            class FilterLayout : virtual public ComponentLayout, virtual public __ReadOnly_FilterLayout
            {
                public :
                    // Tools
                        FilterLayout(const std::string& type, const __ReadOnly_HdlTextureFormat& fout, const ShaderSource& fragment, ShaderSource* vertex = NULL);
            };

            class Filter : virtual public Component, virtual public __ReadOnly_FilterLayout
            {
                private :
                    // Data
                        HdlShader*  vertexShader;
                        HdlShader*  fragmentShader;
                        HdlProgram* program;
                        HdlVBO*     vbo;
                protected :
                    // Tools
                        Filter(const __ReadOnly_FilterLayout&); // A filter can't be created outside a Pipeline

                        void setInputForNextRendering(int id, HdlTexture* ptr);
                        void process(HdlFBO& renderer);
                    // Friends
                        friend class Pipeline;
                public :
                    // Tools
                        ~Filter(void);
                        HdlProgram& operator->(void);
                        void setGeometry(HdlVBO* v);
            };
    }
}

#endif

/**
    Protoype de fonctionnement

using namespace Glip::CorePipeline;

Pipeline* pipe1, pipe2;
CameraFirewire* cam;
DisplayQT* disp;

void load(void)
{
    try
    {
        cam   = new CameraFirewire(640,480,30, GL_RGB, GL_UNSIGNED_BYTE);
        disp  = new DisplayQT(640,480,30);
        pipe1 = new Pipeline("./PipelineTest1.xml");
        pipe2 = new Pipeline("./PipelineTest2.xml");
    }
    catch(std::exception& e)
    {
        std::cerr << "Exception : " << e.what() << std::endl;
        throw e;
    }

    cam->start();
}

void run(void)
{
    if(cam->newFrame())
    {
        pipe1 << cam.out(0)   << Process;
        pipe2 << cam.out(0)   << pipe1.out(0) << Process;
        disp  << pipe2.out(0) << Process;
    }
}
**/

