#pragma once

#include "Buffer.h"

namespace dae
{
	struct Context;

	class Mesh final
	{
	public:
		void LoadObj(const Context& context, const std::string& path);
		void Destroy(const Context& context);

		void RecordDraw(const CommandBuffer& command) const;

	private:
		Buffer vertexBuffer{};
		Buffer indexBuffer{};
		std::uint32_t vertexCount = 0;
		std::uint32_t indexCount = 0;
	};
}