#pragma once

#include <stdexcept>

#define LOG_VULKAN_ERROR(expr, msg)                              \
    [&]() {                                                      \
        VkResult res = (expr);                                   \
        if (res != VK_SUCCESS) {                                 \
            throw std::runtime_error(msg);                       \
        }                                                        \
    }()
