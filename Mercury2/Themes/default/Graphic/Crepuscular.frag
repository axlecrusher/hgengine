uniform vec4 HG_DepthRange;
uniform sampler2D HG_Texture0;
uniform sampler2D HG_Texture1;

varying vec3 normal;
varying vec3 pos;
varying vec3 posn;
varying vec3 tlipos;

void main()
{
	vec3 rLpos = normalize(tlipos - posn);
	float fbrgt = .7;
	vec2 sspos = gl_TexCoord[0].xy;
	for( int i = 0; i < 50; i++ )
	{
		vec4 SW = texture2D( HG_Texture1, sspos );
		sspos += rLpos.xy * 0.005;
		sspos = clamp( sspos, vec2( 0. ), vec2( .99 ) );
		if( SW.a < 0.5 ) fbrgt += 0.01;
	}

	vec4 IM = texture2D( HG_Texture0, gl_TexCoord[0].xy );
	gl_FragColor = IM * fbrgt;
}

