varying vec3 normal;
uniform mat4 HG_ModelMatrix;

void main()
{
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = ftransform();

	vec4 n = vec4(gl_Normal, 0);
	
	//normalize in fragment
	normal = (HG_ModelMatrix * n).xyz;
}
