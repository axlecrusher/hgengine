varying vec3 color;

uniform vec4 HG_EyePos;
uniform mat4 HG_ModelMatrix;

void main()
{
	gl_Position = ftransform();
	color = vec3(0,0,1);

	vec3 v = normalize(HG_EyePos - (HG_ModelMatrix*vec4(0,0,0,1))).xyz;

	color = (v+1.0)*0.5;
}
