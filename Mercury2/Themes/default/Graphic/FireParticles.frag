uniform sampler2D HG_Texture0;
varying vec4 particleData;

vec3 Blend(vec3 c1, vec3 c2, float p)
{
	return c1*(1.0-p) + c2*p;
}

void main()
{
	float pComplete = (particleData.x/particleData.y);

	vec3 blue = vec3(0,0,1);
	vec3 orange = vec3(1,0.917647059,0.1372549);
	vec3 color = vec3(0,0,0);
	
	//blues
	color += Blend(blue, orange, min(1.0,pComplete*3.33))*float(pComplete<0.5);
	color += Blend(orange, vec3(-3), (pComplete-0.5)*2.0)*float(pComplete>0.5);

//	color += blue*(1.0-(pComplete*3.33))*float(pComplete<0.3); //blue
//	color += orange*(pComplete*3.33)*float(pComplete<0.3); //orange

	//orange to black fade
//	color += 1.25*orange*(1.0-(pComplete-0.2))*float(pComplete>0.2); //orange
//	color += vec4(-1,-1,-1,1)*(pComplete)*float(pComplete>0.2); //smoke -1 offset

	color.rgb *= texture2D(HG_Texture0, gl_TexCoord[0].st).a;
	color.rgb *= 1.0-(particleData.x/particleData.y);

	gl_FragData[0].rgb = texture2D(HG_Texture0, gl_TexCoord[0].st).rgb*color.rgb;
	gl_FragData[0].a = texture2D(HG_Texture0, gl_TexCoord[0].st).a;
	gl_FragData[0].a *= 1.0-(particleData.x/particleData.y);
}
