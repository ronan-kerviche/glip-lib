/*
	Histogram, fragment shader
*/


void main()  
{
	// if there is a pixel to stack here, add the following to the current stack!
	// Constant is given by 1/(640*480) (weight of a pixel in the histogram)
	if(gl_Color.b>0.5)
		gl_FragColor = vec4(0.000003255,0.000003255,0.000003255,1.0);
	else
		gl_FragColor = vec4(0.0,0.0,0.0,0.0);   // this line will always be unused (just to get the balance)
}
