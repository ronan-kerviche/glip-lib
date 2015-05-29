/*
	Game of Life - John Conway - Rule B3S23
*/

#version 130
uniform sampler2D inputTexture;
out vec4 outputTexture;

void main()
{
	// Reqd and count the nature of the surrounding cells :
	int alive = 0;
	for(int i=-1; i<=1; i++)
	{
		for(int j=-1; j<=1; j++)
		{
			vec4 c = texelFetch(inputTexture, ivec2(gl_FragCoord.xy) + ivec2(j, i), 0);
			alive += int(c.r>=1.0);
		}
	}

	// Older cell are shifted in the next channel :
	vec4 c = texelFetch(inputTexture, ivec2(gl_FragCoord.xy), 0);
	outputTexture.gba = vec3(c.rg, 1.0);

	// New state :
	if(c.r>=1.0) // The center cell was previously alive
	{
		if((alive==2) || (alive==3))
			outputTexture.r = 1.0; // It stays alive
		else
			outputTexture.r = 0.0; // It dies
	}
	else
	{
		if(alive==3)
			outputTexture.r = 1.0; // Birth
		else
			outputTexture.r = 0.0; // Stays dead
	}
}


/*#version 130
precision mediump float;

uniform sampler2D 	inText;		// input sampler
out     vec4 		outText;	// output sampler

void main()
{
	ivec2 sz 	= textureSize(inText, 0);
	float sx 	= 1.0/(float(sz.x));
	float sy 	= 1.0/(float(sz.y));

	// Read all the eight previous and surrounding cells :
	vec4 a00	= textureLod(inText, gl_TexCoord[0].st + vec2(-sx,-sy)	, 0);
	vec4 a01	= textureLod(inText, gl_TexCoord[0].st + vec2(-sx,0.0)	, 0);
	vec4 a02	= textureLod(inText, gl_TexCoord[0].st + vec2(-sx,+sy)	, 0);
	vec4 a10	= textureLod(inText, gl_TexCoord[0].st + vec2(0.0,-sy)	, 0);
	vec4 a11	= textureLod(inText, gl_TexCoord[0].st			, 0);
	vec4 a12	= textureLod(inText, gl_TexCoord[0].st + vec2(0.0,+sy)	, 0);
	vec4 a20	= textureLod(inText, gl_TexCoord[0].st + vec2(+sx,-sy)	, 0);
	vec4 a21	= textureLod(inText, gl_TexCoord[0].st + vec2(+sx,0.0)	, 0);
	vec4 a22	= textureLod(inText, gl_TexCoord[0].st + vec2(+sx,+sy)	, 0);

	float s = a00.r+a01.r+a02.r+a10.r+a12.r+a20.r+a21.r+a22.r;

	// Older cell are shifted in the next channel :
	outText.g = a11.r;
	outText.b = a11.g;
	outText.a = 1.0;

	// New :
	if(a11.r==1.0)
	{
		if((s==2.0) || (s==3.0))
			outText.r = 1.0;
		else
			outText.r = 0.0;
	}
	else
	{
		if(s==3.0)
			outText.r = 1.0;
		else
			outText.r = 0.0;
	}
}*/
