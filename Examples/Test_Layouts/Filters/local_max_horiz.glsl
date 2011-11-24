/**

	Local Maximum

**/

#version 130

uniform sampler2D inRed, inGreen, inBlue;
out     vec4 red, green, blue;
uniform int nMax;

void main()
{
	//Tmp data
	int   mv = 0;
	vec4  col;

	red   = vec4(0,0,0,0);
	green = vec4(0,0,0,0);
	blue  = vec4(0,0,0,0);

	// Get the size
	ivec2 sz = textureSize(inRed, 0);
	float sx = 1.0/(float(sz.x));

	// get the data
    for(mv=-nMax; mv<nMax; mv++)
	{
		col = textureLod(inRed, gl_TexCoord[0].st + vec2(mv*sx,0.0), 0.0);
		/*if( col.r>red.r ) red.r = col.r;
		if( col.g>red.g ) red.g = col.g;
		if( col.b>red.b ) red.b = col.b;
		if( col.a>red.a ) red.a = col.a;*/
		red = max(red, col);

		col = textureLod(inGreen, gl_TexCoord[0].st + vec2(mv*sx,0.0), 0.0);
		/*if( col.r>green.r ) green.r = col.r;
		if( col.g>green.g ) green.g = col.g;
		if( col.b>green.b ) green.b = col.b;
		if( col.a>green.a ) green.a = col.a;*/
		green = max(green, col);

		col = textureLod(inBlue, gl_TexCoord[0].st + vec2(mv*sx,0.0), 0.0);
		/*if( col.r>blue.r ) blue.r = col.r;
		if( col.g>blue.g ) blue.g = col.g;
		if( col.b>blue.b ) blue.b = col.b;
		if( col.a>blue.a ) blue.a = col.a;*/
		blue = max(blue, col);
	}
}
