//uniform vec4 HG_ParticleTime;
uniform vec4 HG_EyePos;
uniform vec4 HG_LookVector;
uniform mat4 HG_ModelMatrix;
uniform mat4 HG_WorldMatrix;
uniform mat4 HG_ViewMatrix;
varying vec3 angleC;
varying vec4 particleData;

mat4 glRotate(float angle, vec3 axis)
{
	axis=normalize(axis);
	mat4 m = mat4(0);
	float c = cos(angle);
	float s = sin(angle);
	float nc = 1.0-c;

	m[0][0] = (axis.x*axis.x*nc)+c;
	m[0][1] = (axis.x*axis.y*nc)-(axis.z*s);
	m[0][2] = (axis.x*axis.z*nc)+(axis.y*s);

	m[1][0] = (axis.y*axis.x*nc)+(axis.z*s);
	m[1][1] = (axis.y*axis.y*nc)+c;
	m[1][2] = (axis.y*axis.z*nc)-(axis.x*s);

	m[2][0] = (axis.x*axis.z*nc)-(axis.y*s);
	m[2][1] = (axis.y*axis.z*nc)+(axis.x*s);
	m[2][2] = (axis.z*axis.z*nc)+c;

	m[3][3] = 1.0;

	return m;
}

mat4 Billboard(vec3 pos)
{
	vec3 objToEye = (HG_EyePos - HG_ModelMatrix*vec4(pos,1)).xyz;
//	objToEye.y = 0.0;

	vec3 objLookAt = normalize((HG_ModelMatrix * vec4(0,0,-1,0)).xyz);
	objToEye = normalize(objToEye);

	vec3 up = cross(objLookAt, objToEye);
	float angleCos = dot(objLookAt, objToEye);

	//add in an extra rotation around y to make particles look more unique
	float o = 360.0*((particleData.z-50000.0)/50000.0);
	return glRotate(-acos(angleCos), up)*glRotate(o, vec3(0,0,1));
}

void main()
{
 	particleData = gl_Color; 

	vec4 pos = vec4(1.0);
	pos.y = 0.3*(particleData.x*particleData.x);
	pos.x = 0.40*((particleData.z-50000.0)/50000.0)*particleData.x; //rand num
	pos.z = 0.40*((particleData.w-50000.0)/50000.0)*particleData.x; //rand num

	mat4 m = Billboard(pos.xyz);
	m[3].xyz = pos.xyz;

	mat4 s = mat4(0.0);
	s[0][0] = 1.0+3.0*(particleData.x/particleData.y);
	s[1][1] = 1.0+3.0*(particleData.x/particleData.y);
	s[2][2] = 1.0+3.0*(particleData.x/particleData.y);
	s[3][3] = 1.0;
	gl_Position = gl_ProjectionMatrix *HG_ViewMatrix *HG_ModelMatrix *m*s* gl_Vertex;
  	gl_TexCoord[0] = gl_MultiTexCoord0;
}
