#pragma once

#include <deque>
#include <functional>
#include <ranges>
#include <stdexcept>

#define LOG_VULKAN_ERROR(expr, msg)                              \
    [&]() {                                                      \
        VkResult res = (expr);                                   \
        if (res != VK_SUCCESS) {                                 \
            throw std::runtime_error(msg);                       \
        }                                                        \
    }()

constexpr static int MAX_FRAMES_IN_FLIGHT = 3;

#ifdef NDEBUG
constexpr static bool ENABLE_VALIDATION = false;
#else
constexpr static bool ENABLE_VALIDATION = true;
#endif

class DeletionQueue final
{
public:
    void PushDeletor(std::function<void()>&& deletorFunc)
    {
        m_deletors.push_back(deletorFunc);
    }

    void Flush()
    {
	    for (auto& m_deletor : std::ranges::reverse_view(m_deletors))
	    {
		    m_deletor();
	    }

        m_deletors.clear();
    }

private:
       std::deque< std::function<void()>> m_deletors;
};