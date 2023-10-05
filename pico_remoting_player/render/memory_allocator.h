#pragma once

#include "vulkan_helpers.h"


class MemoryAllocator {
public:
    void Init(VkPhysicalDevice physicalDevice, VkDevice device);

    static const VkFlags defaultFlags;

    void Allocate(VkMemoryRequirements const& memReqs, VkDeviceMemory* mem, VkFlags flags = defaultFlags,
                  const void* pNext = nullptr) const;

private:
    VkDevice m_vkDevice{ VK_NULL_HANDLE };
    VkPhysicalDeviceMemoryProperties m_memProps{};
};