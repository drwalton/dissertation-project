#ifndef SHADER_HPP
#define SHADER_HPP

#include <GL/glew.h>
#include <glm.hpp>

#include <string>
#include <vector>

struct Material
{
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;
	float exponent;
};

/* Shader
 * Handles opening & compiling glsl source from a file, and setting uniforms.
 */
class Shader
{
public:
	Shader(bool hasGeomShader, const std::string& filename,
		bool hasCamera = true, bool hasModelToWorld = true);
	Shader(bool hasGeomShader, const std::string& filename,
		std::vector<std::string> subs,
		bool hasCamera = true, bool hasModelToWorld = true);
	virtual ~Shader();
	void use();
	void setModelToWorld(const glm::mat4& _modelToWorld);
	GLuint getAttribLoc(const std::string& name);

	virtual void setMaterial(unsigned index, const Material& material) {};
	virtual void setMaterials(const std::vector<Material> _materials) {};

	const std::string filename;
	static GLuint getUBlockBindingIndex(const std::string& name);
	static void gsub(
		std::string& input,
		const std::string& pattern,
		const std::string& replacement);
protected:
	GLuint getUniformLoc(const std::string& name);
	void setupUniformBlock(const std::string& name);

	static const std::vector<std::string> PHONG_SUBS;
	static const std::vector<std::string> SH_SUBS;
private:
	GLuint id;
	GLuint loadShader(const std::string& filename,
	int shaderType, bool DEBUG, std::vector<std::string> subs);
	GLuint compileShader(const std::string& filename,
		bool hasGeomShader, bool DEBUG,	std::vector<std::string> subs);
	GLuint modelToWorld_u;
	GLuint cameraBlock_i;
};

/* ParticleShader
 * A Shader with additional setters for uniforms related to particle systems.
 * Designed to handle shaders for the AdvectParticle class.
 */
class ParticleShader : public Shader
{
public:
	ParticleShader(bool hasGeomShader, bool hasBBTex, const std::string& filename,
		bool hasCamera = true, bool hasModelToWorld = true);
	void setAlpha(float alpha);
	void setBBWidth(float _bbWidth);
	void setBBHeight(float _bbHeight);
	void setBBTexUnit(GLuint _bbTexUnit);
	void setDecayTexUnit(GLuint _decayTexUnit);
private:
	GLuint alpha_u;
	GLuint bbWidth_u;
	GLuint bbHeight_u;
	GLuint bbTex_u;
	GLuint decayTex_u;
};

class CubemapShader : public ParticleShader
{
public:
	CubemapShader(bool hasGeomShader, bool hasBBTex, const std::string& filename);
	void setWorldToObject(const glm::mat4& worldToObject);
	void setRotation(const glm::mat4& rotation);
	void setPerspective(const glm::mat4& perspective);
private:
	GLuint worldToObject_u;
	GLuint rotation_u;
	GLuint perspective_u;
};

/* LightShader
 * A Shader with additional setters for uniforms related to light sources.
 * Designed for objects illuminated by ambient, point and directional lights
 * (e.g. Solid)
 */
class LightShader : public Shader
{
public:
	LightShader(bool hasGeomShader, const std::string& filename);
	LightShader(bool hasGeomShader, const std::string& filename,
		std::vector<std::string> subs);

	void setAmbTexUnit(GLuint ambTexUnit);
	void setDiffTexUnit(GLuint diffTexUnit);
	void setSpecTexUnit(GLuint specTexUnit);
	void setSpecExp(float exponent);
	void setWorldToCamera(const glm::mat4& _worldToCamera);
private:
	void init();

	int maxPhongLights;

	GLuint ambBlock_i;

	GLuint ambTex_u;
	GLuint diffTex_u;
	GLuint specTex_u;
	GLuint specExp_u;
};

class SHShader : public Shader
{
public:
	SHShader(bool hasGeomShader, const std::string& filename);
	SHShader(bool hasGeomShader, const std::string& filename,
		std::vector<std::string> subs);
	void setTexUnit(GLuint unit);
private:
	void init();
	GLuint texUnit_u;
};

class AOShader : public LightShader
{
public:
	AOShader(bool hasGeomShader, const std::string& filename);
	AOShader(bool hasGeomShader, const std::string& filename,
		std::vector<std::string> subs);
};

#endif
