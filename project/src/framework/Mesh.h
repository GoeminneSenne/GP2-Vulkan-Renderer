#pragma once

#include "Buffer.h"

namespace dae
{
	struct Context;

	struct Mesh final
	{
		Buffer vertexBuffer{};
		Buffer indexBuffer{};
		std::uint32_t vertexCount = 0;
		std::uint32_t indexCount = 0;

		void LoadObj(const Context& context, const std::string& path);
		void Destroy(const Context& context);

		void RecordDraw(const CommandBuffer& command) const;
	};
}