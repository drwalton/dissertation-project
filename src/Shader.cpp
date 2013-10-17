#include "Shader.hpp"
#include "Exception.hpp"

#include "glsw.h"
#include "GC.hpp"

#include <gtc/matrix_transform.hpp>

#include <iostream>

std::string phong_subs[2] = 
{
	"$maxPhongLights$", std::to_string(static_cast<long long>(GC::maxPhongLights))
};

std::string sh_subs[2] = 
{
	"$nSHCoeffts$", std::to_string(static_cast<long long>(GC::nSHCoeffts))
};


const std::vector<std::string> Shader::PHONG_SUBS(phong_subs, phong_subs+2);
const std::vector<std::string> Shader::SH_SUBS(sh_subs, sh_subs+2);

Shader::Shader(bool hasGeomShader, const std::string& filename,
	bool hasCamera, bool hasModelToWorld)
	:filename(filename)
{
	std::vector<std::string> subs;
	id = compileShader(filename, hasGeomShader, true, subs);
	if(hasModelToWorld) modelToWorld_u = getUniformLoc("modelToWorld");
	if(hasCamera) setupUniformBlock("cameraBlock");
}

Shader::Shader(bool hasGeomShader, const std::string& filename,
	std::vector<std::string> subs, bool hasCamera, bool hasModelToWorld)
	:filename(filename)
{
	id = compileShader(filename, hasGeomShader, true, subs);
	if(hasModelToWorld) modelToWorld_u = getUniformLoc("modelToWorld");
	if(hasCamera) setupUniformBlock("cameraBlock");
}

Shader::~Shader()
{
	glDeleteProgram(id);
}

void Shader::use()
{
	glUseProgram(id);
}

void Shader::setModelToWorld(const glm::mat4& modelToWorld)
{
	use();
	glUniformMatrix4fv(modelToWorld_u, 1, GL_FALSE, &(modelToWorld[0][0]));
	glUseProgram(0);
}

GLuint Shader::loadShader(const std::string& filename,
	int shaderType, bool DEBUG, std::vector<std::string> subs)
{
	glswInit();
	glswSetPath("../shaders/", ".glsl");

	const char* source;
	switch(shaderType)
	{
		case GL_VERTEX_SHADER:
			source = glswGetShader((filename + ".Vertex").c_str());
			break;
		case GL_FRAGMENT_SHADER:
			source = glswGetShader((filename + ".Fragment").c_str());
			break;
		case GL_GEOMETRY_SHADER:
			source = glswGetShader((filename + ".Geometry").c_str());
			break;
		default:
			if(DEBUG) std::cout <<"!! loadShader error: invalid shader type\n";
			return 0;
			break;
	}
	if(!source) throw Exception("Unable to find shader of name " + filename +".glsl in ../shaders/\n");

	GLuint shaderObject = glCreateShader(shaderType);
	std::string modSource(source);

	if(subs.size() > 0)
	{
		for(auto i = subs.begin(); i != subs.end(); i += 2)
			Shader::gsub(modSource, *i, *(i + 1));

		const char* src = modSource.c_str();
		glShaderSource(shaderObject, 1, &src, 0);
	}
	else
		glShaderSource(shaderObject, 1, &source, 0);

	glCompileShader(shaderObject);

	GLint compiled;
	glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &compiled);
	if (compiled)
	{
	   if(DEBUG) std::cout <<
			((shaderType == GL_VERTEX_SHADER) ? "> Vertex" :
			(shaderType == GL_FRAGMENT_SHADER) ? "> Fragment" :
			"> Geometry") << " shader compiled successfully.\n";
	   return shaderObject;
	}
	else
	{
		if(DEBUG)
		{
			GLchar messages[256];
			glGetShaderInfoLog(shaderObject, sizeof(messages), 0, &messages[0]);
			std::cout << "!! Compilation error:\n" << messages;
			std::cout << "Loaded source:\n" << ((subs.size() > 0) ? modSource : source);
		}
		throw Exception("Shader from filename " + filename + "could not be compiled.\n");
	}
	glswShutdown();
}

GLuint Shader::compileShader(const std::string& filename,
		bool hasGeomShader, bool DEBUG,	std::vector<std::string> subs)
{
	if(DEBUG) std::cout << "Attempting to load shaders from ../shaders/" << filename << ".glsl" << std::endl;

	GLuint vertexShader = loadShader(filename, GL_VERTEX_SHADER, DEBUG, subs);
	GLuint fragmentShader = loadShader(filename, GL_FRAGMENT_SHADER, DEBUG, subs);
	GLuint geomShader = 1;
	if(hasGeomShader) geomShader = 
		loadShader(filename, GL_GEOMETRY_SHADER, DEBUG, subs);

	if(!vertexShader || !fragmentShader || !geomShader)
		return 0;

	GLuint program = glCreateProgram();

	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	if(hasGeomShader) glAttachShader(program, geomShader);

	glLinkProgram(program);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	if(hasGeomShader) glDeleteShader(geomShader);

	GLint linked;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	if (linked)
	{
	   if(DEBUG) std::cout << "> Program linked successfully.\n";
	   return program;
	}
	else
	{
		if(DEBUG) 
		{
			std::cout << "!! Program linking failed.\n";
			GLchar messages[256];
			glGetProgramInfoLog(program, sizeof(messages), 0, &messages[0]);
			std::cout << "!! Compilation error:\n" << messages;
		}
		throw Exception("Shader from filename " + filename + "could not be compiled.\n");
	}
}

GLuint Shader::getAttribLoc(const std::string& name)
{
	GLuint loc = glGetAttribLocation(id, name.c_str());
	if (loc == -1) throw Exception("Unable to find attribute of name " + name + " in shader.\n");
	return loc;
}

GLuint Shader::getUBlockBindingIndex(const std::string& name)
{
	if (name.compare("cameraBlock") == 0) return 0;
	if (name.compare("ambBlock")    == 0) return 1;
	if (name.compare("phongBlock")  == 0) return 2;
	if (name.compare("SHBlock")     == 0) return 3;
	return -1; // Name not found
}

void Shader::gsub(
	std::string& input,
	const std::string& pattern,
	const std::string& replacement)
{
	if(input.empty() || pattern.empty()) return;
	size_t position = input.find(pattern, 0);
	while(position != std::string::npos)
	{
		input.replace(position, pattern.length(), replacement);
		position += replacement.length();
		position = input.find(pattern, position);
	}
}

GLuint Shader::getUniformLoc(const std::string& name)
{
	GLuint loc = glGetUniformLocation(id, name.c_str());
	if (loc == -1) throw Exception("Unable to find uniform of name " + name + " in shader.\n");
	return loc;
}

void Shader::setupUniformBlock(const std::string& name)
{
	GLuint unfIndex = glGetUniformBlockIndex(id, name.c_str());
	GLuint bindIndex = getUBlockBindingIndex(name);
	if (unfIndex == -1 || bindIndex == -1) 
		throw Exception("Unable to find uniform of name " + name + " in shader.\n");
	glUniformBlockBinding(id, unfIndex, bindIndex);
}

LightShader::LightShader(bool hasGeometry, const std::string& filename)
	:Shader(hasGeometry, filename, PHONG_SUBS), maxPhongLights(50)
{
	init();
}

LightShader::LightShader(bool hasGeometry, const std::string& filename,
	std::vector<std::string> subs)
	:Shader(hasGeometry, filename, subs), maxPhongLights(50)
{
	init();
}

void LightShader::init()
{
	use();
	setupUniformBlock("ambBlock");
	setupUniformBlock("phongBlock");

	ambTex_u  = getUniformLoc("ambTex");
	diffTex_u = getUniformLoc("diffTex");
	specTex_u = getUniformLoc("specTex");
	specExp_u = getUniformLoc("specExp");

	glUseProgram(0);
}

void LightShader::setAmbTexUnit(GLuint ambTexUnit)
{
	use();
	glUniform1i(ambTex_u, ambTexUnit);
	glUseProgram(0);
}

void LightShader::setDiffTexUnit(GLuint diffTexUnit)
{
	use();
	glUniform1i(diffTex_u, diffTexUnit);
	glUseProgram(0);
}

void LightShader::setSpecTexUnit(GLuint specTexUnit)
{
	use();
	glUniform1i(specTex_u, specTexUnit);
	glUseProgram(0);
}

void LightShader::setSpecExp(float exponent)
{
	use();
	glUniform1f(specExp_u, exponent);
	glUseProgram(0);
}

ParticleShader::ParticleShader(bool hasGeomShader, bool hasBBTex, const std::string& filename,
	bool hasCamera, bool hasModelToWorld)
	:Shader(hasGeomShader, filename, hasCamera, hasModelToWorld)
{
	use();
	alpha_u = getUniformLoc("globalAlpha");
	bbWidth_u = getUniformLoc("bbWidth");
	bbHeight_u = getUniformLoc("bbHeight");
	if(hasBBTex) bbTex_u = getUniformLoc("bbTexture");
	decayTex_u = getUniformLoc("decayTexture");
	glUseProgram(0);
}

void ParticleShader::setAlpha(float alpha)
{
	use();
	glUniform1fv(alpha_u, 1, &alpha);
	glUseProgram(0);
}

void ParticleShader::setBBWidth(float _bbWidth)
{
	use();
	glUniform1fv(bbWidth_u, 1, &_bbWidth);
	glUseProgram(0);
}

void ParticleShader::setBBHeight(float _bbHeight)
{
	use();
	glUniform1fv(bbHeight_u, 1, &_bbHeight);
	glUseProgram(0);
}

void ParticleShader::setBBTexUnit(GLuint _bbTexUnit)
{
	use();
	glUniform1i(bbTex_u, _bbTexUnit);
	glUseProgram(0);
}

void ParticleShader::setDecayTexUnit(GLuint _decayTexUnit)
{
	use();
	glUniform1i(decayTex_u, _decayTexUnit);
	glUseProgram(0);
}

CubemapShader::CubemapShader(
	bool hasGeomShader, bool hasBBTex, const std::string& filename)
	:ParticleShader(hasGeomShader, hasBBTex, filename, false, true)
{
	use();
	worldToObject_u = getUniformLoc("worldToObject");
	rotation_u = getUniformLoc("rotation");
	perspective_u = getUniformLoc("perspective");
	glm::mat4 perspective = glm::perspective(90.0f, 1.0f, 0.01f, 50.0f);
	glUniformMatrix4fv(perspective_u, 1, GL_FALSE, &(perspective[0][0]));
	glUseProgram(0);
}

void CubemapShader::setWorldToObject(const glm::mat4& worldToObject)
{
	use();
	glUniformMatrix4fv(worldToObject_u, 1, GL_FALSE, &(worldToObject[0][0]));
	glUseProgram(0);
}

void CubemapShader::setRotation(const glm::mat4& rotation)
{
	use();
	glUniformMatrix4fv(rotation_u, 1, GL_FALSE, &(rotation[0][0]));
}

SHShader::SHShader(bool hasGeomShader,  const std::string& filename)
	:Shader(hasGeomShader, filename, SH_SUBS)
{
	init();
}

SHShader::SHShader(bool hasGeomShader,  const std::string& filename,
	std::vector<std::string> subs)
	:Shader(hasGeomShader, filename, subs)
{
	init();
}

void SHShader::setTexUnit(GLuint unit)
{
	use();
	glUniform1i(texUnit_u, unit);
	glUseProgram(0);
}

void SHShader::init()
{
	texUnit_u = getUniformLoc("coefftTex");
	setupUniformBlock("SHBlock");
}

AOShader::AOShader(bool hasGeomShader,  const std::string& filename)
	:LightShader(hasGeomShader, filename, PHONG_SUBS)
{
	setupUniformBlock("ambBlock");
	setupUniformBlock("phongBlock");
}

AOShader::AOShader(bool hasGeomShader,  const std::string& filename, 
	std::vector<std::string> subs)
	:LightShader(hasGeomShader, filename, subs)
{
	setupUniformBlock("ambBlock");
	setupUniformBlock("PhongBlock");
}
