#include "core/render/CustomShader.h"

#include <GLFW/glfw3.h>

namespace VoxelWasteland::Core
{
	CustomShader::CustomShader(const char* vertPath, const char* fragPath)
	{
		programID = ResourceHelper::LoadShaders(vertPath, fragPath);
	}

	CustomShader::~CustomShader()
	{
	}

	/*
	* @brief Use the shader
	*/
	void CustomShader::Use()
	{
		glUseProgram(programID);
	}

	/*
	* @brief Set a boolean value in the shader
	* @param name of the variable
	* @param value to set
	*/
	void CustomShader::SetBool(const std::string& name, bool value)
	{
		glUniform1i(glGetUniformLocation(programID, name.c_str()), (int)value);
	}

	/*
	* @brief Set an integer value in the shader
	* @param name of the variable
	* @param value to set
	*/
	void CustomShader::SetInt(const std::string& name, int value)
	{
		glUniform1i(glGetUniformLocation(programID, name.c_str()), value);
	}

	/*
	* @brief Set a float value in the shader
	* @param name of the variable
	* @param value to set
	*/
	void CustomShader::SetFloat(const std::string& name, float value)
	{
		glUniform1f(glGetUniformLocation(programID, name.c_str()), value);
	}

	/*
	* @brief Set an array of float values in the shader
	* @param name of the variable
	* @param value to set
	*/
	void CustomShader::SetFloatArray(const std::string& name, float* value, int size)
	{
		glUniform1fv(glGetUniformLocation(programID, name.c_str()), size, value);
	}

	/*
	* @brief Set a vector3 value in the shader
	* @param name of the variable
	* @param value to set
	*/
	void CustomShader::SetVec3(const std::string& name, glm::vec3 value)
	{
		glUniform3fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
	}

	/*
	* @brief Set a matrix4 value in the shader
	* @param name of the variable
	* @param value to set

	*/
	void CustomShader::SetMat4(const std::string& name, glm::mat4 value)
	{
		glUniformMatrix4fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
	}

	/*
	* @brief Get the shader ID
	* @return the shader ID
	*/
	unsigned int CustomShader::GetID()
	{
		return programID;
	}
}
