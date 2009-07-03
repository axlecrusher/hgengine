//uniform sampler2D HG_Texture0;
varying vec3 normal;

void main()
{
//	gl_FragData[0] = texture2D(HG_Texture0, gl_TexCoord[0].st);
	vec3 n = normalize(normal);
	gl_FragData[0] = vec4(0.5 + 0.5 * n, 1.0);
}
