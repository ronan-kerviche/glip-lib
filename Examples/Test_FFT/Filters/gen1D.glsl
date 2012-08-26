#version 130

out vec4 outTexture;

void main()
{
	outTexture = vec4(0.0,0.0,0.0,0.0);

	// Sinusoid :
	//outTexture.r = cos(4.0*gl_TexCoord[0].s*3.14159265)/2.0+0.5;

	// Square :
	if(abs(gl_TexCoord[0].s-0.5)<0.01)
		outTexture.r = 1.0;
}
