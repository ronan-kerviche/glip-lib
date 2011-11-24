#include "HdlInfo.hpp"
#include <GL/gl.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

using namespace Glip::CoreGL;

    HdlInfo* HdlInfo::p_instance = 0;
    HdlInfo* HdlInfo::instance ()
    {
        //Singleton
        if (p_instance == 0) // Si on appel cette méthode pour la premiere fois
        {
            p_instance = new HdlInfo (); // on créé l'objet
        }
        return p_instance; // on renvoie le pointeur de l'objet
    }

    bool HdlInfo::init(std::string extensionfile)
    {
        glinfo = new glinfo_data();
        checkExtSupportedByGpu();
        return checkExtUsedByLib(extensionfile) && loadConfig();
    }

    bool HdlInfo::loadConfig()
    {
        char* str = 0;
        char* tok = 0;

        // get vendor string
        str = (char*)glGetString(GL_VENDOR);
        if(str) glinfo->vendor = str;                  // check NULL return value
        else return false;

        // get renderer string
        str = (char*)glGetString(GL_RENDERER);
        if(str) glinfo->renderer = str;                // check NULL return value
        else return false;

        // get version string
        str = (char*)glGetString(GL_VERSION);
        if(str) glinfo->version = str;                 // check NULL return value
        else return false;

        // get number of color bits
        glGetIntegerv(GL_RED_BITS, &glinfo->redBits);
        glGetIntegerv(GL_GREEN_BITS, &glinfo->greenBits);
        glGetIntegerv(GL_BLUE_BITS, &glinfo->blueBits);
        glGetIntegerv(GL_ALPHA_BITS, &glinfo->alphaBits);

        // get depth bits
        glGetIntegerv(GL_DEPTH_BITS, &glinfo->depthBits);

        // get stecil bits
        glGetIntegerv(GL_STENCIL_BITS, &glinfo->stencilBits);

        // get max number of lights allowed
        glGetIntegerv(GL_MAX_LIGHTS, &glinfo->maxLights);

        // get max texture resolution
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &glinfo->maxTextureSize);

        // get max number of clipping planes
        glGetIntegerv(GL_MAX_CLIP_PLANES, &glinfo->maxClipPlanes);

        // get max modelview and projection matrix stacks
        glGetIntegerv(GL_MAX_MODELVIEW_STACK_DEPTH, &glinfo->maxModelViewStacks);
        glGetIntegerv(GL_MAX_PROJECTION_STACK_DEPTH, &glinfo->maxProjectionStacks);
        glGetIntegerv(GL_MAX_ATTRIB_STACK_DEPTH, &glinfo->maxAttribStacks);

        // get max texture stacks
        glGetIntegerv(GL_MAX_TEXTURE_STACK_DEPTH, &glinfo->maxTextureStacks);
    }

    void HdlInfo::checkExtSupportedByGpu()
    {
        char* str = 0;
        char* tok = 0;

        // get all extensions as a string
        str = (char*)glGetString(GL_EXTENSIONS);

        // split extensions
        if(str)
        {
            tok = strtok((char*)str, " ");
            while(tok)
            {
                extentions[tok] = true;   // put a extension into struct
                tok = strtok(0, " ");               // next token
            }
        }
    }

    bool HdlInfo::checkExtUsedByLib(std::string extensionfile)
    {
        bool test = true;
        std::cout << "Check extension needed by Lib" << std::endl;
        // On ouvre le flux
        std::ifstream LoadFile(extensionfile.c_str());
        std::string   ligne;

        // On vérifie si le fichier est bien ouvert
        while  (std::getline (LoadFile, ligne))
        {
            if (ligne[0] != '#')
            {
                std::cout  << ligne << " : ";
                // On remplit le tableau d'extensions
                if (extentions.find (ligne) == extentions.end())
                {
                    std::cout << "Not supported" << std::endl;
                    test = false;
                }
                else
                {
                    std::cout << "OK" << std::endl;
                }
            }
        }

        LoadFile.close();

        return test;
    }

    bool HdlInfo::isExtensionSupported(std::string ext)
    {
        if (extentions.find(ext) == extentions.end())
            return false;

        return extentions[ext];
    }
