uniform sampler2D HG_Texture0;
uniform sampler2D HG_Texture1;
uniform sampler2D HG_Texture2;
uniform vec4 HG_EyePos;
uniform ivec4 HG_ViewPort;
uniform vec4 HG_LightPos;
uniform vec4 HG_LightAtten;

void main()
{
	vec2 coord = gl_FragCoord.xy / vec2(HG_ViewPort.zw);
	vec3 pos = texture2D(HG_Texture0, coord).xyz;
	vec3 norm = texture2D(HG_Texture1, coord).xyz;
	float att;
	vec3 color, hv, diffuse, ambient;
	diffuse = vec3(1.0);
	ambient = vec3(0.0);
	norm = normalize(norm);

	vec3 eyeVec = normalize(HG_EyePos.xyz - pos);
	vec3 lightDir = HG_LightPos.xyz - pos;
	float dist = length(lightDir);

	if(dist > HG_LightAtten.w) discard;

	lightDir = normalize(lightDir);

	float NdotL = max(dot(norm, lightDir),0.0);

	color = vec3(0.0);
	hv = normalize(eyeVec - lightDir);
	float NdotHV;

	vec3 materialSpec = vec3(1.0,1.0,1.0);
	vec3 lightSpec = vec3(1.0,1.0,1.0);
	float shine = 0.0;

	//x = constant, y = linear, z = quad
	att = 1.0 / (HG_LightAtten.x +
	HG_LightAtten.y * dist +
	HG_LightAtten.z * dist * dist);

	vec3 c = att * (diffuse * NdotL + ambient);
	color += att * (diffuse * NdotL + ambient);

	NdotHV = max(dot(norm,hv),0.0);

	color += att * materialSpec * lightSpec * pow(NdotHV, shine);

	color *= float(NdotL > 0.0);
	gl_FragColor = vec4(color, 1.0);

//	gl_FragColor = vec4(1.0);
//	gl_FragColor = vec4(lightDir,1.0);
}

