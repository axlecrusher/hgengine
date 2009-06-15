uniform sampler2D HG_Texture0;
varying vec3 normal;

void main()
{
//	gl_FragData[0] = texture2D(HG_Texture0, gl_TexCoord[0].st);
	gl_FragData[0].rgb = (normalize(normal)+1.0)/2.0;
}
