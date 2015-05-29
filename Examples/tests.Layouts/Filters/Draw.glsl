/*
	Perform drawing on surface tex0 in coordinates given by tex1.
	Draw BLACK.

	tex0 : input position
	tex1 : input last
*/

uniform sampler2D tex0, tex1; 
uniform float radius;

void main()  
{
	vec4 pos = texture(tex0, vec2(0.5, 0.5) );
	vec4 col = texture(tex1, gl_TexCoord[0].st);

	ivec2 sz = textureSize(tex1, 0);
	float sx = 1.0/(float(sz.x));
	float sy = 1.0/(float(sz.y));

	if( distance(gl_TexCoord[0].st, pos.gb) < radius )
		col = vec4(0.0, 0.0, 0.0, 0.0);

	gl_FragColor = col;
}
