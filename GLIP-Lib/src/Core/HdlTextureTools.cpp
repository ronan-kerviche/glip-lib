/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : HdlTextureTools.cpp                                                                       */
/*     Original Date : August 18th 2012                                                                          */
/*                                                                                                               */
/*     Description   : OpenGL Texture Handle and descriptors                                                     */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    HdlTextureTools.cpp
 * \brief   OpenGL Texture Handle Tools
 * \author  R. KERVICHE
 * \date    August 18th 2012
**/

#include <cstring>
#include "Core/HdlTextureTools.hpp"
#include "Core/Exception.hpp"

using namespace Glip::CoreGL;

GLIP_API const int HdlTextureFormatDescriptor::maxNumChannels = 4;
GLIP_API const int HdlTextureFormatDescriptor::maxPixelSizeInBits = 128;
GLIP_API const int HdlTextureFormatDescriptor::maxPixelSize = 16;

// List of all the Mode accepted :
const HdlTextureFormatDescriptor HdlTextureFormatDescriptorsList::textureFormatDescriptors[] = {
{GL_RED, GL_RED, GL_RED,GL_COMPRESSED_RED, 1, {GL_RED, GL_NONE, GL_NONE, GL_NONE}, {-1, 0, 0, 0}, {GL_NONE, GL_NONE, GL_NONE, GL_NONE}, false, false, true},
{GL_GREEN, GL_RED, GL_GREEN,GL_COMPRESSED_RED, 1, {GL_GREEN, GL_NONE, GL_NONE, GL_NONE}, {-1, 0, 0, 0}, {GL_NONE, GL_NONE, GL_NONE, GL_NONE}, false, false, true},
{GL_BLUE, GL_RED, GL_BLUE, GL_COMPRESSED_RED, 1, {GL_BLUE, GL_NONE, GL_NONE, GL_NONE}, {-1, 0, 0, 0}, {GL_NONE, GL_NONE, GL_NONE, GL_NONE}, false, false, true},
{GL_R8,	GL_RED,	GL_R8,	GL_COMPRESSED_RED, 1, {GL_RED, GL_NONE, GL_NONE, GL_NONE}, {8, 0, 0, 0}, {GL_BYTE, GL_NONE, GL_NONE, GL_NONE}, false, false, true},
{GL_R8I, GL_RED, GL_R8I, GL_COMPRESSED_RED, 1, {GL_RED, GL_NONE, GL_NONE, GL_NONE}, {8, 0, 0, 0}, {GL_BYTE, GL_NONE, GL_NONE, GL_NONE}, false, false, true},
{GL_R8UI, GL_RED, GL_R8UI, GL_COMPRESSED_RED, 1, {GL_RED, GL_NONE, GL_NONE, GL_NONE}, {8, 0, 0, 0}, {GL_UNSIGNED_BYTE, GL_NONE, GL_NONE, GL_NONE}, false, false, true},
{GL_R8_SNORM, GL_RED, GL_R8_SNORM, GL_COMPRESSED_RED, 1, {GL_RED, GL_NONE, GL_NONE, GL_NONE}, {8, 0, 0, 0}, {GL_BYTE, GL_NONE, GL_NONE, GL_NONE}, false, false, true},
{GL_R16, GL_RED, GL_R16, GL_COMPRESSED_RED, 1, {GL_RED, GL_NONE, GL_NONE, GL_NONE}, {16, 0, 0, 0}, {GL_SHORT, GL_NONE, GL_NONE, GL_NONE}, false, false, true},
{GL_R16I, GL_RED, GL_R16I, GL_COMPRESSED_RED, 1, {GL_RED, GL_NONE, GL_NONE, GL_NONE}, {16, 0, 0, 0}, {GL_SHORT, GL_NONE, GL_NONE, GL_NONE}, false, false, true},
{GL_R16UI, GL_RED, GL_R16UI, GL_COMPRESSED_RED, 1, {GL_RED, GL_NONE, GL_NONE, GL_NONE}, {16, 0, 0, 0}, {GL_UNSIGNED_SHORT, GL_NONE, GL_NONE, GL_NONE}, false, false, true},
{GL_R16_SNORM, GL_RED, GL_R16_SNORM, GL_COMPRESSED_RED, 1, {GL_RED, GL_NONE, GL_NONE, GL_NONE}, {16, 0, 0, 0}, {GL_SHORT, GL_NONE, GL_NONE, GL_NONE}, false, false, true},
{GL_R16F, GL_RED, GL_R16F, GL_COMPRESSED_RED, 1, {GL_RED, GL_NONE, GL_NONE, GL_NONE}, {16, 0, 0, 0}, {GL_FLOAT, GL_NONE, GL_NONE, GL_NONE}, true, false, true},
{GL_R32I, GL_RED, GL_R32I, GL_COMPRESSED_RED, 1, {GL_RED, GL_NONE, GL_NONE, GL_NONE}, {32, 0, 0, 0}, {GL_INT, GL_NONE, GL_NONE, GL_NONE}, false, false, true},
{GL_R32UI, GL_RED, GL_R32UI, GL_COMPRESSED_RED, 1, {GL_RED, GL_NONE, GL_NONE, GL_NONE}, {32, 0, 0, 0}, {GL_UNSIGNED_INT, GL_NONE, GL_NONE, GL_NONE}, false, false, true},
{GL_R32F, GL_RED, GL_R32F, GL_COMPRESSED_RED, 1, {GL_RED, GL_NONE, GL_NONE, GL_NONE}, {32, 0, 0, 0}, {GL_FLOAT, GL_NONE, GL_NONE, GL_NONE}, true, false, true},
{GL_ALPHA, GL_ALPHA, GL_ALPHA, GL_COMPRESSED_ALPHA, 1, {GL_ALPHA, GL_NONE, GL_NONE, GL_NONE}, {-1, 0, 0, 0}, {GL_NONE, GL_NONE, GL_NONE, GL_NONE}, false, false, true},
{GL_ALPHA4, GL_ALPHA, GL_ALPHA4, GL_COMPRESSED_ALPHA, 1, {GL_ALPHA, GL_NONE, GL_NONE, GL_NONE}, {4, 0, 0, 0}, {GL_NONE, GL_NONE, GL_NONE, GL_NONE}, false, false, false},
{GL_ALPHA8, GL_ALPHA, GL_ALPHA8, GL_COMPRESSED_ALPHA, 1, {GL_ALPHA, GL_NONE, GL_NONE, GL_NONE}, {8, 0, 0, 0}, {GL_BYTE, GL_NONE, GL_NONE, GL_NONE}, false, false, true},
{GL_ALPHA8_SNORM, GL_ALPHA, GL_ALPHA8_SNORM, GL_COMPRESSED_ALPHA, 1, {GL_ALPHA, GL_NONE, GL_NONE, GL_NONE}, {8, 0, 0, 0}, {GL_BYTE, GL_NONE, GL_NONE, GL_NONE}, false, false, true},
{GL_ALPHA12, GL_ALPHA, GL_ALPHA12, GL_COMPRESSED_ALPHA, 1, {GL_ALPHA, GL_NONE, GL_NONE, GL_NONE}, {12, 0, 0, 0}, {GL_NONE, GL_NONE, GL_NONE, GL_NONE}, false, false, false},
{GL_ALPHA16, GL_ALPHA, GL_ALPHA16, GL_COMPRESSED_ALPHA, 1, {GL_ALPHA, GL_NONE, GL_NONE, GL_NONE}, {16, 0, 0, 0}, {GL_SHORT, GL_NONE, GL_NONE, GL_NONE}, false, false, true},
{GL_ALPHA16F_ARB, GL_ALPHA, GL_ALPHA16F_ARB, GL_COMPRESSED_ALPHA, 1, {GL_ALPHA, GL_NONE, GL_NONE, GL_NONE}, {16, 0, 0, 0}, {GL_FLOAT, GL_NONE, GL_NONE, GL_NONE}, true, false, true},
{GL_ALPHA16_SNORM, GL_ALPHA, GL_ALPHA16_SNORM, GL_COMPRESSED_ALPHA, 1, {GL_ALPHA, GL_NONE, GL_NONE, GL_NONE}, {16, 0, 0, 0}, {GL_SHORT, GL_NONE, GL_NONE, GL_NONE}, false, false, true},
{GL_ALPHA32F_ARB, GL_ALPHA, GL_ALPHA32F_ARB, GL_COMPRESSED_ALPHA, 1, {GL_ALPHA, GL_NONE, GL_NONE, GL_NONE}, {32, 0, 0, 0}, {GL_FLOAT, GL_NONE, GL_NONE, GL_NONE}, true, false, true},
{GL_LUMINANCE, GL_LUMINANCE, GL_LUMINANCE, GL_COMPRESSED_LUMINANCE, 1, {GL_LUMINANCE, GL_NONE, GL_NONE, GL_NONE}, {-1, 0, 0, 0}, {GL_NONE, GL_NONE, GL_NONE, GL_NONE}, false, false, true},
{GL_LUMINANCE4, GL_LUMINANCE, GL_LUMINANCE4, GL_COMPRESSED_LUMINANCE, 1, {GL_LUMINANCE, GL_NONE, GL_NONE, GL_NONE}, {4, 0, 0, 0}, {GL_NONE, GL_NONE, GL_NONE, GL_NONE}, false, false, false},
{GL_LUMINANCE8, GL_LUMINANCE, GL_LUMINANCE8, GL_COMPRESSED_LUMINANCE, 1, {GL_LUMINANCE, GL_NONE, GL_NONE, GL_NONE}, {8, 0, 0, 0}, {GL_BYTE, GL_NONE, GL_NONE, GL_NONE}, false, false, true},
{GL_LUMINANCE8_SNORM, GL_LUMINANCE, GL_LUMINANCE8_SNORM, GL_COMPRESSED_LUMINANCE, 1, {GL_LUMINANCE, GL_NONE, GL_NONE, GL_NONE}, {8, 0, 0, 0}, {GL_BYTE, GL_NONE, GL_NONE, GL_NONE}, false, false, true},
{GL_LUMINANCE12, GL_LUMINANCE, GL_LUMINANCE12, GL_COMPRESSED_LUMINANCE, 1, {GL_LUMINANCE, GL_NONE, GL_NONE, GL_NONE}, {12, 0, 0, 0}, {GL_NONE, GL_NONE, GL_NONE, GL_NONE}, false, false, false},
{GL_LUMINANCE16, GL_LUMINANCE, GL_LUMINANCE16, GL_COMPRESSED_LUMINANCE, 1, {GL_LUMINANCE, GL_NONE, GL_NONE, GL_NONE}, {16, 0, 0, 0}, {GL_SHORT, GL_NONE, GL_NONE, GL_NONE}, false, false, true},
{GL_LUMINANCE16F_ARB, GL_LUMINANCE, GL_LUMINANCE16F_ARB, GL_COMPRESSED_LUMINANCE, 1, {GL_LUMINANCE, GL_NONE, GL_NONE, GL_NONE}, {16, 0, 0, 0}, {GL_FLOAT, GL_NONE, GL_NONE, GL_NONE}, true, false, true},
{GL_LUMINANCE16_SNORM, GL_LUMINANCE, GL_LUMINANCE16_SNORM, GL_COMPRESSED_LUMINANCE, 1, {GL_LUMINANCE, GL_NONE, GL_NONE, GL_NONE}, {16, 0, 0, 0}, {GL_SHORT, GL_NONE, GL_NONE, GL_NONE}, false, false, true},
{GL_LUMINANCE32F_ARB, GL_LUMINANCE, GL_LUMINANCE32F_ARB, GL_COMPRESSED_LUMINANCE, 1, {GL_LUMINANCE, GL_NONE, GL_NONE, GL_NONE}, {32, 0, 0, 0}, {GL_FLOAT, GL_NONE, GL_NONE, GL_NONE}, true, false, true},
{GL_INTENSITY, GL_INTENSITY, GL_INTENSITY, GL_COMPRESSED_INTENSITY, 1, {GL_INTENSITY, GL_NONE, GL_NONE, GL_NONE}, {-1, 0, 0, 0}, {GL_NONE, GL_NONE, GL_NONE, GL_NONE}, false, false, true},
{GL_INTENSITY4, GL_INTENSITY, GL_INTENSITY4, GL_COMPRESSED_INTENSITY, 1, {GL_INTENSITY, GL_NONE, GL_NONE, GL_NONE}, {4, 0, 0, 0}, {GL_NONE, GL_NONE, GL_NONE, GL_NONE}, false, false, false},
{GL_INTENSITY8, GL_INTENSITY, GL_INTENSITY8, GL_COMPRESSED_INTENSITY, 1, {GL_INTENSITY, GL_NONE, GL_NONE, GL_NONE}, {8, 0, 0, 0}, {GL_BYTE, GL_NONE, GL_NONE, GL_NONE}, false, false, true},
{GL_INTENSITY8_SNORM, GL_INTENSITY, GL_INTENSITY8_SNORM, GL_COMPRESSED_INTENSITY, 1, {GL_INTENSITY, GL_NONE, GL_NONE, GL_NONE}, {8, 0, 0, 0}, {GL_BYTE, GL_NONE, GL_NONE, GL_NONE}, false, false, true},
{GL_INTENSITY12, GL_INTENSITY, GL_INTENSITY12, GL_COMPRESSED_INTENSITY, 1, {GL_INTENSITY, GL_NONE, GL_NONE, GL_NONE}, {12, 0, 0, 0}, {GL_NONE, GL_NONE, GL_NONE, GL_NONE}, false, false, false},
{GL_INTENSITY16, GL_INTENSITY, GL_INTENSITY16, GL_COMPRESSED_INTENSITY, 1, {GL_INTENSITY, GL_NONE, GL_NONE, GL_NONE}, {16, 0, 0, 0}, {GL_SHORT, GL_NONE, GL_NONE, GL_NONE}, false, false, true},
{GL_INTENSITY16F_ARB, GL_INTENSITY, GL_INTENSITY16F_ARB, GL_COMPRESSED_INTENSITY, 1, {GL_INTENSITY, GL_NONE, GL_NONE, GL_NONE}, {16, 0, 0, 0}, {GL_FLOAT, GL_NONE, GL_NONE, GL_NONE}, true, false, true},
{GL_INTENSITY16_SNORM, GL_INTENSITY, GL_INTENSITY16_SNORM, GL_COMPRESSED_INTENSITY, 1, {GL_INTENSITY, GL_NONE, GL_NONE, GL_NONE}, {16, 0, 0, 0}, {GL_FLOAT, GL_NONE, GL_NONE, GL_NONE}, false, false, true},
{GL_INTENSITY32F_ARB, GL_INTENSITY, GL_INTENSITY32F_ARB, GL_COMPRESSED_INTENSITY, 1, {GL_INTENSITY, GL_NONE, GL_NONE, GL_NONE}, {32, 0, 0, 0}, {GL_FLOAT, GL_NONE, GL_NONE, GL_NONE}, true, false, true},
{GL_RG,	GL_RG, GL_RG, GL_COMPRESSED_RG, 2, {GL_RED, GL_GREEN, GL_NONE, GL_NONE}, {-1, -1, 0, 0}, {GL_NONE, GL_NONE, GL_NONE, GL_NONE}, false, false, true},
{GL_RG8, GL_RG, GL_RG8, GL_COMPRESSED_RG, 2, {GL_RED, GL_GREEN, GL_NONE, GL_NONE}, {8, 8, 0, 0}, {GL_BYTE, GL_BYTE, GL_NONE, GL_NONE}, false, false, true},
{GL_RG8I, GL_RG, GL_RG8I, GL_COMPRESSED_RG, 2, {GL_RED, GL_GREEN, GL_NONE, GL_NONE}, {8, 8, 0, 0}, {GL_BYTE, GL_BYTE, GL_NONE, GL_NONE}, false, false, true},
{GL_RG8UI, GL_RG, GL_RG8UI, GL_COMPRESSED_RG, 2, {GL_RED, GL_GREEN, GL_NONE, GL_NONE}, {8, 8, 0, 0}, {GL_UNSIGNED_BYTE, GL_UNSIGNED_BYTE, GL_NONE, GL_NONE}, false, false, true},
{GL_RG8_SNORM, GL_RG, GL_RG8_SNORM, GL_COMPRESSED_RG, 2, {GL_RED, GL_GREEN, GL_NONE, GL_NONE}, {8, 8, 0, 0}, {GL_BYTE, GL_BYTE, GL_NONE, GL_NONE}, false, false, true},
{GL_RG16, GL_RG, GL_RG16, GL_COMPRESSED_RG, 2, {GL_RED, GL_GREEN, GL_NONE, GL_NONE}, {16, 16, 0, 0}, {GL_SHORT, GL_SHORT, GL_NONE, GL_NONE}, false, false, true},
{GL_RG16I, GL_RG, GL_RG16I, GL_COMPRESSED_RG, 2, {GL_RED, GL_GREEN, GL_NONE, GL_NONE}, {16, 16, 0, 0}, {GL_SHORT, GL_SHORT, GL_NONE, GL_NONE}, false, false, true},
{GL_RG16UI, GL_RG, GL_RG16UI, GL_COMPRESSED_RG, 2, {GL_RED, GL_GREEN, GL_NONE, GL_NONE}, {16, 16, 0, 0}, {GL_UNSIGNED_SHORT, GL_UNSIGNED_SHORT, GL_NONE, GL_NONE}, false, false, true},
{GL_RG16F, GL_RG, GL_RG16F, GL_COMPRESSED_RG, 2, {GL_RED, GL_GREEN, GL_NONE, GL_NONE}, {16, 16, 0, 0}, {GL_FLOAT, GL_FLOAT, GL_NONE, GL_NONE}, true, false, true},
{GL_RG16_SNORM,	GL_RG, GL_RG16_SNORM,	GL_COMPRESSED_RG, 2, {GL_RED, GL_GREEN, GL_NONE, GL_NONE}, {16, 16, 0, 0}, {GL_SHORT, GL_SHORT, GL_NONE, GL_NONE}, false, false, true},
{GL_RG32I, GL_RG, GL_RG32I, GL_COMPRESSED_RG, 2, {GL_RED, GL_GREEN, GL_NONE, GL_NONE}, {32, 32, 0, 0}, {GL_INT, GL_INT, GL_NONE, GL_NONE}, false, false, true},
{GL_RG32UI, GL_RG, GL_RG32UI, GL_COMPRESSED_RG, 2, {GL_RED, GL_GREEN, GL_NONE, GL_NONE}, {32, 32, 0, 0}, {GL_UNSIGNED_INT, GL_UNSIGNED_INT, GL_NONE, GL_NONE}, false, false, true},
{GL_RG32F, GL_RG, GL_RG32F, GL_COMPRESSED_RG, 2, {GL_RED, GL_GREEN, GL_NONE, GL_NONE}, {32, 32, 0, 0}, {GL_FLOAT, GL_FLOAT, GL_NONE, GL_NONE}, true, false, true},
{GL_LUMINANCE_ALPHA, GL_LUMINANCE_ALPHA, GL_LUMINANCE_ALPHA, GL_COMPRESSED_LUMINANCE_ALPHA, 2, {GL_LUMINANCE, GL_ALPHA, GL_NONE, GL_NONE}, {-1, -1, 0, 0}, {GL_NONE, GL_NONE, GL_NONE, GL_NONE}, false, false, true},
{GL_LUMINANCE4_ALPHA4, GL_LUMINANCE_ALPHA, GL_LUMINANCE4_ALPHA4, GL_COMPRESSED_LUMINANCE_ALPHA, 2, {GL_LUMINANCE, GL_ALPHA, GL_NONE, GL_NONE}, {4, 4, 0, 0}, {GL_NONE, GL_NONE, GL_NONE, GL_NONE}, false, false, false},
{GL_LUMINANCE8_ALPHA8, GL_LUMINANCE_ALPHA, GL_LUMINANCE8_ALPHA8, GL_COMPRESSED_LUMINANCE_ALPHA, 2, {GL_LUMINANCE, GL_ALPHA, GL_NONE, GL_NONE}, {8, 8, 0, 0}, {GL_BYTE, GL_BYTE, GL_NONE, GL_NONE}, false, false, true},
{GL_LUMINANCE8_ALPHA8_SNORM, GL_LUMINANCE_ALPHA, GL_LUMINANCE8_ALPHA8_SNORM, GL_COMPRESSED_LUMINANCE_ALPHA, 2, {GL_LUMINANCE, GL_ALPHA, GL_NONE, GL_NONE}, {8, 8, 0, 0}, {GL_BYTE, GL_BYTE, GL_NONE, GL_NONE}, false, false, true},
{GL_LUMINANCE16_ALPHA16, GL_LUMINANCE_ALPHA, GL_LUMINANCE16_ALPHA16, GL_COMPRESSED_LUMINANCE_ALPHA, 2, {GL_LUMINANCE, GL_ALPHA, GL_NONE, GL_NONE}, {16, 16, 0, 0}, {GL_SHORT, GL_SHORT, GL_NONE, GL_NONE}, false, false, true},
{GL_LUMINANCE_ALPHA32F_ARB, GL_LUMINANCE_ALPHA, GL_LUMINANCE_ALPHA32F_ARB, GL_COMPRESSED_LUMINANCE_ALPHA, 2, {GL_LUMINANCE, GL_ALPHA, GL_NONE, GL_NONE}, {32, 32, 0, 0}, {GL_FLOAT, GL_FLOAT, GL_NONE, GL_NONE}, true, false, true},
{GL_RGB, GL_RGB, GL_RGB, GL_COMPRESSED_RGB, 3, {GL_RED, GL_GREEN, GL_BLUE, GL_NONE}, {-1, -1, -1, 0}, {GL_NONE, GL_NONE, GL_NONE, GL_NONE}, false, false, true},
{GL_BGR, GL_BGR, GL_BGR, GL_COMPRESSED_RGB, 3, {GL_RED, GL_GREEN, GL_BLUE, GL_NONE}, {-1, -1, -1, 0}, {GL_NONE, GL_NONE, GL_NONE, GL_NONE}, false, false, true},
{GL_SRGB, GL_SRGB, GL_SRGB, GL_COMPRESSED_SRGB, 3, {GL_RED, GL_GREEN, GL_BLUE, GL_NONE}, {-1, -1, -1, 0}, {GL_NONE, GL_NONE, GL_NONE, GL_NONE}, false, false, true},
{GL_RGB4, GL_RGB, GL_RGB4, GL_COMPRESSED_RGB, 3, {GL_RED, GL_GREEN, GL_BLUE, GL_NONE}, {4, 4, 4, 0}, {GL_NONE, GL_NONE, GL_NONE, GL_NONE}, false, false, false},
{GL_RGB8, GL_RGB, GL_RGB8, GL_COMPRESSED_RGB, 3, {GL_RED, GL_GREEN, GL_BLUE, GL_NONE}, {8, 8, 8, 0}, {GL_BYTE, GL_BYTE, GL_BYTE, GL_NONE}, false, false, true},
{GL_RGB8I, GL_RGB, GL_RGB8I, GL_COMPRESSED_RGB, 3, {GL_RED, GL_GREEN, GL_BLUE, GL_NONE}, {8, 8, 8, 0}, {GL_BYTE, GL_BYTE, GL_BYTE, GL_NONE}, false, false, true},
{GL_RGB8UI, GL_RGB, GL_RGB8UI, GL_COMPRESSED_RGB, 3, {GL_RED, GL_GREEN, GL_BLUE, GL_NONE}, {8, 8, 8, 0}, {GL_UNSIGNED_BYTE, GL_UNSIGNED_BYTE, GL_UNSIGNED_BYTE, GL_NONE}, false, false, true},
{GL_RGB8_SNORM,	GL_RGB,	GL_RGB8_SNORM, GL_COMPRESSED_RGB, 3, {GL_RED, GL_GREEN, GL_BLUE, GL_NONE}, {8, 8, 8, 0}, {GL_BYTE, GL_BYTE, GL_BYTE, GL_NONE}, false, false, true},
{GL_SRGB8, GL_RGB, GL_SRGB8, GL_COMPRESSED_SRGB, 3, {GL_RED, GL_GREEN, GL_BLUE, GL_NONE}, {8, 8, 8, 0}, {GL_BYTE, GL_BYTE, GL_BYTE, GL_NONE}, false, false, true},
{GL_RGB10, GL_RGB, GL_RGB10, GL_COMPRESSED_RGB, 3, {GL_RED, GL_GREEN, GL_BLUE, GL_NONE}, {10, 10, 10, 0}, {GL_NONE, GL_NONE, GL_NONE, GL_NONE}, false, false, false},
{GL_RGB12, GL_RGB, GL_RGB12, GL_COMPRESSED_RGB, 3, {GL_RED, GL_GREEN, GL_BLUE, GL_NONE}, {12, 12, 12, 0}, {GL_NONE, GL_NONE, GL_NONE, GL_NONE}, false, false, false},
{GL_RGB16, GL_RGB, GL_RGB16, GL_COMPRESSED_RGB, 3, {GL_RED, GL_GREEN, GL_BLUE, GL_NONE}, {16, 16, 16, 0}, {GL_SHORT, GL_SHORT, GL_SHORT, GL_NONE}, false, false, true},
{GL_RGB16I, GL_RGB, GL_RGB16I, GL_COMPRESSED_RGB, 3, {GL_RED, GL_GREEN, GL_BLUE, GL_NONE}, {16, 16, 16, 0}, {GL_SHORT, GL_SHORT, GL_SHORT, GL_NONE}, false, false, true},
{GL_RGB16UI, GL_RGB, GL_RGB16UI, GL_COMPRESSED_RGB, 3, {GL_RED, GL_GREEN, GL_BLUE, GL_NONE}, {16, 16, 16, 0}, {GL_UNSIGNED_SHORT, GL_UNSIGNED_SHORT, GL_UNSIGNED_SHORT, GL_NONE}, false, false, true},
{GL_RGB16_SNORM, GL_RGB, GL_RGB16_SNORM, GL_COMPRESSED_RGB, 3, {GL_RED, GL_GREEN, GL_BLUE, GL_NONE}, {16, 16, 16, 0}, {GL_SHORT, GL_SHORT, GL_SHORT, GL_NONE}, false, false, true},
{GL_RGB16F, GL_RGB, GL_RGB16F, GL_COMPRESSED_RGB, 3, {GL_RED, GL_GREEN, GL_BLUE, GL_NONE}, {16, 16, 16, 0}, {GL_FLOAT, GL_FLOAT, GL_FLOAT, GL_NONE}, true, false, true},
{GL_RGB32I, GL_RGB, GL_RGB32I, GL_COMPRESSED_RGB, 3, {GL_RED, GL_GREEN, GL_BLUE, GL_NONE}, {32, 32, 32, 0}, {GL_INT, GL_INT, GL_INT, GL_NONE}, false, false, true},
{GL_RGB32UI, GL_RGB, GL_RGB32UI, GL_COMPRESSED_RGB, 3, {GL_RED, GL_GREEN, GL_BLUE, GL_NONE}, {32, 32, 32, 0}, {GL_UNSIGNED_INT, GL_UNSIGNED_INT, GL_UNSIGNED_INT, GL_NONE}, false, false, true},
{GL_RGB32F, GL_RGB, GL_RGB32F, GL_COMPRESSED_RGB, 3, {GL_RED, GL_GREEN, GL_BLUE, GL_NONE}, {32, 32, 32, 0}, {GL_FLOAT, GL_FLOAT, GL_FLOAT, GL_NONE}, true, false, true},
{GL_RGBA, GL_RGBA, GL_RGBA, GL_COMPRESSED_RGBA, 4, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA}, {-1, -1, -1, -1}, {GL_NONE, GL_NONE, GL_NONE, GL_NONE}, false, false, true},
{GL_BGRA, GL_BGRA, GL_BGRA, GL_COMPRESSED_RGBA, 4, {GL_BLUE, GL_GREEN, GL_RED, GL_ALPHA}, {-1, -1, -1, -1}, {GL_NONE, GL_NONE, GL_NONE, GL_NONE}, false, false, true},
{GL_RGBA_SNORM,	GL_RGBA, GL_RGBA_SNORM, GL_COMPRESSED_RGBA, 4, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA}, {-1, -1, -1, -1}, {GL_NONE, GL_NONE, GL_NONE, GL_NONE}, false, false, true},
{GL_SRGB_ALPHA, GL_RGBA, GL_SRGB_ALPHA, GL_COMPRESSED_SRGB_ALPHA,4, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA}, {-1, -1, -1, -1}, {GL_NONE, GL_NONE, GL_NONE, GL_NONE}, false, false, true},
{GL_RGBA4, GL_RGBA, GL_RGBA4, GL_COMPRESSED_RGBA, 4, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA}, {4, 4, 4, 4}, {GL_NONE, GL_NONE, GL_NONE, GL_NONE}, false, false, false},
{GL_RGBA8, GL_RGBA, GL_RGBA8, GL_COMPRESSED_RGBA, 4, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA}, {8, 8, 8, 8}, {GL_BYTE, GL_BYTE, GL_BYTE, GL_BYTE}, false, false, true},
{GL_RGBA8I, GL_RGBA, GL_RGBA8I, GL_COMPRESSED_RGBA, 4, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA}, {8, 8, 8, 8}, {GL_BYTE, GL_BYTE, GL_BYTE, GL_BYTE}, false, false, true},
{GL_RGBA8UI, GL_RGBA, GL_RGBA8UI, GL_COMPRESSED_RGBA, 4, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA}, {8, 8, 8, 8}, {GL_UNSIGNED_BYTE, GL_UNSIGNED_BYTE, GL_UNSIGNED_BYTE, GL_UNSIGNED_BYTE}, false, false, true},
{GL_RGBA8_SNORM, GL_RGBA, GL_RGBA8_SNORM, GL_COMPRESSED_RGBA, 4, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA}, {8, 8, 8, 8}, {GL_BYTE, GL_BYTE, GL_BYTE, GL_BYTE}, false, false, true},
{GL_SRGB8_ALPHA8, GL_RGBA, GL_SRGB8_ALPHA8, GL_COMPRESSED_SRGB_ALPHA, 4, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA}, {8, 8, 8, 8}, {GL_BYTE, GL_BYTE, GL_BYTE, GL_BYTE}, false, false, true},
{GL_RGBA16, GL_RGBA, GL_RGBA16, GL_COMPRESSED_RGBA, 4, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA}, {16, 16, 16, 16}, {GL_SHORT, GL_SHORT, GL_SHORT, GL_SHORT}, false, false, true},
{GL_RGBA16I, GL_RGBA, GL_RGBA16I, GL_COMPRESSED_RGBA, 4, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA}, {16, 16, 16, 16}, {GL_SHORT, GL_SHORT, GL_SHORT, GL_SHORT}, false, false, true},
{GL_RGBA16UI, GL_RGBA, GL_RGBA16UI, GL_COMPRESSED_RGBA, 4, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA}, {16, 16, 16, 16}, {GL_UNSIGNED_SHORT, GL_UNSIGNED_SHORT, GL_UNSIGNED_SHORT, GL_UNSIGNED_SHORT}, false, false, true},
{GL_RGBA16F, GL_RGBA, GL_RGBA16F, GL_COMPRESSED_RGBA, 4, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA}, {16, 16, 16, 16}, {GL_FLOAT, GL_FLOAT, GL_FLOAT, GL_FLOAT}, true, false, true},
{GL_RGBA16_SNORM, GL_RGBA, GL_RGBA16_SNORM, GL_COMPRESSED_RGBA, 4, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA}, {16, 16, 16, 16}, {GL_SHORT, GL_SHORT, GL_SHORT, GL_SHORT}, false, false, true},
{GL_RGBA32I, GL_RGBA, GL_RGBA32I, GL_COMPRESSED_RGBA, 4, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA}, {32, 32, 32, 32}, {GL_INT, GL_INT, GL_INT, GL_INT}, false, false, true},
{GL_RGBA32UI, GL_RGBA, GL_RGBA32UI, GL_COMPRESSED_RGBA, 4, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA}, {32, 32, 32, 32}, {GL_UNSIGNED_INT, GL_UNSIGNED_INT, GL_UNSIGNED_INT, GL_NONE}, false, false, true},
{GL_RGBA32F, GL_RGBA, GL_RGBA32F, GL_COMPRESSED_RGBA, 4, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA}, {32, 32, 32, 32}, {GL_FLOAT, GL_FLOAT, GL_FLOAT, GL_FLOAT}, true, false, true},

{GL_COMPRESSED_RED, GL_COMPRESSED_RED, GL_RED, GL_COMPRESSED_RED, 1, {GL_RED, GL_NONE, GL_NONE, GL_NONE}, {-1, -1, -1, -1}, {GL_NONE, GL_NONE, GL_NONE, GL_NONE}, false, true, true},
{GL_COMPRESSED_RED_RGTC1, GL_COMPRESSED_RED, GL_RED, GL_COMPRESSED_RED_RGTC1, 1, {GL_RED, GL_NONE, GL_NONE, GL_NONE}, {-1, -1, -1, -1}, {GL_NONE, GL_NONE, GL_NONE, GL_NONE}, false, true, true},
{GL_COMPRESSED_ALPHA, GL_COMPRESSED_ALPHA, GL_ALPHA, GL_COMPRESSED_ALPHA, 1, {GL_ALPHA, GL_NONE, GL_NONE, GL_NONE}, {-1, -1, -1, -1}, {GL_NONE, GL_NONE, GL_NONE, GL_NONE}, false, true, true},
{GL_COMPRESSED_INTENSITY, GL_COMPRESSED_INTENSITY, GL_INTENSITY, GL_COMPRESSED_INTENSITY, 1, {GL_INTENSITY, GL_NONE, GL_NONE, GL_NONE}, {-1, -1, -1, -1}, {GL_NONE, GL_NONE, GL_NONE, GL_NONE}, false, true, true},
{GL_COMPRESSED_LUMINANCE, GL_COMPRESSED_LUMINANCE, GL_LUMINANCE, GL_COMPRESSED_LUMINANCE, 1, {GL_LUMINANCE, GL_NONE, GL_NONE, GL_NONE}, {-1, -1, -1, -1}, {GL_NONE, GL_NONE, GL_NONE, GL_NONE}, false, true, true},
{GL_COMPRESSED_RG, GL_COMPRESSED_RG, GL_RG, GL_COMPRESSED_RG, 2, {GL_RED, GL_GREEN, GL_NONE, GL_NONE}, {-1, -1, -1, -1}, {GL_NONE, GL_NONE, GL_NONE, GL_NONE}, false, true, true},
{GL_COMPRESSED_RG_RGTC2, GL_COMPRESSED_RG, GL_RG, GL_COMPRESSED_RG_RGTC2, 2, {GL_RED, GL_GREEN, GL_NONE, GL_NONE}, {-1, -1, -1, -1}, {GL_NONE, GL_NONE, GL_NONE, GL_NONE}, false, true, true},
{GL_COMPRESSED_LUMINANCE_ALPHA,	GL_COMPRESSED_LUMINANCE_ALPHA, GL_LUMINANCE_ALPHA, GL_COMPRESSED_LUMINANCE_ALPHA, 2, {GL_LUMINANCE, GL_ALPHA, GL_NONE, GL_NONE}, {-1, -1, -1, -1}, {GL_NONE, GL_NONE, GL_NONE, GL_NONE}, false, true, true},
{GL_COMPRESSED_RGB, GL_COMPRESSED_RGB, GL_RGB, GL_COMPRESSED_RGB, 3, {GL_RED, GL_GREEN, GL_BLUE, GL_NONE}, {-1, -1, -1, -1}, {GL_NONE, GL_NONE, GL_NONE, GL_NONE}, false, true, true},
{GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB, GL_COMPRESSED_RGB, GL_RGB, GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB, 3, {GL_RED, GL_GREEN, GL_BLUE, GL_NONE}, {-1, -1, -1, -1}, {GL_NONE, GL_NONE, GL_NONE, GL_NONE}, false, true, true},
{GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB, GL_COMPRESSED_RGB, GL_RGB, GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB, 3, {GL_RED, GL_GREEN, GL_BLUE, GL_NONE}, {-1, -1, -1, -1}, {GL_NONE, GL_NONE, GL_NONE, GL_NONE}, false, true, true},
{GL_COMPRESSED_RGB_FXT1_3DFX, GL_COMPRESSED_RGB, GL_RGB, GL_COMPRESSED_RGB_FXT1_3DFX, 3, {GL_RED, GL_GREEN, GL_BLUE, GL_NONE}, {-1, -1, -1, -1}, {GL_NONE, GL_NONE, GL_NONE, GL_NONE}, false, true, true},
{GL_COMPRESSED_RGB_S3TC_DXT1_EXT, GL_COMPRESSED_RGB, GL_RGB, GL_COMPRESSED_RGB_S3TC_DXT1_EXT, 3, {GL_RED, GL_GREEN, GL_BLUE, GL_NONE}, {-1, -1, -1, -1}, {GL_NONE, GL_NONE, GL_NONE, GL_NONE}, false, true, true},
{GL_COMPRESSED_SRGB, GL_COMPRESSED_SRGB, GL_RGB, GL_COMPRESSED_SRGB, 3, {GL_RED, GL_GREEN, GL_BLUE, GL_NONE}, {-1, -1, -1, -1}, {GL_NONE, GL_NONE, GL_NONE, GL_NONE}, false, true, true},
{GL_COMPRESSED_SRGB_S3TC_DXT1_EXT, GL_COMPRESSED_SRGB, GL_RGB, GL_COMPRESSED_SRGB_S3TC_DXT1_EXT, 3, {GL_RED, GL_GREEN, GL_BLUE, GL_NONE}, {-1, -1, -1, -1}, {GL_NONE, GL_NONE, GL_NONE, GL_NONE}, false, true, true},
{GL_COMPRESSED_RGBA, GL_COMPRESSED_RGBA, GL_RGBA, GL_COMPRESSED_RGBA, 4,  {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA}, {-1, -1, -1, -1}, {GL_NONE, GL_NONE, GL_NONE, GL_NONE}, false, true, true},
{GL_COMPRESSED_RGBA_BPTC_UNORM_ARB, GL_COMPRESSED_RGBA, GL_RGBA, GL_COMPRESSED_RGBA_BPTC_UNORM_ARB, 4, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA}, {-1, -1, -1, -1}, {GL_NONE, GL_NONE, GL_NONE, GL_NONE}, false, true, true},
{GL_COMPRESSED_RGBA_FXT1_3DFX, GL_COMPRESSED_RGBA, GL_RGBA, GL_COMPRESSED_RGBA_FXT1_3DFX, 4, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA}, {-1, -1, -1, -1}, {GL_NONE, GL_NONE, GL_NONE, GL_NONE}, false, true, true},
{GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, GL_COMPRESSED_RGBA, GL_RGBA, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, 4, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA}, {-1, -1, -1, -1}, {GL_NONE, GL_NONE, GL_NONE, GL_NONE}, false, true, true},
{GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, GL_COMPRESSED_RGBA, GL_RGBA, GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, 4, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA}, {-1, -1, -1, -1}, {GL_NONE, GL_NONE, GL_NONE, GL_NONE}, false, true, true},
{GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, GL_COMPRESSED_RGBA, GL_RGBA,	GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, 4, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA}, {-1, -1, -1, -1}, {GL_NONE, GL_NONE, GL_NONE, GL_NONE}, false, true, true},
{GL_COMPRESSED_SRGB_ALPHA, GL_COMPRESSED_SRGB_ALPHA, GL_SRGB_ALPHA, GL_COMPRESSED_SRGB_ALPHA, 4, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA}, {-1, -1, -1, -1}, {GL_NONE, GL_NONE, GL_NONE, GL_NONE}, false, true, true},
{GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB, GL_COMPRESSED_SRGB_ALPHA, GL_SRGB_ALPHA, GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB, 4, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA}, {-1, -1, -1, -1}, {GL_NONE, GL_NONE, GL_NONE, GL_NONE}, false, true, true},
{GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT, GL_COMPRESSED_SRGB_ALPHA, GL_SRGB_ALPHA, GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT, 4, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA}, {-1, -1, -1, -1}, {GL_NONE, GL_NONE, GL_NONE, GL_NONE}, false, true, true},
{GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT, GL_COMPRESSED_SRGB_ALPHA, GL_SRGB_ALPHA, GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT, 4, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA}, {-1, -1, -1, -1}, {GL_NONE, GL_NONE, GL_NONE, GL_NONE}, false, true, true},
{GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT, GL_COMPRESSED_SRGB_ALPHA, GL_SRGB_ALPHA, GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT, 4, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA}, {-1, -1, -1, -1}, {GL_NONE, GL_NONE, GL_NONE, GL_NONE}, false, true, true},

{GL_NONE, GL_NONE, GL_NONE, GL_NONE, 0, {GL_NONE, GL_NONE, GL_NONE, GL_NONE}, {-1, -1, -1, -1}, {GL_NONE, GL_NONE, GL_NONE, GL_NONE}, false, false, false}
};

// HdlTextureFormatDescriptor :
	/**
	\fn bool HdlTextureFormatDescriptor::isDepthValid(GLenum depth) const
	\brief Test if a depth is valid for the current format.
	\param depth The depth to be tested.
	\return True if the depth can be associated to this format.
	**/
	bool HdlTextureFormatDescriptor::isDepthValid(GLenum depth) const
	{
		return 	(channelsDepth[0]==depth || channelsDepth[0]==GL_NONE) && 
			(channelsDepth[1]==depth || channelsDepth[1]==GL_NONE) && 	
			(channelsDepth[2]==depth || channelsDepth[2]==GL_NONE) && 
			(channelsDepth[3]==depth || channelsDepth[3]==GL_NONE);
	}

	/**
	\fn bool HdlTextureFormatDescriptor::hasChannel(GLenum channel) const
	\brief Returns true if the format has the required channel, false otherwise or raise an exception if the channel is unknown.
	\param channel The channel name (GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA or GL_LUMINANCE).
	\return True if the format has the channel.
	**/
	bool HdlTextureFormatDescriptor::hasChannel(GLenum channel) const
	{
		return (channels[0]==channel || channels[1]==channel || channels[2]==channel || channels[3]==channel);
	}

	/**
	\fn int HdlTextureFormatDescriptor::getChannelIndex(GLenum channel) const
	\brief Get the index of a particular channel.
	\param channel The channel name (GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA or GL_LUMINANCE).
	\return The channel index or -1 if not found.
	**/
	int HdlTextureFormatDescriptor::getChannelIndex(GLenum channel) const
	{
		if(channels[0]==channel) return 0;
		if(channels[1]==channel) return 1;
		if(channels[2]==channel) return 2;
		if(channels[3]==channel) return 3;
		return -1;
	}

	/**
	\fn int HdlTextureFormatDescriptor::getChannelOffsetInBits(int channelIndex, GLenum depth) const
	\brief Get the channel offset/position.
	\param channelIndex The index of the channel
	\param depth The depth associated with the current format.
	\return The channel offset in bits or -1 if the channel does not exist.
	**/
	int HdlTextureFormatDescriptor::getChannelOffsetInBits(int channelIndex, GLenum depth) const
	{
		if(channelIndex<0 && channelIndex>=numChannels)
			return -1;
		else
		{
			const int d = getTypeSizeInBits(depth);
			int r = 0;
			for(int k=0; k<channelIndex; k++)
				r += (channelsSizeInBits[k]<0) ? d : channelsSizeInBits[k];
			return r;
		}
	}

	/**
	\fn int HdlTextureFormatDescriptor::getChannelOffset(int channelIndex, GLenum depth) const
	\brief Get the channel offset/position.
	\param channelIndex The index of the channel
	\param depth The depth associated with the current format.
	\return The channel offset in bytes or -1 if the channel does not exist.
	**/
	int HdlTextureFormatDescriptor::getChannelOffset(int channelIndex, GLenum depth) const
	{
		return (getChannelOffsetInBits(channelIndex, depth)+sizeof(char)*8-1)/(sizeof(char)*8);
	}

	/**
	\fn int HdlTextureFormatDescriptor::getChannelSizeInBits(int channelIndex, GLenum depth) const
	\brief Get the size of a channel.
	\param channelIndex The index of the channel
	\param depth The depth associated with the current format.
	\return The channel size in bits or 0 if the channel does not exist.
	**/
	int HdlTextureFormatDescriptor::getChannelSizeInBits(int channelIndex, GLenum depth) const
	{
		if(channelIndex<0 || channelIndex>=numChannels)
			return 0;
		else
			return (channelsSizeInBits[channelIndex]<0) ? getTypeSizeInBits(depth) : channelsSizeInBits[channelIndex];
	}
	
	/**
	\fn int HdlTextureFormatDescriptor::getChannelSize(int channelIndex, GLenum depth) const
	\brief Get the size of a channel.
	\param channelIndex The index of the channel
	\param depth The depth associated with the current format.
	\return The channel size in bytes or 0 if the channel does not exist.
	**/
	int HdlTextureFormatDescriptor::getChannelSize(int channelIndex, GLenum depth) const
	{
		return (getChannelSizeInBits(channelIndex, depth)+sizeof(char)*8-1)/(sizeof(char)*8);
	}

	/**
	\fn int HdlTextureFormatDescriptor::getPixelSizeInBits(GLenum depth) const
	\brief Get the pixel size.
	\param depth The depth associated with the current format.
	\return The pixel size in bits.
	**/
	int HdlTextureFormatDescriptor::getPixelSizeInBits(GLenum depth) const
	{
		const int d = getTypeSizeInBits(depth);
		int s = 0;
		s += (channelsSizeInBits[0]<0) ? d : channelsSizeInBits[0];
		s += (channelsSizeInBits[1]<0) ? d : channelsSizeInBits[1];
		s += (channelsSizeInBits[2]<0) ? d : channelsSizeInBits[2];
		s += (channelsSizeInBits[3]<0) ? d : channelsSizeInBits[3];
		return s;
	}

	/**
	\fn int HdlTextureFormatDescriptor::getPixelSize(GLenum depth) const
	\brief Get the pixel size.
	\param depth The depth associated with the current format.
	\return The pixel size in bytes.
	**/
	int HdlTextureFormatDescriptor::getPixelSize(GLenum depth) const
	{
		return (getPixelSizeInBits(depth)+sizeof(char)*8-1)/(sizeof(char)*8);
	}

	/**
	\fn int HdlTextureFormatDescriptor::getTypeSizeInBits(GLenum depth)
	\brief Return the size of the type associated with depth.
	\param depth The depth, described as its enumerator value (e.g. GL_BYTE for GLbyte).
	\return Size of the type, in bits.
	**/
	int HdlTextureFormatDescriptor::getTypeSizeInBits(GLenum depth)
	{
		switch(depth)
		{
			#define TMP_SIZE(X, Type)	case X : return sizeof(Type)*8;
				TMP_SIZE(GL_BYTE,		GLbyte )
				TMP_SIZE(GL_UNSIGNED_BYTE,	GLubyte )
				TMP_SIZE(GL_SHORT,		GLshort )
				TMP_SIZE(GL_UNSIGNED_SHORT,	GLushort )
				TMP_SIZE(GL_INT,		GLint )
				TMP_SIZE(GL_UNSIGNED_INT,	GLuint )
				TMP_SIZE(GL_FLOAT,		GLfloat )
				TMP_SIZE(GL_DOUBLE,		GLdouble )
			#undef TMP_SIZE
			default :
				throw Exception("HdlTextureFormatDescriptorsList::getTypeSize - cannot recognize color channel depth " + getGLEnumNameSafe(depth), __FILE__, __LINE__, Exception::GLException);
		}
	}

	/**
	\fn int HdlTextureFormatDescriptor::getTypeSize(GLenum depth)
	\brief Return the size of the type associated with depth.
	\param depth The depth, described as its enumerator value (e.g. GL_BYTE for GLbyte).
	\return Size of the type, in bytes.
	**/
	int HdlTextureFormatDescriptor::getTypeSize(GLenum depth)
	{
		switch(depth)
		{
			#define TMP_SIZE(X, Type)	case X : return sizeof(Type);
				TMP_SIZE(GL_BYTE,		GLbyte )
				TMP_SIZE(GL_UNSIGNED_BYTE,	GLubyte )
				TMP_SIZE(GL_SHORT,		GLshort )
				TMP_SIZE(GL_UNSIGNED_SHORT,	GLushort )
				TMP_SIZE(GL_INT,		GLint )
				TMP_SIZE(GL_UNSIGNED_INT,	GLuint )
				TMP_SIZE(GL_FLOAT,		GLfloat )
				TMP_SIZE(GL_DOUBLE,		GLdouble )
			#undef TMP_SIZE
			default :
				throw Exception("HdlTextureFormatDescriptorsList::getTypeSize - cannot recognize color channel depth " + getGLEnumNameSafe(depth), __FILE__, __LINE__, Exception::GLException);
		}
	}

	/**
	\fn void HdlTextureFormatDescriptor::getShuffle(const HdlTextureFormatDescriptor& dst, const HdlTextureFormatDescriptor& src, char* shuffleIndex, const int length)
	\brief Construct the shuffling pattern between two format.
	\param dst The destination format.
	\param src The source format.
	\param shuffleIndex Pointer to a buffer which will contain the shuffling pattern.
	\param length Length of the given buffer (dst.numChannels is sufficient).

	This function constructs an array containing the index of the channels in the source from the point of view of the destination (or -1 if a channel does not exist in the source).
	**/
	void HdlTextureFormatDescriptor::getShuffle(const HdlTextureFormatDescriptor& dst, const HdlTextureFormatDescriptor& src, char* shuffleIndex, const int length)
	{
		char buffer[maxNumChannels];
		for(int k=0; k<dst.numChannels; k++)
			buffer[k] = src.getChannelIndex(dst.channels[k]);
		// Copy to the maximum length : 
		std::memcpy(shuffleIndex, buffer, std::min(dst.numChannels, length));
	}

	/**
	\fn int HdlTextureFormatDescriptor::getBitShuffle(const HdlTextureFormatDescriptor& dst, const GLenum& dstDepth, const HdlTextureFormatDescriptor& src, const GLenum& srcDepth, char* shuffleBitIndex, const int length, bool* isBlack)
	\brief Construct the byte shuffling pattern between two format.
	\param dst The destination format.
	\param dstDepth The depth associated with the destination format.	
	\param src The source format.
	\param srcDepth The depth associated with the source format.
	\param shuffleBitIndex Pointer to a buffer which will contain the bit shuffling pattern.
	\param length Length of the given buffer (dst.getPixelSize() is sufficient).
	\param isBlack If not null, will contain a boolean asserting that there is not correspondence between the source and the destination.
	\return The length of the pattern actually written.

	This function constructs an array containing the bit index of the channels in the source from the point of view of the destination (or -1 if a channel does not exist in the source). To be used with HdlTextureFormatDescriptor::applyBitShuffle. It will raise an exception if either or both the source and the destination are floating point formats.
	**/
	int HdlTextureFormatDescriptor::getBitShuffle(const HdlTextureFormatDescriptor& dst, const GLenum& dstDepth, const HdlTextureFormatDescriptor& src, const GLenum& srcDepth, char* shuffleBitIndex, const int length, bool* isBlack)
	{
		if(dst.isFloatingPoint || src.isFloatingPoint)
			throw Exception("HdlTextureFormatDescriptor::getBitShuffle - Cannot construct bits shuffle pattern for floatting point values.", __FILE__, __LINE__, Exception::CoreException);
		
		const unsigned int v = 0xFF000000;
		const bool isLittleEndian = (*reinterpret_cast<const char*>(&v)==0);

		const int maxLength = maxPixelSize;
		char buffer[maxLength];

		int offset = 0;
		for(int k=0; k<dst.numChannels; k++)
		{
			const int l = src.getChannelIndex(dst.channels[k]);
			const int dstSize = dst.getChannelSize(k, dstDepth);

			if(l<0) // src does not have the channel
				std::memset(buffer+offset, -1, dstSize);
			else
			{
				const int srcSize = src.getChannelSize(l, srcDepth),
					  srcOffset = src.getChannelOffset(l, srcDepth);

				for(int p=0; p<std::min(srcSize, dstSize); p++)
				{
					if(isLittleEndian)
						buffer[offset + dstSize - p - 1] = srcOffset + srcSize - p - 1;
					else // big endian 
						buffer[offset + p] = srcOffset + p;
				}
			}

			offset += dstSize;
		}

		// If needed, test if there is no correspondance :
		if(isBlack!=NULL)
		{
			(*isBlack) = true;
			for(int k=0; k<offset; k++)
				(*isBlack) = (*isBlack) && (buffer[k]<0);
		}

		// Copy to the maximum length : 
		offset = std::min(std::min(offset, maxLength), length);
		std::memcpy(shuffleBitIndex, buffer, offset);

		#ifdef __GLIPLIB_DEVELOPMENT_VERBOSE__
			std::cout << "Shuffle pattern : " << getGLEnumNameSafe(src.mode) << ':' << getGLEnumNameSafe(srcDepth) << " to " << getGLEnumNameSafe(dst.mode) << ':' << getGLEnumNameSafe(dstDepth) << " (offset : " << offset << ')' << std::endl;
			for(int k=0; k<offset; k++)
				std::cout << static_cast<int>(shuffleBitIndex[k]) << ", ";
			std::cout << std::endl;
		#endif

		// The length of the pattern : 
		return offset;
	}

	/**
	\fn void HdlTextureFormatDescriptor::applyBitShuffle(char* dst, const char* src, const char* shuffleBitIndex, const int length)
	\brief Apply the bit shuffling pattern from the source to the destination.
	\param dst Destination pixel address.
	\param src Source pixel address.
	\param shuffleBitIndex Bit shuffle pattern, see HdlTextureFormatDescriptor::getBitShuffle.
	\param length Length of the shuffling pattern, usually returned by HdlTextureFormatDescriptor::getBitShuffle.
	**/
	void HdlTextureFormatDescriptor::applyBitShuffle(char* dst, const char* src, const char* shuffleBitIndex, const int length)
	{
		for(int k=0; k<length; k++)
		{
			if(shuffleBitIndex[k]>=0)
				dst[k] = src[static_cast<int>(shuffleBitIndex[k])];
			else
				dst[k] = 0;
		}
	}

// HdlTextureFormatDescriptorList :
	/**
	\fn int HdlTextureFormatDescriptorsList::getNumDescriptors(void)
	\brief Returns the number of known GL modes for texture formats (GL_RGB, GL_RGBA, ...).
	\return Returns the number of known GL modes.
	**/
	int HdlTextureFormatDescriptorsList::getNumDescriptors(void)
	{
		static int i = 0;

		if(i==0)
			for(i=0; textureFormatDescriptors[i].mode!=GL_NONE; i++);

		return i;
	}

	/**
	\fn const HdlTextureFormatDescriptor& HdlTextureFormatDescriptorsList::get(int id)
	\brief Access to a mode descriptor object. Will raise an exception if the index is out of range.
	\param id Index of the desired descriptor.
	\return A constant reference onto a HdlTextureFormatDescriptor object.
	**/
	const HdlTextureFormatDescriptor& HdlTextureFormatDescriptorsList::get(int id)
	{
		return textureFormatDescriptors[id];
	}

	/**
	\fn const HdlTextureFormatDescriptor& HdlTextureFormatDescriptorsList::get(const GLenum& mode)
	\brief Access to a mode descriptor object. Will raise an exception if the mode is unknown.
	\param mode Searched mode name.
	\return A constant reference onto a HdlTextureFormatDescriptor object.
	**/
	const HdlTextureFormatDescriptor& HdlTextureFormatDescriptorsList::get(const GLenum& mode)
	{
		for(int i=0; textureFormatDescriptors[i].mode!=GL_NONE; i++)
			if(mode==textureFormatDescriptors[i].mode && mode!=GL_NONE)
				return textureFormatDescriptors[i];

		//else
			throw Exception("HdlTextureFormatDescriptorsList::get - No corresponding mode for : " + getGLEnumNameSafe(mode) + ".", __FILE__, __LINE__, Exception::GLException);
	}

