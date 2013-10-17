/* PRTBake
 * Shader intended for rendering per-vertex RGB
 * transfer function coefficients to a framebuffer.
 */

--Vertex
#version 150

in vec2 vTexCoord;
in vec3 vCoefft;

out vec3 smoothCoefft;

void main()
{
	smoothCoefft = vCoefft;
	//Transform tex coords to NDC
	vec2 NDC = (vTexCoord * 2.0) - vec2(1.0, 1.0);
	NDC.y = -NDC.y;
	gl_Position = vec4(NDC, 0.0, 1.0);
}

--Fragment
#version 150

in vec3 smoothCoefft;

out vec4 fragColor;

void main()
{
	fragColor = vec4(abs(smoothCoefft), smoothCoefft.x > 0.0 ? 1.0 : 0.1);
}
