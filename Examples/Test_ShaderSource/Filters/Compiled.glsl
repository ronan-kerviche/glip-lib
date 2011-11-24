
uniform sampler2D tex0, tex1, tex2;

void main() 
{
	vec4  a   =  textureLod(tex0, gl_TexCoord[0].st, 1.0);
	vec4  ma  =  textureLod(tex1, vec2(0.5,0.5), 1.0);
	vec4  bmb = (textureLod(tex2, gl_TexCoord[2].st, 1.0) - vec4(0.5, 0.5, 0.5, 0.5))*2.0;
	float ama = (a.r+a.g+a.b)/3.0-ma.r;
	vec4  col = vec4( ama*ama, ama*bmb.r, 0.0, 0.0); 
	
	gl_FragColor = (col/2.0) + vec4(0.5, 0.5, 0.5, 0.5);
}

