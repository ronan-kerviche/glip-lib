
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

	for( i=0; i<sz.x; i++)
	{
		for( j=0; j<sz.y; j++)
		{
			pos = vec2( i*sx, j*sy);
			col = textureLod(tex0, pos, 1.0);
			if( col.r < fmx.r) fmx.r = col.r;
			if( col.g > fmx.g) fmx.g = col.g;
			if( col.b < fmx.b) fmx.b = col.b;
			if( col.a > fmx.a) fmx.a = col.a;
		}
	}
 
    gl_FragColor = fmx;
}
