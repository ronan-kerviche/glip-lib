#version 130

out     vec4 	outText;
uniform int	t;

void main()
{
	float tf = t;

	outText.g = 0.5*(0.5*sin(t*2.0*gl_TexCoord[0].s*3.1415));
	outText.b = 0.5*(0.5*sin(t*2.0*gl_TexCoord[0].t*3.1415));

	if( mod(gl_TexCoord[0].s*100.0,t)==0 || mod(gl_TexCoord[0].t*100.0,t)==0)
		outText.r = 1.0;
	else
		outText.r = 0.0;
}
