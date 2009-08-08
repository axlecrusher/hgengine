uniform mat4 HG_ModelMatrix;
uniform vec4 HG_EyePos;
uniform vec4 HG_LightPos;

//uniform vec4 HG_NearClip;
uniform vec4 HG_FarClip;
//uniform vec4 HG_ClipExtends;

varying vec3 ecEye;
varying vec3 ecLight;
varying vec3 ecFrag;

void main()
{
	gl_Position = ftransform();
	ecFrag = (gl_ModelViewMatrix * gl_Vertex).xyz; //world position

	ecEye = (gl_ModelViewMatrix * HG_EyePos).xyz;
	ecLight = (gl_ModelViewMatrix * vec4(0,0,0,1)).xyz;
}
