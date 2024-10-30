#pragma once

namespace VoxelWasteland::Core 
{
	class CustomShader
	{
	public:
		CustomShader() = delete;
		CustomShader(const char* vertPath, const char* fragPath);
		~CustomShader();

		void Use();
		void SetBool(const std::string& name, bool value);
		void SetInt(const std::string& name, int value);
		void SetFloat(const std::string& name, float value);
		void SetFloatArray(const std::string& name, float* value, int size);
		void SetVec3(const std::string& name, glm::vec3 value);
		void SetMat4(const std::string& name, glm::mat4 value);

		unsigned int GetID();

	private:
		unsigned int programID;
	};
}
