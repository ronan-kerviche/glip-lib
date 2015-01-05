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

#include "Core/HdlTextureTools.hpp"
#include "Core/Exception.hpp"

using namespace Glip::CoreGL;

// List of all the Mode accepted :
const HdlTextureFormatDescriptor HdlTextureFormatDescriptorsList::textureFormatDescriptors[] =
{
	// name	                     	red     green   blue    alpha   lum.  compress.	float	alias			corresponding       	      Forced Depth           TSBytes  TSbits   RedSb  GreenSb BlueSb  AlphaSb LumSb	redType			greenType		blueType		alphaType		luminanceType
	{GL_RED, 			0, 	-1, 	-1, 	-1, 	-1, 	false,	false,	GL_RED,			GL_COMPRESSED_RED, 		GL_NONE, 		0, 	0, 	0, 	0, 	0, 	0, 	0,	GL_NONE,		GL_NONE,		GL_NONE,		GL_NONE,		GL_NONE},
	{GL_GREEN, 			-1,	0, 	-1,	-1,	-1,	false,	false,	GL_RED,			GL_COMPRESSED_RED, 		GL_NONE,		0, 	0, 	0, 	0, 	0, 	0, 	0,	GL_NONE,		GL_NONE,		GL_NONE,		GL_NONE,		GL_NONE},
	{GL_BLUE, 			-1,	-1, 	0,	-1,	-1,	false,	false,	GL_RED,			GL_COMPRESSED_RED, 		GL_NONE,		0, 	0, 	0, 	0, 	0, 	0, 	0,	GL_NONE,		GL_NONE,		GL_NONE,		GL_NONE,		GL_NONE},
	{GL_R8,				0,	-1, 	-1,	-1,	-1,	false,	false,	GL_RED,			GL_COMPRESSED_RED, 		GL_BYTE,		1, 	8, 	8, 	0, 	0, 	0, 	0,	GL_BYTE,		GL_NONE,		GL_NONE,		GL_NONE,		GL_NONE},
	{GL_R8I,			0,	-1, 	-1,	-1,	-1,	false,	false,	GL_RED,			GL_COMPRESSED_RED, 		GL_BYTE,		1, 	8, 	8, 	0, 	0, 	0, 	0,	GL_BYTE,		GL_NONE,		GL_NONE,		GL_NONE,		GL_NONE},
	{GL_R8UI,			0,	-1, 	-1,	-1,	-1,	false,	false,	GL_RED,			GL_COMPRESSED_RED, 		GL_BYTE,		1, 	8, 	8, 	0, 	0, 	0, 	0,	GL_BYTE,		GL_NONE,		GL_NONE,		GL_NONE,		GL_NONE},
	{GL_R8_SNORM,			0,	-1, 	-1,	-1,	-1,	false,	false,	GL_RED,			GL_COMPRESSED_RED, 		GL_BYTE,		1, 	8, 	8, 	0, 	0, 	0, 	0,	GL_BYTE,		GL_NONE,		GL_NONE,		GL_NONE,		GL_NONE},
	{GL_R16,			0,	-1, 	-1,	-1,	-1,	false,	false,	GL_RED,			GL_COMPRESSED_RED, 		GL_SHORT,		2, 	16, 	16, 	0, 	0, 	0, 	0,	GL_SHORT,		GL_NONE,		GL_NONE,		GL_NONE,		GL_NONE},
	{GL_R16I,			0,	-1, 	-1,	-1,	-1,	false,	false,	GL_RED,			GL_COMPRESSED_RED, 		GL_SHORT,		2, 	16, 	16, 	0, 	0, 	0, 	0,	GL_SHORT,		GL_NONE,		GL_NONE,		GL_NONE,		GL_NONE},
	{GL_R16UI,			0,	-1, 	-1,	-1,	-1,	false,	false,	GL_RED,			GL_COMPRESSED_RED, 		GL_SHORT,		2, 	16, 	16, 	0, 	0, 	0, 	0,	GL_SHORT,		GL_NONE,		GL_NONE,		GL_NONE,		GL_NONE},
	{GL_R16_SNORM,			0,	-1, 	-1,	-1,	-1,	false,	false,	GL_RED,			GL_COMPRESSED_RED, 		GL_SHORT,		2, 	16, 	16, 	0, 	0, 	0, 	0,	GL_SHORT,		GL_NONE,		GL_NONE,		GL_NONE,		GL_NONE},
	{GL_R16F,			0,	-1, 	-1,	-1,	-1,	false,	true,	GL_RED,			GL_COMPRESSED_RED, 		GL_FLOAT,		2, 	16, 	16, 	0, 	0, 	0, 	0,	GL_FLOAT,		GL_NONE,		GL_NONE,		GL_NONE,		GL_NONE},
	{GL_R32I,			0,	-1, 	-1,	-1,	-1,	false,	false,	GL_RED,			GL_COMPRESSED_RED, 		GL_INT,			4, 	32, 	32, 	0, 	0, 	0, 	0,	GL_INT,			GL_NONE,		GL_NONE,		GL_NONE,		GL_NONE},
	{GL_R32UI,			0,	-1, 	-1,	-1,	-1,	false,	false,	GL_RED,			GL_COMPRESSED_RED, 		GL_UNSIGNED_INT,	4, 	32, 	32, 	0, 	0, 	0, 	0,	GL_UNSIGNED_INT,	GL_NONE,		GL_NONE,		GL_NONE,		GL_NONE},
	{GL_R32F,			0,	-1, 	-1,	-1,	-1,	false,	true,	GL_RED,			GL_COMPRESSED_RED, 		GL_FLOAT,		4, 	32, 	32, 	0, 	0, 	0, 	0,	GL_FLOAT,		GL_NONE,		GL_NONE,		GL_NONE,		GL_NONE},
	{GL_ALPHA,			-1,	-1, 	-1,	0,	-1,	false,	false,	GL_ALPHA,		GL_COMPRESSED_ALPHA, 		GL_NONE,		0, 	0, 	0, 	0, 	0, 	0, 	0,	GL_NONE,		GL_NONE,		GL_NONE,		GL_NONE,		GL_NONE},
	{GL_ALPHA4,			-1,	-1, 	-1,	0,	-1,	false,	false,	GL_ALPHA,		GL_COMPRESSED_ALPHA, 		GL_NONE,		1, 	4, 	0, 	0, 	0, 	4, 	0,	GL_NONE,		GL_NONE,		GL_NONE,		GL_BYTE,		GL_NONE},
	{GL_ALPHA8,			-1,	-1, 	-1,	0,	-1,	false,	false,	GL_ALPHA,		GL_COMPRESSED_ALPHA, 		GL_BYTE,		1, 	8, 	0, 	0, 	0, 	8, 	0,	GL_NONE,		GL_NONE,		GL_NONE,		GL_BYTE,		GL_NONE},
	{GL_ALPHA8_SNORM,		-1,	-1, 	-1,	0,	-1,	false,	false,	GL_ALPHA,		GL_COMPRESSED_ALPHA, 		GL_BYTE,		1, 	8, 	0, 	0, 	0, 	8, 	0,	GL_NONE,		GL_NONE,		GL_NONE,		GL_BYTE,		GL_NONE},
	{GL_ALPHA12,			-1,	-1, 	-1,	0,	-1,	false,	false,	GL_ALPHA,		GL_COMPRESSED_ALPHA, 		GL_NONE,		2, 	12, 	0, 	0, 	0, 	12, 	0,	GL_NONE,		GL_NONE,		GL_NONE,		GL_SHORT,		GL_NONE},
	{GL_ALPHA16,			-1,	-1, 	-1,	0,	-1,	false,	false,	GL_ALPHA,		GL_COMPRESSED_ALPHA, 		GL_SHORT,		2, 	16, 	0, 	0, 	0, 	16, 	0,	GL_NONE,		GL_NONE,		GL_NONE,		GL_SHORT,		GL_NONE},
	{GL_ALPHA16F_ARB,		-1,	-1, 	-1,	0,	-1,	false,	true,	GL_ALPHA,		GL_COMPRESSED_ALPHA, 		GL_FLOAT,		2, 	16, 	0, 	0, 	0, 	16, 	0,	GL_NONE,		GL_NONE,		GL_NONE,		GL_FLOAT,		GL_NONE},
	{GL_ALPHA16_SNORM,		-1,	-1, 	-1,	0,	-1,	false,	false,	GL_ALPHA,		GL_COMPRESSED_ALPHA, 		GL_SHORT,		2, 	16, 	0, 	0, 	0, 	16, 	0,	GL_NONE,		GL_NONE,		GL_NONE,		GL_SHORT,		GL_NONE},
	{GL_ALPHA32F_ARB,		-1,	-1, 	-1,	0,	-1,	false,	false,	GL_ALPHA,		GL_COMPRESSED_ALPHA, 		GL_FLOAT,		4, 	32, 	0, 	0, 	0, 	32, 	0,	GL_NONE,		GL_NONE,		GL_NONE,		GL_FLOAT,		GL_NONE},
	{GL_LUMINANCE,			-1,	-1, 	-1,	-1,	0,	false,	false,	GL_LUMINANCE,		GL_COMPRESSED_LUMINANCE, 	GL_NONE,		0, 	0, 	0, 	0, 	0, 	0, 	0,	GL_NONE,		GL_NONE,		GL_NONE,		GL_NONE,		GL_NONE},
	{GL_LUMINANCE4,			-1,	-1, 	-1,	-1,	0,	false,	false,	GL_LUMINANCE,		GL_COMPRESSED_LUMINANCE, 	GL_NONE,		1, 	4, 	0, 	0, 	0, 	0, 	4,	GL_NONE,		GL_NONE,		GL_NONE,		GL_NONE,		GL_BYTE},
	{GL_LUMINANCE8,			-1,	-1, 	-1,	-1,	0,	false,	false,	GL_LUMINANCE,		GL_COMPRESSED_LUMINANCE, 	GL_BYTE,		1, 	8, 	0, 	0, 	0, 	0, 	8,	GL_NONE,		GL_NONE,		GL_NONE,		GL_NONE,		GL_BYTE},
	{GL_LUMINANCE8_SNORM,		-1,	-1, 	-1,	-1,	0,	false,	false,	GL_LUMINANCE,		GL_COMPRESSED_LUMINANCE, 	GL_BYTE,		1, 	8, 	0, 	0, 	0, 	0, 	8,	GL_NONE,		GL_NONE,		GL_NONE,		GL_NONE,		GL_BYTE},
	{GL_LUMINANCE12,		-1,	-1, 	-1,	-1,	0,	false,	false,	GL_LUMINANCE,		GL_COMPRESSED_LUMINANCE, 	GL_NONE,		2, 	12, 	0, 	0, 	0, 	0, 	12,	GL_NONE,		GL_NONE,		GL_NONE,		GL_NONE,		GL_SHORT},
	{GL_LUMINANCE16,		-1,	-1, 	-1,	-1,	0,	false,	false,	GL_LUMINANCE,		GL_COMPRESSED_LUMINANCE, 	GL_UNSIGNED_SHORT,	2, 	16, 	0, 	0, 	0, 	0, 	16,	GL_NONE,		GL_NONE,		GL_NONE,		GL_NONE,		GL_SHORT},
	{GL_LUMINANCE16F_ARB,		-1,	-1, 	-1,	-1,	0,	false,	true,	GL_LUMINANCE,		GL_COMPRESSED_LUMINANCE, 	GL_FLOAT,		2, 	16, 	0, 	0, 	0, 	0, 	16,	GL_NONE,		GL_NONE,		GL_NONE,		GL_NONE,		GL_FLOAT},
	{GL_LUMINANCE16_SNORM,		-1,	-1, 	-1,	-1,	0,	false,	false,	GL_LUMINANCE,		GL_COMPRESSED_LUMINANCE, 	GL_SHORT,		2, 	16, 	0, 	0, 	0, 	0, 	16,	GL_NONE,		GL_NONE,		GL_NONE,		GL_NONE,		GL_SHORT},
	{GL_LUMINANCE32F_ARB,		-1,	-1, 	-1,	-1,	0,	false,	true,	GL_LUMINANCE,		GL_COMPRESSED_LUMINANCE, 	GL_FLOAT,		4, 	32, 	0, 	0, 	0, 	0, 	32,	GL_NONE,		GL_NONE,		GL_NONE,		GL_NONE,		GL_FLOAT},
	{GL_INTENSITY,			-1,	-1, 	-1,	-1,	0,	false,	false,	GL_INTENSITY,		GL_COMPRESSED_INTENSITY, 	GL_NONE,		0, 	0, 	0, 	0, 	0, 	0, 	0,	GL_NONE,		GL_NONE,		GL_NONE,		GL_NONE,		GL_NONE},
	{GL_INTENSITY4,			-1,	-1, 	-1,	-1,	0,	false,	false,	GL_INTENSITY,		GL_COMPRESSED_INTENSITY, 	GL_NONE,		1, 	4, 	0, 	0, 	0, 	0, 	4,	GL_NONE,		GL_NONE,		GL_NONE,		GL_NONE,		GL_NONE},
	{GL_INTENSITY8,			-1,	-1, 	-1,	-1,	0,	false,	false,	GL_INTENSITY,		GL_COMPRESSED_INTENSITY, 	GL_BYTE,		1, 	8, 	0, 	0, 	0, 	0, 	8,	GL_NONE,		GL_NONE,		GL_NONE,		GL_NONE,		GL_NONE},
	{GL_INTENSITY8_SNORM,		-1,	-1, 	-1,	-1,	0,	false,	false,	GL_INTENSITY,		GL_COMPRESSED_INTENSITY, 	GL_BYTE,		1, 	8, 	0, 	0, 	0, 	0, 	8,	GL_NONE,		GL_NONE,		GL_NONE,		GL_NONE,		GL_NONE},
	{GL_INTENSITY12,		-1,	-1, 	-1,	-1,	0,	false,	false,	GL_INTENSITY,		GL_COMPRESSED_INTENSITY, 	GL_NONE,		2, 	12, 	0, 	0, 	0, 	0, 	12,	GL_NONE,		GL_NONE,		GL_NONE,		GL_NONE,		GL_NONE},
	{GL_INTENSITY16,		-1,	-1, 	-1,	-1,	0,	false,	false,	GL_INTENSITY,		GL_COMPRESSED_INTENSITY, 	GL_SHORT,		2, 	16, 	0, 	0, 	0, 	0, 	16,	GL_NONE,		GL_NONE,		GL_NONE,		GL_NONE,		GL_NONE},
	{GL_INTENSITY16F_ARB,		-1,	-1, 	-1,	-1,	0,	false,	true,	GL_INTENSITY,		GL_COMPRESSED_INTENSITY, 	GL_FLOAT,		2, 	16, 	0, 	0, 	0, 	0, 	16,	GL_NONE,		GL_NONE,		GL_NONE,		GL_NONE,		GL_NONE},
	{GL_INTENSITY16_SNORM,		-1,	-1, 	-1,	-1,	0,	false,	false,	GL_INTENSITY,		GL_COMPRESSED_INTENSITY, 	GL_SHORT,		2, 	16, 	0, 	0, 	0, 	0, 	16,	GL_NONE,		GL_NONE,		GL_NONE,		GL_NONE,		GL_NONE},
	{GL_INTENSITY32F_ARB,		-1,	-1, 	-1,	-1,	0,	false,	true,	GL_INTENSITY,		GL_COMPRESSED_INTENSITY, 	GL_FLOAT,		4, 	32, 	0, 	0, 	0, 	0, 	32,	GL_NONE,		GL_NONE,		GL_NONE,		GL_NONE,		GL_NONE},
	{GL_RG,				0,	1, 	-1,	-1,	-1,	false,	false,	GL_RG,			GL_COMPRESSED_RG, 		GL_NONE,		0, 	0, 	0, 	0, 	0, 	0, 	0,	GL_NONE,		GL_NONE,		GL_NONE,		GL_NONE,		GL_NONE},
	{GL_RG8,			0,	1, 	-1,	-1,	-1,	false,	false,	GL_RG,			GL_COMPRESSED_RG, 		GL_BYTE,		2, 	16, 	8, 	8, 	0, 	0, 	0,	GL_BYTE,		GL_BYTE,		GL_NONE,		GL_NONE,		GL_NONE},
	{GL_RG8I,			0,	1, 	-1,	-1,	-1,	false,	false,	GL_RG,			GL_COMPRESSED_RG, 		GL_BYTE,		2, 	16, 	8, 	8, 	0, 	0, 	0,	GL_BYTE,		GL_BYTE,		GL_NONE,		GL_NONE,		GL_NONE},
	{GL_RG8UI,			0,	1, 	-1,	-1,	-1,	false,	false,	GL_RG,			GL_COMPRESSED_RG, 		GL_UNSIGNED_BYTE,	2, 	16, 	8, 	8, 	0, 	0, 	0,	GL_UNSIGNED_BYTE,	GL_UNSIGNED_BYTE,	GL_NONE,		GL_NONE,		GL_NONE},
	{GL_RG8_SNORM,			0,	1, 	-1,	-1,	-1,	false,	false,	GL_RG,			GL_COMPRESSED_RG, 		GL_BYTE,		2, 	16, 	8, 	8, 	0, 	0, 	0,	GL_BYTE,		GL_BYTE,		GL_NONE,		GL_NONE,		GL_NONE},
	{GL_RG16,			0,	1, 	-1,	-1,	-1,	false,	false,	GL_RG,			GL_COMPRESSED_RG, 		GL_SHORT,		4, 	32, 	16, 	16, 	0, 	0, 	0,	GL_SHORT,		GL_SHORT,		GL_NONE,		GL_NONE,		GL_NONE},
	{GL_RG16I,			0,	1, 	-1,	-1,	-1,	false,	false,	GL_RG,			GL_COMPRESSED_RG, 		GL_SHORT,		4, 	32, 	16, 	16, 	0, 	0, 	0,	GL_SHORT,		GL_SHORT,		GL_NONE,		GL_NONE,		GL_NONE},
	{GL_RG16UI,			0,	1, 	-1,	-1,	-1,	false,	false,	GL_RG,			GL_COMPRESSED_RG, 		GL_UNSIGNED_SHORT,	4, 	32, 	16, 	16, 	0, 	0, 	0,	GL_UNSIGNED_SHORT,	GL_UNSIGNED_SHORT,	GL_NONE,		GL_NONE,		GL_NONE},
	{GL_RG16F,			0,	1, 	-1,	-1,	-1,	false,	false,	GL_RG,			GL_COMPRESSED_RG, 		GL_FLOAT,		4, 	32, 	16, 	16, 	0, 	0, 	0,	GL_FLOAT,		GL_FLOAT,		GL_NONE,		GL_NONE,		GL_NONE},
	{GL_RG16_SNORM,			0,	1, 	-1,	-1,	-1,	false,	false,	GL_RG,			GL_COMPRESSED_RG, 		GL_SHORT,		4, 	32, 	16, 	16, 	0, 	0, 	0,	GL_SHORT,		GL_SHORT,		GL_NONE,		GL_NONE,		GL_NONE},
	{GL_RG32I,			0,	1, 	-1,	-1,	-1,	false,	false,	GL_RG,			GL_COMPRESSED_RG, 		GL_INT,			8, 	64, 	32, 	32, 	0, 	0, 	0,	GL_INT,			GL_INT,			GL_NONE,		GL_NONE,		GL_NONE},
	{GL_RG32UI,			0,	1, 	-1,	-1,	-1,	false,	false,	GL_RG,			GL_COMPRESSED_RG, 		GL_UNSIGNED_INT,	8, 	64, 	32, 	32, 	0, 	0, 	0,	GL_UNSIGNED_INT,	GL_UNSIGNED_INT,	GL_NONE,		GL_NONE,		GL_NONE},
	{GL_RG32F,			0,	1, 	-1,	-1,	-1,	false,	true,	GL_RG,			GL_COMPRESSED_RG, 		GL_FLOAT,		8, 	64, 	32, 	32, 	0, 	0, 	0,	GL_FLOAT,		GL_FLOAT,		GL_NONE,		GL_NONE,		GL_NONE},
	{GL_LUMINANCE_ALPHA,		-1,	-1, 	-1,	1,	0,	false,	false,	GL_LUMINANCE_ALPHA,	GL_COMPRESSED_LUMINANCE_ALPHA, 	GL_NONE,		0, 	0, 	0, 	0, 	0, 	0, 	0,	GL_NONE,		GL_NONE,		GL_NONE,		GL_NONE,		GL_NONE},
	{GL_LUMINANCE4_ALPHA4,		-1,	-1, 	-1,	1,	0,	false,	false,	GL_LUMINANCE_ALPHA,	GL_COMPRESSED_LUMINANCE_ALPHA, 	GL_NONE,		1, 	8, 	0, 	0, 	0, 	4, 	4,	GL_NONE,		GL_NONE,		GL_NONE,		GL_BYTE,		GL_BYTE},
	{GL_LUMINANCE8_ALPHA8,		-1,	-1, 	-1,	1,	0,	false,	false,	GL_LUMINANCE_ALPHA,	GL_COMPRESSED_LUMINANCE_ALPHA, 	GL_BYTE,		2, 	16, 	0, 	0, 	0, 	8, 	8,	GL_NONE,		GL_NONE,		GL_NONE,		GL_BYTE,		GL_BYTE},
	{GL_LUMINANCE8_ALPHA8_SNORM,	-1,	-1, 	-1,	1,	0,	false,	false,	GL_LUMINANCE_ALPHA,	GL_COMPRESSED_LUMINANCE_ALPHA, 	GL_BYTE,		2, 	16, 	0, 	0, 	0, 	8, 	8,	GL_NONE,		GL_NONE,		GL_NONE,		GL_BYTE,		GL_BYTE},
	//{GL_LUMINANCE12_ALPHA4,	-1,	-1, 	-1,	1,	0,	false,	false,	GL_LUMINANCE_ALPHA,	GL_COMPRESSED_LUMINANCE_ALPHA, 	GL_NONE,		2, 	16, 	0, 	0, 	0, 	4, 	12,	GL_NONE,		GL_NONE,		GL_NONE,		GL_SHORT,		GL_SHORT},
	//{GL_LUMINANCE12_ALPHA12,	-1,	-1,	-1,	1,	0,	false,	false,	GL_LUMINANCE_ALPHA,	GL_COMPRESSED_LUMINANCE_ALPHA, 	GL_NONE,		3, 	24, 	0, 	0, 	0, 	12, 	12,	GL_NONE,		GL_NONE,		GL_NONE,		GL_SHORT,		GL_SHORT},
	{GL_LUMINANCE16_ALPHA16,	-1,	-1, 	-1,	1,	0,	false,	false,	GL_LUMINANCE_ALPHA,	GL_COMPRESSED_LUMINANCE_ALPHA, 	GL_SHORT,		4, 	32, 	0, 	0, 	0, 	16, 	16,	GL_NONE,		GL_NONE,		GL_NONE,		GL_SHORT,		GL_SHORT},
	{GL_LUMINANCE_ALPHA32F_ARB,	-1,	-1, 	-1,	1,	0,	false,	true,	GL_LUMINANCE_ALPHA,	GL_COMPRESSED_LUMINANCE_ALPHA, 	GL_FLOAT,		8, 	64, 	0, 	0, 	0, 	32, 	32,	GL_NONE,		GL_NONE,		GL_NONE,		GL_FLOAT,		GL_FLOAT},
	{GL_RGB,			0,	1,	2,	-1,	-1,	false,	false,	GL_RGB,			GL_COMPRESSED_RGB, 		GL_NONE,		0, 	0, 	0, 	0, 	0, 	0, 	0,	GL_NONE,		GL_NONE,		GL_NONE,		GL_NONE,		GL_NONE},
	{GL_BGR,			2,	1,	0,	-1,	-1,	false,	false,	GL_BGR,			GL_COMPRESSED_RGB, 		GL_NONE,		0, 	0, 	0, 	0, 	0, 	0, 	0,	GL_NONE,		GL_NONE,		GL_NONE,		GL_NONE,		GL_NONE},
	{GL_SRGB,			0,	1,	2,	-1,	-1,	false,	false,	GL_SRGB,		GL_COMPRESSED_SRGB, 		GL_NONE,		0, 	0, 	0, 	0, 	0, 	0, 	0,	GL_NONE,		GL_NONE,		GL_NONE,		GL_NONE,		GL_NONE},
	{GL_RGB4,			0,	1,	2,	-1,	-1,	false,	false,	GL_RGB,			GL_COMPRESSED_RGB, 		GL_NONE,		2, 	12, 	4, 	4, 	4, 	0, 	0,	GL_BYTE,		GL_BYTE,		GL_BYTE,		GL_NONE,		GL_NONE},
	{GL_RGB8,			0,	1,	2,	-1,	-1,	false,	false,	GL_RGB,			GL_COMPRESSED_RGB, 		GL_BYTE,		3, 	24, 	8, 	8, 	8, 	0, 	0,	GL_BYTE,		GL_BYTE,		GL_BYTE,		GL_NONE,		GL_NONE},
	{GL_RGB8I,			0,	1,	2,	-1,	-1,	false,	false,	GL_RGB,			GL_COMPRESSED_RGB, 		GL_BYTE,		3, 	24, 	8, 	8, 	8, 	0, 	0,	GL_BYTE,		GL_BYTE,		GL_BYTE,		GL_NONE,		GL_NONE},
	{GL_RGB8UI,			0,	1,	2,	-1,	-1,	false,	false,	GL_RGB,			GL_COMPRESSED_RGB, 		GL_UNSIGNED_BYTE,	3, 	24, 	8, 	8, 	8, 	0, 	0,	GL_UNSIGNED_BYTE,	GL_UNSIGNED_BYTE,	GL_UNSIGNED_BYTE,	GL_NONE,		GL_NONE},
	{GL_RGB8_SNORM,			0,	1,	2,	-1,	-1,	false,	false,	GL_RGB,			GL_COMPRESSED_RGB, 		GL_BYTE,		3, 	24, 	8, 	8, 	8, 	0, 	0,	GL_BYTE,		GL_BYTE,		GL_BYTE,		GL_NONE,		GL_NONE},
	{GL_SRGB8,			0,	1,	2,	-1,	-1,	false,	false,	GL_RGB,			GL_COMPRESSED_SRGB, 		GL_BYTE,		3, 	24, 	8, 	8, 	8, 	0, 	0,	GL_BYTE,		GL_BYTE,		GL_BYTE,		GL_NONE,		GL_NONE},
	//{GL_RGB9_E5,			0,	1,	2,	-1,	-1,	false,	false,	GL_RGB,			GL_COMPRESSED_RGB, 		GL_NONE,		4, 	27, 	9, 	9, 	9, 	0, 	0,	GL_SHORT,		GL_SHORT,		GL_SHORT,		GL_NONE,		GL_NONE},
	{GL_RGB10,			0,	1,	2,	-1,	-1,	false,	false,	GL_RGB,			GL_COMPRESSED_RGB, 		GL_NONE,		4, 	30, 	10, 	10, 	10, 	0, 	0,	GL_SHORT,		GL_SHORT,		GL_SHORT,		GL_NONE,		GL_NONE},
	{GL_RGB12,			0,	1,	2,	-1,	-1,	false,	false,	GL_RGB,			GL_COMPRESSED_RGB, 		GL_NONE,		5, 	36, 	12, 	12, 	12, 	0, 	0,	GL_SHORT,		GL_SHORT,		GL_SHORT,		GL_NONE,		GL_NONE},
	{GL_RGB16,			0,	1,	2,	-1,	-1,	false,	false,	GL_RGB,			GL_COMPRESSED_RGB, 		GL_SHORT,		6, 	48, 	16, 	16, 	16, 	0, 	0,	GL_SHORT,		GL_SHORT,		GL_SHORT,		GL_NONE,		GL_NONE},
	{GL_RGB16I,			0,	1,	2,	-1,	-1,	false,	false,	GL_RGB,			GL_COMPRESSED_RGB, 		GL_SHORT,		6, 	48, 	16, 	16, 	16, 	0, 	0,	GL_SHORT,		GL_SHORT,		GL_SHORT,		GL_NONE,		GL_NONE},
	{GL_RGB16UI,			0,	1,	2,	-1,	-1,	false,	false,	GL_RGB,			GL_COMPRESSED_RGB, 		GL_UNSIGNED_SHORT,	6, 	48, 	16, 	16, 	16, 	0, 	0,	GL_UNSIGNED_SHORT,	GL_UNSIGNED_SHORT,	GL_UNSIGNED_SHORT,	GL_NONE,		GL_NONE},
	{GL_RGB16_SNORM,		0,	1,	2,	-1,	-1,	false,	false,	GL_RGB,			GL_COMPRESSED_RGB, 		GL_SHORT,		6, 	48, 	16, 	16, 	16, 	0, 	0,	GL_SHORT,		GL_SHORT,		GL_SHORT,		GL_NONE,		GL_NONE},
	{GL_RGB16F,			0,	1,	2,	-1,	-1,	false,	true,	GL_RGB,			GL_COMPRESSED_RGB, 		GL_FLOAT,		6, 	48, 	16, 	16, 	16, 	0, 	0,	GL_FLOAT,		GL_FLOAT,		GL_FLOAT,		GL_NONE,		GL_NONE},
	{GL_RGB32I,			0,	1,	2,	-1,	-1,	false,	false,	GL_RGB,			GL_COMPRESSED_RGB, 		GL_INT,			12, 	96, 	32, 	32, 	32, 	0, 	0,	GL_INT,			GL_INT,			GL_INT,			GL_NONE,		GL_NONE},
	{GL_RGB32UI,			0,	1,	2,	-1,	-1,	false,	false,	GL_RGB,			GL_COMPRESSED_RGB, 		GL_UNSIGNED_INT,	12, 	96, 	32, 	32, 	32, 	0, 	0,	GL_UNSIGNED_INT,	GL_UNSIGNED_INT,	GL_UNSIGNED_INT,	GL_NONE,		GL_NONE},
	{GL_RGB32F,			0,	1,	2,	-1,	-1,	false,	true,	GL_RGB,			GL_COMPRESSED_RGB, 		GL_FLOAT,		12, 	96, 	32, 	32, 	32, 	0, 	0,	GL_FLOAT,		GL_FLOAT,		GL_FLOAT,		GL_NONE,		GL_NONE},
	{GL_RGBA,			0,	1,	2,	3,	-1,	false,	false,	GL_RGBA,		GL_COMPRESSED_RGBA, 		GL_NONE,		0, 	0, 	0, 	0, 	0, 	0, 	0,	GL_NONE,		GL_NONE,		GL_NONE,		GL_NONE,		GL_NONE},
	{GL_BGRA,			2,	1,	0,	3,	-1,	false,	false,	GL_BGRA,		GL_COMPRESSED_RGBA, 		GL_NONE,		0, 	0, 	0, 	0, 	0, 	0, 	0,	GL_NONE,		GL_NONE,		GL_NONE,		GL_NONE,		GL_NONE},
	{GL_RGBA_SNORM,			0,	1,	2,	3,	-1,	false,	false,	GL_RGBA,		GL_COMPRESSED_RGBA, 		GL_NONE,		0, 	0, 	0, 	0, 	0, 	0, 	0,	GL_NONE,		GL_NONE,		GL_NONE,		GL_NONE,		GL_NONE},
	{GL_SRGB_ALPHA,			0,	1,	2,	3,	-1,	false,	false,	GL_SRGB_ALPHA,		GL_COMPRESSED_SRGB_ALPHA, 	GL_NONE,		0, 	0, 	0, 	0, 	0, 	0, 	0,	GL_NONE,		GL_NONE,		GL_NONE,		GL_NONE,		GL_NONE},
	{GL_RGBA4,			0,	1,	2,	3,	-1,	false,	false,	GL_RGBA,		GL_COMPRESSED_RGBA, 		GL_NONE,		2, 	16, 	4, 	4, 	4, 	4, 	0,	GL_BYTE,		GL_BYTE,		GL_BYTE,		GL_BYTE,		GL_NONE},
	{GL_RGBA8,			0,	1,	2,	3,	-1,	false,	false,	GL_RGBA,		GL_COMPRESSED_RGBA, 		GL_BYTE,		4, 	32, 	8, 	8, 	8, 	8, 	0,	GL_BYTE,		GL_BYTE,		GL_BYTE,		GL_BYTE,		GL_NONE},
	{GL_RGBA8I,			0,	1,	2,	3,	-1,	false,	false,	GL_RGBA,		GL_COMPRESSED_RGBA, 		GL_BYTE,		4, 	32, 	8, 	8, 	8, 	8, 	0,	GL_BYTE,		GL_BYTE,		GL_BYTE,		GL_BYTE,		GL_NONE},
	{GL_RGBA8UI,			0,	1,	2,	3,	-1,	false,	false,	GL_RGBA,		GL_COMPRESSED_RGBA, 		GL_UNSIGNED_BYTE,	4, 	32, 	8, 	8, 	8, 	8, 	0,	GL_UNSIGNED_BYTE,	GL_UNSIGNED_BYTE,	GL_UNSIGNED_BYTE,	GL_UNSIGNED_BYTE,	GL_NONE},
	{GL_RGBA8_SNORM,		0,	1,	2,	3,	-1,	false,	false,	GL_RGBA,		GL_COMPRESSED_RGBA, 		GL_BYTE,		4, 	32, 	8, 	8, 	8, 	8, 	0,	GL_BYTE,		GL_BYTE,		GL_BYTE,		GL_BYTE,		GL_NONE},
	{GL_SRGB8_ALPHA8,		0,	1,	2,	3,	-1,	false,	false,	GL_SRGB_ALPHA,		GL_COMPRESSED_SRGB_ALPHA, 	GL_UNSIGNED_BYTE,	4, 	32, 	8, 	8, 	8, 	8, 	0,	GL_BYTE,		GL_BYTE,		GL_BYTE,		GL_BYTE,		GL_NONE},
	{GL_RGBA16,			0,	1,	2,	3,	-1,	false,	false,	GL_RGBA,		GL_COMPRESSED_RGBA, 		GL_SHORT,		8, 	64, 	16, 	16, 	16, 	16, 	0,	GL_SHORT,		GL_SHORT,		GL_SHORT,		GL_SHORT,		GL_NONE},
	{GL_RGBA16I,			0,	1,	2,	3,	-1,	false,	false,	GL_RGBA,		GL_COMPRESSED_RGBA, 		GL_SHORT,		8, 	64, 	16, 	16, 	16, 	16, 	0,	GL_SHORT,		GL_SHORT,		GL_SHORT,		GL_SHORT,		GL_NONE},
	{GL_RGBA16UI,			0,	1,	2,	3,	-1,	false,	false,	GL_RGBA,		GL_COMPRESSED_RGBA, 		GL_UNSIGNED_SHORT,	8, 	64, 	16, 	16, 	16, 	16, 	0,	GL_UNSIGNED_SHORT,	GL_UNSIGNED_SHORT,	GL_UNSIGNED_SHORT,	GL_UNSIGNED_SHORT,	GL_NONE},
	{GL_RGBA16F,			0,	1,	2,	3,	-1,	false,	true,	GL_RGBA,		GL_COMPRESSED_RGBA, 		GL_SHORT,		8, 	64, 	16, 	16, 	16, 	16, 	0,	GL_FLOAT,		GL_FLOAT,		GL_FLOAT,		GL_FLOAT,		GL_NONE},
	{GL_RGBA16_SNORM,		0,	1,	2,	3,	-1,	false,	false,	GL_RGBA,		GL_COMPRESSED_RGBA, 		GL_SHORT,		8, 	64, 	16, 	16, 	16, 	16, 	0,	GL_SHORT,		GL_SHORT,		GL_SHORT,		GL_SHORT,		GL_NONE},
	{GL_RGBA32I,			0,	1,	2,	3,	-1,	false,	false,	GL_RGBA,		GL_COMPRESSED_RGBA, 		GL_INT,			16, 	128, 	32, 	32, 	32, 	32, 	0,	GL_INT,			GL_INT,			GL_INT,			GL_INT,			GL_NONE},
	{GL_RGBA32UI,			0,	1,	2,	3,	-1,	false,	false,	GL_RGBA,		GL_COMPRESSED_RGBA, 		GL_UNSIGNED_INT,	16, 	128, 	32, 	32, 	32, 	32, 	0,	GL_UNSIGNED_INT,	GL_UNSIGNED_INT,	GL_UNSIGNED_INT,	GL_UNSIGNED_INT,	GL_NONE},
	{GL_RGBA32F,			0,	1,	2,	3,	-1,	false,	true,	GL_RGBA,		GL_COMPRESSED_RGBA, 		GL_FLOAT,		16, 	128, 	32, 	32, 	32, 	32, 	0,	GL_FLOAT,		GL_FLOAT,		GL_FLOAT,		GL_FLOAT,		GL_NONE},
	//{GL_RGB10_A2,			0,	1,	2,	3,	-1,	false,	false,	GL_RGB,			GL_COMPRESSED_RGBA, 		GL_NONE,		4,	32, 	10,	10,	10,	2,	0,	GL_SHORT,		GL_SHORT,		GL_SHORT,		GL_BYTE,		GL_NONE},
	//{GL_RGB10_A2UI,		0,	1,	2,	3,	-1,	false,	false,	GL_RGB,			GL_COMPRESSED_RGBA, 		GL_NONE,		4,	32, 	10,	10,	10,	2,	0,	GL_SHORT,		GL_SHORT,		GL_SHORT,		GL_UNSIGNED_BYTE,	GL_NONE},
	// name				red	green	blue	alpha	lum.  compress. float  alias			corresponding       	      Forced Depth           TSBytes  TSbits   RedSb  GreenSb BlueSb  AlphaSb LumSb	redType			greenType		blueType		alphaType		luminanceType

	// Compressed modes :
	// name						red	green	blue	alpha	lum.	compr.	float	alias 				corresponding		Forced Depth		TSBytes	TSbits	RedSb	GreenSb BlueSb	AlphaSb LumSb	redType		greenType	blueType	alphaType	luminanceType
	{GL_COMPRESSED_RED,				0,	-1,	-1,	-1,	-1,	true,	false,	GL_COMPRESSED_RED,		GL_RED,			GL_NONE,		0,	0,	0,	0,	0,	0,	0,	GL_NONE,	GL_NONE,	GL_NONE,	GL_NONE,	GL_NONE},
	{GL_COMPRESSED_RED_RGTC1,			0,	-1,	-1,	-1,	-1,	true,	false,	GL_COMPRESSED_RED,		GL_RED,			GL_NONE,		0,	0,	0,	0,	0,	0,	0,	GL_NONE,	GL_NONE,	GL_NONE,	GL_NONE,	GL_NONE},
	{GL_COMPRESSED_ALPHA,				-1,	-1,	-1,	0,	-1,	true,	false,	GL_COMPRESSED_ALPHA,		GL_ALPHA,		GL_NONE,		0,	0,	0,	0,	0,	0,	0,	GL_NONE,	GL_NONE,	GL_NONE,	GL_NONE,	GL_NONE},
	{GL_COMPRESSED_INTENSITY,			-1,	-1,	-1,	-1,	0,	true,	false,	GL_COMPRESSED_INTENSITY,	GL_INTENSITY,		GL_NONE,		0,	0,	0,	0,	0,	0,	0,	GL_NONE,	GL_NONE,	GL_NONE,	GL_NONE,	GL_NONE},
	{GL_COMPRESSED_LUMINANCE,			-1,	-1,	-1,	-1,	0,	true,	false,	GL_COMPRESSED_LUMINANCE,	GL_LUMINANCE,		GL_NONE,		0,	0,	0,	0,	0,	0,	0,	GL_NONE,	GL_NONE,	GL_NONE,	GL_NONE,	GL_NONE},
	{GL_COMPRESSED_RG,				0,	1,	-1,	-1,	-1,	true,	false,	GL_COMPRESSED_RG,		GL_RG,			GL_NONE,		0,	0,	0,	0,	0,	0,	0,	GL_NONE,	GL_NONE,	GL_NONE,	GL_NONE,	GL_NONE},
	{GL_COMPRESSED_RG_RGTC2,			0,	1,	-1,	-1,	-1,	true,	false,	GL_COMPRESSED_RG,		GL_RG,			GL_NONE,		0,	0,	0,	0,	0,	0,	0,	GL_NONE,	GL_NONE,	GL_NONE,	GL_NONE,	GL_NONE},
	{GL_COMPRESSED_LUMINANCE_ALPHA,			-1,	-1,	-1,	1,	0,	true,	false,	GL_COMPRESSED_LUMINANCE_ALPHA,	GL_LUMINANCE_ALPHA,	GL_NONE,		0,	0,	0,	0,	0,	0,	0,	GL_NONE,	GL_NONE,	GL_NONE,	GL_NONE,	GL_NONE},
	{GL_COMPRESSED_RGB,				0,	1,	2,	-1,	-1,	true,	false,	GL_COMPRESSED_RGB,		GL_RGB,			GL_NONE,		0,	0,	0,	0,	0,	0,	0,	GL_NONE,	GL_NONE,	GL_NONE,	GL_NONE,	GL_NONE},
	{GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB,	0,	1,	2,	-1,	-1,	true,	false,	GL_COMPRESSED_RGB,		GL_RGB,			GL_NONE,		0,	0,	0,	0,	0,	0,	0,	GL_NONE,	GL_NONE,	GL_NONE,	GL_NONE,	GL_NONE},
	{GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB,	0,	1,	2,	-1,	-1,	true,	false,	GL_COMPRESSED_RGB,		GL_RGB,			GL_NONE,		0,	0,	0,	0,	0,	0,	0,	GL_NONE,	GL_NONE,	GL_NONE,	GL_NONE,	GL_NONE},
	{GL_COMPRESSED_RGB_FXT1_3DFX,			0,	1,	2,	-1,	-1,	true,	false,	GL_COMPRESSED_RGB,		GL_RGB,			GL_NONE,		0,	0,	0,	0,	0,	0,	0,	GL_NONE,	GL_NONE,	GL_NONE,	GL_NONE,	GL_NONE},
	{GL_COMPRESSED_RGB_S3TC_DXT1_EXT,		0,	1,	2,	-1,	-1,	true,	false,	GL_COMPRESSED_RGB,		GL_RGB,			GL_NONE,		0,	0,	0,	0,	0,	0,	0,	GL_NONE,	GL_NONE,	GL_NONE,	GL_NONE,	GL_NONE},
	{GL_COMPRESSED_SRGB,				0,	1,	2,	-1,	-1,	true,	false,	GL_COMPRESSED_SRGB,		GL_SRGB,		GL_NONE,		0,	0,	0,	0,	0,	0,	0,	GL_NONE,	GL_NONE,	GL_NONE,	GL_NONE,	GL_NONE},
	{GL_COMPRESSED_SRGB_S3TC_DXT1_EXT,		0,	1,	2,	-1,	-1,	true,	false,	GL_COMPRESSED_SRGB,		GL_SRGB,		GL_NONE,		0,	0,	0,	0,	0,	0,	0,	GL_NONE,	GL_NONE,	GL_NONE,	GL_NONE,	GL_NONE},
	{GL_COMPRESSED_RGBA,				0,	1,	2,	3,	-1,	true,	false,	GL_COMPRESSED_RGBA,		GL_RGBA,		GL_NONE,		0,	0,	0,	0,	0,	0,	0,	GL_NONE,	GL_NONE,	GL_NONE,	GL_NONE,	GL_NONE},
	{GL_COMPRESSED_RGBA_BPTC_UNORM_ARB,		0,	1,	2,	3,	-1,	true,	false,	GL_COMPRESSED_RGBA,		GL_RGBA,		GL_NONE,		0,	0,	0,	0,	0,	0,	0,	GL_NONE,	GL_NONE,	GL_NONE,	GL_NONE,	GL_NONE},
	{GL_COMPRESSED_RGBA_FXT1_3DFX,			0,	1,	2,	3,	-1,	true,	false,	GL_COMPRESSED_RGBA,		GL_RGBA,		GL_NONE,		0,	0,	0,	0,	0,	0,	0,	GL_NONE,	GL_NONE,	GL_NONE,	GL_NONE,	GL_NONE},
	{GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,		0,	1,	2,	3,	-1,	true,	false,	GL_COMPRESSED_RGBA,		GL_RGBA,		GL_NONE,		0,	0,	0,	0,	0,	0,	0,	GL_NONE,	GL_NONE,	GL_NONE,	GL_NONE,	GL_NONE},
	{GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,		0,	1,	2,	3,	-1,	true,	false,	GL_COMPRESSED_RGBA,		GL_RGBA,		GL_NONE,		0,	0,	0,	0,	0,	0,	0,	GL_NONE,	GL_NONE,	GL_NONE,	GL_NONE,	GL_NONE},
	{GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,		0,	1,	2,	3,	-1,	true,	false,	GL_COMPRESSED_RGBA,		GL_RGBA,		GL_NONE,		0,	0,	0,	0,	0,	0,	0,	GL_NONE,	GL_NONE,	GL_NONE,	GL_NONE,	GL_NONE},
	{GL_COMPRESSED_SRGB_ALPHA,			0,	1,	2,	3,	-1,	true,	false,	GL_COMPRESSED_SRGB_ALPHA,	GL_SRGB_ALPHA,		GL_NONE,		0,	0,	0,	0,	0,	0,	0,	GL_NONE,	GL_NONE,	GL_NONE,	GL_NONE,	GL_NONE},
	{GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB,	0,	1,	2,	3,	-1,	true,	false,	GL_COMPRESSED_SRGB_ALPHA,	GL_SRGB_ALPHA,		GL_NONE,		0,	0,	0,	0,	0,	0,	0,	GL_NONE,	GL_NONE,	GL_NONE,	GL_NONE,	GL_NONE},
	{GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT,	0,	1,	2,	3,	-1,	true,	false,	GL_COMPRESSED_SRGB_ALPHA,	GL_SRGB_ALPHA,		GL_NONE,		0,	0,	0,	0,	0,	0,	0,	GL_NONE,	GL_NONE,	GL_NONE,	GL_NONE,	GL_NONE},
	{GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT,	0,	1,	2,	3,	-1,	true,	false,	GL_COMPRESSED_SRGB_ALPHA,	GL_SRGB_ALPHA,		GL_NONE,		0,	0,	0,	0,	0,	0,	0,	GL_NONE,	GL_NONE,	GL_NONE,	GL_NONE,	GL_NONE},
	{GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT,	0,	1,	2,	3,	-1,	true,	false,	GL_COMPRESSED_SRGB_ALPHA,	GL_SRGB_ALPHA,		GL_NONE,		0,	0,	0,	0,	0,	0,	0,	GL_NONE,	GL_NONE,	GL_NONE,	GL_NONE,	GL_NONE},

	{GL_NONE, 					-1, 	-1, 	-1, 	-1, 	-1,	false, false, 	GL_NONE, 			GL_NONE, 		GL_NONE, 		0, 	0,	0,	0,	0,	0,	0,	GL_NONE,	GL_NONE,	GL_NONE,	GL_NONE,	GL_NONE}  // END
};

// HdlTextureFormatDescriptor :
	/**
	\fn int HdlTextureFormatDescriptor::numChannels(void) const
	\brief Returns the number of channels in the mode.
	\return The number of channels in the mode.
	**/
	int HdlTextureFormatDescriptor::numChannels(void) const
	{
		return (hasRedChannel()?1:0) + (hasGreenChannel()?1:0) + (hasBlueChannel()?1:0) + (hasAlphaChannel()?1:0) + (hasLuminanceChannel()?1:0);
	}

	/**
	\fn bool HdlTextureFormatDescriptor::hasRedChannel(void) const
	\brief Test if the mode has a red channel.
	\return True if the mode has a red channel.
	**/
	bool HdlTextureFormatDescriptor::hasRedChannel(void) const
	{
		return (indexRed>=0);
	}

	/**
	\fn bool HdlTextureFormatDescriptor::hasGreenChannel(void) const
	\brief Test if the mode has a green channel.
	\return True if the mode has a green channel.
	**/
	bool HdlTextureFormatDescriptor::hasGreenChannel(void) const
	{
		return (indexGreen>=0);
	}

	/**
	\fn bool HdlTextureFormatDescriptor::hasBlueChannel(void) const
	\brief Test if the mode has a blue channel.
	\return True if the mode has a blue channel.
	**/
	bool HdlTextureFormatDescriptor::hasBlueChannel(void) const
	{
		return (indexBlue>=0);
	}

	/**
	\fn bool HdlTextureFormatDescriptor::hasAlphaChannel(void) const
	\brief Test if the mode has a alpha channel.
	\return True if the mode has a alpha channel.
	**/
	bool HdlTextureFormatDescriptor::hasAlphaChannel(void) const
	{
		return (indexAlpha>=0);
	}

	/**
	\fn bool HdlTextureFormatDescriptor::hasLuminanceChannel(void) const
	\brief Test if the mode has a luminance channel.
	\return True if the mode has a luminance channel.
	**/
	bool HdlTextureFormatDescriptor::hasLuminanceChannel(void) const
	{
		return (indexLuminance>=0);
	}

	/**
	\fn bool HdlTextureFormatDescriptor::hasChannel(GLenum channel) const
	\brief Returns true if the format has the required channel, false otherwise or raise an exception if the channel is unknown.
	\param channel The channel name (GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA or GL_LUMINANCE).
	\return True if the format has the channel.
	**/
	bool HdlTextureFormatDescriptor::hasChannel(GLenum channel) const
	{
		switch(channel)
		{
			case GL_RED:		return hasRedChannel();
			case GL_GREEN:		return hasGreenChannel();
			case GL_BLUE:		return hasBlueChannel();
			case GL_ALPHA:		return hasAlphaChannel();
			case GL_LUMINANCE:	return hasLuminanceChannel();
			default :
				throw Exception("HdlTextureFormatDescriptor::hasChannel - Unknown channel name : \"" + glParamName(channel) + "\".", __FILE__, __LINE__, Exception::GLException);
		}
	}

	/**
	\fn int HdlTextureFormatDescriptor::getPixelSize(GLenum depth) const
	\brief Get the size of one pixel for the current mode with the given depth.
	\param depth The depth associated with this mode (bypassed if the mode has only one valid depth).
	\return The size of one pixel in bytes.
	**/
	int HdlTextureFormatDescriptor::getPixelSize(GLenum depth) const
	{
		if(pixelSizeInBytes!=0)
			return pixelSizeInBytes;
		else
			return numChannels() * HdlTextureFormatDescriptorsList::getTypeDepth(depth);
	}

	/**
	\fn unsigned int HdlTextureFormatDescriptor::channelSizeInBits(GLenum channel) const
	\brief Returns the channel size in bits or raise an exception if the channel is unknown.
	\param channel The channel name (GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA or GL_LUMINANCE).
	\return The channel size in bits.
	**/
	unsigned int HdlTextureFormatDescriptor::channelSizeInBits(GLenum channel) const
	{
		switch(channel)
		{
			case GL_RED:		return redDepthInBits;
			case GL_GREEN:		return greenDepthInBits;
			case GL_BLUE:		return blueDepthInBits;
			case GL_ALPHA:		return alphaDepthInBits;
			case GL_LUMINANCE:	return luminanceDepthInBits;
			default :
				throw Exception("HdlTextureFormatDescriptor::channelSizeInBits - Unknown channel name : \"" + glParamName(channel) + "\".", __FILE__, __LINE__, Exception::GLException);
		}
	}

	/**
	\fn GLenum HdlTextureFormatDescriptor::channelType(GLenum channel) const
	\brief Returns the channel type or raise an exception if the channel is unknown.
	\param channel The channel name (GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA or GL_LUMINANCE).
	\return Returns the channel type.
	**/
	GLenum HdlTextureFormatDescriptor::channelType(GLenum channel) const
	{
		switch(channel)
		{
			case GL_RED:		return redType;
			case GL_GREEN:		return greenType;
			case GL_BLUE:		return blueType;
			case GL_ALPHA:		return alphaType;
			case GL_LUMINANCE:	return luminanceType;
			default :
				throw Exception("HdlTextureFormatDescriptor::channelType - Unknown channel name : \"" + glParamName(channel) + "\".", __FILE__, __LINE__, Exception::GLException);
		}
	}

	/**
	\fn int HdlTextureFormatDescriptor::channelIndex(GLenum channel) const
	\brief Returns the channel index or raise an exception if the channel is unknown.
	\param channel The channel name (GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA, GL_LUMINANCE or GL_NONE).
	\return Returns the channel index (-1 for GL_NONE).
	**/
	int HdlTextureFormatDescriptor::channelIndex(GLenum channel) const
	{
		switch(channel)
		{
			case GL_RED:		return indexRed;
			case GL_GREEN:		return indexGreen;
			case GL_BLUE:		return indexBlue;
			case GL_ALPHA:		return indexAlpha;
			case GL_LUMINANCE:	return indexLuminance;
			case GL_NONE:		return -1;
			default :
				throw Exception("HdlTextureFormatDescriptor::channelIndex - Unknown channel name : \"" + glParamName(channel) + "\".", __FILE__, __LINE__, Exception::GLException);
		}
	}

	/**
	\fn GLenum HdlTextureFormatDescriptor::channelAtIndex(int idx) const
	\brief Get the channel name at the specified index.
	\param idx The targeted index.
	\return The channel name (GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA or GL_LUMINANCE) or GL_NONE if none was found.
	**/
	GLenum HdlTextureFormatDescriptor::channelAtIndex(int idx) const
	{
		if(idx==indexRed)	return GL_RED;
		if(idx==indexGreen) 	return GL_GREEN;
		if(idx==indexBlue) 	return GL_BLUE;
		if(idx==indexAlpha) 	return GL_ALPHA;
		if(idx==indexLuminance) return GL_LUMINANCE; 
		
		return GL_NONE;
	}

	/**
	\fn std::vector<GLenum> HdlTextureFormatDescriptor::getChannelsList(void) const
	\brief Get the list of the channels of this mode.
	\return A list of the channels, in the correct order of appearance.
	**/
	std::vector<GLenum> HdlTextureFormatDescriptor::getChannelsList(void) const
	{
		std::vector<GLenum> res(numChannels(), GL_NONE);

		if(indexRed>=0)		res[indexRed]		= GL_RED;
		if(indexGreen>=0)	res[indexGreen] 	= GL_GREEN;
		if(indexBlue>=0)	res[indexBlue] 		= GL_BLUE;
		if(indexAlpha>=0)	res[indexAlpha] 	= GL_ALPHA;
		if(indexLuminance>=0)	res[indexLuminance] 	= GL_LUMINANCE;

		return res;
	}

	std::vector<int> HdlTextureFormatDescriptor::getOffsetsList(void) const
	{
		std::vector<int> res(numChannels(), 0);

		#define ALL_OFFSETS_BUT( currentIndex) \
			(indexRed<currentIndex		?0:(redDepthInBits+1)/8) + \
			(indexGreen<currentIndex	?0:(redDepthInBits+1)/8) + \
			(indexBlue<currentIndex		?0:(redDepthInBits+1)/8) + \
			(indexAlpha<currentIndex	?0:(redDepthInBits+1)/8) + \
			(indexLuminance<currentIndex	?0:(redDepthInBits+1)/8)

		if(indexRed>=0)		res[indexRed]		= ALL_OFFSETS_BUT( indexRed );
		if(indexGreen>=0)	res[indexGreen]		= ALL_OFFSETS_BUT( indexGreen );
		if(indexBlue>=0)	res[indexBlue]		= ALL_OFFSETS_BUT( indexBlue );
		if(indexAlpha>=0)	res[indexAlpha]		= ALL_OFFSETS_BUT( indexAlpha );
		if(indexLuminance>=0)	res[indexLuminance]	= ALL_OFFSETS_BUT( indexLuminance );

		#undef ALL_OFFSETS_BUT

		return res;
	}

	/**
	\fn GLenum HdlTextureFormatDescriptor::getCompressedMode(void) const
	\brief Get the corresponding compressed mode for the current mode.
	\return The corresponding compressed mode. If the mode is compressed, the function return modeID.
	**/
	GLenum HdlTextureFormatDescriptor::getCompressedMode(void) const
	{
		if(isCompressed)
			return modeID;
		else
			return correspondingModeForCompressing;
	}

	/**
	\fn GLenum HdlTextureFormatDescriptor::getUncompressedMode(void) const
	\brief Get the corresponding uncompressed mode for the current mode.
	\return The corresponding uncompressed mode. If the mode is uncompressed, the function return modeID.
	**/
	GLenum HdlTextureFormatDescriptor::getUncompressedMode(void) const
	{
		if(isCompressed)
			return correspondingModeForCompressing;
		else
			return modeID;
	}

// HdlTextureFormatDescriptorList :
	const int HdlTextureFormatDescriptorsList::MaxPixelSizeInBytes = 16;

	/**
	\fn int HdlTextureFormatDescriptorsList::getNumDescriptors(void)
	\brief Returns the number of known GL modes for texture formats (GL_RGB, GL_RGBA, ...).
	\return Returns the number of known GL modes.
	**/
	int HdlTextureFormatDescriptorsList::getNumDescriptors(void)
	{
		static int i = 0;

		if(i==0)
			for(i=0; textureFormatDescriptors[i].modeID!=GL_NONE; i++);

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
	\fn const HdlTextureFormatDescriptor& HdlTextureFormatDescriptorsList::get(const GLenum& modeID)
	\brief Access to a mode descriptor object. Will raise an exception if the modeID is unknown.
	\param modeID Searched mode name.
	\return A constant reference onto a HdlTextureFormatDescriptor object.
	**/
	const HdlTextureFormatDescriptor& HdlTextureFormatDescriptorsList::get(const GLenum& modeID)
	{
		for(int i=0; textureFormatDescriptors[i].modeID!=GL_NONE; i++)
			if(modeID==textureFormatDescriptors[i].modeID && modeID!=GL_NONE)
				return textureFormatDescriptors[i];

		//else
			throw Exception("HdlTextureFormatDescriptorsList::get - No corresponding mode for : " + glParamName(modeID) + ".", __FILE__, __LINE__, Exception::GLException);
	}

	/**
	\fn int HdlTextureFormatDescriptorsList::getTypeDepth(GLenum depth)
	\brief Return the depth, in bytes, associated with depth.
	\param depth The depth, described as its enumerator value (e.g. GL_BYTE for GLbyte).
	\return Size of the type, in bytes.
	**/
	int HdlTextureFormatDescriptorsList::getTypeDepth(GLenum depth)
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
				//colSize = 0;
				throw Exception("HdlTextureFormatDescriptorsList::getTypeDepth - cannot recognize color channel type " + glParamName(depth), __FILE__, __LINE__, Exception::GLException);
		}
	}

