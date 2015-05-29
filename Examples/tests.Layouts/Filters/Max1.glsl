
uniform sampler2D tex0;  

void main()  
{  
	int i, j;
	vec2 pos = vec2(0.0, 0.0);
	vec4 col = vec4(0.0, 0.0, 0.0, 0.0);
	vec4 fmx = vec4(0.0, 0.0, 0.0, 0.0);

	ivec2 sz = textureSize(tex0, 0);
	float sx = 1.0/(float(sz.x));
	float sy = 1.0/(float(sz.y));

	for( i=-5; i<5; i++)
	{
		for( j=-5; j<5; j++)
		{
			pos   = gl_TexCoord[0].st + vec2( i*sx, j*sy);
			col   = textureLod(tex0, pos, 1.0);
			col.r = (col.r + col.g + col.b)/3.0;
			if( fmx.r < col.r )
			{
				fmx.r = col.r;
				fmx.g = pos.s;
				fmx.b = pos.t;
			}
		}
	}
 
    gl_FragColor = fmx;
}
 
