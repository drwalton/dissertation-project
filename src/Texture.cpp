#include "Texture.hpp"
#include "Exception.hpp"

#include "SOIL.h"
#include <iostream>
#include <fstream>

GLuint Texture::nextTexUnit = 0;

Texture::Texture(const std::string& filename)
	:filename(filename)
{
	std::string fullPath = "../textures/" + filename;

	texUnit = genTexUnit();

	glActiveTexture(GL_TEXTURE0 + texUnit);

	id = SOIL_load_OGL_texture
	(		 
		fullPath.c_str(),
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_INVERT_Y
	);

	if(id == 0) 
	{
		throw Exception("Texture file " + fullPath + " could not be loaded.\n");
	}
}

Texture::~Texture()
{
	glDeleteTextures(1, &id);
}

GLuint Texture::getTexUnit()
{
	return texUnit;
}

GLuint Texture::genTexUnit()
{
	GLuint unit = nextTexUnit;
	nextTexUnit++;
	return unit;
}

ArrayTexture::ArrayTexture(const std::vector<std::string>& filenames)
	:filenames(filenames)
{
	nLayers = filenames.size();

	texUnit = Texture::genTexUnit();
	glActiveTexture(GL_TEXTURE0 + texUnit);

	int width, height, channels;

	std::vector<unsigned char> compiledImages;

	for(auto f = filenames.begin(); f != filenames.end(); ++f)
	{
		unsigned char* fileData = SOIL_load_image
			(
				("../textures/" + *f).c_str(),
				&width, &height, &channels,
				SOIL_LOAD_AUTO
			);

		if(!fileData) throw(new std::exception());

		for(int v = 0; v < height; ++v)
			for(int u = 0; u < width; ++u)
			{
				compiledImages.push_back(fileData[(u + v*width)*channels    ]);
				compiledImages.push_back(fileData[(u + v*width)*channels + 1]);
				compiledImages.push_back(fileData[(u + v*width)*channels + 2]);
				compiledImages.push_back(fileData[(u + v*width)*channels + 3]);
			}

		SOIL_free_image_data(fileData);
	}

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D_ARRAY, id);
	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA, width, height, nLayers);

	glTexImage3D(
		GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, 
		width, height, nLayers, 
		0, GL_RGBA, GL_UNSIGNED_BYTE, 
		compiledImages.data());

	glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
}

ArrayTexture::~ArrayTexture()
{
	glDeleteTextures(1, &id);
}
