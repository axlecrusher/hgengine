uniform vec4 HG_ParticleTime;
uniform vec4 HG_EyePos;
uniform vec4 HG_LookVector;
uniform mat4 HG_ModelMatrix;
uniform mat4 HG_WorldMatrix;
uniform mat4 HG_ViewMatrix;
varying vec3 angleC;

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

	return glRotate(-acos(angleCos), up);
}

void main()
{
	vec4 pos = vec4(1.0);
	pos.y = 0.30*(HG_ParticleTime.x*HG_ParticleTime.x);
	pos.x = 0.40*((HG_ParticleTime.z-50000.0)/50000.0)*HG_ParticleTime.x; //rand num
	pos.z = 0.40*((HG_ParticleTime.w-50000.0)/50000.0)*HG_ParticleTime.x; //rand num

	mat4 m = Billboard(pos.xyz);
	m[3].xyz = pos.xyz;

	gl_Position = gl_ProjectionMatrix *HG_ViewMatrix *HG_ModelMatrix *m* gl_Vertex;
  
  	gl_TexCoord[0] = gl_MultiTexCoord0;
}
