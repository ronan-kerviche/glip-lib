/*
	Histogram, vertex shader
*/

uniform sampler2D tex0;

void main()  
{
	// Vertex texture fetching :
	vec4 col = textureLod(tex0, gl_Vertex+vec4(0.5,0.5,0.0,0.0), 0.0);

	// "Payload"
	gl_FrontColor = vec4(0.0,0.0,1.0,1.0);

	// Compute the new position!
	float tmp = (col.r + col.g + col.b)/3.0;
	gl_Vertex.x = (tmp-0.5)*2.0;
	gl_Vertex.y = 0.0;

	// Done
	gl_Position = gl_Vertex;
}
