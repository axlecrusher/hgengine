uniform sampler2D HG_Texture0;
uniform sampler2D HG_Texture1;

uniform vec4 HG_EyePos;
uniform ivec4 HG_ViewPort;
uniform vec4 HG_LightPos;
uniform vec4 HG_LightAtten;
uniform vec4 HG_LightColor;

uniform mat4 HG_ModelMatrix;
uniform vec4 HG_DepthRange;
uniform vec4 HG_LookVector;


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

	float NdotL = max(dot(norm, lightDir),0.0);

	if( dist > HG_LightAtten.w ) discard;

	float att = 1.0 / (HG_LightAtten.x + HG_LightAtten.y * dist +
	HG_LightAtten.z * dist * dist);

	vec3 diffuse = texture2D(HG_Texture1, coord).rgb;

	vec3 R = reflect(-lightDir, norm);
	vec3 specular = diffuse * pow( max(dot(R,HG_LookVector.xyz), 0.0), 10.0 );

	diffuse	*= HG_LightColor.rgb * NdotL;

	vec3 color = diffuse;

	color += specular;
	color *= att;

	gl_FragColor.rgb = clamp(color, 0.0, 1.0);
}
