#pragma once

#include <chrono>
#include <iostream>

namespace dae
{
	class Timer final
	{
	public:
		Timer() = default;

		void Update()
		{
			const auto currentTime{ std::chrono::high_resolution_clock::now() };

			m_elapsed = std::chrono::duration<float>(currentTime - m_lastTime).count();
			m_lastTime = currentTime;
		}

		float GetElapsed() const { return m_elapsed; }

	private:
		std::chrono::time_point<std::chrono::steady_clock> m_lastTime{std::chrono::high_resolution_clock::now()};
		float m_elapsed{};
	};
}
