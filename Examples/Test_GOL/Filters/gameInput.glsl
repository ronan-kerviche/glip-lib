#version 130

out     vec4 	output1,output2;
uniform int	t;

float rand(vec2 co)
{
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main()
{
	float 	a = rand(gl_TexCoord[0].st*t),
		b = rand(gl_TexCoord[0].st*t);

	if(a<0.8)
		a = 0.0;
	else
		a = 1.0f;

	if(b<0.8)
		b = 0.0;
	else
		b = 1.0f;

	output1 = vec4(a,a,a,a);
	output2 = vec4(b,b,b,b);
}
