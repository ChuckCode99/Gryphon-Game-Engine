#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "glad/glad.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"


class shader
{
public:
	unsigned int ID;

	shader(const char* vertexPath, const char* fragmentPath)
	{
		class create_shader
		{
		public:
			create_shader(const char* filePath, std::string type)
				:shader_type(type)
			{
				shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

				try
				{
					shaderFile.open(filePath);
					std::stringstream ShaderStream;
					ShaderStream << shaderFile.rdbuf();
					shaderFile.close();
					code = ShaderStream.str();
				}
				catch (std::ifstream::failure& e)
				{
					std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
				}

				usable_code = code.c_str();

				if (shader_type == "VERTEX")
					S_ID = glCreateShader(GL_VERTEX_SHADER);
				else
					S_ID = glCreateShader(GL_FRAGMENT_SHADER);

				glShaderSource(S_ID, 1, &usable_code, NULL);
				glCompileShader(S_ID);
				checkCompileErrors(S_ID, shader_type);
			}

			~create_shader() { glDeleteShader(S_ID); }

			unsigned int ID() { return S_ID; }

			std::string ShaderType() { return shader_type; }

		private:
			unsigned int S_ID{};
			std::string shader_type{};
			std::string code{};
			const char* usable_code{};
			std::ifstream shaderFile{};
		};

		create_shader VertexShader(vertexPath, "VERTEX");
		create_shader FragmentShader(fragmentPath, "FRAGMENT");

		ID = glCreateProgram();
		glAttachShader(ID, VertexShader.ID());
		glAttachShader(ID, FragmentShader.ID());
		glLinkProgram(ID);
		checkCompileErrors(ID, "PROGRAM");		
	}


	// utility uniform functions
	// ------------------------------------------------------------------------
	void use() { glUseProgram(ID); }

	void setBool(const std::string& name, bool value) const { glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value); }

	void setInt(const std::string& name, int value) const { glUniform1i(glGetUniformLocation(ID, name.c_str()), value); }

	void setFloat(const std::string& name, float value) const { glUniform1f(glGetUniformLocation(ID, name.c_str()), value); }

	void setVec3(const std::string& name, const glm::vec3 &value) const { glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value)); }

	void setMat4(const std::string& name, const glm::mat4 &value) const { glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value)); }
private:
	// utility function for checking shader compilation/linking errors.
	// ------------------------------------------------------------------------
	static void checkCompileErrors(unsigned int shader, std::string type)
	{
		int success;
		char infoLog[1024];
		if (type != "PROGRAM")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
	}
};