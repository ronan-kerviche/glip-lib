uniform sampler2D tex0; 

void main()
{  
	int i, j;
	vec4 col = vec4(0.0, 0.0, 0.0, 0.0);

	ivec2 sz = textureSize(tex0, 0);
	float sx = 1.0/(float(sz.x));
	float sy = 1.0/(float(sz.y));

	for( i=0; i<sz.x; i++)
	{
		for( j=0; j<sz.y; j++)
			col = col + (textureLod(tex0, vec2( i*sx, j*sy),1.0) - vec4(0.5,0.5,0.5,0.5))*2.0;
	}
 
	col = col/3072.0;

    gl_FragColor = col/2.0 + vec4(0.5,0.5,0.5,0.5);
} 
