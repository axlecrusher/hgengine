uniform sampler2D HG_Texture0;
varying vec4 particleData;

void main()
{
	float pComplete = (particleData.x/particleData.y);

	vec4 blue = vec4(0,0,1,0.125);
	vec4 orange = vec4(1,0.917647059,0.11372549,1);
	vec4 color = vec4(0,0,0,0);
	
	//blues
	color += blue*(1.0-(pComplete*5.0))*float(pComplete<0.2); //blue
	color += 1.25*orange*(pComplete*5.0)*float(pComplete<0.2); //orange

	//orange to black fade
	color += 1.25*orange*(1.0-(pComplete-0.2))*float(pComplete>0.2); //orange
	color += vec4(-1,-1,-1,1)*(pComplete)*float(pComplete>0.2); //smoke -1 offset

	gl_FragData[0] = texture2D(HG_Texture0, gl_TexCoord[0].st)*color;
	gl_FragData[0].a *= 1.0-(particleData.x/particleData.y);
}