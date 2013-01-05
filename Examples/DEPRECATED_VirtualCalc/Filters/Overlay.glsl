/*
	Overlay shader
	tex0 : camera
	tex1 : drawing

	White -> 1.0
	Black -> 0.2

	Ex : 
	1.0 -> 0.0
	0.0 -> 0.5
	
*/

uniform sampler2D tex0, tex1;

void main() 
{
	vec4 cam = textureLod(tex0, gl_TexCoord[0].st, 1.0);
	vec4 drw = textureLod(tex1, gl_TexCoord[0].st, 1.0);
	vec4 col;
	
	col.r = cam.r + (-drw.r*0.5 + 0.5);
	col.g = cam.g + (-drw.g*0.5 + 0.5);
	col.b = cam.b + (-drw.b*0.5 + 0.5);
	col.a = cam.a + (-drw.a*0.5 + 0.5);
	
	gl_FragColor = col;
}

