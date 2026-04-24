#include "DescriptorPool.h"

#include "Common.h"
#include "Context.h"
#include "DescriptorSet.h"
#include "DescriptorSetLayout.h"
#include "LogicalDevice.h"

void dae::DescriptorPool::Create(const Context& context,
    const std::vector<VkDescriptorPoolSize>& poolSizes,uint32_t maxSets)
{
    m_maxSets = maxSets;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = m_maxSets;

    LOG_VULKAN_ERROR(
        vkCreateDescriptorPool(context.logicalDevice.device, &poolInfo, nullptr, &m_pool),
        "failed to create descriptor pool!"
    );
}

void dae::DescriptorPool::Destroy(const Context& context) const
{
    if (m_pool != VK_NULL_HANDLE)
    {
        vkDestroyDescriptorPool(context.logicalDevice.device, m_pool, nullptr);
    }
}

std::vector<dae::DescriptorSet> dae::DescriptorPool::Allocate(const Context& context, DescriptorSetLayout& layout, uint32_t count) const
{
    std::vector<VkDescriptorSetLayout> layouts(count, layout.layout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_pool;
    allocInfo.descriptorSetCount = count;
    allocInfo.pSetLayouts = layouts.data();

    std::vector<VkDescriptorSet> rawSets(count);
    LOG_VULKAN_ERROR(
        vkAllocateDescriptorSets(context.logicalDevice.device, &allocInfo, rawSets.data()),
        "failed to allocate descriptor sets!"
    );

    std::vector<DescriptorSet> sets(count);
    for(uint32_t idx{}; idx < count; ++idx)
    {
        sets[idx].set = rawSets[idx];
        sets[idx].pLayout = &layout;
    }

    return sets;
}
