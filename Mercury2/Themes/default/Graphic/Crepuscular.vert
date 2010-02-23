uniform vec4 HG_EyePos;
uniform mat4 HG_ModelMatrix;
uniform vec4 HG_DepthRange;


varying vec3 normal;
varying vec3 pos;
varying vec3 posn;
varying vec3 tlipos;

void main()
{
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = gl_Vertex * vec4( 2., 2., 1., 1. );

	vec4 n = vec4(gl_Normal, 0.);
	
	//viewspace normal
	normal = (gl_ModelViewMatrix * n).xyz;

	//clip space depth
	pos = (gl_ModelViewMatrix * gl_Vertex).xyz;
	posn = (gl_ModelViewMatrix * vec4( gl_Vertex.xy*2., 2.2, 0.0 ) ).xyz;
	tlipos = (gl_ModelViewMatrix * normalize(vec4( .8, .4, .7, 0.0 )) ).xyz;
}
