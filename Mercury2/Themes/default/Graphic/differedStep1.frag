varying vec3 normal;
varying vec3 pos;

void main()
{
	//store position
	gl_FragData[0] = vec4(pos, 1.0);

	//store normal, a unused
	gl_FragData[1] = vec4( (normalize(normal)+1.0)*0.5, 1.0);
}

