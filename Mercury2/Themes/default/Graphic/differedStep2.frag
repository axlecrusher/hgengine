uniform sampler2D HG_Texture0;
uniform sampler2D HG_Texture1;
uniform sampler2D HG_Texture2;
uniform vec4 HG_EyePos;
uniform ivec4 HG_ViewPort;
uniform vec4 HG_LightPos;
uniform vec4 HG_LightAtten;
uniform vec4 HG_LightColor;

void main()
{
	vec2 coord = gl_FragCoord.xy / vec2(HG_ViewPort.zw);
	vec3 pos = texture2D(HG_Texture0, coord).xyz;
	vec3 norm = texture2D(HG_Texture1, coord).xyz;

	norm = normalize(norm);

	vec3 eyeVec = normalize(HG_EyePos.xyz - pos);
	vec3 lightDir = HG_LightPos.xyz - pos;

	float dist = length(lightDir);
	lightDir /= dist; //normalize
	float NdotL = max(dot(norm, lightDir),0.0);

	if((dist > HG_LightAtten.w) || (NdotL <= 0.0)) discard;

	vec3 materialSpec = vec3(1.0,1.0,1.0);
	vec3 lightSpec = vec3(1.0,1.0,1.0);

	//x = constant, y = linear, z = quad
	float att = 1.0 / (HG_LightAtten.x +
	HG_LightAtten.y * dist +
	HG_LightAtten.z * dist * dist);

	vec3 color = att * (HG_LightColor.rgb * NdotL);

	vec3 hv = normalize( lightDir+(HG_EyePos.xyz-pos) );
	float NdotHV = max(dot(norm,hv),0.0);

	//pow(max(dot(H, normal.xyz), 0.0)

	color += att * materialSpec * lightSpec * pow(max(NdotHV, 0.0), 100.0);
	gl_FragColor.rgb = clamp(color, 0.0, 1.0);
}

