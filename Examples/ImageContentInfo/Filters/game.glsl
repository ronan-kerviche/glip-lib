/**

	Game of life : B2S23

**/

#version 130

uniform sampler2D 	inText;
out     vec4 		outText;

	void main()
	{
		// Read all the eight previous :
		ivec2 sz 	= textureSize(inText, 0);
		float sx 	= 1.0/(float(sz.x));
		float sy 	= 1.0/(float(sz.y));
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

		// Older :
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
	}
