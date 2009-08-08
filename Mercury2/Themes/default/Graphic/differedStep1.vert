uniform vec4 HG_EyePos;
uniform mat4 HG_ModelMatrix;
uniform vec4 HG_DepthRange;
varying vec3 normal;
varying vec3 pos;
varying float dist;

void main()
{
//	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = ftransform();

	vec4 n = vec4(gl_Normal, 0);
	
	//viewspace normal
	normal = (gl_ModelViewMatrix * n).xyz;

	//clip space depth
	pos = (gl_ModelViewMatrix * gl_Vertex).xyz;
	dist = pos.z;
}
