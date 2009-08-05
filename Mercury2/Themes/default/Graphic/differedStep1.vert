uniform mat4 HG_ModelMatrix;

varying vec3 normal;
varying vec3 pos;

void main()
{
//	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = ftransform();

	vec4 n = vec4(gl_Normal, 0);
	
	//normalize in fragment
	normal = (HG_ModelMatrix * n).xyz;

	pos = (HG_ModelMatrix * gl_Vertex).xyz;
}
