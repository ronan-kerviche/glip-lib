/*
	Game of Life - John Conway - Rule B3S23
*/

#version 130

uniform sampler2D 	inputTexture;	// input sampler
out     vec4 		outputTexture;	// output fragment

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

