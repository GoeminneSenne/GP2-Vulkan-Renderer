#pragma once

#include <string>
#include <vector>

namespace dae
{
	struct LogicalDevice;
	struct Shader;

	class ShaderLoader final
	{
	public:
		void Load(const LogicalDevice& logicalDevice, Shader& shader) const;

		ShaderLoader& SetFilename(std::string filename);
		ShaderLoader& SetEntryPoint(std::string entryPoint);

	private:
		std::vector<char> ReadFile() const;

		std::string m_filename;
		std::string m_entryPoint = "main";
	};
}