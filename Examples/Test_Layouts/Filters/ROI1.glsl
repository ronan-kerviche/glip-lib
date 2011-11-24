
uniform sampler2D tex0;

void main()  
{  
	int i, j;
	vec2 pos = vec2(0.0, 0.0);
	vec4 col = vec4(0.0, 0.0, 0.0, 0.0);
	vec4 fmx = vec4(1.0, 0.0, 1.0, 0.0);

	ivec2 sz = textureSize(tex0, 0);
	float sx = 1.0/(float(sz.x));
	float sy = 1.0/(float(sz.y));

	for( i=-5; i<5; i++)
	{
		for( j=-5; j<5; j++)
		{
			pos   = gl_TexCoord[0].st + vec2( i*sx, j*sy);
			col   = textureLod(tex0, pos, 1.0);
			if( col.r<0.9 )
			{
				if( pos.s < fmx.r) fmx.r = pos.s;
				if( pos.s > fmx.g) fmx.g = pos.s;
				if( pos.t < fmx.b) fmx.b = pos.t;
				if( pos.t > fmx.a) fmx.a = pos.t;
			}
		}
	}
 
    gl_FragColor = fmx;
}
