/*
	Game of Life - John Conway - Rule B3S23
*/

#version 130
precision mediump float;

uniform sampler2D 	inputTexture;	// input sampler
out     vec4 		outputTexture;	// output fragment

void main()
{
	// Get the size of the texture and compute the pixel size in the normalized coordinates : 
	ivec2 sz 	= textureSize(inputTexture, 0);
	float sx 	= 1.0/(float(sz.x));
	float sy 	= 1.0/(float(sz.y));

	// Read all the eight previous and surrounding cells :
	vec4 a00	= textureLod(inputTexture, gl_TexCoord[0].st + vec2(-sx,-sy)	, 0);
	vec4 a01	= textureLod(inputTexture, gl_TexCoord[0].st + vec2(-sx,0.0)	, 0);
	vec4 a02	= textureLod(inputTexture, gl_TexCoord[0].st + vec2(-sx,+sy)	, 0);
	vec4 a10	= textureLod(inputTexture, gl_TexCoord[0].st + vec2(0.0,-sy)	, 0);
	vec4 a11	= textureLod(inputTexture, gl_TexCoord[0].st			, 0);
	vec4 a12	= textureLod(inputTexture, gl_TexCoord[0].st + vec2(0.0,+sy)	, 0);
	vec4 a20	= textureLod(inputTexture, gl_TexCoord[0].st + vec2(+sx,-sy)	, 0);
	vec4 a21	= textureLod(inputTexture, gl_TexCoord[0].st + vec2(+sx,0.0)	, 0);
	vec4 a22	= textureLod(inputTexture, gl_TexCoord[0].st + vec2(+sx,+sy)	, 0);

	// Compute the number of cell alive during last process :
	float s = a00.r+a01.r+a02.r+a10.r+a12.r+a20.r+a21.r+a22.r;

	// Older cell are shifted in the next channel :
	outputTexture.g = a11.r;
	outputTexture.b = a11.g;
	outputTexture.a = 1.0; // opaque texture

	// New :
	if(a11.r==1.0) // The center cell was previously alive
	{
		if((s==2.0) || (s==3.0)) 
			outputTexture.r = 1.0; // It stays alive
		else
			outputTexture.r = 0.0; // It dies
	}
	else
	{
		if(s==3.0)
			outputTexture.r = 1.0; // Birth
		else
			outputTexture.r = 0.0; // Stays dead
	}
}
