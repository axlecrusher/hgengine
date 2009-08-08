varying vec3 normal;
varying vec3 pos;
varying float dist;
uniform vec4 HG_DepthRange;
uniform vec4 HG_EyePos;

vec2 CartesianToSpherical(vec3 cartesian)
{
  vec2 spherical;

  spherical.x = atan2(cartesian.y, cartesian.x) / 3.14159;
  spherical.y = cartesian.z;

  return spherical * 0.5 + 0.5;
}

void main()
{
	//linear clip space depth
	float d = -dist/HG_DepthRange.z;

	//store normal, a unused
	vec3 n = normalize(normal);
	n.xy = CartesianToSpherical( n );
	gl_FragData[0] = vec4( n.xy, d, 1.0);
}

