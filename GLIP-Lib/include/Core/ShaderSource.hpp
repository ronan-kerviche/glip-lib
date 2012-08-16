/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : GPLv3                                                                                     */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : ShaderSource.hpp                                                                          */
/*     Original Date : August 15th 2011                                                                          */
/*                                                                                                               */
/*     Description   : Shader source and tools                                                                   */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    ShaderSource.hpp
 * \brief   Shader source and tools
 * \author  R. KERVICHE
 * \date    August 15th 2011
**/

#ifndef __GLIP_SHADERSOURCE__
#define __GLIP_SHADERSOURCE__

	// Includes
        #include <string>
        #include <vector>
        #include <fstream>

	namespace Glip
	{
		namespace CoreGL
		{
			// Constants
			enum GLSL_KEYWORD
			{
				GLSL_KW_attribute,
				GLSL_KW_const,
				GLSL_KW_uniform,
				GLSL_KW_varying,
				GLSL_KW_layout,
				GLSL_KW_centroid,
				GLSL_KW_flat,
				GLSL_KW_smooth,
				GLSL_KW_noperspective,
				GLSL_KW_patch,
				GLSL_KW_sample,
				GLSL_KW_break,
				GLSL_KW_continue,
				GLSL_KW_do,
				GLSL_KW_for,
				GLSL_KW_while,
				GLSL_KW_switch,
				GLSL_KW_case,
				GLSL_KW_default,
				GLSL_KW_if,
				GLSL_KW_else,
				GLSL_KW_subroutine,
				GLSL_KW_in,
				GLSL_KW_out,
				GLSL_KW_inout,
				GLSL_KW_float,
				GLSL_KW_double,
				GLSL_KW_int,
				GLSL_KW_void,
				GLSL_KW_bool,
				GLSL_KW_true,
				GLSL_KW_false,
				GLSL_KW_invariant,
				GLSL_KW_discard,
				GLSL_KW_return,
				GLSL_KW_mat2,
				GLSL_KW_mat3,
				GLSL_KW_mat4,
				GLSL_KW_dmat2,
				GLSL_KW_dmat3,
				GLSL_KW_dmat4,
				GLSL_KW_mat2x2,
				GLSL_KW_mat2x3,
				GLSL_KW_mat2x4,
				GLSL_KW_dmat2x2,
				GLSL_KW_dmat2x3,
				GLSL_KW_dmat2x4,
				GLSL_KW_mat3x2,
				GLSL_KW_mat3x3,
				GLSL_KW_mat3x4,
				GLSL_KW_dmat3x2,
				GLSL_KW_dmat3x3,
				GLSL_KW_dmat3x4,
				GLSL_KW_mat4x2,
				GLSL_KW_mat4x3,
				GLSL_KW_mat4x4,
				GLSL_KW_dmat4x2,
				GLSL_KW_dmat4x3,
				GLSL_KW_dmat4x4,
				GLSL_KW_vec2,
				GLSL_KW_vec3,
				GLSL_KW_vec4,
				GLSL_KW_ivec2,
				GLSL_KW_ivec3,
				GLSL_KW_ivec4,
				GLSL_KW_bvec2,
				GLSL_KW_bvec3,
				GLSL_KW_bvec4,
				GLSL_KW_dvec2,
				GLSL_KW_dvec3,
				GLSL_KW_dvec4,
				GLSL_KW_uint,
				GLSL_KW_uvec2,
				GLSL_KW_uvec3,
				GLSL_KW_uvec4,
				GLSL_KW_lowp,
				GLSL_KW_mediump,
				GLSL_KW_highp,
				GLSL_KW_precision,
				GLSL_KW_sampler1D,
				GLSL_KW_sampler2D,
				GLSL_KW_sampler3D,
				GLSL_KW_samplerCube,
				GLSL_KW_sampler1DShadow,
				GLSL_KW_sampler2DShadow,
				GLSL_KW_samplerCubeShadow,
				GLSL_KW_sampler1DArray,
				GLSL_KW_sampler2DArray,
				GLSL_KW_sampler1DArrayShadow,
				GLSL_KW_sampler2DArrayShadow,
				GLSL_KW_isampler1D,
				GLSL_KW_isampler2D,
				GLSL_KW_isampler3D,
				GLSL_KW_isamplerCube,
				GLSL_KW_isampler1DArray,
				GLSL_KW_isampler2DArray,
				GLSL_KW_usampler1D,
				GLSL_KW_usampler2D,
				GLSL_KW_usampler3D,
				GLSL_KW_usamplerCube,
				GLSL_KW_usampler1DArray,
				GLSL_KW_usampler2DArray,
				GLSL_KW_sampler2DRect,
				GLSL_KW_sampler2DRectShadow,
				GLSL_KW_isampler2DRect,
				GLSL_KW_usampler2DRect,
				GLSL_KW_samplerBuffer,
				GLSL_KW_isamplerBuffer,
				GLSL_KW_usamplerBuffer,
				GLSL_KW_sampler2DMS,
				GLSL_KW_isampler2DMS,
				GLSL_KW_usampler2DMS,
				GLSL_KW_sampler2DMSArray,
				GLSL_KW_isampler2DMSArray,
				GLSL_KW_usampler2DMSArray,
				GLSL_KW_samplerCubeArray,
				GLSL_KW_samplerCubeArrayShadow,
				GLSL_KW_isamplerCubeArray,
				GLSL_KW_usamplerCubeArray,
				GLSL_KW_struct,
				GLSL_KW_common,
				GLSL_KW_partition,
				GLSL_KW_active,
				GLSL_KW_asm,
				GLSL_KW_class,
				GLSL_KW_union,
				GLSL_KW_enum,
				GLSL_KW_typedef,
				GLSL_KW_template,
				GLSL_KW_this,
				GLSL_KW_packed,
				GLSL_KW_vgoto,
				GLSL_KW_inline,
				GLSL_KW_noinline,
				GLSL_KW_volatile,
				GLSL_KW_public,
				GLSL_KW_static,
				GLSL_KW_extern,
				GLSL_KW_external,
				GLSL_KW_interface,
				GLSL_KW_long,
				GLSL_KW_short,
				GLSL_KW_half,
				GLSL_KW_fixed,
				GLSL_KW_unsigned,
				GLSL_KW_superp,
				GLSL_KW_input,
				GLSL_KW_output,
				GLSL_KW_hvec2,
				GLSL_KW_hvec3,
				GLSL_KW_hvec4,
				GLSL_KW_fvec2,
				GLSL_KW_fvec3,
				GLSL_KW_fvec4,
				GLSL_KW_sampler3DRect,
				GLSL_KW_filter,
				GLSL_KW_image1D,
				GLSL_KW_image2D,
				GLSL_KW_image3D,
				GLSL_KW_imageCube,
				GLSL_KW_iimage1D,
				GLSL_KW_iimage2D,
				GLSL_KW_iimage3D,
				GLSL_KW_iimageCube,
				GLSL_KW_uimage1D,
				GLSL_KW_uimage2D,
				GLSL_KW_uimage3D,
				GLSL_KW_uimageCube,
				GLSL_KW_image1DArray,
				GLSL_KW_image2DArray,
				GLSL_KW_iimage1DArray,
				GLSL_KW_iimage2DArray,
				GLSL_KW_uimage1DArray,
				GLSL_KW_uimage2DArray,
				GLSL_KW_image1DShadow,
				GLSL_KW_image2DShadow,
				GLSL_KW_image1DArrayShadow,
				GLSL_KW_image2DArrayShadow,
				GLSL_KW_imageBuffer,
				GLSL_KW_iimageBuffer,
				GLSL_KW_uimageBuffer,
				GLSL_KW_sizeof,
				GLSL_KW_cast,
				GLSL_KW_namespace,
				GLSL_KW_using,
				GLSL_KW_row_major, // 185 lines
				GLSL_KW_END
			};

			extern const char* GLSLKeyword[];

			// Objects
			/**
			\class ShaderSource
			\brief Shader source code and infos
			**/
			class ShaderSource
			{
				private :
					// Data
					std::string source;
					std::string sourceName;
					std::vector<std::string> inVars;
					std::vector<std::string> outVars;
					bool compatibilityRequest;

					// Tools
					std::string getLine(int l);
					int removeKeyword(std::string& str, GLSL_KEYWORD kw);
					int removeAnyOfSampler(std::string& str);
					void parseGlobals(void);

				public :
					// Tools
					ShaderSource(const char** src, bool eol = true, int lines=-1);
					ShaderSource(const std::string& src);
					ShaderSource(const ShaderSource& ss);

					const std::string& getSource(void)     const;
					const std::string& getSourceName(void) const;
					const char*        getSourceCstr(void) const;
					std::string        errorLog(std::string log);
					bool		   requiresCompatibility(void) const;

					const std::vector<std::string>& getInputVars(void);
					const std::vector<std::string>& getOutputVars(void);
			};
		}
	}

/* GLSL Key words list :
attribute
const
uniform
varying
layout
centroid
flat
smooth
noperspective
patch
sample
break
continue
do
for
while
switch
case
default
if
else
subroutine
in
out
inout
float
double
int
void
bool
true
false
invariant
discard
return
mat2
mat3
mat4
dmat2
dmat3
dmat4
mat2x2
mat2x3
mat2x4
dmat2x2
dmat2x3
dmat2x4
mat3x2
mat3x3
mat3x4
dmat3x2
dmat3x3
dmat3x4
mat4x2
mat4x3
mat4x4
dmat4x2
dmat4x3
dmat4x4
vec2
vec3
vec4
ivec2
ivec3
ivec4
bvec2
bvec3
bvec4
dvec2
dvec3
dvec4
uint
uvec2
uvec3
uvec4
lowp
mediump
highp
precision
sampler1D
sampler2D
sampler3D
samplerCube
sampler1DShadow
sampler2DShadow
samplerCubeShadow
sampler1DArray
sampler2DArray
sampler1DArrayShadow
sampler2DArrayShadow
isampler1D
isampler2D
isampler3D
isamplerCube
isampler1DArray
isampler2DArray
usampler1D
usampler2D
usampler3D
usamplerCube
usampler1DArray
usampler2DArray
sampler2DRect
sampler2DRectShadow
isampler2DRect
usampler2DRect
samplerBuffer
isamplerBuffer
usamplerBuffer
sampler2DMS
isampler2DMS
usampler2DMS
sampler2DMSArray
isampler2DMSArray
usampler2DMSArray
samplerCubeArray
samplerCubeArrayShadow
isamplerCubeArray
usamplerCubeArray
struct
common
partition
active
asm
class
union
enum
typedef
template
this
packed
goto
inline
noinline
volatile
public
static
extern
external
interface
long
short
half
fixed
unsigned
superp
input
output
hvec2
hvec3
hvec4
fvec2
fvec3
fvec4
sampler3DRect
filter
image1D
image2D
image3D
imageCube
iimage1D
iimage2D
iimage3D
iimageCube
uimage1D
uimage2D
uimage3D
uimageCube
image1DArray
image2DArray
iimage1DArray
iimage2DArray
uimage1DArray
uimage2DArray
image1DShadow
image2DShadow
image1DArrayShadow
image2DArrayShadow
imageBuffer
iimageBuffer
uimageBuffer
sizeof
cast
namespace
using
row_major
*/

#endif
