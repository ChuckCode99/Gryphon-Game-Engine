#pragma once

#include <iostream>
#include <string>

#include "glad/glad.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

class texture
{
public:
	unsigned int ID, TextureLayer;

	texture(std::string TexturePath, std::string TextureFormat)
		:TextureLayer(TextureLayers++)
	{
		glGenTextures(1, &ID);
		glActiveTexture(GL_TEXTURE0 + TextureLayer);
		glBindTexture(GL_TEXTURE_2D, ID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		stbi_set_flip_vertically_on_load(true);

		unsigned char* data = stbi_load(TexturePath.c_str(), &width, &height, &nrChannels, 0);
		
		if (data)
		{
			if (TextureFormat == "jpg")
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			}
			else if (TextureFormat == "png")
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			}

			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}

	}

	void Activate_Bind()
	{
		glActiveTexture(GL_TEXTURE0 + TextureLayer);
		glBindTexture(GL_TEXTURE_2D, ID);
	}

private:
	int width, height, nrChannels;

	static inline int TextureLayers = 0;
};