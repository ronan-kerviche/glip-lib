/**

	Gradient2

		red   : horizontal
		green : vertical
		blue  : diag X
        alpha : diag Y

        11 21 31
        12 0  32
        13 23 33

**/

#version 130

uniform sampler2D tex0;
out     vec4 red, green, blue;

void main()
{
	// Get the size
	ivec2 sz = textureSize(tex0, 0);
	float sx = 1.0/(float(sz.s));
	float sy = 1.0/(float(sz.t));

	// get the data
	vec4 col0  = textureLod(tex0, gl_TexCoord[0].st, 0.0);
	vec4 col11 = textureLod(tex0, gl_TexCoord[0].st + vec2(-sx, -sy), 0.0);
	vec4 col12 = textureLod(tex0, gl_TexCoord[0].st + vec2(-sx, 0.0), 0.0);
	vec4 col13 = textureLod(tex0, gl_TexCoord[0].st + vec2(-sx,  sy), 0.0);
	vec4 col21 = textureLod(tex0, gl_TexCoord[0].st + vec2(0.0, -sy), 0.0);
	vec4 col23 = textureLod(tex0, gl_TexCoord[0].st + vec2(0.0,  sy), 0.0);
	vec4 col31 = textureLod(tex0, gl_TexCoord[0].st + vec2( sx, -sy), 0.0);
	vec4 col32 = textureLod(tex0, gl_TexCoord[0].st + vec2( sx, 0.0), 0.0);
	vec4 col33 = textureLod(tex0, gl_TexCoord[0].st + vec2( sx,  sy), 0.0);

	red.r      = abs((col32.r - col0.r)-(col0.r - col12.r))/2.0;
	green.r    = abs((col32.g - col0.g)-(col0.g - col12.g))/2.0;
	blue.r     = abs((col32.b - col0.b)-(col0.b - col12.b))/2.0;

	red.g      = abs((col23.r - col0.r)-(col0.r - col21.r))/2.0;
	green.g    = abs((col23.g - col0.g)-(col0.g - col21.g))/2.0;
	blue.g     = abs((col23.b - col0.b)-(col0.b - col21.b))/2.0;

	red.b      = abs((col33.r - col0.r)-(col0.r - col11.r))/2.0;
	green.b    = abs((col33.g - col0.g)-(col0.g - col11.g))/2.0;
	blue.b     = abs((col33.b - col0.b)-(col0.b - col11.b))/2.0;

	red.a      = abs((col31.r - col0.r)-(col0.r - col13.r))/2.0;
	green.a    = abs((col31.g - col0.g)-(col0.g - col13.g))/2.0;
	blue.a     = abs((col31.b - col0.b)-(col0.b - col13.b))/2.0;
}
