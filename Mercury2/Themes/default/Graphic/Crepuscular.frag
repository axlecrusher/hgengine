uniform vec4 HG_DepthRange;
uniform sampler2D HG_Texture0;
uniform sampler2D HG_Texture1;

varying vec3 normal;
varying vec3 pos;
varying float dist;

void main()
{
	float fbrgt = 0.1;
	vec2 sspos = gl_TexCoord[0].xy;
	for( int i = 0; i < 100; i++ )
	{
		vec4 SW = texture2D( HG_Texture1, sspos );
		sspos += 0.001;
		sspos = clamp( sspos, vec2( 0. ), vec2( .99 ) );
		if( SW.a < 0.5 ) fbrgt += 0.01;
	}

	vec4 IM = texture2D( HG_Texture0, gl_TexCoord[0].xy );
	gl_FragColor = IM * fbrgt;
}

