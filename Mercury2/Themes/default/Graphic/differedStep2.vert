uniform mat4 HG_WorldMatrix;
//vec3 lightPos;

void main()
{
//	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = ftransform();
//	lightPos = (HG_WorldMatrix * vec4(0,0,0,1)).xyz;
}
