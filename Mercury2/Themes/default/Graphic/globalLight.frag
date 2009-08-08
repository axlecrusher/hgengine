uniform sampler2D HG_Texture0;
uniform vec4 HG_EyePos;
uniform ivec4 HG_ViewPort;
uniform vec4 HG_LightPos;
uniform vec4 HG_LightAtten;
uniform vec4 HG_LightColor;

uniform mat4 HG_ModelMatrix;
uniform vec4 HG_DepthRange;

varying vec3 ecEye;
varying vec3 ecLight;
varying vec3 ecFrag;

vec3 SphericalToCartesian(vec2 spherical)
{
  vec2 sinCosTheta, sinCosPhi;

  spherical = spherical * 2.0 - 1.0;
  sincos(spherical.x * 3.14159, sinCosTheta.x, sinCosTheta.y);
  sinCosPhi = vec2(sqrt(1.0 - spherical.y * spherical.y), spherical.y);

  return vec3(sinCosTheta.y * sinCosPhi.x, sinCosTheta.x * sinCosPhi.x, sinCosPhi.y);    
}

void main()
{
	vec2 coord = gl_FragCoord.xy / vec2(HG_ViewPort.zw - HG_ViewPort.xy);
	vec4 t1 = texture2D(HG_Texture0, coord);
	vec3 norm = SphericalToCartesian( t1.rg );
	float depth = t1.b;

	vec3 ray = ecFrag * (HG_DepthRange.y/-ecFrag.z);
	vec3 pos = ray * depth;

	vec3 eyeVec = normalize(ecEye - pos);
	vec3 lightDir = ecLight - pos;

	float dist = length(lightDir);
	lightDir /= dist; //normalize
//	gl_FragColor = vec4(vec3(mod(pos,0.998)), 1.0);
//	gl_FragColor = vec4((ray+1.0)*0.5, 1.0);

	float NdotL = max(dot(norm, lightDir),0.0);

	if((dist > HG_LightAtten.w) || (NdotL <= 0.0)) discard;

	vec3 materialSpec = vec3(1.0,1.0,1.0);
	vec3 lightSpec = vec3(1.0,1.0,1.0);

	//x = constant, y = linear, z = quad
	float att = 1.0 / (HG_LightAtten.x +
	HG_LightAtten.y * dist +
	HG_LightAtten.z * dist * dist);

	vec3 color = att * (HG_LightColor.rgb * NdotL);

	vec3 hv = normalize( lightDir+(ecEye-pos) );
	float NdotHV = max(dot(norm,hv),0.0);

	//pow(max(dot(H, normal.xyz), 0.0)

	color += att * materialSpec * lightSpec * pow(max(NdotHV, 0.0), 3.0);
	color = clamp(color, 0.0, 1.0);
	gl_FragColor = vec4(color, 1.0);
}
