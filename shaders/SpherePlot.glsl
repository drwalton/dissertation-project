/* SpherePlot
 * Shader intended to render plots of real-valued
 * spherical functions (SpherePlot objects).
 * Plots are rendered using Gooch shading, in blue
 * or green according to the sign of the function.
 */

--Vertex
#version 150

in vec4 vPosition;
in vec3 vNorm;
in float vPositive;

out vec3 smoothNorm;
out float positive;

uniform mat4 modelToWorld;

layout(std140) uniform cameraBlock
{
	mat4 worldToCamera;
	vec4 cameraPos;
	vec4 cameraDir;
};

void main()
{
	smoothNorm = mat3(modelToWorld) * vNorm;
	gl_Position = worldToCamera * modelToWorld * vPosition;
	positive = vPositive;
}

--Fragment
#version 150

in vec3 smoothNorm;
in vec4 worldPos;
in float positive;

out vec4 fragColor;

layout(std140) uniform cameraBlock
{
	mat4 worldToCamera;
	vec4 cameraPos;
	vec4 cameraDir;
};

const vec4 posWarmColor = vec4(0.0, 0.0, 1.0, 1.0);
const vec4 posCoolColor = vec4(0.0, 0.0, 0.2, 1.0);
const vec4 negWarmColor = vec4(0.0, 1.0, 0.0, 1.0);
const vec4 negCoolColor = vec4(0.0, 0.2, 0.0, 1.0);

void main()
{
	vec3 norm = normalize(smoothNorm);
	float vDotN = max(dot(-cameraDir.xyz, norm), 0.0);
	if(positive < 0.5)
	{
		fragColor = mix(posCoolColor, posWarmColor, vDotN);
	}
	else
	{
		fragColor = mix(negCoolColor, negWarmColor, vDotN);
	}
}
